// Dialog_Chat.cpp: implementation of the CDialog_Chat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Chat.h"
#include "BBDX_ChatOuter.h"
#include "ChildView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CChildView			*g_pChildView;
extern CUIRes				*g_pShowDialog;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Chat::CDialog_Chat()
{
	INIT_PTR(m_pScrollBar);
	m_vPosMode = POS_LEFT;
}

CDialog_Chat::~CDialog_Chat()
{
	SAFE_DELETE(m_pScrollBar);
}

void CDialog_Chat::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Chat.lib", 0, FALSE, FALSE, FALSE))
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	m_pScrollBar->InitScroll();

	CUIRes::ShowWindow();

	m_pChatOuter = g_pChildView->GetChatOuter();
	CRect t_rect = m_arrArea[0]->m_vRect;
	t_rect.OffsetRect(m_ptOffset);
	m_pChatOuter->ShowMode(2);
	m_pChatOuter->ChatAreaChange(t_rect);
	
	SetScrollData();
}

void CDialog_Chat::HideWindow()
{
	g_pShowDialog = NULL;

	HideAllWindow();
}

void CDialog_Chat::HideAllWindow()
{
	CUIRes::HideWindow();

	m_pChatOuter->ShowMode(0);
}

BOOL CDialog_Chat::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[1]->m_vRect, 0);

	return TRUE;
}

BOOL CDialog_Chat::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return FALSE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 2);

	m_pScrollBar->Draw(pDest);

	m_pChatOuter->SetScrollPos(m_pScrollBar->GetStartLine());

	return TRUE;
}

void CDialog_Chat::SetScrollData()
{
	m_pScrollBar->SetMaxLineCount(m_pChatOuter->GetLineCount());
	m_pScrollBar->SetViewLineCount(m_pChatOuter->GetShowLineNum());
	m_pScrollBar->SetStartLine(m_pChatOuter->GetScrollPos());
}

BOOL CDialog_Chat::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if (m_pScrollBar->OnLButtonDown(pMouseInfo) ) {
		SetFocus();
		SetCapture();
		return FALSE;
	}

	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_Chat::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();
	if (m_pScrollBar->OnLButtonUp(pMouseInfo) ) {
		return FALSE;
	}

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_Chat::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if (m_pScrollBar->OnMouseMove(pMouseInfo) ) {
		return FALSE;
	}

	return CUIRes::OnMouseMove(pMouseInfo);
}

BOOL CDialog_Chat::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// ´Ý±â
			HideWindow();
			break;
		case 1:
			m_pScrollBar->ScrollUp();
			break;
		case 2:
			m_pScrollBar->ScrollDown();
			break;
		}
	}

	return TRUE;
}

void CDialog_Chat::ScrollDown()
{
	m_pScrollBar->ScrollDown();
}

void CDialog_Chat::ScrollUp()
{
	m_pScrollBar->ScrollUp();
}

BOOL CDialog_Chat::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL retVal = FALSE;

	switch(nChar)
	{
	case VK_UP:
		m_pChatOuter->ScrollUp();
		retVal = TRUE;
		break;
	case VK_DOWN:
		m_pChatOuter->ScrollDown();
		retVal = TRUE;
		break;
	case VK_PRIOR:
		m_pChatOuter->ScrollPageUp();
		retVal = TRUE;
		break;
	case VK_NEXT:
		m_pChatOuter->ScrollPageDown();
		retVal = TRUE;
		break;
	}

	if(retVal)
	{
		SetScrollData();
		return TRUE;
	}

	if(CUIRes::OnKeyDown(nChar, nRepCnt, nFlags)) return TRUE;
	
	return FALSE;
}