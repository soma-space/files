// Dialog_EventMessage.cpp: implementation of the CDialog_EventMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_EventMessage.h"
#include "TWFont.h"
#include "BBDX_TextOuter.h"
#include "ChildView.h"
#include "User.h"
#include "DManage/BBDX_MouseInformation.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CUIRes *g_pShowDialog;
extern CUIRes				*g_pCaptureDialog;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_EventMessage::CDialog_EventMessage()
{
	m_pFont = NULL;

	m_LClick = false;
	m_RClick = false;

	m_SelectStrNum = 0;//선택문항 수...
	m_StringViewMode = 0;
	m_FocusSel  = -1;
	m_SelStringNUm = -1;

    m_bHaveMsg = false;
	m_vPosMode = POS_NORMAL;

	m_DrawTxtHeight = 0;
	m_StartLine = 0;
}

CDialog_EventMessage::~CDialog_EventMessage()
{
	SAFE_DELETE(m_pFont);
}

BOOL CDialog_EventMessage::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	return TRUE;
}

BOOL CDialog_EventMessage::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;
	if(m_bHaveMsg == true && g_pCaptureDialog == NULL) SetCapture();
	if(g_pCaptureDialog != this) return TRUE;
		
	CPoint pt,ptMapOffset,ptDlg;
	CRect rtDst;

	ptMapOffset = CChildView::MapManager.SetCenterPos(CChildView::UserManager.GetMyPos());
	CUser *pMe = CChildView::UserManager.GetMe();
	pt = pMe->GetPosition()-ptMapOffset;

	ptDlg.x = pt.x - (m_mainRect.right - m_mainRect.left) / 2 + 20;
	ptDlg.y = pt.y - m_mainRect.bottom - m_mainRect.top - 100;
	SetOffset(ptDlg);

	m_bTrans = TRUE;
	CUIRes::Draw(pDest);

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	COLORREF backcolor = pDC->GetBkColor();

	TEXTMETRIC Metrics;

	pDC->GetTextMetrics(&Metrics);
	m_charwidth = Metrics.tmMaxCharWidth;
	m_charheight = Metrics.tmHeight + Metrics.tmExternalLeading;

	CRgn t_rgn;

	switch(m_StringViewMode)
	{
		case 0: rtDst = m_arrArea[0]->m_vRect; rtDst.OffsetRect(m_ptOffset);				
				t_rgn.CreateRectRgnIndirect((LPCRECT)rtDst);
				pDC->SelectClipRgn(&t_rgn);
				rtDst.top -= (m_StartLine * m_charheight);
				pDC->DrawText(m_StringMsg, &rtDst, DT_LEFT|DT_TOP|DT_EXTERNALLEADING|DT_EDITCONTROL|DT_WORDBREAK);
				//m_DrawTxtHeight = rtDst.Height() / m_charheight;
			    break;
		case 1: 
			    for(int i =1; i <= m_SelectStrNum; i++)
				{
					rtDst = m_arrArea[i]->m_vRect; rtDst.OffsetRect(m_ptOffset);
					if(m_FocusSel == i)
					{
						pDC->SetBkColor(RGB(0,0,0));
						pDC->SetBkMode(OPAQUE);
						pDC->DrawText(m_SelectStr[i-1], &rtDst, DT_TOP|DT_LEFT|DT_EXTERNALLEADING|DT_EDITCONTROL);
					}
					else
					{
						pDC->SetBkColor(backcolor);
						pDC->SetBkMode(TRANSPARENT);
						pDC->DrawText(m_SelectStr[i-1], &rtDst, DT_TOP|DT_LEFT|DT_EXTERNALLEADING|DT_EDITCONTROL);
					}
				}
			    break;
	}
	
	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	return TRUE;
}

void CDialog_EventMessage::ShowWindow()
{
	if(!IsLoading())
	{
		CPoint t_pos;

		if(!Create(CChildView::DirectXMng.m_pDD, "quest1.lib", 0, TRUE, FALSE, FALSE))
		{
			return;
		}
		t_pos.x = (800 - m_mainRect.Width()) / 2;
		t_pos.y = (600 - m_mainRect.Height()) / 2;
	}

	m_LClick = false;
	m_RClick = false;
	m_SelectStrNum = 0;//선택문항 수...
	m_StringViewMode = 0;
	m_FocusSel  = -1;
	m_SelStringNUm = -1;

    m_bHaveMsg = true;

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	if(g_pCaptureDialog == NULL) SetCapture();
	CUIRes::ShowWindow();
}

