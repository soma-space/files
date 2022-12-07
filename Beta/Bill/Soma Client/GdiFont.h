// GdiFont.h: interface for the CGdiFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIFONT_H__B1608CC5_92CE_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_GDIFONT_H__B1608CC5_92CE_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGdiFont  
{
public:
	CGdiFont();
	virtual ~CGdiFont();

	BOOL CreateFont(CString strFontName, int nHeight);
	void DeleteFont();
	void TextOut(CDC& dc, CPoint pt, LPCTSTR lpszText, COLORREF bColor=RGB(255,255,255), UINT uFlag=TA_LEFT);
	int TextOutClip(CDC& dc, CRect rc, LPCTSTR lpszText, COLORREF bColor=RGB(255,255,255), 
		int caretPos=-1, int blockStart=0, int blockEnd=0, BOOL fWordBreak=TRUE);
public:
	CFont	*m_pFont;
	int lpnFit[100], alpDX[100];
	SIZE lpSize[100];
};

#endif // !defined(AFX_GDIFONT_H__B1608CC5_92CE_11D4_98B2_00C02615866B__INCLUDED_)
