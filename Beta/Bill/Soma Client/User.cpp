// User.cpp: implementation of the CUser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "MainFrm.h"
#include "User.h"
#include "CharRes.h"
#include "CharManager.h"
#include "DManage/BB_Direct.h"
#include "DManage/DirectSound.h"
#include "DirectXMng.h"
#include "UserManager.h"
#include "PacketParser.h"
#include "ChildView.h"
#include "SocketMng.h"
#include "Magic.h"
#include "ArmatureManager.h"
#include "ArmatureRes.h"
#include "Armature.h"
#include "PathFind.h"
#include "DManage/Direct3d.h"//dspring_0731_3dxx
#include "DarkDef.h"
#include "SoundDef.h"

#include "Dialog_Main.h"
#include "Dialog_DeadMain.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define XMOVE_GAP*m_Run	16
//#define YMOVE_GAP*m_Run	8

#define XMOVE_GAP	8
#define YMOVE_GAP	4


#define	DEFAULT_BUBBLECHAT_DELAY	3000	// 5초

#define XRUN_GAP	16
#define YRUN_GAP	8

#define TIME_SLOW	600
#define TIME_NORMAL	300
#define TIME_FAST	200

#define LATE_RECOVERY_TIME   30000
#define FAST_RECOVERY_TIME   15000
#define FAST_RECOVERY_TIME2  10000 //비무장 상태일 경우.

CCharManager*		CUser::m_bpCharManager = NULL;
DirectXMng*			CUser::m_bpDirectXMng = NULL;
CUserManager*		CUser::m_bpUserManager = NULL;
CArmatureManager*	CUser::m_bpArmatureManager = NULL;

DWORD CUser::m_dwRBBitMask = 0x0000f81f; // 비트 마스크...
DWORD CUser::m_dwRBitMask = 0x0000f800;
DWORD CUser::m_dwGBitMask = 0x000007e0;
DWORD CUser::m_dwBBitMask = 0x0000001f;
WORD  CUser::m_CheckKey;
WORD  CUser::m_LineKey;
DWORD CUser::m_dwRBGBitMask;//r0b0g0
DWORD CUser::m_dwGRBBitMask;//0g0r0b


extern SIZE g_vDispPixSize, g_vDispCellSize;

extern CMap<int, int, char *, char *&> g_SoundName;

extern CDialog_Main		Dialog_Main;
extern CDialog_DeadMain	Dialog_DeadMain;
extern CUIRes			*g_pShowDialog;
extern CUIRes			*g_pFocusDialog;
extern CUIRes			*g_pCaptureDialog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUser::CUser()
{
	INIT_PTR(m_bpCharRes);
	m_iCharResIndex = 0;
	m_nChar = 0;
	m_Direction = UD_Down;
	m_dwTick = 0;
	m_iSpriteIndex = 0;
	m_Arm_type = 0;//맨손
	m_Run = 1;//뛰면 2 값이 된다. 
	m_Run_Flag = 0; // 셀 두칸을 처리해주기 위해서... 1 -> 0 패킷 보내기.. 이런식으루..
	m_Walk = 0;
	m_InHouse = -1;
	m_bDeadEnable = 0;

	m_PkRecive_Flag = 0;// 패킷 0이면 보내지 않았다. 1이면 보냈다..무브 패킷
	m_PkRecive_Attack_Flag = 0;
	m_PkRecive_Attack_Time = GetTickCount();

	m_Cur_Hp = 1;
	m_BattleMode = 0;// 기본 모드
	m_AnimationIndex = UM_Stop;
	SetMotion(UM_Stop);

	m_BloodType = 0;
	m_dwTorchlight = 0;

	//SetPosition(CPoint(320, 240));

	//m_Position = pos; 
	//m_ptCellPos = PixelToCellUnit(m_Position);
	//SetDstPos(pos); 
	m_ptMoveStep.x = 0;
	m_ptMoveStep.y = 0;

	INIT_PTR(m_bpSprData);
	INIT_PTR(m_wpV);
//	INIT_PTR(m_wpV1);
//	INIT_PTR(m_wpV2);
//	INIT_PTR(m_wpS);
//	INIT_PTR(m_wpFX);

	m_bFocus = FALSE;
	m_bPauseChar = FALSE;
	m_bCellArrive = FALSE;
	m_bCellChange = FALSE;// 셀 이동을 했는지 안했는지...

	m_nMotionStep = 0;
	m_bPoisoning = FALSE;
	m_dwPoisonTime = 0;
	m_iUserId = -1;				//Invalid Status
	m_Me = 0;// 나라면 1..
	m_nPoisonKind = 0;
	m_nFocusColor = 3;

	m_dwAttackTick = 0;
	m_dwBeatedTick = 0;
	m_dwMagicTick = 0;
	m_dwDieTick = 0;
	m_dwMoveTick = 0;
	m_FaTickCount = 0;

	INIT_PTR(m_pRoute);
	INIT_PTR(m_pMagic);

	m_nMagicType = MAGIC_TYPE_UP;
	m_pArmature = new CArmature;

	m_vInvCount = 0;
	for(int i = 0; i < INV_COUNT; i++)
	{
		m_MyInvData[i].Init();	// 인벤토리 데이타 초기화...!!
	}
	for(i = 0; i < BELT_COUNT; i++)
	{
		m_MyBeltData[i].Init();
	}

	m_pMagic = new CMagic;//dspring_0719
	m_bDeadFlag = FALSE;
	m_bDying = FALSE;
	m_vIdColor = RGB(255,255,0);

	for(i=0; i <AM_KIND_NUM; i++) m_MeDraw_Order[i] = 0;//우석..

	m_vMoveStep = 0;
	m_rCurCharRect.SetRectEmpty();
	m_prediff = 0;

	m_bFa = false;

//	m_HpRecoveryTime = 0;
//	m_HpTickCount = 0;
//	m_MpRecoveryTime = 0;
//	m_MpTickCount = 0;
}

CUser::~CUser()
{
	if (m_bpCharManager && m_bpCharRes) m_bpCharManager->UnloadCharRes(m_iCharResIndex);
	if(m_pRoute) { m_pRoute->RemoveAll(); SAFE_DELETE(m_pRoute); }
	SAFE_DELETE(m_pMagic);
	SAFE_DELETE(m_pArmature);
	//dspring_0718_start
	CMagic *pMagic;
	for( int i = 0; i < m_arrMagic.GetSize() ; i++ )
	{
		pMagic = m_arrMagic.GetAt(i);
		SAFE_DELETE( pMagic );
	}
	m_arrMagic.RemoveAll();
	//dspring_0718_end
	int number;
	CMyMagicData *magic_data;
	POSITION pos = m_mapMyMagicData.GetStartPosition();
	while(pos)
	{
		m_mapMyMagicData.GetNextAssoc(pos, number, magic_data);
		delete magic_data;
	}
	m_mapMyMagicData.RemoveAll();
	m_arrSpecialMagic.RemoveAll();
	m_arrBlueMagic.RemoveAll();
	m_arrWhiteMagic.RemoveAll();
	m_arrBlackMagic.RemoveAll();
	
	m_nMagicIndex.RemoveAll();
	m_arrFlyEndMagic.RemoveAll();
}
void CUser::SetFlyEndMagic(int index, BOOL bType, int nTableNum, int nPoisonKind)
{
	FLY_END_MAGIC stFlyEndMagic;
	stFlyEndMagic.m_nMagicIndex = index;
	stFlyEndMagic.m_bType = bType;
	stFlyEndMagic.m_nPoisonKind = nPoisonKind;
	stFlyEndMagic.m_nTableNum = nTableNum;
	m_arrFlyEndMagic.Add(stFlyEndMagic); 
}

void CUser::SetCharRes(int index, int nChar)
{
	//index값으로 부터 몸이 남자몸인지 여자몸인지 흑인인지 백인인지 황인인지를 알아낸다.
	//(재덕)

	if (NULL==m_bpCharManager) return;

	m_bpCharManager->LoadCharRes(index);// 몸을 읽어 들인다..음..
	m_bpCharRes = m_bpCharManager->GetIndexedCharRes(index);

	m_iCharResIndex = index;
	m_nChar = nChar;		//Resource상에 몇번째 캐릭터?(사용하지 않을 듯...)
}

void CUser::RenderChar(CDDSurface* pSurface, CPoint PosMapOffset)
{
	if (NULL==m_bpCharManager) return;
	if (NULL==m_bpCharRes) return;
	if (NULL==m_bpSprData) return;

	CPoint ptScreen = PosMapOffset;

	RECT rcV = m_bpSprData->rcV;	//캐릭터 영역1		(lgt3에서 Sprite부분참고)

	int alpha;

	if(m_nMagicType== MAGIC_TYPE_NOTALPHA_UP || m_nMagicType== MAGIC_TYPE_NOTALPHA_DOWN)
		alpha = 0;
	else 
		alpha = 1;
/*
	//캐릭터 아래 매직인 경우 (재덕)
	if (m_pMagic && (m_nMagicType==MAGIC_TYPE_DOWN || m_nMagicType== MAGIC_TYPE_NOTALPHA_DOWN) && m_pMagic->DrawMagic(pSurface, ptScreen.x, ptScreen.y - 40, alpha))
	{
		SAFE_DELETE(m_pMagic);
	}
*/
	m_pMagic->Draw3DMagic(ptScreen.x, ptScreen.y);//dspring_0803_3dxx
	
//	if(m_Direction >= 2 && m_Direction <= 6)// ||m_Direction ==0 ||m_Direction ==0 ||m_Direction ==0 )
//		m_pMagic->DrawMagic(pSurface, ptScreen.x, ptScreen.y-35, alpha);
	m_pMagic->DrawMagic(pSurface, ptScreen.x, ptScreen.y, /*alpha*/1);
	RenderBodyAndArmature(pSurface, ptScreen, rcV);
//	if(m_Direction < 2 && m_Direction > 6)
//		m_pMagic->DrawMagic(pSurface, ptScreen.x, ptScreen.y-40, alpha,TRUE);
	m_pMagic->DrawMagic(pSurface, ptScreen.x, ptScreen.y, /*alpha*/1,TRUE);
	m_pMagic->Draw3DMagic(ptScreen.x, ptScreen.y, TRUE);
/*
	//캐릭터 위에 덮는 매직인 경우
	if (m_pMagic &&(m_nMagicType==MAGIC_TYPE_UP  || m_nMagicType== MAGIC_TYPE_NOTALPHA_UP) && m_pMagic->DrawMagic(pSurface, ptScreen.x, ptScreen.y-40, alpha))
	{
		SAFE_DELETE(m_pMagic);
	}

	// 루프를 도는 마법...
	if (m_pMagic && m_nMagicType==MAGIC_TYPE_LOOP && m_pMagic->DrawMagic(pSurface, ptScreen.x, ptScreen.y+6))
	{
		SAFE_DELETE(m_pMagic);
	}
	//Focus상태 해제.
*/
}

//void CUser::RenderBodyAndArmature(CDDSurface *pSurface, CPoint ptScreen, RECT rV1, RECT rV2)
void CUser::RenderBodyAndArmature(CDDSurface *pSurface, CPoint ptScreen, RECT rV1)
{
	//test
	RECT rArmature;
	static int nDO[7];
	const int AM_KIND_BODY = AM_KIND_ARMS+1;

//	int uid = m_iUserId;

	switch(m_Direction)
	{
	case UD_Down: //

		if( m_MeDraw_Order[AM_KIND_JEAN] == 1 && m_MeDraw_Order[AM_KIND_ARMOR] >= 1)
		{			
			nDO[0] = AM_KIND_BODY;//BODY
			nDO[1] = AM_KIND_BOOT;			
			nDO[2] = AM_KIND_ARMOR;
			nDO[3] = AM_KIND_JEAN;
			nDO[4] = -1;
			nDO[5] = AM_KIND_SHIELD;
			nDO[6] = AM_KIND_ARMS;
			break;
		}
		else
		{
			nDO[0] = AM_KIND_BODY;//BODY
			nDO[1] = AM_KIND_JEAN;
			nDO[2] = AM_KIND_BOOT;
			nDO[3] = AM_KIND_ARMOR;
			nDO[4] = AM_KIND_HELMET;
			nDO[5] = AM_KIND_SHIELD;
			nDO[6] = AM_KIND_ARMS;
		}

		switch(m_MeDraw_Order[AM_KIND_JEAN])
		{		
		case 1: nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_ARMOR; nDO[2] = AM_KIND_BOOT;
				nDO[3] = AM_KIND_JEAN; nDO[4] = AM_KIND_HELMET;	nDO[5] = AM_KIND_SHIELD;
				nDO[6] = AM_KIND_ARMS;
			    break;
		case 0:
		default: break;
				
		}

		switch(m_MeDraw_Order[AM_KIND_ARMOR])
		{
		case 1: nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_JEAN; nDO[2] = AM_KIND_BOOT;
				nDO[3] = AM_KIND_ARMOR; nDO[4] = AM_KIND_HELMET; nDO[5] = AM_KIND_SHIELD;
				nDO[6] = AM_KIND_ARMS;
			    break;
		case 2: nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_JEAN; nDO[2] = AM_KIND_BOOT;
				nDO[3] = AM_KIND_ARMOR; nDO[4] = -1; nDO[5] = AM_KIND_SHIELD;
				nDO[6] = AM_KIND_ARMS;
			    break;
		case 3: nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_JEAN; nDO[2] = AM_KIND_BOOT;
				nDO[3] = AM_KIND_ARMOR; nDO[4] = -1; nDO[5] = AM_KIND_SHIELD;
				nDO[6] = AM_KIND_ARMS;
			    break;
		case 0:
		default: break;
		}

/*		nDO[0] = AM_KIND_BODY;//BODY
		nDO[1] = AM_KIND_JEAN;
		nDO[2] = AM_KIND_BOOT;
		nDO[3] = AM_KIND_ARMOR;
		nDO[4] = AM_KIND_HELMET;
		nDO[5] = AM_KIND_SHIELD;
		nDO[6] = AM_KIND_ARMS;
*/
		break;
	case UD_Up://

		if( m_MeDraw_Order[AM_KIND_JEAN] == 1 && m_MeDraw_Order[AM_KIND_ARMOR] >= 1)
		{
			nDO[0] = AM_KIND_BODY;//BODY			
			nDO[1] = AM_KIND_BOOT;
			nDO[2] = AM_KIND_ARMOR;
			nDO[3] = AM_KIND_JEAN;
			nDO[4] = AM_KIND_ARMS;
			nDO[5] = AM_KIND_SHIELD;			
			nDO[6] = -1;

			break;
		}
		else 
		{
			nDO[0] = AM_KIND_SHIELD;
			nDO[1] = AM_KIND_BODY;//BODY
			nDO[2] = AM_KIND_JEAN;
			nDO[3] = AM_KIND_BOOT;
			nDO[4] = AM_KIND_ARMOR;
			nDO[5] = AM_KIND_ARMS;
			nDO[6] = AM_KIND_HELMET;
		}

		switch(m_MeDraw_Order[AM_KIND_JEAN])
		{		
		case 1: nDO[0] = AM_KIND_SHIELD; nDO[1] = AM_KIND_BODY; nDO[2] = AM_KIND_ARMOR;
				nDO[3] = AM_KIND_BOOT; nDO[4] = AM_KIND_JEAN; nDO[5] = AM_KIND_ARMS;			
				nDO[6] = AM_KIND_HELMET;
			    break;
		case 0:
		default: break;
				
		}

		switch(m_MeDraw_Order[AM_KIND_ARMOR])
		{
		case 1:	nDO[0] = AM_KIND_SHIELD; nDO[1] = AM_KIND_BODY;//BODY
				nDO[2] = AM_KIND_JEAN; nDO[3] = AM_KIND_BOOT; nDO[4] = AM_KIND_ARMOR; nDO[5] = AM_KIND_ARMS;
				nDO[6] = AM_KIND_HELMET;
			    break;
		case 2: nDO[0] = AM_KIND_SHIELD; nDO[1] = AM_KIND_BODY;//BODY
				nDO[2] = AM_KIND_JEAN; nDO[3] = AM_KIND_BOOT; nDO[4] = AM_KIND_ARMOR; nDO[5] = AM_KIND_ARMS;
				nDO[6] = -1;
			    break;
		case 3: nDO[0] = AM_KIND_SHIELD; nDO[1] = AM_KIND_BODY;//BODY
				nDO[2] = AM_KIND_JEAN; nDO[3] = AM_KIND_BOOT; nDO[4] = AM_KIND_ARMOR; nDO[5] = AM_KIND_ARMS;
				nDO[6] = -1;
			    break;
		case 0:
		default: break;
		}
		break;

	case UD_DownLeft:
	case UD_Left:
	case UD_UpLeft:
		if( m_MeDraw_Order[AM_KIND_JEAN] == 1 && m_MeDraw_Order[AM_KIND_ARMOR] >= 1)
		{
			nDO[0] = AM_KIND_BODY;//BODY
			nDO[1] = AM_KIND_BOOT;
			nDO[2] = AM_KIND_ARMOR;
			nDO[3] = AM_KIND_JEAN;		
			nDO[4] = AM_KIND_ARMS;
			nDO[5] = -1;
			nDO[6] = AM_KIND_SHIELD;

			break;
		}
		else 
		{
			if(m_Arm_type == BOW)
			{
				nDO[0] = AM_KIND_BODY;//BODY
				nDO[1] = AM_KIND_JEAN;
				nDO[2] = AM_KIND_ARMS;
				nDO[3] = AM_KIND_BOOT;
				nDO[4] = AM_KIND_ARMOR;				
				nDO[5] = AM_KIND_HELMET;
				nDO[6] = AM_KIND_SHIELD;
			}
			else
			{
				nDO[0] = AM_KIND_BODY;//BODY
				nDO[2] = AM_KIND_JEAN;
				nDO[3] = AM_KIND_BOOT;
				nDO[4] = AM_KIND_ARMOR;
				nDO[1] = AM_KIND_ARMS;
				nDO[5] = AM_KIND_HELMET;
				nDO[6] = AM_KIND_SHIELD;
			}
		}

		switch(m_MeDraw_Order[AM_KIND_JEAN])
		{		
		case 1: nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_ARMOR; nDO[2] = AM_KIND_BOOT;
				nDO[3] = AM_KIND_JEAN; nDO[4] = AM_KIND_ARMS; nDO[5] = AM_KIND_HELMET;
				nDO[6] = AM_KIND_SHIELD;
			    break;
		case 0:
		default: break;
				
		}

		if(m_Arm_type == BOW)
		{
			switch(m_MeDraw_Order[AM_KIND_ARMOR])
			{

			case 1:	nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_JEAN; nDO[3] = AM_KIND_BOOT;
					nDO[4] = AM_KIND_ARMOR; nDO[2] = AM_KIND_ARMS; nDO[5] = AM_KIND_HELMET;
					nDO[6] = AM_KIND_SHIELD;
					break;
			case 2:	nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_JEAN; nDO[3] = AM_KIND_BOOT;
					nDO[4] = AM_KIND_ARMOR; nDO[2] = AM_KIND_ARMS; nDO[5] = -1;
					nDO[6] = AM_KIND_SHIELD;
					break;
			case 3: nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_JEAN; nDO[3] = AM_KIND_BOOT; 
					nDO[4] = AM_KIND_ARMOR; nDO[2] = AM_KIND_ARMS; nDO[5] = -1;
					nDO[6] = AM_KIND_SHIELD;
					break;
			case 0:
			default: break;
			}
		}
		else
		{
			switch(m_MeDraw_Order[AM_KIND_ARMOR])
			{

			case 1:	nDO[0] = AM_KIND_BODY; nDO[2] = AM_KIND_JEAN; nDO[3] = AM_KIND_BOOT;
					nDO[4] = AM_KIND_ARMOR; nDO[1] = AM_KIND_ARMS; nDO[5] = AM_KIND_HELMET;
					nDO[6] = AM_KIND_SHIELD;
					break;
			case 2:	nDO[0] = AM_KIND_BODY; nDO[2] = AM_KIND_JEAN; nDO[3] = AM_KIND_BOOT;
					nDO[4] = AM_KIND_ARMOR; nDO[1] = AM_KIND_ARMS; nDO[5] = -1;
					nDO[6] = AM_KIND_SHIELD;
					break;
			case 3: nDO[0] = AM_KIND_BODY; nDO[2] = AM_KIND_JEAN; nDO[3] = AM_KIND_BOOT; 
					nDO[4] = AM_KIND_ARMOR; nDO[1] = AM_KIND_ARMS; nDO[5] = -1;
					nDO[6] = AM_KIND_SHIELD;
					break;
			case 0:
			default: break;
			}
		}

		break;
	case UD_UpRight:
	case UD_Right:
	case UD_DownRight:
		if( m_MeDraw_Order[AM_KIND_JEAN] == 1 && m_MeDraw_Order[AM_KIND_ARMOR] >= 1)
		{			
			nDO[0] = AM_KIND_BODY;//BODY
			nDO[1] = AM_KIND_BOOT;
			nDO[2] = AM_KIND_ARMOR;
			nDO[3] = AM_KIND_JEAN;			
			nDO[4] = -1;
			nDO[5] = AM_KIND_ARMS;
			nDO[6] = AM_KIND_SHIELD;

			break;
		}
		else 
		{			
			nDO[0] = AM_KIND_BODY;//BODY
			nDO[1] = AM_KIND_JEAN;
			nDO[2] = AM_KIND_BOOT;
			nDO[3] = AM_KIND_ARMOR;
			nDO[4] = AM_KIND_HELMET;
			nDO[5] = AM_KIND_ARMS;
			nDO[6] = AM_KIND_SHIELD;
		}

		switch(m_MeDraw_Order[AM_KIND_JEAN])
		{		
		case 1: nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_ARMOR;
				nDO[2] = AM_KIND_BOOT; nDO[3] = AM_KIND_JEAN; nDO[4] = AM_KIND_HELMET;
				nDO[5] = AM_KIND_ARMS; nDO[6] = AM_KIND_SHIELD; 
			    break;
		case 0:
		default: break;
				
		}

		switch(m_MeDraw_Order[AM_KIND_ARMOR])
		{
		case 1:	nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_JEAN;
				nDO[2] = AM_KIND_BOOT; nDO[3] = AM_KIND_ARMOR; nDO[4] = AM_KIND_HELMET;
				nDO[5] = AM_KIND_ARMS; nDO[6] = AM_KIND_SHIELD;
			    break;
		case 2:	nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_JEAN;
				nDO[2] = AM_KIND_BOOT; nDO[3] = AM_KIND_ARMOR; nDO[4] = -1;
				nDO[5] = AM_KIND_ARMS; nDO[6] = AM_KIND_SHIELD; 
			    break;
		case 3: nDO[0] = AM_KIND_BODY; nDO[1] = AM_KIND_JEAN;
				nDO[2] = AM_KIND_BOOT; nDO[3] = AM_KIND_ARMOR; nDO[4] = -1;
				nDO[5] = AM_KIND_ARMS; nDO[6] = AM_KIND_SHIELD; 
			    break;
		case 0:
		default: break;
		}
		break;
	}

	// 그림자..위치..
	if(m_pArmature->m_pSprData[AM_KIND_SH] != NULL) 
	{
		rArmature = m_pArmature->m_pSprData[AM_KIND_SH]->rcV;
		if(m_pArmature->m_pSprData[AM_KIND_SH] != NULL)
			BltLSPShadowEx(pSurface, ptScreen.x, ptScreen.y, &rArmature, m_pArmature->m_pV1Data[AM_KIND_SH], CChildView::DirectXMng.m_dwHalf16Mask, CChildView::DirectXMng.m_dwHalf32Mask );
			//BltLSPShadow(pSurface, ptScreen.x, ptScreen.y, &rArmature, m_pArmature->m_pV1Data[AM_KIND_SH], 16);//FALSE);
	}

	for (int i=0; i<7; i++)
	{
		if (nDO[i]==AM_KIND_BODY) //BODY
		{
			if(m_wpV !=NULL)
				BltLSPNormal(pSurface, ptScreen.x,   ptScreen.y, &rV1, m_wpV, m_bFocus, m_nPoisonKind, m_nFocusColor);//FALSE);
		}
		else
		{
			if(m_iUserId < 10000) // npc일 경우 생략.. 우석..
			{
				if(m_BattleMode != 0 || (nDO[i]!=AM_KIND_ARMS && nDO[i]!=AM_KIND_SHIELD))// 배틀모드가 0일때는 무기를 들고있지 았게 하기위해..
				{
					if(m_pArmature->m_pV1Data[nDO[i]] == NULL)
					{
						//08.27
						// 로드를 한다..단 index가 0이 아니어야한다.
						// 에니메이션 번호와 종류를 알아야 한다ae.
						// m_AnimationIndex m_AniSprNumber m_nType					
						switch(nDO[i])
						{
						case AM_KIND_SHIELD:
							if(m_pArmature->m_nShield);//로딩						
							break;
						case AM_KIND_JEAN:
							if(m_pArmature->m_nJean)
							{
								//m_pArmature->AddLoadArmature(
								//m_pArmature->ReadyData(....);
							}
							break;
						case AM_KIND_BOOT:
							if(m_pArmature->m_nBoot);
							break;
						case AM_KIND_ARMOR:
							if(m_pArmature->m_nArmor);
							break;
						case AM_KIND_HELMET:
							if(m_pArmature->m_nHelmet);
							break;
						case AM_KIND_ARMS:
							if(m_pArmature->m_nArms);
							break;
						}
						//m_pArmature->ReadyData(....);// 한번만 해주면 된다.
					}
					if(nDO[i] != -1 && m_pArmature->m_pSprData[nDO[i]] && m_pArmature->m_pV1Data[nDO[i]]) 
					{					
						rArmature = m_pArmature->m_pSprData[nDO[i]]->rcV;					
						BltLSPNormal(pSurface, ptScreen.x, ptScreen.y, &rArmature, m_pArmature->m_pV1Data[nDO[i]], m_bFocus, m_nPoisonKind, m_nFocusColor);//FALSE);
					}
				}
			}
		}
	}
}

