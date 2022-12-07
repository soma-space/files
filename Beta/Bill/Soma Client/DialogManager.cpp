// DialogManager.cpp: implementation of the CDialogManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "DialogManager.h"
#include "DManage/BB_Direct.h"
#include "ChildView.h"
#include "UIRes.h"
#include "Dialog_BoardList.h"
#include "Dialog_BoardRead.h"
#include "Dialog_BoardWrite.h"
#include "Dialog_BlackMagic.h"
#include "Dialog_BlueMagic.h"
#include "Dialog_WhiteMagic.h"
#include "Dialog_SpecialMagic.h"
#include "Dialog_Shop.h"
#include "Dialog_Status.h"
#include "Dialog_Inv.h"
#include "Dialog_Option.h"
#include "Dialog_Storage.h"
#include "Dialog_MakeItem.h"
#include "Dialog_Exchange.h"
#include "Dialog_Message.h"
#include "Dialog_Message2.h"
#include "Dialog_Message3.h"
#include "Dialog_HowMuch.h"
#include "Dialog_AuctionSell.h"
#include "Dialog_AuctionBuy.h"
#include "Dialog_AuctionDetail.h"
#include "Dialog_Name.h"
#include "Dialog_Chat.h"
#include "Dialog_Crest.h"
#include "Dialog_GuildPositionName.h"
#include "Dialog_TaxRate.h"
#include "Dialog_Quit.h"
#include "Dialog_HowMany.h"
#include "Dialog_GuildMenu1.h"
#include "Dialog_GuildName.h"
#include "Dialog_GuildApply.h"
#include "Dialog_GuildMenu2.h"
#include "Dialog_Guild.h"
#include "Dialog_GuildMenu5.h"
#include "Dialog_GuildJoin.h"

#include "Dialog_EventMessage.h"
#include "Dialog_EventMessage1.h"
#include "Dialog_EventMessage2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialogManager::CDialogManager()
{
	m_pDialog_BoardList = new CDialog_BoardList;
	m_pDialog_BoardRead = new CDialog_BoardRead;
	m_pDialog_BoardWrite = new CDialog_BoardWrite;
//	m_pDialog_BlackMagic = new CDialog_BlackMagic;
//	m_pDialog_BlueMagic = new CDialog_BlueMagic;
//	m_pDialog_WhiteMagic = new CDialog_WhiteMagic;
//	m_pDialog_SpecialMagic = new CDialog_SpecialMagic;
	m_pDialog_Shop = new CDialog_Shop;
	m_pDialog_Status = new CDialog_Status;
	m_pDialog_Inv = new CDialog_Inv;
	m_pDialog_Option = new CDialog_Option;
	m_pDialog_Storage = new CDialog_Storage;
	m_pDialog_MakeItem = new CDialog_MakeItem;
	m_pDialog_Exchange = new CDialog_Exchange;
	m_pDialog_Message = new CDialog_Message;
	m_pDialog_Message2 = new CDialog_Message2;
	m_pDialog_Message3 = new CDialog_Message3;
	m_pDialog_HowMuch = new CDialog_HowMuch;
	m_pDialog_HowMany = new CDialog_HowMany;
	m_pDialog_Name = new CDialog_Name;
	m_pDialog_AuctionSell = new CDialog_AuctionSell;
	m_pDialog_AuctionBuy = new CDialog_AuctionBuy;
	m_pDialog_AuctionDetail = new CDialog_AuctionDetail;
	m_pDialog_Chat = new CDialog_Chat;
	m_pDialog_Crest = new CDialog_Crest;
	m_pDialog_GuildPositionName = new CDialog_GuildPositionName;
	m_pDialog_TaxRate = new CDialog_TaxRate;
	m_pDialog_Quit = new CDialog_Quit;
	m_pDialog_GuildMenu1 = new CDialog_GuildMenu1;
	m_pDialog_GuildName = new CDialog_GuildName;
	m_pDialog_GuildApply = new CDialog_GuildApply;
	m_pDialog_GuildMenu2 = new CDialog_GuildMenu2;
	m_pDialog_Guild = new CDialog_Guild;
	m_pDialog_GuildMenu5 = new CDialog_GuildMenu5;
	m_pDialog_GuildJoin = new CDialog_GuildJoin;
	m_pDialog_EventMessage = new CDialog_EventMessage;
	m_pDialog_EventMessage1 = new CDialog_EventMessage1;
	m_pDialog_EventMessage2 = new CDialog_EventMessage2;
}

