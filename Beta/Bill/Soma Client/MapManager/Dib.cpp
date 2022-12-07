// Dib.cpp: implementation of the CDib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "Dib.h"

#ifdef _MAPEDITOR_CORE
#include "qvmapeditor.h"
#endif


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDib::CDib()
{
	m_hDIB = NULL;
}

CDib::~CDib()
{
	RemoveData();
}

BOOL CDib::ReadFile(CString filename)
{
	RemoveData();

	if(filename.IsEmpty()) return FALSE;

	CFile file;
	if( !file.Open( filename, CFile::modeRead) )
		return FALSE;

	BITMAPINFO *pbi;
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize;
	HDIB hDIB;
	LPSTR pDIB;

	dwBitsSize = file.GetLength();

	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return FALSE;

	if (bmfHeader.bfType != DIB_HEADER_MARKER)
		return FALSE;

	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if(hDIB == 0)
		return FALSE;

	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	if (file.ReadHuge(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER) )
	{
		::GlobalUnlock((HGLOBAL) hDIB);
		::GlobalFree((HGLOBAL) hDIB);
		return FALSE;
	}

	pbi = (BITMAPINFO *)pDIB;
	if(pbi->bmiHeader.biBitCount != 8) {
		AfxMessageBox("그림은 256색상만 지원합니다.", MB_OK);
		::GlobalUnlock((HGLOBAL) hDIB);
		::GlobalFree((HGLOBAL) hDIB);
		return FALSE;
	}
	m_vSize.cx = pbi->bmiHeader.biWidth;
	m_vSize.cy = abs(pbi->bmiHeader.biHeight);
	m_vDIBSize = dwBitsSize - sizeof(BITMAPFILEHEADER);

	::GlobalUnlock((HGLOBAL) hDIB);
	m_hDIB = hDIB;

	return TRUE;
}

void CDib::StretchDraw(CDC *pdc, CPoint start_point, CSize limit_size)
{
	if(!m_hDIB) return;

	LPSTR lpDIBHdr;
	LPSTR lpDIBBits;

	lpDIBHdr  = (LPSTR)::GlobalLock((HGLOBAL)m_hDIB);
	lpDIBBits = FindDIBBits(lpDIBHdr);

	pdc->SetStretchBltMode(COLORONCOLOR);

	::StretchDIBits(pdc->GetSafeHdc(), start_point.x, start_point.y,	limit_size.cx, limit_size.cy,
		0, 0, m_vSize.cx, m_vSize.cy, lpDIBBits, (LPBITMAPINFO)lpDIBHdr, DIB_RGB_COLORS, SRCCOPY);
	
	::GlobalUnlock((HGLOBAL)m_hDIB);
}

void CDib::Draw(CDC *pDC, CPoint start_point)
{
	if(!m_hDIB) return;

	LPSTR lpDIBHdr;
	LPSTR lpDIBBits;

	lpDIBHdr  = (LPSTR)::GlobalLock((HGLOBAL)m_hDIB);
	lpDIBBits = FindDIBBits(lpDIBHdr);

	pDC->SetStretchBltMode(COLORONCOLOR);

//	::SetDIBitsToDevice(pDC->GetSafeHdc(), start_point.x, start_point.y,	m_vSize.cx,	m_vSize.cy,
//		0, (int)DIBHeight(lpDIBHdr) - m_vSize.cy, 0, (WORD)DIBHeight(lpDIBHdr),	lpDIBBits,
//		(LPBITMAPINFO)lpDIBHdr,	DIB_RGB_COLORS);
	::StretchDIBits(pDC->GetSafeHdc(), start_point.x, start_point.y, m_vSize.cx, m_vSize.cy,
		0, 0, m_vSize.cx, m_vSize.cy, lpDIBBits, (LPBITMAPINFO)lpDIBHdr, DIB_RGB_COLORS, SRCCOPY);
	
	::GlobalUnlock((HGLOBAL)m_hDIB);
}

