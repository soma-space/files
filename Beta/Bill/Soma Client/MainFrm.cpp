// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Soma.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMainFrame* __GetMainWnd()
{
	return (CMainFrame*)AfxGetMainWnd();
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_ACTIVATEAPP()
	ON_MESSAGE(WM_MESSAGE_RESTOREALL, OnRestoreSurface)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bFullScreen = FALSE;	
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

//	CRect r(0, 0, 640, 480);
	CRect r(0, 0, 800, 600);
	if (m_bFullScreen) 
	{
		r.SetRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		cs.style = WS_POPUP|WS_VISIBLE;
		cs.dwExStyle = WS_EX_TOPMOST;
	}
	AdjustWindowRectEx(&r, cs.style, FALSE, cs.dwExStyle);
	cs.cx = r.Width();
	cs.cy = r.Height();

	cs.hMenu = NULL;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	CFrameWnd::OnSetFocus(pOldWnd);
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	//WindowCursor¸¦ °¨Ãá´Ù
	ShowCursor(FALSE);
	return 0;
}

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	((CSomaApp*)AfxGetApp())->m_bActive = bActive;
	// TODO: Add your message handler code here
	TRACE("Active Status Change : %d\n", bActive);	
}

LRESULT CMainFrame::OnRestoreSurface(WPARAM, LPARAM)
{
	SetFocus();
	m_wndView.SetFocus(); 
	if (((CSomaApp*)AfxGetApp())->m_bActive) m_wndView.DirectXMng.m_pDD->RestoreAllSurfaces();
//	if (((CSomaApp*)AfxGetApp())->m_bActive) m_wndView.DirectXMng.RestoreSurface();
	return 0L;
}