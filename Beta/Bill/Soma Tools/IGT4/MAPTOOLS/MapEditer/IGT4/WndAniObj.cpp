// WndAniObj.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "WndAniObj.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndAniObj
CWndAniObj::CWndAniObj()
{
}

CWndAniObj::~CWndAniObj()
{
}


BEGIN_MESSAGE_MAP(CWndAniObj, CWnd)
	//{{AFX_MSG_MAP(CWndAniObj)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndAniObj message handlers

void CWndAniObj::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT)); // 스크롤 위치..

	if(m_Dlg.GetSafeHwnd() == NULL)
	{
		dc.FillSolidRect(&dc.m_ps.rcPaint, RGB(0,0,0));
		return;
	}

	int nWork = m_Dlg.m_CBWork.GetCurSel();
	if(m_Dlg.m_bPlaying == TRUE && nWork == 0) nWork = 1; // 에니메이션 바로 플레이.. 

	g_pTEng->SurfaceFillRect(NULL, &dc.m_ps.rcPaint, RGB(64,64,64));

	if(nWork == 0) // 수동 스프라이트 설정 작업...
	{
		if(m_Dlg.m_pAniObj->m_lpDDS != NULL) // 작업용 서피스가 있으면..
		{
			CRect rc1 = dc.m_ps.rcPaint;
			CRect rc2; g_pTEng->SurfaceGetSize(m_Dlg.m_pAniObj->m_lpDDS, &rc2);
			rc1 += ptScroll;
			rc2.left += ptScroll.x;
			rc2.top += ptScroll.y;
			CRect rcBlt;
			rcBlt.IntersectRect(&rc1, &rc2);

			g_pTEng->BltNormal(m_Dlg.m_pAniObj->m_lpDDS, dc.m_ps.rcPaint.left, dc.m_ps.rcPaint.top, &rcBlt, TRUE);
			
			int nSpr = m_Dlg.m_SliderSpr.GetPos(); // 현재 스프라이트 번호..
			int nSprQt = m_Dlg.m_pAniObj->m_HSpr.nQt; // 현재 작업중인 스프라이트 갯수
			_SPR_TOOL spr;
			COLORREF crBox;
			CRect rcBox;
			for(int i = 0; i < nSprQt; i++)
			{
				if(i == nSpr) crBox = RGB(0,255,0);
				else crBox = RGB(0,128,0);

				m_Dlg.m_pAniObj->SprExGetData(i, &spr);
				// 박스
				rcBox = spr.rcV; rcBox -= ptScroll;
				g_pTEng->DrawBox(NULL, &rcBox, crBox);
				
				CPoint ptC = spr.ptCV; ptC -= ptScroll;
				// 십자선
				g_pTEng->DrawLine(NULL, ptC.x - 10, ptC.y, ptC.x + 10, ptC.y, crBox);
				g_pTEng->DrawLine(NULL, ptC.x, ptC.y - 10, ptC.x, ptC.y + 10, crBox);

				// 글씨 찍기...
				CString szNumSpr; szNumSpr.Format("%d", i);
				if(rcBox.right != 0 && rcBox.bottom != 0)
					g_pTEng->TextOut(NULL, rcBox.right, rcBox.bottom, szNumSpr, RGB(255, 255, 0));
			}
		}
		else // 작업용 서피스가 없으면..
		{
			int nSpr = m_Dlg.m_SliderSpr.GetPos(); // 현재 스프라이트 번호..
			const _SPR_LSP* pSpr = NULL;
			m_Dlg.m_pAniObj->SprGet(nSpr, &pSpr);
			if(pSpr != NULL)
			{
				CRect rc;
				GetClientRect(&rc);
				int x = rc.Width()/2, y = rc.Height()/2; 
				// 십자선
				g_pTEng->DrawLine(NULL, dc.m_ps.rcPaint.left, y, dc.m_ps.rcPaint.right, y, RGB(0,0,255));
				g_pTEng->DrawLine(NULL, x, dc.m_ps.rcPaint.top, x, dc.m_ps.rcPaint.bottom, RGB(0,0,255));

				g_pTEng->BackBufferLock();
				g_pTEng->BltLSP(NULL, x, y, pSpr); // LSP 스프라이트..
				g_pTEng->BackBufferUnlock();
			}
		}
	}
	else if(nWork == 1) // 에니메이션
	{
		int x, y;
		
		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(this->m_hWnd, &pt);

		CRect rc;
		GetClientRect(&rc);
		if(_IsKeyDown(VK_LBUTTON) && rc.PtInRect(pt)) { x = pt.x; y = pt.y; }
		else { x = rc.Width()/2, y = rc.Height()/2; }

		// 십자선
		g_pTEng->DrawLine(NULL, dc.m_ps.rcPaint.left, y, dc.m_ps.rcPaint.right, y, RGB(0,0,255));
		g_pTEng->DrawLine(NULL, x, dc.m_ps.rcPaint.top, x, dc.m_ps.rcPaint.bottom, RGB(0,0,255));

		int nAni = m_Dlg.m_ListAni.GetCurSel();
		int nDir = m_Dlg.m_CBDir.GetCurSel();
		if(m_Dlg.m_bPlaying == FALSE)
		{
			int nFrm = m_Dlg.GetDlgItemInt(IDC_E_ANI_CUR_FRM);
			int nSpr = m_Dlg.m_pAniObj->FrmGetByAniAndDirAndSeq(nAni, nDir, nFrm);
			const _SPR_LSP* pSpr = NULL;

			m_Dlg.m_pAniObj->SprGet(nSpr, &pSpr);
			if(pSpr != NULL)
			{
				g_pTEng->BackBufferLock();
				g_pTEng->BltLSP(NULL, x, y, pSpr);
				g_pTEng->BackBufferUnlock();
			}
			else if(m_Dlg.m_pAniObj->m_lpDDS)
			{
				_SPR_TOOL spr;
				m_Dlg.m_pAniObj->SprExGetData(nSpr, &spr);
				g_pTEng->BltCenter(m_Dlg.m_pAniObj->m_lpDDS, x, y, &spr.rcV, &spr.ptCV, TRUE);
			}
		}
		else // 에니메이션이 재생되고 있을 경우
		{
			const _SPR_LSP* pSpr = NULL;
			m_Dlg.m_pAniObj->AniSet(nAni);
			m_Dlg.m_pAniObj->DirSet(nDir, 0);
			m_Dlg.m_pAniObj->SprGetAnimated(&pSpr, 0);
			if(pSpr != NULL) 
			{
				g_pTEng->BackBufferLock();
				g_pTEng->BltLSP(NULL, x, y, pSpr);
				g_pTEng->BackBufferUnlock();
			}
			else if(m_Dlg.m_pAniObj->m_lpDDS)
			{
				int nSpr = m_Dlg.m_pAniObj->FrmGetAnimated(0);
				_SPR_TOOL spr;
				m_Dlg.m_pAniObj->SprExGetData(nSpr, &spr);
				g_pTEng->BltCenter(m_Dlg.m_pAniObj->m_lpDDS, x, y, &spr.rcV, &spr.ptCV, TRUE);
			}
		}
	}
	g_pTEng->HWndSet(this->m_hWnd);
	g_pTEng->Flip(&dc.m_ps.rcPaint);

	// Do not call CWnd::OnPaint() for painting messages
}

