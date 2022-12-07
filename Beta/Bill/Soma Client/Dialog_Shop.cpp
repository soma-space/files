// Dialog_Shop.cpp: implementation of the CDialog_Shop class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Shop.h"
//#include "BBDX_TextOuter.h"
#include "Item_Total.h"
#include "PacketParser.h"
#include "User.h"
//#include "TWFont.h"
#include "Dialog_HowMany.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CItem_Total         Item_Total;
extern CUIRes *g_pShowDialog;
extern CDialog_HowMany		Dialog_HowMany;

static const char SPECIAL_NAME[][4] = {"Str", "Dex", "Int", "Atk", "Def", "HP", "MP"};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Shop::CDialog_Shop()
{
//	INIT_PTR(m_pMoneyOuter);
//	INIT_PTR(m_pCostOuter);
	INIT_PTR(m_pHandScrollBar);
	INIT_PTR(m_pShopScrollBar);
//	INIT_PTR(m_pTWFont);
	INIT_PTR(m_pFont);

	m_vShopMode = 0;
	m_vShopNumber = -1;
	m_vCurMeItem = m_vCurShopItem = -1;
	m_vTipIndex = -1;
	m_vPosMode = POS_LEFT;
}

CDialog_Shop::~CDialog_Shop()
{
	DeleteContents();

	SAFE_DELETE(m_pHandScrollBar);
	SAFE_DELETE(m_pShopScrollBar);
//	SAFE_DELETE(m_pTWFont);
//	SAFE_DELETE(m_pMoneyOuter);
//	SAFE_DELETE(m_pCostOuter);
	if(m_pFont)
	{
		m_pFont->DeleteObject();
		delete m_pFont;
	}
}

void CDialog_Shop::ShopSendSell(int how_many)
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_SELL_SHOP); //최종점 요청
	TRACE("Send PKT_SELL_SHOP \n");
	
	CPacketParser::SetShort(buf, offset, m_vShopNumber);
	CPacketParser::SetShort(buf, offset, (short)m_arrItemMe[m_vCurMeItem]->m_vIndex);
	CPacketParser::SetShort(buf, offset, (short)how_many);
	CChildView::SocketMng.Send(offset, buf);
	m_bProcessing = TRUE;
}

void CDialog_Shop::ShopSendBuy()
{
	BYTE buf[10];
	int offset = 0;
	int count;

	count = m_arrItemShop.GetSize();
	if(m_vCurShopItem < 0 || m_vCurShopItem >= count) return;
	CPacketParser::SetByte(buf, offset, PKT_BUY_SHOP); //최종점 요청
	TRACE("Send PKT_BUY_SHOP \n");
	
	//Short  상점번호
	//Short  아이템 테이블번호
	CPacketParser::SetShort(buf, offset, m_vShopNumber);
	CPacketParser::SetShort(buf, offset, (short)m_arrItemShop[m_vCurShopItem]->m_vItemNumber);
	CChildView::SocketMng.Send(offset, buf);
	m_bProcessing = TRUE;
}

BOOL CDialog_Shop::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex) {
		case 0:	// 닫기
			HideWindow();
			break;
		case 1:	// 팔기
			if(m_bProcessing == FALSE)
			{
				int count;
				ITEM_ME *item_me;

				count = m_arrItemMe.GetSize();
				if(m_vCurMeItem < 0 || m_vCurMeItem >= count) return TRUE;
				item_me = m_arrItemMe[m_vCurMeItem];
				if(item_me->m_vType >= TYPE_POTION && item_me->m_vType <= TYPE_TOKEN && item_me->m_vUsage > 1)
				{
					Dialog_HowMany.ShowWindow(0, this);
				}
				else
				{
					ShopSendSell(1);
				}
			}
			break;
		case 2:	// 사기
			if(m_bProcessing == FALSE) ShopSendBuy();
			break;
		case 3:
			m_pHandScrollBar->ScrollUp();
			break;
		case 4:
			m_pHandScrollBar->ScrollDown();
			break;
		case 5:
			m_pShopScrollBar->ScrollUp();
			break;
		case 6:
			m_pShopScrollBar->ScrollDown();
			break;
		}
	}
	else if(nSpec == AREA_CTRL)
	{
		if(nIndex >= 4 && nIndex <= 13)
		{
			m_vCurMeItem = nIndex - 4 + m_pHandScrollBar->GetStartLine()*2;
			if(m_vCurMeItem < 0 || m_vCurMeItem >= m_arrItemMe.GetSize()) m_vCurMeItem = -1;
			else m_vCurShopItem = -1;
		}
		if(nIndex >= 14 && nIndex <= 23)
		{
			m_vCurShopItem = nIndex - 14 + m_pShopScrollBar->GetStartLine()*2;
			if(m_vCurShopItem < 0 || m_vCurShopItem >= m_arrItemShop.GetSize()) m_vCurShopItem = -1;
			else m_vCurMeItem = -1;
		}
	}

	return TRUE;
}

