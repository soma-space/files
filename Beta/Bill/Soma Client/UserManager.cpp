// UserManager.cpp: implementation of the CUserManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "MainFrm.h"
#include "Blood.h"//this_1101
#include "UserManager.h"
#include "User.h"
#include "DManage/BB_Direct.h"
#include "PathFind.h"
#include "MapManager/SM_MapManager.h"
#include "DirectXMng.h"
#include "DManage/BBDX_MouseInformation.h"
#include "ChildView.h"
#include "DManage/DirectSound.h"
#include "Magic.h"
#include "MagicComet.h"
#include "ArmatureManager.h"
#include "SoundDef.h"

#include "Dialog_Main.h"
#include "Dialog_SmallMain.h"
#include "Dialog_DeadMain.h"
#include "Dialog_Exchange.h"
#include "Item_Field.h"
#include "DxCursor.h"
#include "arrow.h"

#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CDialog_Main		Dialog_Main;
extern CDialog_SmallMain Dialog_SmallMain;
extern CDialog_DeadMain	Dialog_DeadMain;
extern CDialog_Exchange	Dialog_Exchange;
extern CPoint g_vEventPos;
extern CUIRes			*g_pShowDialog;
extern CUIRes			*g_pFocusDialog;
extern CUIRes			*g_pCaptureDialog;
extern CChildView		*g_pChildView;

extern CMap<int, int, char *, char *&> g_SoundName;
extern CMap<int, int, char *, char *&> g_MusicName;
extern CArray<int, int> g_arrRequestId;

extern DWORD				g_vRenderTime;
extern CItem_Field			Item_Field;
extern int					g_vMainMode;
extern int					g_vMapNumber, g_mainSoundIndex;

extern SIZE g_vDispPixSize, g_vDispCellSize;

//BOOL CUserManager::m_bMyCovered = FALSE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserManager::CUserManager()
{
	m_Blood = new CBlood;
	m_Arrow = new CArrowManager;
	m_pPathFind = new CPathFind;
	INIT_PTR(m_pNode);
	m_bCursorInRange = FALSE;
	m_iCurFocusUidInOthers = UM_UID_INVALID;
	m_iMyUserId = UM_UID_INVALID;
	m_ReadyOK = 0;
	m_Comet = NULL;
	m_pMe = NULL;

	m_bMyCovered = FALSE;
	m_nCoverDegree = 0;
}

CUserManager::~CUserManager()
{
	SAFE_DELETE( m_Blood );
	SAFE_DELETE( m_Arrow );
	RemoveAllData();
	SAFE_DELETE(m_pPathFind);
	SAFE_DELETE(m_pNode);

	int nArrNum = m_arrSetMagicInfo.GetSize();
	_SET_MAGIC_INFO *stSetMagicInfo = NULL;

	for( int i = 0; i < nArrNum; i++ )
	{
		stSetMagicInfo = m_arrSetMagicInfo.GetAt(i); 
		if( stSetMagicInfo )
		{
			stSetMagicInfo->m_arrDestUid.RemoveAll();
			SAFE_DELETE(stSetMagicInfo->m_Comet);
			delete stSetMagicInfo;
		}
		stSetMagicInfo = NULL;
	}
	m_arrSetMagicInfo.RemoveAll(); 

	_MAGIC_TABLE* stMagicTable;
	nArrNum = m_arrMagicTableInfo.GetSize(); 
	for( i = 0; i < nArrNum; i++ )
	{
		stMagicTable = m_arrMagicTableInfo.GetAt(i); 
		SAFE_DELETE( stMagicTable );
	}
	m_arrMagicTableInfo.RemoveAll();
}

void CUserManager::RemoveAllData()
{
	POSITION pos;
	int key;
	CUser *value;
	int size, i;

	pos = m_mapUser.GetStartPosition();
	while(pos)
	{
		m_mapUser.GetNextAssoc(pos, key, value);
		delete value;
	}
	m_mapUser.RemoveAll();

	size = m_arrActionInfo.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrActionInfo[i];
	}
	m_arrActionInfo.RemoveAll();

	pos = m_lstMagic.GetHeadPosition();
	while (pos)
	{
		delete m_lstMagic.GetNext(pos);
	}
	m_lstMagic.RemoveAll();
}

void CUserManager::RemoveAllDataWithoutMe()
{
	POSITION pos;
	int key;
	CUser *value, *me = NULL;
	int size, i;

	if(m_iMyUserId != UM_UID_INVALID)
	{
		if(m_mapUser.Lookup(m_iMyUserId, me))
		{
			m_mapUser.RemoveKey(m_iMyUserId);
		}
	}

	pos = m_mapUser.GetStartPosition();
	while(pos)
	{
		m_mapUser.GetNextAssoc(pos, key, value);
		if(key != m_iMyUserId)
		{
			delete value;
		}
	}
	m_mapUser.RemoveAll();

	if(me) m_mapUser[m_iMyUserId] = me;

	size = m_arrActionInfo.GetSize();
	for(i = 0; i < size; i++)
	{
		delete m_arrActionInfo[i];
	}
	m_arrActionInfo.RemoveAll();

	pos = m_lstMagic.GetHeadPosition();
	while (pos)
	{
		delete m_lstMagic.GetNext(pos);
	}
	m_lstMagic.RemoveAll();
}

void CUserManager::RemoveUser(int index)
{
	CUser *value;

	if(m_mapUser.Lookup(index, value) == TRUE)
	{
		delete value;
		m_mapUser.RemoveKey(index);
	}
}

void CUserManager::RenderUsers(CDDSurface *pSurface, CPoint ptMapOffset)
{
	CRect rSurf;
	CPoint ptRenderingPos;

	if(m_pMe == NULL) return;

	pSurface->GetRect(rSurf);

	ApplyMagicEx();
	SetUserDead();
	// 내가 집안에 있는지 밖에 있는지를 알아서 안에 있으면 안에 있는 사람만 밖에 있으면 밖에있는 사람만.보여준다.
	CPoint p = GetMyCellPos();
	int MyIn = CChildView::MapManager.IsHouse(p);
	m_pMe->m_InHouse = MyIn;

	int validCount = 0, validDeadCount = 0;
	int My_validcount = 0;
	POSITION pos;
	int key;
	CUser *pUser;
	int i;
	CMainFrame *main_frame;
	CPoint MouseCellpos = pUser->PixelToCellUnit(pUser->m_bpDirectXMng->m_pMouseInfo->m_posMouse+ptMapOffset);

	main_frame = __GetMainWnd();
	m_iCurFocusUidInOthers = UM_UID_INVALID;

	pos = m_mapUser.GetStartPosition();
	while(pos)
	{
		m_mapUser.GetNextAssoc(pos, key, pUser);
		ptRenderingPos = pUser->GetPosition()-ptMapOffset;
		if(IsInRange(rSurf, pUser, ptRenderingPos)) // 화면에 나타날 영역인가 확인
		{
			CPoint UserCellpos = pUser->GetCellPosition();
			int UserIn = CChildView::MapManager.IsHouse(UserCellpos);

			// 나랑 같은 위치에 있는 사람만 나타나게 하기위해..집안 집 밖...
			if(MyIn == UserIn)
			{
				if(pUser->m_bDead == TRUE)
				{
					m_arrDrawDeadUser[validDeadCount++] = pUser;	// 죽은 사람 리스트
				}
				else
				{
					if(pUser->m_bDeadFlag != TRUE && pUser->m_Me != 1)
					{
//						if(UserCellpos == MouseCellpos)
						if(m_bCursorInRange == TRUE)
						{
							pUser->SetFocus(TRUE);	//커서가 캐릭의 Rect상에 있으면 Focus를 줌
							m_iCurFocusUidInOthers = pUser->GetUserId();
						}
					}
					m_arrDrawUser[validCount++] = pUser;	//그릴 사람 목록에 추가 클래스를 집어 넣는군..쩝
				}
			}
		}
		else
		{
			if(ptRenderingPos.x < -1536/*32 Cell*/ || ptRenderingPos.x > 2352/*49 Cell*/ || ptRenderingPos.y < -672/*28 Cell*/ || ptRenderingPos.y > 1272/*53 Cell*/)
			{
				m_mapUser.RemoveKey(key);
				delete pUser;
				TRACE1("%d번 유저 지움\n", key);
			}
		}
	}

	DrawDownMagics(pSurface);	//아래 깔리는 마법 그리기(날아가는 마법)

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if (validCount)
	{
		DrawUserSortY(validCount);	//Y축에 대해 Sorting함

		//Focus 상태 한명 선택 - 현재 위치로 포커스를 정하기 때문에 겹칠 수가 없다.
		BOOL bFocusFind = FALSE;
		int uid;
		for (i=validCount-1; i>=0; i--)	//Focus를 한사람만 남기고 다 제거(그중에서 나포함, 나제외에 대해 생성)
		{
			if(m_arrDrawUser[i]->GetFocus()) 
			{				
				uid = m_arrDrawUser[i]->GetUserId();
				if (uid!=m_iMyUserId)
				{
					bFocusFind = TRUE;
					m_iCurFocusUidInOthers = uid;
					i--;
					break;
				}
				else
				{
					m_arrDrawUser[i]->SetFocus(FALSE);
				}

			}
		}
		for(; i >= 0; i--)
		{
			if (m_arrDrawUser[i]->GetFocus()) m_arrDrawUser[i]->SetFocus(FALSE);
		}
		m_Blood->DrawBloodStain( pSurface,CChildView::MapManager.GetMapStartPos());  

		ActionProcess();// 액션 제어하기...
//		if( main_frame->m_wndView.DirectXMng.m_bHalEnabled )
//			main_frame->m_wndView.DrawShawdowObjEx();// 그림자를 미리 찍는다.
//		else
//			main_frame->m_wndView.DrawShawdowObj();// 그림자를 미리 찍는다. //우석. 내컴에서는 왜밑어걸로하면 죽는거지..

		for(i = 0; i < validDeadCount; i++)
		{
			pUser = m_arrDrawDeadUser[i];
			ptRenderingPos = pUser->GetPosition()-ptMapOffset;

			pUser->RenderChar(pSurface, ptRenderingPos);
			m_Blood->Draw( pSurface,CChildView::MapManager.GetMapStartPos(),pUser->GetUserId() );
			//pUser->View_BubbleChat(ptMapOffset-CPoint(0,50));//this_chat
		}

		Item_Field.Draw(CChildView::DirectXMng.m_pDrawSurface, ptMapOffset, MyIn);
		if(bFocusFind == TRUE)
		{
			CUser *me = GetMe();
			CUser *pDest = GetUserWithUid(m_iCurFocusUidInOthers);

			if(m_iCurFocusUidInOthers < 10000)
			{
				if(me->m_BattleMode == 2) CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(ATTACK_CURSOR);
				else CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
			}
			else
			{
				if(me->m_BattleMode && pDest->m_bDeadEnable == 1) CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(ATTACK_CURSOR);
				else CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
			}
		}
		else
		{
			if(Item_Field.IsExistItem(MouseCellpos, MyIn,ptMapOffset,pSurface) == TRUE) CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(PICKUP_CURSOR);
			else CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
		}

		CPoint org_pos, offset_pos, scroll_pos, x_range;
		org_pos.x = (ptMapOffset.x >> 5); // /32
		offset_pos.x = 32 - (ptMapOffset.x & 0x1f); // %32
		org_pos.y = (ptMapOffset.y >> 5); // /32
		offset_pos.y = 32 - (ptMapOffset.y & 0x1f); // %32
		int limit_y;
		int half_size_x;

		half_size_x = (int)(g_vDispCellSize.cx / 2.0 + 0.5);

		if(org_pos.x-half_size_x < 0) x_range.x = 0;
		else x_range.x = org_pos.x - half_size_x;
		if(org_pos.x+g_vDispCellSize.cx+half_size_x >= CChildView::MapObj.m_Header.nWidth) x_range.y = CChildView::MapObj.m_Header.nWidth;
		else x_range.y = org_pos.x + g_vDispCellSize.cx + half_size_x;

		BOOL bMyAbove = TRUE;
		CUser* pMe = GetMe();
		for (i=0; i<validCount; i++)	//그릴 사람의 목록만 가지고 그리기
		{
			pUser = m_arrDrawUser[i];
			ptRenderingPos = pUser->GetPosition();
			limit_y = ptRenderingPos.y;
			ptRenderingPos -= ptMapOffset;
						
			limit_y >>= 5;
			main_frame->m_wndView.DrawMyOutObj(limit_y, org_pos, offset_pos, x_range,bMyAbove);
			//main_frame->m_wndView.DrawObjEx(limit_y, org_pos, offset_pos, x_range,bMyAbove);
			pUser->RenderChar(pSurface, ptRenderingPos);
			if(pUser == pMe) bMyAbove = FALSE;

			m_Blood->Draw( pSurface,CChildView::MapManager.GetMapStartPos(),pUser->GetUserId() );
			pUser->View_BubbleChat(ptMapOffset);
		}
		// 남은 배경 스프라이트들을 다 찍는다.
		limit_y = g_vDispPixSize.cy * 3 + ptMapOffset.y;
		limit_y >>= 5;
		main_frame->m_wndView.DrawMyOutObj(limit_y, org_pos, offset_pos, x_range);
		//main_frame->m_wndView.DrawObjEx(limit_y, org_pos, offset_pos, x_range);

		// 바닥에 떨어진 아이템 이름 출력
		Item_Field.DrawLabel(CChildView::DirectXMng.m_pDrawSurface, MouseCellpos, MyIn);
	}
	m_Arrow->DrawArrow(pSurface);
	DrawUpMagics(pSurface);	//위에 덮이는 마법 그리기(날아가는 마법)//this_fly
}

