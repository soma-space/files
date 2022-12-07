#if !defined(AFX_DLGFOLDERBROWSE_H__0A119878_40BB_4E0C_A462_C22453F6DB58__INCLUDED_)
#define AFX_DLGFOLDERBROWSE_H__0A119878_40BB_4E0C_A462_C22453F6DB58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFolderBrowse.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFolderBrowse dialog

class CDlgFolderBrowse : public CDialog
{
// Construction
public:
	CDlgFolderBrowse(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFolderBrowse)
	enum { IDD = IDD_FOLDER_BROWSE };
	CTreeCtrl	m_TreeFolder;
	CComboBox	m_CBRecentFolder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFolderBrowse)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFolderBrowse)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFOLDERBROWSE_H__0A119878_40BB_4E0C_A462_C22453F6DB58__INCLUDED_)
