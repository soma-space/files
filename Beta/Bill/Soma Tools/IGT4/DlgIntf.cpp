// TabZone.cpp : implementation file
//

#include "stdafx.h"
#include "DlgIntf.h"

#include "GlobalVar.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgIntf dialog
CDlgIntf::CDlgIntf(CWnd* pParent) : CDialog(CDlgIntf::IDD)
{
	BOOL m_bUpdateData = FALSE;
	BOOL m_bUpdateInfo = FALSE;
	m_lpDDSBkg = NULL;

	//{{AFX_DATA_INIT(CDlgIntf)
	//}}AFX_DATA_INIT
}

CDlgIntf::~CDlgIntf()
{
	RELEASE_SAFE(m_lpDDSBkg);
}

void CDlgIntf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgIntf)
	DDX_Control(pDX, IDC_C_VIEW_OUTLINE, m_CViewOutLine);
	DDX_Control(pDX, IDC_CB_INTF_TYPE, m_CBIntfType);
	DDX_Control(pDX, IDC_CB_RENDER_TYPE, m_CBRenderType);
	DDX_Control(pDX, IDC_C_VIEW_BKG, m_CViewBkg);
	DDX_Control(pDX, IDC_C_VIEW_TRANS, m_CViewTrans);
	DDX_Control(pDX, IDC_C_VIEW_LINE, m_CViewLine);
	DDX_Control(pDX, IDC_C_VIEW_ZOOM, m_CViewZoom);
	DDX_Control(pDX, IDC_C_PLAY_ANIMATION, m_CPlayAnimation);
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Control(pDX, IDC_ANI_LIST, m_AniList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgIntf, CDialog)
	//{{AFX_MSG_MAP(CDlgIntf)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_SAVE_AS, OnSaveAs)
	ON_BN_CLICKED(IDC_BROWSE_SPR, OnBrowseSpr)
	ON_BN_CLICKED(IDC_BROWSE_ANI, OnBrowseAni)
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnRclickTree)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE, OnKeydownTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE, OnEndlabeleditTree)
	ON_COMMAND(ID_MENU_LAYER_ADD, OnMenuLayerAdd)
	ON_COMMAND(ID_MENU_LAYER_INSERT, OnMenuLayerInsert)
	ON_COMMAND(ID_MENU_LAYER_DELETE, OnMenuLayerDelete)
	ON_COMMAND(ID_MENU_INTF_ADD, OnMenuIntfAdd)
	ON_COMMAND(ID_MENU_INTF_INSERT, OnMenuIntfInsert)
	ON_COMMAND(ID_MENU_INTF_DELETE, OnMenuIntfDelete)
	ON_COMMAND(ID_MENU_INTF_COPY, OnMenuIntfCopy)
	ON_COMMAND(ID_MENU_INTF_PASTE, OnMenuIntfPaste)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_LBN_SELCHANGE(IDC_ANI_LIST, OnSelchangeAniList)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE, OnBegindragTree)
	ON_NOTIFY(NM_CLICK, IDC_TREE, OnClickTree)
	ON_BN_CLICKED(IDC_C_PLAY_ANIMATION, OnCPlayAnimation)
	ON_BN_CLICKED(IDC_C_VIEW_LINE, OnCViewLine)
	ON_BN_CLICKED(IDC_C_VIEW_BKG, OnCViewBkg)
	ON_BN_CLICKED(IDC_C_VIEW_ZOOM, OnCViewZoom)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnChangeEditWidth)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnChangeEditHeight)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE, OnBeginlabeleditTree)
	ON_BN_CLICKED(IDC_B_BROWSE_BKG, OnBBrowseBkg)
	ON_CBN_SELCHANGE(IDC_CB_INTF_TYPE, OnSelchangeCbIntfType)
	ON_EN_CHANGE(IDC_E_FLAG0, OnChangeEFlag0)
	ON_EN_CHANGE(IDC_E_FLAG1, OnChangeEFlag1)
	ON_EN_CHANGE(IDC_E_FLAG2, OnChangeEFlag2)
	ON_EN_CHANGE(IDC_E_FLAG3, OnChangeEFlag3)
	ON_CBN_SELCHANGE(IDC_CB_RENDER_TYPE, OnSelchangeCbRenderType)
	ON_EN_CHANGE(IDC_E_A, OnChangeEA)
	ON_EN_CHANGE(IDC_E_R, OnChangeER)
	ON_EN_CHANGE(IDC_E_G, OnChangeEG)
	ON_EN_CHANGE(IDC_E_B, OnChangeEB)
	ON_BN_CLICKED(IDC_C_VIEW_OUTLINE, OnCViewOutline)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDlgIntf::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	New();
	
	m_CViewLine.SetCheck(TRUE); // 선보기 체크...
	m_CViewOutLine.SetCheck(TRUE); // 화면 외곽선보기 체크...
	SetDlgItemInt(IDC_EDIT_WIDTH, 800);
	SetDlgItemInt(IDC_EDIT_HEIGHT, 600);

