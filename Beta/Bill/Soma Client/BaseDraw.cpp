// BaseDraw.cpp: implementation of the CBaseDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "BaseDraw.h"
#include "DManage/BB_Direct.h"
#include "DirectXMng.h"
#include "ChildView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const BOOL bUCK[] = {FALSE, TRUE, TRUE, FALSE, TRUE, TRUE};
static const int nRenderType[] = {RENDER_NORMAL, RENDER_NORMAL, RENDER_NORMAL, RENDER_SHADOW, RENDER_NORMAL, RENDER_NORMAL};

#define SIZE_X		26
#define SIZE_Y		20

#define	WIDTH		800
#define HEIGHT		600

extern DWORD					g_vRenderTime;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseDraw::CBaseDraw()
{
	m_pBaseSurface = NULL;
	m_vCurPos.x = WIDTH;
	m_vCurPos.y = HEIGHT;
	m_vMapPos.x = m_vMapPos.y = -1000;

	m_rcBaseCenter.SetRect(WIDTH,HEIGHT,WIDTH+TILE_SIZE*SIZE_X,HEIGHT+TILE_SIZE*SIZE_Y);
}

CBaseDraw::~CBaseDraw()
{
	if(m_pBaseSurface)
	{
		delete m_pBaseSurface;
		m_pBaseSurface = NULL;
	}
}

void CBaseDraw::InitSurface()
{
	if(CChildView::DirectXMng.m_pDD == NULL) return;

	m_vWidth = WIDTH * 3;
	m_vHeight = HEIGHT * 3;

	DDSURFACEDESC2 sd;
	memset(&sd, 0, sizeof(sd));
	sd.dwSize = sizeof(sd);

	sd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_SYSTEMMEMORY;
	sd.dwWidth = m_vWidth;
	sd.dwHeight = m_vHeight;

	m_pBaseSurface = new CDDSurface();
	if(!m_pBaseSurface->Create(CChildView::DirectXMng.m_pDD, &sd))
	{
		delete m_pBaseSurface;
		m_pBaseSurface = NULL;
		return;
	}

	m_pBaseSurface->SetColorKey(CChildView::DirectXMng.m_colorTrans);	//TransParent로 처리할 색 설정
}

