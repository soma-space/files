// DlgSprMerge.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgSprMerge.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSprMerge dialog

CDlgSprMerge::CDlgSprMerge(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSprMerge::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSprMerge)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSprMerge::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSprMerge)
	DDX_Control(pDX, IDC_LIST_REMARK, m_ListRemark);
	DDX_Control(pDX, IDC_C_ANI_GENERATE_AUTO, m_CAniGenerateAuto);
	DDX_Control(pDX, IDC_LIST_SPR, m_ListSprs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSprMerge, CDialog)
	//{{AFX_MSG_MAP(CDlgSprMerge)
	ON_BN_CLICKED(IDC_B_SPR_SET, OnBSprSet)
	ON_BN_CLICKED(IDC_B_SPR_INSERT, OnBSprInsert)
	ON_BN_CLICKED(IDC_B_SPR_ADD, OnBSprAdd)
	ON_BN_CLICKED(IDC_B_SPR_DELETE, OnBSprDelete)
	ON_BN_CLICKED(IDC_B_SPR_MERGE, OnBSprMerge)
	ON_BN_CLICKED(IDC_B_SPR_LIST_NEW, OnBSprListNew)
	ON_BN_CLICKED(IDC_B_SPR_LIST_LOAD, OnBSprListLoad)
	ON_BN_CLICKED(IDC_B_SPR_LIST_SAVE, OnBSprListSave)
	ON_BN_CLICKED(IDC_B_SPR_LIST_SAVE_AS, OnBSprListSaveAs)
	ON_BN_CLICKED(IDC_B_MOVE_POS_UP, OnBMovePosUp)
	ON_BN_CLICKED(IDC_B_MOVE_POS_DOWN, OnBMovePosDown)
	ON_BN_CLICKED(IDC_B_SPR_INSERT_BLANK, OnBSprInsertBlank)
	ON_LBN_SELCHANGE(IDC_LIST_REMARK, OnSelchangeListRemark)
	ON_LBN_SELCHANGE(IDC_LIST_SPR, OnSelchangeListSpr)
	ON_BN_CLICKED(IDC_B_SPR_FILE_NAME_COPY, OnBSprFileNameCopy)
	ON_BN_CLICKED(IDC_B_SPR_FILE_NAME_PASTE, OnBSprFileNamePaste)
	ON_LBN_DBLCLK(IDC_LIST_SPR, OnDblclkListSpr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSprMerge message handlers

void CDlgSprMerge::OnBSprSet() 
{
	int index = m_ListSprs.GetCurSel();
	if(index < 0) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "LSP Sprite(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR_자동추출");

	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetPathName();
	m_ListSprs.DeleteString(index);
	m_ListSprs.InsertString(index, FileName);
	m_ListSprs.SetCurSel(index);
}

void CDlgSprMerge::OnBSprInsert() 
{
	int index = m_ListSprs.GetCurSel();
	if(index < 0) return;

	char szBuff[10240]; MAKE_ZERO(szBuff);
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "LSP Sprite(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR_자동추출");
	dlg.m_ofn.lpstrFile = szBuff;
	dlg.m_ofn.nMaxFile = 10240;
	if(dlg.DoModal() == IDCANCEL) return;

	POSITION pos = dlg.GetStartPosition();
	CString FileName;
	while(pos != NULL)
	{
		FileName = dlg.GetNextPathName(pos);
		m_ListSprs.InsertString(index, FileName);
		m_ListSprs.SetCurSel(index);
		m_ListSprs.SetSel(index, TRUE);
	}
	FileName = dlg.GetPathName();
}

void CDlgSprMerge::OnBSprInsertBlank() 
{
	int index = m_ListSprs.GetCurSel();
	if(index < 0) return;

	index = m_ListSprs.InsertString(index, "");
	m_ListSprs.SetCurSel(index);

	this->OnSelchangeListSpr();
}

