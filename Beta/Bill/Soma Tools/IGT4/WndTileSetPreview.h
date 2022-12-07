#if !defined(AFX_WNDTILESETPREVIEW_H__F0987F2A_D4D1_4418_A0F8_68502E92F5EE__INCLUDED_)
#define AFX_WNDTILESETPREVIEW_H__F0987F2A_D4D1_4418_A0F8_68502E92F5EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndTileSetPreview.h : header file
//

#include "MouseSelectManager.h"
#include "DlgHouseOption.h"

/////////////////////////////////////////////////////////////////////////////
// CWndTileSetPreview window

class CWndTileSetPreview : public CWnd
{
// Construction
public:
	CWndTileSetPreview();

// Attributes
public:
	CDlgHouseOption m_DlgHouse;
	MouseSelectManager m_MSM;
	int m_nTSCur;
	int m_nTSSCur;
	int m_nAniCur;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndTileSetPreview)
	//}}AFX_VIRTUAL

// Implementation
public:
	void TileSetCur(int nTS, int nTSS);
	virtual ~CWndTileSetPreview();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndTileSetPreview)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDTILESETPREVIEW_H__F0987F2A_D4D1_4418_A0F8_68502E92F5EE__INCLUDED_)