void CBaseDraw::DrawBaseTile(CPoint cell_offset)
{
	m_vCurPos.x = WIDTH;
	m_vCurPos.y = HEIGHT;
	m_vOrgPos = m_vMapPos = cell_offset;

	for(int nLayer = 0; nLayer < 3; nLayer++)
	{
		int x, y;

		IDirectDrawSurface7 *destSurface = m_pBaseSurface->GetInterface();
		for(y = cell_offset.y; y < cell_offset.y + SIZE_Y; y++) // 바닥에 깔리는 맵 //전에 있던 원래 소스
		{
			for(x = cell_offset.x; x < cell_offset.x + SIZE_X; x++)
			{
				const _TILE_MAP_DATA_TOOL* pTMData = CChildView::TileMapTool.ToolDataGet(nLayer, x, y);
				if(pTMData == NULL) continue;
				_TILE_SET_TOOL *tile_set_tool;

				tile_set_tool = CChildView::TileSetTool.GetTileSet(pTMData->snTS);
				if(tile_set_tool == NULL) continue;
				if(tile_set_tool->m_lpDDSs == NULL)
				{// 읽지 않은 서페이스이므로 읽어 들인다. 
#ifdef _DEBUG
					DWORD STLOAD = GetTickCount();
#endif
					CString szBMPFN = tile_set_tool->m_HTSs.szBMPFN;
					CString szHFN = "house"; 
					if(szBMPFN.GetLength() > 0)
					{
						szBMPFN = szBMPFN.Left(szBMPFN.GetLength() - 3);
						CChildView::g_pTEng.PathSetCur("BMP");
						
						//  9.15 먼저 obm을 읽어보고 안되면 bmp 를 읽는다.
//						tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK);
						if( szBMPFN.Left(5) == szHFN )
						tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK, TRUE);
						else
						tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK);
						if(tile_set_tool->m_lpDDSs == NULL)
						{
							tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "bmp", FALSE, tile_set_tool->m_HTSs.crCK);
							if(tile_set_tool->m_lpDDSs)
							{
								TRACE("%s 읽음\n", szBMPFN + "bmp");
							}
							else continue;
						}
						else
						{
							TRACE("DrawBaseTile:%s 읽음(%ld)\n", szBMPFN + "obm", g_vRenderTime);
						}
					}
#ifdef _DEBUG
					DWORD ENDLOAD = GetTickCount();
					TRACE("맵로딩시간 %d\n",ENDLOAD-STLOAD);
#endif
					////////////////////////////
				}
				tile_set_tool->m_vUseTime = g_vRenderTime;

				POINT *t_pos = CChildView::TileSetTool.DataGetPos(pTMData->snTS, pTMData->snTSS, pTMData->snNum);

				if(t_pos == NULL ||
					t_pos->x < 0 ||
					t_pos->x >= MAX_TILE_SET_WIDTH ||
					t_pos->y < 0 ||
					t_pos->y >= MAX_TILE_SET_HEIGHT) continue; // 쓰지 않는 타일이면 지나간다..

				CRect rcSrc;
				rcSrc.left = t_pos->x << 5; // *32
				rcSrc.top = t_pos->y << 5; // *32
				rcSrc.right = rcSrc.left + TILE_SIZE;
				rcSrc.bottom = rcSrc.top + TILE_SIZE;
				
				CChildView::g_pTEng.BltNormal(tile_set_tool->m_lpDDSs,
					((x-cell_offset.x)<<5)+m_vCurPos.x,
					((y-cell_offset.y)<<5)+m_vCurPos.y,
					rcSrc, bUCK[nLayer], nRenderType[nLayer], 16, 0, destSurface);
			}
		}
	}
}
#include "user.h"
void CBaseDraw::DrawShadow(CPoint cell_offset)
{
	int Cell_X = cell_offset.x;
	int Cell_Y = cell_offset.y;
	CPoint ptScroll = CPoint(Cell_X+1, Cell_Y+1);
	int start_x, end_x, start_y, end_y;
	SIZE half_size;

	short int nData, nSet = 0, nSubSet = 0;
	const _SPR_LSP* pSpr = NULL;

	half_size.cx = (int)(SIZE_X / 2.0 + 0.5);
	half_size.cy = (int)(SIZE_Y / 2.0 + 0.5);

	if(Cell_X-half_size.cx < 0) start_x = 0;
	else start_x = Cell_X - half_size.cx;
	if(Cell_X+SIZE_X+half_size.cx >= CChildView::MapObj.m_Header.nWidth) end_x = CChildView::MapObj.m_Header.nWidth;
	else end_x = Cell_X + SIZE_X + half_size.cx;

	if(Cell_Y-half_size.cy < 0) start_y = 0;
	else start_y = Cell_Y - half_size.cy;
	if(Cell_Y+SIZE_Y+half_size.cy >= CChildView::MapObj.m_Header.nWidth) end_y = CChildView::MapObj.m_Header.nHeight;
	else end_y = Cell_Y + SIZE_Y + half_size.cy;
	
	for(int y = start_y; y < end_y; y++) // 바닥에 깔리는 맵
	{
		for(int x = start_x; x < end_x; x++)
		{
			nData = CChildView::MapShadowObj.DataGet(x, y);
			nSet = nData >> 8; // 세트 번호는 상위 바이트 이다.
			if(nSet < 0 || nSet > MAX_MAP_OBJ_REG) continue; // 배열 범위 체크
			nSubSet = nData & 0x00ff; // 서브 세트 번호는 하위 바이트

			//고정..
			int nSpr = CChildView::MapShadowAniObjs[nSet].FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
			CChildView::MapShadowAniObjs[nSet].SprGet(nSpr, &pSpr);
			if(pSpr == NULL) // 9.28 맵 오브젝트 스프라이트가 없으면 읽기를 시도한다.
			{
				if(CChildView::MapShadowObj.m_Header.szRegObjSprFNs[nSet][0] == 0) continue;

				CString szFN = "map\\object\\";
				szFN += CChildView::MapShadowObj.m_Header.szRegObjAniFNs[nSet]; // 에니메이션 파일 읽기
				CChildView::MapShadowAniObjs[nSet].AniLoadEx(szFN,TRUE);
				CChildView::MapShadowAniObjs[nSet].AniCtrlAllocEx(256,TRUE);
				szFN = "map\\object\\";
				szFN += CChildView::MapShadowObj.m_Header.szRegObjSprFNs[nSet];
				nSpr = CChildView::MapShadowAniObjs[nSet].FrmGetAnimated(nSubSet);
				//nSpr = MapAniObjs[nSet].FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
				CChildView::MapShadowAniObjs[nSet].SprLoadEx(szFN, CChildView::g_pTEng.m_nPF, nSpr); // 스프라이트 파일 읽기.
				CChildView::MapShadowAniObjs[nSet].SprGet(nSpr, &pSpr);
				if(pSpr == NULL) continue;

/*
				CString szFN = "map\\object\\";
				szFN += CChildView::MapShadowObj.m_Header.szRegObjAniFNs[nSet]; // 에니메이션 파일 읽기
				CChildView::MapShadowAniObjs[nSet].AniLoad(szFN);
				CChildView::MapShadowAniObjs[nSet].AniCtrlAlloc(256);
				szFN = "map\\object\\";
				szFN += CChildView::MapShadowObj.m_Header.szRegObjSprFNs[nSet];

				CChildView::MapShadowAniObjs[nSet].SprLoad(szFN, CChildView::g_pTEng.m_nPF); // 스프라이트 파일 읽기.
				//MapShadowAniObjs[nSet].SprLoad(szFN, 0); // 스프라이트 파일 읽기.
				// 다시 한번 스프라이트 데이터를 가져오기 시도. 
				nSpr = CChildView::MapShadowAniObjs[nSet].FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
				CChildView::MapShadowAniObjs[nSet].SprGet(nSpr, &pSpr);
				if(pSpr == NULL) continue;
*/
			}
		
			RECT rcV = pSpr->rcV;
			PWORD wpV = pSpr->pwV;	
			BltLSPPartShadow(m_pBaseSurface, ((cell_offset.x-m_vOrgPos.x)<<5)+m_vCurPos.x+(x-ptScroll.x+1)*TILE_SIZE+16,((cell_offset.y-m_vOrgPos.y)<<5)+m_vCurPos.y+(y-ptScroll.y+1)*TILE_SIZE+16, &rcV, wpV, CChildView::DirectXMng.m_dwHalf16Mask, CChildView::DirectXMng.m_dwHalf32Mask );
//			BltLSPShadow(m_pBaseSurface, ((cell_offset.x-m_vOrgPos.x)<<5)+m_vCurPos.x+(x-ptScroll.x+1)*TILE_SIZE+16,((cell_offset.y-m_vOrgPos.y)<<5)+m_vCurPos.y+(y-ptScroll.y+1)*TILE_SIZE+16, &rcV, wpV, CChildView::DirectXMng.m_dwHalf16Mask, CChildView::DirectXMng.m_dwHalf32Mask );
		}
	}

}

