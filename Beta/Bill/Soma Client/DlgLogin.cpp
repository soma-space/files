// DlgLogin.cpp : implementation file
//

#include "stdafx.h"
#include "soma.h"
#include "DlgLogin.h"
#include "SMProtocol.h"
#include "ChildView.h"
#include "SocketMng.h"
#include "Msg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLogin dialog


CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogin::IDD, pParent)
{
	m_bOk = FALSE;
	//{{AFX_DATA_INIT(CDlgLogin)
	m_szIP = _T("192.203.141.20");
//	m_szIP = _T("192.203.141.142");
	m_szID = _T("soma");
	m_szPasswd = _T("soma");
	m_bWindowMode = TRUE;
	//}}AFX_DATA_INIT
}


void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLogin)
	DDX_Text(pDX, IDC_EDIT1, m_szIP);
	DDV_MaxChars(pDX, m_szIP, 25);//15
	DDX_Text(pDX, IDC_EDIT2, m_szID);
	DDV_MaxChars(pDX, m_szID, 12);
	DDX_Text(pDX, IDC_EDIT3, m_szPasswd);
	DDV_MaxChars(pDX, m_szPasswd, 12);
	DDX_Check(pDX, IDC_CHECK1, m_bWindowMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	//{{AFX_MSG_MAP(CDlgLogin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLogin message handlers

void CDlgLogin::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	GetDlgItem(IDOK)->EnableWindow(FALSE);

/*	CChildView::SocketMng.m_szDestIp = m_szIP;
	CChildView::SocketMng.m_iPort = 4110;
	CChildView::SocketMng.m_szLoginId = m_szID;
	CChildView::SocketMng.m_szPasswd = m_szPasswd;
	if (CChildView::SocketMng.ConnectSocket()) m_bOk = TRUE;
	else 
	{
		CDialog::OnOK();
		return;
	}

	//Session에 초기 패킷을 보냄
	CMsg cmsg;
	cmsg.ID(SM_GAMEINFO_REQ);
	CChildView::SocketMng.SendRawPacket(cmsg.GetSize(), (PBYTE)cmsg.GetBuf());

	MSG msg;
	while (TRUE==CChildView::PacketParser.GetSessionLoginStatus() && GetSafeHwnd()) //Session과 통신이 끝날때까지 빠져나가지 않음 (Cancel버튼 사용가능)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message==WM_QUIT)
			{
				m_bOk = FALSE;
				break;
			}
		}
	}
*/
	m_bOk = TRUE;
	if (GetSafeHwnd()) CDialog::OnOK();
}

void CDlgLogin::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();	//서버와 연결하기 위해 대기하다 지침... 포기할 경우
	AfxPostQuitMessage(0);
}
