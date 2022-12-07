// Dialog_AuctionDetail.h: interface for the CDialog_AuctionDetail class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_AUCTIONDETAIL_H__9FCA6741_78DB_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_AUCTIONDETAIL_H__9FCA6741_78DB_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "DarkDef.h"
class BBDX_MouseInformation;
//class BBDX_TextOuter;
//class CTWFont;

class CDialog_AuctionDetail : public CUIRes  
{
public:
	CDialog_AuctionDetail();
	virtual ~CDialog_AuctionDetail();

public:
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void ShowWindow(int Id, CUIRes *pParent, ITEM_AUCTION *item_auction);
	void HideWindow();
	void HideAllWindow();

//	BBDX_TextOuter		*m_pDetailOuter, *m_pUserOuter, *m_pCostOuter;
//	CTWFont				*m_pTWFont;
	CString				m_strDetail, m_strUser, m_strCost;
	CFont				*m_pFont;
	ITEM_AUCTION		*m_pAuctionItem;
	CString				m_vRegistrant;
	int					m_vAuctionCount;
	int					m_vId;
	DWORD				m_vValue;
};

#endif // !defined(AFX_DIALOG_AUCTIONDETAIL_H__9FCA6741_78DB_11D4_98B0_00C02615866B__INCLUDED_)
