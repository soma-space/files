#if !defined(AFX_WNDANITILEPREVIEW_H__0CCC0042_F43F_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_WNDANITILEPREVIEW_H__0CCC0042_F43F_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndAniTilePreview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndAniTilePreview window

class CWndAniTilePreview : public CWnd
{
// Construction
public:
	CWndAniTilePreview();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndAniTilePreview)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ActiveAniTile( BOOL bActive );
	void TileSetCur(int nTS, int nTSS);
	void UpdateAniTilePreviewWindow();
	int m_nAniCur;
	int m_nTSSCur;
	int m_nTSCur;
	virtual ~CWndAniTilePreview();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndAniTilePreview)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDANITILEPREVIEW_H__0CCC0042_F43F_11D4_956A_00D0B77259E8__INCLUDED_)
