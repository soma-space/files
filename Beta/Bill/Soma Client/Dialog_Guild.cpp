// Dialog_Guild.cpp: implementation of the CDialog_Guild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Guild.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "UIScrollBar.h"
//#include "BBDX_TextOuter.h"
#include "PacketParser.h"
//#include "TWFont.h"
#include "ChildView.h"
#include "User.h"
#include "Dialog_Crest.h"
#include "Dialog_GuildPositionName.h"
#include "Dialog_TaxRate.h"
#include "Dialog_HowMuch.h"
#include "Dialog_Message.h"
#include "Dialog_Message2.h"
#include "GuildDef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GUILD_CHIEF			1
#define GUILD_FIRST			2
#define GUILD_SECOND		3
#define GUILD_MEMBER		10

extern CDialog_Crest		Dialog_Crest;
extern CDialog_GuildPositionName Dialog_GuildPositionName;
extern CDialog_TaxRate		Dialog_TaxRate;
extern CDialog_HowMuch		Dialog_HowMuch;
extern CDialog_Message		Dialog_Message;
extern CDialog_Message2		Dialog_Message2;
extern CUIRes				*g_pShowDialog;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Guild::CDialog_Guild()
{
	INIT_PTR(m_pScrollBar);
	INIT_PTR(m_pFont);
/*	INIT_PTR(m_pTWFont);
	INIT_PTR(m_pGuildNameOuter);
	INIT_PTR(m_pPersonCountOuter);
	INIT_PTR(m_pTaxValueOuter);
	INIT_PTR(m_pGuildMoneyOuter);
	INIT_PTR(m_pTextOuter);
	for(int i = 0; i < 10; i++)
	{
		INIT_PTR(m_pPosOuter[i]);
		INIT_PTR(m_pPosNameOuter[i]);
	}*/

	m_vPosMode = POS_LEFT;
	InitData();
}

CDialog_Guild::~CDialog_Guild()
{
	DeleteContents();
	SAFE_DELETE(m_pScrollBar);
	SAFE_DELETE(m_pFont);
/*	SAFE_DELETE(m_pTWFont);
	SAFE_DELETE(m_pGuildNameOuter);
	SAFE_DELETE(m_pPersonCountOuter);
	SAFE_DELETE(m_pTaxValueOuter);
	SAFE_DELETE(m_pGuildMoneyOuter);
	SAFE_DELETE(m_pTextOuter);
	for(int i = 0; i < 10; i++)
	{
		SAFE_DELETE(m_pPosOuter[i]);
		SAFE_DELETE(m_pPosNameOuter[i]);
	}*/
}

void CDialog_Guild::InitData()
{
	m_vDragIndex = -1;
	m_bProcessing = FALSE;
	m_vSelIndex = -1;
	m_vProcessIndex = -1;
	m_vMeIndex = -1;
	m_bQuit = FALSE;
}

void CDialog_Guild::ShowWindow()
{
	if(!IsLoading())
	{
		SetOffset(CPoint(0,0));
		if(!Create(CChildView::DirectXMng.m_pDD, "Guild.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	CUIRes::ShowWindow();
	InitData();
	m_pScrollBar->InitScroll();
}

void CDialog_Guild::HideWindow()
{
	g_pShowDialog = NULL;
	HideAllWindow();
}

void CDialog_Guild::HideAllWindow()
{
	CUIRes::HideWindow();
	DeleteContents();
}

void CDialog_Guild::DeleteContents()
{
	m_listGuildPersonName.RemoveAll();
}

void CDialog_Guild::SetScrollData()
{
	int count;

	count = m_listGuildPersonName.GetCount();
	count = count / 3 + ((count%3)?1:0);
	m_pScrollBar->SetMaxLineCount(count);
	m_pScrollBar->SetViewLineCount(3);
	m_pScrollBar->SetStartLine(m_pScrollBar->GetStartLine());
}

BOOL CDialog_Guild::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}
		
/*	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		delete m_pTWFont; m_pTWFont=NULL;
		TRACE("####### Dialog Guild Init 실패.. ############ \n");
		return FALSE;
	}
	m_pGuildNameOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pPersonCountOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pTaxValueOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pGuildMoneyOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	m_pTextOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);
	for(int i = 0; i < 10; i++)
	{
		m_pPosOuter[i] = new BBDX_TextOuter(m_pTWFont, 14, 14);
		m_pPosNameOuter[i] = new BBDX_TextOuter(m_pTWFont, 14, 14);
	}*/
	m_pFont = new CFont;
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[0]->m_vRect, 0);

	return TRUE;
}

BOOL CDialog_Guild::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 6);

	CRect rtDst, rtSrc, t_rect;
	CPoint t_pos, m_pos;
	DWORD dwTrans;
	int i, j, start_index;
	CString t_str;

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));

	// 길드 이름 출력
	rtDst = m_arrArea[1]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
