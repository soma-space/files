// Dialog_Option.cpp: implementation of the CDialog_Option class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Option.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "ChildView.h"
#include "UIHorzScrollBar.h"
#include "Dialog_Quit.h"
#include "DManage/DirectSound.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CDialog_Quit		Dialog_Quit;
extern BOOL				g_bDisplayDate;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Option::CDialog_Option()
{
	m_vAreaFocus = -1;
	m_vGammaRate = -25;
	m_vPosMode = POS_NOTEFFECT;

	INIT_PTR(m_pSoundScrollBar);
	INIT_PTR(m_pMusicScrollBar);
	INIT_PTR(m_pGammaScrollBar);
}

CDialog_Option::~CDialog_Option()
{
	SAFE_DELETE(m_pSoundScrollBar);
	SAFE_DELETE(m_pMusicScrollBar);
	SAFE_DELETE(m_pGammaScrollBar);
}

BOOL CDialog_Option::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		if(nIndex == 0)
		{
			Dialog_Quit.ShowWindow(-1, NULL);
			HideWindow();
		}
	}
	else if(nSpec == BTN_CTRL_EX)
	{
		if(nIndex == 3)
		{
			g_bDisplayDate = TRUE;
			m_arrBtnEx[3]->m_vState = BUTTON_DISABLE;
			m_arrBtnEx[4]->m_vState = BUTTON_NORMAL;
		}
		else if(nIndex == 4)
		{
			g_bDisplayDate = FALSE;
			m_arrBtnEx[3]->m_vState = BUTTON_NORMAL;
			m_arrBtnEx[4]->m_vState = BUTTON_DISABLE;
		}
	}
	return TRUE;
}

BOOL CDialog_Option::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 0);
	if(g_bDisplayDate == TRUE) Draw4ButtonCtrl(pDest, 4, 4);
	else Draw4ButtonCtrl(pDest, 3, 3);

	DWORD dwTrans =	DDBLTFAST_WAIT;
	CPoint start_pos;

	switch(m_vAreaFocus)
	{
	case 0:
		start_pos = m_arrArea[3]->m_vRect.TopLeft();
		start_pos.Offset(m_ptOffset);
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[4]->m_vRect, dwTrans);
		break;
	case 1:
		start_pos = m_arrArea[5]->m_vRect.TopLeft();
		start_pos.Offset(m_ptOffset);
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[6]->m_vRect, dwTrans);
		break;
	case 2:
		start_pos = m_arrArea[7]->m_vRect.TopLeft();
		start_pos.Offset(m_ptOffset);
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[8]->m_vRect, dwTrans);
		break;
	}

	m_pSoundScrollBar->Draw(pDest);
	m_pMusicScrollBar->Draw(pDest);
	if(m_pGammaScrollBar) m_pGammaScrollBar->Draw(pDest);

	return TRUE;
}

