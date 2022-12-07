// DlgTileSet.cpp : implementation file
//

#include "stdafx.h"
#include "TileSetTool.h"
#include "DlgTileMap.h"
#include "DlgColor.h"

#include "GlobalVar.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTileMap dialog

CDlgTileMap::CDlgTileMap(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTileMap::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTileMap)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgTileMap::~CDlgTileMap()
{
}

void CDlgTileMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTileMap)
	DDX_Control(pDX, IDC_E_TILE_SET_NAME, m_EditTileSetName);
	DDX_Control(pDX, IDC_E_TILE_SUB_SET_NAME, m_EditTileSubSetName);
	DDX_Control(pDX, IDC_LIST_TILE_SUB_SET, m_ListTSS);
	DDX_Control(pDX, IDC_LIST_TILE_SET, m_ListTS);
	DDX_Control(pDX, IDC_CB_WORK, m_CBWork);
	DDX_Control(pDX, IDC_C_BRUSH_ERASE, m_CBrushErase);
	DDX_Control(pDX, IDC_CB_BRUSH, m_CBBrush);
	DDX_Control(pDX, IDC_CHECK_LAYER_HIDE, m_CLayerHide);
	DDX_Control(pDX, IDC_LB_LAYER, m_LBLayer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTileMap, CDialog)
	//{{AFX_MSG_MAP(CDlgTileMap)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_B_BROWSE_BMP, OnBBrowseBmp)
	ON_BN_CLICKED(IDC_B_NEW_TILE_SET, OnBNewTileSet)
	ON_BN_CLICKED(IDC_B_LOAD_TILE_SET, OnBLoadTileSet)
	ON_BN_CLICKED(IDC_B_SAVE_TILE_SET, OnBSaveTileSet)
	ON_BN_CLICKED(IDC_B_SAVE_AS_TILE_SET, OnBSaveAsTileSet)
	ON_BN_CLICKED(IDC_CHECK_VIEW_GRID, OnCheckViewGrid)
	ON_BN_CLICKED(IDC_CHECK_TILE_SET_T_R, OnCheckTileSetTR)
	ON_BN_CLICKED(IDC_CHECK_TILE_SET_T_G, OnCheckTileSetTG)
	ON_BN_CLICKED(IDC_CHECK_TILE_SET_T_B, OnCheckTileSetTB)
	ON_BN_CLICKED(IDC_B_NEW_TILE_MAP, OnBNewTileMap)
	ON_BN_CLICKED(IDC_B_LOAD_TILE_MAP, OnBLoadTileMap)
	ON_BN_CLICKED(IDC_B_SAVE_TILE_MAP, OnBSaveTileMap)
	ON_BN_CLICKED(IDC_B_SAVE_AS_TILE_MAP, OnBSaveAsTileMap)
	ON_BN_CLICKED(IDC_CHECK_LAYER_HIDE, OnCheckLayerHide)
	ON_LBN_SELCHANGE(IDC_LB_LAYER, OnSelchangeLLayer)
	ON_BN_CLICKED(IDC_B_TILE_MAP_SET_SIZE, OnBTileMapSetSize)
	ON_BN_CLICKED(IDC_B_TILE_MAP_INIT_RANDOM, OnBTileMapInitRandom)
	ON_BN_CLICKED(IDC_B_TILE_MAP_INIT, OnBTileMapInit)
	ON_BN_CLICKED(IDC_CHECK_VIEW_ATTR, OnCheckViewAttr)
	ON_CBN_SELENDOK(IDC_CB_WORK, OnSelendokCbWork)
	ON_LBN_SELCHANGE(IDC_LIST_TILE_SET, OnSelchangeListTileSet)
	ON_LBN_SELCHANGE(IDC_LIST_TILE_SUB_SET, OnSelchangeListTileSubSet)
	ON_EN_CHANGE(IDC_E_TILE_SET_NAME, OnChangeETileSetName)
	ON_EN_CHANGE(IDC_E_TILE_SUB_SET_NAME, OnChangeETileSubSetName)
	ON_BN_CLICKED(IDC_B_TILE_SET_IMPORT, OnBTileSetImport)
	ON_BN_CLICKED(IDC_B_TILE_SET_EXPORT, OnBTileSetExport)
	ON_BN_CLICKED(IDC_B_SAVE_TILE_MAP_TO_GAME_FORMAT, OnBSaveTileMapToGameFormat)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_NO_EDIT, OnRadioNoEdit)
	ON_BN_CLICKED(IDC_RADIO_MOVE_EDIT, OnRadioMoveEdit)
	ON_BN_CLICKED(IDC_RADIO_HOUSE_EDIT, OnRadioHouseEdit)
	ON_BN_CLICKED(IDC_RADIO_EVENT_EDIT, OnRadioEventEdit)
	ON_BN_CLICKED(IDC_ACT_ANI, OnActAni)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTileMap message handlers

