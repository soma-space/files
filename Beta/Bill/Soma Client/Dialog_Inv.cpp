// Dialog_Inv.cpp: implementation of the CDialog_Inv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "MainFrm.h"
#include "Dialog_Inv.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "Item_Total.h"
#include "Item_Inv.h"
#include "User.h"
#include "ChildView.h"
#include "Dialog_Main.h"
#include "DarkDef.h"
#include "BBDX_TextOuter.h"
#include "User.h"
#include "TWFont.h"
#include "SoundDef.h"
#include "DManage/DirectSound.h"
#include "Dialog_HowMuch.h"
#include "Dialog_HowMany.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CItem_Total			Item_Total;
//우석.. 포인터로 바꿈... 9월 9일...
extern CItem_Inv			*Item_Inv;
extern CDialog_Main			Dialog_Main;
extern CUIRes *g_pShowDialog;
extern CDialog_HowMuch		Dialog_HowMuch;
extern CDialog_HowMany		Dialog_HowMany;

extern CMap<int, int, char *, char *&> g_SoundName;

void MovingSoundPlay(int bBelt, short index);

static const char SPECIAL_NAME[][4] = {"Str", "Dex", "Int", "Atk", "Def", "HP", "MP"};

void BltBlackTrans(CDDSurface *pSurface, CRect rc);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Inv::CDialog_Inv()
{
	INIT_PTR(m_pScrollBar);
//	INIT_PTR(m_pMoneyOuter);
	INIT_PTR(m_pFont);
//	INIT_PTR(m_pTWFont);
	m_vCurArmItem = m_vCurInvItem = -1;
	m_vDragItem = -1;
	m_vTipIndex = -1;
	for(int i = 0; i < ARM_COUNT; i++)
	{
		m_pArmItem[i] = NULL;
		m_vCurEquipItem[i] = 0;
	}
	m_bChanging = FALSE;
	m_vPosMode = POS_LEFT;
	m_TransParentColor = 0x00;
}

CDialog_Inv::~CDialog_Inv()
{
	SAFE_DELETE(m_pScrollBar);
//	SAFE_DELETE(m_pMoneyOuter);
//	SAFE_DELETE(m_pTWFont);
	if(m_pFont)
	{
		m_pFont->DeleteObject();
		delete m_pFont;
	}
	DeleteContents();
}

BOOL CDialog_Inv::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// 나가기
			HideWindow();
			break;
		case 1:			// 돈 버리기
			Dialog_HowMuch.ShowWindow(0, this);
			break;
		case 2:
			m_pScrollBar->ScrollUp();
			break;
		case 3:
			m_pScrollBar->ScrollDown();
			break;
		}
	}
	return TRUE;
}

static int AREATOITEM[ARM_COUNT] = {1, 0, 3, 2, 6, 7, 8, 9, 4, 5};
static int ITEM_DRAW_POS[ARM_COUNT] = {13, 12, 15, 14, 20, 21, 16, 17, 18, 19};
static int BLANK_ITEM_SRC[ARM_COUNT] = {37, 39, 39, 38, 43, 41, 40, 40, 42, 42};

BOOL CDialog_Inv::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CRect rtDst, rtSrc;
	DWORD dwTrans;
	int item_start, item_end;
	int i;
	ITEM_INV *item_inv;
	CPoint t_pos;
	CUser *ptmp;
	CString t_str;

	Draw3ButtonCtrl(pDest, 0, 3);

	m_pScrollBar->Draw(pDest);

	ptmp = CChildView::UserManager.GetMe();
	if(ptmp == NULL) return TRUE;

	// 장착 아이템 그리기
	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	for(i = 0; i < 7; i++)
	{
		if(m_pArmItem[i])
		{
			t_pos = m_arrArea[ITEM_DRAW_POS[i]]->m_vRect.TopLeft();
			t_pos.Offset(m_ptOffset);
			rtSrc = Item_Total.m_Item_Small->m_SprData[m_pArmItem[i]->m_vPicNum].rcV;
			pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
		}
		else
		{
			t_pos = m_arrArea[ITEM_DRAW_POS[i]]->m_vRect.TopLeft();
			t_pos.Offset(m_ptOffset);
			rtSrc = m_arrArea[BLANK_ITEM_SRC[i]]->m_vRect;
			pDest->BltFast(t_pos, m_pDDSurface, &rtSrc, dwTrans);
		}
	}
	if(m_pArmItem[7])
	{
		t_pos = m_arrArea[ITEM_DRAW_POS[7]]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_pArmItem[7]->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}
	if(m_pArmItem[6] && m_pArmItem[6]->m_vArmPos == ARM_HANDS)
	{
		t_pos = m_arrArea[ITEM_DRAW_POS[7]]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_pArmItem[6]->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
		rtSrc = m_arrArea[9]->m_vRect;
		pDest->BltFast(t_pos, m_pDDSurface, &rtSrc, dwTrans);
		TransParentItem(pDest, t_pos, rtSrc);
	}
	else if(m_pArmItem[7] == NULL)
	{
		t_pos = m_arrArea[ITEM_DRAW_POS[7]]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = m_arrArea[BLANK_ITEM_SRC[7]]->m_vRect;
		pDest->BltFast(t_pos, m_pDDSurface, &rtSrc, dwTrans);
	}
	for(i = 8; i < ARM_COUNT; i++)
	{
		if(m_pArmItem[i])
		{
			t_pos = m_arrArea[ITEM_DRAW_POS[i]]->m_vRect.TopLeft();
			t_pos.Offset(m_ptOffset);
			rtSrc = Item_Total.m_Item_Small->m_SprData[m_pArmItem[i]->m_vPicNum].rcV;
			pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
		}
		else
		{
			t_pos = m_arrArea[ITEM_DRAW_POS[i]]->m_vRect.TopLeft();
			t_pos.Offset(m_ptOffset);
			rtSrc = m_arrArea[BLANK_ITEM_SRC[i]]->m_vRect;
			pDest->BltFast(t_pos, m_pDDSurface, &rtSrc, dwTrans);
		}
	}
	// 소지 아이템 그리기
	item_start = m_pScrollBar->GetStartLine() * 6 + 10;
	item_end = item_start + 12;
	for(i = item_start; i < item_end; i++)
	{
		if(m_mapItem.Lookup(i, item_inv) == TRUE)
		{
			t_pos = m_arrArea[22+(i-item_start)]->m_vRect.TopLeft();
			t_pos.Offset(m_ptOffset);
			rtSrc = Item_Total.m_Item_Small->m_SprData[item_inv->m_vPicNum].rcV;
			pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
		}
	}

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));

	t_str.Format("%lu", ptmp->m_Money);
