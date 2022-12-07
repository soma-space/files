// DlgBMPConvert.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgBMPConvert.h"
#include "..\\IGObject\\Dib16.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBMPConvert dialog


CDlgBMPConvert::CDlgBMPConvert(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBMPConvert::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBMPConvert)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgBMPConvert::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBMPConvert)
	DDX_Control(pDX, IDC_LIST_FILES, m_ListFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBMPConvert, CDialog)
	//{{AFX_MSG_MAP(CDlgBMPConvert)
	ON_BN_CLICKED(IDC_B_ADD_FILE, OnBAddFile)
	ON_BN_CLICKED(IDC_B_REMOVE_FILE, OnBRemoveFile)
	ON_BN_CLICKED(IDC_B_BMP_TO_OBM, OnBBmpToObm)
	ON_BN_CLICKED(IDC_B_OBM_TO_BMP, OnBObmToBmp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBMPConvert message handlers

void CDlgBMPConvert::OnBAddFile() 
{
	char szBuff[10240]; MAKE_ZERO(szBuff);
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT;
	CFileDialog dlg(TRUE, "bmp", NULL, dwFlags, "비트맵 파일(*.bmp;*.obm)|*.bmp;*obm||", NULL);
	dlg.m_ofn.lpstrFile = szBuff;
	dlg.m_ofn.nMaxFile = 10240;
	dlg.m_ofn.lpstrTitle = "비트맵 파일 리스트에 추가";
	if(dlg.DoModal() == IDCANCEL) OnCancel();

	POSITION pos = dlg.GetStartPosition();
	CStringArray FileNames;
	while(pos != NULL)
	{
		this->m_ListFiles.AddString(dlg.GetNextPathName(pos));
	}
}

void CDlgBMPConvert::OnBRemoveFile() 
{
	int indices[512]; MAKE_ZERO(indices);
	int nCount = m_ListFiles.GetSelItems(512, indices);

	for(int i = nCount - 1; i >= 0; i--)
	{
		m_ListFiles.DeleteString(indices[i]);
	}
}

void CDlgBMPConvert::OnBBmpToObm() 
{
	CDib16 dib;

	CString FileName, FileExt, FileName2;
	int indices[512]; MAKE_ZERO(indices);
	int nCount = m_ListFiles.GetSelItems(512, indices);

	for(int i = 0; i < nCount; i++)
	{
		m_ListFiles.GetText(indices[i], FileName);
		FileExt = FileName.Right(3);
		FileExt.MakeLower();
		if(FileExt[0] == 'b' && FileExt[1] == 'm' && FileExt[2] == 'p')
		{
			dib.ImportGenericBMP(FileName);

			FileName2 = FileName.Left(FileName.GetLength() - 3) + "obm";
			dib.Save(FileName2);
		}
	}
}

void CDlgBMPConvert::OnBObmToBmp() 
{
	CDib16 dib;

	CString FileName, FileExt, FileName2;
	int indices[512]; MAKE_ZERO(indices);
	int nCount = m_ListFiles.GetSelItems(512, indices);

	for(int i = 0; i < nCount; i++)
	{
		m_ListFiles.GetText(indices[i], FileName);
		FileExt = FileName.Right(3);
		FileExt.MakeLower();
		if(FileExt[0] == 'o' && FileExt[1] == 'b' && FileExt[2] == 'm')
		{
			dib.ImportGenericBMP(FileName);

			FileName2 = FileName.Left(FileName.GetLength() - 3) + "bmp";
			dib.ExportTo24BMP(FileName2);
		}
	}
}
