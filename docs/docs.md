# NSEDrawOverlayIcon

IconOverlay in Namespace Extension without count limit.

## How to use (Sample Code)
```c++
//In 'IExtractIconW'
HRESULT CExtracticon::GetIconLocation(UINT uFlags,LPWSTR szIconFile,UINT cchMax,int *piIndex,UINT *pwFlags) {
	//check current file/folder need show icon overlay.
	if(/* .. */) {
		//Draw icon overlay.
		//pass flags to use Extract()
		*pwFlags = GIL_NOTFILENAME | GIL_DONTCACHE;
		return S_OK;
	}
	else {
		//normal item (Not draw overlay icon)
	}
	
	return S_OK;
}

HRESULT CExtractIcon::Extract(LPCWSTR pszFile,UINT nIconIndex,HICON* phiconLarge,HICON* phiconSmall,UINT nIconSize) {
	HRESULT hr = S_OK;
	LPCWSTR overlayIcoFile = L"c:\\overlay.ico";
	HICON tempIcon = NULL;
	
	//Get item's file extension
	//In this sample, use sample ext.
	LPWSTR fileExt = L".txt";
	
	int largeIconSize = LOWORD(nIconSize);
	int smallIconSize = HIWORD(nIconSize);
	
	if (phiconLarge) {
		//Get file icon from system.
		if (!NSUtils::GetIconFromFileExtension(fileExt, -1, largeIconSize, &tempIcon)) {
			hr = E_FAIL;
			goto escapeArea;
		}
		//Draw overlay icon
		if (!NSUtils::DrawOverlayAtIcon(tempIcon,NULL,overlayIcoFile,largeIconSize,0,0,0,phIconLarge)) {
			hr = E_FAIL;
			goto escapeArea;
		}
	}
	
	DestroyIcon(tempIcon);
	tempIcon = NULL;
	
	if (phiconSmall) {
		//Get file icon from system.
		if (!NSUtils::GetIconFromFileExtension(fileExt, -1, smallIconSize, &tempIcon)) {
			hr = E_FAIL;
			goto escapeArea;
		}
		//Draw overlay icon
		if (!NSUtils::DrawOverlayAtIcon(tempIcon,NULL,overlayIcoFile,smallIconSize,0,0,0,phiconSmall)) {
			hr = E_FAIL;
			goto escapeArea;
		}
	}
	
	escapeArea:
	
	DestroyIcon(tempIcon);
	
	return hr;
}

//In IPropertyStore (for control 'IThumbnailProvider' thumbnail cache)
STDMETHODIMP CPropertyStore::GetValue(REFPROPERTYKEY key, PROPVARIANT *pv) //IPropertyStore
{
// .......
	//System.ThumbnailCacheId -- PKEY_ThumbnailCacheId
	//446D16B1-8DAD-4870-A748-402EA43D788C}, 100
	if (key == PKEY_ThumbnailCacheId) {
		LPWSTR pPath = CNSEData::GetPath(m_pidl);
		FILETIME lastModified = CNSEData::GetLastModified(m_pidl);
		//example : 0 -> custom icon overlay disabled (not showed)
		//		  : 1 -> custom icon overlay type 1 
		//        : 2 -> custom icon overlay type 2
		//cache id is different by overlay icon status
		DWORD overlayIconStatus = CNSEData::GetOverlayStatus(m_pidl);
		pv->vt = VT_UI8;
		pv->uhVal = _CalcCacheId(pPath,&lastModified,overlayIconStatus);
		
		hr = S_OK;
	}
// ......
}

ULONGLONG CPropertyStore::_CalcCacheId(LPWSTR pPath,PFILETIME pLastModified,DWORD overlayStatus){
	ULONGLONG id = 185684268; //unique own basic id
	DWORD pathLen = 0;
	if(pPath){
		pathLen = (DWORD)wcslen(pPath);
		
		for(DWORD i=0;i<pathLen;i++){
			id += (DWORD)pPath[i];
		}
	}

	if(pLastModified){
		id += pLastModified->dwLowDateTime;
		id += (pLastModified->dwHighDateTime << 32);
	}
	
	id += overlayStatus;
	
	return id;
}


```
# API
## ** NSUtils class
### BOOL GetIconFromFileExtension(LPCWSTR ext, INT iconSystemIndex, INT iconType, HICON* pIcon); -static.
Get icon(HICON) registered in system. You can get icon using file extension or iconIndex number.\
To get folder icon. set 'ext' to NULL and set iconSystemIndex to -1.\
If this function succeeded, return TRUE and pIcon has icon value.\
If failed, return FALSE.\
	* ext: File Extension. if know icon’s index number, this parameter can be NULL.\
	* iconSystemIndex: if know icon’s index number, pass index number and set ext parameter to NULL. if don’t know index number , just set -1.\
	* iconType: set icon type. value can be icon width(height) or SHIL value in 'SHGetImageList function'.\
	* pIcon: returned HICON.

### BOOL DrawOverlayAtIcon(HICON originalIcon, HICON overlayIcon, LPCWSTR overlayPath, INT oIconSize, INT overlaySize, INT overlayX, INT overlayY, HICON* pResult); -static
Draw overlay icon at original icon.\
If this function succeeded, return TRUE and pIcon has drawed overlay icon image.\
If failed, return FALSE.\
	* originalIcon: Target to draw overlay icon.\
	* overlayIcon:  Overlay icon value. if you use 'overlayPath' value, this parameter must be NULL.\
	* overlayPath: Overlay icon 'ico' file path. if you 'overlayIcon' value, this parameter must be NULL.\
	* oIconSize: Original icon size.\
	* overlaySize: Overlay icon size.(not use yet)\
	* overlayX: Draw overlay location x value. (not use yet)\
	* overlayY: Draw overlay location y value. (not use yet)\
	* pIcon: returned new HICON.

### BOOL DrawOverlayAtBitmap(HBITMAP originalImage, HICON overlayIcon, LPCWSTR overlayPath, INT oImageSize, INT overlaySize, INT overlayX, INT overlayY, HBITMAP* pResult); -static
Draw overlay icon at original bitmap.\
This function use in when draw overlay at Thumbnail image.\
If this function succeeded, return TRUE and pResult has drawed overlay icon image.\
If failed, return FALSE.\
	* originalImage: Target to draw overlay bitmap image.\
	* overlayIcon:  Overlay icon value. if you use 'overlayPath' value, this parameter must be NULL.\
	* overlayPath: Overlay icon 'ico' file path. if you 'overlayIcon' value, this parameter must be NULL.\
	* oImageSize: Original image size.\
	* overlaySize: Overlay icon size.(not use yet)\
	* overlayX: Draw overlay location x value. (not use yet)\
	* overlayY: Draw overlay location y value. (not use yet)\
	* pResult: returned new HBITMAP.
	
