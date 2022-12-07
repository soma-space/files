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

// Ver.100 (2000/12/21) : 집 Object 추가
#define CURRENT_VER		100
/////////////////////////////////////////////////////////////////////////////
// CDlgTileMapObj dialog

CDlgTileMapObj::CDlgTileMapObj(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTileMapObj::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTileMapObj)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_vCurrentVersion = CURRENT_VER;
	m_bShowInnerHouse = FALSE;
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
	ON_BN_CLICKED(IDC_B_SPR_FILE_NAME_SET2, OnBSprFileNameSet2)
	ON_BN_CLICKED(IDC_B_SPR_FILE_NAME_DELETE2, OnBSprFileNameDelete2)
	ON_BN_CLICKED(IDC_B_SPR_FILE_NAME_SET3, OnBSprFileNameSet3)
	ON_BN_CLICKED(IDC_B_SPR_FILE_NAME_SET4, OnBSprFileNameSet4)
	ON_BN_CLICKED(IDC_B_SPR_FILE_NAME_DELETE3, OnBSprFileNameDelete3)
	ON_BN_CLICKED(IDC_B_SPR_FILE_NAME_DELETE4, OnBSprFileNameDelete4)
	ON_BN_CLICKED(IDC_CHECK_SHOW_INNERHOUSE, OnCheckShowInnerhouse)
	ON_BN_CLICKED(IDC_C_OBJ_PRE_VIEW, OnCObjPreView)
	ON_WM_DESTROY()
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
	lstrcpy(m_ShadowObj.m_Header.szRegObjAniFNs[index], FileName); // 헤더에 파일 이름 넣어주고..
	m_ShadowObj.m_Objs[index].AniLoad(FileName); // 에니메이션 로딩
	lstrcpy(m_HOutObj.m_Header.szRegObjAniFNs[index], FileName); // 헤더에 파일 이름 넣어주고..
	m_HOutObj.m_Objs[index].AniLoad(FileName); // 에니메이션 로딩
	lstrcpy(m_HInObj.m_Header.szRegObjAniFNs[index], FileName); // 헤더에 파일 이름 넣어주고..
	m_HInObj.m_Objs[index].AniLoad(FileName); // 에니메이션 로딩

	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_ShadowObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HOutObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HInObj.m_Header.szRegObjSprFNs[index];

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
	m_ShadowObj.m_Objs[index].AniRelease();
	lstrcpy(m_ShadowObj.m_Header.szRegObjAniFNs[index], ""); // 헤더에 파일 이름 넣어주고..
	m_HOutObj.m_Objs[index].AniRelease();
	lstrcpy(m_HOutObj.m_Header.szRegObjAniFNs[index], ""); // 헤더에 파일 이름 넣어주고..
	m_HInObj.m_Objs[index].AniRelease();
	lstrcpy(m_HInObj.m_Header.szRegObjAniFNs[index], ""); // 헤더에 파일 이름 넣어주고..

	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_ShadowObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HOutObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HInObj.m_Header.szRegObjSprFNs[index];

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
	sz += " + ";
	sz += m_ShadowObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HOutObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HInObj.m_Header.szRegObjSprFNs[index];

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

	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_ShadowObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HOutObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HInObj.m_Header.szRegObjSprFNs[index];

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, sz);
	m_ListObjSet.SetCurSel(index);
	this->UpdateObjSubSet(); // 서브 리스트 업데이트

	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnBSprFileNameSet2() 
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "단순 LSP 스프라이트(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	lstrcpy(m_ShadowObj.m_Header.szRegObjSprFNs[index], FileName); // 헤더에 파일 이름 넣어주고..
	m_ShadowObj.m_Objs[index].SprLoad(FileName, g_pTEng->PixelFormatGet()); // 에니메이션 로딩

	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_ShadowObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HOutObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HInObj.m_Header.szRegObjSprFNs[index];

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, sz);
	m_ListObjSet.SetCurSel(index);
	this->UpdateObjSubSet(); // 서브 리스트 업데이트

	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnBSprFileNameDelete2() 
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	m_ShadowObj.m_Objs[index].SprRelease();

	lstrcpy(m_ShadowObj.m_Header.szRegObjSprFNs[index], ""); // 헤더에 파일 이름 넣어주고..

	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_ShadowObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HOutObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HInObj.m_Header.szRegObjSprFNs[index];

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, sz);
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
	m_ShadowObj.ReleaseEx();
	m_HOutObj.ReleaseEx();
	m_HInObj.ReleaseEx();
	
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

	HANDLE hFile;
	hFile = CreateFile(FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
		OutputDebugString("타일맵 데이터 파일 열기 실패 : ");
		OutputDebugString(FileName);
		OutputDebugString("\n");
		return;
	}

	DWORD dwAccessed = 0;
	int version;

	ReadFile(hFile, &version, sizeof(int), &dwAccessed, NULL);

	m_MapObj.ReleaseEx(); // 해제
	m_MapObj.Load(hFile); // 부르고
	for(int i = 0; i < MAX_MAP_OBJ_REG; i++) // 객체들 로딩.
	{
		g_pTEng->PathSetCur("ANI");
		m_MapObj.m_Objs[i].AniLoad(m_MapObj.m_Header.szRegObjAniFNs[i]);
		g_pTEng->PathSetCur("SPR");
		m_MapObj.m_Objs[i].SprLoad(m_MapObj.m_Header.szRegObjSprFNs[i], g_pTEng->PixelFormatGet());
	}
	if(m_MapObj.m_pTData)
	{
		for(i = 0; i < m_MapObj.m_vTCount; i++)
		{
			if(m_MapObj.m_pTData[i].value < 0) continue;
			
			int nSet = m_MapObj.m_pTData[i].value >> 8; // 오브젝트 세트
			if(nSet < 0 || nSet >= MAX_MAP_OBJ_REG) continue;
			int nSubSet = m_MapObj.m_pTData[i].value & 0xff; // 오브젝트 서브 세트
			if(nSubSet < 0) continue;
			
			CAniObjLSP* pAniObj = &m_MapObj.m_Objs[nSet];
			const _SPR_LSP* pSpr = NULL;
			int nSpr = pAniObj->FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
			pAniObj->SprGet(nSpr, &pSpr);
			if(pSpr == NULL)
			{
				m_MapObj.m_pnData[m_MapObj.m_pTData[i].index] = -1;
				TRACE("M(%d,%d) : %d,%d\n", m_MapObj.m_pTData[i].index%m_MapObj.m_Header.nWidth,
					m_MapObj.m_pTData[i].index/m_MapObj.m_Header.nWidth, nSet, nSubSet);
			}
		}
		delete[] m_MapObj.m_pTData;
		m_MapObj.m_pTData = NULL;
	}
	m_ShadowObj.ReleaseEx(); // 해제
	m_ShadowObj.Load(hFile); // 부르고
	for(i = 0; i < MAX_MAP_OBJ_REG; i++) // 객체들 로딩.
	{
		g_pTEng->PathSetCur("ANI");
		m_ShadowObj.m_Objs[i].AniLoad(m_ShadowObj.m_Header.szRegObjAniFNs[i]);
		g_pTEng->PathSetCur("SPR");
		m_ShadowObj.m_Objs[i].SprLoad(m_ShadowObj.m_Header.szRegObjSprFNs[i], g_pTEng->PixelFormatGet());
	}
	if(m_ShadowObj.m_pTData)
	{
		for(i = 0; i < m_ShadowObj.m_vTCount; i++)
		{
			if(m_ShadowObj.m_pTData[i].value < 0) continue;
			
			int nSet = m_ShadowObj.m_pTData[i].value >> 8; // 오브젝트 세트
			if(nSet < 0 || nSet >= MAX_MAP_OBJ_REG) continue;
			int nSubSet = m_ShadowObj.m_pTData[i].value & 0xff; // 오브젝트 서브 세트
			if(nSubSet < 0) continue;
			
			CAniObjLSP* pAniObj = &m_ShadowObj.m_Objs[nSet];
			const _SPR_LSP* pSpr = NULL;
			int nSpr = pAniObj->FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
			pAniObj->SprGet(nSpr, &pSpr);
			if(pSpr == NULL)
			{
				m_ShadowObj.m_pnData[m_ShadowObj.m_pTData[i].index] = -1;
				TRACE("S(%d,%d) : %d,%d\n", m_ShadowObj.m_pTData[i].index%m_ShadowObj.m_Header.nWidth,
					m_ShadowObj.m_pTData[i].index/m_ShadowObj.m_Header.nWidth, nSet, nSubSet);
			}
		}
		delete[] m_ShadowObj.m_pTData;
		m_ShadowObj.m_pTData = NULL;
	}
