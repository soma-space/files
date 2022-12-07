// Dialog_GuildApply.cpp: implementation of the CDialog_GuildApply class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_GuildApply.h"
//#include "BBDX_TextOuter.h"
#include "PacketParser.h"
#include "ChildView.h"
#include "DManage/BBDX_MouseInformation.h"
//#include "TWFont.h"
#include "Dialog_Message.h"
#include "GuildDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CUIRes *g_pShowDialog;
extern CDialog_Message Dialog_Message;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_GuildApply::CDialog_GuildApply()
{
	INIT_PTR(m_pScrollBar);
	INIT_PTR(m_pFont);
//	INIT_PTR(m_pTWFont);

	InitData();
}

CDialog_GuildApply::~CDialog_GuildApply()
{
	SAFE_DELETE(m_pScrollBar);
	SAFE_DELETE(m_pFont);
//	SAFE_DELETE(m_pTWFont);
	
	DeleteContents();
}

void CDialog_GuildApply::DeleteContents()
{
	m_GuildName.RemoveAll();
	m_GuildDesc.RemoveAll();
	m_GuildDescFlag.RemoveAll();
	m_vGuildCount = 0;
}

void CDialog_GuildApply::SendApplyGuild()
{
	BYTE *buf;
	int offset = 0;
	BYTE len;
	CString t_str;

	if(m_vSelIndex < 0) return;

	m_bProcessing = TRUE;
	t_str = m_GuildName[m_vSelIndex];
	len = t_str.GetLength();
	buf = new BYTE[10 + len];
	CPacketParser::SetByte(buf, offset, PKT_GUILD_REQ); //최종점 요청

	CPacketParser::SetByte(buf, offset, len);
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)t_str, len);

	CChildView::SocketMng.Send(offset, buf);

	delete[] buf;
}

BOOL CDialog_GuildApply::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex) {
		case 0:	// 신청
			if(m_bProcessing == FALSE) SendApplyGuild();
			break;
		case 1:	// 닫기
			HideWindow();
			break;
		case 2: // 스크롤업
			m_pScrollBar->ScrollUp();
			break;
		case 3: // 스크롤다운
			m_pScrollBar->ScrollDown();
			break;
		}
	}
	else if(nSpec == AREA_CTRL && m_bProcessing == FALSE)
	{
		int index;

		index = m_pScrollBar->GetStartLine() + nIndex - 1;
		if(index >= 0 && index < m_vGuildCount)
		{
			m_vSelIndex = index;
			if(m_GuildDescFlag[m_vSelIndex] == 0)
			{
				SendAskGuildDesc();
			}
		}
		else m_vSelIndex = -1;
	}

	return TRUE;
}

BOOL CDialog_GuildApply::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CRect rtDst;
	int guild_start, i, j;
	
	Draw3ButtonCtrl(pDest, 0, 3);

	if(m_vGuildCount > 0)
	{
		CDC *pDC = pDest->GetDC();
		CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
		pDC->SetBkMode(TRANSPARENT);

		guild_start = m_pScrollBar->GetStartLine();
		for(i = guild_start, j = 1; i < m_vGuildCount && j <= 10; i++,j++)
		{
			rtDst = m_arrArea[j]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			if(i == m_vSelIndex)
			{
				pDC->SetTextColor(RGB(255,255,0));
				pDC->DrawText(m_GuildName[i], &rtDst, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
			}
			else
			{
				pDC->SetTextColor(RGB(255,255,255));
				pDC->DrawText(m_GuildName[i], &rtDst, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
			}
		}

		pDC->SelectObject(old_font);
		pDest->ReleaseDC(pDC);
	}

	m_pScrollBar->Draw(pDest);

	if(m_vSelIndex >= 0 && m_GuildDescFlag[m_vSelIndex])
	{
		int t_index = m_vSelIndex - m_pScrollBar->GetStartLine() + 1;
		if(t_index >= 1 && t_index <= 10)
		{
			CPoint t_pos;
			t_pos.x = m_arrArea[t_index]->m_vRect.right;
			t_pos.y = m_arrArea[t_index]->m_vRect.top;
			t_pos.Offset(m_ptOffset);
			ViewCaption(pDest, t_pos, m_GuildDesc[m_vSelIndex], 11);
		}
	}
	return TRUE;
}

void CDialog_GuildApply::ShowWindow(int Id, CUIRes *pParent)
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Guild-A.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
		CPoint t_pos;
		t_pos.x = (800 - m_mainRect.Width()) / 2;
		t_pos.y = (600 - m_mainRect.Height()) / 2;
		SetOffset(t_pos);
	}

	if(g_pShowDialog && g_pShowDialog != pParent) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	InitData();
	m_vId = Id;
	m_pParent = pParent;
	if(m_pParent) m_pParent->m_pChild = this;
	m_pScrollBar->InitScroll();
	CUIRes::ShowWindow();
}

void CDialog_GuildApply::HideWindow()
{
	CUIRes::HideWindow();

	if(m_pParent)
	{
		m_pParent->m_pChild = NULL;
		if(m_pParent->IsVisible())
		{
			g_pShowDialog = m_pParent;
			m_pParent->SetFocus();
		}
		else g_pShowDialog = NULL;
	}
	else g_pShowDialog = NULL;

	DeleteContents();
}

void CDialog_GuildApply::HideAllWindow()
{
	CUIRes::HideWindow();

	if(m_pParent)
	{
		m_pParent->m_pChild = NULL;
		m_pParent->HideAllWindow();
	}
}

