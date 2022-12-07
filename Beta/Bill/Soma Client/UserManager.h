// UserManager.h: interface for the CUserManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERMANAGER_H__A8E67521_39D1_11D3_8422_00105A6B97E2__INCLUDED_)
#define AFX_USERMANAGER_H__A8E67521_39D1_11D3_8422_00105A6B97E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class NODE;
class CPathFind;
class CDDSurface;
class SM_MapManager;
class CUser;
class CMagic;
class CMagicComet;
class CBlood;
class CArrowManager;

typedef CMap<int, int, CUser *, CUser *> MAP_USER;
typedef CTypedPtrArray<CPtrArray, CUser*> ARR_USER;
typedef CTypedPtrList<CPtrList, CMagic*> LST_MAGIC;

#define	UM_UID_INVALID		-1

#define BEATEDMOTION         1
#define BEATEDSOUND          2
#define BEATEDEFFECT         3
#define ATTACKMOTION         4
#define ATTACKSOUND          5
#define ATTACKEFFECT         6
#define ATTACKFAIL           7

//////////////////////////////////////////////////////////////////////////////////////////////////
// 마법 테이블로 데이터를 읽을 구조체
typedef struct _TAG_MAGIC_TABLE
{
	int		m_nTableNum		; //테이블 정보 순서
//	CString m_strMagicName	; //마법이름
	int		m_nAttackType	; //공격유형
							  //1)캐릭터에 효과 2)발사되어 목표물로 날아가는 마법 3)캐릭터에 효과 후 지속되는 마법

	short   m_vMethod		; //패킷종류
							  //1)PKT_ATTACK_ARROW 2)PKT_ATTACK_MAGIC_RAIL 3)PKT_MAGIC_CIRCLE	
	int		m_nDes			; //마법을 줄 대상 유형
							  //1)자신 2)자신/타인 3)타인
	int		m_nStPreMagic	; //캐릭터 뒤쪽에 그려질 그림
	int		m_nStPostMagic	; //캐릭터 앞쪽에 그려질 그림. 0이면 파일이 없음..
	int		m_nStTime		; //날아가는 매직이 생길 시간....

	int		m_nFlyMagic		; //날아가는 매직...
	int		m_nEffecKind	; 
							  
	int		m_nFireNum		; //날라가는 갯수

	int		m_nEndPreMagic	; //대상 캐릭터 뒤쪽에 그려질 그림
	int		m_nEndPostMagic	; //대상 캐릭터 앞쪽에 그려질 그림
//	int		m_nEndTime		;

//	int		m_nConPreMagic	;
//	int		m_nConPostMagic	;
	int		m_nConType		; //지속되는 매직의 타입...
	int		m_nConTime		;
} _MAGIC_TABLE;

//////////////////////////////////////////////////////////////////////////////////////////////////
// 마법 설정에 대한 정보를 리스트로 관리할 구조체
typedef struct _TAG_SET_MAGIC_INFO
{
	_MAGIC_TABLE		m_stMagicTable;
	int					m_nSorUid	  ;
	CArray<int , int>	m_arrDestUid  ;
	DWORD				m_dwNowTime   ;
	BOOL				m_bReady	  ; //공격준비가 필요한 마법인가? 
	int 				m_nSIndex	  ; //패킷을 보냈는가?
	int					m_nDir		  ; //마법적용방향 100이면 방향에 적용되지 않음
	CMagicComet*		m_Comet		  ; //꼬리가 다린 
} _SET_MAGIC_INFO;

typedef struct _TAG_ACTION_INFO
{
	int  m_ActionId;
	int  m_ActionType;
	DWORD m_ActionTickTime;
} ACTION_INFO;

typedef struct _TAG_DEAD_INFO
{
	DWORD	m_dwDeadTime;	//죽은 시간
	CPoint	m_ptDeadPos;	//죽은 위치
	int		m_nDeadUserId;	//아이디
} DEAD_INFO;

typedef CTypedPtrArray<CPtrArray, ACTION_INFO *> ARR_ACTION_INFO;

class CUserManager  //유저 관리자
{
public:
	void Set3DMagic(int uid, TCHAR* strName, int nMagicType, int nHeight=0, int nWidth=0);//dspring_0731_3dxx

	void ChangItem(int uid, short fromindex, short toindex);// 따른 캐릭터가 장착을 바꾸었을때..물론 본인도 가능..
	void ChangItem(short fromindex, short toindex);// 장착 인벤토리에서 가방 인벤토리로..또는 반대..
	BOOL SendChang_Item(short fromindex, short toindex);
	void Set_MyInv_Data_All(int uid, PBYTE bypData);


