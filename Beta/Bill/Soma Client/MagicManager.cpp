// MagicManager.cpp: implementation of the CMagicManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "MagicManager.h"
#include "MagicRes.h"
#include "DManage/BB_Direct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAGIC_PATH	"MAGICRES\\"

MAGIC_RES::MAGIC_RES()
{
	m_RefCount = 0;
}

MAGIC_RES::~MAGIC_RES()
{
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMagicManager::CMagicManager()
{
	m_szMagicPath.Empty();
}

CMagicManager::~CMagicManager()
{
	RemoveAllData();
}

void CMagicManager::RemoveAllData()
{
	POSITION pos = m_mapMagicRes.GetStartPosition();
	int key;
	MAGIC_RES *value;

	while(pos)
	{
		m_mapMagicRes.GetNextAssoc(pos, key, value);
		delete value;
	}
	m_mapMagicRes.RemoveAll();
}

CMagicRes* CMagicManager::GetIndexedMagic(int index)
{
	MAGIC_RES *pMagic;
	if(m_mapMagicRes.Lookup(index, pMagic) == TRUE) return &pMagic->m_MagicRes;
	else return NULL;
}

BOOL CMagicManager::LoadMagicRes(int index)
{
	if (m_szMagicPath.IsEmpty())
	{
		m_szMagicPath = CDirectDraw::GetExecutionPath();
		m_szMagicPath += MAGIC_PATH;
	}

	MAGIC_RES *value;

	if(m_mapMagicRes.Lookup(index, value))
	{
		value->m_RefCount++;
		return TRUE;
	}

	value = new MAGIC_RES;

	CString szFileName;
	szFileName.Format("%smgr_%03d.mrf", (LPCTSTR)m_szMagicPath, index);
	if (value->m_MagicRes.LoadMagic(szFileName))
	{
		m_mapMagicRes[index] = value;
		value->m_RefCount++;
	}
	else 
	{
		SAFE_DELETE(value);
		return FALSE;
	}

	return TRUE;
}

void CMagicManager::UnloadMagicRes(int nIndex)
{
	MAGIC_RES *value;

	if(m_mapMagicRes.Lookup(nIndex, value) == TRUE)
	{
		value->m_RefCount--;

		if(value->m_RefCount <= 0)
		{
			delete value;
			m_mapMagicRes.RemoveKey(nIndex);
		}
	}
}
