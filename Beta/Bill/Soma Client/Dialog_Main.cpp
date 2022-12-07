// Dialog_Main.cpp: implementation of the CDialog_Main class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Main.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "User.h"
#include "ChildView.h"
#include "PacketParser.h"
#include "MyMagicData.h"
#include "Item_Total.h"

#include "Dialog_Status.h"
#include "Dialog_Inv.h"
#include "Dialog_Option.h"
#include "Dialog_MagicCtrl.h"
#include "Dialog_Chat.h"

extern CDialog_Status		Dialog_Status;
extern CDialog_Inv			Dialog_Inv;
extern CDialog_Option		Dialog_Option;
extern CDialog_MagicCtrl	Dialog_MagicCtrl;
extern CDialog_Chat			Dialog_Chat;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CItem_Total			Item_Total;
extern CUIRes				*g_pShowDialog;
extern int					g_vMainMode;

void MovingSoundPlay(int bBelt, short index);

static const char SPECIAL_NAME[][4] = {"Str", "Dex", "Int", "Atk", "Def", "HP", "MP"};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Main::CDialog_Main()
{
	InitData();
	INIT_PTR(m_pFont);
	m_bViewCaption = -1;
	m_vPosMode = POS_NOTEFFECT;
	m_vOldHP = m_vNewHP = m_vCurHP = 0;
	m_vShowHP = 0;
	m_vOldMP = m_vNewMP = m_vCurMP = 0;
	m_vShowMP = 0;
}

CDialog_Main::~CDialog_Main()
{
	if(m_pFont)
	{
		m_pFont->DeleteObject();
		delete m_pFont;
	}
}

void CDialog_Main::InitData()
{
	int i;

	m_bShow = TRUE;
	m_bEnableFocus = FALSE;
	m_vSelMagicIndex = -1;
	m_bChanging = FALSE;
	m_vDragItem = -1;

	for(i = 0; i < 4; i++)
	{
		m_pMagicData[i] = NULL;
	}
}

BOOL CDialog_Main::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// 상태창 띄우기
			if(Dialog_Status.IsVisible()) Dialog_Status.HideWindow();
			else Dialog_Status.ShowWindow();
			break;
		case 1:			// 인벤토리창 띄우기
			if(Dialog_Inv.IsVisible()) Dialog_Inv.HideWindow();
			else Dialog_Inv.ShowWindow();
			break;
		case 2:			// 채팅창 보이기
			if(Dialog_Chat.IsVisible()) Dialog_Chat.HideWindow();
			else Dialog_Chat.ShowWindow();
			break;
		case 3:			// 마법창 띄우기
			if(Dialog_MagicCtrl.IsVisible()) Dialog_MagicCtrl.HideWindow();
			else Dialog_MagicCtrl.ShowWindow();
			break;
		case 4:			// 옵션창 띄우기
			Dialog_Option.ShowWindow();
			break;
		}
	}
	else if(nSpec == BTN_CTRL_EX)
	{
		if(nIndex == 0 || nIndex == 1 || nIndex == 2) // 보통, 전투, PK 모드 전환
		{
			CUser *ptmp = CChildView::UserManager.GetMe();
			if(ptmp == NULL) return TRUE;

			BYTE buf[10];
			int offset = 0;
			BYTE battletype = 0x00;//
			CPacketParser::SetByte(buf, offset, PKT_BATTLEMODE);

			if(ptmp->m_BattleMode == 0)
			{
				battletype = 0x01;
			}
			else if(ptmp->m_BattleMode == 1)
			{
				battletype = 0x02;
			}
			else
			{
				battletype = 0x00;
			}
			CPacketParser::SetByte(buf, offset, battletype);
			CChildView::SocketMng.Send(offset, buf);
		}
	}
	return TRUE;
}

