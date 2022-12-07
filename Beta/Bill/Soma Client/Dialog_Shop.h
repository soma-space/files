// Dialog_Shop.h: interface for the CDialog_Shop class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_SHOP_H__66BD2D01_6924_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_SHOP_H__66BD2D01_6924_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"
#include "DarkDef.h"
//class BBDX_TextOuter;
class BBDX_MouseInformation;
//class CTWFont;

class CDialog_Shop : public CUIRes  
{
public:
	CDialog_Shop();
	virtual ~CDialog_Shop();

public:
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);

	void InitData();
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);
	void CallBack_MessageBox(int nId, int nValue);

	void SetScrollData();

	void ShopSendSell(int how_many);
	void ShopSendBuy();

	void ShowWindow();
	void HideWindow();
	void HideAllWindow();
	void RefreshData();
	void ShopOpen(PBYTE bypData);
	void DeleteContents();

	void DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index);
	void ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index);

	CUIScrollBar *m_pHandScrollBar, *m_pShopScrollBar;
//	BBDX_TextOuter *m_pMoneyOuter, *m_pCostOuter;
	short m_vShopMode;
	CItemMeArray m_arrItemMe;
	CItemShopArray m_arrItemShop;
	short m_vShopNumber;
	int m_vCurMeItem, m_vCurShopItem;
//	CTWFont *m_pTWFont;
	CFont			*m_pFont;
	int				m_vTipIndex;
	DWORD			m_vMarketPrice;
	BOOL			m_bProcessing;
};

#endif // !defined(AFX_DIALOG_SHOP_H__66BD2D01_6924_11D4_98B0_00C02615866B__INCLUDED_)
