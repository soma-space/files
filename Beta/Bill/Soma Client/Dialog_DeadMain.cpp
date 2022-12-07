// Dialog_DeadMain.cpp: implementation of the CDialog_DeadMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_DeadMain.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "PacketParser.h"
#include "ChildView.h"

#include "Dialog_Main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CDialog_Main Dialog_Main;
extern CChildView *g_pChildView;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_DeadMain::CDialog_DeadMain()
{
	m_vCountDown = 9;
	m_pWnd = NULL;
	m_bEnableFocus = FALSE;
	m_vPosMode = POS_NOTEFFECT;
}

CDialog_DeadMain::~CDialog_DeadMain()
{

}

BOOL CDialog_DeadMain::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// 바로 살아나기
			m_pWnd->KillTimer(10);
			m_vCountDown = 0;
			SendAliveFromHere();
			break;
		case 1:			// 마을에서 살아나기
			m_pWnd->KillTimer(10);
			m_vCountDown = 0;
			SendAliveFromTown();
			break;
		}
	}
	return TRUE;
}

BOOL CDialog_DeadMain::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	DWORD dwTrans;
	CRect t_rect;

	Draw3ButtonCtrl(pDest, 0, 1);

	// Draw CountDown Number
	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	if(m_vCountDown > 0 && m_vCountDown <= 9)
	{
		pDest->BltFast(m_ptOffset+m_arrArea[9]->m_vRect.TopLeft(), m_pDDSurface, &m_arrArea[9-m_vCountDown]->m_vRect, dwTrans);
	}

	return TRUE;
}

void CDialog_DeadMain::ShowWindow()
{
	if(!IsLoading())
	{
		SetOffset(CPoint(0, 495));
		SetWnd(g_pChildView);
		if(!Create(CChildView::DirectXMng.m_pDD, "DeadUI.lib", 0, FALSE, FALSE, FALSE))
		{
			return;
		}
	}

	CUIRes::ShowWindow();
}

BOOL CDialog_DeadMain::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_DeadMain::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_DeadMain::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	return CUIRes::OnMouseMove(pMouseInfo);
}

int CDialog_DeadMain::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

void CDialog_DeadMain::StartCountDown()
{
	if(m_pWnd)
	{
		m_vCountDown = 9;
		m_pWnd->SetTimer(10, 3000, NULL);
	}
}

void CDialog_DeadMain::CountDown()
{
	m_vCountDown--;
	if(m_vCountDown == 0)
	{
		m_pWnd->KillTimer(10);
		SendAliveFromTown();
		TRACE0("카운트 엔드~!\n");
	}
}

void CDialog_DeadMain::SendAliveFromHere()
{
	BYTE buf[10];
	int offset = 0;
	BYTE type;
	
	type = 1;
	CPacketParser::SetByte(buf, offset, PKT_LIFE); //최종점 요청
	CPacketParser::SetByte(buf, offset, (BYTE)type);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_DeadMain::SendAliveFromTown()
{
	BYTE buf[10];
	int offset = 0;
	BYTE type;

	type = 2;
	CPacketParser::SetByte(buf, offset, PKT_LIFE); //최종점 요청
	CPacketParser::SetByte(buf, offset, (BYTE)type);
	CChildView::SocketMng.Send(offset, buf);
}
