// CharManager.h: interface for the CCharManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHARMANAGER_H__5A5901E1_3853_11D3_8422_00105A6B97E2__INCLUDED_)
#define AFX_CHARMANAGER_H__5A5901E1_3853_11D3_8422_00105A6B97E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDDSurface;
#include "CharRes.h"

class CHAR_RES
{
public:
	CHAR_RES();
	~CHAR_RES();

public:
	CCharRes	m_CharRes;
	int			m_RefCount;
};

typedef CMap<int, int, CHAR_RES *, CHAR_RES *> MAP_CHARRES;

//캐릭터의 resource를 관리하는 클래스
class CCharManager  
{
public:
	int					GetPixelFormat() {return m_PixelFormat;}	//16bitmode에서의 pixel format : 555 565 bit(in RGB)
	CCharRes*			GetIndexedCharRes(int index);				//Resource Index를 이용한 캐릭터 resouce가져오기
	void				Release(int index);		//공통으로 쓰이는 항목에 대한 보호 Index (다쓰면 index감소)
	void				AddRef(int index);		//공통으로 쓰이는 항목에 대한 보호 Index (사용할때 Index증가)
	void				UnloadCharRes(int index); //보호 Index가 Zero가 될때 Resource 제거
	void				SetPixelFormat(CDDSurface*	pSurface);
	void				SetPixelFormat(int pf) {m_PixelFormat = pf;}
	void				LoadAllCharRes();
	BOOL				LoadCharRes(int index);	//캐릭터의 Resource Load
	void				RemoveAllData();

	CCharManager();
	virtual ~CCharManager();

protected:
	MAP_CHARRES			m_mapCharRes; 
	int					m_PixelFormat;
};

#endif // !defined(AFX_CHARMANAGER_H__5A5901E1_3853_11D3_8422_00105A6B97E2__INCLUDED_)