	BOOL MagicChar( _SET_MAGIC_INFO* stSetMagicInfo );
	void ApplyMagicEx();
	void ApplyConMagic();
	void ApplyConMagic( int t_index, CUser* pUser );
	void UserAppliedMagic( int uid, BYTE num, short* magic_index, DWORD* dwConTime );
	void ReadMagicTable(int bSuccess, int iSorUid, short Mp, DWORD exp, int *iDestUid,short *pHP,int nDstUidNum, int magicindex, DWORD dwConTime, int Dir = 100);
	void ReadMagicTable(int bSuccess, int iSorUid, int nDestUid, int magicindex, DWORD dwStartTime, int Dir = 100);
	void MagicTableInit();
	void DeleteMagic(int iDestUid);
	void PoisonAffect(int nDesUid,short HP);
	void CharStateChange( int nDestUid, DWORD dwStatus );
	void MagicAttackKind( _SET_MAGIC_INFO* stSetMagicInfo, int nSorUid, int* pDesUid, int nDestNum );
	void SetMagicComet(  _SET_MAGIC_INFO* stSetMagicInfo, int nSorUid, int* pDesUid, int nDestNum );

	BOOL FlyMagicChar( _SET_MAGIC_INFO* stSetMagicInfo );
	BOOL ConMagicChar( _SET_MAGIC_INFO* stSetMagicInfo );
	BOOL FlyMagicComet( _SET_MAGIC_INFO* stSetMagicInfo );
	void Set3DMagicEx(int uid, int index, int nMagicType, int nHeight = 0, int nWidth = 0);
	int	 SetAimed3DMagicEx(CPoint ptSor, CPoint ptDst, int index, int nMagicType, int euid, int nSeries = 0);
	void Attack3DMagicEx(int suid, int euid, int index, int nSeries = 0);
	void Cyclone(int suid, int index);
	void AttackDir3DMagicEx(int suid, int nDir, int index, int nDestUid);
	int  SetDirAimed3DMagicEx(CPoint ptSor, int index, int nMagicType, int nDir, int nDestUid);

//void AttackDir3DMagicEx(int suid, int nDir, int index);
//int SetDirAimed3DMagicEx(CPoint ptSor, int index, int nMagicType, int nDir);
	
	//마법관련 처리함수 (uid를 가진 유저에게 nIndex의 마법을 걸어줌)
	int					SetAimedMagic(int nSorUid, int nDstUid, int nMagicIndex, int nMagicType);
	int					SetAimedMagic(CPoint ptSor, CPoint ptnDstUid, int nMagicIndex, int nMagicType, int nDesUid = -1,int nSeries = 1);
	int					SetAimed3DMagic(CPoint ptSor, CPoint ptDst, TCHAR* strName, int nMagicType, int euid);//dspring_0802_3dxx
	void				DrawDownMagics(CDDSurface *pSurface);	//바닥에 깔리는 마법 그리기
	void				DrawUpMagics(CDDSurface *pSurface);		//날으는 마법 그리기
	void				DrawUp3DMagics();//dspring_0802_3dxx

	void				SetMagic(int uid, int nIndex, int nMagicType, int height = 0);	//캐릭터에 단위 마법 검
	void				RailAttackMagic(int bSuccess, int iSorUid, int bDir, int bLen, int nMagicType);
	int					SetViewMagic(int uid, int nMagicIndex, int nMagicType, int iDir, int iLen);
	//공격관련 처리함수
	BOOL				SendAttack(int type = 0);							//공격을 함 (Focus가 있는 User를 대상으로함)
	BOOL				SendReadyMagic(int type);
	BOOL				SendMagicAttack(int type, short method, int nDestUid = -1, BYTE dir = 100);
	void				AttackChar(BYTE bSuccess, int iSorUid, CPoint ptSorPos, int iDestUid, CPoint ptDestPos);	//화면표시(Source가 Destination공격)
	void                AttackMagic(int suid, int euid, int magic_type,int nSeries = 1);// 날라 가는 매닉을 버여준다..음냐..위함수에 포함..
	void				Attack3DMagic(int suid, int euid, TCHAR* strName);


	//이동관련 패킷처리 함수
	void				SetMoveFirst(CUser *pUser, CPoint ptDst, CPoint ptFirst);	//서버에서 온 응답처리(Move First)
	void				SetMoveMid(CUser *pUser, CPoint ptDst);					//			"		  (Move Middle)
	void				SetMoveEnd(CUser *pUser, CPoint ptDst);					//			"		  (Move End)

	void				SetRunMoveFirst(CUser *pUser, CPoint ptDst, CPoint ptFirst, CPoint ptNext);	//서버에서 온 응답처리(Move First)
	void				SetRunMoveMid(CUser *pUser, CPoint ptDst, CPoint ptNext);					//			"		  (Move Middle)
	void				SetRunMoveEnd(CUser *pUser, CPoint ptDst, CPoint ptNext);					//			"		  (Move End)

	void				ModifyUser(int uid, BYTE bDeadEnable, CString t_name, CString t_guild, int picnum, short moral, BYTE live, BYTE battle_mode, int x, int y, short item[10], BOOL bIsI=FALSE, short hair = 101 , short skin = 0);		//사용자 정보갱신 (있으면 갱신, 없으면 추가)
	void				DeleteUser(int uid, int x, int y);						//사용자 정보 없애기

