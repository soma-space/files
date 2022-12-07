// User.h: interface for the CUser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USER_H__5A5901E5_3853_11D3_8422_00105A6B97E2__INCLUDED_)
#define AFX_USER_H__5A5901E5_3853_11D3_8422_00105A6B97E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "illstruct.h"

#include "MyInvData.h"
#include "MyMagicData.h"

class DirectXMng;
class CCharManager;
class CCharRes;
class CDDSurface;
class CUserManager;
class CMagic;
class CArmatureManager;
class CArmature;

//class CMyInvData;

#define MOVEPOS_KIND_NEXT	0
#define MOVEPOS_KIND_END	1

#define AM_KIND_NUM		7

//0:맨손,너클, 용조권 1:한손검 2:양손검 3: 창 4:활 5:도끼,해머, 전부 6:지팡이 7: 석궁 8:쌍칼(여자)
#define NOTHINGARM      0
#define SINGLESWORD     1
#define BIGSWORD        2 // 양손검
#define SPEAR           3 // 활
#define BOW             4
#define AX              5
#define WAND            6 //마법사 지팡이
#define CROSSBOW        7
#define DOUBLESWORD     8 //쌍칼

typedef struct
{
	int kind;
	CPoint ptPos;
} MOVEPOS;		//Move를 요청할 Routing Data보관장소

typedef struct _TAG_FLY_END_MAGIC
{
	int		m_nMagicIndex	;//마법 인덱스
	BOOL	m_bType			;//마법 타입
	int		m_nPoisonKind	;//독마법 종류
	int		m_nTableNum		;//마법 테이블 번호
} FLY_END_MAGIC;

typedef struct _TAG_APPLIED_CON_MAGIC
{
	DWORD		m_dwContime	;//지속시간
	short		m_nTableNum	;//마법 테이블 번호
} APPLIED_CON_MAGIC;

#define		SPECIAL_MAGIC		0
#define		BLUE_MAGIC			1
#define		WHITE_MAGIC			2
#define		BLACK_MAGIC			3

typedef CTypedPtrArray<CPtrArray, CMyMagicData *> CMyMagicDataArray;
typedef CMap<int, int, CMyMagicData *, CMyMagicData *> CMyMagicDataMap;

#define	INV_COUNT				50
#define BELT_COUNT				4

//유저정보 처리 클래스(한명의 User에 해당)
class CUser  
{
public:
	short GetInv_ItemUsage(int index);
	int GetInv_ItemIndex(int index);
	int GetInv_ItemType(int index);
	int GetInv_ItemPic(int index);
	int GetInv_ItemArm(int index);
	CString GetInv_ItemName(int index);

	int GetJangchk_Item(int index);
	void Change_View_item(short index, short picnum);
	void Chang_Item(short fromindex, short toindex);
	void SetInvAll(PBYTE bypData);
	void			SetWearItem(int nType, int nIndex, int btype = -1, int barm = -1);
	void ChangeItemInfo(BYTE bBelt, BYTE nSpec, CMyInvData& inv_data);

	void			SetMagic(int nIndex, int nMagicType, int height =0);	//단위객체에 걸리는 마법 (날아가는 마법은 UserManager에있음)
	void			Set3DMagic(TCHAR* strName, int nMagicType, int nHeight=0, int nWidth=0);//dspring_0731_3dxx
	void			Set3DMagicEx(int index, int nMagicType, int nHeight=0, int nWidth=0);//dspring_0731_3dxx

	void			SendNextMoveReq(CPoint ptPosAllowed);	//다음 이동점 요청
	void			SetMoveFirst(CPoint ptDst, CPoint ptFirst);	//최초의 움직임 허가, 실패
	void			SetMoveMiddle(CPoint ptDst);					//다음점 처리
	void			SetMoveEnd(CPoint ptDst);					//최종점 움직임의 결과

	void			SendNextRunMoveReq(CPoint ptPosAllowed, CPoint nextpt);	//다음 이동점 요청
	void			SetRunMoveFirst(CPoint ptDst, CPoint ptFirstv, CPoint ptNext);	//최초의 움직임 허가, 실패
	void			SetRunMoveMiddle(CPoint ptDst, CPoint ptNext);					//다음점 처리
	void			SetRunMoveEnd(CPoint ptDst, CPoint ptNext);					//최종점 움직임의 결과

	void			SetMoveList(CList<CPoint, CPoint>* pRoute);					//이동 요청 목록
	static CList<CPoint, CPoint>* CalcMoveList(CPoint ptStart, CPoint ptEnd, int Direction = 0);	//이동 경로 탐색
	//우석.. 10월 11일..
	CList<CPoint, CPoint>* CalcMoveList2(CPoint ptStart, CPoint ptEnd, int FindStep);
	int				GetUserId(){return m_iUserId;}								//유저 Id(서버와 동기화)
	void			SetUserId(int id) {m_iUserId = id;}							//	"

	int             m_MeDraw_Order[AM_KIND_NUM]; //우석..

