#pragma once
#include "StdAfx.h"

#include <commoncontrols.h>


class NSUtils
{
public:
	static BOOL GetIconFromFileExtension(LPCWSTR ext, INT iconSystemIndex, INT iconType, HICON* pIcon);
	static BOOL DrawOverlayAtIcon(HICON originalIcon, HICON overlayIcon, LPCWSTR overlayPath, INT oIconSize, INT overlaySize, INT overlayX, INT overlayY, HICON* pResult);
	static BOOL DrawOverlayAtBitmap(HBITMAP originalImage, HICON overlayIcon, LPCWSTR overlayPath, INT oImageSize, INT overlaySize, INT overlayX, INT overlayY, HBITMAP* pResult);


	//codes from http://www.winprog.org/tutorial/transparency.html
	static HBITMAP CreateBitmapMask(HBITMAP hbmColour, COLORREF crTransparent);
};
