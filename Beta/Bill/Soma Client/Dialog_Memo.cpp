// Dialog_Memo.cpp: implementation of the CDialog_Memo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Memo.h"
#include "PacketParser.h"
//#include "TWFont.h"
//#include "BBDX_TextOuterWithScroll.h"
#include "BBDX_Editor.h"
#include "ChildView.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "Dialog_MemoCtrl.h"
#include "BBDX_ChatOuter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define LINE_GAP		3

extern CUIRes *g_pFocusDialog;
extern CDialog_MemoCtrl Dialog_MemoCtrl;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Memo::CDialog_Memo(CWnd *pWnd)
: CUIRes(pWnd)
{
	INIT_PTR(m_pScrollBar);
//	INIT_PTR(m_pUserOuter);
	INIT_PTR(m_pChatOuter);
//	INIT_PTR(m_pTWFont);
	m_pFont = NULL;
	m_bIcon = FALSE;
	m_vIconIndex = 4;
	m_vPosMode = POS_NOTEFFECT;
	m_bEnableFocus = TRUE;
	m_vMemoRect.SetRect(0,0,0,0);
}

CDialog_Memo::~CDialog_Memo()
{
	SAFE_DELETE(m_pScrollBar);
//	SAFE_DELETE(m_pUserOuter);
	SAFE_DELETE(m_pChatOuter);
//	SAFE_DELETE(m_pTWFont);
	if(m_pFont) delete m_pFont;
}

void CDialog_Memo::ShowWindow(BOOL bIcon)
{
	m_bIcon = bIcon;
	m_pScrollBar->InitScroll();

	if(m_bIcon) 
	{
		m_bShow = TRUE;

		m_vMemoRect.right = m_arrArea[4]->m_vRect.Width();
		m_vMemoRect.bottom = m_arrArea[4]->m_vRect.Height();
		m_vIconIndex = 5;
	}
	else
	{
		CUIRes::ShowWindow();
		
		m_vMemoRect.right = m_mainRect.Width();
		m_vMemoRect.bottom = m_mainRect.Height();
		m_ptOffset.x = 800-m_mainRect.Width();
	}
}

void CDialog_Memo::HideWindow()
{
	CUIRes::HideWindow();

//	m_pUserOuter->ClearString();
	m_pChatOuter->ClearString();
	m_vUserName.Empty();
}

void CDialog_Memo::SetUserName(CString user_name)
{
	m_vUserName = user_name;
//	m_pUserOuter->SetString(m_vUserName);
}

void CDialog_Memo::AddMessage(BOOL bMe, CString chat)
{
	m_pChatOuter->AddMemo(bMe, chat);
	SetScrollData();

/*	CString t_str;

	if(bMe)
	{
		t_str = "> ";
	}
	else
	{
		t_str = "< ";
	}
	t_str += chat;

	m_ChatString.Add(t_str);

	// 줄 수를 구하기 위해서
	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	CRect t_rect = m_arrArea[1]->m_vRect;
	int line_count;

	pDC->DrawText(str, &t_rect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EDITCONTROL|DT_CALCRECT);
	line_count = t_rect.Height() / 12;

	pDC->SelectObject(old_font);
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);

	m_LineCount.Add(line_count);

//	m_pChatOuter->AddString(t_str);
	BOOL bScroll = m_pScrollBar->IsEndScroll();
	SetScrollData();
	if(bScroll)
	{
		m_pScrollBar->SetEndPos();
	}*/
	if(m_bIcon) m_vIconIndex = 5;
}

