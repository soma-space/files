// DlgScript.cpp : implementation file
//

#include "StdAfx.h"
#include "DlgScript.h"
#include "GlobalVar.h"

CDlgScript::CDlgScript(CWnd* pParent) : CDialog(CDlgScript::IDD)
{
	//{{AFX_DATA_INIT(CDlgScript)
	//}}AFX_DATA_INIT

	for(int i = 0; i < MAX_SCT_COMMAND; i++)
	{
		m_nSC[i] = 0;
		m_SC[i] = "";
		m_SD[i] = "";
	}

	int n = 0;
	n=0; m_nSC[n] = SCT_TALK;				m_SC[n] = "대화";				m_SD[n] = "(Text - 대사내용 F1 - 캐릭터 번호 F2 - 플래그(0:풍선,1:대화창,2:메시지창,4:스크린 고정,8:맵에고정,16:지정된캐릭터에 고정)";

	n++; m_nSC[n] = SCT_IMAGE_OUTPUT;		m_SC[n] = "그림 출력";			m_SD[n] = "(Text - 비트맵 파일 이름 | PT1 - 위치";
	n++; m_nSC[n] = SCT_AVI_PLAY;			m_SC[n] = "동영상 재생";		m_SD[n] = "(Text - 파일 이름, RC1:출력위치(0,0,0,0 이면 전체화면 출력))";

	n++; m_nSC[n] = SCT_LOAD;				m_SC[n] = "부르기";				m_SD[n] = "(Text - 스크립트 파일 이름)";
	n++; m_nSC[n] = SCT_GAME_PAUSE;			m_SC[n] = "게임 잠시 멈춤"; 	m_SD[n] = "(F1 - 0:시간, 1:마우스 클릭할때까지 Time - 시간일 경우 mSec단위 시간, 2:무한정 멈춤)";
	n++; m_nSC[n] = SCT_GAME_RESUME;		m_SC[n] = "게임 다시 진행";		m_SD[n] = "인수 없음";
	n++; m_nSC[n] = SCT_PAUSE;				m_SC[n] = "스크립트 잠시 멈춤"; m_SD[n] = "(F1 - 0:시간, 1:마우스 클릭할때까지 Time - 시간일 경우 mSec단위 시간, 2:무한정 멈춤)";
	n++; m_nSC[n] = SCT_RESUME;				m_SC[n] = "스크립트 다시 진행";	m_SD[n] = "인수 없음";
	n++; m_nSC[n] = SCT_END;				m_SC[n] = "끝";					m_SD[n] = "인수 없음";

	n++; m_nSC[n] = SCT_JUMP;				m_SC[n] = "점프";				m_SD[n] = "(F1 - 스크립트 일련번호)";
	n++; m_nSC[n] = SCT_JUMP_RANDOM;		m_SC[n] = "임의 점프";			m_SD[n] = "(F1 - 스크립트 일련번호, F2 - 스크립트 일련번호, F3 - 스크립트 일련번호)";
	n++; m_nSC[n] = SCT_JUMP_IF;			m_SC[n] = "조건부점프";			m_SD[n] = "(F1 - 판단의 근거가 되는 전역변수 | F2 - 참일때 점프할 스크립트 일련번호 | F2 - 거짓일때 점프할 스크립트 일련번호)";
	n++; m_nSC[n] = SCT_SET_GLOBAL_VAR;		m_SC[n] = "전역변수 설정";		m_SD[n] = "(F1 - 전역변수 번호 | F2 - 참,거짓)";

	n++; m_nSC[n] = SCT_SOUND_PLAY;			m_SC[n] = "음악 재생"; 			m_SD[n] = "사운드 재생 Text - 파일 이름 | F1- 0:임시채널재생, 1:스트리밍채널 | F2- 0:루프, 1:한번만";
	n++; m_nSC[n] = SCT_SOUND_STOP;			m_SC[n] = "음악 멈춤";			m_SD[n] = "F1- 0:임시채널, 1:스트리밍채널";
	
	n++; m_nSC[n] = SCT_MODE_CHANGE;		m_SC[n] = "게임모드 바꿈";		m_SD[n] = "(??????????????)";
	
	n++; m_nSC[n] = SCT_MAP_LOAD_ATTR;		m_SC[n] = "맵 속성 부르기";		m_SD[n] = "(Text - 맵 속성 파일이름 | F1 - 0:메인 배경 1: 서브 배경)";
	n++; m_nSC[n] = SCT_MAP_LOAD_SETTING;	m_SC[n] = "맵 세팅 부르기";		m_SD[n] = "(Text - 맵 세팅 파일이름 | F1 - 0:메인 배경 1: 서브 배경)";
	n++; m_nSC[n] = SCT_MAP_RETURN;			m_SC[n] = "주배경으로 복귀";	m_SD[n] = "(Text - 맵 세팅 파일이름, 없으면 그냥 원래 맵으로 복귀";
	n++; m_nSC[n] = SCT_MAP_SCROLL;			m_SC[n] = "맵 스크롤";			m_SD[n] = "(F1 - 0:보통 1:워프 | PT1 - 맵 좌표)";

	n++; m_nSC[n] = SCT_CHR_SET_ATTACK;		m_SC[n] = "캐릭터 공격";		m_SD[n] = "(F1 - 캐릭터 번호 | F2 - 대상 캐릭터)";
	n++; m_nSC[n] = SCT_CHR_SET_DIRECTION;	m_SC[n] = "캐릭터 방향지정";	m_SD[n] = "(F1 - 캐릭터 번호 | F2 - 0~7 정면부터시계방향)";
	n++; m_nSC[n] = SCT_CHR_SET_ANIMATION;	m_SC[n] = "캐릭터 에니메이션";	m_SD[n] = "(F1 - 캐릭터 번호 | F2 - 에니메이션 번호 | F3 - 0:Loop 1:한번만)";
	n++; m_nSC[n] = SCT_CHR_SET_MOVE;		m_SC[n] = "캐릭터 이동";		m_SD[n] = "(F1 - 캐릭터 번호 | F2 - 0:걸어서이동, 1:워프, 8:뛰어감 F3 - 0:다음 스크립트로 바로 진행 1:이동 끝날 때까지 기다림 | PT1 - Map 픽셀 좌표)";
	n++; m_nSC[n] = SCT_CHR_STOP_MOVE;		m_SC[n] = "캐릭터 멈춤";		m_SD[n] = "(F1 - 캐릭터 번호)";
	n++; m_nSC[n] = SCT_CHR_SET_PARTY;		m_SC[n] = "캐릭터 파티 지정";	m_SD[n] = "(F1 - 캐릭터 번호 | F2 - 0:쫓아냄 1:합류)";

	n++; m_nSC[n] = SCT_FX_ENV_BEGIN;		m_SC[n] = "특수효과 시작";		m_SD[n] = "(F1 - 특수효과종류 | Time - 지속 시간 mSec)";
	n++; m_nSC[n] = SCT_FX_ENV_END;			m_SC[n] = "특수효과 멈춤";		m_SD[n] = "(F1 - 특수효과종류)";
	n++; m_nSC[n] = SCT_RENDER_MAIN;		m_SC[n] = "렌더링 시작/멈춤";	m_SD[n] = "(F1 - 0:멈춤 1:시작)";

	n++; m_nSC[n] = SCT_INTF_SET;			m_SC[n] = "인터페이스 설정";	m_SD[n] = "(F1 - 인터페이스 번호 | F2 - 레이어 번호 | F3 - 버튼 번호)";
}

