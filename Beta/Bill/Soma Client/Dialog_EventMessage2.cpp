// Dialog_EventMessage2.cpp: implementation of the CDialog_EventMessage2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_EventMessage2.h"
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
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_EventMessage2::CDialog_EventMessage2()
{
	m_pFont = NULL;

	m_LClick = false;
	m_RClick = false;

    m_bHaveMsg = false;
	m_vPosMode = POS_NORMAL;
    m_DrawTxtHeight;
    m_StartLine;
}

CDialog_EventMessage2::~CDialog_EventMessage2()
{
	SAFE_DELETE(m_pFont);
}

BOOL CDialog_EventMessage2::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	return TRUE;
}

BOOL CDialog_EventMessage2::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;
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

	
	rtDst = m_arrArea[0]->m_vRect; rtDst.OffsetRect(m_ptOffset);

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));
	COLORREF backcolor = pDC->GetBkColor();

	TEXTMETRIC Metrics;

	pDC->GetTextMetrics(&Metrics);
	m_charwidth = Metrics.tmMaxCharWidth;
	m_charheight = Metrics.tmHeight;

	CRgn t_rgn;

	t_rgn.CreateRectRgnIndirect((LPCRECT)rtDst);
	pDC->SelectClipRgn(&t_rgn);
	rtDst.top -= (m_StartLine * (m_charheight - 1));

	pDC->DrawText(m_StringMsg, &rtDst, DT_TOP|DT_LEFT|DT_EXTERNALLEADING|DT_WORDBREAK   );

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);
	return TRUE;
}

void CDialog_EventMessage2::ShowWindow()
{
	if(!IsLoading())
		if(!Create(CChildView::DirectXMng.m_pDD, "quest1.lib", 0, TRUE, FALSE, FALSE)) return;

	SetCapture();
	CUIRes::ShowWindow();
  
	if(g_pShowDialog && g_pShowDialog != this) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	m_LClick = false;
	m_RClick = false;
}

void CDialog_EventMessage2::HideWindow()
{
    m_DrawTxtHeight = 0;
    m_StartLine = 0;
	ReleaseCapture();
	HideAllWindow();
	
	m_StringList.RemoveAll();
}

void CDialog_EventMessage2::HideAllWindow()
{
	CUIRes::HideWindow();
}

BOOL CDialog_EventMessage2::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
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
	m_LClick = true;

	return TRUE;
}

BOOL CDialog_EventMessage2::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
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
		//한번에 보여질수 있는 양의 메세진인가 아닌가를 체크..
		//=================================================
//----------
					rtDst = m_arrArea[0]->m_vRect; rtDst.OffsetRect(m_ptOffset);
					int colcount  = rtDst.Width() / m_charwidth;
					int rowcount  = rtDst.Height() / m_charheight;
					int charcount = colcount * rowcount;

					int			str_len;
					int         index = 0;
					CString		str;
					LPSTR		lpStr;

					if(m_str_len >charcount)
					{						
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
					}
//----------
/*		int strlength = m_pTWFont->GetPutCharCount();//widthcount * heightcount;

		int len = m_StringMsg.GetLength();
		if( strlength <  len)
		{
			CString  tmp;
			LPSTR  dstBuff = tmp.GetBuffer(m_StringMsg.GetLength() - strlength);
			LPSTR  srcBuff = m_StringMsg.GetBuffer(m_StringMsg.GetLength());
			strcpy(dstBuff,&srcBuff[strlength+1]);
			m_StringMsg = tmp;
		}
*/		else
		{
			if(m_StringList.IsEmpty())
			{
				HideWindow();
			}
			else 
			{
				m_StringList.RemoveHead();
				if(!m_StringList.IsEmpty())
				{
					int count = m_StringList.GetCount();
					POSITION pos = m_StringList.GetHeadPosition();
					m_StringMsg = m_StringList.GetNext(pos);

					str_len = m_StringMsg.GetLength();
					lpStr = m_StringMsg.GetBufferSetLength(str_len);

					m_str_len = 0;
					for(int i = 0; i < str_len; i++)
					{
						m_str_len++;
						if(lpStr[i] & 0x80 ) ++i;
					}
					m_StringMsg.ReleaseBuffer();
				}
				else HideWindow();
			}
		}
		//=================================================

		m_LClick = false;
	}

	return TRUE;
}


