// AutoSpriteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSprAutoExtract.h"

#include "DDEngineTool4.h"
#include "DlgFolderSelect.h"
#include "..\\IGObject\\Dib16.h"
#include "AniObjTool.h"
#include "DlgSetCenter.h"

#include "GlobalVar.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSprAutoExtract property page

CDlgSprAutoExtract::CDlgSprAutoExtract() : CDialog(CDlgSprAutoExtract::IDD)
{
	//{{AFX_DATA_INIT(CDlgSprAutoExtract)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bStopProcess = FALSE;
}

CDlgSprAutoExtract::~CDlgSprAutoExtract()
{
}

void CDlgSprAutoExtract::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSprAutoExtract)
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_FILE_NAME, m_FileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSprAutoExtract, CDialog)
	//{{AFX_MSG_MAP(CDlgSprAutoExtract)
	ON_BN_CLICKED(IDC_BROWSE_WORK_FOLDER_AND_START_WORKING, OnBrowseWorkFolderAndStartWorking)
	ON_BN_CLICKED(IDC_CLOSEDLG, OnClosedlg)
	ON_BN_CLICKED(IDC_CANCEL_PROCESS, OnCancelProcess)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSprAutoExtract message handlers

void CDlgSprAutoExtract::OnBrowseWorkFolderAndStartWorking() 
{
	// TODO: Add your control notification handler code here
	CDlgFolderSelect dlgFS;
	if(dlgFS.DoModal() == IDCANCEL) return;

	CString Path = dlgFS.GetFolderName();

	m_bStopProcess = FALSE; // 일단 중단 플래그 거짓으로,. 버튼 누르면 바뀐다..
	SetCurrentDirectory(Path);

	int check = GetCheckedRadioButton(IDC_R_SPL, IDC_R_SP8);

	CDlgSetCenter DlgSC; // 센터점 잡기
	if(DlgSC.DoModal() == IDCANCEL) return;
	CPoint ptCenter = DlgSC.GetCenter(); // 센터점..

	switch(check)
	{
	case IDC_R_SPL:
	case IDC_R_SP8:
		{
			BOOL bSuccess = FALSE;
			CString SrcFN = ProcessLSPSprSimple(Path, ptCenter, 0);
/*			if(check == IDC_R_SP8) // 8 비트 파일을 만들 경우..
			{
				DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				CFileDialog dlg(TRUE, "bmp", NULL, dwFlags, "8 비트 비트맵 파일(*.bmp)|*.bmp||", NULL);
				char szBuff[128]; lstrcpy(szBuff, Path); lstrcat(szBuff, "에 대한 8비트 인덱스 비트맵 파일 지정");
				dlg.m_ofn.lpstrTitle = szBuff;
				dlg.m_ofn.lpstrInitialDir = Path;

				if(dlg.DoModal() == IDCANCEL ) break;

				CString IndexFN = dlg.GetPathName();
				CString DestFN = SrcFN;
				int n = DestFN.GetLength();
				DestFN.SetAt(n-3, 's');
				DestFN.SetAt(n-2, 'p');
				DestFN.SetAt(n-1, '8');
				// 단순 LSP 파일을 8비트 인덱스 파일로 바꾼다..
//					BOOL bSuccess = g_pTEng->ConvertSImpleLSPFileToIndexedLSPFile(SrcFN, DestFN, IndexFN);
				if(!bSuccess) break;
				CString tbuff;
				tbuff = SrcFN + " -> " + DestFN + " 로 저장 되었습니다.\n 원본 " + SrcFN + "을 지울까요?";
				int yesno = MessageBox(tbuff, "저장 확인", MB_YESNO);
				if(yesno == IDYES) remove(SrcFN); // 원본은 지운다..
			}
			if(m_bStopProcess == TRUE) break;
*/
		}
		break;
	default:
		MessageBox("작업 종류 지정이 안되어 있습니다.");
		break;
	}
}

void CDlgSprAutoExtract::OnClosedlg() 
{
	CDialog::OnCancel();
}

void CDlgSprAutoExtract::OnCancelProcess() 
{
	m_bStopProcess = TRUE;
}

