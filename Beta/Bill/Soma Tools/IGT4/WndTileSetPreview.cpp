// WndTileSetPreview.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "WndTileSetPreview.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

POINT PosToCell(POINT p);
POINT CellToPos(POINT p);
/////////////////////////////////////////////////////////////////////////////
// CWndTileSetPreview
CWndTileSetPreview::CWndTileSetPreview()
{
	m_nTSCur = 0;
	m_nTSSCur = 0;
	m_nAniCur = 0;
	m_MSM.SetViewPtr((CWnd *)this);
}

CWndTileSetPreview::~CWndTileSetPreview()
{
}


BEGIN_MESSAGE_MAP(CWndTileSetPreview, CWnd)
	//{{AFX_MSG_MAP(CWndTileSetPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndTileSetPreview message handlers

void CWndTileSetPreview::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_DlgHouse.m_vHouse && m_DlgHouse.m_vHouseOption && m_nTSSCur == 3)
	{
		m_MSM.SetStartPos(point);
		SetCapture();
	}
	else
	{
		CPoint pt(point.x/TILE_SIZE, point.y/TILE_SIZE);
		
		_TILE_SUB_SET_DATA_TOOL* pTSData = g_pTSet->DataGetSafe(m_nTSCur, m_nTSSCur);
		if(pTSData == NULL) return;
		if(pt.x >= pTSData->nWidth || pt.y >= pTSData->nHeight) return;

		if(nFlags & MK_CONTROL) // 안쓰는 부분으로 만든다..
		{
			POINT* pPtTmp = pTSData->GetPos(pt, m_nAniCur);
			if(pPtTmp != NULL)
			{
				if(pPtTmp->x < 0 || pPtTmp->y < 0)
				{
					pPtTmp->x = 0;
					pPtTmp->y = 0;
				}
				else
				{
					pPtTmp->x = -1;
					pPtTmp->y = -1;
				}
				InvalidateRect(NULL, FALSE);
			}
		}
		
		if(nFlags & MK_SHIFT) // 투명색 찍기로 만든다.
		{
			if(pTSData->pnFlags[m_nAniCur][pt.y*pTSData->nWidth + pt.x] & 2) pTSData->pnFlags[m_nAniCur][pt.y*pTSData->nWidth + pt.x] -= 2;
			else pTSData->pnFlags[m_nAniCur][pt.y*pTSData->nWidth + pt.x] |= 2;
			InvalidateRect(NULL, FALSE);
		}
		
		if(_IsKeyDown(VK_MENU)) // 충돌로 만든다..
		{
			if(pTSData->pnFlags[m_nAniCur][pt.y*pTSData->nWidth + pt.x] & 1) pTSData->pnFlags[m_nAniCur][pt.y*pTSData->nWidth + pt.x] -= 1;
			else pTSData->pnFlags[m_nAniCur][pt.y*pTSData->nWidth + pt.x] |= 1;

			InvalidateRect(NULL, FALSE);
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CWndTileSetPreview::TileSetCur(int nTS, int nTSS)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;

	m_nTSCur = nTS;
	m_nTSSCur = nTSS;
	m_nAniCur = 0;

	m_DlgHouse.SetTileSet(nTS, nTSS);
}

void CWndTileSetPreview::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	g_pTEng->SurfaceFillRect(NULL, &dc.m_ps.rcPaint, RGB(64,64,64)); // 시커멓게 한다..

	_TILE_SUB_SET_DATA_TOOL* pTSData = g_pTSet->DataGetSafe(m_nTSCur, m_nTSSCur);
	if(pTSData == NULL)
	{
		g_pTEng->HWndSet(this->m_hWnd);
		g_pTEng->Flip(&dc.m_ps.rcPaint);
		return;
	}
	if(pTSData->nWidth <= 0 || pTSData->nHeight <= 0)
	{
		g_pTEng->HWndSet(this->m_hWnd);
		g_pTEng->Flip(&dc.m_ps.rcPaint);
		return;
	}

	// 프리뷰 클라이언트 영역이 맞지 않으면 맞춘다..
	CRect rcClient;
	GetClientRect(&rcClient);

	g_pTEng->SurfaceFillRect(NULL, &rcClient, RGB(64,64,64)); // 시커멓게 한다..
	
	// 타일셋 그리기
	CRect rcSrc;
	POINT* pPtTmp, t_p;
	for(int y = 0; y < pTSData->nHeight; y++)
	{
		for(int x = 0; x < pTSData->nWidth; x++)
		{
			t_p.x = x; t_p.y = y;
			pPtTmp = pTSData->GetPos(t_p, m_nAniCur);
			if(pPtTmp == NULL) continue;

			if(pPtTmp->x < 0 || pPtTmp->y < 0 ) // 사용하지 않는 부분...
			{
				rcSrc.SetRect(x*TILE_SIZE, y*TILE_SIZE, x*TILE_SIZE + TILE_SIZE, y*TILE_SIZE + TILE_SIZE);
				g_pTEng->SurfaceFillRect(NULL, &rcSrc, 0); // 시커멓게 한다..
			}
			else
			{
				SetRect(&rcSrc, pPtTmp->x * TILE_SIZE, pPtTmp->y * TILE_SIZE, pPtTmp->x * TILE_SIZE + TILE_SIZE, pPtTmp->y * TILE_SIZE + TILE_SIZE);
				if(pTSData->pnFlags[m_nAniCur][y*pTSData->nWidth + x] & 2)
				{
					g_pTEng->BltNormal(g_pTSet->m_lpDDSs[m_nTSCur], x*TILE_SIZE, y*TILE_SIZE, rcSrc, TRUE); // 투명색은 투명하게 찍는다.
					g_pTEng->TextOut(NULL, x*TILE_SIZE, y*TILE_SIZE, "투명", RGB(255,255,0));
				}
				else
				{
					g_pTEng->BltNormal(g_pTSet->m_lpDDSs[m_nTSCur], x*TILE_SIZE, y*TILE_SIZE, rcSrc, FALSE); // 그대로 찍는다.
				}
			}

			if(pTSData->pnFlags[m_nAniCur][y*pTSData->nWidth + x] & 1)
			{
				g_pTEng->TextOut(NULL, x*TILE_SIZE, y*TILE_SIZE+TILE_SIZE/2, "충돌", RGB(255,255,0));
			}
		}
	}
	
	// 그리드 그리기..
	if(m_DlgHouse.m_vHouse && m_DlgHouse.m_vHouseOption)
	{
		int x, y;
		RECT client_rect;
		char t_info;

		GetClientRect(&client_rect);

		for(y = 24; y <= client_rect.bottom+48; y += 48)
		{
			for(x = 48; x <= client_rect.right+96; x += 96)
			{
				g_pTEng->DrawRhombus64(NULL, x, y, 0x00ff7fff);			// 그리드 그리기
			}
		}
/*		for(y = 16; y <= client_rect.bottom+32; y += 32)
		{
			for(x = 32; x <= client_rect.right+64; x += 64)
			{
				g_pTEng->DrawRhombus(NULL, x, y, 0x00ff7fff);			// 그리드 그리기
			}
		}*/

		CHouseInfo *pHouseInfo;

		if(g_pHouseInfo->m_MapHouseInfo.Lookup(m_nTSCur, pHouseInfo))
		{
			if(m_DlgHouse.m_vEditMode >= 0 && m_DlgHouse.m_vEditMode <= 2)
			{
				for(y = 0; y <= pHouseInfo->m_vSize.cy; y++)
				{
					for(x = 0; x <= pHouseInfo->m_vSize.cx; x++)
					{
						if((y+x)%2==0)
						{
							t_info = pHouseInfo->GetMoveInfo(x, y);
							if(t_info == 1)
							{
								CPoint pos(x-1,y-1);
								pos = CellToPos(pos);
								g_pTEng->TextOut(NULL, pos.x, pos.y-7, "M", RGB(255,255,255), TA_CENTER);
							}
							else if(t_info == 0)
							{
								CPoint pos(x-1,y-1);
								pos = CellToPos(pos);
								g_pTEng->TextOut(NULL, pos.x, pos.y-7, "N", RGB(192,192,192), TA_CENTER);
							}
						}
					}
				}
			}
			else
			{
				for(y = 0; y <= pHouseInfo->m_vSize.cy; y++)
				{
					for(x = 0; x <= pHouseInfo->m_vSize.cx; x++)
					{
						if((y+x)%2==0)
						{
							t_info = pHouseInfo->GetInnerInfo(x, y);
							if(t_info == 1)
							{
								CPoint pos(x-1,y-1);
								pos = CellToPos(pos);
								g_pTEng->TextOut(NULL, pos.x, pos.y-7, "I", RGB(255,255,255), TA_CENTER);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		int x1 = rcClient.left/TILE_SIZE;
		int y1 = rcClient.top/TILE_SIZE;
		int x2 = rcClient.right/TILE_SIZE;
		int y2 = rcClient.bottom/TILE_SIZE;

		for(int x = x1; x <= x2; x++)
		{
			g_pTEng->DrawLine(NULL, x*TILE_SIZE, rcClient.top, x*TILE_SIZE, rcClient.bottom, 0x0000ff00);
		}
		for(y = y1; y <= y2; y++)
		{
			g_pTEng->DrawLine(NULL, rcClient.left, y*TILE_SIZE, rcClient.right, y*TILE_SIZE, 0x0000ff00);
		}
	}

	if(m_MSM.IsUsing())
	{
		g_pTEng->DrawPolygon(NULL, m_MSM.m_vCurPos, 5, 0x00ffffff);
	}

	g_pTEng->HWndSet(this->m_hWnd);
	g_pTEng->Flip(&rcClient);
}

void CWndTileSetPreview::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
}

void CWndTileSetPreview::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int new_ani;
	_TILE_SUB_SET_DATA_TOOL* pTSData = g_pTSet->DataGetSafe(m_nTSCur, m_nTSSCur);
	
	new_ani = pTSData->Increase(m_nAniCur);
	if(new_ani == m_nAniCur) return;

	m_nAniCur = new_ani;
	Invalidate(FALSE);
	
	CWnd::OnRButtonDown(nFlags, point);
}

int CWndTileSetPreview::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect t_rect1, t_rect2;

	if(m_DlgHouse.GetSafeHwnd() == NULL)
	{
		m_DlgHouse.Create(IDD_HOUSE_OPTION, this);
	}
	m_DlgHouse.ShowWindow(SW_SHOW);
	GetWindowRect(&t_rect1);
	m_DlgHouse.GetWindowRect(&t_rect2);
	if(t_rect1.left >= t_rect2.Width())
	{
		m_DlgHouse.SetWindowPos(NULL, t_rect1.left-t_rect2.Width(), t_rect1.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	else
	{
		t_rect1.left = t_rect2.Width();
		SetWindowPos(NULL, t_rect2.Width(), t_rect1.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_DlgHouse.SetWindowPos(NULL, 0, t_rect1.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}
	
	return 0;
}

void CWndTileSetPreview::OnDestroy() 
{
	CWnd::OnDestroy();
	
	m_DlgHouse.DestroyWindow();
}

void CWndTileSetPreview::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_MSM.IsUsing() && m_DlgHouse.m_vHouse && m_DlgHouse.m_vHouseOption && m_nTSSCur == 3)
	{
		m_MSM.SetEndPos(point);
		ReleaseCapture();
		POINT *p = m_MSM.GetSelectRgn();
		CHouseInfo *pHouseInfo;

		if(g_pHouseInfo->m_MapHouseInfo.Lookup(m_nTSCur, pHouseInfo) == FALSE) return;
		switch(m_DlgHouse.m_vEditMode)
		{
		case 0:
			pHouseInfo->SetMoveInfo(p, -1);
			break;
		case 1:
			pHouseInfo->SetMoveInfo(p, 1);
			break;
		case 2:
			pHouseInfo->SetMoveInfo(p, 0);
			break;
		case 3:
			pHouseInfo->SetInnerInfo(p, -1);
			break;
		case 4:
			pHouseInfo->SetInnerInfo(p, 1);
			break;
		}
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CWndTileSetPreview::OnMouseMove(UINT nFlags, CPoint point) 
{
	if((m_DlgHouse.m_vHouse && m_DlgHouse.m_vHouseOption && m_nTSSCur == 3) && (nFlags & MK_LBUTTON))
	{
		m_MSM.MouseMoving(point);
	}
	
	CWnd::OnMouseMove(nFlags, point);
}
