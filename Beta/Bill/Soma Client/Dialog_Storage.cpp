// Dialog_Storage.cpp: implementation of the CDialog_Storage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Storage.h"
#include "BBDX_Editor.h"
//#include "BBDX_TextOuter.h"
#include "Item_Total.h"
#include "PacketParser.h"
#include "User.h"
//#include "TWFont.h"
#include "Dialog_HowMuch.h"
#include "Dialog_HowMany.h"
#include "ChildView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CItem_Total         Item_Total;
extern CUIRes *g_pShowDialog;
extern CDialog_HowMuch		Dialog_HowMuch;
extern CDialog_HowMany		Dialog_HowMany;
extern CChildView			*g_pChildView;

static const char SPECIAL_NAME[][4] = {"Str", "Dex", "Int", "Atk", "Def", "HP", "MP"};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Storage::CDialog_Storage()
{
//	INIT_PTR(m_pOuter);
	INIT_PTR(m_pHandScrollBar);
	INIT_PTR(m_pStorageScrollBar);
//	INIT_PTR(m_pTWFont);
	INIT_PTR(m_pFont);

	m_vCurMeItem = m_vCurStorageItem = -1;
	m_vTipIndex = -1;
	m_vPosMode = POS_LEFT;
}

CDialog_Storage::~CDialog_Storage()
{
	DeleteContents();

	SAFE_DELETE(m_pHandScrollBar);
	SAFE_DELETE(m_pStorageScrollBar);
//	SAFE_DELETE(m_pTWFont);
//	SAFE_DELETE(m_pOuter);
	if(m_pFont)
	{
		m_pFont->DeleteObject();
		delete m_pFont;
	}
}

void CDialog_Storage::SendToStorage(short count)
{
	BYTE buf[10];
	int offset = 0;
	int t_count;

	t_count = m_arrItemMe.GetSize();
	if(m_vCurMeItem < 0 || m_vCurMeItem >= t_count) return;
	CPacketParser::SetByte(buf, offset, PKT_SAVEITEM);
	TRACE("Send PKT_SAVEITEM\n");
	
	CPacketParser::SetShort(buf, offset, (short)m_arrItemMe[m_vCurMeItem]->m_vIndex);
	CPacketParser::SetShort(buf, offset, count);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Storage::GetFromStorage(short count)
{
	BYTE buf[10];
	int offset = 0;
	int t_count;

	t_count = m_arrItemStorage.GetSize();
	if(m_vCurStorageItem < 0 || m_vCurStorageItem >= t_count) return;
	CPacketParser::SetByte(buf, offset, PKT_TAKEBACKITEM);
	TRACE("Send PKT_TAKEBACKITEM\n");
	
	CPacketParser::SetShort(buf, offset, (short)m_arrItemStorage[m_vCurStorageItem]->m_vIndex);
	CPacketParser::SetShort(buf, offset, count);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Storage::DepositMoney(DWORD money)
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_SAVEMONEY);
	TRACE("Send PKT_SAVEMONEY\n");
	
	CPacketParser::SetDword(buf, offset, money);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Storage::DrawMoney(DWORD money)
{
	BYTE buf[10];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_TAKEBACKMONEY);
	TRACE("Send PKT_TAKEBACKMONEY\n");
	
	CPacketParser::SetDword(buf, offset, money);
	CChildView::SocketMng.Send(offset, buf);
}