//	m_pMoneyOuter->SetString(t_str);
	rtDst = m_arrArea[44]->m_vRect;		// Money
	rtDst.OffsetRect(m_ptOffset);
//	m_pMoneyOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);

	for(i = item_start; i < item_end; i++)
	{
		if(m_mapItem.Lookup(i, item_inv) == TRUE && item_inv->m_vType >= TYPE_POTION && item_inv->m_vType <= TYPE_TOKEN)
		{
			rtDst = m_arrArea[22+(i-item_start)]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", item_inv->m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
		}
	}

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	// 장착 캐릭터 그리기
	t_pos.x = m_arrArea[34]->m_vRect.left + m_ptOffset.x - 95 + m_ptOffset.x;
	t_pos.y = m_arrArea[34]->m_vRect.top + m_ptOffset.y - 360 + m_ptOffset.y;
//우석.. 포인터로 바꿈.. 9월 9일....
	Item_Inv->Draw(t_pos.x, t_pos.y);

	if(m_vTipIndex >= 0)
	{
		item_inv = NULL;
		if(m_vTipIndex >= 12 && m_vTipIndex <= 21 && m_pArmItem[AREATOITEM[m_vTipIndex-12]])
		{
			item_inv = m_pArmItem[AREATOITEM[m_vTipIndex-12]];
		}
		else if(m_vTipIndex >= 22 && m_vTipIndex <= 33)
		{
			if(m_mapItem.Lookup(item_start+m_vTipIndex-22, item_inv) == FALSE) item_inv = NULL;
		}
		if(item_inv)
		{
			if(item_inv->m_vType <= TYPE_ARMOR)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;
				
				if(item_inv->m_vType == TYPE_ARMOR)
				{
					switch(item_inv->m_vGender)
					{
					case ITEM_WOMAN:
						t_str.Format("%s(여)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_inv->m_vName,
							item_inv->m_vMinDamage, item_inv->m_vMaxDamage, item_inv->m_vUsage,
							item_inv->m_vLimitStr, item_inv->m_vLimitInt);
						break;
					case ITEM_MAN:
						t_str.Format("%s(남)\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_inv->m_vName,
							item_inv->m_vMinDamage, item_inv->m_vMaxDamage, item_inv->m_vUsage,
							item_inv->m_vLimitStr, item_inv->m_vLimitInt);
						break;
					default:
						t_str.Format("%s\n방어력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d",
							item_inv->m_vName,
							item_inv->m_vMinDamage, item_inv->m_vMaxDamage, item_inv->m_vUsage,
							item_inv->m_vLimitStr, item_inv->m_vLimitInt);
						break;
					}
				}
				else if(item_inv->m_vType < TYPE_WEAPON)
				{
					switch(item_inv->m_vGender)
					{
					case ITEM_WOMAN:
						t_str.Format("%s(여)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_inv->m_vName,
							item_inv->m_vMinDamage, item_inv->m_vMaxDamage, item_inv->m_vUsage,
							item_inv->m_vLimitStr, item_inv->m_vLimitInt, item_inv->m_vLimitSkill);
						break;
					case ITEM_MAN:
						t_str.Format("%s(남)\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_inv->m_vName,
							item_inv->m_vMinDamage, item_inv->m_vMaxDamage, item_inv->m_vUsage,
							item_inv->m_vLimitStr, item_inv->m_vLimitInt, item_inv->m_vLimitSkill);
						break;
					default:
						t_str.Format("%s\n위력:%d~%d 내구력:%d\n필요Str:%d 필요Int:%d 필요숙련도:%d",
							item_inv->m_vName,
							item_inv->m_vMinDamage, item_inv->m_vMaxDamage, item_inv->m_vUsage,
							item_inv->m_vLimitStr, item_inv->m_vLimitInt, item_inv->m_vLimitSkill);
						break;
					}
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_inv->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_inv->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_inv->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_inv->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_inv->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_inv->m_strMagic;
				}
				if(item_inv->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_inv->m_strSNum;
				}
			}
			else if(item_inv->m_vType == TYPE_ACC)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				switch(item_inv->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)\n내구력:%d",
						item_inv->m_vName,
						item_inv->m_vUsage);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)\n내구력:%d",
						item_inv->m_vName,
						item_inv->m_vUsage);
					break;
				default:
					t_str.Format("%s\n내구력:%d",
						item_inv->m_vName,
						item_inv->m_vUsage);
					break;
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_inv->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_inv->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_inv->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_inv->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_inv->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_inv->m_strMagic;
				}
				if(item_inv->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_inv->m_strSNum;
				}
			}
			else if(item_inv->m_vType == TYPE_POTION || item_inv->m_vType == TYPE_FOOD)
			{
				CString tt_str, tt_str1;
				BOOL btrue = FALSE;

				switch(item_inv->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)", item_inv->m_vName);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)", item_inv->m_vName);
					break;
				default:
					t_str.Format("%s", item_inv->m_vName);
					break;
				}
				tt_str = "\n";
				for(int k = 0; k < 7; k++)
				{
					if(item_inv->m_vSpecial[k] != 0)
					{
						if(btrue) tt_str += _T(",");
						if(item_inv->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], item_inv->m_vSpecial[k]);
						else tt_str1.Format("%s%d", SPECIAL_NAME[k], item_inv->m_vSpecial[k]);
						tt_str += tt_str1;
						btrue = TRUE;
					}
				}
				if(btrue == TRUE)
				{
					t_str += tt_str;
				}
				if(item_inv->m_strMagic.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_inv->m_strMagic;
				}
				if(item_inv->m_strSNum.IsEmpty() == FALSE)
				{
					t_str += "\n";
					t_str += item_inv->m_strSNum;
				}
			}
			else
			{
				switch(item_inv->m_vGender)
				{
				case ITEM_WOMAN:
					t_str.Format("%s(여)", item_inv->m_vName);
					break;
				case ITEM_MAN:
					t_str.Format("%s(남)", item_inv->m_vName);
					break;
				default:
					t_str = item_inv->m_vName;
					break;
				}
			}

			CPoint caption_pos;

			caption_pos.x = m_arrArea[m_vTipIndex]->m_vRect.right + m_ptOffset.x;
			caption_pos.y = m_arrArea[m_vTipIndex]->m_vRect.top + m_ptOffset.y;
			ViewCaption(pDest, caption_pos, t_str, 0);
		}
	}

	return TRUE;
}

