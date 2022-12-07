// Dialog_GuildName.h: interface for the CDialog_GuildName class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_GUILDNAME_H__152D4C24_947D_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_GUILDNAME_H__152D4C24_947D_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_Editor;

class CDialog_GuildName : public CUIRes  
{
public:
	CDialog_GuildName();
	virtual ~CDialog_GuildName();

	void SendMakeGuild();
	void RcvMakeGuild(PBYTE bypData);
public:
	void SetName(CString name, CString desc);
	void SetWnd(CWnd *wnd) {m_pWnd = wnd;};
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void ShowWindow(int Id, CUIRes *pParent);
	void HideWindow();
	void HideAllWindow();

	BOOL IsValidName(CString t_name);

	int			m_vId;
	CString		m_vName, m_vDesc;
	BOOL		m_bProcessing;
	BBDX_Editor	*m_pDescEditor;
};

#endif // !defined(AFX_DIALOG_GUILDNAME_H__152D4C24_947D_11D4_98B2_00C02615866B__INCLUDED_)
