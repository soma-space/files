// DlgItem.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "DlgItem.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgItem dialog

CDlgItem::CDlgItem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgItem::IDD, pParent)
{
	m_Item.Alloc(MAX_ITEM);
	m_Item.m_Header.nQt = 0;
	m_nCurSel = 0;

	//{{AFX_DATA_INIT(CDlgItem)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgItem::~CDlgItem()
{
	m_Item.Release();
}

void CDlgItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgItem)
	DDX_Control(pDX, IDC_S_PREVIEW, m_SPreview);
	DDX_Control(pDX, IDC_LB_ANI, m_LBAni);
	DDX_Control(pDX, IDC_LB_ITEM, m_LBItem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgItem, CDialog)
	//{{AFX_MSG_MAP(CDlgItem)
	ON_BN_CLICKED(IDC_B_BROWSE_SPR, OnBBrowseSpr)
	ON_BN_CLICKED(IDC_B_BROWSE_ANI, OnBBrowseAni)
	ON_BN_CLICKED(IDC_B_NEW, OnBNew)
	ON_BN_CLICKED(IDC_B_LOAD, OnBLoad)
	ON_BN_CLICKED(IDC_B_SAVE, OnBSave)
	ON_BN_CLICKED(IDC_B_SAVE_AS, OnBSaveAs)
	ON_LBN_SELCHANGE(IDC_LB_ITEM, OnSelchangeLbItem)
	ON_BN_CLICKED(IDC_B_QT_SET, OnBQtSet)
	ON_BN_CLICKED(IDC_S_PREVIEW, OnSPreview)
	ON_LBN_SELCHANGE(IDC_LB_ANI, OnSelchangeLbAni)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgItem message handlers

void CDlgItem::OnBBrowseSpr() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "spl", NULL, dwFlags, "단순 LSP 스프라이트(*.spl)|*.spl||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("SPR");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_Item.SprImport(FileName, g_pTEng->PixelFormatGet());
	
	SetDlgItemText(IDC_E_SPR_FILE_NAME, FileName);
	
	this->UpdatePreview();
}

void CDlgItem::OnBBrowseAni() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "ani", NULL, dwFlags, "에니메이션 데이터(*.ani)|*.ani||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("ANI");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_Item.AniImport(FileName);
	
	this->UpdateAniListBox();
	SetDlgItemText(IDC_E_ANI_FILE_NAME, FileName);

	this->UpdatePreview();
}

void CDlgItem::OnBNew() 
{
	SetDlgItemText(IDC_E_SPR_FILE_NAME, "");
	SetDlgItemText(IDC_E_ANI_FILE_NAME, "");
	SetDlgItemText(IDC_E_FILE_NAME, "");
	SetDlgItemInt(IDC_E_QT, 0);

	m_Item.Release();
	m_Item.Alloc(MAX_ITEM);
	m_Item.m_Header.nQt = 0;
	
	m_nCurSel = 0;
	this->UpdateItemListBox();
	SetDlgItemInt(IDC_E_QT, m_Item.m_Header.nQt);
	this->UpdateAniListBox();
	this->UpdatePreview();
}

void CDlgItem::OnBLoad() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "itm", NULL, dwFlags, "아이템 데이터(*.itm)|*.itm||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	m_Item.Load(FileName, g_pTEng->PathGetCur("ANI", 0), g_pTEng->PathGetCur("SPR", 1), g_pTEng->PixelFormatGet());
	
	SetDlgItemText(IDC_E_FILE_NAME, FileName);
	SetDlgItemText(IDC_E_ANI_FILE_NAME, m_Item.m_Header.szAniFN);
	SetDlgItemText(IDC_E_SPR_FILE_NAME, m_Item.m_Header.szSprFN);
	
	m_nCurSel = 0;
	this->UpdateAniListBox();
	SetDlgItemInt(IDC_E_QT, m_Item.m_Header.nQt);
	this->UpdateItemListBox();
	this->UpdateItemInfo();
}

void CDlgItem::OnBSave() 
{
	CString FileName;
	GetDlgItemText(IDC_E_FILE_NAME, FileName);

	if(FileName.GetLength() <= 0)
	{
		this->OnBSaveAs();
		return;
	}

	g_pTEng->PathSetCur("DATA");
	m_Item.Save(FileName);
}

void CDlgItem::OnBSaveAs() 
{
	CString FileName;
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	CFileDialog dlg(TRUE, "itm", NULL, dwFlags, "아이템 데이터(*.itm)|*.itm||", NULL);
	dlg.m_ofn.lpstrInitialDir = g_pTEng->PathGetCur("DATA");
	if(dlg.DoModal() == IDCANCEL) return;

	FileName = dlg.GetFileName();

	SetDlgItemText(IDC_E_FILE_NAME, FileName);

	this->OnBSave();
}