	int             m_AniSprNumber;
	BOOL			m_bDying;
	BOOL			m_bDeadFlag;
	int				m_bDead;
	int				m_vMoveStep;
	enum UserMotion //다음의 정의는 lgt3의 animation항목의 처리와 동일(Animation Index)
	{
		UM_Stop = 0, 
		UM_Walk, 
		UM_Attack,// 내가 새롭게 설정..할꺼..
		UM_Beated,
		UM_Magic,
		UM_Die,
		UM_RUN,


/*		UM_StopInAttack,
		UM_WalkInAttack,
		UM_Run,
		UM_AttackSecond,
		
		UM_Push,
		UM_Dash,
		UM_Defence,
		UM_Ko,
		

/*		UM_Stop = 0, 
		UM_StopInAttack,
		UM_Walk, 
		UM_WalkInAttack,
		UM_Run,
		UM_Attack,
		UM_AttackSecond,
		UM_Magic,
		UM_Push,
		UM_Dash,
		UM_Defence,
		UM_Ko,
		UM_Die,
		UM_Beated,
*/	} m_AnimationIndex, m_prevAnimation;
	BOOL	SetMotion(enum UserMotion um);	//Animation 설정: 이동, 공격, 마법, 정지 등등
//	void	ActionMotion(enum UserMotion um, int delay_time);	//Animation 설정: 이동, 공격, 마법, 정지 등등

	enum UserDirection	//User의 방향
	{
		UD_Down = 0,
		UD_DownLeft,
		UD_Left,
		UD_UpLeft,
		UD_Up,
		UD_UpRight,
		UD_Right,
		UD_DownRight
	} m_Direction;
	void			SetDirection(enum UserDirection ud) {m_Direction = ud;}		//캐릭터의 방향설정(8방향)

	//LSP 처리루틴
	static  void    PutPixelNormal2(const WORD* pDest, const WORD* pSrc, int nCopyCount,const DWORD color, int alpha, DWORD dwRBBitMask,DWORD dwGBitMask);

	static	void	PutPixelNormal(const WORD* pDest, const WORD* pSrc, int nCopyCount);
	static	void	PutPixelAlphaAdd(const WORD* pDest, const WORD* pSrc, int nCopyCount, DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask);
	static	void	PutPixelAlphaSoft(const WORD* pDest, const WORD* pSrc, int nCopyCount, DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask);
	static	void	PutPixelTrans(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaDest, int nAlphaSrc, DWORD dwRBBitMask, DWORD dwGBitMask); // nAlphaSrc 와 nAlphaDest 의 합은 반드시 32 이어야 한다..
	static	void	PutPixelDark(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaSrc, DWORD dwRBBitMask, DWORD dwGBitMask); // nAlphaSrc - 0 이 젤 시커멓다.. 32 가 제일 밝다.
	static	void	PutPixelShadow(const WORD* pDest, int nCopyCount, int nAlphaDest, DWORD dwRBBitMask, DWORD dwGBitMask); // 0 시커먼 그림자.. 32 그림자 없음..
	static	void	PutPixelColor(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaSrc, DWORD dwDestColor, DWORD dwRBBitMask, DWORD dwGBitMask); // nAlphaSrc - 스프라이트의 밝기 값. dwDestColor - 컬러의 32비트 값. (반드시 "32-nAlphaSrc"의 비율과 일치하여야 한다.
	static	void	PutPixelHalf(const WORD* pDest, const WORD* pSrc, int nCopyCount, DWORD dwHalf16Mask, DWORD dwHalf32Mask);
	static  void	PutPixelShadowEx(const WORD* pDest, int nCopyCount, DWORD dwHalf16Mask, DWORD dwHalf32Mask);
	static	void	PutPixelBitStepTrans(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaDestBit, int nAlphaSrcBit, DWORD dwRBGBitMask, DWORD dwGRBBitMask);

	static	void	BltLSPShadow(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data,	int alpha);
	static	void	BltLSPShadowEx(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, DWORD dwHalf16Mask, DWORD dwHalf32Mask);
	static	void	BltLSPTrans(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, int alpha, BOOL bFocus=FALSE);
	static	void	BltLSPAlphaAdd(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data);
	static	void	BltLSPNormal(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, BOOL bFocus=FALSE, int nPoisonKind = 0, int nFocusColor = 3);
	static	void	BltLSPDark(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, int strength = 1);
	static	void	BltLSPHalfAlpha(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, DWORD dwHalf16Mask, DWORD dwHalf32Mask);
	static	void	BltLSPHalfAlphaEx(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, DWORD dwHalf16Mask, DWORD dwHalf32Mask);
	static  void	BltLSPDrawLine(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data,int skip_line = 0);
	static  void	BltLSPDrawObj(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data);
	static  void	BltLSPBitStepTrans(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, int nAlphaSrc, int nAlphaDst);

	static	BOOL	GetClippedRect(RECT *pRC, RECT* pRCClip);
	static  void	BltLSPChat(CDDSurface *pSurface);
	static  void	BltBlackTrans(CDDSurface *pSurface,CRect rc);


	static	CPoint	PixelToCellUnit(CPoint ptPixel);	//pixel좌표를 Cell좌표로(Cell좌표는 서버와 동기됨)
	static	CPoint	CellUnitToPixel(CPoint ptCell);		//Cell좌표를 Pixel좌표로

	BOOL			IsArrivedInCell() {return m_bCellArrive;}	//Cell의 중심에 도착?
	BOOL			IsCharStop() {return m_AnimationIndex==UM_Stop;}	//캐릭터가 완전히 정지한 상태(이동및 공격동작이 아님)
	BOOL			IsCharAttack() {return m_AnimationIndex==UM_Attack;}	//
	BOOL			IsCharDie() {return m_bDead == TRUE || m_bDeadFlag == TRUE;}	//
	void			Pause(BOOL tf) {m_bPauseChar = tf;}	//캐릭터의 동작 일시정지 OR 일시정지 해제
	BOOL			GetFocus() {return m_bFocus;}		//현재의 Focus상태
	void			SetFocus(BOOL tf) {m_bFocus = tf;}	//그린후 자동으로 풀림 (커서가 대상위에 있는 경우 Focus설정됨)
	void			SetDirection(int direction);		//direction은 illstruct.h 에 define됨
	void			MoveNextStep_New2();					// Frame변화시 다음이동함,