//연결되어 있는 Resource에서 LSP데이터 가져오기
//BOOL CUser::GetSpr(_SPR_LSP** spr, PWORD* pV1, PWORD* pV2, PWORD* pS, PWORD* pFX)
BOOL CUser::GetSpr(_SPR_LSP** spr, PWORD* pV)
{
	int frame;
	SPR_DATA *spr_data;

	spr_data = m_bpCharRes->GetSprData(m_AniSprNumber);
	if(spr_data == NULL)
	{
		TRACE1("Character Resource(%d)가 없음\n", m_AniSprNumber);
		*spr = NULL;
		*pV = NULL;
		return FALSE;
	}

	// 프레임이 끝에 도달했으면.. 
	if (spr_data->m_AniData.nMaxFrm) 
	{
		if(m_bDead == TRUE) m_iSpriteIndex = spr_data->m_AniData.nMaxFrm-1;
		else if(m_iSpriteIndex == spr_data->m_AniData.nMaxFrm)
		{
			if(m_AniSprNumber == 15)
			{
				m_iSpriteIndex = spr_data->m_AniData.nMaxFrm-1;
				m_bDead = TRUE;
			}
			else if(m_AnimationIndex == UM_Stop || m_AnimationIndex == UM_Walk || m_AnimationIndex == UM_RUN)
			{
				m_iSpriteIndex = 0;
			}
			else
			{
				m_iSpriteIndex = spr_data->m_AniData.nMaxFrm-1;
			}
		}
		else m_iSpriteIndex %= spr_data->m_AniData.nMaxFrm;
	}
	else m_iSpriteIndex = 0;

	//무기장착 그릴 준비
	m_pArmature->ReadyData(m_AniSprNumber, m_iSpriteIndex, m_Direction);

	if (m_bPauseChar)
	{
		int ii = m_Direction * spr_data->m_AniData.nMaxFrm + m_iSpriteIndex;
		if(ii < 0 || ii >= spr_data->m_HAni.nDirQt * spr_data->m_AniData.nMaxFrm)
		{
			return FALSE;
		}
		frame = spr_data->m_AniData.pnFrms[ii];
		if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
		{
			return FALSE;
		}
		*spr = &spr_data->m_arrSprData[frame];
		*pV = spr_data->m_arrVData[frame];

		m_pArmature->ReadyData(m_AniSprNumber, m_iSpriteIndex, m_Direction);
		return FALSE;
	}

	DWORD dwTick = GetTickCount();
	BOOL bFrameChange = FALSE;
	// 시간이 지나면 다음 프레임 돌려줌..
	if (dwTick > m_dwTick + 1000 / spr_data->m_AniData.fSpd)
	{
		m_dwTick = dwTick;
		int ii = m_Direction * spr_data->m_AniData.nMaxFrm + m_iSpriteIndex;
		if(ii < 0 || ii >= spr_data->m_HAni.nDirQt * spr_data->m_AniData.nMaxFrm)
		{
			return FALSE;
		}
		frame = spr_data->m_AniData.pnFrms[ii];
		if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
		{
			return FALSE;
		}
		m_iSpriteIndex++;
		bFrameChange = TRUE;
	}
	else
	{
		int ii = m_Direction * spr_data->m_AniData.nMaxFrm + m_iSpriteIndex;
		if(ii < 0 || ii >= spr_data->m_HAni.nDirQt * spr_data->m_AniData.nMaxFrm)
		{
			return FALSE;
		}
		frame = spr_data->m_AniData.pnFrms[ii];
		if(frame < 0 || frame >= spr_data->m_HSpr.nQt)
		{
			return FALSE;
		}
	}
	*spr = &spr_data->m_arrSprData[frame];
	*pV = spr_data->m_arrVData[frame];

	return bFrameChange;
}

//Mp 회복...
/*
void CUser::MpRecovery()
{
	//mp회복..
	if(m_MpRecoveryTime == 0) m_MpTickCount = ::GetTickCount();
	if(m_Cur_Weight > (m_Max_Weight >> 1))
	{
		if(m_BattleMode) //무장 상태일경우..
		{
			m_MpRecoveryTime = (float(m_Cur_Weight - (m_Max_Weight >> 1)) / (m_Max_Weight >> 1)) * 
							 (LATE_RECOVERY_TIME - FAST_RECOVERY_TIME) + FAST_RECOVERY_TIME;	
		}
		else //무장 해제일 경우..
		{
			m_MpRecoveryTime = (float(m_Cur_Weight - (m_Max_Weight >> 1)) / (m_Max_Weight >> 1)) * 
							 (LATE_RECOVERY_TIME - FAST_RECOVERY_TIME2) + FAST_RECOVERY_TIME2;	
		}
	}
	else
	{
		if(m_BattleMode) m_MpRecoveryTime = 15000;
		else			 m_MpRecoveryTime = 10000;
	}

	DWORD ThisTick = ::GetTickCount();
	if(ThisTick - m_MpTickCount >= m_MpRecoveryTime)
	{
		m_Cur_Mp += 3 + (m_Wis - 10);
		if(m_Cur_Mp > m_Max_Mp)	m_Cur_Mp = m_Max_Mp;

		m_MpTickCount = ThisTick;
	}		
}
*/

//Hp회복..
/*
void CUser::HpRecovery()
{
	//hp회복..
	if(m_HpRecoveryTime == 0) m_HpTickCount = ::GetTickCount();
	if(m_Cur_Weight > (m_Max_Weight >> 1))
	{
		if(m_BattleMode) //무장 상태일경우..
		{
			m_HpRecoveryTime = (float(m_Cur_Weight - (m_Max_Weight >> 1)) / (m_Max_Weight >> 1)) * 
							 (LATE_RECOVERY_TIME - FAST_RECOVERY_TIME) + FAST_RECOVERY_TIME;	
		}
		else //무장 해제일 경우..
		{
			m_HpRecoveryTime = (float(m_Cur_Weight - (m_Max_Weight >> 1)) / (m_Max_Weight >> 1)) * 
							 (LATE_RECOVERY_TIME - FAST_RECOVERY_TIME2) + FAST_RECOVERY_TIME2;	
		}
	}
	else
	{
		if(m_BattleMode) m_HpRecoveryTime = 15000;
		else			 m_HpRecoveryTime = 10000;
	}

	DWORD ThisTick = ::GetTickCount();
	if(ThisTick - m_HpTickCount >= m_HpRecoveryTime)
	{
		m_Cur_Hp += 3 + (m_Wis - 10);
		if(m_Cur_Hp > m_Max_Hp)	m_Cur_Hp = m_Max_Hp;

		m_HpTickCount = ThisTick;
	}		
}
*/

void CUser::Move()
{
//	BOOL bFrameChange = GetSpr(&m_bpSprData, &m_wpV1, &m_wpV2, &m_wpS, &m_wpFX);
	BOOL bFrameChange = GetSpr(&m_bpSprData, &m_wpV);

	if(m_bpSprData == NULL || m_wpV == NULL) return;
//	m_rCurCharRect = m_bpSprData->rcS;
	// 임시..방식이 틀린거 같음..느낌이..일단..캐릭터를 뿌려주는 부분에드ㅏ가 다시 만들어야지..이것을..
//	m_rCurCharRect.UnionRect(&m_rCurCharRect, &m_bpSprData->rcV);
	m_rCurCharRect = m_bpSprData->rcV;

//	m_rCurCharRect.UnionRect(&m_rCurCharRect, &m_bpSprData->rcV1);
//	m_rCurCharRect.UnionRect(&m_rCurCharRect, &m_bpSprData->rcV2);

	//Hp 회복...
//	if(m_Me && m_Cur_Hp < m_Max_Hp) HpRecovery();
	//Mp 회복...
//	if(m_Me && m_Cur_Mp < m_Max_Mp) MpRecovery();
	
	switch(m_AnimationIndex)
	{
	case UM_Walk: 
		MoveNextStep_New2();
		
		if (m_ptDstPos==m_Position) 
			m_nMotionStep = 0;

		if (m_ptDestFinalPos==m_Position && m_pRoute->IsEmpty()) 
		{
			SetMotion(UM_Stop);
			m_nMotionStep = 0;

			m_FaTickCount = ::GetTickCount();

			if(m_Me)
				TRACE("m_Position: %d , %d \n",PixelToCellUnit(m_Position).x,PixelToCellUnit(m_Position).y);
		}
		CChildView::UserManager.DeleteMagic(this->m_iUserId);
			break;
	case UM_RUN:
		m_bFa = false;
		MoveNextStep_New2();

		if (m_ptDstPos==m_Position) 
			m_nMotionStep = 0;

		if(m_Position == m_ptDestFinalPos && m_pRoute->IsEmpty())
		{
			SetMotion(UM_Stop);
			m_nMotionStep = 0;

			m_FaTickCount = ::GetTickCount();
		}		
		CChildView::UserManager.DeleteMagic(this->m_iUserId);
		 break;
	case UM_Attack:
	case UM_Beated:
	case UM_Magic:
		if (0==m_nMotionStep)
		{
			SetMotion(m_prevAnimation);	//동작의 표현이 끝난경우 Stop상태로함
		}
		break;
	case UM_Stop:
		SetMotion(UM_Stop);
		if(m_Me)
		{
			int   time_gap;
			DWORD tick = ::GetTickCount();
			if(m_bFa) time_gap = tick - m_FaTickCount;
			else      time_gap = tick - m_FaTickCount - 1600;
			if(time_gap >= 0) { if(!m_bFa) m_FaTickCount = tick; m_bFa = true; }
			else              break;
			if(m_Cur_Fa < m_Max_Fa)
			{
				if(time_gap >= 1000)
				{					
					m_Cur_Fa += int(time_gap/1000) * 5;								
					m_FaTickCount = tick;
					TRACE("timegap = %d , m_Cur_Fa = %d\n", time_gap, m_Cur_Fa);
				}
				if(m_Cur_Fa > m_Max_Fa) m_Cur_Fa = m_Max_Fa;
			}
			else m_FaTickCount = tick;
		}
		break;
	}

//	if (m_AnimationIndex==UM_Walk) MoveNextStep_New2();

	if (bFrameChange)	//그림이 바뀌었음
	{
		//if (m_AnimationIndex==UM_Walk) MoveNextStep_New();
		if ((m_AnimationIndex!=UM_Walk && m_AnimationIndex!=UM_Stop) && m_nMotionStep>0) m_nMotionStep--;
	}
}

