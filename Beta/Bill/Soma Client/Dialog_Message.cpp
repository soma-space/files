// Dialog_Message.cpp: implementation of the CDialog_Message class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Message.h"
//#include "BBDX_TextOuter.h"
//#include "TWFont.h"
#include "ChildView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CUIRes *g_pShowDialog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Message::CDialog_Message()
{
//	INIT_PTR(m_pTWFont);
//	INIT_PTR(m_pOuter);
	m_pFont = NULL;
}

CDialog_Message::~CDialog_Message()
{
//	SAFE_DELETE(m_pTWFont);
//	SAFE_DELETE(m_pOuter);
	if(m_pFont) delete m_pFont;
}

BOOL CDialog_Message::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}
		
/*	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		delete m_pTWFont; m_pTWFont=NULL;
		TRACE("####### Dialog Message Init ����.. ############ \n");
		return FALSE;
	}

	m_pOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pOuter->SetSplitRegion(m_arrArea[0]->m_vRect.Width());*/
	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "����");

	return TRUE;
}

BOOL CDialog_Message::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;
	CRect rtDst;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 0);

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,0));

	rtDst = m_arrArea[0]->m_vRect; rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawShadowTexts(pDest, rtDst, 0xffe0, 0);
	pDC->DrawText(m_strMessage, &rtDst, DT_TOP|DT_LEFT|DT_WORDBREAK);

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	return TRUE;
}

BOOL CDialog_Message::OnButtonAction(int nSpec, int nIndex)
{
	if(nSpec == BTN_CTRL)
	{
		if(nIndex == 0)		// OK
		{
			HideWindow();
			if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 2);
		}
	}

	return TRUE;
}

void CDialog_Message::SetMessage(CString &message)
{
/*	char *t_buf;
	char seps[] = "\n";
	char *token;
	int len;
	CString t_str;

	len = message.GetLength();
	t_buf = new char[len+1];
	strcpy(t_buf, (LPCTSTR)message);
	t_buf[len] = 0;
	m_pOuter->ClearString();
	token = strtok(t_buf, seps);
	while(token != NULL)
	{
		t_str = token;
		m_pOuter->AddString(t_str);
		token = strtok(NULL, seps);
	}
	delete[] t_buf;*/
	m_strMessage = message;
}

void CDialog_Message::ShowWindow(int Id, CUIRes *pParent)
{
	if(!IsLoading())
	{
		CPoint t_pos;

		if(!Create(CChildView::DirectXMng.m_pDD, "message.lib", 0, TRUE, FALSE, FALSE))
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

void CDialog_Message::HideWindow()
{
	CUIRes::HideWindow();

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
}

void CDialog_Message::HideAllWindow()
{
	CUIRes::HideWindow();

	if(m_pParent)
	{
		m_pParent->m_pChild = NULL;
		m_pParent->HideAllWindow();
	}
}

BOOL CDialog_Message::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_RETURN || nChar == VK_ESCAPE)
	{
		HideWindow();
		if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 1);
		return TRUE;
	}
	return FALSE;
}
