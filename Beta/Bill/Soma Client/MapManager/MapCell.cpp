// MapCell.cpp: implementation of the CMapCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapCell.h"
#include "SM_MapManager.h"
#include "SgrRes.h"
#include "Tile.h"
#include "TileGroup.h"
#include "TileVary.h"
#include "TilePiece.h"
#include "CustomTile.h"
#include "CustomTileVary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SM_MapManager* CMapCell::m_bpMapManager = NULL;


CMapCell::CMapCell()
{
	ASSERT(m_bpMapManager);
	m_bpTilePiece = NULL;
	m_iAnimationOrder = 0;
}

CMapCell::~CMapCell()
{

}

BOOL CMapCell::Load(CArchive &ar)
{
	//다음은 보관할 필요가 없는 데이터임
	//m_bpTilePiece를 지정하기 위해서만 필요한 데이터임
/*	BYTE	bIsCustomTile;
	short	idxCustomTile;
	short	posCustom_x;
	short	posCustom_y;

	short	idxTilePiece;
	short	idxTileVary;
	short	idxTileGroup;
	short	idxTile;
	short	idxSgr;

	ar >> bIsCustomTile;
	ar >> idxCustomTile;
	ar >> posCustom_x;
	ar >> posCustom_y;
	ar >> m_iLayer[MC_LEFT] >> m_iLayer[MC_RIGHT];
	ar >> m_bMoveable[MC_LEFT] >> m_bMoveable[MC_RIGHT];

	ar >> idxTilePiece;
	ar >> idxTileVary;
	ar >> idxTileGroup;
	ar >> idxTile;
	ar >> idxSgr;
	ar >> m_iEventNum;
*/
	///////////////////////////배경 추가..
	ar.Read((void *)&TileInfo1, 4);
	if(TileInfo1.m_bIsCustomTile == 1) ar.Read((void *)&TileInfo2, 4);
//08.27
/*	if (TileInfo1.m_bIsCustomTile)
	{
		m_bpTilePiece = m_bpMapManager->
						GetSgrRes(TileInfo1.m_idxSgr)->
						GetApCustomTile().GetAt(TileInfo2.m_idxCustomTile)->
						GetApCustomTileVary().GetAt(TileInfo1.m_idxVary)->
						GetApTilePiece(TileInfo2.m_posCustom_x).GetAt(TileInfo2.m_posCustom_y);
	}
	else
	{
		m_bpTilePiece = m_bpMapManager->
						GetSgrRes(TileInfo1.m_idxSgr)->
						GetApTile().GetAt(TileInfo1.m_idxTile)->
						GetApTileGroup().GetAt(TileInfo1.m_idxTileGroup)->
						GetApTileVary().GetAt(TileInfo1.m_idxVary)->
						GetApTilePiece().GetAt(TileInfo1.m_idxVaryTile);

	}
	/////////////////////////////////////
*/
	//m_bpTilePiece에 Resource를 직접연결하므로 인덱스들을 이용해서 추적하지 않음,,, 그리기에는 무조건 Resource만 연결되면 됨
	//빠른 Access 가능
/*	if (bIsCustomTile)
	{
		m_bpTilePiece = m_bpMapManager->
						GetSgrRes(idxSgr)->
						GetApCustomTile().GetAt(idxCustomTile)->
						GetApCustomTileVary().GetAt(idxTileVary)->
						GetApTilePiece(posCustom_x).GetAt(posCustom_y);
	}
	else
	{
		m_bpTilePiece = m_bpMapManager->
						GetSgrRes(idxSgr)->
						GetApTile().GetAt(idxTile)->
						GetApTileGroup().GetAt(idxTileGroup)->
						GetApTileVary().GetAt(idxTileVary)->
						GetApTilePiece().GetAt(idxTilePiece);
	}
*/
	return TRUE;
}

void CMapCell::RotateAnimation()
{
//	m_iAnimationOrder ++;
//	m_iAnimationOrder %= m_bpTilePiece->GetAnimationNum();// 임시
}

void CMapCell::Draw(CDDSurface *pDestSurface, CPoint pos)
{
	m_bpTilePiece->Draw(pDestSurface, pos, m_iAnimationOrder);
}
