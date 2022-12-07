// DlgSetCenter.cpp : implementation file
//

#include "stdafx.h"
#include "..\\IGObject\\AniObjLSP.h"
#include "DlgSetCenter.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetCenter dialog

CDlgSetCenter::CDlgSetCenter(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetCenter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetCenter)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_ptCenter.x = 0;
	m_ptCenter.y = 0;
	m_lpdds = NULL;
}

CDlgSetCenter::~CDlgSetCenter()
{
	RELEASE_SAFE(m_lpdds);
}

void CDlgSetCenter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetCenter)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetCenter, CDialog)
	//{{AFX_MSG_MAP(CDlgSetCenter)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_VIEW_PICTURE, OnViewPicture)
	ON_EN_CHANGE(IDC_CENTER_X, OnChangeCenterX)
	ON_EN_CHANGE(IDC_CENTER_Y, OnChangeCenterY)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetCenter message handlers

void CDlgSetCenter::OnOK() 
{
	// TODO: Add extra validation here
	m_ptCenter.x = GetDlgItemInt(IDC_CENTER_X);
	m_ptCenter.y = GetDlgItemInt(IDC_CENTER_Y);

	CRect rc(0,0,0,0);
	if(m_lpdds)
	{
		g_pTEng->SurfaceGetSize(m_lpdds, &rc);
	}
	
	if(m_ptCenter.x <= 0 || m_ptCenter.x >= rc.right || m_ptCenter.y <= 0 || m_ptCenter.y >= rc.bottom)
	{
		int yesno = MessageBox("중심점이 잘못되어 있습니다. 다시 잡겠습니까?", "중심점 에러", MB_YESNO);
		if(yesno == IDYES) return;
	}
	CDialog::OnOK();
}

void CDlgSetCenter::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

CPoint CDlgSetCenter::GetCenter()
{
	return m_ptCenter;
}

