#if !defined(AFX_WNDANIOBJ_H__A6D30361_E15A_4823_821F_B9E8442CB4EB__INCLUDED_)
#define AFX_WNDANIOBJ_H__A6D30361_E15A_4823_821F_B9E8442CB4EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndAniObj.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndAniObj window
#include "DlgAniObj.h"

class CWndAniObj : public CWnd
{
// Construction
public:
	CWndAniObj();
	virtual ~CWndAniObj();

// Attributes
public:
	CDlgAniObj m_Dlg;

	CPoint m_ptLBDown;
	CPoint m_ptLBUp;
	CPoint m_ptRBDown;
	CPoint m_ptRBUp;
	CPoint m_ptLMouse;
	CPoint m_ptRMouse;

	CRect m_rcPrevBox; // 조금 전에 그린 박스
	CRect m_rcPrevCross; // 조금 전에 그린 십자

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndAniObj)
	//}}AFX_VIRTUAL

// Implementation
public:
	void RecalcScrollBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndAniObj)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDANIOBJ_H__A6D30361_E15A_4823_821F_B9E8442CB4EB__INCLUDED_)