CDlgScript::~CDlgScript()
{
}

void CDlgScript::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScript)
	DDX_Control(pDX, IDC_E_SCRIPT_FILE_NAME, m_Edit_ScriptFileName);
//	DDX_Control(pDX, IDC_STATIC_FILENAME, m_Static_FileName);
//	DDX_Control(pDX, IDC_STATIC_USAGE, m_Static_Usage);
	DDX_Control(pDX, IDC_E_SCRIPT_USAGE, m_Edit_Usage);
	DDX_Control(pDX, IDC_B_SCRIPT_NEW, m_Button_New);
	DDX_Control(pDX, IDC_B_SCRIPT_LOAD, m_Button_Load);
	DDX_Control(pDX, IDC_B_SCRIPT_SAVE, m_Button_Save);
	DDX_Control(pDX, IDC_B_SCRIPT_SAVE_AS, m_Button_SaveAs);
	DDX_Control(pDX, IDC_SPREAD_SCRIPT, m_Sheet);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// DlgScript message handlers
BEGIN_MESSAGE_MAP(CDlgScript, CDialog)
	//{{AFX_MSG_MAP(CDlgScript)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_SAVE_AS, OnSaveAs)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_SCRIPT_EXPAND_WIDTH, OnSheetExpandWidth)
	ON_COMMAND(ID_SCRIPT_REDUCE_WIDTH, OnSheetReduceWidth)
	ON_COMMAND(ID_SCRIPT_REPAIR_WIDTH, OnSheetRepairWidth)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgScript::Refresh()
{
	UpdateData(FALSE);
}

