// CharManager.cpp: implementation of the CCharManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "CharRes.h"
#include "CharManager.h"
#include "DManage/BB_Direct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CHARFILE_NAME _T("%sarmature\\man") 
//#define CHARFILE_NAME _T("%sCHR\\%d.chx")

#define MAX_CHAR_MANAGE		2000

CHAR_RES::CHAR_RES()
{
	m_RefCount = 0;
}

CHAR_RES::~CHAR_RES()
{
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCharManager::CCharManager()
{
	m_PixelFormat = PF_565;
}

CCharManager::~CCharManager()
{
	RemoveAllData();
}

void CCharManager::RemoveAllData()
{
	POSITION pos;
	int key;
	CHAR_RES *value;

	pos = m_mapCharRes.GetStartPosition();
	while(pos)
	{
		m_mapCharRes.GetNextAssoc(pos, key, value);
		SAFE_DELETE(value);
	}
	m_mapCharRes.RemoveAll();
}

void CCharManager::LoadAllCharRes()
{
	CString szIndexFileName, szManPath;
	CString szFileName, szLineString;
	CStdioFile IndexFile;
	int index;
	CHAR_RES *value;

	szIndexFileName = szManPath = CDirectDraw::GetExecutionPath();
	szIndexFileName += "man.dat";

	szManPath += "Man";
	SetCurrentDirectory((LPCTSTR)szManPath);

	if(IndexFile.Open(szIndexFileName, CFile::modeRead))
	{
		while(IndexFile.ReadString(szFileName))
		{
			index = atoi((LPCTSTR)szFileName);
			if(m_mapCharRes.Lookup(index, value)) continue;

			value = new CHAR_RES;
			if(value->m_CharRes.Load(szFileName, m_PixelFormat, index))
			{
				m_mapCharRes[index] = value;
				TRACE1("Loaded Man: %d\n", index);
			}
			else 
			{
				SAFE_DELETE(value);
				TRACE1("Failed Man: %d\n", index);
			}
		}
		IndexFile.Close();
	}
	SetCurrentDirectory((LPCTSTR)CDirectDraw::GetExecutionPath());
}

BOOL CCharManager::LoadCharRes(int index)
{
	if (index<0) return FALSE;

	CHAR_RES *value;
	CString szFileName;

	if(m_mapCharRes.Lookup(index, value) == FALSE)
	{
		value = new CHAR_RES;
		value->m_CharRes.SetIndex(index);
		value->m_RefCount++;
		m_mapCharRes[index] = value;
		return TRUE;
/*		if(value->m_CharRes.Load(szFileName, m_PixelFormat, index))
		{
			m_mapCharRes[index] = value;
			value->m_RefCount++;
			return TRUE;
		}
		else
		{
			TRACE("새로운 %d 번 캐릭터를 읽기 시작 이건 두번 읽으면 안됨...\n", index);
			delete value;
			return FALSE;
		}*/
	}
	else
	{
		value->m_RefCount++;
		return TRUE;
	}
}

void CCharManager::UnloadCharRes(int index)
{
	CHAR_RES *value;

	if(m_mapCharRes.Lookup(index, value))
	{
		value->m_RefCount--;
		if(value->m_RefCount <= 0)
		{
			delete value;
			m_mapCharRes.RemoveKey(index);
		}
	}
}

void CCharManager::SetPixelFormat(CDDSurface* pSurface)
{
	DWORD r, g, b;
	pSurface->GetRGBMasks(r, g, b);

	if (r == 0x7c00) m_PixelFormat = PF_555;		//RGB구성: 555포맷( 16bit Mode일 경우 -> 특히 cirrus칩 종류)
	else if (r == 0xf800) m_PixelFormat = PF_565;	//RGB구성: 565포맷 
}

CCharRes *CCharManager::GetIndexedCharRes(int index)
{
	CHAR_RES *value;

	if(m_mapCharRes.Lookup(index, value)) return &value->m_CharRes;
	else return NULL;
}