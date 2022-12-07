// Dialog_AuctionSell.cpp: implementation of the CDialog_AuctionSell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_AuctionSell.h"
#include "UIScrollBar.h"
//#include "BBDX_TextOuter.h"
//#include "TWFont.h"
#include "Item_Total.h"
#include "ChildView.h"
#include "User.h"
#include "Dialog_HowMuch.h"
#include "Dialog_HowMany.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CItem_Total			Item_Total;
extern CDialog_HowMuch		Dialog_HowMuch;
extern CDialog_HowMany		Dialog_HowMany;
extern CUIRes *g_pShowDialog;

static const char SPECIAL_NAME[][4] = {"Str", "Dex", "Int", "Atk", "Def", "HP", "MP"};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_AuctionSell::CDialog_AuctionSell()
{
	INIT_PTR(m_pScrollBar);
//	INIT_PTR(m_pTWFont);
//	INIT_PTR(m_pCostOuter);
//	INIT_PTR(m_pMoneyOuter);
	INIT_PTR(m_pAuctionItem);
	INIT_PTR(m_pFont);
	m_vDelItem = -1;
	m_vPosMode = POS_LEFT;
	m_vTipIndex = -1;
	m_vHowMany = 1;
	m_bItemProcess = FALSE;
}

CDialog_AuctionSell::~CDialog_AuctionSell()
{
	DeleteContents();
	SAFE_DELETE(m_pScrollBar);
//	SAFE_DELETE(m_pTWFont);
//	SAFE_DELETE(m_pCostOuter);
//	SAFE_DELETE(m_pMoneyOuter);
	if(m_pFont)
	{
		m_pFont->DeleteObject();
		delete m_pFont;
	}
}

BOOL CDialog_AuctionSell::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// 나가기
			HideWindow();
			break;
		case 1:			// 경매에 보내기
			if(m_pAuctionItem == NULL && m_vCurItem >= 0 && m_bItemProcess == FALSE)
			{
				int count;
				ITEM_DETAIL *item_detail;

				count = m_arrItem.GetSize();
				if(m_vCurItem >= count) return TRUE;
				m_bItemProcess = TRUE;
				item_detail = m_arrItem[m_vCurItem];
				if(item_detail->m_vType >= TYPE_POTION && item_detail->m_vType <= TYPE_TOKEN && item_detail->m_vUsage > 1)
				{
					Dialog_HowMany.ShowWindow(1, this);
				}
				else
				{
					m_vHowMany = 1;
					AskAuctionMoney();
				}
			}
			break;
		case 2:			// 내놓은 경매를 가져오기
			if(m_pAuctionItem && m_bItemProcess == FALSE)
			{
				SendGetAuctionItem();
			}
			break;
		case 3:			// 스크롤업
			m_pScrollBar->ScrollUp();
			break;
		case 4:			// 스크롤다운
			m_pScrollBar->ScrollDown();
			break;
		}
	}
	else if(nSpec == AREA_CTRL)
	{
		if(nIndex >= 0 && nIndex <= 9)
		{
			m_vCurItem = nIndex + m_pScrollBar->GetStartLine() * 2;
			if(m_vCurItem < 0 || m_vCurItem >= m_arrItem.GetSize()) m_vCurItem = -1;
		}
	}
	return TRUE;
}

