// BBDX_ChatOuter.cpp: implementation of the BBDX_ChatOuter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Soma.h"
#include "DManage/BB_Direct.h"
#include "ChildView.h"
#include "BBDX_ChatOuter.h"
#include "Dialog_Chat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const COLORREF COLOR_CKN = RGB(255,255,255);
const COLORREF COLOR_CKF = RGB(0,255,0);
const COLORREF COLOR_CKG = RGB(192,192,192);
const COLORREF COLOR_CKT = RGB(0,255,255);
const COLORREF COLOR_MSG = RGB(255,255,0);
const COLORREF COLOR_NTC = RGB(255,0,255);

extern DWORD	g_vRenderTime;
extern CDialog_Chat		Dialog_Chat;

#define SHOW_NORMAL			0
#define SHOW_RIGHT			1
#define	SHOW_DIALOG			2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BBDX_ChatOuter::BBDX_ChatOuter(CString fontname, int height, CRect area, int maxLine, int lineGap)
{
	m_vCharHeight = height;
	m_vAreaRect = area;
	m_vMaxLine = maxLine;
	m_vLineGap = lineGap;
	m_iShowLineNum = 0;
	m_iScrollPos = 0;
	m_vOldScrollPos = 0;

//	m_pMyFont = new CFont();
//	m_pMyFont->CreateFont(-height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
//		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, fontname);

	m_ChatColor[COCK_NORMAL] = COLOR_CKN;
	m_ChatColor[COCK_FAMILY] = COLOR_CKF;
	m_ChatColor[COCK_GLOBAL] = COLOR_CKG;
	m_ChatColor[COCK_TOSOMEONE] = COLOR_CKT;
	m_ChatColor[COCK_MESSAGE] = COLOR_MSG;
	m_ChatColor[COCK_NOTICE] = COLOR_CKT; 
	m_iCount = 0;
	m_vRealShowLineNum = CalcShowLineNum();
	m_vEndLine = 0;
	m_vChatTime = 0;
	m_vShowMode = SHOW_NORMAL;
}

BBDX_ChatOuter::~BBDX_ChatOuter()
{
//	delete m_pMyFont;

	m_ChatList.RemoveAll();
	m_ChatKindList.RemoveAll();
	m_ChatLineCount.RemoveAll();
}

int BBDX_ChatOuter::AddChatWithKind(CString &str, int chatKind)
{
	int line_count;
	int chat_count;

	chat_count = m_ChatLineCount.GetSize();
	m_ChatList.AddTail(str);
	m_ChatKindList.AddTail(chatKind);

	// 줄 수를 구하기 위해서
	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(CChildView::pAppFont);
	CRect t_rect = m_vAreaRect;

	pDC->DrawText(str, &t_rect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EDITCONTROL|DT_CALCRECT);
	line_count = t_rect.Height() / m_vCharHeight;

	pDC->SelectObject(old_font);
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);
	
	m_ChatLineCount.Add(line_count);
	if(chat_count >= m_vMaxLine)
	{
		m_ChatList.RemoveHead();
		m_ChatKindList.RemoveHead();
		m_ChatLineCount.RemoveAt(0);

		if(chat_count == m_vEndLine)
		{
			int t_count = 0;

			for(int i = m_vEndLine-1; i >= 0; i--)
			{
				t_count += m_ChatLineCount[i];
				if(t_count > m_vRealShowLineNum)
				{
					if(m_iScrollPos < i+1) m_iScrollPos = i+1;
					TRACE1("1 - %d\n", m_iScrollPos);
					break;
				}
			}
		}
	}
	else
	{
		if(chat_count == m_vEndLine)
		{
			int t_count = 0;

			m_vEndLine++;
			for(int i = m_vEndLine-1; i >= 0; i--)
			{
				t_count += m_ChatLineCount[i];
				if(t_count > m_vRealShowLineNum)
				{
					if(m_iScrollPos < i+1) m_iScrollPos = i+1;
					TRACE1("2 - %d\n", m_iScrollPos);
					break;
				}
			}
		}
	}

	m_iCount = (m_iCount+1) % 10000;
	if (m_iCount==0) ResetStrLists();
	else if (m_iCount==5000) ResetIntLists();

	m_vChatTime = GetTickCount();

	return 0;
}

void BBDX_ChatOuter::ResetIntLists()
{
	POSITION pos = m_ChatKindList.GetHeadPosition();
	CList <int , int> tmpIntList;
	while (pos)
	{
		tmpIntList.AddTail(m_ChatKindList.GetNext(pos));
	}
	m_ChatKindList.RemoveAll();
	pos = tmpIntList.GetHeadPosition();
	while (pos)
	{
		m_ChatKindList.AddTail(tmpIntList.GetNext(pos));
	}
	tmpIntList.RemoveAll();

	CArray<int, int> t_array;
	int count = m_ChatLineCount.GetSize();
	t_array.SetSize(count);
	for(int i = 0; i < count; i++)
	{
		t_array[i] = m_ChatLineCount[i];
	}
	m_ChatLineCount.RemoveAll();
	m_ChatLineCount.SetSize(count);
	for(i = 0; i < count; i++)
	{
		m_ChatLineCount[i] = t_array[i];
	}
	t_array.RemoveAll();
}

