// DlgColor.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgColor dialog


CDlgColor::CDlgColor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgColor::IDD, pParent)
{
	m_crColor = 0;
	//{{AFX_DATA_INIT(CDlgColor)
	//}}AFX_DATA_INIT
}


void CDlgColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColor)
	DDX_Control(pDX, IDC_SLD_R, m_SldR);
	DDX_Control(pDX, IDC_SLD_G, m_SldG);
	DDX_Control(pDX, IDC_SLD_B, m_SldB);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_StaticColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColor, CDialog)
	//{{AFX_MSG_MAP(CDlgColor)
	ON_EN_CHANGE(IDC_E_R, OnChangeER)
	ON_EN_CHANGE(IDC_E_G, OnChangeEG)
	ON_EN_CHANGE(IDC_E_B, OnChangeEB)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColor message handlers

void CDlgColor::OnChangeER() 
{
	this->UpdateColor();
}

void CDlgColor::OnChangeEG() 
{
	this->UpdateColor();
}

void CDlgColor::OnChangeEB() 
{
	this->UpdateColor();
}

void CDlgColor::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(pScrollBar == (CScrollBar*)&m_SldR) SetDlgItemInt(IDC_E_R, m_SldR.GetPos());
	else if(pScrollBar == (CScrollBar*)&m_SldG) SetDlgItemInt(IDC_E_G, m_SldG.GetPos());
	else if(pScrollBar == (CScrollBar*)&m_SldB) SetDlgItemInt(IDC_E_B, m_SldB.GetPos());
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgColor::UpdateColor()
{
	int r = GetDlgItemInt(IDC_E_R);
	int g = GetDlgItemInt(IDC_E_G);
	int b = GetDlgItemInt(IDC_E_B);

	m_crColor = r | (g<<8) | (b<<16);
	CRect rc;
	m_StaticColor.GetClientRect(&rc);
	CDC* pDC = m_StaticColor.GetDC();

	pDC->FillSolidRect(&rc, m_crColor);

	m_StaticColor.ReleaseDC(pDC);
}

BOOL CDlgColor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_SldR.SetRange(0, 255);
	m_SldG.SetRange(0, 255);
	m_SldB.SetRange(0, 255);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