void CUserManager::InHouseRenderUsers(CDDSurface *pSurface, CPoint ptMapOffset)
{
	CRect rSurf;
	CPoint ptRenderingPos;

	if(m_pMe == NULL) return;

	pSurface->GetRect(rSurf);

	ApplyMagicEx();
	SetUserDead();
	// 내가 집안에 있는지 밖에 있는지를 알아서 안에 있으면 안에 있는 사람만 밖에 있으면 밖에있는 사람만.보여준다.
	CPoint p = GetMyCellPos();
	int MyIn = CChildView::MapManager.IsHouse(p);
	m_pMe->m_InHouse = MyIn;

	int validCount = 0, validDeadCount = 0;
	int My_validcount = 0;
	POSITION pos;
	int key;
	CUser *pUser;
	int i;
	CMainFrame *main_frame;
	CPoint MouseCellpos = pUser->PixelToCellUnit(pUser->m_bpDirectXMng->m_pMouseInfo->m_posMouse+ptMapOffset);

	main_frame = __GetMainWnd();
	m_iCurFocusUidInOthers = UM_UID_INVALID;

	pos = m_mapUser.GetStartPosition();
	while(pos)
	{
		m_mapUser.GetNextAssoc(pos, key, pUser);
		ptRenderingPos = pUser->GetPosition()-ptMapOffset;
		if(IsInRange(rSurf, pUser, ptRenderingPos)) // 화면에 나타날 영역인가 확인
		{
			CPoint UserCellpos = pUser->GetCellPosition();
			int UserIn = CChildView::MapManager.IsHouse(UserCellpos);

			// 나랑 같은 위치에 있는 사람만 나타나게 하기위해..집안 집 밖...
			if(MyIn == UserIn)
			{
				if(pUser->m_bDead == TRUE)
				{
					m_arrDrawDeadUser[validDeadCount++] = pUser;	// 죽은 사람 리스트
				}
				else
				{
					if(pUser->m_bDeadFlag != TRUE && pUser->m_Me != 1)
					{
//						if(UserCellpos == MouseCellpos)
						if(m_bCursorInRange == TRUE)
						{
							pUser->SetFocus(TRUE);	//커서가 캐릭의 Rect상에 있으면 Focus를 줌
							m_iCurFocusUidInOthers = pUser->GetUserId();
						}
					}
					m_arrDrawUser[validCount++] = pUser;	//그릴 사람 목록에 추가 클래스를 집어 넣는군..쩝
				}
			}
		}
		else
		{
			if(ptRenderingPos.x < -1536 || ptRenderingPos.x > 2352 || ptRenderingPos.y < -672 || ptRenderingPos.y > 1272)
			{
				m_mapUser.RemoveKey(key);
				delete pUser;
				TRACE1("%d번 유저 지움\n", key);
			}
		}
	}

	DrawDownMagics(pSurface);	//아래 깔리는 마법 그리기(날아가는 마법)

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if (validCount)
	{
		DrawUserSortY(validCount);	//Y축에 대해 Sorting함

		//Focus 상태 한명 선택 - 현재 위치로 포커스를 정하기 때문에 겹칠 수가 없다.
		BOOL bFocusFind = FALSE;
		int uid;
		for (i=validCount-1; i>=0; i--)	//Focus를 한사람만 남기고 다 제거(그중에서 나포함, 나제외에 대해 생성)
		{
			if(m_arrDrawUser[i]->GetFocus()) 
			{				
				uid = m_arrDrawUser[i]->GetUserId();
				if (uid!=m_iMyUserId)
				{
					bFocusFind = TRUE;
					m_iCurFocusUidInOthers = uid;
					i--;
					break;
				}
				else
				{
					m_arrDrawUser[i]->SetFocus(FALSE);
				}

			}
		}
		for(; i >= 0; i--)
		{
			if (m_arrDrawUser[i]->GetFocus()) m_arrDrawUser[i]->SetFocus(FALSE);
		}

		CPoint org_pos, offset_pos, scroll_pos, x_range;
		org_pos.x = (ptMapOffset.x >> 5); // /32
		offset_pos.x = 32 - (ptMapOffset.x & 0x1f); // %32
		org_pos.y = (ptMapOffset.y >> 5); // /32
		offset_pos.y = 32 - (ptMapOffset.y & 0x1f); // %32
		int limit_y;
		int half_size_x;

		half_size_x = (int)(g_vDispCellSize.cx / 2.0 + 0.5);

		if(org_pos.x-half_size_x < 0) x_range.x = 0;
		else x_range.x = org_pos.x - half_size_x;
		if(org_pos.x+g_vDispCellSize.cx+half_size_x >= CChildView::MapObj.m_Header.nWidth) x_range.y = CChildView::MapObj.m_Header.nWidth;
		else x_range.y = org_pos.x + g_vDispCellSize.cx + half_size_x;

		CPoint ptHouseIn;
		if( main_frame->m_wndView.m_house.IsHouseIn() )
		{
			limit_y = g_vDispPixSize.cy * 3 + ptMapOffset.y;
			limit_y >>= 5;
			ptHouseIn = main_frame->m_wndView.DrawMyInObj(limit_y, org_pos, offset_pos, x_range,TRUE);
			if( ptHouseIn.x != -1 && ptHouseIn.y != -1 )
			{
				main_frame->m_wndView.DrawHouseIn(ptHouseIn.x, ptHouseIn.y, org_pos, offset_pos);
			}
		}
		m_Blood->DrawBloodStain( pSurface,CChildView::MapManager.GetMapStartPos());

		ActionProcess();// 액션 제어하기...

		for(i = 0; i < validDeadCount; i++)
		{
			pUser = m_arrDrawDeadUser[i];
			ptRenderingPos = pUser->GetPosition()-ptMapOffset;

			pUser->RenderChar(pSurface, ptRenderingPos);
			m_Blood->Draw( pSurface,CChildView::MapManager.GetMapStartPos(),pUser->GetUserId() );
			//pUser->View_BubbleChat(ptMapOffset-CPoint(0,50));//this_chat
		}

		Item_Field.Draw(CChildView::DirectXMng.m_pDrawSurface, ptMapOffset, MyIn);
		if(CChildView::bDlgIn)
		{
			CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
		}
		else
		{
			if(bFocusFind == TRUE)
			{
				CUser *me = GetMe();
				CUser *pDest = GetUserWithUid(m_iCurFocusUidInOthers);

				if(m_iCurFocusUidInOthers < 10000)
				{
					if(me->m_BattleMode == 2) CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(ATTACK_CURSOR);
					else CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
				}
				else
				{
					if(me->m_BattleMode && pDest->m_bDeadEnable == 1) CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(ATTACK_CURSOR);
					else CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
				}
			}
			else
			{
				if(Item_Field.IsExistItem(MouseCellpos, MyIn,ptMapOffset,pSurface) == TRUE) CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(PICKUP_CURSOR);
				else CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
			}
		}
		BOOL bTrans_house = FALSE;
		for (i=0; i<validCount; i++)	//그릴 사람의 목록만 가지고 그리기
		{
			pUser = m_arrDrawUser[i];
			ptRenderingPos = pUser->GetPosition();
			limit_y = ptRenderingPos.y;
			ptRenderingPos -= ptMapOffset;

			limit_y >>= 5;
			if( !main_frame->m_wndView.m_house.IsHouseIn() )
			{
				ptHouseIn = main_frame->m_wndView.DrawMyInObj(limit_y, org_pos, offset_pos, x_range,TRUE);
				if( ptHouseIn.x != -1 && ptHouseIn.y != -1 )
				{
					main_frame->m_wndView.DrawHouseIn(ptHouseIn.x, ptHouseIn.y, org_pos, offset_pos);
//					int trans_degree = main_frame->m_wndView.m_house.TransDegree();
					int trans_degree = main_frame->m_wndView.m_house.GetTransDegree();
					if( trans_degree != 0 && trans_degree != 32 )
					{
						main_frame->m_wndView.DrawTransOutHouse(ptHouseIn.x, ptHouseIn.y, org_pos, offset_pos,trans_degree,32-trans_degree);
					}
				}
			}

			pUser->RenderChar(pSurface, ptRenderingPos);
			m_Blood->Draw( pSurface,CChildView::MapManager.GetMapStartPos(),pUser->GetUserId() );
			pUser->View_BubbleChat(ptMapOffset);
		}

		if( ptHouseIn.x != -1 && ptHouseIn.y != -1 && main_frame->m_wndView.m_house.IsHouseIn() )
		{
//			int trans_degree = main_frame->m_wndView.m_house.TransDegree();
			int trans_degree = main_frame->m_wndView.m_house.GetTransDegree();
			if( trans_degree != 0 && trans_degree != 32 )
			{
				main_frame->m_wndView.DrawTransOutHouse(ptHouseIn.x, ptHouseIn.y, org_pos, offset_pos,trans_degree,32-trans_degree);
			}
		}

		// 남은 배경 스프라이트들을 다 찍는다.
		limit_y = g_vDispPixSize.cy * 3 + ptMapOffset.y;
		limit_y >>= 5;
		main_frame->m_wndView.DrawMyInObj(limit_y, org_pos, offset_pos, x_range);
		// 바닥에 떨어진 아이템 출력
		Item_Field.DrawLabel(CChildView::DirectXMng.m_pDrawSurface, MouseCellpos, MyIn);
	}


	DrawUpMagics(pSurface);	//위에 덮이는 마법 그리기(날아가는 마법)//this_fly
}

void CUserManager::DrawUserSortY(int count)
{
	qsort(m_arrDrawUser, count, sizeof(CUser*), compare);
}

int CUserManager::compare(const void *arg1, const void *arg2)
{
	CUser* psd1 = *((CUser**)arg1);
	CUser* psd2 = *((CUser**)arg2);

	CPoint posP1 = psd1->GetPosition();
	CPoint posP2 = psd2->GetPosition();
	if(posP1.y < posP2.y) return -1;
	else if(posP1.y > posP2.y) return 1;
	else return 0;
}

void CUserManager::Move()	//유저들 이동시키기
{
	POSITION pos;
	int key;
	CUser *value;

	pos = m_mapUser.GetStartPosition();

	while(pos)
	{
		m_mapUser.GetNextAssoc(pos, key, value);

		value->Move();
	}
}

CPoint CUserManager::GetMyPos()
{
	if(m_pMe) return m_pMe->GetPosition();
	else return CPoint(10,7);
}

BOOL CUserManager::IsInRange(CRect rSurface, CUser *pUser, CPoint ptPos) //화면에 보여?
{
	CRect usrRect = pUser->GetCharRect();
	usrRect.OffsetRect(ptPos);
	usrRect.NormalizeRect();

	BOOL bInRange = rSurface.IntersectRect(&rSurface, &usrRect);
	m_bCursorInRange = rSurface.PtInRect(pUser->m_bpDirectXMng->m_pMouseInfo->m_posMouse); //커서밑에 있어?
	return bInRange;
}

//PathFinder에 설정함
void CUserManager::SetMoveableTable(SM_MapManager* pMapManager)
{
	PBYTE*	byppTable;
	int		dimX;
	int		dimY;
	byppTable = pMapManager->GetMoveableTable(dimX, dimY);
	m_pPathFind->SetAllocedMap(dimX, dimY, byppTable);

//	byppTable = pMapManager->GetHouseTable(dimX, dimY);
//	m_pPathFind->SetAllocedHouseMap(dimX, dimY, byppTable);

	// 임시 나중에 이벤트 정보두..이곳에서..
}
#include "Item_inv.h"
extern CItem_Inv           *Item_Inv;
void CUserManager::ModifyUser(int uid,  BYTE bDeadEnable, CString t_name, CString t_guild, int picnum, short moral, BYTE live, BYTE battle_mode, int x, int y, short *item, BOOL bIsI, short hair, short skin)
{
//	TRACE("17\n");//del
	BOOL bModify = FALSE;
	CPoint pos;
	CUser *pUser;

	if(m_mapUser.Lookup(uid, pUser) == TRUE) bModify = TRUE;
	else pUser = new CUser;

//	pUser->SetCellPosition(CPoint (x, y));
	pUser->m_strUserId = t_name;
	pUser->m_strUserId.MakeUpper();

	pUser->m_Guild = t_guild;
	pUser->m_Guild.MakeUpper();

	pUser->m_Skin = skin;
	pUser->m_Hair = hair;
	pUser->m_bDeadEnable = bDeadEnable;
	//임시 //서버에서 받아 와야 하는군...쩝..
	//몬스터는 몸만 존재 시키면 되겠당...캬캬캬..
	//((서버에서 받아올부분....
	if(bIsI)// 본인이면..
	{
		if(!pUser->m_pRoute) pUser->m_pRoute = new CList<CPoint, CPoint>;
		pUser->SetCellPosition(CPoint (x, y));
//		pUser->SetCharRes(0);
		pUser->m_nType = 0;//임시
		if( Item_Inv->m_Gender != 0 ) 
			pUser->SetCharRes(skin + 3);//여자
		else
			pUser->SetCharRes(skin);//남자

		pUser->m_Gender = !(Item_Inv->m_Gender);
//		pUser->m_Gender = 1;	// 0 : 여자 1 :  남자
		pUser->m_Me = 1;
		m_pMe = pUser;

		if(item[0] <= 0) item[0] = 0; //디폴트 머리.. 나중에 캐릭터 생성부분과 일치시켜줘야한다..우석..10월 17일..
		pUser->SetWearItem(AM_KIND_HELMET, item[0]);

		if(item[3] <= 0) item[3] = 0;
		pUser->SetWearItem(AM_KIND_ARMOR, item[3]);
		if(item[4] <= 0) item[4] = 0;
		pUser->SetWearItem(AM_KIND_JEAN, item[4]);
		if(item[5] <= 0) item[5] = 0;
		pUser->SetWearItem(AM_KIND_BOOT, item[5]);
		if(item[6] <= 0) item[6] = 0;
		pUser->SetWearItem(AM_KIND_ARMS, item[6]);//10월 16일 우석...
		if(item[7] <= 0) item[7] = 0;
		pUser->SetWearItem(AM_KIND_SHIELD, item[7]);

		pUser->SetWearItem(AM_KIND_SH, 1);
		TRACE(" 나 옷 다입음...!.\n");

		if(bModify == FALSE)
		{
			CPoint pos;

			pos.x = x * 48 - 400;
			pos.y = y * 24 - 300;
//			g_pChildView->BackLoading(pos);
//			g_vRenderTime = GetTickCount();
//			g_pChildView->BackLoadingEx(pos);//this_xxx

			BYTE buf[10];
			int offset = 0;
			CPacketParser::SetByte(buf, offset, PKT_GAMESTART);
			CChildView::SocketMng.Send(offset, buf);
		}
	}
	else
	{
		//	우석.. 10월 11일..
		if(!pUser->m_pRoute || pUser->IsCharDie())
		{
			pUser->SetCellPosition(CPoint(x, y));
			if(pUser->m_pRoute) pUser->m_pRoute->RemoveAll();
			SAFE_DELETE(pUser->m_pRoute);
			pUser->m_pRoute = new CList<CPoint, CPoint>;
			m_pPathFind->SetMovable(CPoint(x, y),false);
		}
		else 
		{
			TRACE("modify를 받았다... [%d, %d]\n", x,y);			
		}
//-------------------

		if(uid < 10000)	//유저 캐릭터일때..
		{//서버~``여자 0: 남자 1: 클라이언트 여자:1 남자 0: 
			// 아이템 저장...이 필요하군..이곳에...
			pUser->m_nType = 0;// 임시
			if( picnum )//npc가 아닌경우 성별이 들어옴
			{
				pUser->SetCharRes(skin);// 남자를 먼저 만들어서..남자가..0
				pUser->m_Gender = 1;// 이건 반대..//남자
			}
			else
			{
				pUser->SetCharRes(skin+3);// 남자를 먼저 만들어서..남자가..0
				pUser->m_Gender = 0;// 이건 반대..//여자
			}

//			pUser->SetCharRes(0);// 남자를 먼저 만들어서..남자가..0
//			pUser->m_Gender = 1;// 이건 반대..
			TRACE("새로운 유저를 읽어 들인다.\n");

			if(item[0] <= 0) item[0] = 0;
			pUser->SetWearItem(AM_KIND_HELMET, item[0]);
			if(item[3] <= 0) item[3] = 0;
			pUser->SetWearItem(AM_KIND_ARMOR, item[3]);
			if(item[4] <= 0) item[4] = 0;
			pUser->SetWearItem(AM_KIND_JEAN, item[4]);
			if(item[5] <= 0) item[5] = 0;
			pUser->SetWearItem(AM_KIND_BOOT, item[5]);
			if(item[6] <= 0) item[6] = 0;
			pUser->SetWearItem(AM_KIND_ARMS, item[6]);
			if(item[7] <= 0) item[7] = 0;
			pUser->SetWearItem(AM_KIND_SHIELD, item[7]);

			pUser->SetWearItem(AM_KIND_SH, 1);
		}
		else
		{
//			if(picnum >= 17)  picnum  = 16;// 임시..
//			if(picnum == 11 || picnum == 12 || picnum == 13 || picnum == 14 ) picnum = 16;

//			picnum+=9;// npc나 몹은 10번부터 시작...
//			if(picnum == 19 || picnum == 22 || picnum == 31 || picnum == 32  || picnum == 33 || picnum == 34) picnum = 30; //없는그림은 임시로 30번그림사용..

			//picnum = 15;// 호랭이..				
			int chartertype = picnum;
			pUser->m_nType = chartertype;
			pUser->SetCharRes(chartertype);//11  늑대
			pUser->m_Gender = 1;// 이건 반대..
			TRACE("Modify %s(%d)-%d,%d\n", pUser->m_strUserId, uid, x, y);
			pUser->m_BloodType = skin;//mon일경우 스킨에 피타입이 들어가 있다.

/*			pUser->SetWearItem(AM_KIND_HELMET, 0);
			pUser->SetWearItem(AM_KIND_ARMOR, 0);
			pUser->SetWearItem(AM_KIND_JEAN, 0);
			pUser->SetWearItem(AM_KIND_BOOT, 0);
			pUser->SetWearItem(AM_KIND_ARMS, 0);
			pUser->SetWearItem(AM_KIND_SHIELD, 0);
*/
			pUser->SetWearItem(AM_KIND_SH, chartertype);
		}

/*
		pUser->SetWearItem(AM_KIND_ARMOR, 1);
		pUser->SetWearItem(AM_KIND_HELMET, 1);
		pUser->SetWearItem(AM_KIND_BOOT, 1);
		pUser->SetWearItem(AM_KIND_JEAN, 1);
		pUser->SetWearItem(AM_KIND_SHIELD, 1);
		pUser->SetWearItem(AM_KIND_ARMS, 1);
*/
	}
	
	/*
	pUser->SetWearItem(AM_KIND_ARMOR, 1);
	pUser->SetWearItem(AM_KIND_HELMET, 1);
	pUser->SetWearItem(AM_KIND_BOOT, 1);
	pUser->SetWearItem(AM_KIND_JEAN, 1);
	pUser->SetWearItem(AM_KIND_SHIELD, 1);
	pUser->SetWearItem(AM_KIND_ARMS, 1);
	*/
	
	pUser->SetUserId(uid);
	pUser->m_bDeadFlag = (live == 1) ? FALSE : TRUE;
	pUser->m_bDying = pUser->m_bDeadFlag;
	pUser->m_BattleMode = (int)battle_mode;
	pUser->SetDeposition(moral);
	if(pUser->m_bDeadFlag == TRUE)
	{
		pUser->m_bDead = TRUE;
		pUser->SetMotion(CUser::UM_Die);
	}
	else
	{
		pUser->m_bDead = FALSE;
		pUser->SetMotion(CUser::UM_Stop);
		if(bIsI && bModify == TRUE)
		{
			if(g_vMainMode == 1) g_pChildView->SetMainMode(1);
			else Dialog_Main.ShowWindow();
			Dialog_DeadMain.HideWindow();
		}
	}
	pUser->m_vMoveStep = 0;
	if(bIsI) TRACE0("MoveStep = 0\n");
	if(bModify == FALSE)
	{
		m_mapUser[uid] = pUser;
		
		int size = g_arrRequestId.GetSize();
		for(int i = 0; i < size; i++)
		{
			if(g_arrRequestId[i] == uid)
			{
				g_arrRequestId.RemoveAt(i);
				break;
			}
		}
	}
	if(bIsI)
	{
		m_ReadyOK = 1;// npc 무브패킷을 받기 시작함. 임시 존이동할때 다시 이용해야함.지금은 안하지..
	}
	
	// 임시 이부분은 뭐지? 위에는 뭐구..음..내가해놓구두..겹친거 같군...쩝..
//	for(i=0; i < 10;i++)
//		ChangItem(uid, i, item[i]);
}

void CUserManager::DeleteUser(int uid, int x, int y)
{
	CUser *pUser;
	DeleteMagic(uid);

	if(m_mapUser.Lookup(uid, pUser))
	{
		delete pUser;
		m_mapUser.RemoveKey(uid);
		m_pPathFind->SetMovable(CPoint(x,y),true);
	}
}

BOOL CUserManager::IsMoveable(CPoint pt)	//Cell단위
{
	return m_pPathFind->IsBlankMap(pt.x, pt.y);
}

CUser* CUserManager::GetUserWithPos(CPoint ptPixelPos, CPoint ptMapOffset)
{
	POSITION pos;
	int key;
	CUser *pUser;
	CRect r;

	ptPixelPos.Offset(-ptMapOffset.x, -ptMapOffset.y);
	pos = m_mapUser.GetStartPosition();

	while(pos)
	{
		m_mapUser.GetNextAssoc(pos, key, pUser);
		r = pUser->GetCharRect();
		if(r.PtInRect(ptPixelPos)) return pUser;
	}
	return NULL;
}

