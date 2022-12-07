// HouseNumberDialog.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "HouseNumberDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHouseNumberDialog dialog


CHouseNumberDialog::CHouseNumberDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CHouseNumberDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHouseNumberDialog)
	m_vHouseNumber = -1;
	//}}AFX_DATA_INIT
}


void CHouseNumberDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHouseNumberDialog)
	DDX_Text(pDX, IDC_HOUSE_NUMBER, m_vHouseNumber);
	//}}AFX_DATA_MAP
}

void CHouseNumberDialog::SetCaption(LPCTSTR caption)
{
	m_vCaption = caption;
}

BEGIN_MESSAGE_MAP(CHouseNumberDialog, CDialog)
	//{{AFX_MSG_MAP(CHouseNumberDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHouseNumberDialog message handlers

BOOL CHouseNumberDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_vCaption);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
