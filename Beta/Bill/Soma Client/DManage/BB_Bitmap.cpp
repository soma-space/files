// BB_Bitmap.cpp: implementation of the BB_Bitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BB_Bitmap.h"
#include "BB_Direct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BB_Bitmap::BB_Bitmap()
{
	m_pBitmap = NULL;
	m_pPalette = NULL;
	m_BitmapSize.cx = m_BitmapSize.cy = 0;
}

BB_Bitmap::~BB_Bitmap()
{
	DeleteObject();
}

BOOL BB_Bitmap::LoadResourceBitmap(UINT bitmapID, BOOL bMakePalette)
{
	DeleteObject();

	HRSRC  hRsrc;
	HGLOBAL hGlobal;
	HBITMAP hBitmapFinal = NULL;
	LPBITMAPINFOHEADER  lpbi;
	HDC hdc;
	int iNumColors;
	BOOL loadSuccess=FALSE;
	HINSTANCE hInstance = AfxGetResourceHandle();
	HPALETTE pal;
 
	if (hRsrc = FindResource(hInstance, MAKEINTRESOURCE(bitmapID), RT_BITMAP))
	{
		hGlobal = LoadResource(hInstance, hRsrc);
		lpbi = (LPBITMAPINFOHEADER)LockResource(hGlobal);

		hdc = GetDC(NULL);
		pal =  CreateDIBPalette ((LPBITMAPINFO)lpbi, &iNumColors);
		if (pal)
		{
			SelectPalette(hdc,pal,FALSE);
			RealizePalette(hdc);
			if (bMakePalette)
			{
				m_pPalette = new CPalette;
				m_pPalette->Attach(pal);
			}
		}

		hBitmapFinal = CreateDIBitmap(hdc,
						(LPBITMAPINFOHEADER)lpbi,
						(LONG)CBM_INIT,
						(LPSTR)lpbi + lpbi->biSize + iNumColors * sizeof(RGBQUAD),
						(LPBITMAPINFO)lpbi,
						DIB_RGB_COLORS );

		if (hBitmapFinal)
		{
			m_pBitmap = new CBitmap;
			m_pBitmap->Attach(hBitmapFinal);

			BITMAP bm;
			::GetObject(hBitmapFinal, sizeof(BITMAP), &bm);
			m_BitmapSize.cx = bm.bmWidth;
			m_BitmapSize.cy = bm.bmHeight;

			loadSuccess = TRUE;
		}

		ReleaseDC(NULL,hdc);
		UnlockResource(hGlobal);
		FreeResource(hGlobal);
	}
	return loadSuccess;
}

HPALETTE BB_Bitmap::CreateDIBPalette(LPBITMAPINFO lpbmi, LPINT lpiNumColors)
{
	LPBITMAPINFOHEADER  lpbi;
	LPLOGPALETTE     lpPal;
	HANDLE           hLogPal;
	HPALETTE         hPal = NULL;
	int              i;
 
	lpbi = (LPBITMAPINFOHEADER)lpbmi;
	if (lpbi->biBitCount <= 8)
		*lpiNumColors = (1 << lpbi->biBitCount);
	else
		*lpiNumColors = 0;  // No palette needed for 24 BPP DIB
 
	if (*lpiNumColors)
	{
		hLogPal = GlobalAlloc (GHND, sizeof (LOGPALETTE) +
							 sizeof (PALETTEENTRY) * (*lpiNumColors));
		lpPal = (LPLOGPALETTE) GlobalLock (hLogPal);
		lpPal->palVersion    = 0x300;
		lpPal->palNumEntries = *lpiNumColors;

		for (i = 0;  i < *lpiNumColors;  i++)
		{
			lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
			lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
			lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
			lpPal->palPalEntry[i].peFlags = 0;
		}

		hPal = CreatePalette (lpPal);
		GlobalUnlock (hLogPal);
		GlobalFree   (hLogPal);
	}
	return hPal;
}

