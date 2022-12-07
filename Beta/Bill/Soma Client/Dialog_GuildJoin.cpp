// Dialog_GuildJoin.cpp: implementation of the CDialog_GuildJoin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_GuildJoin.h"
//#include "BBDX_TextOuter.h"
#include "PacketParser.h"
#include "ChildView.h"
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

CDialog_GuildJoin::CDialog_GuildJoin()
{
//	INIT_PTR(m_pPersonOuter);
	INIT_PTR(m_pScrollBar);
	INIT_PTR(m_pFont);
//	INIT_PTR(m_pTWFont);

	m_bShow = FALSE;
	m_vSelIndex = -1;
	m_bProcessing = FALSE;
}

CDialog_GuildJoin::~CDialog_GuildJoin()
{
	SAFE_DELETE(m_pScrollBar);
	SAFE_DELETE(m_pFont);
//	SAFE_DELETE(m_pTWFont);
//	SAFE_DELETE(m_pPersonOuter);
}

void CDialog_GuildJoin::DeleteContents()
{
	m_arrPerson.RemoveAll();
	m_arrInfo.RemoveAll();
	m_arrInfoFlag.RemoveAll();
}

void CDialog_GuildJoin::SendJoinPerson()
{
	BYTE *buf;
	int offset = 0;
	BYTE len;

	if(m_vSelIndex < 0) return;

	m_bProcessing = TRUE;
	m_vProcessIndex = m_vSelIndex;
	len = m_arrPerson[m_vSelIndex].GetLength();
	buf = new BYTE[10+len];
	CPacketParser::SetByte(buf, offset, PKT_GUILD_MOVE_REQ); //최종점 요청

	CPacketParser::SetByte(buf, offset, len);
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)m_arrPerson[m_vProcessIndex], len);	// 가입시킬 유저 이름

	CChildView::SocketMng.Send(offset, buf);

	delete[] buf;
}

void CDialog_GuildJoin::SendRejectPerson()
{
	BYTE *buf;
	int offset = 0;
	BYTE len;

	if(m_vSelIndex < 0) return;

	m_bProcessing = TRUE;
	m_vProcessIndex = m_vSelIndex;
	len = m_arrPerson[m_vSelIndex].GetLength();
	buf = new BYTE[10+len];
	CPacketParser::SetByte(buf, offset, PKT_GUILD_MOVE_REJECT); //최종점 요청

	CPacketParser::SetByte(buf, offset, len);
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)m_arrPerson[m_vProcessIndex], len);	// 가입시킬 유저 이름

	CChildView::SocketMng.Send(offset, buf);

	delete[] buf;
}

BOOL CDialog_GuildJoin::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex) {
		case 0:	// 가입 허가
			if(m_bProcessing == FALSE) SendJoinPerson();
			break;
		case 1:	// 가입 거절
			if(m_bProcessing == FALSE) SendRejectPerson();
			break;
		case 2: // 스크롤업
			m_pScrollBar->ScrollUp();
			break;
		case 3: // 스크롤다운
			m_pScrollBar->ScrollDown();
			break;
		case 4:	// 닫기
			HideWindow();
			break;
		}
	}
	else if(nSpec == AREA_CTRL && m_bProcessing == FALSE)
	{
		int index, count;

		index = m_pScrollBar->GetStartLine() + nIndex - 1;
		count = m_arrPerson.GetSize();
		if(index >= 0 && index < count)
		{
			m_vSelIndex = index;
			if(m_arrInfoFlag[m_vSelIndex] == 0)
			{
				SendAskUserInfo();
			}
		}
	}

	return TRUE;
}

