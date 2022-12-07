// DlgTileSet.cpp : implementation file
//

#include "stdafx.h"
#include "TileSetTool.h"
#include "DlgTileMap.h"
#include "DlgColor.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//AFXAPI
//AFX_CDECL
//afxEmptyString
//AfxIsValidString

HANDLE BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal);
WORD SaveDIB(HANDLE hDib, LPSTR lpFileName);
/////////////////////////////////////////////////////////////////////////////
// CDlgTileMap dialog

CDlgTileMap::CDlgTileMap(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTileMap::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTileMap)
	//}}AFX_DATA_INIT
}

CDlgTileMap::~CDlgTileMap()
{
}

void CDlgTileMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTileMap)
	DDX_Control(pDX, IDC_EDIT_ANI_SUB_SET_NAME, m_EditASSN);
	DDX_Control(pDX, IDC_LIST_ANI_SUB_SETS, m_ListATSS);
	DDX_Control(pDX, IDC_AREA_NUMBER, m_wndAreaNumber);
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
	ON_BN_CLICKED(IDC_B_LIGHT_SOURCE_COLOR_SET, OnBLightSourceColorSet)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_B_NEW_LIGHT_SOURCE_DATA, OnBNewLightSourceData)
	ON_BN_CLICKED(IDC_B_LOAD_LIGHT_SOURCE_DATA, OnBLoadLightSourceData)
	ON_BN_CLICKED(IDC_B_SAVE_LIGHT_SOURCE_DATA, OnBSaveLightSourceData)
	ON_EN_CHANGE(IDC_E_LIGHT_SOURCE_NUMBER, OnChangeELightSourceNumber)
	ON_BN_CLICKED(IDC_RADIO_AREA_EDIT, OnRadioAreaEdit)
	ON_BN_CLICKED(IDC_DEL_TILESET, OnDelTileset)
	ON_BN_CLICKED(IDC_B_SAVE_TS_SPL, OnBSaveTsSpl)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_B_TILE_ANI_ADD, OnBTileAniAdd)
	ON_BN_CLICKED(IDC_B_TILE_ANI_DEL, OnBTileAniDel)
	ON_BN_CLICKED(IDC_CHECK_TILE_ANIMATION, OnCheckTileAnimation)
	ON_BN_CLICKED(IDC_B_TILE_ANIMATION, OnBTileAnimation)
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

	// 타일 에니메이션 프리뷰 윈도우...
	if(m_WndATPreview.GetSafeHwnd() == NULL)
	{
		HCURSOR hCur = LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_C_NORMAL));
		CString szWndClass;
		szWndClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, hCur, NULL, 0);
		m_WndATPreview.CreateEx(NULL,
								szWndClass,
								"에니타일 프리뷰",
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								256,
								256,
								this->m_hWnd,
								NULL);
	}
	m_WndATPreview.ShowWindow(SW_SHOW);

	// 레이어 리스트 박스 세팅.
	CString szItem;
	CString szLayer[MAX_TILE_MAP_LAYER];
	szLayer[0] = "기본 맵"; szLayer[1] = "겹치는 맵1"; szLayer[2] = "겹치는 맵2"; szLayer[3] = "그림자"; szLayer[4] = "오브젝트 1", szLayer[5] = "오브젝트 2";
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
	
	CheckRadioButton(IDC_RADIO_NO_EDIT, IDC_RADIO_AREA_EDIT, IDC_RADIO_NO_EDIT);
	
	SetDlgItemInt(IDC_AREA_NUMBER, 0, FALSE);

	this->UpdateTileSetList();

	this->OnBNewLightSourceData(); // 광원 데이터..

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
	DELETE_SAFE(g_pHouseInfo); g_pHouseInfo = new CHouseInfoManager();
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
	
	g_pTEng->PathSetCur("MAP"); // 서피스 로딩...

	CString str = FileName;
	str = str.Left(str.GetLength() - 3);
	str += "hif";
	g_pHouseInfo->Load(str);

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

	CString str = FileName;
	str = str.Left(str.GetLength() - 3);
	str += "hif";
	g_pHouseInfo->Save(str);
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
		if(pTSSData)
		{
			szTSS = pTSSData->szRemark;
			if( pTSSData->nAniCount > 1 )
				szTSS += " + ANI";
		}
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
		int t_x, t_y;
		t_x = g_pTMap->m_HTMap.nWidth * 32;
		t_y = g_pTMap->m_HTMap.nHeight * 32;
		g_pTMove->Initialize(t_x/96*2 + ((t_x%96>=48)?1:0), t_y/48*2 + ((t_y%48>=24)?1:0));
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
	int t_x, t_y;

	if(	nWidth <= 0 || nWidth > MAX_TILE_MAP_WIDTH || nHeight <= 0 || nHeight > MAX_TILE_MAP_HEIGHT) return;

	g_pTMap->ReAlloc(nWidth, nHeight); // 타일맵 다시 할당.
	t_x = g_pTMap->m_HTMap.nWidth * 32;
	t_y = g_pTMap->m_HTMap.nHeight * 32;
	g_pTMove->Initialize(t_x/96*2 + ((t_x%96>=48)?1:0), t_y/48*2 + ((t_y%48>=24)?1:0));
	
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
/*	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "gmd", NULL, dwFlags, "게임 타일 맵 데이터(*.gmd)|*.gmd||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("MAP");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	CDlgColor dlg2; // 컬러키
	if(dlg2.DoModal() == IDCANCEL) return;

	COLORREF crCK = dlg2.GetColor();

	g_pTMap->SaveToGameFormat(FileName, crCK);*/

	int width, height, cell_size;
	int x_count, y_count, tileX, tileY;
	CRect draw_rect;
	HRESULT rval;
	CString file_name, t_str;
	
	g_pTEng->PathSetCur("MAPBMP");

	GetDlgItem(IDC_CELLSIZE)->GetWindowText(t_str);
	cell_size = atoi((LPCTSTR)t_str);
	if(cell_size < 0 || cell_size > 32)
	{
		AfxMessageBox("셀 크기를 1~32사이로 넣어주세요.");
		return;
	}
	tileX = 50;
	tileY = 50;

	if(tileX > g_pTMap->m_HTMap.nWidth) tileX = g_pTMap->m_HTMap.nWidth;
	if(tileY > g_pTMap->m_HTMap.nHeight) tileY = g_pTMap->m_HTMap.nHeight;

	x_count = g_pTMap->m_HTMap.nWidth / tileX;
	if(g_pTMap->m_HTMap.nWidth % tileX) x_count++;
	y_count = g_pTMap->m_HTMap.nHeight / tileY;
	if(g_pTMap->m_HTMap.nHeight % tileY) y_count++;

	width = tileX * cell_size;
	height = tileY * cell_size;

	LPDIRECTDRAWSURFACE test_surface;
	DDSURFACEDESC ddsd;
	MAKE_ZERO(ddsd);
	ddsd.dwSize         = sizeof(ddsd);
	ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;// | DDSD_PIXELFORMAT;
	ddsd.dwWidth        = tileX*TILE_SIZE;
	ddsd.dwHeight       = tileY*TILE_SIZE;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	rval = g_pTEng->GetDirectDraw()->CreateSurface(&ddsd, &test_surface, NULL); // Create the rendering surface
	
	if(rval != DD_OK)
	{
		AfxMessageBox("불가능~!", MB_OK);
	}
	else
	{
		BOOL whole = TRUE;
		HDC hmdc, hsrcdc;
		HBITMAP hBitmap, hOldBitmap;
		HANDLE hDIB;
		HRESULT result;
		CString t_caption, t_progress;

		GetWindowText(t_caption);
		hmdc = ::CreateCompatibleDC(g_pTEng->m_hDC);
		hBitmap = ::CreateBitmap(width*x_count, height*y_count, 1, 16, NULL);
		if(hBitmap == NULL)
		{
			hBitmap = ::CreateBitmap(width, height, 1, 16, NULL);
			whole = FALSE;
		}
		for(int i = 0; i < x_count; i++)
		{
			for(int j = 0; j < y_count; j++)
			{
				SetWindowText(t_progress);
				draw_rect.left = i * tileX;
				draw_rect.right = draw_rect.left + tileX;
				if(draw_rect.right > g_pTMap->m_HTMap.nWidth) draw_rect.right = g_pTMap->m_HTMap.nWidth;
				draw_rect.top = j * tileY;
				draw_rect.bottom = draw_rect.top + tileY;
				if(draw_rect.bottom > g_pTMap->m_HTMap.nHeight) draw_rect.bottom = g_pTMap->m_HTMap.nHeight;

				g_WndTMap.DrawBitmap(test_surface, draw_rect);

				result = test_surface->GetDC(&hsrcdc);
				if(result == DD_OK)
				{
					hOldBitmap = (HBITMAP)::SelectObject(hmdc, hBitmap);
					if(whole == FALSE) ::StretchBlt(hmdc, 0, 0, width, height, hsrcdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, SRCCOPY);
					else ::StretchBlt(hmdc, draw_rect.left*cell_size, draw_rect.top*cell_size, width, height, hsrcdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, SRCCOPY);
					::SelectObject(hmdc, hOldBitmap);
					test_surface->ReleaseDC(hsrcdc);

					if(whole == FALSE)
					{
						hDIB = BitmapToDIB(hBitmap, (HPALETTE)NULL);

						file_name.Format("map%02d%02d.bmp", i, j);
						SaveDIB(hDIB, (LPTSTR)(LPCTSTR)file_name);
						GlobalFree( hDIB );
					}
				}
				t_progress.Format("Progress : %d%%", (i*y_count+j)*100/(x_count*y_count));
			}
		}
		if(whole == TRUE)
		{
			hDIB = BitmapToDIB(hBitmap, (HPALETTE)NULL);
			SaveDIB(hDIB, "map.bmp");
			GlobalFree( hDIB );
		}
		::DeleteObject(hBitmap);
		::DeleteDC(hmdc);
		RELEASE_SAFE(test_surface);
		SetWindowText(t_caption);
	}
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