BOOL CDialog_Main::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CUser *ptmp;
	int t_value;
	DWORD dwTrans;
	CRect t_rect;

	Draw3ButtonCtrl(pDest, 0, 4);

	ptmp = CChildView::UserManager.GetMe();
	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;

	Draw4ButtonCtrl(pDest, ptmp->m_BattleMode, ptmp->m_BattleMode);
	// 레벨 표시
	if(ptmp->m_Level < 10)
	{
		pDest->BltFast(m_ptOffset+m_arrArea[40]->m_vRect.TopLeft(), m_pDDSurface, &m_arrArea[ptmp->m_Level+30]->m_vRect, dwTrans);
	}
	else if(ptmp->m_Level < 100)
	{
		pDest->BltFast(m_ptOffset+m_arrArea[41]->m_vRect.TopLeft(), m_pDDSurface, &m_arrArea[ptmp->m_Level/10+30]->m_vRect, dwTrans);
		pDest->BltFast(m_ptOffset+m_arrArea[40]->m_vRect.TopLeft(), m_pDDSurface, &m_arrArea[ptmp->m_Level%10+30]->m_vRect, dwTrans);
	}
	else if(ptmp->m_Level < 1000)
	{
		pDest->BltFast(m_ptOffset+m_arrArea[42]->m_vRect.TopLeft(), m_pDDSurface, &m_arrArea[ptmp->m_Level/100+30]->m_vRect, dwTrans);
		pDest->BltFast(m_ptOffset+m_arrArea[41]->m_vRect.TopLeft(), m_pDDSurface, &m_arrArea[ptmp->m_Level%100/10+30]->m_vRect, dwTrans);
		pDest->BltFast(m_ptOffset+m_arrArea[40]->m_vRect.TopLeft(), m_pDDSurface, &m_arrArea[ptmp->m_Level%10+30]->m_vRect, dwTrans);
	}

	// 성향 표시
	if(ptmp->m_Deposition <= -50) t_value = 18;
	else if(ptmp->m_Deposition >= -49 && ptmp->m_Deposition <= -30) t_value = 17;
	else if(ptmp->m_Deposition >= -29 && ptmp->m_Deposition <= -11) t_value = 16;
	else if(ptmp->m_Deposition >= -10 && ptmp->m_Deposition <= -4) t_value = 15;
	else if(ptmp->m_Deposition >= -3 && ptmp->m_Deposition <= 3) t_value = 14;
	else if(ptmp->m_Deposition >= 4 && ptmp->m_Deposition <= 10) t_value = 13;
	else if(ptmp->m_Deposition >= 11 && ptmp->m_Deposition <= 29) t_value = 12;
	else if(ptmp->m_Deposition >= 30 && ptmp->m_Deposition <= 49) t_value = 11;
	else if(ptmp->m_Deposition >= 50) t_value = 10;
	dwTrans = DDBLTFAST_WAIT;
	pDest->BltFast(m_ptOffset+m_arrArea[5]->m_vRect.TopLeft(), m_pDDSurface, &m_arrArea[t_value]->m_vRect, dwTrans);

	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	// 스태미너 표시
	t_value = m_arrArea[2]->m_vRect.Width();
	t_value = MulDiv(t_value, ptmp->m_Cur_Fa, ptmp->m_Max_Fa);
	t_rect = m_arrArea[23]->m_vRect;
	t_rect.right = t_rect.left + t_value;
	pDest->BltFast(m_ptOffset+m_arrArea[2]->m_vRect.TopLeft(), m_pDDSurface, &t_rect, dwTrans);
	// 무게 표시
	t_value = m_arrArea[3]->m_vRect.Width();
	t_value = MulDiv(t_value, ptmp->m_Cur_Weight, ptmp->m_Max_Weight);
	t_rect = m_arrArea[24]->m_vRect;
	t_rect.right = t_rect.left + t_value;
	pDest->BltFast(m_ptOffset+m_arrArea[3]->m_vRect.TopLeft(), m_pDDSurface, &t_rect, dwTrans);
	// 경험치 표시
	t_value = m_arrArea[4]->m_vRect.Width();
	t_value = MulDiv(t_value, ptmp->m_Cur_exp, ptmp->m_Max_exp);
	t_rect = m_arrArea[25]->m_vRect;
	t_rect.right = t_rect.left + t_value;
	pDest->BltFast(m_ptOffset+m_arrArea[4]->m_vRect.TopLeft(), m_pDDSurface, &t_rect, dwTrans);

	// HP 표시
	DWORD tick_count = GetTickCount();

	t_value = m_arrArea[0]->m_vRect.Width();