CUser* CUserManager::GetUserWithCellPos(CPoint ptCellPos)
{
	POSITION pos;
	int key;
	CUser *pUser;

	pos = m_mapUser.GetStartPosition();

	while(pos)
	{
		m_mapUser.GetNextAssoc(pos, key, pUser);

		if(pUser->GetCellPosition() == ptCellPos) return pUser;
	}
	return NULL;
}

CUser* CUserManager::GetUserWithUid(int uid)
{
	CUser *pUser;

	if(m_mapUser.Lookup(uid, pUser)) return pUser;
	else return NULL;
}

void CUserManager::SetMoveFirst(CUser *pUser, CPoint ptDst, CPoint ptFirst)
{
	pUser->SetMoveFirst(ptDst, ptFirst);
}

void CUserManager::SetMoveMid(CUser *pUser, CPoint ptDst)
{
	pUser->SetMoveMiddle(ptDst);
}

void CUserManager::SetMoveEnd(CUser *pUser, CPoint ptDst)
{
	pUser->SetMoveEnd(ptDst);	
}

void CUserManager::SetRunMoveFirst(CUser *pUser, CPoint ptDst, CPoint ptFirst, CPoint ptNext)
{
	pUser->SetRunMoveFirst(ptDst, ptFirst, ptNext);
}

void CUserManager::SetRunMoveMid(CUser *pUser, CPoint ptDst, CPoint ptNext)
{
	pUser->SetRunMoveMiddle(ptDst, ptNext);
}

void CUserManager::SetRunMoveEnd(CUser *pUser, CPoint ptDst, CPoint ptNext)
{
	pUser->SetRunMoveEnd(ptDst, ptNext);
}


//공격이 성공했을 경우
void CUserManager::AttackChar(BYTE bSuccess, int iSorUid, CPoint ptSorPos, int iDestUid, CPoint ptDestPos)
{
	CUser *pSorUser = GetUserWithUid(iSorUid);
	CUser *pDestUser = GetUserWithUid(iDestUid);

	if(pSorUser == NULL) return;
	
	pSorUser->m_PkRecive_Flag = 0;// 패킷을 받았다.
	int attackDir = 0;
	int beatedDir = 0;

	if(!pSorUser->m_Me && !pSorUser->m_pRoute->IsEmpty())//pSorUser->m_Position != ptSorPos)
	{
		CPoint tmp;
		while(!pSorUser->m_pRoute->IsEmpty())
		{
			tmp = pSorUser->m_pRoute->RemoveHead();
			m_pPathFind->SetMovable(tmp,true);
		}		
		pSorUser->m_ptDestFinalPos = pSorUser->m_Position = pSorUser->m_ptDstPos = pSorUser->CellUnitToPixel(tmp);
		pSorUser->m_ptPostCellPos = tmp;
		m_pPathFind->SetMovable(tmp,false);
		TRACE("공격위치가 맞지않다..\n");
	}

	if (ptSorPos.x<ptDestPos.x) attackDir |= DIR_RIGHT;
	else if (ptSorPos.x>ptDestPos.x) attackDir |= DIR_LEFT;

	if (ptSorPos.y<ptDestPos.y) attackDir |= DIR_DOWN;
	else if (ptSorPos.y>ptDestPos.y) attackDir |= DIR_UP;

	if (attackDir==0) attackDir = -1;

	BOOL bOk = FALSE;
	if (pSorUser && pSorUser->IsArrivedInCell() && pSorUser->GetCellPosition() == ptSorPos) bOk = TRUE;

/*	switch(attackDir)
	{
	case DIR_LEFT:
		beatedDir = DIR_RIGHT;
		break;
	case DIR_RIGHT:
		beatedDir = DIR_LEFT;
		break;
	case DIR_UP:
		beatedDir = DIR_DOWN;
		break;
	case DIR_DOWN:
		beatedDir = DIR_UP;
		break;
	case DIR_LEFT|DIR_UP:
		beatedDir = DIR_RIGHT|DIR_DOWN;
		break;
	case DIR_LEFT|DIR_DOWN:
		beatedDir = DIR_RIGHT|DIR_UP;
		break;
	case DIR_RIGHT|DIR_UP:
		beatedDir = DIR_LEFT|DIR_DOWN;
		break;
	case DIR_RIGHT|DIR_DOWN:
		beatedDir = DIR_LEFT|DIR_UP;
		break;
	default:
		beatedDir = -1;
		break;
	}*/
	
	// 형식이 바뀌어서 필요 없을꺼 같은데...	
	if(pDestUser == NULL) return;
	if( pDestUser->m_bDeadFlag ) pDestUser->m_nFocusColor = 3;


//	if (bOk && pDestUser && pDestUser->IsArrivedInCell() && pDestUser->GetCellPosition() == ptDestPos)
//	{
	//pStartUser->SetDirection(nDir);

	if(bSuccess == 0x01)
	{
		if( pSorUser->m_Arm_type != CROSSBOW && pSorUser->m_Arm_type != BOW )
		{
			//if( pDestUser->m_BloodType != 0 )//0은 죽을때 피가 없다.
			CRect rSurf(0,0,800,600);
			CPoint ptRenderingPos = pDestUser->GetPosition()-CChildView::MapManager.GetMapStartPos();
			if(IsInRange(rSurf, pDestUser, ptRenderingPos)) // 화면에 나타날 영역인가 확인
				m_Blood->Add( iDestUid,(int)pDestUser->m_BloodType,pDestUser->GetPosition(), TRUE );
		}
	}

	if( pSorUser->m_Arm_type == CROSSBOW || pSorUser->m_Arm_type == BOW )
	{
		CPoint pt_sor = pSorUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
		CPoint pt_dst = pDestUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
		CRect usrRect = pDestUser->GetCharRect();
		pt_dst.y -= usrRect.Height() / 2;
		pt_sor.y -= 50;
		m_Arrow->Add( iDestUid, pt_sor, pt_dst, 1, 0, bSuccess == 0x01 );
	}

	if(pSorUser && attackDir != -1)
		pSorUser->SetDirection(attackDir);
//	if(pDestUser && beatedDir != 1)
//		pDestUser->SetDirection(attackDir);

	// 방향만 먼저 바꿔 놓고 나중에 맞는 동작을 취하게 한다. 임시
/*	if(pDestUser)
		if (-1 != beatedDir && !pDestUser->IsCharAttack() && !pDestUser->IsCharDie()) // 공격도중에는 맞는도ㅗㅇ작을 취하지 않으므로.
		{
			pDestUser->SetDirection(beatedDir);
//			ActionMotion(iDestUid, 1, 350);	// 350 => 맞는 동작을 뒤에 취한다.
		}*/

	if( bSuccess == 0x01 )
	{
		ActionMotion(iSorUid, 5, 0);// 공격
		if( pSorUser->m_Arm_type != CROSSBOW && pSorUser->m_Arm_type != BOW )
			ActionMotion(iDestUid, 1, 400);	// 맞는 동작을 뒤에 취한다.
	}
	else
	{
		ActionMotion(iSorUid, 5, 0);// 공격
	}
	
//발동매직인 경우 적에게 공격을 받으면 매직공격을 소멸 시켜 준다... 
	if(bSuccess == 1)
		DeleteMagic(iDestUid);
}

void CUserManager::DeleteMagic(int iDestUid)
{
	_SET_MAGIC_INFO* stMagicInfo;
	CUser* pUser = NULL;
	int		suid;
	BOOL	bReady;
	int		s_time;
	int		pre_index;
	int		post_index;
	int		nArrSize = m_arrSetMagicInfo.GetSize();
	
	for( int i = 0 ; i < nArrSize; i++ )
	{
		stMagicInfo = m_arrSetMagicInfo.GetAt(i);
		if( stMagicInfo != NULL )
		{
			suid	= stMagicInfo->m_nSorUid;
			bReady	= stMagicInfo->m_bReady;
			s_time	= stMagicInfo->m_stMagicTable.m_nStTime;

			if( suid == iDestUid && bReady != TRUE && s_time )
			{
				pUser = GetUserWithUid(iDestUid);
				if( pUser )
				{
					post_index = stMagicInfo->m_stMagicTable.m_nStPostMagic;
					pre_index  = stMagicInfo->m_stMagicTable.m_nStPreMagic;

					int size = pUser->m_pMagic->m_arrMagicInfo.GetSize();
					int magic_index;

					for( int ii = 0; ii < size ; ii++ )
					{
						magic_index = pUser->m_pMagic->m_arrMagicInfo.GetAt(ii)->m_nMagicIndex;
						if( post_index == magic_index || pre_index == magic_index )
						{
							pUser->m_pMagic->m_arrMagicInfo.GetAt(ii)->m_bShowContinue = FALSE;
							pUser->m_pMagic->m_arrMagicInfo.GetAt(ii)->m_nCurFrame = 0;
						}
					}
				}
				SAFE_DELETE( stMagicInfo->m_Comet );
				SAFE_DELETE( stMagicInfo );
				m_arrSetMagicInfo.RemoveAt(i);
				nArrSize--; i--;
			}
		}//if( stMagicInfo != NULL )
	}
}

void CUserManager::Attack3DMagic(int suid, int euid, TCHAR* strName)
{
	// uid로 부터 위치를 가져와야함..음...
	CUser *pDestUser = GetUserWithUid(euid);
	CUser *pStartUser = GetUserWithUid(suid);
	CPoint ptSPos;
	CPoint ptEPos;

	if (pStartUser && pDestUser) 
	{
		pStartUser->SetMotion(CUser::UM_Attack);
		ptSPos = pStartUser->GetPosition();
		ptEPos = pDestUser->GetPosition();

		CPoint tmp = CChildView::MapManager.GetMapStartPos();

		CPoint UserPos;
		CPoint tmp_Pos;// 보정할 값이 들어갈곳..

		UserPos = ptSPos - tmp;
		ptEPos -= tmp;
		tmp_Pos = ptEPos - UserPos;
		ptEPos = ptSPos + tmp_Pos;

		int nDir = SetAimed3DMagic(ptSPos, ptEPos, strName, MAGIC_TYPE_UP, euid);
		pStartUser->SetDirection(nDir);
	}

}
void CUserManager::Attack3DMagicEx(int suid, int euid, int index, int nSeries)
{
	// uid로 부터 위치를 가져와야함..음...
	CUser *pDestUser = GetUserWithUid(euid);
	CUser *pStartUser = GetUserWithUid(suid);
	CPoint ptSPos;
	CPoint ptEPos;

	if (pStartUser && pDestUser) 
	{
		pStartUser->SetMotion(CUser::UM_Attack);
		ptSPos = pStartUser->GetPosition();
		ptEPos = pDestUser->GetPosition();

		CPoint tmp = CChildView::MapManager.GetMapStartPos();

		CPoint UserPos;
		CPoint tmp_Pos;// 보정할 값이 들어갈곳..

		UserPos = ptSPos - tmp;
		ptEPos -= tmp;
		tmp_Pos = ptEPos - UserPos;
		ptEPos = ptSPos + tmp_Pos;

		int nDir = SetAimed3DMagicEx(ptSPos, ptEPos, index, MAGIC_TYPE_UP, euid, nSeries);
		pStartUser->SetDirection(nDir);
	}

}

void CUserManager::AttackDir3DMagicEx(int suid, int nDir, int index, int nDestUid)
{
	CUser *pStartUser = GetUserWithUid(suid);
	CPoint ptSPos;
	if (pStartUser) 
	{
		pStartUser->SetMotion(CUser::UM_Attack);
		ptSPos = pStartUser->GetPosition();
		SetDirAimed3DMagicEx(ptSPos, index, MAGIC_TYPE_UP, nDir, nDestUid);
		pStartUser->SetDirection(nDir);
	}
}

int CUserManager::SetAimed3DMagic(CPoint ptSor, CPoint ptDst, TCHAR* strName, int nMagicType, int euid)
{
	CMagic *pMagic = new CMagic;
	int nDir = 0;
	if (pMagic->SetFly3DMagicRes(strName, ptSor, ptDst, nDir, nMagicType))
	{
		pMagic->SetMagicType(nMagicType);
		pMagic->SetMagicAttackDesUid( euid );
		m_lstMagic.AddTail(pMagic);
	}
	else delete pMagic;
	return nDir;
}

int CUserManager::SetAimed3DMagicEx(CPoint ptSor, CPoint ptDst, int index, int nMagicType, int euid, int nSeries)
{
	CMagic *pMagic = new CMagic;
	int nDir = 0;
	if (pMagic->SetFly3DMagicResEx(index, ptSor, ptDst, nDir, nMagicType, nSeries))
	{
		pMagic->SetMagicType(nMagicType);
		pMagic->SetMagicAttackDesUid( euid );
		m_lstMagic.AddTail(pMagic);
	}
	else delete pMagic;
	return nDir;
}

int CUserManager::SetDirAimed3DMagicEx(CPoint ptSor, int index, int nMagicType, int nDir, int nDestUid)
{
	CMagic *pMagic = new CMagic;
	if (pMagic->SetDirFly3DMagicResEx(index, ptSor, nDir, nMagicType))
	{
		pMagic->SetMagicType(nMagicType);
		pMagic->SetMagicAttackDesUid( nDestUid );
		m_lstMagic.AddTail(pMagic);
	}
	else delete pMagic;
	return nDir;
}

void CUserManager::MagicTableInit()
{
	CFile file;
	_MAGIC_TABLE* stMagicTable;
	
	UINT	uiReadCount;
	CString szTmpFileName;
	const CString szExePath = CDirectDraw::GetExecutionPath();
	szTmpFileName.Format("%sMagicRes\\magic.tbl", (LPCTSTR)szExePath);

	if (NULL==file.Open(szTmpFileName, CFile::modeRead)) 
	{
		TRACE("마법테이블을 읽지 못했습니다....\n");
		return;
	}
	UINT nFileLength = file.GetLength();
	int nTableSize = nFileLength / sizeof( _MAGIC_TABLE );
	TRACE("읽은 마법 테이블 총 항목: %d\n",nTableSize);
	for( int i = 0; i < nTableSize ; i++ )
	{
		stMagicTable = new _MAGIC_TABLE;
		ASSERT( NULL != stMagicTable );
		uiReadCount = file.Read((PVOID) stMagicTable, sizeof(_MAGIC_TABLE));
		ASSERT( uiReadCount == sizeof(_MAGIC_TABLE) );
		m_arrMagicTableInfo.Add( stMagicTable );
	}
	file.Close();

}
/*
PVOID CUserManager::CreateTypeMagic(int type)
{
	PVOID MAGICPOINTER;
	switch(type)
	{
	case 4:
		CMagicComet* pComet = new CMagicComet( ptSPos, ptEPos );
		if( pComet )
		{
			MAGICPOINTER = (PVOID)pComet; 
			return MAGICPOINTER
		}
		else return NULL;

		break;
	default:
		return NULL;
		break;
	}
}
*/

//발동시간을 받아야 할듯
void CUserManager::ReadMagicTable(int bSuccess, int iSorUid, int nDestUid, int magicindex, DWORD dwStartTime, int Dir)
{
	TRACE("1 SUID:%d DUID:%d MagicIndex:%d \n",iSorUid,nDestUid,magicindex);
	if( bSuccess == 2 ) return;
	if( nDestUid < -1 ) return;
	
	CUser *pSorUser = GetUserWithUid(iSorUid);
	if( !pSorUser ) return;
	
	_SET_MAGIC_INFO *stSetMagicInfo = NULL;
	int nArrSize = m_arrSetMagicInfo.GetSize();
	
	_MAGIC_TABLE stMagicTable;
	
	stSetMagicInfo = new _SET_MAGIC_INFO;
	
	if( stSetMagicInfo == NULL ) return;
	ASSERT( NULL != stSetMagicInfo );
	DWORD dwNowTime = GetTickCount();
	stMagicTable = *(m_arrMagicTableInfo.GetAt(magicindex -1)); 
	TRACE("AType:%d TableNum:%d \n",stMagicTable.m_nAttackType,stMagicTable.m_nTableNum);  
	stSetMagicInfo->m_stMagicTable	= stMagicTable;
	stSetMagicInfo->m_nSorUid		= iSorUid	  ;
	stSetMagicInfo->m_dwNowTime		= dwNowTime	  ;
	stSetMagicInfo->m_nDir			= Dir		  ;
	stSetMagicInfo->m_Comet			= NULL		  ;
	stSetMagicInfo->m_bReady		= FALSE		  ;
	stSetMagicInfo->m_stMagicTable.m_nStTime = dwStartTime;

	stSetMagicInfo->m_arrDestUid.Add( nDestUid );
	TRACE(" PDestUid: %d\n",nDestUid);

	MagicAttackKind( stSetMagicInfo, iSorUid, &nDestUid, 1 );

	int nSIndex = -1;
	int key;
	char *value;

	if( pSorUser )
	{
		CPoint ptSoundPos = pSorUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
		key = SOUND_MAGIC+(stSetMagicInfo->m_stMagicTable.m_nTableNum-1)*3;
		if(g_SoundName.Lookup(key, value))
		{
			nSIndex = CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
		}
	}

	stSetMagicInfo->m_nSIndex = nSIndex;
	TRACE("%d\n",nSIndex);
	m_arrSetMagicInfo.Add( stSetMagicInfo );
}

void CUserManager::PoisonAffect(int nDesUid,short HP)
{
	CUser* pDest;
	pDest = GetUserWithUid( nDesUid );
	if( pDest == NULL ) 
	{
		TRACE("대상 캐릭터가 없다\n");
		return;
	}
//	if(*pHP <= 0) pDest->m_bDeadFlag = TRUE;
	pDest->m_Cur_Hp = HP;
	pDest->SetMotion(CUser::UM_Beated);
	TRACE("POISONAFFECT\n");
}

#define POISON_BIT		0x0001
#define CURSE_BIT		0x0002
#define HIDDEN_BIT		0x0004
#define PROTECTION_BIT	0x0008

