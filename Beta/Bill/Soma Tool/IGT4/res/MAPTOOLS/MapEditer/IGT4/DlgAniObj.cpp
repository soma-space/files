// DlgAniObj.cpp : implementation file
//

#include "stdafx.h"

#include "DlgAniObj.h"
#include "Dib16.h"

#include "GlobalVar.h"

CDlgAniObj::CDlgAniObj() : CDialog(CDlgAniObj::IDD)
{
	//{{AFX_DATA_INIT(CDlgAniObj)
	//}}AFX_DATA_INIT

	m_pWndWork = NULL;
	m_bPlaying = 0;
	m_pClipAni = NULL;

	MAKE_ZERO(m_nFrmCurs);
}

CDlgAniObj::~CDlgAniObj()
{
	if(m_pClipAni)
	{
		DELETE_ARRAY_SAFE(m_pClipAni->pnFrms);
		DELETE_SAFE(m_pClipAni);
	}
}

void CDlgAniObj::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAniObj)
	DDX_Control(pDX, IDC_CB_DIR, m_CBDir);
	DDX_Control(pDX, IDC_CB_WORK, m_CBWork);
	DDX_Control(pDX, IDC_SLIDER_SPR, m_SliderSpr);
	DDX_Control(pDX, IDC_ANI_NAME, m_EditAniName);
	DDX_Control(pDX, IDC_ANI_LIST, m_ListAni);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAniObj, CDialog)
	//{{AFX_MSG_MAP(CDlgAniObj)
	ON_BN_CLICKED(IDC_B_ANI_FRM_NEXT, OnBAniFrmNext)
	ON_BN_CLICKED(IDC_B_ANI_FRM_PREV, OnBAniFrmPrev)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_B_ANI_PLAY, OnBAniPlay)
	ON_BN_CLICKED(IDC_B_ANI_ADD, OnBAniAdd)
	ON_BN_CLICKED(IDC_B_ANI_DELETE, OnBAniDelete)
	ON_BN_CLICKED(IDC_B_ANI_INSERT, OnBAniInsert)
	ON_BN_CLICKED(IDC_B_ANI_COPY, OnBAniCopy)
	ON_BN_CLICKED(IDC_B_ANI_PASTE, OnBAniPaste)
	ON_BN_CLICKED(IDC_B_ANI_NEW, OnBAniNew)
	ON_BN_CLICKED(IDC_B_ANI_LOAD, OnBAniLoad)
	ON_BN_CLICKED(IDC_B_ANI_SAVE, OnBAniSave)
	ON_BN_CLICKED(IDC_B_ANI_SAVE_AS, OnBAniSaveAs)
	ON_LBN_SELCHANGE(IDC_ANI_LIST, OnSelchangeAniList)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_B_ANI_FRMS_GET_FROM_STRING, OnBAniFrmsGetFromString)
	ON_BN_CLICKED(IDC_B_ANI_FRMS_GET_AUTO_FROM_FRONT, OnBAniFrmsGetAutoFromFront)
	ON_BN_CLICKED(IDC_B_ANI_FRMS_GET_AUTO_SEQ, OnBAniFrmsGetAutoSeq)
	ON_BN_CLICKED(IDC_B_ANI_FRMS_SET_SAME_TO_FRONT, OnBAniFrmsSetSameToFront)
	ON_EN_CHANGE(IDC_ANI_NAME, OnChangeAniName)
	ON_BN_CLICKED(IDC_B_SPR_EXPORT_TO_BMP, OnBSprExportToBmp)
	ON_BN_CLICKED(IDC_B_SPR_IMPORT_FROM_BMP, OnBSprImportFromBmp)
	ON_EN_CHANGE(IDC_E_ANI_MAX_FRM, OnChangeEAniMaxFrm)
	ON_BN_CLICKED(IDC_B_SPR_LOAD, OnBSprLoad)
	ON_BN_CLICKED(IDC_B_SPR_NEW, OnBSprNew)
	ON_BN_CLICKED(IDC_B_SPR_SAVE, OnBSprSave)
	ON_BN_CLICKED(IDC_B_SPR_SAVE_AS, OnBSprSaveAs)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_B_SPR_SET_QT, OnBSprSetQt)
	ON_BN_CLICKED(IDC_B_SPR_BROWSE_BMP, OnBSprBrowseBmp)
	ON_BN_CLICKED(IDC_C_SPR_CK_R, OnCSprCkR)
	ON_BN_CLICKED(IDC_C_SPR_CK_G, OnCSprCkG)
	ON_BN_CLICKED(IDC_C_SPR_CK_B, OnCSprCkB)
	ON_CBN_SELENDOK(IDC_CB_WORK, OnSelendokCbWork)
	ON_EN_CHANGE(IDC_E_ANI_SPEED, OnChangeEAniSpeed)
	ON_BN_CLICKED(IDC_B_ANI_SET_DIR_QT, OnBAniSetDirQt)
	ON_CBN_SELCHANGE(IDC_CB_DIR, OnSelchangeCbDir)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_E_ANI_SPEED, OnChangeEAniSpeed)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAniObj message handlers