/*	if(ptmp->m_Cur_Hp < m_vNewHP)
	{
		m_vCurHP = m_vNewHP = ptmp->m_Cur_Hp;
		m_vShowHP = 0;
	}
	else if(ptmp->m_Cur_Hp > m_vNewHP)
	{
		m_vShowHP = 10.0;
		m_vHPFactor = 0.0;
		m_vNewHP = (double)ptmp->m_Cur_Hp;
		m_vOldHP = m_vCurHP;
		m_vOldHPTickCount = GetTickCount();
	}
	if(m_vShowHP > 0)
	{
		if(tick_count - m_vOldHPTickCount >= 10)
		{
			m_vShowHP -= 1.0;
			m_vHPFactor += m_vShowHP;
			m_vCurHP = m_vOldHP + ((ptmp->m_Cur_Hp-m_vOldHP)*m_vHPFactor/45.0);
			t_value = (int)((double)t_value * m_vCurHP / (double)ptmp->m_Max_Hp + 0.5);
			if(m_vShowHP == 0) m_vOldHP = ptmp->m_Cur_Hp;
			m_vOldHPTickCount = tick_count;
		}
		else t_value = (int)((double)t_value * m_vCurHP / (double)ptmp->m_Max_Hp + 0.5);
	}
	else */t_value = MulDiv(t_value, ptmp->m_Cur_Hp, ptmp->m_Max_Hp);
	if(t_value > m_arrArea[0]->m_vRect.Width()) t_value = m_arrArea[0]->m_vRect.Width();
	t_rect = m_arrArea[20]->m_vRect;
	t_rect.right = t_rect.left + t_value;
	pDest->BltFast(m_ptOffset+m_arrArea[0]->m_vRect.TopLeft(), m_pDDSurface, &t_rect, dwTrans);
	// MP 표시
	t_value = m_arrArea[1]->m_vRect.Width();
/*	if(ptmp->m_Cur_Mp < m_vNewMP)
	{
		m_vCurMP = m_vNewMP = ptmp->m_Cur_Mp;
		m_vShowMP = 0;
	}
	else if(ptmp->m_Cur_Mp > m_vNewMP)
	{
		m_vShowMP = 10.0;
		m_vMPFactor = 0.0;
		m_vNewMP = (double)ptmp->m_Cur_Mp;
		m_vOldMP = m_vCurMP;
		m_vOldMPTickCount = GetTickCount();
	}
	if(m_vShowMP > 0)
	{
		if(tick_count - m_vOldMPTickCount >= 10)
		{
			m_vShowMP -= 1.0;
			m_vMPFactor += m_vShowMP;
			m_vCurMP = m_vOldMP + ((ptmp->m_Cur_Mp-m_vOldMP)*m_vMPFactor/45.0);
			t_value = (int)((double)t_value * m_vCurMP / (double)ptmp->m_Max_Mp + 0.5);
			if(m_vShowMP == 0) m_vOldMP = ptmp->m_Cur_Mp;
			m_vOldMPTickCount = tick_count;
		}
		else t_value = (int)((double)t_value * m_vCurMP / (double)ptmp->m_Max_Mp + 0.5);
	}
	else */t_value = MulDiv(t_value, ptmp->m_Cur_Mp, ptmp->m_Max_Mp);
	if(t_value > m_arrArea[1]->m_vRect.Width()) t_value = m_arrArea[1]->m_vRect.Width();
	t_rect = m_arrArea[21]->m_vRect;
	t_rect.right = t_rect.left + t_value;
	pDest->BltFast(m_ptOffset+m_arrArea[1]->m_vRect.TopLeft(), m_pDDSurface, &t_rect, dwTrans);

	CRect rtDst, rtSrc;
	CPoint t_pos;
	CString t_str;

	dwTrans = DDBLT_WAIT|DDBLT_KEYSRC;
	// 아이템 그리기
	for(int i = 0; i < 4; i++)
	{
		if(ptmp->m_MyBeltData[i].m_vUsage == 0) continue;

		rtDst = m_arrArea[i+6]->m_vRect;
		rtDst.OffsetRect(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[ptmp->m_MyBeltData[i].m_vPicNum].rcV;
		pDest->Blt(&rtDst, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	// 마법 그리기
	for(i = 0; i < 4; i++)
	{
		if(m_pMagicData[i] == NULL) continue;

		t_pos = m_arrArea[i+26]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Magic_Item->m_SprData2[m_pMagicData[i]->m_vNumber-1].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Magic_Item->m_lpSurface2, &rtSrc, dwTrans);
	}

	// 선택 마법 그리기
	if(m_vSelMagicIndex >= 0 && m_vSelMagicIndex <= 4 && m_pMagicData[m_vSelMagicIndex])
	{
		rtDst = m_arrArea[m_vSelMagicIndex+26]->m_vRect;
		rtDst.OffsetRect(m_ptOffset);
		rtSrc = m_arrArea[19]->m_vRect;
		pDest->BltFast(rtDst.TopLeft(), m_pDDSurface, &rtSrc, dwTrans);
	}

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));

	for(i = 0; i < 4; i++)
	{
		if(ptmp->m_MyBeltData[i].m_vUsage == 0) continue;

		rtDst = m_arrArea[i+6]->m_vRect;
		rtDst.OffsetRect(m_ptOffset);
		t_str.Format("%d", ptmp->m_MyBeltData[i].m_vUsage);
		pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
	}

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	if(m_bViewCaption >= 0)
	{
		CRect t_rect = m_arrArea[m_bViewCaption]->m_vRect;

		t_rect.OffsetRect(m_ptOffset);
		ViewCaption(pDest, t_rect, m_vStrCaption, 43);
	}

	return TRUE;
}

