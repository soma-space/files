// Char.h: interface for the CCharRes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHAR_H__529038C1_36AA_11D3_8422_00105A6B97E2__INCLUDED_)
#define AFX_CHAR_H__529038C1_36AA_11D3_8422_00105A6B97E2__INCLUDED_

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

class CCharRes  
{
public:
	SPR_DATA			*GetSprData(int ani_number);
	
	void				SetIndex(int index) {m_vIndex = index;};
	static	void		Decode2_555(int nLine, WORD *data);
	static  void		Decode2_565_0(int nLine, WORD *data); //565로 되어있는 데이터를 555 카드에 맞도록 색 보정 (LSP포맷)

	BOOL				Load(LPCSTR	szFileName, int pf, int index);
	BOOL				LoadElement(int ani_index);
	void				RemoveAllData();

	CCharRes();
	virtual ~CCharRes();

protected:
	int					m_vIndex;
	BOOL				m_bLoad[MAX_RES_COUNT];
	SPR_DATA			*m_CharRes[MAX_RES_COUNT];
};

#endif // !defined(AFX_CHAR_H__529038C1_36AA_11D3_8422_00105A6B97E2__INCLUDED_)
