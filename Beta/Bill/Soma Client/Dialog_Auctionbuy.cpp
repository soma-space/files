// Dialog_AuctionBuy.cpp: implementation of the CDialog_AuctionBuy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_AuctionBuy.h"
//#include "BBDX_TextOuter.h"
//#include "TWFont.h"
#include "Item_Total.h"
#include "ChildView.h"
#include "User.h"
#include "Dialog_AuctionDetail.h"
#include "Dialog_Message.h"
#include "UIScrollBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CItem_Total				Item_Total;
extern CDialog_AuctionDetail	Dialog_AuctionDetail;
extern CDialog_Message			Dialog_Message;
extern CUIRes *g_pShowDialog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_AuctionBuy::CDialog_AuctionBuy()
{
//	INIT_PTR(m_pTWFont);
	INIT_PTR(m_pScrollBar);
//	INIT_PTR(m_pMoneyOuter);
	m_pFont = NULL;
	m_pExplain = NULL;
	m_vCurItem = -1;
	m_vPosMode = POS_LEFT;
}

CDialog_AuctionBuy::~CDialog_AuctionBuy()
{
	DeleteContents();
//	DELETE_SAFE(m_pTWFont);
	SAFE_DELETE(m_pScrollBar);
//	DELETE_SAFE(m_pMoneyOuter);
	if(m_pFont) delete m_pFont;
	if(m_pExplain) delete[] m_pExplain;
//	for(int i = 0; i < 10; i++)
//	{
//		DELETE_SAFE(m_pNameOuter[i]);
//	}
}

BOOL CDialog_AuctionBuy::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[22]->m_vRect, 0);

/*	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		delete m_pTWFont; m_pTWFont=NULL;
		TRACE("####### Dialog AuctionSell Init 실패.. ############ \n");
		return FALSE;
	}
	m_pMoneyOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	for(int i = 0; i < 10; i++)
	{
		m_pNameOuter[i] = new BBDX_TextOuter(m_pTWFont, 14, 14);
	}*/
	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	return TRUE;
}

void CDialog_AuctionBuy::InitData()
{
//	m_pMoneyOuter->ClearString();
//	for(int i = 0; i < 10; i++)
//	{
//		m_pNameOuter[i]->ClearString();
//	}
	m_vCurItem = -1;
}

void CDialog_AuctionBuy::DeleteContents()
{
	int i;
	int count;

	count = m_arrAuctionItem.GetSize();
	for(i = 0; i < count; i++)
	{
		delete m_arrAuctionItem[i];
	}
	m_arrAuctionItem.RemoveAll();
	if(m_pExplain)
	{
		delete[] m_pExplain;
		m_pExplain = NULL;
	}
}

void CDialog_AuctionBuy::ShowWindow()
{
	if(IsLoading() == FALSE)
	{
		if(Create(CChildView::DirectXMng.m_pDD, "Auct-B.lib", 0, TRUE, FALSE, FALSE) == FALSE)
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	InitData();
	CUIRes::ShowWindow();
}

void CDialog_AuctionBuy::HideWindow()
{
	g_pShowDialog = NULL;
	CUIRes::HideWindow();
	DeleteContents();
}

void CDialog_AuctionBuy::HideAllWindow()
{
	CUIRes::HideWindow();
	DeleteContents();
}

BOOL CDialog_AuctionBuy::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:		// 나가기
			HideWindow();
			break;
		case 1:
			m_pScrollBar->ScrollUp();
			break;
		case 2:
			m_pScrollBar->ScrollDown();
			break;
		}
	}

	return TRUE;
}

BOOL CDialog_AuctionBuy::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CString t_str;
	DWORD dwTrans;
	CRect rtDst, rtSrc;
	CPoint s_pos;
	int i, j, start_pos, end_pos;
	CUser *ptmp;
	int count;

	Draw3ButtonCtrl(pDest, 0, 2);

	m_pScrollBar->Draw(pDest);

	count = m_arrAuctionItem.GetSize();
	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
	start_pos = m_pScrollBar->GetStartLine() * 2;
	if(start_pos+10 > count) end_pos = count;
	else end_pos = start_pos+10;
	for(i = start_pos, j = 0; i < end_pos; i++, j++)
	{
		s_pos = m_arrArea[j]->m_vRect.TopLeft();
		s_pos.Offset(m_ptOffset);
		rtSrc = Item_Total.m_Item_Small->m_SprData[m_arrAuctionItem[i]->m_vPicNum].rcV;
		pDest->BltFast(s_pos, Item_Total.m_Item_Small->m_lpSurface, &rtSrc, dwTrans);
	}

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));

	for(i = start_pos, j = 0; i < end_pos; i++, j++)
	{
		if(m_arrAuctionItem[i]->m_vType >= TYPE_POTION && m_arrAuctionItem[i]->m_vType <= TYPE_TOKEN)
		{
			rtDst = m_arrArea[j]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_str.Format("%d", m_arrAuctionItem[i]->m_vUsage);
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_RIGHT|DT_SINGLELINE);
		}

		rtDst = m_arrArea[j+10]->m_vRect;
		rtDst.OffsetRect(m_ptOffset);
		pDC->DrawText(m_pExplain[i], &rtDst, DT_TOP|DT_LEFT|DT_EXTERNALLEADING);
