// Dialog_Exchange.cpp: implementation of the CDialog_Exchange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Exchange.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "UIScrollBar.h"
//#include "BBDX_TextOuter.h"
#include "Item_Total.h"
#include "PacketParser.h"
#include "User.h"
//#include "TWFont.h"
#include "ChildView.h"
#include "Dialog_Message.h"
#include "Dialog_Message2.h"
#include "Dialog_Message3.h"
#include "Dialog_HowMuch.h"
#include "Dialog_HowMany.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CItem_Total			Item_Total;
extern CDialog_Message		Dialog_Message;
extern CDialog_Message2		Dialog_Message2;
extern CDialog_Message3		Dialog_Message3;
extern CDialog_HowMuch		Dialog_HowMuch;
extern CDialog_HowMany		Dialog_HowMany;
extern CUIRes *g_pShowDialog;

static const char SPECIAL_NAME[][4] = {"Str", "Dex", "Int", "Atk", "Def", "HP", "MP"};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Exchange::CDialog_Exchange()
{
	INIT_PTR(m_pMeExgScrollBar);
	INIT_PTR(m_pOtherExgScrollBar);
	INIT_PTR(m_pMeScrollBar);
//	INIT_PTR(m_pTWFont);
//	INIT_PTR(m_pMeExgMoney);
//	INIT_PTR(m_pOtherExgMoney);
//	INIT_PTR(m_pMeMoney);
	m_pFont = NULL;
	m_vUId = -1;
	m_vPosMode = POS_LEFT;
	m_vSelSpec = -1;
	m_vTipIndex = -1;
}

CDialog_Exchange::~CDialog_Exchange()
{
	DeleteContents();
	SAFE_DELETE(m_pMeExgScrollBar);
	SAFE_DELETE(m_pOtherExgScrollBar);
	SAFE_DELETE(m_pMeScrollBar);
//	SAFE_DELETE(m_pTWFont);
//	SAFE_DELETE(m_pOuter);
//	SAFE_DELETE(m_pMeExgMoney);
//	SAFE_DELETE(m_pOtherExgMoney);
//	SAFE_DELETE(m_pMeMoney);
	if(m_pFont) delete m_pFont;
}

void CDialog_Exchange::ShowWindow()
{
	if(!IsLoading())
	{
		SetOffset(CPoint(0,0));
		if(!Create(CChildView::DirectXMng.m_pDD, "Exchange.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	CUIRes::ShowWindow();
	InitData();
	RefreshData();
	m_pMeExgScrollBar->InitScroll();
	m_pOtherExgScrollBar->InitScroll();
	m_pMeScrollBar->InitScroll();
}

void CDialog_Exchange::HideWindow()
{
	g_pShowDialog = NULL;
	m_vTipIndex = -1;
	CUIRes::HideWindow();
	DeleteContents();
	m_vUId = -1;
}

void CDialog_Exchange::HideAllWindow()
{
	SendTradeCancel();
	m_vTipIndex = -1;
	CUIRes::HideWindow();
	DeleteContents();
	m_vUId = -1;
}

void CDialog_Exchange::RefreshData()
{
	int i, size;
	ITEM_DETAIL *item_me;
	CUser *ptmp;

	// 초기화
	size = m_arrItemMe.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemMe[i];
	}
	m_arrItemMe.RemoveAll();

	ptmp = CChildView::UserManager.GetMe();
	for(i = 10; i < 50; i++)
	{
		if(ptmp->GetInv_ItemUsage(i) > 0)
		{
			item_me = new ITEM_DETAIL;

			item_me->m_vIndex = i;
			item_me->m_vPicNum = ptmp->GetInv_ItemPic(i);
			item_me->m_vArmPos = ptmp->GetInv_ItemArm(i);
			item_me->m_vGender = ptmp->m_MyInvData[i].m_vGender;
			item_me->m_vName = ptmp->GetInv_ItemName(i);
			item_me->m_vType = ptmp->GetInv_ItemType(i);
			item_me->m_vUsage = ptmp->GetInv_ItemUsage(i);
			item_me->m_vMinDamage = ptmp->m_MyInvData[i].m_vMinDamage;
			item_me->m_vMaxDamage = ptmp->m_MyInvData[i].m_vMaxDamage;
			item_me->m_vLimitStr = ptmp->m_MyInvData[i].m_vLimitStr;
			item_me->m_vLimitInt = ptmp->m_MyInvData[i].m_vLimitInt;
			item_me->m_vLimitSkill = ptmp->m_MyInvData[i].m_vLimitSkill;
			item_me->m_strMagic = ptmp->m_MyInvData[i].m_strMagic;
			item_me->m_strSNum = ptmp->m_MyInvData[i].m_strSNum;
			for(int k = 0; k < 7; k++) item_me->m_vSpecial[k] = ptmp->m_MyInvData[i].m_vSpecial[k];

			m_arrItemMe.Add(item_me);
		}
	}
	m_vCurMeItem = -1;

	SetScrollMeData();
}

void CDialog_Exchange::DeleteContents()
{
	int size, i;

	size = m_arrItemMe.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemMe[i];
	}
	m_arrItemMe.RemoveAll();

	size = m_arrItemMeExg.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemMeExg[i];
	}
	m_arrItemMeExg.RemoveAll();

	size = m_arrItemOtherExg.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemOtherExg[i];
	}
	m_arrItemOtherExg.RemoveAll();
}