BOOL CDialog_Storage::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex) {
		case 0:	// 보관하기
			if(m_vCurMeItem >= 0)
			{
				if(m_arrItemMe[m_vCurMeItem]->m_vType >= TYPE_POTION && m_arrItemMe[m_vCurMeItem]->m_vType <= TYPE_TOKEN && m_arrItemMe[m_vCurMeItem]->m_vUsage > 1)
				{
					Dialog_HowMany.ShowWindow(2, this);
				}
				else
				{
					SendToStorage();
				}
			}
			break;
		case 1:	// 꺼내기
			if(m_vCurStorageItem >= 0)
			{
				if(m_arrItemStorage[m_vCurStorageItem]->m_vType >= TYPE_POTION && m_arrItemStorage[m_vCurStorageItem]->m_vType <= TYPE_TOKEN && m_arrItemStorage[m_vCurStorageItem]->m_vUsage > 1)
				{
					Dialog_HowMany.ShowWindow(3, this);
				}
				else
				{
					GetFromStorage();
				}
			}
			break;
		case 2:	// 입금
			Dialog_HowMuch.ShowWindow(0, this);
			break;
		case 3:	// 출금
			Dialog_HowMuch.ShowWindow(1, this);
			break;
		case 4: // 닫기
			HideWindow();
			break;
		case 5:
			m_pHandScrollBar->ScrollUp();
			break;
		case 6:
			m_pHandScrollBar->ScrollDown();
			break;
		case 7:
			m_pStorageScrollBar->ScrollUp();
			break;
		case 8:
			m_pStorageScrollBar->ScrollDown();
			break;
		}
	}
	else if(nSpec == AREA_CTRL)
	{
		if(nIndex >= 0 && nIndex <= 9)
		{
			m_vCurMeItem = nIndex + m_pHandScrollBar->GetStartLine()*2;
			if(m_vCurMeItem < 0 || m_vCurMeItem >= m_arrItemMe.GetSize()) m_vCurMeItem = -1;
			else m_vCurStorageItem = -1;
		}
		else if(nIndex >= 11 && nIndex <= 20)
		{
			m_vCurStorageItem = nIndex - 11 + m_pStorageScrollBar->GetStartLine()*2;
			if(m_vCurStorageItem < 0 || m_vCurStorageItem >= m_arrItemStorage.GetSize()) m_vCurStorageItem = -1;
			else m_vCurMeItem = -1;
		}
	}

	return TRUE;
}

BOOL CDialog_Storage::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CRect rtDst, rtSrc;
	DWORD dwTrans;
	int hand_end, hand_start, storage_end, storage_start, i, j;
	CUser *ptmp;
	CString t_str;
	CPoint t_pos;
	
	dwTrans = DDBLTFAST_WAIT;

	Draw3ButtonCtrl(pDest, 0, 8);

	ptmp = CChildView::UserManager.GetMe();

	m_pHandScrollBar->Draw(pDest);
	m_pStorageScrollBar->Draw(pDest);

	hand_end = m_arrItemMe.GetSize();
	hand_start = m_pHandScrollBar->GetStartLine() * 2;
	if(hand_end > hand_start+10) hand_end = hand_start+10;
	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	for(i = hand_start, j = 0; i < hand_end; i++, j++)
	{
		t_pos = m_arrArea[j]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_arrItemMe[i]->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	storage_end = m_arrItemStorage.GetSize();
	storage_start = m_pStorageScrollBar->GetStartLine() * 2;
	if(storage_end > storage_start+10) storage_end = storage_start+10;
	for(i = storage_start, j = 0; i < storage_end; i++, j++)
	{
		t_pos = m_arrArea[j+11]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_arrItemStorage[i]->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));

	t_str.Format("%lu", ptmp->m_Money);
//	m_pOuter->SetString(t_str);

	rtDst = m_arrArea[22]->m_vRect;		// My Money
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);

	t_str.Format("%lu", m_vStorageMoney);
