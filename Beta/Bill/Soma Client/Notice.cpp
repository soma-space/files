// Notice.cpp: implementation of the CNotice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Notice.h"
#include "DManage/BBDX_MouseInformation.h"
#include "UIRes.h"
#include "./DManage/BB_Direct.h"
#include "ChildView.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotice::CNotice()
{
	m_pFont = NULL;

    m_bHaveMsg = false;
	m_bExit = false;
}

CNotice::~CNotice()
{
	SAFE_DELETE(m_pFont);
}


BOOL CNotice::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	return TRUE;
}

BOOL CNotice::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;
	CPoint pt;
	CRect rtDst;

	m_bTrans = TRUE;
	CUIRes::Draw(pDest);

	m_arrBtn[0]->m_bTransparent = TRUE;
	Draw3ButtonCtrl(pDest, 0, 0);

	rtDst = m_arrArea[0]->m_vRect;

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

void CNotice::ShowWindow()
{
	if(!IsLoading())
		if(!Create(CChildView::DirectXMng.m_pDD, "notice.lib", 0, TRUE, FALSE, FALSE)) return;

	m_bShow = TRUE;
}

void CNotice::HideWindow()
{
	HideAllWindow();
}

void CNotice::HideAllWindow()
{
	m_bShow = FALSE;
}

BOOL CNotice::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CPoint		point;
	int			size, i;

	point = pMouseInfo->GetPos();

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

BOOL CNotice::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	CPoint point;
	
	point = pMouseInfo->GetPos();

	if(m_vSpecCtrl == BTN_CTRL)
	{
		MBtnCtrl *btn_ctrl = m_arrBtn[m_vIndexCtrl];
		if(btn_ctrl->m_vRect.PtInRect(CPoint(point-m_ptOffset)))
		{
			switch(m_vIndexCtrl)	
			{
				case 0: HideWindow(); m_bExit = true; break;
			}
			btn_ctrl->m_vState = BUTTON_NORMAL;
		}
	}

	return	TRUE;
}

int CNotice::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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
			return 2;
		}
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

BOOL CNotice::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	return CUIRes::OnMouseMove(pMouseInfo);
}

void CNotice::AddString(PBYTE bypData)
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
	lpStr[str_len] = '0';
	m_StringMsg = str;
}

void CNotice::KillFocus()
{
}

void CNotice::SetFocus()
{
}