BOOL CDialog_GuildJoin::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CRect rtDst;
	int person_start, i, j, person_count;
	
	Draw3ButtonCtrl(pDest, 0, 4);

	person_count = m_arrPerson.GetSize();
	if(person_count > 0)
	{
		CDC *pDC = pDest->GetDC();
		CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
		pDC->SetBkMode(TRANSPARENT);

		person_start = m_pScrollBar->GetStartLine();
		for(i = person_start, j = 1; i < person_count && j <= 10; i++, j++)
		{
			rtDst = m_arrArea[j]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
//			m_pPersonOuter->SetString(m_arrPerson[i]);
			if(i == m_vSelIndex)
			{
				pDC->SetTextColor(RGB(255,255,0));
				pDC->DrawText(m_arrPerson[i], &rtDst, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
//				m_pPersonOuter->DrawShadowTexts(pDest, rtDst, 0xffe0, 0);
			}
			else
			{
				pDC->SetTextColor(RGB(255,255,255));
				pDC->DrawText(m_arrPerson[i], &rtDst, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
//				m_pPersonOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
			}
		}

		pDC->SelectObject(old_font);
		pDest->ReleaseDC(pDC);
	}

	m_pScrollBar->Draw(pDest);

	if(m_vSelIndex >= 0 && m_arrInfo[m_vSelIndex].IsEmpty() == FALSE)
	{
		int t_index = m_vSelIndex - m_pScrollBar->GetStartLine() + 1;
		if(t_index >= 1 && t_index <= 10)
		{
			CPoint t_pos;
			t_pos.x = m_arrArea[t_index]->m_vRect.right;
			t_pos.y = m_arrArea[t_index]->m_vRect.top;
			t_pos.Offset(m_ptOffset);
			ViewCaption(pDest, t_pos, m_arrInfo[m_vSelIndex], 11);
		}
	}
	return TRUE;
}

void CDialog_GuildJoin::ShowWindow(int Id, CUIRes *pParent)
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Guild-J.lib", 0, TRUE, FALSE, FALSE))
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

	m_vSelIndex = -1;
	m_vId = Id;
	m_pParent = pParent;
	if(m_pParent) m_pParent->m_pChild = this;
	m_pScrollBar->InitScroll();
	CUIRes::ShowWindow();
}

void CDialog_GuildJoin::HideWindow()
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

void CDialog_GuildJoin::HideAllWindow()
{
	CUIRes::HideWindow();

	if(m_pParent)
	{
		m_pParent->m_pChild = NULL;
		m_pParent->HideAllWindow();
	}
}

void CDialog_GuildJoin::DialogOpen(PBYTE bypData)
{
	int offset = 0;
	short count;
	BYTE len;
	CString t_str, t_info;

	DeleteContents();

	count = CPacketParser::GetShort(bypData, offset);

	for(int i = 0; i < count; i++)
	{
		len = CPacketParser::GetByte(bypData, offset);
		CPacketParser::GetString(bypData, offset, (PBYTE)t_str.GetBuffer(len), len);
		t_str.ReleaseBuffer();
		t_str.MakeUpper();
		AddPerson(t_str);
	}

	SetScrollData();
	m_vSelIndex = -1;
}

void CDialog_GuildJoin::SetScrollData()
{
	int count = m_arrPerson.GetSize();
	m_pScrollBar->SetMaxLineCount(count);
	m_pScrollBar->SetViewLineCount(10);
	m_pScrollBar->SetStartLine(m_pScrollBar->GetStartLine());
}

BOOL CDialog_GuildJoin::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

/*	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		delete m_pTWFont; m_pTWFont=NULL;
		TRACE("####### Dialog GuildJoin Init 실패.. ############ \n");
		return FALSE;
	}

	m_pPersonOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);*/
	m_pFont = new CFont;
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[0]->m_vRect, 0);
		
	return TRUE;
}

BOOL CDialog_GuildJoin::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}

	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_GuildJoin::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();
	if(m_pScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_GuildJoin::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;

	return CUIRes::OnMouseMove(pMouseInfo);
}

void CDialog_GuildJoin::RcvJoinPerson(PBYTE bypData)
{
	int offset = 0;

	if(m_bShow == FALSE) return;

	m_vSelIndex = -1;
	m_bProcessing = FALSE;
	BYTE bResult = CPacketParser::GetByte(bypData, offset);
	if(bResult == 1)
	{
		m_arrPerson.RemoveAt(m_vProcessIndex);
		m_arrInfo.RemoveAt(m_vProcessIndex);
		m_arrInfoFlag.RemoveAt(m_vProcessIndex);
		SetScrollData();
	}
	else
	{
		CString message;
		BYTE sub_result;

		sub_result = CPacketParser::GetByte(bypData, offset);

		switch(sub_result)
		{
		case GUILD_NOT_GUILD_USER:
			message.LoadString(IDS_NOT_GUILD_USER);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		case GUILD_SMALL_RANK:
			message.LoadString(IDS_SMALL_RANK);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		case GUILD_ABSENT_REQ_USER:
			message.LoadString(IDS_ABSENT_REQ_USER);
			break;
		case GUILD_ABSENT_GUILD_NAME:
			message.LoadString(IDS_ABSENT_GUILD_NAME);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		case GUILD_ALREADY_JOIN:
			message.LoadString(IDS_ALREADY_JOIN);
			break;
		case GUILD_SYSTEM_ERROR:
		default:
			message.LoadString(IDS_JOIN_SYSTEM_ERROR);
			break;
		}
		Dialog_Message.ShowWindow(255, this);
		Dialog_Message.SetMessage(message);
	}
}

