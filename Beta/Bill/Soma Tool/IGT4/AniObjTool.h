// AniObjEx2.h: interface for the CAniObj2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIOBJ2_H__A17D6FE0_3883_11D3_9559_00C026EE03FC__INCLUDED_)
#define AFX_ANIOBJ2_H__A17D6FE0_3883_11D3_9559_00C026EE03FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../IGObject/AniObjLSP.h"

class CAniObjTool : public CAniObjLSP
{
public:
	LPDIRECTDRAWSURFACE m_lpDDS;		// 스프라이트 작업용 서피스..
	RGBQUAD* m_pRGBTbls; // 8비트 컬러 인덱스 테이블

protected:
	int		m_nSprWorkQt;	// 작업중인 스프라이트 갯수.
	_SPR_TOOL m_SprExs[MAX_SPR]; // 툴용 확장 스프라이트는 따로 써야 한다.

public:
	void AniDirQtSet(int nDirQt);
	void	AniLoadOldFormat(const char* szFileName);
	void	SprLoadOldFormat(const char* szFileName);
	void	SprLoad(const char* szFileName, int nPF);
	void	SprSave(const char* szFileName);
	void	SprAlloc(int nQt); // 임의대로 마구 배정.. 툴이니까 가능하다요..
	void	AniSave(const char* szFileName);
	void	GetStandLSP(RECT* prc, const WORD** pwStand);
	void	SetColorKey(COLORREF ColorKey);
	int		HitTest(CPoint point, _SPR_TOOL** ppSprEx);
	
	void	AniNew();
	void	SprNew();

	BOOL	AniAdd(); // 끝에 추가
	BOOL	AniInsert(int index); // 중간에 삽입
	BOOL	AniDelete(int index); // 중간에 삭제.
	
	void	SprExGetData(int index, _SPR_TOOL* pSprData);
	void	SprExSetData(int index, _SPR_TOOL* pSprData);

	CAniObjTool();
	virtual ~CAniObjTool();
};

#endif // !defined(AFX_ANIOBJ2_H__A17D6FE0_3883_11D3_9559_00C026EE03FC__INCLUDED_)