void CDlgItem::OnSelchangeLbItem() 
{
	this->UpdateItemData();
	m_nCurSel = m_LBItem.GetCurSel();
	this->UpdateItemInfo();
	this->UpdatePreview();
}

void CDlgItem::UpdateItemInfo()
{
	_ITEM_DATA* pItem = m_Item.DataGet(m_nCurSel);
	if(pItem == NULL) return;

	m_LBAni.SetCurSel(pItem->nAni);
	SetDlgItemInt(IDC_E_WIDTH, pItem->snWidth);
	SetDlgItemInt(IDC_E_HEIGHT, pItem->snHeight);

	SetDlgItemText(IDC_E_NAME, pItem->szName);
	SetDlgItemText(IDC_E_REMARK, pItem->szRemark);
	SetDlgItemText(IDC_E_REMARK_NPC, pItem->szRemarkNPC);
	SetDlgItemInt(IDC_E_STRENGTH, pItem->snStrength);
	SetDlgItemInt(IDC_E_SPIRIT, pItem->snSpirit);
	SetDlgItemInt(IDC_E_CHARM, pItem->snCharm);
	SetDlgItemInt(IDC_E_IQ, pItem->snIQ);
	SetDlgItemInt(IDC_E_EQ, pItem->snEQ);
	SetDlgItemInt(IDC_E_LUCK, pItem->snLuck);
	SetDlgItemInt(IDC_E_EXTRA0, pItem->snExtra0);
	SetDlgItemInt(IDC_E_EXTRA1, pItem->snExtra1);
	SetDlgItemInt(IDC_E_EXTRA2, pItem->snExtra2);
	SetDlgItemInt(IDC_E_EXTRA3, pItem->snExtra3);
	SetDlgItemInt(IDC_E_SPECIAL0, pItem->snSpecial0);
	SetDlgItemInt(IDC_E_SPECIAL1, pItem->snSpecial1);
	SetDlgItemInt(IDC_E_SPECIAL2, pItem->snSpecial2);
	SetDlgItemInt(IDC_E_SPECIAL3, pItem->snSpecial3);
	SetDlgItemInt(IDC_E_DURABILITY, pItem->snDurability);
	
	SetDlgItemInt(IDC_E_STRENGTH_NEED, pItem->snStrengthNeed);
	SetDlgItemInt(IDC_E_SPIRIT_NEED, pItem->snSpiritNeed);
	SetDlgItemInt(IDC_E_CHARM_NEED, pItem->snCharmNeed);
	SetDlgItemInt(IDC_E_IQ_NEED, pItem->snIQNeed);
	SetDlgItemInt(IDC_E_EQ_NEED, pItem->snEQNeed);
	SetDlgItemInt(IDC_E_LUCK_NEED, pItem->snLuckNeed);
	SetDlgItemInt(IDC_E_EXTRA0_NEED, pItem->snExtra0Need);
	SetDlgItemInt(IDC_E_EXTRA1_NEED, pItem->snExtra1Need);
	SetDlgItemInt(IDC_E_EXTRA2_NEED, pItem->snExtra2Need);
	SetDlgItemInt(IDC_E_EXTRA3_NEED, pItem->snExtra3Need);
}

void CDlgItem::UpdateItemData()
{
	_ITEM_DATA* pItem = m_Item.DataGet(m_nCurSel);
	if(pItem == NULL) return;

	pItem->nAni = m_LBAni.GetCurSel();
	pItem->snWidth = GetDlgItemInt(IDC_E_WIDTH);
	pItem->snHeight = GetDlgItemInt(IDC_E_HEIGHT);

	GetDlgItemText(IDC_E_NAME, pItem->szName, 32);
//	GetDlgItemText(IDC_E_REMARK, pItem->szRemark);
//	GetDlgItemText(IDC_E_REMARK_NPC, pItem->szRemarkNPC);
	pItem->snStrength = GetDlgItemInt(IDC_E_STRENGTH);
	pItem->snSpirit = GetDlgItemInt(IDC_E_SPIRIT);
	pItem->snCharm = GetDlgItemInt(IDC_E_CHARM);
	pItem->snIQ = GetDlgItemInt(IDC_E_IQ);
	pItem->snEQ = GetDlgItemInt(IDC_E_EQ);
	pItem->snLuck = GetDlgItemInt(IDC_E_LUCK);
	pItem->snExtra0 = GetDlgItemInt(IDC_E_EXTRA0);
	pItem->snExtra1 = GetDlgItemInt(IDC_E_EXTRA1);
	pItem->snExtra2 = GetDlgItemInt(IDC_E_EXTRA2);
	pItem->snExtra3 = GetDlgItemInt(IDC_E_EXTRA3);
	pItem->snSpecial0 = GetDlgItemInt(IDC_E_SPECIAL0);
	pItem->snSpecial1 = GetDlgItemInt(IDC_E_SPECIAL1);
	pItem->snSpecial2 = GetDlgItemInt(IDC_E_SPECIAL2);
	pItem->snSpecial3 = GetDlgItemInt(IDC_E_SPECIAL3);
	pItem->snDurability = GetDlgItemInt(IDC_E_DURABILITY);
	
	pItem->snStrengthNeed = GetDlgItemInt(IDC_E_STRENGTH_NEED);
	pItem->snSpiritNeed = GetDlgItemInt(IDC_E_SPIRIT_NEED);
	pItem->snCharmNeed = GetDlgItemInt(IDC_E_CHARM_NEED);
	pItem->snIQNeed = GetDlgItemInt(IDC_E_IQ_NEED);
	pItem->snEQNeed = GetDlgItemInt(IDC_E_EQ_NEED);
	pItem->snLuckNeed = GetDlgItemInt(IDC_E_LUCK_NEED);
	pItem->snExtra0Need = GetDlgItemInt(IDC_E_EXTRA0_NEED);
	pItem->snExtra1Need = GetDlgItemInt(IDC_E_EXTRA1_NEED);
	pItem->snExtra2Need = GetDlgItemInt(IDC_E_EXTRA2_NEED);
	pItem->snExtra3Need = GetDlgItemInt(IDC_E_EXTRA3_NEED);
}

