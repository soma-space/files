// ArmatureRes.cpp: implementation of the CArmatureRes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "ArmatureRes.h"
#include "ChildView.h"
#include "CharManager.h"
#include "CharRes.h"
#include "DManage/BB_Direct.h"

#define ARMATURE_PATH	"ARMATURE\\"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArmatureRes::CArmatureRes()
{
	int i;

	for(i = 0; i < MAX_RES_COUNT; i++)
	{
		m_ArmRes[i] = NULL;
		m_bLoad[i] = FALSE;
	}
}

CArmatureRes::~CArmatureRes()
{
	int i;

	for(i = 0; i < MAX_RES_COUNT; i++)
	{
		if(m_ArmRes[i])
		{
			delete m_ArmRes[i];
			m_ArmRes[i] = NULL;
		}
	}
}

BOOL CArmatureRes::SetArmatureInfo(CString szArmatureFileName, int index, int type)
{
	m_szFileName = szArmatureFileName;
	m_vIndex = index;

    switch(type)
	{
		case AM_KIND_ARMOR:
		case AM_KIND_HELMET:
		case AM_KIND_BOOT:
		case AM_KIND_JEAN:
		case AM_KIND_SHIELD:
		case AM_KIND_ARMS:
			 if(m_vIndex >= 500) m_vIndex = 4; //여자
			 else m_vIndex = 0; //남자.
			 break;
		case AM_KIND_SH:
			 if(m_vIndex < 10) m_vIndex = 0; // 남자..아바타..	
			 else if(m_vIndex >= 500) m_vIndex = 4; //여자.
			 else m_vIndex = m_vIndex; //npc
			 break;
	}

	return (m_szFileName.IsEmpty() == FALSE);
}

BOOL CArmatureRes::LoadElement(int ani_index)
{
	const CString szExePath = CDirectDraw::GetExecutionPath();
	CString szSplFileName, szAniFileName;
	int i=0;

	int Count = 0;
	UINT count;
	CFile spl_file, ani_file;
	WORD *word;
	int index = 0;
	SPR_DATA *spr_data;

	m_bLoad[ani_index] = TRUE;

	szSplFileName.Format("%s%02d.spl", (LPCTSTR)m_szFileName, ani_index);
	szAniFileName.Format("%sman\\man%03d%02d%s", (LPCTSTR)szExePath, m_vIndex,ani_index,".ani");
	
	if(spl_file.Open(szSplFileName, CFile::modeRead) == FALSE)
	{
		return FALSE;
	}
	if(ani_file.Open(szAniFileName, CFile::modeRead) == FALSE)
	{
		spl_file.Close();
		return FALSE;
	}
	//Spl 파일 읽기 (포맷은 Spr과 다름)
	//우석.. 10월 15일..
	
	spr_data = new SPR_DATA;
	
	spl_file.Read(&spr_data->m_HSpr, sizeof(_SPR_HEADER));
	spl_file.Seek(sizeof(_SPR_TOOL)*spr_data->m_HSpr.nQt, CFile::current);
	
	if(spr_data->m_HSpr.nQt > 0)
	{
		spr_data->m_arrSprData = new _SPR_LSP[spr_data->m_HSpr.nQt];
		spr_data->m_arrVData = new WORD *[spr_data->m_HSpr.nQt];
		
		for (i=0; i<spr_data->m_HSpr.nQt; i++)
		{
			spl_file.Read((PVOID)&spr_data->m_arrSprData[i], sizeof(_SPR_LSP));
			spr_data->m_arrSprData[i].pwV = NULL;
		}
		
		for (i=0; i<spr_data->m_HSpr.nQt; i++)
		{
			count = spr_data->m_arrSprData[i].nDataSize;
			if(count)
			{
				word = new WORD[count>>1];
				spl_file.Read((PVOID)word, count);
				if(CChildView::CharManager.GetPixelFormat()==PF_555) CCharRes::Decode2_555(spr_data->m_arrSprData[i].nLine, word);
				else CCharRes::Decode2_565_0( spr_data->m_arrSprData[i].nLine, word);//this_1
				spr_data->m_arrVData[i] = word;
			}
			else
			{
				spr_data->m_arrVData[i] = NULL;
			}
		}
	}
	
	// 애니메이션
	ani_file.Read((PVOID)&spr_data->m_HAni, sizeof(_ANI_HEADER));
	ani_file.Read((PVOID)&spr_data->m_AniData, sizeof(_ANI_DATA));
	spr_data->m_AniData.pnFrms = NULL;
	if(spr_data->m_AniData.nMaxFrm > 0)
	{
		int nQt = spr_data->m_HAni.nDirQt * spr_data->m_AniData.nMaxFrm; // 갯수 계산 = 방향 * 프레임갯수
		spr_data->m_AniData.pnFrms = new short int[nQt]; // 갯수 만큼 생성
		ani_file.Read(spr_data->m_AniData.pnFrms, nQt*2); // 갯수 만큼 읽는다.
	}
	
	m_ArmRes[ani_index] = spr_data;
	
	spl_file.Close();
	ani_file.Close();

	return TRUE;
}

