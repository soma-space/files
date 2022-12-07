// ArmatureRes.h: interface for the CArmatureRes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARMATURERES_H__A1428FE9_62AB_11D3_8424_00105A6B96E7__INCLUDED_)
#define AFX_ARMATURERES_H__A1428FE9_62AB_11D3_8424_00105A6B96E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "illstruct.h"

#ifndef _SPR_DATA
#define _SPR_DATA
class SPR_DATA
{
public:
	_ANI_HEADER		m_HAni;
	_ANI_DATA		m_AniData;
	_SPR_HEADER		m_HSpr;
	_SPR_LSP		*m_arrSprData;
	WORD			**m_arrVData;

public:
	SPR_DATA();
	~SPR_DATA();
};
#endif

class CArmature;

class CArmatureRes  
{
public:
	CArmatureRes();
	virtual ~CArmatureRes();

	SPR_DATA			*GetSprData(int ani_number);
	BOOL				SetArmatureInfo(CString szArmatureFileName, int index, int type);
	BOOL				LoadArmature(CString szArmatureFileName, int Null_index = 1, int Null_type = 1);
	BOOL				LoadElement(int ani_index);

	friend	class		CArmature;

protected:
	int					m_vIndex;
	CString				m_szFileName;				//확장자를 포함하지 않음 (.ani, .spx)를 읽어야 함
	BOOL				m_bLoad[MAX_RES_COUNT];
	SPR_DATA			*m_ArmRes[MAX_RES_COUNT];
};

#endif // !defined(AFX_ARMATURERES_H__A1428FE9_62AB_11D3_8424_00105A6B96E7__INCLUDED_)