void CUserManager::CharStateChange( int nDestUid, DWORD dwStatus )
{
	CUser* pDest;
	pDest = GetUserWithUid( nDestUid );
	int t_index;
	int attack_type;
	APPLIED_CON_MAGIC	ACM;
	_MAGIC_TABLE stMagicTable;

	if( pDest )
	{
		if( dwStatus & POISON_BIT )
		{
			pDest->m_nPoisonKind = 3;
			pDest->SetMotion(CUser::UM_Beated);
		}
		else
		{
			int size = pDest->m_arrACM.GetSize();
			for( int i = 0; i < size ; i++ )
			{
				ACM = pDest->m_arrACM.GetAt(i); 
				t_index = ACM.m_nTableNum;
				stMagicTable = *(m_arrMagicTableInfo.GetAt(t_index -1));
				attack_type = stMagicTable.m_nAttackType;

				if( attack_type == 3 )
				{
					pDest->m_arrACM.RemoveAt(i);
					i--; size--;
					continue;
				}
			}

			pDest->m_bPoisoning = FALSE;
			pDest->m_nPoisonKind = 0; 
		}
	}
}

void CUserManager::ReadMagicTable(int bSuccess, int iSorUid, short Mp, DWORD exp, int *iDestUid,short *pHP,int nDstUidNum, int magicindex, DWORD dwConTime, int Dir)
{
	TRACE("2 SUID:%d DUID:%d MagicIndex:%d \n",iSorUid,*iDestUid,magicindex);
	DWORD dwNowTime = GetTickCount();
	CUser* pDest = NULL;
	CUser* pSor  = NULL;

	pSor = GetUserWithUid( iSorUid );
	if( pSor )
	{
		pSor->m_Cur_Mp = Mp;
		pSor->m_Cur_exp = exp;
	}
	_SET_MAGIC_INFO *stSetMagicInfo = NULL;
	int nArrSize = m_arrSetMagicInfo.GetSize();
	
	BOOL	bMagicSet = FALSE;
	int*	pDestUid = iDestUid;
	int		table_num;
	int		sor_uid;
	BOOL	b_ready;
	int		con_time;
	int		attack_type;
	int		effect_kind;
	for( int i = 0; i < nArrSize; i++ )
	{
		stSetMagicInfo = m_arrSetMagicInfo.GetAt(i);
		b_ready		= stSetMagicInfo->m_bReady;
		sor_uid		= stSetMagicInfo->m_nSorUid;
		con_time	= stSetMagicInfo->m_stMagicTable.m_nConTime;
		table_num	= stSetMagicInfo->m_stMagicTable.m_nTableNum;
		attack_type = stSetMagicInfo->m_stMagicTable.m_nAttackType;
		effect_kind = stSetMagicInfo->m_stMagicTable.m_nEffecKind;

		if( table_num == magicindex && sor_uid == iSorUid && b_ready == TRUE && con_time > 0  )
		{
			if( attack_type != 3 ) 
				stSetMagicInfo->m_dwNowTime = dwNowTime + con_time; 
		}
		else if( table_num == magicindex && sor_uid == iSorUid && b_ready == FALSE )
		{
			if( stSetMagicInfo->m_arrDestUid.GetSize() )
				stSetMagicInfo->m_arrDestUid.RemoveAll();

			if( bSuccess == 2 || pSor == NULL || nDstUidNum == 0)
			{
				SAFE_DELETE( stSetMagicInfo->m_Comet );
				SAFE_DELETE( stSetMagicInfo );
				m_arrSetMagicInfo.RemoveAt(i);
				return;
			}
			stSetMagicInfo->m_dwNowTime		= dwNowTime	  ;
			stSetMagicInfo->m_nDir			= Dir		  ;
			stSetMagicInfo->m_bReady		= TRUE		  ;
			stSetMagicInfo->m_stMagicTable.m_nStTime =	 0; 
			stSetMagicInfo->m_stMagicTable.m_nConTime = dwConTime;
			bMagicSet = TRUE;
			TRACE("TABLENUM: %d ID: %d 설정됨\n",magicindex,iSorUid);
			
			for ( int ii = 0; ii < nDstUidNum; ii++ )
			{
				pDest = GetUserWithUid( *pDestUid );
				if( pDest == NULL )
				{ pDestUid++; pHP++; continue; }
				
				pDest->m_Cur_Hp = *pHP;
				if( dwConTime )
				{
					BOOL bAdd = FALSE;
					APPLIED_CON_MAGIC ACM;
					int arr_size = pDest->m_arrACM.GetSize();
					for( int jj = 0 ; jj < arr_size ; jj++ )
					{
						ACM = pDest->m_arrACM.GetAt(jj);
						if( ACM.m_nTableNum == magicindex )
						{
							pDest->m_arrACM.RemoveAt(jj);
							ACM.m_nTableNum = magicindex;
							ACM.m_dwContime = dwNowTime + dwConTime;
							pDest->m_arrACM.Add(ACM);
							bAdd = TRUE;
							break;
						}
					}
					if( !bAdd )
					{
						ACM.m_dwContime = dwNowTime + dwConTime;
						ACM.m_nTableNum = magicindex;
						pDest->m_arrACM.Add( ACM );
						stSetMagicInfo->m_stMagicTable.m_nConTime = 1;
					}

				}

				if( attack_type == 3 ) pDest->m_bPoisoning = TRUE;
				if( attack_type != 2 && attack_type != 4 && effect_kind == 0 )
					pDest->SetMotion( CUser::UM_Beated );
				
				stSetMagicInfo->m_arrDestUid.Add( *pDestUid );
				TRACE("DestUid :%d\n",*pDestUid);
				pDestUid++;	pHP++;
			}

			if( stSetMagicInfo->m_arrDestUid.GetSize() == 0 )
			{
				SAFE_DELETE( stSetMagicInfo->m_Comet );
				SAFE_DELETE( stSetMagicInfo );
				m_arrSetMagicInfo.RemoveAt(i);
				return;
			}
		}
	}
	if( bMagicSet ) return;
//	if( nDstUidNum == 0 ) return;

	if( bSuccess == 2 ) return;
	if( pSor == NULL ) return;

	_MAGIC_TABLE stMagicTable;
	
	stSetMagicInfo = new _SET_MAGIC_INFO;
	
	ASSERT( NULL != stSetMagicInfo );

	stMagicTable = *(m_arrMagicTableInfo.GetAt(magicindex -1)); 

	TRACE("AType:%d TableNum:%d \n",stMagicTable.m_nAttackType,stMagicTable.m_nTableNum);  
	stSetMagicInfo->m_stMagicTable	= stMagicTable;
	stSetMagicInfo->m_nSorUid		= iSorUid	  ;
	stSetMagicInfo->m_dwNowTime		= dwNowTime	  ;
	stSetMagicInfo->m_nDir			= Dir		  ;
	stSetMagicInfo->m_bReady		= TRUE		  ;
	stSetMagicInfo->m_Comet 		= NULL		  ;
	stSetMagicInfo->m_stMagicTable.m_nStTime = 0  ;
	stSetMagicInfo->m_stMagicTable.m_nConTime = dwConTime;
	ASSERT( NULL != iDestUid );
	
	attack_type = stSetMagicInfo->m_stMagicTable.m_nAttackType;
	effect_kind = stSetMagicInfo->m_stMagicTable.m_nEffecKind;

	MagicAttackKind( stSetMagicInfo, iSorUid, iDestUid, nDstUidNum );

	for ( i = 0; i < nDstUidNum; i++ )
	{
		pDest = GetUserWithUid( *iDestUid );
		if( pDest == NULL ) continue;//ASSERT( pDest != NULL );
//		if(*pHP <= 0) pDest->m_bDeadFlag = TRUE;
		pDest->m_Cur_Hp = *pHP;
		if( attack_type != 4 && effect_kind != 2  && effect_kind == 0 )  
			pDest->SetMotion( CUser::UM_Beated );

		stSetMagicInfo->m_arrDestUid.Add( *iDestUid );
		iDestUid++;
		pHP++;
	}

	int key;
	char *value;

	CUser *pSorUser = GetUserWithUid(iSorUid);
	if( pSorUser )
	{
		CPoint ptSoundPos = pSorUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
		key = SOUND_MAGIC+(stSetMagicInfo->m_stMagicTable.m_nTableNum-1)*3;
		if(g_SoundName.Lookup(key, value))
		{
			CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
		}
	}
//	CUser *pUser = GetUserWithUid( stSetMagicInfo->m_nSorUid );
//	CMyMagicData *magic_data;
//	pUser->m_mapMyMagicData.Lookup( stSetMagicInfo->m_stMagicTable.m_nTableNum + 1, magic_data );


	stSetMagicInfo->m_nSIndex = -1;
	m_arrSetMagicInfo.Add( stSetMagicInfo );
}

void CUserManager::MagicAttackKind( _SET_MAGIC_INFO* stSetMagicInfo, int nSorUid, int* pDesUid, int nDestNum )
{
	int attack_type = stSetMagicInfo->m_stMagicTable.m_nAttackType;

	switch( attack_type )
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		SetMagicComet( stSetMagicInfo, nSorUid, pDesUid ,nDestNum );
		break;
	}
}

