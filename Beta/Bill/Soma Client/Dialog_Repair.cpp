// Dialog_Repair.cpp: implementation of the CDialog_Repair class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Repair.h"
#include "User.h"
#include "ChildView.h"
#include "DManage/BBDX_MouseInformation.h"
#include "Item_Total.h"
#include "Dialog_Message.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMap<int, int, char *, char *&> g_SoundName;
extern CUIRes *g_pShowDialog;
extern CItem_Total			Item_Total;
extern CDialog_Message		Dialog_Message;

void MovingSoundPlay(int bBelt, short index);

static const char SPECIAL_NAME[][4] = {"Str", "Dex", "Int", "Atk", "Def", "HP", "MP"};

#define ARM_COUNT	10
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Repair::CDialog_Repair()
{
	INIT_PTR(m_pScrollBar);
	INIT_PTR(m_pFont);
	m_vPosMode = POS_LEFT;

	InitData();
}

CDialog_Repair::~CDialog_Repair()
{
	SAFE_DELETE(m_pScrollBar);
	if(m_pFont)
	{
		m_pFont->DeleteObject();
		delete m_pFont;
	}
	DeleteContents();
}

void CDialog_Repair::InitData()
{
	m_vRepairIndex = -1;
	m_vRepairMoney = 0;
	m_vRepairDur = 0;
	m_vCurInvItem = -1;
	m_vDragItem = -1;
	m_vTipIndex = -1;
	m_bProcessing = FALSE;
}

BOOL CDialog_Repair::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// 나가기
			HideWindow();
			break;
		case 1:			// 수리
			if(m_pRepairItem)
			{
				if(m_pRepairItem->m_vUsage >= m_pRepairItem->m_vMaxUsage)
				{
/*					m_mapItem[m_vRepairIndex] = m_pRepairItem;
					m_vRepairIndex = -1;
					m_vRepairMoney = 0;
					m_vRepairDur = 0;
					m_pRepairItem = NULL;*/
					CString t_str;
					t_str.LoadString(IDS_NOMORE_REPAIR);
					Dialog_Message.ShowWindow(255, this);
					Dialog_Message.SetMessage(t_str);
				}
				else
				{
					SendRepairItem();
				}
			}
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

BOOL CDialog_Repair::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CRect rtDst, rtSrc;
	DWORD dwTrans;
	int item_start, item_end;
	int i;
	ITEM_ME *item_me;
	CPoint t_pos;
	CUser *ptmp;
	CString t_str;

	Draw3ButtonCtrl(pDest, 0, 3);

	m_pScrollBar->Draw(pDest);

	ptmp = CChildView::UserManager.GetMe();
	if(ptmp == NULL) return TRUE;

	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;

	// 수리 아이템 그리기
	if(m_pRepairItem)
	{
		t_pos = m_arrArea[23]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_pRepairItem->m_vPicNum].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}
	// 소지 아이템 그리기
	item_start = m_pScrollBar->GetStartLine() * 6 + 10;
	item_end = item_start + 12;
	for(i = item_start; i < item_end; i++)
	{
		if(m_mapItem.Lookup(i, item_me) == TRUE && item_me->m_vType <= TYPE_ACC)
		{
			t_pos = m_arrArea[11+(i-item_start)]->m_vRect.TopLeft();
			t_pos.Offset(m_ptOffset);
			rtSrc = Item_Total.m_Item_Small->m_SprData[item_me->m_vPicNum].rcV;
			pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
		}
	}

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));

	t_str.Format("%lu", ptmp->m_Money);
	rtDst = m_arrArea[26]->m_vRect;		// Money
	rtDst.OffsetRect(m_ptOffset);
	pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	if(m_vTipIndex >= 0)
	{
		item_me = NULL;
		if(m_vTipIndex == 23)
		{
			item_me = m_pRepairItem;
		}
		else if(m_vTipIndex >= 11 && m_vTipIndex <= 22)
		{
			if(m_mapItem.Lookup(item_start+m_vTipIndex-11, item_me) == FALSE) item_me = NULL;
		}
		if(item_me)
		{
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

			if(m_vTipIndex == 23)
			{
				CString tt_str;

				tt_str.Format("\n\n수리후 내구력:%d 수리비용:%d", m_vRepairDur, m_vRepairMoney);
				t_str += tt_str;
			}

			CPoint caption_pos;

			caption_pos.x = m_arrArea[m_vTipIndex]->m_vRect.right + m_ptOffset.x;
			caption_pos.y = m_arrArea[m_vTipIndex]->m_vRect.top + m_ptOffset.y;
			ViewCaption(pDest, caption_pos, t_str, 0);
		}
	}

	return TRUE;
}

