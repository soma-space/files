// Dialog_GuildMenu4.cpp: implementation of the CDialog_GuildMenu4 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_GuildMenu4.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "ChildView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CPoint g_vEventPos;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_GuildMenu4::CDialog_GuildMenu4()
{
}

CDialog_GuildMenu4::~CDialog_GuildMenu4()
{

}

BOOL CDialog_GuildMenu4::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// Guild
			HideWindow();
			break;
		case 1:			// Buy House
			HideWindow();
			break;
		case 2:			// Repair House
			HideWindow();
			break;
		case 3:			// View Map
			HideWindow();
			break;
		}
	}
	return TRUE;
}

BOOL CDialog_GuildMenu4::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 3);

	return TRUE;
}

int CDialog_GuildMenu4::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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
		}
		else
		{
			HideWindow();
		}
		return 2;
	}

	return 0;
}

void CDialog_GuildMenu4::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Guild-4.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
	}
	SetOffset(g_vEventPos);

	CUIRes::ShowWindow();
}