void CDlgTileMap::OnBLightSourceColorSet() 
{
	int index = GetDlgItemInt(IDC_E_LIGHT_SOURCE_NUMBER);
	_LIGHT_SOURCE_DATA* pData = g_LSource.DataGet(index);
	if(pData == NULL) return;

	CColorDialog dlg;
	
	if(dlg.DoModal() == IDCANCEL) return;

	pData->crColor = dlg.GetColor();

	this->InvalidateRect(NULL, FALSE);
	this->UpdateWorkWindow();
}

void CDlgTileMap::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CWnd* pWnd = GetDlgItem(IDC_B_LIGHT_SOURCE_COLOR_SET);
	
	CRect rcB;
	pWnd->GetClientRect(&rcB);
	
	int index = GetDlgItemInt(IDC_E_LIGHT_SOURCE_NUMBER);
	_LIGHT_SOURCE_DATA* pData = g_LSource.DataGet(index);
	if(pData != NULL)
	{
		CDC* pDC = pWnd->GetDC();
		pDC->FillSolidRect(0, 0, rcB.Width(), rcB.Height(), pData->crColor);
		pWnd->ReleaseDC(pDC);
	}
	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgTileMap::OnBNewLightSourceData() 
{
	g_LSource.Alloc(2048);
	
	SetDlgItemInt(IDC_E_LIGHT_SOURCE_NUMBER, 0);
	SetDlgItemInt(IDC_E_LIGHT_SOURCE_QT, g_LSource.m_Header.nQt);

	this->UpdateWorkWindow();
}

