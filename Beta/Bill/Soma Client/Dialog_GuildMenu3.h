// Dialog_GuildMenu3.h: interface for the CDialog_GuildMenu3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_GUILDMENU3_H__8850F5A3_9462_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_GUILDMENU3_H__8850F5A3_9462_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_MouseInformation;

class CDialog_GuildMenu3 : public CUIRes  // 길드장일 경우, 중립 길드
{
public:
	CDialog_GuildMenu3();
	virtual ~CDialog_GuildMenu3();

	void SendApplicationList();
	void SendGuildManList();

public:
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void ShowWindow();
};

#endif // !defined(AFX_DIALOG_GUILDMENU3_H__8850F5A3_9462_11D4_98B2_00C02615866B__INCLUDED_)