BOOL CDialog_AuctionSell::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CString t_str;
	DWORD dwTrans;
	CRect rtDst, rtSrc;
	CPoint s_pos;
	int hand_end, hand_start, i, j;
	CUser *ptmp;

	Draw3ButtonCtrl(pDest, 0, 4);

	m_pScrollBar->Draw(pDest);

	hand_end = m_arrItem.GetSize();
	hand_start = m_pScrollBar->GetStartLine() * 2;
	if(hand_end > hand_start+10) hand_end = hand_start+10;
	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	for(i = hand_start, j = 0; i < hand_end; i++, j++)
	{
		s_pos = m_arrArea[j]->m_vRect.TopLeft();
		s_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_arrItem[i]->m_vPicNum].rcV;
		pDest->BltFast(s_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	dwTrans = DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY;
	if(m_vCurItem >= 0)
	{
		int index;

		index = m_vCurItem - m_pScrollBar->GetStartLine() * 2;
		if(index >= 0 && index < 10)
		{
			s_pos = m_arrArea[index]->m_vRect.TopLeft();
			s_pos.Offset(m_ptOffset);
			pDest->BltFast(s_pos, m_pDDSurface, &m_arrArea[23]->m_vRect, dwTrans);
		}
	}

	if(m_pAuctionItem)
	{
		// 경매에 내놓은 아이템 그림
		s_pos = m_arrArea[10]->m_vRect.TopLeft();
		s_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_pAuctionItem->m_vPicNum].rcV;
		pDest->BltFast(s_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}
		
	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));

	for(i = hand_start, j = 0; i < hand_end; i++, j++)
	{
		if(m_arrItem[i]->m_vType >= TYPE_POTION && m_arrItem[i]->m_vType <= TYPE_TOKEN)
		{
			rtDst = m_arrArea[j]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", m_arrItem[i]->m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
		}
	}

	if(m_pAuctionItem)
	{
		// 경매에 내놓은 아이템의 개수
		if(m_pAuctionItem->m_vType >= TYPE_POTION && m_pAuctionItem->m_vType <= TYPE_TOKEN)
		{
			rtDst = m_arrArea[10]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", m_pAuctionItem->m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
		}

		// 경매에 내놓은 아이템의 가격
		rtDst = m_arrArea[13]->m_vRect;
		rtDst.OffsetRect(m_ptOffset);
//		m_pCostOuter->DrawTexts(pDest, rtDst);
		pDC->DrawText(m_strCost, &rtDst, DT_TOP|DT_CENTER|DT_SINGLELINE);
	}

	// 소지한 돈
	ptmp = CChildView::UserManager.GetMe();
	t_str.Format("%lu", ptmp->m_Money);
//	m_pMoneyOuter->SetString(t_str);
	rtDst = m_arrArea[12]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
//	m_pMoneyOuter->DrawTexts(pDest, rtDst);
	pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	if(m_vTipIndex >= 0)
	{
		if(m_vTipIndex <= 9 && hand_start+m_vTipIndex < hand_end)
		{
			ITEM_DETAIL *item_detail;

			s_pos.x = m_arrArea[m_vTipIndex]->m_vRect.right + m_ptOffset.x;
			s_pos.y = m_arrArea[m_vTipIndex]->m_vRect.top + m_ptOffset.y;
			item_detail = m_arrItem[hand_start+m_vTipIndex];
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
			ViewCaption(pDest, s_pos, t_str, 14);
		}
		else if(m_vTipIndex == 10 && m_pAuctionItem)
		{
			s_pos.x = m_arrArea[m_vTipIndex]->m_vRect.right + m_ptOffset.x;
			s_pos.y = m_arrArea[m_vTipIndex]->m_vRect.top + m_ptOffset.y;
			if(m_pAuctionItem->m_vType <= TYPE_ARMOR)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				if(m_pAuctionItem->m_vType == TYPE_ARMOR)
				{
					switch(m_pAuctionItem->m_vGender)
					{
					case ITEM_WOMAN:
						t_str.Format("%s(여)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							m_pAuctionItem->m_vName,
							m_pAuctionItem->m_vMinDamage, m_pAuctionItem->m_vMaxDamage, m_pAuctionItem->m_vUsage,
							m_pAuctionItem->m_vLimitStr, m_pAuctionItem->m_vLimitInt);
						break;
					case ITEM_MAN:
						t_str.Format("%s(남)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							m_pAuctionItem->m_vName,
							m_pAuctionItem->m_vMinDamage, m_pAuctionItem->m_vMaxDamage, m_pAuctionItem->m_vUsage,
							m_pAuctionItem->m_vLimitStr, m_pAuctionItem->m_vLimitInt);
						break;
					default:
						t_str.Format("%s\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							m_pAuctionItem->m_vName,
							m_pAuctionItem->m_vMinDamage, m_pAuctionItem->m_vMaxDamage, m_pAuctionItem->m_vUsage,
							m_pAuctionItem->m_vLimitStr, m_pAuctionItem->m_vLimitInt);
						break;
					}
				}
				else if(m_pAuctionItem->m_vType < TYPE_WEAPON)
				{
					switch(m_pAuctionItem->m_vGender)
					{
					case ITEM_WOMAN:
						t_str.Format("%s(여)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							m_pAuctionItem->m_vName,
							m_pAuctionItem->m_vMinDamage, m_pAuctionItem->m_vMaxDamage, m_pAuctionItem->m_vUsage,
							m_pAuctionItem->m_vLimitStr, m_pAuctionItem->m_vLimitInt, m_pAuctionItem->m_vLimitSkill);
						break;
					case ITEM_MAN:
						t_str.Format("%s(남)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							m_pAuctionItem->m_vName,
							m_pAuctionItem->m_vMinDamage, m_pAuctionItem->m_vMaxDamage, m_pAuctionItem->m_vUsage,
							m_pAuctionItem->m_vLimitStr, m_pAuctionItem->m_vLimitInt, m_pAuctionItem->m_vLimitSkill);
						break;
					default:
						t_str.Format("%s\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							m_pAuctionItem->m_vName,
							m_pAuctionItem->m_vMinDamage, m_pAuctionItem->m_vMaxDamage, m_pAuctionItem->m_vUsage,
							m_pAuctionItem->m_vLimitStr, m_pAuctionItem->m_vLimitInt, m_pAuctionItem->m_vLimitSkill);
						break;
					}
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(m_pAuctionItem->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(m_pAuctionItem->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], m_pAuctionItem->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], m_pAuctionItem->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(m_pAuctionItem->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += m_pAuctionItem->m_strMagic;
				}
				if(m_pAuctionItem->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += m_pAuctionItem->m_strSNum;
				}
			}
			else if(m_pAuctionItem->m_vType == TYPE_ACC)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				switch(m_pAuctionItem->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)\n내구력:%d",
						m_pAuctionItem->m_vName,
						m_pAuctionItem->m_vUsage);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)\n내구력:%d",
						m_pAuctionItem->m_vName,
						m_pAuctionItem->m_vUsage);
					break;
				default:
					t_str.Format("%s\n내구력:%d",
						m_pAuctionItem->m_vName,
						m_pAuctionItem->m_vUsage);
					break;
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(m_pAuctionItem->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(m_pAuctionItem->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], m_pAuctionItem->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], m_pAuctionItem->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(m_pAuctionItem->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += m_pAuctionItem->m_strMagic;
				}
				if(m_pAuctionItem->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += m_pAuctionItem->m_strSNum;
				}
			}
			else if(m_pAuctionItem->m_vType == TYPE_POTION || m_pAuctionItem->m_vType == TYPE_FOOD)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				switch(m_pAuctionItem->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)", m_pAuctionItem->m_vName);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)", m_pAuctionItem->m_vName);
					break;
				default:
					t_str.Format("%s", m_pAuctionItem->m_vName);
					break;
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(m_pAuctionItem->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(m_pAuctionItem->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], m_pAuctionItem->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], m_pAuctionItem->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(m_pAuctionItem->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += m_pAuctionItem->m_strMagic;
				}
				if(m_pAuctionItem->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += m_pAuctionItem->m_strSNum;
				}
			}
			else
			{
				switch(m_pAuctionItem->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)", m_pAuctionItem->m_vName);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)", m_pAuctionItem->m_vName);
					break;
				default:
					t_str = m_pAuctionItem->m_vName;
					break;
				}
			}
			ViewCaption(pDest, s_pos, t_str, 14);
		}
	}
	return TRUE;
}