BOOL CDialog_Inv::DragDraw(CDDSurface *pDest)
{
	ITEM_INV *item_inv;
	CRect rtSrc;
	CPoint t_pos;
	DWORD dwTrans;

	// Drag 아이템 그리기
	if(m_vDragItem >= 0)
	{
		if(m_vDragItem < 10)
		{
			item_inv = m_pArmItem[m_vDragItem];
		}
		else
		{
			if(m_mapItem.Lookup(m_vDragItem, item_inv) == FALSE) item_inv = NULL;
		}
		if(item_inv)
		{
			dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
			t_pos = CChildView::DirectXMng.GetMouseInfoPtr()->GetPos() - m_vDragOffset;
			rtSrc = Item_Total.m_Item_Small->m_SprData[item_inv->m_vPicNum].rcV;
			pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
		}
	}

	return TRUE;
}

void CDialog_Inv::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Invent.lib", 0, FALSE, FALSE, FALSE))
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	CUIRes::ShowWindow();
	m_pScrollBar->InitScroll();
	RefreshData();
}

void CDialog_Inv::HideWindow()
{
	g_pShowDialog = NULL;

	HideAllWindow();
}

void CDialog_Inv::HideAllWindow()
{
	m_vTipIndex = -1;
	CUIRes::HideWindow();
	DeleteContents();
}

void CDialog_Inv::RefreshData()
{
	int i;
	ITEM_INV *item_inv;
	CUser *ptmp;

	// 초기화
	DeleteContents();

	ptmp = CChildView::UserManager.GetMe();
	for(i = 0; i < ARM_COUNT; i++)
	{
		if(ptmp->GetInv_ItemUsage(i) > 0)
		{
			item_inv = new ITEM_INV;

			item_inv->m_vPicNum = ptmp->GetInv_ItemPic(i);
			item_inv->m_vType = ptmp->GetInv_ItemType(i);
			item_inv->m_vUsage = ptmp->GetInv_ItemUsage(i);
			item_inv->m_vGender = ptmp->m_MyInvData[i].m_vGender;
			item_inv->m_vName = ptmp->GetInv_ItemName(i);
			item_inv->m_vArmPos = ptmp->GetInv_ItemArm(i);
			item_inv->m_vMinDamage = ptmp->m_MyInvData[i].m_vMinDamage;
			item_inv->m_vMaxDamage = ptmp->m_MyInvData[i].m_vMaxDamage;
			item_inv->m_vLimitStr = ptmp->m_MyInvData[i].m_vLimitStr;
			item_inv->m_vLimitInt = ptmp->m_MyInvData[i].m_vLimitInt;
			item_inv->m_vLimitSkill = ptmp->m_MyInvData[i].m_vLimitSkill;
			item_inv->m_strMagic = ptmp->m_MyInvData[i].m_strMagic;
			item_inv->m_strSNum = ptmp->m_MyInvData[i].m_strSNum;
			for(int k = 0; k < 7; k++) item_inv->m_vSpecial[k] = ptmp->m_MyInvData[i].m_vSpecial[k];

			m_pArmItem[i] = item_inv;
			int index = __GetMainWnd()->m_wndView.GetItemPicIndex(item_inv->m_vPicNum);
			if(m_vCurEquipItem[i] != index)
			{
				m_vCurEquipItem[i] = index;
//우석.. 포인터로 바꿈.. 9월 9일..
				Item_Inv->Change_InvItem(i, index, item_inv->m_vPicNum);
			}
		}
		else
		{
			m_vCurEquipItem[i] = 0;
			Item_Inv->Change_InvItem(i, 0, 0);
		}
	}
	for(i = ARM_COUNT; i < 50; i++)
	{
		if(ptmp->GetInv_ItemUsage(i) > 0)
		{
			item_inv = new ITEM_INV;

			item_inv->m_vPicNum = ptmp->GetInv_ItemPic(i);
			item_inv->m_vType = ptmp->GetInv_ItemType(i);
			item_inv->m_vUsage = ptmp->GetInv_ItemUsage(i);
			item_inv->m_vGender = ptmp->m_MyInvData[i].m_vGender;
			item_inv->m_vName = ptmp->GetInv_ItemName(i);
			item_inv->m_vArmPos = ptmp->GetInv_ItemArm(i);
			item_inv->m_vMinDamage = ptmp->m_MyInvData[i].m_vMinDamage;
			item_inv->m_vMaxDamage = ptmp->m_MyInvData[i].m_vMaxDamage;
			item_inv->m_vLimitStr = ptmp->m_MyInvData[i].m_vLimitStr;
			item_inv->m_vLimitInt = ptmp->m_MyInvData[i].m_vLimitInt;
			item_inv->m_vLimitSkill = ptmp->m_MyInvData[i].m_vLimitSkill;
			item_inv->m_strMagic = ptmp->m_MyInvData[i].m_strMagic;
			item_inv->m_strSNum = ptmp->m_MyInvData[i].m_strSNum;
			for(int k = 0; k < 7; k++) item_inv->m_vSpecial[k] = ptmp->m_MyInvData[i].m_vSpecial[k];

			m_mapItem[i] = item_inv;
		}
	}
	m_vCurArmItem = m_vCurInvItem = -1;

	SetScrollData();
}