void BBDX_ChatOuter::ResetStrLists()
{
	POSITION pos = m_ChatList.GetHeadPosition();
	CStringList tmpStrList;
	while (pos)
	{
		tmpStrList.AddTail(m_ChatList.GetNext(pos));
	}
	m_ChatList.RemoveAll();
	pos = tmpStrList.GetHeadPosition();
	while (pos)
	{
		m_ChatList.AddTail(tmpStrList.GetNext(pos));
	}
	tmpStrList.RemoveAll();
}

void BBDX_ChatOuter::DrawChat(CDDSurface *dest, CPoint offset, CDC *pdc)
{
	BOOL bRelease;
	CDC *pDC;

	CRect r = m_vAreaRect;
	r.OffsetRect(offset);

	int count = m_ChatList.GetCount();
	if(count == 0) return;

	if(m_vShowMode != SHOW_DIALOG)
	{
		if(g_vRenderTime - m_vChatTime > 5000)
		{
			m_vChatTime = g_vRenderTime;
			ScrollDown();
		}
		if(m_iScrollPos >= count) return;
	}

	POSITION pos = m_ChatList.FindIndex(m_iScrollPos);
	POSITION ck_pos = m_ChatKindList.FindIndex(m_iScrollPos);
	
	if(pdc)
	{
		pDC = pdc;
		bRelease = FALSE;
	}
	else
	{
		pDC = dest->GetDC();
		bRelease = TRUE;
	}
	CFont *old_font = (CFont *)pDC->SelectObject(CChildView::pAppFont);
	pDC->SetBkMode(TRANSPARENT);

	CRect curRect;
	CString t_str;

	curRect = r;
	for (int i = m_iScrollPos; i < count; i++)
	{
		t_str = m_ChatList.GetNext(pos);
		pDC->DrawText(t_str, &curRect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EDITCONTROL|DT_CALCRECT);
		if(m_vShowMode != SHOW_DIALOG)
		{
			CRect t_rect = curRect;
			t_rect.OffsetRect(1, 1);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->DrawText(t_str, &t_rect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EDITCONTROL);
		}
		pDC->SetTextColor(m_ChatColor[m_ChatKindList.GetNext(ck_pos)]);
		pDC->DrawText(t_str, &curRect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EDITCONTROL);
		
		curRect.top = curRect.bottom + m_vLineGap;
		curRect.bottom = r.bottom;
		curRect.left = r.left;
		curRect.right = r.right;
		if(curRect.bottom - curRect.top < m_vCharHeight)
		{
			i++;
			break;
		}
	}

	pDC->SelectObject(old_font);
	if(bRelease)
	{
		dest->ReleaseDC(pDC);
	}

	if(curRect.bottom - curRect.top < m_vCharHeight*2)
	{
		m_vEndLine = i;
	}
	else m_vEndLine = i+1;
}

void BBDX_ChatOuter::ChatAreaChange(CRect r)
{
	m_vAreaRect = r;
	m_iScrollPos = 0;
	TRACE1("3 - %d\n", m_iScrollPos);
	m_iShowLineNum = 0;
	m_vRealShowLineNum = CalcShowLineNum();

	int t_count, i;
	int width = m_vAreaRect.Width();
	int count = m_ChatLineCount.GetSize();
	CRect t_rect;
	m_ChatLineCount.RemoveAll();
	m_ChatLineCount.SetSize(count);

	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(CChildView::pAppFont);

	i = 0;
	POSITION pos = m_ChatList.GetHeadPosition();
	while(pos)
	{
		t_rect = m_vAreaRect;
		pDC->DrawText(m_ChatList.GetNext(pos), &t_rect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EDITCONTROL|DT_CALCRECT);
		m_ChatLineCount[i++] = t_rect.Height() / m_vCharHeight;
	}

	pDC->SelectObject(old_font);
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);

	t_count = 0;
	m_vEndLine = count;
	for(i = m_vEndLine-1; i >= 0; i--)
	{
		t_count += m_ChatLineCount[i];
		if(t_count > m_vRealShowLineNum)
		{
			m_iScrollPos = i+1;
			TRACE1("4 - %d\n", m_iScrollPos);
			break;
		}
	}
}

int BBDX_ChatOuter::CalcShowLineNum()
{
	int linenum;

	int height = m_vAreaRect.Height();
	int lineHeight = m_vCharHeight+m_vLineGap;
	
	linenum = (height+m_vLineGap)/lineHeight;

	return linenum;
}

