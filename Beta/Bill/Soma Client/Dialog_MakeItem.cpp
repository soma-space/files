	// Dialog_MakeItem.cpp: implementation of the CDialog_MakeItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_MakeItem.h"
#include "BBDX_Editor.h"
//#include "BBDX_TextOuter.h"
#include "PacketParser.h"
#include "User.h"
//#include "TWFont.h"
#include "ChildView.h"
#include "Item_Total.h"
#include "Dialog_Name.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CItem_Total         Item_Total;
extern CUIRes *g_pShowDialog;
extern CDialog_Name			Dialog_Name;
extern CChildView			*g_pChildView;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_MakeItem::CDialog_MakeItem()
{
//	INIT_PTR(m_pOuter);
//	INIT_PTR(m_pItemOuter);
//	INIT_PTR(m_pNameOuter);
	INIT_PTR(m_pScrollBar);
//	INIT_PTR(m_pTWFont);
	INIT_PTR(m_pFont);

	int i;

	m_pWnd = NULL;
	m_vDragItem = -1;
	m_vCurMakeSpec = 0;
	for(i = 0; i < 6; i++)
	{
		m_vMakeSpec[i] = -1;
	}
	for(i = 0; i < 5; i++)
	{
		m_vSelItemMake[i].m_vUsage = 0;
	}

	m_vItemPic = -1;
	m_vPosMode = POS_LEFT;
	m_vTipIndex = -1;
	m_vSelSpec = -1;
	m_vGender = m_vOldGender = ITEM_MAN;
}

CDialog_MakeItem::~CDialog_MakeItem()
{
	DeleteContents();
	SAFE_DELETE(m_pScrollBar);
//	SAFE_DELETE(m_pTWFont);
//	SAFE_DELETE(m_pOuter);
//	SAFE_DELETE(m_pItemOuter);
//	SAFE_DELETE(m_pNameOuter);
	if(m_pFont)
	{
		m_pFont->DeleteObject();
		delete m_pFont;
	}
}

void CDialog_MakeItem::MakerOpen(PBYTE bypData)
{
	int offset, i;
	short len;

	// 초기화
	offset = 0;
	len = CPacketParser::GetShort(bypData, offset);				// 제작 종류 개수
	for(i = 0; i < len; i++)
	{
		m_vMakeSpec[i] = CPacketParser::GetShort(bypData, offset);	// 제작 종류
	}
	for(i = len; i < 6; i++)
	{
		m_vMakeSpec[i] = -1;
	}
	m_vCurMakeSpec = m_vMakeSpec[0];
	if((m_vCurMakeSpec >= 9 && m_vCurMakeSpec <= 15) || m_vCurMakeSpec == 22 || m_vCurMakeSpec == 23)
	{
		SetGender(m_vOldGender);
	}
	else SetGender(ITEM_ALL);
}