BOOL CDialog_Main::DragDraw(CDDSurface *pDest)
{
	DWORD dwTrans;
	CMyInvData *t_inv;
	CPoint t_pos;
	CRect rtSrc;
	CUser *ptmp;

	// Drag 아이템 그리기
	if(m_vDragItem >= 0)
	{
		dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
		if(m_vDragSpec == 1)
		{
			ptmp = CChildView::UserManager.GetMe();
			t_inv = &ptmp->m_MyBeltData[m_vDragItem];
			t_pos = CChildView::DirectXMng.GetMouseInfoPtr()->GetPos() - m_vDragOffset;
			rtSrc = Item_Total.m_Item_Small->m_SprData[t_inv->m_vPicNum].rcV;
			pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
		}
		else if(m_vDragSpec == 2)
		{
			t_pos = CChildView::DirectXMng.GetMouseInfoPtr()->GetPos() - m_vDragOffset;
			rtSrc = Item_Total.m_Magic_Item->m_SprData2[m_pMagicData[m_vDragItem]->m_vNumber-1].rcV;
			pDest->BltFast(t_pos, Item_Total.m_Magic_Item->m_lpSurface2, &rtSrc, dwTrans);
		}
	}

	return TRUE;
}

void CDialog_Main::HideWindow()
{
	m_vDragItem = -1;

	CUIRes::HideWindow();
}

BOOL CDialog_Main::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	BOOL retVal = CUIRes::OnLButtonDown(pMouseInfo);
	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

	if(retVal)
	{
		if(m_vSpecCtrl == AREA_CTRL)
		{
			if(m_vIndexCtrl >= 6 && m_vIndexCtrl <= 9 && m_bChanging == FALSE)
			{
				int index = m_vIndexCtrl - 6;
				CUser *ptmp = CChildView::UserManager.GetMe();
				if(ptmp && ptmp->m_MyBeltData[index].m_vUsage > 0)
				{
					m_vDragSpec = 1;
					m_vDragItem = index;
					m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
					SetCapture();
				}
			}
			else if(m_vIndexCtrl >= 26 && m_vIndexCtrl <= 29)
			{
				int index = m_vIndexCtrl - 26;
				if(m_pMagicData[index])
				{
					m_vLBtnDown = index;
					m_vDragSpec = 2;
					m_vDragItem = index;
					m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
					SetCapture();
				}
			}
		}
	}

	return retVal;
}

