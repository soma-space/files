// DlgLook.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgLook.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLook dialog


CDlgLook::CDlgLook(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLook::IDD, pParent)
{
	m_ptCenter.x = 0; m_ptCenter.y = 0;
	MAKE_ZERO(m_nPartIndices);
	m_pLookClip = NULL;
	
	//{{AFX_DATA_INIT(CDlgLook)
	//}}AFX_DATA_INIT
}

CDlgLook::~CDlgLook()
{
	DELETE_SAFE(m_pLookClip);
}

void CDlgLook::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLook)
	DDX_Control(pDX, IDC_CB_PART, m_CBPart);
	DDX_Control(pDX, IDC_LIST_PART_ANI, m_ListPartAni);

	DDX_Control(pDX, IDC_CB_LOOK, m_CBLook);
	DDX_Control(pDX, IDC_C_ANIMATION, m_CAnimation);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_Preview);
	DDX_Control(pDX, IDC_LIST_ANI, m_ListAni);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLook, CDialog)
	//{{AFX_MSG_MAP(CDlgLook)
	ON_BN_CLICKED(IDC_STATIC_PREVIEW, OnStaticPreview)
	ON_BN_CLICKED(IDC_B_LOAD, OnBLoad)
	ON_BN_CLICKED(IDC_B_NEW, OnBNew)
	ON_BN_CLICKED(IDC_B_SAVE, OnBSave)
	ON_BN_CLICKED(IDC_B_SAVE_AS, OnBSaveAs)
	ON_BN_CLICKED(IDC_B_BROWSE_SPR, OnBBrowseSpr)
	ON_BN_CLICKED(IDC_B_BROWSE_ANI, OnBBrowseAni)
	ON_LBN_SELCHANGE(IDC_LIST_ANI, OnSelchangeListAni)
	ON_BN_CLICKED(IDC_C_ANIMATION, OnCAnimation)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_B_LOOK_QT_SET, OnBLookQtSet)
	ON_BN_CLICKED(IDC_B_PART_QT_SET, OnBPartQtSet)
	ON_BN_CLICKED(IDC_B_ANI_QT_SET, OnBAniQtSet)
	ON_LBN_SELCHANGE(IDC_LIST_PART_ANI, OnSelchangeListPartAni)
	ON_CBN_SELCHANGE(IDC_CB_PART, OnSelchangeCbPart)
	ON_CBN_SELCHANGE(IDC_CB_LOOK, OnSelchangeCbLook)
	ON_BN_CLICKED(IDC_B_COPY, OnBCopy)
	ON_BN_CLICKED(IDC_B_PASTE, OnBPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLook message handlers

void CDlgLook::OnStaticPreview() 
{
	CWnd* pWnd = GetDlgItem(IDC_STATIC_PREVIEW);
	if(pWnd == NULL) return;

	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	pWnd->ScreenToClient(&ptMouse);

	m_ptCenter = ptMouse;

	this->UpdatePreview();
}

void CDlgLook::UpdatePreview()
{
	CRect rcPreview;
	m_Preview.GetClientRect(&rcPreview);

	g_pTEng->SurfaceFillRect(NULL, &rcPreview, RGB(64,64,64));
	g_pTEng->HWndSet(m_Preview.m_hWnd);

	static DWORD dwTickPeriods0[MAX_LOOK_PART] = {0,0,0,0,0,0,0,0,0,0,0,0};
	static DWORD dwTickPeriods1[MAX_LOOK_PART] = {0,0,0,0,0,0,0,0,0,0,0,0};
	DWORD dwTick = GetTickCount();

	int nLook = m_CBLook.GetCurSel();
	_LOOK_DATA* pData = m_ObjLook.DataGet(nLook);

	if(pData == NULL)
	{
		g_pTEng->Flip(&rcPreview);
		return;
	}

	CAniObjLSP* pAniObj = &m_ObjLook.m_AniObj;
	const _SPR_LSP* pSpr;
	g_pTEng->BackBufferLock();
	
	int nPart = m_CBPart.GetCurSel();

	for(int i = 0; i < pData->nQtPart && i < MAX_LOOK_PART; i++) // 각 구성 요소 갯수 만큼 화면에 뿌린다..
	{
		if(m_nPartIndices[i] < 0 || m_nPartIndices[i] >= MAX_LOOK_PART_ANI || m_nPartIndices[i] >= pData->nQtAnisPerParts[i]) continue;
		int nAni = pData->nAnis[i][m_nPartIndices[i]];

		pSpr = NULL;
		if(m_CAnimation.GetCheck() == TRUE)
		{
			if(pData->nPeriods[i] <= 0)
			{
				pAniObj->AniSet(nAni, FL_ANI_LOOP, i);
			}
			else if(dwTick > dwTickPeriods0[i] + dwTickPeriods1[i] + pData->nPeriods[i])
			{
				pAniObj->FrmSet(0, i);
				pAniObj->AniSet(nAni, FL_ANI_ONCE_FIX, i);
				dwTickPeriods0[i] = GetTickCount();
				if(pData->nPeriodsRnd[i] > 0) dwTickPeriods1[i] = rand()%pData->nPeriodsRnd[i];
			}
			pAniObj->SprGetAnimated(&pSpr, i);
		}
		else
		{
			int nSpr = pAniObj->FrmGetByAniAndDirAndSeq(nAni, 0, 0);
			pAniObj->SprGet(nSpr, &pSpr);
		}

		if(pSpr == NULL) continue;
		g_pTEng->BltLSP(NULL, m_ptCenter.x, m_ptCenter.y, pSpr);
	}
	g_pTEng->BackBufferUnlock();

	g_pTEng->Flip(&rcPreview);
}

void CDlgLook::OnBLoad() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "lad", NULL, dwFlags, "Look 조합 데이터(*.lad)|*.lad||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_ObjLook.Load(FileName, g_pTEng->PathGetCur("ANI", 0), g_pTEng->PathGetCur("SPR", 1), g_pTEng->PixelFormatGet());
	
	SetDlgItemText(IDC_E_FILE_NAME, FileName);
	SetDlgItemText(IDC_E_ANI_FILE_NAME, m_ObjLook.m_Header.szAniFN);
	SetDlgItemText(IDC_E_SPR_FILE_NAME, m_ObjLook.m_Header.szSprFN);
	
	this->UpdateLookComboBox();
	this->UpdateAniList();
	this->UpdatePreview();
}

