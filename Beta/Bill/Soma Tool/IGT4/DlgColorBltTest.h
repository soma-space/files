#if !defined(AFX_DLGCOLORBLTTEST_H__00F0772F_1B80_4133_AE7D_12BC308CDA7A__INCLUDED_)
#define AFX_DLGCOLORBLTTEST_H__00F0772F_1B80_4133_AE7D_12BC308CDA7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgColorBltTest.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgColorBltTest dialog

#include "..\\IGObject\\AniObjLSP.h"

class CDlgColorBltTest : public CDialog
{
protected:
	CAniObjLSP m_AniObj;

public:
	void UpdatePreview();
	CDlgColorBltTest(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgColorBltTest)
	enum { IDD = IDD_COLOR_BLT_TEST };
	CSliderCtrl	m_SldSpr;
	CStatic	m_SPreview;
	CSliderCtrl	m_SldR;
	CSliderCtrl	m_SldG;
	CSliderCtrl	m_SldB;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColorBltTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgColorBltTest)
	afx_msg void OnBSprBrowse();
	afx_msg void OnChangeER();
	afx_msg void OnChangeEG();
	afx_msg void OnChangeEB();
	afx_msg void OnChangeESpr();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLORBLTTEST_H__00F0772F_1B80_4133_AE7D_12BC308CDA7A__INCLUDED_)