	BOOL				IsMoveable(CPoint pt);									//이동가능 위치인가?(맵상에서)
	void				SetMoveableTable(SM_MapManager* pMapManager);			//PathFinder에서 사용할 Map Table 생성및 설정 (PathFinder사용하지 않고 있음)
	BOOL				IsInRange(CRect r, CUser *pUser, CPoint ptPos);			//Rect에 유저가 포함되는가(화면에 유저?) ptPos는 맵의 그리기 Offset
	CPoint				GetMyPos();												//나의 위치 가져오기(Pixel)
	void				Move();													//유저들 이동처리
	static	int			compare(const void *arg1, const void *arg2);			//화면 그리기 위해 User Quick Sort 비교함수
	void				DrawUserSortY(int count);								//그릴 User를 Sorting함(Y축에 대해서)
	void				RenderUsers(CDDSurface *pSurface, CPoint ptMapOffset);	//유저들 그리기
	void				InHouseRenderUsers(CDDSurface *pSurface, CPoint ptMapOffset);
	//void				OutHouseDrawObj(CDDSurface *pSurface, CPoint ptMapOffset);

	CUser*				GetUserWithPos(CPoint ptPixelPos, CPoint ptMapOffset);	//지정한 위치에 있는 유저는 누구?(Pixel단위)
	CUser*				GetUserWithCellPos(CPoint ptCellPos);					//				"				(Cell단위)
	CUser*				GetUserWithUid(int uid);								//User Id를 이용한 User가져오기
	int					GetUserNum() {return m_mapUser.GetCount();}				// 몇명?
	CUser*				GetMe();												//내정보 가져오기
	int					GetFocusedUidInOthers() {return m_iCurFocusUidInOthers;}//Focus를 가진사람 누구? (나 제외)
	void				RemoveUser(int index);									//지정한 Index의 유저 제거
	void				SetMyUserId(int iUid){m_iMyUserId = iUid;}				//나의 유저 Id설정
	BYTE				GetDirection(CPoint ptMyPos, CPoint ptDstPos);
	int					GetClientDirection(CPoint ptStart, CPoint ptEnd);		//클라이언트용 방향

	void				RemoveAllData();
	void				RemoveAllDataWithoutMe();
	void                HpMpRecovery(PBYTE bypData);
	
	CUserManager();
	virtual ~CUserManager();

//protected:
public:
	BOOL m_bMyCovered;
	int  m_nCoverDegree;
	int m_ReadyOK;
	// 딜레이를 주면서 동작시키는 부분들.
	void ActionProcess();
	void ActionMotion(int uid, int type, int delay_time);
	int m_ActionCount;
	int  m_ActionId[50];// 임의대로 50개를 잡았다. 시야에서 동작하는 동작들이 이정도면 넉넉할꺼 같아서..
	int  m_ActionType[50];
	DWORD m_ActionTickTime[50];




	CPoint GetMyCellPos();
	void				UserDead(int uid, int  x, int y);
	void				PreUserDead(int uid, int x, int y);
	void				SetUserDead();

	int					GetMapData(CPoint pt);
	int					GetHouseMapData(CPoint pt);
	int					m_iMyUserId;		//자신의 UserId
	int					m_iCurFocusUidInOthers; //Focus누구? (나빼고) Index

	CUser*				m_pMe;
	CUser*				m_arrDrawUser[500];	//데이터 보관용이 아님(그리기위한용) (한화면에 필요한 갯수: 300(20*15))
	CUser*				m_arrDrawDeadUser[1000];	//데이터 보관용이 아님(그리기위한용)
	MAP_USER			m_mapUser;			// 유저 데이터 보관 장소
	BOOL				m_bCursorInRange;	//현재의 마우스 좌표가 유저위치에 있는지 검사.
	CBlood*				m_Blood;
	CArrowManager*		m_Arrow;

	LST_MAGIC			m_lstMagic;	//개인에게 걸리는 마법이 아니라 여러대상에 걸리는 마법
	CMagicComet*		m_Comet;	
	CPathFind*			m_pPathFind;//거의 사용하지 않음(맵속성 참조함)
	NODE*				m_pNode;	//사용하지 않음
	CArray< _SET_MAGIC_INFO*, _SET_MAGIC_INFO* > m_arrSetMagicInfo;
	CArray< _MAGIC_TABLE*	, _MAGIC_TABLE*	   > m_arrMagicTableInfo;
	CArray< DEAD_INFO       , DEAD_INFO        > m_arrDeadUserInfo;
	ARR_ACTION_INFO		m_arrActionInfo;
};

#endif // !defined(AFX_USERMANAGER_H__A8E67521_39D1_11D3_8422_00105A6B97E2__INCLUDED_)