void CDlgLook::OnBNew() 
{
	m_ObjLook.Release();
	m_ObjLook.Alloc(MAX_LOOK_PART);
	
	SetDlgItemText(IDC_E_FILE_NAME, "");
	SetDlgItemText(IDC_E_ANI_FILE_NAME, m_ObjLook.m_Header.szAniFN);
	SetDlgItemText(IDC_E_SPR_FILE_NAME, m_ObjLook.m_Header.szSprFN);
	
	this->UpdateLookComboBox();
	this->UpdateAniList();
	this->UpdatePreview();
}

void CDlgLook::OnBSave() 
{
	CString FileName;
	GetDlgItemText(IDC_E_FILE_NAME, FileName);

	if(FileName.GetLength() <= 0)
	{
		this->OnBSaveAs();
		return;
	}

	g_pTEng->PathSetCur("DATA");
	m_ObjLook.Save(FileName);
}

void CDlgLook::OnBSaveAs() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "lad", NULL, dwFlags, "Look 조합 데이터(*.lad)|*.lad||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	SetDlgItemText(IDC_E_FILE_NAME, FileName);

	this->OnBSave();
}

void CDlgLook::OnBBrowseSpr() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "LSP 스프라이트(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_ObjLook.m_AniObj.SprLoad(FileName, g_pTEng->PixelFormatGet()); // 8방향 스프라이트에서 읽어들임..
	lstrcpy(m_ObjLook.m_Header.szSprFN, FileName);
	SetDlgItemText(IDC_E_SPR_FILE_NAME, FileName);
	this->UpdatePreview();
}

void CDlgLook::OnBBrowseAni() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "ani", NULL, dwFlags, "에니메이션(*.ani)|*.ani||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("ANI");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_ObjLook.m_AniObj.AniLoad(FileName); // 에니메이션 파일에서 읽어들임..
	lstrcpy(m_ObjLook.m_Header.szAniFN, FileName);
	SetDlgItemText(IDC_E_ANI_FILE_NAME, FileName);
	
	this->UpdateAniList();
	this->UpdatePreview();
}

