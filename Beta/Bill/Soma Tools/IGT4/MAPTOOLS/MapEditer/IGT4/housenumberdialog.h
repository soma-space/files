#if !defined(AFX_HOUSENUMBERDIALOG_H__F6D9F6C6_6212_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_HOUSENUMBERDIALOG_H__F6D9F6C6_6212_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HouseNumberDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHouseNumberDialog dialog

class CHouseNumberDialog : public CDialog
{
// Construction
public:
	CHouseNumberDialog(CWnd* pParent = NULL);   // standard constructor

	void SetCaption(LPCTSTR caption);
// Dialog Data
	//{{AFX_DATA(CHouseNumberDialog)
	enum { IDD = IDD_HOUSE_NUMBER };
	short	m_vHouseNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHouseNumberDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_vCaption;
	// Generated message map functions
	//{{AFX_MSG(CHouseNumberDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOUSENUMBERDIALOG_H__F6D9F6C6_6212_11D4_98B0_00C02615866B__INCLUDED_)
