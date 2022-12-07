#if !defined(AFX_DLGFACEEDIT_H__923958C1_1F46_4BDB_8E76_BDAE2D6CE35B__INCLUDED_)
#define AFX_DLGFACEEDIT_H__923958C1_1F46_4BDB_8E76_BDAE2D6CE35B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFaceEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFaceEdit dialog

#include "ObjFaceTool.h"

class CDlgFaceEdit : public CDialog
{
// Construction
public:
	void UpdatePreview();
	CDlgFaceEdit(CWnd* pParent = NULL);   // standard constructor

protected:
	CComboBox*		m_pCBs[12];
	CObjFaceTool	m_ObjFace;
	CPoint			m_ptCenter;
	int				m_nPartSel;

public:
	void UpdateFaceComboBox(int index);
	void UpdateFaceSingleInfo(int index);
	void UpdateFaceWholeInfo();
	void UpdateFaceData();
	void UpdateAniList();
// Dialog Data
	//{{AFX_DATA(CDlgFaceEdit)
	enum { IDD = IDD_FACE_EDIT };
	CButton	m_CAnimation;
	CStatic	m_Preview;
	CListBox	m_ListAni;
	CComboBox	m_CBNose;
	CComboBox	m_CBNeckLace0;
	CComboBox	m_CBNeckLace1;
	CComboBox	m_CBLips;
	CComboBox	m_CBHairPin;
	CComboBox	m_CBHairRear;
	CComboBox	m_CBHairFront;
	CComboBox	m_CBFace;
	CComboBox	m_CBEye;
	CComboBox	m_CBEarRing;
	CComboBox	m_CBEar;
	CComboBox	m_CBClothes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFaceEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFaceEdit)
	afx_msg void OnSetFocusCBHairRear();
	afx_msg void OnSetFocusCBFace();
	afx_msg void OnSetFocusCBNectLace0();
	afx_msg void OnSetFocusCBClothes();
	afx_msg void OnSetFocusCBNectLace1();
	afx_msg void OnSetFocusCBEye();
	afx_msg void OnSetFocusCBNose();
	afx_msg void OnSetFocusCBLips();
	afx_msg void OnSetFocusCBEar();
	afx_msg void OnSetFocusCBEarring();
	afx_msg void OnSetFocusCBHairFront();
	afx_msg void OnSetFocusCBHairpin();
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
	afx_msg void OnBAssembleQtSet();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFACEEDIT_H__923958C1_1F46_4BDB_8E76_BDAE2D6CE35B__INCLUDED_)
