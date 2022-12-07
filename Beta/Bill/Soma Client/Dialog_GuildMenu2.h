// Dialog_GuildMenu2.h: interface for the CDialog_GuildMenu2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_GUILDMENU2_H__8850F5A2_9462_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_GUILDMENU2_H__8850F5A2_9462_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_MouseInformation;

class CDialog_GuildMenu2 : public CUIRes  // 길드 소속인 PC, 중립 길드
{
public:
	CDialog_GuildMenu2();
	virtual ~CDialog_GuildMenu2();

	void SendGuildManList();
public:
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void ShowWindow();
};

#endif // !defined(AFX_DIALOG_GUILDMENU2_H__8850F5A2_9462_11D4_98B2_00C02615866B__INCLUDED_)