void CDialog_Exchange::SetScrollMeData()
{
	int count;

	count = m_arrItemMeExg.GetSize();
	count = count / 2 + count%2;
	m_pMeExgScrollBar->SetMaxLineCount(count);
	m_pMeExgScrollBar->SetViewLineCount(3);
	m_pMeExgScrollBar->SetStartLine(m_pMeExgScrollBar->GetStartLine());

	count = m_arrItemMe.GetSize();
	count = count / 6 + (count%6?1:0);
	m_pMeScrollBar->SetMaxLineCount(count);
	m_pMeScrollBar->SetViewLineCount(2);
	m_pMeScrollBar->SetStartLine(m_pMeScrollBar->GetStartLine());
}

void CDialog_Exchange::SetScrollOtherData()
{
	int count;

	count = m_arrItemOtherExg.GetSize();
	count = count / 2 + count%2;
	m_pOtherExgScrollBar->SetMaxLineCount(count);
	m_pOtherExgScrollBar->SetViewLineCount(3);
	m_pOtherExgScrollBar->SetStartLine(m_pOtherExgScrollBar->GetStartLine());
}

BOOL CDialog_Exchange::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}
		
/*	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		delete m_pTWFont; m_pTWFont=NULL;
		TRACE("####### Dialog Shop Init 실패.. ############ \n");
		return FALSE;
	}
	m_pMeExgMoney = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pOtherExgMoney = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pMeMoney = new BBDX_TextOuter(m_pTWFont, 14, 14);*/
	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	m_pMeScrollBar = new CUIScrollBar();
	m_pMeScrollBar->Create((CUIRes*)this, m_arrArea[34]->m_vRect, 2);
	m_pMeExgScrollBar = new CUIScrollBar();
	m_pMeExgScrollBar->Create((CUIRes*)this, m_arrArea[29]->m_vRect, 0);
	m_pOtherExgScrollBar = new CUIScrollBar();
	m_pOtherExgScrollBar->Create((CUIRes*)this, m_arrArea[30]->m_vRect, 1);

	return TRUE;
}