	void			Move();								// 이동처리 (위의 함수사용 MoveXY, MoveX, MoveY)
	CRect&			GetCharRect() {return m_rCurCharRect;} //화면상의 표시영역 (캐릭터의 표시영역)
//	BOOL			GetSpr(_SPR_LSP** spr, PWORD* pV1, PWORD* pV2, PWORD* pS, PWORD* pFX); //Frame의 변화가 있으면 TRUE
	BOOL			GetSpr(_SPR_LSP** spr, PWORD* pV); //Frame의 변화가 있으면 TRUE
	void			RenderChar(CDDSurface* pSurface, CPoint PosMapOffset);	//캐릭터를 그림
	void			DrawLine(CDDSurface* pSurface, CPoint PosMapOffsetint, int skip_line);

//	void			RenderBodyAndArmature(CDDSurface *pSurface, CPoint ptScreen, RECT rcV1, RECT rcV2);
	void			RenderBodyAndArmature(CDDSurface *pSurface, CPoint ptScreen, RECT rcV1);

	void			SetDstPos(CPoint ptDstPos);			//이동할 목적지 설정 , Pixel상의 좌표
	void			SetPosition(CPoint pos);			//초기좌표 설정(설정시 Warp가능), Pixel상의 좌표

	void			SetCellPosition(CPoint ptPosCell);	//지정한 Cell상에 캐릭터 위치시킴
	void			SetCellDstPos(CPoint ptPosDstCell);	//이동할 목적지 Cell설정

	CPoint&			GetPosition(){return m_Position;}	//현재의 위치(Pixel단위)
	CPoint			GetCellPosition() {return PixelToCellUnit(m_Position);}	//현재의 위치(Cell 위치)
	void			SetCharRes(int index, int nChar=0);		//캐릭터의 Resource설정

	void			SetChatString(CString &str);		//채팅 설정 (나중에 머리위에 띄울시)
	void			SetIdNameString(CString &str);		//채팅 설정 (나중에 머리위에 띄울시)

	CUser();
	virtual ~CUser();

	static	CCharManager*		m_bpCharManager;		//캐릭터 관리자 (CChildView::CharManager)
	static	DirectXMng*			m_bpDirectXMng;			//DirectX 관리자 (CChildView::DirectXMng)
	static	CUserManager*		m_bpUserManager;		//유저 관리자 (CChildView::UserManager)
	static	CArmatureManager*	m_bpArmatureManager;	//장착아이템 관리자

	static	DWORD m_dwRBBitMask; // 비트 마스크...	//LSP처리루틴에서 사용
	static	DWORD m_dwRBitMask;
	static	DWORD m_dwGBitMask;
	static	DWORD m_dwBBitMask;
	static	DWORD m_dwRBGBitMask;//r0b0g0
	static	DWORD m_dwGRBBitMask;//0g0r0b
	static	WORD  m_CheckKey;							 //픽셀 정보 체크값
	static	WORD  m_LineKey;


	//(재덕)
	CString m_strUserId;// 이건 퍼블릭으로 선언을 하는것이..쩝..

	///////////////////////////////



/*	int Get_Type() { return m_sType;}
	int Get_Hp() { return m_Hp;}
	int Get_Mp() { return m_Mp;}
	int Get_MaxHp() { return m_MaxHp;}
	int Get_MaxMp() { return m_MaxMp;}
	int Get_Fa() { return m_Fa;}//피로도
	int Get_At() { return m_At;}//공격력
	int Get_Wg() { return m_Wg;}//근력
	int Get_Dx() { return m_Dx;}// 속도
	int Get_Int() { return m_Int;}//지능
	int Get_Luck() { return m_Luck;}//운
	int Get_Fame() { return m_Fame;}//명성
	int Get_Mone() { return m_Money;}
	int Get_Style() { return m_Style;}//몸 형태.
	int Get_Gender() { return m_Gender;}//남성?여성?
*/

//protected:
public:
	void ChangeMoney(DWORD money);

	void AddMagic(short number, BYTE method1, BYTE method2, short start_time, BYTE type, BYTE target, CString name, CString explain, short distance, short use_mp, int use_time, short damage, short game_pic);
	void DelMagic(short number);

	void Add_Item(short index, BYTE type, BYTE arm, short picnum, BYTE gender, short min_damage, short max_damage, short limit_str, short limit_int, short limit_skill, CString magic_name, CString s_num, short special[7], DWORD value, short use_count, short max_use_count, short org_max_use_count, CString name);
	void Add_Item(PBYTE bypData);
	void Del_Item(BYTE bBelt, short index);

	BOOL MoveItemToBelt(short from, short to);
	BOOL MoveItemToInv(short from, short to);
	void View_BubbleChat(CPoint bgpos);
	void View_Id(CDC *pDC, CPoint bgpos);
	void SendRevival(int type);
	void SetFlyEndMagic(int index, BOOL bType, int nTableNum, int nPoisonKind = 0);

	int m_Run;
	int m_Run_Flag;
	int m_Walk;// 나아님 따른 유저들의 상태를 알기위해서..end패킷에서 0으로 만든다. 