void BBDX_ChatOuter::SetScrollPos(int pos)
{
	int scrollRange = m_ChatList.GetCount()-m_iShowLineNum;

	if(scrollRange <= 0) return;
	if(pos < 0)
	{
		m_iScrollPos = 0;
		TRACE1("5 - %d\n", m_iScrollPos);
		return;
	}
	else if(pos >= scrollRange)
	{
		m_iScrollPos = scrollRange;
		TRACE1("6 - %d\n", m_iScrollPos);
		return;
	}
	else
	{
		m_iScrollPos = pos;
		TRACE1("7 - %d\n", m_iScrollPos);
	}
}

void BBDX_ChatOuter::ScrollUp()
{
	if (m_iScrollPos>0)
	{
		m_iScrollPos--;
		TRACE1("8 - %d\n", m_iScrollPos);
	}
}

void BBDX_ChatOuter::ScrollDown()
{
	int count = m_ChatList.GetCount();
	int scrollRange = count-m_iShowLineNum;
	if (scrollRange<=0) return;
	if (m_iScrollPos<scrollRange)
	{
		m_iScrollPos ++;
		TRACE1("9 - %d\n", m_iScrollPos);
	}
}

BOOL BBDX_ChatOuter::ScrollPageDown()
{
	int count = m_ChatList.GetCount();
	int scrollRange = count-m_iShowLineNum;
	if (scrollRange<=0) return FALSE;
	if (m_iScrollPos+10<count)	
	{
		m_iScrollPos += 10;
		TRACE1("10 - %d\n", m_iScrollPos);
		return TRUE;
	}
	return FALSE;
}

void BBDX_ChatOuter::ScrollPageUp()
{
	m_iScrollPos -= 10;
	if (m_iScrollPos<0) m_iScrollPos = 0;
	TRACE1("11 - %d\n", m_iScrollPos);
}

void BBDX_ChatOuter::ClearString()
{
	m_ChatList.RemoveAll();
	m_ChatKindList.RemoveAll();
	m_ChatLineCount.RemoveAll();
	m_iScrollPos = 0;
	TRACE1("12 - %d\n", m_iScrollPos);
}

int BBDX_ChatOuter::AddMemo(BOOL bMe, CString chat)
{
	CString t_str;
	int line_count;
	int chat_count;

	if(bMe)
	{
		t_str = "> ";
	}
	else
	{
		t_str = "< ";
	}
	t_str += chat;

	chat_count = m_ChatLineCount.GetSize();
	m_ChatList.AddTail(t_str);
	m_ChatKindList.AddTail(COCK_NORMAL);

	// 줄 수를 구하기 위해서
	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(CChildView::pAppFont);
	CRect t_rect = m_vAreaRect;

	pDC->DrawText(t_str, &t_rect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EDITCONTROL|DT_CALCRECT);
	line_count = t_rect.Height() / m_vCharHeight;

	pDC->SelectObject(old_font);
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);
	
	m_ChatLineCount.Add(line_count);
	if(chat_count >= m_vMaxLine)
	{
		m_ChatList.RemoveHead();
		m_ChatKindList.RemoveHead();
		m_ChatLineCount.RemoveAt(0);

		if(chat_count == m_vEndLine)
		{
			int t_count = 0;

			for(int i = m_vEndLine-1; i >= 0; i--)
			{
				t_count += m_ChatLineCount[i];
				if(t_count > m_vRealShowLineNum)
				{
					if(m_iScrollPos < i+1) m_iScrollPos = i+1;
					break;
				}
			}
		}
	}
	else
	{
		if(chat_count == m_vEndLine)
		{
			int t_count = 0;

			m_vEndLine++;
			for(int i = m_vEndLine-1; i >= 0; i--)
			{
				t_count += m_ChatLineCount[i];
				if(t_count > m_vRealShowLineNum)
				{
					if(m_iScrollPos < i+1) m_iScrollPos = i+1;
					break;
				}
			}
		}
	}

	m_iCount = (m_iCount+1) % 10000;
	if (m_iCount==0) ResetStrLists();
	else if (m_iCount==5000) ResetIntLists();

	return 0;
}

void BBDX_ChatOuter::ShowMode(int show_mode)
{
	if(m_vShowMode == show_mode) return;

	CRect t_rect;
	switch(show_mode)
	{
	case SHOW_NORMAL:		// 일반 모드
		t_rect.SetRect(30, 10, 770, 100);
		if(m_vShowMode != SHOW_DIALOG) m_vOldScrollPos = m_iScrollPos;
		ChatAreaChange(t_rect);
		if(m_iScrollPos < m_vOldScrollPos) SetScrollPos(m_vOldScrollPos);
		break;
	case SHOW_RIGHT:		// 일반 다이얼로그가 떠 있을 경우
		t_rect.SetRect(430, 10, 770, 100);
		if(m_vShowMode != SHOW_DIALOG) m_vOldScrollPos = m_iScrollPos;
		ChatAreaChange(t_rect);
		if(m_iScrollPos < m_vOldScrollPos) SetScrollPos(m_vOldScrollPos);
		break;
	case SHOW_DIALOG:		// 다이얼로그 내에 출력할 경우
		m_vOldScrollPos = m_iScrollPos;
		break;
	}
	m_vShowMode = show_mode;
}