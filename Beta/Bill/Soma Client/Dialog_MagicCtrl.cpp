// Dialog_MagicCtrl.cpp: implementation of the CDialog_MagicCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_MagicCtrl.h"
#include "ChildView.h"
#include "DManage/BBDX_MouseInformation.h"
#include "DManage/BB_Direct.h"
#include "Dialog_SpecialMagic.h"
#include "Dialog_WhiteMagic.h"
#include "Dialog_BlueMagic.h"
#include "Dialog_BlackMagic.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_MagicCtrl::CDialog_MagicCtrl()
{
	INIT_PTR(m_pDialog_Special);
	INIT_PTR(m_pDialog_Blue);
	INIT_PTR(m_pDialog_White);
	INIT_PTR(m_pDialog_Black);

	INIT_PTR(m_pDD);
	INIT_PTR(m_pWnd);

	m_nDrawMode = DRAW_SPECIALMAGIC;

	m_bInit = FALSE;
}

CDialog_MagicCtrl::~CDialog_MagicCtrl()
{
	SAFE_DELETE(m_pDialog_Special);
	SAFE_DELETE(m_pDialog_Blue);
	SAFE_DELETE(m_pDialog_White);
	SAFE_DELETE(m_pDialog_Black);
}

void CDialog_MagicCtrl::Draw(CDDSurface *pDest)
{
	if(m_bShow && m_pDrawDialog) m_pDrawDialog->Draw(pDest);
}

BOOL CDialog_MagicCtrl::InitData(CDirectDraw *pDD, CChildView *pWnd)
{
	m_pDD = pDD;
	m_pWnd = pWnd;

	m_pDialog_Special = new CDialog_SpecialMagic((CWnd *)m_pWnd);
	m_pDialog_Special->m_pDialogCtrl = this;
	m_pDialog_Special->SetOffset(CPoint(0,0));

	m_pDialog_Blue = new CDialog_BlueMagic((CWnd *)m_pWnd);
	m_pDialog_Blue->m_pDialogCtrl = this;
	m_pDialog_Blue->SetOffset(CPoint(0,0));

	m_pDialog_White = new CDialog_WhiteMagic((CWnd *)m_pWnd);
	m_pDialog_White->m_pDialogCtrl = this;
	m_pDialog_White->SetOffset(CPoint(0,0));

	m_pDialog_Black = new CDialog_BlackMagic((CWnd *)m_pWnd);
	m_pDialog_Black->m_pDialogCtrl = this;
	m_pDialog_Black->SetOffset(CPoint(0,0));

	m_bInit = TRUE;
	return TRUE;
}

int CDialog_MagicCtrl::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	CPoint pos = pMouseInfo->GetPos();

	if (m_bShow && m_pDrawDialog && m_pDrawDialog->m_pChild == NULL) {
		if(m_pDrawDialog->GetCaptureStatus() || m_pDrawDialog->PtInUIRes(pMouseInfo->GetPos()))
		{
			if (pMouseInfo->IsLButtonDown()) {
				m_pDrawDialog->OnLButtonDown(pMouseInfo);
			}
			if(pMouseInfo->IsMouseMove()) {
				m_pDrawDialog->OnMouseMove(pMouseInfo);
			}
			// ButtonUp이 일어나면서 ButtonAction()이 호출되기 때문에 
			// 여기서 m_pDrawDialog가 NULL값을 가질 수도 있다. 
			if(pMouseInfo->IsLButtonUp()) {
				m_pDrawDialog->OnLButtonUp(pMouseInfo);
			}
			return 2;
		}
		return m_pDrawDialog->IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

void CDialog_MagicCtrl::DrawMode(int nMode)
{
	m_nDrawMode = nMode;
	switch(nMode){
	case DRAW_SPECIALMAGIC:
		m_pDialog_Special->ShowWindow();
		m_pDrawDialog = m_pDialog_Special;
		break;
	case DRAW_BLUEMAGIC:
		m_pDialog_Blue->ShowWindow();
		m_pDrawDialog = m_pDialog_Blue;
		break;
	case DRAW_WHITEMAGIC:
		m_pDialog_White->ShowWindow();
		m_pDrawDialog = m_pDialog_White;
		break;
	case DRAW_BLACKMAGIC:
		m_pDialog_Black->ShowWindow();
		m_pDrawDialog = m_pDialog_Black;
		break;
	default:
		m_pDrawDialog = NULL;
		break;
	}

}

void CDialog_MagicCtrl::ShowWindow()
{
	m_bShow = TRUE;
	switch(m_nDrawMode){
	case DRAW_SPECIALMAGIC:
		m_pDialog_Special->ShowWindow();
		m_pDrawDialog = m_pDialog_Special;
		break;
	case DRAW_BLUEMAGIC:
		m_pDialog_Blue->ShowWindow();
		m_pDrawDialog = m_pDialog_Blue;
		break;
	case DRAW_WHITEMAGIC:
		m_pDialog_White->ShowWindow();
		m_pDrawDialog = m_pDialog_White;
		break;
	case DRAW_BLACKMAGIC:
		m_pDialog_Black->ShowWindow();
		m_pDrawDialog = m_pDialog_Black;
		break;
	}
}

void CDialog_MagicCtrl::HideWindow()
{
	m_bShow = FALSE;
	m_pDrawDialog->HideWindow();
	m_pDrawDialog = NULL;
}