void CBaseDraw::DrawPartShadow(CPoint cell_offset)
{
	int Cell_X = cell_offset.x;
	int Cell_Y = cell_offset.y;
	CPoint ptScroll = CPoint(Cell_X+1, Cell_Y+1);
	int start_x, end_x, start_y, end_y;

	short int nData, nSet = 0, nSubSet = 0;
	const _SPR_LSP* pSpr = NULL;

	if(Cell_X-13 < 0) start_x = 0;
	else start_x = Cell_X - 13;
	if(Cell_X+26+13 >= CChildView::MapObj.m_Header.nWidth) end_x = CChildView::MapObj.m_Header.nWidth;
	else end_x = Cell_X + 26 + 13;

	if(Cell_Y-9 < 0) start_y = 0;
	else start_y = Cell_Y - 9;
	if(Cell_Y+17+9 >= CChildView::MapObj.m_Header.nWidth) end_y = CChildView::MapObj.m_Header.nHeight;
	else end_y = Cell_Y + 17 + 9;
	
	for(int y = start_y; y < end_y; y++) // 바닥에 깔리는 맵
	{
		for(int x = start_x; x < end_x; x++)
		{
			nData = CChildView::MapShadowObj.DataGet(x, y);
			nSet = nData >> 8; // 세트 번호는 상위 바이트 이다.
			if(nSet < 0 || nSet > MAX_MAP_OBJ_REG) continue; // 배열 범위 체크
			nSubSet = nData & 0x00ff; // 서브 세트 번호는 하위 바이트

			//고정..
			int nSpr = CChildView::MapShadowAniObjs[nSet].FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
			CChildView::MapShadowAniObjs[nSet].SprGet(nSpr, &pSpr);
			if(pSpr == NULL) // 9.28 맵 오브젝트 스프라이트가 없으면 읽기를 시도한다.
			{
				if(CChildView::MapShadowObj.m_Header.szRegObjSprFNs[nSet][0] == 0) continue;

				CString szFN = "map\\object\\";
				szFN += CChildView::MapShadowObj.m_Header.szRegObjAniFNs[nSet]; // 에니메이션 파일 읽기
				CChildView::MapShadowAniObjs[nSet].AniLoadEx(szFN,TRUE);
				CChildView::MapShadowAniObjs[nSet].AniCtrlAllocEx(256,TRUE);
				szFN = "map\\object\\";
				szFN += CChildView::MapShadowObj.m_Header.szRegObjSprFNs[nSet];
				nSpr = CChildView::MapShadowAniObjs[nSet].FrmGetAnimated(nSubSet);
				//nSpr = MapAniObjs[nSet].FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
				CChildView::MapShadowAniObjs[nSet].SprLoadEx(szFN, CChildView::g_pTEng.m_nPF, nSpr); // 스프라이트 파일 읽기.
				CChildView::MapShadowAniObjs[nSet].SprGet(nSpr, &pSpr);
				if(pSpr == NULL) continue;

/*
				CString szFN = "map\\object\\";
				szFN += CChildView::MapShadowObj.m_Header.szRegObjAniFNs[nSet]; // 에니메이션 파일 읽기
				CChildView::MapShadowAniObjs[nSet].AniLoad(szFN);
				CChildView::MapShadowAniObjs[nSet].AniCtrlAlloc(256);
				szFN = "map\\object\\";
				szFN += CChildView::MapShadowObj.m_Header.szRegObjSprFNs[nSet];

				CChildView::MapShadowAniObjs[nSet].SprLoad(szFN, CChildView::g_pTEng.m_nPF); // 스프라이트 파일 읽기.
				nSpr = CChildView::MapShadowAniObjs[nSet].FrmGetByAniAndDirAndSeq(nSubSet, 0, 0);
				CChildView::MapShadowAniObjs[nSet].SprGet(nSpr, &pSpr);
				if(pSpr == NULL) continue;
*/
			}
		
			RECT rcV = pSpr->rcV;
			// 유저 함수를 이용하려고 임시로 이용한다.
			PWORD wpV = pSpr->pwV;	
			BltLSPPartShadow(m_pBaseSurface, ((cell_offset.x-m_vOrgPos.x)<<5)+m_vCurPos.x+(x-ptScroll.x+1)*TILE_SIZE+16,((cell_offset.y-m_vOrgPos.y)<<5)+m_vCurPos.y+(y-ptScroll.y+1)*TILE_SIZE+16, &rcV, wpV, CChildView::DirectXMng.m_dwHalf16Mask, CChildView::DirectXMng.m_dwHalf32Mask );

		}
	}

}

