// TileSetTool.h: interface for the CTileSetTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILESETTOOL_H__C0E02734_3F05_4163_8E9A_FDAE5D9BA470__INCLUDED_)
#define AFX_TILESETTOOL_H__C0E02734_3F05_4163_8E9A_FDAE5D9BA470__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "IGObject\\DDEngine.h"
#include "_TILE_SUB_SET_DATA_TOOL.h"

class _TILE_SET_TOOL
{
public:
	_TILE_SET_HEADER		m_HTSs;
	_TILE_SUB_SET_DATA_TOOL	*m_Datas;
	LPDIRECTDRAWSURFACE7	m_lpDDSs;
	DWORD					m_vUseTime;

public:
	_TILE_SET_TOOL();
	~_TILE_SET_TOOL();
};

typedef CMap<int, int, _TILE_SET_TOOL *, _TILE_SET_TOOL *> CMapTileSetTool;

class CTileSetTool
{
public:
	CMapTileSetTool m_mapData;

public:
	POINT *DataGetPosCur(int nTS, int nTSS, int nIndex);
	int *DataGetFlagCur(int nTS, int nTSS, int nIndex);
	POINT *DataGetPos(int nTS, int nTSS, int nIndex, int nAniNum = 0);
	_TILE_SUB_SET_DATA_TOOL* DataGetSafe(int nTS, int nTSS);
	_TILE_SET_TOOL *GetTileSet(int nTS);
	
	void Alloc(int nTS, int nTSS, int width, int height);
	void Release(int nTS, int nTSS);
	void ReleaseAll();

	void Load(const char* szFileName);
	void ReAlloc(int nTS, int nTSS, int width, int height);

	void ResetAni();
	void Increase();
	void Decrease();

	CTileSetTool();
	virtual ~CTileSetTool();
};

#endif // !defined(AFX_TILESETTOOL_H__C0E02734_3F05_4163_8E9A_FDAE5D9BA470__INCLUDED_)

