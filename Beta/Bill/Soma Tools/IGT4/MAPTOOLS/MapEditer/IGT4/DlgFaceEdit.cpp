// DlgFaceEdit.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgFaceEdit.h"

#include "DDEngineTool4.h"
extern CDDEngineTool4*	g_pTEng;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFaceEdit dialog


CDlgFaceEdit::CDlgFaceEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFaceEdit::IDD, pParent)
{
	m_ptCenter.x = 0; m_ptCenter.y = 0;
	int m_nPartSel = 0;
	MAKE_ZERO(m_pCBs);
	//{{AFX_DATA_INIT(CDlgFaceEdit)
	//}}AFX_DATA_INIT
}


void CDlgFaceEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFaceEdit)
	DDX_Control(pDX, IDC_C_ANIMATION, m_CAnimation);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_Preview);
	DDX_Control(pDX, IDC_LIST_ANI, m_ListAni);
	DDX_Control(pDX, IDC_CB_NOSE, m_CBNose);
	DDX_Control(pDX, IDC_CB_NECKLACE0, m_CBNeckLace0);
	DDX_Control(pDX, IDC_CB_NECKLACE1, m_CBNeckLace1);
	DDX_Control(pDX, IDC_CB_LIPS, m_CBLips);
	DDX_Control(pDX, IDC_CB_HAIRPIN, m_CBHairPin);
	DDX_Control(pDX, IDC_CB_HAIR_REAR, m_CBHairRear);
	DDX_Control(pDX, IDC_CB_HAIR_FRONT, m_CBHairFront);
	DDX_Control(pDX, IDC_CB_FACE, m_CBFace);
	DDX_Control(pDX, IDC_CB_EYE, m_CBEye);
	DDX_Control(pDX, IDC_CB_EARRING, m_CBEarRing);
	DDX_Control(pDX, IDC_CB_EAR, m_CBEar);
	DDX_Control(pDX, IDC_CB_CLOTHES, m_CBClothes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFaceEdit, CDialog)
	//{{AFX_MSG_MAP(CDlgFaceEdit)
	ON_CBN_SETFOCUS(IDC_CB_HAIR_REAR, OnSetFocusCBHairRear)
	ON_CBN_SETFOCUS(IDC_CB_FACE, OnSetFocusCBFace)
	ON_CBN_SETFOCUS(IDC_CB_NECKLACE0, OnSetFocusCBNectLace0)
	ON_CBN_SETFOCUS(IDC_CB_CLOTHES, OnSetFocusCBClothes)
	ON_CBN_SETFOCUS(IDC_CB_NECKLACE1, OnSetFocusCBNectLace1)
	ON_CBN_SETFOCUS(IDC_CB_EYE, OnSetFocusCBEye)
	ON_CBN_SETFOCUS(IDC_CB_NOSE, OnSetFocusCBNose)
	ON_CBN_SETFOCUS(IDC_CB_LIPS, OnSetFocusCBLips)
	ON_CBN_SETFOCUS(IDC_CB_EAR, OnSetFocusCBEar)
	ON_CBN_SETFOCUS(IDC_CB_EARRING, OnSetFocusCBEarring)
	ON_CBN_SETFOCUS(IDC_CB_HAIR_FRONT, OnSetFocusCBHairFront)
	ON_CBN_SETFOCUS(IDC_CB_HAIRPIN, OnSetFocusCBHairpin)
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
	ON_BN_CLICKED(IDC_B_ASSEMBLE_QT_SET, OnBAssembleQtSet)
	ON_CBN_SELENDOK(IDC_CB_HAIR_REAR, OnSetFocusCBHairRear)
	ON_CBN_SELENDOK(IDC_CB_FACE, OnSetFocusCBFace)
	ON_CBN_SELENDOK(IDC_CB_NECKLACE0, OnSetFocusCBNectLace0)
	ON_CBN_SELENDOK(IDC_CB_CLOTHES, OnSetFocusCBClothes)
	ON_CBN_SELENDOK(IDC_CB_NECKLACE1, OnSetFocusCBNectLace1)
	ON_CBN_SELENDOK(IDC_CB_EYE, OnSetFocusCBEye)
	ON_CBN_SELENDOK(IDC_CB_NOSE, OnSetFocusCBNose)
	ON_CBN_SELENDOK(IDC_CB_LIPS, OnSetFocusCBLips)
	ON_CBN_SELENDOK(IDC_CB_EAR, OnSetFocusCBEar)
	ON_CBN_SELENDOK(IDC_CB_EARRING, OnSetFocusCBEarring)
	ON_CBN_SELENDOK(IDC_CB_HAIR_FRONT, OnSetFocusCBHairFront)
	ON_CBN_SELENDOK(IDC_CB_HAIRPIN, OnSetFocusCBHairpin)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFaceEdit message handlers

