// Dialog_GuildPositionName.h: interface for the CDialog_GuildPositionName class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_GUILDPOSITIONNAME_H__152D4C25_947D_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_GUILDPOSITIONNAME_H__152D4C25_947D_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_Editor;

class CDialog_GuildPositionName : public CUIRes  
{
public:
	CDialog_GuildPositionName();
	virtual ~CDialog_GuildPositionName();

public:
	void SetName(CString name);
	void SetWnd(CWnd *wnd) {m_pWnd = wnd;};
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void ShowWindow(int Id, CUIRes *pParent, int str_len = 0);
	void HideWindow();
	void HideAllWindow();

	int			m_vId;
	CString		m_vName;
};

#endif // !defined(AFX_DIALOG_GUILDPOSITIONNAME_H__152D4C25_947D_11D4_98B2_00C02615866B__INCLUDED_)
