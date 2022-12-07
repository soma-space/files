// Dialog_EventMessage1.cpp: implementation of the CDialog_EventMessage1 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_EventMessage1.h"
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

CDialog_EventMessage1::CDialog_EventMessage1()
{
	m_pFont = NULL;

    m_bHaveMsg = false;
	m_vPosMode = POS_NORMAL;
}

CDialog_EventMessage1::~CDialog_EventMessage1()
{
	SAFE_DELETE(m_pFont);
}


BOOL CDialog_EventMessage1::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	return TRUE;
}

BOOL CDialog_EventMessage1::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;
	CPoint pt,ptMapOffset,ptDlg;
	CRect rtDst;

	ptMapOffset = CChildView::MapManager.SetCenterPos(CChildView::UserManager.GetMyPos());
	CUser *pMe = CChildView::UserManager.GetMe();
	pt = pMe->GetPosition()-ptMapOffset;

	ptDlg.x = pt.x - (m_mainRect.right - m_mainRect.left) / 2 + 20;
	ptDlg.y = pt.y - m_mainRect.bottom - m_mainRect.top - 100;
	SetOffset(ptDlg); //보여질 메세지 박스위치지정..

	m_bTrans = TRUE;
	CUIRes::Draw(pDest);

	m_arrBtn[0]->m_bTransparent = TRUE;
	Draw3ButtonCtrl(pDest, 0, 0);

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

	pDC->DrawText(m_StringMsg, &rtDst, DT_TOP|DT_LEFT|DT_EXTERNALLEADING|DT_WORDBREAK   );

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);
	return TRUE;
}

void CDialog_EventMessage1::ShowWindow()
{
	if(!IsLoading())
		if(!Create(CChildView::DirectXMng.m_pDD, "quest2.lib", 0, TRUE, FALSE, FALSE)) return;

	SetCapture();
	
	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	CUIRes::ShowWindow();
}

void CDialog_EventMessage1::HideWindow()
{
	ReleaseCapture();
	HideAllWindow();
}

void CDialog_EventMessage1::HideAllWindow()
{
	CUIRes::HideWindow();
}

BOOL CDialog_EventMessage1::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CRect		rtDlg;
	CPoint		point;
	int			size, i;

	point = pMouseInfo->GetPos();

	rtDlg.SetRect(0, 0, m_mainRect.Width(), m_mainRect.Height());
	rtDlg.OffsetRect(m_ptOffset);

	if (!rtDlg.PtInRect(point)) 
	{
		return FALSE;
	}

	size = m_arrBtn.GetSize();

	m_vSpecCtrl = -1;
	for(i = 0; i < size; i++)
	{
		if(m_arrBtn[i]->m_vRect.PtInRect(CPoint(point-m_ptOffset)))
		{
			m_vSpecCtrl = m_arrBtn[i]->m_vSpec;
			m_vIndexCtrl = m_arrBtn[i]->m_vIndex;
			if(m_vSpecCtrl == BTN_CTRL)
			{
				m_arrBtn[i]->m_vState = BUTTON_DOWN;
			}
			break;
		}
	}

	return TRUE;
}

BOOL CDialog_EventMessage1::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	CRect rtDlg;
	CPoint point;
	
	point = pMouseInfo->GetPos();

	rtDlg.SetRect(0, 0, m_mainRect.Width(), m_mainRect.Height());
	rtDlg.OffsetRect(m_ptOffset);

	if (!rtDlg.PtInRect(point)) 
	{
		return FALSE;
	}

	if(m_vSpecCtrl == BTN_CTRL)
	{
		MBtnCtrl *btn_ctrl = m_arrBtn[m_vIndexCtrl];
		if(btn_ctrl->m_vRect.PtInRect(CPoint(point-m_ptOffset)))
		{
			switch(m_vIndexCtrl)	
			{
				case 0: HideWindow(); break;
			}
			btn_ctrl->m_vState = BUTTON_NORMAL;
		}
	}

	return	TRUE;
}

int CDialog_EventMessage1::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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
			if(pMouseInfo->IsRButtonUp()) {
				OnRButtonUp(pMouseInfo);
			}
			return 2;
		}
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

BOOL CDialog_EventMessage1::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	return CUIRes::OnMouseMove(pMouseInfo);
}


BOOL CDialog_EventMessage1::OnRButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CRect rtDlg;
	CPoint point;
	
	rtDlg.SetRect(0, 0, m_mainRect.Width(), m_mainRect.Height());
	rtDlg.OffsetRect(m_ptOffset);
	point = pMouseInfo->GetPos();
	if (!rtDlg.PtInRect(point)) {
		return FALSE;
	}

	return TRUE;
}

BOOL CDialog_EventMessage1::OnRButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	CRect rtDlg;
	CPoint point;
	
	rtDlg.SetRect(0, 0, m_mainRect.Width(), m_mainRect.Height());
	rtDlg.OffsetRect(m_ptOffset);
	point = pMouseInfo->GetPos();
	if (!rtDlg.PtInRect(point)) {
		return FALSE;
	}

	return TRUE;
}

void CDialog_EventMessage1::AddString(PBYTE bypData)
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
	m_StringMsg = str;

/*	m_str_len = 0;
	for(int i = 0; i < str_len; i++)
	{
		m_str_len++;
		if(lpStr[i] & 0x80 ) ++i;
	}
*/
}