BOOL CBaseDraw::GetClippedRect(RECT *pRC, RECT* pRCClip)
{
	BOOL bUseXClip = FALSE;
	*pRCClip = *pRC;

	if(pRC->left < m_rcBaseCenter.left)
	{ 
		pRCClip->left = m_rcBaseCenter.left;
		bUseXClip = TRUE;
	}
	if(pRC->right > m_rcBaseCenter.right)
	{ 
		pRCClip->right = m_rcBaseCenter.right;
		bUseXClip = TRUE;
	}
	if(pRC->top < m_rcBaseCenter.top)
	{
		pRCClip->top = m_rcBaseCenter.top;
	}
	if(pRC->bottom > m_rcBaseCenter.bottom)
	{
		pRCClip->bottom = m_rcBaseCenter.bottom;
	}
	return bUseXClip;
}

void CBaseDraw::BltLSPShadow(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, DWORD dwHalf16Mask, DWORD dwHalf32Mask)
{

	if(data == NULL) return;
	if(pRC == NULL) return;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < m_rcBaseCenter.left || rc.bottom < m_rcBaseCenter.top || rc.left >= m_rcBaseCenter.right || rc.top >= m_rcBaseCenter.bottom ) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이

	WORD* pSrc = data; 
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < m_rcBaseCenter.top)
	{
		int skipY = -rc.top+m_rcBaseCenter.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;

	WORD* pDest = NULL;
	
	if( height >= 495 ) height = 400;
	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산

				CUser::PutPixelShadowEx(pDest, nCopyCount, dwHalf16Mask, dwHalf32Mask);
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = - rc.left + m_rcBaseCenter.left;
		int clipXR =  rc.right - m_rcBaseCenter.right;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;
			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ >= clipXL) { pDest += nZeroCount; }
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL) { nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS; }
						else { pDest += ndxS - clipXL; }
					}

				}
				if(clipXR > 0) // 오른쪽 클리핑
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}
				}
				
				CUser::PutPixelShadowEx(pDest, nCopyCount, dwHalf16Mask, dwHalf32Mask);
			}
		}
	}
	pSurface->Unlock();

}