/*
const int RENDER_NORMAL = 0;
const int RENDER_TRANS = 1;
const int RENDER_ALPHA_ADD = 2;
const int RENDER_ALPHA_SOFT = 3;
const int RENDER_SHADOW = 4;
const int RENDER_DARK = 5;
const int RENDER_COLOR = 6;

const int INTF_TYPE_NORMAL = 0;
const int INTF_TYPE_BUTTON_NORMAL = 1;
const int INTF_TYPE_BUTTON_TOGGLE = 2;
const int INTF_TYPE_BUTTON_JUMP = 3;
const int INTF_TYPE_BUTTON_RADIO = 4;
const int INTF_TYPE_SCROLL_LR = 5;
const int INTF_TYPE_SCROLL_TB = 6;
const int INTF_TYPE_DIAL_LR = 7;
const int INTF_TYPE_DIAL_TB = 8;
const int INTF_TYPE_PROGRESS = 9;
const int INTF_TYPE_COLOR_BOX = 10;
const int INTF_TYPE_TEXT_IN = 11;
const int INTF_TYPE_TEXT_OUT = 12;
const int INTF_TYPE_LIST_BOX = 13;
const int INTF_TYPE_COMBO_BOX = 14;
const int INTF_TYPE_DRAG_RGN = 15;
*/
	
	m_CBIntfType.AddString("보통");
	m_CBIntfType.AddString("버튼");
	m_CBIntfType.AddString("버튼 - 토글");
	m_CBIntfType.AddString("버튼 - 이동");
	m_CBIntfType.AddString("버튼 - 라디오");
	m_CBIntfType.AddString("스크롤바 - 좌우");
	m_CBIntfType.AddString("스크롤바 - 상하");
	m_CBIntfType.AddString("다이얼 - 좌우");
	m_CBIntfType.AddString("다이얼 - 상하");
	m_CBIntfType.AddString("진행 상황");
	m_CBIntfType.AddString("영역 - 색칠");
	m_CBIntfType.AddString("텍스트 - 입력");
	m_CBIntfType.AddString("텍스트 - 출력");
	m_CBIntfType.AddString("리스트 박스");
	m_CBIntfType.AddString("콤보 박스");
	m_CBIntfType.AddString("드래깅 영역");


	m_CBRenderType.AddString("보통");
	m_CBRenderType.AddString("반투명");
	m_CBRenderType.AddString("Alpha Add");
	m_CBRenderType.AddString("Alpha Soft");
	m_CBRenderType.AddString("그림자");
	m_CBRenderType.AddString("어둡게");

	UpdateDlg();
	UpdateWorkWnd();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CDlgIntf message handlers

void CDlgIntf::AniLoad(const char* szFileName)
{
	m_pWndWork = NULL;
}

void CDlgIntf::AniNew()
{
}

void CDlgIntf::UpdateWorkWnd()
{
	if(m_pWndWork) m_pWndWork->InvalidateRect(NULL, FALSE);
}

void CDlgIntf::New()
{
	g_pIntf->New();
	UpdateDlg();
}

void CDlgIntf::OnNew() 
{
	SetDlgItemText(IDC_FILE_NAME, "");
	g_pIntf->New();

	UpdateDlg();
	UpdateWorkWnd();
}

void CDlgIntf::OnLoad() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "int", NULL, dwFlags, "인터페이스 데이터(*.int)|*.int||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	g_pIntf->Load(FileName, g_pTEng->PathGetCur("ANI"), g_pTEng->PathGetCur("SPR", 1), g_pTEng->PixelFormatGet());
	
	UpdateDlg();
	UpdateWorkWnd();

	SetDlgItemText(IDC_FILE_NAME, FileName);
}

void CDlgIntf::OnSave() 
{
	CString FileName;
	GetDlgItemText(IDC_FILE_NAME, FileName);
	if(!FileName.GetLength())
	{
		OnSaveAs();
		return;
	}
	
	g_pTEng->PathSetCur("DATA");
	g_pIntf->Save(FileName);
	
	this->UpdateDlg();
	this->Refresh();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnSaveAs() 
{
	CString FN; GetDlgItemText(IDC_EDIT_ANI_FILE_NAME, FN);
	if(FN.GetLength() >= 5) FN = FN.Left(FN.GetLength() - 4);

	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "int", FN, dwFlags, "인터페이스 데이터(*.int)|*.int||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();
	
	SetDlgItemText(IDC_FILE_NAME, FileName);

	OnSave();
}

void CDlgIntf::OnBrowseSpr() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "LSP 스프라이트(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	g_pIntf->ImportSpr(FileName, g_pTEng->PixelFormatGet()); // 8방향 스프라이트에서 읽어들임..

	UpdateDlg();
	UpdateWorkWnd();
}

void CDlgIntf::OnBrowseAni() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "ani", NULL, dwFlags, "에니메이션(*.ani)|*.ani||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("ANI");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	g_pIntf->ImportAni(FileName); // 에니메이션 파일에서 읽어들임..

	UpdateDlg();
	UpdateWorkWnd();
}