BOOL CDialog_Repair::DragDraw(CDDSurface *pDest)
{
	ITEM_ME *item_me;
	CRect rtSrc;
	CPoint t_pos;
	DWORD dwTrans;

	// Drag 아이템 그리기(수리 아이템은 Index가 0)
	if(m_vDragItem >= 0)
	{
		if(m_vDragItem == 0)
		{
			item_me = m_pRepairItem;
		}
		else
		{
			if(m_mapItem.Lookup(m_vDragItem, item_me) == FALSE) item_me = NULL;
		}
		if(item_me)
		{
			dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
			t_pos = CChildView::DirectXMng.GetMouseInfoPtr()->GetPos() - m_vDragOffset;
			rtSrc = Item_Total.m_Item_Small->m_SprData[item_me->m_vPicNum].rcV;
			pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
		}
	}

	return TRUE;
}

void CDialog_Repair::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Repair.lib", 0, FALSE, FALSE, FALSE))
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	CUIRes::ShowWindow();

	InitData();
	m_pScrollBar->InitScroll();
	RefreshData();
}

void CDialog_Repair::HideWindow()
{
	g_pShowDialog = NULL;

	HideAllWindow();
}

void CDialog_Repair::HideAllWindow()
{
	CUIRes::HideWindow();
	DeleteContents();
}

void CDialog_Repair::RefreshData()
{
	int i;
	ITEM_ME *item_me;
	CUser *ptmp;

	// 초기화
	DeleteContents();

	ptmp = CChildView::UserManager.GetMe();
	for(i = ARM_COUNT; i < 50; i++)
	{
		if(ptmp->GetInv_ItemUsage(i) > 0 && ptmp->GetInv_ItemType(i) <= TYPE_ACC)
		{
			item_me = new ITEM_ME;

			item_me->m_vPicNum = ptmp->GetInv_ItemPic(i);
			item_me->m_vType = ptmp->GetInv_ItemType(i);
			item_me->m_vValue = ptmp->m_MyInvData[i].m_vValue;
			item_me->m_vUsage = ptmp->GetInv_ItemUsage(i);
			item_me->m_vMaxUsage = ptmp->m_MyInvData[i].m_vMaxUsage;
			item_me->m_vOrgMaxUsage = ptmp->m_MyInvData[i].m_vOrgMaxUsage;
			item_me->m_vGender = ptmp->m_MyInvData[i].m_vGender;
			item_me->m_vName = ptmp->GetInv_ItemName(i);
			item_me->m_vMinDamage = ptmp->m_MyInvData[i].m_vMinDamage;
			item_me->m_vMaxDamage = ptmp->m_MyInvData[i].m_vMaxDamage;
			item_me->m_vLimitStr = ptmp->m_MyInvData[i].m_vLimitStr;
			item_me->m_vLimitInt = ptmp->m_MyInvData[i].m_vLimitInt;
			item_me->m_vLimitSkill = ptmp->m_MyInvData[i].m_vLimitSkill;
			item_me->m_strMagic = ptmp->m_MyInvData[i].m_strMagic;
			item_me->m_strSNum = ptmp->m_MyInvData[i].m_strSNum;
			for(int k = 0; k < 7; k++) item_me->m_vSpecial[k] = ptmp->m_MyInvData[i].m_vSpecial[k];

			m_mapItem[i] = item_me;
		}
	}
	m_vCurArmItem = m_vCurInvItem = -1;

	SetScrollData();
}

void CDialog_Repair::DeleteContents()
{
	POSITION pos;

	pos = m_mapItem.GetStartPosition();
	while (pos != NULL)
	{
		int dwKey;
		ITEM_ME* item_me;
		m_mapItem.GetNextAssoc(pos, dwKey, item_me);
		delete item_me;
	}
	m_mapItem.RemoveAll();

	if(m_pRepairItem)
	{
		delete m_pRepairItem;
		m_pRepairItem = NULL;
	}
}

void CDialog_Repair::SetScrollData()
{
	int max_line;
	POSITION pos;
	int max_key = ARM_COUNT;

	pos = m_mapItem.GetStartPosition();
	while (pos != NULL)
	{
		int dwKey;
		ITEM_ME* item_me;
		m_mapItem.GetNextAssoc(pos, dwKey, item_me);
		if(max_key < dwKey) max_key = dwKey;
	}

	max_key -= 9;
	max_line = max_key / 6 + ((max_key%6)?1:0);
	m_pScrollBar->SetMaxLineCount(max_line);
	m_pScrollBar->SetViewLineCount(2);
	m_pScrollBar->SetStartLine(m_pScrollBar->GetStartLine());
}