void CDialog_Inv::DeleteContents()
{
	POSITION pos;

	pos = m_mapItem.GetStartPosition();
	while (pos != NULL)
	{
		int dwKey;
		ITEM_INV* item_inv;
		m_mapItem.GetNextAssoc(pos, dwKey, item_inv);
		delete item_inv;
	}
	m_mapItem.RemoveAll();

	for(int i = 0; i < ARM_COUNT; i++)
	{
		if(m_pArmItem[i])
		{
			delete m_pArmItem[i];
			m_pArmItem[i] = NULL;
		}
	}
}

void CDialog_Inv::SetScrollData()
{
	int max_line;
	POSITION pos;
	int max_key = ARM_COUNT;

	pos = m_mapItem.GetStartPosition();
	while (pos != NULL)
	{
		int dwKey;
		ITEM_INV* item_inv;
		m_mapItem.GetNextAssoc(pos, dwKey, item_inv);
		if(max_key < dwKey) max_key = dwKey;
	}

	max_key -= 9;
	max_line = max_key / 6 + ((max_key%6)?1:0);
	m_pScrollBar->SetMaxLineCount(max_line);
	m_pScrollBar->SetViewLineCount(2);
	m_pScrollBar->SetStartLine(m_pScrollBar->GetStartLine());
}

BOOL CDialog_Inv::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
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

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[11]->m_vRect, 0);

	return TRUE;
}

BOOL CDialog_Inv::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}

	BOOL retVal = CUIRes::OnLButtonDown(pMouseInfo);
	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

	if(retVal && m_bChanging == FALSE)
	{
		if(m_vSpecCtrl == AREA_CTRL)
		{
			if(m_vIndexCtrl >= 12 && m_vIndexCtrl <= 33)
			{
				if(m_vIndexCtrl < 22)	// 장착 아이템 선택
				{
					for(int i = 0; i < ARM_COUNT; i++)
					{
						if(ITEM_DRAW_POS[i] == m_vIndexCtrl)
						{
							if(m_pArmItem[i])
							{
								m_vDragItem = i;
								m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
								SetCapture();
							}
							break;
						}
					}
				}
				else					// 소지 아이템 선택
				{
					int item_index;
					ITEM_INV *item_inv;

					item_index = m_pScrollBar->GetStartLine() * 6 + 10;
					item_index = item_index + (m_vIndexCtrl-22);
					if(m_mapItem.Lookup(item_index, item_inv) == TRUE)
					{
						m_vDragItem = item_index;
						m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
						SetCapture();
					}
				}
			}
		}
	}

	return retVal;
}

