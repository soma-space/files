// PacketParser.cpp: implementation of the CPacketParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "PacketParser.h"
#include "ChildView.h"
#include "DManage/BB_Direct.h"
#include "DManage/DirectSound.h"
#include "DManage/Direct3d.h"
#include "DManage/Night.h"
#include "MapManager/SM_MapManager.h"
#include "User.h"
#include "SMProtocol.h"
#include "Msg.h"
#include "MainFrm.h"
#include "BBDX_ChatOuter.h"
#include "Dialog_AccountLogin.h"
#include "Dialog_BoardCtrl.h"
#include "Dialog_Shop.h"
#include "Dialog_Inv.h"
#include "Dialog_Storage.h"
#include "Dialog_MakeItem.h"
#include "Dialog_MemoCtrl.h"
#include "Dialog_Main.h"
#include "Dialog_Exchange.h"
#include "Dialog_AuctionSell.h"
#include "Dialog_AuctionBuy.h"
#include "Dialog_Chat.h"
#include "Dialog_Repair.h"
#include "BuildHouseManager.h"
#include "HousePlaceManager.h"
#include "LightSource.h"

#include "Dialog_EventMessage.h"
#include "Dialog_EventMessage1.h"
#include "Dialog_EventMessage2.h"
#include "Dialog_GuildMenu1.h"
#include "Dialog_GuildMenu2.h"
#include "Dialog_GuildMenu3.h"
#include "Dialog_GuildName.h"
#include "Dialog_GuildApply.h"
#include "Dialog_GuildJoin.h"
#include "Dialog_Guild.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CChildView			*g_pChildView;
extern CDialog_AccountLogin		*GDialog_AccountLogin; //우석.
//extern CDialog_Login            *GDialog_Login; // 우석.
extern CDialog_BoardCtrl		Dialog_BoardCtrl;
extern CDialog_Shop				Dialog_Shop;
extern CDialog_Inv				Dialog_Inv;
extern CDialog_Storage			Dialog_Storage;
extern CDialog_MakeItem			Dialog_MakeItem;
extern CDialog_MemoCtrl			Dialog_MemoCtrl;
extern CDialog_Main				Dialog_Main;
extern CDialog_Exchange			Dialog_Exchange;
extern CDialog_AuctionSell		Dialog_AuctionSell;
extern CDialog_AuctionBuy		Dialog_AuctionBuy;
extern CDialog_Chat				Dialog_Chat;
extern CDialog_Repair			Dialog_Repair;

extern CBuildHouseManager		BuildHouseManager;
extern CHousePlaceManager		HousePlaceManager;

extern CDialog_EventMessage     Dialog_EventMessage;
extern CDialog_EventMessage1     Dialog_EventMessage1;
extern CDialog_EventMessage2     Dialog_EventMessage2;

extern CDialog_GuildMenu1		Dialog_GuildMenu1;
extern CDialog_GuildMenu2		Dialog_GuildMenu2;
extern CDialog_GuildMenu3		Dialog_GuildMenu3;
extern CDialog_GuildName		Dialog_GuildName;
extern CDialog_GuildApply		Dialog_GuildApply;
extern CDialog_GuildJoin		Dialog_GuildJoin;
extern CDialog_Guild			Dialog_Guild;

extern int								g_vMapNumber, g_vOldMapNumber;

void RcvGuildManList(PBYTE bypData);
void RcvApplicationList(PBYTE bypData);
void RcvGuildList(PBYTE bypData);
void RcvCancelApplyGuild(PBYTE bypData);

CArray<int, int> g_arrRequestId;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPacketParser::CPacketParser()
{
	m_bpConnectedView = NULL;
	m_bIsSessionLogin = TRUE;

	m_bPathFind = false;
}

CPacketParser::~CPacketParser()
{
}
//Session과 연결중일때,
BOOL CPacketParser::SessionPacketProcess(PBYTE bypData, int size)
{
	int		read_offset = 0;
	WORD	header = GetWord(bypData, read_offset);
	WORD    tmp    = GetWord(bypData, read_offset);// tmp는 사용안함...

	CString tmpString = _T("Soma");//씨리얼 번호..개인유저는 필요없다.

	BOOL	bPacketProcessed = TRUE;
	CMsg	msg;
	int     offset = 0;

	switch(header)
	{
	case SM_GAMEINFO_ACK:
		msg.ID(SM_LOGIN_REQ);
		msg << CChildView::SocketMng.m_szAcountLoginId
			<< CChildView::SocketMng.m_szPasswd
			<< tmpString << tmpString;
		CChildView::SocketMng.SendRawPacket(msg.GetSize(), (PBYTE)msg.GetBuf());
		break;
	case SM_LOGIN_ACK:
		{
			BYTE LoginResult_bOk = GetByte(bypData, read_offset);//0:정상 1: 미등록 2: 비정상 3: 이미사용중
			BYTE SerialResult_bOk = GetByte(bypData, read_offset);//0:정상 1:미등록 2: 사용안함 3:비정상 4:유효기간이지남 

			switch( LoginResult_bOk )
			{
				case 0:						
						TRACE("로긴 성공...\n");

						msg.ID(SM_GAME_REQ);
						msg << (int)0;
						msg << (BYTE)1;
						CChildView::SocketMng.SendRawPacket(msg.GetSize(), (PBYTE)msg.GetBuf());
						GDialog_AccountLogin->m_SuccessLogin = 1;// 성공... //우석.
					   break;
				case 1: 
						GDialog_AccountLogin->m_SuccessLogin = 2;
						TRACE("미등록 아이디다.");
					   break;
				case 2:
						GDialog_AccountLogin->m_SuccessLogin = 3;
						TRACE("비밀번호가 틀렸다.");
					   break;
				case 3:
						GDialog_AccountLogin->m_SuccessLogin = 4;
						TRACE("이미 사용중이다.");
					   break;
			}

/*			if(LoginResult_bOk == 0x00)// 성공
			{
				if(SerialResult_bOk == 0x00)// 성공
				{
					GDialog_AccountLogin->m_SuccessLogin = 1;// 성공... //우석.
					TRACE("로긴 성공...\n");
					msg.ID(SM_GAME_REQ);
					msg << (int)0;
					msg << (BYTE)1;
					CChildView::SocketMng.SendRawPacket(msg.GetSize(), (PBYTE)msg.GetBuf());
				}
				else if(SerialResult_bOk == 0x01)// 미등록
				{
					GDialog_AccountLogin->m_SuccessLogin = 2;
					TRACE("미등록 아이디다.\n");
				}
				else if(SerialResult_bOk == 0x02)// 비정상
				{
					GDialog_AccountLogin->m_SuccessLogin = 3;
					TRACE("사용안함 \n");
				}
				else if(SerialResult_bOk == 0x03)// 비정상
				{
					GDialog_AccountLogin->m_SuccessLogin = 4;
					TRACE("비정상이다.\n");
				}
				else if(SerialResult_bOk == 0x04)// 유효 기간이 지났다.
				{
					GDialog_AccountLogin->m_SuccessLogin = 5;
					TRACE("유효 기간이 지났다..\n");
				}
			}
			else if(LoginResult_bOk == 0x01)// 미등록
			{
				GDialog_AccountLogin->m_SuccessLogin = 6;
				TRACE("미등록 아이디다.");
			}
			else if(LoginResult_bOk == 0x02)// 비정상
			{
				GDialog_AccountLogin->m_SuccessLogin = 7;
				TRACE("비정상 아이디이다.");
			}
			else if(LoginResult_bOk == 0x03)// 이미 사용중이다.
			{
				GDialog_AccountLogin->m_SuccessLogin = 8;
				TRACE("이미 사용중이다.");
			}
*/
		break;
		}
	case SM_GAME_ACK: //실제 게임 진행
		m_bIsSessionLogin = FALSE;
		break;
	case SM_REGUSER_ACK:
		{
			BYTE NewUSERResult_bOk = GetByte(bypData, read_offset);//1:성공 0: 실패 		
//			BYTE NewUSERResult_bOk2 = GetByte(bypData, offset);//1:성공 0: 실패 		
			if(NewUSERResult_bOk == 0) TRACE("새 계정 설정에 실패\n");
			else GDialog_AccountLogin->m_SuccessNewChar = 1;// 성공... //우석.
			break;
		}
	case SM_CONNECTINFO_ACK:
		{
			ConnectInfoProcess(bypData+read_offset);
			break;
		}
	case SM_UPGRADING_ACK: //공지사항..
		m_bpConnectedView->Notice(bypData+read_offset);		
		break;
	default:
		bPacketProcessed = FALSE;
		break;
	}
	return bPacketProcessed;
}