//	m_HOutObj.Realloc(m_MapObj.m_Header.nWidth, m_MapObj.m_Header.nHeight);
//	m_HInObj.Realloc(m_MapObj.m_Header.nWidth, m_MapObj.m_Header.nHeight);
	m_HOutObj.ReleaseEx(); // 해제
	m_HOutObj.Load(hFile); // 부르고
	for(i = 0; i < MAX_MAP_OBJ_REG; i++) // 객체들 로딩.
	{
		g_pTEng->PathSetCur("ANI");
		m_HOutObj.m_Objs[i].AniLoad(m_HOutObj.m_Header.szRegObjAniFNs[i]);
		g_pTEng->PathSetCur("SPR");
		m_HOutObj.m_Objs[i].SprLoad(m_HOutObj.m_Header.szRegObjSprFNs[i], g_pTEng->PixelFormatGet());
	}
	if(m_HOutObj.m_pTData)
	{
		for(i = 0; i < m_HOutObj.m_vTCount; i++)
		{
			if(m_HOutObj.m_pTData[i].value < 0) continue;
			
			int nSet = m_HOutObj.m_pTData[i].value >> 8; // 오브젝트 세트
			if(nSet < 0 || nSet >= MAX_MAP_OBJ_REG) continue;
			int nSubSet = m_HOutObj.m_pTData[i].value & 0xff; // 오브젝트 서브 세트
			if(nSubSet < 0) continue;
			
			CAniObjLSP* pAniObj = &m_HOutObj.m_Objs[nSet];
			const _SPR_LSP* pSpr = NULL;
			int nSpr = pAniObj->FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
			pAniObj->SprGet(nSpr, &pSpr);
			if(pSpr == NULL)
			{
				m_HOutObj.m_pnData[m_HOutObj.m_pTData[i].index] = -1;
				TRACE("HO(%d,%d) : %d,%d\n", m_HOutObj.m_pTData[i].index%m_HOutObj.m_Header.nWidth,
					m_HOutObj.m_pTData[i].index/m_HOutObj.m_Header.nWidth, nSet, nSubSet);
			}
		}
		delete[] m_HOutObj.m_pTData;
		m_HOutObj.m_pTData = NULL;
	}
	m_HInObj.ReleaseEx(); // 해제
	m_HInObj.Load(hFile); // 부르고
	for(i = 0; i < MAX_MAP_OBJ_REG; i++) // 객체들 로딩.
	{
		g_pTEng->PathSetCur("ANI");
		m_HInObj.m_Objs[i].AniLoad(m_HInObj.m_Header.szRegObjAniFNs[i]);
		g_pTEng->PathSetCur("SPR");
		m_HInObj.m_Objs[i].SprLoad(m_HInObj.m_Header.szRegObjSprFNs[i], g_pTEng->PixelFormatGet());
	}
	if(m_HInObj.m_pTData)
	{
		for(i = 0; i < m_HInObj.m_vTCount; i++)
		{
			if(m_HInObj.m_pTData[i].value < 0) continue;
			
			int nSet = m_HInObj.m_pTData[i].value >> 8; // 오브젝트 세트
			if(nSet < 0 || nSet >= MAX_MAP_OBJ_REG) continue;
			int nSubSet = m_HInObj.m_pTData[i].value & 0xff; // 오브젝트 서브 세트
			if(nSubSet < 0) continue;
			
			CAniObjLSP* pAniObj = &m_HInObj.m_Objs[nSet];
			const _SPR_LSP* pSpr = NULL;
			int nSpr = pAniObj->FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
			pAniObj->SprGet(nSpr, &pSpr);
			if(pSpr == NULL)
			{
				m_HInObj.m_pnData[m_ShadowObj.m_pTData[i].index] = -1;
				TRACE("HI(%d,%d) : %d,%d\n", m_HInObj.m_pTData[i].index%m_HInObj.m_Header.nWidth,
					m_HInObj.m_pTData[i].index/m_HInObj.m_Header.nWidth, nSet, nSubSet);
			}
		}
		delete[] m_HInObj.m_pTData;
		m_HInObj.m_pTData = NULL;
	}

	CloseHandle(hFile);

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
	
	CFile file;

	file.Open(FileName, CFile::modeWrite | CFile::modeCreate);

	file.Write(&m_vCurrentVersion, sizeof(int));

	m_MapObj.Save(file);
	m_ShadowObj.Save(file);
	m_HOutObj.Save(file);
	m_HInObj.Save(file);

	file.Close();
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
	m_ShadowObj.Realloc(nW, nH); // 그림자 오브젝트 맵 다시 할당.
	m_HOutObj.Realloc(nW, nH); // 오브젝트 맵 다시 할당.
	m_HInObj.Realloc(nW, nH); // 그림자 오브젝트 맵 다시 할당.
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
	if(m_pWndObjPreview) m_pWndObjPreview->InvalidateRect(NULL,FALSE);
}