void CDlgAniObj::OnBAniFrmsGetFromString() 
{
	// 에디트에 있는 문자열에서 숫자 배열을 끄집어 온다.
	CString frames;
	int dir = m_CBDir.GetCurSel();
	GetDlgItemText(IDC_E_ANI_FRMS, frames);

	CString temp;
	int n = 0;
	int nPrev = 0;
	int nCount = 0;
	
	// 문자열을 뒤져서 , 앞에 있는 문자열은 숫자로 바꾼후..
	int nAni = m_ListAni.GetCurSel(); if(nAni == CB_ERR) return; // 리스트에서 에니메이션이 선택되어 있지 않다..
	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL) return;

	while(TRUE)
	{
		if(nCount >= pAni->nMaxFrm) break; // 최대 프레임까지 가면 잘라버린다.
		n = frames.Find(",", nPrev);
		// 더이상 따옴표가 없으면 맨뒤의 문자열을 숫자열로 바꾸어 준다.
		if(n == -1)
		{
			temp = frames.Mid(nPrev, frames.GetLength() - nPrev);
			if(temp.GetLength()) pAni->pnFrms[dir*pAni->nMaxFrm + nCount] = atoi(temp);
			break;
		}
		temp = frames.Mid(nPrev, n-nPrev);
		pAni->pnFrms[dir*pAni->nMaxFrm + nCount] = atoi(temp);
		nCount++;
		nPrev = n+1;
	}

	// 부모 윈도우 및 대화상자 갱신
	UpdateWorkWindow();
	this->UpdateAniInfo();
}

void CDlgAniObj::OnBAniFrmPrev() 
{
	int nIndex = m_ListAni.GetCurSel();
	if(nIndex < 0 || nIndex >= MAX_ANI) return;
	m_nFrmCurs[nIndex]--;

	// 부모 윈도우 및 대화상자 갱신
	this->UpdateAniInfo();
	UpdateWorkWindow();
}

void CDlgAniObj::OnBAniFrmNext() 
{
	int nIndex = m_ListAni.GetCurSel();
	if(nIndex < 0 || nIndex >= MAX_ANI) return;
	m_nFrmCurs[nIndex]++;

	// 부모 윈도우 및 대화상자 갱신
	this->UpdateAniInfo();
	UpdateWorkWindow();
}

void CDlgAniObj::UpdateAniInfo()
{
	m_bRefreshing = TRUE;

	int nAni = m_ListAni.GetCurSel();
	if(nAni < 0 || nAni >= MAX_ANI || nAni == CB_ERR) return; // 리스트에서 에니메이션이 선택되어 있지 않다..

	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL) return;
	
	int nDirQt = m_pAniObj->m_HAni.nDirQt;
	int nDir = m_CBDir.GetCurSel();
	if(nDir < 0) nDir = 0;
	if(nDir >= nDirQt) nDir = nDirQt - 1;
	
	// 대화상자의 값 업데이트
	CString szSpd; szSpd.Format("%5.2f", pAni->fSpd);
	int nMaxFrm = GetDlgItemInt(IDC_E_ANI_MAX_FRM);
	if(nMaxFrm != pAni->nMaxFrm) SetDlgItemInt(IDC_E_ANI_MAX_FRM, pAni->nMaxFrm);
	
	if(pAni->nMaxFrm > 0)
	{
		if(m_nFrmCurs[nAni] < 0) m_nFrmCurs[nAni] = 0; // 몇번째 프레임인지..
		if(m_nFrmCurs[nAni] >= pAni->nMaxFrm) m_nFrmCurs[nAni] = pAni->nMaxFrm - 1;
		SetDlgItemInt(IDC_E_ANI_CUR_FRM, m_nFrmCurs[nAni]);
		SetDlgItemInt(IDC_E_ANI_SPR, pAni->pnFrms[nDir*pAni->nMaxFrm + m_nFrmCurs[nAni]]); // 스프라이트 번호 표시
	}
	
	SetDlgItemText(IDC_E_ANI_SPEED, szSpd);
	SetDlgItemInt(IDC_E_ANI_NUMBER, nAni);

	// 프레임을 업데이트 해준다...
	CString temp, szFrms;
	szFrms.Empty();
	for(int j = 0; j < pAni->nMaxFrm; j++)
	{
		temp.Format("%d,", pAni->pnFrms[nDir*pAni->nMaxFrm + j]);
		szFrms += temp;
	}
	SetDlgItemText(IDC_E_ANI_FRMS, szFrms);
	
	m_bRefreshing = FALSE;
}

void CDlgAniObj::UpdateSprInfo()
{
	SetDlgItemInt(IDC_E_SPR_CUR, m_SliderSpr.GetPos());
}

void CDlgAniObj::UpdateSprObjInfo()
{
	m_SliderSpr.SetRange(0, m_pAniObj->m_HSpr.nQt - 1, TRUE);
	SetDlgItemText(IDC_E_SPR_BMP_FILE_NAME, m_pAniObj->m_HSpr.szBMPFN);
	SetDlgItemInt(IDC_E_SPR_WHOLE, m_pAniObj->m_HSpr.nQt);

	if(m_pAniObj->m_lpDDS) 
	{
		CRect rc; g_pTEng->SurfaceGetSize(m_pAniObj->m_lpDDS, &rc);
		m_pAniObj->m_HSpr.nWidth = rc.Width();
		m_pAniObj->m_HSpr.nHeight = rc.Height();
	}

	if(m_pAniObj->m_HSpr.SrcCK & 0x000000ff) CheckDlgButton(IDC_C_SPR_CK_R, 1); else CheckDlgButton(IDC_C_SPR_CK_R, 0);
	if(m_pAniObj->m_HSpr.SrcCK & 0x0000ff00) CheckDlgButton(IDC_C_SPR_CK_G, 1); else CheckDlgButton(IDC_C_SPR_CK_G, 0);
	if(m_pAniObj->m_HSpr.SrcCK & 0x00ff0000) CheckDlgButton(IDC_C_SPR_CK_B, 1); else CheckDlgButton(IDC_C_SPR_CK_B, 0);
}

