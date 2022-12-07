#if !defined(AFX_DLGSPRMERGE_H__5C17E841_C780_488B_8C50_A8C4CC1651CA__INCLUDED_)
#define AFX_DLGSPRMERGE_H__5C17E841_C780_488B_8C50_A8C4CC1651CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSprMerge.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSprMerge dialog

class CDlgSprMerge : public CDialog
{
protected:
	CString m_szFNClip;

public:
	void OnCancel();
	void OnOK();
	CDlgSprMerge(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSprMerge)
	enum { IDD = IDD_SPR_MERGE };
	CListBox	m_ListRemark;
	CButton	m_CAniGenerateAuto;
	CListBox	m_ListSprs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSprMerge)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgSprMerge)
	afx_msg void OnBSprSet();
	afx_msg void OnBSprInsert();
	afx_msg void OnBSprAdd();
	afx_msg void OnBSprDelete();
	afx_msg void OnBSprMerge();
	afx_msg void OnBSprListNew();
	afx_msg void OnBSprListLoad();
	afx_msg void OnBSprListSave();
	afx_msg void OnBSprListSaveAs();
	virtual BOOL OnInitDialog();
	afx_msg void OnBMovePosUp();
	afx_msg void OnBMovePosDown();
	afx_msg void OnBSprInsertBlank();
	afx_msg void OnSelchangeListRemark();
	afx_msg void OnSelchangeListSpr();
	afx_msg void OnBSprFileNameCopy();
	afx_msg void OnBSprFileNamePaste();
	afx_msg void OnDblclkListSpr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSPRMERGE_H__5C17E841_C780_488B_8C50_A8C4CC1651CA__INCLUDED_)