void CDlgTileMapObj::UpdateObjInfo()
{
	m_ListObjSet.ResetContent();
	for(int i = 0; i < MAX_MAP_OBJ_REG; i++)
	{
		CString sz = m_MapObj.m_Header.szRegObjAniFNs[i];
		sz += " + ";
		sz += m_MapObj.m_Header.szRegObjSprFNs[i];
		sz += " + ";
		sz += m_ShadowObj.m_Header.szRegObjSprFNs[i];
		sz += " + ";
		sz += m_HOutObj.m_Header.szRegObjSprFNs[i];
		sz += " + ";
		sz += m_HInObj.m_Header.szRegObjSprFNs[i];
		m_ListObjSet.AddString(sz);
	}
	m_ListObjSet.SetCurSel(0);

	this->UpdateObjSubSet(); // 서브셋 정보 나타내기.

	SetDlgItemInt(IDC_E_WIDTH, m_MapObj.m_Header.nWidth);
	SetDlgItemInt(IDC_E_HEIGHT, m_MapObj.m_Header.nHeight);
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
	

	((CButton *)GetDlgItem(IDC_CHECK_SHOW_INNERHOUSE))->SetCheck(m_bShowInnerHouse);
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

void CDlgTileMapObj::OnBSprFileNameSet3() 
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "단순 LSP 스프라이트(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	lstrcpy(m_HOutObj.m_Header.szRegObjSprFNs[index], FileName); // 헤더에 파일 이름 넣어주고..
	m_HOutObj.m_Objs[index].SprLoad(FileName, g_pTEng->PixelFormatGet()); // 에니메이션 로딩

	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_ShadowObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HOutObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HInObj.m_Header.szRegObjSprFNs[index];

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, sz);
	m_ListObjSet.SetCurSel(index);
	this->UpdateObjSubSet(); // 서브 리스트 업데이트

	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnBSprFileNameSet4() 
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "단순 LSP 스프라이트(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	lstrcpy(m_HInObj.m_Header.szRegObjSprFNs[index], FileName); // 헤더에 파일 이름 넣어주고..
	m_HInObj.m_Objs[index].SprLoad(FileName, g_pTEng->PixelFormatGet()); // 에니메이션 로딩

	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_ShadowObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HOutObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HInObj.m_Header.szRegObjSprFNs[index];

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, sz);
	m_ListObjSet.SetCurSel(index);
	this->UpdateObjSubSet(); // 서브 리스트 업데이트

	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnBSprFileNameDelete3() 
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	m_HOutObj.m_Objs[index].SprRelease();

	lstrcpy(m_HOutObj.m_Header.szRegObjSprFNs[index], ""); // 헤더에 파일 이름 넣어주고..

	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_ShadowObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HOutObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HInObj.m_Header.szRegObjSprFNs[index];

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, sz);
	m_ListObjSet.SetCurSel(index);
	this->UpdateObjSubSet(); // 서브 리스트 업데이트

	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnBSprFileNameDelete4() 
{
	int index = m_ListObjSet.GetCurSel();
	if(index < 0 || index >= MAX_MAP_OBJ_REG) return;

	m_HInObj.m_Objs[index].SprRelease();

	lstrcpy(m_HInObj.m_Header.szRegObjSprFNs[index], ""); // 헤더에 파일 이름 넣어주고..

	CString sz = m_MapObj.m_Header.szRegObjAniFNs[index]; // 리스트에 넣을 이름 만든다..
	sz += " + ";
	sz += m_MapObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_ShadowObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HOutObj.m_Header.szRegObjSprFNs[index];
	sz += " + ";
	sz += m_HInObj.m_Header.szRegObjSprFNs[index];

	m_ListObjSet.DeleteString(index); // 리스트 박스 내용 바꾸어 주고..
	m_ListObjSet.InsertString(index, sz);
	m_ListObjSet.SetCurSel(index);
	this->UpdateObjSubSet(); // 서브 리스트 업데이트

	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnCheckShowInnerhouse() 
{
	m_bShowInnerHouse = ((CButton *)GetDlgItem(IDC_CHECK_SHOW_INNERHOUSE))->GetCheck();
	this->UpdateWorkWindow();
}

void CDlgTileMapObj::OnCObjPreView() 
{
	if(IsDlgButtonChecked(IDC_C_OBJ_PRE_VIEW) == 1)
	{
		m_pWndObjPreview = new CWndObjPreview( this );
		if(m_pWndObjPreview)
		{
			// 타일 에니메이션 프리뷰 윈도우...
			if(m_pWndObjPreview->GetSafeHwnd() == NULL)
			{
				HCURSOR hCur = LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_C_NORMAL));
				CString szWndClass;
				szWndClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, hCur, NULL, 0);
				m_pWndObjPreview->CreateEx(NULL,
										szWndClass,
										"오브젝트 프리뷰",
										WS_OVERLAPPEDWINDOW,
										CW_USEDEFAULT,
										CW_USEDEFAULT,
										256,
										256,
										this->m_hWnd,
										NULL);
			}
			m_pWndObjPreview->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		if( m_pWndObjPreview )
		{
			m_pWndObjPreview->DestroyWindow();
			delete m_pWndObjPreview;
		}
		m_pWndObjPreview = NULL;
	}
}

void CDlgTileMapObj::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if( m_pWndObjPreview )
	{
		if( m_pWndObjPreview->GetSafeHwnd != NULL )
			m_pWndObjPreview->DestroyWindow();
		delete m_pWndObjPreview;
	}
	m_pWndObjPreview = NULL;
}
