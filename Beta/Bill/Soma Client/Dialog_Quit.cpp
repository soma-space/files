// Dialog_Quit.cpp: implementation of the CDialog_Quit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Quit.h"
#include "ChildView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CUIRes *g_pShowDialog;

void SendMagicBelt();
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Quit::CDialog_Quit()
{
	m_vId = -1;
}

CDialog_Quit::~CDialog_Quit()
{

}

BOOL CDialog_Quit::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}
		
	return TRUE;
}

BOOL CDialog_Quit::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 1);

	return TRUE;
}

BOOL CDialog_Quit::OnButtonAction(int nSpec, int nIndex)
{
	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:		// Ok
			HideWindow();
			if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 1);
			else
			{
				SendMagicBelt();
				//AfxPostQuitMessage(0);
				//if(__GetMainWnd())  __GetMainWnd()->m_wndView.DDRelease();
				CChildView::SocketMng.SendLogout(1);
			}
			break;
		case 1:		// Cancel
			HideWindow();
			if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 2);
			break;
		}
	}

	return TRUE;
}

void CDialog_Quit::ShowWindow(int Id, CUIRes *pParent)
{
	if(!IsLoading())
	{
		CPoint t_pos;

		if(!Create(CChildView::DirectXMng.m_pDD, "quit.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
		t_pos.x = (800 - m_mainRect.Width()) / 2;
		t_pos.y = (600 - m_mainRect.Height()) / 2;
		SetOffset(t_pos);
	}

	if(g_pShowDialog && g_pShowDialog != pParent) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	m_vId = Id;
	m_pParent = pParent;
	if(m_pParent) m_pParent->m_pChild = this;
	CUIRes::ShowWindow();
}

void CDialog_Quit::HideWindow()
{
	if(m_pParent)
	{
		m_pParent->m_pChild = NULL;
		if(m_pParent->IsVisible())
		{
			g_pShowDialog = m_pParent;
			m_pParent->SetFocus();
		}
		else g_pShowDialog = NULL;
	}
	else g_pShowDialog = NULL;

	CUIRes::HideWindow();
}

void CDialog_Quit::HideAllWindow()
{
	if(m_pParent)
	{
		m_pParent->m_pChild = NULL;
		m_pParent->HideAllWindow();
	}

	CUIRes::HideWindow();
}

BOOL CDialog_Quit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_RETURN)
	{
		HideWindow();
		if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 1);
		else
		{
			SendMagicBelt();
			//AfxPostQuitMessage(0);
			CChildView::SocketMng.SendLogout(1);
		}
		return TRUE;
	}
	else if(nChar == VK_ESCAPE)
	{
		HideWindow();
		if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 2);
		return TRUE;
	}

	return FALSE;
}