BOOL BB_Bitmap::LoadFileBitmapFromLib(CFile &file, long offset, long nFileLen)
{
	BOOL loadSuccess=FALSE;
	DeleteObject();
	BITMAPFILEHEADER bmfHeader;
	LPVOID lpBits;


	VERIFY(file.Seek(offset, CFile::begin)==offset);
	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
			return loadSuccess;

	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
			return loadSuccess;

	BITMAPINFO *pbmInfo;
	pbmInfo = (BITMAPINFO *)::GlobalAlloc(GMEM_FIXED, 
							sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256 );
	if (pbmInfo == NULL)
			return loadSuccess;

	// Read the BITMAPINFO
	file.Read( pbmInfo, sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256 );

	BITMAPINFO &bmInfo = *pbmInfo ;
	//palette create

	CDC dc;
	dc.CreateIC("DISPLAY", NULL, NULL, NULL);
	if (dc.GetDeviceCaps(BITSPIXEL)==8)
		LoadPalette((PALETTEENTRY*)((BYTE*)pbmInfo+sizeof(BITMAPINFO)));

	HBITMAP hBmp = CreateDIBSection( dc.GetSafeHdc(), pbmInfo, DIB_RGB_COLORS, &lpBits, 
											NULL, 0 );

	LPBYTE  lpDIBBits;              // Pointer to DIB bits
	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
							1 << bmInfo.bmiHeader.biBitCount;

	if( bmInfo.bmiHeader.biBitCount > 8 )
			lpDIBBits = (LPBYTE)((LPDWORD)(bmInfo.bmiColors + 
					bmInfo.bmiHeader.biClrUsed) + 
					((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
			lpDIBBits = (LPBYTE)(bmInfo.bmiColors + nColors);

	int nOffset = sizeof(BITMAPFILEHEADER) + (lpDIBBits - (LPBYTE)pbmInfo);
	VERIFY(file.Seek( offset+nOffset, CFile::begin)==offset+nOffset);
	file.ReadHuge((LPSTR)lpBits, nFileLen - nOffset); //bmInfo.biSizeImage ); 

	::GlobalFree(pbmInfo);

	if (hBmp)
	{
		m_pBitmap = new CBitmap;
		m_pBitmap->Attach(hBmp);

		BITMAP bm;
		::GetObject(hBmp, sizeof(BITMAP), &bm);
		m_BitmapSize.cx = bm.bmWidth;
		m_BitmapSize.cy = bm.bmHeight;
		loadSuccess = TRUE;
	}
	else DeleteObject();
	return loadSuccess;
}

BOOL BB_Bitmap::LoadFileBitmap(CString filename)
{
	BOOL loadSuccess=FALSE;
	DeleteObject();

	CFile file;
	if( !file.Open( filename, CFile::modeRead) )
			return loadSuccess;

	int nFileLen = file.GetLength();
	loadSuccess = LoadFileBitmapFromLib(file, 0, nFileLen);
	file.Close();
	return loadSuccess;
}

void BB_Bitmap::LoadPalette(PALETTEENTRY palentrys [256])
{
	if (m_pPalette) delete m_pPalette, m_pPalette=NULL;

	m_pPalette = new CPalette;
	LoadPaletteToPara(&m_pPalette, palentrys);
}

void BB_Bitmap::LoadPaletteToPara(CPalette ** pal, PALETTEENTRY palentrys [256])
{
	CPalette *palette = *pal;
	palette->DeleteObject();

	HLOCAL plocal = LocalAlloc(LPTR, sizeof(LOGPALETTE) + 256 * sizeof(PALETTEENTRY));
	LOGPALETTE* lp = (LOGPALETTE*) plocal;

	lp->palVersion = 0x300;
	lp->palNumEntries = 256;

	for (int i = 0; i<256; i++) {
		lp->palPalEntry[i].peRed = palentrys[i].peRed;
		lp->palPalEntry[i].peGreen = palentrys[i].peGreen;
		lp->palPalEntry[i].peBlue = palentrys[i].peBlue;
		lp->palPalEntry[i].peFlags = 0;
	}

	palette->CreatePalette(lp);
	palette->UnrealizeObject();
	LocalFree(plocal);
}

BOOL BB_Bitmap::WriteBitmap(CString szFile)
{
	if (!m_pBitmap) return FALSE;
//
	HANDLE hDIB = DDBToDIB(*m_pBitmap, BI_RGB, m_pPalette);

	BITMAPFILEHEADER        hdr;
	LPBITMAPINFOHEADER      lpbi;

	if (!hDIB)
			return FALSE;

	CFile file;
	if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
			return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header 
	hdr.bfType              = ((WORD) ('M' << 8) | 'B');    // is always "BM"
	hdr.bfSize              = GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1         = 0;
	hdr.bfReserved2         = 0;
	hdr.bfOffBits           = (DWORD) (sizeof( hdr ) + lpbi->biSize +
											nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );

	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) );

	file.Close();
	GlobalFree(hDIB);
	return TRUE;
}

HANDLE BB_Bitmap::DDBToDIB(CBitmap & bitmap, DWORD dwCompression, CPalette * pPal)
{
	BITMAP                  bm;
	BITMAPINFOHEADER        bi;
	LPBITMAPINFOHEADER      lpbi;
	DWORD                   dwLen;
	HANDLE                  hDIB;
	HANDLE                  handle;
	HDC                     hDC;
	HPALETTE                hPal;


	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
			hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize               = sizeof(BITMAPINFOHEADER);
	bi.biWidth              = bm.bmWidth;
	bi.biHeight             = bm.bmHeight;
	bi.biPlanes             = 1;
	bi.biBitCount           = bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression        = dwCompression;
	bi.biSizeImage          = 0;
	bi.biXPelsPerMeter      = 0;
	bi.biYPelsPerMeter      = 0;
	bi.biClrUsed            = 0;
	bi.biClrImportant       = 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 ) 
			nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
			SelectPalette(hDC,hPal,FALSE);
			ReleaseDC(NULL,hDC);
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
	if (bi.biSizeImage == 0){
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
	else{
			GlobalFree(hDIB);

			// Reselect the original palette
			SelectPalette(hDC,hPal,FALSE);
			ReleaseDC(NULL,hDC);
			return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
							0L,                             // Start scan line
							(DWORD)bi.biHeight,             // # of scan lines
							(LPBYTE)lpbi                    // address for bitmap bits
							+ (bi.biSize + nColors * sizeof(RGBQUAD)),
							(LPBITMAPINFO)lpbi,             // address of bitmapinfo
							(DWORD)DIB_RGB_COLORS);         // Use RGB for color table

	if( !bGotBits )
	{
			GlobalFree(hDIB);
        
			SelectPalette(hDC,hPal,FALSE);
			ReleaseDC(NULL,hDC);
			return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	ReleaseDC(NULL,hDC);
	return hDIB;
}

CDDSurface*	BB_Bitmap::CreateSurfaceFromLib(CDirectDraw *pDirectDraw, CFile &file, long offset, long nFileLen)
{
	CDDSurface *pSurface = NULL;
	VERIFY(file.Seek(offset, CFile::begin)==offset);

	BITMAPFILEHEADER bmfHeader;	
	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return pSurface;
	
	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return pSurface;
	
	// Get length of the remainder of the file and allocate memory
	DWORD nPackedDIBLen = nFileLen - sizeof(BITMAPFILEHEADER);
	HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, nPackedDIBLen);
	if (hDIB == 0)
		return pSurface;
	
	// Read the remainder of the bitmap file.
	if (file.ReadHuge((LPSTR)hDIB, nPackedDIBLen) != nPackedDIBLen )
	{
		::GlobalFree(hDIB);
		return pSurface;
	}
	
	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	
	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 
	1 << bmiHeader.biBitCount;
	
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	
	CSize imageSize;
	imageSize.cx = bmInfo.bmiHeader.biWidth;
	imageSize.cy = bmInfo.bmiHeader.biHeight;
	pSurface = new CDDSurface;
	pSurface->Create(pDirectDraw, imageSize.cx, imageSize.cy, FALSE);
	CDC *pSurfaceDC = pSurface->GetDC();

	::SetDIBitsToDevice (pSurfaceDC->GetSafeHdc(), 
		0, 0, imageSize.cx, imageSize.cy, 
		0, 0, 0, imageSize.cy, 
		lpDIBBits, &bmInfo, DIB_RGB_COLORS);
	
	::GlobalFree(hDIB);
	pSurface->ReleaseDC(pSurfaceDC);
	return pSurface;
}

BOOL BB_Bitmap::CopySurfaceFromLib(CDDSurface *pSurface, CFile &file, long offset, long nFileLen)
{
	VERIFY(file.Seek(offset, CFile::begin)==offset);

	BITMAPFILEHEADER bmfHeader;	
	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return FALSE;
	
	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return FALSE;
	
	// Get length of the remainder of the file and allocate memory
	DWORD nPackedDIBLen = nFileLen - sizeof(BITMAPFILEHEADER);
	HGLOBAL hDIB = ::GlobalAlloc(GMEM_FIXED, nPackedDIBLen);
	if (hDIB == 0)
		return FALSE;
	
	// Read the remainder of the bitmap file.
	if (file.ReadHuge((LPSTR)hDIB, nPackedDIBLen) != nPackedDIBLen )
	{
		::GlobalFree(hDIB);
		return FALSE;
	}
	
	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)hDIB ;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB ;
	
	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 
	1 << bmiHeader.biBitCount;
	
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	
	CSize imageSize;
	imageSize.cx = bmInfo.bmiHeader.biWidth;
	imageSize.cy = bmInfo.bmiHeader.biHeight;

	CDC *pSurfaceDC = pSurface->GetDC();

	::SetDIBitsToDevice (pSurfaceDC->GetSafeHdc(), 
		0, 0, imageSize.cx, imageSize.cy, 
		0, 0, 0, imageSize.cy, 
		lpDIBBits, &bmInfo, DIB_RGB_COLORS);
	
	::GlobalFree(hDIB);
	pSurface->ReleaseDC(pSurfaceDC);
	return TRUE;
}