void CDialog_MakeItem::ReceiveProduct(PBYTE bypData)
{
	int offset=0, i;
	short len;
	CString t_str;
	short index, usage;
	CUser *ptmp = CChildView::UserManager.GetMe();

	BYTE bRet = CPacketParser::GetByte(bypData, offset);		// 성공/실패
	if(bRet == 1)
	{
		len = CPacketParser::GetShort(bypData, offset);				// 재료 아이템 개수
		if(len > 0)
		{
			for(i = 0; i < len; i++)
			{
				index = CPacketParser::GetShort(bypData, offset);				// 재료 아이템 인덱스
				usage = CPacketParser::GetShort(bypData, offset);				// 재료 아이템 사용회수
				if(usage == 0)
				{
					ptmp->Del_Item(BASIC_INV, index);// 아이템을 지워준다.
				}
				else
				{
					ptmp->m_MyInvData[index].m_vUsage = usage;
				}
			}

			index = CPacketParser::GetShort(bypData, offset);
			if(index >= 0)
			{
				CString name, magic_name, s_num;
				BYTE len;
				short special[7];

				BYTE type = CPacketParser::GetByte(bypData, offset);
				BYTE arm = CPacketParser::GetByte(bypData, offset);// 장착위..
				short picnum = CPacketParser::GetShort(bypData, offset);
				BYTE gender = CPacketParser::GetByte(bypData, offset);
				short min = CPacketParser::GetShort(bypData, offset);
				short max = CPacketParser::GetShort(bypData, offset);
				DWORD value = CPacketParser::GetDword(bypData, offset);
				short limit_str = CPacketParser::GetShort(bypData, offset);
				short limit_int = CPacketParser::GetShort(bypData, offset);
				short limit_skill = CPacketParser::GetShort(bypData, offset);
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
				short use_count = CPacketParser::GetShort(bypData, offset);
				short max_use_count = CPacketParser::GetShort(bypData, offset);
				short org_max_use_count = CPacketParser::GetShort(bypData, offset);
				
				len = CPacketParser::GetByte(bypData, offset);
				CPacketParser::GetString(bypData, offset, (PBYTE)name.GetBuffer(len), (int)len);
				name.ReleaseBuffer();
				
				ptmp->Add_Item(index, type, arm, picnum, gender, min, max, limit_str, limit_int, limit_skill, magic_name, s_num, special, value, use_count, max_use_count, org_max_use_count, name);

				m_vItemPic = picnum;
		//		t_str = _T("아이템이 완성되었습니다.");
		//		m_pItemOuter->SetString(t_str);
				m_strItemOuter = _T("아이템이 완성되었습니다.");
				if(name.IsEmpty()) t_str = _T("\n이름 : 없음");
				else t_str.Format("\n이름 : %s", (LPCTSTR)name);
		//		m_pItemOuter->AddString(t_str);
				m_strItemOuter += t_str;
			}
		}
	}
	else
	{
		len = CPacketParser::GetShort(bypData, offset);				// 재료 아이템 개수
		if(len > 0)
		{
			for(i = 0; i < len; i++)
			{
				index = CPacketParser::GetShort(bypData, offset);				// 재료 아이템 인덱스
				usage = CPacketParser::GetShort(bypData, offset);				// 재료 아이템 사용회수
				if(usage == 0)
				{
					ptmp->Del_Item(BASIC_INV, index);// 아이템을 지워준다.
				}
				else
				{
					ptmp->m_MyInvData[index].m_vUsage = usage;
				}
			}
		}

//		m_pItemOuter->ClearString();
//		t_str = _T("아이템 제작에 실패하였습니다.");
//		m_pItemOuter->SetString(t_str);
		m_strItemOuter = _T("아이템 제작에 실패하였습니다.");
		m_vItemPic = -1;
	}

	for(i = 0; i < 5; i++)
	{
		m_vSelItemMake[i].m_vUsage = 0;
	}
	RefreshData();
}

void CDialog_MakeItem::SendProduce()
{
	BYTE buf[30];
	int offset = 0, i;
	BYTE count;
	BYTE len;

	count = 0;
	for(i = 0; i < 5; i++)
	{
		if(m_vSelItemMake[i].m_vUsage > 0) count++;
	}
	if(count == 0) return;

	len = (BYTE)m_vItemName.GetLength();
	if(len == 0) return;

	CPacketParser::SetByte(buf, offset, PKT_MAKEITEM); //최종점 요청
	TRACE("Send PKT_MAKEITEM \n");
	
	CPacketParser::SetShort(buf, offset, m_vCurMakeSpec);
	CPacketParser::SetByte(buf, offset, m_vGender);										// 남성/여성/공용
	CPacketParser::SetByte(buf, offset, len);											// 제작 아이템 이름 길이
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)m_vItemName, len);			// 제작 아이템 이름
	CPacketParser::SetByte(buf, offset, count);											// 재료 종류 개수
	for(i = 0; i < 5; i++)
	{
		if(m_vSelItemMake[i].m_vUsage > 0)
		{
			CPacketParser::SetShort(buf, offset, (short)m_vSelItemMake[i].m_vIndex);	// 재료 Inv 인덱스
			CPacketParser::SetShort(buf, offset, (short)m_vSelItemMake[i].m_vUsage);	// 재료 개수
		}
	}
	CChildView::SocketMng.Send(offset, buf);
}

BOOL CDialog_MakeItem::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex) {
		case 0:	// 제작하기
			SendProduce();
			break;
		case 1:	// 닫기
			HideWindow();
			break;
		case 25: // 이름 정하기
			Dialog_Name.ShowWindow(0, this, 12);
			Dialog_Name.SetName(m_vItemName);
			break;
		case 26:
			m_pScrollBar->ScrollUp();
			break;
		case 27:
			m_pScrollBar->ScrollDown();
			break;
		}
	}
	else if(nSpec == BTN_CTRL_EX)
	{
		switch(nIndex) {
		case 1:	// Man -> Woman
			SetGender(ITEM_WOMAN);
			break;
		case 2:	// Woman -> Man
			SetGender(ITEM_MAN);
			break;
		}
	}

	return TRUE;
}

