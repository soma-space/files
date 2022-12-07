#if !defined(AFX_DIRTREECTRL_H__F5512005_305A_11D2_B989_00AA00AC7921__INCLUDED_)
#define AFX_DIRTREECTRL_H__F5512005_305A_11D2_B989_00AA00AC7921__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DirTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl window
//
// By Nicola Delfino 1999 delfinon@altavista.net
//
// 
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. 
// THE AUTHOR DISCLAIMS ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, 
// INCLUDING THE WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
// ANY DAMAGES WHATSOEVER INCLUDING DIRECT, INDIRECT, INCIDENTAL, 
// CONSEQUENTIAL, LOSS OF BUSINESS PROFITS OR SPECIAL DAMAGES.
// 
////////////////////////////////////////////////////////////////////////////////

class CDirTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CDirTreeCtrl();

// Attributes
public:

// Operations
public:
    void Initialize (bool clear=true);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	bool SetPath (CString path);
	CString GetFullPath (HTREEITEM item);
	
	virtual ~CDirTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDirTreeCtrl)
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	HTREEITEM Dir2Item(HTREEITEM current, char *dir);
	virtual bool MatchExtension(CString file);
	void RefreshTreeImages(HTREEITEM item, int img);
	CString GetPathFromHere (CString s, HTREEITEM itm);
	HTREEITEM AddItem_Core (const char *name, HTREEITEM parent, bool isDir, int icon, int iconOpen);
	void ExpandBranch (HTREEITEM parent);
	void DeleteAllChild (HTREEITEM itm,  bool isEmpty=true);
	HTREEITEM AddItem(const char * path, HTREEITEM parent, bool isDir=FALSE, int iconEx=-1, int iconEx2=-1);

	HTREEITEM m_desktop_root;
	CString   m_desktop_path;

	HTREEITEM m_drives_root;
	char      m_drives_name[500];

	CImageList m_img;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREECTRL_H__F5512005_305A_11D2_B989_00AA00AC7921__INCLUDED_)
