#if !defined(AFX_DLGBMPCONVERT_H__26853FE6_A83E_4CD5_89CA_F15A2D9E3484__INCLUDED_)
#define AFX_DLGBMPCONVERT_H__26853FE6_A83E_4CD5_89CA_F15A2D9E3484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBMPConvert.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBMPConvert dialog

class CDlgBMPConvert : public CDialog
{
// Construction
public:
	CDlgBMPConvert(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBMPConvert)
	enum { IDD = IDD_BMP_CONVERT };
	CListBox	m_ListFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBMPConvert)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBMPConvert)
	afx_msg void OnBAddFile();
	afx_msg void OnBRemoveFile();
	afx_msg void OnBBmpToObm();
	afx_msg void OnBObmToBmp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBMPCONVERT_H__26853FE6_A83E_4CD5_89CA_F15A2D9E3484__INCLUDED_)
