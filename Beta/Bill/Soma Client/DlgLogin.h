#if !defined(AFX_DLGLOGIN_H__8A02FEA1_4E3D_11D3_8423_00105A6B96E7__INCLUDED_)
#define AFX_DLGLOGIN_H__8A02FEA1_4E3D_11D3_8423_00105A6B96E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLogin.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgLogin dialog

class CDlgLogin : public CDialog
{
// Construction
public:
	CDlgLogin(CWnd* pParent = NULL);   // standard constructor

	BOOL	m_bOk;
// Dialog Data
	//{{AFX_DATA(CDlgLogin)
	enum { IDD = IDD_DIALOG_LOGIN };
	CString	m_szIP;
	CString	m_szID;
	CString	m_szPasswd;
	BOOL	m_bWindowMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLogin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLogin)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLOGIN_H__8A02FEA1_4E3D_11D3_8423_00105A6B96E7__INCLUDED_)