void CDlgSprMerge::OnBSprAdd() 
{
	char szBuff[10240]; MAKE_ZERO(szBuff);
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "LSP Sprite(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR_자동추출");
	dlg.m_ofn.lpstrFile = szBuff;
	dlg.m_ofn.nMaxFile = 10240;
	if(dlg.DoModal() == IDCANCEL) return;

	POSITION pos = dlg.GetStartPosition();
	CString FileName;
	int index = 0;
	while(pos != NULL)
	{
		FileName = dlg.GetNextPathName(pos);
		index = m_ListSprs.AddString(FileName);
		m_ListSprs.SetSel(index, TRUE);
	}
	m_ListSprs.SetCurSel(index);
}

void CDlgSprMerge::OnBSprDelete() 
{
	int index = m_ListSprs.GetCurSel();
	if(index < 0) return;

	m_ListSprs.DeleteString(index);
	m_ListSprs.SetCurSel(index);

	this->OnSelchangeListSpr();
}

void CDlgSprMerge::OnBSprMerge() 
{
	int nFC = m_ListSprs.GetCount();
	if(nFC <= 0) return;

	CAniObjLSP* pAniObjs[64];
	MAKE_ZERO(pAniObjs);

	CString szSprFN;
	CStringArray szAniNames;

	int nSprCounts[64]; MAKE_ZERO(nSprCounts);
	int nSprCountAll = 0;
	for(int i = 0; i < nFC; i++)
	{
		m_ListSprs.GetText(i, szSprFN);
		pAniObjs[i] = new CAniObjLSP();
		pAniObjs[i]->SprLoad(szSprFN, g_pTEng->PixelFormatGet());
		nSprCounts[i] = pAniObjs[i]->m_HSpr.nQt;
		nSprCountAll += pAniObjs[i]->m_HSpr.nQt;

		int n1 = szSprFN.ReverseFind('\\');
		if(n1 >= 5)
		{
			szSprFN = szSprFN.Right(szSprFN.GetLength() - n1 - 1);
			szSprFN = szSprFN.Left(szSprFN.GetLength() - 4);
		}
		int n2 = szSprFN.ReverseFind('_');
		if(n2 > 1)
		{
			szSprFN = szSprFN.Right(szSprFN.GetLength() - n2 - 1);
		}
		szAniNames.Add(szSprFN); // 에니메이션 이름을 정해 놓는다.
	}

	CAniObjTool AniObj;
	AniObj.SprAlloc(nSprCountAll);
	AniObj.m_HSpr.nQt = nSprCountAll; // 모두 합친 숫자로 헤더를 만들고
	int nTmp = 0;
	const _SPR_LSP* pSpr = NULL;
	const _SPR_LSP* pSprTmp = NULL;
	_SPR_LSP* pSprWork = NULL;
	for(i = 0; i < nFC; i++)
	{
		for(int j = 0; j < nSprCounts[i]; j++)
		{
			pAniObjs[i]->SprGet(j, &pSpr); // 이해 하려고 하지 마라 원래 그런거다.. 내부적으로 소멸자를 호출하기 땜시 신경 꺼라..
			AniObj.SprGet(nTmp, &pSprTmp);
			pSprWork = (_SPR_LSP*)pSprTmp;
			*pSprWork = *pSpr;
			pSprWork->pwV = new WORD[pSprWork->nDataSize/2];
			memcpy(pSprWork->pwV, pSpr->pwV, pSprWork->nDataSize);
			nTmp++;
		}
	}
	
	for(i = 0; i < 64; i++)
	{
		DELETE_SAFE(pAniObjs[i]);
	}

	CString FileName;
	GetDlgItemText(IDC_E_SPR_LIST_FILE_NAME, FileName);
	if(FileName.GetLength() > 0) 
	{
		int nnnn1 = FileName.ReverseFind('\\');
		int nnnn2 = FileName.ReverseFind('.');
		FileName = FileName.Mid(nnnn1 + 1, nnnn2 - nnnn1 - 1);
	}
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "spl", FileName, dwFlags, "LSP Sprite(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");

	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();
	AniObj.SprSave(FileName);

	if(m_CAniGenerateAuto.GetCheck() == TRUE) // 자동으로 에니메이션 추출 이면...
	{
		int nDirQt = GetDlgItemInt(IDC_E_ANI_DIR_QT);
		if(nDirQt < 0)
		{
			MessageBox("에니메이션은 최소 1방향 이상이어야 합니다.");
		}
		else
		{
			AniObj.AniDirQtSet(nDirQt); // 에니메이션 방향 적용
			AniObj.AniAlloc(nFC); // 합친 파일 갯수 만큼 에니메이션 할당..
			int nnnn = 0;
			for(i = 0; i < nFC; i++)
			{
				_ANI_DATA* pAniWork = AniObj.AniDataGet(i);
				if(pAniWork == NULL) continue;

				CString szAni = szAniNames.GetAt(i);
				pAniWork->fSpd = 10.0f;
				pAniWork->nMaxFrm = nSprCounts[i]/8; // 각 프레임 갯수 / 8방향
				if(pAniWork->nMaxFrm > 0)
				{
					pAniWork->pnFrms = new short int[pAniWork->nMaxFrm*AniObj.m_HAni.nDirQt];
					for(int j = 0; j < AniObj.m_HAni.nDirQt; j++)
					{
						for(int k = 0; k < pAniWork->nMaxFrm; k++)
						{
							pAniWork->pnFrms[j*pAniWork->nMaxFrm + k] = nnnn;
							nnnn++;
						}
					}
				}
				if(szAni.GetLength() >= 32) szAni = szAni.Right(31);
				lstrcpy(pAniWork->szName, szAni);
			}
		}

		FileName = FileName.Left(FileName.GetLength() - 4);

		CString FileName2;
		DWORD dwFlags2 = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		CFileDialog dlg2(FALSE, "ani", FileName, dwFlags2, "Animation File(*.ani)|*.ani||", NULL);
		dlg2.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("ANI");

		if(dlg2.DoModal() == IDCANCEL) return;
		FileName2 = dlg2.GetPathName();

		AniObj.AniSave(FileName2);
	}
}