void CDlgIntf::RefreshData()
{
	if(m_bUpdateInfo == TRUE) return; // 대화상자 업데이트 중에 데이터 업데이트를 막는다...
	m_bUpdateData = TRUE; // 데이터 업데이트중 플래그 세팅

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem == NULL) return;

	HTREEITEM hIntf = NULL, hChild = NULL;
	int nNodeType = HIWORD(m_Tree.GetItemData(hItem));
	if(nNodeType == NODE_INTF)
	{
		hIntf = hItem;
		hChild = m_Tree.GetChildItem(hIntf);
	}
	else if(nNodeType == NODE_RENDER_STATE)
	{
		hIntf = m_Tree.GetParentItem(hItem);
		hChild = hItem;
	}
	else return;

	int nIntf = LOWORD(m_Tree.GetItemData(hIntf));
	_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
	if(pIntf == NULL) return;

	int nState = LOWORD(m_Tree.GetItemData(hChild));
	if(nState < 0 || nState >= 4) nState = 0;

	// 적용될 에니메이션
	int nAni = m_AniList.GetCurSel();
	pIntf->nAnis[nState] = nAni;

	// 인터페이스 타입..
	pIntf->nType = m_CBIntfType.GetCurSel();
	pIntf->crFill = RGB(GetDlgItemInt(IDC_E_R), GetDlgItemInt(IDC_E_G), GetDlgItemInt(IDC_E_B));
	pIntf->crFill |= GetDlgItemInt(IDC_E_A) << 24; // alpha 값

	// 플래그
	pIntf->nFlags[0] = GetDlgItemInt(IDC_E_FLAG0);
	pIntf->nFlags[1] = GetDlgItemInt(IDC_E_FLAG1);
	pIntf->nFlags[2] = GetDlgItemInt(IDC_E_FLAG2);
	pIntf->nFlags[3] = GetDlgItemInt(IDC_E_FLAG3);

	// 상태에 따른 렌더링 타입(옵션??)
	pIntf->nRenderTypes[nState] = m_CBRenderType.GetCurSel();

	// 채울색 및 Alpha 값
	int a = GetDlgItemInt(IDC_E_A);
	int r = GetDlgItemInt(IDC_E_R);
	int g = GetDlgItemInt(IDC_E_G);
	int b = GetDlgItemInt(IDC_E_B);
	pIntf->crFill = RGB(r,g,b);
	pIntf->crFill |= a << 24;

	// 버튼으로 새로 지정되었으면..
	BOOL bUNI = FALSE;
	if(	(pIntf->nType >= INTF_TYPE_BUTTON_NORMAL && pIntf->nType <= INTF_TYPE_BUTTON_RADIO) == FALSE &&
		(pIntf->nType >= INTF_TYPE_BUTTON_NORMAL && pIntf->nType <= INTF_TYPE_BUTTON_RADIO) == TRUE)
	{
		for(int i = 1; i < 4; i++) pIntf->nAnis[i] = pIntf->nAnis[0];
		bUNI = TRUE; // 트리 인터페이스 노드 업데이트 필요..
	}

	// 데이터 업데이트중 플래그 세팅
	m_bUpdateData = FALSE; 

	if(bUNI == TRUE) this->UpdateNodeIntf(NULL); // 트리 인터페이스 노드 업데이트
}

void CDlgIntf::Refresh()
{
	if(m_bUpdateData == TRUE) return; // 데이터 업데이트 되는 중에 고쳐지는 걸 막기위해...
	m_bUpdateInfo = TRUE; // 대화상자 업데이트중 플래그 세팅

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem == NULL) return;

	HTREEITEM hChild = NULL;
	int nNodeType = HIWORD(m_Tree.GetItemData(hItem));
	if(nNodeType == NODE_INTF) hChild = m_Tree.GetChildItem(hItem);
	else if(nNodeType == NODE_RENDER_STATE)
	{
		hChild = hItem;
		hItem = m_Tree.GetParentItem(hItem); // 인터페이스 노드 핸들을 가리키게 한다.
	}
	else return;

	int nIntf = LOWORD(m_Tree.GetItemData(hItem));
	_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
	if(pIntf == NULL) return;

	// 커서 상황 위치에 따른 에니메이션 번호 및 이름표시
	int nStatus = LOWORD(m_Tree.GetItemData(hChild));
	if(nStatus < 0 || nStatus >= 4) nStatus = 0;

	m_CBIntfType.SetCurSel(pIntf->nType); // 인터페이스 타입
	m_CBRenderType.SetCurSel(pIntf->nRenderTypes[nStatus]); // 렌더링 타입..
	
	SetDlgItemInt(IDC_E_FLAG0, pIntf->nFlags[0]); // 플래그
	SetDlgItemInt(IDC_E_FLAG1, pIntf->nFlags[1]);
	SetDlgItemInt(IDC_E_FLAG2, pIntf->nFlags[2]);
	SetDlgItemInt(IDC_E_FLAG3, pIntf->nFlags[3]);

	SetDlgItemInt(IDC_E_A, pIntf->crFill >> 24); // 채울색 및 투명도..
	SetDlgItemInt(IDC_E_R, (pIntf->crFill & 0x00ff0000) >> 16);
	SetDlgItemInt(IDC_E_G, (pIntf->crFill & 0x0000ff00) >> 8);
	SetDlgItemInt(IDC_E_B, pIntf->crFill & 0x000000ff);

	m_AniList.SetCurSel(pIntf->nAnis[nStatus]);

	m_bUpdateInfo = FALSE; // 대화상자 업데이트중 플래그 세팅
}

void CDlgIntf::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case 0: // Animation..
		UpdateWorkWnd();
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CDlgIntf::OnCViewBkg() 
{
	UpdateWorkWnd();
}

void CDlgIntf::OnCViewTrans() 
{
	UpdateWorkWnd();	
}

void CDlgIntf::OnCViewZoom() 
{
	UpdateWorkWnd();
}

void CDlgIntf::OnCViewLine() 
{
	UpdateWorkWnd();
}

void CDlgIntf::OnCPlayAnimation() 
{
	if(m_CPlayAnimation.GetCheck())
		SetTimer(0, 20, NULL);
	else KillTimer(0);
}

void CDlgIntf::UpdateDlg()
{
	// 스프라이트 이름과 에니메이션 이름을 갱신
	SetDlgItemText(IDC_EDIT_SPR_FILE_NAME, g_pIntf->m_Header.szSprFN);
	SetDlgItemText(IDC_EDIT_ANI_FILE_NAME, g_pIntf->m_Header.szAniFN);

	SetDlgItemInt(IDC_EDIT_WIDTH, g_pIntf->m_Header.rcWhole.right);
	SetDlgItemInt(IDC_EDIT_HEIGHT, g_pIntf->m_Header.rcWhole.bottom);

	// 현재 데이터로 트리를 갱신한다..
	UpdateTreeByData();

	// 에니메이션 리스트 리셋
	m_AniList.ResetContent();
	int nAniQt = g_pIntf->AniObjGet()->m_HAni.nQt;
	
	_ANI_DATA* pAni;
	CString str;
	for(int i = 0; i < nAniQt; i++)
	{
		pAni = g_pIntf->AniObjGet()->AniDataGet(i);
		if(pAni == NULL) continue;
		if(lstrlen(pAni->szName)) str.Format("%3d : %s", i, pAni->szName);
		else str.Format("%3d : 이름 없음", i);
		m_AniList.AddString(str);
	}

	Refresh();
}

