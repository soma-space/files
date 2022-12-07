// TileSet.h: interface for the CTileSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILESET_H__9FDAF9DB_6AA6_4F51_B307_C1CEB034CC07__INCLUDED_)
#define AFX_TILESET_H__9FDAF9DB_6AA6_4F51_B307_C1CEB034CC07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTileSet  
{
protected:
	_TILE_SUB_SET_DATA_TOOL m_Datas[MAX_TILE_SET][MAX_TILE_SUB_SET];

public:
	_TILE_SET_HEADER m_HTSs[MAX_TILE_SET];
	LPDIRECTDRAWSURFACE7 m_lpDDSs[MAX_TILE_SET];

public:
	void Alloc(int nTS, int nTSS, int width, int height);
	void Release(int nTS, int nTSS);
	void ReleaseAll();

	void Load(const char* szFileName);
	const _TILE_SUB_SET_DATA_TOOL* DataGet(int nTS, int nTSS) { return &m_Datas[nTS][nTSS]; }

	CTileSet();
	virtual ~CTileSet();

};

#endif // !defined(AFX_TILESET_H__9FDAF9DB_6AA6_4F51_B307_C1CEB034CC07__INCLUDED_)