void CDlgSprMerge::OnBSprListNew() 
{
	m_ListSprs.ResetContent(); // 에니메이션 오브젝트 초기화
	SetDlgItemText(IDC_E_SPR_LIST_FILE_NAME, "");
}

void CDlgSprMerge::OnBSprListLoad() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spmd", NULL, dwFlags, "Spr Merge Data File(*.spmd)|*.spmd||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetPathName();
	SetDlgItemText(IDC_E_SPR_LIST_FILE_NAME, FileName);

	CFile file;
	file.Open(FileName, CFile::modeRead);

	m_ListSprs.ResetContent();
	int nCount;
	file.Read(&nCount, 4);
	char szSprFN[512];
	int n;
	for(int i = 0; i < nCount; i++)
	{
		file.Read(&n, 4);
		file.Read(szSprFN, n);
		int index = m_ListSprs.AddString(szSprFN);
		m_ListSprs.SetSel(index, TRUE);
	}
	file.Close();
	m_ListSprs.SetCurSel(0);
}

void CDlgSprMerge::OnBSprListSave() 
{
	CString FileName;
	GetDlgItemText(IDC_E_SPR_LIST_FILE_NAME, FileName);

	if(FileName.GetLength() == 0)
	{
		this->OnBSprListSaveAs();
		return;
	}

	CFile file;
	file.Open(FileName, CFile::modeCreate | CFile::modeWrite);

	char szSprFN[512];
	int nCount = m_ListSprs.GetCount();
	file.Write(&nCount, 4);
	for(int i = 0; i < nCount; i++)
	{
		m_ListSprs.GetText(i, szSprFN);
		int n = lstrlen(szSprFN) + 1; // 끝의 널문자까지...
		file.Write(&n, 4);
		file.Write(szSprFN, n);
	}
	file.Close();
}

