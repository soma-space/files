// WndAniTilePreview.cpp : implementation file
//

#include "stdafx.h"
#include "igt4.h"
#include "WndAniTilePreview.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndAniTilePreview

CWndAniTilePreview::CWndAniTilePreview()
{
}

CWndAniTilePreview::~CWndAniTilePreview()
{
}


BEGIN_MESSAGE_MAP(CWndAniTilePreview, CWnd)
	//{{AFX_MSG_MAP(CWndAniTilePreview)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndAniTilePreview message handlers

void CWndAniTilePreview::OnPaint() 
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
	int ani_sub_set_index;
	if( pTSData->GetAniSubSetIndex( &ani_sub_set_index ) )
	{
		pTSData = g_pTSet->DataGetSafe(m_nTSCur, ani_sub_set_index);
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

		}
	}
	g_pTEng->HWndSet(this->m_hWnd);
	g_pTEng->Flip(&rcClient);
	// Do not call CWnd::OnPaint() for painting messages
}

int CWndAniTilePreview::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_nTSCur = 0;
	m_nTSSCur = 0;
	m_nAniCur = 0;
	

	return 0;
}

void CWndAniTilePreview::OnDestroy() 
{
	CWnd::OnDestroy();
}

void CWndAniTilePreview::OnTimer(UINT nIDEvent) 
{
	UpdateAniTilePreviewWindow();
	CWnd::OnTimer(nIDEvent);
}

void CWndAniTilePreview::UpdateAniTilePreviewWindow()
{
	_TILE_SUB_SET_DATA_TOOL* pData = g_pTSet->DataGetSafe(m_nTSCur, m_nTSSCur);
	int width = 0, height = 0;
	if(pData)
	{
		pData->Increase();
		CRect rcAdjust(0, 0, pData->nWidth*TILE_SIZE, pData->nHeight*TILE_SIZE);
		::AdjustWindowRect(&rcAdjust, this->GetStyle(), FALSE);
		width = rcAdjust.Width();
		height = rcAdjust.Height();
	}
	this->SetWindowPos(NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
	this->InvalidateRect(NULL, FALSE);

}

void CWndAniTilePreview::TileSetCur(int nTS, int nTSS)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;

	m_nTSCur = nTS;
	m_nTSSCur = nTSS;
	m_nAniCur = 0;
}

void CWndAniTilePreview::ActiveAniTile( BOOL bActive )
{
	if( bActive )
	{
		this->SetTimer(0, 100, NULL);
	}
	else
	{
		this->KillTimer(0);
	}
}

void CWndAniTilePreview::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	if( nState == WA_INACTIVE ) this->KillTimer(0);	
}
