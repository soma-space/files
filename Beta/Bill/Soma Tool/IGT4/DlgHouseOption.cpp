// DlgHouseOption.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "GlobalVar.h"
#include "DlgHouseOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgHouseOption dialog


CDlgHouseOption::CDlgHouseOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHouseOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgHouseOption)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_vHouse = 0;
	m_vHouseOption = 0;
	m_vEditMode = 0;
	m_bEnable = FALSE;
	m_vTS = m_vTSS = -1;
}


void CDlgHouseOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHouseOption)
	DDX_Control(pDX, IDC_CHECK_HOUSE, m_wndCheckHouse);
	DDX_Control(pDX, IDC_CHECK_HOUSEOPTION, m_wndCheckHouseOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgHouseOption, CDialog)
	//{{AFX_MSG_MAP(CDlgHouseOption)
	ON_BN_CLICKED(IDC_CHECK_HOUSEOPTION, OnCheckHouseoption)
	ON_BN_CLICKED(IDC_RADIO_NOMOVE, OnRadioNomove)
	ON_BN_CLICKED(IDC_RADIO_MOVABLE, OnRadioMovable)
	ON_BN_CLICKED(IDC_RADIO_NOTMOVABLE, OnRadioNotmovable)
	ON_BN_CLICKED(IDC_RADIO_NOHOUSE, OnRadioNohouse)
	ON_BN_CLICKED(IDC_RADIO_HOUSE, OnRadioHouse)
	ON_BN_CLICKED(IDC_CHECK_HOUSE, OnCheckHouse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHouseOption message handlers

void CDlgHouseOption::OnCheckHouseoption() 
{
	m_vHouseOption = m_wndCheckHouseOption.GetCheck();
	GetParent()->Invalidate(FALSE);
}

BOOL CDlgHouseOption::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckRadioButton(IDC_RADIO_NOMOVE, IDC_RADIO_HOUSE, IDC_RADIO_NOMOVE);

	EnableHouse(FALSE);
	CheckHouse(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgHouseOption::OnRadioNomove() 
{
	m_vEditMode = 0;
	GetParent()->Invalidate(FALSE);
}

void CDlgHouseOption::OnRadioMovable() 
{
	m_vEditMode = 1;
	GetParent()->Invalidate(FALSE);
}

void CDlgHouseOption::OnRadioNotmovable() 
{
	m_vEditMode = 2;
	GetParent()->Invalidate(FALSE);
}

void CDlgHouseOption::OnRadioNohouse() 
{
	m_vEditMode = 3;
	GetParent()->Invalidate(FALSE);
}

void CDlgHouseOption::OnRadioHouse() 
{
	m_vEditMode = 4;
	GetParent()->Invalidate(FALSE);
}

void CDlgHouseOption::OnCheckHouse() 
{
	m_vHouse = m_wndCheckHouse.GetCheck();
	_TILE_SUB_SET_DATA_TOOL* pData = g_pTSet->DataGetSafe(m_vTS, m_vTSS);
	if(m_vHouse)
	{
		CheckHouse(TRUE);

		int t_x, t_y;
		t_x = pData->nWidth * 32;
		t_y = pData->nHeight * 32;
		g_pHouseInfo->SetHouse(m_vTS, t_x/96*2+1 + ((t_x%96>=48)?1:0), t_y/48*2+1 + ((t_y%48>=24)?1:0));
	}
	else
	{
		CheckHouse(FALSE);

		g_pHouseInfo->DeleteHouse(m_vTS);
	}
	GetParent()->Invalidate(FALSE);
}

void CDlgHouseOption::SetCheckHouse(int house)
{
	if(m_vHouse == house) return;

	m_vHouse = house;
	if(m_vHouse)
	{
		m_wndCheckHouse.SetCheck(1);
		CheckHouse(TRUE);
	}
	else
	{
		m_wndCheckHouse.SetCheck(0);
		m_wndCheckHouseOption.SetCheck(0);
		CheckHouse(FALSE);
	}
}

void CDlgHouseOption::EnableHouse(BOOL bEnable)
{
	GetDlgItem(IDC_CHECK_HOUSE)->EnableWindow(bEnable);
}

void CDlgHouseOption::CheckHouse(BOOL bEnable)
{
	GetDlgItem(IDC_CHECK_HOUSEOPTION)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_NOMOVE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_MOVABLE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_NOTMOVABLE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_NOHOUSE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_HOUSE)->EnableWindow(bEnable);
}

void CDlgHouseOption::SetTileSet(int ts, int tss)
{
	m_vTS = ts;
	m_vTSS = tss;

	_TILE_SUB_SET_DATA_TOOL* pData = g_pTSet->DataGetSafe(ts, tss);

	if(pData = NULL)
	{
		EnableHouse(FALSE);
		CheckHouse(FALSE);
		return;
	}

	if(m_vTSS == 3)
	{
		EnableHouse(TRUE);
		CHouseInfo *pHouseInfo;
		if(g_pHouseInfo->m_MapHouseInfo.Lookup(m_vTS, pHouseInfo))
		{
			SetCheckHouse(1);
		}
		else
		{
			SetCheckHouse(0);
		}
	}
	else
	{
		EnableHouse(FALSE);
		SetCheckHouse(0);
	}
}