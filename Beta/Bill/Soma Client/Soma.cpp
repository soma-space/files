// Soma.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Soma.h"

#include "MainFrm.h"
#include "DlgLogin.h"
#include "DManage/BB_Direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define WM_KICKIDLE         0x036A  // (params unused) causes idles to kick in
/////////////////////////////////////////////////////////////////////////////
// CSomaApp

BEGIN_MESSAGE_MAP(CSomaApp, CWinApp)
	//{{AFX_MSG_MAP(CSomaApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSomaApp construction

CSomaApp::CSomaApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_bActive = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSomaApp object

CSomaApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSomaApp initialization

BOOL CSomaApp::InitInstance()
{
	CString	szExePath = CDirectDraw::GetExecutionPath(FALSE);
	::SetCurrentDirectory((LPCTSTR)szExePath);

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	//우석...접속할 ip.
	CString   conIP;

#ifndef _DEBUG
	if (!CommandLineParsing(conIP))
	{
		AfxMessageBox("메뉴에서 실행 시켜 주십시요.");
		return FALSE;
	}
#else
	CommandLineParsing(conIP);
#endif
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Menics"));

	if (!CChildView::SocketMng.InitManage())
	{
		return FALSE;
	}

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	//통신 연결부 
#ifdef _DEBUG
	CDlgLogin dlgLogin;
	if(!conIP.IsEmpty()) dlgLogin.m_szIP = conIP;
	dlgLogin.DoModal();
	if (FALSE==dlgLogin.m_bOk)
	{
		SAFE_DELETE(pFrame);
		m_pMainWnd = NULL;
		return FALSE;
	}
	else
	{
		conIP = dlgLogin.m_szIP;	
	}

	pFrame->m_bFullScreen = !dlgLogin.m_bWindowMode;
#else
	pFrame->m_bFullScreen = TRUE;
#endif
	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME, 
		WS_OVERLAPPEDWINDOW&(~WS_THICKFRAME), NULL,
		NULL);

	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	//우석. 접속할 ip 셋팅.
	pFrame->m_wndView.m_conIP = conIP;

	if (!pFrame->m_wndView.DDInit())
	{
		pFrame->DestroyWindow();
		SAFE_DELETE(pFrame);
		m_pMainWnd = NULL;
		return FALSE;
	}
	
//	CChildView::SocketMng.SendLogin();
	// 처음에 계정 아이디로 접속한뒤 내가 가지고 있는 아이디를 얻어 낸다.
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSomaApp message handlers


int CSomaApp::Run() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_pMainWnd == NULL && AfxOleGetUserCtrl())
	{
		// Not launched /Embedding or /Automation, but has no main window!
		TRACE0("Warning: m_pMainWnd is NULL in CWinApp::Run - quitting application.\n");
		AfxPostQuitMessage(0);
	}
	
	ASSERT_VALID(this);

	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;

	CMainFrame **mainWnd = (CMainFrame**)&m_pMainWnd;
	BOOL bPeekMessage = TRUE;

	for (;;)
	{
//		if (!m_bActive) WaitMessage();
		bPeekMessage = ::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_REMOVE);
		if (bIdle&&!bPeekMessage&&*mainWnd) 
		{
			if (m_bActive) (*mainWnd)->m_wndView.IdleProcess(lIdleCount);
			else (*mainWnd)->m_wndView.NFocusIdleProcess(lIdleCount);
			continue;
		}

		if (bPeekMessage)
		{
			if (m_msgCur.message==WM_QUIT) return ExitInstance();
			else if (m_msgCur.message != WM_KICKIDLE && !PreTranslateMessage(&m_msgCur))
			{
				::TranslateMessage(&m_msgCur);
				::DispatchMessage(&m_msgCur);
			}
			if (IsIdleMessage(&m_msgCur)) bIdle = TRUE;
			else bIdle = FALSE;
		}
		else bIdle = TRUE;
	}

	ASSERT(FALSE);  // not reachable
}

int CSomaApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	CChildView::SocketMng.EndManage();
	if (m_pMainWnd&&m_pMainWnd->GetSafeHwnd()) 
	{
		m_pMainWnd->DestroyWindow();
		SAFE_DELETE(m_pMainWnd);
	}
	AfxSocketTerm();
	return CWinApp::ExitInstance();
}

BOOL CSomaApp::CommandLineParsing(CString& conip)
{
	if (__argc!=2) return FALSE;
	conip = __argv[1];
/*	for (int i=1; i<__argc; i++)
	{
		switch(i)
		{
		case 1: CChildView::SocketMng.m_szDestIp = __argv[i];
			break;
		case 2: CChildView::SocketMng.m_iPort = atoi(__argv[i]);
			break;
		default:
			break;
		}
	}*/
	return TRUE;
}