void CDlgTileMap::OnBBrowseBmp() 
{
	int nTS = m_ListTS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "bmp", NULL, dwFlags, "24비트 비트맵 파일(*.bmp)|*.bmp|16비트 게임 비트맵 파일(*.obm)|*.obm||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("BMP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();
	
	// 비트맵 로딩
	LPDIRECTDRAWSURFACE lpdds = g_pTEng->SurfaceCreateWithBMP(FileName, FALSE, 0);
	if(lpdds == NULL) return;
	
	CRect rcTS;
	g_pTEng->SurfaceGetSize(lpdds, &rcTS);
	if(rcTS.Width()%32 != 0 || rcTS.Height()%32 != 0)
	{
		MessageBox(FileName, "비트맵의 크기는 32x32 단위이어야 합니다.");
		RELEASE_SAFE(lpdds);
		return;
	}

	RELEASE_SAFE(g_pTSet->m_lpDDSs[nTS]);
	g_pTSet->m_lpDDSs[nTS] = lpdds;

	// 헤더 설정..
	lstrcpy(g_pTSet->m_HTSs[nTS].szBMPFN, FileName);

	// 자동으로 이름 정하기.
	if(lstrlen(g_pTSet->m_HTSs[nTS].szName) <= 0)
		lstrcpy(g_pTSet->m_HTSs[nTS].szName, FileName.Left(FileName.GetLength() - 4)); // 이름 정하기

	// 컬러키 설정(0,0 좌표의 픽셀값이다.
	DDSURFACEDESC ddsd;
	g_pTEng->SurfaceLock(g_pTSet->m_lpDDSs[nTS], &ddsd);
	WORD wCK = *((WORD*)ddsd.lpSurface);
	g_pTSet->m_HTSs[nTS].crCK = g_pTEng->RGB24(wCK);
	g_pTEng->SurfaceColorKeySet(g_pTSet->m_lpDDSs[nTS], g_pTSet->m_HTSs[nTS].crCK);
	g_pTEng->SurfaceUnlock(g_pTSet->m_lpDDSs[nTS]);

	this->UpdateTileSetInfo();
	this->UpdateTileSetPreviewWindow();
	UpdateWorkWindow();
}


void CDlgTileMap::UpdateWorkWindow()
{
	GetParent()->InvalidateRect(NULL);
}

void CDlgTileMap::UpdateDlg()
{
}

BOOL CDlgTileMap::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// 자동 백업용 타이머 작동
	this->SetTimer(0, 600000, NULL); // 10 분에 한번씩 자동 백업

	// 타일셋 프리뷰 윈도우...
	if(m_WndTSPreview.GetSafeHwnd() == NULL)
	{
		HCURSOR hCur = LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_C_NORMAL));
		CString szWndClass;
		szWndClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, hCur, NULL, 0);
		m_WndTSPreview.CreateEx(NULL,
								szWndClass,
								"타일셋 프리뷰",
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								256,
								256,
								this->m_hWnd,
								NULL);
	}
	m_WndTSPreview.ShowWindow(SW_SHOW);

	// 레이어 리스트 박스 세팅.
	CString szItem;
	CString szLayer[MAX_TILE_MAP_LAYER];
	szLayer[0] = "기본 맵"; szLayer[1] = "겹치는 맵"; szLayer[2] = "그림자"; szLayer[3] = "오브젝트 1", szLayer[4] = "오브젝트 2";
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		szItem.Format("레이어 %d", i);
		szItem += szLayer[i];
		m_LBLayer.AddString(szItem);
		m_LBLayer.SetItemData(i, 0); // 트루면 숨기기...
	}
	m_LBLayer.SetCurSel(0);

	// 브러쉬 콤보 박스 세팅
	for(i = 0; i <= 3; i++)
	{
		szItem.Format("%d x %d", i*2+1, i*2+1);
		m_CBBrush.AddString(szItem);
	}
	m_CBBrush.SetCurSel(0);

	// 작업 종류..
	m_CBWork.SetCurSel(0);
	
	CheckRadioButton(IDC_RADIO_NO_EDIT, IDC_RADIO_EVENT_EDIT, IDC_RADIO_NO_EDIT);
	
	this->UpdateTileSetList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTileMap::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CDlgTileMap::UpdateTileSetPreviewWindow()
{
	int nTS = m_ListTS.GetCurSel();
	int nTSS = m_ListTSS.GetCurSel();

	m_WndTSPreview.TileSetCur(nTS, nTSS);
	
	_TILE_SUB_SET_DATA_TOOL* pData = g_pTSet->DataGetSafe(nTS, nTSS);
	int width = 0, height = 0;
	if(pData)
	{
		CRect rcAdjust(0, 0, pData->nWidth*TILE_SIZE, pData->nHeight*TILE_SIZE);
		::AdjustWindowRect(&rcAdjust, m_WndTSPreview.GetStyle(), FALSE);
		width = rcAdjust.Width();
		height = rcAdjust.Height();
	}
	m_WndTSPreview.SetWindowPos(NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
	m_WndTSPreview.InvalidateRect(NULL, FALSE);
}

void CDlgTileMap::OnBNewTileSet() 
{
	SetDlgItemText(IDC_E_FILE_NAME_TILE_SET, "");
	
	DELETE_SAFE(g_pTSet); g_pTSet = new CTileSetTool();
	this->UpdateTileSetList();
	g_WndTMapPreview.UpdateData(NULL); // 미니 맵 업데이트
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);
	UpdateWorkWindow();
}