BOOL CDialog_Inv::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	if(m_vDragItem >= 0)
	{
		CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

		if(m_vDragItem < 10)
		{
			if(m_arrArea[36]->m_vRect.PtInRect(m_pos))
			{
				SendChangeItem(m_vDragItem);
			}
			else if(!PtInUIRes(pMouseInfo->GetPos()))
			{
				SendThrowItem(m_vDragItem, 1);
			}
		}
		else
		{
			CPoint t_pos = pMouseInfo->GetPos() - Dialog_Main.m_ptOffset;
			if(m_arrArea[35]->m_vRect.PtInRect(m_pos))		// 무기, 방어구 장착
			{
				CUser *ptmp;

				ptmp = CChildView::UserManager.GetMe();
				if(ptmp && (ptmp->m_MyInvData[m_vDragItem].m_vType == TYPE_POTION || ptmp->m_MyInvData[m_vDragItem].m_vType == TYPE_FOOD))
				{
					SendUseItem(m_vDragItem);
				}
				else
				{
					if(ptmp->m_MyInvData[m_vDragItem].m_vType < TYPE_WEAPON && ptmp->m_MyInvData[m_vDragItem].m_vArmPos == ARM_HANDS)
					{
						if(m_pArmItem[6] == NULL || m_pArmItem[7] == NULL)
						{
							SendChangeItem(m_vDragItem);
						}
					}
					else
					{
						SendChangeItem(m_vDragItem);
					}
				}
			}
			else if(Dialog_Main.PtInUIRes(pMouseInfo->GetPos()))		// 벨트에 장착
			{
				int i;
				CUser *ptmp;

				ptmp = CChildView::UserManager.GetMe();
				if(ptmp && (ptmp->m_MyInvData[m_vDragItem].m_vType == TYPE_POTION || ptmp->m_MyInvData[m_vDragItem].m_vType == TYPE_FOOD))
				{
					for(i = 6; i <= 9; i++)
					{
						if(Dialog_Main.m_arrArea[i]->m_vRect.PtInRect(t_pos))
						{
							SendItemToBelt(m_vDragItem, i-6);
						}
					}
				}
			}
			else if(!PtInUIRes(pMouseInfo->GetPos()))
			{
				ITEM_INV *item_inv;

				if(m_mapItem.Lookup(m_vDragItem, item_inv))
				{
					if(item_inv->m_vType >= TYPE_POTION && item_inv->m_vType <= TYPE_TOKEN && item_inv->m_vUsage > 1)
					{
						Dialog_HowMany.ShowWindow(1, this);
						ReleaseCapture();
						return FALSE;
					}
					else
					{
						SendThrowItem(m_vDragItem, 1);
					}
				}
			}
			else
			{
				CUser *ptmp = CChildView::UserManager.GetMe();
				BYTE arm = ptmp->m_MyInvData[m_vDragItem].m_vArmPos;
				if(arm == ARM_HANDS)
				{
					if(m_arrArea[ITEM_DRAW_POS[6]]->m_vRect.PtInRect(m_pos))
					{
						SendChangeItem(m_vDragItem, 6);
					}
				}
				else if(arm == ARM_WRIST)
				{
					if(m_arrArea[ITEM_DRAW_POS[8]]->m_vRect.PtInRect(m_pos))
					{
						SendChangeItem(m_vDragItem, 8);
					}
					if(m_arrArea[ITEM_DRAW_POS[9]]->m_vRect.PtInRect(m_pos))
					{
						SendChangeItem(m_vDragItem, 9);
					}
				}
				else
				{
					if(m_arrArea[ITEM_DRAW_POS[arm]]->m_vRect.PtInRect(m_pos))
					{
						SendChangeItem(m_vDragItem, arm);
					}
				}
			}
		}
		m_vDragItem = -1;
	}
	ReleaseCapture();
	if(m_pScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_Inv::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;

	if(m_vDragItem < 0)
	{
		CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;
		int i;

		for(i = 12; i <= 33; i++)
		{
			if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
			{
				m_vTipIndex = i;
				return CUIRes::OnMouseMove(pMouseInfo);
			}
		}
		m_vTipIndex = -1;
	}
	else m_vTipIndex = -1;

	return CUIRes::OnMouseMove(pMouseInfo);
}

BOOL CDialog_Inv::OnRButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CRect t_rect;
	int i, index;
	CPoint t_pos;

	t_pos = pMouseInfo->GetPos() - m_ptOffset;
	index = -1;
	for(i = 12; i <= 33; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(t_pos))
		{
			index = i;
			break;
		}
	}
	if(index == -1) return TRUE;

	if(index < 22)	// 장착 아이템
	{
		for(i = 0; i < ARM_COUNT; i++)
		{
			if(ITEM_DRAW_POS[i] == index)
			{
				if(m_pArmItem[i])
				{
					SendChangeItem(i);
				}
				break;
			}
		}
	}
	else			// 소지 아이템
	{
		int item_index;
		ITEM_INV *item_inv;
		
		item_index = m_pScrollBar->GetStartLine() * 6 + 10;
		item_index = item_index + (index-22);
		if(m_mapItem.Lookup(item_index, item_inv) == TRUE)
		{
			if(item_inv->m_vArmPos == ARM_USE) // 소비 아이템
			{
				SendUseItem(item_index);
			}
			else if(item_inv->m_vArmPos >= ARM_HEAD && item_inv->m_vArmPos <= ARM_WRIST)
			{
				SendChangeItem(item_index);
			}
		}
	}

	return TRUE;
}

