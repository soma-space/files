// ArmatureManager.h: interface for the CArmatureManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARMATUREMANAGER_H__A1428FE8_62AB_11D3_8424_00105A6B96E7__INCLUDED_)
#define AFX_ARMATUREMANAGER_H__A1428FE8_62AB_11D3_8424_00105A6B96E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArmatureRes.h"

class ARMATURE_RES
{
public:
	ARMATURE_RES();
	~ARMATURE_RES();

public:
	CArmatureRes	m_ArmatureRes;
	int				m_RefCount;
};

typedef CMap<int, int, ARMATURE_RES *, ARMATURE_RES *> MAP_ARMATURE;

#define AM_KIND_ARMOR	0
#define AM_KIND_HELMET	1
#define AM_KIND_BOOT	2
#define AM_KIND_JEAN	3
#define AM_KIND_SHIELD	4
#define AM_KIND_ARMS	5
#define AM_KIND_SH      6 //그림자.

#define AM_KIND_NUM		7

class CArmatureManager  
{
public:
	int m_nNull_index;
	CArmatureManager();
	virtual ~CArmatureManager();

	void				RemoveAllData();			//데이터 청소
	BOOL				LoadArmatureRes(int nType, int index);
	void				UnloadArmatureRes(int nType, int index);
	void				LoadAllArmatureRes();

	CArmatureRes*		GetTypeIndexedArmature(int nType, int nIndex);
protected:
	CString				m_szArmaturePath;//장비 아이템 리소스 path

public://8.27
	MAP_ARMATURE		m_ArmatureType[AM_KIND_NUM];
};

#endif // !defined(AFX_ARMATUREMANAGER_H__A1428FE8_62AB_11D3_8424_00105A6B96E7__INCLUDED_)