void CDlgIntf::OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint point;
	GetCursorPos(&point);
	m_Tree.ScreenToClient(&point);
	HTREEITEM hItem = m_Tree.HitTest(point);
	if(hItem == NULL) return;
	
	HTREEITEM hRoot = m_Tree.GetRootItem();
	HTREEITEM hParent = m_Tree.GetParentItem(hItem);
	int nItem = HIWORD(m_Tree.GetItemData(hItem));
	
	m_Tree.SelectItem(hItem); // 선택

	CMenu popup;
	popup.LoadMenu(IDM_POPUP_INTF);
	CMenu* pSubMenu = popup.GetSubMenu(0);
		
	if(hItem == hRoot) // 아이템이 루트이면..
	{
		// 레이어 추가 메뉴만 나타나게 한다..
		pSubMenu = pSubMenu->GetSubMenu(0);
		m_Tree.ClientToScreen(&point);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	else if(nItem == NODE_LAYER) // 아이템이 레이어이면
	{
		// 레이어 추가, 삽입, 삭제, 인터페이스 추가메뉴가 나타나게 한다..
		pSubMenu = pSubMenu->GetSubMenu(1);
		m_Tree.ClientToScreen(&point);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	else if(nItem == NODE_INTF) // 아이템이 인터페이스 항목이면...
	{
		// 인터페이스 추가, 삽입, 삭제, 연결 항목만 나타나게 한다..
		pSubMenu = pSubMenu->GetSubMenu(2);
		m_Tree.ClientToScreen(&point);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}

	*pResult = 0;
}

void CDlgIntf::OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	
	int nKey = pTVKeyDown->wVKey;
	UINT nFlags = pTVKeyDown->flags;

	HTREEITEM hRoot = m_Tree.GetRootItem();
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem == NULL) return;
	int nNodeType = HIWORD(m_Tree.GetItemData(hItem));

	switch(nKey)
	{
	case VK_ADD:
		if(nNodeType == NODE_LAYER || nNodeType == NODE_INTF)
			OnMenuIntfAdd();
		else if(hItem == hRoot)
			OnMenuLayerAdd();
		break;
	case VK_INSERT:
		if(nNodeType == NODE_INTF)
			OnMenuIntfInsert();
		else if(nNodeType == NODE_LAYER)
			OnMenuLayerInsert();
		break;
	case VK_DELETE:
		if(nNodeType == NODE_INTF)
			OnMenuIntfDelete();
		else if(nNodeType == NODE_LAYER)
			OnMenuLayerDelete();
		break;
	case VK_F2:
		if(nNodeType == NODE_LAYER)
			m_Tree.EditLabel(hItem);
		break;
	case VK_RETURN:
		if(nNodeType == NODE_LAYER)
			m_Tree.SelectItem(hItem);
		break;
	}

	*pResult = 0;
}

void CDlgIntf::OnEndlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	CString str = pTVDispInfo->item.pszText;
	HTREEITEM hItem = pTVDispInfo->item.hItem;

	if(hItem == NULL || str.GetLength() <= 0)
	{
		*pResult = TRUE;  // 라벨 수정 취소
		return;
	}

	int nNodeType = HIWORD(m_Tree.GetItemData(hItem));
	if(nNodeType == NODE_LAYER) // 노드가 레이어면..
	{
		char szTemp[128]; int nTemp = 0; // 문자열로 변환...
		lstrcpy(szTemp, str);
		for(int i = 0; i < str.GetLength(); i++)
			if(str[i] >= '0' && str[i] <= '9') szTemp[nTemp++] = str[i];
		szTemp[nTemp] = NULL;
		int nLayer = atoi(szTemp); // 문자열에서 숫자를 추출..

		BOOL bOverLapLayer = FALSE; // 중복되는 레이어 번호가 있는지 살펴본다..
		HTREEITEM hRoot = m_Tree.GetRootItem(); // 트리를 돌면서 유효한 인터페이스 번호 리스트를 뽑는다.
		HTREEITEM hLayer;
		hLayer = m_Tree.GetChildItem(hRoot); // 레이어 노드..
		while(hLayer)
		{
			int nTmp = LOWORD(m_Tree.GetItemData(hLayer)); // 아이템에 적용된 레이어 번호 가져오고...
			if(hItem != hLayer) // 현재 것과 같지않은 레이어 일때..
			{
				if(nLayer == nTmp)
				{
					bOverLapLayer = TRUE; // 중복..
					break;
				}
			}
			hLayer = m_Tree.GetNextSiblingItem(hLayer); // 다음 노드 가져오기..
		}

		if(nLayer < 0 || nLayer >= MAX_LAYER || bOverLapLayer == TRUE)
		{
			MessageBox("문자열 중의 레이어 번호가 중복되거나 잘못되어 있습니다.");
		}
		else
		{
			HTREEITEM hIntf = m_Tree.GetChildItem(hItem); // 인터페이스 노드..
			while(hIntf) // 자식 노드(인터페이스 노드) 레이어 값을 바꾸기..
			{
				int nIntf = LOWORD(m_Tree.GetItemData(hIntf)); // 인터페이스 값 가져오기..
				_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
				if(pIntf)
				{
					pIntf->nParentLayer = nLayer;
				}

				hIntf = m_Tree.GetNextSiblingItem(hIntf); // 다음 노드 가져오기..
			}
			str.Format("레이어 %2d", nLayer);
			m_Tree.SetItemText(hItem, str); // 문자열 바꾸기..
			m_Tree.SetItemData(hItem, MAKELPARAM(nLayer, NODE_LAYER));
		}
	}
	else if(nNodeType == NODE_INTF)
	{
		int nIntf = LOWORD(m_Tree.GetItemData(hItem));
		_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
		if(pIntf == NULL) *pResult = TRUE; // 취소..
		else
		{
			lstrcpy(pIntf->szName, str); // 이름 바꾸고 
			
			this->UpdateNodeIntf(hItem); // 트리 노드 업데이트
		}
	}
	*pResult = 0;
}