void CUser::MoveNextStep_New2()
{	//이곳에서 새롭게 체크를 한다.
	m_FaTickCount = ::GetTickCount();

	DWORD tick = ::GetTickCount();	
	int diff = tick - m_dwnewMoveTick;

	//시간 갭이 너무클경우 보정해준다.
/*	if( diff - m_prediff > 70 )
	{
		diff = m_prediff + 70;
		m_dwnewMoveTick = m_prediff + 70;
		TRACE("시간차이가 넘 많아보정했음..\n");
	}
*/	m_prediff = diff;

	if(m_pRoute == NULL) return;
/*
	if(m_Me != 1)
	{
		CPoint  pt;
		if(!m_pRoute->IsEmpty() && (m_ptDestFinalPos == m_Position))
		{
			pt = m_pRoute->RemoveHead();
		
			m_ptPostCellPos = pt;//m_ptDestCellPos;
			m_ptDstPos = CellUnitToPixel(m_ptPostCellPos);// 목적지 픽셀 값을 집어 넣는다.
		}
	}
*/
	int uid = GetUserId();
	int standard;

	if(m_Run == 2)
	{
		if(m_iUserId < 10000) standard = 500;
		else     standard = 750;
	}
	else
	{
		standard = 750;
	}
	if( !m_Me && m_pRoute->GetCount() >= 4)
	{
		standard = 600;
		TRACE("이동속도를 빠르게................\n");
	}


	if(diff >= standard)
	{	
		if(m_Run == 2)// 뛸때..
		{
			if(m_BattleMode != 0)
			{
				m_Run = 1;
				SetMotion(CUser::UM_Walk);
				MoveNextStep_New2();
				return;
			}

			CPoint nextpt;
			CPoint CurentCell = PixelToCellUnit(m_Position);
			m_dwnewMoveTick = ::GetTickCount();

			m_ptStartCellPos = CurentCell;//현재에 위치 하고있는 셀의 값을 집어 넣는다.
			if(m_pRoute->GetCount() != 0)
			{				
				CPoint pRemove = m_pRoute->RemoveHead();// 내가 움직일 곳...			
				m_ptPostCellPos = pRemove;
				m_ptDstPos = CellUnitToPixel(m_ptPostCellPos);

				TRACE("NPC가이동하려는점..[%d, %d]\n",m_ptPostCellPos.x,m_ptPostCellPos.y);
				if(m_pRoute->IsEmpty()) nextpt = CPoint(-1,-1);					
				else					nextpt  = m_pRoute->GetHead();
					

				if(m_Me) SendNextRunMoveReq(m_ptPostCellPos, nextpt);
			}
			else { m_Run = 1; return; }
		}
		else if(m_Run == 1)// 걸을때..
		{
			//////////////////////
			if(m_Me == 1)//나일때..
			{
				//m_Position; // 현재 위치
				CPoint CurentCell = PixelToCellUnit(m_Position);
				CPoint CurentDestpos = CellUnitToPixel(m_ptDestCellPos);// 목적지 픽셀값
				m_dwnewMoveTick = ::GetTickCount();

				m_ptStartCellPos = CurentCell;//현재에 위치 하고있는 셀의 값을 집어 넣는다.
				if(m_pRoute->GetCount() != 0)
				{				
					CPoint pRemove = m_pRoute->RemoveHead();// 내가 움직일 곳...			
					m_ptPostCellPos = pRemove;
					SendNextMoveReq(m_ptPostCellPos);
				}
				else
					return;
			}
			else// 다른유저 또는 npc
			{
				CPoint CurentCell = PixelToCellUnit(m_Position);
				m_ptStartCellPos = CurentCell;//현재에 위치 하고있는 셀의 값을 집어 넣는다.

				CPoint CurentDestpos;
				if(!m_pRoute->IsEmpty()) 
				{
					if(m_bpUserManager->m_pPathFind->SetMovable(m_ptPostCellPos,true) == FALSE)
					{
						TRACE("좌표가 이상하다...1\n");
						return;
					}
					
					CurentDestpos = m_pRoute->RemoveHead();// 목적지 셀값
					m_ptPostCellPos = m_ptDestCellPos = CurentDestpos;
					m_ptDstPos = CellUnitToPixel(m_ptDestCellPos);// 목적지 픽셀값					
				}					
				m_dwnewMoveTick = ::GetTickCount();
			}
		}
	}//if(diff > standard)
	else// 이동 시간 안에...
	{
		CPoint  cell_gap = m_ptStartCellPos - m_ptPostCellPos;

		if( m_Me != 1 && (abs(cell_gap.x) >=6  || abs(cell_gap.y) >=6) )
		{
			m_Position = m_ptDstPos = CellUnitToPixel(m_ptPostCellPos); //내가 아니면..
//------------------------------------------------------------------
			CPoint CurentCell = PixelToCellUnit(m_Position);

			m_ptStartCellPos = CurentCell;//현재에 위치 하고있는 셀의 값을 집어 넣는다.
			m_ptPostCellPos = m_ptDestCellPos = CurentCell;

			CPoint CurentDestpos;
			if(!m_pRoute->IsEmpty()) 
			{
				if(m_bpUserManager->m_pPathFind->SetMovable(m_ptPostCellPos,true) == FALSE)
				{
					TRACE("좌표가 이상하다...2\n");
					return;
				}
				
				CurentDestpos = m_pRoute->RemoveHead();// 목적지 셀값
				m_ptPostCellPos = m_ptDestCellPos = CurentDestpos;
				m_ptDstPos = CellUnitToPixel(m_ptDestCellPos);// 목적지 픽셀값											
			}
			else if(m_ptDestFinalPos == m_Position) { SetMotion(UM_Stop); return; }
//------------------------------------------------------------------
			TRACE("셀차이나 넘 많이나서점프..\n");
		}	

		CPoint curPos = CellUnitToPixel(m_ptStartCellPos);// 출발한 셀의 픽셀 위치값..
		CPoint ptDstPos = CellUnitToPixel(m_ptPostCellPos);// 목적지 셀의 픽셀 위치값

		CPoint gap = ptDstPos - curPos;

		CPoint dest;
		dest.x = (gap.x*diff)/standard;
		dest.y = (gap.y*diff)/standard;


		int x_gap, y_gap;
		x_gap = (curPos.x + dest.x) - m_Position.x;
		y_gap = (curPos.y + dest.y) - m_Position.y;

		if( abs(x_gap) > 50 )      x_gap /= 6;
		else if( abs(x_gap) > 40 ) x_gap /= 4;
		else if( abs(x_gap) > 30 ) x_gap /= 3;
		else if( abs(x_gap) > 20 ) x_gap /= 2;
//		else x_gap = 0;

		if( abs(y_gap) > 50 ) y_gap /= 6;
		else if( abs(y_gap) > 40 ) y_gap /= 4;
		else if( abs(y_gap) > 30 ) y_gap /= 3;
		else if( abs(y_gap) > 20 ) y_gap /= 2;
//		else y_gap = 0;

		m_Position.x += x_gap;
		m_Position.y += y_gap;

//		m_Position.x = curPos.x + dest.x;
//		m_Position.y = curPos.y + dest.y;

		if(gap.x > 0)
		{
			m_dwTickPrev = 0;
			if(gap.y == 0)
				SetDirection(UD_Right);
			else if(gap.y > 0)
				SetDirection(UD_DownRight);
			else if(gap.y < 0)
			{
				SetDirection(UD_UpRight);
			}
		}
		else if(gap.x < 0)
		{
			m_dwTickPrev = 0;
			if(gap.y == 0)
				SetDirection(UD_Left);
			else if(gap.y > 0)
				SetDirection(UD_DownLeft);
			else if(gap.y < 0)
				SetDirection(UD_UpLeft);
		}
		else //gap.x == 0
		{
			if(gap.y == 0)
			{
/*				DWORD dwTick = ::GetTickCount();
				if(m_dwTickPrev == 0)
					m_dwTickPrev = ::GetTickCount();
				if(dwTick - m_dwTickPrev > 600)
				{	
#ifdef _DEBUG
					if(m_Me != 1 && uid > 10000)
						TRACE("따른 npc가 움직임을 시작했다.\n");
#endif
					SetMotion(UM_Stop); // 목적지까지 도착했을 경우..
					m_dwTickPrev = 0;
				}
*/
//=======================================================
				if(!m_Me && m_ptDestFinalPos != m_Position)
				{					
					CPoint CurentCell = PixelToCellUnit(m_Position);
					m_ptStartCellPos = CurentCell;//현재에 위치 하고있는 셀의 값을 집어 넣는다.

					CPoint CurentDestpos;
					if(!m_pRoute->IsEmpty()) 
					{
						//TRACE("목적지까지 와서 다음점을 찾았다..\n");
						if(m_bpUserManager->m_pPathFind->SetMovable(m_ptPostCellPos,true) == FALSE)
						{
							TRACE("좌표가 이상하다...3\n");
							return;
						}
						
						CurentDestpos = m_pRoute->RemoveHead();// 목적지 셀값
						m_ptPostCellPos = m_ptDestCellPos = CurentDestpos;
						m_ptDstPos = CellUnitToPixel(m_ptDestCellPos);// 목적지 픽셀값						
						m_dwnewMoveTick = ::GetTickCount();
					}
					else
					{
						SetMotion(UM_Stop); // 목적지까지 도착했을 경우..					
						//TRACE("종점이다......\n");
					}
				}
//=======================================================
			}
			else if(gap.y > 0)
			{
				m_dwTickPrev = 0;
				SetDirection(UD_Down);
			}
			else if(gap.y < 0)
			{
				m_dwTickPrev = 0;
				SetDirection(UD_Up);
			}
		}				
	
	}
}

///LSP 그리기에서 사용
BOOL CUser::GetClippedRect(RECT *pRC, RECT* pRCClip)
{
	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = g_vDispPixSize.cy;//m_bpDirectXMng->GetScreenSize().cy;

	BOOL bUseXClip = FALSE;
	*pRCClip = *pRC;

	if(pRC->left < 0)
	{ 
		pRCClip->left = 0;
		bUseXClip = TRUE;
	}
	else if(pRC->right > ScreenX)
	{ 
		pRCClip->right = ScreenX;
		bUseXClip = TRUE;
	}
	if(pRC->top < 0)
	{
		pRCClip->top = 0;
	}
	else if(pRC->bottom > ScreenY)
	{
		pRCClip->bottom = ScreenY;
	}
	return bUseXClip;
}

void CUser::SetDirection(int direction)//direction은 illstruct.h 에 define됨
{
	switch(direction)
	{
	case DIR_DOWN:
		SetDirection(UD_Down);
		break;
	case DIR_DOWN | DIR_LEFT:
		SetDirection(UD_DownLeft);
		break;
	case DIR_LEFT:
		SetDirection(UD_Left);
		break;
	case DIR_LEFT | DIR_UP:
		SetDirection(UD_UpLeft);
		break;
	case DIR_UP:
		SetDirection(UD_Up);
		break;
	case DIR_UP | DIR_RIGHT:
		SetDirection(UD_UpRight);
		break;
	case DIR_RIGHT:
		SetDirection(UD_Right);
		break;
	case DIR_RIGHT | DIR_DOWN:
		SetDirection(UD_DownRight);
		break;
	}
}

//Pixel좌표 단위의 이동 목적지 설정
void CUser::SetDstPos(CPoint ptDstPos)
{
//	TRACE ("SetDstPos함수 들어옴.\n");

//	ASSERT(ptDstPos.x%(XMOVE_GAP*m_Run)==0);
//	ASSERT(ptDstPos.y%(YMOVE_GAP*m_Run)==0);

	//if(m_ptDstPos.x >= 0)// 처음 들어 올때를 제외 하려고..초기화 전에 들어 오면..-값을 가진다.
	if(m_ptDstPos != m_Position)// 처음 들어 올때를 제외 하려고..초기화 전에 들어 오면..-값을 가진다.
	{
		if(m_Run == 2) SetMotion(UM_RUN);
		else           SetMotion(UM_Walk);
//		m_ptDstPos = ptDstPos;
//		m_ptDestCellPos = PixelToCellUnit(m_ptDstPos);
	}
	else SetMotion(UM_Stop);

//	m_ptMoveStep = m_ptDstPos - m_Position;
//	m_ptMoveStep.x /= XMOVE_GAP*m_Run;
//	m_ptMoveStep.y /= YMOVE_GAP*m_Run;
//	TRACE ("이동할 거리 (%d, %d)\n", m_ptMoveStep.x, m_ptMoveStep.y);
}

//Pixel좌표 단위로 캐릭터 위치 설정(Warp)
void CUser::SetPosition(CPoint pos)
{
	// 처음에 한번만 동작..모디파이에서..
	//WarpSet
	if(m_Me == 1)
	{
		CPoint old_pos, new_pos;

		old_pos = PixelToCellUnit(m_Position);
		new_pos = PixelToCellUnit(pos);
		TRACE("m_Position=[%d][%d], pos=[%d][%d]\n", old_pos.x,old_pos.y, new_pos.x, new_pos.y);
	}
	m_Position = pos; 
	m_ptDstPos = pos;
	m_ptDestFinalPos = pos;	

	m_ptCellPos = PixelToCellUnit(m_Position);
	SetDstPos(pos); 
	m_ptMoveStep.x = 0; 
	m_ptMoveStep.y = 0;
	m_ptStartCellPos = m_ptCellPos;// 출발한 셀의 픽셀 위치값..// 워프이므로 맞춰준다.
	m_ptPostCellPos = m_ptCellPos;// 목적지 셀의 픽셀 위치값   // 워프이므로 맞춰준다.	
//	m_ptPreMyCellPos = m_ptCellPos;
}

CPoint CUser::PixelToCellUnit(CPoint ptPixel)
{
	CPoint p = ptPixel;

	int X = 48;
	int Y = 24;

	int k1, k2;
	k1 = (int)ceil((.5*p.x+p.y-Y)/X);
	k2 = (int)ceil((-.5*p.x+p.y+Y)/X);
	
	k1 = X*k1 + Y;
	k2 = X*k2 - Y;
	
	p.x = k1 - k2;
	p.y = (k1+k2)/2-Y;
	
	p.x = int(p.x/24.+.5)*Y;
	p.y = int(p.y/24.+.5)*Y;
	
	p.x /= X;
	p.y /= Y;
	p.x--;
	p.y--;

	return p;
	
}

CPoint CUser::CellUnitToPixel(CPoint ptCell)
{
	CPoint ptPixel;
	int X = 48;
	int Y = 24;
	ptPixel.x = (ptCell.x+1)*X;
	ptPixel.y = (ptCell.y+1)*Y;
	//그리기의 기준위치 설정
//	ptPixel.x += 16;
//	ptPixel.y += 16;
	return ptPixel;
}
//Cell위치 기준으로 캐릭초기 위치설정(사용하면 Warp함)
void CUser::SetCellPosition(CPoint ptPosCell)
{
	CPoint ptPixel = CellUnitToPixel(ptPosCell);
	SetPosition(ptPixel);
}
//Cell위치 기준으로 한 목적지 설정
void CUser::SetCellDstPos(CPoint ptPosDstCell)
{
	CPoint ptPixel = CellUnitToPixel(ptPosDstCell);
	SetDstPos(ptPixel);
}

//우석.. 10월 11일..
CList<CPoint, CPoint>* CUser::CalcMoveList2(CPoint ptStart, CPoint ptEnd, int FindStep)
{
	CList<CPoint, CPoint>*	pRoute = new CList<CPoint, CPoint>;

	if (ptStart==ptEnd) 
	{
		TRACE("출발점과 목적지가 동일하다.\n");
		SAFE_DELETE(pRoute);
		return pRoute;
	}

	m_bpUserManager->m_pPathFind->ReSetFindStep(FindStep);
	//m_bpUserManager->m_pPathFind->FindPath2(pRoute, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y, 0, 0);
	m_bpUserManager->m_pPathFind->FindPath(pRoute, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);

	return pRoute;
}

//이동경로 탐색
CList<CPoint, CPoint>* CUser::CalcMoveList(CPoint ptStart, CPoint ptEnd, int Direction)
{
	if (ptStart==ptEnd) 
	{
		TRACE("출발점과 목적지가 동일하다.\n");
		return NULL;
	}

	CList<CPoint, CPoint>*	pRoute = new CList<CPoint, CPoint>;

	CPoint ptMap;

//	ptStart = CPacketParser::Conv2ServerMapPos(ptStart);
//	ptEnd = CPacketParser::Conv2ServerMapPos(ptEnd);
	CPoint pt = ptStart;

	CPoint ptGap = ptEnd - ptStart;
	int abx, aby;
	abx = abs(ptGap.x);
	aby = abs(ptGap.y);
		
/*	UD_Down = 0,
		UD_DownLeft,
		UD_Left,
		UD_UpLeft,
		UD_Up,
		UD_UpRight,
		UD_Right,
		UD_DownRight
	} m_Direction
*/
//	if(Direction == UD_DownLeft || Direction == UD_UpLeft || Direction == UD_UpRight || Direction == UD_DownRight)
	if(Direction == UD_Down || Direction == UD_Up || Direction == UD_Right || Direction == UD_Left)
	{
		while (abx!=0||aby!=0)
		{
			// 길찾기를 여기서 하면 되겠군...내가 앞으로 갈 셀의 위치를 결정...(재덕)
			if(abx > aby)
			{
				if (ptGap.x>0) pt.x +=2;
				else pt.x -=2;

				//ptMap = CPacketParser::Conv2ServerMapPos(pt);
				ptMap = pt;
				if(m_bpUserManager->m_pPathFind->IsMovable(ptMap.x, ptMap.y))
				//if (m_bpUserManager->IsMoveable(ptMap))
				{
					pRoute->AddTail(pt);
					abx-=2;
				}
				else break;
			}
			else if(aby > abx)
			{
				if (ptGap.y>0) pt.y +=2;
				else pt.y -=2;

				//ptMap = CPacketParser::Conv2ServerMapPos(pt);
				ptMap = pt;
				//if (m_bpUserManager->IsMoveable(ptMap))
				if(m_bpUserManager->m_pPathFind->IsMovable(ptMap.x, ptMap.y))
				{
					pRoute->AddTail(pt);
					aby-=2;
				}
				else break;
			}
			else
			{
				if (ptGap.x>0) pt.x+=1;
				else pt.x-=1;
				if (ptGap.y>0) pt.y+=1;
				else pt.y-=1;

				//ptMap = CPacketParser::Conv2ServerMapPos(pt);
				ptMap = pt;

				//if (m_bpUserManager->IsMoveable(ptMap))
				if(m_bpUserManager->m_pPathFind->IsMovable(ptMap.x, ptMap.y))
				{
					pRoute->AddTail(pt);
					abx-=1;
					aby-=1;
				}
				else break;
			}
		}
	}
	else
	{
		while (abx!=0||aby!=0)
		{
			// 길찾기를 여기서 하면 되겠군...내가 앞으로 갈 셀의 위치를 결정...(재덕)
			if(abx == aby)
			{
				if (ptGap.x>0) pt.x+=1;
				else pt.x-=1;
				if (ptGap.y>0) pt.y+=1;
				else pt.y-=1;

				//ptMap = CPacketParser::Conv2ServerMapPos(pt);
				ptMap = pt;

				//if (m_bpUserManager->IsMoveable(ptMap))
				if(m_bpUserManager->m_pPathFind->IsMovable(ptMap.x, ptMap.y))
				{
					pRoute->AddTail(pt);
					abx-=1;
					aby-=1;
				}
				else break;				
			}
			else if(abx > aby)
			{
				if (ptGap.x>0) pt.x +=2;
				else pt.x -=2;

				//ptMap = CPacketParser::Conv2ServerMapPos(pt);
				ptMap = pt;
				if(m_bpUserManager->m_pPathFind->IsMovable(ptMap.x, ptMap.y))
				//if (m_bpUserManager->IsMoveable(ptMap))
				{
					pRoute->AddTail(pt);
					abx-=2;
				}
				else break;
			}
			else //if(aby > abx)
			{
				if (ptGap.y>0) pt.y +=2;
				else pt.y -=2;

				//ptMap = CPacketParser::Conv2ServerMapPos(pt);
				ptMap = pt;
				//if (m_bpUserManager->IsMoveable(ptMap))
				if(m_bpUserManager->m_pPathFind->IsMovable(ptMap.x, ptMap.y))
				{
					pRoute->AddTail(pt);
					aby-=2;
				}
				else break;
			}
			
		}
	}


	if (pRoute->GetCount()>=1) //Routing에 추가된 것이 있음
	{
		if (abx||aby) //가다가 막힘 -> 막힌점이 마지막 점이 됨
		{
			TRACE("가다가 막힌다넹..--;;\n");
			return pRoute;
		}
	}
	else //움직이지 못함
	{
		TRACE("움직이지 못함???\n");
		SAFE_DELETE(pRoute);
		return NULL;
	}

//	pRoute->AddTail(ptEnd);
	return pRoute;


}

