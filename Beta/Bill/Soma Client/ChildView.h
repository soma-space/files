// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__180690AB_F9A1_11D2_8421_0020AF9F40BD__INCLUDED_)
#define AFX_CHILDVIEW_H__180690AB_F9A1_11D2_8421_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//

#include "DirectXMng.h"
#include "SocketMng.h"
#include "CharManager.h"
#include "UserManager.h"
#include "PacketParser.h"
#include "MapManager/SM_MapManager.h"
#include "MagicManager.h"
#include "ArmatureManager.h"
#include "Weather.h"

//#include "IGObject\\DDEngine.h"
#include "IGObject\\DDEngineLSP.h"
#include "IGObject\\MapObj.h"
#include "IGObject\\AniObjLsp.h"
//#include "TestEngine.h"
#include "TileMapTool.h"
#include "TileSetTool.h"

#include "Dialog_AccountLogin.h"
#include "Dialog_Login.h"
#include "SLoadingView.h"

#include "Rain.h"
#include "LightSource.h"
#include "house.h"
#include "TransObj.h"

#define MAX_RAIN 150
//#include "FogEffect.h"


//((재덕))
//#include "Item.h"
//#include "Item_Arm.h"
//#include "Item_sArm.h"

class BBDX_Editor;
class BBDX_ChatEditor;
class BBDX_ChatOuter;
//우석.
class CDialog_Login;

//우석
typedef struct  _tgCharData 
{
	BYTE   IdSize;
	char   *userID;
	short  Class;
	short  Level;
	short  Str;
	short  Dex;
	short  Inteli;
	short  Cha;
	short  Wis;
	short  Con;
	short  Hp;
	short  MaxHp;
	short  Mp;
	short  MaxMp;
	short  Stm;
	short  MaxStm;
	short  Age;
	short  Moral;
	short  Gender;
	short  Hair;
	short  Skin;
	CString  Group;
	short  Fame;
	int    SwordExp;
	int    SpearExp;
	int    BowExp;
	int    AxeExp;
	int    KnuckleExp;
	int    StaffExp;
	int    WeaponMakeExp;
	int    ArmorMakeExp;
	int    AccMakeExp;
	int    PotionMakeExp;
	int    CookingExp;
	short  ItemCheck[10];

public:
	_tgCharData()
	{
		userID = NULL;
	}
	~_tgCharData()
	{
		if(userID) { delete [] userID; userID = NULL; }
	}
}CharData;
//우석.
#ifndef LIGHTINFO
#define LIGHTINFO
typedef struct TAG_LIGHT_INFO
{
	int m_kind;
	CPoint m_pt;
}LIGHT_INFO;
#endif



/////////////////////////////////////////////////////////////////////////////
// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();
	virtual ~CChildView();

// Attributes
public:
	static CUserManager		UserManager;		//User 관리자 (Server의 유저데이터 보관장소-서버와 동기화)
	static CCharManager		CharManager;		//캐릭터 Resource관리자
	static DirectXMng		DirectXMng;			//DirctX 관리자
	static SocketMng		SocketMng;			//Socket 관리자
	static SM_MapManager	MapManager;			//Map 관리자
	static CPacketParser	PacketParser;		//서버로 부터 받은 패킷 분석기
	static CMagicManager	MagicManager;		//Magic Resource 관리자
	static CArmatureManager	ArmatureManager;	//장착 아이템 관리자
	static CWeather			Weather;

	static CDDEngineNew     g_pTEng;            // 광범 엔진..
//	static CDDEngineLSP     g_pTEng;            // 광범 엔진..
	static CTileMapTool     TileMapTool;
	static CTileSetTool     TileSetTool;
	static CMapObj          MapObj;
	static CMapObj          MapShadowObj;
	static CMapObj			MapHouseIn;
	static CMapObj			MapHouseOut;
	static CAniObjLSP		MapAniObjs[MAX_MAP_OBJ_REG];
	static CAniObjLSP		MapShadowAniObjs[MAX_MAP_OBJ_REG];
	static CAniObjLSP		MapAniHouseIn[MAX_MAP_OBJ_REG];
	static CAniObjLSP		MapAniHouseOut[MAX_MAP_OBJ_REG];

	CRain					m_Rain[MAX_RAIN];
	CTransObj				m_TrnasObj;
	static CLightSource		m_LightSource;
	static CArray<LIGHT_INFO,LIGHT_INFO> m_ptLightPos;
	static CFont			*pAppFont;
	static int				bDlgIn;
//	static CFogEffect       FogEffect;          //안개 효과...

//	static CItem            Item;
//	static CItem_Arm        Item_Arm;

//public:
//	static CItem_Arm        Item_Arm;
//	static CItem_sArm        Item_sArm;