BOOL CDialog_Memo::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return FALSE;

	DWORD dwTrans;
	CPoint start_pos;
	CRect rtDst;
	int width;

	if(m_bIcon)
	{
		dwTrans = DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY;
		start_pos = m_ptOffset;
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[m_vIconIndex]->m_vRect, dwTrans);

		CDC *pDC = pDest->GetDC();
		CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
		pDC->SetTextColor(RGB(255,255,255));
		pDC->SetBkMode(TRANSPARENT);

		width = m_arrArea[m_vIconIndex]->m_vRect.Width();
		rtDst.left = rtDst.top = 0;
		rtDst.right = width;
		rtDst.bottom = 12;
		pDC->DrawText(m_vUserName, &rtDst, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_CALCRECT);
		if(rtDst.right < width)
		{
			rtDst.left = start_pos.x + (width-rtDst.right)/2;
			rtDst.right += rtDst.left;
		}
		else
		{
			if(rtDst.right+start_pos.x > 800)
			{
				rtDst.left = 800 - rtDst.right;
				rtDst.right = 800;
			}
			else
			{
				rtDst.left = start_pos.x;
				rtDst.right += start_pos.x;
			}
		}
		rtDst.top = start_pos.y + m_arrArea[m_vIconIndex]->m_vRect.Height() + 2;
		rtDst.bottom = rtDst.top + 12;
		pDC->DrawText(m_vUserName, &rtDst, DT_LEFT|DT_VCENTER|DT_SINGLELINE);

		pDC->SelectObject(old_font);
		pDest->ReleaseDC(pDC);
	}
	else
	{
		CUIRes::Draw(pDest);

		Draw3ButtonCtrl(pDest, 0, 2);
	
		CDC *pDC = pDest->GetDC();
		CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
		pDC->SetBkMode(TRANSPARENT);

		rtDst = m_arrArea[0]->m_vRect;		// 번호
		rtDst.OffsetRect(m_ptOffset);
		pDC->SetTextColor(RGB(255,255,0));
		pDC->DrawText(m_vUserName, &rtDst, DT_LEFT|DT_VCENTER|DT_SINGLELINE);

		// 내용
		m_pChatOuter->SetScrollPos(m_pScrollBar->GetStartLine());
		m_pChatOuter->DrawChat(NULL, m_ptOffset, pDC);

		pDC->SelectObject(old_font);
		pDest->ReleaseDC(pDC);

		m_pMainEditor->DCDrawOffsetedCurText(pDest, m_ptOffset);

		m_pScrollBar->Draw(pDest);
	}

	return TRUE;
}

void CDialog_Memo::SetScrollData()
{
	m_pScrollBar->SetMaxLineCount(m_pChatOuter->GetLineCount());
	m_pScrollBar->SetViewLineCount(m_pChatOuter->GetShowLineNum());
	m_pScrollBar->SetStartLine(m_pChatOuter->GetScrollPos());
/*	int max_count, rect_height;
	CSize t_size;
	int line_gap;

	line_gap = 12 + LINE_GAP;
	rect_height = m_arrArea[1]->m_vRect.Height();//321-65; // 내용을 표시하는 영역의 높이
	max_count = rect_height / line_gap; // 화면에 보이는  ViewRect(내용을 표시하는 영역)에서 보이는 줄의 수.

	m_pScrollBar->SetMaxLineCount(m_pChatOuter->GetLineCount());
	m_pScrollBar->SetViewLineCount(max_count);*/
}

void CDialog_Memo::SetFocus()
{
	m_pMainEditor->SetFocus();
}

BOOL CDialog_Memo::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if(m_pWnd == NULL) return FALSE;

	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");
/*	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		delete m_pTWFont; m_pTWFont=NULL;
		TRACE("####### Dialog Shop Init 실패.. ############ \n");
		return FALSE;
	}

	m_pUserOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pUserOuter->SetSplitRegion(m_arrArea[0]->m_vRect.Width());
	m_pChatOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pChatOuter->SetSplitRegion(m_arrArea[1]->m_vRect.Width());*/
	m_pMainEditor = new BBDX_Editor(m_pWnd, this, FALSE, m_arrArea[2]->m_vRect);
	m_pChatOuter = new BBDX_ChatOuter("굴림", 12, m_arrArea[1]->m_vRect, 500, 2);
	m_pChatOuter->ShowMode(2);		// Custom Mode

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[3]->m_vRect, 0);

	return TRUE;
}

BOOL CDialog_Memo::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_bIcon) return TRUE; 

	if (m_pScrollBar->OnLButtonDown(pMouseInfo) ) {
		SetFocus();
		SetCapture();
		return FALSE;
	}

	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_Memo::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();
	if (m_pScrollBar->OnLButtonUp(pMouseInfo) ) {
		return FALSE;
	}

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_Memo::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if (m_pScrollBar->OnMouseMove(pMouseInfo) ) {
		return FALSE;
	}

	return CUIRes::OnMouseMove(pMouseInfo);
}