void CDialog_EventMessage::HideWindow()
{
	g_pShowDialog = NULL;

	m_bHaveMsg = false;
	m_StartLine = 0;
	m_DrawTxtHeight= 0;
	ReleaseCapture();

	HideAllWindow();
}

void CDialog_EventMessage::HideAllWindow()
{
	CUIRes::HideWindow();
}

BOOL CDialog_EventMessage::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CRect rtDlg;
	CPoint point;
	
	rtDlg.SetRect(0, 0, m_mainRect.Width(), m_mainRect.Height());
	rtDlg.OffsetRect(m_ptOffset);
	point = pMouseInfo->GetPos();
	if (!rtDlg.PtInRect(point)) 
	{
		return FALSE;
	}
	if(m_StringViewMode == 1)
	{
		for(int i = 1; i<= m_SelectStrNum; i++)
		{
			rtDlg = m_arrArea[i]->m_vRect;
			rtDlg.OffsetRect(m_ptOffset);
		
			if (rtDlg.PtInRect(point)) { m_SelStringNUm  = i; m_LClick = true; return TRUE; }
		}
	}

	m_LClick = true;

	return TRUE;
}

BOOL CDialog_EventMessage::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	CRect rtDlg, rtDst;
	CPoint point;
	
	rtDlg.SetRect(0, 0, m_mainRect.Width(), m_mainRect.Height());
	rtDlg.OffsetRect(m_ptOffset);
	point = pMouseInfo->GetPos();
	if (!rtDlg.PtInRect(point)) 
	{
		return FALSE;
	}
	if(m_LClick)
	{
		switch(m_StringViewMode)
		{
			case 0:
				{
					//한번에 보여질수 있는 양의 메세진인가 아닌가를 체크..
					//=================================================
					rtDst = m_arrArea[0]->m_vRect; rtDst.OffsetRect(m_ptOffset);
					int colcount  = rtDst.Width() / m_charwidth;
					int rowcount  = rtDst.Height() / m_charheight;
					int charcount = colcount * rowcount;

					if(m_str_len >charcount)
					{
						int			str_len;
						int         index = 0;
						CString		str;
						LPSTR		lpStr;
						
						str_len = m_StringMsg.GetLength();
						lpStr = m_StringMsg.GetBufferSetLength(str_len);
						for(int i = 1; i <= charcount; i++)
						{
							index++;
							if(lpStr[i] & 0x80 ) ++i;
						}
						lpStr += index;
						str = lpStr;
						m_StringMsg.ReleaseBuffer();
						m_StringMsg = str;

						m_str_len = 0;
						for(i = index; i < str_len; i++)
						{
							m_str_len++;
							if(lpStr[i] & 0x80 ) ++i;
						}

						m_StringViewMode = 0; // 일반 메세지..
					}
					else m_StringViewMode = 1;
					//=================================================
				}
				break;
			case 1:				
					for(int i = 1; i<= m_SelectStrNum; i++)
					{
						rtDlg = m_arrArea[i]->m_vRect;
						rtDlg.OffsetRect(m_ptOffset);
					
						if (rtDlg.PtInRect(point))
							if (m_SelStringNUm == i) { SendClickNum(m_SelStringNUm); m_SelStringNUm = -1; return TRUE; }
					}
			  	break;
		} //switch;
		m_LClick = false;
	} //if

	return TRUE;
}

BOOL CDialog_EventMessage::OnLButtonUp2(BBDX_MouseInformation *pMouseInfo)
{
	CRect rtDlg, rtDst;
	CPoint point;
	
	rtDlg.SetRect(0, 0, m_mainRect.Width(), m_mainRect.Height());
	rtDlg.OffsetRect(m_ptOffset);
	point = pMouseInfo->GetPos();
	if (!rtDlg.PtInRect(point)) 
	{
		return FALSE;
	}

	if(m_LClick)
	{
		switch(m_StringViewMode)
		{
			case 0:
					//한번에 보여질수 있는 양의 메세진인가 아닌가를 체크..
					//=================================================
					rtDst = m_arrArea[0]->m_vRect;
					m_StartLine  += rtDst.Height() / m_charheight;
					if(m_DrawTxtHeight <= m_StartLine) m_StringViewMode = 1;
					//=================================================
				break;
			case 1:				
					for(int i = 1; i<= m_SelectStrNum; i++)
					{
						rtDlg = m_arrArea[i]->m_vRect;
						rtDlg.OffsetRect(m_ptOffset);
					
						if (rtDlg.PtInRect(point))
							if (m_SelStringNUm == i) { SendClickNum(m_SelStringNUm); m_SelStringNUm = -1; return TRUE; }
					}
			  	break;
		} //switch;
		m_LClick = false;
	} //if

	return TRUE;
}