BOOL CDialog_Exchange::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 8);

	CRect rtDst, rtSrc;
	CPoint t_pos;
	DWORD dwTrans;
	CUser *ptmp;
	CString t_str;
	int me_exg_end, me_exg_start, other_exg_end, other_exg_start, me_end, me_start, i, j;

	ptmp = CChildView::UserManager.GetMe();

	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	if(m_bOtherOk)
	{
		t_pos = m_arrArea[36]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = m_arrArea[35]->m_vRect;
		pDest->BltFast(t_pos, m_pDDSurface, &rtSrc, dwTrans);
	}
	if(ptmp->m_Gender == 0) // 여성이면
	{
		t_pos = m_arrArea[32]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = m_arrArea[31]->m_vRect;
		pDest->BltFast(t_pos, m_pDDSurface, &rtSrc, dwTrans);
	}

	m_pMeScrollBar->Draw(pDest);
	m_pMeExgScrollBar->Draw(pDest);
	m_pOtherExgScrollBar->Draw(pDest);

	me_exg_end = m_arrItemMeExg.GetSize();
	me_exg_start = m_pMeExgScrollBar->GetStartLine() * 2;
	if(me_exg_end > me_exg_start+6) me_exg_end = me_exg_start+6;
	for(i = me_exg_start, j = 0; i < me_exg_end; i++, j++)
	{
		t_pos = m_arrArea[j]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_arrItemMeExg[i]->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	other_exg_end = m_arrItemOtherExg.GetSize();
	other_exg_start = m_pOtherExgScrollBar->GetStartLine() * 2;
	if(other_exg_end > other_exg_start+6) other_exg_end = other_exg_start+6;
	for(i = other_exg_start, j = 0; i < other_exg_end; i++, j++)
	{
		t_pos = m_arrArea[j+7]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_arrItemOtherExg[i]->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	me_end = m_arrItemMe.GetSize();
	me_start = m_pMeScrollBar->GetStartLine() * 6;
	if(me_end > me_start+12) me_end = me_start+12;
	for(i = me_start, j = 0; i < me_end; i++, j++)
	{
		t_pos = m_arrArea[j+16]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_arrItemMe[i]->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	
	for(i = me_exg_start, j = 0; i < me_exg_end; i++, j++)
	{
		ITEM_DETAIL *item_detail;

		item_detail = m_arrItemMeExg[i];
		if(item_detail->m_vType >= TYPE_POTION && item_detail->m_vType <= TYPE_TOKEN)
		{
			rtDst = m_arrArea[j]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", item_detail->m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
		}
	}

	for(i = other_exg_start, j = 0; i < other_exg_end; i++, j++)
	{
		ITEM_DETAIL *item_detail;

		item_detail = m_arrItemOtherExg[i];
		if(item_detail->m_vType >= TYPE_POTION && item_detail->m_vType <= TYPE_TOKEN)
		{
			rtDst = m_arrArea[j+7]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", item_detail->m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
		}
	}

	for(i = me_start, j = 0; i < me_end; i++, j++)
	{
		ITEM_DETAIL *item_detail;

		item_detail = m_arrItemMe[i];
		if(item_detail->m_vType >= TYPE_POTION && item_detail->m_vType <= TYPE_TOKEN)
		{
			rtDst = m_arrArea[j+16]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", item_detail->m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
		}
	}

	t_str.Format("%lu", ptmp->m_Money);
//	m_pMeMoney->SetString(t_str);
	rtDst = m_arrArea[28]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
//	m_pMeMoney->DrawTexts(pDest, rtDst, 0xffff, 0);			// My Money
	pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);

	rtDst = m_arrArea[14]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
//	m_pMeExgMoney->DrawTexts(pDest, rtDst, 0xffff, 0);		// My Exchnage Money
	pDC->DrawText(m_strMeExgMoney, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);

	rtDst = m_arrArea[15]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
//	m_pOtherExgMoney->DrawTexts(pDest, rtDst, 0xffff, 0);	// Other Exchnage Money
	pDC->DrawText(m_strOtherExgMoney, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	// Drag 아이템 그리기
	if(m_vSelSpec >= 0)
	{
		ITEM_DETAIL *item_me;

		item_me = m_arrItemMe[m_vDragItem];

		t_pos = CChildView::DirectXMng.GetMouseInfoPtr()->GetPos() - m_vDragOffset;
		rtSrc = Item_Total.m_Item_Small->m_SprData[item_me->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}
	else
	{
		if(m_vTipIndex >= 0)
		{
			CPoint t_pos;

			t_pos.x = m_arrArea[m_vTipIndex]->m_vRect.right+m_ptOffset.x;
			t_pos.y = m_arrArea[m_vTipIndex]->m_vRect.top+m_ptOffset.y;
			if(m_vTipIndex <= 5 && m_vTipIndex+me_exg_start < me_exg_end)
			{
				ITEM_DETAIL *item_detail = m_arrItemMeExg[me_exg_start+m_vTipIndex];
				t_str = MakeCaption(item_detail);
				ViewCaption(pDest, t_pos, t_str, 37);
			}
			else if(m_vTipIndex >= 7 && m_vTipIndex <= 12 && m_vTipIndex+other_exg_start-7 < other_exg_end)
			{
				ITEM_DETAIL *item_detail = m_arrItemOtherExg[other_exg_start+m_vTipIndex-7];
				t_str = MakeCaption(item_detail);
				ViewCaption(pDest, t_pos, t_str, 37);
			}
			else if(m_vTipIndex >= 16 && m_vTipIndex <= 27 && m_vTipIndex+me_start-16 < me_end)
			{
				ITEM_DETAIL *item_detail = m_arrItemMe[me_start+m_vTipIndex-16];
				t_str = MakeCaption(item_detail);
				ViewCaption(pDest, t_pos, t_str, 37);
			}
		}
	}

	return TRUE;
}

BOOL CDialog_Exchange::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:		// Close
			SendTradeCancel();
			HideWindow();
			break;
		case 1:		// Trade Ok
			SendTradeOk();
			break;
		case 2:		// Money Setting
			if(m_bOtherOk == FALSE) AskHowMuch();
			break;
		case 3:		// 스크롤업
			m_pMeExgScrollBar->ScrollUp();
			break;
		case 4:		// 스크롤다운
			m_pMeExgScrollBar->ScrollDown();
			break;
		case 5:		// 스크롤업
			m_pOtherExgScrollBar->ScrollUp();
			break;
		case 6:		// 스크롤다운
			m_pOtherExgScrollBar->ScrollDown();
			break;
		case 7:		// 스크롤업
			m_pMeScrollBar->ScrollUp();
			break;
		case 8:		// 스크롤다운
			m_pMeScrollBar->ScrollDown();
			break;
		}
	}

	return TRUE;
}

BOOL CDialog_Exchange::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pMeScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}
	if(m_pMeExgScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}
	if(m_pOtherExgScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}
	BOOL retVal = CUIRes::OnLButtonDown(pMouseInfo);
	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

	if(retVal)
	{
		if(m_vSpecCtrl == AREA_CTRL)
		{
			if(m_vIndexCtrl >= 16 && m_vIndexCtrl <= 27)
			{
				int item_index, count;

				item_index = m_pMeScrollBar->GetStartLine() * 6 + (m_vIndexCtrl-16);
				count = m_arrItemMe.GetSize();
				if(item_index < count)
				{
					m_vSelSpec = 0;
					m_vDragItem = item_index;
					m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
					SetCapture();
				}
			}
		}
	}

	return retVal;
}