void CDlgIntf::OnBeginlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = pTVDispInfo->item.hItem;

	*pResult = TRUE; // 요렇게 하면 편집 취소지롱..!!

	if(hItem == NULL) return;
	int nNodeType = HIWORD(m_Tree.GetItemData(hItem));
	if(nNodeType != NODE_LAYER && nNodeType != NODE_INTF) return; // 노드가 레이어나 인터페이스가 아니면 돌아간다..
	
	*pResult = 0;
}

void CDlgIntf::OnMenuLayerAdd() 
{
	HTREEITEM hRoot = m_Tree.GetRootItem();
	if(hRoot == NULL) return;
	
	CString str = "레이어 - 번호 지정해 주세요";
	HTREEITEM hLayer = m_Tree.InsertItem(str, hRoot, TVI_LAST);
	m_Tree.SetItemData(hLayer, MAKELPARAM(-1, NODE_LAYER));
	m_Tree.Expand(hRoot, TVE_EXPAND);
	m_Tree.SelectItem(hLayer);
}

void CDlgIntf::OnMenuLayerInsert() 
{
	HTREEITEM hLayer = m_Tree.GetSelectedItem();
	if(hLayer == NULL) return;
	int nNodeType = HIWORD(m_Tree.GetItemData(hLayer));
	HTREEITEM hRoot = m_Tree.GetRootItem();

	if(nNodeType != NODE_LAYER) return;
	
	CString str = "레이어 - 번호 지정해 주세요";
	hLayer = m_Tree.InsertItem(str, hRoot, hLayer);
	m_Tree.SetItemData(hLayer, MAKELPARAM(-1, NODE_LAYER));
	m_Tree.Expand(hRoot, TVE_EXPAND);
	m_Tree.SelectItem(hLayer);
}

void CDlgIntf::OnMenuLayerDelete()
{
	HTREEITEM hLayer = m_Tree.GetSelectedItem();
	if(hLayer == NULL) return;

	int nNodeType = HIWORD(m_Tree.GetItemData(hLayer));
	if(nNodeType != NODE_LAYER) return;

	HTREEITEM hIntf = m_Tree.GetChildItem(hLayer);
	while(hIntf) // 몽땅 지운다..
	{
		HTREEITEM hTmp = hIntf;
		int nIntf = LOWORD(m_Tree.GetItemData(hIntf));
		g_pIntf->Delete(nIntf);
		hIntf = m_Tree.GetNextSiblingItem(hIntf);
		m_Tree.DeleteItem(hTmp);
	}
	
	m_Tree.DeleteItem(hLayer); // 레이어 노드 지우기..
}

// 인터페이스 추가
void CDlgIntf::OnMenuIntfAdd() 
{
	HTREEITEM hLayer = m_Tree.GetSelectedItem();
	if(hLayer == NULL) return;

	int nNodeType = HIWORD(m_Tree.GetItemData(hLayer));

	if(nNodeType == NODE_INTF) hLayer = m_Tree.GetParentItem(hLayer);
	else if(nNodeType == NODE_LAYER) hLayer = hLayer;
	else return;

	int nIntf = g_pIntf->Add();
	if(nIntf < 0 || nIntf >= MAX_INTF) return; // 새로 할당에 실패

	CString str; str.Format("인터페이스 - %3d", nIntf);
	HTREEITEM hIntf = m_Tree.InsertItem(str, hLayer, TVI_LAST);
	m_Tree.SetItemData(hIntf, MAKELPARAM(nIntf, NODE_INTF));

	this->UpdateNodeIntf(hIntf);
	m_Tree.Expand(hLayer, TVE_EXPAND);
	m_Tree.SelectItem(hIntf); // 선택

	int nLayer = LOWORD(m_Tree.GetItemData(hLayer));
	_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
	if(pIntf)
	{
		pIntf->nParentLayer = nLayer;  // 인터페이스에 레이어 번호 기록..
	}
	else
	{
		MessageBox("인터페이스 추가에 실패. 저장하시고  다시 불러서 작업해 주세요.", MB_OK);
		return;
	}
}

// 인터페이스 삽입
void CDlgIntf::OnMenuIntfInsert() 
{
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem == NULL) return;

	int nNodeType = HIWORD(m_Tree.GetItemData(hItem));

	if(nNodeType != NODE_INTF) return;

	int nIntf = g_pIntf->Add(); // 인터페이스 추가후 번호를 받아와 하위워드로 넣는다.
	HTREEITEM hParent = m_Tree.GetParentItem(hItem);
	CString str; str.Format("인터페이스 - %3d", nIntf);
	HTREEITEM hIntf = m_Tree.InsertItem(str, hParent, hItem);
	m_Tree.SetItemData(hIntf, MAKELPARAM(nIntf, NODE_INTF));

	int nLayer = LOWORD(m_Tree.GetItemData(hParent));

	// 인터페이스에 레이어 번호 기록..
	_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
	if(pIntf)
	{
		pIntf->nParentLayer = nLayer;
	}
	else
	{
		MessageBox("인터페이스 추가에 실패. 저장하시고  다시 불러서 작업해 주세요.", MB_OK);
		return;
	}

	this->UpdateNodeIntf(hIntf);
	m_Tree.SelectItem(hIntf); // 선택
}