void CUser::SetMoveList(CList<CPoint, CPoint>* pRoute)
{
	if(m_pRoute) m_pRoute->RemoveAll();
	SAFE_DELETE(m_pRoute);
	m_pRoute = pRoute;
}


//응답받고 다음 이동점 요청
void CUser::SendNextRunMoveReq(CPoint pt, CPoint nextpt)
{
	BYTE buf[10];
	int offset = 0;

	BOOL bFind = FALSE;

	if (m_pRoute->IsEmpty())
	{
		CPacketParser::SetByte(buf, offset, PKT_RUN_MOVEEND); //최종점 요청
		TRACE("Send PKT_RUN_MOVEEND:");
	}
	else 
	{
		CPacketParser::SetByte(buf, offset, PKT_RUN_MOVEMIDDLE); //중간 단계
		TRACE("Send PKT_RUN_MOVEMIDDLE:");
	}

	CPacketParser::SetShort(buf, offset, (short)pt.x);
	CPacketParser::SetShort(buf, offset, (short)pt.y);

	CPacketParser::SetShort(buf, offset, (short)nextpt.x);
	CPacketParser::SetShort(buf, offset, (short)nextpt.y);

	TRACE("first[%d,%d] : next[%d,%d]\n", pt.x, pt.y, nextpt.x,nextpt.y);

	CChildView::SocketMng.Send(offset, buf);
	m_vMoveStep++;
}

//서버에서 온 응답 처리 (Move_First)
void CUser::SetMoveFirst(CPoint ptDst, CPoint ptFirst)
{
	if(ptDst.x < 0 || ptDst.y < 0)
	{
		if(m_pRoute) m_pRoute->RemoveAll();
		return;
	}
	if(m_pRoute == NULL) return;

	if (m_Me) //자기자신의 Move
	{
		if(ptDst == ptFirst)
		{
			// 한칸 움직일때 엔드 패킷을 보낸다.
			BYTE buf[10];
			int offset = 0;

			CPacketParser::SetByte(buf, offset, PKT_MOVEEND); //최종점 요청

			CPacketParser::SetShort(buf, offset, (short)ptDst.x);
			CPacketParser::SetShort(buf, offset, (short)ptDst.y);
			CChildView::SocketMng.Send(offset, buf);
			m_PkRecive_Flag = 1;
			m_vMoveStep++;
			m_ptPostCellPos = ptFirst;// 목적지 값을 이곳에서 집어 넣는다.
			m_ptDstPos = CellUnitToPixel(ptFirst);
			TRACE("우석..send PKT_MOVEEND (%d,%d)\n",ptFirst.x,ptFirst.y);
		}
		else
		{
			m_ptDestFinalPos = CellUnitToPixel(ptFirst);
			m_ptPostCellPos = ptFirst;// 목적지 값을 이곳에서 집어 넣는다.
			m_ptDstPos = CellUnitToPixel(ptFirst);
			SetMotion(CUser::UM_Walk);

			if(m_Cur_Fa < m_Max_Fa) m_Cur_Fa += 1; //피로도 증가..
		}
	}
	else 
	{
		//우석.. 10월 11일..
		if(m_bpUserManager->m_pPathFind->SetMovable(PixelToCellUnit(m_Position),true) == FALSE)
		{
			CString str;
			
			str.Format("2 - %d - (%d,%d)", m_iUserId, m_Position.x, m_Position.y);
			AfxGetMainWnd()->SetWindowText(str);
			return;
		}
		if(m_bpUserManager->m_pPathFind->SetMovable(ptFirst,false) == FALSE)
		{
			CString str;
			
			str.Format("3에러.. - %d - (%d,%d)", m_iUserId, ptFirst.x, ptFirst.y);
			AfxGetMainWnd()->SetWindowText(str);
			return;
		}

		if(m_ptDestFinalPos == m_Position && m_pRoute->IsEmpty())
		{
			m_ptDestFinalPos = CellUnitToPixel(ptFirst);
			m_ptPostCellPos = ptFirst;// 목적지 값을 이곳에서 집어 넣는다.
			m_ptDstPos = CellUnitToPixel(ptFirst);
			m_dwnewMoveTick = ::GetTickCount();
		}
		else m_pRoute->AddTail(ptFirst);
		SetMotion(CUser::UM_Walk);
		//-------------------
	}
}

//응답받고 다음 이동점 요청
void CUser::SendNextMoveReq(CPoint ptPosAllowed)
{
	BYTE buf[10];
	int offset = 0;

	CPoint pt = ptPosAllowed;

	//CPoint ptDest = PixelToCellUnit(m_ptDestFinalPos);// 최종 목적지..
	if (m_pRoute->IsEmpty())//pt.x==ptDest.x && pt.y==ptDest.y) 
	{
		CPacketParser::SetByte(buf, offset, PKT_MOVEEND); //최종점 요청
		TRACE2("우석.Send PKT_MOVEEND(%d,%d)\n", pt.x, pt.y);
	}
	else 
	{
		CPacketParser::SetByte(buf, offset, PKT_MOVEMIDDLE); //중간 단계
		TRACE2("우석.Send PKT_MOVEMIDDLE(%d,%d)\n", pt.x, pt.y);
	}

	m_dwMoveTick= GetTickCount();
	CPacketParser::SetShort(buf, offset, (short)pt.x);
	CPacketParser::SetShort(buf, offset, (short)pt.y);
	CChildView::SocketMng.Send(offset, buf);
	m_vMoveStep++;
	
	m_PkRecive_Flag = 1;// 패킷을 보냈다.
}


//서버에서 온 응답 처리 (Move_Middle)
void CUser::SetMoveMiddle(CPoint ptDst)
{
//	우석.. 10월 11일..
	if(ptDst.x < 0 || ptDst.y < 0)
	{
		if(m_pRoute) m_pRoute->RemoveAll();
		return;
	}
	if(m_pRoute == NULL) return;

	if(m_Me != 1) 
	{
		m_bpUserManager->m_pPathFind->SetMovable(PixelToCellUnit(m_Position),true);
		m_bpUserManager->m_pPathFind->SetMovable(ptDst,false);

		if(m_ptDestFinalPos == m_Position && m_pRoute->IsEmpty())
		{
			m_ptDestFinalPos = CellUnitToPixel(ptDst);
			m_ptPostCellPos = ptDst;// 목적지 값을 이곳에서 집어 넣는다.
			m_ptDstPos = CellUnitToPixel(ptDst);
			m_dwnewMoveTick = ::GetTickCount();
		}
		else m_pRoute->AddTail(ptDst);

		//m_pRoute->AddTail(ptDst);
		//m_ptDestFinalPos = CellUnitToPixel(ptDst);
		SetMotion(CUser::UM_Walk);
	}
	else
	{
		m_ptDestFinalPos = CellUnitToPixel(ptDst);
		SetMotion(CUser::UM_Walk);

		if(m_Cur_Fa < m_Max_Fa) m_Cur_Fa += 1; //피로도 증가..
	}
}
//서버에서 온 응답 처리(최정점 도착)
void CUser::SetMoveEnd(CPoint ptDst)
{
//	우석.. 10월 11일..
	if(ptDst.x < 0 || ptDst.y < 0)
	{
		m_pRoute->RemoveAll();
		return;
	}
	if(m_pRoute == NULL) return;

	if(m_Me != 1) 
	{
		m_bpUserManager->m_pPathFind->SetMovable(PixelToCellUnit(m_Position),true);
		m_bpUserManager->m_pPathFind->SetMovable(ptDst,false);

		if(m_ptDestFinalPos == m_Position && m_pRoute->IsEmpty())
		{
			m_ptDestFinalPos = CellUnitToPixel(ptDst);
			m_ptPostCellPos = ptDst;// 목적지 값을 이곳에서 집어 넣는다.
			m_ptDstPos = CellUnitToPixel(ptDst);
			m_dwnewMoveTick = ::GetTickCount();
		}
		else m_pRoute->AddTail(ptDst);

		//m_pRoute->AddTail(ptDst);
		//m_ptDestFinalPos = CellUnitToPixel(ptDst);
		SetMotion(CUser::UM_Walk);
	}
	else
	{
		m_ptDestFinalPos = CellUnitToPixel(ptDst);
		SetMotion(CUser::UM_Walk);

		if(m_Cur_Fa < m_Max_Fa) m_Cur_Fa += 1; //피로도 증가..
	}
}

//서버에서 온 응답 처리 (Move_First)
void CUser::SetRunMoveFirst(CPoint ptDst, CPoint ptFirst, CPoint ptNext)
{
	if(ptDst.x < 0 || ptDst.y < 0)
	{
		if(m_pRoute) m_pRoute->RemoveAll();
		return;
	}
	if(m_pRoute == NULL) return;

	if (m_Me) //자기자신의 Move
	{
		m_ptDestFinalPos = CellUnitToPixel(ptFirst);// 최종 목적지 값을 집어 넣어 준다.
		m_ptPostCellPos = ptFirst;
		m_ptDestCellPos = ptFirst;// 목적지 값을 이곳에서 집어 넣는다.
		m_ptNextCellPos = ptNext;
		m_ptDstPos = CellUnitToPixel(ptFirst);// 목적지 픽셀 값을 집어 넣는다.
		SetMotion(CUser::UM_RUN);
	}
	else
	{
		//우석.. 10월 11일..
		if(m_bpUserManager->m_pPathFind->SetMovable(PixelToCellUnit(m_Position),true) == FALSE)
		{
			TRACE("좌표가 이상하다...\n");
			return;
		}
		if(m_bpUserManager->m_pPathFind->SetMovable(ptFirst,false) == FALSE)
		{
			TRACE("좌표가 이상하다...\n");
			return;
		}

		if( m_iUserId < 10000)
		{
			m_ptDestFinalPos = CellUnitToPixel(ptFirst);
			m_ptPostCellPos = ptFirst;
		}
		SetMotion(CUser::UM_RUN);		
	}
}
//서버에서 온 응답 처리 (Move_Middle)
void CUser::SetRunMoveMiddle(CPoint ptDst, CPoint ptNext)
{
//	우석.. 10월 11일..
	if(ptDst.x < 0 || ptDst.y < 0)
	{
		if(m_pRoute) m_pRoute->RemoveAll();
		return;
	}
	if(m_pRoute == NULL) return;

	if(m_Me != 1) 
	{
		if(m_bpUserManager->m_pPathFind->SetMovable(PixelToCellUnit(m_Position),true) == FALSE)
		{
			TRACE("좌표가 이상하다...\n");
			return;
		}
		if(m_bpUserManager->m_pPathFind->SetMovable(ptNext,false) == FALSE)
		{
			TRACE("좌표가 이상하다...\n");
			return;
		}

		m_pRoute->AddTail(ptDst);
		m_ptDestFinalPos = CellUnitToPixel(ptDst);
		SetMotion(CUser::UM_RUN);
	}
	else 
	{
		m_ptDestFinalPos = CellUnitToPixel(ptDst);
		SetMotion(CUser::UM_RUN);
	}
}
//서버에서 온 응답 처리(최정점 도착)
void CUser::SetRunMoveEnd(CPoint ptDst, CPoint ptNext)
{
	if(ptDst.x < 0 || ptDst.y < 0)
	{
		if(m_pRoute) m_pRoute->RemoveAll();
		return;
	}
	if(m_pRoute == NULL) return;

	if(m_Me != 1) 
	{
		if(m_bpUserManager->m_pPathFind->SetMovable(PixelToCellUnit(m_Position),true) == FALSE)
		{
			TRACE("좌표가 이상하다...\n");
			return;
		}
		if(ptNext.x < 0 || ptNext. y < 0)
		{
			if( m_iUserId < 10000)
			{
				m_pRoute->AddTail(ptDst);
				m_ptDestFinalPos = CellUnitToPixel(ptDst);
			}
		}
		else 
		{
			if(m_bpUserManager->m_pPathFind->SetMovable(ptNext,false) == FALSE)
			{
				TRACE("좌표가 이상하다...\n");
				return;
			}

			m_pRoute->AddTail(ptDst);
			m_ptDestFinalPos = CellUnitToPixel(ptDst);
		}
		SetMotion(CUser::UM_RUN);
	}
	else
	{
		m_ptDestFinalPos = CellUnitToPixel(ptDst);
		SetMotion(CUser::UM_RUN);
	}
}


void CUser::SetMagic(int nIndex, int nMagicType, int height)
{
/*	CMagic *pMagic = new CMagic;
	
	pMagic->m_height = height;// 위치 보정값.
	if (pMagic->SetMagicRes(nIndex))
	{
		SAFE_DELETE(m_pMagic);
		m_pMagic = pMagic;
		m_nMagicType = nMagicType;
		pMagic->SetMagicType(nMagicType);
	}
*/
	m_pMagic->m_height = height;// 위치 보정값.
	if (m_pMagic->SetMagicRes(nIndex,nMagicType))//dspring_0720a
	{
		m_nMagicType = nMagicType;
		m_pMagic->SetMagicType(nMagicType);//dspring_0719a
	}
	//dspring_0719a

}
//dspring_0731_3dxx
void CUser::Set3DMagic(TCHAR* strName, int nMagicType, int nHeight, int nWidth)
{
	m_pMagic->Set3DMagic(strName, nMagicType, nHeight, nWidth);
}
void CUser::Set3DMagicEx(int index, int nMagicType, int nHeight, int nWidth)
{
	m_pMagic->Set3DMagicEx(index, nMagicType, nHeight, nWidth);
}

