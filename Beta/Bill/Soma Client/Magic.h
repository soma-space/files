// Magic.h: interface for the CMagic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAGIC_H__652BC4ED_5F86_11D3_8424_00105A6B96E7__INCLUDED_)
#define AFX_MAGIC_H__652BC4ED_5F86_11D3_8424_00105A6B96E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMagicManager;
class CMagicRes;
class CDDSurface;

#define MAGIC_TYPE_UP		0
#define MAGIC_TYPE_DOWN		1
#define MAGIC_TYPE_POSION   2 // 임의의 위치에 나타나게 한다.
#define MAGIC_TYPE_LOOP     3 // 루프를 도는 마법.
#define MAGIC_TYPE_NOTALPHA_UP		4
#define MAGIC_TYPE_NOTALPHA_DOWN	5



typedef struct
{
	CPoint	m_ptPos;	//날아가는 마법일 경우의 좌표
	int		m_nDir;		//	"                    방향
} MR_CELL;	//Magic Routing cell

typedef struct
{
	int		m_nMagicIndex;	//마법의 인덱스
	int		m_nCurFrame;	//마법이 그려지는 현재 프레임
	int		m_nFrame;		//마법 리소스의 총 프레임
	int		m_nMagicType;	//마법의 타입
	DWORD	m_dwLastTick;	
	BOOL	m_bShowContinue;//마법을 반복해서 다시 그릴것인지.....
	int		m_nHeight;
} MAGIC_INFO;//dspring_0720	

typedef struct
{
	CString	m_strName;
	int		m_nMagicType;
	BOOL	m_bShowContinue;
	int		m_nWidth;
	int		m_nHeight;
	DWORD	m_dwTime;
	int		m_nCurFrame;
	int		m_nFrame;
} MAGIC3D_INFO;//dspring_3dxx	

class CMagic  
{
public:
	//*********************************************
	//자신의 User객체에 마법을 경우 사용하는 함수들
	//*********************************************
	//마지막 Frame을 그린경우  True 를 return함
	BOOL					DrawMagic(CDDSurface *pSurface);
	BOOL					DrawMagic(CDDSurface *pSurface, int x, int y, int Alpha = 1, BOOL bNext = FALSE);//dspring_0721
	BOOL					DrawFlying3DMagic();//dspring_0802_3dxx
	BOOL					DrawFlying3DMagic(int nDir, int x, int y);//dspring_0802_3dxx
//	BOOL					DrawMagic(CDDSurface *pSurface, int x, int y, int Alpha = 1);//soma_0721
	//BOOL					SetMagicRes(int nIndex);//soma_0720
	BOOL					SetMagicRes(int nIndex,int MagicType=0);//dspring_0720
	void					SetMagicInfo( int index , int MagicType );//dspring_0720
	BOOL					SetFly3DMagicRes(TCHAR* strName, CPoint startPos, CPoint EndPos, int &nFirstDir, int nMagicType);//dspring_0802_3dxx
	BOOL					SetFly3DMagicResEx(int index, CPoint startPos, CPoint EndPos, int &nFirstDir, int nMagicType, int nSeries = 0); //좌표는 맵상의 좌표
	void					Set3DMagicEx(int index, int nMagicType, int height = 0, int nWidth=0);//dspring_0802_3dxx
	void					Set3DMagic(TCHAR* strName, int nMagicType, int height = 0, int nWidth=0);//dspring_0802_3dxx
	BOOL					Draw3DMagic( int x, int y, BOOL bNext = FALSE);//dspring_0802_3dxx

	int						GetMagicAttackDesUid(){ return m_nDstUid;}//dspring_0722
	void					SetMagicAttackDesUid( int uid ){ m_nDstUid = uid;}//dspring_0722
	//*******************************************************
	//전체의 범위에 적용되는 마법을 그릴 경우 사용하는 함수들
	//*******************************************************
	BOOL					SetDirFly3DMagicResEx(int index, CPoint startPos, int nDir, int nMagicType); //좌표는 맵상의 좌표
	void					RouteDirPositon(CPoint ptStart, int nDir);
	BOOL					SetCycloneMagicRes(int index, CPoint ptSPos, int nMagicType, int nSRadians );
	void					RouteCyclone( CPoint ptSPos, int nSRadians );

	//다그린경우 TRUE return
	BOOL					DrawFlyingMagic(CDDSurface *pSurface);
	//Cell이동이 되면 TRUE return
	BOOL					DrawFlyingMagic(CDDSurface *pSurface, int nDir, int x, int y); //위의 함수에서 사용
	int						DirStartFrame(int nDir, int nResMaxFrame);
	BOOL					SetFlyMagicRes(int nIndex, CPoint ptStart, CPoint ptEnd, int &nFirstDir,int nSeries);	//마법Resource Load및 초기정보 설정
	void					RoutePositon(CPoint ptStart, CPoint ptEnd, int nSeries = 0);	//날아가는 마법 궤적생성(현재:직선)
	BOOL					IsFlyingMagic() {return m_bTypeFlying;}		

	BOOL					IsDownMagic() {return (BOOL)(m_nType==MAGIC_TYPE_DOWN);}	//아래 깔리는 마법인가?
	void					SetMagicType(int nType) {m_nType = nType;}					//마법 Type설정
	int                     GetMagicType() { return m_nType; }

	void					SetLastBeatEffect(int nDstUid);//마법 끝난후 맞는 효과 보여줄 사람 선택
	CMagic();
	virtual ~CMagic();

	static	CMagicManager*	pMagicManager;					//마법 Resource관리자 backup ptr(CChildView::MagicManager)
	static	DWORD			dwMagicMoveDelay;				//날아가는 속도 Delay

public:
	CPoint                  m_Pos;// 
	int                     m_nsUid;//마법을 사용한 사람 아디..
	int                     m_nDir;
	int                     m_nCount;
	int                     m_height;
	CArray<MAGIC_INFO* ,MAGIC_INFO*> m_arrMagicInfo;//dspring_0720마법정보를 가지고 있는 array

protected:
	int						m_nDstUid;
	int						m_nType;	//마법의 방향성 (아래 깔리는 마법인가 , 위에 덮는 마법인가?);
	BOOL					m_bTypeFlying;
	DWORD					m_dwLastMoveTick;	//마법의 이동속도를 점검하는 시간
	int						m_nCurDir; //날아가는 마법일 경우 날아가는 방향
	int						m_nFrame;	//마법의 Frame수(전체 몇 Frame)
	int						m_nCurFrame;	//현재 그리는 과정의 Frame
	CMagicRes*				m_bpMagicRes;	//마법 Resource의 참조 포인터(리소스는 MagicManager에 있음)
	DWORD					m_dwLastTick;	//마법의 Frame변화를 점검
	int						m_nResIndex;	//MagicManager에서의 Resource Index
	
	CArray<MAGIC3D_INFO*, MAGIC3D_INFO*>	m_arr3DMagicInfo;//3dxx
	CList<MR_CELL, MR_CELL>	m_lstMagicRoutingCell;	//궤적을 담고있을 곳(날아가는 마법 일 경우)
};

#endif // !defined(AFX_MAGIC_H__652BC4ED_5F86_11D3_8424_00105A6B96E7__INCLUDED_)