// 인터페이스 삭제
void CDlgIntf::OnMenuIntfDelete() 
{
	HTREEITEM hIntf = m_Tree.GetSelectedItem();
	if(hIntf == NULL) return;
	int nNodeType = HIWORD(m_Tree.GetItemData(hIntf));

	if(nNodeType != NODE_INTF) return;
	
	int nIntf = LOWORD(m_Tree.GetItemData(hIntf));
	g_pIntf->Delete(nIntf);
	
	m_Tree.DeleteItem(hIntf); // 인터페이스 아이템 삭제..
}

void CDlgIntf::OnMenuIntfCopy() 
{
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem == NULL) return;

	int nNodeType = HIWORD(m_Tree.GetItemData(hItem));
	g_pIntf->ClipBoardClear(); // 클립보드 지우고..
	
	if(nNodeType == NODE_INTF) // 레이어 카피
	{
		int nIntf = LOWORD(m_Tree.GetItemData(hItem));
		g_pIntf->ClipBoardAdd(nIntf);
	}
	else if(nNodeType == NODE_LAYER)
	{
		HTREEITEM hIntf = m_Tree.GetChildItem(hItem);
		while(hIntf)
		{
			int nIntf = LOWORD(m_Tree.GetItemData(hIntf));
			g_pIntf->ClipBoardAdd(nIntf);
			hIntf = m_Tree.GetNextSiblingItem(hIntf);
		}
	}
	else return;

	Refresh();
	UpdateWorkWnd();
}

void CDlgIntf::OnMenuIntfPaste() 
{
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem == NULL) return;
	int nNodeType = HIWORD(m_Tree.GetItemData(hItem));

	int nIntf = this->IntfGetValid();
	int nLayer = this->LayerGetValid();

	if(nNodeType == NODE_INTF) // 레이어 카피
	{
		g_pIntf->ClipBoardPaste(nIntf, 0, nLayer);

		HTREEITEM hIntf = hItem;
		this->UpdateNodeIntf(hIntf); // 인터페이스 노드 업데이트
		m_Tree.Expand(hIntf, TVE_EXPAND);
	}
	else if(nNodeType == NODE_LAYER)
	{
		HTREEITEM hLayer = hItem;
		HTREEITEM hIntf = m_Tree.GetChildItem(hItem);
		while(hIntf) // 몽땅 지운다..
		{
			HTREEITEM hTmp = hIntf;
			nIntf = LOWORD(m_Tree.GetItemData(hIntf));
			g_pIntf->Delete(nIntf); // 지우고..
			hIntf = m_Tree.GetNextSiblingItem(hIntf);
		}

		int nClipQt = g_pIntf->ClipBoardGetQt();
		for(int i = 0; i < nClipQt; i++)
		{
			nIntf = g_pIntf->Add();
			g_pIntf->ClipBoardPaste(nIntf, i, nLayer);
			hIntf = m_Tree.InsertItem("", hLayer, TVI_LAST);
			m_Tree.SetItemData(hIntf, MAKELPARAM(nIntf, NODE_INTF));
			this->UpdateNodeIntf(hIntf); // 인터페이스 노드 업데이트
		}
		
		m_Tree.Expand(hItem, TVE_EXPAND);
	}
	else return;

	Refresh();
	UpdateWorkWnd();
}

void CDlgIntf::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if(hItem == NULL) return;

	int nNodeType = HIWORD(m_Tree.GetItemData(hItem));
	if(nNodeType != NODE_LAYER && nNodeType != NODE_INTF && nNodeType != NODE_RENDER_STATE) return;

	Refresh();
	UpdateWorkWnd();

	*pResult = 0;
	
}

void CDlgIntf::OnSelchangeAniList() 
{
	int nSel = m_AniList.GetCurSel();
	if(nSel == CB_ERR) return;

	HTREEITEM hIntf = m_Tree.GetSelectedItem();
	if(hIntf == NULL) return;

	int nNodeType = HIWORD(m_Tree.GetItemData(hIntf));

	HTREEITEM hChild = NULL;
	if(nNodeType == NODE_INTF) hChild = m_Tree.GetChildItem(hIntf); // 인터페이스 노드
	else if(nNodeType == NODE_RENDER_STATE)
	{
		hChild = hIntf;
		hIntf = m_Tree.GetParentItem(hIntf); // 컨디션 노드 - 부모 노드로 바꾼다..!!
	}
	else return;

	RefreshData();
	this->UpdateNodeIntf(hIntf);

	Refresh();
	UpdateWorkWnd();
}

int CDlgIntf::GetRenderStatus()
{
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem == NULL) return 0;

	int nNodeType = HIWORD(m_Tree.GetItemData(hItem));
	
	if(nNodeType == NODE_RENDER_STATE) return LOWORD(m_Tree.GetItemData(hItem));
	else return 0;
}

// 현재 트리에서 선택되어 있는 레이어 번호를 가져온다.. 실패하면 -1..
int CDlgIntf::LayerGetValid()
{
	HTREEITEM hRoot = m_Tree.GetRootItem();
	HTREEITEM hItem = m_Tree.GetSelectedItem();

	if(hItem == NULL || hItem == hRoot) return -1;
	
	int nNodeType = 0;
	while(nNodeType != NODE_LAYER && hItem)
	{
		nNodeType = HIWORD(m_Tree.GetItemData(hItem)); // 노드 타입 가져오고..
		hItem = m_Tree.GetParentItem(hItem);
	}
	if(hItem) return LOWORD(m_Tree.GetItemData(hItem));
	else return -1;
}

int CDlgIntf::IntfGetValid()
{
	HTREEITEM hIntf = m_Tree.GetSelectedItem();
	if(hIntf == NULL) return -1;
	
	int nNodeType = HIWORD(m_Tree.GetItemData(hIntf)); // 노드 타입 가져오고..
	if(nNodeType == NODE_INTF) hIntf = hIntf;
	else if(nNodeType == NODE_RENDER_STATE) hIntf = m_Tree.GetParentItem(hIntf);
	else return -1;

	return LOWORD(m_Tree.GetItemData(hIntf));
}

