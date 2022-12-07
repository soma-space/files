// MagicManager.h: interface for the CMagicManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAGICMANAGER_H__0669E990_5FB5_11D3_8424_00105A6B96E7__INCLUDED_)
#define AFX_MAGICMANAGER_H__0669E990_5FB5_11D3_8424_00105A6B96E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MagicRes.h"

class MAGIC_RES
{
public:
	MAGIC_RES();
	~MAGIC_RES();

public:
	CMagicRes	m_MagicRes;
	int			m_RefCount;
};

typedef CMap<int, int, MAGIC_RES *, MAGIC_RES *> MAP_MAGICRES;

#define MAX_MAGIC_NUM	1000

class CMagicManager  
{
public:
	CMagicManager();
	virtual ~CMagicManager();

	void				RemoveAllData();			//单捞磐 没家
	void				AddRef(int nIndex);			//CharManager客 悼老
	void				Release(int nIndex);
	BOOL				LoadMagicRes(int index);
	void				UnloadMagicRes(int index);
	CMagicRes*			GetIndexedMagic(int index);

protected:
	CString				m_szMagicPath;				//付过 Resource Folder
	MAP_MAGICRES		m_mapMagicRes;
};

#endif // !defined(AFX_MAGICMANAGER_H__0669E990_5FB5_11D3_8424_00105A6B96E7__INCLUDED_)
