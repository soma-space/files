// DialogManager.h: interface for the CDialogManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOGMANAGER_H__E9FF3A81_7A74_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOGMANAGER_H__E9FF3A81_7A74_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class	BBDX_MouseInformation;
class	CChildView;
class	CUIRes;
class	CDDSurface;
class	CDialog_BoardList;
class	CDialog_BoardRead;
class	CDialog_BoardWrite;
class	CDialog_BlackMagic;
class	CDialog_BlueMagic;
class	CDialog_WhiteMagic;
class	CDialog_SpecialMagic;
class	CDialog_Shop;
class	CDialog_Status;
class	CDialog_Inv;
class	CDialog_Option;
class	CDialog_Storage;
class	CDialog_MakeItem;
class	CDialog_Exchange;
class	CDialog_Message;
class	CDialog_Message2;
class	CDialog_Message3;
class	CDialog_HowMuch;
class	CDialog_HowMany;
class	CDialog_Name;
class	CDialog_AuctionSell;
class	CDialog_AuctionBuy;
class	CDialog_AuctionDetail;
class	CDialog_Chat;
class	CDialog_Crest;
class	CDialog_GuildPositionName;
class	CDialog_TaxRate;
class	CDialog_Quit;
class	CDialog_GuildMenu1;
class	CDialog_GuildName;
class	CDialog_GuildApply;
class	CDialog_GuildMenu2;
class	CDialog_Guild;
class	CDialog_GuildMenu5;
class	CDialog_GuildJoin;
class	CDialog_EventMessage;
class	CDialog_EventMessage1;
class	CDialog_EventMessage2;

typedef CTypedPtrArray<CPtrArray, CUIRes *> CUIResArray;

class CDialogManager  
{
public:
	CDialogManager();
	virtual ~CDialogManager();

	BOOL InitData(CChildView *view);
	void SetFocus(CUIRes *uiRes);
	void AddDialog(CUIRes *uiRes);
	void DelDialog();
	void Draw(CDDSurface *pDest);
	BOOL MouseProcess(BBDX_MouseInformation *pMouseInfo);

public:
	CDialog_BoardList			*m_pDialog_BoardList;
	CDialog_BoardRead			*m_pDialog_BoardRead;
	CDialog_BoardWrite			*m_pDialog_BoardWrite;
	CDialog_BlackMagic			*m_pDialog_BlackMagic;
	CDialog_BlueMagic			*m_pDialog_BlueMagic;
	CDialog_WhiteMagic			*m_pDialog_WhiteMagic;
	CDialog_SpecialMagic		*m_pDialog_SpecialMagic;
	CDialog_Shop				*m_pDialog_Shop;
	CDialog_Status				*m_pDialog_Status;
	CDialog_Inv					*m_pDialog_Inv;
	CDialog_Option				*m_pDialog_Option;
	CDialog_Storage				*m_pDialog_Storage;
	CDialog_MakeItem			*m_pDialog_MakeItem;
	CDialog_Exchange			*m_pDialog_Exchange;
	CDialog_Message				*m_pDialog_Message;
	CDialog_Message2			*m_pDialog_Message2;
	CDialog_Message3			*m_pDialog_Message3;
	CDialog_HowMuch				*m_pDialog_HowMuch;
	CDialog_HowMany				*m_pDialog_HowMany;
	CDialog_Name				*m_pDialog_Name;
	CDialog_AuctionSell			*m_pDialog_AuctionSell;
	CDialog_AuctionBuy			*m_pDialog_AuctionBuy;
	CDialog_AuctionDetail		*m_pDialog_AuctionDetail;
	CDialog_Chat				*m_pDialog_Chat;
	CDialog_Crest				*m_pDialog_Crest;
	CDialog_GuildPositionName	*m_pDialog_GuildPositionName;
	CDialog_TaxRate				*m_pDialog_TaxRate;
	CDialog_Quit				*m_pDialog_Quit;
	CDialog_GuildMenu1			*m_pDialog_GuildMenu1;
	CDialog_GuildName			*m_pDialog_GuildName;
	CDialog_GuildApply			*m_pDialog_GuildApply;
	CDialog_GuildMenu2			*m_pDialog_GuildMenu2;
	CDialog_Guild				*m_pDialog_Guild;
	CDialog_GuildMenu5			*m_pDialog_GuildMenu5;
	CDialog_GuildJoin			*m_pDialog_GuildJoin;
	CDialog_EventMessage		*m_pDialog_EventMessage;
	CDialog_EventMessage1		*m_pDialog_EventMessage1;
	CDialog_EventMessage2		*m_pDialog_EventMessage2;
	
	CUIRes					*m_pCaptureDialog;
	CUIResArray				m_arrDialog;

	CChildView				*m_pMainWnd;
};

#endif // !defined(AFX_DIALOGMANAGER_H__E9FF3A81_7A74_11D4_98B0_00C02615866B__INCLUDED_)