int CDialog_Inv::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	if(m_bShow && m_pChild == NULL)
	{
		if(GetCaptureStatus() || PtInUIRes(pMouseInfo->GetPos()))
		{
			if(pMouseInfo->IsLButtonDobleDown())
			{
				OnLButtonDblClk(pMouseInfo);
			}
			if(pMouseInfo->IsLButtonDown()) {
				OnLButtonDown(pMouseInfo);
			}
			if(pMouseInfo->IsMouseMove()) {
				OnMouseMove(pMouseInfo);
			}
			if(pMouseInfo->IsLButtonUp()) {
				OnLButtonUp(pMouseInfo);
			}
			if(pMouseInfo->IsRButtonDown()) {
				OnRButtonDown(pMouseInfo);
			}
			return 2;
		}
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

void CDialog_Inv::ChangeItem(short fromindex, short toindex)
{
	ITEM_INV *item_inv1, *item_inv2;

	if(fromindex >= 10)
	{
		if(toindex >= 10)
		{
			if(m_mapItem.Lookup(fromindex, item_inv1))
			{
				if(m_mapItem.Lookup(toindex, item_inv2))
				{
					m_mapItem[fromindex] = item_inv2;
					m_mapItem[toindex] = item_inv1;
					MovingSoundPlay(BASIC_INV, toindex);
				}
				else
				{
					m_mapItem.RemoveKey(fromindex);
					m_mapItem[toindex] = item_inv1;
					MovingSoundPlay(BASIC_INV, toindex);
				}
			}
		}
		else
		{
			if(m_mapItem.Lookup(fromindex, item_inv1))
			{
				if(item_inv2 = m_pArmItem[toindex])
				{
					m_mapItem[fromindex] = item_inv2;
					m_pArmItem[toindex] = item_inv1;
					MovingSoundPlay(BASIC_INV, toindex);

					int index = __GetMainWnd()->m_wndView.GetItemPicIndex(item_inv1->m_vPicNum);
					if(m_vCurEquipItem[toindex] != index)
					{
						m_vCurEquipItem[toindex] = index;
//우석.. 포인터로 바꿈... 9월 9일...
						Item_Inv->Change_InvItem(toindex, index, item_inv1->m_vPicNum);
					}
				}
				else
				{
					m_mapItem.RemoveKey(fromindex);
					m_pArmItem[toindex] = item_inv1;
					MovingSoundPlay(BASIC_INV, toindex);

					int index = __GetMainWnd()->m_wndView.GetItemPicIndex(item_inv1->m_vPicNum);
					if(m_vCurEquipItem[toindex] != index)
					{
						m_vCurEquipItem[toindex] = index;
//우석.. 포인터로 바꿈... 9월 9일...
						Item_Inv->Change_InvItem(toindex, index, item_inv1->m_vPicNum);
					}
				}
			}
		}
	}
	else
	{
		if(toindex >= 10)
		{
			if(item_inv1 = m_pArmItem[fromindex])
			{
				if(m_mapItem.Lookup(toindex, item_inv2))
				{
					m_pArmItem[fromindex] = item_inv2;
					m_mapItem[toindex] = item_inv1;
					MovingSoundPlay(BASIC_INV, toindex);

					int index = __GetMainWnd()->m_wndView.GetItemPicIndex(item_inv2->m_vPicNum);
					if(m_vCurEquipItem[fromindex] != index)
					{
						m_vCurEquipItem[fromindex] = index;
//우석.. 포인터로 바꿈... 9월 9일...
						Item_Inv->Change_InvItem(fromindex, index, item_inv2->m_vPicNum);
					}
				}
				else
				{
					m_pArmItem[fromindex] = NULL;
					m_mapItem[toindex] = item_inv1;
					MovingSoundPlay(BASIC_INV, toindex);

					if(m_vCurEquipItem[fromindex] != 0)
					{
						m_vCurEquipItem[fromindex] = 0;
//우석.. 포인터로 바꿈... 9월 9일...
						Item_Inv->Change_InvItem(fromindex, 0);
					}
				}
			}
		}
		else
		{
			if(item_inv1 = m_pArmItem[fromindex])
			{
				if(item_inv2 = m_pArmItem[toindex])
				{
					m_pArmItem[fromindex] = item_inv2;
					m_pArmItem[toindex] = item_inv1;
					MovingSoundPlay(BASIC_INV, toindex);

					int index = __GetMainWnd()->m_wndView.GetItemPicIndex(item_inv2->m_vPicNum);
					if(m_vCurEquipItem[fromindex] != index)
					{
						m_vCurEquipItem[fromindex] = index;
//우석.. 포인터로 바꿈... 9월 9일...
						Item_Inv->Change_InvItem(fromindex, index, item_inv2->m_vPicNum);
					}
					index = __GetMainWnd()->m_wndView.GetItemPicIndex(item_inv1->m_vPicNum);
					if(m_vCurEquipItem[toindex] != index)
					{
						m_vCurEquipItem[toindex] = index;
//우석.. 포인터로 바꿈... 9월 9일...
						Item_Inv->Change_InvItem(toindex, index, item_inv2->m_vPicNum);
					}
				}
				else
				{
					m_pArmItem[fromindex] = NULL;
					m_pArmItem[toindex] = item_inv1;
					MovingSoundPlay(BASIC_INV, toindex);
					
					if(m_vCurEquipItem[fromindex] != 0)
					{
						m_vCurEquipItem[fromindex] = 0;
//우석.. 포인터로 바꿈... 9월 9일...
						Item_Inv->Change_InvItem(fromindex, 0);
					}
					int index = __GetMainWnd()->m_wndView.GetItemPicIndex(item_inv1->m_vPicNum);
					if(m_vCurEquipItem[toindex] != index)
					{
						m_vCurEquipItem[toindex] = index;
//우석.. 포인터로 바꿈... 9월 9일...
						Item_Inv->Change_InvItem(toindex, index, item_inv1->m_vPicNum);
					}
				}
			}
		}
	}
}

void CDialog_Inv::SendChangeItem(short index)
{
	if(index < 10)
	{
		if(m_pArmItem[index] == NULL) return;
		CChildView::UserManager.SendChang_Item(index, -1);
	}
	else
	{
		CUser *ptmp;
		
		ptmp = CChildView::UserManager.GetMe();
		if(ptmp && (ptmp->m_MyInvData[index].m_vType == TYPE_POTION || ptmp->m_MyInvData[index].m_vType == TYPE_FOOD))
		{
			SendUseItem(index);
		}
		else if(ptmp->m_MyInvData[index].m_vType <= TYPE_ACC)
		{
			BYTE arm = ptmp->m_MyInvData[index].m_vArmPos;
			if(arm == ARM_HANDS) arm = 6;
			else if(arm == ARM_WRIST)
			{
				if(ptmp->m_MyInvData[index].m_vUsage > 0)
				{
					if(ptmp->m_MyInvData[8].m_vUsage > 0) arm = 9;
					else arm = 8;
				}
				else arm = 8;
			}
			CChildView::UserManager.SendChang_Item(index, arm);
		}
	}
}

void CDialog_Inv::SendChangeItem(short from_index, short to_index)
{
	if(from_index < 10)
	{
		if(m_pArmItem[from_index] == NULL) return;
		CChildView::UserManager.SendChang_Item(from_index, -1);
	}
	else
	{
		CUser *ptmp;
		
		ptmp = CChildView::UserManager.GetMe();
		if(ptmp->m_MyInvData[from_index].m_vType <= TYPE_ACC)
		{
			if(to_index >= 0 && to_index < 10)
			{
				CChildView::UserManager.SendChang_Item(from_index, to_index);
			}
		}
	}
}

void CDialog_Inv::SendThrowItem(short index, short count)
{
	CPoint MyPos = CChildView::UserManager.GetMyCellPos();
	
	BYTE buf[10];
	int offset = 0;
	CPacketParser::SetByte(buf, offset, PKT_ITEM_THROW); //최종점 요청
	CPacketParser::SetByte(buf, offset, BASIC_INV);
	CPacketParser::SetShort(buf, offset, (short)index);
	CPacketParser::SetShort(buf, offset, (short)count);
	CPacketParser::SetShort(buf, offset, (short)MyPos.x);
	CPacketParser::SetShort(buf, offset, (short)MyPos.y);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Inv::SendUseItem(short index)
{
	BYTE buf[10];
	int offset = 0;
	CUser *ptmp = CChildView::UserManager.GetMe();
	if(ptmp == NULL) return;

	CPacketParser::SetByte(buf, offset, PKT_ITEM_USE); //최종점 요청
	CPacketParser::SetByte(buf, offset, BASIC_INV); // 인벤토리창에서 사용
	CPacketParser::SetShort(buf, offset, (short)index);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Inv::SendItemToBelt(short fromItem, short toItem)
{
	if(m_bChanging) return;

	m_bChanging = TRUE;
	m_vFromItem = fromItem;
	m_vToItem = toItem;

	BYTE buf[10];
	int offset = 0;
	CPacketParser::SetByte(buf, offset, PKT_PUTITEM_BELT); //최종점 요청
	CPacketParser::SetShort(buf, offset, fromItem);
	CPacketParser::SetShort(buf, offset, toItem);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Inv::RcvItemToBelt(PBYTE bypData)
{
	int offset = 0;

	BYTE result = CPacketParser::GetByte(bypData, offset);
	if(result == 1)
	{
		CUser *ptmp;

		ptmp = CChildView::UserManager.GetMe();
		if(ptmp)
		{
			ptmp->MoveItemToBelt(m_vFromItem, m_vToItem);
			MovingSoundPlay(BELT_INV, m_vToItem);
			RefreshData();
		}
	}
	else if(result == 3)
	{
		CUser *ptmp;

		ptmp = CChildView::UserManager.GetMe();
		if(ptmp)
		{
			ptmp->Del_Item(BASIC_INV, m_vFromItem);
			MovingSoundPlay(BELT_INV, m_vToItem);
			RefreshData();
		}
	}
	m_bChanging = FALSE;
}

void CDialog_Inv::DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index)
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

void CDialog_Inv::ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index)
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
//	BltBlackTrans(pDest, t_rect);

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

void CDialog_Inv::CallBack_MessageBox(int nId, int nValue)
{
	CString t_str;

	if(nId == 0)
	{
		if(nValue == 1 && Dialog_HowMuch.m_vValue > 0)
		{
			CUser *ptmp = CChildView::UserManager.GetMe();
			if(ptmp->m_Money >= Dialog_HowMuch.m_vValue) SendDumpMoney(Dialog_HowMuch.m_vValue);
		}
	}
	else if(nId == 1)
	{
		if(nValue == 1 && Dialog_HowMany.m_vValue > 0)
		{
			ITEM_INV *item_inv;

			if(m_mapItem.Lookup(m_vDragItem, item_inv))
			{
				if((int)Dialog_HowMany.m_vValue <= item_inv->m_vUsage)
				{
					SendThrowItem(m_vDragItem, (short)Dialog_HowMany.m_vValue);
				}
			}
		}
		m_vDragItem = -1;
	}
}

void CDialog_Inv::SendDumpMoney(DWORD money)
{
	CPoint MyPos = CChildView::UserManager.GetMyCellPos();
	
	BYTE buf[10];
	int offset = 0;
	CPacketParser::SetByte(buf, offset, PKT_ITEM_DUMP); //최종점 요청
	CPacketParser::SetDword(buf, offset, money);
	CPacketParser::SetShort(buf, offset, (short)MyPos.x);
	CPacketParser::SetShort(buf, offset, (short)MyPos.y);
	CChildView::SocketMng.Send(offset, buf);
}

void MovingSoundPlay(int bBelt, short index)
{
	char *value;
	CMyInvData *inv_data;
	CUser *ptmp;
	int sound_number;

	ptmp = CChildView::UserManager.GetMe();
	if(ptmp == NULL) return;

	if(bBelt == BASIC_INV) inv_data = &ptmp->m_MyInvData[index];
	else inv_data = &ptmp->m_MyBeltData[index];
	if(inv_data->m_vUsage == 0) return;
		
	if(inv_data->m_vType < TYPE_WEAPON) sound_number = SOUND_INV2;
	else if(inv_data->m_vType == TYPE_ARMOR) sound_number = SOUND_INV1;
	else if(inv_data->m_vType == TYPE_ACC) sound_number = SOUND_INV3;
	else sound_number = SOUND_INV4;

	if(g_SoundName.Lookup(sound_number, value))
	{
		CPoint ptScale = ptmp->GetPosition() - CChildView::MapManager.GetMapStartPos(); 
		CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, sound_number, FALSE, ptScale);
	}
}

BOOL CDialog_Inv::OnLButtonDblClk(BBDX_MouseInformation *pMouseInfo)
{
	CRect t_rect;
	int i, index;
	CPoint t_pos;

	t_pos = pMouseInfo->GetPos() - m_ptOffset;
	index = -1;
	for(i = 12; i <= 33; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(t_pos))
		{
			index = i;
			break;
		}
	}
	if(index == -1) return TRUE;

	if(index < 22)	// 장착 아이템
	{
		for(i = 0; i < ARM_COUNT; i++)
		{
			if(ITEM_DRAW_POS[i] == index)
			{
				if(m_pArmItem[i])
				{
					SendChangeItem(i);
				}
				break;
			}
		}
	}
	else			// 소지 아이템
	{
		int item_index;
		ITEM_INV *item_inv;
		
		item_index = m_pScrollBar->GetStartLine() * 6 + 10;
		item_index = item_index + (index-22);
		if(m_mapItem.Lookup(item_index, item_inv) == TRUE)
		{
			if(item_inv->m_vArmPos == ARM_USE) // 소비 아이템
			{
				SendUseItem(item_index);
			}
			else if(item_inv->m_vArmPos >= ARM_HEAD && item_inv->m_vArmPos <= ARM_WRIST)
			{
				SendChangeItem(item_index);
			}
		}
	}

	return TRUE;
}