void CDlgAniObj::OnChangeEAniMaxFrm() 
{
	if(m_bRefreshing == TRUE) return;

	int n = GetDlgItemInt(IDC_MAX_FRAME);
	if(n < 0) n = 0;

	int nAni = m_ListAni.GetCurSel(); if(nAni == CB_ERR) return; // 리스트에서 에니메이션이 선택되어 있지 않다..
	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL) return;

	int nDirQt = m_pAniObj->m_HAni.nDirQt; // 전의 프레임을 기억한다..
	int nFrms[32][64]; MAKE_ZERO(nFrms);
	if(pAni->nMaxFrm > 0)
	{
		for(int i = 0; i < nDirQt; i++)
		{
			for(int j = 0; j < pAni->nMaxFrm; j++)
			{
				nFrms[i][j] = pAni->pnFrms[nDirQt * i + j];
			}
		}
	}

	DELETE_ARRAY_SAFE(pAni->pnFrms);
	pAni->nMaxFrm = n;
	if(pAni->nMaxFrm > 0)
	{
		pAni->pnFrms = new short int[pAni->nMaxFrm*m_pAniObj->m_HAni.nDirQt];
		memset(pAni->pnFrms, 0, pAni->nMaxFrm*m_pAniObj->m_HAni.nDirQt*2);
	}

	// 대화상자 갱신
	this->UpdateAniInfo();
}

void CDlgAniObj::OnChangeEAniSpeed() 
{
	CString string; 
	GetDlgItemText(IDC_ANI_SPEED, string);
	float f = (float)atof(string);
	if(f <= 0.0f) f = 10.0f;

	int nAni = m_ListAni.GetCurSel(); if(nAni == CB_ERR) return; // 리스트에서 에니메이션이 선택되어 있지 않다..
	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL) return;
	pAni->fSpd = f;
}

BOOL CDlgAniObj::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_CBWork.AddString("스프라이트 작업");
	m_CBWork.AddString("에니메이션 작업");
	m_CBWork.SetCurSel(0); // 작업 종류를 정해준다..

	this->OnBAniNew();
	this->OnBSprNew();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgAniObj::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pAniObj = new CAniObjTool();
	
	return 0;
}

void CDlgAniObj::OnDestroy() 
{
	DELETE_SAFE(m_pAniObj);

	CDialog::OnDestroy();
}

void CDlgAniObj::OnBAniPlay() 
{
	if(m_bPlaying == FALSE)
	{
		CButton* b = (CButton*) GetDlgItem(IDC_PLAY);
		b->SetWindowText("Stop");
		b->UpdateWindow();
		m_bPlaying = TRUE;
		SetTimer(1, 5, NULL); // 1 번타이머를 에니메이션 용으로 쓴다.
	}
	else // 플레이중이면..
	{
		CButton* b = (CButton*) GetDlgItem(IDC_PLAY);
		b->SetWindowText("Play");
		b->UpdateWindow();
		this->UpdateAniInfo();
		m_bPlaying = FALSE;
		KillTimer(1); // 1 번타이머를 에니메이션 용으로 쓴다.
	}
}

void CDlgAniObj::UpdateWorkWindow()
{
	if(m_pWndWork == NULL) return;

	m_pWndWork->InvalidateRect(NULL, FALSE);
}

void CDlgAniObj::OnBAniNew() 
{
	m_pAniObj->AniNew(); // 에니메이션 오브젝트 초기화
	this->UpdateAniList();
	this->UpdateAniInfo();
	this->UpdateWorkWindow();

	m_CBDir.ResetContent(); // 방향 갯수 표시
	SetDlgItemInt(IDC_E_ANI_DIR_QT, 0); // 방향 갯수 표시
	SetDlgItemText(IDC_E_ANI_FILE_NAME, "");
}

void CDlgAniObj::OnBAniLoad() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "ani", NULL, dwFlags, "에니메이션(*.ani)|*.ani||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("ANI");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_pAniObj->AniNew(); // 에니메이션 새로...
	m_pAniObj->AniLoad(FileName);

	int nDirQt = m_pAniObj->m_HAni.nDirQt;
	m_CBDir.ResetContent(); // 방향 갯수 표시
	CString szDir;
	m_CBDir.ResetContent();
	for(int i = 0; i < nDirQt; i++)
	{
		szDir.Format("%d", i);
		m_CBDir.AddString(szDir);
	}
	m_CBDir.SetCurSel(0);
	SetDlgItemInt(IDC_E_ANI_DIR_QT, nDirQt); // 방향 갯수 표시
	SetDlgItemText(IDC_E_ANI_FILE_NAME, FileName);

	this->UpdateAniList();
	this->UpdateAniInfo();
	this->UpdateWorkWindow();
}

void CDlgAniObj::OnBAniSave() 
{
	CString FileName;
	GetDlgItemText(IDC_E_ANI_FILE_NAME, FileName);
	if(FileName.GetLength() == 0)
	{
		OnBAniSaveAs();
		return;
	}
	
	g_pTEng->PathSetCur("ANI");  // 경로 지정
	m_pAniObj->AniSave(FileName); // 에니메이션만
}