void CDib::TransparentDraw(CDC *pDC, CPoint start_point, COLORREF colorTransparent)
{
	CDC memDC, maskDC, tempDC;
	maskDC.CreateCompatibleDC(pDC);
	CBitmap maskBitmap;
	
	//add these to store return of SelectObject() calls
	CBitmap* pOldMemBmp = NULL;
	CBitmap* pOldMaskBmp = NULL;
	
	memDC.CreateCompatibleDC(pDC);
	tempDC.CreateCompatibleDC(pDC);
	CBitmap bmpImage;
	bmpImage.CreateCompatibleBitmap(pDC, m_vSize.cx, m_vSize.cy);
	pOldMemBmp = memDC.SelectObject(&bmpImage);
	
	{
		LPSTR lpDIBHdr;
		LPSTR lpDIBBits;
		
		lpDIBHdr  = (LPSTR)::GlobalLock((HGLOBAL)m_hDIB);
		lpDIBBits = FindDIBBits(lpDIBHdr);

		memDC.SetStretchBltMode(COLORONCOLOR);
		::SetDIBitsToDevice(memDC.GetSafeHdc(), 0, 0, m_vSize.cx, m_vSize.cy,
			0, (int)DIBHeight(lpDIBHdr) - m_vSize.cy, 0, (WORD)DIBHeight(lpDIBHdr),	lpDIBBits,
			(LPBITMAPINFO)lpDIBHdr,	DIB_RGB_COLORS);
		::GlobalUnlock((HGLOBAL)m_hDIB);
	}
	
	// Create monochrome bitmap for the mask
	maskBitmap.CreateBitmap(m_vSize.cx, m_vSize.cy, 1, 1, NULL);
	pOldMaskBmp = maskDC.SelectObject(&maskBitmap);
	memDC.SetBkColor(colorTransparent);
	
	// Create the mask from the memory DC
	maskDC.BitBlt(0, 0, m_vSize.cx, m_vSize.cy, &memDC, 0, 0, SRCCOPY);
	
	// Set the background in memDC to black. Using SRCPAINT with black 
	// and any other color results in the other color, thus making 
	// black the transparent color
	memDC.SetBkColor(RGB(0,0,0));
	memDC.SetTextColor(RGB(255,255,255));
	memDC.BitBlt(0, 0, m_vSize.cx, m_vSize.cy, &maskDC, 0, 0, SRCAND);
	
	// Set the foreground to black. See comment above.
	pDC->SetBkColor(RGB(255,255,255));
	pDC->SetTextColor(RGB(0,0,0));
	pDC->BitBlt(start_point.x, start_point.y, m_vSize.cx, m_vSize.cy, &maskDC, 0, 0, SRCAND);
	
	// Combine the foreground with the background
	pDC->BitBlt(start_point.x, start_point.y, m_vSize.cx, m_vSize.cy, &memDC, 0, 0, SRCPAINT);
	
	if (pOldMaskBmp)
		maskDC.SelectObject(pOldMaskBmp);
	if (pOldMemBmp)
		memDC.SelectObject(pOldMemBmp);
}

void CDib::RemoveData()
{
	if(m_hDIB) {
		::GlobalFree((HGLOBAL)m_hDIB);
		m_hDIB = NULL;
	}
}

BOOL CDib::SaveCore(CArchive &file)
{
	if(!m_hDIB) return FALSE;

	LPSTR ldib, lpcore;
	DWORD core_size;

	ldib = (LPSTR)::GlobalLock((HGLOBAL)m_hDIB);
	lpcore = FindDIBBits(ldib);

	core_size = m_vDIBSize - (lpcore-ldib);
	
	file.Write((const void *)&core_size, sizeof(DWORD));
	file.Write((const void *)lpcore, core_size);

	::GlobalUnlock((HGLOBAL)m_hDIB);
	
	return TRUE;
}

BOOL CDib::LoadCore(CArchive &file, LPSTR pheader, DWORD pheader_size)
{
	RemoveData();
	
	LPSTR ldib;
	DWORD core_size;

	file.Read((void *)&core_size, sizeof(DWORD));
	if(!core_size) return TRUE;

	m_hDIB = (HDIB)::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, pheader_size+core_size);
	if(!m_hDIB) return FALSE;

	ldib = (LPSTR)::GlobalLock((HGLOBAL)m_hDIB);
	::CopyMemory((void *)ldib, (const void *)pheader, pheader_size);
	file.Read((void *)(ldib+pheader_size), core_size);

	BITMAPINFO *pbi = (BITMAPINFO *)ldib;
	m_vSize.cx = pbi->bmiHeader.biWidth;
	m_vSize.cy = abs(pbi->bmiHeader.biHeight);
	m_vDIBSize = pheader_size+core_size;

	::GlobalUnlock((HGLOBAL)m_hDIB);

	return TRUE;
}