//08.25
BOOL CArmatureRes::LoadArmature(CString szArmatureFileName, int Index, int Type)
{
	m_szFileName = szArmatureFileName;
	CString szSplFileName, szAniFileName;
	int i=0;

	int Count = 0;
	UINT count;
	CFile spl_file, ani_file;
	WORD *word;
	int index = 0;
	SPR_DATA *spr_data;
	
	const CString szExePath = CDirectDraw::GetExecutionPath();

    switch(Type)
	{
		case AM_KIND_ARMOR:
		case AM_KIND_HELMET:
		case AM_KIND_BOOT:
		case AM_KIND_JEAN:
		case AM_KIND_SHIELD:
		case AM_KIND_ARMS:
			 if(Index >= 500) index = 4; //여자
			 else index = 0; //남자.
			 break;
		case AM_KIND_SH:
			 if(Index < 10) index = 0; // 남자..아바타..	
			 else if(Index >= 500) index = 4; //여자.
			 else index = Index; //npc
			 break;
	}
	/////////////////////////////////////////////////////////////////
//여자 캐릭터는 18개이다..
	for(int k = 0; k < 18; k++)
	{
		szSplFileName.Format("%s%02d.spl", (LPCTSTR)szArmatureFileName, k);
		szAniFileName.Format("%sman\\man%03d%02d%s", (LPCTSTR)szExePath ,index ,k,".ani");

		if(spl_file.Open(szSplFileName, CFile::modeRead) == FALSE) continue;
		if(ani_file.Open(szAniFileName, CFile::modeRead) == FALSE)
		{
			spl_file.Close();
			continue;
		}
		//Spl 파일 읽기 (포맷은 Spr과 다름)
		//우석.. 10월 15일..

		spr_data = new SPR_DATA;

		spl_file.Read(&spr_data->m_HSpr, sizeof(_SPR_HEADER));
		spl_file.Seek(sizeof(_SPR_TOOL)*spr_data->m_HSpr.nQt, CFile::current);

		if(spr_data->m_HSpr.nQt > 0)
		{
			spr_data->m_arrSprData = new _SPR_LSP[spr_data->m_HSpr.nQt];
			spr_data->m_arrVData = new WORD *[spr_data->m_HSpr.nQt];

			for (i=0; i<spr_data->m_HSpr.nQt; i++)
			{
				spl_file.Read((PVOID)&spr_data->m_arrSprData[i], sizeof(_SPR_LSP));
				spr_data->m_arrSprData[i].pwV = NULL;
			}

			for (i=0; i<spr_data->m_HSpr.nQt; i++)
			{
				count = spr_data->m_arrSprData[i].nDataSize;
				if(count)
				{
					word = new WORD[count>>1];
					spl_file.Read((PVOID)word, count);
					if(CChildView::CharManager.GetPixelFormat()==PF_555) CCharRes::Decode2_555(spr_data->m_arrSprData[i].nLine, word);
					else CCharRes::Decode2_565_0( spr_data->m_arrSprData[i].nLine, word);//this_1
					spr_data->m_arrVData[i] = word;
				}
				else
				{
					spr_data->m_arrVData[i] = NULL;
				}
			}
		}

		// 애니메이션
		ani_file.Read((PVOID)&spr_data->m_HAni, sizeof(_ANI_HEADER));
		ani_file.Read((PVOID)&spr_data->m_AniData, sizeof(_ANI_DATA));
		spr_data->m_AniData.pnFrms = NULL;
		if(spr_data->m_AniData.nMaxFrm > 0)
		{
			int nQt = spr_data->m_HAni.nDirQt * spr_data->m_AniData.nMaxFrm; // 갯수 계산 = 방향 * 프레임갯수
			spr_data->m_AniData.pnFrms = new short int[nQt]; // 갯수 만큼 생성
			ani_file.Read(spr_data->m_AniData.pnFrms, nQt*2); // 갯수 만큼 읽는다.
		}

		m_ArmRes[k] = spr_data;

		spl_file.Close();
		ani_file.Close();
	}
	return TRUE;
}

SPR_DATA *CArmatureRes::GetSprData(int ani_number)
{
	ASSERT(ani_number >= 0 && ani_number < MAX_RES_COUNT);

	if(m_bLoad[ani_number] == FALSE) LoadElement(ani_number);

	return m_ArmRes[ani_number];
}