//	m_pGuildNameOuter->DrawShadowTexts(pDest, rtDst);
	pDC->DrawText(m_strGuildName, &rtDst, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	// 길드 인원수 출력
	rtDst = m_arrArea[2]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
	t_str.Format("%d", m_vPersonCount);
//	m_pPersonCountOuter->DrawShadowTexts(pDest, rtDst);
	pDC->DrawText(t_str, &rtDst, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	// 길드 세율 출력
	rtDst = m_arrArea[3]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
	t_str.Format("%d", m_vTaxRate);
//	m_pTaxValueOuter->DrawShadowTexts(pDest, rtDst);
	pDC->DrawText(t_str, &rtDst, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	// 길드 자금 출력
	rtDst = m_arrArea[22]->m_vRect;
	rtDst.OffsetRect(m_ptOffset);
	t_str.Format("%u", m_vGuildMoney);
	pDC->DrawText(t_str, &rtDst, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);

	for(i = 0; i < 9; i++)
	{
		if(m_strGuildPos[i].IsEmpty() == FALSE)
		{
			// 길드 지위 표시
			rtDst = m_arrArea[25+i]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_rect = rtDst;
			t_rect.OffsetRect(1, 1);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->DrawText(m_strGuildPos[i], &t_rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			pDC->SetTextColor(RGB(255,0,255));
			pDC->DrawText(m_strGuildPos[i], &rtDst, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		if(m_strGuildPosName[i].IsEmpty() == FALSE)
		{
			// 길드 지위에 해당하는 유저 이름 표시
			rtDst = m_arrArea[34+i]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			t_rect = rtDst;
			t_rect.OffsetRect(1, 1);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->DrawText(m_strGuildPosName[i], &t_rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			pDC->SetTextColor(RGB(255,255,255));
			pDC->DrawText(m_strGuildPosName[i], &rtDst, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
	}

	POSITION pos;
	int sel_index;

	// 일반 길드 유저 이름 표시
	start_index = m_pScrollBar->GetStartLine() * 3;
	pos = m_listGuildPersonName.FindIndex(start_index);
	j = 0;
	while(pos && j < 10)
	{
		rtDst = m_arrArea[j+43]->m_vRect;
		rtDst.OffsetRect(m_ptOffset);
		t_str = m_listGuildPersonName.GetNext(pos);
		t_rect = rtDst;
		t_rect.OffsetRect(1, 1);
		pDC->SetTextColor(RGB(0,0,0));
		pDC->DrawText(t_str, &t_rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		pDC->SetTextColor(RGB(255,255,255));
		pDC->DrawText(t_str, &rtDst, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		
		j++;
	}

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	if(m_vSelIndex >= 0 && m_vSelIndex < 9)
	{
		t_pos = m_arrArea[m_vSelIndex+4]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = m_arrArea[54]->m_vRect;
		dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
		pDest->BltFast(t_pos, m_pDDSurface, &rtSrc, dwTrans);
	}
	else if(m_vSelIndex >= 9)
	{
		sel_index = m_vSelIndex - 9 - start_index;
		t_pos = m_arrArea[sel_index+13]->m_vRect.TopLeft();
		t_pos.Offset(m_ptOffset);
		rtSrc = m_arrArea[54]->m_vRect;
		dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;
		pDest->BltFast(t_pos, m_pDDSurface, &rtSrc, dwTrans);
	}
	m_pScrollBar->Draw(pDest);

	// Drag 아이템 그리기
	if(m_vDragIndex >= 0)
	{
		CPoint offset;

		t_pos = CChildView::DirectXMng.GetMouseInfoPtr()->GetPos();
		dwTrans = DDBLTFAST_WAIT;
		rtSrc = m_arrArea[23]->m_vRect;
		m_pos = t_pos - m_vDragOffset;
		pDest->BltFast(m_pos, m_pDDSurface, &rtSrc, dwTrans);

		pDC = pDest->GetDC();
		old_font = (CFont *)pDC->SelectObject(m_pFont);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255,255,255));

		if(m_vDragFlag == 0)
		{
			// 길드 지위 표시
			if(m_strGuildPos[m_vDragIndex].IsEmpty() == FALSE)
			{
				offset = m_arrArea[52]->m_vRect.TopLeft() - m_arrArea[23]->m_vRect.TopLeft();
				rtDst.left = m_pos.x + offset.x;
				rtDst.top = m_pos.y + offset.y;
				rtDst.right = rtDst.left + m_arrArea[52]->m_vRect.Width();
				rtDst.bottom = rtDst.top + m_arrArea[52]->m_vRect.Height();
				t_rect = rtDst;
				t_rect.OffsetRect(1, 1);
				pDC->SetTextColor(RGB(0,0,0));
				pDC->DrawText(m_strGuildPos[m_vDragIndex], &t_rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
				pDC->SetTextColor(RGB(255,0,255));
				pDC->DrawText(m_strGuildPos[m_vDragIndex], &rtDst, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			}
			// 길드 지위에 해당하는 유저 이름 표시
			offset = m_arrArea[53]->m_vRect.TopLeft() - m_arrArea[23]->m_vRect.TopLeft();
			rtDst.left = m_pos.x + offset.x;
			rtDst.top = m_pos.y + offset.y;
			rtDst.right = rtDst.left + m_arrArea[53]->m_vRect.Width();
			rtDst.bottom = rtDst.top + m_arrArea[53]->m_vRect.Height();
			t_rect = rtDst;
			t_rect.OffsetRect(1, 1);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->DrawText(m_strGuildPosName[m_vDragIndex], &t_rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			pDC->SetTextColor(RGB(255,255,255));
			pDC->DrawText(m_strGuildPosName[m_vDragIndex], &rtDst, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		else
		{
			// 일반 길드 유저 이름 표시
			offset = m_arrArea[53]->m_vRect.TopLeft() - m_arrArea[23]->m_vRect.TopLeft();
			rtDst.left = m_pos.x + offset.x;
			rtDst.top = m_pos.y + offset.y;
			rtDst.right = rtDst.left + m_arrArea[53]->m_vRect.Width();
			rtDst.bottom = rtDst.top + m_arrArea[53]->m_vRect.Height();
			t_rect = rtDst;
			t_rect.OffsetRect(1, 1);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->DrawText(m_vDragName, &t_rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			pDC->SetTextColor(RGB(255,255,255));
			pDC->DrawText(m_vDragName, &rtDst, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}

		pDC->SelectObject(old_font);
		pDest->ReleaseDC(pDC);
	}

	return TRUE;
}

BOOL CDialog_Guild::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		CString message;
		switch(nIndex)
		{
		case 0:		// 길드 문장 변경
/*			if(m_bProcessing == FALSE && m_vRank == GUILD_CHIEF)
			{
				if(Dialog_Crest.ShowWindow(0, this) == TRUE)
				{
					message.LoadString(IDS_GUILD_CHANGE_SYMBOL);
				}
				else
				{
					Dialog_Message.ShowWindow(255, this);
					message.LoadString(IDS_GUILD_INVALID_SYMBOL);
					Dialog_Message.SetMessage(message);
				}
			}*/
			break;
		case 1:		// 닫기
			HideWindow();
			break;
		case 2:		// 탈퇴, 길드원 제명
			if(m_bProcessing == FALSE)
			{
				CString t_str;

				if(m_vSelIndex < 0) return TRUE;
				else if(m_vSelIndex < 9)
				{
					t_str = m_strGuildPosName[m_vSelIndex];
				}
				else
				{
					POSITION pos = m_listGuildPersonName.FindIndex(m_vSelIndex-9);
					if(pos) t_str = m_listGuildPersonName.GetAt(pos);
				}
				if(t_str.IsEmpty()) return TRUE;

				m_vFromIndex = m_vSelIndex;
				if(m_vFromIndex == m_vMeIndex)
				{
					m_bQuit = TRUE;
					message.LoadString(IDS_ASK_QUIT_MEMBER);
				}
				else
				{
					CString tt_str;

					tt_str.LoadString(IDS_ASK_REMOVE_MEMBER);
					message.Format((LPCTSTR)tt_str, t_str);
				}
				Dialog_Message2.ShowWindow(5, this);
				Dialog_Message2.SetMessage(message);
			}
			break;
		case 3:		// 기부, 연체세금 내기
/*			if(m_bProcessing == FALSE)
			{
				Dialog_HowMuch.ShowWindow(1, this);
			}*/
			break;
		case 4:		// 스크롤업
			m_pScrollBar->ScrollUp();
			break;
		case 5:		// 스크롤다운
			m_pScrollBar->ScrollDown();
			break;
		case 6:		// 세율 조정
/*			if(m_bProcessing == FALSE && m_vRank == GUILD_CHIEF)
			{
				Dialog_TaxRate.ShowWindow(2, this);
				Dialog_TaxRate.SetTaxRate(m_vTaxRate);
			}*/
			break;
		}
	}

	return TRUE;
}

BOOL CDialog_Guild::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}
	BOOL retVal = CUIRes::OnLButtonDown(pMouseInfo);

	if(m_bProcessing == TRUE) return retVal;

	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

	if(retVal)
	{
		if(m_vSpecCtrl == AREA_CTRL)
		{
			if(m_vRank == GUILD_MEMBER)
			{
				if(m_vIndexCtrl >= 13 && m_vIndexCtrl <= 21)
				{
					int index = m_pScrollBar->GetStartLine() * 3 + m_vIndexCtrl - 13;
					if(index+9 == m_vMeIndex)
					{
						m_vDragIndex = index;
						m_vSelIndex = index + 9;
						m_vDragFlag = 1;
						m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
						m_vDragName = m_listGuildPersonName.GetAt(m_listGuildPersonName.FindIndex(m_vDragIndex));
						SetCapture();
					}
				}
			}
			else if(m_vRank == GUILD_SECOND)
			{
				if(m_vIndexCtrl-4 == m_vMeIndex)
				{
					m_vSelIndex = m_vDragIndex = m_vMeIndex;
					m_vDragFlag = 0;
					m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
					SetCapture();
				}
			}
			else if(m_vRank == GUILD_FIRST)
			{
				if(m_vIndexCtrl-4 == m_vMeIndex)
				{
					m_vSelIndex = m_vDragIndex = m_vMeIndex;
					m_vDragFlag = 0;
					m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
					SetCapture();
				}
				else if(m_vIndexCtrl >= 13 && m_vIndexCtrl <= 21) // 일반 길드원의 명패 이동
				{
					int index, count;

					index = m_pScrollBar->GetStartLine() * 3 + m_vIndexCtrl - 13;
					count = m_listGuildPersonName.GetCount();
					if(index >= 0 && index < count)
					{
						m_vDragIndex = index;
						m_vSelIndex = index + 9;
						m_vDragFlag = 1;
						m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
						m_vDragName = m_listGuildPersonName.GetAt(m_listGuildPersonName.FindIndex(m_vDragIndex));
						SetCapture();
					}
				}
			}
			else if(m_vRank == GUILD_CHIEF)
			{
				if(m_vIndexCtrl >= 4 && m_vIndexCtrl <= 12) // 간부들의 명패 이동
				{
					int index;

					index = m_vIndexCtrl - 4;
					if(!m_strGuildPosName[index].IsEmpty())
					{
						m_vSelIndex = m_vDragIndex = index;
						m_vDragFlag = 0;
						m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
						SetCapture();
					}
				}
				else if(m_vIndexCtrl >= 13 && m_vIndexCtrl <= 21) // 일반 길드원의 명패 이동
				{
					int index, count;

					index = m_pScrollBar->GetStartLine() * 3 + m_vIndexCtrl - 13;
					count = m_listGuildPersonName.GetCount();
					if(index >= 0 && index < count)
					{
						m_vDragIndex = index;
						m_vSelIndex = index + 9;
						m_vDragFlag = 1;
						m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
						m_vDragName = m_listGuildPersonName.GetAt(m_listGuildPersonName.FindIndex(m_vDragIndex));
						SetCapture();
					}
				}
			}
		}
	}
	else m_vDragIndex = -1;

	return retVal;
}

BOOL CDialog_Guild::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	if(m_bProcessing == FALSE && m_vDragIndex >= 0)
	{
		CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;
		CString message;

		if(m_vDragFlag == 0)
		{
			if(m_arrArea[m_vDragIndex+4]->m_vRect.PtInRect(m_pos))
			{
				m_vDragIndex = -1;
				return TRUE;
			}
		}
		else
		{
			if(m_arrArea[24]->m_vRect.PtInRect(m_pos))
			{
				m_vDragIndex = -1;
				return TRUE;
			}
		}
		if(m_vDragFlag == 0)
		{
			m_vFromIndex = m_vDragIndex;
			if(m_vRank == GUILD_CHIEF)
			{
				if(m_arrArea[24]->m_vRect.PtInRect(m_pos))
				{
					// 간부를 일반 길드원으로 강등.
					m_vToIndex = 10;

					message.LoadString(IDS_ASK_CHANGE_MEMBER);
					Dialog_Message2.ShowWindow(4, this);
					Dialog_Message2.SetMessage(message);
				}
				else if(m_arrArea[4]->m_vRect.PtInRect(m_pos))
				{
					// 간부에서 길드장으로 승진.
					m_vToIndex = 0;

					message.LoadString(IDS_ASK_QUIT_MASTER);
					Dialog_Message2.ShowWindow(4, this);
					Dialog_Message2.SetMessage(message);
				}
				else if(PtInUIRes(pMouseInfo->GetPos()) == FALSE)
				{
					m_vToIndex = -1;

					if(m_vFromIndex == 0)
					{
						message.LoadString(IDS_ASK_DISSOLVE_GUILD);

						Dialog_Message2.ShowWindow(6, this);
						Dialog_Message2.SetMessage(message);
					}
					else
					{
						CString t_str;

						t_str.LoadString(IDS_ASK_REMOVE_MEMBER);
						message.Format((LPCTSTR)t_str, m_vDragName);

						Dialog_Message2.ShowWindow(5, this);
						Dialog_Message2.SetMessage(message);
					}
				}
				else
				{
					for(int i = 5; i <= 12; i++)
					{
						if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
						{
							m_vToIndex = i-4;

							message.LoadString(IDS_ASK_CHANGE_MEMBER);
							Dialog_Message2.ShowWindow(4, this);
							Dialog_Message2.SetMessage(message);
							break;
						}
					}
				}
			}
			else
			{
				if(PtInUIRes(pMouseInfo->GetPos()) == FALSE)
				{
					m_vToIndex = -1;

					if(m_vMeIndex == m_vFromIndex)
					{
						m_bQuit = TRUE;
						message.LoadString(IDS_ASK_QUIT_MEMBER);
						Dialog_Message2.ShowWindow(5, this);
						Dialog_Message2.SetMessage(message);
					}
					else
					{
						CString t_str;

						t_str.LoadString(IDS_ASK_REMOVE_MEMBER);
						message.Format((LPCTSTR)t_str, m_vDragName);
						Dialog_Message2.ShowWindow(5, this);
						Dialog_Message2.SetMessage(message);
					}
				}
			}
		}
		else if(m_vDragFlag == 1)
		{
			m_vFromIndex = m_vDragIndex + 9;
			if(m_vRank == GUILD_CHIEF)
			{
				if(m_arrArea[4]->m_vRect.PtInRect(m_pos))
				{
					// 일반 길드원에서 길드장으로 승진.
					m_vToIndex = 0;

					message.LoadString(IDS_ASK_QUIT_MASTER);
					Dialog_Message2.ShowWindow(4, this);
					Dialog_Message2.SetMessage(message);
				}
				else if(PtInUIRes(pMouseInfo->GetPos()) == FALSE)
				{
					CString t_str;

					m_vToIndex = -1;

					t_str.LoadString(IDS_ASK_REMOVE_MEMBER);
					message.Format((LPCTSTR)t_str, m_vDragName);
					Dialog_Message2.ShowWindow(5, this);
					Dialog_Message2.SetMessage(message);
				}
				else
				{
					for(int i = 5; i <= 12; i++)
					{
						if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
						{
							// 일반 길드원을 간부로 승진.
							m_vToIndex = i-4;

							message.LoadString(IDS_ASK_CHANGE_MEMBER);
							Dialog_Message2.ShowWindow(4, this);
							Dialog_Message2.SetMessage(message);
							break;
						}
					}
				}
			}
			else if(m_vRank == GUILD_FIRST)
			{
				if(PtInUIRes(pMouseInfo->GetPos()) == FALSE)
				{
					CString t_str;
					m_vToIndex = -1;

					t_str.LoadString(IDS_ASK_REMOVE_MEMBER);
					message.Format((LPCTSTR)t_str, m_vDragName);
					Dialog_Message2.ShowWindow(5, this);
					Dialog_Message2.SetMessage(message);
				}
			}
			else
			{
				if(PtInUIRes(pMouseInfo->GetPos()) == FALSE)
				{
					if(m_vFromIndex == m_vMeIndex)
					{
						CString t_str;

						m_bQuit = TRUE;
						t_str.LoadString(IDS_ASK_QUIT_MEMBER);
						message.Format((LPCTSTR)t_str, m_vDragName);
						Dialog_Message2.ShowWindow(5, this);
						Dialog_Message2.SetMessage(message);
					}
				}
			}
		}
		m_vDragIndex = -1;
	}
	ReleaseCapture();
	if(m_pScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_Guild::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;
	return CUIRes::OnMouseMove(pMouseInfo);
}

BOOL CDialog_Guild::OnRButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_bProcessing == TRUE || m_vDragIndex >= 0 || m_vRank != GUILD_CHIEF) return TRUE;

	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

	for(int i = 4; i <= 12; i++)
	{
		if(m_arrArea[i]->m_vRect.PtInRect(m_pos))
		{
			// 길드 간부 호칭 변경
			m_vSelIndex = i-4;
			Dialog_GuildPositionName.ShowWindow(3, this, 12);
			Dialog_GuildPositionName.SetName(m_strGuildPos[m_vSelIndex]);
			return FALSE;
		}
	}

	return TRUE;
}

void CDialog_Guild::CallBack_MessageBox(int nId, int nValue)
{
	CString t_str;

	switch(nId)
	{
	case 0: // 길드 문장 변경 요청
		if(nValue == 1)
		{
		}
		else
		{
		}
		break;
	case 1: // 기부금/연체세금 내기
		if(nValue == 1)
		{
			CUser *me = CChildView::UserManager.GetMe();
			if(me)
			{
				DWORD t_value = (DWORD)Dialog_HowMuch.m_vValue;
				if(t_value > 0 && t_value <= me->m_Money)
				{
					SendDonation(t_value);
				}
				else
				{
					CString message;

					message.LoadString(IDS_LESS_MONEY);
					Dialog_Message.ShowWindow(255, this);
					Dialog_Message.SetMessage(message);
				}
			}
		}
		break;
	case 2: // 세율 조정하기
		if(nValue == 1)
		{
			SendChangeTaxRate(Dialog_TaxRate.m_vTaxRate);
		}
		break;
	case 3: // 간부 호칭 변경
		if(nValue == 1)
		{
			SendChangePos(Dialog_GuildPositionName.m_vName);
		}
		break;
	case 4:	// 간부 위치 변경
		if(nValue == 1)
		{
			SendChangePosName(m_vFromIndex, m_vToIndex);
		}
		break;
	case 5: // 길드원 제명
		if(nValue == 1)
		{
			SendRemoveMember();
		}
		break;
	case 6:	// 길드 해체
		if(nValue == 1)
		{
			SendDissolveGuild();
		}
		break;
	}
}

int CDialog_Guild::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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
			if(pMouseInfo->IsRButtonDown()) {
				OnRButtonDown(pMouseInfo);
			}
			return 2;
		}
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

void CDialog_Guild::SendChangeTaxRate(int tax_rate)
{
	BYTE buf[10];
	int offset = 0;

	m_bProcessing = TRUE;
	CPacketParser::SetByte(buf, offset, PKT_CHANGE_GUILD_TAX);

	CPacketParser::SetInt(buf, offset, m_vGuildIndex);
	CPacketParser::SetByte(buf, offset, (BYTE)tax_rate);
	
	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Guild::RcvChangeTaxRate(PBYTE bypData)
{
	int offset = 0;

	if(m_bShow == FALSE) return;

	m_bProcessing = FALSE;
	BYTE bResult = CPacketParser::GetByte(bypData, offset);
	if(bResult == 1)		// 성공
	{
		m_vTaxRate = CPacketParser::GetByte(bypData, offset);
	}
	else
	{
		CString message;

		message.LoadString(IDS_FAIL_CHANGE_TAXRATE);
		Dialog_Message.ShowWindow(255, this);
		Dialog_Message.SetMessage(message);
	}
}

void CDialog_Guild::SendChangePosName(int from, int to)
{
	BYTE *buf;
	int offset = 0;
	BYTE len;
	CString from_str;

	if(from < 0 || to < 0) return;

	m_vProcessIndex = from;
	m_vProcessIndex2 = to;
	
	if(from < 9)
	{
		from_str = m_strGuildPosName[from];
	}
	else
	{
		POSITION pos;

		pos = m_listGuildPersonName.FindIndex(from-9);
		from_str = m_listGuildPersonName.GetAt(pos);
		from = 9;
	}
	if(to >= 9) to = 9;
	len = from_str.GetLength();

	buf = new BYTE[len+10];

	m_bProcessing = TRUE;
	CPacketParser::SetByte(buf, offset, PKT_CHANGE_MEMBER);

	CPacketParser::SetByte(buf, offset, len);
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)from_str, len);	// 길드원 이름
	CPacketParser::SetByte(buf, offset, (BYTE)(from+1));		// 길드원 인덱스
	CPacketParser::SetByte(buf, offset, (BYTE)(to+1));		// 길드원 인덱스(10일 경우 : 길드원으로)
	
	CChildView::SocketMng.Send(offset, buf);

	delete[] buf;
}

void CDialog_Guild::RcvChangePosName(PBYTE bypData)
{
	int offset = 0;
	CString from_str;

	if(m_bShow == FALSE) return;

	m_bProcessing = FALSE;
	m_vSelIndex = -1;
	BYTE bResult = CPacketParser::GetByte(bypData, offset);
	if(bResult == 1)
	{
		if(m_bQuit == TRUE)
		{
			HideAllWindow();
			return;
		}

		POSITION pos;

		if(m_vProcessIndex < 9) from_str = m_strGuildPosName[m_vProcessIndex];
		else
		{
			pos = m_listGuildPersonName.FindIndex(m_vFromIndex-9);
			from_str = m_listGuildPersonName.GetAt(pos);
		}

		if(m_vProcessIndex < 9)
		{
			m_strGuildPosName[m_vProcessIndex].Empty();
		}
		else
		{
			pos = m_listGuildPersonName.FindIndex(m_vProcessIndex-9);
			if(pos) m_listGuildPersonName.RemoveAt(pos);
		}

		if(m_vProcessIndex2 == 0)
		{
			CString old_str = m_strGuildPosName[0];

			m_strGuildPosName[m_vProcessIndex2] = from_str;
			AddMember(old_str);
			m_vRank = GUILD_MEMBER;
		}
		else if(m_vProcessIndex2 > 0 && m_vProcessIndex2 < 9)
		{
			m_strGuildPosName[m_vProcessIndex2] = from_str;
		}
		else
		{
			pos = m_listGuildPersonName.FindIndex(m_vProcessIndex2-9);
			AddMember(from_str);
		}
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
			break;
		case GUILD_SMALL_RANK:
			message.LoadString(IDS_SMALL_RANK);
			break;
		case GUILD_ABSENT_JOIN:
			message.LoadString(IDS_ABSENT_JOIN);
			break;
		case GUILD_NEED_EMPTY_RANK:
			message.LoadString(IDS_CHANGE_NEED_EMPTY_RANK);
			break;
		case GUILD_ABSENT_GUILD_NAME:
			message.LoadString(IDS_ABSENT_GUILD_NAME);
			break;
		case GUILD_INVALID_RANK:
			message.LoadString(IDS_CHANGE_INVALID_RANK);
			break;
		case GUILD_SYSTEM_ERROR:
		default:
			message.LoadString(IDS_CHANGE_SYSTEM_ERROR);
			break;
		}
		Dialog_Message.ShowWindow(255, this);
		Dialog_Message.SetMessage(message);
	}
}

void CDialog_Guild::SendChangePos(CString pos_name)
{
	BYTE *buf;
	int offset = 0;
	BYTE len;

	if(m_vSelIndex < 0 || m_vSelIndex > 8) return;

	m_bProcessing = TRUE;

	m_vProcessIndex = m_vSelIndex;
	m_vProcessStr = pos_name;
	len = (BYTE)pos_name.GetLength();

	buf = new BYTE[10+len];

	CPacketParser::SetByte(buf, offset, PKT_CHANGE_GUILD_POS);

	CPacketParser::SetByte(buf, offset, (BYTE)(m_vSelIndex+1));		// 인덱스
	CPacketParser::SetByte(buf, offset, len);
	if(len > 0) CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)pos_name, len);	// 이름

	CChildView::SocketMng.Send(offset, buf);

	delete[] buf;
}

void CDialog_Guild::RcvChangePos(PBYTE bypData)
{
	int offset = 0;

	if(m_bShow == FALSE) return;

	m_bProcessing = FALSE;

	BYTE bResult = CPacketParser::GetByte(bypData, offset);
	if(bResult == 1) // 성공
	{
		m_strGuildPos[m_vProcessIndex] = m_vProcessStr;
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
			break;
		case GUILD_SMALL_RANK:
			message.LoadString(IDS_SMALL_RANK);
			break;
		case GUILD_INVALID_RANK:
			message.LoadString(IDS_CPOS_INVALID_RANK);
			break;
		case GUILD_INVALID_GUILD_CALL_NAME:
			message.LoadString(IDS_CPOS_INVALID_GUILD_CALL_NAME);
			break;
		case GUILD_ABSENT_GUILD_NAME:
			message.LoadString(IDS_ABSENT_GUILD_NAME);
			break;
		case GUILD_SYSTEM_ERROR:
		default:
			message.LoadString(IDS_CPOS_SYSTEM_ERROR);
			break;
		}
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(message);
	}
}

void CDialog_Guild::SendDissolveGuild()
{
	BYTE buf[10];
	int offset = 0;

	m_bProcessing = TRUE;
	CPacketParser::SetByte(buf, offset, PKT_GUILD_DISSOLVE);

	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Guild::RcvDissolveGuild(PBYTE bypData)
{
	int offset = 0;

	if(m_bShow == FALSE) return;

	m_bProcessing = FALSE;

	BYTE bResult = CPacketParser::GetByte(bypData, offset);
	if(bResult == 1)
	{
		HideWindow();

		CString message;

		message.LoadString(IDS_SUC_DISSOLVE_GUILD);
		Dialog_Message.ShowWindow();
		Dialog_Message.SetMessage(message);
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
		case GUILD_SMALL_RANK:
			message.LoadString(IDS_SMALL_RANK);
			break;
		case GUILD_ABSENT_GUILD_NAME:
			message.LoadString(IDS_ABSENT_GUILD_NAME);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		case GUILD_SYSTEM_ERROR:
		default:
			message.LoadString(IDS_DISSOLVE_SYSTEM_ERROR);
			break;
		}
		Dialog_Message.ShowWindow(255, this);
		Dialog_Message.SetMessage(message);
	}
}

void CDialog_Guild::SendDonation(DWORD money)
{
	BYTE buf[10];
	int offset = 0;

	m_bProcessing = TRUE;
	CPacketParser::SetByte(buf, offset, PKT_GIVE_DONATION);

	CPacketParser::SetDword(buf, offset, money);

	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_Guild::RcvDonation(PBYTE bypData)
{
	int offset = 0;

	if(m_bShow == FALSE) return;

	m_bProcessing = FALSE;
	BYTE bResult = CPacketParser::GetByte(bypData, offset);
	CString message;
	if(bResult == 1)
	{
		message.LoadString(IDS_SUC_DONATION);
	}
	else
	{
		message.LoadString(IDS_FAIL_DONATION);
	}
	Dialog_Message.ShowWindow(255, this);
	Dialog_Message.SetMessage(message);
}

void CDialog_Guild::OpenGuild(PBYTE bypData)
{
	int offset = 0;
	short count;
	BYTE len;
	CString t_str;
	int i;

	ShowWindow();

	m_vPersonCount = 0;			// 길드원 초기화
	m_vRank = CPacketParser::GetByte(bypData, offset);			// Rank
	count = CPacketParser::GetShort(bypData, offset);
	len = CPacketParser::GetByte(bypData, offset);
	CPacketParser::GetString(bypData, offset, (PBYTE)m_strGuildName.GetBuffer(len), len);		// 길드 이름
	m_strGuildName.ReleaseBuffer();
	m_vTaxRate = CPacketParser::GetShort(bypData, offset);		// 세율
	m_vGuildMoney = CPacketParser::GetDword(bypData, offset);		// 길드 돈

	CUser *me = CChildView::UserManager.GetMe();

	for(i = 0; i < 9; i++)
	{
		// 이름
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)m_strGuildPosName[i].GetBuffer(len), len);
			m_strGuildPosName[i].ReleaseBuffer();
			m_strGuildPosName[i].MakeUpper();
			m_vPersonCount++;
			if(me->m_strUserId.CompareNoCase(m_strGuildPosName[i]) == FALSE)
			{
				m_vMeIndex = i;
			}
		}
		else m_strGuildPosName[i].Empty();

		// 호칭
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)m_strGuildPos[i].GetBuffer(len), len);
			m_strGuildPos[i].ReleaseBuffer();
			m_strGuildPos[i].MakeUpper();
		}
		else m_strGuildPos[i].Empty();
	}
	m_vPersonCount += count;
	for(i = 0; i < count; i++)
	{
		len = CPacketParser::GetByte(bypData, offset);
		CPacketParser::GetString(bypData, offset, (PBYTE)t_str.GetBuffer(len), len);
		t_str.ReleaseBuffer();
		t_str.MakeUpper();
		if(me->m_strUserId.CompareNoCase(t_str) == FALSE)
		{
			m_vMeIndex = i+9;
		}

		AddMember(t_str);
	}

	SetScrollData();
}

