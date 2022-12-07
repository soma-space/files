#if !defined(AFX_WNDTILEMINIMAP_H__1AD7DF07_8EBE_4676_90ED_2CAA6ECC9C66__INCLUDED_)
#define AFX_WNDTILEMINIMAP_H__1AD7DF07_8EBE_4676_90ED_2CAA6ECC9C66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndTileMiniMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndTileMapPreview window

class CWndTileMapPreview : public CWnd
{
// Construction
public:
	CWndTileMapPreview();

protected:
	CPoint m_ptLBDown;
	LPDIRECTDRAWSURFACE m_lpDDSs[MAX_TILE_MAP_LAYER];
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndTileMapPreview)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Alloc(int width, int height);
	void UpdateData(RECT* pRC, BOOL bDelete = FALSE);
	virtual ~CWndTileMapPreview();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndTileMapPreview)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDTILEMINIMAP_H__1AD7DF07_8EBE_4676_90ED_2CAA6ECC9C66__INCLUDED_)
