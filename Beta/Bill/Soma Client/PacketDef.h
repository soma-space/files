#ifndef __PACKET_DEF
#define __PACKET_DEF

//서버와 통신을 위한 Packet정의

#define PKT_LOGIN				0X01
#define PKT_LOGOUT				0x02
#define PKT_ACOUNT_LOGIN        0Xfe

#define PKT_MOVEFIRST			0X03
#define PKT_MOVEMIDDLE			0X04
#define PKT_MOVEEND				0X06

#define PKT_RUN_MOVEFIRST		0X15
#define PKT_RUN_MOVEMIDDLE		0X16
#define PKT_RUN_MOVEEND			0X17

//채팅
#define PKT_CHAT				0X07

#define PKT_USERMODIFY			0X09

#define PKT_ZONECHANGE			0X0A
#define PKT_GAMESTART           0x0b
#define PKT_RESTARTGAME         0x0c

//공격
#define PKT_ATTACK				0X10
#define PKT_ATTACK_ARROW		0X11  // 매직또는 화살도 통용
#define PKT_ATTACK_MAGIC_RAIL	0X12  // 방향을 가지고 타격을 입히면서 나가는 마법.
#define PKT_MAGIC_READY         0X13  // 마법을 사용하기 전 준비 
#define PKT_MAGIC_CIRCLE        0x14  // 범위 마법.
#define PKT_STATUS_INFO			0X19  // 캐릭터 상태정보
#define PKT_POISON				0x20  // 지속 매직에서 간격이 있는 독마법

#define PKT_DEAD                0x23// 죽었을때..
#define PKT_DEAD_INFO           0x24// 시체 정보
#define PKT_LIFE                0x25// 살아 날때..
                 
#define PKT_MAGIC_ALL			0x26	// 매직
#define PKT_MAGIC_BELT			0x27	// 벨트에 있는 매직
#define PKT_SET_TIME			0x29  //날짜,시간,비정보
#define PKT_SHOW_MAGIC			0x2a  //이벤트 그림

#define PKT_INV_ALL             0X30

#define PKT_CHANGE_ITEM_INDEX   0X32 
#define PKT_ITEM_INFO           0X33 
#define PKT_ITEM_THROW          0x34
#define PKT_ITEM_PICKUP         0x35
#define PKT_ITEM_FIELD_INFO     0x36

#define PKT_GETITEM             0x37  //아이템 획득
#define PKT_OUTITEM             0x38 // 인벤토리에서 아이템 빼낸다.
#define PKT_MONEYCHANGE			0x39  //돈이 바뀌었다.

#define PKT_WEIGHT				0x3a	// 무게를 준다.

#define PKT_ITEM_USE			0x3c	// 아이템 사용
#define PKT_ITEM_CHANGE_INFO	0x3d	// 아이템 정보 변경
#define PKT_ITEM_DUMP			0x3e	// 돈 버리기

#define PKT_NEW_HERO            0x40
#define PKT_DEL_HERO            0x41

#define PKT_CHARTER_DATA        0x50  // 유저 데이타...

#define PKT_CLIENTEVENT         0x60
    
#define PKT_SHOPOPEN            0x70

#define PKT_SELL_SHOP           0x72
#define PKT_BUY_SHOP            0x73

#define PKT_STORAGEOPEN			0x76
#define PKT_SAVEITEM			0x77
#define PKT_TAKEBACKITEM		0x78
#define PKT_SAVEMONEY			0x79
#define PKT_TAKEBACKMONEY		0x7a

#define PKT_MAKEROPEN			0x74	// 아이템 제작창 열림
#define PKT_MAKEITEM			0x75	// 아이템 제작 완료

#define PKT_TRADE_REQ			0x7b	// 개인 거래 요청을 받을 때
#define PKT_TRADE_ACK			0x7c	// 개인 거래 요청 승낙
#define PKT_TRADE_ADDITEM		0x7d	// 개인 거래시 아이템 추가
#define PKT_TRADE_SETMONEY		0x7e	// 개인 거래시 돈 셋팅
#define PKT_TRADE_OK			0x7f	// 개인 거래 확인
#define PKT_TRADE_CANCEL		0x80	// 개인 거래 취소
#define PKT_TRADE_RESULT		0x81	// 개인 거래 성공

#define PKT_AUCTION_SELL_OPEN	0x82	// 경매 Sell 열기
#define PKT_AUCTION_SELL_SEND	0x83	// 경매 아이템 보내기
#define PKT_AUCTION_SELL_RCV	0x84	// 경매 아이템 가져오기
#define PKT_AUCTION_BUY_OPEN	0x85	// 경매 Buy 열기
#define PKT_AUCTION_BUY_SEND	0x86	// 경매 Buy 올리기

#define PKT_PUTITEM_BELT		0x87	// Inv->Belt 아이템 놓기
#define PKT_PUTITEM_INV			0x88	// Belt->Inv 아이템 놓기

#define PKT_SELHOUSEPLACE		0x90	// 선택한 집터를 서버에 보냄
#define PKT_HOUSEPLACE			0x91	// 집터를 내려줌.

#define PKT_EVENTSELBOX         0x92    // 선택 메세지 관련
#define PKT_EVENTOKBOX          0x93    // 확인 메세지.
#define PKT_EVENTNORMAL         0x94    // 일반 메세지.
#define PKT_HPMP_RECOVERY       0x96    // Hp, Mp회복.

#define PKT_REPAIR_OPEN			0x99	// 수리창 열기
#define PKT_REPAIR_REQ			0xa0	// 수리 요청

#define PKT_BATTLEMODE			0x18 // 0 : 비무장 1이면 무장..

#define PKT_GUILD_OPEN			0x97	// 길드 메뉴 열기
#define	PKT_GUILD_NEW			0x95	// 길드 설립
#define PKT_GUILD_LIST			0x98	// 길드 리스트
#define PKT_GUILD_INFO			0x9f	// 길드 설명 요청
#define PKT_GUILD_REQ			0x9a	// 길드 가입 신청
#define PKT_GUILD_MOVE_REQ		0x9b	// 길드원 신청 처리
#define PKT_GUILD_REMOVE_REQ	0x9c	// 길드 가입 신청 취소
#define PKT_GUILD_DISSOLVE		0x9d	// 길드 해체
#define PKT_GUILD_REMOVE_USER	0x9e	// 길드 탈퇴/길드원 제명
#define PKT_GUILD_REQ_USER_INFO	0xa1	// 길드원 리스트
#define PKT_GUILD_MAN_LIST		0xa2	// 길드원 정보 신청
#define PKT_GUILD_MOVE_REJECT	0xa3	// 길드장 가입 신청 거절
#define PKT_GUILD_APPLICATION_LIST	0xa4	// 길드 신청자 리스트
#define PKT_CHANGE_MEMBER		0xa5	// 길드원간의 위치 변경
#define PKT_CHANGE_GUILD_POS	0xa6	// 길드 간부 호칭 변경
#define PKT_CHANGE_GUILD_SYMBOL	0xb5	// 길드 심볼 등록
#define PKT_CHANGE_GUILD_TAX	0xb6	// 길드 세금 변경
#define PKT_GIVE_DONATION		0xb9	// 기부금/연체 세금 내기
#define PKT_DISSOLVE_GUILD		0xbc	// 길드 해체 요청

// by krazi.
/////////////////////////////
#define BBS_OPEN				0x65
#define BBS_NEXT				0x66
#define BBS_READ				0x67
#define BBS_WRITE				0x68
#define BBS_EDIT				0x69
#define BBS_DELETE				0x6A
////////////////////////////////////////////////////

#endif