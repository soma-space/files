// IGT4.h : main header file for the IGT4 application
//

#if !defined(AFX_IGT4_H__0E6E61C0_3744_4690_B443_AC86460E1DCE__INCLUDED_)
#define AFX_IGT4_H__0E6E61C0_3744_4690_B443_AC86460E1DCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIGT4App:
// See IGT4.cpp for the implementation of this class
//

class CIGT4App : public CWinApp
{
public:
	CIGT4App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIGT4App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIGT4App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IGT4_H__0E6E61C0_3744_4690_B443_AC86460E1DCE__INCLUDED_)
