// ObjBasic.h: interface for the CObjBasic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJBASIC_H__A9A3B8B4_EE85_4846_8CC5_F833731E75A1__INCLUDED_)
#define AFX_OBJBASIC_H__A9A3B8B4_EE85_4846_8CC5_F833731E75A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "AniObjLSP.h"

class CDDEngineLSP;

class CObjBasic  
{
protected:
	BOOL		m_bReferenceAniObj;
	POINT		m_ptCur;
	int			m_nAniCtrl;
	int			m_nDir; 
	CAniObjLSP* m_pAniObjs[MAX_OBJ_ANI]; // 참조값으로 갖고 있는 경우는 날리지 않는다.

public:
	_OBJ_HEADER m_Header;

protected:
	void	ReleaseAniObjs();

public:
	void	Render(CDDEngineLSP* pEng, POINT ptScroll);
	BOOL	HasReferenceAniObj() { return m_bReferenceAniObj; }
	void 	SetReferenceAniObj(BOOL bReferenceAniObj);

	void	Load(const char* szFileName, const char* szAniPath, const char* szSprPath, int nAniCtrlQt, int nPF);
	void	Release();
	POINT	PosGetCur() { return m_ptCur; }
	void	PosSetCur(POINT pt) { m_ptCur = pt; }
	
	int		DirGet() { return m_nDir; }
	void	DirSet(int nDir) { if(nDir < 0 || nDir >= 8) return; m_nDir = nDir; }

	CAniObjLSP* AniObjGet(int index) { if(index < 0 || index >= MAX_OBJ_ANI) return NULL; return m_pAniObjs[index]; }
	void	AniObjSet(int index, int nAniCtrl, CAniObjLSP* pAniObj);

	CObjBasic();
	virtual ~CObjBasic();
};

#endif // !defined(AFX_OBJBASIC_H__A9A3B8B4_EE85_4846_8CC5_F833731E75A1__INCLUDED_)
