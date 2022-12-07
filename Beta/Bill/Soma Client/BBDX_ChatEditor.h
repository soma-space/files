#if !defined(AFX_BBDX_CHATEDITOR_H__DAE22186_7E60_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_BBDX_CHATEDITOR_H__DAE22186_7E60_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BBDX_ChatEditor.h : header file
//

#include "BBDX_Editor.h"

/////////////////////////////////////////////////////////////////////////////
// BBDX_ChatEditor window

class BBDX_ChatEditor : public BBDX_Editor
{
// Construction
public:
	BBDX_ChatEditor(CWnd *pParent, CUIRes *resParent, BOOL bModeMulti, CRect area, int lineGap = 0, DWORD moreStyle=0L);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BBDX_ChatEditor)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~BBDX_ChatEditor();

	// Generated message map functions
protected:
	//{{AFX_MSG(BBDX_ChatEditor)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BBDX_CHATEDITOR_H__DAE22186_7E60_11D4_98B0_00C02615866B__INCLUDED_)