void CDialog_AuctionSell::SetScrollData()
{
	int count;

	count = m_arrItem.GetSize();
	count = count / 2 + count%2;
	m_pScrollBar->SetMaxLineCount(count);
	m_pScrollBar->SetViewLineCount(5);
	m_pScrollBar->SetStartLine(m_pScrollBar->GetStartLine());
}

void CDialog_AuctionSell::RefreshData()
{
	int i, size;
	ITEM_DETAIL *item_me;
	CUser *ptmp;

	// 초기화
	size = m_arrItem.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItem[i];
	}
	m_arrItem.RemoveAll();

	ptmp = CChildView::UserManager.GetMe();
	for(i = 10; i < 50; i++)
	{
		if(ptmp->GetInv_ItemUsage(i) > 0)
		{
			item_me = new ITEM_DETAIL;

			item_me->m_vIndex = i;
			item_me->m_vPicNum = ptmp->GetInv_ItemPic(i);
			item_me->m_vArmPos = ptmp->GetInv_ItemArm(i);
			item_me->m_vName = ptmp->GetInv_ItemName(i);
			item_me->m_vGender = ptmp->m_MyInvData[i].m_vGender;
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

			m_arrItem.Add(item_me);
		}
	}
	m_vCurItem = -1;

	SetScrollData();
}

BOOL CDialog_AuctionSell::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[11]->m_vRect, 0);
	
/*	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		delete m_pTWFont; m_pTWFont=NULL;
		TRACE("####### Dialog AuctionSell Init 실패.. ############ \n");
		return FALSE;
	}
	m_pCostOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pMoneyOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);*/

	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	return TRUE;
}