void CDlgTileMap::OnBLoadTileSet() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "tsd", NULL, dwFlags, "타일 셋 데이터(*.tsd)|*.tsd||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	SetDlgItemText(IDC_E_FILE_NAME_TILE_SET, FileName);

	g_pTSet->Load(FileName);
	
	g_pTEng->PathSetCur("BMP"); // 서피스 로딩...

	this->UpdateTileSetList();
	g_WndTMapPreview.UpdateData(NULL); // 미니 맵 업데이트
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);
	UpdateWorkWindow();
}

void CDlgTileMap::OnBSaveTileSet() 
{
	CString FileName;
	GetDlgItemText(IDC_E_FILE_NAME_TILE_SET, FileName);
	if(!FileName.GetLength())
	{
		OnBSaveAsTileSet();
		return;
	}

	g_pTEng->PathSetCur("MAP");
	g_pTSet->Save(FileName);
}

void CDlgTileMap::OnBSaveAsTileSet() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "tsd", NULL, dwFlags, "타일 셋 데이터(*.tsd)|*.tsd||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	SetDlgItemText(IDC_E_FILE_NAME_TILE_SET, FileName);

	OnBSaveTileSet();
}

void CDlgTileMap::OnCheckViewGrid() 
{
	this->UpdateTileSetPreviewWindow();
	UpdateWorkWindow();
}

void CDlgTileMap::OnCheckViewAttr() 
{
	this->UpdateTileSetPreviewWindow();
	UpdateWorkWindow();
}

void CDlgTileMap::OnCheckTileSetTR() 
{
	UpdateTransparentColor();
}

void CDlgTileMap::OnCheckTileSetTG() 
{
	UpdateTransparentColor();
}

void CDlgTileMap::OnCheckTileSetTB() 
{
	UpdateTransparentColor();
}

void CDlgTileMap::UpdateTransparentColor()
{
	int nTS = m_ListTS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;

	BOOL bR = this->IsDlgButtonChecked(IDC_CHECK_TILE_SET_T_R);
	BOOL bG = this->IsDlgButtonChecked(IDC_CHECK_TILE_SET_T_G);
	BOOL bB = this->IsDlgButtonChecked(IDC_CHECK_TILE_SET_T_B);
	
	g_pTSet->m_HTSs[nTS].crCK = 0;
	if(bR) g_pTSet->m_HTSs[nTS].crCK |= 0x000000ff;
	if(bG) g_pTSet->m_HTSs[nTS].crCK |= 0x0000ff00;
	if(bB) g_pTSet->m_HTSs[nTS].crCK |= 0x00ff0000;

	if(g_pTSet->m_lpDDSs[nTS]) g_pTEng->SurfaceColorKeySet(g_pTSet->m_lpDDSs[nTS], g_pTSet->m_HTSs[nTS].crCK);
	this->UpdateTileSetPreviewWindow();
	UpdateWorkWindow();
}

