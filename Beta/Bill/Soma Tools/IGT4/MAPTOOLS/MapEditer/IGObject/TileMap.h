// TileMap.h: interface for the CTileMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILEMAP_H__3180BEE7_24AA_4EF2_BEEF_8E93159D5533__INCLUDED_)
#define AFX_TILEMAP_H__3180BEE7_24AA_4EF2_BEEF_8E93159D5533__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <DDraw.h>

class CTileMap  
{
public:
	_TILE_MAP_HEADER m_HTMap; // 맵 헤더
	LPDIRECTDRAWSURFACE m_lpDDSs[MAX_TILE_MAP_LAYER]; // 맵 리소스 서피스

protected:
	HGLOBAL m_hDatas[MAX_TILE_MAP_LAYER]; // 맵 데이터 메모리 핸들
	short int* m_pnDatas[MAX_TILE_MAP_LAYER]; // 실제 맵 데이터.. 상위 4비트는 플래그.. 0-없음 1-충돌 2-투명찍기 4-안찍기. 하위 12비트는 인덱스

public:
	void Alloc(int nWidth, int nHeight);
	const short int* DataGet(int nLayer, int x, int y) { return &m_pnDatas[nLayer][y*m_HTMap.nWidth + x]; }
	void Load(const char* szFileName);
	void Release();
	CTileMap();
	virtual ~CTileMap();

};

#endif // !defined(AFX_TILEMAP_H__3180BEE7_24AA_4EF2_BEEF_8E93159D5533__INCLUDED_)
