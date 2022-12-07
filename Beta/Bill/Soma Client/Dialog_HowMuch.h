// Dialog_HowMuch.h: interface for the CDialog_HowMuch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_HOWMUCH_H__B2B3A2A1_7790_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_HOWMUCH_H__B2B3A2A1_7790_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_Editor;

class CDialog_HowMuch : public CUIRes  
{
public:
	CDialog_HowMuch();
	virtual ~CDialog_HowMuch();

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

#endif // !defined(AFX_DIALOG_HOWMUCH_H__B2B3A2A1_7790_11D4_98B0_00C02615866B__INCLUDED_)
