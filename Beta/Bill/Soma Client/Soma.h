// Soma.h : main header file for the SOMA application
//

#if !defined(AFX_SOMA_H__180690A5_F9A1_11D2_8421_0020AF9F40BD__INCLUDED_)
#define AFX_SOMA_H__180690A5_F9A1_11D2_8421_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

//Packet Á¤ÀÇ
#include "PacketDef.h"
//-----------------------------------------------------------------------------
// Typing shortcuts for deleting and freeing objects.
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETES(p) { if(p) { delete[] (p);	  (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define INIT_PTR(p)		{ (p) = NULL;}



#define WM_PACKET_ARRIVED			(WM_USER+1)
#define WM_MESSAGE_RESTOREALL		(WM_USER+2)
#define WM_PACKET_PROCESS_PENDING	(WM_USER+100)

/////////////////////////////////////////////////////////////////////////////
// CSomaApp:
// See Soma.cpp for the implementation of this class
//

class CSomaApp : public CWinApp
{
public:
	BOOL	CommandLineParsing(CString& conip);

	CSomaApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSomaApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL	m_bActive;
public:
	//{{AFX_MSG(CSomaApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOMA_H__180690A5_F9A1_11D2_8421_0020AF9F40BD__INCLUDED_)