void CDlgAniObj::OnBAniSaveAs() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "ani", NULL, dwFlags, "에니메이션(*.ani)|*.ani||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("ANI");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();
	SetDlgItemText(IDC_E_ANI_FILE_NAME, FileName);
	OnBAniSave();
}

void CDlgAniObj::OnSelchangeAniList() 
{
	int sel = m_ListAni.GetCurSel();
	if(sel == LB_ERR) return;

	_ANI_DATA* pAni = m_pAniObj->AniDataGet(sel);
	if(pAni == NULL) return;

	m_EditAniName.SetWindowText(pAni->szName); // 이름 바꿔주기..

	this->UpdateAniInfo();
	UpdateWorkWindow();
}

void CDlgAniObj::OnBAniInsert() 
{
	int sel = m_ListAni.GetCurSel();
	if(sel == LB_ERR) return;
	if(m_pAniObj->AniInsert(sel) != TRUE) return; // 유효 에니메이션 끝에 하나 추가..
	
	char szBuff[128];
	sprintf(szBuff, "삽입 에니메이션 %.2d", sel+1);
	m_ListAni.InsertString(sel, szBuff);
}

void CDlgAniObj::OnBAniAdd() 
{
	int sel = m_ListAni.GetCurSel();
	if(m_pAniObj->AniAdd() != TRUE) return; // 유효 에니메이션 끝에 하나 추가..
	char szBuff[128];
	sprintf(szBuff, "추가 에니메이션 %.2d", sel+1);
	m_ListAni.AddString(szBuff);
}

void CDlgAniObj::OnBAniDelete()
{
	int sel = m_ListAni.GetCurSel();
	if(sel == LB_ERR) return;

	if(m_pAniObj->AniDelete(sel) != TRUE) return; // 에니메이션 삭제...
	
	m_ListAni.DeleteString(sel);
	this->UpdateAniInfo();
	UpdateWorkWindow();
}

void CDlgAniObj::OnBAniCopy() 
{
	// TODO: Add your control notification handler code here
	int nAni = m_ListAni.GetCurSel(); if(nAni == CB_ERR) return; // 리스트에서 에니메이션이 선택되어 있지 않다..
	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL) return;

	if(m_pClipAni)
	{
		DELETE_ARRAY_SAFE(m_pClipAni->pnFrms);
		DELETE_SAFE(m_pClipAni);
	}
	m_pClipAni = new _ANI_DATA;
	memcpy(m_pClipAni, pAni, sizeof(_ANI_DATA));
	m_pClipAni->pnFrms = NULL;
	if(pAni->nMaxFrm > 0)
	{
		m_pClipAni->pnFrms = new short int[pAni->nMaxFrm*m_pAniObj->m_HAni.nDirQt];
		memcpy(m_pClipAni->pnFrms, pAni->pnFrms, pAni->nMaxFrm*m_pAniObj->m_HAni.nDirQt*2);
	}
}

void CDlgAniObj::OnBAniPaste() 
{
	// TODO: Add your control notification handler code here
	if(m_pClipAni == NULL) return;

	int nAni = m_ListAni.GetCurSel();
	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL) return;

	DELETE_ARRAY_SAFE(pAni->pnFrms);
	memcpy(pAni, m_pClipAni, sizeof(_ANI_DATA));
	pAni->pnFrms = NULL;

	if(m_pClipAni->nMaxFrm > 0)
	{
		pAni->pnFrms = new short int[pAni->nMaxFrm*m_pAniObj->m_HAni.nDirQt];
		memcpy(pAni->pnFrms, m_pClipAni->pnFrms, pAni->nMaxFrm*m_pAniObj->m_HAni.nDirQt*2);
	}

	m_ListAni.DeleteString(nAni);
	m_ListAni.InsertString(nAni, pAni->szName);
	m_ListAni.SetCurSel(nAni);

	UpdateWorkWindow();
	this->UpdateAniInfo();
}

void CDlgAniObj::UpdateAniList()
{
	m_ListAni.ResetContent(); // 리스트 박스 컨트롤 초기화
	m_pAniObj->m_HAni;

	// 리스트 박스 컨트롤에 추가한다. (New 할때 모두 삭제)
	for(int i=0; i < m_pAniObj->m_HAni.nQt; i++)
	{
		_ANI_DATA* pAni = m_pAniObj->AniDataGet(i);
		m_ListAni.AddString(pAni->szName);
		m_ListAni.SetItemData(i, i);
	}
	m_ListAni.SetCurSel(0);
}

void CDlgAniObj::OnTimer(UINT nIDEvent) 
{

	if(nIDEvent != 1) return; // 1번을 에니메이션 용으로 쓴다.
	
	// 스피드 점검후 실행
	int nAni = m_ListAni.GetCurSel(); if(nAni == CB_ERR) return; // 리스트에서 에니메이션이 선택되어 있지 않다..
	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL) return;

	if(pAni->fSpd <= 0) pAni->fSpd = 10.0f;

	UpdateWorkWindow();

	CDialog::OnTimer(nIDEvent);
}