void CDialog_Guild::SendRemoveMember()
{
	BYTE *buf;
	int offset = 0;
	BYTE len;
	CString t_str;

	m_bProcessing = TRUE;
	if(m_vSelIndex < 0) return;
	else if(m_vSelIndex < 9)
	{
		t_str = m_strGuildPosName[m_vSelIndex];
	}
	else
	{
		POSITION pos = m_listGuildPersonName.FindIndex(m_vSelIndex-9);
		if(pos) t_str = m_listGuildPersonName.GetAt(pos);
	}
	if(t_str.IsEmpty()) return;
	m_vProcessIndex = m_vSelIndex;
	len = (BYTE)t_str.GetLength();

	buf = new BYTE[len+10];
	CPacketParser::SetByte(buf, offset, PKT_GUILD_REMOVE_USER);

	CPacketParser::SetByte(buf, offset, len);
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)t_str, len);

	CChildView::SocketMng.Send(offset, buf);
	
	delete[] buf;
}

void CDialog_Guild::RcvRemoveMember(PBYTE bypData)
{
	int offset = 0;

	if(m_bShow == FALSE) return;

	m_bProcessing = FALSE;
	BYTE bResult = CPacketParser::GetByte(bypData, offset);
	CString message;
	if(bResult == 1)
	{
		if(m_bQuit == TRUE)
		{
			HideAllWindow();
			message.LoadString(IDS_SUC_QUIT_MEMBER);
		}
		else
		{
			if(m_vProcessIndex < 9)
			{
				m_strGuildPosName[m_vProcessIndex].Empty();
			}
			else
			{
				POSITION pos = m_listGuildPersonName.FindIndex(m_vProcessIndex-9);
				if(pos) m_listGuildPersonName.RemoveAt(pos);
			}
			m_vSelIndex = -1;
			return;
		}
	}
	else
	{
		BYTE sub_result;

		sub_result = CPacketParser::GetByte(bypData, offset);
		switch(sub_result)
		{
		case GUILD_NOT_GUILD_USER:
			message.LoadString(IDS_NOT_GUILD_USER);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		case GUILD_ABSENT_JOIN:
			message.LoadString(IDS_ABSENT_JOIN);
			break;
		case GUILD_ABSENT_GUILD_NAME:
			message.LoadString(IDS_ABSENT_GUILD_NAME);
			Dialog_Message.ShowWindow();
			Dialog_Message.SetMessage(message);
			return;
		case GUILD_SMALL_RANK:
			message.LoadString(IDS_SMALL_RANK);
			break;
		case GUILD_SYSTEM_ERROR:
		default:
			message.LoadString(IDS_REMOVE_SYSTEM_ERROR);
			break;
		}
	}
	Dialog_Message.ShowWindow(255, this);
	Dialog_Message.SetMessage(message);
}

void CDialog_Guild::AddMember(CString m_name)
{
	POSITION pos = m_listGuildPersonName.GetHeadPosition();
	CString t_str;
	BOOL retVal = FALSE;

	while(pos)
	{
		t_str = m_listGuildPersonName.GetNext(pos);
		if(t_str.CompareNoCase(m_name) > 0)
		{
			m_listGuildPersonName.InsertBefore(pos, m_name);
			retVal = TRUE;
			break;
		}
	}

	if(retVal == FALSE)
	{
		m_listGuildPersonName.AddTail(m_name);
	}
}