#if !defined(AFX_DLGCOLOR_H__44E3A6E4_B217_4AA2_8B5C_10C95710435F__INCLUDED_)
#define AFX_DLGCOLOR_H__44E3A6E4_B217_4AA2_8B5C_10C95710435F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgColor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgColor dialog

class CDlgColor : public CDialog
{
protected:
	COLORREF m_crColor;

// Construction
public:
	void UpdateColor();
	COLORREF GetColor() { return m_crColor; }
	CDlgColor(CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CDlgColor)
	enum { IDD = IDD_COLOR };
	CSliderCtrl	m_SldR;
	CSliderCtrl	m_SldG;
	CSliderCtrl	m_SldB;
	CStatic	m_StaticColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgColor)
	afx_msg void OnChangeER();
	afx_msg void OnChangeEG();
	afx_msg void OnChangeEB();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOLOR_H__44E3A6E4_B217_4AA2_8B5C_10C95710435F__INCLUDED_)
