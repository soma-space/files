// BuildHouseManager.cpp: implementation of the CBuildHouseManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "BuildHouseManager.h"
#include "TileSetTool.h"
#include "ChildView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuildHouseManager::CBuildHouseManager()
{

}

CBuildHouseManager::~CBuildHouseManager()
{

}

CSize CBuildHouseManager::TileMapSet(int x, int y, int nLayer, int nTS, int nTSS)
{
	_TILE_SUB_SET_DATA_TOOL* pTSData;
	_TILE_MAP_DATA_TOOL* pMData;
	CRect rcDest;
	int sx, sy, dx, dy;
	int t_int;

	pTSData = CChildView::TileSetTool.DataGetSafe(nTS, nTSS);

	rcDest.left = x;
	rcDest.top = y;
	rcDest.right = rcDest.left + pTSData->nWidth;
	rcDest.bottom = rcDest.top + pTSData->nHeight;

	for(sy = 0, dy = rcDest.top; dy < rcDest.bottom; dy++, sy++)
	{
		for(sx = 0, dx = rcDest.left; dx < rcDest.right; dx++, sx++)
		{
			pMData = CChildView::TileMapTool.ToolDataGet(nLayer, dx, dy);
			if(pMData == NULL) continue;

			t_int = pTSData->GetSafe(sx, sy);
			if(t_int == -1) continue;
			
			pMData->snTS = nTS;
			pMData->snTSS = nTSS;
			pMData->snNum = t_int;
			pMData->snFlags = pTSData->pnFlags[0][sy*pTSData->nWidth+sx];
		}
	}

	return CSize(pTSData->nWidth, pTSData->nHeight);
}

void CBuildHouseManager::BuildHouse(int x, int y, int nTS)
{
	int t_height;
	DDSURFACEDESC2 ddsd;
	POINT offset, inner_start;
	CRect t_rect1, t_rect2;
	_TILE_SET_TOOL *tile_set_tool;

	tile_set_tool = CChildView::TileSetTool.GetTileSet(nTS);
	if(tile_set_tool == NULL) return;
	if(tile_set_tool->m_lpDDSs == NULL)
	{
		if(lstrlen(tile_set_tool->m_HTSs.szBMPFN) > 0)
		{
			CChildView::g_pTEng.PathSetCur("BMP");
			CString szBMPFN = tile_set_tool->m_HTSs.szBMPFN;
			if(szBMPFN.GetLength() > 0)
			{
				szBMPFN = szBMPFN.Left(szBMPFN.GetLength() - 3);
				tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK,TRUE);
				if(tile_set_tool->m_lpDDSs == NULL)
				{
					tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "bmp", FALSE, tile_set_tool->m_HTSs.crCK);
					TRACE("%s 佬澜\n", tile_set_tool->m_HTSs.szBMPFN);
				}
			}
			else return;
		}
	}
	
	if(tile_set_tool->m_lpDDSs == NULL) return;

	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_HEIGHT;
	tile_set_tool->m_lpDDSs->GetSurfaceDesc(&ddsd);
	t_height = ddsd.dwHeight / 32 / 3;
	TileMapSet(x, y, 4, nTS, 2);	// 拉何盒

	t_rect1 = GetTileSetRect(nTS, 2);
	t_rect2 = GetTileSetRect(nTS, 0);
	offset.x = t_rect2.left - t_rect1.left;
	offset.y = t_rect2.top - t_rect1.top;
	TileMapSet(x+offset.x, y+offset.y, 0, nTS, 0);				// 关何盒
	t_rect2 = GetTileSetRect(nTS, 1);
	offset.x = t_rect2.left - t_rect1.left;
	offset.y = t_rect2.top - t_rect1.top;
	TileMapSet(x+offset.x, y+offset.y, 1, nTS, 1);				// 关捧疙何盒
	t_rect2 = GetTileSetRect(nTS, 3);
	offset.x = t_rect2.left - t_rect1.left;
	offset.y = t_rect2.top - t_rect1.top - t_height;
	inner_start.x = x+offset.x;
	inner_start.y = y+offset.y;
	TileMapSet(inner_start.x, inner_start.y, 5, nTS, 3);		// 郴何
	t_rect2 = GetTileSetRect(nTS, 4);
	offset.x = t_rect2.left - t_rect1.left;
	offset.y = t_rect2.top - t_rect1.top - t_height*2;
	TileMapSet(x+offset.x, y+offset.y, 3, nTS, 4);				// 弊覆磊
	offset.x = inner_start.x*32;
	offset.y = inner_start.y*32;
	offset = CChildView::MapManager.PixelToCell(offset);
	HouseInfoSet(offset.x, offset.y, nTS);
}

void CBuildHouseManager::LoadHouseInfo(CString file_name)
{
	m_HouseInfoManager.Load(file_name);
}

