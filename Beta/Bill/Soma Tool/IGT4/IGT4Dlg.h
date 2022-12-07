// IGT4Dlg.h : header file
//

#if !defined(AFX_IGT4DLG_H__FC3CED68_56A5_44B4_B8F5_BDDE3DF9B8C7__INCLUDED_)
#define AFX_IGT4DLG_H__FC3CED68_56A5_44B4_B8F5_BDDE3DF9B8C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CIGT4Dlg dialog

class CIGT4Dlg : public CDialog
{
// Construction
public:
	void ConvertOldSprToNewSpr(const char* szFolder);
	void ConvertOldAniToNewAni(const char* szFolder);
	void TmpProc(CString szFolder);
	void OnCancel();
	void OnOK();
	CIGT4Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CIGT4Dlg)
	enum { IDD = IDD_IGT4_DIALOG };
	CButton	m_B_MapTile;
	CButton	m_B_AutoSpr;
	CButton	m_B_AniSpr;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIGT4Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	HICON m_hIconAniSpr, m_hIconAutoSpr, m_hIconMapTile; // 각 버튼 아이콘

	// Generated message map functions
	//{{AFX_MSG(CIGT4Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBActivateToolAniSpr();
	afx_msg void OnDestroy();
	afx_msg void OnBActivateToolMapTile();
	afx_msg void OnBActivateToolAutoSpr();
	afx_msg void OnBWorkPathSet();
	afx_msg void OnBActivateToolIntf();
	afx_msg void OnBActivateToolScript();
	afx_msg void OnBActivateToolSprMerge();
	afx_msg void OnBActivateToolConvertBmp();
	afx_msg void OnBActivateToolLookEdit();
	afx_msg void OnBActivateToolItemEdit();
	afx_msg void OnBActivateToolTestColorProcess();
	afx_msg void OnBActivateToolConvertSpr();
	afx_msg void OnBActivateToolConvertAni();
	afx_msg void OnBActivateToolConvertSpr2();
	afx_msg void OnBActivateToolResrc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IGT4DLG_H__FC3CED68_56A5_44B4_B8F5_BDDE3DF9B8C7__INCLUDED_)