BOOL CDialog_Shop::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE || m_vShopNumber == -1) return TRUE;

	CUIRes::Draw(pDest);

	CRect rtDst, rtSrc;
	DWORD dwTrans;
	int hand_end, hand_start, shop_end, shop_start, i, j;
	CUser *ptmp;
	CString t_str;
	CPoint t_pos;
	
	rtDst.SetRect(m_ptOffset.x, m_ptOffset.y, m_ptOffset.x+m_arrArea[0]->m_vRect.Width(), m_ptOffset.y+m_arrArea[0]->m_vRect.Height());
	dwTrans = DDBLTFAST_WAIT;

	switch(m_vShopMode)		// 6 : Weapon Shop
	{
	case 7:					// Armor Shop
		pDest->BltFast(m_ptOffset, m_pDDSurface, &m_arrArea[1]->m_vRect, dwTrans);
		break;
	case 8:					// Accessary Shop
		pDest->BltFast(m_ptOffset, m_pDDSurface, &m_arrArea[2]->m_vRect, dwTrans);
		break;
	case 9:					// Potion Shop
		pDest->BltFast(m_ptOffset, m_pDDSurface, &m_arrArea[3]->m_vRect, dwTrans);
		break;
	}

	Draw3ButtonCtrl(pDest, 0, 6);

	m_pHandScrollBar->Draw(pDest);
	m_pShopScrollBar->Draw(pDest);

	hand_end = m_arrItemMe.GetSize();
	hand_start = m_pHandScrollBar->GetStartLine() * 2;
	if(hand_end > hand_start+10) hand_end = hand_start+10;
	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	for(i = hand_start, j = 0; i < hand_end; i++, j++)
	{
		t_pos = m_arrArea[j+4]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_arrItemMe[i]->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	shop_end = m_arrItemShop.GetSize();
	shop_start = m_pShopScrollBar->GetStartLine() * 2;
	if(shop_end > shop_start+10) shop_end = shop_start+10;
	for(i = shop_start, j = 0; i < shop_end; i++, j++)
	{
		t_pos = m_arrArea[j+14]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_arrItemShop[i]->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));

	ptmp = CChildView::UserManager.GetMe();
	t_str.Format("%lu", ptmp->m_Money);
//	m_pMoneyOuter->SetString(t_str);

	rtDst = m_arrArea[26]->m_vRect;		// Money
	rtDst.OffsetRect(m_ptOffset);
