// DlgTileMapObj.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgTileMapObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "GlobalVar.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgTileMapObj dialog

CDlgTileMapObj::CDlgTileMapObj(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTileMapObj::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTileMapObj)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgTileMapObj::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTileMapObj)
	DDX_Control(pDX, IDC_LIST_OBJ_SUB_SET, m_ListObjSubSet);
	DDX_Control(pDX, IDC_LIST_OBJ_SET, m_ListObjSet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTileMapObj, CDialog)
	//{{AFX_MSG_MAP(CDlgTileMapObj)
	ON_BN_CLICKED(IDC_B_SET_SIZE, OnBSetSize)
	ON_BN_CLICKED(IDC_B_ANI_FILE_NAME_SET, OnBAniFileNameSet)
	ON_BN_CLICKED(IDC_B_ANI_FILE_NAME_DELETE, OnBAniFileNameDelete)
	ON_BN_CLICKED(IDC_B_SPR_FILE_NAME_SET, OnBSprFileNameSet)
	ON_BN_CLICKED(IDC_B_SPR_FILE_NAME_DELETE, OnBSprFileNameDelete)
	ON_BN_CLICKED(IDC_B_NEW, OnBNew)
	ON_BN_CLICKED(IDC_B_LOAD, OnBLoad)
	ON_BN_CLICKED(IDC_B_SAVE, OnBSave)
	ON_BN_CLICKED(IDC_B_SAVE_AS, OnBSaveAs)
	ON_LBN_SELCHANGE(IDC_LIST_OBJ_SET, OnSelchangeListObjSet)
	ON_LBN_SELCHANGE(IDC_LIST_OBJ_SUB_SET, OnSelchangeListObjSubSet)
	ON_BN_CLICKED(IDC_B_SET_SIZE, OnBSetSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTileMapObj message handlers

void CDlgTileMapObj::OnBAniFileNameSet() 
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "ani", NULL, dwFlags, "에니메이션(*.ani)|*.ani||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("ANI");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	lstrcpy(m_MapObj.m_Header.szRegObjAniFNs[index], FileName); // 헤더에 파일 이름 넣어주고..
	m_MapObj.m_Objs[index].AniLoad(FileName); // 에니메이션 로딩


	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, sz);
	m_ListObjSet.SetCurSel(index);
	this->UpdateObjSubSet(); // 서브 리스트 업데이트

	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnBAniFileNameDelete() 
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	m_MapObj.m_Objs[index].AniRelease();

	lstrcpy(m_MapObj.m_Header.szRegObjAniFNs[index], ""); // 헤더에 파일 이름 넣어주고..

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, "");
	m_ListObjSet.SetCurSel(index);
	this->UpdateObjSubSet(); // 서브 리스트 업데이트

	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnBSprFileNameSet() 
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "단순 LSP 스프라이트(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	lstrcpy(m_MapObj.m_Header.szRegObjSprFNs[index], FileName); // 헤더에 파일 이름 넣어주고..
	m_MapObj.m_Objs[index].SprLoad(FileName, g_pTEng->PixelFormatGet()); // 에니메이션 로딩

	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, sz);
	m_ListObjSet.SetCurSel(index);
	this->UpdateObjSubSet(); // 서브 리스트 업데이트

	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnBSprFileNameDelete() 
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	m_MapObj.m_Objs[index].SprRelease();

	lstrcpy(m_MapObj.m_Header.szRegObjSprFNs[index], ""); // 헤더에 파일 이름 넣어주고..

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, "");
	m_ListObjSet.SetCurSel(index);
	this->UpdateObjSubSet(); // 서브 리스트 업데이트

	this->UpdateWorkWindow();
}

void CDlgTileMapObj::UpdateWorkWindow()
{
	CWnd* pWnd = this->GetOwner();
	if(pWnd)
		pWnd->InvalidateRect(NULL, FALSE);
}

