// MyInvData.cpp: implementation of the CMyInvData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyInvData.h"
#include "DarkDef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyInvData::CMyInvData()
{

}

CMyInvData::~CMyInvData()
{

}

void CMyInvData::Init()
{
	m_vIndex = -1;
	m_vPicNum = 0;
	m_vType = 0;//무기 타입 모션 조정을 위해서...
	m_vArmPos = 0;// 장착위
	m_vGender = ITEM_ALL;
	m_vMinDamage = 0;
	m_vMaxDamage = 0;
	m_vUsage = 0;
	m_vMaxUsage = 0;
	m_vOrgMaxUsage = 0;
	for(int i = 0; i < 7; i++) m_vSpecial[i] = 0;
	m_vLimitStr = 0;
	m_vLimitInt = 0;
	m_vLimitSkill = 0;
	m_vValue = 0;
	m_vName.Empty();
	m_strMagic.Empty();
	m_strSNum.Empty();
}

CMyInvData& CMyInvData::operator =(CMyInvData& other)
{
	m_vIndex = other.m_vIndex;
	m_vPicNum = other.m_vPicNum;
	m_vType = other.m_vType;
	m_vArmPos = other.m_vArmPos;
	m_vGender = other.m_vGender;
	m_vMinDamage = other.m_vMinDamage;
	m_vMaxDamage = other.m_vMaxDamage;
	m_vUsage = other.m_vUsage;
	m_vMaxUsage = other.m_vMaxUsage;
	m_vOrgMaxUsage = other.m_vOrgMaxUsage;
	m_vName = other.m_vName;
	for(int i = 0; i < 7; i++)
	{
		m_vSpecial[i] = other.m_vSpecial[i];
	}
	m_vValue = other.m_vValue;
	m_vLimitStr = other.m_vLimitStr;
	m_vLimitInt = other.m_vLimitInt;
	m_vLimitSkill = other.m_vLimitSkill;
	m_strMagic = other.m_strMagic;
	m_strSNum = other.m_strSNum;

	return *this;
}