BOOL CDlgScript::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// 각셀의 원래 너비와 높이를 기억한다..
	int w = m_Sheet.GetMaxCols();
	int i;
	for(i = 0; i <= w; i++)
	{
		m_Widths[i] = m_Sheet.GetColWidth(i);
	}

	// 스크립트 스프레드 시트 초기화..
	m_Sheet.SetCol(1); m_Sheet.SetCol2(9); m_Sheet.SetRow(1); m_Sheet.SetRow2(1024);
	m_Sheet.SetBlockMode(TRUE);
	m_Sheet.SetAction(3); // 범위 안에 있는 모든 데이터 리셋
	m_Sheet.SetBlockMode(FALSE);

	// 스크립트리스트를  콤보 박스에 입력
	long rows;
	rows = m_Sheet.GetMaxRows();
	CString list;
	for(int m = 0; m < MAX_SCT_COMMAND; m++)
	{
		if(m_SC[m].GetLength() != 0)
		{
			list += m_SC[m];
			list += '\t'; // 탭을 붙여주면 된다.
		}
	}

	for(int l = 1; l <= MAX_SCT; l++)
	{
		m_Sheet.SetCol(1);
		m_Sheet.SetRow(l);
		m_Sheet.SetTypeComboBoxList(list);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgScript::OnOK()
{
}

void CDlgScript::OnCancel()
{
}

void CDlgScript::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	BOOL b = FALSE;
	m_Sheet.SetWindowPos(NULL, 10, 10, cx - 20, cy - 130, NULL);

//	m_Static_Usage.SetWindowPos(NULL, 10, cy - 115, 50, 25, NULL);
	m_Edit_Usage.SetWindowPos(NULL, 70, cy - 115, cx - 80, 25, NULL);

//	m_Static_FileName.SetWindowPos(NULL, 10, cy - 85, 50, 25, NULL);
	m_Edit_ScriptFileName.SetWindowPos(NULL, 70, cy - 85, cx - 80, 25, NULL);
	
	int s = (cx-20) / 4;
	m_Button_New.SetWindowPos(NULL, 10, cy - 55, s-10, 25, NULL);
	m_Button_Load.SetWindowPos(NULL, 10 + s*1, cy - 55, s-10, 25, NULL);
	m_Button_Save.SetWindowPos(NULL, 10 + s*2, cy - 55, s-10, 25, NULL);
	m_Button_SaveAs.SetWindowPos(NULL, 10 + s*3, cy - 55, s-10, 25, NULL);
}

void CDlgScript::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_szSctFN = ""; // 모두 초기화...
	m_Script.Release();

	CDialog::OnCancel();
//	ShowWindow(SW_HIDE);
	CDialog::OnClose();
}

BEGIN_EVENTSINK_MAP(CDlgScript, CDialog)
    //{{AFX_EVENTSINK_MAP(CDlgScript)
	ON_EVENT(CDlgScript, IDC_SPREAD_SCRIPT, -602 /* KeyDown */, OnSheetKeyDown, VTS_PI2 VTS_I2)
	ON_EVENT(CDlgScript, IDC_SPREAD_SCRIPT, 3 /* ButtonClicked */, OnSheetButtonClicked, VTS_I4 VTS_I4 VTS_I2)
	ON_EVENT(CDlgScript, IDC_SPREAD_SCRIPT, 22 /* RightClick */, OnSheetRightClick, VTS_I2 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CDlgScript, IDC_SPREAD_SCRIPT, 5 /* Click */, OnSheetClick, VTS_I4 VTS_I4)
	ON_EVENT(CDlgScript, IDC_SPREAD_SCRIPT, 31 /* ComboSelChange */, OnSheetComboSelChange, VTS_I4 VTS_I4)
	ON_EVENT(CDlgScript, IDC_SPREAD_SCRIPT, 32 /* TextTipFetch */, OnSheetTextTipFetch, VTS_I4 VTS_I4 VTS_PI2 VTS_PI4 VTS_PBSTR VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDlgScript::OnNew()
{
	// 몽땅 초기화..
	m_Script.Release();
	SetDlgItemText(IDC_E_SCRIPT_FILE_NAME, "");

	// 스크립트 스프레드 시트 초기화..
	RefreshSheet();
}

void CDlgScript::OnLoad() 
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "Sct", NULL, dwFlags, "Script files(*.sct)|*.sct|All files(*.*)|*.*||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SCRIPT");
	if(dlg.DoModal() == IDCANCEL) return;

	CString FileName = dlg.GetFileName();

	// 로딩 및 스프레드 시트 갱신
	g_pTEng->PathSetCur("SCRIPT");
	m_Script.Load(FileName);
	RefreshSheet();
	SetDlgItemText(IDC_E_SCRIPT_FILE_NAME, FileName);
}

