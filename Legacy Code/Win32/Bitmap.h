// DDBToDIB		- Creates a DIB from a DDB
// bitmap		- Device dependent bitmap
// dwCompression	- Type of compression - see BITMAPINFOHEADER
// pPal			- Logical palette
HANDLE DDBToDIB(CBitmap& bitmap, DWORD dwCompression, CPalette* pPal)
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;

	ASSERT(bitmap.GetSafeHandle());

	// The function has no arg for bitfields
	if (dwCompression == BI_BITFIELDS)
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE)pPal->GetSafeHandle();
	if (hPal == NULL)
		hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm), (LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression = dwCompression;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// Compute the size of the  infoheader and the color table
	/*int nColors = (1 << bi.biBitCount);
	if( nColors > 256 )
		nColors = 0;*/

		// Compute the size of the  infoheader and the color table
	int nColors = 0;

	if (bi.biBitCount <= 8)
	{
		nColors = (1 << bi.biBitCount);
	}

	dwLen = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);
	hPal = SelectPalette(hDC, hPal, FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED, dwLen);

	if (!hDIB) {
		SelectPalette(hDC, hPal, FALSE);
		ReleaseDC(NULL, hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver
	// will calculate the biSizeImage field
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0) {
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8)
			* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else {
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC, hPal, FALSE);
		ReleaseDC(NULL, hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(),
		0L,				// Start scan line
		(DWORD)bi.biHeight,		// # of scan lines
		(LPBYTE)lpbi 			// address for bitmap bits
		+ (bi.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi,		// address of bitmapinfo
		(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if (!bGotBits)
	{
		GlobalFree(hDIB);

		SelectPalette(hDC, hPal, FALSE);
		ReleaseDC(NULL, hDC);
		return NULL;
	}

	SelectPalette(hDC, hPal, FALSE);
	ReleaseDC(NULL, hDC);
	return hDIB;
}

// CopyBitmapToClipboard	- Copies a device-dependent bitmap to clipboard
// pWnd				- Pointer to window that opens the clipboard
// bitmap			- The device-dependent bitmap
// pPal				- Pointer to logical palette - Can be NULL
// NOTE				- GDI objects are detached from bitmap & pPal
//				  as the clipboard owns them after the copy
void CopyBitmapToClipboard(const CWnd *pWnd, CBitmap& bitmap, CPalette* pPal)
{
	OpenClipboard(pWnd->GetSafeHwnd());
	EmptyClipboard();

	if (pPal)
		::SetClipboardData(CF_PALETTE, pPal->GetSafeHandle());

	SetClipboardData(CF_BITMAP, bitmap.GetSafeHandle());
	CloseClipboard();

	bitmap.Detach();
	if (pPal)
		pPal->Detach();
}

// CopyDIBToClipboard	- Copies a device-dependent bitmap to clipboard
// pWnd			- Pointer to window that opens the clipboard
// hDIB			- Memory handle that contains BITMAPINFO & bitmap bits
// pPal			- Pointer to logical palette - Can be NULL
// NOTE			- GDI objects are detached from bitmap & pPal
//			  as the clipboard owns them after the copy
void CopyDIBToClipboard(const CWnd *pWnd, HGLOBAL hDIB, CPalette* pPal)
{
	::OpenClipboard(pWnd->GetSafeHwnd());
	::EmptyClipboard();
	if (pPal)
		::SetClipboardData(CF_PALETTE, pPal->GetSafeHandle());
	::SetClipboardData(CF_DIB, hDIB);
	::CloseClipboard();
	//bitmap.Detach();
	if (pPal)
		pPal->Detach();
}

//The CopyWndToClipboard() function copies the image of the window to the clipboard.
void CopyWndToClipboard(CWnd *pWnd)
{
	CBitmap 	bitmap;
	CClientDC	dc(pWnd);
	CDC 		memDC;
	CRect		rect;

	memDC.CreateCompatibleDC(&dc);

	pWnd->GetWindowRect(rect);

	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());

	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);

	pWnd->OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, bitmap.GetSafeHandle());
	CloseClipboard();

	memDC.SelectObject(pOldBitmap);
	bitmap.Detach();
}