// 현재 레이어에 속해있는 인터페이스 갯수를 가져온다..
int CDlgIntf::IntfGetValidQt()
{
	HTREEITEM hLayer = m_Tree.GetSelectedItem();
	if(hLayer == NULL) return -1;
	
	int nNodeType = HIWORD(m_Tree.GetItemData(hLayer)); // 노드 타입 가져오고..
	if(nNodeType == NODE_LAYER) hLayer = hLayer;
	else if(nNodeType == NODE_INTF) hLayer = m_Tree.GetParentItem(hLayer);
	else if(nNodeType == NODE_RENDER_STATE)
	{
		hLayer = m_Tree.GetParentItem(hLayer);
		hLayer = m_Tree.GetParentItem(hLayer);
	}
	else return -1;

	HTREEITEM hIntf = m_Tree.GetChildItem(hLayer);
	int nIntfQt = 0;
	while(hIntf) // 실제 숫자 세기..
	{
		nIntfQt++;
		hIntf = m_Tree.GetNextSiblingItem(hIntf);
	}
	return nIntfQt;
}

// 현재 레이어에 속해있는 인터페이스 인덱스를 가져온다..
BOOL CDlgIntf::IntfGetValidIndex(int index, int* pnIntf, int* pnState)
{
	if(index < 0 || index >= MAX_INTF) return FALSE;
	if(pnIntf == NULL || pnState == NULL) return FALSE;

	*pnIntf = -1;
	*pnState = 0;

	HTREEITEM hLayer = m_Tree.GetSelectedItem();
	if(hLayer == NULL) return -1;
	
	int nNodeType = HIWORD(m_Tree.GetItemData(hLayer)); // 노드 타입 가져오고..
	if(nNodeType == NODE_LAYER) hLayer = hLayer;
	else if(nNodeType == NODE_INTF) hLayer = m_Tree.GetParentItem(hLayer);
	else if(nNodeType == NODE_RENDER_STATE)
	{
		*pnState = LOWORD(m_Tree.GetItemData(hLayer)); // 차일드..
		hLayer = m_Tree.GetParentItem(hLayer);
		hLayer = m_Tree.GetParentItem(hLayer);
	}
	else return FALSE;

	HTREEITEM hIntf = m_Tree.GetChildItem(hLayer);
	for(int i = 0; i < index; i++)
	{
		if(hIntf == NULL) return FALSE;
		hIntf = m_Tree.GetNextSiblingItem(hIntf);
	}
	*pnIntf = LOWORD(m_Tree.GetItemData(hIntf));

	return TRUE;
}

int CDlgIntf::IntfGetValidCheck(int index)
{
	if(index < 0 || index >= MAX_INTF) return 0;

	HTREEITEM hLayer = m_Tree.GetSelectedItem();
	if(hLayer == NULL) return 0;
	
	int nNodeType = HIWORD(m_Tree.GetItemData(hLayer)); // 노드 타입 가져오고..
	if(nNodeType == NODE_LAYER) hLayer = hLayer;
	else if(nNodeType == NODE_INTF) hLayer = m_Tree.GetParentItem(hLayer);
	else if(nNodeType == NODE_RENDER_STATE)
	{
		hLayer = m_Tree.GetParentItem(hLayer);
		hLayer = m_Tree.GetParentItem(hLayer);
	}
	else return 0;

	HTREEITEM hIntf = m_Tree.GetChildItem(hLayer);
	for(int i = 0; i < index; i++)
	{
		if(hIntf == NULL) return 0;
		hIntf = m_Tree.GetNextSiblingItem(hIntf);
	}
	return m_Tree.GetCheck(hIntf);
}

// 트리를 뒤져서 인터페이스 번호가 일치하면 선택...
void CDlgIntf::IntfSetValid(int nIntf)
{
	HTREEITEM hRoot = m_Tree.GetRootItem();
	
	HTREEITEM hLayer = m_Tree.GetChildItem(hRoot);
	while(hLayer)
	{
		HTREEITEM hIntf = m_Tree.GetChildItem(hLayer);
		while(hIntf)
		{
			int tmp = LOWORD(m_Tree.GetItemData(hIntf));
			if(tmp == nIntf)
			{
				m_Tree.SelectItem(hIntf);
				return;
			}
			hIntf = m_Tree.GetNextSiblingItem(hIntf);
		}
		hLayer = m_Tree.GetNextSiblingItem(hLayer);
	}
}

// 현재 데이터를 가지고 트리를 갱신한다.
void CDlgIntf::UpdateTreeByData()
{
	CString FileName;
	GetDlgItemText(IDC_FILE_NAME, FileName);
	if(FileName.GetLength() <= 0) FileName = "제목 없음";

	// 트리 리셋
	m_Tree.DeleteAllItems();
	
	BOOL bLayers[MAX_LAYER]; MAKE_ZERO(bLayers);
	HTREEITEM hR = m_Tree.InsertItem(FileName, TVI_ROOT, TVI_LAST);
	HTREEITEM hRoot = m_Tree.GetRootItem();
	HTREEITEM hLayers[MAX_LAYER]; MAKE_ZERO(hLayers);
	
	CString str;
	for(int i = 0 ; i < MAX_INTF; i++)
	{
		_INTF_DATA* pIntf = g_pIntf->IntfGet(i);
		if(pIntf == NULL) continue; // 데이터 가져오기 실패

		int nLayer = pIntf->nParentLayer;
		if(nLayer < 0 || nLayer >= MAX_LAYER)
		{
			CString str; str.Format("%d번째 인터페이스 - 레이어 번호 : %d", i, nLayer);
			MessageBox(str, "잘못된 인터페이스 데이터 검출");
		}
		else
		{
			if(bLayers[nLayer] == TRUE) // 이미 레이어 노드 삽입한 경우...
			{
			}
			else // 레이어 노드 첨 삽입하는 경우
			{
				CString str;
				str.Format("레이어 %d", nLayer);
				hLayers[nLayer] = m_Tree.InsertItem(str, hRoot, TVI_LAST);
				m_Tree.SetItemData(hLayers[nLayer], MAKELPARAM(nLayer, NODE_LAYER));
			}
			bLayers[nLayer] = TRUE;
			
			// 인터페이스 노드 삽입..
			str.Format("%3d", i);
			HTREEITEM hIntf = m_Tree.InsertItem(str, hLayers[nLayer], TVI_LAST);
			m_Tree.SetItemData(hIntf, MAKELPARAM(i, NODE_INTF)); // 인터페이스 노드에 인터페이스 번호 붙이기..

			this->UpdateNodeIntf(hIntf);
		}
	}
	m_Tree.Expand(hRoot, TVE_EXPAND); // 트리 쭉 편다..
}