	CArray<CMagic*, CMagic*>	m_arrMagic;	//설정된 마법의 리스트//dspring_0720a
	CArray<CString , CString>	m_arrMagicName;//마법공격을 했을때 적캐릭터에 설정될 마법 이름dspring_0802_3dxx
	CArray<int , int>			m_nMagicIndex;//마법공격을 했을때 적캐릭터에 설정될 마법 인덱스dspring_0724
	CArray< FLY_END_MAGIC	 , FLY_END_MAGIC>		m_arrFlyEndMagic;
	CArray< APPLIED_CON_MAGIC, APPLIED_CON_MAGIC>	m_arrACM;

	BOOL m_bCellChange;
	CMagic*			m_pMagic;				//설정된 마법
	CString			m_szChatString;			//현재 자신의 채팅

	_SPR_LSP*		m_bpSprData;			//그리기에 사용할 Sprite Data (ILL Struct에 정의됨)
	PWORD			m_wpV;					//캐릭터 영역1
//	PWORD			m_wpV1;					//캐릭터 영역1
//	PWORD			m_wpV2;					//캐릭터 영역2
//	PWORD			m_wpS;					//그림자 영역
//	PWORD			m_wpFX;					//특수효과 영역 (현재 사용하지 않음)

	CRect			m_rCurCharRect;			//현재 캐릭터의 화면표시영역(화면상의 좌표)
	DWORD			m_dwTick;				
	int				m_iSpriteIndex;			//SpriteIndex

	int				m_iCharResIndex;		//캐릭터의 Resource인덱스
	int				m_nChar;		//CharRes상의 Char Index (_CHR_HEADER 의 nQt->캐릭터 숫자)
	int				m_nCharRes;		//캐릭터의 Resource번호
	CCharRes*		m_bpCharRes;	//지정된 번호에의해 Setting된 Resource

	CPoint			m_Position;			//현재 캐릭터의 위치	(Pixel Unit)
	CPoint			m_ptDstPos;			//다음 목적지의 좌표			(Pixel Unit)
	CPoint          m_ptDestFinalPos; // 이동의 최종 목표점...좌표..

	CPoint			m_ptCellPrev;
	CPoint			m_ptCellPos;		//Map의 Cell단위 위치
	CPoint			m_ptDestCellPos;	//Map의 다음 Cell단위 위치
	
	int             m_RunEnd;
//	DWORD			m_dwMoveTick;
	DWORD           m_dwTickPrev;
	int				m_nFocusColor;

	////////////////////////////////////
	// 새롭게 움직이는거 변화를 줘보기 위해서..중복되지만 섞이는것을 방지하기위해서 다시 만듬..
	CPoint			m_ptStartCellPos;   //내가 출발한 셀 위치
//	CPoint			m_ptPreMyCellPos;		//현재 내가 있는 Cell 위치
	CPoint			m_ptPostCellPos;	//이동 할 셀 위치
	CPoint          m_ptNextCellPos;    //뛰는것 때문에 존재.

	CPoint			m_ptMoveStep;		//이동중 좌표보관

	BOOL			m_bFocus;			//Cursor에의해 Focus됨을 의미
	BOOL			m_bPauseChar;		//현재의 동작에서 일시정지
	BOOL			m_bCellArrive;		//Cell단위의 위치에 도착했음
	int				m_nMotionStep;		//Walk와 Stop을 제외한 움직임의 Step;
	int				m_nPoisonKind;		//독마법 종류
	BOOL			m_bPoisoning;		//중독중인가
	DWORD			m_dwPoisonTime;		//중독시간
	DWORD			m_dwTorchlight;		//횃불유지시간
	int				m_iUserId;			//서버에서 할당받은 User Id

	CList<CPoint, CPoint>*	m_pRoute;	//자기자신의 갈길을 설정해서 가지고 있음
	int				m_nMagicType;		//마법의 종료 (바닥 타입? 공중 타입?)

	CArmature*		m_pArmature;			//장착 아이템 인덱스

	//(재덕)
	// 개인 수치들을 구조체로 가지고 있을까? 아니면..각각의 데이타로 가지고 있을까 생각..
	//일단은 각각의 수치로..
//public:
	int m_nType;// 캐릭터의 종류....0 남자황인 1 남자 백인 2 남자 흑인 3 여자황 4, 5.....11 npc....
	int m_Me;
	int m_InHouse;// -1이면 밖이고...기타 번호면..집번호이다.

	int m_Level;

	DWORD m_Max_exp;
	DWORD m_Cur_exp;

	int m_Cur_Hp;
	int m_Max_Hp;

	int m_Cur_Mp;
	int m_Max_Mp;

	int m_Max_Fa;
	int m_Cur_Fa;//피로도

	int m_Max_Weight;//들수있는 가방무게.. 
	int m_Cur_Weight;

	int m_At;//공격력
	DWORD m_Money;
	int m_Style;//몸 형태.
	int m_Gender;
	BYTE m_bDeadEnable;

	//착용 장비..
	int m_Armor;// 상체 갑옷
	int m_Helmet;// 투구 및 머리
	int m_Boot;//신발
	int m_Jean;// 바지
	int m_Shield;// 방패
	int m_Sword;// 무기
	int m_item_1;
	int m_item_2;
	int m_item_3;
	int m_item_4;
	int m_Skin;
	int m_Hair;

	int m_Arm_type;// 현재 가지고 있는 무기의 타입 0:맨손 1:한손검 2:양손검, 3: 창, 4: 활, 5:도끼,6:쌍칼(여자)

	int m_BattleMode;// 0 : 무기를 놓고 있다. 1 : 일반 전투 모드 2 : PK 모드

