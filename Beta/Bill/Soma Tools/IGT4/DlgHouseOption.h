#if !defined(AFX_DLGHOUSEOPTION_H__AC0C1EA1_84A6_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DLGHOUSEOPTION_H__AC0C1EA1_84A6_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgHouseOption.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgHouseOption dialog

class CDlgHouseOption : public CDialog
{
// Construction
public:
	CDlgHouseOption(CWnd* pParent = NULL);   // standard constructor

	void SetCheckHouse(int house);
	void EnableHouse(BOOL bEnable);
	void CheckHouse(BOOL bEnable);
	void SetTileSet(int ts, int tss);

	int m_vTS, m_vTSS;
	int m_vHouse;
	int m_vHouseOption;
	int m_vEditMode;
	BOOL m_bEnable;
// Dialog Data
	//{{AFX_DATA(CDlgHouseOption)
	enum { IDD = IDD_HOUSE_OPTION };
	CButton	m_wndCheckHouse;
	CButton	m_wndCheckHouseOption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHouseOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgHouseOption)
	afx_msg void OnCheckHouseoption();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioNomove();
	afx_msg void OnRadioMovable();
	afx_msg void OnRadioNotmovable();
	afx_msg void OnRadioNohouse();
	afx_msg void OnRadioHouse();
	afx_msg void OnCheckHouse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHOUSEOPTION_H__AC0C1EA1_84A6_11D4_98B0_00C02615866B__INCLUDED_)
