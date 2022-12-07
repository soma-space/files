// Dialog_TaxRate.cpp: implementation of the CDialog_TaxRate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_TaxRate.h"
#include "BBDX_Editor.h"
#include "ChildView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CUIRes *g_pShowDialog;
extern CChildView *g_pChildView;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_TaxRate::CDialog_TaxRate()
{
	m_bEnableFocus = TRUE;
}

CDialog_TaxRate::~CDialog_TaxRate()
{

}

BOOL CDialog_TaxRate::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		if(nIndex == 0)
		{
			CString t_str;
			BOOL bValid;
			m_pMainEditor->GetWindowText(t_str);
			if(t_str.IsEmpty()) return TRUE;
			bValid = TRUE;
			for(int i = 0; i < t_str.GetLength(); i++)
			{
				if(t_str[i] < '0' || t_str[i] > '9')
				{
					bValid = FALSE;
					break;
				}
			}
			if(bValid == FALSE) return TRUE;
			m_vTaxRate = atoi((LPCTSTR)t_str);
			if(m_vTaxRate < 0 || m_vTaxRate > 100) return TRUE;

			HideWindow();
			if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 1);
		}
		else if(nIndex == 1)
		{
			HideWindow();
			if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 2);
		}
	}

	return TRUE;
}

BOOL CDialog_TaxRate::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 1);

	m_pMainEditor->DCDrawOffsetedCurText(pDest, m_ptOffset);

	return TRUE;
}

BOOL CDialog_TaxRate::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if(m_pWnd == NULL) return FALSE;

	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pMainEditor = new BBDX_Editor(m_pWnd, this, FALSE, m_arrArea[0]->m_vRect);
		
	return TRUE;
}

void CDialog_TaxRate::ShowWindow(int Id, CUIRes *pParent)
{
	if(!IsLoading())
	{
		CPoint t_pos;

		SetWnd(g_pChildView);
		if(!Create(CChildView::DirectXMng.m_pDD, "taxrate.lib", 0, TRUE, FALSE, FALSE))
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
	m_pMainEditor->ClearTexts();
	CUIRes::ShowWindow();
}

BOOL CDialog_TaxRate::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_RETURN)
	{
		CString t_str;
		BOOL bValid;
		t_str = m_pMainEditor->GetCurEditorText();
		if(t_str.IsEmpty()) return TRUE;
		bValid = TRUE;
		for(int i = 0; i < t_str.GetLength(); i++)
		{
			if(t_str[i] < '0' || t_str[i] > '9')
			{
				bValid = FALSE;
				break;
			}
		}
		if(bValid == FALSE) return TRUE;
		m_vTaxRate = atoi((LPCTSTR)t_str);
		if(m_vTaxRate < 0 || m_vTaxRate > 100) return TRUE;
		
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

void CDialog_TaxRate::HideWindow()
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

void CDialog_TaxRate::HideAllWindow()
{
	CUIRes::HideWindow();

	if(m_pParent)
	{
		m_pParent->m_pChild = NULL;
		m_pParent->HideAllWindow();
	}
}

void CDialog_TaxRate::SetTaxRate(int tax_rate)
{
	CString t_str;

	t_str.Format("%d", tax_rate);
	m_pMainEditor->SetWindowText(t_str);
}