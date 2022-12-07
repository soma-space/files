// Dialog_Quit.h: interface for the CDialog_Quit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_QUIT_H__7408B4E1_96BD_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_QUIT_H__7408B4E1_96BD_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"

class CDialog_Quit : public CUIRes  
{
public:
	CDialog_Quit();
	virtual ~CDialog_Quit();

public:
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void ShowWindow(int Id, CUIRes *pParent);
	void HideWindow();
	void HideAllWindow();

	int				m_vId;
};

#endif // !defined(AFX_DIALOG_QUIT_H__7408B4E1_96BD_11D4_98B2_00C02615866B__INCLUDED_)
