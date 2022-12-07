// Dialog_WhiteMagic.cpp: implementation of the CDialog_WhiteMagic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_WhiteMagic.h"
#include "Dialog_MagicCtrl.h"
#include "ChildView.h"
#include "User.h"
#include "Item_Total.h"
//#include "BBDX_TextOuter.h"
//#include "TWFont.h"
#include "Dialog_Main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CItem_Total         Item_Total;
extern CDialog_Main			Dialog_Main;
extern CUIRes *g_pShowDialog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_WhiteMagic::CDialog_WhiteMagic(CWnd *wnd)
:CUIRes(wnd)
{
	INIT_PTR(m_pScrollBar);
//	INIT_PTR(m_pTWFont);
//	INIT_PTR(m_pOuter);
	m_pFont = NULL;
	m_vMagicCount = 0;
	m_vDragItem = -1;
	m_vPosMode = POS_LEFT;
}

CDialog_WhiteMagic::~CDialog_WhiteMagic()
{
	SAFE_DELETE(m_pScrollBar);
//	SAFE_DELETE(m_pTWFont);
//	SAFE_DELETE(m_pOuter);
	if(m_pFont) delete m_pFont;
}

BOOL CDialog_WhiteMagic::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
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
	}

	m_pOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);*/
	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[10]->m_vRect, 0);

	return TRUE;
}

BOOL CDialog_WhiteMagic::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 5);

	CUser *me = CChildView::UserManager.GetMe();

	if(me)
	{
		int i, j;
		CRect rtDst, rtSrc, t_rect;
		DWORD dwTrans;
		int start, end;
		CString t_str;
		CMyMagicData *magic_data;

		dwTrans = DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY;
		end = me->m_arrWhiteMagic.GetSize();
		if(m_vMagicCount != end) SetScrollData();
		start = m_pScrollBar->GetStartLine();
		if(end > start+5) end = start + 5;
		for(i = start, j = 0; i < end; i++,j++)
		{
			magic_data = me->m_arrWhiteMagic[i];
			rtDst = m_arrArea[j]->m_vRect;
			rtDst.OffsetRect(m_ptOffset);
			rtSrc = Item_Total.m_Magic_Item->m_SprData[magic_data->m_vNumber-1].rcV;
			pDest->BltFast(rtDst.TopLeft(), Item_Total.m_Magic_Item->m_lpSurface, &rtSrc, dwTrans);
		}

		CDC *pDC = pDest->GetDC();
		CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
		pDC->SetBkMode(TRANSPARENT);

		for(i = start, j = 0; i < end; i++,j++)
		{
			magic_data = me->m_arrWhiteMagic[i];
			rtDst = m_arrArea[j+5]->m_vRect; rtDst.OffsetRect(m_ptOffset);
			t_rect = rtDst; t_rect.OffsetRect(1, 1);
			t_str = magic_data->m_vName;
//			m_pOuter->SetString(t_str);
//			m_pOuter->DrawShadowTexts(pDest, rtDst, 0xffe0, 0);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->DrawText(t_str, &t_rect, DT_SINGLELINE|DT_TOP|DT_LEFT);
			pDC->SetTextColor(RGB(255,255,0));
			pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_TOP|DT_LEFT);

			rtDst.top += 14;
			t_rect.top += 14;
			if(magic_data->m_vUseTime == 0)
			{
//				t_str = magic_data->m_vExplain;
//				m_pOuter->SetString(t_str);
				t_str.Format("%s\n거리 %d 위력 %d 심력 %d", magic_data->m_vExplain, magic_data->m_vDistance, magic_data->m_vDamage, magic_data->m_vUseMp);
//				m_pOuter->AddString(t_str);
			}
			else
			{
//				t_str = magic_data->m_vExplain;
//				m_pOuter->SetString(t_str);
				t_str.Format("%s\n거리 %d 지속시간 %d\n위력 %d 심력 %d", magic_data->m_vExplain, magic_data->m_vDistance, magic_data->m_vUseTime/1000, magic_data->m_vDamage, magic_data->m_vUseMp);
//				m_pOuter->AddString(t_str);
//				t_str.Format("위력 %d 심력 %d", magic_data->m_vDamage, magic_data->m_vUseMp);
//				m_pOuter->AddString(t_str);
			}
//			m_pOuter->DrawShadowTexts(pDest, rtDst, 0xffff, 0);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->DrawText(t_str, &t_rect, DT_TOP|DT_LEFT|DT_EXTERNALLEADING);
			pDC->SetTextColor(RGB(255,255,255));
			pDC->DrawText(t_str, &rtDst, DT_TOP|DT_LEFT|DT_EXTERNALLEADING);
		}
		pDC->SelectObject(old_font);
		pDest->ReleaseDC(pDC);

		m_pScrollBar->Draw(pDest);
	}
	return TRUE;
}

