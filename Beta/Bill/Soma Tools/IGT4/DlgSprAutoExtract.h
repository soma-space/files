#if !defined(AFX_DLGSPRAUTOEXTRACT_H__09866B21_4EF3_11D3_9559_00C026EE03FC__INCLUDED_)
#define AFX_DLGSPRAUTOEXTRACT_H__09866B21_4EF3_11D3_9559_00C026EE03FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoSpriteDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSprAutoExtract dialog
#include "Resource.h"
#include "..\\IGObject\\AniObjLSP.h"

class CDlgSprAutoExtract : public CDialog
{
// Construction
public:
	void OnCancel();
	void OnOK();
	static int CompareIntegerProc(const void *arg1, const void *arg2);
	static int CompareDirAndFrmsProc(const void *arg1, const void *arg2);
	static int CompareStringAscendProc(const void *arg1, const void *arg2);

	CDlgSprAutoExtract();
	~CDlgSprAutoExtract();

// Dialog Data
	//{{AFX_DATA(CDlgSprAutoExtract)
	enum { IDD = IDD_SPR_AUTO_EXTRACT };
	CProgressCtrl	m_Progress;
	CEdit	m_FileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDlgSprAutoExtract)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	BOOL m_bStopProcess;
// Implementation
protected:
	CString ProcessLSPSprSimple(const char* szPath, POINT ptCenter, int nSubFolderDepth);

	void ClearPreview();
	void UpdatePreview(const _SPR_LSP* pSpr);

	// Generated message map functions
	//{{AFX_MSG(CDlgSprAutoExtract)
	afx_msg void OnBrowseWorkFolderAndStartWorking();
	afx_msg void OnClosedlg();
	afx_msg void OnCancelProcess();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSPRAUTOEXTRACT_H__09866B21_4EF3_11D3_9559_00C026EE03FC__INCLUDED_)
