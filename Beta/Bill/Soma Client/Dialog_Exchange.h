// Dialog_Exchange.h: interface for the CDialog_Exchange class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_EXCHANGE_H__D6602F24_774A_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_EXCHANGE_H__D6602F24_774A_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "DarkDef.h"
class CUIScrollBar;
class BBDX_MouseInformation;
//class BBDX_TextOuter;
//class CTWFont;

class CDialog_Exchange : public CUIRes  
{
public:
	CDialog_Exchange();
	virtual ~CDialog_Exchange();

	void RcvTradeOk();
	void RcvTradeCancel();
	void RcvTradeResult(PBYTE bypData);
	void RcvTradeMoney(PBYTE bypData);
	void RcvTradeItem(PBYTE bypData);
	void RcvAckTrade(PBYTE bypData);
	void RcvReqTrade(PBYTE bypData);
	void CallBack_MessageBox(int nId, int nValue);
	void SendTradeOther();
	void SendTradeCancel();
	void SendTradeAck(BYTE result);
	void SendTradeAddItem(short item_number, short count = 1);
	void SendTradeSetMoney(DWORD money);
	void SendTradeOk();
public:
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);
	void DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index);
	void ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index = 37);
	CString MakeCaption(ITEM_DETAIL *item_detail);

	void SetScrollMeData();
	void SetScrollOtherData();

	void AskHowMuch();
	void AskTrade(CString other_user, int UId);
	void WaitTrade();
	void ShowWindow();
	void HideWindow();
	void HideAllWindow();
	void RefreshData();
	void DeleteContents();
	void InitData();

	CUIScrollBar	*m_pMeExgScrollBar, *m_pOtherExgScrollBar, *m_pMeScrollBar;
//	BBDX_TextOuter	*m_pMeExgMoney, *m_pOtherExgMoney, *m_pMeMoney;
//	CTWFont			*m_pTWFont;
	CFont			*m_pFont;
	CString			m_strMeExgMoney, m_strOtherExgMoney;
	CItemDetailArray	m_arrItemMe;
	CItemDetailArray	m_arrItemMeExg;
	CItemDetailArray	m_arrItemOtherExg;
	int				m_vCurMeItem;
	int				m_vDragItem;
	CPoint			m_vDragOffset;
	CString			m_vOtherUser;
	int				m_vUId;
	BOOL			m_bTrading;
	BOOL			m_bOtherOk;
	int				m_vSelSpec;
	int				m_vTipIndex;
	CString			m_strCaption;
};

#endif // !defined(AFX_DIALOG_EXCHANGE_H__D6602F24_774A_11D4_98B0_00C02615866B__INCLUDED_)