void CDlgSprMerge::OnBSprListSaveAs() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "spmd", NULL, dwFlags, "Spr Merge Data File(*.spmd)|*.spmd||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetPathName();
	SetDlgItemText(IDC_E_SPR_LIST_FILE_NAME, FileName);

	this->OnBSprListSave();
}

void CDlgSprMerge::OnOK()
{
}

void CDlgSprMerge::OnCancel()
{
	CDialog::OnCancel();
}

BOOL CDlgSprMerge::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_CAniGenerateAuto.SetCheck(TRUE);
	SetDlgItemInt(IDC_E_ANI_DIR_QT, 8); // 기본적으로는 8방향 에니메이션이다..

	// 설명...
	m_ListRemark.AddString("ANI_BASIC = 0; // 기본 동작 - 숨쉬기.");
	m_ListRemark.AddString("ANI_EVENT_0 = 1; // 그냥 하는 짓 0");
	m_ListRemark.AddString("ANI_EVENT_1 = 2; // 그냥 하는 짓 1");
	m_ListRemark.AddString("ANI_EVENT_2 = 3; // 그냥 하는 짓 2");
	m_ListRemark.AddString("ANI_MOVE_WALK = 4; // 걸어감");
	m_ListRemark.AddString("ANI_MOVE_RUN = 5; // 뛰어감");
	m_ListRemark.AddString("ANI_ATTACK_0 = 6; // 공격중의 하나");
	m_ListRemark.AddString("ANI_ATTACK_1 = 7; // 공격중의 하나");
	m_ListRemark.AddString("ANI_ATTACK_2 = 8; // 공격중의 하나");
	m_ListRemark.AddString("ANI_HITTED_0 = 9; // 얻어 맞음");
	m_ListRemark.AddString("ANI_HITTED_1 = 10; // 얻어 맞음");
	m_ListRemark.AddString("ANI_PHONE_USE = 11; // 뒤지폰 사용");
	m_ListRemark.AddString("ANI_MOVE_SWIM = 12; // 수영");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSprMerge::OnBMovePosUp() 
{
	int index = m_ListSprs.GetCurSel();
	if(index == LB_ERR || index == 0) return;

	CString str;
	m_ListSprs.GetText(index, str);
	m_ListSprs.DeleteString(index);
	m_ListSprs.InsertString(index - 1, str);
	m_ListSprs.SetCurSel(index - 1);

	this->OnSelchangeListSpr();
}

void CDlgSprMerge::OnBMovePosDown() 
{
	int index = m_ListSprs.GetCurSel();
	if(index == LB_ERR || index == m_ListSprs.GetCount() - 1) return;

	CString str;
	m_ListSprs.GetText(index, str);
	m_ListSprs.DeleteString(index);
	m_ListSprs.InsertString(index + 1, str);
	m_ListSprs.SetCurSel(index + 1);

	this->OnSelchangeListSpr();
}


void CDlgSprMerge::OnSelchangeListRemark() 
{
	int nSel = this->m_ListRemark.GetCurSel();
	if(nSel >= 0) m_ListSprs.SetCurSel(nSel);
}

void CDlgSprMerge::OnSelchangeListSpr() 
{
	int nSel = m_ListSprs.GetCurSel();
	if(nSel >= 0) m_ListRemark.SetCurSel(nSel);
}

void CDlgSprMerge::OnBSprFileNameCopy() 
{
	int nSel = m_ListSprs.GetCurSel();
	if(nSel < 0) return;
	
	m_ListSprs.GetText(nSel, m_szFNClip);
}

void CDlgSprMerge::OnBSprFileNamePaste() 
{
	int nSel = m_ListSprs.GetCurSel();
	if(nSel < 0) return;

	m_ListSprs.DeleteString(nSel);
	m_ListSprs.InsertString(nSel, m_szFNClip);
	m_ListSprs.SetCurSel(nSel);
}

void CDlgSprMerge::OnDblclkListSpr() 
{
	this->OnBSprSet();
}