	int m_PkRecive_Flag;// 무브페킷을 보낸뒤 응답을 받지 않았다면 새로운것을 보내지 않게 하기위해서..
	int m_PkRecive_Attack_Flag;// 공격 패킷을 보낸뒤..
	DWORD m_PkRecive_Attack_Time;// 공격한 시간

	DWORD m_dwBubbleTick;// 버블채팅에 사용할꺼..
	COLORREF m_vIdColor;

	DWORD m_dwMoveTick;
	DWORD m_dwnewMoveTick;


	DWORD m_dwAttackTick;
	DWORD m_dwBeatedTick;
	DWORD m_dwMagicTick;
	DWORD m_dwDieTick;

	DWORD m_FaTickCount;
	//인벤토리....
	CMyInvData m_MyInvData[INV_COUNT];
	CMyInvData m_MyBeltData[BELT_COUNT];
	int		m_vInvCount;

	//가지고 있는 마법 표현.
	CMyMagicDataArray m_arrSpecialMagic;// 00 8.16추가 재덕...
	CMyMagicDataArray m_arrBlueMagic;
	CMyMagicDataArray m_arrWhiteMagic;
	CMyMagicDataArray m_arrBlackMagic;
	CMyMagicDataMap m_mapMyMagicData;			// 관리 차원

	CString	m_Designation;		// 칭호
	CString	m_Guild;			// 소속
	CString	m_Grade;			// 지위
	short	m_Age;				// 나이
	short	m_Deposition;		// 성향
	short	m_Str;				// 힘
	short	m_Int;				// 지식
	short	m_Dex;				// 민첩
	short	m_Wis;				// 지혜
	short	m_Cha;				// 매력
	short	m_Con;				// 체질
	int		m_UsingSword;		// 검술
	int		m_UsingSpear;		// 창술
	int		m_UsingAxe;			// 둔기술
	int		m_UsingKnuckle;		// 권술
	int		m_UsingBow;			// 궁술
	int		m_UsingCrook;		// 지팡이
	int		m_MakingWeapon;		// 무기 제작술
	int		m_MakingArmor;		// 방어구 제작술
	int		m_MakingAccessory;	// 장신구 제작술
	int		m_MakingPotion;		// 시약 제작술
	int		m_Cooking;			// 요리
	int		m_BlackMagicExp;	// 흑마법 기술치
	int		m_WhiteMagicExp;	// 백마법 기술치
	int		m_BlueMagicExp;		// 청마법 기술치

	short	m_BloodType;

	void	SetDeposition(short deposition);

	int     m_prediff;
	bool    m_bFa;
//	int     m_HpRecoveryTime;
//	DWORD   m_HpTickCount;	
//	int     m_MpRecoveryTime;
//	DWORD   m_MpTickCount;	

//	void	HpRecovery();
//	void	MpRecovery();
};

inline void CUser::SetDeposition(short deposition)
{
	m_Deposition = deposition;
	if(m_Deposition <= -50) m_vIdColor = RGB(255, 0, 0);
	else if(m_Deposition >= -49 && m_Deposition <= -30) m_vIdColor = RGB(255, 70, 70);
	else if(m_Deposition >= -29 && m_Deposition <= -11) m_vIdColor = RGB(255, 140, 140);
	else if(m_Deposition >= -10 && m_Deposition <= -4) m_vIdColor = RGB(255, 200, 200);
	else if(m_Deposition >= -3 && m_Deposition <= 3) m_vIdColor = RGB(211, 237, 251);
	else if(m_Deposition >= 4 && m_Deposition <= 10) m_vIdColor = RGB(187, 230, 244);
	else if(m_Deposition >= 11 && m_Deposition <= 30) m_vIdColor = RGB(158, 208, 246);
	else if(m_Deposition >= 30 && m_Deposition <= 49) m_vIdColor = RGB(167, 190, 248);
	else if(m_Deposition >= 50) m_vIdColor = RGB(150, 150, 255);
}


inline void CUser::PutPixelNormal2(const WORD* pDest, const WORD* pSrc, int nCopyCount,const DWORD color, int alpha, DWORD dwRBBitMask,DWORD dwGBitMask)
{
	int dstalpha = 32 - alpha;

	__asm // RGB 값마다 강도 계산..
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;
COPY_LOOP:
		push ecx; // 카운트 저장..
		
		mov ebx, 0; // 최종 색깔 초기화
		
		mov ecx, color; // Spr Pixel
		
		jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..
		
		mov eax, ecx; // Spr - R B
		and eax, dwRBBitMask;
		shr eax, 5;
		mul alpha;		
		and eax, dwRBBitMask;
		add ebx, eax;
		
		mov eax, color; // Spr - G
		and eax, dwGBitMask;
		mul alpha;
		shr eax, 5;
		and eax, dwGBitMask;
		add ebx, eax;
		
		mov ecx, [esi]; // Bkg Pixel
		
		mov eax, ecx; // Bkg - R B
		and eax, dwRBBitMask;
		shr eax, 5;
		mul dstalpha;		
		and eax, dwRBBitMask;
		add ebx, eax;
		
		mov eax, [esi]; // Bkg - G
		and eax, dwGBitMask;
		mul dstalpha;
		shr eax, 5;
		and eax, dwGBitMask;
		add ebx, eax;
		
		mov [edi], ebx;
		
DONT_COPY:
		
		add edi, 2;
		add esi, 2;
		
		pop ecx; // 카운트 복구..
		dec ecx;		
		jnz COPY_LOOP;
	}
}