//	m_pOuter->SetString(t_str);

	rtDst = m_arrArea[23]->m_vRect;		// Storage Money
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);

	ITEM_ME *item_me;
	ITEM_STORAGE *item_storage;
	for(i = hand_start, j = 0; i < hand_end; i++, j++)
	{
		item_me = m_arrItemMe[i];
		if(item_me->m_vType >= TYPE_POTION && item_me->m_vType <= TYPE_TOKEN)
		{
			rtDst = m_arrArea[j]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", item_me->m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
		}
	}

	for(i = storage_start, j = 0; i < storage_end; i++, j++)
	{
		item_storage = m_arrItemStorage[i];
		if(item_storage->m_vType >= TYPE_POTION && item_storage->m_vType <= TYPE_TOKEN)
		{
			rtDst = m_arrArea[j+11]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", item_storage->m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
		}
	}

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	if(m_vCurMeItem >= hand_start && m_vCurMeItem < hand_start+10)
	{
		pDest->BltFast(m_arrArea[m_vCurMeItem-hand_start]->m_vRect.TopLeft()+m_ptOffset, m_pDDSurface, &m_arrArea[24]->m_vRect, dwTrans);
	}
	else if(m_vCurStorageItem >= storage_start && m_vCurStorageItem < storage_start+10)
	{
		pDest->BltFast(m_arrArea[m_vCurStorageItem-storage_start+11]->m_vRect.TopLeft()+m_ptOffset, m_pDDSurface, &m_arrArea[24]->m_vRect, dwTrans);
	}

	if(m_vTipIndex >= 0)
	{
		CPoint caption_pos;

		caption_pos.x = m_arrArea[m_vTipIndex]->m_vRect.right + m_ptOffset.x;
		caption_pos.y = m_arrArea[m_vTipIndex]->m_vRect.top + m_ptOffset.y;
		if(m_vTipIndex <= 9 && m_vTipIndex+hand_start < hand_end)
		{
			ITEM_ME *item_me = m_arrItemMe[hand_start+m_vTipIndex];
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
			ViewCaption(pDest, caption_pos, t_str, 25);
		}
		else if(m_vTipIndex >= 11 && m_vTipIndex <= 20 && m_vTipIndex+storage_start-11 < storage_end)
		{
			ITEM_STORAGE *item_storage = m_arrItemStorage[storage_start+m_vTipIndex-11];
			if(item_storage->m_vType <= TYPE_ARMOR)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				if(item_storage->m_vType == TYPE_ARMOR)
				{
					switch(item_storage->m_vGender)
					{
					case ITEM_WOMAN:
						t_str.Format("%s(여)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_storage->m_vName,
							item_storage->m_vMinDamage, item_storage->m_vMaxDamage, item_storage->m_vUsage,
							item_storage->m_vLimitStr, item_storage->m_vLimitInt);
						break;
					case ITEM_MAN:
						t_str.Format("%s(남)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_storage->m_vName,
							item_storage->m_vMinDamage, item_storage->m_vMaxDamage, item_storage->m_vUsage,
							item_storage->m_vLimitStr, item_storage->m_vLimitInt);
						break;
					default:
						t_str.Format("%s\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_storage->m_vName,
							item_storage->m_vMinDamage, item_storage->m_vMaxDamage, item_storage->m_vUsage,
							item_storage->m_vLimitStr, item_storage->m_vLimitInt);
						break;
					}
				}
				else if(item_storage->m_vType < TYPE_WEAPON)
				{
					switch(item_storage->m_vGender)
					{
					case ITEM_WOMAN:
						t_str.Format("%s(여)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_storage->m_vName,
							item_storage->m_vMinDamage, item_storage->m_vMaxDamage, item_storage->m_vUsage,
							item_storage->m_vLimitStr, item_storage->m_vLimitInt, item_storage->m_vLimitSkill);
						break;
					case ITEM_MAN:
						t_str.Format("%s(남)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_storage->m_vName,
							item_storage->m_vMinDamage, item_storage->m_vMaxDamage, item_storage->m_vUsage,
							item_storage->m_vLimitStr, item_storage->m_vLimitInt, item_storage->m_vLimitSkill);
						break;
					default:
						t_str.Format("%s\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_storage->m_vName,
							item_storage->m_vMinDamage, item_storage->m_vMaxDamage, item_storage->m_vUsage,
							item_storage->m_vLimitStr, item_storage->m_vLimitInt, item_storage->m_vLimitSkill);
						break;
					}
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_storage->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_storage->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_storage->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_storage->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_storage->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_storage->m_strMagic;
				}
				if(item_storage->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_storage->m_strSNum;
				}
			}
			else if(item_storage->m_vType == TYPE_ACC)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				switch(item_storage->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)\n내구력:%d",
						item_storage->m_vName,
						item_storage->m_vUsage);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)\n내구력:%d",
						item_storage->m_vName,
						item_storage->m_vUsage);
					break;
				default:
					t_str.Format("%s\n내구력:%d",
						item_storage->m_vName,
						item_storage->m_vUsage);
					break;
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_storage->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_storage->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_storage->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_storage->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_storage->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_storage->m_strMagic;
				}
				if(item_storage->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_storage->m_strSNum;
				}
			}
			else if(item_storage->m_vType == TYPE_POTION || item_storage->m_vType == TYPE_FOOD)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				switch(item_storage->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)", item_storage->m_vName);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)", item_storage->m_vName);
					break;
				default:
					t_str.Format("%s", item_storage->m_vName);
					break;
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_storage->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_storage->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_storage->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_storage->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_storage->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_storage->m_strMagic;
				}
				if(item_storage->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_storage->m_strSNum;
				}
			}
			else
			{
				switch(item_storage->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)", item_storage->m_vName);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)", item_storage->m_vName);
					break;
				default:
					t_str = item_storage->m_vName;
					break;
				}
			}
			ViewCaption(pDest, caption_pos, t_str, 25);
		}
	}
	return TRUE;
}

