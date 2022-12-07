// Dialog_Crest.cpp: implementation of the CDialog_Crest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Crest.h"
#include "BBDX_TextOuter.h"
#include "TWFont.h"
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

CDialog_Crest::CDialog_Crest()
{
	INIT_PTR(m_pTWFont);
	INIT_PTR(m_pOuter);
	m_pSymbolSurface = NULL;
	m_pDDSymbolSurface = NULL;
	m_vId = -1;
}

CDialog_Crest::~CDialog_Crest()
{
	SAFE_DELETE(m_pTWFont);
	SAFE_DELETE(m_pOuter);
	SAFE_DELETE(m_pDDSymbolSurface);
	RELEASE_SAFE(m_pSymbolSurface);
}

BOOL CDialog_Crest::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}
		
	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		delete m_pTWFont; m_pTWFont=NULL;
		TRACE("####### Dialog Crest Init ½ÇÆÐ.. ############ \n");
		return FALSE;
	}

	m_pOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pOuter->SetSplitRegion(m_arrArea[0]->m_vRect.Width());

	return TRUE;
}

BOOL CDialog_Crest::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CRect rtDst, rtSrc;
	CPoint t_pos;
	DWORD dwTrans;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 1);

	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	rtSrc.SetRect(0, 0, 16, 14);
	t_pos = m_arrArea[1]->m_vRect.TopLeft(); t_pos.Offset(m_ptOffset);
	pDest->BltFast(t_pos, m_pDDSymbolSurface, &rtSrc, dwTrans);

	rtDst = m_arrArea[0]->m_vRect; rtDst.OffsetRect(m_ptOffset);
	m_pOuter->DrawShadowTexts(pDest, rtDst, 0xffe0, 0);

	return TRUE;
}

BOOL CDialog_Crest::OnButtonAction(int nSpec, int nIndex)
{
	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:		// Ok
			HideWindow();
			if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 1);
			break;
		case 1:		// Cancel
			HideWindow();
			if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 2);
			break;
		}
	}

	return TRUE;
}

void CDialog_Crest::SetMessage(CString &message)
{
	char *t_buf;
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
	delete[] t_buf;
}

BOOL CDialog_Crest::ShowWindow(int Id, CUIRes *pParent)
{
	if(!IsLoading())
	{
		CPoint t_pos;

		if(!Create(CChildView::DirectXMng.m_pDD, "Crest.lib", 0, TRUE, FALSE, FALSE))
		{
			return TRUE;
		}
		t_pos.x = (800 - m_mainRect.Width()) / 2;
		t_pos.y = (600 - m_mainRect.Height()) / 2;
		SetOffset(t_pos);
	}

	SAFE_DELETE(m_pDDSymbolSurface);
	RELEASE_SAFE(m_pSymbolSurface);
	CChildView::g_pTEng.PathSetCur("SYMBOL");
	m_pSymbolSurface = CChildView::g_pTEng.SurfaceCreateWithBMP("Symbol.bmp", FALSE, RGB(255,0,255));
	if(m_pSymbolSurface == NULL) return FALSE;
	m_pDDSymbolSurface = new CDDSurface(m_pSymbolSurface);

	if(g_pShowDialog && g_pShowDialog != pParent) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	m_vId = Id;
	m_pParent = pParent;
	if(m_pParent) m_pParent->m_pChild = this;
	CUIRes::ShowWindow();

	return TRUE;
}

void CDialog_Crest::HideWindow()
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

void CDialog_Crest::HideAllWindow()
{
	SAFE_DELETE(m_pDDSymbolSurface);
	RELEASE_SAFE(m_pSymbolSurface);

	CUIRes::HideWindow();

	if(m_pParent)
	{
		m_pParent->m_pChild = NULL;
		m_pParent->HideAllWindow();
	}
}

BOOL CDialog_Crest::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_RETURN)
	{
		HideWindow();
		if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 1);
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
