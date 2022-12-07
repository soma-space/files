// IGT4.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "IGT4.h"
#include "IGT4Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIGT4App

BEGIN_MESSAGE_MAP(CIGT4App, CWinApp)
	//{{AFX_MSG_MAP(CIGT4App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIGT4App construction

CIGT4App::CIGT4App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIGT4App object

CIGT4App theApp;

#include "DDEngineTool4.h"
#include "AniObjTool.h"
#include "TileSetTool.h"
#include "TileMapTool.h"
#include "IntfTool.h"
#include "MoveCell.h"

#include "WndAniObj.h"
#include "WndTileMap.h"
#include "WndTileMapPreview.h"
#include "WndIntf.h"
#include "DlgScript.h"
#include "DlgLook.h"
#include "DlgItem.h"
#include "DlgColorBltTest.h"
#include "DlgResrc.h"

CDDEngineTool4*		g_pTEng = NULL;
CAniObjTool*		g_pAniObj = NULL;
CTileSetTool*		g_pTSet = NULL;
CTileMapTool*		g_pTMap = NULL;
CIntfTool*			g_pIntf = NULL;
MoveCell*			g_pTMove = NULL;

CWndAniObj			g_WndAni;
CWndTileMapPreview	g_WndTMapPreview;
CWndTileMap			g_WndTMap;
CWndIntf			g_WndIntf;
CDlgScript			g_DlgScript;
CDlgLook			g_DlgLook;
CDlgItem			g_DlgItem;
CDlgColorBltTest	g_DlgCBT;
CDlgResrc			g_DlgResrc;
/////////////////////////////////////////////////////////////////////////////
// CIGT4App initialization

BOOL CIGT4App::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CIGT4Dlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