BOOL CDialog_WhiteMagic::DragDraw(CDDSurface *pDest)
{
	CRect rtSrc;
	CUser *me = CChildView::UserManager.GetMe();
	
	// Drag 아이템 그리기
	if(me && m_vDragItem >= 0)
	{
		CPoint t_pos;
		DWORD dwTrans;

		dwTrans = DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY;
		t_pos = CChildView::DirectXMng.GetMouseInfoPtr()->GetPos() - m_vDragOffset;
		rtSrc = Item_Total.m_Magic_Item->m_SprData[me->m_arrWhiteMagic[m_vDragItem]->m_vNumber-1].rcV;
		pDest->BltFast(t_pos, Item_Total.m_Magic_Item->m_lpSurface, &rtSrc, dwTrans);
	}

	return TRUE;
}

BOOL CDialog_WhiteMagic::OnButtonAction(int nSpec, int nIndex)
{
	if(nSpec == BTN_CTRL)
	{
		switch(nIndex)
		{
		case 0:			// 닫기
			m_pDialogCtrl->HideWindow();
			break;
		case 1:			// Special Magic으로...
			HideWindow();
			m_pDialogCtrl->DrawMode(DRAW_SPECIALMAGIC);
			break;
		case 2:			// Blue Magic으로...
			HideWindow();
			m_pDialogCtrl->DrawMode(DRAW_BLUEMAGIC);
			break;
		case 3:			// Black Magic으로...
			HideWindow();
			m_pDialogCtrl->DrawMode(DRAW_BLACKMAGIC);
			break;
		case 4:
			m_pScrollBar->ScrollUp();
			break;
		case 5:
			m_pScrollBar->ScrollDown();
			break;
		}
	}

	return TRUE;
}

void CDialog_WhiteMagic::SetScrollData()
{
	int max_line;
	int count;
	CUser *me;

	me = CChildView::UserManager.GetMe();
	if(me == NULL) return;

	count = me->m_arrWhiteMagic.GetSize();
	m_vMagicCount = count;

	max_line = count;
	m_pScrollBar->SetMaxLineCount(max_line);
	m_pScrollBar->SetViewLineCount(5);
	m_pScrollBar->SetStartLine(m_pScrollBar->GetStartLine());
}

void CDialog_WhiteMagic::InitData()
{
	m_pScrollBar->InitScroll();
	SetScrollData();
}

BOOL CDialog_WhiteMagic::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnLButtonDown(pMouseInfo) )
	{
		SetFocus();
		SetCapture();
		return FALSE;
	}

	BOOL retVal = CUIRes::OnLButtonDown(pMouseInfo);
	CPoint m_pos = pMouseInfo->GetPos() - m_ptOffset;

	if(retVal)
	{
		if(m_vSpecCtrl == AREA_CTRL)
		{
			if(m_vIndexCtrl >= 0 && m_vIndexCtrl <= 4)
			{
				int item_index;
				CUser *me;

				me = CChildView::UserManager.GetMe();

				item_index = m_pScrollBar->GetStartLine() + m_vIndexCtrl;
				if(item_index < me->m_arrWhiteMagic.GetSize())
				{
					m_vDragItem = item_index;
					m_vDragOffset = m_pos - m_arrArea[m_vIndexCtrl]->m_vRect.TopLeft();
					SetCapture();
				}
			}
		}
	}

	return retVal;
}

BOOL CDialog_WhiteMagic::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(m_pScrollBar->OnMouseMove(pMouseInfo) )
		return FALSE;

	return CUIRes::OnMouseMove(pMouseInfo);
}

BOOL CDialog_WhiteMagic::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	if(m_vDragItem >= 0)
	{
		CPoint m_pos = pMouseInfo->GetPos() - Dialog_Main.m_ptOffset;
		int i;

		for(i = 26; i <= 29; i++)
		{
			if(Dialog_Main.m_arrArea[i]->m_vRect.PtInRect(m_pos))
			{
				CUser *me;

				me = CChildView::UserManager.GetMe();
				Dialog_Main.SetMagicData(i-26, me->m_arrWhiteMagic[m_vDragItem]);
			}
		}
		m_vDragItem = -1;
	}
	ReleaseCapture();
	if(m_pScrollBar->OnLButtonUp(pMouseInfo) )
		return FALSE;

	return CUIRes::OnLButtonUp(pMouseInfo);
}

void CDialog_WhiteMagic::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Magic-W.lib", 0, FALSE, FALSE))
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	InitData();

	CUIRes::ShowWindow();
}

void CDialog_WhiteMagic::HideWindow()
{
	g_pShowDialog = NULL;

	CUIRes::HideWindow();
}

void CDialog_WhiteMagic::HideAllWindow()
{
	m_pDialogCtrl->HideWindow();
}

BOOL CDialog_WhiteMagic::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_ESCAPE)
	{
		m_pDialogCtrl->HideWindow();
		return TRUE;
	}

	return FALSE;
}