void CBaseDraw::DrawChangeTile(CPoint cell_offset)
{
	RECT draw1, draw2;
	CSize gap;

	gap.cx = cell_offset.x - m_vMapPos.x;
	gap.cy = cell_offset.y - m_vMapPos.y;

	if(abs(gap.cx) > SIZE_X || abs(gap.cy) > SIZE_Y)
	{
		DrawBaseTile(cell_offset);
		m_rcBaseCenter.SetRect(WIDTH,HEIGHT,WIDTH+TILE_SIZE*SIZE_X,HEIGHT+TILE_SIZE*SIZE_Y);
		DrawShadow(cell_offset);
		return;
	}
	else
	{
		if(abs(cell_offset.x-m_vOrgPos.x) >= SIZE_X-2 || abs(cell_offset.y-m_vOrgPos.y) >= SIZE_Y-2)
		{
			DrawBaseTile(cell_offset);
			m_rcBaseCenter.SetRect(WIDTH,HEIGHT,WIDTH+TILE_SIZE*SIZE_X,HEIGHT+TILE_SIZE*SIZE_Y);
			DrawShadow(cell_offset);
			return;
		}
		if(gap.cx > 0)
		{
			draw1.left = m_vMapPos.x + SIZE_X;
			draw1.right = draw1.left + gap.cx;
			draw1.top = cell_offset.y;
			draw1.bottom = draw1.top + SIZE_Y;

			if(gap.cy > 0)
			{
				draw2.left = cell_offset.x;
				draw2.right = m_vMapPos.x + SIZE_X;
				draw2.top = m_vMapPos.y + SIZE_Y;
				draw2.bottom = draw2.top + gap.cy;
			}
			else if(gap.cy < 0)
			{
				draw2.left = cell_offset.x;
				draw2.right = m_vMapPos.x + SIZE_X;
				draw2.top = cell_offset.y;
				draw2.bottom = m_vMapPos.y;
			}
			else
			{
				draw2.left = draw2.right = draw2.top = draw2.bottom = 0;
			}
		}
		else if(gap.cx < 0)
		{
			draw1.left = cell_offset.x;
			draw1.right = m_vMapPos.x;
			draw1.top = cell_offset.y;
			draw1.bottom = draw1.top + SIZE_Y;

			if(gap.cy > 0)
			{
				draw2.left = m_vMapPos.x;
				draw2.right = cell_offset.x + SIZE_X;
				draw2.top = m_vMapPos.y + SIZE_Y;
				draw2.bottom = draw2.top + gap.cy;
			}
			else if(gap.cy < 0)
			{
				draw2.left = m_vMapPos.x;
				draw2.right = cell_offset.x + SIZE_X;
				draw2.top = cell_offset.y;
				draw2.bottom = m_vMapPos.y;
			}
			else
			{
				draw2.left = draw2.right = draw2.top = draw2.bottom = 0;
			}
		}
		else
		{
			draw1.left = draw1.right = draw1.top = draw1.bottom = 0;
			if(gap.cy > 0)
			{
				draw2.left = m_vMapPos.x;
				draw2.right = m_vMapPos.x + SIZE_X;
				draw2.top = m_vMapPos.y + SIZE_Y;
				draw2.bottom = draw2.top + gap.cy;
			}
			else if(gap.cy < 0)
			{
				draw2.left = m_vMapPos.x;
				draw2.right = m_vMapPos.x + SIZE_X;
				draw2.top = cell_offset.y;
				draw2.bottom = m_vMapPos.y;
			}
			else
			{
				return;
			}
		}
	}

	int nLayer, x, y;
	m_vMapPos = cell_offset;

	IDirectDrawSurface7 *destSurface = m_pBaseSurface->GetInterface();
	if((draw1.right-draw1.left) != 0 && (draw1.bottom-draw1.top) != 0)
	{
		m_rcBaseCenter.SetRect(((draw1.left-m_vOrgPos.x)<<5)+m_vCurPos.x,
							   ((draw1.top-m_vOrgPos.y)<<5)+m_vCurPos.y,
							   ((draw1.right-m_vOrgPos.x)<<5)+m_vCurPos.x,
							   ((draw1.bottom-m_vOrgPos.y)<<5)+m_vCurPos.y);
		if( m_rcBaseCenter.bottom  > HEIGHT * 3)
		{
			m_rcBaseCenter.bottom = HEIGHT * 3;
		}
		for(nLayer = 0; nLayer < 3; nLayer++)
		{
			for(y = draw1.top; y < draw1.bottom; y++) // 바닥에 깔리는 맵 //전에 있던 원래 소스
			{
				for(x = draw1.left; x < draw1.right; x++)
				{
					const _TILE_MAP_DATA_TOOL* pTMData = CChildView::TileMapTool.ToolDataGet(nLayer, x, y);
					if(pTMData == NULL) continue;
					_TILE_SET_TOOL *tile_set_tool;

					tile_set_tool = CChildView::TileSetTool.GetTileSet(pTMData->snTS);
					if(tile_set_tool == NULL) continue;
					if(tile_set_tool->m_lpDDSs == NULL)
					{// 읽지 않은 서페이스이므로 읽어 들인다. 
#ifdef _DEBUG
						DWORD STLOAD = GetTickCount();
#endif
						CString szBMPFN = tile_set_tool->m_HTSs.szBMPFN;
						CString szHFN = "house"; 
						if(szBMPFN.GetLength() > 0)
						{
							szBMPFN = szBMPFN.Left(szBMPFN.GetLength() - 3);
							CChildView::g_pTEng.PathSetCur("BMP");
							
							//  9.15 먼저 obm을 읽어보고 안되면 bmp 를 읽는다.
//							tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK);
							if( szBMPFN.Left(5) == szHFN )
							tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK,TRUE);
							else
							tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK);
							if(tile_set_tool->m_lpDDSs == NULL)
							{
								tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "bmp", FALSE, tile_set_tool->m_HTSs.crCK);
								if(tile_set_tool->m_lpDDSs)
								{
									TRACE("%s 읽음\n", szBMPFN + "bmp");
								}
								else continue;
							}
							else
							{
								TRACE("DrawChangeTile:%s 읽음(%ld)\n", szBMPFN + "obm", g_vRenderTime);
							}
						}
						////////////////////////////
#ifdef _DEBUG
						DWORD ENDLOAD = GetTickCount();
						TRACE("맵로딩시간 %d\n",ENDLOAD-STLOAD);
#endif

					}
					tile_set_tool->m_vUseTime = g_vRenderTime;

					POINT *t_pos = CChildView::TileSetTool.DataGetPos(pTMData->snTS, pTMData->snTSS, pTMData->snNum);

					if(t_pos == NULL ||
						t_pos->x < 0 ||
						t_pos->x >= MAX_TILE_SET_WIDTH ||
						t_pos->y < 0 ||
						t_pos->y >= MAX_TILE_SET_HEIGHT) continue; // 쓰지 않는 타일이면 지나간다..

					CRect rcSrc;
					rcSrc.left = t_pos->x << 5; // *32
					rcSrc.top = t_pos->y << 5; // *32
					rcSrc.right = rcSrc.left + TILE_SIZE;
					rcSrc.bottom = rcSrc.top + TILE_SIZE;
					
					CChildView::g_pTEng.BltNormal(tile_set_tool->m_lpDDSs, 
						((x-m_vOrgPos.x)<<5)+m_vCurPos.x,
						((y-m_vOrgPos.y)<<5)+m_vCurPos.y,
						rcSrc, bUCK[nLayer], nRenderType[nLayer], 16, 0, destSurface);
				}
			}
		}
		DrawPartShadow( CPoint(draw1.left,draw1.top) );
	}
	if((draw2.right-draw2.left) != 0 && (draw2.bottom-draw2.top) != 0)
	{
		m_rcBaseCenter.SetRect(((draw2.left-m_vOrgPos.x)<<5)+m_vCurPos.x,
							   ((draw2.top-m_vOrgPos.y)<<5)+m_vCurPos.y,
							   ((draw2.right-m_vOrgPos.x)<<5)+m_vCurPos.x,
							   ((draw2.bottom-m_vOrgPos.y)<<5)+m_vCurPos.y);
		if( m_rcBaseCenter.bottom  > HEIGHT * 3)
		{
			m_rcBaseCenter.bottom = HEIGHT * 3;
		}
		for(nLayer = 0; nLayer < 3; nLayer++)
		{
			for(y = draw2.top; y < draw2.bottom; y++) // 바닥에 깔리는 맵 //전에 있던 원래 소스
			{
				for(x = draw2.left; x < draw2.right; x++)
				{
					const _TILE_MAP_DATA_TOOL* pTMData = CChildView::TileMapTool.ToolDataGet(nLayer, x, y);
					if(pTMData == NULL) continue;
					_TILE_SET_TOOL *tile_set_tool;

					tile_set_tool = CChildView::TileSetTool.GetTileSet(pTMData->snTS);
					if(tile_set_tool == NULL) continue;
					if(tile_set_tool->m_lpDDSs == NULL)
					{// 읽지 않은 서페이스이므로 읽어 들인다.
#ifdef _DEBUG
						DWORD STLOAD = GetTickCount();
#endif
						CString szBMPFN = tile_set_tool->m_HTSs.szBMPFN;
						CString szHFN = "house"; 
						if(szBMPFN.GetLength() > 0)
						{
							szBMPFN = szBMPFN.Left(szBMPFN.GetLength() - 3);
							CChildView::g_pTEng.PathSetCur("BMP");
							
							//  9.15 먼저 obm을 읽어보고 안되면 bmp 를 읽는다.
							tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK);
							if( szBMPFN.Left(5) == szHFN )
							tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK,TRUE);
							else
							tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "obm", FALSE, tile_set_tool->m_HTSs.crCK);
							if(tile_set_tool->m_lpDDSs == NULL)
							{
								tile_set_tool->m_lpDDSs = CChildView::g_pTEng.SurfaceCreateWithBMP(szBMPFN + "bmp", FALSE, tile_set_tool->m_HTSs.crCK);
								if(tile_set_tool->m_lpDDSs)
								{
									TRACE("%s 읽음\n", szBMPFN + "bmp");
								}
								else continue;
							}
							else
							{
								TRACE("DrawChangeTile:%s 읽음(%ld)\n", szBMPFN + "obm", g_vRenderTime);
							}
						}
						////////////////////////////
#ifdef _DEBUG
						DWORD ENDLOAD = GetTickCount();
						TRACE("맵로딩시간 %d\n",ENDLOAD-STLOAD);
#endif

					}
					tile_set_tool->m_vUseTime = g_vRenderTime;

					POINT *t_pos = CChildView::TileSetTool.DataGetPos(pTMData->snTS, pTMData->snTSS, pTMData->snNum);

					if(t_pos == NULL ||
						t_pos->x < 0 ||
						t_pos->x >= MAX_TILE_SET_WIDTH ||
						t_pos->y < 0 ||
						t_pos->y >= MAX_TILE_SET_HEIGHT) continue; // 쓰지 않는 타일이면 지나간다..

					CRect rcSrc;
					rcSrc.left = t_pos->x << 5; // *32
					rcSrc.top = t_pos->y << 5; // *32
					rcSrc.right = rcSrc.left + TILE_SIZE;
					rcSrc.bottom = rcSrc.top + TILE_SIZE;
					
					CChildView::g_pTEng.BltNormal(tile_set_tool->m_lpDDSs, 
						((x-m_vOrgPos.x)<<5)+m_vCurPos.x,
						((y-m_vOrgPos.y)<<5)+m_vCurPos.y,
						rcSrc, bUCK[nLayer], nRenderType[nLayer], 16, 0, destSurface);
				}
			}
		}
		DrawShadow( CPoint(draw2.left,draw2.top) );
	}
}

