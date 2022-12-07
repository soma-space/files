#if !defined(AFX_DLGGAMERESRCEDIT_H__55231DF0_91CF_4629_B666_519FFEEFEE10__INCLUDED_)
#define AFX_DLGGAMERESRCEDIT_H__55231DF0_91CF_4629_B666_519FFEEFEE10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgResrc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgResrc dialog
#include "..\\IGObject\\GameResrc.h"

class CDlgResrc : public CDialog
{
public:
protected:
	CGameResrc m_Resrc;
// Construction
public:
	void UpdateInfoAvata1(int nIndex);
	void UpdateInfoAvata0(int nIndex);
	void UpdateInfoNPC(int nIndex);
	void UpdateInfo();
	CDlgResrc(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgResrc)
	enum { IDD = IDD_RESRC };
	CListBox	m_ListMap;
	CListBox	m_ListAvata0;
	CListBox	m_ListAvata1;
	CListBox	m_ListNPC;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgResrc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgResrc)
	virtual BOOL OnInitDialog();
	afx_msg void OnBMapBrowse();
	afx_msg void OnBMapDelete();
	afx_msg void OnBAvata0Browse();
	afx_msg void OnBAvata0Delete();
	afx_msg void OnBAvata1Browse();
	afx_msg void OnBAvata1Delete();
	afx_msg void OnBNPCAniBrowse();
	afx_msg void OnBNPCAniDelete();
	afx_msg void OnBNPCSprBrowse();
	afx_msg void OnBNPCSprDelete();
	afx_msg void OnBNPCShadowBrowse();
	afx_msg void OnBNPCShadowDelete();
	afx_msg void OnBNew();
	afx_msg void OnBLoad();
	afx_msg void OnBSave();
	afx_msg void OnBSaveAs();
	afx_msg void OnSelchangeListMap();
	afx_msg void OnSelchangeListAvata0();
	afx_msg void OnSelchangeListAvata1();
	afx_msg void OnSelchangeListNpc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGAMERESRCEDIT_H__55231DF0_91CF_4629_B666_519FFEEFEE10__INCLUDED_)
