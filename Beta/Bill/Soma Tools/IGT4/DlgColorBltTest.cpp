// DlgColorBltTest.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgColorBltTest.h"
#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgColorBltTest dialog


CDlgColorBltTest::CDlgColorBltTest(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColorBltTest::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgColorBltTest)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgColorBltTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColorBltTest)
	DDX_Control(pDX, IDC_SLD_SPR, m_SldSpr);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_SPreview);
	DDX_Control(pDX, IDC_SLD_R, m_SldR);
	DDX_Control(pDX, IDC_SLD_G, m_SldG);
	DDX_Control(pDX, IDC_SLD_B, m_SldB);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColorBltTest, CDialog)
	//{{AFX_MSG_MAP(CDlgColorBltTest)
	ON_BN_CLICKED(IDC_B_SPR_BROWSE, OnBSprBrowse)
	ON_EN_CHANGE(IDC_E_R, OnChangeER)
	ON_EN_CHANGE(IDC_E_G, OnChangeEG)
	ON_EN_CHANGE(IDC_E_B, OnChangeEB)
	ON_EN_CHANGE(IDC_E_SPR, OnChangeESpr)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColorBltTest message handlers

void CDlgColorBltTest::OnBSprBrowse() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "LSP Sprite(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");

	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetPathName();

	SetDlgItemText(IDC_E_SPR_FILE_NAME, FileName);
	m_AniObj.SprLoad(FileName, g_pTEng->PixelFormatGet());

	m_SldSpr.SetRange(0, m_AniObj.m_HSpr.nQt);
	m_SldSpr.SetPos(0);

	this->UpdatePreview();
}

void CDlgColorBltTest::OnChangeER() 
{
	this->UpdatePreview(); // 프리뷰 업데이트
}

void CDlgColorBltTest::OnChangeEG() 
{
	this->UpdatePreview(); // 프리뷰 업데이트
}

void CDlgColorBltTest::OnChangeEB() 
{
	this->UpdatePreview(); // 프리뷰 업데이트
}

void CDlgColorBltTest::OnChangeESpr() 
{
	this->UpdatePreview(); // 프리뷰 업데이트
}

void CDlgColorBltTest::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl* pSld = (CSliderCtrl*)pScrollBar;

	if(pSld == &m_SldR)
	{
		SetDlgItemInt(IDC_E_R, m_SldR.GetPos());
	}
	else if (pSld == &m_SldG)
	{
		SetDlgItemInt(IDC_E_G, m_SldG.GetPos());
	}
	else if (pSld == &m_SldB)
	{
		SetDlgItemInt(IDC_E_B, m_SldB.GetPos());
	}
}

void CDlgColorBltTest::UpdatePreview()
{
	int nSpr = m_SldSpr.GetPos();
	const _SPR_LSP* pSpr;
	m_AniObj.SprGet(nSpr, &pSpr);
	if(pSpr== NULL) return;

	COLORREF crColor = RGB(m_SldR.GetPos(), m_SldG.GetPos(), m_SldB.GetPos());
	if(crColor == 0) return;

	CRect rc;
	m_SPreview.GetClientRect(&rc);
	int x = rc.Width()/2;
	int y = rc.Height()/2;
	g_pTEng->SurfaceFillRect(NULL, &rc, RGB(64,64,64));
	
	g_pTEng->BackBufferLock();
	g_pTEng->BltLSP(NULL, x, y, pSpr, RENDER_COLOR_LEVEL, 31, crColor);
	g_pTEng->BackBufferUnlock();

	g_pTEng->HWndSet(m_SPreview.m_hWnd);
	g_pTEng->Flip(&rc);
}

void CDlgColorBltTest::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	this->UpdatePreview();	
	// Do not call CDialog::OnPaint() for painting messages
}

BOOL CDlgColorBltTest::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_SldR.SetRange(0,255);
	m_SldG.SetRange(0,255);
	m_SldB.SetRange(0,255);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