int CWndAniObj::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if(m_Dlg.GetSafeHwnd() == NULL)
	{
		m_Dlg.Create(IDD_ANI_OBJ, this);
	}
	CRect rc;
	this->GetWindowRect(&rc);
	m_Dlg.m_pWndWork = this; // 작업 윈도우....
	m_Dlg.SetWindowPos(NULL, rc.right, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER); // 오른쪽으로..
	m_Dlg.ShowWindow(SW_SHOW);

	return 0;
}

void CWndAniObj::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	m_Dlg.DestroyWindow();
}

void CWndAniObj::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT)); // 스크롤 위치..
	CPoint pt = point + ptScroll;

	int nSpr = m_Dlg.m_SliderSpr.GetPos(); // 현재 스프라이트 번호..
	int nSprQt = m_Dlg.m_pAniObj->m_HSpr.nQt; // 현재 작업중인 스프라이트 갯수
	if(nSpr < 0 || nSpr >= nSprQt) return;
	_SPR_TOOL spr;
	m_Dlg.m_pAniObj->SprExGetData(nSpr, &spr);

	spr.rcV.left = pt.x; spr.rcV.top = pt.y; spr.rcV.right = pt.x; spr.rcV.bottom = pt.y;
	m_Dlg.m_pAniObj->SprExSetData(nSpr, &spr);

	InvalidateRect(NULL, FALSE); // 찍은 영역 만큼...

	m_ptLBDown = point;

	CWnd::OnLButtonDown(nFlags, point);
}

