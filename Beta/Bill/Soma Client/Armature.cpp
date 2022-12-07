// Armature.cpp: implementation of the CArmature class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Armature.h"
#include "ArmatureRes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArmature::CArmature()
{
	//변수 초기화
	m_nArmor = -1;
	m_nHelmet = -1;
	m_nBoot = -1;
	m_nJean = -1;
	m_nShield = -1;
	m_nArms = -1;

	//AM_KIND_NUM = 7// 강제적으로 7로 잡았다.
	ZeroMemory(m_pArmatureRes, sizeof(CArmatureRes*)*7);
}

CArmature::~CArmature()
{

}

void CArmature::ReadyData(int nAniIndex, int nSpriteIndex, enum CUser::UserDirection eDir)
{
	SPR_DATA*		spr_data;
	int				nFrame;
	CArmatureRes*	pArmature	= NULL;
	BOOL			bSetFail	= FALSE;

	// 강제적으로 7로 잡았다..나중에 고쳐야함..
	for (int i=0; i<7; i++)
	{
		pArmature = m_pArmatureRes[i];
		if (pArmature)
		{
			spr_data = pArmature->GetSprData(nAniIndex);
			if(spr_data)
			{
				int ii = eDir*spr_data->m_AniData.nMaxFrm + nSpriteIndex;
				if(ii < 0 || ii >= spr_data->m_HAni.nDirQt * spr_data->m_AniData.nMaxFrm)
				{
					return;
				}
				nFrame = spr_data->m_AniData.pnFrms[ii];
				if(nFrame < 0 || nFrame >= spr_data->m_HSpr.nQt)
				{
					return;
				}
				m_pSprData[i] = &spr_data->m_arrSprData[nFrame];
				m_pV1Data[i] = spr_data->m_arrVData[nFrame];

				bSetFail	= FALSE;
			}
			else bSetFail = TRUE;
		}
		else bSetFail = TRUE;

		if (bSetFail)
		{
			INIT_PTR(m_pSprData[i]);
			INIT_PTR(m_pV1Data[i]);
		}
	}
}
