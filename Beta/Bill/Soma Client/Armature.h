// Armature.h: interface for the CArmature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARMATURE_H__28B7FFC1_6507_11D3_8424_00105A6B96E7__INCLUDED_)
#define AFX_ARMATURE_H__28B7FFC1_6507_11D3_8424_00105A6B96E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "illstruct.h"
#include "User.h"

class CArmatureRes;

class CArmature  
{
public:
	CArmature();
	virtual ~CArmature();

	void	ReadyData(int nAniIndex, int nSpriteIndex, enum CUser::UserDirection eDir);

	friend		class	CUser;
protected:	//DataMember
	CArmatureRes*	m_pArmatureRes[7];	//AM_KIND_NUM -> 7
	_SPR_LSP*		m_pSprData[7];
	PWORD			m_pV1Data[7];	//리소스가 캐릭터 영역1에만 있을 것임.
//	PWORD			m_pV2Data[6];
//	PWORD			m_pSData[6];
//	PWORD			m_pFX[6];

	int				m_nArmor;
	int				m_nHelmet;
	int				m_nBoot;
	int				m_nJean;
	int				m_nShield;
	int				m_nArms;
};

#endif // !defined(AFX_ARMATURE_H__28B7FFC1_6507_11D3_8424_00105A6B96E7__INCLUDED_)