BOOL CUser::SetMotion(enum UserMotion um)
{	
	enum UserMotion prev_motion;
	BOOL retVal;

	if(m_AnimationIndex != um) retVal = TRUE;
	else retVal = FALSE;
//	if(m_AnimationIndex==UM_Attack && um == UM_Stop) return;
	if (m_AnimationIndex==UM_Stop||m_AnimationIndex==UM_Walk) m_prevAnimation = m_AnimationIndex;
	if(um == UM_Beated && m_AnimationIndex == UM_Attack) return retVal;
	prev_motion = m_AnimationIndex;
	m_AnimationIndex = um;
//	m_iSpriteIndex = 0;
	
	// 남자일때..이게 정확한 데이타
	//m_Arm_type;//0:맨손,너클, 용조권 1:한손검 2:양손검 3: 창 4:활 5:도끼,해머, 전부 6:지팡이 7: 석궁 8:쌍칼(여자)
/*
#define NOTHINGARM      0
#define SINGLESWORD     1
#define BIGSWORD        2 // 양손검
#define SPEAR           3 // 창
#define BOW             4 // 활
#define AX              5
#define WAND            6 //마법사 지팡이
#define CROSSBOW        7
#define DOUBLESWORD     8 //쌍칼(여자만..)
*/
// 우석.. 남녀를 구분해놨음...
	if(m_bDying == FALSE)
	{
		switch(m_AnimationIndex)
		{
		case UM_Stop:
			if(m_Gender == 0) //여자..
			{
				if(m_BattleMode == 0 )
				{
					if(m_iUserId < 10000) m_AniSprNumber = 18;
					else				  m_AniSprNumber = 0;	
				}
				else if(m_Arm_type == NOTHINGARM || m_Arm_type == SINGLESWORD || m_Arm_type == BOW || m_Arm_type == WAND )
					m_AniSprNumber = 0;
				else if(m_Arm_type == BIGSWORD)//양손검
					m_AniSprNumber = 1;
				else if(m_Arm_type == SPEAR || m_Arm_type == AX)//창도끼
					m_AniSprNumber = 2;
				else if(m_Arm_type == CROSSBOW)//석궁
					m_AniSprNumber = 3;
			}
			else 
			{
				if(m_BattleMode == 0 )
				{
					if(m_iUserId < 10000) m_AniSprNumber = 18;
					else				  m_AniSprNumber = 0;	
				}
				else if(m_Arm_type == NOTHINGARM || m_Arm_type == SINGLESWORD || m_Arm_type == BOW || m_Arm_type == WAND )
					m_AniSprNumber = 0;
				else if(m_Arm_type == BIGSWORD || m_Arm_type == AX)//양손검 도끼
					m_AniSprNumber = 1;
				else if(m_Arm_type == SPEAR)//창
					m_AniSprNumber = 2;
				else if(m_Arm_type == CROSSBOW)//석궁
					m_AniSprNumber = 3;
			}
			break;
		case UM_Walk:
//			TRACE1("(%d)-Walk\n", m_iUserId);
			if(m_Gender == 0) //여자..
			{
				if(m_BattleMode == 0 || m_Arm_type == NOTHINGARM || m_Arm_type == SINGLESWORD || m_Arm_type == BOW || m_Arm_type == WAND) //기본걷기.
					m_AniSprNumber = 4;
				else if(m_Arm_type == BIGSWORD)//양손검
					m_AniSprNumber = 5;
				else if(m_Arm_type == SPEAR || m_Arm_type == AX)//창 도끼
					m_AniSprNumber = 6;
				else m_AniSprNumber = 4; //위에 해당이 안되면 기본걷기..

				if(m_Run == 2) m_AniSprNumber = 16;// 뛰기..
			}
			else 
			{
				if(m_BattleMode == 0 || m_Arm_type == NOTHINGARM || m_Arm_type == SINGLESWORD || m_Arm_type == BOW || m_Arm_type == WAND || m_Arm_type == CROSSBOW)
					m_AniSprNumber = 4;
				else if(m_Arm_type == BIGSWORD || m_Arm_type == AX)//양손검 도끼
					m_AniSprNumber = 5;
				else if(m_Arm_type == SPEAR)//창 
					m_AniSprNumber = 6;

				if(m_Run == 2) m_AniSprNumber = 16;// 뛰기..
			}
			CChildView::UserManager.DeleteMagic(this->m_iUserId);
			break;
	// 남자와 여자의 공격애니 번호가 다르다.. 구별해서 해줌.. 우석...
		case UM_Attack:
//			TRACE1("(%d)-Attack\n", m_iUserId);
			if(m_Gender == 0) //여자..
			{
				if(m_BattleMode == 0 || m_Arm_type == NOTHINGARM)// 맨손공격
					m_AniSprNumber = 7;
				else if(m_Arm_type == SINGLESWORD || m_Arm_type == WAND)// 한손검 공격 지팡이
					m_AniSprNumber = 8;
				else if(m_Arm_type == BIGSWORD)// 양손검 공격
					m_AniSprNumber = 9;
				else if(m_Arm_type == SPEAR)//창공격
					m_AniSprNumber = 10;
				else if(m_Arm_type == BOW)//활
					m_AniSprNumber = 11;
				else if(m_Arm_type == AX)//도끼
					m_AniSprNumber = 12;
				else if(m_Arm_type == CROSSBOW)// 석궁
					m_AniSprNumber = 3;
				else if(m_Arm_type == DOUBLESWORD)//쌍칼공격
					m_AniSprNumber = 17;
			}
			else 
			{
				if(m_BattleMode == 0 || m_Arm_type == NOTHINGARM)// 맨손공격
					m_AniSprNumber = 7;
				else if(m_Arm_type == SINGLESWORD || m_Arm_type == WAND)// 한손검 공격 지팡이
					m_AniSprNumber = 8;
				else if(m_Arm_type == BIGSWORD || m_Arm_type == AX)// 양손검 공격
					m_AniSprNumber = 9;
				else if(m_Arm_type == SPEAR)//창공격
					m_AniSprNumber = 10;
				else if(m_Arm_type == BOW)//활
					m_AniSprNumber = 11;
		//		else if(m_Arm_type == AX)//도끼
		//			m_AniSprNumber = 12;
				else if(m_Arm_type == CROSSBOW)// 석궁
					m_AniSprNumber = 3;
			}
			break;
		case UM_Beated:
//			TRACE1("(%d)-Beated\n", m_iUserId);
			m_AniSprNumber = 13;
			break;
		case UM_Magic:
			m_AniSprNumber = 14;
			break;
		case UM_Die:
//			TRACE1("(%d)-Die\n", m_iUserId);
			m_bDying = TRUE;
			m_AniSprNumber = 15;
			break;
		case UM_RUN:
			m_AniSprNumber = 16;
			CChildView::UserManager.DeleteMagic(this->m_iUserId);
			break;
		default:
			break;
		}
	}
	else
	{
//		TRACE1("(%d)-Die\n", m_iUserId);
		m_AniSprNumber = 15;
	}
/*	else if(prev_motion != UM_Die)
	{
		m_AnimationIndex = UM_Die;
		m_AniSprNumber = 15;

		int key;
		char *value;

		CPoint ptSoundPos = GetPosition() - CChildView::MapManager.GetMapStartPos();

		if(m_nType < 10) // 유저
		{
			if(m_Gender == 1) // 남자
			{
				if(g_SoundName.Lookup(SOUND_DEAD1, value))
				{
					CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_DEAD1, FALSE, ptSoundPos);
				}
			}
			else
			{
				if(g_SoundName.Lookup(SOUND_DEAD2, value))
				{
					CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_DEAD2, FALSE, ptSoundPos);
				}
			}
		}
		else // NPC
		{
			key = SOUND_MONSTER+(m_nType-10)*4+3;
			if(g_SoundName.Lookup(key, value))
			{
				CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
			}
		}

		if(m_Me == 1)
		{
			if(g_pShowDialog)
			{
				if(g_pShowDialog->IsVisible()) g_pShowDialog->HideAllWindow();
				g_pShowDialog = NULL;
			}
			if(g_pFocusDialog)
			{
				if(g_pFocusDialog->IsVisible()) g_pFocusDialog->HideAllWindow();
				g_pFocusDialog = NULL;
			}
			if(g_pCaptureDialog)
			{
				if(g_pCaptureDialog->IsVisible()) g_pCaptureDialog->HideAllWindow();
				g_pCaptureDialog = NULL;
			}
			Dialog_Main.HideWindow();
			Dialog_DeadMain.ShowWindow();
			Dialog_DeadMain.StartCountDown();
		}
	}*/

	// 이곳에서 에니메이션 종류를 변화 시켜줘야 하는가?
	// 현재 상태를 파악해서..에니메이션을 결정해야 한다.
	// 새로운 변수 하나를 만들어야 겠군..
	//
	// if (m_AnimationIndex==UM_Walk||m_AnimationIndex==UM_Stop) return;

	if (m_bpCharRes)
	{
		SPR_DATA* spr_data;
		
		spr_data = m_bpCharRes->GetSprData(m_AniSprNumber);
		if(spr_data == NULL)
		{
			TRACE1("SetMotion(%d) Fail!!!\n", m_AniSprNumber);
			return FALSE;
		}
		if(retVal)
		{
			m_iSpriteIndex = 0;
			m_nMotionStep = spr_data->m_AniData.nMaxFrm;
		}
	}

	return retVal;
}

//입고 있는 아이템 설정
void CUser::SetWearItem(int nType, int nIndex, int btype, int barm)
{
//장착아이템을 해제할 경우. 우석..

//	bool   bCancel;
//	if(nIndex == -1) { bCancel = true; nIndex = 0; }
//	else			 bCancel = false;
	

	//nIndex는 그림 번호가 들어 온다.
/*  // 임시로 막아 놓는다.
	if(m_Gender == 0)// 여자면..
		nIndex += 500;

	if(m_Gender == 1 && nIndex >= 500)// 남자인데 여자 갑옷을 입을려고 하면..
		return;
	if(m_Gender == 0 && nIndex <= 499)// 여자인데 남자 갑옷을 입을려고 하면..
		return;
*/

/////////////////////////////////////////////////////////
	//임시로 이곳에서 그림을 바꿔준다.
	
	// 아래부분과 바꿔야 하는 부분..아이템 정보 파일이 있으면...
	//nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
	// 무기 타입 설정하는 것은 있어야 겠군..무기는 안에 집어 넣어 버릴까나?
/*	switch(nType)
	{
	case AM_KIND_ARMOR	:
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		break;
	case AM_KIND_HELMET	:
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		break;
	case AM_KIND_BOOT	:	
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		break;
	case AM_KIND_JEAN	:	
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		break;
	case AM_KIND_SHIELD	:	
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		break;
	case AM_KIND_ARMS	:		
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		break;
	case AM_KIND_SH	:	
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		break;
	default: ASSERT(0);		//적용되지 않는 아이템임
		break;
	}
*/
	switch(nType)
	{
	case AM_KIND_ARMOR	:		
		m_MeDraw_Order[AM_KIND_ARMOR] = __GetMainWnd()->m_wndView.GetItemDrawOrderIndex(nIndex);//우석..
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		/*if(nIndex == 0) nIndex = 0;// 맨몸
		else if(nIndex == 13) nIndex = 1;//기본1
		else if(nIndex == 14) nIndex = 1;//기본2
		else if(nIndex == 15) nIndex = 1;//기본3
		else if(nIndex == 16) nIndex = 1;//기본4
		else if(nIndex == 19 || nIndex == 17 || nIndex == 18 ) nIndex = 5;//철가죽
		else nIndex = 4;// 가죽
		*/
		break;
	case AM_KIND_HELMET	:
		m_MeDraw_Order[AM_KIND_HELMET] = __GetMainWnd()->m_wndView.GetItemDrawOrderIndex(nIndex);//우석..

		if(nIndex == 0)
		{
			nIndex = m_Hair; 
			nIndex += 100;
			break;
		}

		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);		
		
		// 임시.. 헬멧처리... 인텍스는 헬멧의 인덱스...
		// 임시로 0번 디폴트로 해놓는다.... 우석..
//		if(nIndex == 0) nIndex = 4;// 기본머리..지금은 없다.. 캐릭터 만들기에서 선택.. 우석..
//		else nIndex = 1;//기본1
//		nIndex = 2;//기본  .. 우석... 10월 14일..
		
		break;
	case AM_KIND_BOOT	:	
		m_MeDraw_Order[AM_KIND_BOOT] = __GetMainWnd()->m_wndView.GetItemDrawOrderIndex(nIndex);//우석..
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		
		/*if(nIndex == 0) nIndex = 0;// 맨몸
		else if(nIndex == 29) nIndex = 5;//기본1
		else if(nIndex == 30) nIndex = 5;//기본2
		else if(nIndex == 31) nIndex = 5;//기본3
		else if(nIndex == 32) nIndex = 4;//기본4
		else nIndex = 3;// 가죽
		*/
		break;
	case AM_KIND_JEAN	:
		m_MeDraw_Order[AM_KIND_JEAN] = __GetMainWnd()->m_wndView.GetItemDrawOrderIndex(nIndex);//우석..
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		
		/*if(nIndex == 0) nIndex = 0;// 맨몸
		else if(nIndex == 21) nIndex = 1;//기본1
		else if(nIndex == 22) nIndex = 2;//기본2
		else if(nIndex == 23) nIndex = 3;//기본3
		else if(nIndex == 24) nIndex = 6;//기본3
		else if(nIndex == 28) nIndex = 4;//기본4
		else if(nIndex == 27) nIndex = 5;//철가죽
		else nIndex = 1;// 가죽
		*/
		break;
	case AM_KIND_SHIELD	:	
		m_MeDraw_Order[AM_KIND_SHIELD] = __GetMainWnd()->m_wndView.GetItemDrawOrderIndex(nIndex);//우석..
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);		
		
		/*if(nIndex == 0) nIndex = 0;// 맨몸
		else nIndex = 1;//기본1
		*/
		break;
	case AM_KIND_ARMS	:
/*		// 임시 불러 읽는 부분을 추가.
		if(nIndex == 0) ;// 맨
		else if(nIndex == 1) {m_Arm_type = 1;}//검
		else if(nIndex == 2) {m_Arm_type = 1;}//지팡이
		else if(nIndex == 4) {m_Arm_type = 4;}//활
		else if(nIndex == 5) {m_Arm_type = 3;}//창
		else if(nIndex == 10) {m_Arm_type = 5;}//도끼또는 양손검
		else {m_Arm_type = 1;}// 		
*/
		m_MeDraw_Order[AM_KIND_ARMS] = __GetMainWnd()->m_wndView.GetItemDrawOrderIndex(nIndex);//우석..
		//0:맨손,너클, 용조권 1:한손검 2:양손검 3: 창 4:활 5:도끼,해머, 전부 6:지팡이 7: 석궁 8:쌍칼(여자)
		m_Arm_type = __GetMainWnd()->m_wndView.GetItemArmTypeIndex(nIndex);
		nIndex = __GetMainWnd()->m_wndView.GetItemPicIndex(nIndex);
		
		
		break;
	case AM_KIND_SH	:	
		break;
	default:
		ASSERT(0);		//적용되지 않는 아이템임
		break;
	}

	////////////////////////////////////////////////////
/*
	// 임시 나중에는 지워야됨..
	if(m_Gender == 0 && AM_KIND_ARMS == nType)// 여자면..
		nIndex += 500;

	if(m_Gender == 0 && AM_KIND_SH == nType)// 여자면.. 그림자처리..10월 14일.
		nIndex += 500;

	//우석.. 가려놨음.. 10월 14일...
	if(nIndex == 500)
		nIndex = 0;// 비어있는 갑옷...

	// 임시 나중에는 지워야됨..
	if(m_Gender == 0 && AM_KIND_HELMET == nType)// 여자면..
		nIndex += 500;
*/
	if(m_Gender == 0) nIndex += 500; // 여자면..
	if(nIndex == 500) nIndex = 0;// 장착 아이템을 해제할 경우..


	int tmp;
	switch(nType)
	{
	case AM_KIND_ARMOR	:	
		tmp = m_pArmature->m_nArmor; 
		m_pArmature->m_nArmor	= nIndex;		
		break;
	case AM_KIND_HELMET	:	
		tmp = m_pArmature->m_nHelmet; 
		m_pArmature->m_nHelmet	= nIndex;		
		break;
	case AM_KIND_BOOT	:	
		tmp = m_pArmature->m_nBoot; 
		m_pArmature->m_nBoot	= nIndex;		
		break;
	case AM_KIND_JEAN	:	
		tmp = m_pArmature->m_nJean; 
		m_pArmature->m_nJean	= nIndex;		
		break;
	case AM_KIND_SHIELD	:	
		tmp = m_pArmature->m_nShield; 
		m_pArmature->m_nShield	= nIndex;		
		break;
	case AM_KIND_ARMS	:	
		tmp = m_pArmature->m_nArms; 
		m_pArmature->m_nArms	= nIndex;		
		break;
	case AM_KIND_SH	:	
		break;
	default:
		ASSERT(0);		//적용되지 않는 아이템임
		break;
	}

	if(nIndex == 0) m_pArmature->m_pArmatureRes[nType] = NULL;
	else
	{
		if (m_bpArmatureManager->LoadArmatureRes(nType, nIndex))
		{
			m_pArmature->m_pArmatureRes[nType] = m_bpArmatureManager->GetTypeIndexedArmature(nType, nIndex);
		}
		else 
		{
	//		INIT_PTR(m_pArmature->m_pArmatureRes[nType]);
			switch(nType)
			{
			case AM_KIND_ARMOR	:	
				m_pArmature->m_nArmor	= tmp;		
				break;
			case AM_KIND_HELMET	:	
				m_pArmature->m_nHelmet	= tmp;		
				break;
			case AM_KIND_BOOT	:	
				m_pArmature->m_nBoot	= tmp;		
				break;
			case AM_KIND_JEAN	:	
				m_pArmature->m_nJean	= tmp;		
				break;
			case AM_KIND_SHIELD	:	
				m_pArmature->m_nShield	= tmp;		
				break;
			case AM_KIND_ARMS	:	
				m_pArmature->m_nArms	= tmp;		
				break;
			case AM_KIND_SH	:	
				break;
			default:
				ASSERT(0);		//적용되지 않는 아이템임
				break;
			}
		}
	}

//우석.. 무기를 착용했다가 없앴을 경우..
//기본 동작으로 바꿔줘야 한다..
/*
	if(bCancel)
	{
		if(m_pArmature->m_pArmatureRes[nType] != NULL)
		{
			m_pArmature->m_pArmatureRes[nType] = NULL;
		 	return;
		}
	}
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//         LSP 처리용 함수들
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CUser::BltLSPDark(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, int alpha)
{
	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이

	const WORD* pSrc = data; 
//	int nLine = *pSrc; pSrc++; // 높이가 기록되어 있다.
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	if(alpha < 0) alpha = 0; if(alpha > 32) alpha = 32;
	int AlphaSrc = alpha; // 스프라이트 투명도

	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드단위 라인당 피치.. 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;
	
	WORD* pDest = NULL;
	
	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산

				PutPixelDark(pDest, pSrc, nCopyCount, alpha, m_dwRBBitMask, m_dwGBitMask);
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = -rc.left;
		int clipXR = rc.right - ScreenX;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;
			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ >= clipXL) { pDest += nZeroCount; }
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL) { nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS; }
						else { pDest += ndxS - clipXL; }
					}
				}
				if(clipXR > 0) // 오른쪽 클리핑
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}
				}
				
				PutPixelDark(pDest, pSrc, nCopyCount, alpha, m_dwRBBitMask, m_dwGBitMask);
			}
		}
	}
	pSurface->Unlock();
}

void CUser::BltLSPNormal(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, BOOL bFocus, int nPoisonKind, int nFocusColor)
{
	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이
	int i = 0;

	const WORD* pSrc = data; 
//	int nLine = *pSrc; pSrc++; // 높이가 기록되어 있다.
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	if( NULL == pSurface->Lock() ) return;
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left; 
	int height = rcClip.bottom - rcClip.top;

	WORD* Alter = NULL;
	WORD POISON;
	WORD G;
	WORD R;
	WORD FOCUSCOLOR;

	DWORD RBBitMask, GBitMask;

	if( CChildView::CharManager.GetPixelFormat() == 565 ) 
	{
		RBBitMask = 0x0000f81f;
		GBitMask  = 0x000007e0;
		POISON = 0x781F;
//		B = 0x1F;
		G = 0x1E0;
		R = 0xD000;
//		FOCUSCOLOR = 0x3807;
		FOCUSCOLOR = 0x7be0;
	}
	else
	{
		RBBitMask = 0x00007c1f;
		GBitMask  = 0x000003e0;
		POISON = 0x3C1F;
		G = 0x1E0;
		R = 0x6800;
//		FOCUSCOLOR = 0x1C07;
		FOCUSCOLOR = 0x3de0;

	}

	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}

				//========================================
/*				DWORD RBBitMask, GBitMask;
				if( CChildView::CharManager.GetPixelFormat() == 565 ) 
				{
					RBBitMask = 0x0000f81f;
				    GBitMask  = 0x000007e0;
				}
				else
				{
					RBBitMask = 0x00007c1f;
				    GBitMask  = 0x000003e0;
				}

				WORD color = 0x007f;
				int  alpha = 16;
				PutPixelNormal2(pDest, pSrc, nCopyCount,color, alpha, RBBitMask,GBitMask);
*/				//=========================================

				const WORD* pDestAlter = pSrc;
