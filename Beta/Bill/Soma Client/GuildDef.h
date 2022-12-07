#ifndef _GUILDDEF_H_

#define GUILD_INVALID_GUILD_NAME		1		// 잘못된 길드 이름(길이)
#define GUILD_INVALID_GUILD_INFO		2		// 잘못된 길드 설명(길이)
#define GUILD_FULL						3		// 길드를 더이상 만들 수 없다
#define GUILD_SYSTEM_ERROR				4		// 시스템 에러(DB작업실패)
#define GUILD_ALREADY_JOIN				5		// 이미 길드에 가입되어 있다
#define GUILD_SMALL_LEVEL				6		// 길드 제작에 레벨이 미달
#define GUILD_SAME_GUILD_NAME			7		// 이미 같은 이름의 길드 존재
#define GUILD_ABSENT_JOIN				8		// 유저가 길드 정보 없음/길드명단에서 찾을 수 없음
#define GUILD_ABSENT_REQ_USER			9		// 길드 신청자 명단에서 유저 찾을 수 없음
#define GUILD_ABSENT_GUILD_NAME			10		// 요청된 길드 이름과 일치하는 이름을 가진 길드 없음
#define GUILD_SMALL_RANK				11		// 요청된 작업에 권한미달
#define GUILD_ABSENT_JOIN_REQ			12		// 가입 신청한 적이 없다
#define GUILD_ALREADY_JOIN_REQ			13		// 이미 가입신청을 했다
#define GUILD_NEED_EMPTY_RANK			14		// 계급 변동시 빈칸이 필요하다
#define GUILD_NOT_GUILD_USER			15		// 작업을 요청한 사람이 길드원이 아니다
#define GUILD_INVALID_RANK				16		// 유효하지 않은 랭크 또는 랭크 관련 데이터
#define GUILD_INVALID_GUILD_CALL_NAME	17		// 잘못된 길드 호칭(길이)
#define GUILD_SMALL_MONEY				18		// 길드 제작에 돈이 모자르다

#endif