BOOL CDialog_Option::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pSoundScrollBar->OnLButtonDown(pMouseInfo))
	{
		SetFocus();
		SetCapture();
		if(m_pSoundScrollBar->GetStartLine() - 3000 != m_vSoundVolume)
		{
			m_vSoundVolume = m_pSoundScrollBar->GetStartLine() - 3000;
			if(m_vSoundVolume == -3000) CChildView::DirectXMng.m_pDirectSound->SetSoundVolume(DSBVOLUME_MIN);
			else CChildView::DirectXMng.m_pDirectSound->SetSoundVolume(m_vSoundVolume);
		}
		return FALSE;
	}
	if(m_pMusicScrollBar->OnLButtonDown(pMouseInfo))
	{
		SetFocus();
		SetCapture();
		if(m_pMusicScrollBar->GetStartLine() - 3000 != m_vMusicVolume)
		{
			m_vMusicVolume = m_pMusicScrollBar->GetStartLine() - 3000;
			if(m_vMusicVolume == -3000) CChildView::DirectXMng.m_pDirectSound->SetMusicVolume(DSBVOLUME_MIN);
			else CChildView::DirectXMng.m_pDirectSound->SetMusicVolume(m_vMusicVolume);
		}
		return FALSE;
	}
	if(m_pGammaScrollBar && m_pGammaScrollBar->OnLButtonDown(pMouseInfo))
	{
		SetFocus();
		SetCapture();
		if(m_pGammaScrollBar->GetStartLine() - 75 != m_vGammaRate)//this
		{
			m_vGammaRate = m_pGammaScrollBar->GetStartLine() - 75;
			CChildView::DirectXMng.SetGammaRate( m_vGammaRate );
		}
		return FALSE;
	}

	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_Option::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pSoundScrollBar->OnMouseMove(pMouseInfo) )
	{
		if(m_pSoundScrollBar->GetStartLine() - 3000 != m_vSoundVolume)
		{
			m_vSoundVolume = m_pSoundScrollBar->GetStartLine() - 3000;
			if(m_vSoundVolume == -3000) CChildView::DirectXMng.m_pDirectSound->SetSoundVolume(DSBVOLUME_MIN);
			else CChildView::DirectXMng.m_pDirectSound->SetSoundVolume(m_vSoundVolume);
		}
		return FALSE;
	}
	if(m_pMusicScrollBar->OnMouseMove(pMouseInfo) )
	{
		if(m_pMusicScrollBar->GetStartLine() - 3000 != m_vMusicVolume)
		{
			m_vMusicVolume = m_pMusicScrollBar->GetStartLine() - 3000;
			if(m_vMusicVolume == -3000) CChildView::DirectXMng.m_pDirectSound->SetMusicVolume(DSBVOLUME_MIN);
			else CChildView::DirectXMng.m_pDirectSound->SetMusicVolume(m_vMusicVolume);
		}
		return FALSE;
	}
//	if(m_pGammaScrollBar && m_pGammaScrollBar->OnMouseMove(pMouseInfo) )
//		return FALSE;
	if(m_pGammaScrollBar && m_pGammaScrollBar->OnMouseMove(pMouseInfo) )
	{
		if(m_pGammaScrollBar->GetStartLine() - 75 != m_vGammaRate)
		{
			m_vGammaRate = m_pGammaScrollBar->GetStartLine() - 75;
			CChildView::DirectXMng.SetGammaRate( m_vGammaRate );
		}
		return FALSE;
	}//this

	BOOL retVal = CUIRes::OnMouseMove(pMouseInfo);

	if(retVal == FALSE)
	{
		CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;
		if(m_arrArea[3]->m_vRect.PtInRect(m_pos))
		{
			m_vAreaFocus = 0;
		}
		else if(m_arrArea[5]->m_vRect.PtInRect(m_pos))
		{
			m_vAreaFocus = 1;
		}
		else if(m_arrArea[7]->m_vRect.PtInRect(m_pos))
		{
			m_vAreaFocus = 2;
		}
		else m_vAreaFocus = -1;
	}
	else m_vAreaFocus = -1;

	return retVal;
}

BOOL CDialog_Option::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();

	if(m_pSoundScrollBar->OnLButtonUp(pMouseInfo) )
	{
		if(m_pSoundScrollBar->GetStartLine() - 3000 != m_vSoundVolume)
		{
			m_vSoundVolume = m_pSoundScrollBar->GetStartLine() - 3000;
			if(m_vSoundVolume == -3000) CChildView::DirectXMng.m_pDirectSound->SetSoundVolume(DSBVOLUME_MIN);
			else CChildView::DirectXMng.m_pDirectSound->SetSoundVolume(m_vSoundVolume);
		}
		AfxGetApp()->WriteProfileInt("Setting", "Sound Volume", m_vSoundVolume);
		return FALSE;
	}
	if(m_pMusicScrollBar->OnLButtonUp(pMouseInfo) )
	{
		if(m_pMusicScrollBar->GetStartLine() - 3000 != m_vMusicVolume)
		{
			m_vMusicVolume = m_pMusicScrollBar->GetStartLine() - 3000;
			if(m_vMusicVolume == -3000) CChildView::DirectXMng.m_pDirectSound->SetMusicVolume(DSBVOLUME_MIN);
			else CChildView::DirectXMng.m_pDirectSound->SetMusicVolume(m_vMusicVolume);
		}
		AfxGetApp()->WriteProfileInt("Setting", "Music Volume", m_vMusicVolume);
		return FALSE;
	}
