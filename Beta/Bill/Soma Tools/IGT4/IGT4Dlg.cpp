// IGT4Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "IGT4.h"
#include "IGT4Dlg.h"

#include <stdio.h>
#include <wchar.h>

#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
#include "DlgSprAutoExtract.h"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIGT4Dlg dialog

CIGT4Dlg::CIGT4Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIGT4Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIGT4Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIGT4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIGT4Dlg)
	DDX_Control(pDX, IDC_B_ACTIVATE_TOOL_MAP_TILE, m_B_MapTile);
	DDX_Control(pDX, IDC_B_ACTIVATE_TOOL_AUTO_SPR, m_B_AutoSpr);
	DDX_Control(pDX, IDC_B_ACTIVATE_TOOL_ANI_SPR, m_B_AniSpr);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIGT4Dlg, CDialog)
	//{{AFX_MSG_MAP(CIGT4Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_ANI_SPR, OnBActivateToolAniSpr)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_MAP_TILE, OnBActivateToolMapTile)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_AUTO_SPR, OnBActivateToolAutoSpr)
	ON_BN_CLICKED(IDC_B_WORK_PATH_SET, OnBWorkPathSet)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_INTF, OnBActivateToolIntf)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_SCRIPT, OnBActivateToolScript)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_SPR_MERGE, OnBActivateToolSprMerge)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_CONVERT_BMP, OnBActivateToolConvertBmp)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_LOOK_EDIT, OnBActivateToolLookEdit)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_ITEM_EDIT, OnBActivateToolItemEdit)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_TEST_COLOR_PROCESS, OnBActivateToolTestColorProcess)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_CONVERT_SPR, OnBActivateToolConvertSpr)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_CONVERT_ANI, OnBActivateToolConvertAni)
	ON_BN_CLICKED(IDC_B_ACTIVATE_TOOL_RESRC, OnBActivateToolResrc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIGT4Dlg message handlers

BOOL CIGT4Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	// 각 버튼 마다 하나씩 비트맵 적용..
	m_hIconAniSpr = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TOOL_ANI_SPR));
	m_hIconAutoSpr = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TOOL_AUTO_SPR));
	m_hIconMapTile = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_TOOL_MAP_TILE));
	m_B_AniSpr.SetIcon(m_hIconAniSpr);
	m_B_AutoSpr.SetIcon(m_hIconAutoSpr);
	m_B_MapTile.SetIcon(m_hIconMapTile);

	// 엔진 초기화...
	g_pTEng = new CDDEngineTool4;
	g_pTEng->Init(this->m_hWnd, FALSE, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	// 최근에 쓴 폴더를 가져온다...
	char szInitDir[128]; memset(szInitDir, 0, 128);
	DWORD dwLength = 128, dwType = REG_SZ;
	LONG success = RegQueryValueEx(HKEY_CURRENT_USER, "SOFTWARE\\I'llSoft\\IGT4", 0, &dwType, (unsigned char*)szInitDir, &dwLength);
	if(success == ERROR_SUCCESS) g_pTEng->PathSetWork(szInitDir);
	SetDlgItemText(IDC_E_WORK_PATH, g_pTEng->PathGetCur(NULL));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIGT4Dlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	DestroyIcon(m_hIconAniSpr);
	DestroyIcon(m_hIconAutoSpr);
	DestroyIcon(m_hIconMapTile);

	// 마지막에 작업한 폴더 기억
	char szInitDir[128]; memset(szInitDir, 0, 128);
	lstrcpy(szInitDir, g_pTEng->PathGetCur(NULL));
	DWORD dwLength = lstrlen(szInitDir); DWORD dwType = REG_SZ;
	LONG success = RegSetValueEx(HKEY_CURRENT_USER, "SOFTWARE\\I'llSoft\\IGT4", 0, dwType, (unsigned char*)szInitDir, dwLength);

	if(g_pAniObj) { delete g_pAniObj; g_pAniObj = NULL; }
	if(g_pTEng) { delete g_pTEng; g_pTEng = NULL; }
}