void CDialog_Storage::ShowWindow()
{
	if(!IsLoading())
	{
		SetWnd(g_pChildView);
		if(!Create(CChildView::DirectXMng.m_pDD, "Stash.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	m_vCurMeItem = m_vCurStorageItem = -1;
	CUIRes::ShowWindow();
	RefreshData();
}

void CDialog_Storage::HideWindow()
{
	g_pShowDialog = NULL;

	HideAllWindow();
}

void CDialog_Storage::HideAllWindow()
{
	m_vTipIndex = -1;
	CUIRes::HideWindow();

	DeleteContents();
}

void CDialog_Storage::DeleteContents()
{
	int size, i;

	size = m_arrItemMe.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemMe[i];
	}
	m_arrItemMe.RemoveAll();

	size = m_arrItemStorage.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemStorage[i];
	}
	m_arrItemStorage.RemoveAll();
}

void CDialog_Storage::RefreshData()
{
	int i, size;
	ITEM_ME *item_me;
	CUser *ptmp;
	int count, max_line;

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

void CDialog_Storage::StorageOpen(PBYTE bypData)
{
	int offset, i, size;
	short itemcount;
	BYTE len;
	CString item_name;
	ITEM_STORAGE *item_storage;
	int count, max_line;

	// 초기화
	size = m_arrItemStorage.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemStorage[i];
	}
	m_arrItemStorage.RemoveAll();

	offset = 0;
	m_vStorageMoney = CPacketParser::GetDword(bypData, offset);		// 창고돈
	itemcount = CPacketParser::GetShort(bypData, offset);			// 아이템 개수
	for(i = 0; i < itemcount; i++)
	{
		// data를 집어 넣는다.
		item_storage = new ITEM_STORAGE;

		item_storage->m_vIndex = CPacketParser::GetShort(bypData, offset);		// 창고에서의 아이템 Index
		item_storage->m_vType = CPacketParser::GetByte(bypData, offset);				// 아이템 종류
		item_storage->m_vPicNum = CPacketParser::GetShort(bypData, offset);		// 아이템 그림
		item_storage->m_vGender = CPacketParser::GetByte(bypData, offset);
		item_storage->m_vMinDamage = CPacketParser::GetShort(bypData, offset);		// 최소 위력
		item_storage->m_vMaxDamage = CPacketParser::GetShort(bypData, offset);		// 최대 위력
		item_storage->m_vLimitStr = CPacketParser::GetShort(bypData, offset);
		item_storage->m_vLimitInt = CPacketParser::GetShort(bypData, offset);
		item_storage->m_vLimitSkill = CPacketParser::GetShort(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)item_storage->m_strMagic.GetBuffer(len), len);
			item_storage->m_strMagic.ReleaseBuffer();
		}
		else item_storage->m_strMagic.Empty();
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)item_storage->m_strSNum.GetBuffer(len), len);
			item_storage->m_strSNum.ReleaseBuffer();
		}
		else item_storage->m_strMagic.Empty();
		for(int k = 0; k < 7; k++) item_storage->m_vSpecial[k] = CPacketParser::GetShort(bypData, offset);	// 특수 기능
		item_storage->m_vUsage = CPacketParser::GetShort(bypData, offset);		// 사용횟수...
		len = CPacketParser::GetByte(bypData, offset);				// 아이템 이름 길이
		LPSTR lpStr = item_storage->m_vName.GetBufferSetLength(len);			// 아이템 이름
		CPacketParser::GetString(bypData, offset, (PBYTE)lpStr, len);
		item_storage->m_vName.ReleaseBuffer();

		m_arrItemStorage.Add(item_storage);
	}

	count = m_arrItemStorage.GetSize();
	max_line = count / 2 + (count % 2);
	m_pStorageScrollBar->SetMaxLineCount(max_line);
	m_pStorageScrollBar->SetViewLineCount(5);
	m_pStorageScrollBar->SetStartLine(m_pStorageScrollBar->GetStartLine());
}