//	m_pMoneyOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);

	if(m_vCurShopItem >= 0)
	{
		i = m_vCurShopItem;// + m_pShopScrollBar->GetStartLine() * 2;
		t_str.Format("%lu", m_arrItemShop[i]->m_vValue);
//		m_pCostOuter->SetString(t_str);

		rtDst = m_arrArea[27]->m_vRect;		// Cost
		rtDst.OffsetRect(m_ptOffset);
//		m_pCostOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
		pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
	}
	else if(m_vCurMeItem >= 0)
	{
		i = m_vCurMeItem;// + m_pHandScrollBar->GetStartLine() * 2;
		t_str.Format("%lu", m_arrItemMe[i]->m_vValue);
//		m_pCostOuter->SetString(t_str);

		rtDst = m_arrArea[27]->m_vRect;		// Cost
		rtDst.OffsetRect(m_ptOffset);
//		m_pCostOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
		pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
	}

	for(i = hand_start, j = 0; i < hand_end; i++, j++)
	{
		if(m_arrItemMe[i]->m_vType >= TYPE_POTION && m_arrItemMe[i]->m_vType <= TYPE_TOKEN)
		{
			rtDst = m_arrArea[j+4]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", m_arrItemMe[i]->m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
		}
	}

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	if(m_vCurMeItem >= hand_start && m_vCurMeItem < hand_start+10)
	{
		pDest->BltFast(m_arrArea[m_vCurMeItem-hand_start+4]->m_vRect.TopLeft()+m_ptOffset, m_pDDSurface, &m_arrArea[28]->m_vRect, dwTrans);
	}
	else if(m_vCurShopItem >= shop_start && m_vCurShopItem < shop_start+10)
	{
		pDest->BltFast(m_arrArea[m_vCurShopItem-shop_start+14]->m_vRect.TopLeft()+m_ptOffset, m_pDDSurface, &m_arrArea[28]->m_vRect, dwTrans);
	}

	if(m_vTipIndex >= 0)
	{
		CPoint caption_pos;

		caption_pos.x = m_arrArea[m_vTipIndex]->m_vRect.right + m_ptOffset.x;
		caption_pos.y = m_arrArea[m_vTipIndex]->m_vRect.top + m_ptOffset.y;
		if(m_vTipIndex >= 4 && m_vTipIndex <= 13 && m_vTipIndex+hand_start-4 < hand_end)
		{
			ITEM_ME *item_me = m_arrItemMe[m_vTipIndex+hand_start-4];
			if(item_me->m_vType <= TYPE_ARMOR)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				if(item_me->m_vType == TYPE_ARMOR)
				{
					switch(item_me->m_vGender)
					{
					case ITEM_WOMAN:
						t_str.Format("%s(여)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_me->m_vName,
							item_me->m_vMinDamage, item_me->m_vMaxDamage, item_me->m_vUsage,
							item_me->m_vLimitStr, item_me->m_vLimitInt);
						break;
					case ITEM_MAN:
						t_str.Format("%s(남)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_me->m_vName,
							item_me->m_vMinDamage, item_me->m_vMaxDamage, item_me->m_vUsage,
							item_me->m_vLimitStr, item_me->m_vLimitInt);
						break;
					default:
						t_str.Format("%s\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_me->m_vName,
							item_me->m_vMinDamage, item_me->m_vMaxDamage, item_me->m_vUsage,
							item_me->m_vLimitStr, item_me->m_vLimitInt);
						break;
					}
				}
				else if(item_me->m_vType < TYPE_WEAPON)
				{
					switch(item_me->m_vGender)
					{
					case ITEM_WOMAN:
						t_str.Format("%s(여)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_me->m_vName,
							item_me->m_vMinDamage, item_me->m_vMaxDamage, item_me->m_vUsage,
							item_me->m_vLimitStr, item_me->m_vLimitInt, item_me->m_vLimitSkill);
						break;
					case ITEM_MAN:
						t_str.Format("%s(남)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_me->m_vName,
							item_me->m_vMinDamage, item_me->m_vMaxDamage, item_me->m_vUsage,
							item_me->m_vLimitStr, item_me->m_vLimitInt, item_me->m_vLimitSkill);
						break;
					default:
						t_str.Format("%s\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_me->m_vName,
							item_me->m_vMinDamage, item_me->m_vMaxDamage, item_me->m_vUsage,
							item_me->m_vLimitStr, item_me->m_vLimitInt, item_me->m_vLimitSkill);
						break;
					}
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_me->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_me->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_me->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_me->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_me->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_me->m_strMagic;
				}
				if(item_me->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_me->m_strSNum;
				}
			}
			else if(item_me->m_vType == TYPE_ACC)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				switch(item_me->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)\n내구력:%d",
						item_me->m_vName,
						item_me->m_vUsage);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)\n내구력:%d",
						item_me->m_vName,
						item_me->m_vUsage);
					break;
				default:
					t_str.Format("%s\n내구력:%d",
						item_me->m_vName,
						item_me->m_vUsage);
					break;
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_me->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_me->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_me->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_me->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_me->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_me->m_strMagic;
				}
				if(item_me->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_me->m_strSNum;
				}
			}
			else if(item_me->m_vType == TYPE_POTION || item_me->m_vType == TYPE_FOOD)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				switch(item_me->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)", item_me->m_vName);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)", item_me->m_vName);
					break;
				default:
					t_str.Format("%s", item_me->m_vName);
					break;
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_me->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_me->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_me->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_me->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_me->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_me->m_strMagic;
				}
				if(item_me->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_me->m_strSNum;
				}
			}
			else
			{
				switch(item_me->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)", item_me->m_vName);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)", item_me->m_vName);
					break;
				default:
					t_str = item_me->m_vName;
					break;
				}
			}
			ViewCaption(pDest, caption_pos, t_str, 29);
		}
		else if(m_vTipIndex >= 14 && m_vTipIndex <= 23 && m_vTipIndex+shop_start-14 < shop_end)
		{
			ITEM_SHOP *item_shop = m_arrItemShop[shop_start+m_vTipIndex-14];
			if(m_vShopMode == 6 || m_vShopMode == 7)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				if(m_vShopMode == 7)
				{
					switch(item_shop->m_vGender)
					{
					case ITEM_WOMAN:
						t_str.Format("%s(여)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_shop->m_vName,
							item_shop->m_vMinDamage, item_shop->m_vMaxDamage, item_shop->m_vUsage,
							item_shop->m_vLimitStr, item_shop->m_vLimitInt);
						break;
					case ITEM_MAN:
						t_str.Format("%s(남)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_shop->m_vName,
							item_shop->m_vMinDamage, item_shop->m_vMaxDamage, item_shop->m_vUsage,
							item_shop->m_vLimitStr, item_shop->m_vLimitInt);
						break;
					default:
						t_str.Format("%s\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_shop->m_vName,
							item_shop->m_vMinDamage, item_shop->m_vMaxDamage, item_shop->m_vUsage,
							item_shop->m_vLimitStr, item_shop->m_vLimitInt);
						break;
					}
				}
				else if(m_vShopMode == 6)
				{
					switch(item_shop->m_vGender)
					{
					case ITEM_WOMAN:
						t_str.Format("%s(여)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_shop->m_vName,
							item_shop->m_vMinDamage, item_shop->m_vMaxDamage, item_shop->m_vUsage,
							item_shop->m_vLimitStr, item_shop->m_vLimitInt, item_shop->m_vLimitSkill);
						break;
					case ITEM_MAN:
						t_str.Format("%s(남)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_shop->m_vName,
							item_shop->m_vMinDamage, item_shop->m_vMaxDamage, item_shop->m_vUsage,
							item_shop->m_vLimitStr, item_shop->m_vLimitInt, item_shop->m_vLimitSkill);
						break;
					default:
						t_str.Format("%s\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_shop->m_vName,
							item_shop->m_vMinDamage, item_shop->m_vMaxDamage, item_shop->m_vUsage,
							item_shop->m_vLimitStr, item_shop->m_vLimitInt, item_shop->m_vLimitSkill);
						break;
					}
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_shop->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_shop->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_shop->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_shop->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_shop->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_shop->m_strMagic;
				}
				if(item_shop->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_shop->m_strSNum;
				}
			}
			else if(m_vShopMode == 8)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				switch(item_shop->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)\n내구력:%d",
						item_shop->m_vName,
						item_shop->m_vUsage);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)\n내구력:%d",
						item_shop->m_vName,
						item_shop->m_vUsage);
					break;
				default:
					t_str.Format("%s\n내구력:%d",
						item_shop->m_vName,
						item_shop->m_vUsage);
					break;
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_shop->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_shop->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_shop->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_shop->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_shop->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_shop->m_strMagic;
				}
				if(item_shop->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_shop->m_strSNum;
				}
			}
			else if(m_vShopMode == 9)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				switch(item_shop->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)", item_shop->m_vName);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)", item_shop->m_vName);
					break;
				default:
					t_str.Format("%s", item_shop->m_vName);
					break;
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_shop->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_shop->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_shop->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_shop->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_shop->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_shop->m_strMagic;
				}
				if(item_shop->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_shop->m_strSNum;
				}
			}
			else
			{
				switch(item_shop->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)", item_shop->m_vName);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)", item_shop->m_vName);
					break;
				default:
					t_str = item_shop->m_vName;
					break;
				}
			}
			ViewCaption(pDest, caption_pos, t_str, 29);
		}
	}
	return TRUE;
}