void CIGT4Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIGT4Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIGT4Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CIGT4Dlg::OnBActivateToolAniSpr() 
{
	if(g_WndAni.GetSafeHwnd() == NULL)
	{
		// 스프라이트 윈도우를 만든다.. - 아직 보이지는 않고 툴바에서 호출해야만 보인다.
		HCURSOR hCur = LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_C_NORMAL));
		CString szClass;
		szClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, hCur, NULL, 0);
		
		g_WndAni.CreateEx(	NULL,
							szClass,
							"Sprite, Animation Working Window",
							WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							800,
							600,
							this->m_hWnd,
							NULL,
							NULL);
		ShowCursor(TRUE);
		CRect rcAdjust(0, 0, 800, 600);
		AdjustWindowRect(rcAdjust, g_WndAni.GetStyle(), FALSE);
		g_WndAni.SetWindowPos(NULL, 0, 0, rcAdjust.Width(), rcAdjust.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	g_WndAni.ShowWindow(SW_SHOW);
	g_WndAni.EnableToolTips(TRUE); // 툴팁 가능..
	g_WndAni.SetFocus();
}

void CIGT4Dlg::OnBActivateToolMapTile() 
{
	if(g_WndTMap.GetSafeHwnd() == NULL)
	{
		// 스프라이트 윈도우를 만든다.. - 아직 보이지는 않고 툴바에서 호출해야만 보인다.
		HCURSOR hCur = LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_C_NORMAL));
		CString szClass;
		szClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, hCur, NULL, 0);
		
		g_WndTMap.CreateEx(	NULL,
									szClass,
									"Tile Map Editor Window",
									WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
									CW_USEDEFAULT,
									CW_USEDEFAULT,
									800,
									600,
									this->m_hWnd,
									NULL,
									NULL);
		ShowCursor(TRUE);
		CRect rcAdjust(0, 0, 800, 600);
		AdjustWindowRect(rcAdjust, g_WndTMap.GetStyle(), FALSE);
		g_WndTMap.SetWindowPos(NULL, 0, 0, rcAdjust.Width(), rcAdjust.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	g_WndTMap.ShowWindow(SW_SHOW);
	g_WndTMap.EnableToolTips(TRUE); // 툴팁 가능..
	g_WndTMap.SetFocus();
}

void CIGT4Dlg::OnBActivateToolAutoSpr() 
{
	CDlgSprAutoExtract dlg;
	dlg.DoModal();
}

void CIGT4Dlg::OnBActivateToolIntf() 
{
	if(g_WndIntf.GetSafeHwnd() == NULL)
	{
		// 스프라이트 윈도우를 만든다.. - 아직 보이지는 않고 툴바에서 호출해야만 보인다.
		HCURSOR hCur = LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_C_NORMAL));
		CString szClass;
		szClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, hCur, NULL, 0);
		
		g_WndIntf.CreateEx(	NULL,
							szClass,
							"Sprite, Animation Working Window",
							WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							800,
							600,
							this->m_hWnd,
							NULL,
							NULL);
		ShowCursor(TRUE);
		CRect rcAdjust(0, 0, 800, 600);
		AdjustWindowRect(rcAdjust, g_WndIntf.GetStyle(), FALSE);
		g_WndIntf.SetWindowPos(NULL, 0, 0, rcAdjust.Width(), rcAdjust.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	g_WndIntf.ShowWindow(SW_SHOW);
	g_WndIntf.EnableToolTips(TRUE); // 툴팁 가능..
	g_WndIntf.SetFocus();
}

void CIGT4Dlg::OnBWorkPathSet() 
{
	CDlgFolderSelect dlg;
	CString str = g_pTEng->PathGetCur(NULL);
	dlg.SetInitFolderName(str);
	if(dlg.DoModal() == IDCANCEL) return;

	CString FolderName = dlg.GetFolderName();
	g_pTEng->PathSetWork(FolderName);

	SetDlgItemText(IDC_E_WORK_PATH, g_pTEng->PathGetCur(NULL));
}

void CIGT4Dlg::OnBActivateToolScript() 
{
	if(g_DlgScript.GetSafeHwnd() == NULL)
	{
		g_DlgScript.Create(IDD_SCRIPT, this);
	}
	g_DlgScript.ShowWindow(SW_SHOW);
}

void CIGT4Dlg::OnBActivateToolConvertBmp() 
{
	CDlgBMPConvert dlg;
	dlg.DoModal();
}

// 스프라이트 합치기...
void CIGT4Dlg::OnBActivateToolSprMerge() 
{
	CDlgSprMerge dlg;
	dlg.DoModal();
}

void CIGT4Dlg::OnOK()
{

}

void CIGT4Dlg::OnCancel()
{
	CDialog::OnCancel();
}