BOOL CDialog_MakeItem::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CRect rtDst, rtSrc;
	DWORD dwTrans;
	int hand_end, hand_start, i, j;
	CString t_str;
	MBtnCtrl *btn_ctrl;
	ITEM_DETAIL *item_detail;
	BBDX_MouseInformation* pMouseInfo = CChildView::DirectXMng.GetMouseInfoPtr();
	CPoint m_pos, t_pos;

	m_pos = pMouseInfo->GetPos() - m_ptOffset;
	dwTrans = DDBLTFAST_WAIT;

	for(i = 0; i < 6; i++)
	{
		if(m_vMakeSpec[i] == -1)
		{
			pDest->BltFast(m_ptOffset+m_arrArea[22+i]->m_vRect.TopLeft(), m_pDDSurface, &m_arrArea[28]->m_vRect, dwTrans);
		}
		else
		{
			btn_ctrl = m_arrBtn[m_vMakeSpec[i]+1];
			if(m_vMakeSpec[i] == m_vCurMakeSpec)
			{
				rtSrc = btn_ctrl->m_vDownRect;
			}
			else
			{
				if(m_arrArea[22+i]->m_vRect.PtInRect(m_pos)) rtSrc = btn_ctrl->m_vFocusRect;
				else rtSrc = btn_ctrl->m_vRect;
			}
			pDest->BltFast(m_ptOffset+m_arrArea[22+i]->m_vRect.TopLeft(), m_pDDSurface, &rtSrc, dwTrans);
		}
	}
	Draw3ButtonCtrl(pDest, 0, 1);
	Draw3ButtonCtrl(pDest, 25, 27);
	if(m_vGender == ITEM_ALL)
	{
		pDest->BltFast(m_ptOffset+m_arrBtnEx[1]->m_vRect.TopLeft(), m_pDDSurface, &m_arrArea[42]->m_vRect, dwTrans);
	}
	else
	{
		Draw4ButtonCtrl(pDest, 1, 2);
	}

	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	if(m_vItemPic >= 0)
	{
		t_pos = m_arrArea[6]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_vItemPic].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	for(i = 0; i < 5; i++)
	{
		if(m_vSelItemMake[i].m_vUsage > 0)
		{
			rtDst = m_arrArea[i]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			rtSrc = Item_Total.m_Item_Small->m_SprData[m_vSelItemMake[i].m_vPicNum].rcV;
			pDest->BltFast(rtDst.TopLeft(), Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);

//			t_str.Format("%d", m_vSelItemMake[i].m_vUsage);
//			m_pOuter->SetString(t_str);
//			m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
		}
	}
	hand_end = m_arrItemMake.GetSize();
	hand_start = m_pScrollBar->GetStartLine() * 6;
	if(hand_end > hand_start+12) hand_end = hand_start+12;
	for(i = hand_start, j = 8; i < hand_end; i++, j++)
	{
		rtDst = m_arrArea[j]->m_vRect;
		rtDst.OffsetRect(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_arrItemMake[i]->m_vPicNum].rcV;
		pDest->BltFast(rtDst.TopLeft(), Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);

//		t_str.Format("%d", m_arrItemMake[i]->m_vUsage);
//		m_pOuter->SetString(t_str);
//		m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	}

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	
	rtDst = m_arrArea[7]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
//	m_pItemOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(m_strItemOuter, &rtDst, DT_TOP|DT_LEFT);

	rtDst = m_arrArea[5]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
//	m_pNameOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(m_vItemName, &rtDst, DT_TOP|DT_LEFT);

	for(i = 0; i < 5; i++)
	{
		if(m_vSelItemMake[i].m_vUsage > 0)
		{
			rtDst = m_arrArea[i]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", m_vSelItemMake[i].m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
//			m_pOuter->SetString(t_str);
//			m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
		}
	}
	for(i = hand_start, j = 8; i < hand_end; i++, j++)
	{
		rtDst = m_arrArea[j]->m_vRect;
		rtDst.OffsetRect(m_ptOffset);
		t_str.Format("%d", m_arrItemMake[i]->m_vUsage);
//		m_pOuter->SetString(t_str);
//		m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
		pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
	}

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	m_pScrollBar->Draw(pDest);

	// Drag 아이템 그리기
	if(m_vSelSpec >= 0)
	{
		if(m_vSelSpec == 0)
		{
			t_pos = pMouseInfo->GetPos() - m_vDragOffset;
			rtSrc = Item_Total.m_Item_Small->m_SprData[m_vSelItemMake[m_vDragItem].m_vPicNum].rcV;
			pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
		}
		else if(m_vSelSpec == 1)
		{
			item_detail = m_arrItemMake[m_vDragItem];

			t_pos = pMouseInfo->GetPos() - m_vDragOffset;
			rtSrc = Item_Total.m_Item_Small->m_SprData[item_detail->m_vPicNum].rcV;
			pDest->BltFast(t_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
		}
	}

	if(m_vTipIndex >= 0)
	{
		item_detail = NULL;
		if(m_vTipIndex <= 4 && m_vSelItemMake[m_vTipIndex].m_vUsage > 0)
		{
			item_detail = &m_vSelItemMake[m_vTipIndex];
		}
		else if(m_vTipIndex >= 8 && m_vTipIndex <= 19 && hand_start+m_vTipIndex-8 < hand_end)
		{
			item_detail = m_arrItemMake[hand_start+m_vTipIndex-8];
		}
		if(item_detail)
		{
			CPoint caption_pos;

			caption_pos.x = m_arrArea[m_vTipIndex]->m_vRect.right + m_ptOffset.x;
			caption_pos.y = m_arrArea[m_vTipIndex]->m_vRect.top + m_ptOffset.y;
			ViewCaption(pDest, caption_pos, item_detail->m_vName, 29);
		}
	}

	return TRUE;
}

void CDialog_MakeItem::ShowWindow()
{
	if(!IsLoading())
	{
		SetWnd(g_pChildView);
		if(!Create(CChildView::DirectXMng.m_pDD, "MakeItem.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
	}
	else
	{
		InitData();
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	CUIRes::ShowWindow();

	RefreshData();
}

void CDialog_MakeItem::HideWindow()
{
	g_pShowDialog = NULL;

	HideAllWindow();
}

void CDialog_MakeItem::HideAllWindow()
{
	m_vTipIndex = -1;
	CUIRes::HideWindow();

	DeleteContents();
}

void CDialog_MakeItem::DeleteContents()
{
	int size, i;

	size = m_arrItemMake.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemMake[i];
	}
	m_arrItemMake.RemoveAll();
}

void CDialog_MakeItem::RefreshData()
{
	int i, size;
	ITEM_DETAIL *item_detail;
	CUser *ptmp;
	int count, max_line;

	// 초기화
	size = m_arrItemMake.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrItemMake[i];
	}
	m_arrItemMake.RemoveAll();

	ptmp = CChildView::UserManager.GetMe();
	for(i = 10; i < 50; i++)
	{
		if(ptmp->GetInv_ItemUsage(i) > 0 && ptmp->GetInv_ItemType(i) == TYPE_MTR)
		{
			item_detail = new ITEM_DETAIL;

			item_detail->m_vIndex = i;
			item_detail->m_vType = ptmp->GetInv_ItemType(i);
			item_detail->m_vArmPos = ptmp->GetInv_ItemArm(i); // 재료일 경우 재료의 종류
			item_detail->m_vPicNum = ptmp->GetInv_ItemPic(i);
			item_detail->m_vUsage = ptmp->GetInv_ItemUsage(i);
			item_detail->m_vGender = ptmp->m_MyInvData[i].m_vGender;
			item_detail->m_vName = ptmp->GetInv_ItemName(i);
			item_detail->m_vMinDamage = ptmp->m_MyInvData[i].m_vMinDamage;
			item_detail->m_vMaxDamage = ptmp->m_MyInvData[i].m_vMaxDamage;
			item_detail->m_vLimitStr = ptmp->m_MyInvData[i].m_vLimitStr;
			item_detail->m_vLimitInt = ptmp->m_MyInvData[i].m_vLimitInt;
			item_detail->m_vLimitSkill = ptmp->m_MyInvData[i].m_vLimitSkill;
			item_detail->m_strMagic = ptmp->m_MyInvData[i].m_strMagic;
			item_detail->m_strSNum = ptmp->m_MyInvData[i].m_strSNum;
			for(int k = 0; k < 7; k++) item_detail->m_vSpecial[k] = ptmp->m_MyInvData[i].m_vSpecial[k];

			m_arrItemMake.Add(item_detail);
		}
	}

	count = m_arrItemMake.GetSize();
	max_line = count / 6 + ((count % 6) ? 1 : 0);
	m_pScrollBar->SetMaxLineCount(max_line);
	m_pScrollBar->SetViewLineCount(2);
	m_pScrollBar->SetStartLine(m_pScrollBar->GetStartLine());
}

BOOL CDialog_MakeItem::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
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
//	m_pItemOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
//	m_pNameOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[21]->m_vRect, 0);

	return TRUE;
}

void CDialog_MakeItem::InitData()
{
//	m_pOuter->ClearString();
//	m_pItemOuter->ClearString();
//	m_pNameOuter->ClearString();
	m_strItemOuter.Empty();
	m_vItemName.Empty();

	m_pScrollBar->InitScroll();

	SetScrollData();

	int i;

	m_vSelSpec = -1;
	m_vDragItem = -1;
	for(i = 0; i < 5; i++)
	{
		m_vSelItemMake[i].m_vUsage = 0;
	}

	m_vItemPic = -1;
}

void CDialog_MakeItem::SetScrollData()
{
	int count, max_line;

	count = m_arrItemMake.GetSize();
	max_line = count / 6 + ((count%6)?1:0);
	m_pScrollBar->SetMaxLineCount(max_line);
	m_pScrollBar->SetViewLineCount(2);
	m_pScrollBar->SetStartLine(m_pScrollBar->GetStartLine());
}

BOOL CDialog_MakeItem::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}

	int i;
	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

	for(i = 22; i <= 27; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
		{
			if(m_vMakeSpec[i-22] != -1)
			{
				m_vCurMakeSpec = m_vMakeSpec[i-22];
				if((m_vCurMakeSpec >= 9 && m_vCurMakeSpec <= 15) || m_vCurMakeSpec == 22 || m_vCurMakeSpec == 23)
				{
					SetGender(m_vGender);
				}
				else SetGender(ITEM_ALL);
			}
			SetFocus();
			return TRUE;
		}
	}

	BOOL retVal = CUIRes::OnLButtonDown(pMouseInfo);

	if(retVal)
	{
		if(m_vSpecCtrl == AREA_CTRL)
		{
			if(m_vIndexCtrl >= 0 && m_vIndexCtrl <= 4)
			{
				int item_index;

				item_index = m_vIndexCtrl;
				if(m_vSelItemMake[item_index].m_vUsage > 0)
				{
					m_vSelSpec = 0;
					m_vDragItem = item_index;
					m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
					SetCapture();
				}
			}
			if(m_vIndexCtrl >= 8 && m_vIndexCtrl <= 19)
			{
				int item_index;
				
				item_index = m_pScrollBar->GetStartLine() * 6 + (m_vIndexCtrl-8);
				if(item_index < m_arrItemMake.GetSize() && m_arrItemMake[item_index]->m_vUsage > 0)
				{
					m_vSelSpec = 1;
					m_vDragItem = item_index;
					m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
					SetCapture();
				}
			}
		}
	}

	return retVal;
}