void CDlgAniObj::OnBAniFrmsGetAutoFromFront() 
{
	// 에디트에 있는 문자열에서 숫자 배열을 끄집어 온다.
	CString frames;
	GetDlgItemText(IDC_E_ANI_FRMS, frames);
	CString temp;

	int n = 0;
	int nPrev = 0;
	
	// 문자열을 뒤져서 , 앞에 있는 문자열은 숫자로 바꾼후..
	int nAni = m_ListAni.GetCurSel(); if(nAni == CB_ERR) return; // 리스트에서 에니메이션이 선택되어 있지 않다..
	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL) return;

	int nDirQt = m_pAniObj->m_HAni.nDirQt; // 방향 갯수
	int min = pAni->nMaxFrm, max = 0;;
	for(int i = 0; i < pAni->nMaxFrm; i++)
	{
		n = frames.Find(",", nPrev);
		// 더이상 따옴표가 없으면 맨뒤의 문자열을 숫자열로 바꾸어 준다.
		if(n == -1)
		{
			temp = frames.Mid(nPrev, frames.GetLength() - nPrev);
			if(temp.GetLength()) pAni->pnFrms[i] = atoi(temp);
			break;
		}
		temp = frames.Mid(nPrev, n-nPrev);
		pAni->pnFrms[i] = atoi(temp);
		
		if(pAni->pnFrms[i] > max) max = pAni->pnFrms[i]; // 최대 값과 최소 값을 찾는다.
		if(pAni->pnFrms[i] < min) min = pAni->pnFrms[i];

		nPrev = n+1;
	}

	int range = max-min; // 제일 작은 숫자와 큰 숫자의 차이를 찾는다.
	int nMaxFrm = pAni->nMaxFrm;
	for(i = 1; i < nDirQt; i++)
	{
		for(int j = 0; j < pAni->nMaxFrm; j++)
		{
			pAni->pnFrms[i*nMaxFrm+j] = pAni->pnFrms[(i-1)*nMaxFrm+j]+range+1;
		}
	}


	// 부모 윈도우 및 대화상자 갱신
	this->UpdateAniInfo();
	UpdateWorkWindow();
}

void CDlgAniObj::OnBAniFrmsSetSameToFront() 
{
	this->OnBAniFrmsGetFromString();
	
	// 앞방향 프레임과 똑같이 만들기..
	int nAni = m_ListAni.GetCurSel(); if(nAni == CB_ERR) return; // 리스트에서 에니메이션이 선택되어 있지 않다..
	
	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL) return;
	if(pAni->nMaxFrm <= 0) return;
	
	
	int nDir = m_CBDir.GetCurSel();
	if(nDir < 0 || nDir >= 8) return;

	int nDirQt = m_pAniObj->m_HAni.nDirQt;
	int nMaxFrm = pAni->nMaxFrm;

	for(int i = 1; i < nDirQt; i++)
	{
		memcpy(&pAni->pnFrms[i*nMaxFrm], pAni->pnFrms, nMaxFrm*2);
	}

	this->UpdateAniInfo();
}

void CDlgAniObj::OnBAniFrmsGetAutoSeq() 
{
	 // 앞방향 프레임과 똑같이 만들기..
	int nAni = m_ListAni.GetCurSel(); if(nAni == CB_ERR) return; // 리스트에서 에니메이션이 선택되어 있지 않다..
	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL) return;
	if(pAni->nMaxFrm <= 0) return;

	int nDirQt = m_pAniObj->m_HAni.nDirQt;
	int nSeq = pAni->pnFrms[0];
	for(int i = 0; i < nDirQt; i++)
	{
		for(int j = 0; j < pAni->nMaxFrm; j++)
		{
			pAni->pnFrms[i*pAni->nMaxFrm+j] = nSeq;
			nSeq++;
		}
	}

	this->UpdateAniInfo();
}

CString CDlgAniObj::GetFileName()
{
	CString fn;
	GetDlgItemText(IDC_FILE_NAME, fn);
	return fn;
}

void CDlgAniObj::SetFileName(const char* szFileName)
{
	SetDlgItemText(IDC_FILE_NAME, szFileName);
}

void CDlgAniObj::OnChangeAniName()
{
	int nSel = m_ListAni.GetCurSel();
	if(nSel == CB_ERR) return;
	
	_ANI_DATA* pAni = m_pAniObj->AniDataGet(nSel);
	if(pAni == NULL) return;

	CString str;
	m_EditAniName.GetWindowText(str);
	lstrcpy(pAni->szName, str);

	m_ListAni.DeleteString(nSel);
	m_ListAni.InsertString(nSel, str);
	m_ListAni.SetCurSel(nSel);
}


