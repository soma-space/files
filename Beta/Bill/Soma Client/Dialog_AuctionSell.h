// Dialog_AuctionSell.h: interface for the CDialog_AuctionSell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_AUCTIONSELL_H__FE821CE2_74F0_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_AUCTIONSELL_H__FE821CE2_74F0_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "DarkDef.h"
class CUIScrollBar;
class BBDX_MouseInformation;
//class BBDX_TextOuter;
//class CTWFont;

class CDialog_AuctionSell : public CUIRes  
{
public:
	CDialog_AuctionSell();
	virtual ~CDialog_AuctionSell();

	void AuctionOpen(PBYTE bypData);
	void AskAuctionMoney();
	void SendAuctionItem(DWORD money);
	void SendGetAuctionItem();
	void RcvAuctionItem(PBYTE bypData);
	void RcvGetAuctionItem(PBYTE bypData);
public:
	void CallBack_MessageBox(int nId, int nValue);
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void SetScrollData();
	void SetAuctionData(ITEM_AUCTION *auction_item);
	void RefreshData();
	void InitData();
	void DeleteContents();
	void ShowWindow();
	void HideWindow();
	void HideAllWindow();

	void DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index);
	void ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index);

	CUIScrollBar		*m_pScrollBar;
//	BBDX_TextOuter		*m_pCostOuter, *m_pMoneyOuter;
//	CTWFont				*m_pTWFont;
	CString				m_strCost;
	CItemDetailArray	m_arrItem;
	ITEM_AUCTION		*m_pAuctionItem;
	int					m_vCurItem;
	short				m_vAuctionNumber;
	int					m_vDelItem;
	CFont				*m_pFont;
	int					m_vTipIndex;
	int					m_vHowMany;
	BOOL				m_bItemProcess;
};

#endif // !defined(AFX_DIALOG_AUCTIONSELL_H__FE821CE2_74F0_11D4_98B0_00C02615866B__INCLUDED_)
