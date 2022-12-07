#if !defined(AFX_WNDOBJPREVIEW_H__F115A667_F5E9_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_WNDOBJPREVIEW_H__F115A667_F5E9_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndObjPreview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndObjPreview window
class CDlgTileMapObj;
class CWndObjPreview : public CWnd
{
// Construction
public:
	CWndObjPreview( CDlgTileMapObj* DlgTMapObj );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndObjPreview)
	//}}AFX_VIRTUAL

// Implementation
public:
	CDlgTileMapObj* m_DlgTMapObj;
	virtual ~CWndObjPreview();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndObjPreview)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDOBJPREVIEW_H__F115A667_F5E9_11D4_956A_00D0B77259E8__INCLUDED_)
