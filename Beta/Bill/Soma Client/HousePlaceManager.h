// HousePlaceManager.h: interface for the CHousePlaceManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOUSEPLACEMANAGER_H__964FF6A3_8E14_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_HOUSEPLACEMANAGER_H__964FF6A3_8E14_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHousePlaceManager  
{
public:
	CHousePlaceManager();
	virtual ~CHousePlaceManager();

	void RcvBuildHouse(PBYTE bypData);
	void SendHousePlace();
	BOOL PtInPlace(CPoint pos);
	void SetHousePlace(PBYTE bypData);
	void DeleteHousePlace();
	int IsSelectMode() {return m_bEnableHousePlace;};
public:
	short m_vTileSet, m_vSelPlace;
	BOOL m_bEnableHousePlace;
	int *m_pPlaceIndex;
	CRect *m_pPlaceRect;
	int m_vPlaceCount;
};

#endif // !defined(AFX_HOUSEPLACEMANAGER_H__964FF6A3_8E14_11D4_98B0_00C02615866B__INCLUDED_)