void CDlgLook::OnSelchangeListAni()
{
	this->UpdateLookData();

	int nLook = m_CBLook.GetCurSel();
	if(nLook < 0 || nLook >= MAX_LOOK) return;
	_LOOK_DATA* pData = m_ObjLook.DataGet(nLook);
	if(pData == NULL) return;

	int nPart = m_CBPart.GetCurSel();
	if(nPart < 0 || nPart >= MAX_LOOK_PART) return;

	int nPartAni = m_ListPartAni.GetCurSel();
	if(nPartAni < 0 || nPartAni >= MAX_LOOK_PART_ANI) return;
	
	int nAni = m_ListAni.GetCurSel();
	if(nAni < 0 || nAni >= MAX_ANI) return;
	
	CString str;
	str.Format("%3d : ", nAni);
	_ANI_DATA* pAni = m_ObjLook.m_AniObj.AniDataGet(nAni);
	if(pAni != NULL) str += pAni->szName;
	else str += "에러 - 에니메이션 없음!!";
	m_ListPartAni.DeleteString(nPartAni);
	m_ListPartAni.InsertString(nPartAni, str);
	m_ListPartAni.SetCurSel(nPartAni);

	this->UpdatePreview();
}

BOOL CDlgLook::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rcPreview;
	m_Preview.GetClientRect(&rcPreview);
	m_ptCenter.x = rcPreview.Width()/2;
	m_ptCenter.y = rcPreview.Height()/2;

	m_CBLook.EnableWindow(FALSE);

	// 얼굴 각 부분 이름 넣기...
	m_CBPart.AddString("머리카락 - 뒷 부분");
	m_CBPart.AddString("귀");
	m_CBPart.AddString("얼굴 윤곽");
	m_CBPart.AddString("옷");
	m_CBPart.AddString("눈");
	m_CBPart.AddString("안경");
	m_CBPart.AddString("코");
	m_CBPart.AddString("입");
	m_CBPart.AddString("머리카락 - 앞 부분");
	m_CBPart.AddString("액세서리 - 귀걸이");
	m_CBPart.AddString("액세서리 - 목걸이");
	m_CBPart.AddString("액세서리 - 머리핀");
	m_CBPart.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgLook::UpdateAniList()
{
	m_ListAni.ResetContent();

	int nQt = m_ObjLook.m_AniObj.m_HAni.nQt;
	if(nQt <= 0 || nQt > MAX_ANI) return;

	CAniObjLSP* pAniObj = &m_ObjLook.m_AniObj;
	_ANI_DATA* pAni = NULL;
	for(int i = 0; i < nQt; i++)
	{
		CString szAni;
		pAni = pAniObj->AniDataGet(i);
		if(pAni == NULL) szAni = "에러 - 에니메이션 데이터 없음!!";
		else szAni = pAni->szName;

		if(szAni.GetLength() <= 0) szAni = "이름없음";

		CString szTmp; szTmp.Format("%3d : ", i);
		m_ListAni.AddString(szTmp + szAni);
	}
}

void CDlgLook::OnCAnimation() 
{
	if(m_CAnimation.GetCheck() == FALSE) this->KillTimer(0);
	else this->SetTimer(0, 20, NULL);

	this->UpdatePreview();
}

void CDlgLook::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 0) // 재생..
	{
		this->UpdatePreview();
	}	
	CDialog::OnTimer(nIDEvent);
}

void CDlgLook::OnBLookQtSet()
{
	int nQt = GetDlgItemInt(IDC_E_LOOK_QT);
	if(nQt < 0 || nQt > MAX_LOOK) return;

	m_ObjLook.m_Header.nQt = nQt;

	this->UpdateLookComboBox();
}

void CDlgLook::OnSelchangeListPartAni() 
{
	int nPart = m_CBPart.GetCurSel();
	if(nPart >= 0 && nPart < MAX_LOOK_PART) m_nPartIndices[nPart] = this->m_ListPartAni.GetCurSel();
	
	this->UpdateLookInfo();
	this->UpdatePreview();
}

void CDlgLook::OnSelchangeCbPart() 
{
	this->UpdatePartAniList();
	this->UpdatePreview();
}

void CDlgLook::OnSelchangeCbLook() 
{
	this->UpdatePartComboBox();
	this->UpdatePreview();
}

void CDlgLook::OnBPartQtSet()
{
	this->UpdateLookData();
	this->UpdateLookInfo();
	this->UpdatePreview();
}

void CDlgLook::OnBAniQtSet() 
{
	this->UpdateLookData();
	this->UpdatePartAniList();
	this->UpdatePreview();
}

void CDlgLook::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	this->UpdatePreview();
}

