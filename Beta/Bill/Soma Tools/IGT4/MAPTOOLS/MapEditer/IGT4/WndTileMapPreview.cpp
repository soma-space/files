// WndTileMiniMap.cpp : implementation file
//

#include "stdafx.h"
#include "WndTileMapPreview.h"

#include "GlobalVar.h"

/////////////////////////////////////////////////////////////////////////////
// CWndTileMapPreview

CWndTileMapPreview::CWndTileMapPreview()
{
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		m_lpDDSs[i] = NULL;
	}
}

CWndTileMapPreview::~CWndTileMapPreview()
{
}

BEGIN_MESSAGE_MAP(CWndTileMapPreview, CWnd)
	//{{AFX_MSG_MAP(CWndTileMapPreview)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndTileMapPreview message handlers

void CWndTileMapPreview::Alloc(int width, int height)
{
	if(width <= 0 || width > MAX_TILE_MAP_WIDTH || height <= 0 || height > MAX_TILE_MAP_HEIGHT) return;

	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		RELEASE_SAFE(m_lpDDSs[i]);
		m_lpDDSs[i] = g_pTEng->SurfaceCreate(width/2, height/2, FALSE, RGB(0,0,0)); // 절반만 할당한다.
	}
}

void CWndTileMapPreview::UpdateData(RECT* pRC, BOOL bDelete)
{
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++) if(m_lpDDSs[i] == NULL) return;

	CRect rcUpdate;
	if(pRC == NULL)
	{
		rcUpdate.SetRect(0,0,g_pTMap->m_HTMap.nWidth, g_pTMap->m_HTMap.nHeight);
		pRC = &rcUpdate;
	}
	
	CRect rc;
	SetRect(&rc, pRC->left/2, pRC->top/2, pRC->right/2, pRC->bottom/2);

	CRect rcDDS; // 서피스의 범위와 맞는지 확인
	g_pTEng->SurfaceGetSize(m_lpDDSs[0], &rcDDS);
	
	CRect rcWork; // 실제 업데이트 할 범위
	rcWork.IntersectRect(&rc, &rcDDS);
	
	LPDIRECTDRAWSURFACE lpDDS;
	DDSURFACEDESC ddsdSrc, ddsdDest;
	WORD* pwSrc, *pwDest;
	WORD wPixel;
	int nTS, pitchSrc, pitchDest, x2, y2;
	_TILE_MAP_DATA_TOOL* pMData;
	for(int n = 0; n < MAX_TILE_MAP_LAYER; n++)
	{
		if(g_pTEng->SurfaceLock(m_lpDDSs[n], &ddsdDest) != DD_OK) continue;

		pwDest = (WORD*)ddsdDest.lpSurface;
		pitchDest = ddsdDest.lPitch/2;

		for(int y = rcWork.top; y < rcWork.bottom; y++)
		{
			for(int x = rcWork.left; x < rcWork.right; x++)
			{
				pMData = g_pTMap->ToolDataGet(n, x*2, y*2);
				if(pMData == NULL) continue;

				nTS = pMData->snTS;
				if(	nTS < 0 || nTS >= MAX_TILE_SET ||
					pMData->snNum < 0)
				{
					SET_ARRAY_VALUE(pwDest, pitchDest, x, y, 0);
					continue;
				}
				
				lpDDS = g_pTSet->m_lpDDSs[nTS];
				if(lpDDS == NULL)
				{
					SET_ARRAY_VALUE(pwDest, pitchDest, x, y, 0);
					continue;
				}
				else
				{
					POINT *t_pos;
					g_pTEng->SurfaceLock(lpDDS, &ddsdSrc); // 타일에서 가운데 한점만 빼온다..
					pwSrc = (WORD*)ddsdSrc.lpSurface;
					pitchSrc = ddsdSrc.lPitch/2;
					t_pos = g_pTSet->DataGetPos(pMData->snTS, pMData->snTSS, pMData->snNum);
					if(t_pos && t_pos->x != -1 && t_pos->y != -1)
					{
						x2 = t_pos->x*TILE_SIZE + TILE_SIZE/2;
						y2 = t_pos->y*TILE_SIZE + TILE_SIZE/2;
						wPixel = GET_ARRAY_VALUE(pwSrc, pitchSrc, x2, y2);
						g_pTEng->SurfaceUnlock(lpDDS);
						
						if(wPixel != ddsdSrc.ddckCKSrcBlt.dwColorSpaceLowValue)
							SET_ARRAY_VALUE(pwDest, pitchDest, x, y, wPixel); // 픽셀 데이터 세팅
						else
							SET_ARRAY_VALUE(pwDest, pitchDest, x, y, 0); // 픽셀 데이터 세팅
					}
					else
					{
						g_pTEng->SurfaceUnlock(lpDDS);
						SET_ARRAY_VALUE(pwDest, pitchDest, x, y, 0); // 픽셀 데이터 세팅
					}
				}
			}
			g_pTEng->SurfaceUnlock(m_lpDDSs[n]);
		}
	}
}

