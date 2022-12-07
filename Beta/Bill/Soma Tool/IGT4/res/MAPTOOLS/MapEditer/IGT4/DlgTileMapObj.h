#if !defined(AFX_DLGTILEMAPOBJ_H__4530154A_8EF0_43D8_98C8_A090F6855620__INCLUDED_)
#define AFX_DLGTILEMAPOBJ_H__4530154A_8EF0_43D8_98C8_A090F6855620__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTileMapObj.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTileMapObj dialog
#include "MapObjTool.h"

class CDlgTileMapObj : public CDialog
{
public:
	CMapObjTool m_MapObj; // 오브젝트 객체...

public:
	short int GetSelectedObjData();
	void UpdateObjInfo();
	void UpdateObjSubSet();
	void UpdateWorkWindow();
	CDlgTileMapObj(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTileMapObj)
	enum { IDD = IDD_TILE_MAP_OBJ };
	CListBox	m_ListObjSubSet;
	CListBox	m_ListObjSet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTileMapObj)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTileMapObj)
	afx_msg void OnBSetSize();
	afx_msg void OnBAniFileNameSet();
	afx_msg void OnBAniFileNameDelete();
	afx_msg void OnBSprFileNameSet();
	afx_msg void OnBSprFileNameDelete();
	afx_msg void OnBNew();
	afx_msg void OnBLoad();
	afx_msg void OnBSave();
	afx_msg void OnBSaveAs();
	afx_msg void OnSelchangeListObjSet();
	afx_msg void OnSelchangeListObjSubSet();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTILEMAPOBJ_H__4530154A_8EF0_43D8_98C8_A090F6855620__INCLUDED_)
