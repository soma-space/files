// DlgHouseInfo.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgHouseInfo.h"
#include "WndTileMap.h"
#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgHouseInfo dialog


CDlgHouseInfo::CDlgHouseInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHouseInfo::IDD, pParent)
{
	lstrcpy(m_Header.szID, "HPF");
	ZeroMemory(m_Header.szRemark, 64);
	m_Header.nQt = MAX_HOUSE_DATA;
	MAKE_ZERO(m_Datas); // 집정보 데이터
	m_vSizeX = m_vSizeY = 8;

	//{{AFX_DATA_INIT(CDlgHouseInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgHouseInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHouseInfo)
	DDX_Control(pDX, IDC_LIST_HOUSE, m_ListHouse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgHouseInfo, CDialog)
	//{{AFX_MSG_MAP(CDlgHouseInfo)
	ON_LBN_SELCHANGE(IDC_LIST_HOUSE, OnSelchangeListHouse)
	ON_EN_CHANGE(IDC_E_HOUSE_NUMBER, OnChangeEHouseNumber)
	ON_BN_CLICKED(IDC_B_NEW, OnBNew)
	ON_BN_CLICKED(IDC_B_LOAD, OnBLoad)
	ON_BN_CLICKED(IDC_B_SAVE, OnBSave)
	ON_BN_CLICKED(IDC_B_SAVE_AS, OnBSaveAs)
	ON_BN_CLICKED(IDC_B_RGN_INIT, OnBRgnInit)
	ON_BN_CLICKED(IDC_GO_POS, OnGoPos)
	ON_EN_CHANGE(IDC_SIZE_X, OnChangeSizeX)
	ON_EN_CHANGE(IDC_SIZE_Y, OnChangeSizeY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHouseInfo message handlers

void CDlgHouseInfo::OnSelchangeListHouse() 
{
	int index = m_ListHouse.GetCurSel();
	if(index < 0 || index >= MAX_HOUSE_DATA) return;

	SetDlgItemInt(IDC_E_HOUSE_NUMBER, m_Datas[index].nNum); // 집번호 표시
}

void CDlgHouseInfo::OnChangeEHouseNumber() 
{
	int index = m_ListHouse.GetCurSel();
	if(index < 0 || index >= MAX_HOUSE_DATA) return;

	int nHouseNumber = GetDlgItemInt(IDC_E_HOUSE_NUMBER);
	m_Datas[index].nNum = nHouseNumber; // 집번호 바꾸기

	// 작업 윈도우 갱신.
	CWnd* pWnd = this->GetOwner();
	if(pWnd) pWnd->InvalidateRect(NULL, FALSE);
}

BOOL CDlgHouseInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ListHouse.ResetContent();
	CString num;
	for(int i = 0; i < MAX_HOUSE_DATA; i++)
	{
		num.Format("%d", i);
		m_ListHouse.AddString(num);
	}

	SetDlgItemInt(IDC_SIZE_X, m_vSizeX, FALSE);
	SetDlgItemInt(IDC_SIZE_Y, m_vSizeY, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgHouseInfo::OnBNew() 
{
	SetDlgItemText(IDC_E_FILE_NAME_TILE_SET, "");

	lstrcpy(m_Header.szID, "HPF");
	ZeroMemory(m_Header.szRemark, 64);
	m_Header.nQt = MAX_HOUSE_DATA;
	MAKE_ZERO(m_Datas); // 집정보 데이터

	CWnd* pWnd = this->GetOwner();
	if(pWnd) pWnd->Invalidate(FALSE);
}

void CDlgHouseInfo::OnBLoad() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "hpf", NULL, dwFlags, "집터 데이터(*.hpf)|*.hpf||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	SetDlgItemText(IDC_E_HOUSE_LIST_FILE_NAME, FileName);

	int i;
	CFile file;
	int file_size;
	char t_id[4];
	char t_remark[64];
	int t_qt;

	if(!file.Open(FileName, CFile::modeRead)) return;

	file_size = file.GetLength();
	if(file_size < 4+64+4+sizeof(_HOUSE_DATA))
	{
		file.Close();
		AfxMessageBox("HPF 포맷과 맞지 않습니다.", MB_OK|MB_ICONSTOP);
		return;
	}

	file.Read(t_id, 4);
	file.Read(t_remark, 64);
	file.Read(&t_qt, 4);

	if(lstrcmp(m_Header.szID, "HPF") || m_Header.nQt != MAX_HOUSE_DATA)
	{
		file.Close();
		AfxMessageBox("HPF 포맷과 맞지 않습니다.", MB_OK|MB_ICONSTOP);
		return;
	}

	for(i = 0; i < MAX_HOUSE_DATA; i++)
	{
		file.Read(&m_Datas[i].rcRgn, 16);
		file.Read(&m_Datas[i].nNum, 4);
	}

	file.Close();

	CWnd* pWnd = this->GetOwner();
	if(pWnd) pWnd->Invalidate(FALSE);
}

void CDlgHouseInfo::OnBSave() 
{
	CString FileName;
	GetDlgItemText(IDC_E_HOUSE_LIST_FILE_NAME, FileName);
	if(!FileName.GetLength())
	{
		OnBSaveAs();
		return;
	}

	g_pTEng->PathSetCur("MAP");

	int i;
	CFile file;

	file.Open(FileName, CFile::modeWrite | CFile::modeCreate);

	file.Write(m_Header.szID, 4);
	file.Write(m_Header.szRemark, 64);
	file.Write(&m_Header.nQt, 4);

	for(i = 0; i < MAX_HOUSE_DATA; i++)
	{
		file.Write(&m_Datas[i].rcRgn, 16);
		file.Write(&m_Datas[i].nNum, 4);
	}

	file.Close();
}

void CDlgHouseInfo::OnBSaveAs() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "hpf", NULL, dwFlags, "집터 데이터(*.hpf)|*.hpf||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	SetDlgItemText(IDC_E_HOUSE_LIST_FILE_NAME, FileName);

	OnBSave();
}

// 영역을 초기화 해준다.
void CDlgHouseInfo::OnBRgnInit() 
{
	int index = m_ListHouse.GetCurSel();
	if(index < 0 || index >= MAX_HOUSE_DATA) return;

	SetRect(&m_Datas[index].rcRgn, 0, 0, 0, 0); // 영역 초기화

	// 작업 윈도우 갱신.
	CWnd* pWnd = this->GetOwner();
	if(pWnd) pWnd->InvalidateRect(NULL, FALSE);
}

void CDlgHouseInfo::OnGoPos() 
{
	int index = m_ListHouse.GetCurSel();
	if(index < 0 || index >= MAX_HOUSE_DATA) return;

	CWndTileMap *parent = (CWndTileMap *)this->GetOwner();
	if(parent)
	{
		parent->GotoHouse(index);
	}
}

void CDlgHouseInfo::OnChangeSizeX() 
{
	m_vSizeX = GetDlgItemInt(IDC_SIZE_X, NULL, FALSE);
	if(m_vSizeX == 0) m_vSizeX = 1;

	CWnd* pWnd = this->GetOwner();
	if(pWnd) pWnd->Invalidate(FALSE);
}

void CDlgHouseInfo::OnChangeSizeY() 
{
	m_vSizeY = GetDlgItemInt(IDC_SIZE_Y, NULL, FALSE);
	if(m_vSizeY == 0) m_vSizeY = 1;

	CWnd* pWnd = this->GetOwner();
	if(pWnd) pWnd->Invalidate(FALSE);
}