void CDialog_Inv::TransParentItem(CDDSurface *pDestSur, CPoint ptDst, CRect rcSrc)
{
	WORD* pSrcOrg;
	int width = rcSrc.right  - rcSrc.left;
	int height= rcSrc.bottom - rcSrc.top;
	m_pDDSurface->Lock();
	DDSURFACEDESC2& ddsdSrc = m_pDDSurface->GetDescription();
	pSrcOrg = (PWORD)((PBYTE)ddsdSrc.lpSurface+rcSrc.left*2+rcSrc.top*ddsdSrc.lPitch);
	int pitchSrc = ddsdSrc.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.

	pDestSur->Lock();
	DDSURFACEDESC2& ddsdDest = pDestSur->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+ptDst.x*2+ptDst.y*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.

	WORD* pDest = pDestOrg;
	WORD* pSrc = pSrcOrg;
	
	for( int j = 0; j < height; j++ )
	{
		WORD* pDest = pDestOrg+pitchDest*j;
		WORD* pSrc = pSrcOrg+pitchSrc*j;
		DWORD color = 0x39e739e7;
		PutPixelHalf(pDest,&color,width,CChildView::DirectXMng.m_dwHalf16Mask,CChildView::DirectXMng.m_dwHalf32Mask); 
	}

	pDestSur->Unlock();
	m_pDDSurface->Unlock();
}

