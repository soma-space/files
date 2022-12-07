// WndIntf.cpp : implementation file
//

#include "stdafx.h"
#include "WndIntf.h"

#include "GlobalVar.h"

/////////////////////////////////////////////////////////////////////////////
// CWndIntf

CWndIntf::CWndIntf()
{
	m_rcRecentLB.SetRect(0,0,0,0);
	m_rcRecentRB.SetRect(0,0,0,0);
	m_ptRecentLB.x = 0;
	m_ptRecentLB.y = 0;
	m_ptRecentRB.x = 0;
	m_ptRecentRB.y = 0;
}

CWndIntf::~CWndIntf()
{
}


BEGIN_MESSAGE_MAP(CWndIntf, CWnd)
	//{{AFX_MSG_MAP(CWndIntf)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CLOSE()
	ON_WM_LBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndIntf message handlers

void CWndIntf::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	g_pTEng->HWndSet(this->m_hWnd);
	
	g_pTEng->SurfaceFillRect(NULL, NULL, RGB(96,96,96));
	if(m_Dlg.m_lpDDSBkg && m_Dlg.m_CViewBkg.GetCheck()) // 배경이 있고.. 체크되어 있으면...
	{
		g_pTEng->BltNormal(m_Dlg.m_lpDDSBkg, 0, 0, NULL, FALSE);
	}

	CAniObjLSP* pAniObj = g_pIntf->AniObjGet();
	if(pAniObj == NULL) return;

	int nVI = m_Dlg.IntfGetValid(); // 현재 선택된 레이어 번호 가져오고.
	int nState = 0;

	int nVIQt = m_Dlg.IntfGetValidQt(); // 유효한 인터페이스 숫자 가져오고...

	RECT rcIntfs[MAX_INTF]; MAKE_ZERO(rcIntfs);
	RECT rcRgnIntfs[MAX_INTF]; MAKE_ZERO(rcRgnIntfs);
	POINT ptIntfs[MAX_INTF]; MAKE_ZERO(ptIntfs);
	int nCurWork = 0;

	BOOL bAni = m_Dlg.m_CPlayAnimation.GetCheck(); // 에니메이션이 되고 있는지 ??

	// 외곽선 그리기.
	CRect rcWhole = g_pIntf->m_Header.rcWhole;
	g_pTEng->DrawBox(NULL, rcWhole, RGB(255,255,255));

	for(int i = 0; i < nVIQt; i++)
	{
		if(m_Dlg.IntfGetValidCheck(i) != 0) continue; // 아이템이 체크 되어 있음 넘어간다..
		int nIntf = 0;
		m_Dlg.IntfGetValidIndex(i, &nIntf, &nState);
		
		if(nIntf == nVI) nCurWork = i; // 현재 작업중인 인터페이스이면..
		else nState = 0;

		_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
		if(pIntf == NULL) continue;
		if(pIntf->ptView.x <= 0 && pIntf->ptView.y <= 0) continue; // 찍기 위치가 0,0 이하이면 넘어간다.

		// 사각 영역과 위치를 기록해둔다.
		ptIntfs[i] = pIntf->ptView;
		rcRgnIntfs[i] = pIntf->rcRgn;


		if(pIntf->nType == INTF_TYPE_COLOR_BOX) // 컬러 채우기면 스프라이트는 찍지 않는다...
		{
			g_pTEng->SurfaceFillRect(NULL, &pIntf->rcRgn, pIntf->crFill, pIntf->crFill >> 24);
		}
		else if(pIntf->nType == INTF_TYPE_SCROLL_LR || pIntf->nType == INTF_TYPE_SCROLL_TB)
		{
			CRect rcRgn = pIntf->rcRgn;
			int min = pIntf->nFlags[0];
			int max = pIntf->nFlags[1];
			int w = pIntf->rcRgn.right - pIntf->rcRgn.left;
			int h = pIntf->rcRgn.bottom - pIntf->rcRgn.top;
			if(min >= max || w <= 0 || h <= 0) continue;

			const _SPR_LSP* pSprs[4] = { 0,0,0,0 };
			for(int j = 0; j < 4; j++)
			{
				if(bAni)
				{
					pAniObj->AniSet(pIntf->nAnis[j], FL_ANI_LOOP, nIntf*4 + j);
					pAniObj->SprGetAnimated(&pSprs[j], nIntf*4 + j);
				}
				else
				{
					_ANI_DATA* pAni = pAniObj->AniDataGet(pIntf->nAnis[j]);
					if(pAni == NULL) continue;
					pAniObj->SprGet(pAni->pnFrms[0], &pSprs[j]);
				}
			}
			
			g_pTEng->BackBufferLock();
			if(pIntf->nType == INTF_TYPE_SCROLL_LR)
			{
				if(pSprs[0]) g_pTEng->BltLSP(NULL, rcRgn.left + h/2, rcRgn.top + h/2, pSprs[0]);
				if(pSprs[1]) g_pTEng->BltLSP(NULL, rcRgn.right - h/2, rcRgn.top + h/2, pSprs[1]);
				if(pSprs[2]) g_pTEng->BltLSP(NULL, rcRgn.left + w/2, rcRgn.top + h/2, pSprs[2]);
			}
			else if(pIntf->nType == INTF_TYPE_SCROLL_TB)
			{
				if(pSprs[0]) g_pTEng->BltLSP(NULL, rcRgn.left + w/2, rcRgn.top + w/2, pSprs[0]);
				if(pSprs[1]) g_pTEng->BltLSP(NULL, rcRgn.left + w/2, rcRgn.bottom - w/2, pSprs[1]);
				if(pSprs[2]) g_pTEng->BltLSP(NULL, rcRgn.left + w/2, rcRgn.top + h/2, pSprs[2]);
			}
			g_pTEng->BackBufferUnlock();
		}
		else if(pIntf->nType == INTF_TYPE_DIAL_LR || pIntf->nType == INTF_TYPE_DIAL_TB)
		{
		}
		else if(pIntf->nType == INTF_TYPE_PROGRESS)
		{
		}
		else if(pIntf->nType == INTF_TYPE_LIST_BOX)
		{
		}
		else if(pIntf->nType == INTF_TYPE_COMBO_BOX)
		{
		}
		else
		{
			const _SPR_LSP* pSpr = NULL;
			if(bAni)
			{
				pAniObj->AniSet(pIntf->nAnis[nState], FL_ANI_LOOP, nIntf*4);
				pAniObj->SprGetAnimated(&pSpr, nIntf*4);
			}
			else
			{
				_ANI_DATA* pAni = pAniObj->AniDataGet(pIntf->nAnis[nState]);
				if(pAni == NULL) continue;
				pAniObj->SprGet(pAni->pnFrms[0], &pSpr);
			}
			if(pSpr == NULL) continue;

			SetRect(&rcIntfs[i],ptIntfs[i].x + pSpr->rcV.left, // 뷰영역정보 기록..
								ptIntfs[i].y + pSpr->rcV.top,
								ptIntfs[i].x + pSpr->rcV.right,
								ptIntfs[i].y + pSpr->rcV.bottom);
			
			g_pTEng->BackBufferLock();
			if(m_Dlg.m_CViewTrans.GetCheck() && nIntf == nVI) // 반투명 체크 되어 있고 현재 작업중인 거면....
				g_pTEng->BltLSP(NULL, pIntf->ptView.x, pIntf->ptView.y, pSpr, RENDER_TRANS);
			else
				g_pTEng->BltLSP(NULL, pIntf->ptView.x, pIntf->ptView.y, pSpr);
			g_pTEng->BackBufferUnlock();
		}
	}

	if(m_Dlg.m_CViewLine.GetCheck())
	{
		COLORREF crColor1, crColor2;
		int nDelta;
		CPoint pt1, pt2;
		for(i = 0; i < nVIQt; i++)
		{
			if(ptIntfs[i].x == 0 && ptIntfs[i].y == 0) continue;
			
			if(i == nCurWork)
			{
				nDelta = 32;
				crColor1 = RGB(0,255,0);
				crColor2 = RGB(255,255,0);
			}
			else
			{
				nDelta = 16;
				crColor1 = RGB(0,128,0);
				crColor2 = RGB(128,128,0);
			}

			g_pTEng->DrawBox(NULL, &rcIntfs[i], crColor1);
			g_pTEng->DrawBox(NULL, &rcRgnIntfs[i], crColor2);

			pt1 = ptIntfs[i]; pt1.x -= 20; pt2 = ptIntfs[i]; pt2.x += 20;
			g_pTEng->DrawLine(NULL, pt1.x, pt1.y, pt2.x, pt2.y, crColor1);
			pt1 = ptIntfs[i]; pt1.y -= 20; pt2 = ptIntfs[i]; pt2.y += 20;
			g_pTEng->DrawLine(NULL, pt1.x, pt1.y, pt2.x, pt2.y, crColor1);

			if(rcIntfs[i].left != 0 && rcIntfs[i].top != 0)
			{
				CString str; str.Format("%d", i);
//				g_pTEng->PutFontOutline(rcIntfs[i].left + 2, rcIntfs[i].top + 2, crColor2, str);
			}
			else if(rcRgnIntfs[i].left != 0 && rcRgnIntfs[i].top != 0)
			{
				CString str; str.Format("%d", i);
//				g_pTEng->PutFontOutline(rcRgnIntfs[i].left + 2, rcRgnIntfs[i].top + 2, crColor2, str);
			}
		}
	}
	
	if(m_Dlg.m_CViewZoom.GetCheck())
	{
//		CRect rcClient;
//		GetClientRect(&rcClient);
//		int w = rcClient.Width();
//		int h = rcClient.Height();
//		CRect rcSrc(0,0,w/2,h/2);
//
//		CPoint pt(0,0);
//		ClientToScreen(&pt);
//
//		rcClient += pt;
//
//		g_pTEng->m_lpPrimary->Blt(&rcClient, g_pTEng->m_lpBackBuffer, &rcSrc, NULL, NULL);
	}
	else
	{
		g_pTEng->Flip();
	}

}