BOOL CDlgSprAutoExtract::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckRadioButton(IDC_R_SPL, IDC_R_SP8, IDC_R_SPL); // 라디오 버튼 체크
	SetDlgItemInt(IDC_REMOVE_PIXEL_DELTA, 100); // 기본적으로 100 을 주면 그 값만큼 픽셀을 색상에 따라 깎고 보정한다.

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CDlgSprAutoExtract::ProcessLSPSprSimple(const char* szPath, POINT ptCenter, int nSubFolderDepth)
{
	static nSFD = 0;
	if(nSubFolderDepth == 0) nSFD = 0;

	SetCurrentDirectory(szPath);
	CString Path = szPath;
	
	CFileFind finder; // 폴더에 있는 파일을 다 찾는다.
	BOOL bWorking = finder.FindFile("*.*");

	CString FileName;
	char* szFNs = new char [1024*256]; memset(szFNs, 0, 1024*256);
	int nFileCount = 0; // 총 파일 갯수..
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		FileName = finder.GetFilePath();
		if(finder.IsDirectory() == TRUE && FileName.Right(2) == "\\." || FileName.Right(3) == "\\..") continue;
		
		if(finder.IsDirectory() == TRUE)
		{
			nSFD++;
			this->ProcessLSPSprSimple(FileName, ptCenter, nSFD); // 폴더는 빼고 딴다.. // 재귀 호출이다.. 조심하자.
			nSFD--;
		}

		if(lstrcmpi(FileName.Right(3), "bmp") == 0)
		{
			lstrcpy(&szFNs[nFileCount*256], FileName);
			nFileCount++;
		}
	}
	if(nFileCount <= 0)
	{
		DELETE_ARRAY_SAFE(szFNs);
		return "";
	}

	CFile file; // goto 문 땜시 부득이 하게 옮김..
	CFileDialog* pDlg = NULL;

	int n1 = 0;
	CString szFolder;
	char szBuff[256]; MAKE_ZERO(szBuff);
	int n2 = 0;
	for(int j = Path.GetLength(); j > 2; j--)
	{
		if(n1 > nSubFolderDepth) break;
		if(Path[j-1] == '\\')
		{
			szBuff[n2++] = '_';
			n1++;
		}
		else
		{
			if(Path[j-1] & 0x80)
			{
				szBuff[n2++] = Path[j-2];
				szBuff[n2++] = Path[j-1];
				j--;
			}
			else szBuff[n2++] = Path[j-1];
		}
	}
	szFolder = szBuff;
	szFolder.MakeReverse(); // 문자열 뒤집기.
	if(szFolder.GetLength() <= 0)
	{
		DELETE_ARRAY_SAFE(szFNs);
		return "";
	}

	int nFrmCount = 0;

	// 방향과 프레임 번호로 정렬 	// "NASTASHA_0_01_00_00_V1_A_4_0000.bmp"
	qsort(szFNs, nFileCount, 256, CompareDirAndFrmsProc);

	// 스프라이트 데이터 준비..
	CAniObjTool sprobj;
	sprobj.SprAlloc(nFileCount);

	BOOL bSuccess = FALSE;
	int nReinforceDelta = GetDlgItemInt(IDC_REMOVE_PIXEL_DELTA); // 깎아낼 넘위 지정.. 지정했을 경우 깎아내고 보정...
	m_Progress.SetRange(0, nFileCount);
	for(int i = 0; i < nFileCount; i++)
	{
		m_Progress.SetPos(i+1); // 진행 상황
		CString str;
		str.Format("%s - 프레임 : %d", &szFNs[i*256], i);
		m_FileName.SetWindowText(str);
		
		const _SPR_LSP* pSprTmp = NULL;
		sprobj.SprGet(i, &pSprTmp);
		_SPR_LSP* pSpr = (_SPR_LSP*)pSprTmp;
		if(pSpr == NULL) goto ERROR_PROCESS;

		bSuccess = g_pTEng->TransactPieceLSPSpr(&szFNs[i*256], nReinforceDelta, ptCenter, pSpr); // 파일 타이틀에 따라 스프라이트 및 LSP 자동 처리..		
		UpdatePreview(pSprTmp); // 프리뷰 업데이트
		if(bSuccess == FALSE) goto ERROR_PROCESS;// 변환에 실패 하거나 Esc 누르면 변환 중단..
		if(m_bStopProcess == TRUE) goto STOP_PROCESS;

		// 시스템을 계속 유지한다...  // 강제로 새로 업데이트
		MSG msg; PeekMessage(&msg, NULL, 0, 0, PM_REMOVE); TranslateMessage(&msg); DispatchMessage(&msg); this->UpdateWindow();
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////
	// 스프라이트 파일 저장 ..
	if(pDlg) { delete pDlg; pDlg = NULL; }
	pDlg = new CFileDialog(FALSE, "spl", szFolder+".spl", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "단순 LSP 스프라이트(*.spl)|*.spl||", NULL);
	pDlg->m_ofn.lpstrTitle = "스프라이트 파일 저장";
	pDlg->m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR_자동추출");
	
	if(pDlg->DoModal() == IDCANCEL)
	{
		delete pDlg; pDlg = NULL;
		goto STOP_PROCESS;
	}
	
	FileName = pDlg->GetPathName();

	// 투명색 검출.... 가능하면..
	if(lstrlen(&szFNs[0]) > 0)
	{
		SetCurrentDirectory(Path);
		LPDIRECTDRAWSURFACE lpDDS = g_pTEng->SurfaceCreateWithBMP(&szFNs[0], FALSE, 0);
		if(lpDDS)
		{
			DDSURFACEDESC ddsd;
			g_pTEng->SurfaceLock(lpDDS, &ddsd);
			sprobj.m_HSpr.SrcCK = g_pTEng->RGB24(GET_ARRAY_VALUE((WORD*)ddsd.lpSurface, ddsd.lPitch / 2, 0, 0)); // 화면의 0,0 의 색..
			g_pTEng->SurfaceUnlock(lpDDS);
			RELEASE_SAFE(lpDDS);
		}
	}
	sprobj.SprSave(FileName); // 실제 저장..
	
	goto STOP_PROCESS;

ERROR_PROCESS:
	FileName.Format(" %d 프레임 처리중 에러 발생했어", i);
	MessageBox(&szFNs[i*256], "Error");
	FileName = "";

STOP_PROCESS:
	
	DELETE_ARRAY_SAFE(szFNs);
	DELETE_SAFE(pDlg);

	return FileName;
}