void CDlgTileMap::UpdateTileSetList()
{
	m_ListTS.ResetContent();
	CString szTS;
	for(int i = 0; i < MAX_TILE_SET; i++)
	{
		m_ListTS.AddString(g_pTSet->m_HTSs[i].szName);
	}
	m_ListTS.SetCurSel(0);

	this->UpdateTileSetInfo(); // 정보 업데이트
}

void CDlgTileMap::UpdateTileSetInfo()
{
	int nTS = m_ListTS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;

	// 타일셋 이름 업데이트
	m_ListTS.DeleteString(nTS);
	m_ListTS.InsertString(nTS, g_pTSet->m_HTSs[nTS].szName);
	m_ListTS.SetCurSel(nTS);

	// 비트맵 파일 이름.
	SetDlgItemText(IDC_BMP_FILE_NAME_TILE_SET, g_pTSet->m_HTSs[nTS].szBMPFN); 
	
	// 컬러키
	if(g_pTSet->m_HTSs[nTS].crCK & 0x000000ff) this->CheckDlgButton(IDC_CHECK_TILE_SET_T_R, TRUE);
	else this->CheckDlgButton(IDC_CHECK_TILE_SET_T_R, FALSE);
	if(g_pTSet->m_HTSs[nTS].crCK & 0x0000ff00) this->CheckDlgButton(IDC_CHECK_TILE_SET_T_G, TRUE);
	else this->CheckDlgButton(IDC_CHECK_TILE_SET_T_G, FALSE);
	if(g_pTSet->m_HTSs[nTS].crCK & 0x0ff00000) this->CheckDlgButton(IDC_CHECK_TILE_SET_T_B, TRUE);
	else this->CheckDlgButton(IDC_CHECK_TILE_SET_T_B, FALSE);
		
	m_bDontUpdateName = TRUE;
	m_EditTileSetName.SetWindowText(g_pTSet->m_HTSs[nTS].szName); // 번호 바꿔주기..
	SetDlgItemInt(IDC_E_TILE_SET_NUMBER, nTS);
	m_bDontUpdateName = FALSE;

	this->UpdateTileSubSetList(); // 서브셋 리스트 업데이트
}

void CDlgTileMap::UpdateTileSubSetList()
{
	// 타일 서브셋 리스트 업데이트
	int nTS = m_ListTS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;

	m_ListTSS.ResetContent();
	CString szTSS;
	_TILE_SUB_SET_DATA_TOOL* pTSSData;
	for(int i = 0; i < MAX_TILE_SUB_SET; i++)
	{
		pTSSData = g_pTSet->DataGetSafe(nTS, i);
		if(pTSSData) szTSS = pTSSData->szRemark; 
		else szTSS = "";
		m_ListTSS.AddString(szTSS);
	}
	m_ListTSS.SetCurSel(0);

	this->UpdateTileSubSetInfo(); // 서브셋 정보 업데이트
}

void CDlgTileMap::UpdateTileSubSetInfo()
{
	// 서브셋 정보 업데이트
	int nTS = m_ListTS.GetCurSel();
	int nTSS = m_ListTSS.GetCurSel();

	_TILE_SUB_SET_DATA_TOOL* pTSSData = g_pTSet->DataGet(nTS, nTSS);
	if(pTSSData == NULL) return;

	int w = 0, h = 0;
	if(pTSSData)
	{
		w = pTSSData->nWidth;
		h = pTSSData->nHeight;
	}
	
	SetDlgItemInt(IDC_EDIT_TILE_SUB_SET_WIDTH, w); // 너비
	SetDlgItemInt(IDC_EDIT_TILE_SUB_SET_HEIGHT, h); // 높이
}

void CDlgTileMap::OnBNewTileMap() 
{
	DELETE_SAFE(g_pTMap); g_pTMap = new CTileMapTool();
	DELETE_SAFE(g_pTMove); g_pTMove = new MoveCell();
	
	UpdateDlg();
	UpdateWorkWindow();
	SetDlgItemText(IDC_E_FILE_NAME_TILE_MAP, "");
	SetDlgItemInt(IDC_EDIT_TILE_MAP_WIDTH, 0);
	SetDlgItemInt(IDC_EDIT_TILE_MAP_HEIGHT, 0);

	g_WndTMapPreview.Alloc(0, 0); // 미니맵 업데이트..
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);

	// 스크롤 다시 계산..
	CRect rcParent;
	GetParent()->GetClientRect(&rcParent);
	GetParent()->SendMessage(WM_SIZE, NULL, MAKELPARAM(rcParent.Width(), rcParent.Height()));
}