void CWndAniObj::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT)); // 스크롤 위치..

	m_ptLBUp = point;
	
	if(m_Dlg.m_CBWork.GetCurSel() != 0) return; // 스프라이트 사용자 설정
	if(m_Dlg.m_pAniObj->m_lpDDS == NULL) return; // 서피스가 없으면...
	
	int n = m_Dlg.m_SliderSpr.GetPos();
	if(n < 0 && n >= m_Dlg.m_pAniObj->m_HSpr.nQt) return; // 지정 스프라이트 번호가 이상하면..

	CRect rc(m_ptLBDown, m_ptLBUp);
	rc += ptScroll;

	if(rc.Width() <= 5 && rc.Height() <= 5) // 마우스 움직임이 별로 없음...
	{
		rc = g_pTEng->FindRegionAuto(m_Dlg.m_pAniObj->m_lpDDS, rc.BottomRight(), m_Dlg.m_pAniObj->m_HSpr.SrcCK); // 영역 찾기
	}
	else
	{
		rc = g_pTEng->FindRegionManual(m_Dlg.m_pAniObj->m_lpDDS, rc, m_Dlg.m_pAniObj->m_HSpr.SrcCK); // 영역 찾기
	}
	COLORREF crCenter = 0; // 센터점 색...
	if(m_Dlg.IsDlgButtonChecked(IDC_C_SPR_CENTER_R)) crCenter |= RGB(255,0,0);
	if(m_Dlg.IsDlgButtonChecked(IDC_C_SPR_CENTER_G)) crCenter |= RGB(0,255,0);
	if(m_Dlg.IsDlgButtonChecked(IDC_C_SPR_CENTER_B)) crCenter |= RGB(0,0,255);
	CPoint ptCenter = g_pTEng->FindCenterAuto(m_Dlg.m_pAniObj->m_lpDDS, rc, crCenter);

	_SPR_TOOL spr;
	spr.rcV = rc;
	spr.ptCV = ptCenter;
	m_Dlg.m_pAniObj->SprExSetData(n, &spr);

	InvalidateRect(NULL, FALSE);

	if(m_Dlg.IsDlgButtonChecked(IDC_C_NEXT_AUTO))
	{
		int n = m_Dlg.m_SliderSpr.GetPos();
		if(n >= 0 && n < m_Dlg.m_pAniObj->m_HSpr.nQt-1) n++;
		m_Dlg.m_SliderSpr.SetPos(n);
		m_Dlg.UpdateSprInfo();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CWndAniObj::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT)); // 스크롤 위치..
	CPoint pt = point + ptScroll;

	if(nFlags & MK_LBUTTON)
	{
		int nSpr = m_Dlg.m_SliderSpr.GetPos(); // 현재 스프라이트 번호..
		int nSprQt = m_Dlg.m_pAniObj->m_HSpr.nQt; // 현재 작업중인 스프라이트 갯수
		if(nSpr < 0 || nSpr >= nSprQt) return;
		_SPR_TOOL spr;
		m_Dlg.m_pAniObj->SprExGetData(nSpr, &spr);

		CRect rcPrev = spr.rcV;
		
		spr.rcV.right = pt.x;
		spr.rcV.bottom = pt.y;
		CRect rc = spr.rcV;
		rc.NormalizeRect();
		spr.rcV = rc;
		m_Dlg.m_pAniObj->SprExSetData(nSpr, &spr);

		rcPrev -= ptScroll;
		InvalidateRect(&rcPrev, FALSE); // 찍은 영역 만큼...
		rc -= ptScroll;
		InvalidateRect(&rc, FALSE);

		m_ptLMouse = point;
	}
	if(nFlags & MK_RBUTTON)
	{
		int nSpr = m_Dlg.m_SliderSpr.GetPos(); // 현재 스프라이트 번호..
		int nSprQt = m_Dlg.m_pAniObj->m_HSpr.nQt; // 현재 작업중인 스프라이트 갯수
		if(nSpr < 0 || nSpr >= nSprQt) return;
		_SPR_TOOL spr;
		m_Dlg.m_pAniObj->SprExGetData(nSpr, &spr);

		CRect rcPrev; rcPrev.SetRect(spr.ptCV.x - 10, spr.ptCV.y - 10, spr.ptCV.x + 10, spr.ptCV.y + 10);

		spr.ptCV = pt;
		m_Dlg.m_pAniObj->SprExSetData(nSpr, &spr);

		rcPrev -= ptScroll;
		InvalidateRect(&rcPrev, FALSE); // 찍은 영역 만큼...
		pt.x -= ptScroll.x; pt.y -= ptScroll.y;
		CRect rc(pt.x - 10, pt.y - 10, pt.x + 10, pt.y + 10);
		InvalidateRect(&rc, FALSE); // 찍은 영역 만큼...

		m_ptRMouse = point;
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CWndAniObj::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT)); // 스크롤 위치..
	
	CPoint pt = point + ptScroll;

	int nSpr = m_Dlg.m_SliderSpr.GetPos(); // 현재 스프라이트 번호..
	int nSprQt = m_Dlg.m_pAniObj->m_HSpr.nQt; // 현재 작업중인 스프라이트 갯수
	if(nSpr < 0 || nSpr >= nSprQt) return;
	_SPR_TOOL spr;
	m_Dlg.m_pAniObj->SprExGetData(nSpr, &spr);
	spr.ptCV = pt;
	m_Dlg.m_pAniObj->SprExSetData(nSpr, &spr);

	InvalidateRect(NULL, FALSE);

	m_ptRBDown = point;
	
	CWnd::OnRButtonDown(nFlags, point);
}