BOOL CDialog_Main::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	if(m_vDragItem >= 0)
	{
		if(m_vDragSpec == 1)
		{
			if(Dialog_Inv.IsVisible())
			{
				CPoint t_pos = pMouseInfo->GetPos() - Dialog_Inv.m_ptOffset;
				if(Dialog_Inv.m_arrArea[36]->m_vRect.PtInRect(t_pos))
				{
					int i;

					for(i = 22; i <= 33; i++)
					{
						if(Dialog_Inv.m_arrArea[i]->m_vRect.PtInRect(t_pos))
						{
							int index = i - 22 + Dialog_Inv.m_pScrollBar->GetStartLine() * 6 + 10;
							SendItemToInv(m_vDragItem, index);
						}
					}
				}
				else if(!Dialog_Inv.PtInUIRes(pMouseInfo->GetPos()))
				{
					if(!PtInUIRes(pMouseInfo->GetPos()))
					{
						SendThrowItem(m_vDragItem, 1);
					}
				}
			}
			else
			{
				if(!PtInUIRes(pMouseInfo->GetPos()))
				{
					SendThrowItem(m_vDragItem, 1);
				}
			}
		}
		else if(m_vDragSpec == 2)
		{
			if(!PtInUIRes(pMouseInfo->GetPos()))
			{
				m_pMagicData[m_vDragItem] = NULL;
			}
			else
			{
				if(m_vDragItem == m_vIndexCtrl-26)
				{
					SetSelMagic(m_vDragItem);
				}
			}
		}
		m_vDragItem = -1;
		ReleaseCapture();
	}
	else
	{
		if(m_vLBtnDown == m_vIndexCtrl-26)
		{
			SetSelMagic(m_vLBtnDown);
		}
	}

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_Main::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_vDragItem >= 0 && !pMouseInfo->IsLButtonPressing())
	{
		m_vDragItem = -1;
		ReleaseCapture();
	}

	if(m_vDragItem == -1)
	{
		CPoint point, off_point;
		MAreaCtrl *area_ctrl;
		int size;
		int i;

		point = pMouseInfo->GetPos();
		off_point = point - m_ptOffset;

		size = m_arrArea.GetSize();
		for(i = 0; i < size; i++)
		{
			area_ctrl = m_arrArea[i];
			if(area_ctrl->m_vRect.PtInRect(off_point)) break;
		}
		CUser *ptmp = CChildView::UserManager.GetMe();
		switch(i)
		{
		case 0:		// 체력
			m_vStrCaption.Format("체력 : %d/%d", ptmp->m_Cur_Hp, ptmp->m_Max_Hp);
			m_bViewCaption = i;
			break;
		case 1:		// 심력
			m_vStrCaption.Format("심력 : %d/%d", ptmp->m_Cur_Mp, ptmp->m_Max_Mp);
			m_bViewCaption = i;
			break;
		case 2:		// 스테미너
			m_vStrCaption.Format("지구력 : %d/%d", ptmp->m_Cur_Fa, ptmp->m_Max_Fa);
			m_bViewCaption = i;
			break;
		case 3:		// 무게
			m_vStrCaption.Format("무게 : %d/%d", ptmp->m_Cur_Weight, ptmp->m_Max_Weight);
			m_bViewCaption = i;
			break;
		case 4:		// 경험치
			m_vStrCaption = _T("경험치");
			m_bViewCaption = i;
			break;
		case 6:
		case 7:
		case 8:
		case 9:
			{
				CMyInvData *inv_data = &ptmp->m_MyBeltData[i-6];
				if(inv_data->m_vUsage > 0)
				{
					CString tt_str, tt_str1;
					BOOL btrue = FALSE;

					switch(inv_data->m_vGender)
					{
					case ITEM_WOMAN:
						m_vStrCaption.Format("%s(여)", inv_data->m_vName);
						break;
					case ITEM_MAN:
						m_vStrCaption.Format("%s(남)", inv_data->m_vName);
						break;
					default:
						m_vStrCaption.Format("%s", inv_data->m_vName);
						break;
					}
					tt_str = "\n";
					for(int k = 0; k < 7; k++)
					{
						if(inv_data->m_vSpecial[k] != 0)
						{
							if(btrue) tt_str += _T(",");
							if(inv_data->m_vSpecial[k] > 0) tt_str1.Format("%s+%d", SPECIAL_NAME[k], inv_data->m_vSpecial[k]);
							else tt_str1.Format("%s%d", SPECIAL_NAME[k], inv_data->m_vSpecial[k]);
							tt_str += tt_str1;
							btrue = TRUE;
						}
					}
					if(btrue == TRUE)
					{
						m_vStrCaption += tt_str;
					}
					if(inv_data->m_strMagic.IsEmpty() == FALSE)
					{
						m_vStrCaption += "\n";
						m_vStrCaption += inv_data->m_strMagic;
					}
					if(inv_data->m_strSNum.IsEmpty() == FALSE)
					{
						m_vStrCaption += "\n";
						m_vStrCaption += inv_data->m_strSNum;
					}
					m_bViewCaption = i;
				}
			}
			break;
		case 26:
		case 27:
		case 28:
		case 29:
			if(m_pMagicData[i-26])
			{
				m_vStrCaption.Format("%s/%d", m_pMagicData[i-26]->m_vName, m_pMagicData[i-26]->m_vUseMp);
				m_bViewCaption = i;
			}
			break;
		default:
			m_bViewCaption = -1;
		}
	}
	else m_bViewCaption = -1;
	return CUIRes::OnMouseMove(pMouseInfo);
}

