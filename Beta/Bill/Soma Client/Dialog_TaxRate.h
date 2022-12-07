// Dialog_TaxRate.h: interface for the CDialog_TaxRate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_TAXRATE_H__152D4C26_947D_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_TAXRATE_H__152D4C26_947D_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_Editor;

class CDialog_TaxRate : public CUIRes  
{
public:
	CDialog_TaxRate();
	virtual ~CDialog_TaxRate();

public:
	void SetTaxRate(int tax_rate);
	void SetWnd(CWnd *wnd) {m_pWnd = wnd;};
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void ShowWindow(int Id, CUIRes *pParent);
	void HideWindow();
	void HideAllWindow();

	int			m_vId;
	int			m_vTaxRate;
};

#endif // !defined(AFX_DIALOG_TAXRATE_H__152D4C26_947D_11D4_98B2_00C02615866B__INCLUDED_)
