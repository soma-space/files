// Dialog_GuildMenu1.cpp: implementation of the CDialog_GuildMenu1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_GuildMenu1.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "ChildView.h"
#include "Dialog_Message2.h"
#include "Dialog_GuildName.h"
#include "Dialog_GuildApply.h"
#include "Dialog_Message.h"
#include "User.h"
#include "GuildDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CPoint g_vEventPos;
extern CDialog_Message2 Dialog_Message2;
extern CDialog_GuildName Dialog_GuildName;
extern CDialog_GuildApply Dialog_GuildApply;
extern CDialog_Message Dialog_Message;
extern CChildView *g_pChildView;
extern CUIRes *g_pShowDialog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_GuildMenu1::CDialog_GuildMenu1()
{
}

CDialog_GuildMenu1::~CDialog_GuildMenu1()
{

}

BOOL CDialog_GuildMenu1::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// Make Guild
			{
				CString message, t_str;

				if(m_bApplied == TRUE)
				{
					message.LoadString(IDS_ALREADY_JOIN_REQ);
					Dialog_Message.ShowWindow(255, this);
					Dialog_Message.SetMessage(message);
					return TRUE;
				}
				t_str.LoadString(IDS_ASK_MAKE_GUILD);
				message.Format((LPCTSTR)t_str, m_vGuildMoney);
				Dialog_Message2.ShowWindow(0, this);
				Dialog_Message2.SetMessage(message);
			}
			break;
		case 1:			// Buy House
			HideWindow();
			break;
		case 2:			// Repair House
			HideWindow();
			break;
		}
	}
	else if(nSpec == BTN_CTRL_EX)
	{
		switch(nIndex)
		{
		case 0:			// Apply to Guild
			SendGuildList();
			break;
		case 1:			// Cancel Apply
			{
				CString message, t_str;

				t_str.LoadString(IDS_ASK_CANCEL_APPLY);
				message.Format((LPCTSTR)t_str, m_vGuildName);
				Dialog_Message2.ShowWindow(2, this);
				Dialog_Message2.SetMessage(message);
			}
			break;
		}
	}
	return TRUE;
}

BOOL CDialog_GuildMenu1::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 2);
	Draw4ButtonCtrl(pDest, 0, 1);

	return TRUE;
}

int CDialog_GuildMenu1::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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

void CDialog_GuildMenu1::ShowWindow(BOOL bGuild, CString guildName, DWORD guildMoney, short limitLevel)
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Guild-3.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
	}

	CPoint t_pos;

	t_pos = g_vEventPos;
	t_pos.Offset(-m_mainRect.Width()/2, -m_mainRect.Height()/2);
	SetOffset(t_pos);

	m_bApplied = bGuild;
	if(bGuild == FALSE)
	{
		m_arrBtnEx[0]->m_vState = BUTTON_NORMAL;
		m_arrBtnEx[1]->m_vState = BUTTON_DISABLE;
	}
	else
	{
		m_vGuildName = guildName;
		m_arrBtnEx[0]->m_vState = BUTTON_DISABLE;
		m_arrBtnEx[1]->m_vState = BUTTON_NORMAL;
	}
	m_vGuildMoney = guildMoney;
	m_vLimitLevel = limitLevel;

	CUIRes::ShowWindow();
}

void CDialog_GuildMenu1::CallBack_MessageBox(int nId, int nValue)
{
	if(nId == 0)
	{
		if(nValue == 1)	// 길드 설립 비용 OK!
		{
			CUser *pMe = CChildView::UserManager.GetMe();
			if(pMe->m_Money < m_vGuildMoney)
			{
				CString message;

				message.LoadString(IDS_MAKEGUILD_SMALL_MONEY);
				Dialog_Message.ShowWindow(255, this);
				Dialog_Message.SetMessage(message);
				return;
			}
			if(pMe->m_Level < m_vLimitLevel)
			{
				CString message;

				message.Format(IDS_MAKEGUILD_SMALL_LEVEL, m_vLimitLevel);
				Dialog_Message.ShowWindow(255, this);
				Dialog_Message.SetMessage(message);
				return;
			}
			Dialog_GuildName.ShowWindow(1, this);
		}
	}
	else if(nId == 2)
	{
		if(nValue == 1) // 길드 가입 취소
		{
			SendCancelApplyGuild();
		}
	}
}

void CDialog_GuildMenu1::SendGuildList()
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_GUILD_LIST);

	CChildView::SocketMng.Send(offset, buf);

	HideWindow();
}

void CDialog_GuildMenu1::SendCancelApplyGuild()
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_GUILD_REMOVE_REQ);

	CChildView::SocketMng.Send(offset, buf);

	HideWindow();
}

void CDialog_GuildMenu1::HideWindow()
{
	g_pShowDialog = NULL;

	HideAllWindow();
}

void CDialog_GuildMenu1::HideAllWindow()
{
	CUIRes::HideWindow();
}

void RcvGuildList(PBYTE bypData)
{
	int offset = 0;

	BYTE len = CPacketParser::GetByte(bypData, offset);
	if(len == 0)
	{
		CString message;

		message.LoadString(IDS_EMPTY_GUILD);
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(message);
	}
	else
	{
		Dialog_GuildApply.ShowWindow();
		Dialog_GuildApply.DialogOpen(bypData);
	}
}

void RcvCancelApplyGuild(PBYTE bypData)
{
	int offset = 0;

	BYTE bResult = CPacketParser::GetByte(bypData, offset);
	if(bResult == 1) // 성공
	{
		CString message;

		message.LoadString(IDS_SUC_CANCEL_APPLY);
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(message);
	}
	else // 실패
	{
		CString message;
		BYTE sub_result;

		sub_result = CPacketParser::GetByte(bypData, offset);
		switch(sub_result)
		{
		case GUILD_ALREADY_JOIN:
			message.LoadString(IDS_ALREADY_JOIN);
			break;
		case GUILD_ABSENT_JOIN_REQ:
			message.LoadString(IDS_ABSENT_JOIN_REQ);
			break;
		case GUILD_ABSENT_GUILD_NAME:
			message.LoadString(IDS_ABSENT_GUILD_NAME);
			break;
		case GUILD_ABSENT_REQ_USER:
		default:
			message.LoadString(IDS_ABSENT_REQ_USER);
			break;
		}
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(message);
	}
}
