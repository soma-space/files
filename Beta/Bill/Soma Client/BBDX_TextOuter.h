#ifndef __BBDX_TEXTOUTER_H
#define __BBDX_TEXTOUTER_H

class CTWFont;
class CDDSurface;

class BBDX_TextOuter
{
public:
	BBDX_TextOuter(CString fontname, int x, int y);
	BBDX_TextOuter(CTWFont *font, int x, int y);//
	virtual ~BBDX_TextOuter();

	void	ReArrange();//splitWidth 가 변경된 후 새로 String을 정리함
	void	SetSplitRegion(int splitWidth) {m_iSplitWidth = splitWidth;}
	int		AddString(CString &str);
	void	SetString(CString &str);

	//우석.
	void	SetMultiLineString(CString &str);
	void    DrawMultiLineTexts(CDDSurface *dest, CRect r, WORD color, int lineGap, bool bInvert = false);
	void	DrawTextsFromIndexedLine(CDDSurface *dest, CRect r, int from=0, WORD color=0xFFFF, int lineGap=0);
	void	DrawTexts(CDDSurface *dest, CRect r, WORD color=0xFFFF, int lineGap=0); //우석.
	void	DrawShadowTexts(CDDSurface *dest, CRect r, WORD color=0xFFFF, int lineGap=0);
	int		GetLineCount() {return m_strList.GetCount();}
	void	SetMaxLineNum(int lineNum) {m_iMaxLineNum = lineNum;}
	int		GetMaxLineNum() {return m_iMaxLineNum;}
	CString	GetString(int lineNum);

	void	ClearString(){m_strList.RemoveAll();}
	int		GetFontHeight(){return m_iHeight;}
	int		GetFontWidth(){return m_iWidth;}
	CTWFont*	GetBitmapFont(){return m_pBitmapFont;}

protected:
	CTWFont	*m_pBitmapFont;
	int		m_iMaxLineNum;
	int		m_iWidth;
	int		m_iHeight;
	int		m_iSplitWidth;
	BOOL	m_bShouldDelete;

	CStringList	m_strList;
};

inline CString	BBDX_TextOuter::GetString(int lineNum)
{
	CString ret_val;
	int count = m_strList.GetCount();
	if (lineNum>=count) return ret_val;
	POSITION pos = m_strList.FindIndex(lineNum);
	if (pos) ret_val = m_strList.GetNext(pos);
	return ret_val;
}

#endif