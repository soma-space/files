#if !defined(AFX_WNDINTF_H__79758CC1_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_)
#define AFX_WNDINTF_H__79758CC1_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndIntf.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndIntf window
#include "DlgIntf.h"

class CWndIntf : public CWnd
{
// Attributes
public:
	CDlgIntf m_Dlg;

protected:
	CPoint m_ptRecentLB;
	CPoint m_ptRecentRB;
	CRect m_rcRecentLB;
	CRect m_rcRecentRB;

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndIntf)
	//}}AFX_VIRTUAL

// Implementation
public:
	CWndIntf();
	virtual ~CWndIntf();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndIntf)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDINTF_H__79758CC1_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_)