void CWndIntf::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_Dlg.m_CViewZoom.GetCheck()) // 줌된 상태면
	{
		point.x  = point.x / 2;
		point.y  = point.y / 2;
	}

	m_ptRecentLB = point; // 최근 찍었던 위치 기억..
	m_rcRecentLB.left = point.x; m_rcRecentLB.top = point.y;

	// 누르면 힛트 테스트
	int nIntfQt = m_Dlg.IntfGetValidQt();
	_INTF_DATA* pIntf = NULL;
	_ANI_DATA* pAni = NULL;
	const _SPR_LSP* pSpr = NULL;
	int tmp = -1;
	CRect rcHit1(0,0,0,0), rcHit2(0,0,0,0);
	int nIntf = 0, nState = 0;
	for(int i = nIntfQt - 1; i >= 0; i--)
	{
		if(m_Dlg.IntfGetValidCheck(i)) continue; // 보이지 않기 체크되어 있음 넘어간다..
		m_Dlg.IntfGetValidIndex(i, &nIntf, &nState);
		pIntf = g_pIntf->IntfGet(nIntf);
		if(pIntf == NULL) continue;
		if(pIntf->ptView.x == 0 && pIntf->ptView.y == 0) continue;
		pAni = g_pIntf->AniObjGet()->AniDataGet(pIntf->nAnis[nState]);
		if(pAni == NULL) continue;
		g_pIntf->AniObjGet()->SprGet(pAni->pnFrms[0], &pSpr);
		if(pSpr == NULL) continue;
		
		rcHit1 = pIntf->rcRgn;
		rcHit2.SetRect(	pIntf->ptView.x + pSpr->rcV.left, 
						pIntf->ptView.y + pSpr->rcV.top, 
						pIntf->ptView.x + pSpr->rcV.right, 
						pIntf->ptView.y + pSpr->rcV.bottom);

		if(rcHit1.PtInRect(point) || rcHit2.PtInRect(point))
		{
			tmp = nIntf;
			m_Dlg.IntfSetValid(tmp);
			break;
		}
	}

	InvalidateRect(NULL);
	m_Dlg.Refresh(); // 대화상자 정보 업데이트