void CDlgFaceEdit::OnSetFocusCBHairRear() 
{
	m_nPartSel = 0;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBFace()
{
	m_nPartSel = 1;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBNectLace0()
{
	m_nPartSel = 2;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBClothes()
{
	m_nPartSel = 3;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBNectLace1()
{
	m_nPartSel = 4;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBEye()
{
	m_nPartSel = 5;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBNose()
{
	m_nPartSel = 6;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBLips()
{
	m_nPartSel = 7;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBEar()
{
	m_nPartSel = 8;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBEarring()
{
	m_nPartSel = 9;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBHairFront()
{
	m_nPartSel = 10;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSetFocusCBHairpin()
{
	m_nPartSel = 11;
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnStaticPreview() 
{
	CWnd* pWnd = GetDlgItem(IDC_STATIC_PREVIEW);
	if(pWnd == NULL) return;

	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	pWnd->ScreenToClient(&ptMouse);

	m_ptCenter = ptMouse;

	this->UpdatePreview();
}

void CDlgFaceEdit::UpdatePreview()
{
	static DWORD dwTickPeriods0[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	static DWORD dwTickPeriods1[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	DWORD dwTick = GetTickCount();

	CRect rcPreview;
	m_Preview.GetClientRect(&rcPreview);

	g_pTEng->SurfaceFillRect(NULL, &rcPreview, RGB(64,64,64));

	CAniObjLSP* pAniObj = &m_ObjFace.m_AniObj;
	const _SPR_LSP* pSpr;
	g_pTEng->BackBufferLock();
	
	for(int i = 0; i < 12; i++)
	{
		const _FACE_DATA* pData = m_ObjFace.DataGet(i);
		if(pData == NULL) continue;

		int n = m_pCBs[i]->GetCurSel();
		if(n < 0 || n >= MAX_FACE_PART_ASSEMBLE) continue;

		int nAni = pData->nAnis[n];

		pSpr = NULL;
		if(m_CAnimation.GetCheck() == TRUE)
		{
			if(pData->nAniPeriod0 <= 0)
			{
				pAniObj->AniSet(nAni, FL_ANI_LOOP, i);
			}
			else if(dwTick > dwTickPeriods0[i] + dwTickPeriods1[i] + pData->nAniPeriod0)
			{
				pAniObj->FrmSet(0, i);
				pAniObj->AniSet(nAni, FL_ANI_ONCE_FIX, i);
				dwTickPeriods0[i] = GetTickCount();
				if(pData->nAniPeriod1 > 0) dwTickPeriods1[i] = rand()%pData->nAniPeriod1;
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

	g_pTEng->HWndSet(m_Preview.m_hWnd);
	g_pTEng->Flip(&rcPreview);
}

void CDlgFaceEdit::OnBLoad() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "fad", NULL, dwFlags, "Face 조합 데이터(*.fad)|*.fad||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_ObjFace.Load(FileName, g_pTEng->PathGetCur("ANI", 0), g_pTEng->PathGetCur("SPR", 1), g_pTEng->PixelFormatGet());
	
	SetDlgItemText(IDC_E_FILE_NAME, FileName);
	SetDlgItemText(IDC_E_ANI_FILE_NAME, m_ObjFace.m_Header.szAniFN);
	SetDlgItemText(IDC_E_SPR_FILE_NAME, m_ObjFace.m_Header.szSprFN);
	
	this->UpdateAniList();
	this->UpdateFaceWholeInfo();
	this->UpdatePreview();
}

void CDlgFaceEdit::OnBNew() 
{
	m_ObjFace.Release();
	m_ObjFace.Alloc(MAX_FACE_PART);
	
	SetDlgItemText(IDC_E_FILE_NAME, "");
	SetDlgItemText(IDC_E_ANI_FILE_NAME, m_ObjFace.m_Header.szAniFN);
	SetDlgItemText(IDC_E_SPR_FILE_NAME, m_ObjFace.m_Header.szSprFN);
	
	this->UpdateAniList();
	this->UpdateFaceWholeInfo();
	this->UpdatePreview();
}

void CDlgFaceEdit::OnBSave() 
{
	CString FileName;
	GetDlgItemText(IDC_E_FILE_NAME, FileName);

	if(FileName.GetLength() <= 0)
	{
		this->OnBSaveAs();
		return;
	}

	m_ObjFace.m_Header.nQt = 12;
	g_pTEng->PathSetCur("DATA");
	m_ObjFace.Save(FileName);
}

void CDlgFaceEdit::OnBSaveAs() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "fad", NULL, dwFlags, "Face 조합 데이터(*.fad)|*.fad||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	SetDlgItemText(IDC_E_FILE_NAME, FileName);

	this->OnBSave();
}

void CDlgFaceEdit::OnBBrowseSpr() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "LSP 스프라이트(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_ObjFace.m_AniObj.SprLoad(FileName, g_pTEng->PixelFormatGet()); // 8방향 스프라이트에서 읽어들임..
	lstrcpy(m_ObjFace.m_Header.szSprFN, FileName);
	SetDlgItemText(IDC_E_SPR_FILE_NAME, FileName);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnBBrowseAni() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "ani", NULL, dwFlags, "에니메이션(*.ani)|*.ani||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("ANI");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_ObjFace.m_AniObj.AniLoad(FileName); // 에니메이션 파일에서 읽어들임..
	lstrcpy(m_ObjFace.m_Header.szAniFN, FileName);
	SetDlgItemText(IDC_E_ANI_FILE_NAME, FileName);
	
	this->UpdateAniList();
	this->UpdatePreview();
}

void CDlgFaceEdit::OnSelchangeListAni() 
{
	if(m_nPartSel < 0 || m_nPartSel >= 12) return;

	this->UpdateFaceData();
	this->UpdatePreview();
}

BOOL CDlgFaceEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rcPreview;
	m_Preview.GetClientRect(&rcPreview);
	m_ptCenter.x = rcPreview.Width()/2;
	m_ptCenter.y = rcPreview.Height()/2;
	
	this->UpdateFaceWholeInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFaceEdit::UpdateAniList()
{
	m_ListAni.ResetContent();

	int nQt = m_ObjFace.m_AniObj.m_HAni.nQt;
	if(nQt <= 0 || nQt > MAX_ANI) return;

	CAniObjLSP* pAniObj = &m_ObjFace.m_AniObj;
	const _ANI8_DATA* pAni = NULL;
	for(int i = 0; i < nQt; i++)
	{
		CString szAni;
		pAni = pAniObj->AniDataGet(i);
		if(pAni == NULL) szAni = "에러 : 에니메이션 데이터 없음";
		else szAni = pAni->szName;

		if(szAni.GetLength() <= 0) szAni = "이름없음";

		CString szTmp; szTmp.Format("%3d : ", i);
		m_ListAni.AddString(szTmp + szAni);
	}
}

void CDlgFaceEdit::UpdateFaceData()
{
	if(m_nPartSel < 0 || m_nPartSel >= 12) return;

	_FACE_DATA* pData = (_FACE_DATA*)m_ObjFace.DataGet(m_nPartSel);
	if(pData == NULL) return;

	pData->nQt = GetDlgItemInt(IDC_E_FACE_PART_ASSEMBLE_QT);
	pData->nAniPeriod0 = GetDlgItemInt(IDC_E_ANI_PERIOD0);
	pData->nAniPeriod1 = GetDlgItemInt(IDC_E_ANI_PERIOD1);

	int nIndex = m_pCBs[m_nPartSel]->GetCurSel();
	if(nIndex >= 0 && nIndex < pData->nQt) // 현재 선택된 얼굴 부품 콤보박스 데이터를 갱신
	{
		int nAni = pData->nAnis[nIndex] = m_ListAni.GetCurSel();
		CString szAni; m_ListAni.GetText(nAni, szAni);
		if(nAni != 0) m_ListAni.SetCurSel(nAni); // 자꾸 움직여서 귀찮아 죽겠구만...

		m_pCBs[m_nPartSel]->DeleteString(nIndex);
		m_pCBs[m_nPartSel]->InsertString(nIndex, szAni);
		m_pCBs[m_nPartSel]->SetItemData(nIndex, nAni);
		m_pCBs[m_nPartSel]->SetCurSel(nIndex);
	}
}

void CDlgFaceEdit::UpdateFaceSingleInfo(int index)
{
	if(index < 0 || index >= 12) return;
	_FACE_DATA* pData = (_FACE_DATA*)m_ObjFace.DataGet(index);
	if(pData == NULL) return;
	
	SetDlgItemInt(IDC_E_FACE_PART_ASSEMBLE_QT, pData->nQt);
	SetDlgItemInt(IDC_E_ANI_PERIOD0, pData->nAniPeriod0);
	SetDlgItemInt(IDC_E_ANI_PERIOD1, pData->nAniPeriod1);

	int nSel = m_pCBs[index]->GetCurSel();
	if(nSel >= 0 && nSel < MAX_FACE_PART_ASSEMBLE)
	{
		if(pData->nAnis[nSel] != 0) m_ListAni.SetCurSel(pData->nAnis[nSel]);
	}
}

void CDlgFaceEdit::UpdateFaceComboBox(int index)
{
	if(index < 0 || index >= 12) return;
	_FACE_DATA* pData = (_FACE_DATA*)m_ObjFace.DataGet(index);
	if(pData == NULL) return;

	if(pData->nQt < 0) pData->nQt = 0;
	if(pData->nQt > MAX_FACE_PART_ASSEMBLE) pData->nQt = MAX_FACE_PART_ASSEMBLE;
	
	m_pCBs[index]->ResetContent();
	for(int i = 0; i < pData->nQt; i++)
	{
		int nAni = pData->nAnis[i];
		CString szAni; m_ListAni.GetText(nAni, szAni);
		m_pCBs[index]->AddString(szAni);
		m_pCBs[index]->SetItemData(i, nAni);
	}
	m_pCBs[index]->SetCurSel(0);
}

void CDlgFaceEdit::UpdateFaceWholeInfo()
{
	m_nPartSel = 0;

	m_pCBs[0] = &m_CBHairRear;
	m_pCBs[1] = &m_CBFace;
	m_pCBs[2] = &m_CBNeckLace0;
	m_pCBs[3] = &m_CBClothes;
	m_pCBs[4] = &m_CBNeckLace1;
	m_pCBs[5] = &m_CBEye;
	m_pCBs[6] = &m_CBNose;
	m_pCBs[7] = &m_CBLips;
	m_pCBs[8] = &m_CBEar;
	m_pCBs[9] = &m_CBEarRing;
	m_pCBs[10] = &m_CBHairFront;
	m_pCBs[11] = &m_CBHairPin;

	for(int i = 11; i >= 0; i--)
	{
		m_nPartSel = i;
		this->UpdateFaceSingleInfo(i);
		this->UpdateFaceComboBox(i);
	}
}

void CDlgFaceEdit::OnCAnimation() 
{
	if(m_CAnimation.GetCheck() == FALSE) this->KillTimer(0);
	else this->SetTimer(0, 20, NULL);

	this->UpdatePreview();
}

void CDlgFaceEdit::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 0) // 재생..
	{
		this->UpdatePreview();
	}	
	CDialog::OnTimer(nIDEvent);
}

void CDlgFaceEdit::OnBAssembleQtSet() 
{
	this->UpdateFaceData();
	this->UpdateFaceSingleInfo(m_nPartSel);
	this->UpdateFaceComboBox(m_nPartSel);
	this->UpdatePreview();
}

void CDlgFaceEdit::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	this->UpdatePreview();
	
	// Do not call CDialog::OnPaint() for painting messages
}