void CDlgTileMap::OnBLoadLightSourceData() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "lsd", NULL, dwFlags, "광원 데이터 파일(*.lsd)|*.lsd||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	g_LSource.Alloc(2048);
	CLightSource ls;
	ls.Load(FileName);
	memcpy(g_LSource.DataGet(0), ls.DataGet(0), ls.m_Header.nQt*sizeof(_LIGHT_SOURCE_DATA));

	SetDlgItemInt(IDC_E_LIGHT_SOURCE_NUMBER, 0);
	SetDlgItemInt(IDC_E_LIGHT_SOURCE_QT, ls.m_Header.nQt);

	this->UpdateWorkWindow();
}

void CDlgTileMap::OnBSaveLightSourceData() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "lsd", NULL, dwFlags, "광원 데이터 파일(*.lsd)|*.lsd||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	int nOldQt = g_LSource.m_Header.nQt;
	g_LSource.m_Header.nQt = GetDlgItemInt(IDC_E_LIGHT_SOURCE_QT);
	g_LSource.Save(FileName);
	g_LSource.m_Header.nQt = nOldQt;
}

void CDlgTileMap::OnChangeELightSourceNumber() 
{
	this->InvalidateRect(NULL, FALSE);
}

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

/* Error constants */
enum {
      ERR_MIN = 0,                     // All error #s >= this value
      ERR_NOT_DIB = 0,                 // Tried to load a file, NOT a DIB!
      ERR_MEMORY,                      // Not enough memory!
      ERR_READ,                        // Error reading file!
      ERR_LOCK,                        // Error on a GlobalLock()!
      ERR_OPEN,                        // Error opening a file!
      ERR_CREATEPAL,                   // Error creating palette.
      ERR_GETDC,                       // Couldn't get a DC.
      ERR_CREATEDDB,                   // Error create a DDB.
      ERR_STRETCHBLT,                  // StretchBlt() returned failure.
      ERR_STRETCHDIBITS,               // StretchDIBits() returned failure.
      ERR_SETDIBITSTODEVICE,           // SetDIBitsToDevice() failed.
      ERR_STARTDOC,                    // Error calling StartDoc().
      ERR_NOGDIMODULE,                 // Couldn't find GDI module in memory.
      ERR_SETABORTPROC,                // Error calling SetAbortProc().
      ERR_STARTPAGE,                   // Error calling StartPage().
      ERR_NEWFRAME,                    // Error calling NEWFRAME escape.
      ERR_ENDPAGE,                     // Error calling EndPage().
      ERR_ENDDOC,                      // Error calling EndDoc().
      ERR_SETDIBITS,                   // Error calling SetDIBits().
      ERR_FILENOTFOUND,                // Error opening file in GetDib()
      ERR_INVALIDHANDLE,               // Invalid Handle
      ERR_DIBFUNCTION,                 // Error on call to DIB function
      ERR_MAX                          // All error #s < this value
};