void CDialog_Storage::SaveItemToStorage(PBYTE bypData)
{
	int offset;
	short del_item_number, del_count;
	BYTE len;
	CString item_name;
	ITEM_STORAGE *item_storage;
	int count, max_line;
	CUser *ptmp;
	int index;
	BOOL bCreate;

	offset = 0;
	// 아이템 고유번호이다.
	del_item_number = CPacketParser::GetShort(bypData, offset);		// 소지품에서의 지울 아이템 Index
	if(del_item_number == -1) return;								// 실패
	del_count = CPacketParser::GetShort(bypData, offset);			// 지우고 남은 개수

	// 소지품에서 아이템을 지운다.
	ptmp = CChildView::UserManager.GetMe();
	if(del_count == 0)
	{
		ptmp->Del_Item(BASIC_INV, del_item_number);
	}
	else
	{
		ptmp->m_MyInvData[del_item_number].m_vUsage = del_count;
	}
	RefreshData();

	index = CPacketParser::GetShort(bypData, offset);

	bCreate = TRUE;
	count = m_arrItemStorage.GetSize();
	for(int i = 0; i < count; i++)
	{
		if(index == m_arrItemStorage[i]->m_vIndex)
		{
			item_storage = m_arrItemStorage[i];
			bCreate = FALSE;
		}
	}
	// 창고에 아이템을 추가한다.
	if(bCreate == TRUE) item_storage = new ITEM_STORAGE;
	
	item_storage->m_vIndex = index;
	item_storage->m_vType = CPacketParser::GetByte(bypData, offset);		// 아이템 종류
	item_storage->m_vPicNum = CPacketParser::GetShort(bypData, offset);		// 아이템 그림
	item_storage->m_vGender = CPacketParser::GetByte(bypData, offset);
	item_storage->m_vMinDamage = CPacketParser::GetShort(bypData, offset);		// 최소 위력
	item_storage->m_vMaxDamage = CPacketParser::GetShort(bypData, offset);		// 최대 위력
	item_storage->m_vLimitStr = CPacketParser::GetShort(bypData, offset);
	item_storage->m_vLimitInt = CPacketParser::GetShort(bypData, offset);
	item_storage->m_vLimitSkill = CPacketParser::GetShort(bypData, offset);
	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		CPacketParser::GetString(bypData, offset, (PBYTE)item_storage->m_strMagic.GetBuffer(len), len);
		item_storage->m_strMagic.ReleaseBuffer();
	}
	else item_storage->m_strMagic.Empty();
	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		CPacketParser::GetString(bypData, offset, (PBYTE)item_storage->m_strSNum.GetBuffer(len), len);
		item_storage->m_strSNum.ReleaseBuffer();
	}
	else item_storage->m_strMagic.Empty();
	for(int k = 0; k < 7; k++) item_storage->m_vSpecial[k] = CPacketParser::GetShort(bypData, offset);	// 특수 기능
	item_storage->m_vUsage  = CPacketParser::GetShort(bypData, offset);		// 사용횟수...
	len = CPacketParser::GetByte(bypData, offset);				// 아이템 이름 길이
	LPSTR lpStr = item_storage->m_vName.GetBufferSetLength(len);			// 아이템 이름
	CPacketParser::GetString(bypData, offset, (PBYTE)lpStr, len);
	item_storage->m_vName.ReleaseBuffer();

	if(bCreate == TRUE)
	{
		m_arrItemStorage.Add(item_storage);

		count = m_arrItemStorage.GetSize();
		max_line = count / 2 + (count % 2);
		m_pStorageScrollBar->SetMaxLineCount(max_line);
		m_pStorageScrollBar->SetViewLineCount(5);
		m_pStorageScrollBar->SetStartLine(m_pStorageScrollBar->GetStartLine());
	}
	m_vCurMeItem = -1;
}