void CWndAniObj::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT)); // 스크롤 위치..

	m_ptRBUp = point;
	
	CWnd::OnRButtonUp(nFlags, point);
}

void CWndAniObj::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CPoint ptScroll(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT)); // 스크롤 위치..

	switch(nChar)
	{
	case VK_TAB:
		{
			int n = m_Dlg.m_SliderSpr.GetPos();
			if(_IsKeyDown(VK_SHIFT))
			{
				if(n > 0 && n < m_Dlg.m_pAniObj->m_HSpr.nQt) n--;
			}
			else
			{
				if(n >= 0 && n < m_Dlg.m_pAniObj->m_HSpr.nQt-1) n++;
			}
			m_Dlg.m_SliderSpr.SetPos(n);
			m_Dlg.UpdateSprInfo();
			InvalidateRect(NULL, FALSE);
		}
		break;
	case VK_UP:
		{
			CRect rc;
			GetClientRect(&rc);
			ptScroll.y -= rc.Height()/2;
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, ptScroll.y), NULL);
		}
		break;
	case VK_DOWN:
		{
			CRect rc;
			GetClientRect(&rc);
			ptScroll.y += rc.Height()/2;
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, ptScroll.y), NULL);
		}
		break;
	case VK_LEFT:
		{
			CRect rc;
			GetClientRect(&rc);
			ptScroll.x -= rc.Width()/2;
			SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, ptScroll.x), NULL);
		}
		break;
	case VK_RIGHT:
		{
			CRect rc;
			GetClientRect(&rc);
			ptScroll.x += rc.Width()/2;
			SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, ptScroll.x), NULL);
		}
		break;
	}
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CWndAniObj::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pSHcrollBar) 
{
	if(m_Dlg.m_pAniObj->m_HSpr.nWidth <= 0 || m_Dlg.m_pAniObj->m_HSpr.nHeight <= 0) return; 

	int min, max;
	GetScrollRange(SB_HORZ, &min, &max);
	int pos = GetScrollPos(SB_HORZ);

	int delta = (max - min) / 10;

	switch(nSBCode)
	{
	case SB_LINEUP:
		pos -= 10;
		break;
	case SB_LINEDOWN:
		pos += 10;
		break;
	case SB_PAGEUP:
		pos -= delta;
		break;
	case SB_PAGEDOWN:
		pos += delta;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		pos = nPos;
		break;
	}

	// 스크롤 범위 검사후 세팅
	if(pos < min) pos = min;
	if(pos > max) pos = max;
	SetScrollPos(SB_HORZ, pos);

	InvalidateRect(NULL, FALSE);

	CWnd::OnHScroll(nSBCode, nPos, pSHcrollBar);
}