/*
				if(bFocus)
				{
					switch( nFocusColor )
					{
					case 1://RED 강조 
						color = R;
						alpha = 8;
						PutPixelNormal2(pDest, pSrc, nCopyCount,color, alpha, RBBitMask,GBitMask);
//						Alter = new WORD[nCopyCount]; 
//						for( i = 0; i < nCopyCount; i++ )
//						{
//							Alter[i] =  (*pDestAlter) | R;
//							pDestAlter++;
//						}
//						PutPixelNormal(pDest, Alter, nCopyCount);
//						delete [] Alter;
						break;
					case 2://YELLOW 강조
						color = FOCUSCOLOR;
						alpha = 8;
						PutPixelNormal2(pDest, pSrc, nCopyCount,color, alpha, RBBitMask,GBitMask);
//						Alter = new WORD[nCopyCount]; 
//						for( i = 0; i < nCopyCount; i++ )
//						{
//							Alter[i] =  (*pDestAlter) | FOCUSCOLOR;
//							pDestAlter++;
//						}
//						PutPixelNormal(pDest, Alter, nCopyCount);
//						delete [] Alter;
						break;
					case 3://GREEN 강조 
						color = G;
						alpha = 14;
						PutPixelNormal2(pDest, pSrc, nCopyCount,color, alpha, RBBitMask,GBitMask);
//						Alter = new WORD[nCopyCount]; 
//						for( i = 0; i < nCopyCount; i++ )
//						{
//							Alter[i] =  (*pDestAlter) | G;
//							pDestAlter++;
//						}
//						PutPixelNormal(pDest, Alter, nCopyCount);
//						delete [] Alter;
						break;
					}
				}
				else
*/				{
					switch( nPoisonKind )
					{
					case 0://자기색 
	/*					
						if(bFocus)
						{
							Alter = new WORD[nCopyCount]; 
							for( i = 0; i < nCopyCount; i++ )
							{
								Alter[i] =  (*pDestAlter) | FOCUSCOLOR;
								pDestAlter++;
							}
							PutPixelNormal(pDest, Alter, nCopyCount);
							delete [] Alter;

						}
						else
						PutPixelNormal(pDest, pSrc, nCopyCount);
	*/
						PutPixelNormal(pDest, pSrc, nCopyCount);
						break;
					case 3://BLUE 강조
						Alter = new WORD[nCopyCount]; 
						for( i = 0; i < nCopyCount; i++ )
						{
							Alter[i] =  (*pDestAlter) | POISON;
							pDestAlter++;
						}
						PutPixelNormal(pDest, Alter, nCopyCount);
						delete [] Alter;
						break;
					default:
						PutPixelNormal(pDest, pSrc, nCopyCount);
						break;
					}
				}
				//테두리처리 {
				if (bFocus)
				{
					*(pDest-1) = 0x07e0;
					*(pDest+nCopyCount) = 0x07e0;
					if (y==0)
					{
						int count = nCopyCount;
						while (count)
						{
							*(pDest+count) = 0x07e0;
							count--;
						}
						continue;
					}
				}
				// }
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = -rc.left;
		int clipXR = rc.right - ScreenX;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;
			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ >= clipXL) { pDest += nZeroCount; }
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL) { nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS; }
						else { pDest += ndxS - clipXL; }
					}
				}
				if(clipXR > 0) // 오른쪽 클리핑
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}
				}
//				PutPixelNormal(pDest, pSrc, nCopyCount);
				
				const WORD* pDestAlter = pSrc;
/*				if(bFocus)
				{
					switch( nFocusColor )
					{
					case 1://RED 강조 
						Alter = new WORD[nCopyCount]; 
						for( i = 0; i < nCopyCount; i++ )
						{
							Alter[i] =  (*pDestAlter) | R;
							pDestAlter++;
						}
						PutPixelNormal(pDest, Alter, nCopyCount);
						delete [] Alter;
						break;
					case 2://YELLOW 강조
						Alter = new WORD[nCopyCount]; 
						for( i = 0; i < nCopyCount; i++ )
						{
							Alter[i] =  (*pDestAlter) | FOCUSCOLOR;
							pDestAlter++;
						}
						PutPixelNormal(pDest, Alter, nCopyCount);
						delete [] Alter;
						break;
					case 3://GREEN 강조 
						Alter = new WORD[nCopyCount]; 
						for( i = 0; i < nCopyCount; i++ )
						{
							Alter[i] =  (*pDestAlter) | G;
							pDestAlter++;
						}
						PutPixelNormal(pDest, Alter, nCopyCount);
						delete [] Alter;
						break;
					}
				}
				else
*/				{
					switch( nPoisonKind )
					{
					case 0://자기색 
	/*					
						if(bFocus)
						{
							Alter = new WORD[nCopyCount]; 
							for( i = 0; i < nCopyCount; i++ )
							{
								Alter[i] =  (*pDestAlter) | FOCUSCOLOR;
								pDestAlter++;
							}
							PutPixelNormal(pDest, Alter, nCopyCount);
							delete [] Alter;

						}
						else
						PutPixelNormal(pDest, pSrc, nCopyCount);
	*/
						PutPixelNormal(pDest, pSrc, nCopyCount);
						break;
					case 3://BLUE 강조
						Alter = new WORD[nCopyCount]; 
						for( i = 0; i < nCopyCount; i++ )
						{
							Alter[i] =  (*pDestAlter) | POISON;
							pDestAlter++;
						}
						PutPixelNormal(pDest, Alter, nCopyCount);
						delete [] Alter;
						break;
					default:
						PutPixelNormal(pDest, pSrc, nCopyCount);
						break;
					}
				}
				//테두리처리 {
				if (bFocus)
				{
					*(pDest-1) = 0x07e0;
					*(pDest+nCopyCount) = 0x07e0;
					if (y==0)
					{
						int count = nCopyCount;
						while (count)
						{
							*(pDest+count) = 0x07e0;
							count--;
						}
						continue;
					}
				}
				// }
			}
		}
	}
	pSurface->Unlock();
}

void CUser::BltLSPDrawObj(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data)
{

	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= g_vDispPixSize.cy) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이


	WORD* pSrc = data; 
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;
	int basewidth = ScreenX;

	if( height >= g_vDispPixSize.cy ) height = g_vDispPixSize.cy;

	WORD* pDest = NULL;
	
	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산
				PutPixelNormal( pDest, pSrc, nCopyCount);

				//CUser::PutPixelShadowEx(pDest, nCopyCount, dwHalf16Mask, dwHalf32Mask);
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = - rc.left;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;

			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nAddCount = 0;
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ > clipXL + basewidth)
					{
						pSrc += nCopyCount;
						nCopyCount = 0;

						continue;
					}
					if(ndxZ >= clipXL)
					{
						if( ndxS >= clipXL + basewidth )
						{
							pSrc += nCopyCount;
							nCopyCount = 0;
							continue;
						}
						else
						{
							if( ndxE > clipXL + basewidth )
							{
								pDest += nZeroCount;
								nAddCount = ndxE - clipXL - basewidth;
								nCopyCount -= nAddCount;
							}
							else
							{
								pDest += nZeroCount;
							}
						}
					}
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL)
						{
							if( ndxE <= clipXL + basewidth )//그리는 영역보다 작거나 같을때
							{
								nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS;								
							}
							else//그리는 영역 보다 클때
							{
								nAddCount = ndxE - basewidth - clipXL;
								pSrc += clipXL -ndxS; nCopyCount -= clipXL - ndxS + nAddCount;
							}
						}
						else if( ndxS >= clipXL + basewidth )
						{
							pSrc += nCopyCount;
							nCopyCount = 0;
							continue;
						}
						else
						{
							if( ndxE <= clipXL + basewidth )
							{
								pDest += ndxS - clipXL;
							}
							else
							{
								pDest += ndxS - clipXL;
								nAddCount = ndxE - clipXL - basewidth;
								nCopyCount -= nAddCount;
							}
						}
					}

				}//if(clipXL > 0) // 왼쪽 클리핑
				else
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}

				}

				if( nCopyCount != 0 )
				PutPixelNormal( pDest, pSrc, nCopyCount);
				//CUser::PutPixelShadowEx(pDest, nCopyCount, dwHalf16Mask, dwHalf32Mask);
			}
		}
	}
	pSurface->Unlock();

}

void CUser::BltLSPAlphaAdd(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data)
{
	if(data == NULL) return;
	if(pRC == NULL) return;
	
	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이

	WORD* pSrc = data; 
//	int nLine = *pSrc; pSrc++; // 높이가 기록되어 있다.
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;

	WORD* pDest = NULL;
	
	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산
				PutPixelAlphaAdd(pDest, pSrc, nCopyCount, m_dwRBitMask, m_dwGBitMask, m_dwBBitMask);
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = -rc.left;
		int clipXR = rc.right - ScreenX;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;
			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ >= clipXL) { pDest += nZeroCount; }
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL) { nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS; }
						else { pDest += ndxS - clipXL; }
					}
				}
				if(clipXR > 0) // 오른쪽 클리핑
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}
				}
				
				// 인라인 함수 호출..
				PutPixelAlphaAdd(pDest, pSrc, nCopyCount, m_dwRBitMask, m_dwGBitMask, m_dwBBitMask);

			}
		}
	}
	pSurface->Unlock();
}

void CUser::BltLSPTrans(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, int alpha, BOOL bFocus)
{
	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이

	WORD* pSrc = data; 
//	int nLine = *pSrc; pSrc++; // 높이가 기록되어 있다.
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	if(alpha < 0) alpha = 0; if(alpha > 32) alpha = 32;
	int alphaDest = 32 - alpha; // 배경 투명도
	int alphaSrc = alpha; // 스프라이트 투명도

	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;

	WORD* pDest = NULL;
	
	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산
				
				PutPixelTrans(pDest, pSrc, nCopyCount, alphaDest, alphaSrc, m_dwRBBitMask, m_dwGBitMask);

				//테두리처리 {
				if (bFocus)
				{
					*(pDest-1) = 0x07e0;
					*(pDest+nCopyCount) = 0x07e0;
					if (y==0)
					{
						int count = nCopyCount;
						while (count)
						{
							*(pDest+count) = 0x07e0;
							count--;
						}
						continue;
					}
				}
				// }

			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = -rc.left;
		int clipXR = rc.right - ScreenX;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;
			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ >= clipXL) { pDest += nZeroCount; }
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL) { nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS; }
						else { pDest += ndxS - clipXL; }
					}
				}
				if(clipXR > 0) // 오른쪽 클리핑
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}
				}
				
				PutPixelTrans(pDest, pSrc, nCopyCount, alphaDest, alphaSrc, m_dwRBBitMask, m_dwGBitMask);

				//테두리처리 {
				if (bFocus)
				{
					*(pDest-1) = 0x07e0;
					*(pDest+nCopyCount) = 0x07e0;
					if (y==0)
					{
						int count = nCopyCount;
						while (count)
						{
							*(pDest+count) = 0x07e0;
							count--;
						}
						continue;
					}
				}
				// }
			}
		}
	}
	pSurface->Unlock();
}

void CUser::BltLSPChat(CDDSurface *pSurface)
{
	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface);
	WORD* pDest = NULL;
	for( int i = 0 ; i < 18 ; i++ )
	{
		pDest = pDestOrg + 21 + (460+i)*pitchDest;
		PutPixelShadow(pDest, 757, 16, m_dwRBBitMask, m_dwGBitMask);
	}
	pSurface->Unlock();
}

void CUser::BltBlackTrans(CDDSurface *pSurface,CRect rc)
{
	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return;

	CRect rcClip;
	GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역
	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface);
	WORD* pDest = NULL;
	int width  = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;
	for( int i = 0 ; i < height ; i++ )
	{
		pDest = pDestOrg + rcClip.left + (rcClip.top+i)*pitchDest;
		PutPixelShadowEx(pDest, width, CChildView::DirectXMng.m_dwHalf16Mask, CChildView::DirectXMng.m_dwHalf32Mask);
	}
	pSurface->Unlock();
}

void CUser::BltLSPShadowEx(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, DWORD dwHalf16Mask, DWORD dwHalf32Mask)
{
	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return; // 화면을 완전히 벗어남..
//shadow__
	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역
//	rcClip = rc;
//	BOOL bUseXClip = FALSE;
//shadow__
	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이

	WORD* pSrc = data; 
//	int nLine = *pSrc; pSrc++; // 높이가 기록되어 있다.
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	//if(alpha < 0) alpha = 0; if(alpha > 32) alpha = 32;
	//int AlphaDest = 32 - alpha; // 배경 투명도

	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;

	WORD* pDest = NULL;
	
	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산

				//PutPixelShadow(pDest, nCopyCount, alpha, m_dwRBBitMask, m_dwGBitMask);
				PutPixelShadowEx(pDest, nCopyCount, dwHalf16Mask, dwHalf32Mask);
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = -rc.left;
		int clipXR = rc.right - ScreenX;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;
			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ >= clipXL) { pDest += nZeroCount; }
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL) { nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS; }
						else { pDest += ndxS - clipXL; }
					}
				}
				if(clipXR > 0) // 오른쪽 클리핑
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}
				}
				
				//PutPixelShadow(pDest, nCopyCount, alpha, m_dwRBBitMask, m_dwGBitMask);
				PutPixelShadowEx(pDest, nCopyCount, dwHalf16Mask, dwHalf32Mask);
			}
		}
	}
	pSurface->Unlock();

}

void CUser::BltLSPShadow(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, int alpha)
{
	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이

	WORD* pSrc = data; 
//	int nLine = *pSrc; pSrc++; // 높이가 기록되어 있다.
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	if(alpha < 0) alpha = 0; if(alpha > 32) alpha = 32;
	int AlphaDest = 32 - alpha; // 배경 투명도

	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;

	WORD* pDest = NULL;
	
	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산

				PutPixelShadow(pDest, nCopyCount, alpha, m_dwRBBitMask, m_dwGBitMask);
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = -rc.left;
		int clipXR = rc.right - ScreenX;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;
			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ >= clipXL) { pDest += nZeroCount; }
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL) { nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS; }
						else { pDest += ndxS - clipXL; }
					}
				}
				if(clipXR > 0) // 오른쪽 클리핑
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}
				}
				
				PutPixelShadow(pDest, nCopyCount, alpha, m_dwRBBitMask, m_dwGBitMask);
			}
		}
	}
	pSurface->Unlock();
}

void CUser::BltLSPBitStepTrans(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, int nAlphaSrc, int nAlphaDst)
{
	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= 495) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이


	WORD* pSrc = data; 
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;
	int basewidth = ScreenX;

	if( height >= g_vDispPixSize.cy ) height = g_vDispPixSize.cy;

	WORD* pDest = NULL;
	
	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산
				//PutPixelHalf(pDest, pSrc, nCopyCount, dwHalf16Mask, dwHalf32Mask);
				PutPixelBitStepTrans(pDest,pSrc,nCopyCount, nAlphaDst,nAlphaSrc, m_dwRBGBitMask, m_dwGRBBitMask);
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = - rc.left;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;

			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nAddCount = 0;
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ > clipXL + basewidth)
					{
						pSrc += nCopyCount;
						nCopyCount = 0;

						continue;
					}
					if(ndxZ >= clipXL)
					{
						if( ndxS >= clipXL + basewidth )
						{
							pSrc += nCopyCount;
							nCopyCount = 0;
							continue;
						}
						else
						{
							if( ndxE > clipXL + basewidth )
							{
								pDest += nZeroCount;
								nAddCount = ndxE - clipXL - basewidth;
								nCopyCount -= nAddCount;
							}
							else
							{
								pDest += nZeroCount;
							}
						}
					}
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL)
						{
							if( ndxE <= clipXL + basewidth )//그리는 영역보다 작거나 같을때
							{
								nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS;								
							}
							else//그리는 영역 보다 클때
							{
								nAddCount = ndxE - basewidth - clipXL;
								pSrc += clipXL -ndxS; nCopyCount -= clipXL - ndxS + nAddCount;
							}
						}
						else if( ndxS >= clipXL + basewidth )
						{
							pSrc += nCopyCount;
							nCopyCount = 0;
							continue;
						}
						else
						{
							if( ndxE <= clipXL + basewidth )
							{
								pDest += ndxS - clipXL;
							}
							else
							{
								pDest += ndxS - clipXL;
								nAddCount = ndxE - clipXL - basewidth;
								nCopyCount -= nAddCount;
							}
						}
					}

				}//if(clipXL > 0) // 왼쪽 클리핑
				else
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}

				}

				if( nCopyCount != 0 )
				//PutPixelHalf(pDest, pSrc, nCopyCount, dwHalf16Mask, dwHalf32Mask);
				PutPixelBitStepTrans(pDest,pSrc,nCopyCount, nAlphaDst,nAlphaSrc, m_dwRBGBitMask, m_dwGRBBitMask);
			}
		}
	}
	pSurface->Unlock();

}

void CUser::BltLSPHalfAlpha(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data,DWORD dwHalf16Mask, DWORD dwHalf32Mask)
{
	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이
	int i = 0;

	const WORD* pSrc = data; 
//	int nLine = *pSrc; pSrc++; // 높이가 기록되어 있다.
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	if( NULL == pSurface->Lock() ) return;
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left; 
	int height = rcClip.bottom - rcClip.top;

	if( CChildView::CharManager.GetPixelFormat() == 565 ) 
	{
	}
	else
	{
	}

	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				PutPixelHalf(pDest, pSrc, nCopyCount, dwHalf16Mask, dwHalf32Mask);
				//PutPixelNormal(pDest, pSrc, nCopyCount);

			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = -rc.left;
		int clipXR = rc.right - ScreenX;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;
			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ >= clipXL) { pDest += nZeroCount; }
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL) { nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS; }
						else { pDest += ndxS - clipXL; }
					}
				}
				if(clipXR > 0) // 오른쪽 클리핑
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}
				}
				PutPixelHalf(pDest, pSrc, nCopyCount, dwHalf16Mask, dwHalf32Mask);
//				PutPixelNormal(pDest, pSrc, nCopyCount);
			}
		}
	}
	pSurface->Unlock();
}
void CUser::BltLSPHalfAlphaEx(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data,DWORD dwHalf16Mask, DWORD dwHalf32Mask)
{

	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= 495) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이


	WORD* pSrc = data; 
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	pSurface->Lock();
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+rcClip.top*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;
	int basewidth = ScreenX;

	if( height >= g_vDispPixSize.cy ) height = g_vDispPixSize.cy;

	WORD* pDest = NULL;
	
	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산
				PutPixelHalf(pDest, pSrc, nCopyCount, dwHalf16Mask, dwHalf32Mask);
//				PutPixelNormal( pDest, pSrc, nCopyCount);

				//CUser::PutPixelShadowEx(pDest, nCopyCount, dwHalf16Mask, dwHalf32Mask);
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = - rc.left;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;

			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nAddCount = 0;
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ > clipXL + basewidth)
					{
						pSrc += nCopyCount;
						nCopyCount = 0;

						continue;
					}
					if(ndxZ >= clipXL)
					{
						if( ndxS >= clipXL + basewidth )
						{
							pSrc += nCopyCount;
							nCopyCount = 0;
							continue;
						}
						else
						{
							if( ndxE > clipXL + basewidth )
							{
								pDest += nZeroCount;
								nAddCount = ndxE - clipXL - basewidth;
								nCopyCount -= nAddCount;
							}
							else
							{
								pDest += nZeroCount;
							}
						}
					}
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL)
						{
							if( ndxE <= clipXL + basewidth )//그리는 영역보다 작거나 같을때
							{
								nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS;								
							}
							else//그리는 영역 보다 클때
							{
								nAddCount = ndxE - basewidth - clipXL;
								pSrc += clipXL -ndxS; nCopyCount -= clipXL - ndxS + nAddCount;
							}
						}
						else if( ndxS >= clipXL + basewidth )
						{
							pSrc += nCopyCount;
							nCopyCount = 0;
							continue;
						}
						else
						{
							if( ndxE <= clipXL + basewidth )
							{
								pDest += ndxS - clipXL;
							}
							else
							{
								pDest += ndxS - clipXL;
								nAddCount = ndxE - clipXL - basewidth;
								nCopyCount -= nAddCount;
							}
						}
					}

				}//if(clipXL > 0) // 왼쪽 클리핑
				else
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}

				}

				if( nCopyCount != 0 )
				PutPixelHalf(pDest, pSrc, nCopyCount, dwHalf16Mask, dwHalf32Mask);