// 엄청난 크기의 인라인 함수.. 속도에 매우 민감하다. - BltAlphaAdd.
inline void CUser::PutPixelNormal(const WORD* pDest, const WORD* pSrc, int nCopyCount)
{
/*	__asm
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;
MOV_LOOP:
		movsw;
		dec ecx;
		jnz MOV_LOOP;
	}*/
	__asm
	{
		push edi;
		push esi;
		cld;
		mov edi, pDest;
		mov esi, pSrc;
		mov ebx, nCopyCount;
		mov eax, ebx;
		shr eax, 1;
		mov ecx, eax;
		rep movsd;
		mov eax, ebx;
		and eax, 1;
		mov ecx, eax;
		rep movsw;
		pop esi;
		pop edi;
	}
};

inline void CUser::PutPixelAlphaAdd(const WORD* pDest, const WORD* pSrc, int nCopyCount, DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask)
{
	__asm
	{
		mov edi, pDest;	// 백그라운드 주소 저장.
		mov esi, pSrc;	// 스프라이트 주소 저장.
		
		mov ecx, nCopyCount; // 가로 길이.
		
COPY_LOOP:
		push ecx;
		mov ecx, [esi]; // Spr
		
		jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..
		
		mov ebx, 0; // 최종 픽셀
		
		mov eax, ecx; // Spr - R
		mov edx, [edi]; // Bkg - R
		and eax, dwRBitMask;
		and edx, dwRBitMask;
		add edx, eax;
		cmp edx, dwRBitMask;
		ja COPY_R1;
		jmp ADD_R1;
COPY_R1:
		mov edx, dwRBitMask;
ADD_R1:
		or ebx, edx;
		
		mov eax, ecx; // Spr - G
		mov edx, [edi]; // Bkg - G
		and eax, dwGBitMask;
		and edx, dwGBitMask;
		add edx, eax;
		cmp edx, dwGBitMask;
		ja COPY_G1;
		jmp ADD_G1;
COPY_G1:
		mov edx, dwGBitMask;
ADD_G1:
		or ebx, edx;
		
		mov eax, ecx; // Spr - B
		mov edx, [edi]; // Bkg - B
		and eax, dwBBitMask;
		and edx, dwBBitMask;
		add edx, eax;
		cmp edx, dwBBitMask;
		ja COPY_B1;
		jmp ADD_B1;
COPY_B1:
		mov edx, dwBBitMask;
ADD_B1:
		or ebx, edx;
		
		mov [edi], bx; // 백버퍼에 씀
		
DONT_COPY:
		add edi, 2;
		add esi, 2;
		
		pop ecx;
		dec ecx;
		jnz COPY_LOOP;
	}
};

inline void CUser::PutPixelAlphaSoft(const WORD* pDest, const WORD* pSrc, int nCopyCount, DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask)
{
	__asm
	{
		mov edi, pDest;	// 백그라운드 주소 저장.
		mov esi, pSrc;	// 스프라이트 주소 저장.
		
		mov ecx, nCopyCount; // 가로 길이.
		
COPY_LOOP:
		push ecx; // 카운트 기억
		
		mov ecx, [esi]; // 스프라이트 픽셀
		
		jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..
		
		mov ebx, 0; // 최종 픽셀 값..
		
		mov ecx, [esi]; // Spr 픽셀
		
		mov edx, [edi]; // Bkg - R
		mov eax, ecx // Spr - R
			and edx, dwRBitMask;
		and eax, dwRBitMask;
		cmp eax, edx;
		jb COPY_R1_BKG;
		mov edx, eax
COPY_R1_BKG:
		or ebx, edx;
		
		mov edx, [edi]; // Bkg - R
		mov eax, ecx // Spr - R
			and edx, dwGBitMask;
		and eax, dwGBitMask;
		cmp eax, edx;
		jb COPY_G1_BKG;
		mov edx, eax
COPY_G1_BKG:
		or ebx, edx;
		
		mov edx, [edi]; // Bkg - R
		mov eax, ecx // Spr - R
			and edx, dwBBitMask;
		and eax, dwBBitMask;
		cmp eax, edx;
		jb COPY_B1_BKG;
		mov edx, eax
COPY_B1_BKG:
		or ebx, edx;
		
		mov [edi], bx; // 백버퍼에 씀
		
DONT_COPY:
		add edi, 2;
		add esi, 2;
		
		pop ecx; // 카운트 복구
		dec ecx;
		jnz COPY_LOOP;
	}
};

// nAlphaSrc 와 nAlphaDest 의 합은 반드시 5이어야 한다..
inline void CUser::PutPixelBitStepTrans(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaDestBit, int nAlphaSrcBit, DWORD dwRBGBitMask, DWORD dwGRBBitMask)
{
	__asm
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;

		mov ebx, ecx;
		and ebx, 1;
		jz	DWORD_ALPHA_MIX;
/////////////////////////////////
// 한픽셀 처리해주는 부분
/////////////////////////////////
		add edi, 2;
		add esi, 2;
		dec ecx;
		jz  END_LOOP;

DWORD_ALPHA_MIX:
		shr ecx, 1;

COPY_LOOP:
		push ecx;
//first

		mov eax, [esi];
		and eax, dwRBGBitMask;
		shr eax, 5;
		mul nAlphaSrcBit;

		and eax, dwRBGBitMask;
		mov ecx, eax;

		mov eax, [esi];
		and eax, dwGRBBitMask;
		mul nAlphaSrcBit;

		shr eax, 5;
		and eax, dwGRBBitMask;
		or  ecx, eax;
//		or  eax, ecx;


//second
		mov eax, [edi];
		and eax, dwRBGBitMask;
		shr eax, 5;
		mul nAlphaDestBit;

		and eax, dwRBGBitMask;
		mov ebx, eax;

		mov eax, [edi];
		and eax, dwGRBBitMask;

		mul nAlphaDestBit;
		shr eax, 5;
		and eax, dwGRBBitMask;
		or  eax, ebx;

		add  eax,ecx;//두색더하기

		mov [edi],eax;

		add edi, 4;
		add esi, 4;
		
		pop ecx;
		dec ecx;
		jnz COPY_LOOP;

END_LOOP:
	}
}