BOOL CDialog_Exchange::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	if(m_vSelSpec >= 0)
	{
		CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

		if(m_arrArea[6]->m_vRect.PtInRect(m_pos))
		{
			ITEM_DETAIL *item_detail;

			item_detail = m_arrItemMe[m_vDragItem];
			if(item_detail->m_vType >= TYPE_POTION && item_detail->m_vType <= TYPE_TOKEN && item_detail->m_vUsage > 1)
			{
				m_vCurMeItem = m_vDragItem;
				Dialog_HowMany.ShowWindow(4, this);
			}
			else
			{
				SendTradeAddItem(m_arrItemMe[m_vDragItem]->m_vIndex);
				m_arrItemMeExg.Add(m_arrItemMe[m_vDragItem]);
				m_arrItemMe.RemoveAt(m_vDragItem);
				SetScrollMeData();
			}
		}
		m_vSelSpec = -1;
		m_vDragItem = -1;
	}
	ReleaseCapture();
	if(m_pMeExgScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;
	if(m_pOtherExgScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;
	if(m_pMeScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_Exchange::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pMeExgScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;
	if(m_pOtherExgScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;
	if(m_pMeScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;

	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;
	int i;
	
	for(i = 0; i <= 5; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
		{
			m_vTipIndex = i;
			return CUIRes::OnMouseMove(pMouseInfo);
		}
	}
	for(i = 7; i <= 12; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
		{
			m_vTipIndex = i;
			return CUIRes::OnMouseMove(pMouseInfo);
		}
	}
	for(i = 16; i <= 27; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
		{
			m_vTipIndex = i;
			return CUIRes::OnMouseMove(pMouseInfo);
		}
	}
	m_vTipIndex = -1;

	return CUIRes::OnMouseMove(pMouseInfo);
}

void CDialog_Exchange::AskTrade(CString other_user, int UId)
{
	CString t_str, message;

	if(m_bTrading == TRUE) return;
	m_bTrading = TRUE;
	m_vUId = UId;
	m_vOtherUser = other_user;
	Dialog_Message2.ShowWindow(0, this);
	t_str.LoadString(IDS_EXCHANGE_ASK);
	message.Format(t_str, other_user);
	Dialog_Message2.SetMessage(message);
}

void CDialog_Exchange::WaitTrade()
{
	CString t_str, message;

	Dialog_Message3.ShowWindow(1, this);
	t_str.LoadString(IDS_EXCHANGE_WAIT);
	message.Format(t_str, m_vOtherUser);
	Dialog_Message3.SetMessage(message);
}

void CDialog_Exchange::CallBack_MessageBox(int nId, int nValue)
{
//	CString t_str;

	switch(nId)
	{
	case 0:
		if(nValue == 1)
		{
			SendTradeOther();
			WaitTrade();
		}
		else
		{
			m_bTrading = FALSE;
			m_bOtherOk = FALSE;
			m_vUId = -1;
		}
		break;
	case 1:
		SendTradeCancel();
		break;
	case 2:
		if(nValue == 1)
		{
			SendTradeAck(1);	// Ok
		}
		else
		{
			SendTradeAck(2);	// Cancel
			m_bTrading = FALSE;
			m_bOtherOk = FALSE;
		}
		break;
	case 3:
		if(m_bOtherOk == FALSE && nValue == 1)
		{
			CUser *ptmp = CChildView::UserManager.GetMe();
			if(ptmp->m_Money >= Dialog_HowMuch.m_vValue)
			{
				SendTradeSetMoney(Dialog_HowMuch.m_vValue);
				m_strMeExgMoney.Format("%lu", Dialog_HowMuch.m_vValue);
			}
//		t_str.Format("%lu", Dialog_HowMuch.m_vValue);
//		m_pMeExgMoney->SetString(t_str);
		}
		break;
	case 4:
		if(nValue == 1)
		{
			ITEM_DETAIL *item_detail;
			short count;

			item_detail = m_arrItemMe[m_vCurMeItem];
			count = (short)Dialog_HowMany.m_vValue;

			if(count > 0 && count <= item_detail->m_vUsage)
			{
				SendTradeAddItem(item_detail->m_vIndex, count);
				if(count == item_detail->m_vUsage)
				{
					int exg_count = m_arrItemMeExg.GetSize();
					for(int i = 0; i < exg_count; i++)
					{
						if(m_arrItemMeExg[i]->m_vIndex == item_detail->m_vIndex)
						{
							m_arrItemMeExg[i]->m_vUsage += count;
							delete item_detail;
							m_arrItemMe.RemoveAt(m_vCurMeItem);
							SetScrollMeData();
							return;
						}
					}
					m_arrItemMeExg.Add(m_arrItemMe[m_vCurMeItem]);
					m_arrItemMe.RemoveAt(m_vCurMeItem);
					SetScrollMeData();
				}
				else
				{
					int exg_count = m_arrItemMeExg.GetSize();
					for(int i = 0; i < exg_count; i++)
					{
						if(m_arrItemMeExg[i]->m_vIndex == item_detail->m_vIndex)
						{
							m_arrItemMeExg[i]->m_vUsage += count;
							item_detail->m_vUsage -= count;
							return;
						}
					}
					ITEM_DETAIL *add_item = new ITEM_DETAIL;

					add_item->m_vIndex = item_detail->m_vIndex;
					add_item->m_vPicNum = item_detail->m_vPicNum;
					add_item->m_vType = item_detail->m_vType;
					add_item->m_vArmPos = item_detail->m_vArmPos;
					add_item->m_vGender = item_detail->m_vGender;
					add_item->m_vUsage = count;
					add_item->m_vName = item_detail->m_vName;
					m_arrItemMeExg.Add(add_item);
					m_arrItemMe[m_vCurMeItem]->m_vUsage -= count;
				}
			}
		}
		break;
	}
}

void CDialog_Exchange::SendTradeOther()
{
	BYTE buf[10];
	int offset = 0;
	BYTE event_type = 0x03;		// Request trading

	CPacketParser::SetByte(buf, offset, PKT_CLIENTEVENT);

	CPacketParser::SetByte(buf, offset, event_type);
	CPacketParser::SetInt(buf, offset, m_vUId);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Exchange::SendTradeAck(BYTE result)
{
	BYTE buf[30];
	int offset = 0;
	BYTE len;
	BYTE event_type = 0x03;		// Request trading

	CPacketParser::SetByte(buf, offset, PKT_TRADE_ACK);

	CPacketParser::SetInt(buf, offset, m_vUId);
	len = m_vOtherUser.GetLength();
	CPacketParser::SetByte(buf, offset, len);
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)m_vOtherUser, len);
	CPacketParser::SetByte(buf, offset, result);
	
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Exchange::SendTradeCancel()
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_TRADE_CANCEL);

	CPacketParser::SetInt(buf, offset, m_vUId);
	CChildView::SocketMng.Send(offset, buf);
	m_bTrading = FALSE;
	m_bOtherOk = FALSE;
}

void CDialog_Exchange::RcvReqTrade(PBYTE bypData)
{
	int offset = 0;
	int UId;
	BYTE len;
	CString user_name;
	CString t_str, message;
	LPTSTR t_buf;
	CPoint t_pos;

	m_bTrading = TRUE;
	UId = CPacketParser::GetInt(bypData, offset);		// UId
	len = CPacketParser::GetByte(bypData, offset);
	t_buf = user_name.GetBuffer(len);
	CPacketParser::GetString(bypData, offset, (PBYTE)t_buf, len);
	user_name.ReleaseBuffer();

	m_vUId = UId;
	m_vOtherUser = user_name;
	Dialog_Message2.ShowWindow(2, this);
	t_pos.x = (800 - m_mainRect.Width()) / 2;
	t_pos.y = (600 - m_mainRect.Height()) / 2;
	Dialog_Message2.SetOffset(t_pos);
	t_str.LoadString(IDS_EXCHANGE_ACK);
	message.Format(t_str, m_vOtherUser);
	Dialog_Message2.SetMessage(message);
}

void CDialog_Exchange::RcvAckTrade(PBYTE bypData)
{
	int offset = 0;
	BYTE result;

	result = CPacketParser::GetByte(bypData, offset);		// result

	Dialog_Message3.HideWindow();
	if(result == 1)
	{
		ShowWindow();
	}
	else
	{
		m_bTrading = FALSE;
		m_bOtherOk = FALSE;

		CString t_str;
		t_str.LoadString(IDS_EXCHANGE_DENY);
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(t_str);
	}
}

void CDialog_Exchange::SendTradeAddItem(short item_number, short count)
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_TRADE_ADDITEM);

	CPacketParser::SetShort(buf, offset, item_number);
	CPacketParser::SetShort(buf, offset, count);
	
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Exchange::RcvTradeItem(PBYTE bypData)
{
	int offset = 0;
	BYTE len;
	LPTSTR t_str;
	ITEM_DETAIL *item_detail;
	short index = CPacketParser::GetShort(bypData, offset);
	short count;

	int exg_count = m_arrItemOtherExg.GetSize();
	for(int i = 0; i < exg_count; i++)
	{
		if(m_arrItemOtherExg[i]->m_vIndex == index)
		{
			CPacketParser::GetByte(bypData, offset);	// type skip
			CPacketParser::GetByte(bypData, offset);	// armpos skip
			CPacketParser::GetShort(bypData, offset);	// picnum skip
			count = CPacketParser::GetShort(bypData, offset);
			m_arrItemOtherExg[i]->m_vUsage += count;
			return;
		}
	}
	item_detail = new ITEM_DETAIL;

	item_detail->m_vIndex = index;
	item_detail->m_vType = CPacketParser::GetByte(bypData, offset);
	item_detail->m_vArmPos = CPacketParser::GetByte(bypData, offset);
	item_detail->m_vPicNum = CPacketParser::GetShort(bypData, offset);
	item_detail->m_vUsage = CPacketParser::GetShort(bypData, offset);
	item_detail->m_vGender = CPacketParser::GetByte(bypData, offset);
	item_detail->m_vMinDamage = CPacketParser::GetShort(bypData, offset);		// 최소 위력
	item_detail->m_vMaxDamage = CPacketParser::GetShort(bypData, offset);		// 최대 위력
	item_detail->m_vLimitStr = CPacketParser::GetShort(bypData, offset);
	item_detail->m_vLimitInt = CPacketParser::GetShort(bypData, offset);
	item_detail->m_vLimitSkill = CPacketParser::GetShort(bypData, offset);
	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		CPacketParser::GetString(bypData, offset, (PBYTE)item_detail->m_strMagic.GetBuffer(len), len);
		item_detail->m_strMagic.ReleaseBuffer();
	}
	else item_detail->m_strMagic.Empty();
	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		CPacketParser::GetString(bypData, offset, (PBYTE)item_detail->m_strSNum.GetBuffer(len), len);
		item_detail->m_strSNum.ReleaseBuffer();
	}
	else item_detail->m_strMagic.Empty();
	for(int k = 0; k < 7; k++) item_detail->m_vSpecial[k] = CPacketParser::GetShort(bypData, offset);	// 특수 기능
	len = CPacketParser::GetByte(bypData, offset);
	t_str = item_detail->m_vName.GetBuffer(len);
	CPacketParser::GetString(bypData, offset, (PBYTE)t_str, len);
	item_detail->m_vName.ReleaseBuffer();

	m_arrItemOtherExg.Add(item_detail);
	SetScrollOtherData();
}