void CDialog_Shop::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Shop.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;
	m_bProcessing = FALSE;

	CUIRes::ShowWindow();
	InitData();
	RefreshData();
}

void CDialog_Shop::HideWindow()
{
	g_pShowDialog = NULL;

	HideAllWindow();
}

void CDialog_Shop::HideAllWindow()
{
	CUIRes::HideWindow();
	m_vShopNumber = -1;
	m_vTipIndex = -1;

	DeleteContents();
}

void CDialog_Shop::DeleteContents()
{
	int size, i;

	size = m_arrItemMe.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemMe[i];
	}
	m_arrItemMe.RemoveAll();

	size = m_arrItemShop.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemShop[i];
	}
	m_arrItemShop.RemoveAll();
}

void CDialog_Shop::RefreshData()
{
	int i, size;
	ITEM_ME *item_me;
	CUser *ptmp;
	int count, max_line;
	double t_value;

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
			item_me = new ITEM_ME;

			item_me->m_vIndex = i;
			item_me->m_vPicNum = ptmp->GetInv_ItemPic(i);
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
			if(item_me->m_vType <= TYPE_ACC)
			{
				t_value = (double)ptmp->m_MyInvData[i].m_vValue * 0.5;	// 팔 때는 아이템 가격의 50%
				t_value *= (double)m_vMarketPrice / 100.0;		// 그 상점의 시세 적용
				t_value *= (double)ptmp->m_MyInvData[i].m_vUsage/(double)ptmp->m_MyInvData[i].m_vOrgMaxUsage;		// 사용회수 적용
				item_me->m_vValue = (DWORD)(t_value + 0.999);
			}
			else
			{
				t_value = (double)ptmp->m_MyInvData[i].m_vValue * 0.5;	// 팔 때는 아이템 가격의 50%
				t_value *= (double)m_vMarketPrice / 100.0;		// 그 상점의 시세 적용
				item_me->m_vValue = (DWORD)(t_value + 0.999);
			}

			m_arrItemMe.Add(item_me);
		}
	}
	m_vCurMeItem = -1;

	count = m_arrItemMe.GetSize();
	max_line = count / 2 + (count % 2);
	m_pHandScrollBar->SetMaxLineCount(max_line);
	m_pHandScrollBar->SetViewLineCount(5);
	m_pHandScrollBar->SetStartLine(m_pHandScrollBar->GetStartLine());
}