void CDialog_Storage::TakebackItemFromStorage(PBYTE bypData)
{
	int offset, i;
	BYTE item_arm, item_type, item_gender;
	short del_item_number, del_count, item_index, item_pic, item_min, item_max, item_usage, item_max_usage, item_org_max_usage;
	short limit_str, limit_int, limit_skill;
	short special[7];
	CString magic_name, s_num;
	BYTE len;
	CString item_name;
	int count, max_line;
	DWORD value;
	CUser *ptmp;

	offset = 0;
	// 아이템 고유번호이다.
	del_item_number = CPacketParser::GetShort(bypData, offset);		// 창고에서의 지울 아이템 Index
	if(del_item_number == -1) return;								// 실패
	del_count = CPacketParser::GetShort(bypData, offset);			// 지우고 남은 개수

	item_index = CPacketParser::GetShort(bypData, offset);
	item_type = CPacketParser::GetByte(bypData, offset);
	item_arm = CPacketParser::GetByte(bypData, offset);// 장착위..
	item_pic = CPacketParser::GetShort(bypData, offset);
	item_gender = CPacketParser::GetByte(bypData, offset);
	item_min = CPacketParser::GetShort(bypData, offset);
	item_max = CPacketParser::GetShort(bypData, offset);
	value = CPacketParser::GetDword(bypData, offset);
	limit_str = CPacketParser::GetShort(bypData, offset);
	limit_int = CPacketParser::GetShort(bypData, offset);
	limit_skill = CPacketParser::GetShort(bypData, offset);
	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		CPacketParser::GetString(bypData, offset, (PBYTE)magic_name.GetBuffer(len), len);
		magic_name.ReleaseBuffer();
	}
	else magic_name.Empty();
	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		CPacketParser::GetString(bypData, offset, (PBYTE)s_num.GetBuffer(len), len);
		s_num.ReleaseBuffer();
	}
	else s_num.Empty();
	for(int k = 0; k < 7; k++) special[k] = CPacketParser::GetShort(bypData, offset);
	item_usage = CPacketParser::GetShort(bypData, offset);
	item_max_usage = CPacketParser::GetShort(bypData, offset);
	item_org_max_usage = CPacketParser::GetShort(bypData, offset);
	len = CPacketParser::GetByte(bypData, offset);
	LPSTR lpStr = item_name.GetBufferSetLength((int)len);
	CPacketParser::GetString(bypData, offset, (PBYTE)lpStr, (int)len);
	item_name.ReleaseBuffer();
	
	// 소지품에 아이템을 추가한다.
	ptmp = CChildView::UserManager.GetMe();
	ptmp->Add_Item(item_index, item_type, item_arm, item_pic, item_gender, item_min, item_max, limit_str, limit_int, limit_skill, magic_name, s_num, special, value, item_usage, item_max_usage, item_org_max_usage, item_name);
	RefreshData();

	// 창고에서 아이템을 삭제한다.
	for(i = 0; i < m_arrItemStorage.GetSize(); i++)
	{
		if(m_arrItemStorage[i]->m_vIndex == del_item_number)
		{
			if(del_count == 0)
			{
				delete m_arrItemStorage[i];
				m_arrItemStorage.RemoveAt(i);

				count = m_arrItemStorage.GetSize();
				max_line = count / 2 + (count % 2);
				m_pStorageScrollBar->SetMaxLineCount(max_line);
				m_pStorageScrollBar->SetViewLineCount(5);
				m_pStorageScrollBar->SetStartLine(m_pStorageScrollBar->GetStartLine());
			}
			else
			{
				m_arrItemStorage[i]->m_vUsage = del_count;
			}
		}
	}

	m_vCurStorageItem = -1;
}

void CDialog_Storage::SaveMoney(PBYTE bypData)
{
	int offset;
	CUser *ptmp;
	DWORD money1, money2;

	offset = 0;
	money1 = CPacketParser::GetDword(bypData, offset);		// 소지금
	money2 = CPacketParser::GetDword(bypData, offset);		// 저금

	ptmp = CChildView::UserManager.GetMe();
	ptmp->m_Money = money1;
	m_vStorageMoney = money2;
}