void CDlgTileMap::OnBLoadTileMap() 
{
	CString FileName, str;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "tmd", NULL, dwFlags, "타일 맵 데이터(*.tmd)|*.tmd||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	this->OnBNewTileMap(); // 새로
	g_pTMap->Release();
	g_pTMap->LoadFromToolData(FileName);
	
	str = FileName;
	str = str.Left(str.GetLength() - 3);
	str += "imf";
	if(g_pTMove->Load((LPCTSTR)str) == FALSE)
	{
		g_pTMove->Initialize(g_pTMap->m_HTMap.nWidth, g_pTMap->m_HTMap.nHeight*2);
	}

	this->UpdateTileSetList();
	UpdateWorkWindow();
	
	SetDlgItemText(IDC_E_FILE_NAME_TILE_MAP, FileName);
	SetDlgItemInt(IDC_EDIT_TILE_MAP_WIDTH, g_pTMap->m_HTMap.nWidth);
	SetDlgItemInt(IDC_EDIT_TILE_MAP_HEIGHT, g_pTMap->m_HTMap.nHeight);

	// 스크롤 다시 계산..
	CRect rcParent;
	GetParent()->GetClientRect(&rcParent);
	GetParent()->SendMessage(WM_SIZE, NULL, MAKELPARAM(rcParent.Width(), rcParent.Height()));

	// 미니맵 업데이트..
	g_WndTMapPreview.Alloc(g_pTMap->m_HTMap.nWidth, g_pTMap->m_HTMap.nHeight);
	g_WndTMapPreview.UpdateData(NULL);
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);
}

void CDlgTileMap::OnBSaveTileMap() 
{
	CString FileName;
	GetDlgItemText(IDC_E_FILE_NAME_TILE_MAP, FileName);
	if(!FileName.GetLength())
	{
		this->OnBSaveAsTileMap();
		return;
	}

	g_pTEng->PathSetCur("MAP");
	g_pTMap->SaveToToolData(FileName);

	CString str = FileName;
	
	str = str.Left(str.GetLength() - 3);
	str += "imf";
	g_pTMove->Save((LPCTSTR)str);
}

void CDlgTileMap::OnBSaveAsTileMap() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "tmd", NULL, dwFlags, "타일 맵 데이터(*.tmd)|*.tmd||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	SetDlgItemText(IDC_E_FILE_NAME_TILE_MAP, FileName);
	CString szFN2 = FileName.Left(FileName.GetLength() - 3) + "gmd";

	OnBSaveTileMap();
}

void CDlgTileMap::OnCheckLayerHide() 
{
	int nCurLayer = m_LBLayer.GetCurSel();
	if(nCurLayer < 0 || nCurLayer >= MAX_TILE_MAP_LAYER) return;
	
	BOOL bHideLayer = m_CLayerHide.GetCheck();
	m_LBLayer.SetItemData(nCurLayer, bHideLayer);
	
	UpdateWorkWindow();
}

void CDlgTileMap::OnSelchangeLLayer() 
{
	int nCurLayer = m_LBLayer.GetCurSel();
	if(nCurLayer < 0 || nCurLayer >= MAX_TILE_MAP_LAYER) return;

	BOOL bHideLayer = m_LBLayer.GetItemData(nCurLayer);
	this->m_CLayerHide.SetCheck(bHideLayer);
}

void CDlgTileMap::OnBTileMapSetSize() 
{
	int nWidth = GetDlgItemInt(IDC_EDIT_TILE_MAP_WIDTH);
	int nHeight= GetDlgItemInt(IDC_EDIT_TILE_MAP_HEIGHT);

	if(	nWidth <= 0 || nWidth > MAX_TILE_MAP_WIDTH || nHeight <= 0 || nHeight > MAX_TILE_MAP_HEIGHT) return;

	g_pTMap->ReAlloc(nWidth, nHeight); // 타일맵 다시 할당.
	g_pTMove->Initialize(nWidth, nHeight*2);
	
	// 스크롤 다시 계산..
	CRect rcParent;
	GetParent()->GetClientRect(&rcParent);
	GetParent()->SendMessage(WM_SIZE, NULL, MAKELPARAM(rcParent.Width(), rcParent.Height()));

	// 미니맵 업데이트..
	g_WndTMapPreview.Alloc(g_pTMap->m_HTMap.nWidth, g_pTMap->m_HTMap.nHeight); 
	g_WndTMapPreview.UpdateData(NULL);
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);

	UpdateWorkWindow();
}