BOOL GetClippedRect(RECT *pRC, RECT* pRCClip)
{
	int ScreenX = CChildView::DirectXMng.GetScreenSize().cx;
	int ScreenY = CChildView::DirectXMng.GetScreenSize().cy;

	BOOL bUseXClip = FALSE;
	*pRCClip = *pRC;

	if(pRC->left < 0)
	{ 
		pRCClip->left = 0;
		bUseXClip = TRUE;
	}
	else if(pRC->right > ScreenX)
	{ 
		pRCClip->right = ScreenX;
		bUseXClip = TRUE;
	}
	if(pRC->top < 0)
	{
		pRCClip->top = 0;
	}
	else if(pRC->bottom > ScreenY)
	{
		pRCClip->bottom = ScreenY;
	}
	return bUseXClip;
}

inline void PutPixelShadowEx(const WORD* pDest, int nCopyCount, DWORD dwHalf16Mask, DWORD dwHalf32Mask) // nAlphaSrc - 0 이 젤 시커멓다.. 32 가 제일 밝다.
{
	__asm
	{
		mov edi, pDest;
		mov ecx, nCopyCount;

		// nCopyCount가 홀수인지 짝수인지를 판단. 
		mov eax, ecx;
		and eax, 1;
		jz	DWORD_HALF_MIX;
	
		mov eax, [edi]; // Dest Spr Pixel
		and eax, dwHalf16Mask;
		shr eax, 1;
		mov [edi], ax;		// ax는 16bit Register.
		
		add edi, 2;
		dec ecx;
		jz  END_LOOP;

		// 여기서부터는 32bit(DWORD)계산.
DWORD_HALF_MIX: 
		shr ecx, 1;
		
COPY_LOOP:
		mov eax, [edi]; // 
		and eax, dwHalf32Mask;
		shr eax, 1;
		mov [edi], eax;

		add edi, 4;
		
		dec ecx;
		jnz COPY_LOOP;
END_LOOP:
	}
}

void BltBlackTrans(CDDSurface *pSurface,CRect rc)
{
	int ScreenX = CChildView::DirectXMng.GetScreenSize().cx;
	int ScreenY = CChildView::DirectXMng.GetScreenSize().cy;

	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return;

	CRect rcClip;
	GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역
	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface);
	WORD* pDest = NULL;
	int width  = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;
	for( int i = 0 ; i < height ; i++ )
	{
		pDest = pDestOrg + rcClip.left + (rcClip.top+i)*pitchDest;
		PutPixelShadowEx(pDest, width, CChildView::DirectXMng.m_dwHalf16Mask, CChildView::DirectXMng.m_dwHalf32Mask);
	}
	pSurface->Unlock();
}