void CDialog_Storage::TakebackMoney(PBYTE bypData)
{
	int offset;
	CUser *ptmp;
	DWORD money1, money2;

	offset = 0;
	money1 = CPacketParser::GetDword(bypData, offset);		// 소지금
	money2 = CPacketParser::GetDword(bypData, offset);		// 저금

	ptmp = CChildView::UserManager.GetMe();
	ptmp->m_Money = money1;
	m_vStorageMoney = money2;
}

BOOL CDialog_Storage::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if(m_pWnd == NULL) return FALSE;

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

//	m_pOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);

	m_pHandScrollBar = new CUIScrollBar();
	m_pStorageScrollBar = new CUIScrollBar();
	m_pHandScrollBar->Create((CUIRes*)this, m_arrArea[10]->m_vRect, 0);
	m_pStorageScrollBar->Create((CUIRes*)this, m_arrArea[21]->m_vRect, 1);
		
	return TRUE;
}

void CDialog_Storage::InitData()
{
	m_pHandScrollBar->InitScroll();
	m_pStorageScrollBar->InitScroll();

	SetScrollData();
}

void CDialog_Storage::SetScrollData()
{
	int count, max_line;

	count = m_arrItemMe.GetSize();
	max_line = count / 2 + (count%2);
	m_pHandScrollBar->SetMaxLineCount(max_line);
	m_pHandScrollBar->SetViewLineCount(5);

	count = m_arrItemStorage.GetSize();
	max_line = count / 2 + (count%2);
	m_pStorageScrollBar->SetMaxLineCount(max_line);
	m_pStorageScrollBar->SetViewLineCount(5);
}

BOOL CDialog_Storage::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pHandScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}
	if(m_pStorageScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}

	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_Storage::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();
	if(m_pHandScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;
	if(m_pStorageScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_Storage::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pHandScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;
	if(m_pStorageScrollBar->OnMouseMove(pMouseInfo) )
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
	for(i = 11; i <= 20; i++)
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

int CDialog_Storage::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	if(m_bShow && m_pChild == NULL)
	{
		if(GetCaptureStatus() || PtInUIRes(pMouseInfo->GetPos()))
		{
			if(pMouseInfo->IsLButtonDown()) {
				OnLButtonDown(pMouseInfo);
			}
			if(pMouseInfo->IsMouseMove()) {
				OnMouseMove(pMouseInfo);
			}
			if(pMouseInfo->IsLButtonUp()) {
				OnLButtonUp(pMouseInfo);
			}
			return 2;
		}
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

void CDialog_Storage::CallBack_MessageBox(int nId, int nValue)
{
	CString t_str;

	if(nId == 0 && nValue == 1)
	{
		CUser *ptmp = CChildView::UserManager.GetMe();
		if(ptmp->m_Money >= Dialog_HowMuch.m_vValue) DepositMoney(Dialog_HowMuch.m_vValue);
	}
	else if(nId == 1 && nValue == 1)
	{
		if(m_vStorageMoney >= Dialog_HowMuch.m_vValue) DrawMoney(Dialog_HowMuch.m_vValue);
	}
	else if(nId == 2 && nValue == 1)
	{
		if(m_arrItemMe[m_vCurMeItem]->m_vType >= TYPE_POTION && m_arrItemMe[m_vCurMeItem]->m_vType <= TYPE_TOKEN && m_arrItemMe[m_vCurMeItem]->m_vUsage > 1)
		{
			if(m_arrItemMe[m_vCurMeItem]->m_vUsage >= (short)Dialog_HowMany.m_vValue)
			{
				SendToStorage((short)Dialog_HowMany.m_vValue);
			}
		}
		else
		{
			SendToStorage();
		}
	}
	else if(nId == 3 && nValue == 1)
	{
		if(m_arrItemStorage[m_vCurStorageItem]->m_vType >= TYPE_POTION && m_arrItemStorage[m_vCurStorageItem]->m_vType <= TYPE_TOKEN && m_arrItemStorage[m_vCurStorageItem]->m_vUsage > 1)
		{
			if(m_arrItemStorage[m_vCurStorageItem]->m_vUsage >= (short)Dialog_HowMany.m_vValue)
			{
				GetFromStorage((short)Dialog_HowMany.m_vValue);
			}
		}
		else
		{
			GetFromStorage();
		}
	}
}

void CDialog_Storage::DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index)
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

void CDialog_Storage::ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index)
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