void CDlgTileMap::OnBTileMapInitRandom() 
{
	int nLayer = m_LBLayer.GetCurSel();
	if(nLayer < 0 || nLayer >= MAX_TILE_MAP_LAYER) return;

	CString szTile;
	GetDlgItemText(IDC_EDIT_TILE_MAP_INIT_RANDOM, szTile);
	if(szTile.GetLength() <= 0)
	{
		MessageBox("밑 칸에 확률%타일셋번호-타일서브셋번호, .... 의 형식으로 채워 주세요.");
		return;
	}

	int nProbs[32]; MAKE_ZERO(nProbs);
	int nTSs[32]; MAKE_ZERO(nTSs);
	int nTSSs[32]; MAKE_ZERO(nTSSs);

	CString szTmp, szProb, szTS, szTSS;
	int nStart = 0;
	int nPrev = nStart;
	int nCount = 0;
	BOOL bBreak = FALSE;
	while(TRUE)
	{
		nStart = szTile.Find(',', nStart+1);
		if(nStart == -1)
		{
			bBreak = TRUE;
			szTmp = szTile.Right(szTile.GetLength() - nPrev);
		}
		else
		{
			szTmp = szTile.Mid(nPrev, nStart - nPrev);
		}
		int nProb = szTmp.Find('%');
		int nTmp = szTmp.Find('-');
		if(nProb != -1 && nTmp != -1)
		{
			szProb = szTmp.Left(nProb);
			nProbs[nCount] = atoi(szProb);
			szTS = szTmp.Mid(nProb + 1, nTmp - nProb - 1);
			nTSs[nCount] = atoi(szTS);
			szTSS = szTmp.Right(szTmp.GetLength() - nTmp - 1);
			nTSSs[nCount] = atoi(szTSS);
			nCount++;
		}
		nStart++;
		nPrev = nStart;
		if(bBreak == TRUE) break;
	}

	CString szErr; // 타일셋 번호와 서브셋 번호 검사..
	for(int i = 0; i < nCount; i++)
	{
		if(nTSs[i] < 0 || nTSs[i] >= MAX_TILE_SET || nTSSs[i] < 0 || nTSSs[i] >= MAX_TILE_SUB_SET)
		{
			szErr.Format("%d 번째 타일셋(혹은 서브셋)이 잘못 되어 있습니다.", i);
			MessageBox(szErr, "맵 초기화 에러");
			return;
		}
		else if(g_pTSet->DataGetSafe(nTSs[i], nTSSs[i]) == NULL)
		{
			szErr.Format("%d 번째에 지정된 %d 번째 타일셋의 %d 번째 서브셋이 없습니다.", i, nTSs[i], nTSSs[i]);
			MessageBox(szErr, "맵 초기화 에러");
			return;
		}
	}

	// 각 서브셋의 크기가 일치하는지 점검.
	_TILE_SUB_SET_DATA_TOOL* pTSS0 = g_pTSet->DataGetSafe(nTSs[0], nTSSs[0]);
	if(pTSS0->nWidth <= 0 || pTSS0->nWidth <= 0)
	{
		szErr.Format("타일 서브셋의 너비와 높이가 0 입니다.");
		MessageBox(szErr);
		return;
	}

	// 랜덤하게 나오려면 처리해주어야 한다.. 아울러 서브셋 크기가 일치하는지  점검
	int nRndMax = 0;

	int nRnds[32]; MAKE_ZERO(nRnds);
	for(i = 0; i < nCount; i++)
	{
		_TILE_SUB_SET_DATA_TOOL* pTSS = g_pTSet->DataGetSafe(nTSs[i], nTSSs[i]);
		if(pTSS0->nWidth != pTSS->nWidth || pTSS0->nWidth != pTSS->nWidth)
		{
			szErr.Format("타일 서브셋의 크기가 일치하지 않습니다.");
			MessageBox(szErr);
			return;
		}
		if(nProbs[i] <= 0) 
		{
			szErr.Format("각 타일 서브셋의 확률이 0 보다 작거나 없습니다.");
			MessageBox(szErr);
			return;
		}
		nRndMax += nProbs[i];
		nRnds[i+1] = nRndMax;
	}


	int nW = g_pTMap->m_HTMap.nWidth/pTSS0->nWidth, nH = g_pTMap->m_HTMap.nHeight/pTSS0->nHeight;
	_TILE_MAP_DATA_TOOL* pMData;
	for(int y = 0; y < nH; y++)
	{
		for(int x = 0; x < nW; x++)
		{
			// 랜덤하게 타일셋 및 서브셋 지정..
			int nRnd = rand()%nRndMax;
			int nTS = 0, nTSS = 0;
			for(int n = 0; n < nCount; n++)
			{
				if(nRnd >= nRnds[n] && nRnd < nRnds[n+1])
				{
					nTS = nTSs[n];
					nTSS = nTSSs[n];
					break;
				}
			}
			
			_TILE_SUB_SET_DATA_TOOL* pTSS = g_pTSet->DataGetSafe(nTS, nTSS);
			if(pTSS == NULL) continue;

			int t_index;
			for(int y2 = 0; y2 < pTSS->nHeight; y2++)
			{
				for(int x2 = 0; x2 < pTSS->nWidth; x2++)
				{
					t_index = pTSS->GetSafe(x2, y2);
					pMData = g_pTMap->ToolDataGet(nLayer, x*pTSS0->nWidth + x2, y*pTSS0->nHeight + y2);
					if(t_index == -1 || pMData == NULL) continue;

					pMData->snTS = nTS;
					pMData->snTSS = nTSS;
					pMData->snNum = t_index;
					pMData->snFlags = 0;
				}
			}
		}
	}

	// 미니맵 업데이트..
	g_WndTMapPreview.Alloc(g_pTMap->m_HTMap.nWidth, g_pTMap->m_HTMap.nHeight);
	g_WndTMapPreview.UpdateData(NULL);
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);

	UpdateWorkWindow();
}