void CDialog_Shop::ShopOpen(PBYTE bypData)
{
	int offset, i, size;
	short itemcount;
	BYTE len;
	CString Item_Name;
	ITEM_SHOP *item_shop;
	int count, max_line;

	// 초기화
	size = m_arrItemShop.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemShop[i];
	}
	m_arrItemShop.RemoveAll();

	offset = 0;
	m_vShopNumber = CPacketParser::GetShort(bypData, offset);		// 상점 번호
	m_vShopMode = CPacketParser::GetShort(bypData, offset);			// 상점 종류(6:무기점, 7:방어구점, 8:장신구점, 9:시약, 10:음식점)
	m_vMarketPrice = (DWORD)CPacketParser::GetShort(bypData, offset);	// 상점 시세
	itemcount = CPacketParser::GetShort(bypData, offset);		// 아이템 개수
	for(i = 0; i < itemcount; i++)
	{
		// data를 집어 넣는다.
		item_shop = new ITEM_SHOP;
		
		item_shop->m_vItemNumber = CPacketParser::GetShort(bypData, offset); // 아이템 고유번호이다.
		item_shop->m_vPicNum = CPacketParser::GetShort(bypData, offset);
		item_shop->m_vGender = CPacketParser::GetByte(bypData, offset);
		item_shop->m_vMinDamage = CPacketParser::GetShort(bypData, offset);
		item_shop->m_vMaxDamage = CPacketParser::GetShort(bypData, offset);
		item_shop->m_vLimitStr = CPacketParser::GetShort(bypData, offset);
		item_shop->m_vLimitInt = CPacketParser::GetShort(bypData, offset);
		item_shop->m_vLimitSkill = CPacketParser::GetShort(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)item_shop->m_strMagic.GetBuffer(len), len);
			item_shop->m_strMagic.ReleaseBuffer();
		}
		else item_shop->m_strMagic.Empty();
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)item_shop->m_strSNum.GetBuffer(len), len);
			item_shop->m_strSNum.ReleaseBuffer();
		}
		else item_shop->m_strMagic.Empty();
		for(int k = 0; k < 7; k++) item_shop->m_vSpecial[k] = CPacketParser::GetShort(bypData, offset);
		item_shop->m_vUsage = CPacketParser::GetShort(bypData, offset);
		item_shop->m_vValue = CPacketParser::GetDword(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		LPSTR lpStr = Item_Name.GetBufferSetLength(len);
		CPacketParser::GetString(bypData, offset, (PBYTE)item_shop->m_vName.GetBuffer(len), len);
		item_shop->m_vName.ReleaseBuffer();
		
		m_arrItemShop.Add(item_shop);
	}

	ShowWindow();

	count = m_arrItemShop.GetSize();
	max_line = count / 2 + (count % 2);
	m_pShopScrollBar->SetMaxLineCount(max_line);
	m_pShopScrollBar->SetViewLineCount(5);
	m_pShopScrollBar->SetStartLine(m_pShopScrollBar->GetStartLine());
}