void CBaseDraw::DrawSurface(CDDSurface *pDestSurface, CPoint map_offset)
{
	CRect rcSrc;

	rcSrc.left = map_offset.x - (m_vOrgPos.x<<5) + m_vCurPos.x;
	rcSrc.top = map_offset.y - (m_vOrgPos.y<<5) + m_vCurPos.y;
	rcSrc.right = rcSrc.left + WIDTH;
	rcSrc.bottom = rcSrc.top + HEIGHT;

	IDirectDrawSurface7 *srcSurface = m_pBaseSurface->GetInterface();
	IDirectDrawSurface7 *dstSurface = pDestSurface->GetInterface();

	CChildView::g_pTEng.BltNormal(srcSurface, 0, 0,
		rcSrc, FALSE, RENDER_NORMAL, 16, 0, dstSurface);
}

void CBaseDraw::BltLSPPartShadow(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, DWORD dwHalf16Mask, DWORD dwHalf32Mask)
{

	if(data == NULL) return;
	if(pRC == NULL) return;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < m_rcBaseCenter.left || rc.bottom < m_rcBaseCenter.top || rc.left >= m_rcBaseCenter.right || rc.top >= m_rcBaseCenter.bottom ) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이


	WORD* pSrc = data; 
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < m_rcBaseCenter.top)
	{
		int skipY = -rc.top+m_rcBaseCenter.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;
	int basewidth = m_rcBaseCenter.right - m_rcBaseCenter.left;
//	int baseHeight = m_rcBaseCenter.bottom - m_rcBaseCenter.top;

	WORD* pDest = NULL;
	
	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
//		int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산

				CUser::PutPixelShadowEx(pDest, nCopyCount, dwHalf16Mask, dwHalf32Mask);
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = - rc.left + m_rcBaseCenter.left;
		//int clipXR =  rc.right - m_rcBaseCenter.right;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;

			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nAddCount = 0;
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ > clipXL + basewidth)
					{
						pSrc += nCopyCount;
						nCopyCount = 0;

						continue;
						//pSrc += nCopyCount;
						//nNode--;
						//for(;nNode;nNode--)
						//{
						//	pSrc++; // 0 의 숫자
						//	pSrc += *pSrc + 1; // 유효 픽셀 숫자
						//}
						//break;
					}
					if(ndxZ >= clipXL)
					{
						if( ndxS >= clipXL + basewidth )
						{
							pSrc += nCopyCount;
							nCopyCount = 0;
							continue;
						}
						else
						{
							if( ndxE > clipXL + basewidth )
							{
								pDest += nZeroCount;
								nAddCount = ndxE - clipXL - basewidth;
								nCopyCount -= nAddCount;
							}
							else
							{
								pDest += nZeroCount;
							}
						}
					}
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL)
						{
							if( ndxE <= clipXL + basewidth )//그리는 영역보다 작거나 같을때
							{
								nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS;								
							}
							else//그리는 영역 보다 클때
							{
								nAddCount = ndxE - basewidth - clipXL;
								pSrc += clipXL -ndxS; nCopyCount -= clipXL - ndxS + nAddCount;
							}
						}
						else if( ndxS >= clipXL + basewidth )
						{
							pSrc += nCopyCount;
							nCopyCount = 0;
							continue;
						}
						else
						{
							if( ndxE <= clipXL + basewidth )
							{
								pDest += ndxS - clipXL;
							}
							else
							{
								pDest += ndxS - clipXL;
								nAddCount = ndxE - clipXL - basewidth;
								nCopyCount -= nAddCount;
							}
						}
					}

				}//if(clipXL > 0) // 왼쪽 클리핑
				else
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}

				}

				if( nCopyCount != 0 )
				CUser::PutPixelShadowEx(pDest, nCopyCount, dwHalf16Mask, dwHalf32Mask);
			}
		}
	}
	pSurface->Unlock();

}