WORD DIBNumColors(LPSTR lpDIB)
{
    WORD wBitCount;  // DIB bit count

    // If this is a Windows-style DIB, the number of colors in the
    // color table can be less than the number of bits per pixel
    // allows for (i.e. lpbi->biClrUsed can be set to some value).
    // If this is the case, return the appropriate value.
    

    if (IS_WIN30_DIB(lpDIB))
    {
        DWORD dwClrUsed;

        dwClrUsed = ((LPBITMAPINFOHEADER)lpDIB)->biClrUsed;
        if (dwClrUsed)

        return (WORD)dwClrUsed;
    }

    // Calculate the number of colors in the color table based on
    // the number of bits per pixel for the DIB.
    
    if (IS_WIN30_DIB(lpDIB))
        wBitCount = ((LPBITMAPINFOHEADER)lpDIB)->biBitCount;
    else
        wBitCount = ((LPBITMAPCOREHEADER)lpDIB)->bcBitCount;

    // return number of colors based on bits per pixel

    switch (wBitCount)
    {
        case 1:
            return 2;

        case 4:
            return 16;

        case 8:
            return 256;

        default:
            return 0;
    }
}
WORD PaletteSize(LPSTR lpDIB)
{
    // calculate the size required by the palette
    if (IS_WIN30_DIB (lpDIB))
        return (DIBNumColors(lpDIB) * sizeof(RGBQUAD));
    else
        return (DIBNumColors(lpDIB) * sizeof(RGBTRIPLE));
}

