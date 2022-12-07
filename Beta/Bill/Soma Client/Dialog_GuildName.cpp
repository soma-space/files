// Dialog_GuildName.cpp: implementation of the CDialog_GuildName class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_GuildName.h"
#include "BBDX_Editor.h"
#include "ChildView.h"
#include "Dialog_Message.h"
#include "GuildDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CUIRes *g_pShowDialog;
extern CChildView *g_pChildView;
extern CDialog_Message Dialog_Message;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_GuildName::CDialog_GuildName()
{
	m_bProcessing = FALSE;
	m_bEnableFocus = TRUE;
}

CDialog_GuildName::~CDialog_GuildName()
{
	if(m_pDescEditor)
	{
		delete m_pDescEditor;
	}
}

BOOL CDialog_GuildName::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		if(nIndex == 0)
		{
			m_pMainEditor->GetWindowText(m_vName);
			m_pDescEditor->GetWindowText(m_vDesc);
			SendMakeGuild();
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

BOOL CDialog_GuildName::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 1);

	m_pMainEditor->DCDrawOffsetedCurText(pDest, m_ptOffset);
	m_pDescEditor->DCDrawOffsetedCurText(pDest, m_ptOffset);

	return TRUE;
}

BOOL CDialog_GuildName::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if(m_pWnd == NULL) return FALSE;

	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pMainEditor = new BBDX_Editor(m_pWnd, this, FALSE, m_arrArea[0]->m_vRect);
	m_pMainEditor->m_bChat = FALSE;
	m_pMainEditor->LimitText(12);
	m_pDescEditor = new BBDX_Editor(m_pWnd, this, TRUE, m_arrArea[1]->m_vRect);
	m_pDescEditor->LimitText(120);
		
	return TRUE;
}

void CDialog_GuildName::ShowWindow(int Id, CUIRes *pParent)
{
	if(!IsLoading())
	{
		CPoint t_pos;

		SetWnd(g_pChildView);
		if(!Create(CChildView::DirectXMng.m_pDD, "Guild-N.lib", 0, TRUE, FALSE, FALSE))
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
	m_pDescEditor->ClearTexts();
	CUIRes::ShowWindow();
}

BOOL CDialog_GuildName::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_RETURN)
	{
		if(m_pMainEditor->GetFocusStatus()) m_pDescEditor->SetFocus();
/*		m_vName = m_pMainEditor->GetCurEditorText();
		m_pDescEditor->GetWindowText(m_vDesc);
		SendMakeGuild();
		if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 1);*/
		return TRUE;
	}
	else if(nChar == VK_ESCAPE)
	{
		HideWindow();
		if(m_pParent) m_pParent->CallBack_MessageBox(m_vId, 2);
		return TRUE;
	}
	else if(nChar == VK_TAB)
	{
		if(m_pMainEditor->GetFocusStatus()) m_pDescEditor->SetFocus();
		else m_pMainEditor->SetFocus();

		return TRUE;
	}
	return FALSE;
}

void CDialog_GuildName::HideWindow()
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

void CDialog_GuildName::HideAllWindow()
{
	CUIRes::HideWindow();

	if(m_pParent)
	{
		m_pParent->m_pChild = NULL;
		m_pParent->HideAllWindow();
	}
}

void CDialog_GuildName::SetName(CString name, CString desc)
{
	m_pMainEditor->SetWindowText(name);
	m_pDescEditor->SetWindowText(desc);
}

void CDialog_GuildName::SendMakeGuild()
{
	BYTE *buf;
	int offset = 0;
	BYTE len;

	if(IsValidName(m_vName) == FALSE)
	{
		CString message;

		message.LoadString(IDS_MAKEGUILD_INVALID_GUILD_NAME);
		Dialog_Message.ShowWindow(255, this);
		Dialog_Message.SetMessage(message);
		return;
	}

	m_bProcessing = TRUE;

	buf = new BYTE[10+m_vName.GetLength()+m_vDesc.GetLength()];

	CPacketParser::SetByte(buf, offset, PKT_GUILD_NEW); //최종점 요청
	len = m_vName.GetLength();
	CPacketParser::SetByte(buf, offset, len);								// 길드 이름 길이
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)m_vName, len);	// 길드 이름
	len = m_vDesc.GetLength();
	CPacketParser::SetByte(buf, offset, len);								// 길드 설명/조건 길이
	if(len > 0) CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)m_vDesc, len);	// 길드 설명/조건

	CChildView::SocketMng.Send(offset, buf);

	delete[] buf;
}

void CDialog_GuildName::RcvMakeGuild(PBYTE bypData)
{
	int offset = 0;
	BYTE result, sub_result;
	CString message, t_str;

	result = CPacketParser::GetByte(bypData, offset);
	if(result == 1)			// 성공
	{
		message.LoadString(IDS_SUC_MAKE_GUILD);
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(message);
	}
	else if(result == 2)	// 실패
	{
		sub_result = CPacketParser::GetByte(bypData, offset);
		switch(sub_result)
		{
		case GUILD_ALREADY_JOIN:
			message.LoadString(IDS_ALREADY_JOIN);
			break;
		case GUILD_SMALL_MONEY:
			message.LoadString(IDS_MAKEGUILD_SMALL_MONEY);
			break;
		case GUILD_SMALL_LEVEL:
			message.LoadString(IDS_MAKEGUILD_SMALL_LEVEL);
			break;
		case GUILD_INVALID_GUILD_NAME:
			message.LoadString(IDS_MAKEGUILD_INVALID_GUILD_NAME);
			break;
		case GUILD_INVALID_GUILD_INFO:
			message.LoadString(IDS_MAKEGUILD_INVALID_GUILD_INFO);
			break;
		case GUILD_SAME_GUILD_NAME:
			message.LoadString(IDS_MAKEGUILD_SAME_GUILD_NAME);
			break;
		case GUILD_FULL:
			message.LoadString(IDS_MAKEGUILD_GUILD_FULL);
			break;
		case GUILD_SYSTEM_ERROR:
		default:
			message.LoadString(IDS_MAKEGUILD_SYSTEM_ERROR);
			break;
		}
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(message);
	}

	m_bProcessing = FALSE;
}

BOOL CDialog_GuildName::IsValidName(CString t_name)
{
	PBYTE t_str;
	int i, t_len;
	BOOL valid = TRUE;
	WORD word;

	t_str = (PBYTE)(LPCTSTR)t_name;
	t_len = t_name.GetLength();

	for(i = 0; i < t_len; i++)
	{
		if(!isalpha((int)t_str[i]) && !isdigit((int)t_str[i]) && t_str[i] != 0x5f)
		{
			if(t_str[i] & 0x80)                  // 최상위 비트가 1이면 한글로 판단
			{
				word = (t_str[i] << 8) | t_str[i+1];
				if((word >= 0xb0a0) && (word <= 0xc8ff))
				{
					if((t_str[i+1] == 0xa0) || (t_str[i+1] == 0xff))
					{
						valid = FALSE;
						break;
					}
				}
				else
				{
					valid = FALSE;
					break;
				}
				i++;
			}
			else
			{
				valid = FALSE;
				break;
			}
		}
	}

	return valid;
}