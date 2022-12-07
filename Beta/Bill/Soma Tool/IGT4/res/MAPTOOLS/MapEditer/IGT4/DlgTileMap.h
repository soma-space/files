#if !defined(AFX_DLGMAPTILE_H__FAF0E37B_AAD8_4EB4_8324_3E33CF90F4F7__INCLUDED_)
#define AFX_DLGMAPTILE_H__FAF0E37B_AAD8_4EB4_8324_3E33CF90F4F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTileMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTileMap dialog
#include "resource.h"
#include "WndTileSetPreview.h"

class CDlgTileMap : public CDialog
{
public:
	CPoint m_ptTSDest; // 타일셋 목표 지점??
	CWndTileSetPreview m_WndTSPreview; // 타일셋 프리뷰..

	BOOL m_bDontUpdateName;
protected:

public:
	void UpdateObjInfo();
	void UpdateTileSetList();
	void UpdateTileSubSetList();
	void UpdateTileSetInfo();
	void UpdateTileSubSetInfo();
	void OnCancel();
	void OnOK();
	void UpdateTransparentColor();
	void UpdateTileSetPreviewWindow();
	void UpdateDlg();
	void UpdateWorkWindow();
	CDlgTileMap(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTileMap();

// Dialog Data
	//{{AFX_DATA(CDlgTileMap)
	enum { IDD = IDD_TILE_MAP };
	CEdit		m_EditTileSetName;
	CEdit		m_EditTileSubSetName;
	CListBox	m_ListTSS;
	CListBox	m_ListTS;
	CComboBox	m_CBWork;
	CButton		m_CBrushErase;
	CComboBox	m_CBBrush;
	CButton		m_CLayerHide;
	CListBox	m_LBLayer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTileMap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTileMap)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBBrowseBmp();
	afx_msg void OnBNewTileSet();
	afx_msg void OnBLoadTileSet();
	afx_msg void OnBSaveTileSet();
	afx_msg void OnBSaveAsTileSet();
	afx_msg void OnCheckViewGrid();
	afx_msg void OnCheckTileSetTR();
	afx_msg void OnCheckTileSetTG();
	afx_msg void OnCheckTileSetTB();
	afx_msg void OnBNewTileMap();
	afx_msg void OnBLoadTileMap();
	afx_msg void OnBSaveTileMap();
	afx_msg void OnBSaveAsTileMap();
	afx_msg void OnCheckLayerHide();
	afx_msg void OnSelchangeLLayer();
	afx_msg void OnBTileMapSetSize();
	afx_msg void OnBTileMapInitRandom();
	afx_msg void OnBTileMapInit();
	afx_msg void OnCheckViewAttr();
	afx_msg void OnSelendokCbWork();
	afx_msg void OnSelchangeListTileSet();
	afx_msg void OnSelchangeListTileSubSet();
	afx_msg void OnChangeETileSetName();
	afx_msg void OnChangeETileSubSetName();
	afx_msg void OnBTileSetImport();
	afx_msg void OnBTileSetExport();
	afx_msg void OnBSaveTileMapToGameFormat();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRadioNoEdit();
	afx_msg void OnRadioMoveEdit();
	afx_msg void OnRadioHouseEdit();
	afx_msg void OnRadioEventEdit();
	afx_msg void OnActAni();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMAPTILE_H__FAF0E37B_AAD8_4EB4_8324_3E33CF90F4F7__INCLUDED_)