void CIGT4Dlg::TmpProc(CString szFolder)
{
	CFileFind filefind;
	SetCurrentDirectory(szFolder);
	BOOL bFind = filefind.FindFile("*.*");
	while(bFind)
	{
		bFind = filefind.FindNextFile();
		CString szFN = filefind.GetFileName();
		CString szFN2 = szFN;
		if(szFN.Right(1) == '.' || szFN.Right(1) == "..") continue;

		if(filefind.IsDirectory())
		{
			this->TmpProc(filefind.GetFilePath()); // 재귀 호출
		}
		else
		{
			SetCurrentDirectory(szFolder);
			if(szFN2.Replace("_.", ".") > 0)
				int n = rename(szFN, szFN2);
		}
	}
}

void CIGT4Dlg::OnBActivateToolLookEdit() 
{
	if(g_DlgLook.GetSafeHwnd() == NULL) g_DlgLook.Create(IDD_LOOK, this);
	g_DlgLook.ShowWindow(SW_SHOW);
	g_DlgLook.SetFocus();
}

void CIGT4Dlg::OnBActivateToolItemEdit() 
{
	if(g_DlgItem.GetSafeHwnd() == NULL) g_DlgItem.Create(IDD_ITEM, this);
	g_DlgItem.ShowWindow(SW_SHOW);
	g_DlgItem.SetFocus();
}

void CIGT4Dlg::OnBActivateToolTestColorProcess() 
{
	if(g_DlgCBT.GetSafeHwnd() == NULL) g_DlgCBT.Create(IDD_COLOR_BLT_TEST, this);
	g_DlgCBT.ShowWindow(SW_SHOW);
	g_DlgCBT.SetFocus();
}

// 예전 에니메이션 데이터를 현재 포맷대로 바꾸어준다...
void CIGT4Dlg::OnBActivateToolConvertAni() 
{
	CDlgFolderSelect dlg; // 폴더 선택
	if(dlg.DoModal() == IDCANCEL) return;

	CString szFolder = dlg.GetFolderName();
	szFolder += '\\';
	this->ConvertOldAniToNewAni(szFolder);
}

// 예전 스프라이트 데이터를 현재 포맷대로 바꾸어준다...
void CIGT4Dlg::OnBActivateToolConvertSpr() 
{
	CDlgFolderSelect dlg; // 폴더 선택
	if(dlg.DoModal() == IDCANCEL) return;

	CString szFolder = dlg.GetFolderName();
	szFolder += '\\';
	this->ConvertOldSprToNewSpr(szFolder);
}

void CIGT4Dlg::OnBActivateToolResrc() 
{
	if(g_DlgResrc.GetSafeHwnd() == NULL) g_DlgResrc.Create(IDD_RESRC, this);
	g_DlgResrc.ShowWindow(SW_SHOW);
	g_DlgResrc.SetFocus();
}

void CIGT4Dlg::ConvertOldSprToNewSpr(const char *szFolder)
{
	CString FileName;
	CAniObjTool aniobj;

	SetCurrentDirectory(szFolder); // 폴더를 정하고...

	CFileFind finder; // 폴더에 있는 파일을 다 찾는다.
	BOOL bWorking = finder.FindFile("*.*");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		FileName = finder.GetFilePath();
		if(finder.IsDirectory() == TRUE && FileName.Right(2) != "\\." && FileName.Right(3) != "\\..")
		{
			this->ConvertOldSprToNewSpr(FileName); // 폴더는 빼고 딴다.. // 재귀 호출이다.. 조심하자.
		}
		else if(lstrcmpi(FileName.Right(6), "oldspl") == 0) // 확장자가 oldspl 일때만 컨버팅
		{
			aniobj.SprLoadOldFormat(FileName);
			FileName = FileName.Left(FileName.GetLength() - 6) + "spl";
			aniobj.SprSave(FileName);
		}
	}
}

void CIGT4Dlg::ConvertOldAniToNewAni(const char *szFolder)
{
	CString FileName;
	CAniObjTool aniobj;

	SetCurrentDirectory(szFolder); // 폴더를 정하고...

	CFileFind finder; // 폴더에 있는 파일을 다 찾는다.
	BOOL bWorking = finder.FindFile("*.*");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		FileName = finder.GetFilePath();
		if(finder.IsDirectory() == TRUE && FileName.Right(2) != "\\." && FileName.Right(3) != "\\..")
		{
			this->ConvertOldAniToNewAni(FileName); // 폴더는 빼고 딴다.. // 재귀 호출이다.. 조심하자.
		}
		else if(lstrcmpi(FileName.Right(6), "oldani") == 0) // 확장자가 oldani 일때만 컨버팅
		{
			aniobj.AniLoadOldFormat(FileName);
			FileName = FileName.Left(FileName.GetLength() - 6) + "ani";
			aniobj.AniSave(FileName);
		}
	}
}

