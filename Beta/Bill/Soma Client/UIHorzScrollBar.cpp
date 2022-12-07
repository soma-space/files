// UIHorzScrollBar.cpp: implementation of the CUIHorzScrollBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "UIHorzScrollBar.h"
#include "DManage/BBDX_MouseInformation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SCROLL_NORMAL	0
#define SCROLL_FOCUS	1
#define SCROLL_DOWN		2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIHorzScrollBar::CUIHorzScrollBar()
{
	INIT_PTR(m_bpUIRes);
	m_nViewLineCount = m_nMaxLineCount = 0;
}

CUIHorzScrollBar::~CUIHorzScrollBar()
{

}

BOOL CUIHorzScrollBar::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
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

BOOL CUIHorzScrollBar::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	CPoint pos;

	pos = pMouseInfo->GetPos();

	if (!m_bCapture) {
		return FALSE;
	}
	
	m_rtThumb.OffsetRect(CPoint(pos.x-m_ptCapturePos.x, 0));
	if (m_rtThumb.left < m_rtScroll.left) {
		m_rtThumb.right = m_rtScroll.left + m_rtThumb.Width();
		m_rtThumb.left = m_rtScroll.left;
	}
	else if (m_rtThumb.right > m_rtScroll.right) {
		m_rtThumb.left = m_rtScroll.right-m_rtThumb.Width();
		m_rtThumb.right = m_rtScroll.right;
	}

	m_bCapture = FALSE;

	int nRange = m_rtScroll.Width()-m_rtThumb.Width();
	m_nStartLine = (m_nMaxLineCount-m_nViewLineCount)*(m_rtThumb.left-m_rtScroll.left)/nRange;

	m_nState = SCROLL_NORMAL;
	OnMouseMove(pMouseInfo);

	return TRUE;
}

BOOL CUIHorzScrollBar::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
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
	m_rtThumb.OffsetRect(CPoint(pos.x-m_ptCapturePos.x, 0));
	if (m_rtThumb.left < m_rtScroll.left) {
		m_rtThumb.right = m_rtScroll.left + m_rtThumb.Width();
		m_rtThumb.left = m_rtScroll.left;
	}
	else if (m_rtThumb.right > m_rtScroll.right) {
		m_rtThumb.left = m_rtScroll.right-m_rtThumb.Width();
		m_rtThumb.right = m_rtScroll.right;
	}
	else {
		m_ptCapturePos = pos;
	}
	
	int nRange = m_rtScroll.Width()-m_rtThumb.Width();
	m_nStartLine = (m_nMaxLineCount-m_nViewLineCount)*(m_rtThumb.left-m_rtScroll.left)/nRange;

	return TRUE;
} 

BOOL CUIHorzScrollBar::Create(CUIRes *pUIRes, CRect rtScroll, int nResRectIndex) 
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

void CUIHorzScrollBar::Draw(CDDSurface *pDest)
{
	if(m_nMaxLineCount > m_nViewLineCount)
	{
		CPoint ptDst(m_rtThumb.left, m_rtThumb.top);
		ptDst.Offset(*m_pPosOffset);

		switch(m_nState) {
		case SCROLL_NORMAL:
			pDest->BltFast(ptDst, m_bpUIRes->m_pDDSurface, 
				&(m_bpButton->m_vNormalRect), DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT);
			break;
		case SCROLL_FOCUS:
			pDest->BltFast(ptDst, m_bpUIRes->m_pDDSurface, 
				&(m_bpButton->m_vFocusRect), DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT);
			break;
		case SCROLL_DOWN:
			pDest->BltFast(ptDst, m_bpUIRes->m_pDDSurface, 
				&(m_bpButton->m_vDownRect), DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT);
			break;
		}
	}
};

void CUIHorzScrollBar::InitScroll()
{
	m_bCapture = FALSE;

	m_nMaxLineCount = 0;
	m_nViewLineCount = 0;
	m_nStartLine = 0;

	SetStartLine(m_nStartLine);

	m_nState = SCROLL_NORMAL;
}

void CUIHorzScrollBar::SetStartLine(int nStartLine)
{
	if(m_nMaxLineCount <= m_nViewLineCount)
	{
		m_nStartLine = 0;
		m_rtThumb.right = m_rtScroll.left + m_rtThumb.Width();
		m_rtThumb.left = m_rtScroll.left;
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
		
		int nLength = m_rtThumb.Width();
		if (m_nMaxLineCount>m_nViewLineCount) {
			ASSERT(m_nStartLine<=(m_nMaxLineCount-m_nViewLineCount));
			m_rtThumb.left = m_rtScroll.left + (m_rtScroll.Width()-nLength)*m_nStartLine/(m_nMaxLineCount-m_nViewLineCount);
			m_rtThumb.right = m_rtThumb.left+nLength;
		}
		else {
			m_rtThumb.left = m_rtScroll.left;
			m_rtThumb.right = m_rtThumb.left+nLength;
		}
	}		
};

void CUIHorzScrollBar::SetEndPos()
{
	if(m_nMaxLineCount < m_nViewLineCount) return;
	m_nStartLine = m_nMaxLineCount-m_nViewLineCount;
	SetStartLine(m_nStartLine);
}