BOOL CDialog_Main::OnRButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

	for(int i = 6; i <= 9; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
		{
			CUser *ptmp = CChildView::UserManager.GetMe();
			if(ptmp->m_MyBeltData[i-6].m_vUsage > 0) SendUseItem(i-6);
			return TRUE;
		}
	}
	for(i = 26; i <= 29; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
		{
			SetSelMagic(i-26);
			return TRUE;
		}
	}

	return TRUE;
}

int CDialog_Main::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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
			if(pMouseInfo->IsLButtonUp()) {
				OnLButtonUp(pMouseInfo);
			}
			if(pMouseInfo->IsRButtonDown()) {
				OnRButtonDown(pMouseInfo);
			}
			if(pMouseInfo->IsMouseMove()) {
				OnMouseMove(pMouseInfo);
			}
			return 2;
		}
		else
		{
			ResetStatus();
		}
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

void CDialog_Main::ResetStatus()
{
	m_bViewCaption = -1;
	
	MBtnCtrl *btn_ctrl;
	MBtnCtrlEx *btn_ctrl_ex;
	int size = m_arrBtn.GetSize();
	for(int i = 0; i < size; i++)
	{
		btn_ctrl = m_arrBtn[i];
		if(btn_ctrl->m_vState != BUTTON_DISABLE) btn_ctrl->m_vState = BUTTON_NORMAL;
	}
	size = m_arrBtnEx.GetSize();
	for(i = 0; i < size; i++)
	{
		btn_ctrl_ex = m_arrBtnEx[i];
		if(btn_ctrl_ex->m_vState != BUTTON_DISABLE) btn_ctrl_ex->m_vState = BUTTON_NORMAL;
	}
}

void CDialog_Main::SetMagicData(int index, CMyMagicData *magic_data)
{
	if(index < 0 || index >= 4) return;

	for(int i = 0; i < 4; i++)
	{
		if(m_pMagicData[i] == magic_data)
		{
			m_pMagicData[i] = NULL;
			break;
		}
	}
	m_pMagicData[index] = magic_data;
}

void CDialog_Main::LoadMagicData(CMyMagicData *magic_data[])
{
	for(int i = 0; i < 4; i++)
	{
		m_pMagicData[i] = magic_data[i];
	}
}

