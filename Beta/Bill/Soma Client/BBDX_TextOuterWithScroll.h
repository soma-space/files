#ifndef __BBDX_TEXTOUTERWITHSCROLL_H
#define __BBDX_TEXTOUTERWITHSCROLL_H

#include "BBDX_TextOuter.h"

class BBDX_TextOuterWithScroll : public BBDX_TextOuter
{
public:
	BBDX_TextOuterWithScroll(CString fontname, int x, int y, CRect area, int maxLine, int lineGap=0);
	BBDX_TextOuterWithScroll(CTWFont *font, int x, int y, CRect area, int maxLine, int lineGap=0);
	virtual ~BBDX_TextOuterWithScroll();

	void	ScrollDown();
	void	ScrollUp();
	BOOL	ScrollPageDown();
	void	ScrollPageUp();
	int		GetScrollRange();

	void	Draw(CDDSurface *dest, CPoint offset=CPoint(0,0), WORD color = 0xFFFF);
	void	DrawList(CDDSurface *dest, int focusNum = -1);
	virtual int		CalcShowLineNum();

	int		GetLineIndexWithPoint(CPoint &point);
	void	ClearTexts(){m_strList.RemoveAll(); m_iScrollPos=0;}
	CRect&	GetArea(){return m_rectArea;}
	int		GetShowLineNum(){return m_iShowLineNum;}
	int		GetScrollPos(){return m_iScrollPos;}
	void	SetScrollPos(int pos);
protected:
	CRect	m_rectArea;
	int		m_gapLine;
	int		m_iScrollPos;//มู น๘ศฃ
	int		m_iShowLineNum;
};

#endif