void CDlgTileMap::OnBTileMapInit() 
{
	int nLayer = m_LBLayer.GetCurSel();
	if(nLayer < 0 || nLayer >= MAX_TILE_MAP_LAYER) return;

	CString szConfirm;
	szConfirm.Format("현재 선택된 레이어는 %d 입니다. 정말로 초기화 하시겠습니까 ?", nLayer);
	if(MessageBox(szConfirm,  "맵 초기화 경고", MB_OK) == IDCANCEL) return;

	_TILE_MAP_DATA_TOOL* pMData;
	for(int y = 0; y < g_pTMap->m_HTMap.nHeight; y++)
	{
		for(int x = 0; x < g_pTMap->m_HTMap.nWidth; x++)
		{
			pMData = g_pTMap->ToolDataGet(nLayer, x, y);
			if(pMData)
			{
				pMData->snFlags = 0;
				pMData->snTS = -1;
				pMData->snTSS = -1;
				pMData->snNum = -1;
			}
		}
	}

	// 미니맵 업데이트..
	g_WndTMapPreview.UpdateData(NULL);
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);

	UpdateWorkWindow();
}

void CDlgTileMap::OnSelendokCbWork() 
{
	// 스크롤 다시 계산..
	CRect rcParent;
	GetParent()->GetClientRect(&rcParent);
	GetParent()->SendMessage(WM_SIZE, NULL, MAKELPARAM(rcParent.Width(), rcParent.Height()));

	this->UpdateWorkWindow();
}

void CDlgTileMap::OnOK()
{
	
}

void CDlgTileMap::OnCancel()
{
	
}

void CDlgTileMap::OnSelchangeListTileSet() 
{
	int nTS = m_ListTS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;

	// 스크롤 다시 계산..
	CRect rcParent;
	GetParent()->GetClientRect(&rcParent);
	GetParent()->SendMessage(WM_SIZE, NULL, MAKELPARAM(rcParent.Width(), rcParent.Height()));

	// 서브셋 프리뷰 업데이트
	this->UpdateTileSetInfo();

	this->UpdateTileSetPreviewWindow();
	this->UpdateWorkWindow();
}

void CDlgTileMap::OnSelchangeListTileSubSet()
{
	int nTS = m_ListTS.GetCurSel();
	int nTSS = m_ListTSS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;

	_TILE_SUB_SET_DATA_TOOL* pTSSData = g_pTSet->DataGetSafe(nTS, nTSS);
	if(pTSSData == NULL) return;

	m_bDontUpdateName = TRUE;
	m_EditTileSubSetName.SetWindowText(pTSSData->szRemark); // 이름 바꿔주기..
	SetDlgItemInt(IDC_E_TILE_SUB_SET_NUMBER, nTSS);
	m_bDontUpdateName = FALSE;
	
	m_EditTileSubSetName.SetFocus();
	m_EditTileSubSetName.SetSel(-1); // 이름 전부 선택하기..

	// 서브셋 프리뷰 업데이트
	this->UpdateTileSubSetInfo();
	this->UpdateTileSetPreviewWindow();
	this->UpdateWorkWindow();
}