void CDlgScript::OnSave()
{
	CString FileName;
	GetDlgItemText(IDC_E_SCRIPT_FILE_NAME, FileName);
	if(FileName.GetLength() == NULL)
	{
		OnSaveAs();
		return;
	}
	
	g_pTEng->PathSetCur("SCRIPT");

	m_Script.Release(); // 스크립트 데이터 새로..
	for(int i = 0; i < MAX_SCT; i++) // 시트이 데이터 인코딩 및 정리...
	{
		_SCT_DATA sct;
		MAKE_ZERO(sct);
		Encode(i+1, &sct);
		if(sct.nCommand == 0) break; // 아무것도 없음 중지..
		m_Script.Set(i, &sct);
		if(sct.szText) delete [] sct.szText; sct.szText = NULL;
	}

	m_Script.Save(FileName);

	int n = m_Script.GetQuantity();
	CString str;
	str.Format("%d 개의 스크립트를 저장하였습니다.", n);
	MessageBox(str);
}

void CDlgScript::OnSaveAs()
{
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(FALSE, "Sct", NULL, dwFlags, "스크립트 파일(*.sct)|*.sct||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SCRIPT");
	if(dlg.DoModal() == IDCANCEL) return;

	CString FileName = dlg.GetFileName();
	SetDlgItemText(IDC_E_SCRIPT_FILE_NAME, FileName);
	OnSave();
}

void CDlgScript::OnSheetButtonClicked(long Col, long Row, short ButtonDown) 
{
	if(Col == 3)
	{
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		CFileDialog dlg(TRUE, "All", NULL, dwFlags, "All files(*.*)|*.*||", NULL);
		dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("");
		if(dlg.DoModal() == IDCANCEL) return;

		CString FileName = dlg.GetFileName();

		if(FileName.GetLength() != 0)
		{
			m_Sheet.SetCol(Col-1); m_Sheet.SetRow(Row); m_Sheet.SetText(FileName);
		}
	}
}