void CBuildHouseManager::HouseInfoSet(int x, int y, int nTS)
{
	int i, j;
	int map_width, map_height;
	short **house_info;
	CHouseInfo *pHouseInfo;

	if(m_HouseInfoManager.m_MapHouseInfo.Lookup(nTS, pHouseInfo) == FALSE) return;

	house_info = CChildView::MapManager.GetHouseTable(map_width, map_height);

	for(i = 0; i < pHouseInfo->m_vSize.cx; i++)
	{
		for(j = 0; j < pHouseInfo->m_vSize.cy; j++)
		{
			if((i+j)%2==0 && pHouseInfo->m_ppData[i][j].m_vInner == 1)
			{
				house_info[x+i][y+j] = 100;
			}
		}
	}
}

void CBuildHouseManager::BuildHouse(CRect hRect, int nTS)
{
	int x, y;
	CSize house_size;

	house_size = GetHouseSize(nTS);
	if(house_size.cx == 0) return;

	x = hRect.left + (hRect.Width() - house_size.cx) / 2;
	y = hRect.top + hRect.Height() - house_size.cy;
	BuildHouse(x, y, nTS);
}

CSize CBuildHouseManager::GetHouseSize(int nTS)
{
	_TILE_SUB_SET_DATA_TOOL *pTSData1, *pTSData2;
	CSize house_size;

	pTSData1 = CChildView::TileSetTool.DataGetSafe(nTS, 1);	// 关何盒
	pTSData2 = CChildView::TileSetTool.DataGetSafe(nTS, 2); // 拉何盒
	if(pTSData1 == NULL) return CSize(0,0);

	house_size.cx = (pTSData1->nWidth > pTSData2->nWidth) ? pTSData1->nWidth : pTSData2->nWidth;
	house_size.cy = pTSData1->nHeight + pTSData2->nHeight;

	return house_size;
}

CPoint CBuildHouseManager::GetInnerTopLeft(int nTS, CRect place_rect)
{
	CSize inner_size, house_size;
	int t_height;
	DDSURFACEDESC2 ddsd;
	POINT offset, inner_start;
	CRect t_rect1, t_rect2;
	int x, y;

	_TILE_SET_TOOL *tile_set_tool = CChildView::TileSetTool.GetTileSet(nTS);
	if(tile_set_tool == NULL) return CPoint(-1,-1);
	if(tile_set_tool->m_lpDDSs == NULL)
	{
		if(lstrlen(tile_set_tool->m_HTSs.szBMPFN) > 0)
		{
			CChildView::g_pTEng.PathSetCur("BMP");
			CString szBMPFN = tile_set_tool->m_HTSs.szBMPFN;
			if(szBMPFN.GetLength() > 0)
			{
				szBMPFN = szBMPFN.Left(szBMPFN.GetLength() - 3);
				tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK,TRUE);
				if(tile_set_tool->m_lpDDSs == NULL)
				{
					tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "bmp", FALSE, tile_set_tool->m_HTSs.crCK);
					TRACE("%s 佬澜\n", tile_set_tool->m_HTSs.szBMPFN);
				}
			}
			else return CPoint(-1,-1);
		}
	}
	
	if(tile_set_tool->m_lpDDSs == NULL) return CPoint(-1,-1);

	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_HEIGHT;
	tile_set_tool->m_lpDDSs->GetSurfaceDesc(&ddsd);
	t_height = ddsd.dwHeight / 32 / 3;

	t_rect1 = GetTileSetRect(nTS, 2);
	t_rect2 = GetTileSetRect(nTS, 3);
	house_size = GetHouseSize(nTS);

	offset.x = t_rect2.left - t_rect1.left;
	offset.y = t_rect2.top - t_rect1.top - t_height;
	x = place_rect.left + (place_rect.Width() - house_size.cx) / 2;
	y = place_rect.top + place_rect.Height() - house_size.cy;
	inner_start.x = x+offset.x;
	inner_start.y = y+offset.y;

	return inner_start;
}

CRect CBuildHouseManager::GetTileSetRect(int nTS, int nTSS)
{
	CRect t_rect;
	POINT *p_pos;
	_TILE_SUB_SET_DATA_TOOL *pTSData;

	pTSData = CChildView::TileSetTool.DataGetSafe(nTS, nTSS);
	p_pos = CChildView::TileSetTool.DataGetPosCur(nTS, nTSS, 0);
	if(p_pos->x == -1)
	{
		for(int i = 1;;i++)
		{
			p_pos = CChildView::TileSetTool.DataGetPosCur(nTS, nTSS, i);
			if(p_pos->x != -1) break;
		}
		t_rect.left = p_pos->x - i % pTSData->nWidth;
		t_rect.top = p_pos->y - i / pTSData->nWidth;
	}
	else
	{
		t_rect.left = p_pos->x;
		t_rect.top = p_pos->y;
	}

	t_rect.right = t_rect.left + pTSData->nWidth;
	t_rect.bottom = t_rect.top + pTSData->nHeight;

	return t_rect;
}