void CDialog_AuctionSell::InitData()
{
	m_vCurItem = -1;
	m_vDelItem = -1;
	m_pScrollBar->InitScroll();
//	m_pCostOuter->ClearString();
//	m_pMoneyOuter->ClearString();
}

void CDialog_AuctionSell::DeleteContents()
{
	int size, i;

	size = m_arrItem.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItem[i];
	}
	m_arrItem.RemoveAll();

	SAFE_DELETE(m_pAuctionItem);
}

void CDialog_AuctionSell::SetAuctionData(ITEM_AUCTION *auction_item)
{
	SAFE_DELETE(m_pAuctionItem);

	m_pAuctionItem = auction_item;

//	CString t_str;
//	t_str.Format("%lu", m_pAuctionItem->m_vCost);
//	m_pCostOuter->SetString(t_str);
	m_strCost.Format("%lu", m_pAuctionItem->m_vCost);
}

BOOL CDialog_AuctionSell::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}

	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_AuctionSell::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();
	if(m_pScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_AuctionSell::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;

	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;
	int i;
	
	for(i = 0; i <= 9; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
		{
			m_vTipIndex = i;
			return CUIRes::OnMouseMove(pMouseInfo);
		}
	}
	if(m_arrArea[10]->m_vRect.PtInRect(m_pos))
	{
		m_vTipIndex = i;
		return CUIRes::OnMouseMove(pMouseInfo);
	}
	m_vTipIndex = -1;

	return CUIRes::OnMouseMove(pMouseInfo);
}

void CDialog_AuctionSell::AskAuctionMoney()
{
	Dialog_HowMuch.ShowWindow(0, this);
}

void CDialog_AuctionSell::CallBack_MessageBox(int nId, int nValue)
{
	CString t_str;

	if(nId == 0)
	{
		if(nValue == 1 && Dialog_HowMuch.m_vValue > 0)
		{
			SendAuctionItem(Dialog_HowMuch.m_vValue);
		}
	}
	else if(nId == 1)
	{
		if(nValue == 1 && Dialog_HowMany.m_vValue > 0)
		{
			m_vHowMany = Dialog_HowMany.m_vValue;
			AskAuctionMoney();
		}
	}
}

void CDialog_AuctionSell::SendAuctionItem(DWORD money)
{
	BYTE buf[10];
	int offset = 0;

	m_vDelItem = m_vCurItem;
	CPacketParser::SetByte(buf, offset, PKT_AUCTION_SELL_SEND);

	CPacketParser::SetShort(buf, offset, m_vAuctionNumber);
	CPacketParser::SetShort(buf, offset, m_arrItem[m_vCurItem]->m_vIndex);
	CPacketParser::SetShort(buf, offset, m_vHowMany);
	CPacketParser::SetDword(buf, offset, money);

	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_AuctionSell::RcvAuctionItem(PBYTE bypData)
{
	int offset = 0;
	BYTE result;

	result = CPacketParser::GetByte(bypData, offset);
	if(result == 1)
	{
		BYTE len;
		LPTSTR t_buf;
		ITEM_AUCTION *item_auction;

		item_auction = new ITEM_AUCTION;

		item_auction->m_vCost = CPacketParser::GetDword(bypData, offset);
		item_auction->m_vType = CPacketParser::GetByte(bypData, offset);
		item_auction->m_vGender = CPacketParser::GetByte(bypData, offset);
		item_auction->m_vMinDamage = CPacketParser::GetShort(bypData, offset);
		item_auction->m_vMaxDamage = CPacketParser::GetShort(bypData, offset);
		item_auction->m_vWeight = CPacketParser::GetShort(bypData, offset);
		item_auction->m_vPicNum = CPacketParser::GetShort(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		t_buf = item_auction->m_vName.GetBuffer(len);
		CPacketParser::GetString(bypData, offset, (PBYTE)t_buf, len);
		item_auction->m_vName.ReleaseBuffer();
		item_auction->m_vLimitStr = CPacketParser::GetShort(bypData, offset);
		item_auction->m_vLimitInt = CPacketParser::GetShort(bypData, offset);
		item_auction->m_vLimitSkill = CPacketParser::GetShort(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)item_auction->m_strMagic.GetBuffer(len), len);
			item_auction->m_strMagic.ReleaseBuffer();
		}
		else item_auction->m_strMagic.Empty();
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)item_auction->m_strSNum.GetBuffer(len), len);
			item_auction->m_strSNum.ReleaseBuffer();
		}
		else item_auction->m_strSNum.Empty();
		item_auction->m_vUsage = CPacketParser::GetShort(bypData, offset);
		for(int i = 0; i < 7; i++)
		{
			item_auction->m_vSpecial[i] = CPacketParser::GetShort(bypData, offset);
		}

		SetAuctionData(item_auction);

		short item_count = CPacketParser::GetShort(bypData, offset);
		if(m_vDelItem >= 0)
		{
			CUser *ptmp = CChildView::UserManager.GetMe();
			ptmp->Del_Item(BASIC_INV, m_arrItem[m_vDelItem]->m_vIndex);
			delete m_arrItem[m_vDelItem];
			m_arrItem.RemoveAt(m_vDelItem);
			m_vCurItem = -1;
			m_vDelItem = -1;
			SetScrollData();
		}
	}
	m_bItemProcess = FALSE;
}