void CDlgLook::UpdateLookData()
{
	int nLook = m_CBLook.GetCurSel();
	if(nLook < 0 || nLook >= MAX_LOOK) return;
	_LOOK_DATA* pData = m_ObjLook.DataGet(nLook);
	if(pData == NULL) return;

	int nPart = m_CBPart.GetCurSel();
	if(nPart < 0 || nPart >= MAX_LOOK_PART) return;
	pData->nPeriods[nPart] = GetDlgItemInt(IDC_E_PERIOD);
	pData->nPeriodsRnd[nPart] = GetDlgItemInt(IDC_E_PERIOD_RND);
	pData->nQtAnisPerParts[nPart] = GetDlgItemInt(IDC_E_ANI_QT);

	int nPartAni = m_ListPartAni.GetCurSel();
	if(nPartAni < 0 || nPartAni >= MAX_LOOK_PART_ANI) return;

	pData->nAnis[nPart][nPartAni] = m_ListAni.GetCurSel();
	pData->nQtPart = GetDlgItemInt(IDC_E_PART_QT);
}

void CDlgLook::UpdateLookInfo()
{
	int nLook = m_CBLook.GetCurSel();
	_LOOK_DATA* pData = m_ObjLook.DataGet(nLook);
	if(pData== NULL) return;

	int nPart = m_CBPart.GetCurSel();
	if(nPart < 0 || nPart >= MAX_LOOK_PART) return;

	SetDlgItemInt(IDC_E_PERIOD, pData->nPeriods[nPart]);
	SetDlgItemInt(IDC_E_PERIOD_RND, pData->nPeriodsRnd[nPart]);
	SetDlgItemInt(IDC_E_ANI_QT, pData->nQtAnisPerParts[nPart]);
	SetDlgItemInt(IDC_E_PART_QT, pData->nQtPart);

	int nPartAni = m_ListPartAni.GetCurSel();
	if(nPartAni < 0 || nPartAni >= MAX_LOOK_PART_ANI) return;

	if(pData->nAnis[nPart][nPartAni] != 0) m_ListAni.SetCurSel(pData->nAnis[nPart][nPartAni]);
}

void CDlgLook::UpdatePartAniList()
{
	int nLook = m_CBLook.GetCurSel();
	_LOOK_DATA* pData = m_ObjLook.DataGet(nLook);
	if(pData== NULL) return;

	int nPart = m_CBPart.GetCurSel();
	if(nPart < 0 || nPart >= MAX_LOOK_PART) return;

	m_ListPartAni.ResetContent();
	CString str;
	for(int i = 0; i < pData->nQtAnisPerParts[nPart]; i++)
	{
		str.Format("%3d : ", pData->nAnis[nPart][i]);
		_ANI_DATA* pAni = m_ObjLook.m_AniObj.AniDataGet(pData->nAnis[nPart][i]);
		if(pAni == NULL) str += "에니메이션 에러!!";
		else str += pAni->szName;

		m_ListPartAni.AddString(str);
	}
	m_ListPartAni.SetCurSel(m_nPartIndices[nPart]);

	this->UpdateLookInfo();
}

void CDlgLook::UpdatePartComboBox()
{
	this->UpdatePartAniList();
}

void CDlgLook::UpdateLookComboBox()
{
	m_CBLook.ResetContent();
	if(m_ObjLook.m_Header.nQt < 0 || m_ObjLook.m_Header.nQt > MAX_LOOK) 
	{
		m_CBLook.EnableWindow(FALSE);
	}
	else
	{
		CString str;
		for(int i = 0; i < m_ObjLook.m_Header.nQt; i++)
		{
			str.Format("표정 : %3d", i);
			m_CBLook.AddString(str);
		}
		m_CBLook.SetCurSel(0);
		m_CBLook.EnableWindow(TRUE);
	}
	SetDlgItemInt(IDC_E_LOOK_QT, m_ObjLook.m_Header.nQt);
	this->UpdatePartComboBox();
}

void CDlgLook::OnBCopy() 
{
	int nLook = m_CBLook.GetCurSel();
	if(nLook < 0 || nLook >= MAX_LOOK) return;
	_LOOK_DATA* pData = m_ObjLook.DataGet(nLook);
	if(pData == NULL) return;

	DELETE_SAFE(m_pLookClip);
	m_pLookClip = new _LOOK_DATA;
	memcpy(m_pLookClip, pData, sizeof(_LOOK_DATA));
}

void CDlgLook::OnBPaste() 
{
	if(m_pLookClip == NULL) return;

	int nLook = m_CBLook.GetCurSel();
	if(nLook < 0 || nLook >= MAX_LOOK) return;
	_LOOK_DATA* pData = m_ObjLook.DataGet(nLook);
	if(pData == NULL) return;

	memcpy(pData, m_pLookClip, sizeof(_LOOK_DATA));
	
	this->UpdatePartComboBox();
	this->UpdatePreview();
}