void CUserManager::SetMagicComet(  _SET_MAGIC_INFO* stSetMagicInfo, int nSorUid, int* pDesUid, int nDestNum )
{
	stSetMagicInfo->m_Comet = new CMagicComet();
	if( stSetMagicInfo->m_Comet ) 
	{
		stSetMagicInfo->m_Comet->Load( stSetMagicInfo->m_stMagicTable.m_nFlyMagic );  
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
//매 프레임마다 매직이 있으면 매직을 적용 시켜준다.........
void CUserManager::ApplyMagicEx()
{
	_SET_MAGIC_INFO* stSetMagicInfo = NULL;
	int nSize = m_arrSetMagicInfo.GetSize(), t_size;
	CArray< int , int > arrDelIndex; 
	BOOL bDelMagic = FALSE;
	for( int i = 0; i < nSize; i++ )
	{
		stSetMagicInfo = m_arrSetMagicInfo.GetAt(i);
		switch( stSetMagicInfo->m_stMagicTable.m_nAttackType )
		{
		case 1:
			bDelMagic = MagicChar( stSetMagicInfo );
			if(bDelMagic)
			{
				arrDelIndex.Add( i ); 
				TRACE1("add - %d\n", i);
			}
			break;
		case 2:
			bDelMagic = FlyMagicChar( stSetMagicInfo );
			if(bDelMagic)
			{
				arrDelIndex.Add( i ); 
				TRACE1("add - %d\n", i);
			}
			break;
		case 3:
			bDelMagic = ConMagicChar( stSetMagicInfo );
			if(bDelMagic)
			{
				arrDelIndex.Add( i ); 
				TRACE1("add - %d\n", i);
			}
			break;
		case 4:
			bDelMagic = FlyMagicComet( stSetMagicInfo );
			if(bDelMagic)
			{
				arrDelIndex.Add( i ); 
				TRACE1("add - %d\n", i);
			}
			break;
			 
		}

		bDelMagic = FALSE;
	}//for( int i = 0; i < nSize; i++ )
	t_size = arrDelIndex.GetSize();
	int DelIndex;
	int nReduceIndex = 0;
	for( i = 0; i < t_size; i++ )
	{
		DelIndex = arrDelIndex.GetAt(i); 
		if(DelIndex >= nSize)
		{
			TRACE0("...\n");
		}
		stSetMagicInfo = m_arrSetMagicInfo.GetAt( DelIndex - nReduceIndex);
		stSetMagicInfo->m_arrDestUid.RemoveAll();  

		if( stSetMagicInfo->m_stMagicTable.m_nAttackType == 4 )
		{
			SAFE_DELETE( stSetMagicInfo->m_Comet );
		}

		SAFE_DELETE( stSetMagicInfo );
//		m_arrSetMagicInfo.RemoveAt(i);
		m_arrSetMagicInfo.RemoveAt( DelIndex - nReduceIndex );
		arrDelIndex.RemoveAt(i);
		t_size--;i--;nReduceIndex++;
	}

	arrDelIndex.RemoveAll();

	ApplyConMagic();
/*
	POSITION pos;
	int key;
	CUser *pUser;
	DWORD dwNowTime = GetTickCount();

	pos = m_mapUser.GetStartPosition();

	while(pos)
	{
		m_mapUser.GetNextAssoc(pos, key, pUser);

		if(pUser->m_dwPoisonTime < dwNowTime) pUser->m_nPoisonKind = 0;
	}
*/
}

void CUserManager::UserAppliedMagic( int uid, BYTE num, short* magic_index, DWORD* dwConTime )
{
	DWORD dwNowTime = GetTickCount();
	APPLIED_CON_MAGIC ACM;
	CUser* pUser = GetUserWithUid( uid );
	if( pUser )
	{
		for( int i = 0; i < num ; i++ )
		{
			ACM.m_dwContime = dwNowTime + dwConTime[i];
			ACM.m_nTableNum = magic_index[i];//테이블 번호
			pUser->m_arrACM.Add( ACM );
		}
	}
}

void CUserManager::ApplyConMagic()
{
	int					key;
	int					size;
	int					t_index;
	CUser				*pUser;
	POSITION			pos;
	APPLIED_CON_MAGIC	ACM;

	DWORD dwNowTime = GetTickCount();
	pos = m_mapUser.GetStartPosition();

	while(pos)
	{
		m_mapUser.GetNextAssoc(pos, key, pUser);
		if( pUser == NULL ) continue;
		size = pUser->m_arrACM.GetSize();
		for( int i = 0; i < size ; i++ )
		{
			ACM = pUser->m_arrACM.GetAt(i); 
			if( ACM.m_dwContime <= dwNowTime )
			{
				//pUser->m_nPoisonKind = 0;
				pUser->m_arrACM.RemoveAt(i);
				i--; size--;
				continue;
			}
			t_index = ACM.m_nTableNum;
			ApplyConMagic( t_index, pUser );
		}
	}
}

void CUserManager::ApplyConMagic( int t_index, CUser* pUser )
{
	_MAGIC_TABLE stMagicTable;
	CPoint pt;
	int pre_index;
	int post_index;
	int attack_type;
	LIGHT_INFO LI;

	stMagicTable = *(m_arrMagicTableInfo.GetAt(t_index -1));
	attack_type = stMagicTable.m_nAttackType;
	switch( attack_type )
	{
	case 1:
		pt = pUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
		LI.m_kind = 1;
		LI.m_pt = pt;
		CChildView::m_ptLightPos.Add(LI);    

		pre_index  = stMagicTable.m_nEndPreMagic;
		post_index = stMagicTable.m_nEndPostMagic;
		if( pre_index )
		{
			SetMagic( pUser->m_iUserId, pre_index, MAGIC_TYPE_DOWN );
		}
		if( post_index )
		{
			SetMagic( pUser->m_iUserId, post_index, MAGIC_TYPE_UP );
		}
		break;
	case 2:
		break;
	case 3:
		pUser->m_nPoisonKind = 3;
		break;
	}

}

void CUserManager::Cyclone(int suid, int index)
{
	CUser *pStartUser = GetUserWithUid(suid);
	CPoint ptSPos;
	if( pStartUser )
		ptSPos = pStartUser->GetPosition();
	else return;
	for( int i = 0; i < 6 ; i++ )
	{
		CMagic *pMagic = new CMagic;
		if (pMagic->SetCycloneMagicRes( index, ptSPos, MAGIC_TYPE_UP, i*2 ) )
		{
			pMagic->SetMagicType(MAGIC_TYPE_UP);
			m_lstMagic.AddTail(pMagic);
		}
		else delete pMagic;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
//캐릭터에 주는 매직 발동시간이 없는 것을 전제로 코딩하면
BOOL CUserManager::MagicChar( _SET_MAGIC_INFO* stSetMagicInfo )
{
	DWORD dwNowTime = GetTickCount();
	if( stSetMagicInfo->m_nSorUid != -1 )
	{
		if( stSetMagicInfo->m_stMagicTable.m_nStPreMagic )
		{
			//Set3DMagicEx( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPreMagic, TRUE );
			SetMagic( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPreMagic, MAGIC_TYPE_DOWN );
		}
		if( stSetMagicInfo->m_stMagicTable.m_nStPostMagic )
		{
			//Set3DMagicEx( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPostMagic, FALSE );
			SetMagic( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPostMagic, MAGIC_TYPE_UP );
		}

		if( stSetMagicInfo->m_stMagicTable.m_nStTime )
		{
			if( dwNowTime > stSetMagicInfo->m_dwNowTime + stSetMagicInfo->m_stMagicTable.m_nStTime )
			{	
				stSetMagicInfo->m_stMagicTable.m_nStTime = 0; 
				if( stSetMagicInfo->m_nSorUid == m_iMyUserId)
				{
					BOOL bSucc = TRUE;
					bSucc = SendMagicAttack(stSetMagicInfo->m_stMagicTable.m_nTableNum,stSetMagicInfo->m_stMagicTable.m_vMethod,
									 stSetMagicInfo->m_arrDestUid.GetAt(0),stSetMagicInfo->m_nDir);
					stSetMagicInfo->m_arrDestUid.RemoveAll();  
					if( bSucc == FALSE )
					{
						TRACE("실패\n");
						stSetMagicInfo->m_bReady = TRUE;
						return TRUE;
					}
					return FALSE;
				}
			}
			return FALSE;
		}

		if( stSetMagicInfo->m_stMagicTable.m_nStPreMagic || stSetMagicInfo->m_stMagicTable.m_nStPostMagic )
		{
			CUser *pSor = GetUserWithUid( stSetMagicInfo->m_nSorUid );
			if(pSor)//ASSERT( pSor != NULL );
				pSor->SetMotion( CUser::UM_Magic ); 
		}
		stSetMagicInfo->m_stMagicTable.m_nStPostMagic = 0;
		stSetMagicInfo->m_stMagicTable.m_nStPreMagic  = 0; 

	}//if( stSetMagicInfo->m_nSorUid != -1 )

//	if( dwNowTime >= stSetMagicInfo->m_stMagicTable.m_nStTime + stSetMagicInfo->m_dwNowTime ) 
//	if( stSetMagicInfo->m_bReady == FALSE )
//	{
		if( !(stSetMagicInfo->m_bReady) ) return FALSE; 	
		if( !(stSetMagicInfo->m_stMagicTable.m_nConTime) ) 
		{

			int nArrSize = stSetMagicInfo->m_arrDestUid.GetSize();
			for( int i = 0; i < nArrSize; i++ )
			{
				int nDestUid = stSetMagicInfo->m_arrDestUid.GetAt(i);
				if( nDestUid != -1 )
				{
					CUser *pDest = GetUserWithUid( nDestUid );
					if( pDest == NULL ) continue;//ASSERT( pDest != NULL );
					//if( pDest == NULL ) continue;
					//pDest->m_nPoisonKind = 1;
					if( stSetMagicInfo->m_stMagicTable.m_nEndPreMagic )
					{
						//Set3DMagicEx( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, TRUE );    
						SetMagic( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, MAGIC_TYPE_DOWN );    
					}
					if( stSetMagicInfo->m_stMagicTable.m_nEndPostMagic )
					{
						//Set3DMagicEx( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPostMagic, FALSE);    
						SetMagic( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPostMagic, MAGIC_TYPE_UP);    
					}

					CPoint ptSoundPos = pDest->GetPosition() - CChildView::MapManager.GetMapStartPos();
					
					int key;
					char *value;

					key = SOUND_MAGIC+(stSetMagicInfo->m_stMagicTable.m_nTableNum-1)*3+2;
					if(g_SoundName.Lookup(key, value))
					{
						CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE);
					}
				}


			}//for( int i = 0; i < nArrSize; i++ )
			if( stSetMagicInfo->m_bReady )
			{
//				int key;
//				char *value;

//				key = SOUND_MAGIC+(stSetMagicInfo->m_stMagicTable.m_nTableNum-1)*3+2;
//				if(g_SoundName.Lookup(key, value))
//				{
//					CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE);
//				}
				stSetMagicInfo->m_stMagicTable.m_nConTime  = -1;//결과의 지속시간이 없는 경우 한번만 적용시키고 더이상 적용시키지 않기 위해서 시간을 음수값을 준다. 
			}
		}
		else if( dwNowTime <= stSetMagicInfo->m_dwNowTime + stSetMagicInfo->m_stMagicTable.m_nConTime  )
		{
			int nArrSize = stSetMagicInfo->m_arrDestUid.GetSize();
			for( int i = 0; i < nArrSize; i++ )
			{
				int nDestUid = stSetMagicInfo->m_arrDestUid.GetAt(i);
				if( nDestUid != -1 )
				{
					//CUser *pDest = GetUserWithUid( nDestUid );
					//ASSERT( pDest != NULL );
					//pDest->m_nPoisonKind = 1;
					if( stSetMagicInfo->m_stMagicTable.m_nEndPreMagic )
					{
						//Set3DMagicEx( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, TRUE );    
						SetMagic( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, MAGIC_TYPE_DOWN );    
					}
					if( stSetMagicInfo->m_stMagicTable.m_nEndPostMagic )
					{
						//Set3DMagicEx( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPostMagic,  FALSE);    
						SetMagic( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPostMagic,  MAGIC_TYPE_UP);    
					}
				}//if( nDestUid != -1 )

				if( stSetMagicInfo->m_nSIndex != -1 )
				{
					CUser *pDstUser = GetUserWithUid( nDestUid );
					if( pDstUser )
					{
						CPoint ptSoundPos = pDstUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
						int key;
						char *value;

						key = SOUND_MAGIC+(stSetMagicInfo->m_stMagicTable.m_nTableNum-1)*3+2;
						if(g_SoundName.Lookup(key, value))
						{
							CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE);
						}
					}
				}//if( stSetMagicInfo->m_nSIndex != -1 )

			}//for( int i = 0; i < nArrSize; i++ )
			stSetMagicInfo->m_nSIndex = -1;
			return TRUE;

		}
		else
		{
			if(stSetMagicInfo->m_bReady)
				return TRUE;
		}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
//발사되어 목표물로 날아가는 마법
BOOL CUserManager::FlyMagicChar( _SET_MAGIC_INFO* stSetMagicInfo )
{
	DWORD dwNowTime = GetTickCount();
	if( stSetMagicInfo->m_nSorUid != -1 )
	{
		if( stSetMagicInfo->m_stMagicTable.m_nStPreMagic )
		{
			//Set3DMagicEx( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPreMagic, TRUE );
			SetMagic( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPreMagic, MAGIC_TYPE_DOWN );
		}
		if( stSetMagicInfo->m_stMagicTable.m_nStPostMagic )
		{
			//Set3DMagicEx( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPostMagic,  FALSE);
			SetMagic( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPostMagic,  MAGIC_TYPE_UP);
		}

		if( stSetMagicInfo->m_stMagicTable.m_nStTime )
		{
			if( dwNowTime > stSetMagicInfo->m_dwNowTime + stSetMagicInfo->m_stMagicTable.m_nStTime )
			{	
				stSetMagicInfo->m_stMagicTable.m_nStTime = 0; 
				if( stSetMagicInfo->m_nSorUid == m_iMyUserId)
				{
					BOOL bSucc = TRUE;
					TRACE("Send SUID:%d DUID:%d MagicIndex:%d\n",stSetMagicInfo->m_nSorUid,stSetMagicInfo->m_arrDestUid.GetAt(0),stSetMagicInfo->m_stMagicTable.m_nTableNum);
					bSucc = SendMagicAttack(stSetMagicInfo->m_stMagicTable.m_nTableNum,stSetMagicInfo->m_stMagicTable.m_vMethod,
									 stSetMagicInfo->m_arrDestUid.GetAt(0),stSetMagicInfo->m_nDir);
					stSetMagicInfo->m_arrDestUid.RemoveAll();  
//					Cyclone(stSetMagicInfo->m_nSorUid , 70);//회오리^^
					if( bSucc == FALSE )
					{
						stSetMagicInfo->m_bReady = TRUE;
						return TRUE;
					}
//					for( int i = 0 ; i < 8 ; i++ )
//					{
//						AttackDir3DMagicEx(stSetMagicInfo->m_nSorUid , i, stSetMagicInfo->m_stMagicTable.m_nFlyMagic, /*nDestUid*/-1);//임시
//					}//8방향 날려 보내기...
					return FALSE;
				}
				else
				{
//					Cyclone(stSetMagicInfo->m_nSorUid , 70);//회오리^^
//					if( bSucc == FALSE )
//						stSetMagicInfo->m_bReady = TRUE;
//					for( int i = 0 ; i < 8 ; i++ )
//					{
//						AttackDir3DMagicEx(stSetMagicInfo->m_nSorUid , i, stSetMagicInfo->m_stMagicTable.m_nFlyMagic, /*nDestUid*/-1);//임시
//					}//8방향 날려 보내기...
					return FALSE;

				}
			}
			return FALSE;
		}
		if( stSetMagicInfo->m_stMagicTable.m_nStPreMagic || stSetMagicInfo->m_stMagicTable.m_nStPostMagic )
		{
			CUser *pSor = GetUserWithUid( stSetMagicInfo->m_nSorUid );
			if(pSor)//ASSERT( pSor != NULL );
				pSor->SetMotion( CUser::UM_Magic ); 
		}
		stSetMagicInfo->m_stMagicTable.m_nStPostMagic = 0;
		stSetMagicInfo->m_stMagicTable.m_nStPreMagic  = 0; 

//		stSetMagicInfo->m_nSorUid = -1;
	}//if( stSetMagicInfo->m_nSorUid != -1 )

	if( !(stSetMagicInfo->m_bReady) ) return FALSE;
	if( stSetMagicInfo->m_stMagicTable.m_nFlyMagic && stSetMagicInfo->m_bReady )
	{
		CUser *pSorUser = GetUserWithUid( stSetMagicInfo->m_nSorUid );
		if( pSorUser )
		{
			int key;
			char *value;
			CPoint ptSoundPos = pSorUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
			key = SOUND_MAGIC+(stSetMagicInfo->m_stMagicTable.m_nTableNum-1)*3+1;
			if(g_SoundName.Lookup(key, value))
			{
				CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
			}
		}
		if( stSetMagicInfo->m_nDir == 100 )
		{//지정해준 캐릭터 방향으로 날아가는 매직 설정
			int nArrSize = stSetMagicInfo->m_arrDestUid.GetSize();
			if( nArrSize == 0 ) return TRUE;
			int nDestUid;
			for( int i = 0; i < nArrSize ; i++)
			{
				nDestUid = stSetMagicInfo->m_arrDestUid.GetAt(i);
				for( int j = 0; j < stSetMagicInfo->m_stMagicTable.m_nFireNum ; j++ )
				{
					//Attack3DMagicEx(stSetMagicInfo->m_nSorUid , nDestUid, stSetMagicInfo->m_stMagicTable.m_nFlyMagic,j+1);//임시
					AttackMagic(stSetMagicInfo->m_nSorUid , nDestUid, stSetMagicInfo->m_stMagicTable.m_nFlyMagic,j+1);
					if( nDestUid != -1 )
					{
						CUser *pDest = GetUserWithUid( nDestUid );
						if( pDest == NULL ) continue;//ASSERT( pDest != NULL );

						if( stSetMagicInfo->m_stMagicTable.m_nConTime )
						{
							if( stSetMagicInfo->m_stMagicTable.m_nEndPreMagic != 0) 
								pDest->SetFlyEndMagic(stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, TRUE, stSetMagicInfo->m_stMagicTable.m_nTableNum,1);
							if( stSetMagicInfo->m_stMagicTable.m_nEndPostMagic != 0 )
								pDest->SetFlyEndMagic(stSetMagicInfo->m_stMagicTable.m_nEndPostMagic, stSetMagicInfo->m_stMagicTable.m_nTableNum, FALSE,1);   
						}
						else
						{
							if( stSetMagicInfo->m_stMagicTable.m_nEndPreMagic != 0) 
								pDest->SetFlyEndMagic(stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, TRUE, stSetMagicInfo->m_stMagicTable.m_nTableNum);
							if( stSetMagicInfo->m_stMagicTable.m_nEndPostMagic != 0 )
								pDest->SetFlyEndMagic(stSetMagicInfo->m_stMagicTable.m_nEndPostMagic, FALSE, stSetMagicInfo->m_stMagicTable.m_nTableNum);   
						}

					}
				}
			}
//			return TRUE;
		}
		else
		{//지정해준 방향으로 날아가는 매직
			int nArrSize = stSetMagicInfo->m_arrDestUid.GetSize();
			if( nArrSize == 0 ) return TRUE;
			int nDestUid;
			for( int i = 0; i < nArrSize ; i++ )
			{

				for( int j = 0; j < stSetMagicInfo->m_stMagicTable.m_nFireNum ; j++ )
				{
					nDestUid = stSetMagicInfo->m_arrDestUid.GetAt(i);
					//Attack3DMagicEx(stSetMagicInfo->m_nSorUid , nDestUid, stSetMagicInfo->m_stMagicTable.m_nFlyMagic, j+1);//임시
					AttackMagic(stSetMagicInfo->m_nSorUid , nDestUid, stSetMagicInfo->m_stMagicTable.m_nFlyMagic,j+1);
					if( nDestUid != -1 )
					{
						CUser *pDest = GetUserWithUid( nDestUid );
						if( pDest == NULL ) continue;//ASSERT( pDest != NULL );
						
						if( stSetMagicInfo->m_stMagicTable.m_nConTime )
						{
							if( stSetMagicInfo->m_stMagicTable.m_nEndPreMagic != 0) 
								pDest->SetFlyEndMagic(stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, TRUE, stSetMagicInfo->m_stMagicTable.m_nTableNum,1);
							if( stSetMagicInfo->m_stMagicTable.m_nEndPostMagic != 0 )
								pDest->SetFlyEndMagic(stSetMagicInfo->m_stMagicTable.m_nEndPostMagic, FALSE, stSetMagicInfo->m_stMagicTable.m_nTableNum,1);   
						}
						else
						{
							if( stSetMagicInfo->m_stMagicTable.m_nEndPreMagic != 0) 
								pDest->SetFlyEndMagic(stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, TRUE, stSetMagicInfo->m_stMagicTable.m_nTableNum);
							if( stSetMagicInfo->m_stMagicTable.m_nEndPostMagic != 0 )
								pDest->SetFlyEndMagic(stSetMagicInfo->m_stMagicTable.m_nEndPostMagic, FALSE, stSetMagicInfo->m_stMagicTable.m_nTableNum);   
						}
					}
				}
			}
		}

		stSetMagicInfo->m_stMagicTable.m_nFlyMagic = 0; 
	}
	if( !(stSetMagicInfo->m_stMagicTable.m_nConTime) )
	{
		if(stSetMagicInfo->m_bReady)
			return TRUE;
	}
	else if( dwNowTime >= stSetMagicInfo->m_dwNowTime + stSetMagicInfo->m_stMagicTable.m_nConTime ) 
	{
		if( stSetMagicInfo->m_bReady ) 
			return TRUE;
	}
	return FALSE;

}

/////////////////////////////////////////////////////////////////////////////////////////
//캐릭터에 효과 후 독이 지속되는 마법
BOOL CUserManager::ConMagicChar( _SET_MAGIC_INFO* stSetMagicInfo )
{
	DWORD dwNowTime = GetTickCount();
	if( stSetMagicInfo->m_nSorUid != -1 )
	{
		if( stSetMagicInfo->m_stMagicTable.m_nStPreMagic )
		{
			//Set3DMagicEx( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPreMagic, TRUE);
			SetMagic( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPreMagic, MAGIC_TYPE_DOWN);
		}
		if( stSetMagicInfo->m_stMagicTable.m_nStPostMagic )
		{
			//Set3DMagicEx( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPostMagic, FALSE );
			SetMagic( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPostMagic, MAGIC_TYPE_UP );
		}

		if( stSetMagicInfo->m_stMagicTable.m_nStTime )
		{
			if( dwNowTime > stSetMagicInfo->m_dwNowTime + stSetMagicInfo->m_stMagicTable.m_nStTime )
			{	
				stSetMagicInfo->m_stMagicTable.m_nStTime = 0; 

				if( stSetMagicInfo->m_nSorUid == m_iMyUserId)
				{
					BOOL bSucc = TRUE;					
					bSucc = SendMagicAttack(stSetMagicInfo->m_stMagicTable.m_nTableNum,stSetMagicInfo->m_stMagicTable.m_vMethod,
									 stSetMagicInfo->m_arrDestUid.GetAt(0),stSetMagicInfo->m_nDir);
					stSetMagicInfo->m_arrDestUid.RemoveAll();  
					if( bSucc == FALSE )
					{
						stSetMagicInfo->m_bReady = TRUE;
						return TRUE;
					}
					return FALSE;
				}
			}
			return FALSE;
		}

		if( stSetMagicInfo->m_stMagicTable.m_nStPreMagic || stSetMagicInfo->m_stMagicTable.m_nStPostMagic )
		{
			CUser *pSor = GetUserWithUid( stSetMagicInfo->m_nSorUid );
			if(pSor)//ASSERT( pSor != NULL );
				pSor->SetMotion( CUser::UM_Magic ); 
		}
//		if( stSetMagicInfo->m_bReady == FALSE )
			//stSetMagicInfo->m_nSorUid = -1;
		stSetMagicInfo->m_stMagicTable.m_nStPostMagic = 0;
		stSetMagicInfo->m_stMagicTable.m_nStPreMagic  = 0; 
	}//if( stSetMagicInfo->m_nSorUid != -1 )

	if( !(stSetMagicInfo->m_bReady) ) return FALSE; 	
	if( (stSetMagicInfo->m_stMagicTable.m_nConTime) ) 
	{
		CUser* ppp = GetMe();
		TRACE("%d %d\n",ppp->GetPosition().x,ppp->GetPosition().y);  

		int nArrSize = stSetMagicInfo->m_arrDestUid.GetSize();
		for( int i = 0; i < nArrSize; i++ )
		{
			int nDestUid = stSetMagicInfo->m_arrDestUid.GetAt(i);
			if( nDestUid != -1 )
			{
				CUser *pDest = GetUserWithUid( nDestUid );
				if( pDest == NULL ) continue;//ASSERT( pDest != NULL );
				//pDest->m_nPoisonKind = 1;
				if( stSetMagicInfo->m_stMagicTable.m_nEndPreMagic )
				{
					//Set3DMagicEx( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, TRUE);    
					SetMagic( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, MAGIC_TYPE_DOWN);    
				}
				if( stSetMagicInfo->m_stMagicTable.m_nEndPostMagic )
				{
					//Set3DMagicEx( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPostMagic, FALSE );    
					SetMagic( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPostMagic, MAGIC_TYPE_UP );    
				}
				pDest->m_nPoisonKind = 3;
				pDest->m_dwPoisonTime = dwNowTime  + stSetMagicInfo->m_stMagicTable.m_nConTime;

				if( stSetMagicInfo->m_bReady )
				{
					CPoint ptSoundPos = pDest->GetPosition() - CChildView::MapManager.GetMapStartPos();
					int key;
					char *value;

					key = SOUND_MAGIC+(stSetMagicInfo->m_stMagicTable.m_nTableNum-1)*3+2;
					if(g_SoundName.Lookup(key, value))
					{
						CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
					}
				}
			}
		}//for( int i = 0; i < nArrSize; i++ )
		if( stSetMagicInfo->m_bReady )
		{
			stSetMagicInfo->m_stMagicTable.m_nConTime  = 0;//결과의 지속시간이 없는 경우 한번만 적용시키고 더이상 적용시키지 않기 위해서 시간을 음수값을 준다. 
		}
		return TRUE;
	}
	else
	{
		if(stSetMagicInfo->m_bReady)
			return TRUE;
	}
	return FALSE;

}

BOOL CUserManager::FlyMagicComet( _SET_MAGIC_INFO* stSetMagicInfo )
{
	DWORD dwNowTime = GetTickCount();
	if( stSetMagicInfo->m_nSorUid != -1 )
	{
		if( stSetMagicInfo->m_stMagicTable.m_nStPreMagic )
		{
			//Set3DMagicEx( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPreMagic, TRUE );
			SetMagic( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPreMagic, MAGIC_TYPE_DOWN );
		}
		if( stSetMagicInfo->m_stMagicTable.m_nStPostMagic )
		{
			//Set3DMagicEx( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPostMagic, FALSE );
			SetMagic( stSetMagicInfo->m_nSorUid, stSetMagicInfo->m_stMagicTable.m_nStPostMagic, MAGIC_TYPE_UP );
		}

		if( stSetMagicInfo->m_stMagicTable.m_nStTime )
		{
			if( dwNowTime > stSetMagicInfo->m_dwNowTime + stSetMagicInfo->m_stMagicTable.m_nStTime )
			{	
				stSetMagicInfo->m_stMagicTable.m_nStTime = 0; 
				if( stSetMagicInfo->m_nSorUid == m_iMyUserId)
				{
					BOOL bSucc = TRUE;
					TRACE("Send SUID:%d DUID:%d MagicIndex:%d\n",stSetMagicInfo->m_nSorUid,stSetMagicInfo->m_arrDestUid.GetAt(0),stSetMagicInfo->m_stMagicTable.m_nTableNum);
					bSucc = SendMagicAttack(stSetMagicInfo->m_stMagicTable.m_nTableNum,stSetMagicInfo->m_stMagicTable.m_vMethod,
									 stSetMagicInfo->m_arrDestUid.GetAt(0),stSetMagicInfo->m_nDir);
					stSetMagicInfo->m_arrDestUid.RemoveAll();  
					if( bSucc == FALSE )
					{
						stSetMagicInfo->m_bReady = TRUE;
						return TRUE;//실패 해서 지워버림
					}
					return FALSE;
				}
			}
			return FALSE;
		}
		if( stSetMagicInfo->m_stMagicTable.m_nStPreMagic || stSetMagicInfo->m_stMagicTable.m_nStPostMagic )
		{
			CUser *pSor = GetUserWithUid( stSetMagicInfo->m_nSorUid );
			if(pSor)//ASSERT( pSor != NULL );
				pSor->SetMotion( CUser::UM_Magic ); 
		}
		stSetMagicInfo->m_stMagicTable.m_nStPostMagic = 0;
		stSetMagicInfo->m_stMagicTable.m_nStPreMagic  = 0; 


		if( !(stSetMagicInfo->m_bReady) ) return FALSE;

		CUser *pSorUser = GetUserWithUid(stSetMagicInfo->m_nSorUid);
		if( pSorUser )
		{
			CPoint ptSoundPos = pSorUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
			char *value;
			int key = SOUND_MAGIC+(stSetMagicInfo->m_stMagicTable.m_nTableNum-1)*3+2;
			if(g_SoundName.Lookup(key, value))
			{
				CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
			}
		}
		
		if( !stSetMagicInfo->m_arrDestUid.GetSize() )
		{
			TRACE("대상자가 없어 소멸시킴\n");
			return TRUE;
		}
		CUser *pDestUser = GetUserWithUid(stSetMagicInfo->m_arrDestUid.GetAt(0));
		CUser *pStartUser = GetUserWithUid(stSetMagicInfo->m_nSorUid);
		CPoint ptSPos;
		CPoint ptEPos;

		if (pStartUser && pDestUser) 
		{
			pStartUser->SetMotion(CUser::UM_Attack);
			ptSPos = pStartUser->GetPosition();
			ptEPos = pDestUser->GetPosition();

			CPoint tmp = CChildView::MapManager.GetMapStartPos();

			CPoint UserPos;
			CPoint tmp_Pos;// 보정할 값이 들어갈곳..

			UserPos = ptSPos - tmp;
			ptEPos -= tmp;
			tmp_Pos = ptEPos - UserPos;
			ptEPos = ptSPos + tmp_Pos;

			if( stSetMagicInfo->m_Comet ) 
			{
				stSetMagicInfo->m_Comet->SetPosition( ptSPos, ptEPos );
				pStartUser->SetDirection(stSetMagicInfo->m_Comet->SetRoute());
			}

		}

		stSetMagicInfo->m_nSorUid = -1;

	}//if( stSetMagicInfo->m_nSorUid != -1 )

	if( stSetMagicInfo->m_Comet->DrawComet() )
	{
		int nDestUid = stSetMagicInfo->m_arrDestUid.GetAt(0);
		if( stSetMagicInfo->m_stMagicTable.m_nEndPreMagic )
		{
			//Set3DMagicEx( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, TRUE );    
			SetMagic( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPreMagic, MAGIC_TYPE_DOWN );    
		}
		if( stSetMagicInfo->m_stMagicTable.m_nEndPostMagic )
		{
			//Set3DMagicEx( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPostMagic, FALSE);    
			SetMagic( nDestUid, stSetMagicInfo->m_stMagicTable.m_nEndPostMagic, MAGIC_TYPE_UP);    
		}

		CUser *pDstUser = GetUserWithUid(nDestUid);
		if( pDstUser )
		{
			pDstUser->SetMotion( CUser::UM_Beated );

			CPoint ptSoundPos = pDstUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
			int key;
			char *value;

			key = SOUND_MAGIC+(stSetMagicInfo->m_stMagicTable.m_nTableNum-1)*3+2;
			if(g_SoundName.Lookup(key, value))
			{
				CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CUserManager::AttackMagic(int suid, int euid, int magic_type,int nSeries)
{
	// uid로 부터 위치를 가져와야함..음...
	CUser *pDestUser = GetUserWithUid(euid);
	CUser *pStartUser = GetUserWithUid(suid);
	CPoint ptSPos;
	CPoint ptEPos;

	if (pStartUser && pDestUser) 
	{
		pStartUser->SetMotion(CUser::UM_Attack);
		ptSPos = pStartUser->GetPosition();
		ptEPos = pDestUser->GetPosition();

		CPoint tmp = CChildView::MapManager.GetMapStartPos();

		CPoint UserPos;
		CPoint tmp_Pos;// 보정할 값이 들어갈곳..

		UserPos = ptSPos - tmp;
		ptEPos -= tmp;
		tmp_Pos = ptEPos - UserPos;
		ptEPos = ptSPos + tmp_Pos;

		int nDir = SetAimedMagic(ptSPos, ptEPos, magic_type, MAGIC_TYPE_UP, euid, nSeries);
		pStartUser->SetDirection(nDir);
	}
}
int CUserManager::GetClientDirection(CPoint ptStart, CPoint ptEnd)//클라이언트용 방향
{
	int nDir = 0;

	int deltax = ptEnd.x - ptStart.x;
	int deltay = ptEnd.y - ptStart.y;

	if (deltax==0) 
	{
		if (ptEnd.y>ptStart.y) nDir = DIR_DOWN;
		else nDir = DIR_UP;
	}
	else
	{
		double lean = (double)abs(deltay)/abs(deltax);

		if (ptEnd.y>ptStart.y) nDir |= DIR_DOWN;			//X,Y값에 따른 방향 설정
		else if (ptEnd.y < ptStart.y) nDir |= DIR_UP;
		if (ptEnd.x>ptStart.x) nDir |= DIR_RIGHT;
		else if (ptEnd.x < ptStart.x) nDir |= DIR_LEFT;

		if (lean<.5) nDir &= ~(DIR_UP|DIR_DOWN);			//기울기로 판단한 8방향 재설정
		else if (lean>2.) nDir &= ~(DIR_LEFT|DIR_RIGHT);
	}
	return nDir;
}

BYTE CUserManager::GetDirection(CPoint ptMyPos, CPoint ptDstPos)
{
	BYTE dir = 100;
	double distance = sqrt( (ptDstPos.x - ptMyPos.x)*(ptDstPos.x - ptMyPos.x)
				+ (ptDstPos.y - ptMyPos.y)*(ptDstPos.y - ptMyPos.y) );
	double dx,dy;
	if (distance > 0)
	{
		dx = double(ptDstPos.x - ptMyPos.x) / distance;
		dy = double(ptDstPos.y - ptMyPos.y) / distance;
	}

	if		( dx >= 0.9239  && dy > 0 )	dir = 7;
	else if ( dx >= 0.3827  && dx < 0.9239  && dy > 0 )	dir = 0;
	else if ( dx >= -0.3827 && dx < 0.3827  && dy > 0 )	dir = 1;
	else if ( dx >= -0.9239 && dx < -0.3827 && dy > 0 )	dir = 2;
	else if ( dx < -0.9239  && dy > 0 )	dir = 3;
	else if ( dx < -0.9239  && dy < 0 )	dir = 3;
	else if ( dx >= -0.9239 && dx < -0.3827 && dy < 0 )	dir = 4;
	else if ( dx >= -0.3827 && dx < 0.3827  && dy < 0 )	dir = 5;
	else if ( dx >= 0.3827  && dx < 0.9239  && dy < 0 )	dir = 6;
	else if ( dx >= 0.9239  && dy < 0 )	dir = 7;
	return dir;
}

BOOL CUserManager::SendReadyMagic(int type)
{
	BYTE buf[20];
	BYTE dir;
	CPoint ptMyPos;
	CPoint ptDstPos;
	CPoint ptMapOffset;
	int offset = 0;
	if(m_pMe == NULL) return FALSE;

	if(m_pMe->IsCharAttack())
	{
		TRACE("공격중이다.\n");
		return TRUE;//공격중에는 다시 공격을 못하게 막는다.
	}
	if( m_pMe->m_BattleMode == 0 )
	{
		TRACE("비무장이다.\n");
		return FALSE;//공격중에는 다시 공격을 못하게 막는다.
	}
	switch( m_pMe->m_AnimationIndex )
	{
	case 1:case 6:return FALSE;
	}
//	if( pMe->m_Cur_Mp <= 0 ) 
//	{
//		TRACE0("MP가 없다\n");
//		return FALSE;
//	}

	int nSorUid;
	int nSTime;
	int nArrSize = m_arrSetMagicInfo.GetSize();
	for( int i = 0 ; i < nArrSize; i++ )
	{
		nSorUid = m_arrSetMagicInfo.GetAt(i)->m_nSorUid;
		nSTime =  m_arrSetMagicInfo.GetAt(i)->m_stMagicTable.m_nStTime;
		if( nSorUid == m_iMyUserId && nSTime != 0 )
		{//여러가지의 매직을 동시에 쓰지 못하게 하기 위해서......
			TRACE0("매직사용중\n"); 
			return FALSE;
		}
	}

	short	vMethod = m_arrMagicTableInfo.GetAt(type - 1)->m_vMethod;
	int		nDes	= m_arrMagicTableInfo.GetAt(type - 1)->m_nDes;
//	if( m_arrMagicTableInfo.GetAt(type - 1)->m_vMethod == 1 && m_iCurFocusUidInOthers==UM_UID_INVALID && m_arrMagicTableInfo.GetAt(type - 1)->m_nDes == 3)
	if( vMethod == 1 && m_iCurFocusUidInOthers==UM_UID_INVALID && nDes == 3)
	{
		TRACE("PKT_ATTACK_ARROW 대상자가 없어 실패\n");
		return FALSE;//패킷이 PKT_ATTACK_ARROW이고 포커스가 없다면 패킷을 보내지 않는다.....
	}
	CPacketParser::SetByte(buf, offset, PKT_MAGIC_READY);

	if (m_iCurFocusUidInOthers!=UM_UID_INVALID) //맞을 대상이 존재
	{
		if(nDes != 3 && vMethod == 1 && m_iCurFocusUidInOthers > 10000) return FALSE;
		CPacketParser::SetInt(buf, offset, m_iCurFocusUidInOthers);
	}
	else
	{
//		if(m_arrMagicTableInfo.GetAt(type - 1)->m_nDes != 3 && m_arrMagicTableInfo.GetAt(type - 1)->m_vMethod == 1)
		if(nDes != 3 && vMethod == 1)
			CPacketParser::SetInt(buf, offset, m_iMyUserId);
		else
			CPacketParser::SetInt(buf, offset, -1);
	}

	CPacketParser::SetShort(buf, offset, type);			

	ptMyPos = m_pMe->GetPosition();
	ptDstPos = m_pMe->m_bpDirectXMng->m_pMouseInfo->m_posMouse;

	ptMapOffset = CChildView::MapManager.GetMapStartPos();
	ptMyPos.x -= ptMapOffset.x;
	ptMyPos.y -= ptMapOffset.y;// + 100;

	dir = GetDirection( ptMyPos, ptDstPos );
	CPacketParser::SetByte(buf, offset, dir);

	CChildView::SocketMng.Send(offset, buf);
	TRACE("PKT_MAGIC_READY 보냈다.\n");
	return TRUE;

}
BOOL CUserManager::SendMagicAttack(int type, short method, int nDestUid, BYTE dir)
{
	BYTE buf[20];
	int offset = 0;
	if(m_pMe == NULL)
		return FALSE;

	if(m_pMe->IsCharAttack())
	{
		TRACE("공격중이다.\n");
		return FALSE;//공격중에는 다시 공격을 못하게 막는다.
	}
	if( m_pMe->m_BattleMode == 0 )
	{
		TRACE("비무장이다.\n");
		return FALSE;//공격중에는 다시 공격을 못하게 막는다.
	}
	switch( m_pMe->m_AnimationIndex )
	{
	case 1:case 6:return FALSE;
	}

//	if( pMe->m_Cur_Mp <= 0 ) 
//	{
//		TRACE0("MP가 없다\n");
//		return FALSE;
//	}

	int nSorUid;
	int nSTime;
	int nArrSize = m_arrSetMagicInfo.GetSize();
	for( int i = 0 ; i < nArrSize; i++ )
	{
		nSorUid = m_arrSetMagicInfo.GetAt(i)->m_nSorUid;
		nSTime =  m_arrSetMagicInfo.GetAt(i)->m_stMagicTable.m_nStTime;
//		if( m_arrSetMagicInfo.GetAt(i)->m_nSorUid == m_iMyUserId && m_arrSetMagicInfo.GetAt(i)->m_stMagicTable.m_nStTime != 0 )
		if( nSorUid == m_iMyUserId && nSTime != 0 )
		{//여러가지의 매직을 동시에 쓰지 못하게 하기 위해서......
			TRACE0("매직사용중\n"); 
			return FALSE;
		}
	}

	CRect MyRect;
	CRect DestRect;
	CPoint ptMyPos;
	CPoint ptDstPos;
	CPoint ptMapOffset;

	switch( method )
	{
	case 1://PKT_ATTACK_ARROW
		if(m_pMe->m_BattleMode == 0 )//&& m_iCurFocusUidInOthers >= 10000)
			return FALSE;
		CPacketParser::SetByte(buf, offset, PKT_ATTACK_ARROW);
		if( nDestUid == -1 )
		{
			if (m_iCurFocusUidInOthers!=UM_UID_INVALID) //맞을 대상이 존재
			{
				int	nDes= m_arrMagicTableInfo.GetAt(type - 1)->m_nDes;

				if(nDes != 3 && m_iCurFocusUidInOthers > 10000) return FALSE;

				CUser *pDest = GetUserWithUid(m_iCurFocusUidInOthers);
				ASSERT(m_iCurFocusUidInOthers!=m_iMyUserId);

				if(pDest->IsCharDie())
					return FALSE;//상대방이 죽어 있으면 취소.
				if(m_iCurFocusUidInOthers >= 10000 && pDest->m_bDeadEnable == 0)
					return FALSE;
				CPacketParser::SetInt(buf, offset, m_iCurFocusUidInOthers);
			}
			else if(m_arrMagicTableInfo.GetAt(type - 1)->m_nDes != 3 )
			{
				if( m_iCurFocusUidInOthers > 10000 ) return FALSE;
				CPacketParser::SetInt(buf, offset, m_iMyUserId);
			}
			else
				return FALSE;
		}
		else
			CPacketParser::SetInt(buf, offset, nDestUid);
		CPacketParser::SetByte(buf, offset, type);
		CChildView::SocketMng.Send(offset, buf);
		TRACE("PKT_ATTACK_ARROW 보냈다.\n");
		m_pMe->SetMotion(CUser::UM_Stop);
		return TRUE;
	break;

	case 2://PKT_ATTACK_MAGIC_RAIL
		ptMyPos = m_pMe->GetPosition();
		ptMapOffset = CChildView::MapManager.GetMapStartPos();
		ptMyPos.x -= ptMapOffset.x;
		ptMyPos.y -= ptMapOffset.y;// + 100;
		
		ptDstPos = m_pMe->m_bpDirectXMng->m_pMouseInfo->m_posMouse;

		CPacketParser::SetByte(buf, offset, PKT_ATTACK_MAGIC_RAIL);
		CPacketParser::SetShort(buf, offset, type);			
		if( dir == 100 )
		{
			dir = GetDirection( ptMyPos, ptDstPos );
			CPacketParser::SetByte(buf, offset, dir);
		}
		else
		{
			CPacketParser::SetByte(buf, offset, dir);
		}

		CChildView::SocketMng.Send(offset, buf);
		TRACE("PKT_ATTACK_MAGIC_RAIL 보냈다.\n");
		m_pMe->SetMotion(CUser::UM_Stop);
		return TRUE;

	case 3://PKT_MAGIC_CIRCLE
		CPacketParser::SetByte(buf, offset, PKT_MAGIC_CIRCLE);
		CPacketParser::SetShort(buf, offset, type);			
		CChildView::SocketMng.Send(offset, buf);
		TRACE("PKT_MAGIC_CIRCLE 보냈다.\n");
		m_pMe->SetMotion(CUser::UM_Stop);
		return TRUE;
	}
	return FALSE;
}

BOOL CUserManager::SendAttack(int type)
{
	if (m_pMe == NULL) return FALSE;

	if(m_pMe->IsCharAttack())
	{
		TRACE("공격중이다.\n");
		return TRUE;//공격중에는 다시 공격을 못하게 막는다.
	}

	CRect MyRect;
	CRect DestRect;
	CPoint ptMyPos;
	CPoint ptDstPos;

	// 이동을 멈추게 하기위해서.
//	m_pMe->SetMotion(CUser::UM_Stop);
	int X = 96;
	int Y = 48;		
	switch(type)
	{
	case 0:// 일반 어택
		{
			//////////
			if (m_iCurFocusUidInOthers!=UM_UID_INVALID) //맞을 대상이 존재
			{
				CUser *pDest = GetUserWithUid(m_iCurFocusUidInOthers);
				int distance = 2;
				DWORD attack_time = GetTickCount();
				ASSERT(m_iCurFocusUidInOthers!=m_iMyUserId);

				if(m_pMe->m_PkRecive_Attack_Flag == 1 && ((attack_time-m_pMe->m_PkRecive_Attack_Time) < 3000))
				{
					TRACE("전에 보낸 공격 패킷의 대답을 못받았다.\n");
					return TRUE;// 취소하게끔 한다..넘어가지도 않게.
				}

				if(pDest->IsCharDie()) return FALSE;//상대방이 죽어 있으면 취소.

				if(m_pMe->m_MyInvData[6].m_vType == TYPE_BOW) distance = 6;
				MyRect = m_pMe->GetCharRect();
				DestRect = pDest->GetCharRect();

				ptMyPos = m_pMe->GetCellPosition();
				ptDstPos = pDest->GetCellPosition();

				//활을 든사람은 다르게 처리...
				MyRect.OffsetRect(ptMyPos.x*X, ptMyPos.y*Y);
				DestRect.OffsetRect(ptDstPos.x*X, ptDstPos.y*Y);
			
				/////////
				if(m_pMe->m_BattleMode == 0)
				{
					if(m_iCurFocusUidInOthers >= 10000)
					{
						if((abs(ptMyPos.x - ptDstPos.x)+abs(ptMyPos.y - ptDstPos.y)) > 4)
						{
							TRACE("적과 조금 떨어져 있어서 이동을 한다.\n");
							return FALSE;// 먼거리에 있다면 사정거리에 없다면. 
						}
						BYTE buf[20];
						int offset = 0;
						BYTE Eventtype = 0x02; // Meet NPC
						CPacketParser::SetByte(buf, offset, PKT_CLIENTEVENT);
						CPacketParser::SetByte(buf, offset, Eventtype);
						CPacketParser::SetInt(buf, offset, m_iCurFocusUidInOthers);
						CChildView::SocketMng.Send(offset, buf);
						g_vEventPos = CChildView::DirectXMng.m_pMouseInfo->m_posMouse;
					}
					else if(pDest->m_BattleMode == 0)
					{
						if((abs(ptMyPos.x - ptDstPos.x)+abs(ptMyPos.y - ptDstPos.y)) > distance)
						{
							TRACE("적과 조금 떨어져 있어서 이동을 한다.\n");
							return FALSE;// 먼거리에 있다면 사정거리에 없다면. 
						}

						Dialog_Exchange.AskTrade(pDest->m_strUserId, m_iCurFocusUidInOthers);
					}
					else
					{
						if((abs(ptMyPos.x - ptDstPos.x)+abs(ptMyPos.y - ptDstPos.y)) > distance)
						{
							TRACE("적과 조금 떨어져 있어서 이동을 한다.\n");
							return FALSE;// 먼거리에 있다면 사정거리에 없다면. 
						}
					}
				}
				else
				{
					if(m_iCurFocusUidInOthers >= 10000 && pDest->m_bDeadEnable == 0)
					{
						if((abs(ptMyPos.x - ptDstPos.x)+abs(ptMyPos.y - ptDstPos.y)) > 3)
						{
							TRACE("적과 조금 떨어져 있어서 이동을 한다.\n");
							return FALSE;// 먼거리에 있다면 사정거리에 없다면. 
						}
						BYTE buf[20];
						int offset = 0;
						BYTE Eventtype = 0x02; // Meet NPC
						CPacketParser::SetByte(buf, offset, PKT_CLIENTEVENT);
						CPacketParser::SetByte(buf, offset, Eventtype);
						CPacketParser::SetInt(buf, offset, m_iCurFocusUidInOthers);
						CChildView::SocketMng.Send(offset, buf);
						g_vEventPos = CChildView::DirectXMng.m_pMouseInfo->m_posMouse;
					}
					else if((m_pMe->m_BattleMode == 1 && m_iCurFocusUidInOthers >= 10000) || m_pMe->m_BattleMode == 2)
					{
						if((abs(ptMyPos.x - ptDstPos.x)+abs(ptMyPos.y - ptDstPos.y)) > distance)
						{
							TRACE("적과 조금 떨어져 있어서 이동을 한다.\n");
							return FALSE;// 먼거리에 있다면 사정거리에 없다면. 
						}

						BYTE buf[20];
						int offset = 0;
						CPacketParser::SetByte(buf, offset, PKT_ATTACK);
						CPacketParser::SetInt(buf, offset, m_iCurFocusUidInOthers);
						CChildView::SocketMng.Send(offset, buf);
						m_pMe->m_PkRecive_Attack_Flag = 1;
						m_pMe->m_PkRecive_Attack_Time = attack_time;
					}
					else
					{
						if((abs(ptMyPos.x - ptDstPos.x)+abs(ptMyPos.y - ptDstPos.y)) > distance)
						{
							TRACE("적과 조금 떨어져 있어서 이동을 한다.\n");
							return FALSE;// 먼거리에 있다면 사정거리에 없다면. 
						}
					}
				}

				// 이동을 멈추게 하기위해서.
				m_pMe->SetMotion(CUser::UM_Stop);
				return TRUE;
			}
		}
		break;
	default:
		break;
	}
	return FALSE;
}

void CUserManager::Set3DMagic(int uid, TCHAR* strName, int nMagicType, int nHeight, int nWidth)
{
	CUser *pDstUser = GetUserWithUid(uid);
	if (pDstUser)
	{
		pDstUser->Set3DMagic(strName, nMagicType, nHeight);
	}
	else TRACE("마법을 설정할 대상 캐릭터없음 : uid-> %d\n", uid);

}

void CUserManager::Set3DMagicEx(int uid, int index, int nMagicType, int nHeight, int nWidth)
{
	CUser *pDstUser = GetUserWithUid(uid);
	if (pDstUser)
	{
		pDstUser->Set3DMagicEx(index, nMagicType, nHeight);
	}
	else TRACE("마법을 설정할 대상 캐릭터없음 : uid-> %d\n", uid);

}

void CUserManager::SetMagic(int uid, int nIndex, int nMagicType, int height)
{
	CUser *pDstUser = GetUserWithUid(uid);
	if (pDstUser)
	{
		pDstUser->SetMagic(nIndex, nMagicType, height);
	}
	else TRACE("마법을 설정할 대상 캐릭터없음 : uid-> %d\n", uid);
}

int CUserManager::SetAimedMagic(int nSorUid, int nDstUid, int nMagicIndex, int nMagicType)
{
	CUser *pSorUser = GetUserWithUid(nSorUid);
	CUser *pDstUser = GetUserWithUid(nDstUid);

	int nDir = 0;
	if (pSorUser && pDstUser)
	{
		CPoint ptSor = pSorUser->GetPosition();
		CPoint ptDst = pDstUser->GetPosition();

		//몸의 중앙에서 발사
		ptSor.Offset(0, -pSorUser->GetCharRect().Height()/2);
		ptDst.Offset(0, -pDstUser->GetCharRect().Height()/2); 

		nDir = SetAimedMagic(ptSor, ptDst, nMagicIndex, nMagicType, nDstUid);
		pSorUser->SetDirection(nDir);
	}
	return nDir;
}

//좌표에서 좌표로 마법을 날림
int CUserManager::SetAimedMagic(CPoint ptSor, CPoint ptDst, int nMagicIndex, int nMagicType , int nDesUid,int nSeries)
{
	CMagic *pMagic = new CMagic;
	int nDir = 0;
	if (pMagic->SetFlyMagicRes(nMagicIndex, ptSor, ptDst, nDir, nSeries))
	{
		pMagic->SetMagicType(nMagicType);
		pMagic->SetMagicAttackDesUid( nDesUid );//dspring_0722
		m_lstMagic.AddTail(pMagic);
	}
	else delete pMagic;
	return nDir;
}

// 화면상에 나타나게 하는 마법..영향을 주면서 앞으로 나가는 마법.
int CUserManager::SetViewMagic(int iSorUid, int nMagicIndex, int nMagicType, int iDir, int iCount)
{	
	CMagic *pMagic = new CMagic;
	//int nDir = 0;
	CUser *pSorUser = GetUserWithUid(iSorUid);
	
	if(!pSorUser) return 0;

//	m_nsUid = iSorUid;// 이 마법을 누가 사용하는지를 보여준다.
//	m_nDir = iDir;// 방향성
//	pMagic->m_nCount = iCount;// 영역 범위..


	CPoint ptSor = pSorUser->GetPosition();
	CPoint tmp = CChildView::MapManager.GetMapStartPos();// 배경위치 가져오기..임시 배경이 바뀐다면..

	switch(iDir)
	{
	case 6:
		ptSor.x +=30*iCount+50;
		break;
	case 2:
		ptSor.x -=30*iCount+80;
		break;
	case 4:
		ptSor.y -=30*iCount+50;
		break;
	case 0:
		ptSor.y +=30*iCount+50;
		break;
	case 5:
		ptSor.x +=30*iCount+50;
		ptSor.y -=30*iCount+50;
		break;
	case 7:
		ptSor.x +=30*iCount+50;
		ptSor.y +=30*iCount+50;
		break;
	case 1:
		ptSor.x -=30*iCount+80;
		ptSor.y +=30*iCount+50;
		break;
	case 3:
		ptSor.x -=30*iCount+80;
		ptSor.y -=30*iCount+50;
		break;
	}

	if (pMagic->SetMagicRes(nMagicIndex))
	{
		pMagic->m_Pos = ptSor;// - tmp;// 실제 맵상의 위치를 찍는다.
		pMagic->m_nCount = iCount;// 영역 범위..

		pMagic->SetMagicType(nMagicType);
		m_lstMagic.AddTail(pMagic);
	}
	else delete pMagic;	
	return 0;
}

void CUserManager::RailAttackMagic(int bSuccess, int iSorUid, int bDir, int bLen, int nMagicType)
{
	// 이곳에서 마법을 보여 줘야한다..음..영역만큼..방향..
	// 현재 위치는 내 위치에서 뽑아 내구..길이만큼..영역으루..
	int nMagicIndex = nMagicType;// 임시
	int Type = 2;

	/*
	m_Rail_Magic
	iSorUid
	iDir
	bLen
    */
	if(bSuccess == 1)
	{
		for(int i=0; i< bLen*2;i++)
			SetViewMagic(iSorUid, nMagicIndex, Type, bDir, i);
	}
}

//깔리는 마법
void CUserManager::DrawDownMagics(CDDSurface *pSurface)
{
	POSITION pos = m_lstMagic.GetHeadPosition();
	POSITION prev_pos;
	CMagic *pMagic;
	while (pos)
	{
		prev_pos = pos;
		pMagic = m_lstMagic.GetNext(pos);
		if (pMagic && pMagic->IsDownMagic())
		{
			if (pMagic->DrawFlyingMagic(pSurface)) 
			{
				SAFE_DELETE(pMagic);
				m_lstMagic.RemoveAt(prev_pos);
			}
		}
//		else if (pMagic && (pMagic->GetMagicType()==MAGIC_TYPE_POSION))
//		{
//		}
	}
}
//날으는 마법
void CUserManager::DrawUpMagics(CDDSurface *pSurface)
{
	POSITION pos = m_lstMagic.GetHeadPosition();
	POSITION prev_pos;
	CMagic *pMagic;
	while (pos)
	{
		prev_pos = pos;
		pMagic = m_lstMagic.GetNext(pos);
		int del = 0;
		if (pMagic && (pMagic->GetMagicType()==MAGIC_TYPE_UP))
		{
			if (pMagic->DrawFlyingMagic(pSurface)) 
			{
				//SetMagic(m_iMyId, 4, MAGIC_TYPE_UP);// 도착한뒤 폭파 효과..음냐..
				SAFE_DELETE(pMagic);
				m_lstMagic.RemoveAt(prev_pos);
			}
		}
		else if (pMagic && (pMagic->GetMagicType()==MAGIC_TYPE_POSION))
		{
			int del = 0;
			if(pMagic->m_nCount <= 0)
			{
				if(pMagic->DrawMagic(pSurface))
				{
					SAFE_DELETE(pMagic);
					m_lstMagic.RemoveAt(prev_pos);
				}
			}
			else
				pMagic->m_nCount --;

				
		}
	}
}
//dspring_0802_3dxx_start
void CUserManager::DrawUp3DMagics()
{
	POSITION pos = m_lstMagic.GetHeadPosition();
	POSITION prev_pos;
	CMagic *pMagic;
	while (pos)
	{
		prev_pos = pos;
		pMagic = m_lstMagic.GetNext(pos);
		int del = 0;
		if (pMagic && (pMagic->GetMagicType()==MAGIC_TYPE_UP))
		{
			if (pMagic->DrawFlying3DMagic()) 
			{
				//SetMagic(m_iMyId, 4, MAGIC_TYPE_UP);// 도착한뒤 폭파 효과..음냐..
				SAFE_DELETE(pMagic);
				m_lstMagic.RemoveAt(prev_pos);
			}
		}
		else if (pMagic && (pMagic->GetMagicType()==MAGIC_TYPE_POSION))
		{
			int del = 0;
			if(pMagic->m_nCount <= 0)
			{
//				if(pMagic->DrawMagic(pSurface))
//				{
//					SAFE_DELETE(pMagic);
//					m_lstMagic.RemoveAt(prev_pos);
//				}
			}
			else
				pMagic->m_nCount --;

		}
	}
}

//dspring_0802_3dxx_end

void CUserManager::Set_MyInv_Data_All(int uid, PBYTE bypData)
{
	CUser *pDstUser = GetUserWithUid(uid);
	if (!pDstUser) return;
	pDstUser->SetInvAll(bypData);// 배열로 전체가 세팅된다.
}

BOOL CUserManager::SendChang_Item(short fromindex, short toindex)// 장착 아이템 을 바꿀때 보내는 패킷을 보내는곳..
{
	BYTE buf[10];
	int offset = 0;
	CPacketParser::SetByte(buf, offset, PKT_CHANGE_ITEM_INDEX); //최종점 요청
	
	CPacketParser::SetShort(buf, offset, fromindex);
	CPacketParser::SetShort(buf, offset, toindex);
	CChildView::SocketMng.Send(offset, buf);
	return TRUE;

}

// 자신의 장착 인벤토리에서 가방 인벤토리로 이동또는 반대일때 동작..
void CUserManager::ChangItem(short fromindex, short toindex)
{
	if(m_pMe == NULL) return;

	m_pMe->Chang_Item(fromindex, toindex);
}

// 따른 캐릭터 장착 아이템을 변화 시킨다...물론 나도 변화가능..
void CUserManager::ChangItem(int uid, short index, short picnum)
{
	CUser *pDstUser = GetUserWithUid(uid);
	if (!pDstUser) return;

	if(index < 0) return;
	pDstUser->Change_View_item(index, picnum);
}


int CUserManager::GetMapData(CPoint pt)
{
	return m_pPathFind->GetMapData(pt);
}

int CUserManager::GetHouseMapData(CPoint pt)
{
	return m_pPathFind->GetHouseMapData(pt);
}
void CUserManager::SetUserDead()
{
	int nArrSize = m_arrDeadUserInfo.GetSize();
	if( nArrSize == 0 ) return;
	DWORD dwNowTime = GetTickCount();
	DEAD_INFO stDeadUser;
	for( int i = 0 ; i < nArrSize ; i++ )
	{
		stDeadUser = m_arrDeadUserInfo.GetAt(i);
//		if( dwNowTime > stDeadUser.m_dwDeadTime + 1000 )
//		{
			UserDead( stDeadUser.m_nDeadUserId, stDeadUser.m_ptDeadPos.x, stDeadUser.m_ptDeadPos.y);
			m_arrDeadUserInfo.RemoveAt(i);
			i--; nArrSize--;
//		}

	}
}

void CUserManager::PreUserDead(int uid, int x, int y)
{
	DEAD_INFO stDeadUser;
	CUser *pDstUser;
	DWORD dwDeadTime;
	
	pDstUser = GetUserWithUid(uid);
	if(!pDstUser) return;
	dwDeadTime = GetTickCount();

	stDeadUser.m_dwDeadTime = dwDeadTime;
	stDeadUser.m_ptDeadPos = CPoint( x, y);
	stDeadUser.m_nDeadUserId = uid;
	m_arrDeadUserInfo.Add( stDeadUser );

	if(!pDstUser) return;
	if( pDstUser->m_BloodType != 0 )//0은 죽을때 피가 없다.
	{
		CRect rSurf(0,0,800,600);
		CPoint ptRenderingPos = pDstUser->GetPosition()-CChildView::MapManager.GetMapStartPos();
		if(IsInRange(rSurf, pDstUser, ptRenderingPos)) // 화면에 나타날 영역인가 확인
			m_Blood->Add( uid,(int)pDstUser->m_BloodType,pDstUser->GetPosition() );
	}

	_SET_MAGIC_INFO* stMagicInfo;
	int nArrSize = m_arrSetMagicInfo.GetSize();
	for( int i = 0 ; i < nArrSize; i++ )
	{
		stMagicInfo = m_arrSetMagicInfo.GetAt(i);
		if( stMagicInfo != NULL )
		{
//			if( stMagicInfo->m_nSorUid == uid && stMagicInfo->m_bReady != TRUE && stMagicInfo->m_stMagicTable.m_nStTime )
			if( stMagicInfo->m_nSorUid == uid )
			{
				SAFE_DELETE( stMagicInfo->m_Comet );
				SAFE_DELETE( stMagicInfo );
				m_arrSetMagicInfo.RemoveAt(i);
				nArrSize--; i--;
			}
		}
	}


	pDstUser->m_arrACM.RemoveAll(); 
	pDstUser->m_bDeadFlag = TRUE;
	pDstUser->m_nPoisonKind = 0;

	if(pDstUser->m_Me == 1)
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
		if(g_vMainMode == 1) g_pChildView->SetMainMode(0);
		Dialog_Main.HideWindow();
		Dialog_DeadMain.ShowWindow();
		Dialog_DeadMain.StartCountDown();
	}
	else
	{
		m_pPathFind->SetMovable(CPoint(x,y),true);
		pDstUser->SetMotion(CUser::UM_Stop);
	}
}

void CUserManager::UserDead(int uid, int x, int y)
{
	CUser *pDstUser;

	if(m_mapUser.Lookup(uid, pDstUser) == FALSE) return;
	if(pDstUser->m_bDeadFlag == FALSE) return;

	pDstUser->SetMotion(CUser::UM_Die);
//	pDstUser->m_iSpriteIndex = 0;

	//CChildView::DirectXMng.m_pDirectSound->SoundPlay("sound\\Mondead.WAV", FALSE);//죽을때
	int key;
	char *value;

	CPoint ptSoundPos = pDstUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
	pDstUser->m_nPoisonKind = 0;

	if(pDstUser->m_nType < 10) // 유저
	{
		
		if(pDstUser->m_Gender == 1) // 남자
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
		key = SOUND_MONSTER+(pDstUser->m_nType-10)*4+3;
		if(g_SoundName.Lookup(key, value))
		{
			CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
		}
	}
}

CPoint CUserManager::GetMyCellPos()
{
	if(m_pMe) return m_pMe->GetCellPosition();
	return CPoint (10, 7);
}

void CUserManager::ActionMotion(int uid, int type, int delay_time)
{// uid 누가? type 무엇을 delay_time 얼마나뒤에
	DWORD dwTick = GetTickCount();
	ACTION_INFO *action_info;

	action_info = new ACTION_INFO;

	action_info->m_ActionId = uid;
	action_info->m_ActionType = type;
	action_info->m_ActionTickTime = dwTick+delay_time;
	m_arrActionInfo.Add(action_info);
}

//#define BEATEDMOTION         1
//#define BEATEDSOUND          2
//#define BEATEDEFFECT         3
void CUserManager::ActionProcess()
{// 데이타는 위에서 입력 받고 이 함수는 돌면서 시간이 되면 동작시켜준다.
	int action_count;
	ACTION_INFO *action_info;

	action_count = m_arrActionInfo.GetSize();

	for(int i = action_count-1; i >= 0; i--)
	{
		action_info = m_arrActionInfo[i];
		if(action_info->m_ActionType != 0)
		{
			DWORD dwTick = GetTickCount();

			if(action_info->m_ActionTickTime < dwTick)// 예약 시간이 지났으면..
			{
				// 유저를 동작시킬때..
				CUser *pDstUser = GetUserWithUid(action_info->m_ActionId);
				if (pDstUser)
				{
					CPoint ptSoundPos = pDstUser->GetPosition() - CChildView::MapManager.GetMapStartPos();
					int key;
					char *value;
					// 임시 일단 모션만 맞처보자..
					switch(action_info->m_ActionType)
					{
					case BEATEDMOTION:
						if(pDstUser->m_bDying == FALSE)
						{
							pDstUser->SetMotion(CUser::UM_Beated);
//							pDstUser->m_iSpriteIndex = 0;
						}
//						pDstUser->BeatedDir(-1);//-1은 방향을 바꾸지 않게 한다. 전에 바꿔놓아야겠다.
//						CChildView::DirectXMng.m_pDirectSound->SoundPlay("sound\\hit02.WAV", FALSE);// 맞는소리
//						CChildView::DirectXMng.m_pDirectSound->SoundPlay("sound\\Hitted_1.wav", FALSE);// 괴물 비명소리// 종류에 따라 틀려야함.
						/////////////////
						if(pDstUser->m_nType < 10)			// 유저
						{
							if(pDstUser->m_Gender == 1)		// 남자
							{
								if(g_SoundName.Lookup(SOUND_SCREAM1, value))
								{
									CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_SCREAM1, FALSE, ptSoundPos);
								}
							}
							else							// 여자
							{
								if(g_SoundName.Lookup(SOUND_SCREAM2, value))
								{
									CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_SCREAM2, FALSE, ptSoundPos);
								}
							}
						}
						else	// NPC
						{
							key = SOUND_MONSTER+(pDstUser->m_nType-10)*4+2;
							if(g_SoundName.Lookup(key, value))
							{
								CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
							}
						}
						break;
					case BEATEDSOUND://2
						break;
					case BEATEDEFFECT://3
						break;
					case ATTACKMOTION://4
						break;
					case ATTACKSOUND://5
						if(pDstUser->m_bDying == FALSE)
						{
							pDstUser->SetMotion(CUser::UM_Attack);
//							pDstUser->m_iSpriteIndex = 0;
						}
						if(pDstUser->m_nType < 10)
						{
							switch(pDstUser->m_Arm_type)
							{
							case 0:
								if(g_SoundName.Lookup(SOUND_ATTACK1, value))
								{
									CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_ATTACK1, FALSE, ptSoundPos);
								}
								if(g_SoundName.Lookup(SOUND_HIT1, value))
								{
									CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_HIT1, FALSE, ptSoundPos);
								}
							case 1:
							case 2:
							case 3:
							case 5:
							case 6:
								if(g_SoundName.Lookup(SOUND_ATTACK2, value))
								{
									CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_ATTACK2, FALSE, ptSoundPos);
								}
								if(g_SoundName.Lookup(SOUND_HIT2, value))
								{
									CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_HIT2, FALSE, ptSoundPos);
								}
								break;
							case 4:
								if(g_SoundName.Lookup(SOUND_ATTACK3, value))
								{
									CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_ATTACK3, FALSE, ptSoundPos);
								}
								break;
							default:
								break;
							}
						}
						else
						{
							key = SOUND_MONSTER+(pDstUser->m_nType-10)*4;
							if(g_SoundName.Lookup(key, value))
							{
								CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
							}
							break;
						}
						break;
					case ATTACKEFFECT://6
						break;
					case ATTACKFAIL://7
						if(pDstUser->m_nType < 10)
						{
							switch(pDstUser->m_Arm_type)
							{
							case 0:
								if(g_SoundName.Lookup(SOUND_ATTACK1, value))
								{
									CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_ATTACK1, FALSE, ptSoundPos);
								}
							case 1:
							case 2:
							case 3:
							case 5:
							case 6:
								if(g_SoundName.Lookup(SOUND_ATTACK2, value))
								{
									CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_ATTACK2, FALSE, ptSoundPos);
								}
								break;
							case 4:
								if(g_SoundName.Lookup(SOUND_ATTACK3, value))
								{
									CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, SOUND_ATTACK3, FALSE, ptSoundPos);
								}
								break;
							default:
								break;
							}
						}
						else
						{
							key = SOUND_MONSTER+(pDstUser->m_nType-10)*4;
							if(g_SoundName.Lookup(key, value))
							{
								CChildView::DirectXMng.m_pDirectSound->SoundPlay(value, key, FALSE, ptSoundPos);
							}
							break;
						}
						break;
					default:
						break;
					}
				}
				m_arrActionInfo.RemoveAt(i);
				delete action_info;
			}
		}
	}
}