void CDialog_GuildJoin::RcvRejectPerson(PBYTE bypData)
{
	int offset = 0;

	if(m_bShow == FALSE) return;

	m_vSelIndex = -1;
	m_bProcessing = FALSE;
	BYTE bResult = CPacketParser::GetByte(bypData, offset);
	if(bResult == 1)
	{
		m_arrPerson.RemoveAt(m_vProcessIndex);
		m_arrInfo.RemoveAt(m_vProcessIndex);
		m_arrInfoFlag.RemoveAt(m_vProcessIndex);
		SetScrollData();
	}
	else
	{
		CString message;
		BYTE sub_result;

		sub_result = CPacketParser::GetByte(bypData, offset);

		switch(sub_result)
		{
		case GUILD_NOT_GUILD_USER:
			message.LoadString(IDS_NOT_GUILD_USER);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		case GUILD_SMALL_RANK:
			message.LoadString(IDS_SMALL_RANK);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		case GUILD_ABSENT_JOIN_REQ:
			message.LoadString(IDS_ABSENT_JOIN_REQ);
			break;
		case GUILD_ABSENT_GUILD_NAME:
			message.LoadString(IDS_ABSENT_GUILD_NAME);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		case GUILD_ABSENT_REQ_USER:
			message.LoadString(IDS_ABSENT_REQ_USER);
			break;
		case GUILD_ALREADY_JOIN:
			message.LoadString(IDS_ALREADY_JOIN);
			break;
		default:
			message.LoadString(IDS_JOIN_SYSTEM_ERROR);
			break;
		}
		Dialog_Message.ShowWindow(255, this);
		Dialog_Message.SetMessage(message);
	}
}

void CDialog_GuildJoin::SendAskUserInfo()
{
	BYTE *buf;
	int offset = 0;
	BYTE len;

	if(m_vSelIndex < 0) return;

	m_bProcessing = TRUE;
	m_vProcessIndex = m_vSelIndex;
	len = m_arrPerson[m_vProcessIndex].GetLength();
	buf = new BYTE[10 + len];

	CPacketParser::SetByte(buf, offset, PKT_GUILD_REQ_USER_INFO); //최종점 요청
	CPacketParser::SetByte(buf, offset, len);			// 길드 이름 길이
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)m_arrPerson[m_vProcessIndex], len);

	CChildView::SocketMng.Send(offset, buf);

	delete[] buf;
}

void CDialog_GuildJoin::RcvAskUserInfo(PBYTE bypData)
{
	int offset = 0;
	BYTE len;

	if(m_bShow == FALSE) return;

	m_arrInfoFlag[m_vProcessIndex] = 1;

	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		LPTSTR t_str = m_arrInfo[m_vProcessIndex].GetBuffer(len);
		CPacketParser::GetString(bypData, offset, (PBYTE)t_str, len);
		m_arrInfo[m_vProcessIndex].ReleaseBuffer();
	}
	else
	{
		CString message;

		BYTE sub_result = CPacketParser::GetByte(bypData, offset);
		switch(sub_result)
		{
		case GUILD_NOT_GUILD_USER:
			message.LoadString(IDS_NOT_GUILD_USER);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		case GUILD_ABSENT_REQ_USER:
			message.LoadString(IDS_ABSENT_REQ_USER);
			break;
		case GUILD_ABSENT_GUILD_NAME:	
			message.LoadString(IDS_ABSENT_GUILD_NAME);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		default:
			message.LoadString(IDS_ASKUSER_UNKNOWN_ERR);
			break;
		}
		Dialog_Message.ShowWindow(255, this);
		Dialog_Message.SetMessage(message);
	}
	m_bProcessing = FALSE;
}

void CDialog_GuildJoin::DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index)
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

void CDialog_GuildJoin::ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index)
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

void CDialog_GuildJoin::AddPerson(CString m_name)
{
	int i, t_count;
	BOOL retVal = FALSE;
	CString t_str;

	t_count = m_arrPerson.GetSize();

	for(i = 0; i < t_count; i++)
	{
		if(m_arrPerson[i].CompareNoCase(m_name) > 0)
		{
			m_arrPerson.InsertAt(i, m_name);
			m_arrInfo.InsertAt(i, t_str);
			m_arrInfoFlag.InsertAt(i, (BYTE)0);
			retVal = TRUE;
			break;
		}
	}

	if(retVal == FALSE)
	{
		m_arrPerson.InsertAt(i, m_name);
		m_arrInfo.InsertAt(i, t_str);
		m_arrInfoFlag.InsertAt(i, (BYTE)0);
	}
}