//				PutPixelNormal( pDest, pSrc, nCopyCount);
				//CUser::PutPixelShadowEx(pDest, nCopyCount, dwHalf16Mask, dwHalf32Mask);
			}
		}
	}
	pSurface->Unlock();

}

void CUser::SetInvAll(PBYTE bypData)
{
	// 이곳이 이상..냠냠..
	for(int i=0; i < 50;i++)
	{
		m_MyInvData[i].m_vUsage = 0;
	}
	for(i=0; i < 4;i++)
	{
		m_MyBeltData[i].m_vUsage = 0;
	}

	int offset = 0;
	short count = CPacketParser::GetShort(bypData, offset);
	short belt_count = CPacketParser::GetShort(bypData, offset);
	short t_index;
	BYTE len;

	for(i=0; i < count;i++)
	{
		t_index = CPacketParser::GetShort(bypData, offset);
		m_MyInvData[t_index].m_vIndex = t_index;
		m_MyInvData[t_index].m_vType = CPacketParser::GetByte(bypData, offset);
		m_MyInvData[t_index].m_vArmPos = CPacketParser::GetByte(bypData, offset);
		m_MyInvData[t_index].m_vPicNum = CPacketParser::GetShort(bypData, offset);
		m_MyInvData[t_index].m_vGender = CPacketParser::GetByte(bypData, offset);
		m_MyInvData[t_index].m_vMinDamage = CPacketParser::GetShort(bypData, offset);
		m_MyInvData[t_index].m_vMaxDamage = CPacketParser::GetShort(bypData, offset);
		m_MyInvData[t_index].m_vValue = CPacketParser::GetDword(bypData, offset);
		m_MyInvData[t_index].m_vLimitStr = CPacketParser::GetShort(bypData, offset);
		m_MyInvData[t_index].m_vLimitInt = CPacketParser::GetShort(bypData, offset);
		m_MyInvData[t_index].m_vLimitSkill = CPacketParser::GetShort(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)m_MyInvData[t_index].m_strMagic.GetBuffer(len), len);
			m_MyInvData[t_index].m_strMagic.ReleaseBuffer();
		}
		else m_MyInvData[t_index].m_strMagic.Empty();
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)m_MyInvData[t_index].m_strSNum.GetBuffer(len), len);
			m_MyInvData[t_index].m_strSNum.ReleaseBuffer();
		}
		else m_MyInvData[t_index].m_strSNum.Empty();
		for(int k = 0; k < 7; k++) m_MyInvData[t_index].m_vSpecial[k] = CPacketParser::GetShort(bypData, offset);
		m_MyInvData[t_index].m_vUsage = CPacketParser::GetShort(bypData, offset);
		m_MyInvData[t_index].m_vMaxUsage = CPacketParser::GetShort(bypData, offset);
		m_MyInvData[t_index].m_vOrgMaxUsage = CPacketParser::GetShort(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)m_MyInvData[t_index].m_vName.GetBuffer(len), len);
			m_MyInvData[t_index].m_vName.ReleaseBuffer();
		}
		else m_MyInvData[t_index].m_vName.Empty();
	}

	for(i=0; i < belt_count;i++)
	{
		t_index = CPacketParser::GetShort(bypData, offset);
		m_MyBeltData[t_index].m_vIndex = t_index;
		m_MyBeltData[t_index].m_vType = CPacketParser::GetByte(bypData, offset);
		m_MyBeltData[t_index].m_vArmPos = CPacketParser::GetByte(bypData, offset);
		m_MyBeltData[t_index].m_vPicNum = CPacketParser::GetShort(bypData, offset);
		m_MyBeltData[t_index].m_vGender = CPacketParser::GetByte(bypData, offset);
		m_MyBeltData[t_index].m_vMinDamage = CPacketParser::GetShort(bypData, offset);
		m_MyBeltData[t_index].m_vMaxDamage = CPacketParser::GetShort(bypData, offset);
		m_MyBeltData[t_index].m_vValue = CPacketParser::GetDword(bypData, offset);
		m_MyBeltData[t_index].m_vLimitStr = CPacketParser::GetShort(bypData, offset);
		m_MyBeltData[t_index].m_vLimitInt = CPacketParser::GetShort(bypData, offset);
		m_MyBeltData[t_index].m_vLimitSkill = CPacketParser::GetShort(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)m_MyBeltData[t_index].m_strMagic.GetBuffer(len), len);
			m_MyBeltData[t_index].m_strMagic.ReleaseBuffer();
		}
		else m_MyBeltData[t_index].m_strMagic.Empty();
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)m_MyBeltData[t_index].m_strSNum.GetBuffer(len), len);
			m_MyBeltData[t_index].m_strSNum.ReleaseBuffer();
		}
		else m_MyBeltData[t_index].m_strSNum.Empty();
		for(int k = 0; k < 7; k++) m_MyBeltData[t_index].m_vSpecial[k] = CPacketParser::GetShort(bypData, offset);
		m_MyBeltData[t_index].m_vUsage = CPacketParser::GetShort(bypData, offset);
		m_MyBeltData[t_index].m_vMaxUsage = CPacketParser::GetShort(bypData, offset);
		m_MyBeltData[t_index].m_vOrgMaxUsage = CPacketParser::GetShort(bypData, offset);
		len = CPacketParser::GetByte(bypData, offset);
		if(len > 0)
		{
			CPacketParser::GetString(bypData, offset, (PBYTE)m_MyBeltData[t_index].m_vName.GetBuffer(len), len);
			m_MyBeltData[t_index].m_vName.ReleaseBuffer();
		}
		else m_MyBeltData[t_index].m_vName.Empty();
	}
}

void CUser::Chang_Item(short fromindex, short toindex)// 장착 인벤토리에서 가방 인벤토리로 이동할때..또는 반대..
{
	CMyInvData inv_data;

	inv_data = m_MyInvData[toindex];
	m_MyInvData[toindex] = m_MyInvData[fromindex];
	m_MyInvData[fromindex] = inv_data;
}

void CUser::Change_View_item(short index, short picnum)
{
	if(picnum < 0) picnum = 0; 
	//if(index == 0 && picnum == 0) picnum = 1;      // 머리는 항상 디폴트 머리를 가지고 있어야 한다..
//	else if(picnum <= 0) return; // 나중에 캐릭터 생성시 데이타를 이용해서 디폴트값을 줘야한다. 10 월 17일..우석.

	switch((int)index)
	{
	case 0:// 머리
 			SetWearItem(AM_KIND_HELMET, (int)picnum);
		break;
	case 1:// 귀
	case 2: // 목 
		break;
	case 3:// 상체 갑옷
			SetWearItem(AM_KIND_ARMOR, (int)picnum);
		break;
	case 4:// 바지
			SetWearItem(AM_KIND_JEAN, (int)picnum);
		break;
	case 5://신발
			SetWearItem(AM_KIND_BOOT, (int)picnum);
		break;
	case 6:// 오른손..검..
 			SetWearItem(AM_KIND_ARMS, (int)picnum);		 
		break;
	case 7:// 왼손 방패
			SetWearItem(AM_KIND_SHIELD, (int)picnum);
		break;
	case 8:// 오른 팔찌..
	case 9:// 오니 팔찌..
		break;
	}

	SetMotion(m_AnimationIndex);
}

int CUser::GetJangchk_Item(int index)
{
	if(index < 0 && index > 10) return 0;
	return m_MyInvData[index].m_vPicNum;// 그림 번호를 리턴해준다.
}

short CUser::GetInv_ItemUsage(int index)
{
	return m_MyInvData[index].m_vUsage;
}

int CUser::GetInv_ItemPic(int index)
{
//	if(index < 10 && index > 50) return 0;
	//그림 번호..
	return m_MyInvData[index].m_vPicNum;// 그림 번호를 리턴해준다.
}

int CUser::GetInv_ItemType(int index)
{
	// 무슨 종류인지를 리턴해준다.
	return m_MyInvData[index].m_vType;
}

int CUser::GetInv_ItemIndex(int index)
{
	// 몇번째인지 인덱스를 보내준다..이 인덱스는 서버에서 받아 오는것..내가 못바꾼다.
	return m_MyInvData[index].m_vIndex;
}

int CUser::GetInv_ItemArm(int index)
{
	// 장착위를 가려켜준다.
	return m_MyInvData[index].m_vArmPos;
}

CString CUser::GetInv_ItemName(int index)
{
	// 장착위를 가려켜준다.
	return m_MyInvData[index].m_vName;
}

void CUser::SendRevival(int type)
{
	BYTE buf[10];
	int offset = 0;

	//type 1 바로 살아 나기 2 마을에서 살아 나가 
	CPacketParser::SetByte(buf, offset, PKT_LIFE); //최종점 요청
	CPacketParser::SetByte(buf, offset, (BYTE)type);
	CChildView::SocketMng.Send(offset, buf);
}

void CUser::View_Id(CDC *pDC, CPoint bgpos)
{
	int x = m_Position.x - bgpos.x - 50;
	int y = m_Position.y - bgpos.y - 80;
	int DEFAULT_LENGTH = 100;

	CRect r1, r2;

	pDC->SetBkMode(TRANSPARENT);

	r1.SetRect(0, 0, DEFAULT_LENGTH, 14);
	pDC->DrawText(m_strUserId, &r1, DT_TOP|DT_EXTERNALLEADING|DT_SINGLELINE|DT_CALCRECT);
	if(m_iUserId < 10000 && m_Guild.IsEmpty() == FALSE)		// 길드가 있으면 길드 이름 표시
	{
		r2.SetRect(0, 0, DEFAULT_LENGTH, 14);
		pDC->DrawText(m_Guild, &r2, DT_TOP|DT_EXTERNALLEADING|DT_SINGLELINE|DT_CALCRECT);
		r2.OffsetRect(x+((DEFAULT_LENGTH-r2.right)>>1)+3, y-r1.bottom-r2.bottom);
	}
	r1.OffsetRect(x+((DEFAULT_LENGTH-r1.right)>>1)+3, y-r1.bottom);

	if(m_iUserId < 10000 && m_Guild.IsEmpty() == FALSE)
	{
		pDC->SetTextColor(RGB(0,0,0));
		pDC->DrawText(m_Guild, &r2, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		r2.OffsetRect(-1, -1);
		pDC->SetTextColor(RGB(255,0,0));
		pDC->DrawText(m_Guild, &r2, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}

	// 성향에 따라 색깔을 바꾼다.
	pDC->SetTextColor(RGB(0,0,0));
	pDC->DrawText(m_strUserId, &r1, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	r1.OffsetRect(-1, -1);
	if(m_iUserId < 10000) pDC->SetTextColor(m_vIdColor);
	else pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(m_strUserId, &r1, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
}


void CUser::View_BubbleChat(CPoint bgpos)
{
	DWORD cur = GetTickCount();
	int DEFAULT_BUBBLECHAT_WIDTH = 140;
	int len = m_szChatString.GetLength();

	CPoint pos;

	pos.x = m_Position.x - bgpos.x - 70;
	pos.y = m_Position.y - bgpos.y - 80;

	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
	
	pDC->SetBkMode(TRANSPARENT);
	CFont pFont, bold_font, *old_font;
	CRect r, r1, r2;

	////////////////////////
	pFont.CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, DEFAULT_PITCH, "굴림");
	bold_font.CreateFont(-12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, DEFAULT_PITCH, "굴림");
	old_font = pDC->SelectObject(&pFont);
	/////////////////////////

	if ( cur - m_dwBubbleTick < DEFAULT_BUBBLECHAT_DELAY+(DWORD)len*1000)
	{
		r.SetRect(0, 0, DEFAULT_BUBBLECHAT_WIDTH, 14);
		r1.SetRect(0, 0, DEFAULT_BUBBLECHAT_WIDTH, 14);
		pDC->SelectObject(&bold_font);
		pDC->DrawText(m_strUserId, &r1, DT_TOP|DT_EXTERNALLEADING|DT_SINGLELINE|DT_CALCRECT);
		pDC->SelectObject(&pFont);
		pDC->DrawText(m_szChatString, &r, DT_WORDBREAK|DT_TOP|DT_EXTERNALLEADING|DT_CALCRECT|DT_EDITCONTROL);
		if(r.right > DEFAULT_BUBBLECHAT_WIDTH) r.right = DEFAULT_BUBBLECHAT_WIDTH;
		if(m_Guild.IsEmpty() == FALSE)		// 길드가 있으면 길드 이름 표시
		{
			r2.SetRect(0, 0, DEFAULT_BUBBLECHAT_WIDTH, 14);
			pDC->SelectObject(&bold_font);
			pDC->DrawText(m_Guild, &r2, DT_TOP|DT_EXTERNALLEADING|DT_SINGLELINE|DT_CALCRECT);
			r2.OffsetRect(pos.x+((DEFAULT_BUBBLECHAT_WIDTH-r2.right)>>1)+3, pos.y-r.bottom-r1.bottom-r2.bottom);
		}
		r1.OffsetRect(pos.x+((DEFAULT_BUBBLECHAT_WIDTH-r1.right)>>1)+3, pos.y-r.bottom-r1.bottom);
		r.OffsetRect(pos.x+((DEFAULT_BUBBLECHAT_WIDTH-r.right)>>1)+3, pos.y-r.bottom);

		if(m_Guild.IsEmpty() == FALSE)		// 길드가 있으면 길드 이름 표시
		{
			pDC->SelectObject(&bold_font);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->TextOut(r2.left, r2.top, m_Guild);
			r2.OffsetRect(-1, -1);
			pDC->SetTextColor(RGB(255,0,0));
			pDC->TextOut(r2.left, r2.top, m_Guild);
		}

		// 검은색
		pDC->SelectObject(&bold_font);
		pDC->SetTextColor(RGB(0,0,0));
		pDC->TextOut(r1.left, r1.top, m_strUserId);
		pDC->SelectObject(&pFont);
		pDC->DrawText(m_szChatString, &r, DT_WORDBREAK|DT_TOP|DT_EXTERNALLEADING|DT_EDITCONTROL);
		
		r1.OffsetRect(-1, -1);
		if(m_iUserId < 10000) pDC->SetTextColor(m_vIdColor);
		else pDC->SetTextColor(RGB(255,255,255));
		pDC->SelectObject(&bold_font);
		pDC->TextOut(r1.left, r1.top, m_strUserId);

		// 흰색		
		if (cur-m_dwBubbleTick < (DEFAULT_BUBBLECHAT_DELAY+(DWORD)len*2000)/2)
			pDC->SetTextColor(RGB(255,255,255));
		else
			pDC->SetTextColor(RGB(192,192,192));
		
		r.OffsetRect(-1, -1);
		pDC->SelectObject(&pFont);
		pDC->DrawText(m_szChatString, &r, DT_WORDBREAK|DT_TOP|DT_EXTERNALLEADING|DT_EDITCONTROL);
	}
	else if(m_bFocus == TRUE)
	{
		pDC->SelectObject(&bold_font);
		View_Id(pDC, bgpos);
	}

	pDC->SelectObject(old_font);
	pFont.DeleteObject();
	bold_font.DeleteObject();
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);
	m_bFocus = FALSE;
/////////////////////////////////////////////////////////
}

void CUser::Del_Item(BYTE bBelt, short index)
{
	if(bBelt == BASIC_INV)
	{
		// 아이템을 삭제한다. 
		m_MyInvData[index].Init();
	}
	else
	{
		m_MyBeltData[index].Init();
	}
}

void CUser::Add_Item(short index, BYTE type, BYTE arm, short picnum, BYTE gender, short min_damage, short max_damage, short limit_str, short limit_int, short limit_skill, CString magic_name, CString s_num, short special[], DWORD value, short use_count, short max_use_count, short org_max_use_count, CString name)
{
	// 아이템을 추가한다. 
	m_MyInvData[index].m_vIndex = index;
	m_MyInvData[index].m_vType = type;
	m_MyInvData[index].m_vArmPos = arm;
	m_MyInvData[index].m_vPicNum = picnum;
	m_MyInvData[index].m_vGender = gender;
	m_MyInvData[index].m_vMinDamage = min_damage;
	m_MyInvData[index].m_vMaxDamage = max_damage;
	m_MyInvData[index].m_vLimitStr = limit_str;
	m_MyInvData[index].m_vLimitInt = limit_int;
	m_MyInvData[index].m_vLimitSkill = limit_skill;
	m_MyInvData[index].m_strMagic = magic_name;
	m_MyInvData[index].m_strSNum = s_num;
	for(int i = 0; i < 7; i++) m_MyInvData[index].m_vSpecial[i] = special[i];
	m_MyInvData[index].m_vValue = value;
	m_MyInvData[index].m_vUsage = use_count;
	m_MyInvData[index].m_vMaxUsage = max_use_count;
	m_MyInvData[index].m_vOrgMaxUsage = org_max_use_count;
	m_MyInvData[index].m_vName = name;
}

void CUser::Add_Item(PBYTE bypData)
{
	int offset = 0;
	BYTE len;

	short index = CPacketParser::GetShort(bypData, offset);
	// 아이템을 추가한다. 
	m_MyInvData[index].m_vIndex = index;
	m_MyInvData[index].m_vType = CPacketParser::GetByte(bypData, offset);
	m_MyInvData[index].m_vArmPos = CPacketParser::GetByte(bypData, offset);
	m_MyInvData[index].m_vPicNum = CPacketParser::GetShort(bypData, offset);
	m_MyInvData[index].m_vGender = CPacketParser::GetByte(bypData, offset);
	m_MyInvData[index].m_vMinDamage = CPacketParser::GetShort(bypData, offset);
	m_MyInvData[index].m_vMaxDamage = CPacketParser::GetShort(bypData, offset);
	m_MyInvData[index].m_vValue = CPacketParser::GetDword(bypData, offset);
	m_MyInvData[index].m_vLimitStr = CPacketParser::GetShort(bypData, offset);
	m_MyInvData[index].m_vLimitInt = CPacketParser::GetShort(bypData, offset);
	m_MyInvData[index].m_vLimitSkill = CPacketParser::GetShort(bypData, offset);
	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		CPacketParser::GetString(bypData, offset, (PBYTE)m_MyInvData[index].m_strMagic.GetBuffer(len), len);
		m_MyInvData[index].m_strMagic.ReleaseBuffer();
	}
	else m_MyInvData[index].m_strMagic.Empty();
	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		CPacketParser::GetString(bypData, offset, (PBYTE)m_MyInvData[index].m_strSNum.GetBuffer(len), len);
		m_MyInvData[index].m_strSNum.ReleaseBuffer();
	}
	else m_MyInvData[index].m_strSNum.Empty();
	for(int k = 0; k < 7; k++) m_MyInvData[index].m_vSpecial[k] = CPacketParser::GetShort(bypData, offset);
	m_MyInvData[index].m_vUsage = CPacketParser::GetShort(bypData, offset);
	m_MyInvData[index].m_vMaxUsage = CPacketParser::GetShort(bypData, offset);
	m_MyInvData[index].m_vOrgMaxUsage = CPacketParser::GetShort(bypData, offset);
	len = CPacketParser::GetByte(bypData, offset);
	if(len > 0)
	{
		CPacketParser::GetString(bypData, offset, (PBYTE)m_MyInvData[index].m_vName.GetBuffer(len), len);
		m_MyInvData[index].m_vName.ReleaseBuffer();
	}
	else m_MyInvData[index].m_vName.Empty();
}