void CUserManager::HpMpRecovery(PBYTE bypData)
{
	int offset = 0;
	int uid;
	short Hp, Mp;
	CUser *pMe = GetMe();

	uid = CPacketParser::GetInt(bypData, offset);
	Hp = CPacketParser::GetShort(bypData, offset);
	Mp = CPacketParser::GetShort(bypData, offset);

	if(uid == pMe->m_iUserId)
	{
		TRACE1("***Valid User ID(%d)\n", uid);
		pMe->m_Cur_Hp = Hp;
		if(pMe->m_Cur_Hp > pMe->m_Max_Hp) pMe->m_Cur_Hp = pMe->m_Max_Hp;
		else if(pMe->m_Cur_Hp < 0) pMe->m_Cur_Hp = 0;
		pMe->m_Cur_Mp = Mp;
		if(pMe->m_Cur_Mp > pMe->m_Max_Mp) pMe->m_Cur_Mp = pMe->m_Max_Mp;
		else if(pMe->m_Cur_Mp < 0) pMe->m_Cur_Mp = 0;
	}
	else
	{
		TRACE1("***Invalid User ID(%d)\n", uid);
	}
}

CUser *CUserManager::GetMe()
{
	CUser *pUser;

	if(m_iMyUserId == UM_UID_INVALID) return NULL;

	if(m_mapUser.Lookup(m_iMyUserId, pUser)) return pUser;
	else return NULL;
}