/*	_INTF_HEADER Header;
	_INTF_DATA Data;
	const _SPR_LSP* pSpr;
	g_pIntf->GetHeader(&Header);
	CRect rcHit;
	for(int i = 0; i < Header.nInterfaceQt; i++) // 히트 테스트
	{
		g_pIntf->IntfGet(i, &Data);
		if(Data.ptView.x == 0 && Data.ptView.y == 0) continue;
		g_pIntf->SprGetAnimated(&pSpr, i*3+Data.nRenderTypes[0]);
		if(!pSpr) continue;
		CPoint ptC = pSpr->ptCV;
		CRect rcSrc = pSpr->rcV;
		g_pTEng->BltCenter(g_pIntf->m_lpdds, Data.ptView, ptC, rcSrc, FALSE, TRUE, Data.nRenderTypes[0], 16, 0);
		rcHit.left = Data.ptView.x - (ptC.x - rcSrc.left);
		rcHit.top = Data.ptView.y - (ptC.y - rcSrc.top);
		rcHit.right = rcHit.left + rcSrc.Width();
		rcHit.bottom = rcHit.top + rcSrc.Height();

		if(rcHit.PtInRect(point))
		{
			g_pIntf->SetCurInterface(i);
			m_Dlg.Refresh(); // 컨트롤 다이얼로그 갱신..
			InvalidateRect(NULL, FALSE);
			break;
		}
	}
*/
	CWnd::OnLButtonDown(nFlags, point);
}

