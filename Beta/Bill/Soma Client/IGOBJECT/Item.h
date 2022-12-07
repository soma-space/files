// Item.h: interface for the CItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_H__46BA4ABA_EF29_4984_8272_5CF527F4C911__INCLUDED_)
#define AFX_ITEM_H__46BA4ABA_EF29_4984_8272_5CF527F4C911__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AniObjLSP.h"

class CItem  
{
public:
	CAniObjLSP* m_pAniObj;
	_ITEM_HEADER m_Header;

protected:
	_ITEM_DATA* m_pDatas[MAX_ITEM];

public:
	void Load(const char* szFileName, const char* szAniPath, const char* szSprPath, int nPF);
	_ITEM_DATA* DataGet(int index);
	void Release();
	void Alloc(int nQt);
	CItem();
	virtual ~CItem();

};

#endif // !defined(AFX_ITEM_H__46BA4ABA_EF29_4984_8272_5CF527F4C911__INCLUDED_)