BOOL CDialog_EventMessage2::OnLButtonUp2(BBDX_MouseInformation *pMouseInfo)
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
		//한번에 보여질수 있는 양의 메세진인가 아닌가를 체크..
		//---------------------------------------------------
		rtDst = m_arrArea[0]->m_vRect;
		m_StartLine  += rtDst.Height() / m_charheight;
		if(m_DrawTxtHeight <= m_StartLine)
		//---------------------------------------------------
		{
			if(m_StringList.IsEmpty())
			{
				HideWindow();
			}
			else 
			{
				m_StringList.RemoveHead();
				if(!m_StringList.IsEmpty())
				{
					int count = m_StringList.GetCount();
					POSITION pos = m_StringList.GetHeadPosition();
					m_StringMsg = m_StringList.GetNext(pos);

					int str_len = m_StringMsg.GetLength();
					LPSTR lpStr = m_StringMsg.GetBufferSetLength(str_len);

/*					m_str_len = 0;
					for(int i = 0; i < str_len; i++)
					{
						m_str_len++;
						if(lpStr[i] & 0x80 ) ++i;
					}
					m_StringMsg.ReleaseBuffer();
*/
					//--------------------------------
					//---------------------------------------------------
					//CRect rtDst = m_arrArea[0]->m_vRect;
					CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
					CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);

					//TEXTMETRIC Metrics;

					//pDC->GetTextMetrics(&Metrics);
					//m_charwidth = Metrics.tmMaxCharWidth;
					//m_charheight = Metrics.tmHeight;

					pDC->DrawText(m_StringMsg, &rtDst,  DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EXTERNALLEADING|DT_CALCRECT);
					m_DrawTxtHeight = rtDst.Height() / m_charheight;
					m_StartLine = 0;

					pDC->SelectObject(old_font);
					CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);
					//---------------------------------------------------
					//--------------------------------
				}
				else HideWindow();
			}
		}
		//=================================================

		m_LClick = false;
	}

	return TRUE;
}


int CDialog_EventMessage2::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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

BOOL CDialog_EventMessage2::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	return TRUE;	
}


BOOL CDialog_EventMessage2::OnRButtonDown(BBDX_MouseInformation *pMouseInfo)
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

BOOL CDialog_EventMessage2::OnRButtonUp(BBDX_MouseInformation *pMouseInfo)
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

void CDialog_EventMessage2::AddString(PBYTE bypData)
{
	int			uid = 0;
	int			offset = 0;
	int			str_len;
	CString		str;
	LPSTR		lpStr;
	
	str_len = CPacketParser::GetShort(bypData, offset);
	lpStr = str.GetBufferSetLength(str_len);
	CPacketParser::GetString(bypData, offset, (PBYTE)lpStr, str_len);
	str.ReleaseBuffer();
/*
	m_str_len = 0;
	for(int i = 0; i < str_len; i++)
	{
		m_str_len++;
		if(lpStr[i] & 0x80 ) ++i;
	}
*/
//---------------------------------------------------
	CRect rtDst = m_arrArea[0]->m_vRect;
	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);

	TEXTMETRIC Metrics;

	pDC->GetTextMetrics(&Metrics);
	m_charwidth = Metrics.tmMaxCharWidth;
	m_charheight = Metrics.tmHeight;

	pDC->DrawText(m_StringMsg, &rtDst,  DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EXTERNALLEADING|DT_CALCRECT);
	m_DrawTxtHeight = rtDst.Height() / m_charheight;

	pDC->SelectObject(old_font);
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);
//---------------------------------------------------

	if(m_StringList.IsEmpty()) 
	{
		m_StringList.AddTail(str);

		int count = m_StringList.GetCount();
		POSITION pos = m_StringList.GetHeadPosition();
		m_StringMsg = m_StringList.GetNext(pos);
	}
	else m_StringList.AddTail(str);

}