void CDlgTileMapObj::OnBNew() 
{
	m_MapObj.ReleaseEx();
	
	this->UpdateObjInfo();

	SetDlgItemText(IDC_E_FILE_NAME, "");
}

void CDlgTileMapObj::OnBLoad() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "mod", NULL, dwFlags, "Map, Object 배치 데이터(*.mod)|*.mod||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_MapObj.ReleaseEx(); // 해제
	m_MapObj.Load(FileName); // 부르고
	for(int i = 0; i < MAX_MAP_OBJ_REG; i++) // 객체들 로딩.
	{
		g_pTEng->PathSetCur("ANI");
		m_MapObj.m_Objs[i].AniLoad(m_MapObj.m_Header.szRegObjAniFNs[i]);
		g_pTEng->PathSetCur("SPR");
		m_MapObj.m_Objs[i].SprLoad(m_MapObj.m_Header.szRegObjSprFNs[i], g_pTEng->PixelFormatGet());
	}

	this->UpdateObjInfo(); // 대화 상자 업데이트

	SetDlgItemInt(IDC_E_WIDTH, m_MapObj.m_Header.nWidth);
	SetDlgItemInt(IDC_E_HEIGHT, m_MapObj.m_Header.nHeight);
	SetDlgItemText(IDC_E_FILE_NAME, FileName);
	
	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnBSave() 
{
	CString FileName;
	GetDlgItemText(IDC_E_FILE_NAME, FileName);

	if(FileName.GetLength() <= 0) 
	{
		this->OnBSaveAs();
		return;
	}
	
	g_pTEng->PathSetCur("MAP");
	m_MapObj.Save(FileName);
}

void CDlgTileMapObj::OnBSaveAs() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "mod", NULL, dwFlags, "Map Object Data(*.mod)|*.mod||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	SetDlgItemText(IDC_E_FILE_NAME, FileName);

	this->OnBSave();
}

void CDlgTileMapObj::OnBSetSize() 
{
	int nW = GetDlgItemInt(IDC_E_WIDTH);
	int nH = GetDlgItemInt(IDC_E_HEIGHT);

	m_MapObj.Realloc(nW, nH); // 오브젝트 맵 다시 할당.
	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnSelchangeListObjSet() 
{
	this->UpdateObjSubSet();
	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnSelchangeListObjSubSet() 
{
	// TODO: Add your control notification handler code here
	
}

void CDlgTileMapObj::UpdateObjInfo()
{
	m_ListObjSet.ResetContent();
	for(int i = 0; i < MAX_MAP_OBJ_REG; i++)
	{
		CString sz = m_MapObj.m_Header.szRegObjAniFNs[i];
		sz += " + ";
		sz += m_MapObj.m_Header.szRegObjSprFNs[i];
		m_ListObjSet.AddString(sz);
	}
	m_ListObjSet.SetCurSel(0);

	this->UpdateObjSubSet(); // 서브셋 정보 나타내기.

	SetDlgItemInt(IDC_E_WIDTH, m_MapObj.m_Header.nWidth);
	SetDlgItemInt(IDC_E_HEIGHT, m_MapObj.m_Header.nWidth);
}

void CDlgTileMapObj::UpdateObjSubSet()
{
	m_ListObjSubSet.ResetContent();

	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	int nAniQt = m_MapObj.m_Objs[index].m_HAni.nQt;

	for(int i = 0; i < nAniQt; i++)
	{
		_ANI_DATA* pData = m_MapObj.m_Objs[index].AniDataGet(i);
		if(pData) m_ListObjSubSet.AddString(pData->szName);
	}
	m_ListObjSubSet.SetCurSel(0);
}


BOOL CDlgTileMapObj::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	this->UpdateObjInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

short int CDlgTileMapObj::GetSelectedObjData()
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return -1;
	
	int index2 = m_ListObjSubSet.GetCurSel();
	if(index2 < 0 || index2 >= MAX_ANI) return -1;

	return ((index<<8)|index2); // 상위 바이트는 세트 번호, 하위 바이트는 서브셋 번호
}
