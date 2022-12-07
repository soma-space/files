// BuildHouseManager.h: interface for the CBuildHouseManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUILDHOUSEMANAGER_H__83B96EA1_8BBB_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_BUILDHOUSEMANAGER_H__83B96EA1_8BBB_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HouseInfoManager.h"

class CBuildHouseManager  
{
public:
	CBuildHouseManager();
	virtual ~CBuildHouseManager();

public:
	void LoadHouseInfo(CString file_name);
	void BuildHouse(int x, int y, int nTS);
	void BuildHouse(CRect hRect, int nTS);
	CSize TileMapSet(int x, int y, int nLayer, int nTS, int nTSS);
	void HouseInfoSet(int x, int y, int nTS);
	CSize GetHouseSize(int nTS);
	CPoint GetInnerTopLeft(int nTS, CRect place_rect);
	CRect GetTileSetRect(int nTS, int nTSS);

	CHouseInfoManager m_HouseInfoManager;
};

#endif // !defined(AFX_BUILDHOUSEMANAGER_H__83B96EA1_8BBB_11D4_98B0_00C02615866B__INCLUDED_)
