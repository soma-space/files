// DlgFolderBrowse.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgFolderBrowse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFolderBrowse dialog


CDlgFolderBrowse::CDlgFolderBrowse(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFolderBrowse::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFolderBrowse)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgFolderBrowse::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFolderBrowse)
	DDX_Control(pDX, IDC_TREE, m_TreeFolder);
	DDX_Control(pDX, IDC_CB_RECENT_FOLDER, m_CBRecentFolder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFolderBrowse, CDialog)
	//{{AFX_MSG_MAP(CDlgFolderBrowse)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFolderBrowse message handlers