// 해당 에니메이션에 해당되는 스프라이트를 통짜 비트맵으로 Export
void CDlgAniObj::OnBSprExportToBmp() 
{

	int nAni = m_ListAni.GetCurSel();
	if(nAni == CB_ERR)
	{
		MessageBox("에니메이션을 선택해 주세요");
		return;
	}

	const _ANI_DATA* pAni;
	pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL)
	{
		MessageBox("에니메이션이 없습니다.");
		return;
	}

	// 중복되는 스프라이트 제거후 리스트 만들기..
	int nDirQt = m_pAniObj->m_HAni.nDirQt;
	int* pnTmps = new int[pAni->nMaxFrm * nDirQt]; // 방향을 곱해준다.
	int nCountTmp = 0;
	for(int i = 0; i < nDirQt; i++)
		for(int j = 0; j < pAni->nMaxFrm; j++)
			pnTmps[nCountTmp++] = pAni->pnFrms[i*pAni->nMaxFrm+j]; // 중복되지 않으면 리스트에 추가..
	
	int nCountFrm = 0;
	int *pnSprs = new int[pAni->nMaxFrm * 8]; // 8방향이다.
	for(i = 0; i < nCountTmp; i++)
	{
		BOOL bOverLapped = FALSE;
		for(int j = 0; j < nCountFrm; j++)
		{
			if(pnTmps[i] == pnSprs[j])
			{
				bOverLapped = TRUE;
				break;
			}
		}
		if(bOverLapped == FALSE) pnSprs[nCountFrm++] = pnTmps[i];
	}
	delete [] pnTmps; pnTmps = NULL;
	
	if(nCountFrm <= 0)
	{
		MessageBox("Export할 스프라이트가 없습니다.");
		delete [] pnSprs; pnSprs = NULL;
		return;
	}

	// 제일 큰 영역을 찾는다..
	CRect rcBiggest(0,0,0,0);
	CRect rcSpr;
	const _SPR_LSP* pSpr;
	for(i = 0; i < nCountFrm; i++)
	{
		m_pAniObj->SprGet(pnSprs[i], &pSpr);
		if(pSpr == NULL) continue;

		rcSpr = pSpr->rcV;
		if(rcSpr.left < rcBiggest.left) rcBiggest.left = rcSpr.left;
		if(rcSpr.top < rcBiggest.top) rcBiggest.top = rcSpr.top;
		if(rcSpr.right > rcBiggest.right) rcBiggest.right = rcSpr.right;
		if(rcSpr.bottom > rcBiggest.bottom) rcBiggest.bottom = rcSpr.bottom;
	}

	int width = rcBiggest.Width();
	int height = rcBiggest.Height();
	int nPitch = width*nDirQt; // 한줄의 길이..
	int nPF = g_pTEng->PixelFormatGet();

	if(width <= 0 || height <= 0)
	{
		MessageBox("스프라이트의 너비와 높이가 잘못되어 있습니다. 작업이 불가능합니다.");
		delete [] pnSprs; pnSprs = NULL;
		return;
	}

	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "bmp", NULL, dwFlags, "24비트 비트맵(*.bmp)|*.bmp||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("BMP");
	if(dlg.DoModal() == IDCANCEL)
	{
		delete [] pnSprs; pnSprs = NULL;
		return;
	}
	CString FileName = dlg.GetFileName();

	// 한줄에 방향갯수만큼 배치
	int nAddLine = 0; // 만약 방향수와 스프라이트의 갯수가 나누기가 딱 맞아떨어지면.. 
	(nCountFrm%nDirQt) ? nAddLine = 1 : nAddLine = 0;  // 그렇지 않다면 한줄을 더 해줘야 한다..

	CDib16 dib;
	dib.New(width*nDirQt, height*(nCountFrm/nDirQt + nAddLine)); // 비트맵 만들기....
	if(m_pAniObj->m_HSpr.SrcCK == 0) // 투명색이 검은색이면 확인한다. 보라색으로 만들수도 있다..
	{
		int id = MessageBox("투명색이 검은색으로 지정되어 있습니다. 투명색을 보라색으로 하시겠습니까?", "투명색 확인", MB_YESNO);
		if(id == IDYES) m_pAniObj->m_HSpr.SrcCK = RGB(255,0,255);
	}
	dib.FillColor(NULL, m_pAniObj->m_HSpr.SrcCK); // 전체 영역을 투명색으로 칠해준다..
	WORD* pwDestOrg = dib.Lock();
	for(i = 0; i < nCountFrm; i++)
	{
		m_pAniObj->SprGet(pnSprs[i], &pSpr);

		// LSP 쓰기
		WORD* pwSrc = pSpr->pwV;
		int nLine = pSpr->nLine; // 줄수
		for(int j = 0; j < nLine; j++)
		{
			WORD* pwDest =	pwDestOrg + // 방향갯수만큼의 이미지가 한 줄로 저장된다.
							((i/nDirQt)*height+j)*nPitch + // 줄
							(i%nDirQt)*width; // 열
			int nNode = *pwSrc; pwSrc++;
			for(int k = 0; k < nNode; k++)
			{
				int nZero = *pwSrc; pwSrc++; pwDest += nZero;
				int nPixel = *pwSrc; pwSrc++;
				for(int l = 0; l < nPixel; l++)
				{
					WORD wPixel = *pwSrc; pwSrc++; 
					if(nPF == PF_565) *pwDest = wPixel;
					else
					{
						*pwDest = ((wPixel&0x7fe0)<<1)|(wPixel&0x001f);
					}
					pwDest++;
				}
			}
		}
	}
	dib.Unlock();
	
	dib.ExportTo24BMP(FileName);
	
	delete [] pnSprs; pnSprs = NULL;

}