// Operations
	int m_nQtObjNeededRender; // 렌더링이 되어야 할(화면내에 있어야 할) 오브젝트 갯수 : 광범 9.7
	struct _OBJECT_DATA // 캐릭터 정렬하고 그사이에 오브젝트를 렌더링할려고 만든 구조체 : 광범 9.7
	{
		POINT pt; // 포인트 단위 점좌표
		int nSet; // 세트 번호
		int nSubSet; // 서브셋 번호
	};
	_OBJECT_DATA m_ObjStructs[256]; // 한화면에 256 개 이하로 예상
public:
    int m_S_Connection;// 세션에 연결되어 있는가?

	// newAccount 
	CString m_New_Account_ID;
	CString m_New_Account_Passwd, m_New_Account_Passwd2;
	CString m_New_Account_Name;
	CString m_New_Account_SocNo;
	CString m_New_Account_Address;
	CString m_New_Account_TellNo;
	CString m_New_Account_Email;

	bool    m_bAccountNewOk;
	bool    m_bLoginOk;

	// 우석
	SLoadingView	 *m_LoadinView;
	CharData		*m_CharData[3];
	bool			 m_bMakeChar;
	bool			 m_bMakeFail;
	bool			 m_bMakeProcess;
	int				 m_CharNum; //만들어진 캐릭터 갯수.

	CString			 m_conIP; //접속할 ip...

	void  InitChar();
	// 우석.

