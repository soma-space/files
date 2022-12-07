// Dialog_HowMany.h: interface for the CDialog_HowMany class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_HOWMANY_H__73F22BC4_CF49_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_HOWMANY_H__73F22BC4_CF49_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"

class CDialog_HowMany : public CUIRes  
{
public:
	CDialog_HowMany();
	virtual ~CDialog_HowMany();

	void SetWnd(CWnd *wnd) {m_pWnd = wnd;};
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void ShowWindow(int Id, CUIRes *pParent);
	void HideWindow();
	void HideAllWindow();

	int			m_vId;
	DWORD		m_vValue;
};

#endif // !defined(AFX_DIALOG_HOWMANY_H__73F22BC4_CF49_11D4_98B2_00C02615866B__INCLUDED_)
