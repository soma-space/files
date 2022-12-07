#if !defined(AFX_DLGANIOBJ_H__C581F662_B5E6_11D2_B0A3_0060970DBAD5__INCLUDED_)
#define AFX_DLGANIOBJ_H__C581F662_B5E6_11D2_B0A3_0060970DBAD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabAni.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAniObj dialog
#include "StdAfx.h"
#include "Resource.h"
#include "AniObjTool.h"

class CDlgAniObj : public CDialog
{
// Construction
public:
	void	UpdateAniInfo(); // 대화상자 에니메이션 정보 갱신
	void	UpdateAniList(); // 에니메이션 리스트등 갱신..

	void	UpdateSprInfo(); // 대화상자 스프라이트 정보 갱신
	void	UpdateSprObjInfo(); // 대화상자 스프라이트 정보 갱신
	void	UpdateWorkWindow(); // 작업 윈도우 갱신

	void	SetFileName(const char* szFileName);
	CString GetFileName();

	CDlgAniObj(); // standard constructor
	~CDlgAniObj();

public: // 멤버변수
	void OnCancel();
	void OnOK();

	CWnd*	m_pWndWork; // 작업창

	BOOL	m_bPlaying;
	BOOL	m_bRefreshing; // 정보 갱신중....
	int		m_nFrmCurs[MAX_ANI];
	
	CAniObjTool*	m_pAniObj;
	_ANI_DATA* m_pClipAni; // 복사.. 붙이기에 쓸 데이터..

// Dialog Data
	//{{AFX_DATA(CDlgAniObj)
	enum { IDD = IDD_ANI_OBJ };
	CComboBox	m_CBDir;
	CComboBox	m_CBChrSpr;
	CComboBox	m_CBWork;
	CSliderCtrl	m_SliderSpr;
	CEdit		m_EditAniName;
	CListBox	m_ListAni;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAniObj)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAniObj)
	afx_msg void OnBAniFrmNext();
	afx_msg void OnBAniFrmPrev();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBAniPlay();
	afx_msg void OnBAniAdd();
	afx_msg void OnBAniDelete();
	afx_msg void OnBAniInsert();
	afx_msg void OnBAniCopy();
	afx_msg void OnBAniPaste();
	afx_msg void OnBAniNew();
	afx_msg void OnBAniLoad();
	afx_msg void OnBAniSave();
	afx_msg void OnBAniSaveAs();
	afx_msg void OnSelchangeAniList();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBAniFrmsGetFromString(); // 에디트에 있는 문자열에서 숫자 배열을 끄집어 온다.
	afx_msg void OnBAniFrmsGetAutoFromFront();
	afx_msg void OnBAniFrmsGetAutoSeq();
	afx_msg void OnBAniFrmsSetSameToFront();
	afx_msg void OnChangeAniName();
	afx_msg void OnBSprExportToBmp();
	afx_msg void OnBSprImportFromBmp();
	afx_msg void OnChangeEAniMaxFrm();
	afx_msg void OnBSprLoad();
	afx_msg void OnBSprNew();
	afx_msg void OnBSprSave();
	afx_msg void OnBSprSaveAs();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBSprSetQt();
	afx_msg void OnBSprBrowseBmp();
	afx_msg void OnCSprCkR();
	afx_msg void OnCSprCkG();
	afx_msg void OnCSprCkB();
	afx_msg void OnSelendokCbWork();
	afx_msg void OnChangeEAniSpeed();
	afx_msg void OnBAniSetDirQt();
	afx_msg void OnSelchangeCbDir();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGANIOBJ_H__C581F662_B5E6_11D2_B0A3_0060970DBAD5__INCLUDED_)