/*
	BBDX_Editor*	pBBDX_Editor_id;//1
	BBDX_Editor*	pBBDX_Editor_password;//2
	BBDX_Editor*	pBBDX_Editor_password2;//3 패스워드 확인
	BBDX_Editor*	pBBDX_Editor_name;//4
	BBDX_Editor*	pBBDX_Editor_email;//5
	BBDX_Editor*	pBBDX_Editor_number;//6 주민번호
	BBDX_Editor*	pBBDX_Editor_address;//7 주소
	BBDX_Editor*	pBBDX_Editor_phonenumber;//8집 전화번호
	BBDX_Editor*	pBBDX_Editor_phonenumber2;//9 직장또는 핸드폰...
*/
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_limit_old_y;
	void SetOld_limit_y( int limit_y ){ m_limit_old_y = limit_y; }

	void ArrangeTileResource();

	void RemoveUnUseLSP();
	void DrawShawdowObj();
	void DrawShawdowObj(CDDSurface *pSurface);

	void DrawShawdowObjEx();

	void DrawObj(int limit_y);
	void DrawObjEx(int limit_y, CPoint org_pos, CPoint offset_pos, CPoint x_range, BOOL bAbove = FALSE);

	void DrawHouseIn(int x, int y, CPoint org_pos, CPoint offset_pos, BOOL bAbove = FALSE);//집안내부 그려주기
	CPoint DrawMyInHouseAndInCheck(int y, CPoint org_pos, CPoint offset_pos, CPoint x_range, BOOL bAbove = FALSE);//집 그려주기 집 안에 있을때
	void DrawCommonObj(int y, CPoint org_pos, CPoint offset_pos, CPoint x_range, BOOL bAbove = FALSE);//일반 오브젝트 그리기 
	CPoint DrawMyInObj(int limit_y, CPoint org_pos, CPoint offset_pos, CPoint x_range, BOOL bAbove = FALSE);//집안에 들어갔을때 그려주는 함수
	void DrawMyOutObj(int limit_y, CPoint org_pos, CPoint offset_pos, CPoint x_range, BOOL bAbove = FALSE);//집밖에 있을때 그려주는 함수
	void DrawMyOutHouse(int y, CPoint org_pos, CPoint offset_pos, CPoint x_range, BOOL bAbove = FALSE);//집그려주기 내가 집밖에 있을때 
	void DrawTransOutHouse(int x, int y, CPoint org_pos, CPoint offset_pos, int nAlphaSrc, int nAlphaDst);


	BOOL FieldCheck(CPoint pt, int x, int y, RECT* pRC, WORD* data);
	BOOL LineCheck(CPoint pt, int x, int y, RECT* pRC, WORD* data);

	CHouse m_house;
	int m_Mouse_In;
	void DrawObject();
	void DrawHousePlace();
	void DrawTile(int type =0);
    int GetItemArmTypeIndex(int i); //우석.. 10월 16일..
	int GetItemDrawOrderIndex(int i);
	int GetItemPicIndex(int i);
	void ReadItemIndex();
	void ViewSelectBox(CRect rBox, int color=0);
	void Magic_Draw();
	void Magic_Item_Draw();
	void ChatMode(int type);
	void DrawObjcet();
	void DrawRains();
	void DrawRainDrops();
	void DrawNight();
	void DrawNight1(BOOL bRight);
	void BackLoading(CPoint pos);
	void BackLoadingEx(CPoint pos);

	void SendLivePacket();
	void ClearScreen();
	int m_Old_Item_Type[9];
	static void FieldInfoItemProcess(PBYTE bypData);
	int m_Login_Char;
	void NewCharter();
	void LoginCharterView();
    static  void CharacterDataProcess(PBYTE bypData);
	//우석.. static -> no static
	void DelHeroProcess(PBYTE bypData);
	void NewHeroProcess(PBYTE bypData);
	//우석.
	static  void ThrowItemProcess(PBYTE bypData);
	static	void UseItemProcess(PBYTE bypData);
	static  void PickUpItemProcess(PBYTE bypData);
	static  void OutItemProcess(PBYTE bypData);
	static  void GetItemProcess(PBYTE bypData);
	static	void MoneyChangeProcess(PBYTE bypData);
	
	static  CPoint PixelToCellUnit(CPoint ptPixel);
	static  CPoint CellUnitToPixel(CPoint ptCell);

	int m_test_Flag;
	void SendDelHero(CString id);
	BOOL SendNewHero(CString id, CDialog_Login *Dialog_Login);
	
	BOOL LoginAcount(CString id, CString password);
	BOOL LoginAcountNew();
	BOOL InitLogin();
	void GameLogin();
	void LoginView();
	void Login_Window();
	BOOL LogIn();
	BOOL m_First_Menu;
	void Rail_MagicProcess();
	void AttackMagic(int suid, int euid, int magic_type);
	static BOOL ChangeItem(int index);
	int m_Arry_ItemIndex[50];
	int m_Arry_ItemType[50];
	void Search_Item();
	void Item_MouseProcess(BBDX_MouseInformation *pMouseInfo, int Dlg_Pos, int Select_Box, int MoveFlag = 1);
	void MagicItem_MouseProcess(BBDX_MouseInformation *pMouseInfo, int Dlg_Pos, int Select_Box, int MoveFlag = 1);
	void Item_Init();

	void Item_Draw();
	void AuctionSell_Item_Draw();

	void ShopItem_Draw();

	int m_Item_Pic_Index[1000];// 그림번호를 맞추기 위해서 존재.
	int m_Arm_type[1000]; //10월 16일 우석..
	int m_Draw_Order[1000];

	bool m_RestartGame;

	void					SetMainMode(int mode);

	//DirectInput 마우스 처리함수
	int						MouseProcess();	
	void					AccountMouseProcess(int *id_flag);
	void					LoginMouseProcess(CDialog_Login *Dialog_Login);
	void					LBtnDown(CPoint &pos);
	void					LBtnPressing(CPoint &pos);
	void					LBtnUp(CPoint &pos);
	void					RBtnDown(CPoint &pos);
	void					RBtnPressing(CPoint &pos);
	void					RBtnUp(CPoint &pos);
	void					MouseMove(CPoint &pos, BOOL lClick = FALSE);

	void					InitStaticVariables();

	void					PacketProcess();
	BOOL					SocketInit();
	BOOL					SocketRelease();
	BOOL					IdleProcess(LONG turn);
	BOOL					NFocusIdleProcess(LONG turn);
	BOOL					DDRelease();	//DirectX 종료	 (Application 종료시)
	BOOL					DDInit();		//DirectX 초기화 (Application 시작시)
	void					SetMyUserId(int uid);			//자신의 UserId (서버와 동일)
	int						GetMyUserId() {return m_iMyId;}

	void					CreateEditor();							//Editor생성(다크세이버와 동일)
	BBDX_ChatEditor*		GetEditor(){return m_pBBDX_Editor;}		//	"
	BBDX_ChatOuter*			GetChatOuter(){return m_pChatOuter;}	//	"
	BBDX_Editor*			GetChatOuterid(){return m_pBBDX_Editor_id;}	//	"
	BBDX_Editor*			GetChatOuterpassword(){return m_pBBDX_Editor_password;}	//	"
	int                     m_Chattype;// 채팅 타입... 1: 노말 채팅 2: 쪽지 ...
	CString                 m_CurMemoChatId;
	CString                 GetCurMemoChatId(){ return m_CurMemoChatId;}
	void	                SetCurMemoChatId(CString id_name){ m_CurMemoChatId = id_name;}
	void					DisplayDate();

	void					ReStart();
	void					Notice(PBYTE bypData);

protected:
	BBDX_Editor*			m_pBBDX_Editor_id;
	BBDX_Editor*			m_pBBDX_Editor_password;
	BBDX_ChatOuter*			m_pChatOuter;
	BBDX_ChatEditor*		m_pBBDX_Editor;
	CPoint					m_ptMapOffset;		//Pixel단위의 Offset(현재그려지고 있는 상태의 Offset
	int						m_iMyId;
	DWORD                   m_PathFindGap;

	bool                    m_bCtrlUp;

	//키관련 변수들 눌려졌냐 아니냐 이것들은 임시로 사용될것들이다.....
	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg	LRESULT OnPacketProcess(WPARAM, LPARAM);
	afx_msg	LRESULT	OnPacketPending(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnImeStartComposition(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnImeComposition(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnImeEndComposition(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnImeCompositionFull(WPARAM wParam, LPARAM lParam);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__180690AB_F9A1_11D2_8421_0020AF9F40BD__INCLUDED_)