BOOL CDialog_MakeItem::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	if(m_vSelSpec >= 0)
	{
		CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

		if(m_vSelSpec == 0 && m_arrArea[41]->m_vRect.PtInRect(m_pos))
		{
			int i;

			for(i = 0; i < m_arrItemMake.GetSize(); i++)
			{
				if(!m_arrItemMake[i]->m_vName.Compare((LPCTSTR)m_vSelItemMake[m_vDragItem].m_vName))
				{
					m_arrItemMake[i]->m_vUsage++;
					m_vSelItemMake[m_vDragItem].m_vUsage--;
					break;
				}
			}
		}
		else if(m_vSelSpec == 1 && m_arrArea[40]->m_vRect.PtInRect(m_pos))
		{
			int i;
			ITEM_DETAIL *item_dst, *item_src, *item_detail;

			item_src = m_arrItemMake[m_vDragItem];
			if(item_src->m_vUsage <= 0) return TRUE;

			if(item_src->m_vArmPos == MTR_GEMS)
			{
				item_dst = &m_vSelItemMake[4];
				if(item_dst->m_vUsage > 0)
				{
					if(!item_dst->m_vName.Compare((LPCTSTR)item_src->m_vName))
					{
						item_dst->m_vUsage++;
						item_src->m_vUsage--;
					}
					else
					{
						for(i = 0; i < m_arrItemMake.GetSize(); i++)
						{
							item_detail = m_arrItemMake[i];
							if(!item_dst->m_vName.Compare((LPCTSTR)item_detail->m_vName))
							{
								item_detail->m_vUsage++;
								item_dst->m_vUsage--;
								break;
							}
						}
						item_dst->m_vArmPos = item_src->m_vArmPos;
						item_dst->m_vIndex = item_src->m_vIndex;
						item_dst->m_vName = item_src->m_vName;
						item_dst->m_vPicNum = item_src->m_vPicNum;
						item_dst->m_vType = item_src->m_vType;
						item_dst->m_vUsage = 1;
						item_src->m_vUsage--;
					}
				}
				else
				{
					item_dst->m_vArmPos = item_src->m_vArmPos;
					item_dst->m_vIndex = item_src->m_vIndex;
					item_dst->m_vName = item_src->m_vName;
					item_dst->m_vPicNum = item_src->m_vPicNum;
					item_dst->m_vType = item_src->m_vType;
					item_dst->m_vUsage = 1;
					item_src->m_vUsage--;
				}
			}
			else
			{
				item_dst = NULL;
				for(i = 0; i < 4; i++)
				{
					if(m_vSelItemMake[i].m_vUsage > 0 && !m_vSelItemMake[i].m_vName.Compare((LPCTSTR)item_src->m_vName))
					{
						item_dst = &m_vSelItemMake[i];
						item_dst->m_vUsage++;
						item_src->m_vUsage--;
						break;
					}
				}
				if(item_dst == NULL)
				{
					for(i = 0; i < 4; i++)
					{
						if(m_vSelItemMake[i].m_vUsage == 0)
						{
							item_dst = &m_vSelItemMake[i];
							item_dst->m_vArmPos = item_src->m_vArmPos;
							item_dst->m_vIndex = item_src->m_vIndex;
							item_dst->m_vName = item_src->m_vName;
							item_dst->m_vPicNum = item_src->m_vPicNum;
							item_dst->m_vType = item_src->m_vType;
							item_dst->m_vUsage = 1;
							item_src->m_vUsage--;
							break;
						}
					}
				}
			}
		}
		m_vSelSpec = -1;
		m_vDragItem = -1;
	}
	ReleaseCapture();
	if(m_pScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_MakeItem::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;

	if(m_vSelSpec < 0)
	{
		CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;
		int i;

		for(i = 0; i <= 4; i++)
		{
			if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
			{
				m_vTipIndex = i;
				return CUIRes::OnMouseMove(pMouseInfo);
			}
		}
		for(i = 8; i <= 19; i++)
		{
			if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
			{
				m_vTipIndex = i;
				return CUIRes::OnMouseMove(pMouseInfo);
			}
		}
		m_vTipIndex = -1;
	}

	return CUIRes::OnMouseMove(pMouseInfo);
}

int CDialog_MakeItem::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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

void CDialog_MakeItem::CallBack_MessageBox(int nId, int nValue)
{
	if(nId == 0)
	{
		if(nValue == 1)
		{
			m_vItemName = Dialog_Name.m_vName;
//			m_pNameOuter->SetString(m_vItemName);
		}
	}
}

void CDialog_MakeItem::DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index)
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

void CDialog_MakeItem::ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index)
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

void CDialog_MakeItem::SetGender(int gender)
{
	m_vGender = gender;

	if(m_vGender == ITEM_WOMAN)
	{
		m_vOldGender = m_vGender;
		m_arrBtnEx[1]->m_vState = BUTTON_DISABLE;
		m_arrBtnEx[2]->m_vState = BUTTON_NORMAL;
	}
	else if(m_vGender == ITEM_MAN)
	{
		m_vOldGender = m_vGender;
		m_arrBtnEx[1]->m_vState = BUTTON_NORMAL;
		m_arrBtnEx[2]->m_vState = BUTTON_DISABLE;
	}
	else	// ITEM_ALL
	{
		m_arrBtnEx[1]->m_vState = BUTTON_DISABLE;
		m_arrBtnEx[2]->m_vState = BUTTON_DISABLE;
	}
}