void CWndIntf::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_Dlg.m_CViewZoom.GetCheck()) // 줌된 상태면
	{
		point.x  = point.x / 2;
		point.y  = point.y / 2;
	}

	// TODO: Add your message handler code here and/or call default
	if(nFlags & MK_LBUTTON)
	{
		CPoint ptDelta = point - m_ptRecentLB; // 최근 
		m_ptRecentLB = point;
		m_rcRecentLB.right = point.x; m_rcRecentLB.bottom = point.y;

		if(nFlags & MK_CONTROL)
		{
			int nVI = m_Dlg.IntfGetValid(); // 현재 선택된 레이어 번호 가져오고.
			int nVIQt = m_Dlg.IntfGetValidQt(); // 유효한 인터페이스 숫자 가져오고...
			int nIntf = 0, nState = 0;

			for(int i = 0; i < nVIQt; i++)
			{
				m_Dlg.IntfGetValidIndex(i, &nIntf, &nState);
				_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
				if(pIntf == NULL) continue;
				if(pIntf->ptView.x == 0 && pIntf->ptView.y == 0) continue;
				
				pIntf->ptView.x += ptDelta.x;
				pIntf->ptView.y += ptDelta.y;
				pIntf->rcRgn.left += ptDelta.x;
				pIntf->rcRgn.top += ptDelta.y;
				pIntf->rcRgn.right += ptDelta.x;
				pIntf->rcRgn.bottom += ptDelta.y;
			}
		}
		else
		{
			int nVI = m_Dlg.IntfGetValid();
			if(nVI < 0 || nVI >= MAX_INTF) return; // 트리에 선택된 인터페이스 번호 가져오기..

			_INTF_DATA* pIntf = g_pIntf->IntfGet(nVI);
			if(pIntf == NULL) return; // 데이터 가져오기

			pIntf->ptView.x += ptDelta.x;
			pIntf->ptView.y += ptDelta.y; // 찍는 점 움직이기..
			pIntf->rcRgn.left += ptDelta.x;
			pIntf->rcRgn.top += ptDelta.y;
			pIntf->rcRgn.right += ptDelta.x;
			pIntf->rcRgn.bottom += ptDelta.y;
		}
		m_Dlg.Refresh(); // 대화상자 정보 업데이트
		InvalidateRect(NULL, FALSE);
	}
	else if(nFlags == MK_RBUTTON)
	{
		m_ptRecentRB = point;
		m_rcRecentRB.right = point.x;
		m_rcRecentRB.bottom = point.y;

		int nVI = m_Dlg.IntfGetValid();
		if(nVI < 0 || nVI >= MAX_INTF) return; // 트리에 선택된 인터페이스 번호 가져오기..

		_INTF_DATA* pIntf = g_pIntf->IntfGet(nVI);
		if(pIntf == NULL) return; // 데이터 가져오기

		m_rcRecentRB.NormalizeRect();
		pIntf->rcRgn = m_rcRecentRB; // 체크 영역 움직이기..

		m_Dlg.Refresh(); // 대화상자 정보 업데이트
		InvalidateRect(NULL, FALSE);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CWndIntf::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(m_Dlg.m_CViewZoom.GetCheck()) // 줌된 상태면
	{
		point.x  = point.x / 2;
		point.y  = point.y / 2;
	}

	m_ptRecentRB = point;
	m_rcRecentRB.left = point.x;
	m_rcRecentRB.top = point.y;
	CWnd::OnRButtonDown(nFlags, point);
}

