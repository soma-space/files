// Item_Total.cpp: implementation of the CItem_Total class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Item_Total.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItem_Total::CItem_Total()
{
}

CItem_Total::~CItem_Total()
{
	SAFE_DELETE(m_Item_Small);
	SAFE_DELETE(m_Magic_Item);

}

void CItem_Total::Init(CDirectDraw *pDirectDraw)
{
	m_Item_Small = new CItem_Small();
	m_Item_Small->InitItem(pDirectDraw);
	m_Magic_Item = new CMagicItem();
	m_Magic_Item->InitItem(pDirectDraw);
}