void CDlgIntf::OnBegindragTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CDlgIntf::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	Refresh();
	UpdateWorkWnd();
	*pResult = 0;
}

void CDlgIntf::OnChangeEditWidth() 
{
	g_pIntf->m_Header.rcWhole.left = 0;
	g_pIntf->m_Header.rcWhole.right = GetDlgItemInt(IDC_EDIT_WIDTH);
	UpdateWorkWnd();
}

void CDlgIntf::OnChangeEditHeight() 
{
	g_pIntf->m_Header.rcWhole.top = 0;
	g_pIntf->m_Header.rcWhole.bottom = GetDlgItemInt(IDC_EDIT_HEIGHT);
	UpdateWorkWnd();
}

void CDlgIntf::UpdateNodeIntf(HTREEITEM hIntf)
{
	if(hIntf == NULL) hIntf = m_Tree.GetSelectedItem(); // 널이면 현재 선택된 노드를 가져온다..
	if(hIntf == NULL) return;

	int nNodeType = HIWORD(m_Tree.GetItemData(hIntf));
	
	if(nNodeType == NODE_INTF) hIntf = hIntf;
	else if(nNodeType == NODE_RENDER_STATE) hIntf = m_Tree.GetParentItem(hIntf);
	else return;

	int nIntf = LOWORD(m_Tree.GetItemData(hIntf));
	_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
	if(pIntf == NULL) return;

	// 인터페이스 밑의 노드에 이름 붙이기..
	CString szChilds[4];
	CString szTmps[4];
	szTmps[0] = "평시-%d:%s";
	szTmps[1] = "커서-%d:%s";
	szTmps[2] = "클릭-%d:%s";
	szTmps[3] = "선택-%d:%s";
	for(int i = 0; i < 4; i++)
	{
		_ANI_DATA* pAni = g_pIntf->AniObjGet()->AniDataGet(pIntf->nAnis[i]);
		if(pAni == NULL) szChilds[i].Format(szTmps[i], pIntf->nAnis[i], "에러! 에니메이션 없음");
		else szChilds[i].Format(szTmps[i], pIntf->nAnis[i], pAni->szName);
	}
	
	HTREEITEM hChild = m_Tree.GetChildItem(hIntf);
	if(hChild == NULL) // 새로 삽입하는 경우
	{
		for(i = 0; i < 4; i++)
		{
			HTREEITEM hChild = m_Tree.InsertItem(szChilds[i], hIntf, TVI_LAST);
			m_Tree.SetItemData(hChild, MAKELPARAM(i, NODE_RENDER_STATE));
		}
	}
	else // 이미 있는 경우
	{
		for(i = 0; i < 4; i++)
		{
			if(i == 0) hChild = m_Tree.GetChildItem(hIntf);
			else hChild = m_Tree.GetNextSiblingItem(hChild);
			m_Tree.SetItemData(hChild, MAKELPARAM(i, NODE_RENDER_STATE));
			m_Tree.SetItemText(hChild, szChilds[i]);
		}
	}

	// 인터페이스 노드 이름 붙이기..
	CString szName = pIntf->szName;
	if(szName.GetLength() <= 0) szName = "이름없음";

	CString szInfo;
	_ANI_DATA* pAni = g_pIntf->AniObjGet()->AniDataGet(pIntf->nAnis[0]);
	if(pAni == NULL) szInfo.Format("%s-%d(%d:에러! 에니메이션 없음)", szName, nIntf, pIntf->nAnis[0]);
	else szInfo.Format("%s-%d(%d:%s)", szName, nIntf, pIntf->nAnis[0], pAni->szName);
	
	m_Tree.SetItemText(hIntf, szInfo);
}

void CDlgIntf::OnOK()
{
}

void CDlgIntf::OnCancel()
{
}

void CDlgIntf::OnBBrowseBkg() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "bmp", NULL, dwFlags, "bmp files(*.bmp;*.obm)|*.bmp;*.obm||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("BMP");

	if(dlg.DoModal() == IDCANCEL) return;
	
	FileName = dlg.GetFileName();

	UpdateWorkWnd();
}

void CDlgIntf::OnSelchangeCbIntfType() 
{
	this->RefreshData();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnChangeEFlag0() 
{
	this->RefreshData();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnChangeEFlag1() 
{
	this->RefreshData();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnChangeEFlag2() 
{
	this->RefreshData();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnChangeEFlag3() 
{
	this->RefreshData();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnSelchangeCbRenderType() 
{
	this->RefreshData();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnChangeEA() 
{
	this->RefreshData();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnChangeER() 
{
	this->RefreshData();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnChangeEG() 
{
	this->RefreshData();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnChangeEB() 
{
	this->RefreshData();
	this->UpdateWorkWnd();
}

void CDlgIntf::OnCViewOutline() 
{
	this->UpdateWorkWnd();
}
