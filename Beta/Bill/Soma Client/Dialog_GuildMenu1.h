// Dialog_GuildMenu1.h: interface for the CDialog_GuildMenu1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_GUILDMENU1_H__8850F5A1_9462_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_GUILDMENU1_H__8850F5A1_9462_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_MouseInformation;

class CDialog_GuildMenu1 : public CUIRes  // 길드 소속이 아닌 PC, 중립 길드
{
public:
	CDialog_GuildMenu1();
	virtual ~CDialog_GuildMenu1();

	void SendGuildList();
	void SendCancelApplyGuild();
public:
	void CallBack_MessageBox(int nId, int nValue);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void ShowWindow(BOOL bGuild, CString guildName, DWORD guildMoney, short limitLevel);
	void HideWindow();
	void HideAllWindow();

	DWORD	m_vGuildMoney;
	short	m_vLimitLevel;
	CString	m_vGuildName;
	BOOL	m_bApplied;
};

#endif // !defined(AFX_DIALOG_GUILDMENU1_H__8850F5A1_9462_11D4_98B2_00C02615866B__INCLUDED_)
