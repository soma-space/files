#if !defined(AFX_DLGITEM_H__7508E4BF_94EB_471D_95F2_7719AF3BC4D6__INCLUDED_)
#define AFX_DLGITEM_H__7508E4BF_94EB_471D_95F2_7719AF3BC4D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgItem dialog
#include "ItemTool.h"

class CDlgItem : public CDialog
{
protected:
	int			m_nCurSel;
	CItemTool	m_Item;
public:
	void UpdateItemListBox();
	void UpdateAniListBox();
	void UpdatePreview();
	void UpdateItemData();
	void UpdateItemInfo();
	CDlgItem(CWnd* pParent = NULL);   // standard constructor
	~CDlgItem();

// Dialog Data
	//{{AFX_DATA(CDlgItem)
	enum { IDD = IDD_ITEM };
	CStatic	m_SPreview;
	CListBox	m_LBAni;
	CListBox	m_LBItem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgItem)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgItem)
	afx_msg void OnBBrowseSpr();
	afx_msg void OnBBrowseAni();
	afx_msg void OnBNew();
	afx_msg void OnBLoad();
	afx_msg void OnBSave();
	afx_msg void OnBSaveAs();
	afx_msg void OnSelchangeLbItem();
	afx_msg void OnBQtSet();
	afx_msg void OnSPreview();
	afx_msg void OnSelchangeLbAni();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGITEM_H__7508E4BF_94EB_471D_95F2_7719AF3BC4D6__INCLUDED_)
