// AniObj.h: interface for the CAniObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIOBJ_H__65C6CB21_84CF_11D3_9925_00C026EE03FC__INCLUDED_)
#define AFX_ANIOBJ_H__65C6CB21_84CF_11D3_9925_00C026EE03FC__INCLUDED_

#include "stdafx.h"

class CAniObj // 기본적으로 8 방향을 지원하는 에니메이션 오브젝트
{
public:
	_ANI_HEADER			m_HAni;

protected:
	_ANI_DATA*			m_pAnis[MAX_ANI];

	int		m_nCtrlQt; // 에니메이션 제어 수
	int*	m_nCurAnis; // 현재 지정된 에니메이션
	int*	m_nDefaultAnis; // 기본 에니메이션.. 마지막에 정해진 루프 에니메이션이 된다..
	DWORD*	m_dwTickFrms; // 에니메이션..
	int*	m_nCurFrms; // 현재 지정된 에니메이션의 프레임.
	int*	m_nCurDirs; // 현재 지정된 방향
	int*	m_nFlags; // 에니메이션 컨트롤 플래그   FL_LOOP, FL_ONCE, FL_ONCE_FIX;

	float	m_fASpeed; // 액션 스피드..

public:
	int		FrmGetAnimated(int nCtrl = 0);
	float	ActionSpeedSet(float fSpeed);

	void	AniAlloc(int nQt);
	void	AniCtrlAlloc(int nQt); // 에니메이션 객체 수 설정
	void	AniRelease();
	void	AniCtrlRelease();

	int		AniGet(int nCtrl = 0) { if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return 0; return m_nCurAnis[nCtrl]; }
	void	AniSet(int nAni, int nFlag = FL_ANI_LOOP, int nCtrl = 0);
	void	DirSet(int nDir, int nCtrl = 0);
	int		DirGet(int nCtrl = 0) { if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return 0; return m_nCurDirs[nCtrl]; }

	int		FrmGet(int nCtrl = 0) { if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return 0; return m_nCurFrms[nCtrl]; }
	void	FrmSet(int nFrm, int nCtrl = 0);
	int		FrmGetByAniAndDirAndSeq(int nAni, int nDir, int nFrm); // 현재 에니메이션 방향과 번호에 따라 프레임 번호를 넘겨 준다.

	int		FrmGetMax(int nCtrl = 0);
	
	_ANI_DATA*	AniDataGet(int index);

	void		AniLoad(const char* szFileName); // Pixel Format - 555 or 565
	void		AniLoadEx(const char* szFileName, BOOL bPart = FALSE);//this_obj_load
	void		AniCtrlAllocEx(int nQt, BOOL bPart = FALSE);


	
	CAniObj();
	virtual ~CAniObj();
};

#endif // !defined(AFX_ANIOBJ_H__65C6CB21_84CF_11D3_9925_00C026EE03FC__INCLUDED_)
