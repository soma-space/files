// GdiFont.cpp: implementation of the CGdiFont class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "GdiFont.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DEFAULT_FONT	_T("±¼¸²Ã¼")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGdiFont::CGdiFont()
{
	m_pFont = NULL;
}

CGdiFont::~CGdiFont()
{
	DeleteFont();
}

void CGdiFont::DeleteFont()
{
	if(m_pFont)
	{
		m_pFont->DeleteObject();
		delete m_pFont;
		m_pFont = NULL;
	}
}

BOOL CGdiFont::CreateFont(CString strFontName, int nHeight)
{
	DeleteFont();

	m_pFont = new CFont();
	if(m_pFont->CreateFont(-nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, HANGEUL_CHARSET,
		OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
		FF_DONTCARE|VARIABLE_PITCH, DEFAULT_FONT) == FALSE)
	{
		delete m_pFont;
		m_pFont = NULL;
		return FALSE;
	}

	return TRUE;
}

void CGdiFont::TextOut(CDC& dc, CPoint pt, LPCTSTR lpszText, COLORREF bColor, UINT uFlag)
{
	dc.SetTextColor(bColor);
	dc.SetTextAlign(uFlag);
	dc.TextOut(pt.x, pt.y, lpszText, strlen(lpszText));
}

int CGdiFont::TextOutClip(CDC& dc, CRect rc, LPCTSTR lpszText, COLORREF bColor,
	int caretPos, int blockStart, int blockEnd, BOOL fWordBreak)
{
	CFont *old_font;
	if(m_pFont) old_font = (CFont *)dc.SelectObject(m_pFont);
	GetTextExtentExPoint(dc.GetSafeHdc(), lpszText, strlen(lpszText), rc.Width(), lpnFit, alpDX, lpSize);
	if(m_pFont) dc.SelectObject(old_font);
	return 1;
}