inline void CUser::PutPixelTrans(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaDest, int nAlphaSrc, DWORD dwRBBitMask, DWORD dwGBitMask) // nAlphaSrc 와 nAlphaDest 의 합은 반드시 32 이어야 한다..
{
	__asm // RGB 값마다 강도 계산..
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;
COPY_LOOP:
		push ecx; // 카운트 저장..
		
		mov ebx, 0; // 최종 색깔 초기화
		
		mov ecx, [esi]; // Spr Pixel
		
		jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..
		
		mov eax, ecx; // Spr - R B
		and eax, dwRBBitMask;
		mul nAlphaSrc;
		shr eax, 5;
		and eax, dwRBBitMask;
		add ebx, eax;
		
		mov eax, ecx; // Spr - G
		and eax, dwGBitMask;
		mul nAlphaSrc;
		shr eax, 5;
		and eax, dwGBitMask;
		add ebx, eax;
		
		mov ecx, [edi]; // Bkg Pixel
		
		mov eax, ecx; // Bkg - R B
		and eax, dwRBBitMask;
		mul nAlphaDest;
		shr eax, 5;
		and eax, dwRBBitMask;
		add ebx, eax;
		
		mov eax, ecx; // Bkg - G
		and eax, dwGBitMask;
		mul nAlphaDest;
		shr eax, 5;
		and eax, dwGBitMask;
		add ebx, eax;
		
		mov [edi], bx;
		
DONT_COPY:
		
		add edi, 2;
		add esi, 2;
		
		pop ecx; // 카운트 복구..
		dec ecx;
		jnz COPY_LOOP;
	}
};

inline void CUser::PutPixelDark(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaSrc, DWORD dwRBBitMask, DWORD dwGBitMask) // nAlphaSrc - 0 이 젤 시커멓다.. 32 가 제일 밝다.
{
	__asm // RGB 값마다 강도 계산..
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;
COPY_LOOP:
		push ecx; // 카운트 저장..
		
		mov ebx, 0; // 최종 색깔 초기화
		
		mov ecx, [esi]; // Spr Pixel
		
		jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..
		
		mov eax, ecx; // Spr - R B
		and eax, dwRBBitMask;
		mul nAlphaSrc;
		shr eax, 5;
		and eax, dwRBBitMask;
		add ebx, eax;
		
		mov eax, ecx; // Spr - G
		and eax, dwGBitMask;
		mul nAlphaSrc;
		shr eax, 5;
		and eax, dwGBitMask;
		add ebx, eax;
		
		mov [edi], bx;
		
DONT_COPY:
		
		add edi, 2;
		add esi, 2;
		
		pop ecx; // 카운트 복구..
		dec ecx;
		jnz COPY_LOOP;
	}
};

inline void CUser::PutPixelShadowEx(const WORD* pDest, int nCopyCount, DWORD dwHalf16Mask, DWORD dwHalf32Mask) // nAlphaSrc - 0 이 젤 시커멓다.. 32 가 제일 밝다.
{
	__asm
	{
		mov edi, pDest;
		mov ecx, nCopyCount;

		// nCopyCount가 홀수인지 짝수인지를 판단. 
		mov eax, ecx;
		and eax, 1;
		jz	DWORD_HALF_MIX;
	
		mov eax, [edi]; // Dest Spr Pixel
		and eax, dwHalf16Mask;
		shr eax, 1;
		mov [edi], ax;		// ax는 16bit Register.
		
		add edi, 2;
		dec ecx;
		jz  END_LOOP;

		// 여기서부터는 32bit(DWORD)계산.
DWORD_HALF_MIX: 
		shr ecx, 1;
		
COPY_LOOP:
		mov eax, [edi]; // 
		and eax, dwHalf32Mask;
		shr eax, 1;
		mov [edi], eax;

		add edi, 4;
		
		dec ecx;
		jnz COPY_LOOP;
END_LOOP:
	}
};

inline void CUser::PutPixelShadow(const WORD* pDest, int nCopyCount, int nAlphaDest, DWORD dwRBBitMask, DWORD dwGBitMask) // 0 시커먼 그림자.. 32 그림자 없음..
{
	__asm // RGB 값마다 강도 계산..
	{
		mov edi, pDest;
		//					mov esi, pSrc;
		mov ecx, nCopyCount;
COPY_LOOP:
		push ecx; // 카운트 저장..
		
		mov ebx, 0; // 최종 색깔 초기화
		
		mov ecx, [edi]; // Bkg Pixel
		
		mov eax, ecx; // Bkg - R B
		and eax, dwRBBitMask;
//		mul nAlphaDest;		
//		shr eax, 5;
		shr eax, 1;		// nAlphaDest = 16일경우.(그림자일경우)
		and eax, dwRBBitMask;
		add ebx, eax;
		
		mov eax, ecx; // Bkg - G
		and eax, dwGBitMask;
//		mul nAlphaDest;
//		shr eax, 5;
		shr eax, 1;		// nAlphaDest = 16일경우.(그림자일경우)
		and eax, dwGBitMask;
		add ebx, eax;
		
		mov [edi], bx;
		
		add edi, 2;
		//			add esi, 2;
		
		pop ecx; // 카운트 복구..
		dec ecx;
		jnz COPY_LOOP;
	}

};