void CDialog_Main::SendUseItem(short index)
{
	BYTE buf[10];
	int offset = 0;
	CPacketParser::SetByte(buf, offset, PKT_ITEM_USE); //최종점 요청
	CPacketParser::SetByte(buf, offset, BELT_INV); // 인벤토리창에서 사용
	CPacketParser::SetShort(buf, offset, (short)index);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Main::SendItemToInv(short fromindex, short toindex)
{
	if(m_bChanging) return;

	m_bChanging = TRUE;
	m_vFromItem = fromindex;
	m_vToItem = toindex;

	BYTE buf[10];
	int offset = 0;
	CPacketParser::SetByte(buf, offset, PKT_PUTITEM_INV); //최종점 요청
	CPacketParser::SetShort(buf, offset, fromindex);
	CPacketParser::SetShort(buf, offset, toindex);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Main::RcvItemToInv(PBYTE bypData)
{
	int offset = 0;

	BYTE result = CPacketParser::GetByte(bypData, offset);
	if(result == 1)
	{
		CUser *ptmp;

		ptmp = CChildView::UserManager.GetMe();
		if(ptmp)
		{
			ptmp->MoveItemToInv(m_vFromItem, m_vToItem);
			MovingSoundPlay(BASIC_INV, m_vToItem);
			if(Dialog_Inv.IsVisible()) Dialog_Inv.RefreshData();
		}
	}
	else if(result == 3)
	{
		CUser *ptmp;

		ptmp = CChildView::UserManager.GetMe();
		if(ptmp)
		{
			ptmp->Del_Item(BELT_INV, m_vFromItem);
			MovingSoundPlay(BASIC_INV, m_vToItem);
			if(Dialog_Inv.IsVisible()) Dialog_Inv.RefreshData();
		}
	}
	m_bChanging = FALSE;
}

BOOL CDialog_Main::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pFont = new CFont;
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");
	SetBattleMode(NORMAL_MODE);
	if(g_vMainMode == 0) m_bShow = TRUE;
	else m_bShow = FALSE;

	return TRUE;
}

void CDialog_Main::SetBattleMode(int b_mode)
{
	switch(b_mode)
	{
	case BATTLE_MODE:
		m_arrBtnEx[0]->m_vState = BUTTON_DISABLE;
		m_arrBtnEx[1]->m_vState = BUTTON_NORMAL;
		m_arrBtnEx[2]->m_vState = BUTTON_DISABLE;
		break;
	case PK_MODE:
		m_arrBtnEx[0]->m_vState = BUTTON_DISABLE;
		m_arrBtnEx[1]->m_vState = BUTTON_DISABLE;
		m_arrBtnEx[2]->m_vState = BUTTON_NORMAL;
		break;
	default:
		m_arrBtnEx[0]->m_vState = BUTTON_NORMAL;
		m_arrBtnEx[1]->m_vState = BUTTON_DISABLE;
		m_arrBtnEx[2]->m_vState = BUTTON_DISABLE;
		break;
	}
}

void CDialog_Main::SendThrowItem(short index, short count)
{
	CPoint MyPos = CChildView::UserManager.GetMyCellPos();
	
	BYTE buf[10];
	int offset = 0;
	CPacketParser::SetByte(buf, offset, PKT_ITEM_THROW); //최종점 요청
	CPacketParser::SetByte(buf, offset, BELT_INV);
	CPacketParser::SetShort(buf, offset, (short)index);
	CPacketParser::SetShort(buf, offset, count);
	CPacketParser::SetShort(buf, offset, (short)MyPos.x);
	CPacketParser::SetShort(buf, offset, (short)MyPos.y);
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Main::DrawTipRect(CDDSurface *pDest, CRect& t_rect, CRect standard_rect, int area_index)
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

	rect_size.cx = r_width[0]+r_width[1]*width+r_width[2];
	rect_size.cy = r_height[0]+r_height[1]*height+r_height[2];
	start_pos.x = (standard_rect.Width() - rect_size.cx) / 2 + standard_rect.left;
	start_pos.y = standard_rect.top - rect_size.cy;
	t_rect.OffsetRect(start_pos);

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

	rect_size.cx = (rect_size.cx - t_rect.Width()) / 2;
	rect_size.cy = (rect_size.cy - t_rect.Height()) / 2;

	t_rect.OffsetRect(rect_size.cx, rect_size.cy);
}

void CDialog_Main::ViewCaption(CDDSurface *pDest, CRect standard_rect, CString str, int area_index)
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

//	t_rect.OffsetRect(pos);

	DrawTipRect(pDest, t_rect, standard_rect, area_index);

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

BOOL CDialog_Main::OnLButtonDblClk(BBDX_MouseInformation *pMouseInfo)
{
	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

	for(int i = 6; i <= 9; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
		{
			CUser *ptmp = CChildView::UserManager.GetMe();
			if(ptmp->m_MyBeltData[i-6].m_vUsage > 0) SendUseItem(i-6);
			return TRUE;
		}
	}

	return FALSE;
}

void CDialog_Main::SetSelMagic(int index)
{
	if(m_vSelMagicIndex == index) m_vSelMagicIndex = -1;
	else m_vSelMagicIndex = index;
}