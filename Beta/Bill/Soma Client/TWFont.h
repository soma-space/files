/////////////////////////////////////////////////////////////////////////////
// TWGame Include Files

#ifndef	_TWGAME_H_
#define _TWGAME_H_


//다크세이버 참조
/////////////////////////////////////////////////////////////////////////////
// CTWFont class : Load & Draw Font

class CTWFont
{
private :
	BOOL	m_bIs256;
	int		m_nX;
	int		m_nY;
	int		m_nCharLen;
	void*	m_pBuf;
	BYTE*	m_pBitsAsc;
	BYTE*	m_pBitsHCode1;
	BYTE*	m_pBitsHCode2;
	CFont	m_font;

	int     m_PutCharCount; //우석.. 출력된 글자수...

	void CaretDraw(CPoint pt, BYTE bColor, DDSURFACEDESC2& ddsd);
	void CaretDrawHi(CPoint pt, WORD bColor, DDSURFACEDESC2& ddsd);
	void DrawChar (BYTE* pBuf, CPoint pt, BYTE bColor, DDSURFACEDESC2& ddsd, BOOL bMakeBlock=FALSE);
	void DrawCharHi(BYTE* pBuf, CPoint pt, WORD bColor, DDSURFACEDESC2& ddsd, BOOL bMakeBlock=FALSE);

public :
	CTWFont ();
	~CTWFont ();

	int     GetPutCharCount() { return m_PutCharCount; } //우석...

	int		GetFontWidth(){return m_nX;}
	int		GetFontHeight(){return m_nY;}

	BOOL CreateFont (CString strFontName, int nSizeX, int nSizeY);
	BOOL ChangeFont (CString strFontName, int nSizeX, int nSizeY);
	BOOL CreateFontWithoutPath(CString strFontName, int nSizeX, int nSizeY);
	BOOL ChangeFontWithoutPath(CString strFontName, int nSizeX, int nSizeY);
	BOOL CreateFont (CString strFontName, int nSize);

	void TextOut (CPoint pt, LPCTSTR lpszText, BYTE bColor, 
		DDSURFACEDESC2& ddsd, UINT uFlag = TA_LEFT);
	void TextOutHi (CPoint pt, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, UINT uFlag = TA_LEFT);

	void TextOut (CPoint pt, LPCTSTR lpszText, COLORREF bColor, 
		CDC& dc, UINT uFlag = TA_LEFT);
	int TextOutClip (CRect rc, LPCTSTR lpszText, BYTE bColor, 
		DDSURFACEDESC2& ddsd, int blockStart, int blockEnd, int caretPos, int nLine = 0, BOOL fWordBreak = TRUE);
	int TextOutClipHi (CRect rc, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, int blockStart, int blockEnd, int caretPos, int nLine = 0, BOOL fWordBreak = TRUE);
	int TextOutClip (CRect rc, LPCTSTR lpszText, COLORREF bColor, 
		CDC& dc, int nLine = 0, BOOL fWordBreak = TRUE);
	int ChatOut(CRect rc, LPCTSTR lpszText, WORD bColor, DDSURFACEDESC2& ddsd, int nLine = 0, BOOL fWordBreak = TRUE);

	int	GetWordWidth (LPCTSTR lpszText);
	int GetLineWidth (LPCTSTR lpszText);
	int GetChatLineCount(int width, LPCTSTR lpszText);

	//Independent Color TextOut
	void IC_TextOut(CPoint pt, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, UINT uFlag = TA_LEFT);
	void IC_TextOutShadow(CPoint pt, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, UINT uFlag = TA_LEFT);

	int IC_TextOutClipShadow(CRect rc, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, int blockStart=0, int blockEnd=0, int caretPos = -1, int nLine = 0, BOOL fWordBreak = FALSE);

	int IC_ChatOutShadow(CRect rc, LPCTSTR lpszText, WORD bColor, DDSURFACEDESC2& ddsd, int nLine = 0, BOOL fWordBreak = FALSE);

	int IC_TextOutClip(CRect rc, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, int blockStart=0, int blockEnd=0, int caretPos = -1, int nLine = 0, BOOL fWordBreak = FALSE);
	void	SplitString(LPCTSTR lpszText, char *first, char *second, int width);