void CDlgTileMap::OnChangeETileSetName() 
{
	if(m_bDontUpdateName == TRUE) return;

	int nTS = m_ListTS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;

	CString str;
	m_EditTileSetName.GetWindowText(str);
	m_ListTS.DeleteString(nTS);
	m_ListTS.InsertString(nTS, str);
	m_ListTS.SetCurSel(nTS);

	lstrcpy(g_pTSet->m_HTSs[nTS].szName, str);
	SetDlgItemInt(IDC_E_TILE_SET_NUMBER, nTS);
}

void CDlgTileMap::OnChangeETileSubSetName() 
{
	CString str;
	m_EditTileSubSetName.GetWindowText(str);
	if(str == "") return;

	if(m_bDontUpdateName == TRUE) return;

	int nTS = m_ListTS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;

	int nTSS = m_ListTSS.GetCurSel();
	if(nTSS < 0 || nTS >= MAX_TILE_SUB_SET) return;

	_TILE_SUB_SET_DATA_TOOL* pTSSData = g_pTSet->DataGetSafe(nTS, nTSS);
	if(pTSSData == NULL)
	{
		SetDlgItemText(IDC_E_TILE_SUB_SET_NAME, "");
		return;
	}

	m_ListTSS.DeleteString(nTSS);
	m_ListTSS.InsertString(nTSS, str);
	m_ListTSS.SetCurSel(nTSS);

	lstrcpy(pTSSData->szRemark, str);
}

void CDlgTileMap::OnBTileSetImport() 
{
	int nTS = m_ListTS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "tsed", NULL, dwFlags, "타일 셋 Export 데이터(*.tsed)|*.tsed||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	g_pTSet->TileSetImport(nTS, FileName);

	this->UpdateTileSetInfo();
	this->UpdateTileSubSetList();
	this->UpdateTileSubSetInfo();

	this->UpdateWorkWindow();
	g_WndTMapPreview.UpdateData(NULL); // 미니 맵 업데이트
	g_WndTMapPreview.InvalidateRect(NULL, FALSE);
}

void CDlgTileMap::OnBTileSetExport() 
{
	int nTS = m_ListTS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "tsed", NULL, dwFlags, "타일 셋 Export 데이터(*.tsed)|*.tsed||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	g_pTSet->TileSetExport(nTS, FileName);
}

void CDlgTileMap::OnBSaveTileMapToGameFormat() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "gmd", NULL, dwFlags, "게임 타일 맵 데이터(*.gmd)|*.gmd||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	CDlgColor dlg2; // 컬러키
	if(dlg2.DoModal() == IDCANCEL) return;

	COLORREF crCK = dlg2.GetColor();

	g_pTMap->SaveToGameFormat(FileName, crCK);
}

void CDlgTileMap::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 0) // 자동 백업.
	{
		SYSTEMTIME time;
		::GetSystemTime(&time);
		CString szFN;
		szFN.Format("자동백업-%d년 %d월 %d일 %d시 %d분", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
		if(g_pTSet->DataGet(0, 0) != NULL)
		{
			g_pTEng->PathSetCur("MAP");
			g_pTSet->Save(szFN + ".tsd");
		}
		if(g_pTMap->m_HTMap.nWidth > 0 && g_pTMap->m_HTMap.nHeight > 0)
		{
			g_pTEng->PathSetCur("MAP");
			g_pTMap->SaveToToolData(szFN + ".tmd");
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CDlgTileMap::OnRadioNoEdit() 
{
	this->UpdateTileSetPreviewWindow();
	UpdateWorkWindow();
}

void CDlgTileMap::OnRadioMoveEdit() 
{
	this->UpdateTileSetPreviewWindow();
	UpdateWorkWindow();
}

void CDlgTileMap::OnRadioHouseEdit() 
{
	this->UpdateTileSetPreviewWindow();
	UpdateWorkWindow();
}

void CDlgTileMap::OnRadioEventEdit() 
{
	this->UpdateTileSetPreviewWindow();
	UpdateWorkWindow();
}

void CDlgTileMap::OnActAni() 
{
	if(IsDlgButtonChecked(IDC_ACT_ANI) == 1) g_WndTMap.ActiveAni();
	else g_WndTMap.DeactiveAni();
}