BOOL CPacketParser::PacketProcess(PBYTE bypData, int size)
{
	if (NULL==bypData) return TRUE;

	BOOL bProcess = TRUE;
	int offset = 0;
	switch(bypData[offset++])
	{
	case PKT_ACOUNT_LOGIN :
		AcountLoginProcess(bypData+offset);
		break;
	case PKT_LOGIN://Login
		LoginProcess(bypData+offset);
		break;
	case PKT_MOVEFIRST:
	case PKT_MOVEMIDDLE:
	case PKT_MOVEEND:
		if(CChildView::UserManager.m_ReadyOK == 1)
			MoveProcess(bypData+offset, bypData[0]);
		break;
	case PKT_RUN_MOVEFIRST:
	case PKT_RUN_MOVEMIDDLE:
	case PKT_RUN_MOVEEND:
		if(CChildView::UserManager.m_ReadyOK == 1)
			MoveProcess(bypData+offset, bypData[0]);
		break;
		
	case PKT_NEW_HERO:// 새로운 캐릭터 생성
		m_bpConnectedView->NewHeroProcess(bypData+offset);
		break;
	case PKT_DEL_HERO:// 캐릭터 삭제
		m_bpConnectedView->DelHeroProcess(bypData+offset);
		break;
		// 인벤토리...쪽 패킷들...
	case PKT_CHARTER_DATA:// 이 패킷 다음에 인벤토리 올 패킷이 온다. hp mp받는곳..
		CChildView::CharacterDataProcess(bypData+offset);
		break;
	case PKT_INV_ALL:
		{
			int uid = GetInt(bypData, offset);
			CChildView::UserManager.Set_MyInv_Data_All(uid, bypData+offset);
		}
		break;
	case PKT_MAGIC_ALL:
		{
			short i;
			short count;
			short number;
			BYTE method1;
			BYTE method2;
			short start_time;
			BYTE type;
			BYTE target_method;
			BYTE len;
			CString name;
			CString explain;
			short distance;
			short use_mp;
			int	use_time;
			short damage;
			LPTSTR t_str;
			CUser *pMe = NULL;
			CMyMagicData *magic_item[4];

			count = GetShort(bypData, offset);
			pMe = CChildView::UserManager.GetMe();
			ASSERT(pMe);
			for(i = 0; i < count; i++)
			{
				number = GetShort(bypData, offset);
				method1 = GetByte(bypData, offset);
				method2 = GetByte(bypData, offset);
				start_time = GetShort(bypData, offset);
				type = GetByte(bypData, offset);
				target_method = GetByte(bypData, offset);
				len = GetByte(bypData, offset);
				t_str = name.GetBuffer(len+1);
				GetString(bypData, offset, (BYTE *)t_str, len);
				t_str[len] = 0;
				name.ReleaseBuffer();
				len = GetByte(bypData, offset);
				t_str = explain.GetBuffer(len+1);
				GetString(bypData, offset, (BYTE *)t_str, len);
				t_str[len] = 0;
				explain.ReleaseBuffer();
				distance = GetShort(bypData, offset);
				use_mp = GetShort(bypData, offset);
				use_time = GetInt(bypData, offset);
				damage = GetShort(bypData, offset);

				pMe->AddMagic(number, method1, method2, start_time, type, target_method, name, explain, distance, use_mp, use_time, damage, -1);
			}
			for(i = 0; i < 4; i++)
			{
				number = GetShort(bypData, offset);
				if(number != -1)
				{
					if(pMe->m_mapMyMagicData.Lookup(number, magic_item[i]) == FALSE)
					{
						magic_item[i] = NULL;
					}
				}
				else magic_item[i] = NULL;
			}
			Dialog_Main.LoadMagicData(magic_item);
		}
		break;
	case PKT_CHANGE_ITEM_INDEX:// 받는것과 보내는 데이타가 틀리다.
		{
			BYTE bOK = GetByte(bypData, offset);
			if(bOK == 0x01)// 성공을 하면 뒷따라 오는 데이타가 있다..
			{
				short fromindex  = GetShort(bypData, offset);
				short toindex    = GetShort(bypData, offset);	
				
				CChildView::UserManager.ChangItem(fromindex, toindex);// 데이타를 바꾼다..!
				if(Dialog_Inv.IsVisible()) Dialog_Inv.ChangeItem(fromindex, toindex);
			}
			//bOK가 1이면 성공...아이템 체인지 인증 받은것...
		}
		break;
	case PKT_ITEM_THROW:
		CChildView::ThrowItemProcess(bypData+offset);
		break;
	case PKT_ITEM_PICKUP:
		CChildView::PickUpItemProcess(bypData+offset);
		break;
	case PKT_ITEM_USE:
		CChildView::UseItemProcess(bypData+offset);
		break;
	case PKT_ITEM_FIELD_INFO:
		CChildView::FieldInfoItemProcess(bypData+offset);
		break;

	case PKT_ITEM_INFO:
		{
			int uid = GetInt(bypData, offset);
			short index = GetShort(bypData, offset);    // 아이템 장착 위치 인덱스..
			short picnum = GetShort(bypData, offset);   // 그림 번호...바뀔..
						
			CChildView::UserManager.ChangItem( uid, index, picnum);			
		}
		break;
	case PKT_USERMODIFY:
		{
			CString t_name, t_guild;
			BYTE type = GetByte(bypData, offset);
			int uid = GetInt(bypData, offset);
			BYTE bDeadEnable = GetByte(bypData, offset);
			BYTE len = GetByte(bypData, offset);
			LPTSTR t_buf = t_name.GetBuffer(len);
			GetString(bypData, offset, (BYTE *)t_buf, len);		// 이름
			t_name.ReleaseBuffer();

			len = GetByte(bypData, offset);
			if(len > 0)
			{
				GetString(bypData, offset, (PBYTE)t_guild.GetBuffer(len), len);		// 길드 이름
				t_guild.ReleaseBuffer();
			}

			short hair = GetShort(bypData, offset); //머리.
			short skin = GetShort(bypData, offset); //피부.

			short pic = GetShort(bypData, offset);// 몸 그림 번호
			short moral = GetShort(bypData, offset);	// 성향
			BYTE live = GetByte(bypData, offset);// 살아있는 지...(1)
			BYTE battle_mode = GetByte(bypData, offset); // 배틀모드
			short x = GetShort(bypData, offset);
			short y = GetShort(bypData, offset);
			short item[10];
			short btype[10]; // 타입.. 우석..캐릭터 동작선택위해..10월 16일.
			short barm[10];  // 장착위..
			
			//착용 아이템 순서..
			//머리, 귀, 목, 상체, 하체, 신발, 오른손, 왼손, 오른팔찌, 왼팔찌
			if (type==1) //modify
			{
				for(int i=0;i<10;i++)	// 가방인벤토리에 존재하는 그림번호 테이블에서 읽어서 새롭게 적용해야한다.
				{
					item[i] = GetShort(bypData, offset);									
					btype[i] = GetShort(bypData,offset);// 무기타입.. 우석..캐릭터 동작선택위해..10월 16일.
					barm[i] = GetShort(bypData,offset);// 장착위..
				}

				CChildView::UserManager.ModifyUser(uid, bDeadEnable, t_name, t_guild, pic, moral, live, battle_mode, x, y, &item[0], uid==m_bpConnectedView->GetMyUserId(), hair, skin); //10월 16일 수정..우석..
				BYTE m_num = GetByte(bypData,offset);//아무것도 없을때: 0
				if( m_num > 0 )
				{
					TRACE("m_num %d\n",m_num);
					short* magic_table_index = new short[m_num];
					DWORD* con_time = new DWORD[m_num];
					for( int ii = 0; ii < m_num ; ii++ )
					{
						magic_table_index[ii] = GetShort(bypData,offset);
						TRACE("magic_table_index %d\n",magic_table_index[ii]);
						con_time[ii] = GetDword(bypData,offset);
					}
					CChildView::UserManager.UserAppliedMagic( uid, m_num, magic_table_index, con_time );
					SAFE_DELETES( con_time );
					SAFE_DELETES( magic_table_index );
				}

			}
			else if (type==2) //delete
			{
				//자기 자신일 경우는 지우지 않는다.
				if (uid!=m_bpConnectedView->GetMyUserId()) CChildView::UserManager.DeleteUser(uid, x, y);
				else TRACE("자기 자신을 지우려는 시도가 있었음\n");
			}
			else ASSERT(0);
		}
		break;
	case PKT_ZONECHANGE:
		{
			int uid = GetInt(bypData, offset);
			m_bpConnectedView->SetMyUserId(uid);
			short x = GetShort(bypData, offset);
			short y = GetShort(bypData, offset);
			short mapnum = GetShort(bypData, offset);

			ZoneChange(uid, x, y, mapnum);
		}
		break;
	case PKT_BATTLEMODE:
		BattleModeProcess(bypData+offset);
		//CChildView::UserManager
		break;
	case PKT_ATTACK:
		AttackProcess(bypData+offset);
		break;
	case PKT_ATTACK_ARROW:
		ArrowAttackProcess(bypData+offset);
		break;
	case PKT_ATTACK_MAGIC_RAIL:
		ArrowMagicRailAttackProcess(bypData+offset);
		break;
	case PKT_MAGIC_READY:
		Magic_ReadyProcess(bypData+offset);		
		break;
	case PKT_MAGIC_CIRCLE:
		Magic_CircleProcess(bypData+offset);
		break;
	case PKT_POISON:
		Magic_PoisonProcess(bypData+offset);
		break;
	case PKT_STATUS_INFO:
		CharStateChange(bypData+offset);
		break;
	case PKT_DEAD:
		DeadProcess(bypData+offset);
		break;
	case PKT_DEAD_INFO:
		break;
	case PKT_SET_TIME:
		SetTimeProcess(bypData+offset);
		break;
	case PKT_SHOW_MAGIC:
		ShowMagicProcess(bypData+offset);
		break;
	case PKT_CHAT:
		if(CChildView::UserManager.m_ReadyOK == 1)
			ChatParse(bypData+offset);
		break;
	case PKT_SHOPOPEN:
		Dialog_Shop.ShopOpen(bypData+offset);
		break;
	case PKT_SELL_SHOP:
		if(Dialog_Shop.IsVisible()) Dialog_Shop.m_bProcessing = FALSE;
		break;
	case PKT_BUY_SHOP:
		if(Dialog_Shop.IsVisible()) Dialog_Shop.m_bProcessing = FALSE;
		break;
	case PKT_GETITEM:
		CChildView::GetItemProcess(bypData+offset);
		if(Dialog_Inv.IsVisible()) Dialog_Inv.RefreshData();
		else if(Dialog_Shop.IsVisible()) Dialog_Shop.RefreshData();
		else if(Dialog_Storage.IsVisible()) Dialog_Storage.RefreshData();
		else if(Dialog_AuctionSell.IsVisible()) Dialog_AuctionSell.RefreshData();
		else if(Dialog_Repair.IsVisible()) Dialog_Repair.RefreshData();
		break;
	case PKT_OUTITEM:
		CChildView::OutItemProcess(bypData+offset);
		if(Dialog_Inv.IsVisible()) Dialog_Inv.RefreshData();
		else if(Dialog_Shop.IsVisible()) Dialog_Shop.RefreshData();
		else if(Dialog_Storage.IsVisible()) Dialog_Storage.RefreshData();
		else if(Dialog_AuctionSell.IsVisible()) Dialog_AuctionSell.RefreshData();
		else if(Dialog_Repair.IsVisible()) Dialog_Repair.RefreshData();
		break;
	case PKT_MONEYCHANGE:
		CChildView::MoneyChangeProcess(bypData+offset);
		break;

	case PKT_STORAGEOPEN:
		Dialog_Storage.ShowWindow();
		Dialog_Storage.StorageOpen(bypData+offset);
		break;
	case PKT_SAVEITEM:
		Dialog_Storage.SaveItemToStorage(bypData+offset);
		break;
	case PKT_TAKEBACKITEM:
		Dialog_Storage.TakebackItemFromStorage(bypData+offset);
		break;
	case PKT_SAVEMONEY:
		Dialog_Storage.SaveMoney(bypData+offset);
		break;
	case PKT_TAKEBACKMONEY:
		Dialog_Storage.TakebackMoney(bypData+offset);
		break;
	case PKT_MAKEROPEN:
		Dialog_MakeItem.ShowWindow();
		Dialog_MakeItem.MakerOpen(bypData+offset);
		break;
	case PKT_MAKEITEM:
		Dialog_MakeItem.ReceiveProduct(bypData+offset);
		break;

// by krazi.
/////////////////////////////////////
	case BBS_OPEN:	case BBS_NEXT:	case BBS_READ:	
	case BBS_WRITE:	case BBS_EDIT:	case BBS_DELETE:
		::Dialog_BoardCtrl.BBS_PKT_Process(bypData);
		break;
///////////////////////////////////////////////////////////////
	
	case PKT_WEIGHT:
		ApplyWeight(bypData+offset);
		break;

	case PKT_TRADE_REQ:
		Dialog_Exchange.RcvReqTrade(bypData+offset);
		break;
	case PKT_TRADE_ACK:
		Dialog_Exchange.RcvAckTrade(bypData+offset);
		break;
	case PKT_TRADE_ADDITEM:
		Dialog_Exchange.RcvTradeItem(bypData+offset);
		break;
	case PKT_TRADE_SETMONEY:
		Dialog_Exchange.RcvTradeMoney(bypData+offset);
		break;
	case PKT_TRADE_RESULT:
		Dialog_Exchange.RcvTradeResult(bypData+offset);
		break;
	case PKT_TRADE_CANCEL:
		Dialog_Exchange.RcvTradeCancel();
		break;
	case PKT_TRADE_OK:
		Dialog_Exchange.RcvTradeOk();
		break;
	case PKT_AUCTION_SELL_OPEN:
		Dialog_AuctionSell.AuctionOpen(bypData+offset);
		break;
	case PKT_AUCTION_SELL_SEND: // 경매에 잘 올렸는 지에 대한 응답
		Dialog_AuctionSell.RcvAuctionItem(bypData+offset);
		break;
	case PKT_AUCTION_SELL_RCV:	// 경매에 올린 물건을 되돌려받는다.
		Dialog_AuctionSell.RcvGetAuctionItem(bypData+offset);
		break;
	case PKT_AUCTION_BUY_OPEN:
		Dialog_AuctionBuy.RcvOpen(bypData+offset);
		break;
	case PKT_AUCTION_BUY_SEND:
		Dialog_AuctionBuy.RcvAuction(bypData+offset);
		break;
	case PKT_PUTITEM_BELT:
		if(Dialog_Inv.IsVisible()) Dialog_Inv.RcvItemToBelt(bypData+offset);
		break;
	case PKT_PUTITEM_INV:
		Dialog_Main.RcvItemToInv(bypData+offset);
		break;
	case PKT_ITEM_CHANGE_INFO:
		ChangeItemInfo(bypData+offset);
		break;
	case PKT_HOUSEPLACE:
		HousePlaceManager.SetHousePlace(bypData+offset);
		break;
	case PKT_SELHOUSEPLACE:
		HousePlaceManager.RcvBuildHouse(bypData+offset);
		break;
	case PKT_EVENTSELBOX: 
		Dialog_EventMessage.ShowWindow();
		Dialog_EventMessage.AddString(bypData+offset);
		break;
	case PKT_EVENTOKBOX:
		Dialog_EventMessage1.ShowWindow();
		Dialog_EventMessage1.AddString(bypData+offset);
		break;
	case PKT_EVENTNORMAL:
		Dialog_EventMessage2.ShowWindow();
		Dialog_EventMessage2.AddString(bypData+offset);
		break;
	case PKT_HPMP_RECOVERY:
		CChildView::UserManager.HpMpRecovery(bypData+offset);
		break;
	case PKT_RESTARTGAME:
		m_bpConnectedView->ReStart();
		break;
	case PKT_REPAIR_OPEN:
		Dialog_Repair.ShowWindow();
		break;
	case PKT_REPAIR_REQ:
		Dialog_Repair.RcvRepairItem(bypData+offset);
		break;
	case PKT_GUILD_OPEN:
		OpenGuildMenu(bypData+offset);
		break;
	case PKT_GUILD_NEW:
		Dialog_GuildName.RcvMakeGuild(bypData+offset);
		break;
	case PKT_GUILD_LIST:
		RcvGuildList(bypData+offset);
		break;
	case PKT_GUILD_INFO:
		Dialog_GuildApply.RcvGuildDesc(bypData+offset);
		break;
	case PKT_GUILD_REQ:
		Dialog_GuildApply.RcvApplyGuild(bypData+offset);
		break;
	case PKT_GUILD_REMOVE_REQ:
		RcvCancelApplyGuild(bypData+offset);
		break;
	case PKT_GUILD_APPLICATION_LIST:
		RcvApplicationList(bypData+offset);
		break;
	case PKT_GUILD_REQ_USER_INFO:
		Dialog_GuildJoin.RcvAskUserInfo(bypData+offset);
		break;
	case PKT_GUILD_MOVE_REQ:
		Dialog_GuildJoin.RcvJoinPerson(bypData+offset);
		break;
	case PKT_GUILD_MOVE_REJECT:
		Dialog_GuildJoin.RcvRejectPerson(bypData+offset);
		break;
	case PKT_GUILD_MAN_LIST:
		RcvGuildManList(bypData+offset);
		break;
	case PKT_GUILD_REMOVE_USER:
		Dialog_Guild.RcvRemoveMember(bypData+offset);
		break;
	case PKT_CHANGE_MEMBER:
		Dialog_Guild.RcvChangePosName(bypData+offset);
		break;
	case PKT_CHANGE_GUILD_POS:
		Dialog_Guild.RcvChangePos(bypData+offset);
		break;
	case PKT_GUILD_DISSOLVE:
		Dialog_Guild.RcvDissolveGuild(bypData+offset);
		break;
	default: 
		bProcess = FALSE;
		break;
	}

	return bProcess;
}

