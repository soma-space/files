// TileMapTool.h: interface for the CTileMapTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILEMAPTOOL_H__2D531252_74CC_4D1F_AB25_DF4BD1112B15__INCLUDED_)
#define AFX_TILEMAPTOOL_H__2D531252_74CC_4D1F_AB25_DF4BD1112B15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IGObject\\TileMap.h"

class CTileMapTool : public CTileMap
{
protected:
	HGLOBAL	m_hDatasTool[5]; // ∏  µ•¿Ã≈Õ..
	_TILE_MAP_DATA_TOOL* m_pDatasTool[3]; // ∏  µ•¿Ã≈Õ..

public:
	void AllocTool(int nWidth, int nHeight);
	void ReleaseEx();
	void ReAlloc(int nWidth, int nHeight);
	void New();
	void LoadFromToolData(const char* szFileName);
	void SaveToToolData(const char* szFileName);
	void SaveToGameFormat(const char* szFileName, COLORREF crCK);
	
	_TILE_MAP_DATA_TOOL* ToolDataGet(int nLayer, int x, int y);

	CTileMapTool();
	virtual ~CTileMapTool();

};

#endif // !defined(AFX_TILEMAPTOOL_H__2D531252_74CC_4D1F_AB25_DF4BD1112B15__INCLUDED_)