void CWndAniObj::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pSHcrollBar) 
{
	if(m_Dlg.m_pAniObj->m_HSpr.nWidth <= 0 || m_Dlg.m_pAniObj->m_HSpr.nHeight <= 0) return; 
	
	int min, max;
	GetScrollRange(SB_VERT, &min, &max);
	int pos = GetScrollPos(SB_VERT);

	int delta = (max - min) / 10;

	switch(nSBCode)
	{
	case SB_LINEUP:
		pos -= 10;
		break;
	case SB_LINEDOWN:
		pos += 10;
		break;
	case SB_PAGEUP:
		pos -= delta;
		break;
	case SB_PAGEDOWN:
		pos += delta;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		pos = nPos;
		break;
	}

	// 스크롤 범위 검사후 세팅
	if(pos < min) pos = min;
	if(pos > max) pos = max;
	SetScrollPos(SB_VERT, pos);

	InvalidateRect(NULL, FALSE);

	CWnd::OnVScroll(nSBCode, nPos, pSHcrollBar);
}

void CWndAniObj::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	this->RecalcScrollBar();
}

void CWndAniObj::RecalcScrollBar()
{
	if(m_Dlg.m_pAniObj->m_lpDDS)
	{
		EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
		EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
	}
	
	CRect rc1; GetClientRect(&rc1);
	CRect rc2; g_pTEng->SurfaceGetSize(m_Dlg.m_pAniObj->m_lpDDS, &rc2);
	if(rc2.Width() > rc1.Width())
	{
		EnableScrollBar(SB_HORZ, ESB_ENABLE_BOTH);
		SetScrollRange(SB_HORZ, 0, rc2.Width() - rc1.Width(), TRUE);
	}
	else
	{
		EnableScrollBar(SB_HORZ, ESB_DISABLE_BOTH);
		SetScrollRange(SB_HORZ, 0, 0, TRUE);
	}
	if(rc2.Height() > rc1.Height())
	{
		EnableScrollBar(SB_VERT, ESB_ENABLE_BOTH);
		SetScrollRange(SB_VERT, 0, rc2.Height() - rc1.Height(), TRUE);
	}
	else
	{
		EnableScrollBar(SB_VERT, ESB_DISABLE_BOTH);
		SetScrollRange(SB_VERT, 0, 0, TRUE);
	}
}

BOOL CWndAniObj::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(m_Dlg.m_CBWork.GetCurSel() == 0) // 수동 스프라이트 설정
	{
		if(nFlags & MK_SHIFT) 
		{
			int nPos = this->GetScrollPos(SB_HORZ);
			this->OnHScroll(SB_THUMBPOSITION, nPos - zDelta/WHEEL_DELTA*5, NULL);
		}
		else
		{
			int nPos = this->GetScrollPos(SB_VERT);
			this->OnVScroll(SB_THUMBPOSITION, nPos - zDelta/WHEEL_DELTA*5, NULL);
		}
	}	
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
