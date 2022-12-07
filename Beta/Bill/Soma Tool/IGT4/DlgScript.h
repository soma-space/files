//{{AFX_INCLUDES()
#include "spreadsheet.h"
//}}AFX_INCLUDES
#if !defined(AFX_DLGSCRIPT_H__2CD1AC81_AB14_11D2_A73B_0060970DBAD5__INCLUDED_)
#define AFX_DLGSCRIPT_H__2CD1AC81_AB14_11D2_A73B_0060970DBAD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgScript.h : header file
//

#include "Resource.h"
#include "ScriptTool.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgScript dialog

class CDlgScript : public CDialog
{
public:
	void RefreshSheet();
	void LoadScript(const char* szFileName);
	CPoint ConvertStr2Point(CString& string);
	CRect ConvertStr2Rect(CString& string);

	void Encode(int nRow, _SCT_DATA* pSct);
	void Decode(int nRow, const _SCT_DATA* pSct);

	void OnCancel();
	void OnOK();
	void Refresh();
	
	CDlgScript(CWnd* pParent = NULL);   // standard constructor
	~CDlgScript();

	// 멤버 변수
	CScriptTool m_Script;
	double m_Widths[100];

	CString m_szSctFN; // 스크립트 파일 이름..

	int m_nSC[MAX_SCT_COMMAND];		// Script Command - 스크립트 명령(정수값)
	CString m_SC[MAX_SCT_COMMAND];	// Script Command - 스크립트 명령(문자열)
	CString m_SD[MAX_SCT_COMMAND];	// Script Description - 스크립트 설명

// Dialog Data
	//{{AFX_DATA(CDlgScript)
	enum { IDD = IDD_SCRIPT };
	CEdit	m_Edit_ScriptFileName;
//	CStatic	m_Static_FileName;
//	CStatic	m_Static_Usage;
	CEdit	m_Edit_Usage;
	CButton	m_Button_New;
	CButton	m_Button_Load;
	CButton	m_Button_Save;
	CButton	m_Button_SaveAs;
	CSpreadSheet	m_Sheet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScript)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScript)
	afx_msg void OnNew();
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnSaveAs();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnSheetKeyDown(short FAR* KeyCode, short Shift);
	afx_msg void OnSheetButtonClicked(long Col, long Row, short ButtonDown);
	afx_msg void OnSheetRightClick(short ClickType, long Col, long Row, long MouseX, long MouseY);
	afx_msg void OnSheetExpandWidth();
	afx_msg void OnSheetReduceWidth();
	afx_msg void OnSheetRepairWidth();
	afx_msg void OnSheetClick(long Col, long Row);
	afx_msg void OnSheetComboSelChange(long Col, long Row);
	afx_msg void OnSheetTextTipFetch(long Col, long Row, short FAR* MultiLine, long FAR* TipWidth, BSTR FAR* TipText, BOOL FAR* ShowTip);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCRIPT_H__2CD1AC81_AB14_11D2_A73B_0060970DBAD5__INCLUDED_)