void CDialog_Exchange::SendTradeSetMoney(DWORD money)
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_TRADE_SETMONEY);

	CPacketParser::SetDword(buf, offset, money);
	
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Exchange::RcvTradeMoney(PBYTE bypData)
{
	int offset = 0;
	DWORD money;
//	CString t_str;

	money = CPacketParser::GetDword(bypData, offset);
//	t_str.Format("%lu", money);
//	m_pOtherExgMoney->SetString(t_str);
	m_strOtherExgMoney.Format("%lu", money);
}

void CDialog_Exchange::AskHowMuch()
{
	Dialog_HowMuch.ShowWindow(3, this);
}

void CDialog_Exchange::SendTradeOk()
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_TRADE_OK);

	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Exchange::RcvTradeResult(PBYTE bypData)
{
	int offset = 0;
	BYTE result;
	CString t_str;

	m_bTrading = FALSE;
	m_bOtherOk = FALSE;
	HideWindow();
	result = CPacketParser::GetByte(bypData, offset);	// 0:성공, 1:실패 2:실패(무게 오버) 3:실패(돈 오버) 4:실패(거리 오버)
	switch(result)
	{
	case 0:		// 성공
		t_str.LoadString(IDS_EXCHANGE_SUCCESS);
		break;
	case 1:		// 실패
		t_str.LoadString(IDS_EXCHANGE_FAIL);
		break;
	case 2:		// 실패 (무게 초과)
		t_str.LoadString(IDS_EXCHANGE_FAIL1);
		break;
	case 3:		// 실패 (돈 초과)
		t_str.LoadString(IDS_EXCHANGE_FAIL2);
		break;
	case 4:		// 실패 (거리 초과)
		t_str.LoadString(IDS_EXCHANGE_FAIL3);
		break;
	}
	Dialog_Message.ShowWindow();
	Dialog_Message.SetMessage(t_str);
}

