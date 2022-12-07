// DlgResrc.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgResrc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "GlobalVar.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgResrc dialog


CDlgResrc::CDlgResrc(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgResrc::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgResrc)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgResrc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgResrc)
	DDX_Control(pDX, IDC_LIST_MAP, m_ListMap);
	DDX_Control(pDX, IDC_LIST_AVATA0, m_ListAvata0);
	DDX_Control(pDX, IDC_LIST_AVATA1, m_ListAvata1);
	DDX_Control(pDX, IDC_LIST_NPC, m_ListNPC);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgResrc, CDialog)
	//{{AFX_MSG_MAP(CDlgResrc)
	ON_BN_CLICKED(IDC_B_MAP_BROWSE, OnBMapBrowse)
	ON_BN_CLICKED(IDC_B_MAP_DELETE, OnBMapDelete)
	ON_BN_CLICKED(IDC_B_AVATA0_BROWSE, OnBAvata0Browse)
	ON_BN_CLICKED(IDC_B_AVATA0_DELETE, OnBAvata0Delete)
	ON_BN_CLICKED(IDC_B_AVATA1_BROWSE, OnBAvata1Browse)
	ON_BN_CLICKED(IDC_B_AVATA1_DELETE, OnBAvata1Delete)

	ON_BN_CLICKED(IDC_B_NPC_ANI_BROWSE, OnBNPCAniBrowse)
	ON_BN_CLICKED(IDC_B_NPC_ANI_DELETE, OnBNPCAniDelete)
	ON_BN_CLICKED(IDC_B_NPC_SPR_BROWSE, OnBNPCSprBrowse)
	ON_BN_CLICKED(IDC_B_NPC_SPR_DELETE, OnBNPCSprDelete)
	ON_BN_CLICKED(IDC_B_NPC_SHADOW_BROWSE, OnBNPCShadowBrowse)
	ON_BN_CLICKED(IDC_B_NPC_SHADOW_DELETE, OnBNPCShadowDelete)

	ON_BN_CLICKED(IDC_B_NEW, OnBNew)
	ON_BN_CLICKED(IDC_B_LOAD, OnBLoad)
	ON_BN_CLICKED(IDC_B_SAVE, OnBSave)
	ON_BN_CLICKED(IDC_B_SAVE_AS, OnBSaveAs)
	ON_LBN_SELCHANGE(IDC_LIST_MAP, OnSelchangeListMap)
	ON_LBN_SELCHANGE(IDC_LIST_AVATA0, OnSelchangeListAvata0)
	ON_LBN_SELCHANGE(IDC_LIST_AVATA1, OnSelchangeListAvata1)
	ON_LBN_SELCHANGE(IDC_LIST_NPC, OnSelchangeListNpc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgResrc message handlers

BOOL CDlgResrc::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	this->UpdateInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgResrc::OnBMapBrowse() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgResrc::OnBMapDelete() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgResrc::OnBAvata0Browse() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgResrc::OnBAvata0Delete() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgResrc::OnBAvata1Browse() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgResrc::OnBAvata1Delete() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgResrc::OnBNPCAniBrowse() 
{
	int nIndex = m_ListNPC.GetCurSel();
	if(nIndex < 0 || nIndex >= MAX_NPC_RESRC) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "ani", NULL, dwFlags, "에니메이션(*.ani)|*.ani||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("ANI");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();
	m_Resrc.FileNameSet(3, nIndex, FileName);

	this->UpdateInfoNPC(nIndex);
}

void CDlgResrc::OnBNPCAniDelete() 
{
	int nIndex = m_ListNPC.GetCurSel();
	if(nIndex < 0 || nIndex >= MAX_NPC_RESRC) return;

	m_Resrc.FileNameSet(3, nIndex, "");
	this->UpdateInfoNPC(nIndex);
}

void CDlgResrc::OnBNPCSprBrowse() 
{
	int nIndex = m_ListNPC.GetCurSel();
	if(nIndex < 0 || nIndex >= MAX_NPC_RESRC) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "LSP 스프라이트(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();
	m_Resrc.FileNameSet(4, nIndex, FileName);

	this->UpdateInfoNPC(nIndex);
}

void CDlgResrc::OnBNPCSprDelete() 
{
	int nIndex = m_ListNPC.GetCurSel();
	if(nIndex < 0 || nIndex >= MAX_NPC_RESRC) return;

	m_Resrc.FileNameSet(4, nIndex, "");
	this->UpdateInfoNPC(nIndex);
}

void CDlgResrc::OnBNPCShadowBrowse() 
{
	int nIndex = m_ListNPC.GetCurSel();
	if(nIndex < 0 || nIndex >= MAX_NPC_RESRC) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "LSP 스프라이트(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();
	m_Resrc.FileNameSet(5, nIndex, FileName);

	this->UpdateInfoNPC(nIndex);
}

void CDlgResrc::OnBNPCShadowDelete() 
{
	int nIndex = m_ListNPC.GetCurSel();
	if(nIndex < 0 || nIndex >= MAX_NPC_RESRC) return;

	m_Resrc.FileNameSet(5, nIndex, "");
	this->UpdateInfoNPC(nIndex);
}

void CDlgResrc::OnBNew() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgResrc::OnBLoad() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgResrc::OnBSave() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgResrc::OnBSaveAs() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgResrc::UpdateInfo()
{
	CString szFN, szFN2, szTxt;
	int i = 0;

	m_ListMap.ResetContent();
	for(i = 0; i < MAX_MAP_RESRC; i++)
	{
		szFN = m_Resrc.FileNameGet(0, i);
		if(szFN.GetLength() == 0) szFN = "지정 안됨";
		m_ListMap.AddString(szFN);
	}
	m_ListMap.SetCurSel(0);

	m_ListAvata0.ResetContent();
	for(i = 0; i < MAX_AVATA_RESRC; i++)
	{
		szFN = m_Resrc.FileNameGet(1, i);
		if(szFN.GetLength() == 0) szFN = "지정 안됨";

		if(i == 0) szTxt = "에니메이션 : " + szFN;
		else szTxt = "스프라이트 : " + szFN;
		m_ListAvata0.AddString(szTxt);
	}
	m_ListAvata0.SetCurSel(0);

	m_ListAvata1.ResetContent();
	for(i = 0; i < MAX_AVATA_RESRC; i++)
	{
		szFN = m_Resrc.FileNameGet(2, i);
		if(szFN.GetLength() == 0) szFN = "지정 안됨";

		if(i == 0) szTxt = "에니메이션 : " + szFN;
		else szTxt = "스프라이트 : " + szFN;
		m_ListAvata1.AddString(szTxt);
	}
	m_ListAvata1.SetCurSel(0);

	m_ListNPC.ResetContent();
	for(i = 0; i < MAX_NPC_RESRC; i++)
	{
		szFN = m_Resrc.FileNameGet(3, i);
		szFN2 = m_Resrc.FileNameGet(4, i);
		if(szFN.GetLength() == 0) szFN = "지정 안됨";
		if(szFN2.GetLength() == 0) szFN2 = "지정 안됨";

		szTxt = "에니메이션 : " + szFN + ", 스프라이트 : " + szFN;
		m_ListNPC.AddString(szTxt);
	}
	m_ListNPC.SetCurSel(0);

}

void CDlgResrc::OnSelchangeListMap() 
{
	int nIndex = m_ListMap.GetCurSel();
	SetDlgItemInt(IDC_E_MAP_NUMBER, nIndex);
}

void CDlgResrc::OnSelchangeListAvata0() 
{
	int nIndex = m_ListAvata0.GetCurSel();
	SetDlgItemInt(IDC_E_AVATA0_NUMBER, nIndex);
}

void CDlgResrc::OnSelchangeListAvata1() 
{
	int nIndex = m_ListAvata1.GetCurSel();
	SetDlgItemInt(IDC_E_AVATA1_NUMBER, nIndex);
}

void CDlgResrc::OnSelchangeListNpc() 
{
	int nIndex = m_ListNPC.GetCurSel();
	SetDlgItemInt(IDC_E_NPC_NUMBER, nIndex);
}

void CDlgResrc::UpdateInfoNPC(int nIndex)
{
	CString szFN1 = m_Resrc.FileNameGet(3, nIndex);
	CString szFN2 = m_Resrc.FileNameGet(4, nIndex);
	CString szFN3 = m_Resrc.FileNameGet(5, nIndex);
	if(szFN1.GetLength() == 0) szFN1 = "지정 안됨";
	if(szFN2.GetLength() == 0) szFN2 = "지정 안됨";
	if(szFN3.GetLength() == 0) szFN3 = "지정 안됨";

	CString szTxt = "에니메이션 : " + szFN1 + ", 스프라이트 : " + szFN2 + ", 그림자 : " + szFN3;
	m_ListNPC.DeleteString(nIndex);
	m_ListNPC.InsertString(nIndex,szTxt);
	m_ListNPC.SetCurSel(nIndex);
}

void CDlgResrc::UpdateInfoAvata0(int nIndex)
{
	
}

void CDlgResrc::UpdateInfoAvata1(int nIndex)
{

}
