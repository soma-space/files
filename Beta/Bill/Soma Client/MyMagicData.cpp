// MyMagicData.cpp: implementation of the CMyMagicData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyMagicData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyMagicData::CMyMagicData()
{
	Init();
}

CMyMagicData::~CMyMagicData()
{

}

void CMyMagicData::Init()
{
	m_vNumber = -1;
	m_vName =_T("");
	m_vType = 0;
	m_vMethod = 0;
	m_vUseTime = 0;
	m_vDamage = 0;
	m_vGamePic = -1;
}
