// Item_Total.h: interface for the CItem_Total class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_TOTAL_H__C6DCFD03_F40D_11D3_B175_00105A735718__INCLUDED_)
#define AFX_ITEM_TOTAL_H__C6DCFD03_F40D_11D3_B175_00105A735718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item_Small.h"
#include "MagicItem.h"

class CItem_Total  
{
public:
	CItem_Total();
	virtual ~CItem_Total();

public:
	void Init(CDirectDraw *pDirectDraw);
//	CItem_Arm       m_Item_Arm;
//	CItem_Armor     m_Item_Armor;
//	CItem_Boot      m_Item_Boot;
//	CItem_Helmet    m_Item_Helmet;
//	CItem_Jean      m_Item_Jean;
//	CItem_Shield    m_Item_Shield;

/*	CItem_Armor*     m_Item_Armor;
	CItem_Boot*      m_Item_Boot;
	CItem_Helmet*    m_Item_Helmet;
	CItem_Shield*    m_Item_Shield;
	CItem_Jean*      m_Item_Jean;
	CItem_Arm*      m_Item_Arm;
*/
	CItem_Small*    m_Item_Small;// 작은 아이템들...실제 사용 위에 것들은 사용안할거 같은데..음..
	CMagicItem*     m_Magic_Item;

	int          m_Item_Type[50];//종류
	int          m_Item_Index[50];//번호
	int          m_Item_Picnum[50];// 그림 번호
	int          m_Item_Arm[50];
	CString      m_Item_Name[50];

};

#endif // !defined(AFX_ITEM_TOTAL_H__C6DCFD03_F40D_11D3_B175_00105A735718__INCLUDED_)
