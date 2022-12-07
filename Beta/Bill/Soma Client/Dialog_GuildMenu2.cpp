// Dialog_GuildMenu2.cpp: implementation of the CDialog_GuildMenu2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_GuildMenu2.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "ChildView.h"
#include "Dialog_Message.h"
#include "Dialog_Guild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CPoint g_vEventPos;
extern CDialog_Message Dialog_Message;
extern CDialog_Guild Dialog_Guild;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_GuildMenu2::CDialog_GuildMenu2()
{
}

CDialog_GuildMenu2::~CDialog_GuildMenu2()
{

}

BOOL CDialog_GuildMenu2::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// Guild
			SendGuildManList();
			break;
		case 1:			// Buy House
			HideWindow();
			break;
		case 2:			// Repair House
			HideWindow();
			break;
		}
	}
	return TRUE;
}

BOOL CDialog_GuildMenu2::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 2);

	return TRUE;
}

int CDialog_GuildMenu2::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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

void CDialog_GuildMenu2::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Guild-2.lib", 0, TRUE, FALSE, FALSE))
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

void CDialog_GuildMenu2::SendGuildManList()
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_GUILD_MAN_LIST);

	CChildView::SocketMng.Send(offset, buf);

	HideWindow();
}
