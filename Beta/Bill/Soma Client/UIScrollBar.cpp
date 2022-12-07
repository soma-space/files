// UIScrollBar.cpp: implementation of the CUIScrollBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "UIScrollBar.h"
#include "DManage/BBDX_MouseInformation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIScrollBar::CUIScrollBar()
{
	INIT_PTR(m_bpUIRes);
	m_nViewLineCount = m_nMaxLineCount = 0;
}

CUIScrollBar::~CUIScrollBar()
{

}

BOOL CUIScrollBar::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CPoint pos = pMouseInfo->GetPos();
	CPoint ptPos = pos;
	ptPos -= (*m_pPosOffset);
	if (!m_rtThumb.PtInRect(ptPos)) {
		return FALSE;
	}
	
	m_bCapture = TRUE;
	m_ptCapturePos = pos;

	m_nState = SCROLL_DOWN;
	
	return TRUE;
}

BOOL CUIScrollBar::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	CPoint pos;

	pos = pMouseInfo->GetPos();

	if (!m_bCapture) {
		return FALSE;
	}
	
	m_rtThumb.OffsetRect(CPoint(0, pos.y-m_ptCapturePos.y));
	if (m_rtThumb.top < m_rtScroll.top) {
		m_rtThumb.bottom = m_rtScroll.top + m_rtThumb.Height();
		m_rtThumb.top = m_rtScroll.top;
	}
	else if (m_rtThumb.bottom > m_rtScroll.bottom) {
		m_rtThumb.top = m_rtScroll.bottom-m_rtThumb.Height();
		m_rtThumb.bottom = m_rtScroll.bottom;
	}

	m_bCapture = FALSE;

	int nRange = m_rtScroll.Height()-m_rtThumb.Height();
	m_nStartLine = (m_nMaxLineCount-m_nViewLineCount)*(m_rtThumb.top-m_rtScroll.top)/nRange;

	m_nState = SCROLL_NORMAL;
	OnMouseMove(pMouseInfo);

	return TRUE;
}

BOOL CUIScrollBar::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	CPoint pos = pMouseInfo->GetPos();
	CPoint ptPos = pos;
	
	if (!m_bCapture) {
		ptPos -= (*m_pPosOffset);
		if (m_rtThumb.PtInRect(ptPos)) {
			m_nState = SCROLL_FOCUS;
		}
		else {
			m_nState = SCROLL_NORMAL;
		}
		return FALSE;
	}

	//m_rtThumb.OffsetRect(CPoint(pos.x-m_ptCapturePos.x, pos.y-m_ptCapturePos.y));
	m_rtThumb.OffsetRect(CPoint(0, pos.y-m_ptCapturePos.y));
	if (m_rtThumb.top < m_rtScroll.top) {
		m_rtThumb.bottom = m_rtScroll.top + m_rtThumb.Height();
		m_rtThumb.top = m_rtScroll.top;
	}
	else if (m_rtThumb.bottom > m_rtScroll.bottom) {
		m_rtThumb.top = m_rtScroll.bottom-m_rtThumb.Height();
		m_rtThumb.bottom = m_rtScroll.bottom;
	}
	else {
		m_ptCapturePos = pos;
	}
	
	int nRange = m_rtScroll.Height()-m_rtThumb.Height();
	m_nStartLine = (m_nMaxLineCount-m_nViewLineCount)*(m_rtThumb.top-m_rtScroll.top)/nRange;

	return TRUE;
} 

BOOL CUIScrollBar::Create(CUIRes *pUIRes, CRect rtScroll, int nResRectIndex) 
{
	ASSERT(pUIRes);
	m_bpUIRes = pUIRes;

	m_rtScroll = rtScroll;
	
	m_pPosOffset = &pUIRes->m_ptOffset;
	m_bpButton = pUIRes->m_arrBtnEx[nResRectIndex];

	m_rtThumb = rtScroll;
	m_rtThumb.right = m_rtThumb.left+m_bpButton->m_vRect.Width();
	m_rtThumb.bottom = m_rtThumb.top+m_bpButton->m_vRect.Height();

	
	m_bCapture = FALSE;


	InitScroll();

	return TRUE;
};

void CUIScrollBar::Draw(CDDSurface *pDest)
{
	if(m_nMaxLineCount > m_nViewLineCount)
	{
		CPoint ptDst(m_rtThumb.left, m_rtThumb.top);
		ptDst.Offset(*m_pPosOffset);

		switch(m_nState) {
		case BUTTON_NORMAL:
			pDest->BltFast(ptDst, m_bpUIRes->m_pDDSurface, 
				&(m_bpButton->m_vNormalRect), DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT);
			break;
		case BUTTON_FOCUS:
			pDest->BltFast(ptDst, m_bpUIRes->m_pDDSurface, 
				&(m_bpButton->m_vFocusRect), DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT);
			break;
		case BUTTON_DOWN:
			pDest->BltFast(ptDst, m_bpUIRes->m_pDDSurface, 
				&(m_bpButton->m_vDownRect), DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT);
			break;
		}
	}
};

void CUIScrollBar::InitScroll()
{
	m_bCapture = FALSE;

	m_nMaxLineCount = 0;
	m_nViewLineCount = 0;
	m_nStartLine = 0;

	SetStartLine(m_nStartLine);

	m_nState = SCROLL_NORMAL;
}

void CUIScrollBar::SetStartLine(int nStartLine)
{
	if(m_nMaxLineCount <= m_nViewLineCount)
	{
		m_nStartLine = 0;
		m_rtThumb.bottom = m_rtScroll.top + m_rtThumb.Height();
		m_rtThumb.top = m_rtScroll.top;
	}
	else
	{
		if (nStartLine>(m_nMaxLineCount-m_nViewLineCount)) {
			m_nStartLine=m_nMaxLineCount-m_nViewLineCount;
		}
		else if (nStartLine<0) {
			m_nStartLine = 0;
		}
		else {
			m_nStartLine=nStartLine;
		}
		
		int nLength = m_rtThumb.Height();
		if (m_nMaxLineCount>m_nViewLineCount) {
			ASSERT(m_nStartLine<=(m_nMaxLineCount-m_nViewLineCount));
			m_rtThumb.top = m_rtScroll.top + (m_rtScroll.Height()-nLength)*m_nStartLine/(m_nMaxLineCount-m_nViewLineCount);
			m_rtThumb.bottom = m_rtThumb.top+nLength;
		}
		else {
			m_rtThumb.top = m_rtScroll.top;
			m_rtThumb.bottom = m_rtThumb.top+nLength;
		}
	}		
};

void CUIScrollBar::SetEndPos()
{
	if(m_nMaxLineCount < m_nViewLineCount) return;
	m_nStartLine = m_nMaxLineCount-m_nViewLineCount;
	SetStartLine(m_nStartLine);
}