HANDLE BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal)
{
    BITMAP              bm;         // bitmap structure
    BITMAPINFOHEADER    bi;         // bitmap header
    LPBITMAPINFOHEADER  lpbi;       // pointer to BITMAPINFOHEADER
    DWORD               dwLen;      // size of memory block
    HANDLE              hDIB, h;    // handle to DIB, temp handle
    HDC                 hDC;        // handle to DC
    WORD                biBits;     // bits per pixel

    // check if bitmap handle is valid

    if (!hBitmap)
        return NULL;

    // fill in BITMAP structure, return NULL if it didn't work

    if (!GetObject(hBitmap, sizeof(bm), (LPSTR)&bm))
        return NULL;

    // if no palette is specified, use default palette

    if (hPal == NULL)
        hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

    // calculate bits per pixel

    biBits = bm.bmPlanes * bm.bmBitsPixel;

    // make sure bits per pixel is valid

    if (biBits <= 1)
        biBits = 1;
    else if (biBits <= 4)
        biBits = 4;
    else if (biBits <= 8)
        biBits = 8;
    else // if greater than 8-bit, force to 24-bit
        biBits = 24;

    // initialize BITMAPINFOHEADER

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bm.bmWidth;
    bi.biHeight = bm.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = biBits;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // calculate size of memory block required to store BITMAPINFO

    dwLen = bi.biSize + PaletteSize((LPSTR)&bi);

    // get a DC

    hDC = GetDC(NULL);

    // select and realize our palette

    hPal = SelectPalette(hDC, hPal, FALSE);
    RealizePalette(hDC);

    // alloc memory block to store our bitmap

    hDIB = GlobalAlloc(GHND, dwLen);

    // if we couldn't get memory block

    if (!hDIB)
    {
      // clean up and return NULL

      SelectPalette(hDC, hPal, TRUE);
      RealizePalette(hDC);
      ReleaseDC(NULL, hDC);
      return NULL;
    }

    // lock memory and get pointer to it

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    /// use our bitmap info. to fill BITMAPINFOHEADER

    *lpbi = bi;

    // call GetDIBits with a NULL lpBits param, so it will calculate the
    // biSizeImage field for us    

    GetDIBits(hDC, hBitmap, 0, (UINT)bi.biHeight, NULL, (LPBITMAPINFO)lpbi,
        DIB_RGB_COLORS);

    // get the info. returned by GetDIBits and unlock memory block

    bi = *lpbi;
    GlobalUnlock(hDIB);

    // if the driver did not fill in the biSizeImage field, make one up 
    if (bi.biSizeImage == 0)
        bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;

    // realloc the buffer big enough to hold all the bits

    dwLen = bi.biSize + PaletteSize((LPSTR)&bi) + bi.biSizeImage;

    if (h = GlobalReAlloc(hDIB, dwLen, 0))
        hDIB = h;
    else
    {
        // clean up and return NULL

        GlobalFree(hDIB);
        hDIB = NULL;
        SelectPalette(hDC, hPal, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
        return NULL;
    }

    // lock memory block and get pointer to it */

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    // call GetDIBits with a NON-NULL lpBits param, and actualy get the
    // bits this time

    if (GetDIBits(hDC, hBitmap, 0, (UINT)bi.biHeight, (LPSTR)lpbi +
            (WORD)lpbi->biSize + PaletteSize((LPSTR)lpbi), (LPBITMAPINFO)lpbi,
            DIB_RGB_COLORS) == 0)
    {
        // clean up and return NULL

        GlobalUnlock(hDIB);
        hDIB = NULL;
        SelectPalette(hDC, hPal, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
        return NULL;
    }

    bi = *lpbi;

    // clean up 
    GlobalUnlock(hDIB);
    SelectPalette(hDC, hPal, TRUE);
    RealizePalette(hDC);
    ReleaseDC(NULL, hDC);

    // return handle to the DIB
    return hDIB;
}

WORD SaveDIB(HANDLE hDib, LPSTR lpFileName)
{
    BITMAPFILEHEADER    bmfHdr;     // Header for Bitmap file
    LPBITMAPINFOHEADER  lpBI;       // Pointer to DIB info structure
    HANDLE              fh;         // file handle for opened file
    DWORD               dwDIBSize;
    DWORD               dwWritten;

    if (!hDib)
        return ERR_INVALIDHANDLE;

    fh = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (fh == INVALID_HANDLE_VALUE)
        return ERR_OPEN;

    // Get a pointer to the DIB memory, the first of which contains
    // a BITMAPINFO structure

    lpBI = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    if (!lpBI)
    {
        CloseHandle(fh);
        return ERR_LOCK;
    }

    // Check to see if we're dealing with an OS/2 DIB.  If so, don't
    // save it because our functions aren't written to deal with these
    // DIBs.

    if (lpBI->biSize != sizeof(BITMAPINFOHEADER))
    {
        GlobalUnlock(hDib);
        CloseHandle(fh);
        return ERR_NOT_DIB;
    }

    // Fill in the fields of the file header

    // Fill in file type (first 2 bytes must be "BM" for a bitmap)

    bmfHdr.bfType = DIB_HEADER_MARKER;  // "BM"

    // Calculating the size of the DIB is a bit tricky (if we want to
    // do it right).  The easiest way to do this is to call GlobalSize()
    // on our global handle, but since the size of our global memory may have
    // been padded a few bytes, we may end up writing out a few too
    // many bytes to the file (which may cause problems with some apps,
    // like HC 3.0).
    //
    // So, instead let's calculate the size manually.
    //
    // To do this, find size of header plus size of color table.  Since the
    // first DWORD in both BITMAPINFOHEADER and BITMAPCOREHEADER conains
    // the size of the structure, let's use this.

    // Partial Calculation

    dwDIBSize = *(LPDWORD)lpBI + PaletteSize((LPSTR)lpBI);  

    // Now calculate the size of the image

    // It's an RLE bitmap, we can't calculate size, so trust the biSizeImage
    // field

    if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
        dwDIBSize += lpBI->biSizeImage;
    else
    {
        DWORD dwBmBitsSize;  // Size of Bitmap Bits only

        // It's not RLE, so size is Width (DWORD aligned) * Height

        dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) *
                lpBI->biHeight;

        dwDIBSize += dwBmBitsSize;

        // Now, since we have calculated the correct size, why don't we
        // fill in the biSizeImage field (this will fix any .BMP files which 
        // have this field incorrect).

        lpBI->biSizeImage = dwBmBitsSize;
    }


    // Calculate the file size by adding the DIB size to sizeof(BITMAPFILEHEADER)
                   
    bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;

    // Now, calculate the offset the actual bitmap bits will be in
    // the file -- It's the Bitmap file header plus the DIB header,
    // plus the size of the color table.
    
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize +
            PaletteSize((LPSTR)lpBI);

    // Write the file header

    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

    // Write the DIB header and the bits -- use local version of
    // MyWrite, so we can write more than 32767 bytes of data
    
    WriteFile(fh, (LPSTR)lpBI, dwDIBSize, &dwWritten, NULL);

    GlobalUnlock(hDib);
    CloseHandle(fh);

    if (dwWritten == 0)
        return ERR_OPEN; // oops, something happened in the write
    else
        return 0; // Success code
}

void CDlgTileMap::OnRadioAreaEdit() 
{
	this->UpdateTileSetPreviewWindow();
	UpdateWorkWindow();
}

void CDlgTileMap::OnDelTileset() 
{
	int nTS = m_ListTS.GetCurSel();

	if(AfxMessageBox("삭제를 하시면 다시 복구할 수 없습니다.\n삭제하시겠습니까?", MB_ICONINFORMATION|MB_YESNO) == IDYES)
	{
		if(g_pTSet->Delete(nTS) == TRUE)
		{
			for(int i = 0; i < MAX_TILE_MAP_LAYER; i++) // 툴에서 쓴 데이터 저장...
			{
				for(int y = 0; y < g_pTMap->m_HTMap.nHeight; y++)
				{
					for(int x = 0; x < g_pTMap->m_HTMap.nWidth; x++)
					{
						if(g_pTMap->m_pDatasTool[i][y*g_pTMap->m_HTMap.nWidth+x].snTS == nTS)
						{
							g_pTMap->m_pDatasTool[i][y*g_pTMap->m_HTMap.nWidth+x].snFlags = 0;
							g_pTMap->m_pDatasTool[i][y*g_pTMap->m_HTMap.nWidth+x].snTS = -1;
							g_pTMap->m_pDatasTool[i][y*g_pTMap->m_HTMap.nWidth+x].snTSS = -1;
							g_pTMap->m_pDatasTool[i][y*g_pTMap->m_HTMap.nWidth+x].snNum = -1;
						}
						else if(g_pTMap->m_pDatasTool[i][y*g_pTMap->m_HTMap.nWidth+x].snTS > nTS)
						{
							g_pTMap->m_pDatasTool[i][y*g_pTMap->m_HTMap.nWidth+x].snTS--;
						}
					}
				}
			}
			this->UpdateTileSetList();
			UpdateWorkWindow();
		}
	}
}

/////////////////////////////////////////////////////
// 타일셋 정보를 통한 타일셋 스프라이트 추출
/////////////////////////////////////////////////////
void CDlgTileMap::OnBSaveTsSpl()//this_plus
{
	CString FileName;
	if( 0 == GetDlgItemText(IDC_E_FILE_NAME_TILE_SET, FileName) )
	{
		if(AfxMessageBox("타일셋 파일을 로딩하지 않았습니다.\n 로딩하시겠습니까?", MB_ICONINFORMATION|MB_YESNO) == IDYES)
			OnBLoadTileSet();
		return;
	}
	int nTS = m_ListTS.GetCurSel();
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;

	if(lstrlen(g_pTSet->m_HTSs[nTS].szBMPFN) > 0)
	{
		FileName = g_pTSet->m_HTSs[nTS].szBMPFN;
		FileName = FileName.Left(FileName.GetLength() - 3);
		FileName += "spl";
	}
	else
	{
		AfxMessageBox("타일셋을 선택하지 않았습니다.\n저장할 타일셋을 선택하세요.\n", MB_ICONINFORMATION|MB_OK);
		return;
	}

	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "spl", FileName, dwFlags, "LSP Sprite(*.spl)|*.spl|8Bit LSP Sprite(*.sp8)|*.sp8|Normal Sprite(*.spn)|*.spn||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

//	g_pTEng->PathSetCur("DATA"); // 서피스 로딩...
	g_pTSet->OnSaveSplFromTileSet(FileName,nTS);
}

int CDlgTileMap::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CDlgTileMap::OnBTileAniAdd() 
{
	if(IsDlgButtonChecked(IDC_CHECK_TILE_ANIMATION) != 1) return;

	int nTS = m_ListTS.GetCurSel();
	int nTSS = m_ListTSS.GetCurSel();
	int nTASS =	GetDlgItemInt(IDC_EDIT_ANI_SUB_SET_NUMBER);
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;

	_TILE_SUB_SET_DATA_TOOL* pTSSData = g_pTSet->DataGetSafe(nTS, nTASS);
	if(pTSSData == NULL) return;
	int nAniCnt = pTSSData->nAniCount++;

	BYTE* pBuffer = new BYTE[nAniCnt];
	if( nAniCnt > 1 )
	{
		if( pTSSData->pAniTSSN )
		{
			memcpy(pBuffer,pTSSData->pAniTSSN, (nAniCnt-1)*sizeof(BYTE) );
			delete [] pTSSData->pAniTSSN;
		}
	}
	pTSSData->pAniTSSN = pBuffer;
	pBuffer[nAniCnt-1] = nTSS;

	pTSSData = g_pTSet->DataGetSafe(nTS, nTSS);
	if(pTSSData)
		m_ListATSS.InsertString(nAniCnt-1,pTSSData->szRemark);
	else
		m_ListATSS.InsertString(nAniCnt-1,"error");
}

void CDlgTileMap::OnBTileAniDel()
{
	if(IsDlgButtonChecked(IDC_CHECK_TILE_ANIMATION) != 1) return;

	int nTS = m_ListTS.GetCurSel();
	int nTASS =	GetDlgItemInt(IDC_EDIT_ANI_SUB_SET_NUMBER);
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTASS < 0 || nTASS >= MAX_TILE_SUB_SET) return;

	int nCur = m_ListATSS.GetCurSel();
	if( nCur == -1 ) return;
	TRACE("nCur : %d\n",nCur);
	m_ListATSS.SetCurSel(nCur);

	_TILE_SUB_SET_DATA_TOOL* pTSSData = g_pTSet->DataGetSafe(nTS, nTASS);
	if( pTSSData == NULL ) return;
	if( pTSSData->nAniCount <= 1 ) return;
	if( pTSSData->nAniCount <= nCur ) return;

	int nBuffer = pTSSData->nAniCount - 1;
	pTSSData->nAniCount--;
//	if( nBuffer <= nCur ) return;

	if( nBuffer <= 1 && pTSSData->pAniTSSN )
	{
		delete [] pTSSData->pAniTSSN;
		pTSSData->pAniTSSN = NULL;
		m_ListATSS.DeleteString(nCur);
		return;
	}

	BYTE* pBuffer = new BYTE[nBuffer - 1];
	BYTE* pSrc = pBuffer;
	BYTE* pAniTSSN = pTSSData->pAniTSSN;
	memcpy( pBuffer, pTSSData->pAniTSSN, sizeof(BYTE)*nCur);
	pTSSData->pAniTSSN += (nCur+1);
	pBuffer += nCur;
	memcpy( pBuffer, pTSSData->pAniTSSN, sizeof(BYTE)*(nBuffer - (nCur+1)) );
	pTSSData->pAniTSSN = pSrc;
	delete [] pAniTSSN;
	m_ListATSS.DeleteString(nCur);
}

