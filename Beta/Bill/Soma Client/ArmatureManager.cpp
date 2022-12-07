// ArmatureManager.cpp: implementation of the CArmatureManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "ArmatureManager.h"
#include "ArmatureRes.h"
#include "DManage/BB_Direct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_ARMATURE_MANAGE_NUM	1000
#define ARMATURE_PATH	"ARMATURE\\"

ARMATURE_RES::ARMATURE_RES()
{
	m_RefCount = 0;
}

ARMATURE_RES::~ARMATURE_RES()
{
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArmatureManager::CArmatureManager()
{
}

CArmatureManager::~CArmatureManager()
{
	RemoveAllData();
}

void CArmatureManager::RemoveAllData()
{
	for (int ai = 0; ai<AM_KIND_NUM; ai++)
	{
		POSITION pos;
		int index;
		ARMATURE_RES *value;

		pos = m_ArmatureType[ai].GetStartPosition();
		while(pos)
		{
			m_ArmatureType[ai].GetNextAssoc(pos, index, value);
			delete value;
		}
		m_ArmatureType[ai].RemoveAll();
	}
}

void CArmatureManager::LoadAllArmatureRes()
{
	CString szIndexFileName, szArmPath;
	CString szFileName, szLineString;
	CStdioFile IndexFile;
	int nIndex, nType;
	ARMATURE_RES *value;

	szIndexFileName = szArmPath = CDirectDraw::GetExecutionPath();
	szIndexFileName += "arm.dat";

	szArmPath += "Armature";
	SetCurrentDirectory((LPCTSTR)szArmPath);

	if(IndexFile.Open(szIndexFileName, CFile::modeRead))
	{
		while(IndexFile.ReadString(szFileName))
		{
			sscanf((LPCTSTR)szFileName, "a%01d%03d", &nType, &nIndex);

			if(m_ArmatureType[nType].Lookup(nIndex, value)) continue;

			ARMATURE_RES *pArmatureRes = new ARMATURE_RES;
			if(pArmatureRes->m_ArmatureRes.LoadArmature(szFileName, nIndex, nType))
			{
				m_ArmatureType[nType][nIndex] = pArmatureRes;
				TRACE2("Loaded : %d %d\n", nType, nIndex);
			}
			else 
			{
				SAFE_DELETE(pArmatureRes);
				TRACE2("Failed : %d %d\n", nType, nIndex);
			}
		}
		IndexFile.Close();
	}
	SetCurrentDirectory((LPCTSTR)CDirectDraw::GetExecutionPath());
}

BOOL CArmatureManager::LoadArmatureRes(int nType, int nIndex)
{
	if(nIndex < 0 || nType < 0) return FALSE;

	ARMATURE_RES *value;
	
	if (m_szArmaturePath.IsEmpty())
	{
		//실행 경로 설정
		m_szArmaturePath = CDirectDraw::GetExecutionPath();
		m_szArmaturePath += ARMATURE_PATH;
	}

	CString szFileName;
	szFileName.Format("%sa%d%03d", (LPCTSTR)m_szArmaturePath, nType, nIndex);
	
	if(m_ArmatureType[nType].Lookup(nIndex, value))
	{
		value->m_RefCount++;
		return TRUE;
	}
	
	ARMATURE_RES *pArmatureRes = new ARMATURE_RES;
	
	pArmatureRes->m_ArmatureRes.SetArmatureInfo(szFileName, nIndex, nType);
	m_ArmatureType[nType][nIndex] = pArmatureRes;
/*	if (pArmatureRes->m_ArmatureRes.LoadArmature(szFileName, nIndex, nType))
	{
		m_ArmatureType[nType][nIndex] = pArmatureRes;
		pArmatureRes++;
	}
	else 
	{
		SAFE_DELETE(pArmatureRes);
		return FALSE;
	}*/

	return TRUE;
}

void CArmatureManager::UnloadArmatureRes(int nType, int nIndex)
{
	if(nType < 0 || nIndex < 0) return;

	ARMATURE_RES *value;

	if(m_ArmatureType[nType].Lookup(nIndex, value))
	{
		value->m_RefCount--;

		if(value->m_RefCount <= 0)
		{
			delete value;
			m_ArmatureType[nType].RemoveKey(nIndex);
		}
	}
}

CArmatureRes* CArmatureManager::GetTypeIndexedArmature(int nType, int nIndex)
{
	ARMATURE_RES *value;

	if(m_ArmatureType[nType].Lookup(nIndex, value))
	{
		return &value->m_ArmatureRes;
	}
	else return NULL;
}