// 비트맵을 해당 스프라이트로 Import
void CDlgAniObj::OnBSprImportFromBmp() 
{

	int nAni = m_ListAni.GetCurSel();
	if(nAni == CB_ERR)
	{
		MessageBox("에니메이션을 선택해 주세요");
		return;
	}

	const _ANI_DATA* pAni;
	pAni = m_pAniObj->AniDataGet(nAni);
	if(pAni == NULL)
	{
		MessageBox("에니메이션이 없습니다.");
		return;
	}

	// 중복되는 스프라이트 제거후 리스트 만들기..
	int nDirQt = m_pAniObj->m_HAni.nDirQt;
	if(nDirQt <= 0) return; // 방향갯수가 하나도 없다면 돌아간다..

	int* pnTmps = new int[pAni->nMaxFrm * nDirQt]; // 방향*프레임이다.
	int nCountTmp = 0;
	for(int i = 0; i < nDirQt; i++)
		for(int j = 0; j < pAni->nMaxFrm; j++)
			pnTmps[nCountTmp++] = pAni->pnFrms[i*pAni->nMaxFrm+j]; // 중복되지 않으면 리스트에 추가..
	
	int nCountFrm = 0;
	int *pnSprs = new int[pAni->nMaxFrm * 8]; // 8방향이다.
	for(i = 0; i < nCountTmp; i++)
	{
		BOOL bOverLapped = FALSE;
		for(int j = 0; j < nCountFrm; j++)
		{
			if(pnTmps[i] == pnSprs[j])
			{
				bOverLapped = TRUE;
				break;
			}
		}
		if(bOverLapped == FALSE) pnSprs[nCountFrm++] = pnTmps[i];
	}
	delete [] pnTmps; pnTmps = NULL;
	
	if(nCountFrm <= 0)
	{
		MessageBox("Import할 스프라이트가 없습니다.");
		delete [] pnSprs; pnSprs = NULL;
		return;
	}

	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "bmp", NULL, dwFlags, "24비트 비트맵(*.bmp)|*.bmp||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("BMP");
	if(dlg.DoModal() == IDCANCEL)
	{
		delete [] pnSprs; pnSprs = NULL;
		return;
	}

	CString FileName = dlg.GetFileName();

	// 서피스 로딩... 
	LPDIRECTDRAWSURFACE lpdds = g_pTEng->SurfaceCreateWithBMP(FileName, FALSE, m_pAniObj->m_HSpr.SrcCK);
	if(lpdds == NULL)
	{
		delete [] pnSprs; pnSprs = NULL;
		return;
	}
	
	DDSURFACEDESC ddsd;
	g_pTEng->SurfaceLock(lpdds, &ddsd);
	COLORREF TColor = g_pTEng->RGB24(*((WORD*)(ddsd.lpSurface))); // 0,0 의 점은 투명색이라 여긴다..
	g_pTEng->SurfaceUnlock(lpdds);
	
	CRect rcDDS;
	g_pTEng->SurfaceGetSize(lpdds, &rcDDS);
	
	// 너비와 높이가 맞는지 본다..
	int nAddLine = 0; // 만약 방향수와 스프라이트의 갯수가 나누기가 딱 맞아떨어지면.. 
	(nCountFrm%nDirQt) ? nAddLine = 1 : nAddLine = 0;  // 그렇지 않다면 한줄을 더 해줘야 한다..

	int nWidth = rcDDS.Width() / nDirQt;
	int nHeight = rcDDS.Height() / (nCountFrm/nDirQt+nAddLine);

	POINT ptCenter;
	for(i = 0; i < nCountFrm; i++)
	{
		const _SPR_LSP* pSpr;
		m_pAniObj->SprGet(pnSprs[i], &pSpr);
		_SPR_LSP* pSpr2 = (_SPR_LSP*) pSpr;
		if(pSpr2 == NULL) continue;

		CRect rcEncode; // 추출 영역 정하기...
		rcEncode.left = (i%nDirQt)*nWidth;
		rcEncode.top = (i/nDirQt)*nHeight;
		rcEncode.right = rcEncode.left + nWidth;
		rcEncode.bottom = rcEncode.top + nHeight;

		ptCenter.x = rcEncode.left - pSpr2->rcV.left;
		ptCenter.y = rcEncode.top - pSpr2->rcV.top;

		DELETE_ARRAY_SAFE(pSpr2->pwV);
		g_pTEng->EncodeToLSPData(lpdds, TColor, rcEncode, ptCenter, pSpr2);
	}

	UpdateWorkWindow();
	MessageBox("스프라이트를 저장해야 유효 합니다.");

	RELEASE_SAFE(lpdds);
	delete [] pnSprs; pnSprs = NULL;
}

void CDlgAniObj::OnBSprNew() 
{
	m_pAniObj->SprNew();
	SetDlgItemText(IDC_E_SPR_FILE_NAME, "");

	this->UpdateSprObjInfo();
	this->UpdateSprInfo();

	if(m_pWndWork == NULL) return;

	CRect rcWork; // 사이즈 변경 메시지..
	m_pWndWork->GetWindowRect(&rcWork);
	m_pWndWork->SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(rcWork.Width(), rcWork.Height()));
	this->UpdateWorkWindow();
}

void CDlgAniObj::OnBSprLoad() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "LSP Sprite(*.spl)|*.spl|8Bit LSP Sprite(*.sp8)|*.sp8|Normal Sprite(*.spn)|*.spn||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();
	m_pAniObj->SprLoad(FileName, g_pTEng->PixelFormatGet());
	SetDlgItemText(IDC_E_SPR_FILE_NAME, FileName);

	this->UpdateSprObjInfo();
	this->UpdateSprInfo();

	if(m_pWndWork == NULL) return;

	CRect rcWork; // 사이즈 변경 메시지..
	m_pWndWork->GetWindowRect(&rcWork);
	m_pWndWork->SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(rcWork.Width(), rcWork.Height()));
	this->UpdateWorkWindow();
}

void CDlgAniObj::OnBSprSave() 
{
	CString FileName;
	GetDlgItemText(IDC_E_SPR_FILE_NAME, FileName);
	if(FileName.GetLength() <= 0)
	{
		OnBSprSaveAs();
		return;
	}
	
	g_pTEng->PathSetCur("SPR"); // 경로 지정
	m_pAniObj->SprSave(FileName);
}

