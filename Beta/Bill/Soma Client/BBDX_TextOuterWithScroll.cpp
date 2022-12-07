#include "stdafx.h"
#include "Soma.h"
#include "BBDX_TextOuterWithScroll.h"
#include "DManage/BB_Direct.h"
#include "TWFont.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BBDX_TextOuterWithScroll::BBDX_TextOuterWithScroll(CString fontname, int x, int y, CRect area, int maxLine, int lineGap)
:BBDX_TextOuter(fontname, x, y)
{
	m_rectArea = area;
	m_gapLine = lineGap;
	SetMaxLineNum(maxLine);
	m_iScrollPos = 0;
	m_iShowLineNum = CalcShowLineNum();
}

BBDX_TextOuterWithScroll::BBDX_TextOuterWithScroll(CTWFont *font, int x, int y, CRect area, int maxLine, int lineGap)
:BBDX_TextOuter(font, x, y)
{
	m_rectArea = area;
	m_gapLine = lineGap;
	SetMaxLineNum(maxLine);
	m_iScrollPos = 0;
	m_iShowLineNum = CalcShowLineNum();
}

BBDX_TextOuterWithScroll::~BBDX_TextOuterWithScroll()
{

}

void BBDX_TextOuterWithScroll::ScrollUp()
{
	if (m_iScrollPos>0) m_iScrollPos--;
}

void BBDX_TextOuterWithScroll::ScrollDown()
{
	int count = m_strList.GetCount();
	int scrollRange = count-m_iShowLineNum;
	if (scrollRange<=0) return;
	if (m_iScrollPos<scrollRange) m_iScrollPos ++;
}

BOOL BBDX_TextOuterWithScroll::ScrollPageDown()
{
	int count = m_strList.GetCount();
	int scrollRange = count-m_iShowLineNum;
	if (scrollRange<=0) return FALSE;
	if (m_iScrollPos+m_iShowLineNum<count)	
	{
		m_iScrollPos += m_iShowLineNum;
		return TRUE;
	}
	return FALSE;
}

void BBDX_TextOuterWithScroll::ScrollPageUp()
{
	m_iScrollPos -= m_iShowLineNum;
	if (m_iScrollPos<0) m_iScrollPos = 0;
}

void BBDX_TextOuterWithScroll::Draw(CDDSurface *dest, CPoint offset, WORD color)
{
	CRect r = m_rectArea;
	r.OffsetRect(offset);
	DrawTextsFromIndexedLine(dest, r, m_iScrollPos, color, m_gapLine);
}

int BBDX_TextOuterWithScroll::CalcShowLineNum()
{
	int linenum;

	int height = m_rectArea.Height();
	int lineHeight = m_iHeight+m_gapLine;
	
	linenum = (height+m_gapLine)/lineHeight;

	return linenum;
}


void BBDX_TextOuterWithScroll::DrawList(CDDSurface *dest, int focusNum)
{
	WORD color = 0XFFFF;
	WORD focusColor = 0xbbbb;

	CRect r=m_rectArea;
	int from = m_iScrollPos;
	int lineGap = m_gapLine;

	if (m_iSplitWidth!=r.Width()) TRACE("BBDX_TextOuter's split width is not equal drawing rect's width\n");

	if (!m_pBitmapFont) return;

	r.NormalizeRect();

	int count = m_strList.GetCount();
	POSITION pos = m_strList.FindIndex(from);
	
	DDSURFACEDESC2 ddsd;
	dest->GetDescription(&ddsd);
	ddsd.lpSurface = dest->Lock();

	CRect curRect;
	CRect tmpR;

	for (int i=from; i<count&&pos; i++)
	{
		curRect.SetRect(r.left, r.top+(i-from)*(m_iHeight+lineGap), r.right, 0);
		curRect.bottom = curRect.top+m_iHeight+lineGap;

		curRect.NormalizeRect();
		tmpR = curRect;
		tmpR &= r;
		if (tmpR!=curRect) break;

		m_pBitmapFont->IC_TextOutClip(curRect, m_strList.GetNext(pos), (i==focusNum)? focusColor : color, ddsd, 0, 0, -1, lineGap);
	}

	dest->Unlock();
}

int	BBDX_TextOuterWithScroll::GetLineIndexWithPoint(CPoint &point)
{
	int count = m_strList.GetCount();
	CRect r;
	for (int i=m_iScrollPos; i<count; i++)
	{
		r.SetRect(m_rectArea.left, m_rectArea.top+(i-m_iScrollPos)*(m_iHeight+m_gapLine), m_rectArea.right, 0);
		r.bottom = r.top+m_iHeight+m_gapLine;
		if (r.PtInRect(point)) return i;
	}
	return -1;
}

int BBDX_TextOuterWithScroll::GetScrollRange()
{
	int count;

	count = m_strList.GetCount() - m_iShowLineNum;
	if(count < 0) count = 0;

	return count;
}

void BBDX_TextOuterWithScroll::SetScrollPos(int pos)
{
	int scrollRange = m_strList.GetCount()-m_iShowLineNum;

	if(scrollRange <= 0) return;
	if(pos < 0)
	{
		m_iScrollPos = 0;
		return;
	}
	else if(pos >= scrollRange)
	{
		m_iScrollPos = scrollRange;
		return;
	}
	else
	{
		m_iScrollPos = pos;
	}
}