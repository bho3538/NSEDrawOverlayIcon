#include "stdafx.h"
#include "NSUtils.h"

#ifndef XTrace
#define XTrace ATLTRACE
#endif

BOOL NSUtils::GetIconFromFileExtension(LPCWSTR ext, INT iconSystemIndex, INT iconType, HICON* pIcon) {
	BOOL re = FALSE;
	HRESULT hr = S_OK;

	IImageList* imageList = NULL;
	SHFILEINFOW shfi = { 0, };

	//get icon index from system
	if (iconSystemIndex == -1) {
		SHGetFileInfoW(ext ? ext : L" ", ext ? FILE_ATTRIBUTE_NORMAL : FILE_ATTRIBUTE_DIRECTORY, &shfi, sizeof(SHFILEINFOW), SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
	}
	else {
		shfi.iIcon = iconSystemIndex;
	}

	if (iconType > SHIL_JUMBO) {
		if (iconType >= 256) {
			iconType = SHIL_JUMBO;
		}
		else if (iconType >= 48) {
			iconType = SHIL_EXTRALARGE;
		}
		else if (iconType >= 32) {
			iconType = SHIL_LARGE;
		}
		else {
			iconType = SHIL_SMALL;
		}
	}

	hr = SHGetImageList(iconType, IID_IImageList, (void**)&imageList);

	if (FAILED(hr) || !imageList) {
		XTrace(L"SHGetImageList err %x\n", hr);
		goto escapeArea;
	}

	//XTrace(L"GetIcon %d",shfi.iIcon);
	hr = imageList->GetIcon(shfi.iIcon, 0, pIcon);

	if (FAILED(hr)) {
		XTrace(L"IImageList GetIcon err %x\n", hr);
		goto escapeArea;
	}

	re = TRUE;

escapeArea:

	if (imageList) {
		imageList->Release();
	}

	return re;
}


BOOL NSUtils::DrawOverlayAtIcon(HICON originalIcon, HICON overlayIcon, LPCWSTR overlayPath, INT oIconSize, INT overlaySize, INT overlayX, INT overlayY, HICON* pResult) {
	BOOL re = FALSE;
	HDC hdc = NULL;
	HDC hMemDc = NULL;
	HBITMAP hMergedBitmap = NULL;
	HGDIOBJ gdio = NULL;
	ICONINFO iconInfo = { 0, };
	BOOL freeOverlayIcon = FALSE;

	if (!overlayIcon) {
		if (overlayPath) {
			overlayIcon = (HICON)LoadImageW(NULL, overlayPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			if (!overlayIcon) {
				XTrace(L"LoadImageW err %ls %d", overlayPath, GetLastError());
				goto escapeArea;
			}
		}
		else {
			XTrace(L"Invalid Parameter");
			goto escapeArea;
		}
	}

	hdc = GetDC(NULL);
	if (!hdc) {
		goto escapeArea;
	}

	hMemDc = CreateCompatibleDC(hdc);

	if (!hMemDc) {
		goto escapeArea;
	}

	hMergedBitmap = CreateCompatibleBitmap(hdc, oIconSize, oIconSize);

	if (!hMergedBitmap) {
		XTrace(L"CreateCompatibleBitmap err");
		goto escapeArea;
	}

	gdio = SelectObject(hMemDc, hMergedBitmap);

	if (!gdio) {
		goto escapeArea;
	}

	if (!DrawIconEx(hMemDc, 0, 0, originalIcon, oIconSize, oIconSize, 0, NULL, DI_NORMAL)) {
		XTrace(L"DrawIconEx err1");
		goto escapeArea;
	}

	if (overlaySize == 0) {
		overlaySize = oIconSize / 3;
		if (overlaySize < 15) {
			overlaySize = overlaySize + 9;
		}
	}

	if (!DrawIconEx(hMemDc, 0, 0, overlayIcon, overlaySize, overlaySize, 0, NULL, DI_NORMAL)) {
		XTrace(L"DrawIconEx err2");
		goto escapeArea;
	}

	iconInfo.fIcon = TRUE;
	iconInfo.hbmColor = hMergedBitmap;
	iconInfo.hbmMask = CreateBitmapMask(hMergedBitmap, RGB(0, 0, 0));

	*pResult = CreateIconIndirect(&iconInfo);

	if (*pResult) {
		re = TRUE;
	}

escapeArea:

	SelectObject(hMemDc, gdio);

	DeleteObject(hMergedBitmap);
	DeleteObject(iconInfo.hbmMask);

	DeleteDC(hMemDc);
	ReleaseDC(NULL, hdc);

	if (freeOverlayIcon) {
		DestroyIcon(overlayIcon);
	}

	return re;
}

BOOL NSUtils::DrawOverlayAtBitmap(HBITMAP originalImage, HICON overlayIcon, LPCWSTR overlayPath, INT oImageSize, INT overlaySize, INT overlayX, INT overlayY, HBITMAP* pResult) {
	BOOL re = FALSE;
	HDC hdc = NULL;
	HDC hMemDc = NULL;
	HGDIOBJ gdio = NULL;
	BOOL freeOverlayIcon = FALSE;

	if (!overlayIcon) {
		if (overlayPath) {
			overlayIcon = (HICON)LoadImageW(NULL, overlayPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			if (!overlayIcon) {
				XTrace(L"LoadImageW err %ls %d", overlayPath, GetLastError());
				goto escapeArea;
			}
		}
		else {
			XTrace(L"Invalid Parameter");
			goto escapeArea;
		}
	}

	hdc = GetDC(NULL);
	if (!hdc) {
		goto escapeArea;
	}

	hMemDc = CreateCompatibleDC(hdc);

	if (!hMemDc) {
		goto escapeArea;
	}

	gdio = SelectObject(hMemDc, originalImage);

	if (!gdio) {
		goto escapeArea;
	}

	BitBlt(hMemDc, 0, 0, oImageSize, oImageSize, hMemDc, 0, 0, SRCCOPY);

	if (overlaySize == 0) {
		overlaySize = oImageSize / 3;
		if (overlaySize < 15) {
			overlaySize = overlaySize + 9;
		}
	}

	if (!DrawIconEx(hMemDc, 0, 0, overlayIcon, overlaySize, overlaySize, 0, NULL, DI_NORMAL)) {
		XTrace(L"DrawIconEx err2");
		goto escapeArea;
	}

	*pResult = originalImage;

	if (*pResult) {
		re = TRUE;
	}

escapeArea:

	SelectObject(hMemDc, gdio);

	DeleteDC(hMemDc);
	ReleaseDC(NULL, hdc);

	if (freeOverlayIcon) {
		DestroyIcon(overlayIcon);
	}

	return re;
}

HBITMAP NSUtils::CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent)
{
	HDC hdcMem, hdcMem2;
	HBITMAP hbmMask;
	BITMAP bm;

	// Create monochrome (1 bit) mask bitmap.  

	GetObject(hbmColour, sizeof(BITMAP), &bm);
	hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	// Get some HDCs that are compatible with the display driver

	hdcMem = CreateCompatibleDC(0);
	hdcMem2 = CreateCompatibleDC(0);

	SelectObject(hdcMem, hbmColour);
	SelectObject(hdcMem2, hbmMask);

	// Set the background colour of the colour image to the colour
	// you want to be transparent.
	SetBkColor(hdcMem, crTransparent);

	// Copy the bits from the colour image to the B+W mask... everything
	// with the background colour ends up white while everythig else ends up
	// black...Just what we wanted.

	BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

	// Take our new mask and use it to turn the transparent colour in our
	// original colour image to black so the transparency effect will
	// work right.
	BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

	// Clean up.

	DeleteDC(hdcMem);
	DeleteDC(hdcMem2);

	return hbmMask;
}

__declspec(dllexport) void CRefreshShellViewWndsExcept(HWND hwnd) {
	NSUtils::RefreshShellViewWndsExcept(hwnd);
}