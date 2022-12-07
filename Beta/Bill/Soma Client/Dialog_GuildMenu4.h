// Dialog_GuildMenu4.h: interface for the CDialog_GuildMenu4 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_GUILDMENU4_H__8850F5A4_9462_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_GUILDMENU4_H__8850F5A4_9462_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_MouseInformation;

class CDialog_GuildMenu4 : public CUIRes  // 길드원, 소속 길드
{
public:
	CDialog_GuildMenu4();
	virtual ~CDialog_GuildMenu4();

public:
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void ShowWindow();
};

#endif // !defined(AFX_DIALOG_GUILDMENU4_H__8850F5A4_9462_11D4_98B2_00C02615866B__INCLUDED_)