int CDialog_EventMessage::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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
				OnLButtonUp2(pMouseInfo);
			}
			if(pMouseInfo->IsRButtonDown()) {
				OnRButtonDown(pMouseInfo);
			}
			if(pMouseInfo->IsRButtonUp()) {
				OnRButtonUp(pMouseInfo);
			}
			return 2;
		}
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

BOOL CDialog_EventMessage::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	CRect   rtDlg;
	CPoint  pt = pMouseInfo->GetPos();

	if(m_StringViewMode == 1)
	{
		for(int i = 1; i<= m_SelectStrNum; i++)
		{
			rtDlg = m_arrArea[i]->m_vRect;
			rtDlg.OffsetRect(m_ptOffset);
		
			if (rtDlg.PtInRect(pt)) { m_FocusSel  = i; return TRUE; }
			else m_FocusSel = -1;
		}
	}

	return TRUE;	
}


BOOL CDialog_EventMessage::OnRButtonDown(BBDX_MouseInformation *pMouseInfo)
{
/*	CRect rtDlg;
	CPoint point;
	
	rtDlg.SetRect(0, 0, m_mainRect.Width(), m_mainRect.Height());
	rtDlg.OffsetRect(m_ptOffset);
	point = pMouseInfo->GetPos();
	if (!rtDlg.PtInRect(point)) {
		return FALSE;
	}
*/
	m_RClick = true;

	return TRUE;
}

BOOL CDialog_EventMessage::OnRButtonUp(BBDX_MouseInformation *pMouseInfo)
{
/*	CRect rtDlg;
	CPoint point;
	
	rtDlg.SetRect(0, 0, m_mainRect.Width(), m_mainRect.Height());
	rtDlg.OffsetRect(m_ptOffset);
	point = pMouseInfo->GetPos();
	if (!rtDlg.PtInRect(point)) {
		return FALSE;
	}
*/	if(m_RClick) HideWindow();
	m_RClick = false;

	return TRUE;
}

void CDialog_EventMessage::AddString(PBYTE bypData)
{
	int uid = 0;
	int offset = 0;
	bool bLoginCheck = false;

	
	int			str_len;
	CString		str;
	LPSTR		lpStr;
	
	str_len = CPacketParser::GetShort(bypData, offset);
	lpStr = str.GetBufferSetLength(str_len);
	CPacketParser::GetString(bypData, offset, (PBYTE)lpStr, str_len);
	str.ReleaseBuffer();
	m_StringMsg = str;
/*
	m_str_len = 0;
	for(int i = 0; i < str_len; i++)
	{
		m_str_len++;
		if(lpStr[i] & 0x80 ) ++i;
	}

	m_SelectStrNum = 0;
*/
//---------------------------------------------------
	CRect rtDst = m_arrArea[0]->m_vRect;
	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);

	TEXTMETRIC Metrics;

	pDC->GetTextMetrics(&Metrics);
	m_charwidth = Metrics.tmMaxCharWidth;
	m_charheight = Metrics.tmHeight + Metrics.tmExternalLeading;

	pDC->DrawText(m_StringMsg, &rtDst,  DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EXTERNALLEADING|DT_EDITCONTROL|DT_CALCRECT);
	m_DrawTxtHeight = rtDst.Height() / m_charheight;

	pDC->SelectObject(old_font);
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);
//---------------------------------------------------
	for(int i= 0; i < MAX_SELECT_SIZE; i++)
	{
		if(str_len = CPacketParser::GetShort(bypData, offset))
		{
			lpStr = str.GetBufferSetLength(str_len);
			CPacketParser::GetString(bypData, offset, (PBYTE)lpStr, str_len);
			str.ReleaseBuffer();
			m_SelectStr[m_SelectStrNum] = str;
			m_SelectStrNum++;
		}
	}
}

void CDialog_EventMessage::SendClickNum(int num)
{
	BYTE buf[30];
	int offset = 0;

	HideWindow();
	CPacketParser::SetByte(buf, offset, PKT_EVENTSELBOX); 
	CPacketParser::SetShort(buf, offset,m_SelStringNUm - 1);
	CChildView::SocketMng.Send(offset, buf);
}
