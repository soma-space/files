#if !defined(AFX_DLGHOUSEINFO_H__3A8759B7_1D8F_4BD4_B69F_7E048906F3AF__INCLUDED_)
#define AFX_DLGHOUSEINFO_H__3A8759B7_1D8F_4BD4_B69F_7E048906F3AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgHouseInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgHouseInfo dialog

class CDlgHouseInfo : public CDialog
{
public:
	_HOUSE_HEADER m_Header; // 헤더
	_HOUSE_DATA m_Datas[MAX_HOUSE_DATA]; // 집정보 데이터
	int m_vSizeX;
	int m_vSizeY;

// Construction
public:
	CDlgHouseInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgHouseInfo)
	enum { IDD = IDD_HOUSE_INFO };
	CListBox	m_ListHouse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHouseInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgHouseInfo)
	afx_msg void OnSelchangeListHouse();
	afx_msg void OnChangeEHouseNumber();
	virtual BOOL OnInitDialog();
	afx_msg void OnBNew();
	afx_msg void OnBLoad();
	afx_msg void OnBSave();
	afx_msg void OnBSaveAs();
	afx_msg void OnBRgnInit();
	afx_msg void OnGoPos();
	afx_msg void OnChangeSizeX();
	afx_msg void OnChangeSizeY();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHOUSEINFO_H__3A8759B7_1D8F_4BD4_B69F_7E048906F3AF__INCLUDED_)