BOOL CDialog_Memo::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL retVal = FALSE;

	switch(nChar)
	{
	case VK_RETURN:
		{
			CString chat;

			chat = m_pMainEditor->GetCurEditorText();
			if(chat.IsEmpty()) return TRUE;
			SendChat(chat);
		}
		return TRUE;
	case VK_ESCAPE:
		Dialog_MemoCtrl.HideMemo(this);
		return TRUE;
	case VK_UP:
		m_pChatOuter->ScrollUp();
		retVal = TRUE;
		break;
	case VK_DOWN:
		m_pChatOuter->ScrollDown();
		retVal = TRUE;
		break;
	case VK_PRIOR:
		m_pChatOuter->ScrollPageUp();
		retVal = TRUE;
		break;
	case VK_NEXT:
		m_pChatOuter->ScrollPageDown();
		retVal = TRUE;
		break;
	}

	if(retVal)
	{
		SetScrollData();
		return TRUE;
	}
	else
	{
		if(nChar == VK_TAB || (nChar >= VK_F1 && nChar <= VK_F12))
		{
			m_pWnd->PostMessage(WM_KEYDOWN, (WPARAM)nChar, MAKELPARAM(nFlags, nRepCnt));
		}
	}

	return FALSE;
}

BOOL CDialog_Memo::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// 닫기
			Dialog_MemoCtrl.HideMemo(this);
			break;
		case 1:
			m_pScrollBar->ScrollUp();
			break;
		case 2:
			m_pScrollBar->ScrollDown();
			break;
		}
	}
	else if(nSpec == AREA_CTRL)
	{
		switch(nIndex)
		{
		case 2:
			m_pMainEditor->SetFocus();
			break;
		}
	}

	return TRUE;
}

void CDialog_Memo::SendChat(CString chat)
{
	PBYTE pBuf;
	int offset = 0;
	BYTE type = 2;
	
	pBuf = new BYTE[chat.GetLength()+30];

	CPacketParser::SetByte(pBuf, offset, PKT_CHAT);
	CPacketParser::SetByte(pBuf, offset, type);

	CPacketParser::SetByte(pBuf, offset, (BYTE)m_vUserName.GetLength());// 아이디 이름 길이..
	CPacketParser::SetString(pBuf, offset, (PBYTE)(LPCTSTR)m_vUserName, m_vUserName.GetLength());// 아이디이름.

	CPacketParser::SetString(pBuf, offset, (PBYTE)(LPCTSTR)chat, chat.GetLength());

	CChildView::SocketMng.Send(offset, pBuf);

	SAFE_DELETES(pBuf);
}

int CDialog_Memo::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	if(m_bShow && m_pChild == NULL)
	{
		CPoint pos;

		pos = pMouseInfo->GetPos() - m_ptOffset;
		if(GetCaptureStatus() || m_vMemoRect.PtInRect(pos))
		{
			int ret_val = 2;
			if(pMouseInfo->IsLButtonDown()) {
				OnLButtonDown(pMouseInfo);
				ret_val = 3;
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
			return ret_val;
		}
		else
		{
			if(!m_bIcon && pMouseInfo->IsLButtonDown() && m_pMainEditor == CWnd::GetFocus())
			{
				g_pFocusDialog = NULL;
				m_pWnd->SetFocus();
			}
		}
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

BOOL CDialog_Memo::OnRButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	m_bIcon = !m_bIcon;
	if(m_bIcon)
	{
		m_vMemoRect.right = m_arrArea[4]->m_vRect.Width();
		m_vMemoRect.bottom = m_arrArea[4]->m_vRect.Height();
		g_pFocusDialog = NULL;
		m_pWnd->SetFocus();
	}
	else
	{
		m_vMemoRect.right = m_mainRect.Width();
		m_vMemoRect.bottom = m_mainRect.Height();
		m_vIconIndex = 4;
		if(m_ptOffset.x > 800-m_mainRect.Width()) m_ptOffset.x = 800-m_mainRect.Width();
		g_pFocusDialog = this;
		m_pMainEditor->SetFocus();
	}

	return FALSE;
}

BOOL CDialog_Memo::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_TAB || (nChar >= VK_F1 && nChar <= VK_F12))
	{
		m_pWnd->PostMessage(WM_KEYUP, (WPARAM)nChar, MAKELPARAM(nFlags, nRepCnt));
	}

	return TRUE;
}