CDialogManager::~CDialogManager()
{
	delete m_pDialog_BoardList;
	delete m_pDialog_BoardRead;
	delete m_pDialog_BoardWrite;
	delete m_pDialog_BlackMagic;
	delete m_pDialog_BlueMagic;
	delete m_pDialog_WhiteMagic;
	delete m_pDialog_SpecialMagic;
	delete m_pDialog_Shop;
	delete m_pDialog_Status;
	delete m_pDialog_Inv;
	delete m_pDialog_Option;
	delete m_pDialog_Storage;
	delete m_pDialog_MakeItem;
	delete m_pDialog_Exchange;
	delete m_pDialog_Message;
	delete m_pDialog_Message2;
	delete m_pDialog_Message3;
	delete m_pDialog_HowMuch;
	delete m_pDialog_HowMany;
	delete m_pDialog_Name;
	delete m_pDialog_AuctionSell;
	delete m_pDialog_AuctionBuy;
	delete m_pDialog_AuctionDetail;
	delete m_pDialog_Chat;
	delete m_pDialog_Crest;
	delete m_pDialog_GuildPositionName;
	delete m_pDialog_TaxRate;
	delete m_pDialog_Quit;
	delete m_pDialog_GuildMenu1;
	delete m_pDialog_GuildName;
	delete m_pDialog_GuildApply;
	delete m_pDialog_GuildMenu2;
	delete m_pDialog_Guild;
	delete m_pDialog_GuildMenu5;
	delete m_pDialog_GuildJoin;
	delete m_pDialog_EventMessage;
	delete m_pDialog_EventMessage1;
	delete m_pDialog_EventMessage2;

	m_arrDialog.RemoveAll();
}

BOOL CDialogManager::InitData(CChildView *view)
{
	m_pMainWnd = view;

	return TRUE;
}

void CDialogManager::SetFocus(CUIRes *uiRes)
{
	int i, count;
	CUIRes *t_res;

	count = m_arrDialog.GetSize();

	for(i = 0; i < count-1; i++)
	{
		if(m_arrDialog[i] == uiRes)
		{
			t_res = m_arrDialog[i];
			m_arrDialog.RemoveAt(i);
			m_arrDialog.Add(t_res);
		}
	}
}

void CDialogManager::AddDialog(CUIRes *uiRes)
{
	m_arrDialog.Add(uiRes);
}

void CDialogManager::DelDialog()
{
	int count = m_arrDialog.GetSize();

	if(count)
	{
		m_arrDialog.RemoveAt(count-1);
		if(count == 1) m_pMainWnd->SetFocus();
		else m_arrDialog[count-1]->SetFocus();
	}
}

void CDialogManager::Draw(CDDSurface *pDest)
{
	int i, count;

	count = m_arrDialog.GetSize();
	for(i = 0; i < count; i++)
	{
		m_arrDialog[i]->Draw(pDest);
	}
}

BOOL CDialogManager::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	int i, count;

	if(m_pCaptureDialog) if(m_pCaptureDialog->MouseProcess(pMouseInfo)) return TRUE;
	count = m_arrDialog.GetSize();
	for(i = 0; i < count; i++)
	{
		if(m_arrDialog[i]->MouseProcess(pMouseInfo)) return TRUE;
	}
	return FALSE;
}