BOOL CUser::MoveItemToBelt(short from, short to)
{
	if(from < 0 || from >= INV_COUNT || to < 0 || to >= BELT_COUNT) return FALSE;

	if(m_MyBeltData[to].m_vUsage > 0)
	{
		CMyInvData t_inv;

		t_inv = m_MyBeltData[to];
		m_MyBeltData[to] = m_MyInvData[from];
		m_MyInvData[from] = t_inv;
	}
	else
	{
		m_MyBeltData[to] = m_MyInvData[from];
		m_MyInvData[from].Init();
	}

	return TRUE;
}

BOOL CUser::MoveItemToInv(short from, short to)
{
	if(from < 0 || from >= BELT_COUNT || to < 0 || to >= INV_COUNT) return FALSE;

	if(m_MyInvData[to].m_vUsage > 0)
	{
		CMyInvData t_inv;

		t_inv = m_MyInvData[to];
		m_MyInvData[to] = m_MyBeltData[from];
		m_MyBeltData[from] = t_inv;
	}
	else
	{
		m_MyInvData[to] = m_MyBeltData[from];
		m_MyBeltData[from].Init();
	}

	return TRUE;
}

void CUser::AddMagic(short number, BYTE method1, BYTE method2, short start_time, BYTE type, BYTE target, CString name, CString explain, short distance, short use_mp, int use_time, short damage, short game_pic)
{
	CMyMagicData *magic_data, *org_data;

	magic_data = new CMyMagicData;
	magic_data->m_vNumber = number;
	magic_data->m_vMethod = method1;
	magic_data->m_vCharOrAxis = method2;
	magic_data->m_vStartTime = start_time;
	magic_data->m_vType = type;
	magic_data->m_vTgtMethod = target;
	magic_data->m_vName = name;
	magic_data->m_vExplain = explain;
	magic_data->m_vDistance = distance;
	magic_data->m_vUseMp = use_mp;
	magic_data->m_vUseTime = use_time;
	magic_data->m_vDamage = damage;
	magic_data->m_vGamePic = game_pic;

	if(m_mapMyMagicData.Lookup(number, org_data))
	{
		int count, i;
		switch(org_data->m_vType)
		{
		case BLUE_MAGIC:
			count = m_arrBlueMagic.GetSize();
			for(i = 0; i < count; i++)
			{
				if(m_arrBlueMagic[i]->m_vNumber == org_data->m_vNumber)
				{
					m_arrBlueMagic.RemoveAt(i);
					break;
				}
			}
			break;
		case WHITE_MAGIC:
			count = m_arrWhiteMagic.GetSize();
			for(i = 0; i < count; i++)
			{
				if(m_arrWhiteMagic[i]->m_vNumber == org_data->m_vNumber)
				{
					m_arrWhiteMagic.RemoveAt(i);
					break;
				}
			}
			break;
		case BLACK_MAGIC:
			count = m_arrBlackMagic.GetSize();
			for(i = 0; i < count; i++)
			{
				if(m_arrBlackMagic[i]->m_vNumber == org_data->m_vNumber)
				{
					m_arrBlackMagic.RemoveAt(i);
					break;
				}
			}
			break;
		}
		delete org_data;
		m_mapMyMagicData.RemoveKey(number);
	}

	m_mapMyMagicData[number] = magic_data;
	switch(magic_data->m_vType)
	{
	case SPECIAL_MAGIC:
		m_arrSpecialMagic.Add(magic_data);
		break;
	case BLUE_MAGIC:
		m_arrBlueMagic.Add(magic_data);
		break;
	case WHITE_MAGIC:
		m_arrWhiteMagic.Add(magic_data);
		break;
	case BLACK_MAGIC:
		m_arrBlackMagic.Add(magic_data);
		break;
	}
}

void CUser::DelMagic(short number)
{
	CMyMagicData *org_data;

	if(m_mapMyMagicData.Lookup(number, org_data))
	{
		int count, i;
		switch(org_data->m_vType)
		{
		case BLUE_MAGIC:
			count = m_arrBlueMagic.GetSize();
			for(i = 0; i < count; i++)
			{
				if(m_arrBlueMagic[i]->m_vNumber == org_data->m_vNumber)
				{
					m_arrBlueMagic.RemoveAt(i);
					break;
				}
			}
			break;
		case WHITE_MAGIC:
			count = m_arrWhiteMagic.GetSize();
			for(i = 0; i < count; i++)
			{
				if(m_arrWhiteMagic[i]->m_vNumber == org_data->m_vNumber)
				{
					m_arrWhiteMagic.RemoveAt(i);
					break;
				}
			}
			break;
		case BLACK_MAGIC:
			count = m_arrBlackMagic.GetSize();
			for(i = 0; i < count; i++)
			{
				if(m_arrBlackMagic[i]->m_vNumber == org_data->m_vNumber)
				{
					m_arrBlackMagic.RemoveAt(i);
					break;
				}
			}
			break;
		}
		delete org_data;
		m_mapMyMagicData.RemoveKey(number);
	}
}

void CUser::ChangeMoney(DWORD money)
{
	m_Money = money;
}

void CUser::ChangeItemInfo(BYTE bBelt, BYTE nSpec, CMyInvData& inv_data)
{
	if(bBelt == 0)
	{
		m_MyBeltData[inv_data.m_vIndex].m_vIndex = inv_data.m_vIndex;
		if(nSpec&INFO_TYPE)
		{
			m_MyBeltData[inv_data.m_vIndex].m_vType = inv_data.m_vType;
			m_MyBeltData[inv_data.m_vIndex].m_vGender = inv_data.m_vGender;
		}
		if(nSpec&INFO_ARM) m_MyBeltData[inv_data.m_vIndex].m_vArmPos = inv_data.m_vArmPos;
		if(nSpec&INFO_PICNUM) m_MyBeltData[inv_data.m_vIndex].m_vPicNum = inv_data.m_vPicNum;
		if(nSpec&INFO_DUR) m_MyBeltData[inv_data.m_vIndex].m_vUsage = inv_data.m_vUsage;
		if(nSpec&INFO_NAME) m_MyBeltData[inv_data.m_vIndex].m_vName = inv_data.m_vName;
		if(nSpec&INFO_DAMAGE)
		{
			m_MyBeltData[inv_data.m_vIndex].m_vMinDamage = inv_data.m_vMinDamage;
			m_MyBeltData[inv_data.m_vIndex].m_vMaxDamage = inv_data.m_vMaxDamage;
		}
		if(nSpec&INFO_LIMIT)
		{
			m_MyBeltData[inv_data.m_vIndex].m_vMaxUsage = inv_data.m_vMaxUsage;
			m_MyBeltData[inv_data.m_vIndex].m_vOrgMaxUsage = inv_data.m_vOrgMaxUsage;
			m_MyBeltData[inv_data.m_vIndex].m_vValue = inv_data.m_vValue;
			m_MyBeltData[inv_data.m_vIndex].m_vLimitStr = inv_data.m_vLimitStr;
			m_MyBeltData[inv_data.m_vIndex].m_vLimitInt = inv_data.m_vLimitInt;
			m_MyBeltData[inv_data.m_vIndex].m_vLimitSkill = inv_data.m_vLimitSkill;
			m_MyBeltData[inv_data.m_vIndex].m_strMagic = inv_data.m_strMagic;
			m_MyBeltData[inv_data.m_vIndex].m_strSNum = inv_data.m_strSNum;
		}
		if(nSpec&INFO_SPECIAL)
		{
			for(int k = 0; k < 7; k++) m_MyBeltData[inv_data.m_vIndex].m_vSpecial[k] = inv_data.m_vSpecial[k];
		}
	}
	else
	{
		m_MyInvData[inv_data.m_vIndex].m_vIndex = inv_data.m_vIndex;
		if(nSpec&INFO_TYPE)
		{
			m_MyInvData[inv_data.m_vIndex].m_vType = inv_data.m_vType;
			m_MyInvData[inv_data.m_vGender].m_vGender = inv_data.m_vGender;
		}
		if(nSpec&INFO_ARM) m_MyInvData[inv_data.m_vIndex].m_vArmPos = inv_data.m_vArmPos;
		if(nSpec&INFO_PICNUM) m_MyInvData[inv_data.m_vIndex].m_vPicNum = inv_data.m_vPicNum;
		if(nSpec&INFO_DUR) m_MyInvData[inv_data.m_vIndex].m_vUsage = inv_data.m_vUsage;
		if(nSpec&INFO_NAME) m_MyInvData[inv_data.m_vIndex].m_vName = inv_data.m_vName;
		if(nSpec&INFO_DAMAGE)
		{
			m_MyInvData[inv_data.m_vIndex].m_vMinDamage = inv_data.m_vMinDamage;
			m_MyInvData[inv_data.m_vIndex].m_vMaxDamage = inv_data.m_vMaxDamage;
		}
		if(nSpec&INFO_LIMIT)
		{
			m_MyInvData[inv_data.m_vIndex].m_vMaxUsage = inv_data.m_vMaxUsage;
			m_MyInvData[inv_data.m_vIndex].m_vOrgMaxUsage = inv_data.m_vOrgMaxUsage;
			m_MyInvData[inv_data.m_vIndex].m_vValue = inv_data.m_vValue;
			m_MyInvData[inv_data.m_vIndex].m_vLimitStr = inv_data.m_vLimitStr;
			m_MyInvData[inv_data.m_vIndex].m_vLimitInt = inv_data.m_vLimitInt;
			m_MyInvData[inv_data.m_vIndex].m_vLimitSkill = inv_data.m_vLimitSkill;
			m_MyInvData[inv_data.m_vIndex].m_strMagic = inv_data.m_strMagic;
			m_MyInvData[inv_data.m_vIndex].m_strSNum = inv_data.m_strSNum;
		}
		if(nSpec&INFO_SPECIAL)
		{
			for(int k = 0; k < 7; k++) m_MyInvData[inv_data.m_vIndex].m_vSpecial[k] = inv_data.m_vSpecial[k];
		}
	}
}

void CUser::BltLSPDrawLine(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data,int skip_line)
{
	if(data == NULL) return;
	if(pRC == NULL) return;

	int ScreenX = m_bpDirectXMng->GetScreenSize().cx;
	int ScreenY = m_bpDirectXMng->GetScreenSize().cy;

	if(pRC->right - pRC->left <= 0 || pRC->bottom - pRC->top <= 0) return; // 유효한 픽셀이 없으면 돌아간다..
	RECT rc = {x+pRC->left, y+pRC->top, x+pRC->right, y+pRC->bottom};
	if(rc.right < 0 || rc.bottom < 0 || rc.left >= ScreenX || rc.top >= ScreenY) return; // 화면을 완전히 벗어남..

	RECT rcClip; // 클리핑 영역을 구하고 옵셋도 구한다.
	BOOL bUseXClip = GetClippedRect(&rc, &rcClip); // 원래 영역, 클리핑 처리후 영역

	int nNode; // 한라인당 노드수..
	int nZeroCount = 0; // 건너뛰는 픽셀의 길이
	int nCopyCount = 0; // 복사할 픽셀의 길이
	int i = 0;

	const WORD* pSrc = data; 
//	int nLine = *pSrc; pSrc++; // 높이가 기록되어 있다.
	// 잘리는 영역 줄수 만큼 데이터 포인터 이동..
	if(rc.top < 0)
	{
		int skipY = -rc.top;
		for(int i = 0; i < skipY; i++) // 스킵 줄수..
		{
			nNode = *pSrc; pSrc++; // 노드수;
			for(;nNode; nNode--)
			{
				pSrc++; // 0 의 숫자
				pSrc += *pSrc + 1; // 유효 픽셀 숫자
			}
		}
	}

	for(int j = 0; j < skip_line; j++) // 스킵 줄수..
	{
		nNode = *pSrc; pSrc++; // 노드수;
		for(;nNode; nNode--)
		{
			pSrc++; // 0 의 숫자
			pSrc += *pSrc + 1; // 유효 픽셀 숫자
		}
	}

	if( NULL == pSurface->Lock() ) return;
	DDSURFACEDESC2& ddsdDest = pSurface->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+rcClip.left*2+(rcClip.top+skip_line)*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.
	int width = rcClip.right - rcClip.left; 
	int height = rcClip.bottom - rcClip.top - skip_line;

	if(bUseXClip == FALSE) // X 축 클리핑이 필요없다면..
	{
		int nPixelCount = 0; // 건너뛰는 유효 픽셀의 길이
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg + pitchDest * y;
			nNode = *pSrc; pSrc++;
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc+=nCopyCount) // 픽셀수 만큼 포인터 이동..
			{
				pDest += *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀, 복사할 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}

				//테두리처리 {
				if (TRUE)
				{
					if( *(pDest-1) & m_CheckKey )
						*(pDest-1) = m_LineKey;
					if( *(pDest+nCopyCount) & m_CheckKey )
						*(pDest+nCopyCount) = m_LineKey;
					if (y==0)
					{
//						int count = nCopyCount;
//						while (count)
//						{
//							if( *(pDest+count) & 0x20 );
//								*(pDest+count) = 0x07e0;
//							count--;
//						}
						continue;
					}
				}
				// }
			}
		}
	}
	else // X 축 클리핑이 일어난다면..
	{
		int nAddCount = 0; // 0 픽셀의 길이, 유효 픽셀 길이
		int ndxZ, ndxS, ndxE; // 라인의 현재 시작, 픽셀시작, 끝 위치 인덱스..
		int clipXL = -rc.left;
		int clipXR = rc.right - ScreenX;
		
		for(y = 0; y < height; y++)
		{
			WORD* pDest = pDestOrg+pitchDest*y;
			nNode = *pSrc; pSrc++;
			ndxE = 0; // 라인의 현재 인덱스
			for(;nNode;nNode--, pDest+=nCopyCount, pSrc += nCopyCount + nAddCount)
			{
				nZeroCount = *pSrc; pSrc++; // 0 부분 skip...
				nCopyCount = *pSrc; pSrc++; // 유효 픽셀 숫자 계산
				if(nCopyCount == 0)
				{
					pSurface->Unlock();
					return;
				}
				ndxZ = ndxE;
				ndxS = ndxE + nZeroCount;
				ndxE = ndxS + nCopyCount;
				if(clipXL > 0) // 왼쪽 클리핑
				{
					if(ndxE <= clipXL) { pSrc += nCopyCount; nCopyCount = 0; continue; }
					if(ndxZ >= clipXL) { pDest += nZeroCount; }
					else if(ndxZ < clipXL)
					{
						if(ndxS < clipXL) { nCopyCount -= clipXL - ndxS; pSrc += clipXL - ndxS; }
						else { pDest += ndxS - clipXL; }
					}
				}
				if(clipXR > 0) // 오른쪽 클리핑
				{
					nAddCount = 0;
					if(ndxZ >= width || ndxS >= width) continue;
					pDest += nZeroCount;
					if(ndxE > width)
					{
						nCopyCount -= ndxE - width;
						nAddCount = ndxE - width;
					}
				}
				//테두리처리 {
				if (TRUE)
				{
					if( *(pDest-1) & m_CheckKey )
						*(pDest-1) = m_LineKey;
					if( *(pDest+nCopyCount) & m_CheckKey  )
						*(pDest+nCopyCount) = m_LineKey;
					if (y==0)
					{
//						int count = nCopyCount;
//						while (count)
//						{
//							if( *(pDest+count) & 0x20 );
//								*(pDest+count) = 0x07e0;
//							count--;
//						}
						continue;
					}
				}
				// }
			}
		}
	}
	pSurface->Unlock();
}

void CUser::DrawLine(CDDSurface* pSurface, CPoint PosMapOffset,int skip_line)
{
	if (NULL==m_bpCharManager) return;
	if (NULL==m_bpCharRes) return;
	if (NULL==m_bpSprData) return;

	CPoint ptScreen = PosMapOffset;

	RECT rcV = m_bpSprData->rcV;	//캐릭터 영역1		(lgt3에서 Sprite부분참고)
	RECT rArmature;
	
	BOOL b565 = CChildView::CharManager.GetPixelFormat() == 565 ? TRUE:FALSE;
//	RenderBodyAndArmatureLine(pSurface, ptScreen, rcV);

	if(m_wpV !=NULL)
	{
		BltLSPDrawLine(pSurface, ptScreen.x,   ptScreen.y, &rcV, m_wpV, skip_line);//FALSE);
	}

	if(m_BattleMode)
	{
		if(m_pArmature->m_pSprData[AM_KIND_ARMS] && m_pArmature->m_pV1Data[AM_KIND_ARMS]) 
		{					
			rArmature = m_pArmature->m_pSprData[AM_KIND_ARMS]->rcV;					
			BltLSPDrawLine(pSurface, ptScreen.x, ptScreen.y, &rArmature, m_pArmature->m_pV1Data[AM_KIND_ARMS]);//FALSE);
		}

		if(m_pArmature->m_pSprData[AM_KIND_SHIELD] && m_pArmature->m_pV1Data[AM_KIND_SHIELD]) 
		{					
			rArmature = m_pArmature->m_pSprData[AM_KIND_SHIELD]->rcV;					
			BltLSPDrawLine(pSurface, ptScreen.x, ptScreen.y, &rArmature, m_pArmature->m_pV1Data[AM_KIND_SHIELD]);//FALSE);
		}
	}
}

void CUser::SetChatString(CString &str)
{
	m_szChatString = str;
	m_dwBubbleTick = GetTickCount();
}

void CUser::SetIdNameString(CString &str)
{
	m_strUserId = str;
}