BOOL CDlgSetCenter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	GetClientRect(&m_rcDefaultClient);
	
	char szBuff[128];
	GetCurrentDirectory(128, szBuff);
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "bmp", NULL, dwFlags, "비트맵 파일(*.bmp)|*.bmp||", NULL);
	dlg.m_ofn.lpstrInitialDir = szBuff;
	dlg.m_ofn.lpstrTitle = "센터잡기용 비트맵 파일 열기";
	if(dlg.DoModal() == IDCANCEL) OnCancel();

	CString FileName = dlg.GetPathName();
	RELEASE_SAFE(m_lpdds);
	m_lpdds = g_pTEng->SurfaceCreateWithBMP(FileName, FALSE, NULL);

	// 최근의 센터점을 가져온다...
	m_ptCenter.x = 0;
	m_ptCenter.y = 0;
	
	DWORD dwLength = 4;
	DWORD dwType = REG_DWORD;
	LONG success = 0;
	success = RegQueryValueEx(HKEY_CURRENT_USER, "SOFTWARE\\I'llSoft\\IGT4\\Recent Center X", 0, &dwType, (unsigned char*)&m_ptCenter.x, &dwLength);
	success = RegQueryValueEx(HKEY_CURRENT_USER, "SOFTWARE\\I'llSoft\\IGT4\\Recent Center Y", 0, &dwType, (unsigned char*)&m_ptCenter.y, &dwLength);

	SetDlgItemInt(IDC_CENTER_X, m_ptCenter.x);
	SetDlgItemInt(IDC_CENTER_Y, m_ptCenter.y);
	InvalidateRect(NULL, FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSetCenter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_ptCenter.x = point.x - m_rcDefaultClient.Width();
	m_ptCenter.y = point.y;

	SetDlgItemInt(IDC_CENTER_X, m_ptCenter.x);
	SetDlgItemInt(IDC_CENTER_Y, m_ptCenter.y);
	
	InvalidateRect(NULL, FALSE);

	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgSetCenter::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(nFlags == MK_LBUTTON)
	{
		m_ptCenter.x = point.x - m_rcDefaultClient.Width();
		m_ptCenter.y = point.y;

		SetDlgItemInt(IDC_CENTER_X, m_ptCenter.x);
		SetDlgItemInt(IDC_CENTER_Y, m_ptCenter.y);
	
		InvalidateRect(NULL, FALSE);
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgSetCenter::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
}

void CDlgSetCenter::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if(m_lpdds == NULL) return;
	
	CRect rc, rcClient;
	g_pTEng->SurfaceGetSize(m_lpdds, &rc);
	GetClientRect(&rcClient);
	
	int width, height;
	if(rc.Height() > m_rcDefaultClient.Height()) height = rc.Height();
	else height = m_rcDefaultClient.Height();

	width = m_rcDefaultClient.Width() + rc.Width();
	CRect rcNew(0, 0, width , height);
	if(rcClient != rcNew)
	{
		SetWindowPos(NULL, 0, 0,
			rcNew.Width() + GetSystemMetrics(SM_CXFRAME) * 2,
			rcNew.Height() + GetSystemMetrics(SM_CYFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION), SWP_NOMOVE);
	};

	HDC hdc = NULL;
	m_lpdds->GetDC(&hdc);
	CDC dc2;
	dc2.Attach(hdc);
	BOOL b = dc.BitBlt(m_rcDefaultClient.Width(), 0, rc.right, rc.bottom, &dc2, 0, 0, SRCCOPY);
	dc2.Detach();
	m_lpdds->ReleaseDC(hdc);

	COLORREF color = RGB(255, 255, 0);
	
	if(m_ptCenter.x > 0 && m_ptCenter.x < rc.right && m_ptCenter.y > 0 && m_ptCenter.y < rc.bottom)
	{
		int oldMode = dc.SetROP2(R2_NOT);
		for(int i = m_ptCenter.y - 10; i < m_ptCenter.y + 10; i++)
			dc.SetPixel(m_ptCenter.x + m_rcDefaultClient.Width(), i, color);
		for(i = m_ptCenter.x - 10; i < m_ptCenter.x + 10; i++)
			dc.SetPixel(i + m_rcDefaultClient.Width(), m_ptCenter.y, color);
		dc.SetROP2(oldMode);
	}
	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgSetCenter::OnViewPicture() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "bmp", NULL, dwFlags, "24bit 비트맵 파일(*.bmp)|*.bmp||", NULL);
	if( dlg.DoModal() == IDCANCEL ) OnCancel(); 

	CString FileName = dlg.GetPathName();
	RELEASE_SAFE(m_lpdds);
	m_lpdds = g_pTEng->SurfaceCreateWithBMP(FileName, FALSE, NULL);

	m_ptCenter.x = 0;
	m_ptCenter.y = 0;
	
	SetDlgItemInt(IDC_CENTER_X, m_ptCenter.x);
	SetDlgItemInt(IDC_CENTER_Y, m_ptCenter.y);
	
	InvalidateRect(NULL, FALSE);
}

void CDlgSetCenter::OnChangeCenterX() 
{
	m_ptCenter.x = GetDlgItemInt(IDC_CENTER_X);
	
	InvalidateRect(NULL, FALSE);
}

void CDlgSetCenter::OnChangeCenterY() 
{
	m_ptCenter.y = GetDlgItemInt(IDC_CENTER_Y);

	InvalidateRect(NULL, FALSE);
}


void CDlgSetCenter::OnDestroy() 
{
	CDialog::OnDestroy();
	
	DWORD dwLength = 4;
	DWORD dwType = REG_DWORD;
	LONG success = 0;
	success = RegSetValueEx(HKEY_CURRENT_USER, "SOFTWARE\\I'llSoft\\IGT4\\Recent Center X", 0, dwType, (unsigned char*)&m_ptCenter.x, dwLength);
	success = RegSetValueEx(HKEY_CURRENT_USER, "SOFTWARE\\I'llSoft\\IGT4\\Recent Center Y", 0, dwType, (unsigned char*)&m_ptCenter.y, dwLength);
}