void CDlgItem::OnBQtSet() 
{
	m_Item.m_Header.nQt = GetDlgItemInt(IDC_E_QT);
	this->UpdateItemListBox();
}

void CDlgItem::UpdatePreview()
{
	CRect rc;
	m_SPreview.GetClientRect(&rc);

	g_pTEng->SurfaceFillRect(NULL, &rc, RGB(64,64,64));

	int nItem = m_LBItem.GetCurSel();
	_ITEM_DATA* pItem = m_Item.DataGet(nItem);
	if(pItem != NULL)
	{
		CRect rcBox(0, 0, pItem->snWidth*32, pItem->snHeight*32);
		g_pTEng->DrawBox(NULL, &rcBox, RGB(0,255,0));
		_ANI_DATA* pAni = m_Item.m_pAniObj->AniDataGet(pItem->nAni);
		if(pAni != NULL)
		{
			if(pAni->pnFrms != NULL)
			{
				const _SPR_LSP* pSpr = NULL;
				m_Item.m_pAniObj->SprGet(pAni->pnFrms[0], &pSpr);
				if(pSpr != NULL)
				{
					g_pTEng->BackBufferLock();
					g_pTEng->BltLSP(NULL, pItem->snWidth*32/2,  pItem->snHeight*32/2, pSpr);
					g_pTEng->BackBufferUnlock();
				}
			}
		}
	}
	
	g_pTEng->HWndSet(m_SPreview.m_hWnd);
	g_pTEng->Flip();
}

void CDlgItem::UpdateAniListBox()
{
	m_LBAni.ResetContent();
	int nQt = m_Item.m_pAniObj->m_HAni.nQt;
	CString str;
	for(int i = 0; i < nQt; i++)
	{
		_ANI_DATA* pAni = m_Item.m_pAniObj->AniDataGet(i);
		if(pAni == NULL)
		{
			str = "에러! 에니메이션 없음";
		}
		else
		{
			str.Format("%3d : ", i);
			if(lstrlen(pAni->szName) <= 0) str += "이름 없음";
			else str += pAni->szName;
		}
		m_LBAni.AddString(str);
	}
	m_LBAni.SetCurSel(0);
}

void CDlgItem::UpdateItemListBox()
{
	m_LBItem.ResetContent();
	int nQt = m_Item.m_Header.nQt;
	CString str;
	for(int i = 0; i < nQt; i++)
	{
		_ITEM_DATA* pItem = m_Item.DataGet(i);
		if(pItem == NULL)
		{
			str = "에러! 에니메이션 없음";
		}
		else
		{
			str.Format("%3d : ", i);
			if(lstrlen(pItem->szName) <= 0) str += "이름 없음";
			else str += pItem->szName;
		}
		m_LBItem.AddString(str);
	}
	m_LBItem.SetCurSel(0);
}

void CDlgItem::OnSPreview() 
{
	int nItem = m_LBItem.GetCurSel();
	_ITEM_DATA* pItem = m_Item.DataGet(nItem);
	if(pItem != NULL)
	{
		CPoint pt;
		GetCursorPos(&pt);
		m_SPreview.ScreenToClient(&pt);

		pt.x /= 32; pt.x += 1;
		pt.y /= 32; pt.y += 1;

		pItem->snWidth = (short int)pt.x;
		pItem->snHeight = (short int)pt.y;
	}
	
	this->UpdateItemInfo();
	this->UpdatePreview();
}

void CDlgItem::OnSelchangeLbAni() 
{
	this->UpdateItemData();
	this->UpdatePreview();
}

void CDlgItem::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	this->UpdatePreview();
	
	// Do not call CDialog::OnPaint() for painting messages
}