void CDialog_GuildApply::DialogOpen(PBYTE bypData)
{
	int offset;

	DeleteContents();

	offset = 0;

	m_vGuildCount = (int)CPacketParser::GetShort(bypData, offset);
	if(m_vGuildCount > 0)
	{
		BYTE len;
		CString t_str;
		
		for(int i = 0; i < m_vGuildCount; i++)
		{
			// 길드 이름
			len = CPacketParser::GetByte(bypData, offset);
			CPacketParser::GetString(bypData, offset, (BYTE *)t_str.GetBuffer(len), len);
			t_str.ReleaseBuffer();
			t_str.MakeUpper();
			AddGuild(t_str);
		}
	}

	m_pScrollBar->SetMaxLineCount(m_vGuildCount);
	m_pScrollBar->SetViewLineCount(10);
	m_pScrollBar->SetStartLine(0);

	m_vSelIndex = -1;
}

BOOL CDialog_GuildApply::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

/*	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		delete m_pTWFont; m_pTWFont=NULL;
		TRACE("####### Dialog GuildApply Init 실패.. ############ \n");
		return FALSE;
	}*/
	m_pFont = new CFont;
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[0]->m_vRect, 0);
		
	return TRUE;
}

BOOL CDialog_GuildApply::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}

	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_GuildApply::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();
	if(m_pScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_GuildApply::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;

	return CUIRes::OnMouseMove(pMouseInfo);
}

void CDialog_GuildApply::RcvApplyGuild(PBYTE bypData)
{
	int offset = 0;

	if(m_bShow == FALSE) return;

	BYTE bResult = CPacketParser::GetByte(bypData, offset);		// 성공/실패
	CString message;
	if(bResult == 1)		// 성공
	{
		message.LoadString(IDS_SUC_JOINGUILD);
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(message);
	}
	else					// 실패
	{
		BYTE sub_result;

		sub_result = CPacketParser::GetByte(bypData, offset);
		switch(sub_result)
		{
		case GUILD_ALREADY_JOIN:
			message.LoadString(IDS_ALREADY_JOIN);
			break;
		case GUILD_ALREADY_JOIN_REQ:
			message.LoadString(IDS_ALREADY_JOIN_REQ);
			break;
		case GUILD_ABSENT_GUILD_NAME:
			message.LoadString(IDS_ABSENT_GUILD_NAME);
			break;
		case GUILD_SYSTEM_ERROR:
		default:
			message.LoadString(IDS_GUILDREQ_SYSTEM_ERROR);
			break;
		}
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(message);
	}
	HideAllWindow();
}

void CDialog_GuildApply::DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index)
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

void CDialog_GuildApply::ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index)
{
	int def_width = 200;
	CRect t_rect;
	CFont pFont, *old_font;
	CDC *pDC = pDest->GetDC();

	old_font = pDC->SelectObject(m_pFont);
	t_rect.SetRect(0, 0, def_width, 14);
	pDC->DrawText(str, &t_rect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_CALCRECT|DT_EDITCONTROL);
	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	t_rect.OffsetRect(pos);

	DrawTipRect(pDest, t_rect, area_index);

	pDC = pDest->GetDC();
	pDC->SetBkMode(TRANSPARENT);

	old_font = pDC->SelectObject(m_pFont);

	// 검은색
	pDC->SetTextColor(RGB(5,5,5));
	pDC->DrawText(str, &t_rect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EDITCONTROL);
	
	t_rect.OffsetRect(-1, -1);

	pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(str, &t_rect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EDITCONTROL);

	pDC->SelectObject(old_font);

	pDest->ReleaseDC(pDC);
}

void CDialog_GuildApply::SendAskGuildDesc()
{
	BYTE *buf;
	int offset = 0;
	BYTE len;

	if(m_vSelIndex < 0) return;

	m_bProcessing = TRUE;
	m_vProcessingIndex = m_vSelIndex;
	len = m_GuildName[m_vSelIndex].GetLength();
	buf = new BYTE[10 + len];

	CPacketParser::SetByte(buf, offset, PKT_GUILD_INFO); //최종점 요청
	CPacketParser::SetByte(buf, offset, len);			// 길드 이름 길이
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)m_GuildName[m_vSelIndex], len);

	CChildView::SocketMng.Send(offset, buf);

	delete[] buf;
}

void CDialog_GuildApply::RcvGuildDesc(PBYTE bypData)
{
	int offset = 0;
	BYTE len;

	if(m_bShow == FALSE) return;

	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		LPTSTR t_str = m_GuildDesc[m_vProcessingIndex].GetBuffer(len);
		CPacketParser::GetString(bypData, offset, (PBYTE)t_str, len);
		m_GuildDesc[m_vProcessingIndex].ReleaseBuffer();
	}
	m_GuildDescFlag[m_vProcessingIndex] = 1;
	m_bProcessing = FALSE;
}

void CDialog_GuildApply::InitData()
{
	m_vSelIndex = -1;
	m_vGuildCount = 0;
	m_bProcessing = FALSE;
}

void CDialog_GuildApply::AddGuild(CString m_name)
{
	int i, t_count;
	BOOL retVal = FALSE;
	CString t_str;

	t_str.LoadString(IDS_EMPTY_GUILD_DESC);
	t_count = m_GuildName.GetSize();

	for(i = 0; i < t_count; i++)
	{
		if(m_GuildName[i].CompareNoCase(m_name) > 0)
		{
			m_GuildName.InsertAt(i, m_name);
			m_GuildDesc.InsertAt(i, t_str);
			m_GuildDescFlag.InsertAt(i, (BYTE)0);
			retVal = TRUE;
			break;
		}
	}

	if(retVal == FALSE)
	{
		m_GuildName.Add(m_name);
		m_GuildDesc.InsertAt(i, t_str);
		m_GuildDescFlag.InsertAt(i, (BYTE)0);
	}
}