void CWndTileMapPreview::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	g_pTEng->SurfaceFillRect(NULL, &dc.m_ps.rcPaint, 0);

	if(g_pTMap->m_HTMap.nWidth <= 0 || g_pTMap->m_HTMap.nHeight <= 0)
	{
		g_pTEng->HWndSet(this->m_hWnd);
		g_pTEng->Flip(&dc.m_ps.rcPaint);
		return;
	}
	
	CRect rcSrc;
	g_pTEng->SurfaceGetSize(m_lpDDSs[0], &rcSrc);
	for(int n = 0; n < MAX_TILE_MAP_LAYER; n++)
	{
		g_pTEng->BltNormal(m_lpDDSs[n], 0, 0, rcSrc, TRUE);
	}

	// 현재 영역 박스 그리기..
	CRect rcClient;
	g_WndTMap.GetClientRect(&rcClient);
	CPoint ptScroll(g_WndTMap.GetScrollPos(SB_HORZ), g_WndTMap.GetScrollPos(SB_VERT));

	CRect rcBox;
	rcBox.left = ptScroll.x/2;
	rcBox.top = ptScroll.y/2;
	rcBox.right = rcBox.left + rcClient.Width()/TILE_SIZE/2;
	rcBox.bottom = rcBox.top + rcClient.Height()/TILE_SIZE/2;

	g_pTEng->DrawBox(NULL, &rcBox, RGB(0,255,0));

	g_pTEng->HWndSet(this->m_hWnd);
	g_pTEng->Flip(&dc.m_ps.rcPaint);
	// Do not call CWnd::OnPaint() for painting messages
}

void CWndTileMapPreview::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	// 현재 영역 박스 그리기..
	CRect rcBox;
	g_WndTMap.GetClientRect(&rcBox);
	rcBox.right/=TILE_SIZE*2;
	rcBox.bottom/=TILE_SIZE*2;

	g_WndTMap.SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, point.x*2 - rcBox.Width()), NULL); // 스크롤 바 움직이기..
	g_WndTMap.SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, point.y*2 - rcBox.Height()), NULL); // 스크롤 바 움직이기..
	
	m_ptLBDown = point;

	CWnd::OnLButtonDown(nFlags, point);
}

void CWndTileMapPreview::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(nFlags & MK_LBUTTON)
	{
		CRect rcBox;
		g_WndTMap.GetClientRect(&rcBox);

		g_WndTMap.SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, point.x*2 - rcBox.Width()/TILE_SIZE/2), NULL); // 스크롤 바 움직이기..
		g_WndTMap.SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, point.y*2 - rcBox.Height()/TILE_SIZE/2), NULL); // 스크롤 바 움직이기..

		m_ptLBDown = point;
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CWndTileMapPreview::OnDestroy() 
{
	CWnd::OnDestroy();
	
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++) RELEASE_SAFE(m_lpDDSs[i]); // 여기서 해제해야 에러가 안 난다.. 소멸자 에서 하면 엔진이 널이기 땜시. 그렇다!!
}
