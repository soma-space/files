// Dialog_GuildMenu5.cpp: implementation of the CDialog_GuildMenu5 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_GuildMenu5.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "ChildView.h"
#include "Dialog_Message.h"
#include "Dialog_GuildJoin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CPoint g_vEventPos;
extern CDialog_Message Dialog_Message;
extern CDialog_GuildJoin Dialog_GuildJoin;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_GuildMenu5::CDialog_GuildMenu5()
{
}

CDialog_GuildMenu5::~CDialog_GuildMenu5()
{

}

BOOL CDialog_GuildMenu5::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// Application
			SendApplicationList();
			break;
		case 1:			// Guild
			HideWindow();
			break;
		case 2:			// Buy House
			HideWindow();
			break;
		case 3:			// Buy Guild House
			HideWindow();
			break;
		case 4:			// Buy Town
			HideWindow();
			break;
		case 5:			// Hire Guard
			HideWindow();
			break;
		case 6:			// Repair House
			HideWindow();
			break;
		case 7:			// Repair Guild House
			HideWindow();
			break;
		case 8:			// Repair Town
			HideWindow();
			break;
		case 9:			// View Map
			HideWindow();
			break;
		}
	}
	return TRUE;
}

BOOL CDialog_GuildMenu5::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 9);

	return TRUE;
}

int CDialog_GuildMenu5::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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

void CDialog_GuildMenu5::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Guild-1.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
	}
	CPoint t_pos;

	t_pos = g_vEventPos;
	t_pos.Offset(-m_mainRect.Width()/2, -m_mainRect.Height()/2);
	SetOffset(t_pos);

	CUIRes::ShowWindow();
}

void CDialog_GuildMenu5::SendApplicationList()
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_GUILD_REQ_USER_INFO);

	CChildView::SocketMng.Send(offset, buf);

	HideWindow();
}
