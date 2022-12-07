// Dialog_GuildMenu3.cpp: implementation of the CDialog_GuildMenu3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_GuildMenu3.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "ChildView.h"
#include "Dialog_Message.h"
#include "Dialog_GuildJoin.h"
#include "Dialog_Guild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CPoint g_vEventPos;
extern CDialog_Message Dialog_Message;
extern CDialog_GuildJoin Dialog_GuildJoin;
extern CDialog_Guild Dialog_Guild;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_GuildMenu3::CDialog_GuildMenu3()
{
}

CDialog_GuildMenu3::~CDialog_GuildMenu3()
{

}

BOOL CDialog_GuildMenu3::OnButtonAction(int nSpec, int nIndex) 
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
			SendGuildManList();
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
		case 5:			// Repair House
			HideWindow();
			break;
		case 6:			// Repair Guild House
			HideWindow();
			break;
		case 7:			// Repair Town
			HideWindow();
			break;
		}
	}
	return TRUE;
}

BOOL CDialog_GuildMenu3::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 7);

	return TRUE;
}

int CDialog_GuildMenu3::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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

void CDialog_GuildMenu3::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Guild-5.lib", 0, TRUE, FALSE, FALSE))
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

void CDialog_GuildMenu3::SendApplicationList()
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_GUILD_APPLICATION_LIST);

	CChildView::SocketMng.Send(offset, buf);

	HideWindow();
}

void CDialog_GuildMenu3::SendGuildManList()
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_GUILD_MAN_LIST);

	CChildView::SocketMng.Send(offset, buf);

	HideWindow();
}

void RcvGuildManList(PBYTE bypData)
{
	int offset = 0;
	BYTE rank;

	rank = CPacketParser::GetByte(bypData, offset);
	short count = CPacketParser::GetShort(bypData, offset);
	if(count == -1)		// 길드원이 아닌 경우
	{
		CString message;

		message.LoadString(IDS_NOT_GUILD_USER);
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(message);
	}
	else		// 길드 다이얼로그
	{
		Dialog_Guild.OpenGuild(bypData);
	}
}

void RcvApplicationList(PBYTE bypData)
{
	int offset = 0;

	short len = CPacketParser::GetShort(bypData, offset);

	if(len == 0)
	{
		CString message;

		message.LoadString(IDS_EMPTY_APPLICATION);
		Dialog_Message.ShowWindow(255, NULL);
		Dialog_Message.SetMessage(message);
	}
	else
	{
		Dialog_GuildJoin.ShowWindow(255, NULL);
		Dialog_GuildJoin.DialogOpen(bypData);
	}
}