//		m_pNameOuter[i]->DrawTexts(pDest, rtDst);
	}

	ptmp = CChildView::UserManager.GetMe();
	t_str.Format("%lu", ptmp->m_Money);
//	m_pMoneyOuter->SetString(t_str);
	rtDst = m_arrArea[20]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
//	m_pMoneyOuter->DrawTexts(pDest, rtDst);
	pDC->DrawText(t_str, &rtDst, DT_RIGHT|DT_TOP|DT_SINGLELINE);

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	return TRUE;
}

void CDialog_AuctionBuy::SetScrollData()
{
	int count;

	count = m_arrAuctionItem.GetSize();
	count = count / 2 + count%2;
	m_pScrollBar->SetMaxLineCount(count);
	m_pScrollBar->SetViewLineCount(5);
	m_pScrollBar->SetStartLine(m_pScrollBar->GetStartLine());
}

BOOL CDialog_AuctionBuy::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CPoint t_pos = pMouseInfo->GetPos() - m_ptOffset;
	int i;

	if(Dialog_AuctionDetail.IsVisible()) return TRUE;

	if(m_pScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}

	m_vCurItem = -1;
	for(i = 0; i < 10; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(t_pos))
		{
			m_vCurItem = i + m_pScrollBar->GetStartLine()*2;
			if(m_vCurItem >= 0 && m_vCurItem < m_arrAuctionItem.GetSize())
			{
				Dialog_AuctionDetail.ShowWindow(0, this, m_arrAuctionItem[i]);
			}
			else
			{
				m_vCurItem = -1;
			}
			break;
		}
	}
	if(m_vCurItem == -1) return CUIRes::OnLButtonDown(pMouseInfo);
	else return TRUE;
}

void CDialog_AuctionBuy::CallBack_MessageBox(int nId, int nValue)
{
	CString t_str;

	if(nId == 0)
	{
		if(nValue == 1)		// 경매 참가
		{
			SendAuction(Dialog_AuctionDetail.m_vValue);
		}
		m_vCurItem = -1;
	}
}

void CDialog_AuctionBuy::SendAuction(DWORD money)
{
	BYTE buf[12];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, PKT_AUCTION_BUY_SEND);

	CPacketParser::SetShort(buf, offset, m_vAuctionNumber);
	CPacketParser::SetInt(buf, offset, m_arrAuctionItem[m_vCurItem]->m_vIndex);
	CPacketParser::SetDword(buf, offset, money);

	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_AuctionBuy::RcvOpen(PBYTE bypData)
{
	int offset=0;
//	CString t_str;

	ShowWindow();

	m_vAuctionNumber = CPacketParser::GetShort(bypData, offset);
	int count = CPacketParser::GetShort(bypData, offset);
	if(count <= 0) return;
	m_pExplain = new CString[count];
	for(int i = 0; i < count; i++)
	{
		BYTE len;
		LPTSTR t_buf;
		ITEM_AUCTION *item_auction;

		item_auction = new ITEM_AUCTION;

		item_auction->m_vIndex = CPacketParser::GetInt(bypData, offset);
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
		for(int j = 0; j < 7; j++)
		{
			item_auction->m_vSpecial[j] = CPacketParser::GetShort(bypData, offset);
		}
		len = CPacketParser::GetByte(bypData, offset);
		t_buf = item_auction->m_vRegistrant.GetBuffer(len);
		CPacketParser::GetString(bypData, offset, (PBYTE)t_buf, len);
		item_auction->m_vRegistrant.ReleaseBuffer();

		m_arrAuctionItem.Add(item_auction);

		switch(m_arrAuctionItem[i]->m_vGender)
		{
		case ITEM_WOMAN:
			m_pExplain[i].Format("이름:%s(여)\n현재가:%lu", m_arrAuctionItem[i]->m_vName, m_arrAuctionItem[i]->m_vCost);
			break;
		case ITEM_MAN:
			m_pExplain[i].Format("이름:%s(남)\n현재가:%lu", m_arrAuctionItem[i]->m_vName, m_arrAuctionItem[i]->m_vCost);
			break;
		default:
			m_pExplain[i].Format("이름:%s\n현재가:%lu", m_arrAuctionItem[i]->m_vName, m_arrAuctionItem[i]->m_vCost);
			break;
		}
//		t_str.Format("이름:%s", m_pAuctionItem[i]->m_vName);
//		m_pNameOuter[i]->SetString(t_str);
//		t_str.Format("현재가:%lu", m_pAuctionItem[i]->m_vCost);
//		m_pNameOuter[i]->AddString(t_str);
	}

	SetScrollData();
}

void CDialog_AuctionBuy::RcvAuction(PBYTE bypData)
{
	int offset = 0;
	CString t_str;

	BYTE result = CPacketParser::GetByte(bypData, offset);

	if(result == 1)
	{
		t_str.LoadString(IDS_AUCTION_SUCCESS);
	}
	else
	{
		t_str.LoadString(IDS_AUCTION_FAIL);
	}
	Dialog_Message.ShowWindow();
	Dialog_Message.SetMessage(t_str);
}

BOOL CDialog_AuctionBuy::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();
	if(m_pScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_AuctionBuy::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;

	return CUIRes::OnMouseMove(pMouseInfo);
}