void CDlgTileMap::OnCheckTileAnimation()
{
	m_ListATSS.ResetContent();
	if(IsDlgButtonChecked(IDC_CHECK_TILE_ANIMATION) == 1)
	{
		int nTS = m_ListTS.GetCurSel();
		int nTSS = m_ListTSS.GetCurSel();
		if(nTS < 0 || nTS >= MAX_TILE_SET) return;
		if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;

		m_WndATPreview.TileSetCur( nTS, nTSS );
//		m_WndATPreview.ActiveAniTile(TRUE);

		_TILE_SUB_SET_DATA_TOOL* pTSSData = g_pTSet->DataGetSafe(nTS, nTSS);
		if(pTSSData == NULL) return;

		m_EditASSN.SetWindowText(pTSSData->szRemark); // 이름 바꿔주기..
		SetDlgItemInt(IDC_EDIT_ANI_SUB_SET_NUMBER, nTSS);

		_TILE_SUB_SET_DATA_TOOL* pTASS = g_pTSet->DataGetSafe(nTS, nTSS);
		int nAniCnt = pTSSData->nAniCount - 1;
		for( int i = 0; i < nAniCnt ; i++ )
		{
			pTASS = g_pTSet->DataGetSafe(nTS, pTSSData->pAniTSSN[i]);
			if(pTASS)
				m_ListATSS.InsertString( i,pTASS->szRemark );
			else
				m_ListATSS.InsertString( i,"error");
		}
	}
	else
	{
		UpdateTileSubSetList();
		m_WndATPreview.ActiveAniTile(FALSE);
		m_EditASSN.SetWindowText(NULL);
		SetDlgItemInt(IDC_EDIT_ANI_SUB_SET_NUMBER, 0);
	}
}

void CDlgTileMap::OnBTileAnimation() 
{
	if(IsDlgButtonChecked(IDC_CHECK_TILE_ANIMATION) != 1) return;
	m_WndATPreview.ActiveAniTile(TRUE);
}