void CDlgSprAutoExtract::UpdatePreview(const _SPR_LSP* pSpr)
{
	if(!pSpr) return;

	CRect rc;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_PREVIEW);
	pWnd->GetClientRect(&rc);
	
	g_pTEng->SurfaceFillRect(NULL, &rc, RGB(64,64,64));
	
	g_pTEng->BackBufferLock();
	g_pTEng->BltLSP(NULL, rc.Width()/2, rc.Height()/2, pSpr);
	g_pTEng->BackBufferUnlock();
	
	g_pTEng->HWndSet(pWnd->m_hWnd);
	g_pTEng->Flip(&rc);
}

void CDlgSprAutoExtract::ClearPreview()
{
	CWnd* pWnd = GetDlgItem(IDC_STATIC_PREVIEW);
	CDC* pDC = pWnd->GetDC();
	CRect rc;
	pWnd->GetClientRect(&rc);
	pDC->FillSolidRect(&rc, RGB(96,96,96));
	pWnd->ReleaseDC(pDC);
}

int CDlgSprAutoExtract::CompareIntegerProc(const void *arg1, const void *arg2)
{
	int frame1 = *(int*)arg1;
	int frame2 = *(int*)arg2;

	if(frame1 < frame2) return -1;
	else if(frame1 == frame2) return 0;
	return 1;
}

int CDlgSprAutoExtract::CompareDirAndFrmsProc(const void *arg1, const void *arg2)
{
	CString FN1 = (const char*)arg1;
	CString FN2 = (const char*)arg2;

	char szDir1[3]; 
	char szDir2[3]; 
	lstrcpy(szDir1, FN1.Mid(FN1.GetLength() - 11, 2));
	lstrcpy(szDir2, FN2.Mid(FN2.GetLength() - 11, 2));

	if(szDir1[0] < '0' || szDir1[0] > '9') szDir1[0] = '0';
	if(szDir2[0] < '0' || szDir2[0] > '9') szDir2[0] = '0';
	if(szDir1[1] < '0' || szDir1[1] > '9') szDir1[1] = NULL;
	if(szDir2[1] < '0' || szDir2[1] > '9') szDir2[1] = NULL;

	// "00_0000.bmp"
	int dir1 = atoi(szDir1);
	int dir2 = atoi(szDir2);

	int frm1 = atoi(FN1.Mid(FN1.GetLength() - 8, 4));
	int frm2 = atoi(FN2.Mid(FN2.GetLength() - 8, 4));
	
	int t1 = dir1*1000 + frm1;
	int t2 = dir2*1000 + frm2;

	if(t1 < t2) return -1;
	else if(t1 > t2) return 1;
	else return 0;

	return 0;
}

int CDlgSprAutoExtract::CompareStringAscendProc(const void *arg1, const void *arg2)
{
	const char* pStr1 = (char*)arg1;
	const char* pStr2 = (char*)arg2;

	return lstrcmpi(pStr1, pStr2);
}

void CDlgSprAutoExtract::OnOK()
{

}

void CDlgSprAutoExtract::OnCancel()
{

}
