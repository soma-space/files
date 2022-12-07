#if !defined(AFX_DLGINTF_H__79758CC0_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_)
#define AFX_DLGINTF_H__79758CC0_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabZone.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgIntf dialog
const int NODE_LAYER = 1;
const int NODE_INTF = 2;
const int NODE_RENDER_STATE = 3;

#include "StdAfx.h"
#include "Resource.h"

class CDlgIntf : public CDialog
{
public:
	CWnd*	m_pWndWork;
	LPDIRECTDRAWSURFACE m_lpDDSBkg;

public:
	void	RefreshData();
	int		IntfGetValidCheck(int index);
	BOOL	IntfGetValidIndex(int index, int* pnIntf, int *pnState);
	int		IntfGetValidQt();
	int		IntfGetValid();
	void	IntfSetValid(int nIntf);
	int		LayerGetValid();
	void	UpdateTreeByData();
	int		GetRenderStatus();
	void	UpdateDlg();
	void	Refresh();
	void	New();
	void	UpdateWorkWnd();
	void	AniLoad(const char* szFileName);
	void	AniNew();
	
	CDlgIntf(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgIntf();   // standard constructor

public:
	void OnCancel();
	void OnOK();
	void UpdateNodeIntf(HTREEITEM hIntf);
// Dialog Data
	//{{AFX_DATA(CDlgIntf)
	enum { IDD = IDD_INTF };
	CButton	m_CViewOutLine;
	CComboBox	m_CBIntfType;
	CComboBox	m_CBRenderType;
	CButton	m_CViewBkg;
	CButton	m_CViewTrans;
	CButton	m_CViewLine;
	CButton	m_CViewZoom;
	CButton	m_CPlayAnimation;
	CTreeCtrl	m_Tree;
	CListBox	m_AniList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgIntf)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bUpdateData;
	BOOL m_bUpdateInfo;

	// Generated message map functions
	//{{AFX_MSG(CDlgIntf)
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnSaveAs();
	afx_msg void OnBrowseSpr();
	afx_msg void OnBrowseAni();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuLayerAdd();
	afx_msg void OnMenuLayerInsert();
	afx_msg void OnMenuLayerDelete();
	afx_msg void OnMenuIntfAdd();
	afx_msg void OnMenuIntfInsert();
	afx_msg void OnMenuIntfDelete();
	afx_msg void OnMenuIntfCopy();
	afx_msg void OnMenuIntfPaste();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeAniList();
	afx_msg void OnBegindragTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCPlayAnimation();
	afx_msg void OnCViewLine();
	afx_msg void OnCViewBkg();
	afx_msg void OnCViewTrans();
	afx_msg void OnCViewZoom();
	afx_msg void OnChangeEditWidth();
	afx_msg void OnChangeEditHeight();
	afx_msg void OnBeginlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBBrowseBkg();
	afx_msg void OnSelchangeCbIntfType();
	afx_msg void OnChangeEFlag0();
	afx_msg void OnChangeEFlag1();
	afx_msg void OnChangeEFlag2();
	afx_msg void OnChangeEFlag3();
	afx_msg void OnSelchangeCbRenderType();
	afx_msg void OnChangeEA();
	afx_msg void OnChangeER();
	afx_msg void OnChangeEG();
	afx_msg void OnChangeEB();
	afx_msg void OnCViewOutline();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINTF_H__79758CC0_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_)
