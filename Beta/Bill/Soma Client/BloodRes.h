// BloodRes.h: interface for the CBloodRes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOODRES_H__CD4EDB82_B6FF_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_BLOODRES_H__CD4EDB82_B6FF_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "illstruct.h"

typedef CTypedPtrArray<CPtrArray, PWORD>			ARR_WORDDATA;
typedef CTypedPtrArray<CPtrArray, _SPR_LSP*>		ARR_SPRDATA;

/*
0:없음 시크젠, 신하린, 신하젠
1:작은 캐릭터 적색 코타고 시타
2:인간형캐릭터 적색 그리젠 오크1 오크2 마법사 신하탄 시타젠 NPC
3:큰 캐릭터 적색 도마뱀 카르카 파르카 
4:작은 캐릭터 스타지 청색 
5:인간형 캐릭터 청색
6:큰 캐릭터 청색 거대거미 
*/
class CDDSurface;
class CBloodRes  
{
public:
	int					m_RefCount;
	_SPR_HEADER			m_HSpr;						//CharRes와 동일한 구조(CharRes참고할것)
	ARR_WORDDATA		m_arrFxData;
	ARR_SPRDATA			m_arrSprData;

public:
	CBloodRes();
	virtual ~CBloodRes();
	void AddRef();
	int  Release();
	BOOL Draw(CDDSurface* pSurface,CPoint ptPos,int nCurFrame, BOOL bNext);
	BOOL LoadRes(int nBloodKind);
	BOOL LoadRes(CString szFileName);


};

#endif // !defined(AFX_BLOODRES_H__CD4EDB82_B6FF_11D4_956A_00D0B77259E8__INCLUDED_)