void CDlgScript::OnSheetRightClick(short ClickType, long Col, long Row, long MouseX, long MouseY) 
{
	CMenu popup;
	CMenu* pItemMenu;
	popup.LoadMenu(IDM_POPUP_SCRIPT);
	pItemMenu = popup.GetSubMenu(0);

	CPoint point;
	GetCursorPos(&point);
	BOOL b = pItemMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CDlgScript::OnSheetExpandWidth() 
{
	long x1, x2;
	x1 = m_Sheet.GetSelBlockCol();
	x2 = m_Sheet.GetSelBlockCol2();
	for(;x1 <= x2; x1++)
	{
		m_Sheet.SetColWidth(x1, m_Widths[x1]);
	}
}

void CDlgScript::OnSheetReduceWidth() 
{
	long x1, x2;
	m_Sheet.GetBottomRightCell(&x1, &x2);
	x1 = m_Sheet.GetSelBlockCol();
	x2 = m_Sheet.GetSelBlockCol2();
	for(;x1 <= x2; x1++)
	{
		m_Sheet.SetColWidth(x1, 2.0f);
	}
}

void CDlgScript::OnSheetRepairWidth() 
{
	int w = m_Sheet.GetMaxCols();
	for(int i = 1; i <= w; i++)
	{
		m_Sheet.SetColWidth(i, m_Widths[i]);
	}
}

void CDlgScript::OnSheetClick(long Col, long Row)
{
	if(Row != 0) return; // 헤더 클릭시에...
	if(m_Sheet.GetColWidth(Col) != 2.0f) m_Sheet.SetColWidth(Col, 2.0f);
	else if(m_Sheet.GetColWidth(Col) == 2.0f) m_Sheet.SetColWidth(Col, m_Widths[Col]);
}

void CDlgScript::OnSheetKeyDown(short FAR* KeyCode, short Shift) 
{
	int col, row, col2, row2;
	m_Sheet.SetBlockMode(TRUE);
	col = m_Sheet.GetSelBlockCol(); col2 = m_Sheet.GetSelBlockCol2();
	row = m_Sheet.GetSelBlockRow(); row2 = m_Sheet.GetSelBlockRow2();
	m_Sheet.SetBlockMode(FALSE);
	
	if(col == -1 && col2 == -1)
	{
		switch(*KeyCode)
		{
		case VK_DELETE:
			m_Sheet.SetRow(row);
			m_Sheet.SetRow2(row2);
			m_Sheet.SetBlockMode(TRUE);
			m_Sheet.SetAction(5); // 행 삭제
			m_Sheet.SetBlockMode(FALSE);
			break;
		case VK_INSERT:
			m_Sheet.SetRow(row);
			m_Sheet.SetAction(7); // 행 삽입
			break;
		}
	}
}

void CDlgScript::Encode(int nRow, _SCT_DATA* pSct)
{
	if(pSct == NULL) return;

	long tmp = 0;
	CString str;
	m_Sheet.SetCol(1); m_Sheet.SetRow(nRow); str = m_Sheet.GetText(); // 아무것도 안 써있으면..
	if(str.GetLength() <= 0)
	{
		MAKE_ZERO(*pSct); 
		return;
	}
	
	m_Sheet.SetCol(1); m_Sheet.SetRow(nRow);
	int sel = m_Sheet.GetTypeComboBoxCurSel();
	pSct->nCommand = m_nSC[sel]; // 그냥 숫자가 아니다.. 정의된 숫자이다..
	m_Sheet.SetCol(2); m_Sheet.SetRow(nRow);
	str = m_Sheet.GetText();
	if(str.GetLength() > 0)
	{
		pSct->szText = new char[str.GetLength() + 1]; // 반드시 해제 바람!!!
		lstrcpy(pSct->szText, str);
	}
	m_Sheet.GetInteger(4, nRow, &tmp); pSct->nFlag1 = tmp;
	m_Sheet.GetInteger(5, nRow, &tmp); pSct->nFlag2 = tmp;
	m_Sheet.GetInteger(6, nRow, &tmp); pSct->nFlag3 = tmp;
	m_Sheet.SetCol(7); m_Sheet.SetRow(nRow); str = m_Sheet.GetText(); pSct->pt = ConvertStr2Point(str);
	m_Sheet.SetCol(8); m_Sheet.SetRow(nRow); str = m_Sheet.GetText(); pSct->rc = ConvertStr2Rect(str);
	m_Sheet.SetCol(9); m_Sheet.SetRow(nRow); str = m_Sheet.GetText(); pSct->fSpeed = (float)atof(str);
	m_Sheet.GetInteger(10, nRow, &tmp); pSct->dwTime = tmp;
}

void CDlgScript::Decode(int nRow, const _SCT_DATA* pSct)
{
	// 스크립트 명령 초기화..
	m_Sheet.SetCol(1); m_Sheet.SetRow(nRow);
	m_Sheet.SetTypeComboBoxCurSel(0); m_Sheet.SetText("");

	m_Sheet.SetCol(2); m_Sheet.SetCol2(9); m_Sheet.SetRow(nRow); m_Sheet.SetRow2(nRow);
	m_Sheet.SetBlockMode(TRUE);
	m_Sheet.SetAction(3); // 범위 안에 있는 모든 데이터 리셋
	m_Sheet.SetBlockMode(FALSE);

	if(pSct)
	{
		long tmp = 0;
		CString str;
		
		BOOL bExist = FALSE;
		for(int i = 0; i < MAX_SCT_COMMAND; i++)
		{
			tmp = pSct->nCommand;
			if(tmp == m_nSC[i])
			{
				m_Sheet.SetCol(1); m_Sheet.SetRow(nRow); m_Sheet.SetTypeComboBoxCurSel(i);
				bExist = TRUE;
			}
		}
		if(bExist == FALSE) return;

		m_Sheet.SetCol(2); m_Sheet.SetRow(nRow); 
		if(lstrlen(pSct->szText) != 0) m_Sheet.SetText(pSct->szText);

		tmp = pSct->nFlag1; if(tmp != 0) m_Sheet.SetInteger(4, nRow, tmp);
		tmp = pSct->nFlag2; if(tmp != 0) m_Sheet.SetInteger(5, nRow, tmp);
		tmp = pSct->nFlag3; if(tmp != 0) m_Sheet.SetInteger(6, nRow, tmp);

		if(pSct->pt.x != 0 || pSct->pt.y != 0)
		{
			str.Format("%.4d, %.4d", pSct->pt.x, pSct->pt.y);
			m_Sheet.SetCol(7); m_Sheet.SetRow(nRow); m_Sheet.SetText(str);
		}
		
		if(pSct->rc.left != 0 || pSct->rc.top != 0 || pSct->rc.right != 0 || pSct->rc.bottom != 0)
		{
			str.Format("%.4d, %.4d, %.4d, %.4d", pSct->rc.left, pSct->rc.top, pSct->rc.right, pSct->rc.bottom);
			m_Sheet.SetCol(8); m_Sheet.SetRow(nRow); m_Sheet.SetText(str);
		}

		if(pSct->fSpeed != 0)
		{
			m_Sheet.SetFloat(9, nRow, pSct->fSpeed);
		}

		tmp = pSct->dwTime; if(tmp != 0) m_Sheet.SetInteger(10, nRow, tmp);
	}
}

CRect CDlgScript::ConvertStr2Rect(CString& string)
{
	int nS = 0;
	int nE = 0;
	CString ts;
	CRect rc;
	nS = 0;
	nE = string.Find(",", nS);
	ts = string.Mid(nS, nE-nS);
	rc.left = atoi(ts.GetBuffer(ts.GetLength()));

	nS = nE+1;
	nE = string.Find(",", nS);
	ts = string.Mid(nS, nE-nS);
	rc.top = atoi(ts.GetBuffer(ts.GetLength()));

	nS = nE+1;
	nE = string.Find(",", nS);
	ts = string.Mid(nS, nE-nS);
	rc.right = atoi(ts.GetBuffer(ts.GetLength()));

	nS = nE+1;
	nE = string.Find(",", nS);
	ts = string.Mid(nS, string.GetLength()-nS);
	rc.bottom = atoi(ts.GetBuffer(ts.GetLength()));

	return rc;
}

CPoint CDlgScript::ConvertStr2Point(CString& string)
{
	int nS = 0;
	int nE = 0;
	CString ts;
	CPoint pt;
	nS = 0;
	nE = string.Find(",", nS);
	ts = string.Mid(nS, nE-nS);
	pt.x = atoi(ts.GetBuffer(ts.GetLength()));

	nS = nE+1;
	nE = string.Find(",", nS);
	ts = string.Mid(nS, string.GetLength()-nS);
	pt.y = atoi(ts.GetBuffer(ts.GetLength()));

	return pt;
}

void CDlgScript::OnSheetComboSelChange(long Col, long Row) 
{
}

void CDlgScript::OnSheetTextTipFetch(long Col, long Row, 
		short FAR* MultiLine, long FAR* TipWidth, BSTR FAR* TipText, BOOL FAR* ShowTip) 
{
	if(Col == 1) 
	{
		m_Sheet.SetCol(Col);
		m_Sheet.SetRow(Row);
		CString str = m_Sheet.GetText();
		CString tip;
		for(int i=0; i < MAX_SCT_COMMAND; i++)
		{
			if(str == m_SC[i])
			{
				tip = m_SD[i];
				break;
			}
		}
		if(tip.GetLength())
		{
			*ShowTip = 1;
			*TipText = tip.AllocSysString();
			* MultiLine = FALSE;
//			*TipWidth = tip.GetLength()*128;
		}
	}
}


void CDlgScript::LoadScript(const char* szFileName)
{
	m_szSctFN = szFileName;
}

void CDlgScript::RefreshSheet()
{
	int nQt = m_Script.GetQuantity();
	for(int i = 0; i < MAX_SCT; i++)
	{
		const _SCT_DATA* pSct = m_Script.Get(i);
		Decode(i+1, pSct);
	}
}

void CDlgScript::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if(bShow == TRUE)
	{
		if(m_szSctFN.GetLength() > 0)
		{
			// 로딩 및 스프레드 시트 갱신
			g_pTEng->PathSetCur("SCRIPT");
			m_Script.Load(m_szSctFN);
			RefreshSheet();
			SetDlgItemText(IDC_E_SCRIPT_FILE_NAME, m_szSctFN);
		}
	}
}