inline void CUser::PutPixelColor(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaSrc, DWORD dwDestColor, DWORD dwRBBitMask, DWORD dwGBitMask) // nAlphaSrc - 스프라이트의 밝기 값. dwDestColor - 컬러의 32비트 값. (반드시 "32-nAlphaSrc"의 비율과 일치하여야 한다.
{
	__asm // RGB 값마다 강도 계산..
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;
COPY_LOOP:
		push ecx; // 카운트 저장..
		
		mov ebx, 0; // 최종 색깔 초기화
		
		mov ecx, [esi]; // Spr Pixel
		
		jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..
		
		mov eax, ecx; // Spr - R B
		and eax, dwRBBitMask;
		mul nAlphaSrc;
		shr eax, 5;
		and eax, dwRBBitMask;
		add ebx, eax;
		
		mov eax, ecx; // Spr - G
		and eax, dwGBitMask;
		mul nAlphaSrc;
		shr eax, 5;
		and eax, dwGBitMask;
		add ebx, eax;
		
		add ebx, dwDestColor; // 컬러 값을 더해준다.
		
		mov [edi], bx;
		
DONT_COPY:
		
		add edi, 2;
		add esi, 2;
		
		pop ecx; // 카운트 복구..
		dec ecx;
		jnz COPY_LOOP;
	}
};

inline void CUser::PutPixelHalf(const WORD* pDest, const WORD* pSrc, int nCopyCount, DWORD dwHalf16Mask, DWORD dwHalf32Mask) // nAlphaSrc - 0 이 젤 시커멓다.. 32 가 제일 밝다.
{
	//ASSERT(dwRBMask == 0x7c1f);

	//DWORD dwMask = 0x7bde7bde;
	//DWORD  wMask = 0x00007bde;
	__asm
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;

		// nCopyCount가 홀수인지 짝수인지를 판단. 
		mov ebx, ecx;
		and ebx, 1;
		jz	DWORD_HALF_MIX;

		//push ecx; // 카운트 저장..
	
		mov eax, [esi]; // Spr Pixel
		mov edx, [edi]; // Dest Spr Pixel
		and eax, dwHalf16Mask;
		and edx, dwHalf16Mask;
		shr eax, 1;
		shr edx, 1;
		add eax, edx;
		//and eax, wMask;
		mov [edi], ax;		// ax는 16bit Register.
		
		add edi, 2;
		add esi, 2;
		//pop ecx; // 카운트 복구..
		dec ecx;
		jz  END_LOOP;

		// 여기서부터는 32bit(DWORD)계산.
DWORD_HALF_MIX: 
		shr ecx, 1;
		
COPY_LOOP:
		//push ecx; // 카운트 저장..
		//mov ebx, 0; // 최종 색깔 초기화
		
		mov eax, [esi]; // Spr Pixel
		//jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..
		mov edx, [edi]; // 
		and eax, dwHalf32Mask;
		and edx, dwHalf32Mask;
		shr eax, 1;
		shr edx, 1;
		add eax, edx;
		//and eax, dwMask;
		mov [edi], eax;

//DONT_COPY:
		add edi, 4;
		add esi, 4;
		
		//pop ecx; // 카운트 복구..
		dec ecx;
		jnz COPY_LOOP;
END_LOOP:
	}
};


#endif // !defined(AFX_USER_H__5A5901E5_3853_11D3_8422_00105A6B97E2__INCLUDED_)
/*
// nAlphaSrc 와 nAlphaDest 의 합은 반드시 5이어야 한다..
inline void CUser::PutPixelBitStepTrans(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaDestBit, int nAlphaSrcBit, DWORD dwRBGBitMask, DWORD dwGRBBitMask)
{
	__asm
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;

		mov ebx, ecx;
		and ebx, 1;
		jz	DWORD_ALPHA_MIX;
/////////////////////////////////
// 한픽셀 처리해주는 부분
/////////////////////////////////
		add edi, 2;
		add esi, 2;
		dec ecx;
		jz  END_LOOP;

DWORD_ALPHA_MIX:
		shr ecx, 1;

COPY_LOOP:
		push ecx;
//first
		mov eax, [esi];
		and eax, dwRBGBitMask;
		shr eax, 5;
//		mov cl, byte ptr nAlphaSrcBit;
//		shl eax, cl;
		mul nAlphaSrcBit;

		and eax, dwRBGBitMask;
		mov edx, eax;

		mov eax, [esi];
		and eax, dwGRBBitMask;
//		shl eax, cl;
		mul nAlphaSrcBit;

		shr eax, 5;
		and eax, dwGRBBitMask;
		or  edx, eax;
//second
		mov eax, [edi];
		and eax, dwRBGBitMask;
		shr eax, 5;
//		mov cl, byte ptr nAlphaDestBit;
//		shl eax, cl;
		mul nAlphaDestBit;

		and eax, dwRBGBitMask;
		mov ebx, eax;

		mov eax, [edi];
		and eax, dwGRBBitMask;

		mul nAlphaDestBit;
//		shl eax, cl;
		shr eax, 5;
		and eax, dwGRBBitMask;
		or  edx, eax;

		or  edx,ebx;//두색더하기
		mov [edi],edx;

		add edi, 4;
		add esi, 4;
		
		pop ecx; // 카운트 복구..
		dec ecx;
		jnz COPY_LOOP;

END_LOOP:
	}
}
*/