//	if(m_pGammaScrollBar && m_pGammaScrollBar->OnLButtonUp(pMouseInfo) )
//		return FALSE;
	if(m_pGammaScrollBar && m_pGammaScrollBar->OnLButtonUp(pMouseInfo) )//this
	{
		if(m_pGammaScrollBar->GetStartLine() - 75 != m_vGammaRate)
		{
			m_vGammaRate = m_pGammaScrollBar->GetStartLine() - 75;
			CChildView::DirectXMng.SetGammaRate( m_vGammaRate );
		}
		AfxGetApp()->WriteProfileInt("Setting", "Gamma Control", m_vGammaRate);
		return FALSE;
	}

	return CUIRes::OnLButtonUp(pMouseInfo);
}

int CDialog_Option::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	if(m_bShow && m_pChild == NULL)
	{
		if(GetCaptureStatus() || PtInUIRes(pMouseInfo->GetPos()))
		{
			if(pMouseInfo->IsLButtonDown()) {
				OnLButtonDown(pMouseInfo);
			}
			if(pMouseInfo->IsMouseMove()) {
				OnMouseMove(pMouseInfo);
			}
			if(pMouseInfo->IsLButtonUp()) {
				OnLButtonUp(pMouseInfo);
			}
			return 2;
		}
		else
		{
			if(pMouseInfo->IsLButtonDown())
			{
				HideWindow();
			}
			return 0;
		}
	}

	return 0;
}

void CDialog_Option::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Option.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
		SetOffset(CPoint(438,552-m_mainRect.Height()));
	}

	m_vMusicVolume = CChildView::DirectXMng.m_pDirectSound->GetMusicVolume();
	m_vSoundVolume = CChildView::DirectXMng.m_pDirectSound->GetSoundVolume();
	if( m_pGammaScrollBar )
		m_vGammaRate   = CChildView::DirectXMng.GetGammaRate();//this

	if(m_vMusicVolume <= 0) m_pMusicScrollBar->SetStartLine(m_vMusicVolume+3000);
	if(m_vSoundVolume <= 0) m_pSoundScrollBar->SetStartLine(m_vSoundVolume+3000);
	if(m_pGammaScrollBar && m_vGammaRate <= 0) m_pGammaScrollBar->SetStartLine(m_vGammaRate+75);
//this
	if(g_bDisplayDate == TRUE)
	{
		m_arrBtnEx[3]->m_vState = BUTTON_DISABLE;
		m_arrBtnEx[4]->m_vState = BUTTON_NORMAL;
	}
	else
	{
		m_arrBtnEx[3]->m_vState = BUTTON_NORMAL;
		m_arrBtnEx[4]->m_vState = BUTTON_DISABLE;
	}
	CUIRes::ShowWindow();
}

BOOL CDialog_Option::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}
	m_pSoundScrollBar = new CUIHorzScrollBar();
	m_pSoundScrollBar->Create((CUIRes*)this, m_arrArea[0]->m_vRect, 0);
	m_pSoundScrollBar->SetMaxLineCount(3000);
	m_pSoundScrollBar->SetViewLineCount(1);
	m_pMusicScrollBar = new CUIHorzScrollBar();
	m_pMusicScrollBar->Create((CUIRes*)this, m_arrArea[1]->m_vRect, 0);
	m_pMusicScrollBar->SetMaxLineCount(3000);
	m_pMusicScrollBar->SetViewLineCount(1);
	if(CChildView::DirectXMng.m_bFullScreen == TRUE)
	{
		m_pGammaScrollBar = new CUIHorzScrollBar();
		m_pGammaScrollBar->Create((CUIRes*)this, m_arrArea[2]->m_vRect, 0);
//		m_pGammaScrollBar->SetMaxLineCount(100);
		m_pGammaScrollBar->SetMaxLineCount(75);//this
		m_pGammaScrollBar->SetViewLineCount(1);
	}

	return TRUE;
}