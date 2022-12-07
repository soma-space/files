// HouseInfoManager.h: interface for the CHouseInfoManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOUSEINFOMANAGER_H__CF6FF904_84C6_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_HOUSEINFOMANAGER_H__CF6FF904_84C6_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HouseInfo.h"

class _HOUSE_INFO_HEADER // 집 정보 헤더
{
public:
	char	szID[4]; // 'H' 'I' 'F' NULL
	char	szRemark[64]; // 주석
	int		nQt; // 정보를 가지고 있는 집의 개수

public:
	void Save(CFile& file);
	BOOL Load(CFile& file);
};

typedef CMap<int,int,CHouseInfo*,CHouseInfo*&> CMapHouseInfo;

class CHouseInfoManager  
{
public:
	CHouseInfoManager();
	virtual ~CHouseInfoManager();

	void SetHouse(int number, int cx, int cy);
	void SetHouse(int number, CSize size);
	void DeleteHouse(int number);
	void DeleteAllHouse();

	void Save(CString file_name);
	void Load(CString file_name);

public:
	CMapHouseInfo m_MapHouseInfo;
	_HOUSE_INFO_HEADER m_HouseInfoHeader;
};

#endif // !defined(AFX_HOUSEINFOMANAGER_H__CF6FF904_84C6_11D4_98B0_00C02615866B__INCLUDED_)