void CDialog_AuctionSell::ShowWindow()
{
	if(IsLoading() == FALSE)
	{
		if(Create(CChildView::DirectXMng.m_pDD, "Auct-S.lib", 0, TRUE, FALSE, FALSE) == FALSE)
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	CUIRes::ShowWindow();

	InitData();
	RefreshData();
}

void CDialog_AuctionSell::HideWindow()
{
	g_pShowDialog = NULL;
	HideAllWindow();
}

void CDialog_AuctionSell::HideAllWindow()
{
	m_vTipIndex = -1;
	CUIRes::HideWindow();
	DeleteContents();
}

void CDialog_AuctionSell::AuctionOpen(PBYTE bypData)
{
	int offset = 0;
	BYTE bAuctionItem;

	ShowWindow();

	m_vAuctionNumber = CPacketParser::GetShort(bypData, offset);
	bAuctionItem = CPacketParser::GetByte(bypData, offset);
	if(bAuctionItem == 1)
	{
		BYTE len;
		LPTSTR t_buf;
		ITEM_AUCTION *item_auction;

		item_auction = new ITEM_AUCTION;

		item_auction->m_vCost = CPacketParser::GetDword(bypData, offset);
		item_auction->m_vType = CPacketParser::GetByte(bypData, offset);
		item_auction->m_vGender = CPacketParser::GetByte(bypData, offset);
		item_auction->m_vMinDamage = CPacketParser::GetShort(bypData, offset);
		item_auction->m_vMaxDamage = CPacketParser::GetShort(bypData, offset);
		item_auction->m_vWeight = CPacketParser::GetShort(bypData, offset);
		item_auction->m_vPicNum = CPacketParser::GetShort(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		t_buf = item_auction->m_vName.GetBuffer(len);
		CPacketParser::GetString(bypData, offset, (PBYTE)t_buf, len);
		item_auction->m_vName.ReleaseBuffer();
		item_auction->m_vLimitStr = CPacketParser::GetShort(bypData, offset);
		item_auction->m_vLimitInt = CPacketParser::GetShort(bypData, offset);
		item_auction->m_vLimitSkill = CPacketParser::GetShort(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)item_auction->m_strMagic.GetBuffer(len), len);
			item_auction->m_strMagic.ReleaseBuffer();
		}
		else item_auction->m_strMagic.Empty();
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)item_auction->m_strSNum.GetBuffer(len), len);
			item_auction->m_strSNum.ReleaseBuffer();
		}
		else item_auction->m_strSNum.Empty();
		item_auction->m_vUsage = CPacketParser::GetShort(bypData, offset);
		for(int i = 0; i < 7; i++)
		{
			item_auction->m_vSpecial[i] = CPacketParser::GetShort(bypData, offset);
		}

		SetAuctionData(item_auction);
	}
}

void CDialog_AuctionSell::SendGetAuctionItem()
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_AUCTION_SELL_RCV);

	CPacketParser::SetShort(buf, offset, m_vAuctionNumber);

	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_AuctionSell::RcvGetAuctionItem(PBYTE bypData)
{
	int offset=0;
	CString t_str;
	CUser *ptmp = CChildView::UserManager.GetMe();

	ptmp->Add_Item(bypData);

	DELETE_SAFE(m_pAuctionItem);
	m_strCost.Empty();
	RefreshData();

	m_bItemProcess = FALSE;
}

void CDialog_AuctionSell::DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index)
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

void CDialog_AuctionSell::ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index)
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
	pDC->DrawText(str, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_WORDBREAK);

	pDC->SelectObject(old_font);

	pDest->ReleaseDC(pDC);
}