void CWndIntf::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if(m_Dlg.m_CViewZoom.GetCheck()) // 줌된 상태면
	{
		point.x  = point.x / 2;
		point.y  = point.y / 2;
	}

	CWnd::OnRButtonUp(nFlags, point);
}

void CWndIntf::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	POINT ptDelta = {0,0};
	switch(nChar)
	{
	case VK_LEFT:
		ptDelta.x = -1;
		break;
	case VK_RIGHT:
		ptDelta.x = +1;
		break;
	case VK_UP:
		ptDelta.y = -1;
		break;
	case VK_DOWN:
		ptDelta.y = +1;
		break;
	}

	if(_IsKeyDown(VK_CONTROL))
	{
		int nVI = m_Dlg.IntfGetValid(); // 현재 선택된 레이어 번호 가져오고.
		int nIntf = 0, nState = 0;

		int nVIQt = m_Dlg.IntfGetValidQt(); // 유효한 인터페이스 숫자 가져오고...
		for(int i = 0; i < nVIQt; i++)
		{
			m_Dlg.IntfGetValidIndex(i, &nIntf, &nState);
			_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
			if(pIntf == NULL) continue;
			if(pIntf->ptView.x == 0 && pIntf->ptView.y == 0) continue;
			
			pIntf->ptView.x += ptDelta.x;
			pIntf->ptView.y += ptDelta.y;
			pIntf->rcRgn.left += ptDelta.x;
			pIntf->rcRgn.top += ptDelta.y;
			pIntf->rcRgn.right += ptDelta.x;
			pIntf->rcRgn.bottom += ptDelta.y;
		}
	}
	else
	{
		int nIntf = m_Dlg.IntfGetValid();
		_INTF_DATA* pIntf = g_pIntf->IntfGet(nIntf);
		if(pIntf == NULL) return;

		pIntf->ptView.x += ptDelta.x;
		pIntf->ptView.y += ptDelta.y;
		pIntf->rcRgn.left += ptDelta.x;
		pIntf->rcRgn.top += ptDelta.y;
		pIntf->rcRgn.right += ptDelta.x;
		pIntf->rcRgn.bottom += ptDelta.y;
	}	

	m_Dlg.Refresh(); // 대화상자 정보 업데이트
	InvalidateRect(NULL, FALSE);

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CWndIntf::OnClose() 
{
	g_pIntf->New();
	CWnd::OnClose();
}

void CWndIntf::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_Dlg.m_CViewZoom.GetCheck()) // 줌된 상태면
	{
		point.x  = point.x / 2;
		point.y  = point.y / 2;
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}

int CWndIntf::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(g_pIntf == NULL) g_pIntf = new CIntfTool();

	if(m_Dlg.GetSafeHwnd() == NULL)
	{
		m_Dlg.Create(IDD_INTF, this);
		m_Dlg.m_pWndWork = this;
	}
	CRect rcParent;
	this->GetWindowRect(&rcParent);
	m_Dlg.SetWindowPos(NULL, rcParent.left, rcParent.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	m_Dlg.ShowWindow(SW_SHOW);
	
	return 0;
}

void CWndIntf::OnDestroy() 
{
	CWnd::OnDestroy();

	DELETE_SAFE(g_pIntf);
}