	//우석
	CPoint CTWFont::IC_MultiTextOutClip(CPoint startPt, CRect rc, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, int blockStart=0, int blockEnd=0, int caretPos = -1, int nLine = 0, BOOL fWordBreak = FALSE);

	CPoint CTWFont::MultiTextOutClip (CPoint startPt, CRect rc, LPCTSTR lpszText, BYTE bColor, 
		DDSURFACEDESC2& ddsd, int blockStart, int blockEnd, int caretPos, int nLine = 0, BOOL fWordBreak = TRUE);

	CPoint CTWFont::MultiTextOutClipHi (CPoint startPt, CRect rc, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, int blockStart, int blockEnd, int caretPos, int nLine = 0, BOOL fWordBreak = TRUE);
};

/////////////////////////////////////////////////////////////////////////////

inline int CTWFont::IC_TextOutClip(CRect rc, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, int blockStart, int blockEnd, int caretPos, int nLine, BOOL fWordBreak)
{
	if (m_bIs256) return TextOutClip(rc, lpszText, (BYTE)(bColor&0x00ff), ddsd, blockStart, blockEnd, caretPos, nLine, fWordBreak);
	return TextOutClipHi(rc, lpszText, bColor, ddsd, blockStart, blockEnd, caretPos, nLine, fWordBreak);
}

//우석..
inline CPoint CTWFont::IC_MultiTextOutClip(CPoint startPt, CRect rc, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, int blockStart, int blockEnd, int caretPos, int nLine, BOOL fWordBreak)
{
	if (m_bIs256) return MultiTextOutClip(startPt,rc, lpszText, (BYTE)(bColor&0x00ff), ddsd, blockStart, blockEnd, caretPos, nLine, fWordBreak);
	return MultiTextOutClipHi(startPt,rc, lpszText, bColor, ddsd, blockStart, blockEnd, caretPos, nLine, fWordBreak);
}

inline int CTWFont::IC_TextOutClipShadow(CRect rc, LPCTSTR lpszText, WORD bColor, 
		DDSURFACEDESC2& ddsd, int blockStart, int blockEnd, int caretPos, int nLine, BOOL fWordBreak)
{
	CRect r = rc;
	r.OffsetRect(1,1);
	WORD color = 0x0000;
	if (!m_bIs256) TextOutClipHi(r, lpszText, color, ddsd, 0, 0, -1, nLine, fWordBreak);

	if (m_bIs256) return TextOutClip(rc, lpszText, (BYTE)(bColor&0x00ff), ddsd, blockStart, blockEnd, caretPos, nLine, fWordBreak);
	return TextOutClipHi(rc, lpszText, bColor, ddsd, blockStart, blockEnd, caretPos, nLine, fWordBreak);
}

inline int CTWFont::IC_ChatOutShadow(CRect rc, LPCTSTR lpszText, WORD bColor, DDSURFACEDESC2& ddsd, int nLine, BOOL fWordBreak)
{
	CRect r = rc;
	r.OffsetRect(1,1);
	WORD color = 0x0000;
	if (!m_bIs256) ChatOut(r, lpszText, color, ddsd, nLine, fWordBreak);

	if (m_bIs256) return TextOutClip(rc, lpszText, (BYTE)(bColor&0x00ff), ddsd, 0, 0, -1, nLine, fWordBreak);
	return ChatOut(rc, lpszText, bColor, ddsd, nLine, fWordBreak);
}

inline void CTWFont::IC_TextOut(CPoint pt, LPCTSTR lpszText, WORD bColor, 
	DDSURFACEDESC2& ddsd, UINT uFlag)
{
	if (m_bIs256) TextOut(pt, lpszText, (BYTE)(bColor&0x00ff), ddsd, uFlag);
	else TextOutHi(pt, lpszText, bColor, ddsd, uFlag);
}

inline void CTWFont::IC_TextOutShadow(CPoint pt, LPCTSTR lpszText, WORD bColor, 
	DDSURFACEDESC2& ddsd, UINT uFlag)
{
	CPoint spt = pt;
	spt.Offset(1,1);
	WORD color = 0x0000;
	if (!m_bIs256) TextOutHi(spt, lpszText, color, ddsd, uFlag);

	if (m_bIs256) TextOut(pt, lpszText, (BYTE)(bColor&0x00ff), ddsd, uFlag);
	else TextOutHi(pt, lpszText, bColor, ddsd, uFlag);
}

#endif//_TWGAME_H_