void CPacketParser::ZoneChange(int uid, int x, int y, int mapnum)
{
	////////////////////////////////////
	//새로운 맵을 읽어 들여야지..
	CString szFileName;

	szFileName.Format("map\\MSet%02d.tmd", mapnum);
	CChildView::TileMapTool.LoadFromToolData(szFileName);
	szFileName.Format("map\\MSet%02d.tsd", mapnum);
	CChildView::TileSetTool.Load(szFileName);
	szFileName.Format("map\\MSet%02d.hif", mapnum);
	BuildHouseManager.LoadHouseInfo(szFileName);
	//CChildView::MapObj.Load("map\\test.mod");

	CString szLightSourceName;
	szLightSourceName.Format(".\\Map\\MSet%02d.lsf",mapnum); 

	CChildView::m_LightSource.Release(); 
	CChildView::m_LightSource.Load( szLightSourceName );

	////////////////////////////////////
//	CChildView::MapObj.Load("map\\object\\somaobj.mod"); // 이라인 대신...
	HANDLE hFile;
	szFileName.Format("map\\MSet%02d.mod", mapnum);
	hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
		OutputDebugString("타일맵 데이터 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}
	int count;	DWORD dwCount;
	ReadFile(hFile, &count, sizeof(int), &dwCount, NULL);//버전체크
	CChildView::MapObj.Load(hFile);
	CChildView::MapShadowObj.Load(hFile);
	CChildView::MapHouseOut.Load(hFile);
	CChildView::MapHouseIn.Load(hFile);

	for(int i = 0; i < MAX_MAP_OBJ_REG; i++)
	{
		CChildView::MapAniObjs[i].AniCtrlRelease();
		CChildView::MapAniObjs[i].AniRelease();
		CChildView::MapAniObjs[i].SprRelease();
	}
	for(i = 0; i < MAX_MAP_OBJ_REG; i++)
	{
		CChildView::MapShadowAniObjs[i].AniCtrlRelease();
		CChildView::MapShadowAniObjs[i].AniRelease();
		CChildView::MapShadowAniObjs[i].SprRelease();
	}
	for( i = 0; i < MAX_MAP_OBJ_REG; i++)
	{
		CChildView::MapAniHouseIn[i].AniCtrlRelease();
		CChildView::MapAniHouseIn[i].AniRelease();
		CChildView::MapAniHouseIn[i].SprRelease();
	}
	for(i = 0; i < MAX_MAP_OBJ_REG; i++)
	{
		CChildView::MapAniHouseIn[i].AniCtrlRelease();
		CChildView::MapAniHouseIn[i].AniRelease();
		CChildView::MapAniHouseOut[i].SprRelease();
	}
	
/*	m_ShadowObj.ReleaseEx(); // 해제
	m_ShadowObj.Load(hFile); // 부르고
	for(i = 0; i < MAX_MAP_OBJ_REG; i++) // 객체들 로딩.
	{
		g_pTEng->PathSetCur("ANI");
		m_ShadowObj.m_Objs[i].AniLoad(m_ShadowObj.m_Header.szRegObjAniFNs[i]);
		g_pTEng->PathSetCur("SPR");
		m_ShadowObj.m_Objs[i].SprLoad(m_ShadowObj.m_Header.szRegObjSprFNs[i], g_pTEng->PixelFormatGet());
	}
*/
	CloseHandle(hFile);
	////////////////////////////////////

	CString szMapName;
//	short item[10];
//	szMapName.Format("sz-%03d", mapnum);
	szFileName.Format("map\\MSet%02d.imf", mapnum);
	if (FALSE==CChildView::MapManager.LoadMap(szFileName, 1))
	{
		szMapName += _T("파일을 열 수 없습니다.");
		AfxMessageBox(szMapName);
		AfxPostQuitMessage(0);
	}
	else
	{
		CChildView::UserManager.RemoveAllDataWithoutMe();
		CPoint pos = CPoint(x, y);
		pos = CChildView::MapManager.CellToPixel(pos);
		CChildView::MapManager.SetCenterPos(pos);
		CChildView::UserManager.SetMoveableTable(&CChildView::MapManager);// 맵을 읽어 들이는거 같은데..음냐..
		// 아래..따로 패킷이 날라옴..
//		CChildView::UserManager.ModifyUser(uid, x, y, &item[0], TRUE);//uid==m_bpConnectedView->GetMyUserId());//(재덕) TRUE);	//자신의 정보,
	}

	g_vOldMapNumber = g_vMapNumber;
	g_vMapNumber = mapnum;
}

void CPacketParser::AcountLoginProcess(PBYTE bypData)
{
	int uid = 0;
	int offset = 0;
	bool bLoginCheck = false;
	switch(bypData[offset++])
	{
	case 0x01: //성공
		{
			int name_len;
			CString name[3];
			CString str;
			short   gender;
			BYTE count = GetByte(bypData, offset);
			for(int i=0;i<(int)count;i++)
			{				
				name_len = GetByte(bypData, offset);

				m_bpConnectedView->m_CharData[i]->IdSize = name_len; //우석.
				m_bpConnectedView->m_CharData[i]->userID = new char[name_len + 1]; //우석.


				LPSTR lpStr = str.GetBufferSetLength((int)name_len);
				GetString(bypData, offset, (PBYTE)lpStr, (int)name_len);
				str.ReleaseBuffer();
				name[i] = _T(lpStr);
				name[i].MakeUpper();

				strcpy(m_bpConnectedView->m_CharData[i]->userID, name[i]); //우석.				

				//우석
				m_bpConnectedView->m_CharData[i]->Class = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Level = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Str = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Dex = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Inteli = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Cha = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Wis = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Con = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Hp = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->MaxHp = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Mp = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->MaxMp = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Stm = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->MaxStm = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Age = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Moral = GetShort(bypData,offset);
				//서버 남자 :1 여자:0  클라이언트 남자:0 여자:1
				gender = GetShort(bypData,offset);
				if(gender)	m_bpConnectedView->m_CharData[i]->Gender = 0; //남자..
				else m_bpConnectedView->m_CharData[i]->Gender = 1; //여자..

				m_bpConnectedView->m_CharData[i]->Hair = GetShort(bypData,offset);
				m_bpConnectedView->m_CharData[i]->Skin = GetShort(bypData,offset);
				name_len = GetByte(bypData, offset);
				if(name_len > 0)
				{
					GetString(bypData, offset, (PBYTE)m_bpConnectedView->m_CharData[i]->Group.GetBuffer(name_len), name_len);
					m_bpConnectedView->m_CharData[i]->Group.ReleaseBuffer();
				}
				else m_bpConnectedView->m_CharData[i]->Group.Empty();
				m_bpConnectedView->m_CharData[i]->Fame = GetShort(bypData,offset);

				m_bpConnectedView->m_CharData[i]->SwordExp = GetInt(bypData,offset);
				m_bpConnectedView->m_CharData[i]->SpearExp = GetInt(bypData,offset);
				m_bpConnectedView->m_CharData[i]->BowExp = GetInt(bypData,offset);
				m_bpConnectedView->m_CharData[i]->AxeExp = GetInt(bypData,offset);
				m_bpConnectedView->m_CharData[i]->KnuckleExp = GetInt(bypData,offset);
				m_bpConnectedView->m_CharData[i]->StaffExp = GetInt(bypData,offset);

				m_bpConnectedView->m_CharData[i]->WeaponMakeExp = GetInt(bypData,offset);
				m_bpConnectedView->m_CharData[i]->ArmorMakeExp = GetInt(bypData,offset);
				m_bpConnectedView->m_CharData[i]->AccMakeExp = GetInt(bypData,offset);
				m_bpConnectedView->m_CharData[i]->PotionMakeExp = GetInt(bypData,offset);
				m_bpConnectedView->m_CharData[i]->CookingExp = GetInt(bypData,offset);

				for(int j=0; j < 10; j++)
				{
					m_bpConnectedView->m_CharData[i]->ItemCheck[j] = GetShort(bypData,offset);
				}

				m_bpConnectedView->m_CharNum++;
				bLoginCheck = true;
				//우석.
			}
			CChildView::SocketMng.m_szLoginId = CChildView::SocketMng.m_szLoginId1 = name[0];
			CChildView::SocketMng.m_szLoginId2 = name[1];
			CChildView::SocketMng.m_szLoginId3 = name[2];

			//우석.
			if(bLoginCheck) m_bpConnectedView->InitChar();
		}
		break;
	case 0x02: //실패
		AfxMessageBox("acount로 서버에 접속실패(Fail)");
		AfxPostQuitMessage(0);
		break;
	}
}

void CPacketParser::LoginProcess(PBYTE bypData)
{
	ASSERT(m_bpConnectedView);

	int uid = 0;
	int offset = 0;
	switch(bypData[offset++])
	{
	case 0x01: //성공
		{
			int uid = GetInt(bypData, offset);
			m_bpConnectedView->SetMyUserId(uid);
			short x = GetShort(bypData, offset);
			short y = GetShort(bypData, offset);
			short mapnum = GetShort(bypData, offset);

			TRACE("LOGIN OK\n");
			ZoneChange(uid, x, y, mapnum);
		}
		break;
	case 0x02: //실패
		AfxMessageBox("서버에 접속 실패(Login Fail)");
		AfxPostQuitMessage(0);
		break;
	}
}

void CPacketParser::MoveProcess(PBYTE bypData, BYTE kind)
{
	int			offset = 0;
	static		DWORD deltick = 0;
	BOOL		bSuccess;


	if(GetByte(bypData, offset) == 1 )// 성공이면..
		bSuccess = TRUE;
	else bSuccess = FALSE;

	int uid;

	uid = GetInt(bypData, offset);

	CPoint ptDst, ptFirst, ptNext;

	CUser *pSorUser = CChildView::UserManager.GetUserWithUid(uid);

	if(pSorUser == NULL || pSorUser->IsCharDie() == TRUE)
	{
		TRACE1("캐릭터 없음(%d)\n", uid);

		int size = g_arrRequestId.GetSize();
		for(int i = 0; i < size; i++)
		{
			if(uid == g_arrRequestId[i]) return;
		}

		BYTE buf[10];
		int offset = 0;
		SetByte(buf, offset, PKT_USERMODIFY); //최종점 요청
		
		SetInt(buf, offset, uid);
		CChildView::SocketMng.Send(offset, buf);
		
		g_arrRequestId.Add(uid);
		return;
	}

	ptDst.x = GetShort(bypData, offset);
	ptDst.y = GetShort(bypData, offset);

	if(pSorUser->m_bDeadFlag && pSorUser->m_pRoute) //죽었으면...
	{
		pSorUser->m_pRoute->RemoveAll();
		pSorUser->m_vMoveStep = 0;
		TRACE("죽은 NPC한테 move패킷이 왔다..\n");
		return;
	}

	if(!pSorUser->m_pRoute)
	{
		TRACE("m_pRoute가 NULL이다...\n");
		return;
	}

	pSorUser->m_PkRecive_Flag = 0;
	if(!bSuccess) 
	{
		// 실패를 하면 정지 시킨다. // 나일때만 필요하겠쥐...
		pSorUser->m_vMoveStep--;
		if (pSorUser->m_Me == 1)
		{
			if(pSorUser->m_pRoute) pSorUser->m_pRoute->RemoveAll();
			pSorUser->m_Run = 1;
			//pSorUser->m_PkRecive_Flag = 0;// 패킷을 받았다.

			switch(kind)
			{
				case PKT_RUN_MOVEFIRST: 
				case PKT_RUN_MOVEMIDDLE:
				case PKT_RUN_MOVEEND:					
					pSorUser->SetMotion(CUser::UM_RUN);
					pSorUser->m_Run = 2;
					break;
			}
		}
		TRACE("이동에 실패 패킷이 도착했다.\n");
		if( pSorUser->m_Position == pSorUser->m_ptDestFinalPos ) 
			pSorUser->SetMotion(CUser::UM_Stop);
		else 
		{
			pSorUser->m_ptPostCellPos = pSorUser->PixelToCellUnit(pSorUser->m_ptDestFinalPos);
			pSorUser->m_ptDstPos = pSorUser->m_ptDestFinalPos;
		}
		return;//FAILL...
	}

	switch(kind)
	{
	case PKT_MOVEFIRST: 
		pSorUser->m_vMoveStep--;
		ptFirst.x = GetShort(bypData, offset);
		ptFirst.y = GetShort(bypData, offset);

		pSorUser->m_Run = 1;
		pSorUser->m_Run_Flag = 1;

		pSorUser->m_Walk = 1;
//		if (pSorUser->m_Me != 1) TRACE3("First-(%d, %d) %u\n", ptFirst.x, ptFirst.y, GetTickCount());
		CChildView::UserManager.SetMoveFirst(pSorUser, ptDst,ptFirst); //우석.. 10월 11일..
		break;

	case PKT_MOVEMIDDLE: 
		pSorUser->m_vMoveStep--;
		pSorUser->m_Run = 1;
		pSorUser->m_Run_Flag = 1;

		pSorUser->m_Walk = 1;
//		if (pSorUser->m_Me != 1)TRACE3("Middle-(%d, %d) %u\n", ptDst.x, ptDst.y, GetTickCount());
		CChildView::UserManager.SetMoveMid(pSorUser, ptDst);
		break;

	case PKT_MOVEEND: 
		pSorUser->m_vMoveStep--;
		pSorUser->m_Run = 1;
		pSorUser->m_Run_Flag = 1;
		pSorUser->m_Walk = 0;

//		if (pSorUser->m_Me != 1)TRACE3("End-(%d, %d) %u\n", ptDst.x, ptDst.y, GetTickCount());
		CChildView::UserManager.SetMoveEnd(pSorUser, ptDst);//TRACE("npc End..움직인다. %d, %d\n", ptDst.x, ptDst.y);
		break;

	case PKT_RUN_MOVEFIRST: 
		pSorUser->m_vMoveStep--;
		ptFirst.x = GetShort(bypData, offset);
		ptFirst.y = GetShort(bypData, offset);
		ptNext.x = GetShort(bypData, offset);
		ptNext.y = GetShort(bypData, offset);
		pSorUser->m_Cur_Fa = GetShort(bypData, offset);
		
		pSorUser->m_Walk = 1;
		pSorUser->m_Run = 2;
		pSorUser->m_Run_Flag = 1;
		pSorUser->m_RunEnd = 0;


//		if (pSorUser->m_Me != 1) TRACE("run-first( first[%d][%d] : next[%d][%d]\n", ptFirst.x, ptFirst.y, ptNext.x, ptNext.y);	
		CChildView::UserManager.SetRunMoveFirst(pSorUser, ptDst,ptFirst, ptNext);		

		break;
	case PKT_RUN_MOVEMIDDLE: 
		pSorUser->m_vMoveStep--;
		ptNext.x = GetShort(bypData, offset);
		ptNext.y = GetShort(bypData, offset);
		pSorUser->m_Cur_Fa = GetShort(bypData, offset);

		pSorUser->m_Walk = 1;
		pSorUser->m_Run = 2;
		pSorUser->m_Run_Flag = 1;
		pSorUser->m_RunEnd = 0;

//		if (pSorUser->m_Me != 1) TRACE("run-middle dst[%d][%d] : next[%d][%d]\n", ptDst.x, ptDst.y, ptNext.x, ptNext.y);
		CChildView::UserManager.SetRunMoveMid(pSorUser, ptDst, ptNext);
		break;

	case PKT_RUN_MOVEEND: 
		pSorUser->m_vMoveStep--;
		ptNext.x = GetShort(bypData, offset);
		ptNext.y = GetShort(bypData, offset);
		pSorUser->m_Cur_Fa = GetShort(bypData, offset);

		pSorUser->m_Walk = 0;
		pSorUser->m_Run = 2;
		pSorUser->m_Run_Flag = 1;
		pSorUser->m_RunEnd = 1;

//		if (pSorUser->m_Me != 1) TRACE("run-end dst[%d][%d] : next[%d][%d]\n", ptDst.x, ptDst.y, ptNext.x, ptNext.y);
		CChildView::UserManager.SetRunMoveEnd(pSorUser, ptDst, ptNext);
		break;
	}
}

void CPacketParser::MoveRequest(CUser *pUserMe, CPoint ptDstCellPos,int FindStep)
{
	ASSERT(pUserMe);

	CPoint ptMapPos;
	CPoint pt;
	CPoint ptFirst, ptDst;
	BYTE buf[50];
	int offset = 0;

	if(pUserMe->m_vMoveStep > 0)
	{		
		TRACE("이동중입니다. m_vMoveStep = %d\n", pUserMe->m_vMoveStep);
		return;
	}

	if(pUserMe->m_pRoute)pUserMe->m_pRoute->RemoveAll();

	ptMapPos = pUserMe->PixelToCellUnit(pUserMe->m_ptDestFinalPos);///////////

	
	//10월 11일..우석길찾기..
	CList <CPoint, CPoint> *pRoute = pUserMe->CalcMoveList2(ptMapPos, ptDstCellPos, FindStep);// 패스 파인딩...
	
	if (NULL==pRoute) //같은 지점: 움직이지 못하거나, 같은자리를 클릭한 경우
	{
		TRACE("pRoute가 널이다.\n");
		return;
	}

	if( pUserMe->m_Position != pUserMe->m_ptDestFinalPos )
	{
		pUserMe->m_ptPostCellPos = pUserMe->PixelToCellUnit(pUserMe->m_ptDestFinalPos);
		pUserMe->m_ptDstPos = pUserMe->m_ptDestFinalPos;		
		m_bPathFind = true;
		pUserMe->SetMotion(CUser::UM_Walk);
		pUserMe->m_Run = 1;
		TRACE("서버와 위치가 맞지않아 보정..\n");
		TRACE("m_Position: %d , %d \n",pUserMe->PixelToCellUnit(pUserMe->m_Position).x,pUserMe->PixelToCellUnit(pUserMe->m_Position).y);
		TRACE("m_ptDestFianal: %d , %d \n",pUserMe->PixelToCellUnit(pUserMe->m_ptDestFinalPos).x,pUserMe->PixelToCellUnit(pUserMe->m_ptDestFinalPos).y);
	}
	else m_bPathFind = false;

	if( pRoute->IsEmpty() )
	{
		TRACE("갈 수 있는길이 없다.\n");
		SAFE_DELETE(pRoute);
		return;
	}

	if(m_bPathFind == false)
	{
		SetByte(buf, offset, PKT_MOVEFIRST); //Move First Req
		pt = pRoute->GetTail();		//End Position
		SetShort(buf, offset, (short)pt.x);
		SetShort(buf, offset, (short)pt.y);
		ptDst = pt;

		pt = pRoute->RemoveHead();		//First Request Position
		SetShort(buf, offset, (short)pt.x);
		SetShort(buf, offset, (short)pt.y);
		ptFirst = pt;

		CChildView::SocketMng.Send(offset, buf);
		pUserMe->m_dwnewMoveTick = GetTickCount();
//		TRACE("Send PKT_MOVEFIRST%d(%d,%d)-(%d,%d)\n", pUserMe->m_vMoveStep, ptDst.x, ptDst.y, ptFirst.x, ptFirst.y);
		pUserMe->m_vMoveStep++;
		pUserMe->SetMoveList(pRoute);

		pUserMe->m_PkRecive_Flag = 1;// 패킷을 보냈다.
		pUserMe->m_Run = 1;
		pUserMe->m_Run_Flag = 1;
	}
	else 
	{
		pUserMe->SetMoveList(pRoute);
	}

}

void CPacketParser::MoveRunRequest(CUser *pUserMe, CPoint ptDstCellPos, int FindStep)
{
	ASSERT(pUserMe);

	CPoint		ptMapPos;
	CPoint		pt, next_pt;
	CPoint		ptFirst, ptDst, ptNext;
//	POSITION	NextPosition;
	BYTE		buf[50];
	int			offset = 0;

	if(pUserMe->m_vMoveStep > 0)
	{
		TRACE0("이동중입니다.\n");
		return;
	}

	if(pUserMe->m_pRoute)pUserMe->m_pRoute->RemoveAll();

	ptMapPos = pUserMe->PixelToCellUnit(pUserMe->m_ptDestFinalPos);///////////


	CList <CPoint, CPoint> *pRoute = pUserMe->CalcMoveList2(ptMapPos, ptDstCellPos, FindStep);
	if (NULL==pRoute) //같은 지점: 움직이지 못하거나, 같은자리를 클릭한 경우
	{
		TRACE("pRoute가 널이다.\n");
		return;
	}
/*
	//바로 옆셀을 클릭시 뛰지않고 걷는다.
	if(pRoute->GetCount() < 2)
	{
		SAFE_DELETE(pRoute);
		MoveRequest(pUserMe, ptDstCellPos, FindStep);
		return;
	}
*/
/*
	//스테미너가 다되었을때 걷게한다...
	if(pUserMe->m_Cur_Fa <= 0)
	{
		SAFE_DELETE(pRoute);
		MoveRequest(pUserMe, ptDstCellPos, FindStep);
		return;
	}
*/
	if(pUserMe->m_Position != pUserMe->m_ptDestFinalPos)
	{
		pUserMe->m_ptPostCellPos = pUserMe->PixelToCellUnit(pUserMe->m_ptDestFinalPos);
		pUserMe->m_ptDstPos = pUserMe->m_ptDestFinalPos;		
		m_bPathFind = true;
		pUserMe->SetMotion(CUser::UM_RUN);
		pUserMe->m_Run = 2;
		TRACE("서버와 위치가 맞지않아 보정..RUN\n");
		TRACE("m_Position: %d , %d \n",pUserMe->PixelToCellUnit(pUserMe->m_Position).x,pUserMe->PixelToCellUnit(pUserMe->m_Position).y);
		TRACE("m_ptDestFianal: %d , %d \n",pUserMe->PixelToCellUnit(pUserMe->m_ptDestFinalPos).x,pUserMe->PixelToCellUnit(pUserMe->m_ptDestFinalPos).y);
	}
	else m_bPathFind = false;

	if( pRoute->IsEmpty() )
	{
		TRACE("갈 수 있는길이 없다.\n");
		SAFE_DELETE(pRoute);
		return;
	}


	if(m_bPathFind == false)
	{
		pUserMe->m_RunEnd = 0;

		SetByte(buf, offset, PKT_RUN_MOVEFIRST); //Move First Req
		pt = pRoute->GetTail();		//End Position
		SetShort(buf, offset, (short)pt.x);
		SetShort(buf, offset, (short)pt.y);
		ptDst = pt;

		pt = pRoute->RemoveHead();		//First Request Position
		SetShort(buf, offset, (short)pt.x);
		SetShort(buf, offset, (short)pt.y);
		ptFirst = pt;

		if(pRoute->GetCount() != 0) next_pt = pRoute->GetHead();		//First Request Position
		else                        next_pt = CPoint(-1,-1);		//First Request Position

		SetShort(buf, offset, (short)next_pt.x);
		SetShort(buf, offset, (short)next_pt.y);
		ptNext = next_pt;

		CChildView::SocketMng.Send(offset, buf);
		pUserMe->m_dwnewMoveTick = GetTickCount();
		pUserMe->m_vMoveStep++;
//		TRACE("Send PKT_RUN_MOVEFIRST (dst[%d,%d] : first[%d,%d]: next[%d,%d]\n",
//			   ptDst.x,ptDst.y,ptFirst.x,ptFirst.y,ptNext.x,ptNext.y);

		pUserMe->SetMoveList(pRoute);
		pUserMe->m_PkRecive_Flag = 1;// 패킷을 보냈다.
	}
	else
	{
		pUserMe->SetMoveList(pRoute);
	}
}

void CPacketParser::AttackProcess(PBYTE bypData)
{
	int offset = 0;
	BYTE bSuccess = GetByte(bypData, offset);

	CPoint ptSorPos, ptDestPos;
	int iSorUid, iDestUid;
	short sDemage;
	DWORD cur_exp;
	int i;
	short t_index, t_usage;

	TRACE("ATTACK\n");
	iSorUid = GetInt(bypData, offset);
	iDestUid = GetInt(bypData, offset);

	CUser *pSorUser = CChildView::UserManager.GetUserWithUid(iSorUid);
	CUser *pDestUser = CChildView::UserManager.GetUserWithUid(iDestUid);

	if(pSorUser == NULL) return;
	if(pSorUser->m_Me == 1)
	{
		pSorUser->m_PkRecive_Attack_Flag = 0;
	}
	if(pDestUser == NULL || bSuccess == 2/*실패*/) return;

	if(pSorUser->m_Me == 1)
	{
		for(i = 0; i < 2; i++)
		{
			t_index = GetShort(bypData, offset);
			t_usage = GetShort(bypData, offset);
			if(t_index >= 0) pSorUser->m_MyInvData[t_index].m_vUsage = t_usage;
		}
		for(i = 0; i < 2; i++)
		{
			GetShort(bypData, offset);
			GetShort(bypData, offset);
		}
	}
	else
	{
		for(i = 0; i < 2; i++)
		{
			GetShort(bypData, offset);
			GetShort(bypData, offset);
		}
		if(pDestUser->m_Me == 1)
		{
			for(i = 0; i < 2; i++)
			{
				t_index = GetShort(bypData, offset);
				t_usage = GetShort(bypData, offset);
				if(t_index >= 0) pDestUser->m_MyInvData[t_index].m_vUsage = t_usage;
			}
		}
		else
		{
			for(i = 0; i < 2; i++)
			{
				GetShort(bypData, offset);
				GetShort(bypData, offset);
			}
		}
	}
	ptSorPos = pSorUser->m_Position;
	ptDestPos = pDestUser->m_Position;
	if(bSuccess == 1)
	{
		sDemage = GetShort(bypData, offset);// 맞은뒤 남은 에너지...임시..
		short max_hp= GetShort(bypData, offset);
		cur_exp = GetDword(bypData, offset);

//		m_Position = pos; 
//		m_ptCellPos = PixelToCellUnit(m_Position);
		if(max_hp != 0)
		{
			double ratio;
			pDestUser->m_Max_Hp = max_hp;
			ratio = (double)sDemage / max_hp;
			if( ratio < 0.3 ) pDestUser->m_nFocusColor = 1;
			else if( ratio >= 0.3 && ratio < 0.6 )pDestUser->m_nFocusColor = 2;
			else if( ratio >= 0.6) pDestUser->m_nFocusColor = 3;
		}
		pDestUser->m_Cur_Hp = sDemage;// 나일때만 hp를 집어 넣어 준다.
		if(pSorUser->m_Me == 1)
		{
			pSorUser->m_Cur_exp = cur_exp;
		}
	}
	
	CChildView::UserManager.AttackChar(bSuccess, iSorUid, ptSorPos, iDestUid, ptDestPos);
}

void CPacketParser::ArrowAttackProcess(PBYTE bypData)
{
	int offset = 0;
	BOOL bSuccess = GetByte(bypData, offset);

	CPoint ptSorPos, ptDestPos;
	int iSorUid, iDstUid;
	short Mp;
	DWORD exp;

	TRACE("PKT_ATTACK_ARROW를 받았다.\n");
	iSorUid = GetInt(bypData, offset);
	Mp = GetShort(bypData, offset);
	exp = GetDword(bypData, offset);
	iDstUid = GetInt(bypData, offset);
	short magicindex = GetShort(bypData, offset);//마법 테이블에서 읽을 마법 인덱스
	short hp = GetShort(bypData, offset);

	short max_hp = GetShort(bypData,offset);
	CUser *pDestUser = CChildView::UserManager.GetUserWithUid(iDstUid);
	if (pDestUser && max_hp != 0)
	{
		float ratio;
		pDestUser->m_Max_Hp = max_hp;
		ratio = float(hp)/max_hp;
		if( ratio < 0.3f )pDestUser->m_nFocusColor = 1;
		else if( ratio >= 0.3f && ratio < 0.6f )pDestUser->m_nFocusColor = 2;
		else if( ratio >= 0.6f)pDestUser->m_nFocusColor = 3;
	}
	DWORD dwConTime = GetDword(bypData,offset);

	CChildView::UserManager.ReadMagicTable( bSuccess, iSorUid, Mp, exp, &iDstUid,&hp, 1, magicindex, dwConTime );
}

void CPacketParser::ChatParse(PBYTE bypData)
{
	int offset = 0;
	short chat_kind = GetByte(bypData, offset);// 1.노말 채팅 2. 쪽지 4. 내쪽지 에코
	int uid;
	BYTE id_len, chat_len;
	CString id_str, chat_str;

	switch(chat_kind)
	{
	case 1:		// 노말 채팅
		{
			uid = GetInt(bypData, offset);// 귓속말은 안온다.
			id_len = GetByte(bypData, offset);
			GetString(bypData, offset, (PBYTE)id_str.GetBuffer(id_len), id_len);
			id_str.ReleaseBuffer();
			id_str.MakeUpper();

			chat_len = GetByte(bypData, offset);
			if(chat_len)// 임시 아이디를 보여주는 부분을 만들어야 겠쥐..
			{
				GetString(bypData, offset, (PBYTE)chat_str.GetBuffer(chat_len), chat_len);
				chat_str.ReleaseBuffer();

				CUser *pDestUser = CChildView::UserManager.GetUserWithUid(uid);
				if (pDestUser) pDestUser->SetChatString(chat_str);// 버블 채팅에 사용해야지...^^
				id_str += _T(" : ");
				id_str += chat_str;
				if(uid < 10000)
				{
					//채팅창에 더하는 루틴
					BBDX_ChatOuter* pChatOuter = __GetMainWnd()->m_wndView.GetChatOuter();
					if (pChatOuter)
					{
						pChatOuter->AddChatWithKind(id_str, COCK_NORMAL);
						if(Dialog_Chat.IsVisible()) Dialog_Chat.SetScrollData();
					}
				}
			}	
		}
		break;
	case 2:		// 쪽지
		{
			id_len = GetByte(bypData, offset);
			GetString(bypData, offset, (PBYTE)id_str.GetBuffer(id_len), id_len);
			id_str.ReleaseBuffer();
			id_str.MakeUpper();

			chat_len = GetByte(bypData, offset);

			if(chat_len)// 임시 아이디를 보여주는 부분을 만들어야 겠쥐..
			{
				GetString(bypData, offset, (PBYTE)chat_str.GetBuffer(chat_len), chat_len);
				chat_str.ReleaseBuffer();

				Dialog_MemoCtrl.AddMemoChat(id_str, chat_str);
			}	
		}
		break;
	case 3:		// 전체 채팅
		{
			uid = GetInt(bypData, offset);// 귓속말은 안온다.
			id_len = GetByte(bypData, offset);
			GetString(bypData, offset, (PBYTE)id_str.GetBuffer(id_len), id_len);
			id_str.ReleaseBuffer();
			id_str.MakeUpper();

			chat_len = GetByte(bypData, offset);
			if(chat_len)// 임시 아이디를 보여주는 부분을 만들어야 겠쥐..
			{
				GetString(bypData, offset, (PBYTE)chat_str.GetBuffer(chat_len), chat_len);
				chat_str.ReleaseBuffer();

				CUser *pDestUser = CChildView::UserManager.GetUserWithUid(uid);
				if (pDestUser) pDestUser->SetChatString(chat_str);// 버블 채팅에 사용해야지...^^
				id_str += _T(" : ");
				id_str += chat_str;
				if(uid < 10000)
				{
					//채팅창에 더하는 루틴
					BBDX_ChatOuter* pChatOuter = __GetMainWnd()->m_wndView.GetChatOuter();
					if (pChatOuter)
					{
						pChatOuter->AddChatWithKind(id_str, COCK_GLOBAL);
						if(Dialog_Chat.IsVisible()) Dialog_Chat.SetScrollData();
					}
				}
			}	
		}
		break;
	case 4:		// 내 쪽지 에코
		{
			id_len = GetByte(bypData, offset);
			GetString(bypData, offset, (PBYTE)id_str.GetBuffer(id_len), id_len);
			id_str.ReleaseBuffer();
			id_str.MakeUpper();

			chat_len = GetByte(bypData, offset);

			if(chat_len)// 임시 아이디를 보여주는 부분을 만들어야 겠쥐..
			{
				GetString(bypData, offset, (PBYTE)chat_str.GetBuffer(chat_len), chat_len);
				chat_str.ReleaseBuffer();

				Dialog_MemoCtrl.AddEchoMemoChat(id_str, chat_str);
			}	
		}
		break;
	case 5:		// 시스템
		{
			chat_len = GetByte(bypData, offset);
			if(chat_len)// 임시 아이디를 보여주는 부분을 만들어야 겠쥐..
			{
				GetString(bypData, offset, (PBYTE)chat_str.GetBuffer(chat_len), chat_len);
				chat_str.ReleaseBuffer();

				//채팅창에 더하는 루틴
				BBDX_ChatOuter* pChatOuter = __GetMainWnd()->m_wndView.GetChatOuter();
				if (pChatOuter)
				{
					pChatOuter->AddChatWithKind(chat_str, COCK_MESSAGE);
					if(Dialog_Chat.IsVisible()) Dialog_Chat.SetScrollData();
				}
			}	
		}
		break;
	case 6:		// 공지
		{
			chat_len = GetByte(bypData, offset);
			if(chat_len)// 임시 아이디를 보여주는 부분을 만들어야 겠쥐..
			{
				GetString(bypData, offset, (PBYTE)chat_str.GetBuffer(chat_len), chat_len);
				chat_str.ReleaseBuffer();

				//채팅창에 더하는 루틴
				BBDX_ChatOuter* pChatOuter = __GetMainWnd()->m_wndView.GetChatOuter();
				if (pChatOuter)
				{
					pChatOuter->AddChatWithKind(chat_str, COCK_NOTICE);
					if(Dialog_Chat.IsVisible()) Dialog_Chat.SetScrollData();
				}
			}	
		}
		break;
	case 7:		// 길드 채팅
		{
			chat_len = GetByte(bypData, offset);
			if(chat_len)// 임시 아이디를 보여주는 부분을 만들어야 겠쥐..
			{
				GetString(bypData, offset, (PBYTE)chat_str.GetBuffer(chat_len), chat_len);
				chat_str.ReleaseBuffer();

				//채팅창에 더하는 루틴
				BBDX_ChatOuter* pChatOuter = __GetMainWnd()->m_wndView.GetChatOuter();
				if (pChatOuter)
				{
					pChatOuter->AddChatWithKind(chat_str, COCK_FAMILY);
					if(Dialog_Chat.IsVisible()) Dialog_Chat.SetScrollData();
				}
			}	
		}
		break;
	}
}

// 에버랜드에서 추가..
CPoint CPacketParser::Conv2ServerMapPos(CPoint p)
{
	ASSERT ( (p.x + p.y )%2 == 0);
	
	CSize map = CChildView::MapManager.GetMapDimension();	
	int temph = map.cy/2 - 1;
	int tempx = temph - ( p.y / 2 ) + ( p.x / 2 ), tempy;
	if( p.x % 2 ) tempy = ( p.y / 2 ) + ( ( p.x / 2 ) + 1 );
	else        tempy = ( p.y / 2 ) + ( p.x / 2 );

	return CPoint(tempx, tempy);
}

CPoint CPacketParser::Conv2ClientMapPos(CPoint p)
{
	int org_x, org_y;
	CSize map = CChildView::MapManager.GetMapDimension();
	int temph = map.cy/2 - 1;

	org_x = p.x - temph + p.y;
	org_y = p.y - p.x + temph;

	return CPoint(org_x, org_y);
}

void CPacketParser::ArrowMagicRailAttackProcess(PBYTE bypData)
{
	int offset = 0;
	int bSuccess = GetByte(bypData, offset);

	CPoint ptSorPos, ptDestPos;
	int iSorUid, *iDesUid;
	short* pHP;
	short Mp;
	DWORD exp;
	BYTE bDir;
	BYTE bLen;
	short sUserCount;

	iSorUid = GetInt(bypData, offset);// 시전자..
	Mp = GetShort(bypData, offset);
	exp = GetDword(bypData, offset);
	short magicindex = GetShort(bypData, offset);//마법임시
	bDir = GetByte(bypData, offset);
	bLen = GetByte(bypData, offset);
	sUserCount = GetShort(bypData, offset);
	TRACE("DIR %d USERCOUNT %d\n",bDir,sUserCount);

	iDesUid = new int[sUserCount];
	pHP = new short[sUserCount];
	short* max_hp = new short[sUserCount];

	TRACE("PKT_MAGIC_RAIL를 받았다.%d\n",sUserCount);
	for(int i=0; i<sUserCount;i++)
	{
		iDesUid[i] = GetInt(bypData, offset);// 맞은 사람.....
		pHP[i] = GetShort(bypData, offset);
		max_hp[i] = GetShort(bypData,offset);

		CUser *pDestUser = CChildView::UserManager.GetUserWithUid(iDesUid[i]);
		if (pDestUser && max_hp[i] != 0)
		{
			float ratio;
			pDestUser->m_Max_Hp = max_hp[i];
			ratio = float(pHP[i])/max_hp[i];
			if( ratio < 0.3f )pDestUser->m_nFocusColor = 1;
			else if( ratio >= 0.3f && ratio < 0.6f )pDestUser->m_nFocusColor = 2;
			else if( ratio >= 0.6f)pDestUser->m_nFocusColor = 3;
		}
	}
	DWORD dwConTime = GetDword(bypData,offset);

	CChildView::UserManager.ReadMagicTable( bSuccess, iSorUid, Mp, exp, iDesUid, pHP, sUserCount, magicindex, dwConTime, bDir );
	delete[] max_hp;
	delete[] iDesUid;
	delete[] pHP;
}

void CPacketParser::DeadProcess(PBYTE bypData)
{
	int offset = 0;
	int uid = GetInt(bypData, offset);
	int x = GetShort(bypData, offset);
	int y = GetShort(bypData, offset);
	CChildView::UserManager.PreUserDead(uid, (int) x, (int) y);
//	CChildView::UserManager.UserDead(uid, (int) x, (int) y);
}


void CPacketParser::Magic_ReadyProcess(PBYTE bypData)
{
	int offset = 0;
	int bSuccess = GetByte(bypData, offset);

	int iSorUid, iDesUid;
	
	iSorUid = GetInt(bypData, offset);// 시전자..
	iDesUid = GetInt(bypData, offset);// 맞는사람..
	short type = GetShort(bypData, offset);//마법임시
	BYTE dir = GetByte(bypData, offset);
	TRACE("PKT_MAGIC_READY 받았다.\n");
	DWORD dwStartTime = GetDword(bypData,offset);
	CChildView::UserManager.ReadMagicTable(bSuccess, iSorUid, iDesUid, type, dwStartTime, dir);
}

void CPacketParser::Magic_CircleProcess(PBYTE bypData)
{
	int offset = 0;
	int bSuccess = GetByte(bypData, offset);

	CPoint ptSorPos, ptDestPos;
	int iSorUid, *iDesUid;
	short* pHP;
	BYTE bLen;
	short sUserCount;
	

	iSorUid = GetInt(bypData, offset);// 시전자..
	short Mp = GetShort(bypData, offset);
	DWORD exp = GetDword(bypData, offset);
	short type = GetShort(bypData, offset);//마법임시
	bLen = GetByte(bypData, offset);// 마법범위
	sUserCount = GetShort(bypData, offset);//맞는사람 카운트

	iDesUid = new int[sUserCount];
	pHP = new short[sUserCount];
	short* max_hp = new short[sUserCount];

	TRACE("PKT_MAGIC_CIRCLE를 받았다.%d\n",sUserCount);
	for(int i=0; i<sUserCount;i++)
	{
		iDesUid[i] = GetInt(bypData, offset);// 맞은 사람.....
		pHP[i] = GetShort(bypData, offset);
		max_hp[i] = GetShort(bypData,offset);

		CUser *pDestUser = CChildView::UserManager.GetUserWithUid(iDesUid[i]);
		if (pDestUser && max_hp[i] != 0)
		{
			float ratio;
			pDestUser->m_Max_Hp = max_hp[i];
			ratio = float(pHP[i])/max_hp[i];
			if( ratio < 0.3f )pDestUser->m_nFocusColor = 1;
			else if( ratio >= 0.3f && ratio < 0.6f )pDestUser->m_nFocusColor = 2;
			else if( ratio >= 0.6f)pDestUser->m_nFocusColor = 3;
		}
	}
	DWORD dwConTime = GetDword(bypData,offset);
	CChildView::UserManager.ReadMagicTable( bSuccess, iSorUid, Mp, exp, iDesUid, pHP, sUserCount, type, dwConTime);
	delete[] max_hp;
	delete[] iDesUid;
	delete[] pHP;

//	UserManager.SetMagic(m_iMyId, 4, MAGIC_TYPE_LOOP);

}

void CPacketParser::Magic_PoisonProcess(PBYTE bypData)
{
/*
	int offset = 0;
	int *pDesUid;
	short* pHP;
//	BYTE bLen;
	short sUserCount;

	sUserCount = GetShort(bypData, offset);//맞는사람 카운트

	pDesUid = new int[sUserCount];
	pHP = new short[sUserCount];

	TRACE("PKT_POSION를 받았다.%d\n",sUserCount);
	for(int i=0; i<sUserCount;i++)
	{
		pDesUid[i] = GetInt(bypData, offset);// 맞은 사람.....
		pHP[i] = GetShort(bypData, offset);
	}
	CChildView::UserManager.PoisonAffect( pDesUid, pHP, sUserCount);
	delete[] pDesUid;
	delete[] pHP;
*/
	int offset = 0;
	int nDesUid;
	short HP;

	nDesUid = GetInt(bypData, offset);
	HP = GetShort(bypData, offset);
	CChildView::UserManager.PoisonAffect( nDesUid, HP);
	TRACE("PKT_POSION를 받았다.\n");
}
void CPacketParser::CharStateChange(PBYTE bypData)
{
	int offset = 0;
	int nDestUid = GetInt(bypData ,offset);
	DWORD dwStatus = GetDword(bypData, offset);
	CChildView::UserManager.CharStateChange( nDestUid, dwStatus );
}
void CPacketParser::SetTimeProcess(PBYTE bypData)
{
	DWORD dwHourTime;
	DWORD dwSubTime = GetTickCount();
	int offset	= 0;
	short Year	= GetShort(bypData,offset);
	short Month = GetShort(bypData,offset);
	short Day	= GetShort(bypData,offset);
	dwHourTime  = dwSubTime + GetShort(bypData,offset);
//	short Hour	= GetShort(bypData,offset);
	short Minute= GetShort(bypData,offset);
	BYTE  Rain	= GetByte(bypData ,offset);
	BYTE  NightState = GetByte(bypData, offset);
	TRACE( "NightState %d\n",NightState);
	CChildView::DirectXMng.m_Night->SetFirstNightState(Minute); 
	CChildView::Weather.SetWeatherInfo( Year, Month, Day, dwHourTime, dwSubTime, Rain, NightState);
}
void CPacketParser::ShowMagicProcess(PBYTE bypData)
{
	int offset = 0;
	int nUid = GetInt(bypData,offset);
	short PostMagicIndex = GetShort(bypData,offset);
	short PreMagicIndex	 = GetShort(bypData,offset);
	if( PostMagicIndex > 0 )
		CChildView::UserManager.SetMagic( nUid,PostMagicIndex, 0);
	if( PreMagicIndex  > 0 )
		CChildView::UserManager.SetMagic( nUid,PreMagicIndex, 1);
}

void CPacketParser::BattleModeProcess(PBYTE bypData)
{
	int offset = 0;
	int uid = GetInt(bypData, offset);
	BYTE BattleType = GetByte(bypData, offset);
	CUser *pDstUser = CChildView::UserManager.GetUserWithUid(uid);
	if(pDstUser == NULL)
	{
		int size = g_arrRequestId.GetSize();
		for(int i = 0; i < size; i++)
		{
			if(uid == g_arrRequestId[i]) return;
		}

		BYTE buf[10];
		int offset = 0;
		SetByte(buf, offset, PKT_USERMODIFY); //최종점 요청
		SetInt(buf, offset, uid);
		CChildView::SocketMng.Send(offset, buf);
		
		g_arrRequestId.Add(uid);
		return;
	}
	pDstUser->m_BattleMode = (int) BattleType;
	CUser *pMe = CChildView::UserManager.GetMe();
	if(pMe == pDstUser) Dialog_Main.SetBattleMode(pMe->m_BattleMode);

	pDstUser->SetMotion(pDstUser->m_AnimationIndex);
}

void CPacketParser::ApplyWeight(PBYTE bypData)
{
	int offset = 0;
	short weight = GetShort(bypData, offset);

	CUser *pMe = CChildView::UserManager.GetMe();
	pMe->m_Cur_Weight = weight;
}

void CPacketParser::ChangeItemInfo(PBYTE bypData)
{
	int offset = 0;
	CMyInvData inv_data;
	BYTE bBelt = GetByte(bypData, offset); // 벨트(0), 인벤토리(1)
	inv_data.m_vIndex = GetShort(bypData, offset); // 아이템 인덱스
	BYTE nSpec = GetByte(bypData, offset); // 변경된 아이템 정보 종류
	if(nSpec & INFO_TYPE)
	{
		inv_data.m_vType = GetByte(bypData, offset);
		inv_data.m_vGender = GetByte(bypData, offset);
	}
	if(nSpec & INFO_ARM)
	{
		inv_data.m_vArmPos = GetByte(bypData, offset);
	}
	if(nSpec & INFO_PICNUM)
	{
		inv_data.m_vPicNum = GetShort(bypData, offset);
	}
	if(nSpec & INFO_DUR)
	{
		inv_data.m_vUsage = GetShort(bypData, offset);
	}
	if(nSpec & INFO_NAME)
	{
		BYTE len;
		LPTSTR t_buf;

		len = GetByte(bypData, offset);
		t_buf = inv_data.m_vName.GetBuffer(len);
		GetString(bypData, offset, (PBYTE)t_buf, len);
		inv_data.m_vName.ReleaseBuffer();
	}
	if(nSpec & INFO_DAMAGE)
	{
		inv_data.m_vMinDamage = GetShort(bypData, offset);
		inv_data.m_vMaxDamage = GetShort(bypData, offset);
	}
	if(nSpec & INFO_LIMIT)
	{
		BYTE len;

		inv_data.m_vMaxUsage = GetShort(bypData, offset);
		inv_data.m_vOrgMaxUsage = GetShort(bypData, offset);
		inv_data.m_vValue = GetDword(bypData, offset);
		inv_data.m_vLimitStr = GetShort(bypData, offset);
		inv_data.m_vLimitInt = GetShort(bypData, offset);
		inv_data.m_vLimitSkill = GetShort(bypData, offset);
		len = GetByte(bypData, offset);
		if(len > 0)
		{
			GetString(bypData, offset, (PBYTE)inv_data.m_strMagic.GetBuffer(len), len);
			inv_data.m_strMagic.ReleaseBuffer();
		}
		len = GetByte(bypData, offset);
		if(len > 0)
		{
			GetString(bypData, offset, (PBYTE)inv_data.m_strSNum.GetBuffer(len), len);
			inv_data.m_strSNum.ReleaseBuffer();
		}
	}
	if(nSpec & INFO_SPECIAL)
	{
		for(int k = 0; k < 7; k++) inv_data.m_vSpecial[k] = GetShort(bypData, offset);
	}

	CUser *pMe = CChildView::UserManager.GetMe();
	pMe->ChangeItemInfo(bBelt, nSpec, inv_data);
	if(bBelt == 1)
	{
		if(Dialog_Inv.IsVisible()) Dialog_Inv.RefreshData();
		else if(Dialog_Shop.IsVisible()) Dialog_Shop.RefreshData();
		else if(Dialog_Storage.IsVisible()) Dialog_Storage.RefreshData();
		else if(Dialog_AuctionSell.IsVisible()) Dialog_AuctionSell.RefreshData();
		else if(Dialog_Repair.IsVisible()) Dialog_Repair.RefreshData();
	}
}

void CPacketParser::ConnectInfoProcess(PBYTE bypData)
{
	CString strUserId, strAddr;
	BYTE bStart;
	int nPort;
	short str_len;
	int offset = 0;
	
	str_len = GetShort(bypData, offset);
	if(str_len)
	{
		GetString(bypData, offset, (PBYTE)strUserId.GetBuffer(str_len), str_len);
		strUserId.ReleaseBuffer();
	}
	bStart = GetByte(bypData, offset);
	str_len = GetShort(bypData, offset);
	if(str_len)
	{
		GetString(bypData, offset, (PBYTE)strAddr.GetBuffer(str_len), str_len);
		strAddr.ReleaseBuffer();
	}
	nPort = GetInt(bypData, offset);

	if(CChildView::SocketMng.ConnectGameSocket(strAddr, nPort))
	{
		m_bIsSessionLogin = FALSE;
		if(g_pChildView)
		{
			g_pChildView->SetTimer(0, 30000, NULL);
		}
	}
}

void CPacketParser::OpenGuildMenu(PBYTE bypData)
{
	BYTE bMenu;
	int offset = 0;

	bMenu = GetByte(bypData, offset);

	if(bMenu == 0)		// 길드원이 아닌 경우
	{
		DWORD nGuildMoney;
		short nGuildLevel;
		BYTE nLen;
		CString strGuildName;
		BOOL bGuild;

		nGuildMoney = GetDword(bypData, offset);
		nGuildLevel = GetShort(bypData, offset);
		nLen = GetByte(bypData, offset);
		if(nLen > 0)
		{
			LPTSTR t_str = strGuildName.GetBuffer(nLen);
			GetString(bypData, offset, (PBYTE)t_str, nLen);
			strGuildName.ReleaseBuffer();

			bGuild = TRUE;
		}
		else bGuild = FALSE;

		Dialog_GuildMenu1.ShowWindow(bGuild, strGuildName, nGuildMoney, nGuildLevel);
	}
	else
	{
		switch(bMenu)
		{
		case 1:			// 길드원인 경우
			Dialog_GuildMenu2.ShowWindow();
			break;
		case 2:			// 길드장인 경우
			Dialog_GuildMenu3.ShowWindow();
			break;
		}
	}
}