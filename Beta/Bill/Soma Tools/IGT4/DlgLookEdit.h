#if !defined(AFX_DLGLOOKEDIT_H__923958C1_1F46_4BDB_8E76_BDAE2D6CE35B__INCLUDED_)
#define AFX_DLGLOOKEDIT_H__923958C1_1F46_4BDB_8E76_BDAE2D6CE35B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLookEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgLookEdit dialog

#include "ObjLookTool.h"

class CDlgLookEdit : public CDialog
{
// Construction
public:
	void UpdatePreview();
	CDlgLookEdit(CWnd* pParent = NULL);   // standard constructor
	~CDlgLookEdit();

protected:
	CObjLookTool	m_ObjLook;
	CPoint			m_ptCenter;
	int				m_nPartIndices[MAX_LOOK_PART];
	_LOOK_DATA*		m_pLookClip;

public:
	void UpdateLookComboBox();
	void UpdatePartComboBox();
	void UpdatePartAniList();
	void UpdateLookInfo();
	void UpdateLookData();
	void UpdateAniList();
// Dialog Data
	//{{AFX_DATA(CDlgLookEdit)
	enum { IDD = IDD_LOOK_EDIT };
	
	CComboBox	m_CBPart;
	CListBox	m_ListPartAni;
	CListBox	m_ListAni;
	
	CComboBox	m_CBLook;

	CButton		m_CAnimation;
	CStatic		m_Preview;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLookEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLookEdit)
	afx_msg void OnStaticPreview();
	afx_msg void OnBLoad();
	afx_msg void OnBNew();
	afx_msg void OnBSave();
	afx_msg void OnBSaveAs();
	afx_msg void OnBBrowseSpr();
	afx_msg void OnBBrowseAni();
	afx_msg void OnSelchangeListAni();
	virtual BOOL OnInitDialog();
	afx_msg void OnCAnimation();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnBLookQtSet();
	afx_msg void OnBPartQtSet();
	afx_msg void OnBAniQtSet();
	afx_msg void OnSelchangeListPartAni();
	afx_msg void OnSelchangeCbPart();
	afx_msg void OnSelchangeCbLook();
	afx_msg void OnBCopy();
	afx_msg void OnBPaste();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLOOKEDIT_H__923958C1_1F46_4BDB_8E76_BDAE2D6CE35B__INCLUDED_)
