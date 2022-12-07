// BB_Bitmap.h: interface for the BB_Bitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BB_BITMAP_H__0A6446E4_A2D6_11D1_8BEE_0060979C5900__INCLUDED_)
#define AFX_BB_BITMAP_H__0A6446E4_A2D6_11D1_8BEE_0060979C5900__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDirectDraw;
class CDDSurface;

class AFX_EXT_CLASS BB_Bitmap  
{
public:
	BOOL WriteBitmap(CString szFile);
	CPalette* StealPalette();
	CBitmap* SealBitmap();
	static void LoadPaletteToPara(CPalette **pal, PALETTEENTRY palentrys[256]);
	void LoadPalette(PALETTEENTRY palentrys[256]);
	CSize& GetBitmapSize();
	BOOL LoadFileBitmap(CString filename);
	BOOL LoadFileBitmapFromLib(CFile &file, long offset, long nFileLen);
	CPalette* GetPalette();
	CBitmap* GetBitmap();
	static HPALETTE CreateDIBPalette (LPBITMAPINFO lpbmi, LPINT lpiNumColors);
	void DeleteObject();
	BOOL LoadResourceBitmap(UINT bitmapID, BOOL bMakePalette=TRUE);
	BB_Bitmap();
	virtual ~BB_Bitmap();

	static CDDSurface*	CreateSurfaceFromLib(CDirectDraw *pDirectDraw, CFile &file, long offset, long nFileLen);
	BOOL		CopySurfaceFromLib(CDDSurface *pSurface, CFile &flie, long offset, long nFileLen);
protected:
	HANDLE DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal );
	CSize m_BitmapSize;
	CPalette * m_pPalette;
	CBitmap* m_pBitmap;
};

inline void BB_Bitmap::DeleteObject()
{
	if (m_pBitmap) delete m_pBitmap, m_pBitmap=NULL;
	if (m_pPalette) delete m_pPalette, m_pPalette=NULL;
	m_BitmapSize.cx = m_BitmapSize.cy = 0;
}

inline CBitmap* BB_Bitmap::GetBitmap()
{
	return m_pBitmap;
}

inline CPalette* BB_Bitmap::GetPalette()
{
	return m_pPalette;
}

inline CSize& BB_Bitmap::GetBitmapSize()
{
	return m_BitmapSize;
}
//반드시 호출한 함수에서 CBitmap의 파괴를 책임져야함.
inline CBitmap* BB_Bitmap::SealBitmap()
{
	CBitmap *tmp = m_pBitmap;
	m_pBitmap = NULL;
	return tmp;
}
//반드시 호출한 함수에서 CPalette의 파괴를 책임져야함.
inline CPalette* BB_Bitmap::StealPalette()
{
	CPalette *tmp = m_pPalette;
	m_pPalette = NULL;
	return tmp;
}

#endif // !defined(AFX_BB_BITMAP_H__0A6446E4_A2D6_11D1_8BEE_0060979C5900__INCLUDED_)