void CDlgAniObj::OnBSprSaveAs() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "spl", NULL, dwFlags, "LSP Sprite(*.spl)|*.spl|8Bit LSP Sprite(*.sp8)|*.sp8|Normal Sprite(*.spn)|*.spn||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();
	SetDlgItemText(IDC_E_SPR_FILE_NAME, FileName);
	OnBSprSave();
}

void CDlgAniObj::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(pScrollBar == (CScrollBar*)&m_SliderSpr) // 스프라이트 슬라이더이면..
	{
		this->UpdateSprInfo();
		this->UpdateWorkWindow(); // 작업 윈도우 갱신..
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgAniObj::OnBSprSetQt() 
{
	int nQt = GetDlgItemInt(IDC_E_SPR_QT);
	if(nQt < 0 || nQt >= MAX_SPR) return;
	
	m_pAniObj->m_HSpr.nQt = nQt;
	SetDlgItemInt(IDC_E_SPR_WHOLE, nQt);

	m_SliderSpr.SetRange(0, nQt - 1, TRUE);
}

void CDlgAniObj::OnBSprBrowseBmp() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "bmp", NULL, dwFlags, "24비트 비트맵 파일(*.bmp)|*.bmp|16비트 게임 비트맵 파일(*.obm)|*.obm||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("BMP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();
	
	LPDIRECTDRAWSURFACE lpDDS = g_pTEng->SurfaceCreateWithBMP(FileName, FALSE, m_pAniObj->m_HSpr.SrcCK);
	if(lpDDS == NULL) return;

	RELEASE_SAFE(m_pAniObj->m_lpDDS);
	m_pAniObj->m_lpDDS = lpDDS;
	lstrcpy(m_pAniObj->m_HSpr.szBMPFN, FileName);

	this->UpdateSprObjInfo();
	
	if(m_pWndWork == NULL) return;

	CRect rcWork; // 사이즈 변경 메시지..
	m_pWndWork->GetWindowRect(&rcWork);
	m_pWndWork->SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(rcWork.Width(), rcWork.Height()));
	this->UpdateWorkWindow();
}

void CDlgAniObj::OnCSprCkR() 
{
	m_pAniObj->m_HSpr.SrcCK = 0;

	if(IsDlgButtonChecked(IDC_C_SPR_CK_R)) m_pAniObj->m_HSpr.SrcCK |= RGB(255,0,0);
	if(IsDlgButtonChecked(IDC_C_SPR_CK_G)) m_pAniObj->m_HSpr.SrcCK |= RGB(0,255,0);
	if(IsDlgButtonChecked(IDC_C_SPR_CK_B)) m_pAniObj->m_HSpr.SrcCK |= RGB(0,0,255);

	g_pTEng->SurfaceColorKeySet(m_pAniObj->m_lpDDS, m_pAniObj->m_HSpr.SrcCK);

	this->UpdateWorkWindow();
}

void CDlgAniObj::OnCSprCkG() 
{
	m_pAniObj->m_HSpr.SrcCK = 0;

	if(IsDlgButtonChecked(IDC_C_SPR_CK_R)) m_pAniObj->m_HSpr.SrcCK |= RGB(255,0,0);
	if(IsDlgButtonChecked(IDC_C_SPR_CK_G)) m_pAniObj->m_HSpr.SrcCK |= RGB(0,255,0);
	if(IsDlgButtonChecked(IDC_C_SPR_CK_B)) m_pAniObj->m_HSpr.SrcCK |= RGB(0,0,255);

	g_pTEng->SurfaceColorKeySet(m_pAniObj->m_lpDDS, m_pAniObj->m_HSpr.SrcCK);

	this->UpdateWorkWindow();
}

void CDlgAniObj::OnCSprCkB() 
{
	m_pAniObj->m_HSpr.SrcCK = 0;

	if(IsDlgButtonChecked(IDC_C_SPR_CK_R)) m_pAniObj->m_HSpr.SrcCK |= RGB(255,0,0);
	if(IsDlgButtonChecked(IDC_C_SPR_CK_G)) m_pAniObj->m_HSpr.SrcCK |= RGB(0,255,0);
	if(IsDlgButtonChecked(IDC_C_SPR_CK_B)) m_pAniObj->m_HSpr.SrcCK |= RGB(0,0,255);

	g_pTEng->SurfaceColorKeySet(m_pAniObj->m_lpDDS, m_pAniObj->m_HSpr.SrcCK);

	this->UpdateWorkWindow();
}

void CDlgAniObj::OnSelendokCbWork() 
{
	this->UpdateWorkWindow();	
}

void CDlgAniObj::OnOK()
{

}

void CDlgAniObj::OnCancel()
{

}

void CDlgAniObj::OnBAniSetDirQt() 
{
	int nDirQt = GetDlgItemInt(IDC_E_ANI_DIR_QT);
	
	CString szDir;
	
	m_CBDir.ResetContent(); // 방향 갯수 나타내기
	for(int i = 0; i < nDirQt; i++)
	{
		szDir.Format("%d", i);
		m_CBDir.AddString(szDir);
	}
	m_CBDir.SetCurSel(0);

	m_pAniObj->AniDirQtSet(nDirQt);
	this->UpdateAniInfo();
	UpdateWorkWindow();
}

void CDlgAniObj::OnSelchangeCbDir() 
{
	this->UpdateAniInfo();
	UpdateWorkWindow();
}