void CDialog_Exchange::RcvTradeCancel()
{
	Dialog_Message2.HideWindow();
	Dialog_Message3.HideWindow();
	Dialog_HowMuch.HideWindow();
	Dialog_HowMany.HideWindow();
	HideWindow();
	m_bTrading = FALSE;
	m_bOtherOk = FALSE;

	CString t_str;
	t_str.LoadString(IDS_EXCHANGE_CANCEL);
	Dialog_Message.ShowWindow();
	Dialog_Message.SetMessage(t_str);
}

void CDialog_Exchange::RcvTradeOk()
{
	m_bOtherOk = TRUE;
}

void CDialog_Exchange::InitData()
{
	m_vSelSpec = -1;
	m_vDragItem = -1;
	m_bTrading = FALSE;
	m_bOtherOk = FALSE;
//	m_pMeExgMoney->ClearString();
//	m_pOtherExgMoney->ClearString();
	m_strMeExgMoney.Empty();
	m_strOtherExgMoney.Empty();
}

void CDialog_Exchange::DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index)
{
	int width, height, i, j;
	DWORD dwTrans;
	CPoint start_pos;
	CSize rect_size;
	int r_width[3], r_height[3];

	r_width[0] = m_arrArea[area_index]->m_vRect.Width();
	r_width[1] = m_arrArea[area_index+1]->m_vRect.Width();
	r_width[2] = m_arrArea[area_index+2]->m_vRect.Width();
	r_height[0] = m_arrArea[area_index]->m_vRect.Height();
	r_height[1] = m_arrArea[area_index+3]->m_vRect.Height();
	r_height[2] = m_arrArea[area_index+6]->m_vRect.Height();

	width = t_rect.Width() / r_width[1] + ((t_rect.Width()%r_width[1])?1:0);
	height = t_rect.Height() / r_height[1] + ((t_rect.Height()%r_height[1])?1:0);

	dwTrans = DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY;

	start_pos = t_rect.TopLeft();
	pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index]->m_vRect, dwTrans);
	start_pos.x += r_width[0];
	for(j = 0; j < width; j++)
	{
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+1]->m_vRect, dwTrans);
		start_pos.x += r_width[1];
	}
	pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+2]->m_vRect, dwTrans);
	start_pos.y += r_height[0];

	for(i = 0; i < height; i++)
	{
		start_pos.x = t_rect.left;
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+3]->m_vRect, dwTrans);
		start_pos.x += r_width[0];
		for(j = 0; j < width; j++)
		{
			pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+4]->m_vRect, dwTrans);
			start_pos.x += r_width[1];
		}
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+5]->m_vRect, dwTrans);
		start_pos.y += r_height[1];
	}

	start_pos.x = t_rect.left;
	pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+6]->m_vRect, dwTrans);
	start_pos.x += r_width[0];
	for(j = 0; j < width; j++)
	{
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+7]->m_vRect, dwTrans);
		start_pos.x += r_width[1];
	}
	pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+8]->m_vRect, dwTrans);

	rect_size.cx = ((r_width[0]+r_width[1]*width+r_width[2]) - t_rect.Width()) / 2;
	rect_size.cy = ((r_height[0]+r_height[1]*height+r_height[2]) - t_rect.Height()) / 2;
	t_rect.OffsetRect(rect_size.cx, rect_size.cy);
}