BOOL CDialog_Shop::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
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
	}*/
	m_pFont = new CFont;
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

//	m_pMoneyOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
//	m_pCostOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);

	m_pHandScrollBar = new CUIScrollBar();
	m_pShopScrollBar = new CUIScrollBar();
	m_pHandScrollBar->Create((CUIRes*)this, m_arrArea[24]->m_vRect, 0);
	m_pShopScrollBar->Create((CUIRes*)this, m_arrArea[25]->m_vRect, 1);
		
	return TRUE;
}

void CDialog_Shop::InitData()
{
	m_vCurMeItem = -1;
	m_vCurShopItem = -1;

	m_pHandScrollBar->InitScroll();
	m_pShopScrollBar->InitScroll();
}

void CDialog_Shop::SetScrollData()
{
	int count, max_line;

	count = m_arrItemMe.GetSize();
	max_line = count / 2 + (count%2);
	m_pHandScrollBar->SetMaxLineCount(max_line);
	m_pHandScrollBar->SetViewLineCount(5);

	count = m_arrItemShop.GetSize();
	max_line = count / 2 + (count%2);
	m_pShopScrollBar->SetMaxLineCount(max_line);
	m_pShopScrollBar->SetViewLineCount(5);
}

BOOL CDialog_Shop::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pHandScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}
	if(m_pShopScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}
	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_Shop::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();
	if(m_pHandScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;
	if(m_pShopScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_Shop::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pHandScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;
	if(m_pShopScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;

	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;
	int i;
	
	for(i = 4; i <= 23; i++)
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

void CDialog_Shop::DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index)
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

void CDialog_Shop::ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index)
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
	pDC->DrawText(str, &t_rect, DT_CENTER|DT_TOP|DT_EDITCONTROL|DT_WORDBREAK);
	
	t_rect.OffsetRect(-1, -1);

	pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(str, &t_rect, DT_CENTER|DT_TOP|DT_EDITCONTROL|DT_WORDBREAK);

	pDC->SelectObject(old_font);

	pDest->ReleaseDC(pDC);
}

void CDialog_Shop::CallBack_MessageBox(int nId, int nValue)
{
	CString t_str;

	if(nId == 0)
	{
		if(nValue == 1 && Dialog_HowMany.m_vValue > 0)
		{
			if(m_arrItemMe[m_vCurMeItem]->m_vUsage >= (int)Dialog_HowMany.m_vValue)
			{
				ShopSendSell(Dialog_HowMany.m_vValue);
			}
		}
	}
}