/*
void CUserManager::InHouseRenderUsers(CDDSurface *pSurface, CPoint ptMapOffset)
{
	CRect rSurf;
	CPoint ptRenderingPos;

	if(m_pMe == NULL) return;

	pSurface->GetRect(rSurf);

	ApplyMagicEx();
	SetUserDead();
	// 내가 집안에 있는지 밖에 있는지를 알아서 안에 있으면 안에 있는 사람만 밖에 있으면 밖에있는 사람만.보여준다.
	CPoint p = GetMyCellPos();
	int MyIn = CChildView::MapManager.IsHouse(p);
	m_pMe->m_InHouse = MyIn;

	int validCount = 0, validDeadCount = 0;
	int My_validcount = 0;
	POSITION pos;
	int key;
	CUser *pUser;
	int i;
	CMainFrame *main_frame;
	CPoint MouseCellpos = pUser->PixelToCellUnit(pUser->m_bpDirectXMng->m_pMouseInfo->m_posMouse+ptMapOffset);

	main_frame = __GetMainWnd();
	m_iCurFocusUidInOthers = UM_UID_INVALID;

	pos = m_mapUser.GetStartPosition();
	while(pos)
	{
		m_mapUser.GetNextAssoc(pos, key, pUser);
		ptRenderingPos = pUser->GetPosition()-ptMapOffset;
		if(IsInRange(rSurf, pUser, ptRenderingPos)) // 화면에 나타날 영역인가 확인
		{
			CPoint UserCellpos = pUser->GetCellPosition();
			int UserIn = CChildView::MapManager.IsHouse(UserCellpos);

			// 나랑 같은 위치에 있는 사람만 나타나게 하기위해..집안 집 밖...
			if(MyIn == UserIn)
			{
				if(pUser->m_bDead == TRUE)
				{
					m_arrDrawDeadUser[validDeadCount++] = pUser;	// 죽은 사람 리스트
				}
				else
				{
					if(pUser->m_bDeadFlag != TRUE && pUser->m_Me != 1)
					{
//						if(UserCellpos == MouseCellpos)
						if(m_bCursorInRange == TRUE)
						{
							pUser->SetFocus(TRUE);	//커서가 캐릭의 Rect상에 있으면 Focus를 줌
							m_iCurFocusUidInOthers = pUser->GetUserId();
						}
					}
					m_arrDrawUser[validCount++] = pUser;	//그릴 사람 목록에 추가 클래스를 집어 넣는군..쩝
				}
			}
		}
		else
		{
			if(ptRenderingPos.x < -1536 || ptRenderingPos.x > 2352 || ptRenderingPos.y < -672 || ptRenderingPos.y > 1272)
			{
				m_mapUser.RemoveKey(key);
				delete pUser;
				TRACE1("%d번 유저 지움\n", key);
			}
		}
	}

	DrawDownMagics(pSurface);	//아래 깔리는 마법 그리기(날아가는 마법)

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if (validCount)
	{
		DrawUserSortY(validCount);	//Y축에 대해 Sorting함

		//Focus 상태 한명 선택 - 현재 위치로 포커스를 정하기 때문에 겹칠 수가 없다.
		BOOL bFocusFind = FALSE;
		int uid;
		for (i=validCount-1; i>=0; i--)	//Focus를 한사람만 남기고 다 제거(그중에서 나포함, 나제외에 대해 생성)
		{
			if(m_arrDrawUser[i]->GetFocus()) 
			{				
				uid = m_arrDrawUser[i]->GetUserId();
				if (uid!=m_iMyUserId)
				{
					bFocusFind = TRUE;
					m_iCurFocusUidInOthers = uid;
					i--;
					break;
				}
				else
				{
					m_arrDrawUser[i]->SetFocus(FALSE);
				}

			}
		}
		for(; i >= 0; i--)
		{
			if (m_arrDrawUser[i]->GetFocus()) m_arrDrawUser[i]->SetFocus(FALSE);
		}

		CPoint org_pos, offset_pos, scroll_pos, x_range;
		org_pos.x = (ptMapOffset.x >> 5); // /32
		offset_pos.x = 32 - (ptMapOffset.x & 0x1f); // %32
		org_pos.y = (ptMapOffset.y >> 5); // /32
		offset_pos.y = 32 - (ptMapOffset.y & 0x1f); // %32
		int limit_y;
		int half_size_x;

		half_size_x = (int)(g_vDispCellSize.cx / 2.0 + 0.5);

		if(org_pos.x-half_size_x < 0) x_range.x = 0;
		else x_range.x = org_pos.x - half_size_x;
		if(org_pos.x+g_vDispCellSize.cx+half_size_x >= CChildView::MapObj.m_Header.nWidth) x_range.y = CChildView::MapObj.m_Header.nWidth;
		else x_range.y = org_pos.x + g_vDispCellSize.cx + half_size_x;

		CPoint ptHouseIn;
		limit_y = g_vDispPixSize.cy * 3 + ptMapOffset.y;
		limit_y >>= 5;
		ptHouseIn = main_frame->m_wndView.DrawMyInObj(limit_y, org_pos, offset_pos, x_range,TRUE);
		if( ptHouseIn.x != -1 && ptHouseIn.y != -1 )
		{
			main_frame->m_wndView.DrawHouseIn(ptHouseIn.x, ptHouseIn.y, org_pos, offset_pos);
		}

		m_Blood->DrawBloodStain( pSurface,CChildView::MapManager.GetMapStartPos());

		ActionProcess();// 액션 제어하기...

		for(i = 0; i < validDeadCount; i++)
		{
			pUser = m_arrDrawDeadUser[i];
			ptRenderingPos = pUser->GetPosition()-ptMapOffset;

			pUser->RenderChar(pSurface, ptRenderingPos);
			m_Blood->Draw( pSurface,CChildView::MapManager.GetMapStartPos(),pUser->GetUserId() );
			//pUser->View_BubbleChat(ptMapOffset-CPoint(0,50));//this_chat
		}

		Item_Field.Draw(CChildView::DirectXMng.m_pDrawSurface, ptMapOffset, MyIn);
		if(CChildView::bDlgIn)
		{
			CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
		}
		else
		{
			if(bFocusFind == TRUE)
			{
				CUser *me = GetMe();
				CUser *pDest = GetUserWithUid(m_iCurFocusUidInOthers);

				if(m_iCurFocusUidInOthers < 10000)
				{
					if(me->m_BattleMode == 2) CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(ATTACK_CURSOR);
					else CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
				}
				else
				{
					if(me->m_BattleMode && pDest->m_bDeadEnable == 1) CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(ATTACK_CURSOR);
					else CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
				}
			}
			else
			{
				if(Item_Field.IsExistItem(MouseCellpos, MyIn,ptMapOffset,pSurface) == TRUE) CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(PICKUP_CURSOR);
				else CChildView::DirectXMng.m_pDxCursor->SetCursorIndex(NORMAL_CURSOR);
			}
		}

		for (i=0; i<validCount; i++)	//그릴 사람의 목록만 가지고 그리기
		{
			pUser = m_arrDrawUser[i];
			ptRenderingPos = pUser->GetPosition();
			limit_y = ptRenderingPos.y;
			ptRenderingPos -= ptMapOffset;

			limit_y >>= 5;

			pUser->RenderChar(pSurface, ptRenderingPos);
			m_Blood->Draw( pSurface,CChildView::MapManager.GetMapStartPos(),pUser->GetUserId() );
			pUser->View_BubbleChat(ptMapOffset);
		}

//		if( ptHouseIn.x != -1 && ptHouseIn.y != -1  )
//		{
//			int trans_degree = main_frame->m_wndView.m_house.TransDegree();
//			if( trans_degree != 0 && trans_degree != 32 )
//			{
//				main_frame->m_wndView.DrawTransOutHouse(ptHouseIn.x, ptHouseIn.y, org_pos, offset_pos,trans_degree,32-trans_degree);
//			}
//		}

		// 남은 배경 스프라이트들을 다 찍는다.
		limit_y = g_vDispPixSize.cy * 3 + ptMapOffset.y;
		limit_y >>= 5;
		main_frame->m_wndView.DrawMyInObj(limit_y, org_pos, offset_pos, x_range);
		// 바닥에 떨어진 아이템 출력
		Item_Field.DrawLabel(CChildView::DirectXMng.m_pDrawSurface, MouseCellpos, MyIn);
	}


	DrawUpMagics(pSurface);	//위에 덮이는 마법 그리기(날아가는 마법)//this_fly
}
*/