BOOL CDialog_Repair::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}
	
	m_pFont = new CFont;
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[10]->m_vRect, 0);

	return TRUE;
}

BOOL CDialog_Repair::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}

	BOOL retVal = CUIRes::OnLButtonDown(pMouseInfo);
	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

	if(retVal && m_bProcessing == FALSE)
	{
		if(m_vSpecCtrl == AREA_CTRL)
		{
			if(m_vIndexCtrl >= 11 && m_vIndexCtrl <= 23)
			{
				if(m_vIndexCtrl == 23)	// 장착 아이템 선택
				{
					if(m_pRepairItem)
					{
						m_vDragItem = 0;
						m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
						SetCapture();
					}
				}
				else					// 소지 아이템 선택
				{
					int item_index;
					ITEM_ME *item_me;

					item_index = m_pScrollBar->GetStartLine() * 6 + 10;
					item_index = item_index + (m_vIndexCtrl-11);
					if(m_mapItem.Lookup(item_index, item_me) == TRUE)
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

BOOL CDialog_Repair::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	if(m_vDragItem >= 0)
	{
		CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

		if(m_vDragItem == 0)
		{
			if(m_arrArea[25]->m_vRect.PtInRect(m_pos))	// 수리 -> Inv
			{
				m_mapItem[m_vRepairIndex] = m_pRepairItem;
				m_vRepairIndex = -1;
				m_vRepairMoney = 0;
				m_vRepairDur = 0;
				m_pRepairItem = NULL;
			}
		}
		else
		{
			if(m_arrArea[23]->m_vRect.PtInRect(m_pos))		// Inv -> 수리
			{
				ITEM_ME *item_me;

				if(m_mapItem.Lookup(m_vDragItem, item_me))
				{
					if(m_pRepairItem)
					{
						m_mapItem[m_vRepairIndex] = m_pRepairItem;
					}
					m_pRepairItem = item_me;
					m_vRepairIndex = m_vDragItem;
					m_vRepairDur = item_me->m_vMaxUsage - (int)((double)item_me->m_vMaxUsage*0.1);
					m_vRepairMoney = (DWORD)(((double)item_me->m_vMaxUsage-(double)item_me->m_vUsage)/(double)item_me->m_vOrgMaxUsage * (double)item_me->m_vValue * 0.7);
					if(m_vRepairMoney == 0 && item_me->m_vMaxUsage != item_me->m_vUsage) m_vRepairMoney = 1;
					m_mapItem.RemoveKey(m_vDragItem);
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

BOOL CDialog_Repair::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;

	if(m_vDragItem < 0)
	{
		CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;
		int i;

		for(i = 11; i <= 23; i++)
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

BOOL CDialog_Repair::OnRButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CRect t_rect;
	int i, index;
	CPoint t_pos;

	t_pos = pMouseInfo->GetPos() - m_ptOffset;
	index = -1;
	for(i = 11; i <= 23; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(t_pos))
		{
			index = i;
			break;
		}
	}
	if(index == -1) return TRUE;

	if(index == 23)	// 장착 아이템
	{
		if(m_pRepairItem)
		{
			m_mapItem[m_vRepairIndex] = m_pRepairItem;
			m_vRepairIndex = -1;
			m_vRepairMoney = 0;
			m_vRepairDur = 0;
			m_pRepairItem = NULL;
		}
	}
	else			// 소지 아이템
	{
		int item_index;
		ITEM_ME *item_me;
		
		item_index = m_pScrollBar->GetStartLine() * 6 + 10;
		item_index = item_index + (index-11);
		if(m_mapItem.Lookup(item_index, item_me) == TRUE)
		{
			if(m_pRepairItem)
			{
				m_mapItem[m_vRepairIndex] = m_pRepairItem;
			}
			m_pRepairItem = item_me;
			m_vRepairIndex = item_index;
			m_vRepairDur = item_me->m_vMaxUsage - (int)((double)item_me->m_vMaxUsage*0.1);
			m_vRepairMoney = (DWORD)(((double)item_me->m_vMaxUsage-(double)item_me->m_vUsage)/(double)item_me->m_vOrgMaxUsage * (double)item_me->m_vValue * 0.7);
			if(m_vRepairMoney == 0 && item_me->m_vMaxUsage != item_me->m_vUsage) m_vRepairMoney = 1;
			m_mapItem.RemoveKey(item_index);
		}
	}

	return TRUE;
}

BOOL CDialog_Repair::OnLButtonDblClk(BBDX_MouseInformation *pMouseInfo)
{
	CRect t_rect;
	int i, index;
	CPoint t_pos;

	t_pos = pMouseInfo->GetPos() - m_ptOffset;
	index = -1;
	for(i = 11; i <= 23; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(t_pos))
		{
			index = i;
			break;
		}
	}
	if(index == -1) return TRUE;

	if(index == 23)	// 장착 아이템
	{
		if(m_pRepairItem)
		{
			m_mapItem[m_vRepairIndex] = m_pRepairItem;
			m_vRepairIndex = -1;
			m_vRepairMoney = 0;
			m_vRepairDur = 0;
			m_pRepairItem = NULL;
		}
	}
	else			// 소지 아이템
	{
		int item_index;
		ITEM_ME *item_me;
		
		item_index = m_pScrollBar->GetStartLine() * 6 + 10;
		item_index = item_index + (index-11);
		if(m_mapItem.Lookup(item_index, item_me) == TRUE)
		{
			if(m_pRepairItem)
			{
				m_mapItem[m_vRepairIndex] = m_pRepairItem;
			}
			m_pRepairItem = item_me;
			m_vRepairIndex = item_index;
			m_vRepairDur = item_me->m_vMaxUsage - (int)((double)item_me->m_vMaxUsage*0.1);
			m_vRepairMoney = (DWORD)(((double)item_me->m_vMaxUsage-(double)item_me->m_vUsage)/(double)item_me->m_vOrgMaxUsage * (double)item_me->m_vValue * 0.7);
			if(m_vRepairMoney == 0 && item_me->m_vMaxUsage != item_me->m_vUsage) m_vRepairMoney = 1;
			m_mapItem.RemoveKey(item_index);
		}
	}

	return TRUE;
}

int CDialog_Repair::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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

void CDialog_Repair::DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index)
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

void CDialog_Repair::ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index)
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

void CDialog_Repair::SendRepairItem()
{
	BYTE buf[10];
	int offset = 0;

	m_bProcessing = TRUE;
	CPacketParser::SetByte(buf, offset, PKT_REPAIR_REQ); //최종점 요청
	CPacketParser::SetShort(buf, offset, (short)m_vRepairIndex);
	CChildView::SocketMng.Send(offset, buf);
}

#define FAIL_REPAIR			0//수리 실패
#define FAIL_NONEED			1//수리할필요가 없습니다.
#define FAIL_NOREPAIR		2//더이상수리할수 없습니다.
#define FAIL_NOMONEY		3//돈이 부족합니다
#define FAIL_DESTROY		4//아이템이 깨졌습니다.

void CDialog_Repair::RcvRepairItem(PBYTE bypData)
{
	int offset = 0;
	BYTE ret_val;

	ret_val = CPacketParser::GetByte(bypData, offset);
	if(ret_val == 1)		// 성공
	{
		m_vRepairIndex = -1;
		m_vRepairMoney = 0;
		m_vRepairDur = 0;
		m_pRepairItem = NULL;

		CString t_str;

		t_str.LoadString(IDS_SUC_REPAIR);
		Dialog_Message.ShowWindow(255, this);
		Dialog_Message.SetMessage(t_str);
	}
	else					// 실패
	{
		BYTE sub_ret = CPacketParser::GetByte(bypData, offset);

/*		m_mapItem[m_vRepairIndex] = m_pRepairItem;
		m_vRepairIndex = -1;
		m_vRepairMoney = 0;
		m_vRepairDur = 0;
		m_pRepairItem = NULL;*/

		CString t_str;

		switch(sub_ret)
		{
		case FAIL_REPAIR:
			t_str.LoadString(IDS_FAIL_REPAIR);
			break;
		case FAIL_NONEED:
			t_str.LoadString(IDS_NOMORE_REPAIR);
			break;
		case FAIL_NOREPAIR:
			t_str.LoadString(IDS_NO_REPAIR);
			break;
		case FAIL_NOMONEY:
			t_str.LoadString(IDS_NOMONEY_REPAIR);
			break;
		case FAIL_DESTROY:
			t_str.LoadString(IDS_DESTROY_ITEM);
			break;
		default:
			t_str.LoadString(IDS_FAIL_REPAIR);
			break;
		}
		Dialog_Message.ShowWindow(255, this);
		Dialog_Message.SetMessage(t_str);
	}
	m_bProcessing = FALSE;
}