void CDialog_Exchange::ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index)
{
	int def_width = 200;
	CRect t_rect;
	CFont pFont, *old_font;
	CDC *pDC = pDest->GetDC();

	old_font = pDC->SelectObject(m_pFont);
	t_rect.SetRect(0, 0, def_width, 14);
	pDC->DrawText(str, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_WORDBREAK|DT_CALCRECT);
	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	t_rect.OffsetRect(pos);

	DrawTipRect(pDest, t_rect, area_index);

	pDC = pDest->GetDC();
	pDC->SetBkMode(TRANSPARENT);

	old_font = pDC->SelectObject(m_pFont);

	// 검은색
	pDC->SetTextColor(RGB(5,5,5));
	pDC->DrawText(str, &t_rect, DT_CENTER|DT_TOP);
	
	t_rect.OffsetRect(-1, -1);

	pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(str, &t_rect, DT_CENTER|DT_TOP|DT_EDITCONTROL|DT_WORDBREAK);

	pDC->SelectObject(old_font);

	pDest->ReleaseDC(pDC);
}

CString CDialog_Exchange::MakeCaption(ITEM_DETAIL *item_detail)
{
	CString t_str, tt_str, tt_str1;

	if(item_detail->m_vType <= TYPE_ARMOR)
	{
		CString tt_str, tt_str1;
		BOOL btrue = FALSE;
		
		if(item_detail->m_vType == TYPE_ARMOR)
		{
			switch(item_detail->m_vGender)
			{
			case ITEM_WOMAN:
				t_str.Format("%s(여)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
					item_detail->m_vName,
					item_detail->m_vMinDamage, item_detail->m_vMaxDamage, item_detail->m_vUsage,
					item_detail->m_vLimitStr, item_detail->m_vLimitInt);
				break;
			case ITEM_MAN:
				t_str.Format("%s(남)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
					item_detail->m_vName,
					item_detail->m_vMinDamage, item_detail->m_vMaxDamage, item_detail->m_vUsage,
					item_detail->m_vLimitStr, item_detail->m_vLimitInt);
				break;
			default:
				t_str.Format("%s\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
					item_detail->m_vName,
					item_detail->m_vMinDamage, item_detail->m_vMaxDamage, item_detail->m_vUsage,
					item_detail->m_vLimitStr, item_detail->m_vLimitInt);
				break;
			}
		}
		else if(item_detail->m_vType < TYPE_WEAPON)
		{
			switch(item_detail->m_vGender)
			{
			case ITEM_WOMAN:
				t_str.Format("%s(여)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
					item_detail->m_vName,
					item_detail->m_vMinDamage, item_detail->m_vMaxDamage, item_detail->m_vUsage,
					item_detail->m_vLimitStr, item_detail->m_vLimitInt, item_detail->m_vLimitSkill);
				break;
			case ITEM_MAN:
				t_str.Format("%s(남)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
					item_detail->m_vName,
					item_detail->m_vMinDamage, item_detail->m_vMaxDamage, item_detail->m_vUsage,
					item_detail->m_vLimitStr, item_detail->m_vLimitInt, item_detail->m_vLimitSkill);
				break;
			default:
				t_str.Format("%s\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
					item_detail->m_vName,
					item_detail->m_vMinDamage, item_detail->m_vMaxDamage, item_detail->m_vUsage,
					item_detail->m_vLimitStr, item_detail->m_vLimitInt, item_detail->m_vLimitSkill);
				break;
			}
		}
		tt_str = "\n";
		for(int k = 0; k < 7; k++)
		{
			if(item_detail->m_vSpecial[k] != 0)
			{
				if(btrue) tt_str += _T(",");
				if(item_detail->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_detail->m_vSpecial[k]);
				else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_detail->m_vSpecial[k]);
				tt_str += tt_str1;
				btrue = TRUE;
			}
		}
		if(btrue == TRUE)
		{
			t_str += tt_str;
		}
		if(item_detail->m_strMagic.IsEmpty() == FALSE)
		{
			t_str += "\n";
			t_str += item_detail->m_strMagic;
		}
		if(item_detail->m_strSNum.IsEmpty() == FALSE)
		{
			t_str += "\n";
			t_str += item_detail->m_strSNum;
		}
	}
	else if(item_detail->m_vType == TYPE_ACC)
	{
		CString tt_str, tt_str1;
		BOOL btrue = FALSE;

		switch(item_detail->m_vGender)
		{
		case ITEM_WOMAN:
			t_str.Format("%s(여)\n내구력:%d",
				item_detail->m_vName,
				item_detail->m_vUsage);
			break;
		case ITEM_MAN:
			t_str.Format("%s(남)\n내구력:%d",
				item_detail->m_vName,
				item_detail->m_vUsage);
			break;
		default:
			t_str.Format("%s\n내구력:%d",
				item_detail->m_vName,
				item_detail->m_vUsage);
			break;
		}
		tt_str = "\n";
		for(int k = 0; k < 7; k++)
		{
			if(item_detail->m_vSpecial[k] != 0)
			{
				if(btrue) tt_str += _T(",");
				if(item_detail->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_detail->m_vSpecial[k]);
				else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_detail->m_vSpecial[k]);
				tt_str += tt_str1;
				btrue = TRUE;
			}
		}
		if(btrue == TRUE)
		{
			t_str += tt_str;
		}
		if(item_detail->m_strMagic.IsEmpty() == FALSE)
		{
			t_str += "\n";
			t_str += item_detail->m_strMagic;
		}
		if(item_detail->m_strSNum.IsEmpty() == FALSE)
		{
			t_str += "\n";
			t_str += item_detail->m_strSNum;
		}
	}
	else if(item_detail->m_vType == TYPE_POTION || item_detail->m_vType == TYPE_FOOD)
	{
		CString tt_str, tt_str1;
		BOOL btrue = FALSE;

		switch(item_detail->m_vGender)
		{
		case ITEM_WOMAN:
			t_str.Format("%s(여)", item_detail->m_vName);
			break;
		case ITEM_MAN:
			t_str.Format("%s(남)", item_detail->m_vName);
			break;
		default:
			t_str.Format("%s", item_detail->m_vName);
			break;
		}
		tt_str = "\n";
		for(int k = 0; k < 7; k++)
		{
			if(item_detail->m_vSpecial[k] != 0)
			{
				if(btrue) tt_str += _T(",");
				if(item_detail->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_detail->m_vSpecial[k]);
				else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_detail->m_vSpecial[k]);
				tt_str += tt_str1;
				btrue = TRUE;
			}
		}
		if(btrue == TRUE)
		{
			t_str += tt_str;
		}
		if(item_detail->m_strMagic.IsEmpty() == FALSE)
		{
			t_str += "\n";
			t_str += item_detail->m_strMagic;
		}
		if(item_detail->m_strSNum.IsEmpty() == FALSE)
		{
			t_str += "\n";
			t_str += item_detail->m_strSNum;
		}
	}
	else
	{
		switch(item_detail->m_vGender)
		{
		case ITEM_WOMAN:
			t_str.Format("%s(여)", item_detail->m_vName);
			break;
		case ITEM_MAN:
			t_str.Format("%s(남)", item_detail->m_vName);
			break;
		default:
			t_str = item_detail->m_vName;
			break;
		}
	}

	return t_str;
}