BOOL CDib::Save(CArchive &file)
{
	if(!m_hDIB || !m_vDIBSize) return FALSE;

	LPSTR ldib;

	ldib = (LPSTR)::GlobalLock((HGLOBAL)m_hDIB);
	
	file.Write((const void *)&m_vDIBSize, sizeof(DWORD));
	file.Write((const void *)ldib, m_vDIBSize);

	::GlobalUnlock((HGLOBAL)m_hDIB);

	return TRUE;
}

BOOL CDib::Load(CArchive &file)
{
	RemoveData();

	LPSTR ldib;

	file.Read((void *)&m_vDIBSize, sizeof(DWORD));
	if(!m_vDIBSize) return FALSE;

	m_hDIB = (HDIB)::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, m_vDIBSize);
	if(!m_hDIB) return FALSE;

	ldib = (LPSTR)::GlobalLock((HGLOBAL)m_hDIB);

	file.Read((void *)ldib, m_vDIBSize);

	BITMAPINFO *pbi = (BITMAPINFO *)ldib;
	m_vSize.cx = pbi->bmiHeader.biWidth;
	m_vSize.cy = abs(pbi->bmiHeader.biHeight);

	::GlobalUnlock((HGLOBAL)m_hDIB);

	return TRUE;
}

BOOL CDib::GetBitmapInfoCopy(LPSTR *pbinfo, DWORD &size)
{
	*pbinfo = NULL;
	size = 0;
	if(!m_hDIB) return FALSE;

	LPSTR ldib, lpcore;
	DWORD header_size;

	ldib = (LPSTR)::GlobalLock((HGLOBAL)m_hDIB);
	lpcore = FindDIBBits(ldib);

	header_size = lpcore - ldib;

	*pbinfo = (LPSTR)::GlobalAlloc(GMEM_FIXED, header_size);
	::CopyMemory((void *)*pbinfo, (const void *)ldib, header_size);
	size = header_size;

	::GlobalUnlock((HGLOBAL)m_hDIB);
	
	return TRUE;
}

// DIB function
LPSTR CDib::FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + PaletteSize(lpbi));
}

WORD CDib::PaletteSize(LPSTR lpbi)
{
	return (WORD)(DIBNumColors(lpbi) * sizeof(RGBQUAD));
}

WORD CDib::DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;
	DWORD dwClrUsed;
	
	dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
	if (dwClrUsed != 0)
		return (WORD)dwClrUsed;
	
	wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	
	switch (wBitCount) {
	case 1:
		return 2;
	case 4:
		return 16;
	case 8:
		return 256;
	default:
		return 0;
	}
}

DWORD CDib::DIBWidth(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;

	return lpbmi->biWidth;
}

DWORD CDib::DIBHeight(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;

	return lpbmi->biHeight;
}

void CDib::Copy(CDib& dib)
{
	if(dib.m_hDIB == NULL) return;

	HDIB hDIB;
	LPSTR dst_dib, src_dib;

	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dib.m_vDIBSize);
	if(hDIB == 0) return;

	src_dib = (LPSTR) ::GlobalLock((HGLOBAL)dib.m_hDIB);
	dst_dib = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	CopyMemory(dst_dib, src_dib, dib.m_vDIBSize);

	m_vSize = dib.m_vSize;
	m_vDIBSize = dib.m_vDIBSize;

	::GlobalUnlock((HGLOBAL) hDIB);
	::GlobalUnlock((HGLOBAL) dib.m_hDIB);
	m_hDIB = hDIB;
}

BOOL CDib::ReadData(LPTSTR data, DWORD data_size)
{
	RemoveData();

	LPSTR ldib;

	m_hDIB = (HDIB)::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, data_size);
	if(!m_hDIB) return FALSE;

	ldib = (LPSTR)::GlobalLock((HGLOBAL)m_hDIB);

	CopyMemory((void *)ldib, data, data_size);

	BITMAPINFO *pbi = (BITMAPINFO *)ldib;
	m_vSize.cx = pbi->bmiHeader.biWidth;
	m_vSize.cy = abs(pbi->bmiHeader.biHeight);
	m_vDIBSize = data_size;

	::GlobalUnlock((HGLOBAL)m_hDIB);

	return TRUE;
}
