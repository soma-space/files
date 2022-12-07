#if !defined(AFX_MYCONSTANDDEF_H_INCLUDED_)
#define AFX_MYCONSTANDDEF_H_INCLUDED_

// 스크립트 겸 메시지
// 모든 캐릭터 포인터는 스크립터에서 캐릭터 번호로 대치될 수도 있다.
const int SCT_TALK = 1000; // wParam 문자열 포인터 lParam - LOWORD,F1:캐릭터 번호|HIWORD:플래그 ... F2:0:풍선,1:대화창,2:메시지창,4:스크린 고정,8:맵에고정,16:지정된캐릭터에 고정

const int SCT_IMAGE_OUTPUT = 2000; // 비트맵 이미지 출력 wParam - 파일 이름 문자열 포인터, Pt,lParam - POINT 형 포인터
const int SCT_AVI_PLAY = 2100; // 동영상 파일 출력 wParam - 파일 이름 문자열 포인터, Rect,lParam - RECT형 포인터

const int SCT_GAME_PAUSE = 2200; // 게임 잠시 멈춤 F1,wParam - 0:시간,1:마우스클릭,2:무한정멈춤, Time,lParam - 시간일 경우 DWORD 단위 mSec
const int SCT_GAME_RESUME = 2300; // 게임 잠시 멈춤 해제. 인자 없음

const int SCT_LOAD = 3000; // 스크립트 로드 wParam - 파일 이름 문자열 포인터
const int SCT_PAUSE = 3100; // 스크립트 잠시 멈춤 F1,wParam - 0:시간,1:마우스클릭,2:무한정멈춤, Time,lParam - 시간일 경우 DWORD 단위 mSec
const int SCT_RESUME = 3200; // 스크립트 잠시 멈춤 해제. 인자 없음
const int SCT_END = 3300; // 스크립트 끝 - 인자 없음

const int SCT_PROCESS_NEXT = 3400; // 다음 스크립트 진행
const int SCT_JUMP = 3500; // 현재 위치에서 스크립트 점프 F1,wParam - 스크립트 번호
const int SCT_JUMP_IF = 3600; // 전역변수의 값에 따라 스크립트 점프 F1,wParam - 판단의 근거가 되는 스크립트 번호 F2,F3, lParam - LOWORD:참일때|HIWORD:거짓일때
const int SCT_SET_GLOBAL_VAR = 3700; // F1,wParam - 변수 번호 F2,lParam - 참,거짓
const int SCT_JUMP_RANDOM = 3800; // 현재 위치에서 임의의 위치로 스크립트 점프 F1,F2,wParam - HIWORD, LOWORD, F3,lParam - HIWORD 스크립트 번호

const int SCT_SOUND_PLAY = 4000; // 사운드 재생 wParam - 파일 이름 문자열 포인터. lParam - LOWORD,F1-0:임시채널재생,1:스트리밍재생 HIWORD,F2-0:루프,1:한번만
const int SCT_SOUND_STOP = 4100; // 사운드 재생 멈춤 lParam,F1-0:스트리밍채널,1:임시 채널 

const int SCT_MODE_CHANGE = 5000;

const int SCT_MAP_LOAD_ATTR = 6000; // 맵 속성(길찾기 및 오브젝트데이터 포함) 로딩 wParam - 파일 이름 문자열 포인터 F1,lParam - 0:메인배경 1:서브배경
const int SCT_MAP_LOAD_SETTING = 6100; // 맵 세팅(캐릭터 배치 및 영역 배치 포함) 로딩 wParam - 파일 이름 문자열 포인터 F1,lParam - 0:메인배경 1:각 서브배경
const int SCT_MAP_RETURN = 6200; // 주 배경으로 복귀
const int SCT_MAP_SCROLL = 6300; // 맵 스크롤 F1,wParam - 0:보통 1:순간이동 Pt,lParam - POINT 형 포이터

const int SCT_CHR_SET_ANI_OBJ = 7000; // 캐릭터에 Animation Object 포인터 할당 wParam - Animation Object 포인터 lParam - 살아있는지.. !!!스크립트에선 안 쓴다.
const int SCT_CHR_SET_DATA = 7100; // 캐릭터에 데이터 설정 wParam - 맵 등록 캐릭터 데이터 구조체 포인터 !!!스크립트에선 안 쓴다.
const int SCT_CHR_SET_ATTACK = 7200; // 캐릭터 공격 대상 설정 wParam - 공격 하는 캐릭터 포인터(번호) lParam - 공격 받을 캐릭터 포인터(번호)
const int SCT_CHR_HITTED = 7300; // 캐릭터 공격 받음 wParam - 공격 대상 캐릭터 포인터(번호)
const int SCT_CHR_SET_DIRECTION = 7400; // 캐릭터 방향 바꿈 F1,wParam - 캐릭터 번호 F2,lParam 방향 번호
const int SCT_CHR_SET_ANIMATION = 7500; // 캐릭터 에니메이션 wParam - 캐릭터 번호 lParam - LOWORD:에니메이션 번호,HIWORD:16-루프|32-한번만
const int SCT_CHR_SET_MOVE = 7600; // 캐릭터 이동 wParam - F1,LOWORD-캐릭터번호, F2,HIWORD-이동방법:0보통|1순간이동|8뛰어감 F3:0다음 스크립트 진행 1:이동 끝날때까지 기다림. lParam(Point) - POINT형 포인터
const int SCT_CHR_STOP_MOVE = 7700; // 캐릭터 이동 멈춤 인자 없음
const int SCT_CHR_SET_PARTY = 7800; // 파티 캐릭터로 지정 wParam - 캐릭터 포인터(번호) lParam - 1 ~ 6 파티
const int SCT_CHR_SET_CTRL = 7900; // 파티 캐릭터일 경우 컨트롤 지정  wParam - 캐릭터 포인터(번호) lParam - true || false

const int SCT_FX_ENV_BEGIN = 8000; // 환경 렌더링 시작 wParam - 효과 번호 lParam - 지속 시간 DWORD mSec
const int SCT_FX_ENV_END = 8100; // 환경 렌더링 끝 wParam - 효과 번호
const int SCT_RENDER_MAIN = 8200; // 전체 렌더링 시작(FadeIn,FadeOut) wParam,F1 - 참,거짓

const int SCT_INTF_SET = 9000; // 인터페이스 레이어와 버튼 누름 설정 wParam - F1,LOWORD - 인터페이스 번호, F2,HIWORD 레이어 번호 lParam,F3 - 버튼 번호

const int MAX_SCT_COMMAND = 64;
const int MAX_SCT = 1024;

typedef struct _SCT_HEADER
{
	char szID[64];
	float fVersion;
	int nQt;
	_SCT_HEADER const & operator = (_SCT_HEADER const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _SCT_HEADER;

typedef struct _SCT_DATA
{
	int		nCommand;	// 명령어
	int		nFlag1;		// 플래그1
	int		nFlag2;		// 플래그2
	int		nFlag3;		// 플래그3
	POINT	pt;			// 지점1
	RECT	rc;			// 영역
	float	fSpeed;		// 속도
	DWORD	dwTime;		// mSec 단위 시간
	int		nText;		// 텍스트 데이터 길이
	char*	szText;		// 텍스트 데이터
	_SCT_DATA const & operator = (_SCT_DATA const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _SCT_DATA;





const int MAX_SPR = 1024; 	// 최대 1024 개의 스프라이트 처리
const int MAX_ANI = 512;	// 최대 64 개의 에니메이션 처리
const int MAX_FRM = 32;		// 에니메이션당 최대 32 개 프레임
const int MAX_ANI_CTRL = 1024;	// 한 오브젝트 당 최대 1024 개의 독립적인 에니메이션 처리..

const int FL_ANI_LOOP = 0; // 계속..
const int FL_ANI_ONCE = 1;  // 한번만 돌고 원래 에니로 돌아옴..
const int FL_ANI_ONCE_FIX = 2; // 에니메이션 한번돌고.. 고정

const int PF_555 = 555;
const int PF_565 = 565;

const int MAX_TILE_MAP_LAYER = 6;
const int MAX_TILE_MAP_WIDTH = 2048;
const int MAX_TILE_MAP_HEIGHT = 2048;

const int NODE_TILE_SET = 1;
const int NODE_TILE_SUB_SET = 2;

const int RENDER_NORMAL = 0; // 보통
const int RENDER_TRANS = 1; // 반투명
const int RENDER_ALPHA_ADD = 2; // 
const int RENDER_ALPHA_SOFT = 3; // 
const int RENDER_SHADOW = 4; // 그림자 찍기
const int RENDER_DARK = 5; // 어둡게 찍기
const int RENDER_COLOR = 6; //  컬러값 첨가해 찍기
const int RENDER_COLOR_LEVEL = 7; // 블루 값을 기준으로 컬러변형 찍기

const int MAX_OBJ_ANI = 8; //  한 객체당 들어가는 에니메이션 객체 수

typedef struct _ANI_HEADER
{
	char	szID[4];		// 'A' 'N' 'I' 방향 숫자(1,8) 등등
	char	szRemark[64];	// 주석
	int		nDirQt;			// 방향
	int		nQt;			// Animation 숫자
	_ANI_HEADER const & operator = (_ANI_HEADER const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _ANI_HEADER;

typedef struct _SPR_HEADER
{
	char	szID[4];		// 'SPLN' 보통 LSP Sprite | 'SPLC' 확장된 LSP Sprite || 'SPRN' 보통 Sprite
	char	szRemark[64];	// 주석
	char	szBMPFN[64];	// 비트맵 파일 이름 - 데이터를 갖고 있으면 안 쓴다.
	int		nWidth;			// 너비
	int		nHeight; 		// 높이
	COLORREF SrcCK;			// 소스 컬러키
	int		nQt;			// 스프라이트 수
	_SPR_HEADER const & operator = (_SPR_HEADER const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _SPR_HEADER;

typedef struct _SPR_TOOL
{
	RECT	rcV;			// 캐릭터 영역 - 파트 1
	POINT	ptCV;			// 센터점 - 파트 1.
} _SPR_NORMAL;

typedef struct _SPR_LSP
{
	RECT	rcV;	// 보이는 영역 - Clipping 에 쓰임
	int 	nLine;	// LSP 줄수
	int		nDataSize;	// LSP Data Size
	WORD*	pwV;		// LSP Data
	_SPR_LSP const & operator = (_SPR_LSP const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _SPR_LSP;

typedef struct _LSP_ENCODE_BUFF
{
	_LSP_ENCODE_BUFF* Prev; // 링크드 리스트 형태로 되어 있다.
	int QtZero;				// 0 픽셀 수
	int QtPixel;			// 유효 픽셀 수
	WORD Pixel[1024];		// 1024 픽셀까지 처리 가능
	_LSP_ENCODE_BUFF* Next;
	_LSP_ENCODE_BUFF const & operator = (_LSP_ENCODE_BUFF const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _LSP_ENCODE_BUFF;

/*
// LSP Data 구조
		+--- 2바이트 - 줄에 들어가는 노드수
        |       |
        |       +-----+--- 2바이트 - 노드 투명색수(찍을때 건너뛰는길이)
        |       |     |
        |       |     +--- 2바이트 - 노드 픽셀수
        |       |              |
        |       |              +--- 2바이트 - 실제 픽셀
        |       |              +---        .
        |       |              +---        . 노드 픽셀수 만큼 반복
        |       |              +---        .
        |       +--------
        |            .
        |            . 노드수만큼 반복
        |            .
        +-------- 
		    .
		    . // 줄 수만큼 반복
		    .
*/

typedef struct _ANI_DATA // 8방향 에니메이션 데이터
{
	char		szName[32];	// 에니메이션 이름
	int			nFlag;		// 에니메이션 플래그..
	float		fSpd;		// 스피드.. 단위는 Frame/Sec
	short int 	nMaxFrm;	// 방향별 최대 프레임
	short int*	pnFrms;		// 방향별 프레임 배열
} _ANI_DATA;

typedef struct _OBJ_HEADER // 캐릭터 헤더..
{
	char szID[4]; // 'O' 'B' 'J' 'D'
	char szRemark[64]; // 주석
	char szAniFN[64]; // 에니메이션 파일 이름
	int nSprQt; // 스프라이트 파일 갯수
	char szSprFNs[MAX_OBJ_ANI][64]; // 스프라이트 파일 이름들...
	_OBJ_HEADER const & operator = (_OBJ_HEADER const &other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _OBJ_HEADER;

typedef struct _BMP16_HEADER
{
	char	szID[4]; // 반드시 'B', 'M', 'P', 16
	char	szRemark[64]; // 주석..
	int		nWidth;
	int		nHeight;
	_BMP16_HEADER const & operator = (_BMP16_HEADER const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _BMP16_HEADER;

const int TILE_SIZE = 32; // 타일 사이즈...
const int TILE_SHIFT = 5; // change this to reflect the the size. // Ex. 32x32 tile equals 2^5. or a TILE_SHIFT of 5

typedef struct _TILE_MAP_HEADER
{
	char szID[4]; // 'T' 'M' 'D' NULL;
	char szRemark[64]; // 이름??
	int nWidth; // -1 이면 쓰지 않는다..
	int nHeight; // -1 이면 쓰지 않는다..
	char szBMPFNs[MAX_TILE_MAP_LAYER][64]; // 비트맵 파일 이름... 게임 데이터에서만 쓰인다.
	COLORREF crCKs[MAX_TILE_MAP_LAYER]; // 컬러키
	int nResrcQts[MAX_TILE_MAP_LAYER]; // 타일 리소스 갯수..
	_TILE_MAP_HEADER const & operator = (_TILE_MAP_HEADER const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} TILE_MAP_HEADER, *LPTILE_MAP_HEADER;

typedef struct _TILE_MAP_DATA
{
	short int snIndex; // -1 이면 쓰지 않는다.. 타일 리소스 번호.. 32 개가 한줄이다.
	short int snFlags; // 0 - 아무속성 없음. 1 - 충돌 2 - 4 - 8 - 16 - 32 - ......
} _TILE_MAP_DATA, *LPTILE_MAP_DATA;

/*
typedef struct _TILE_MAP_OBJ_HEADER
{
	char szBMPFNs[MAX_TILE_MAP_OBJ_REG][64];
	short int nX; // -1 이면 쓰지 않는다..
	short int nY; // -1 이면 쓰지 않는다..
} TILE_MAP_OBJ_HEADER, *LPTILE_MAP_OBJ_HEADER;

typedef struct _TILE_MAP_OBJ_DATA
{
	short int nIndex;
	short int nX; // -1 이면 쓰지 않는다..
	short int nY; // -1 이면 쓰지 않는다..
} TILE_MAP_OBJ_DATA, *LPTILE_MAP_OBJ_DATA;
*/

typedef struct _TILE_SET_HEADER
{
	char	szName[64];
	char	szBMPFN[64];
	COLORREF crCK; // 컬러키..
	int		nQtSubSet; // 타일셋 갯수
} TILE_DATA_TOOL, *LPTILE_DATA_TOOL;

const int MAX_MAP_OBJ_REG = 64;
const int MAX_MAP_OBJ = 512;

typedef struct _MAP_OBJ_HEADER
{
	char	szID[4]; // 'M' 'P' 'O' 'D'
	char	szRemark[64]; // 주석
	int		nWidth; // 너비
	int		nHeight; // 높이
	char	szRegObjAniFNs[MAX_MAP_OBJ_REG][64];
	char	szRegObjSprFNs[MAX_MAP_OBJ_REG][64];
	_MAP_OBJ_HEADER const & operator = (_MAP_OBJ_HEADER const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _MAP_OBJ_HEADER;

typedef struct _GAME_RESOURCE_DATA
{
	char szMapFN[128];
	char szObjFN[128];
	char szCursorFN[128];
	char szItemFN[128];
	char szIntfFNs[32][128];
	char szSoundBkgFNs[8][128];
	char szSoundEffectFNs[64][128];
} _GAME_RESOURCE_DATA;

const int MAX_LOOK = 64; // 최대 얼굴 표정수
const int MAX_LOOK_PART = 64; // 표정 구성 최대 파트수 - 눈, 코, 입, 얼굴....머리 등등...
const int MAX_LOOK_PART_ANI = 64; // 얼굴 한 파트당 최대 갯수 - 눈으로 따지면 64개의 눈 종류가 있다..

typedef struct _LOOK_HEADER
{
	char		szID[4]; // 'L' 'A' 'D' 'F' 식별자..
	char		szRemark[64]; // 주석
	int			nQt;
	char		szAniFN[64]; // 에니메이션 파일 이름..
	char		szSprFN[64]; // 스프라이트 파일 이름.
	_LOOK_HEADER const & operator = (_LOOK_HEADER const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _LOOK_HEADER;

typedef struct _LOOK_DATA
{
	char szRemark[64]; // 주석..
	int nQtPart; // 각 부분 갯수
	int nQtAnisPerParts[MAX_LOOK_PART]; // 각 부분 조합 최대 갯수
	int nPeriods[MAX_LOOK_PART]; // 에니메이션 주기 - 이 시간마다 한번씩 반복.
	int nPeriodsRnd[MAX_LOOK_PART]; // 에니메이션 주기 랜덤값....
	int nAnis[MAX_LOOK_PART][MAX_LOOK_PART_ANI];
	_LOOK_DATA const & operator = (_LOOK_DATA const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _LOOK_DATA;

// 인터페이스
const int MAX_LAYER = 64;
const int MAX_INTF = 256;

typedef struct _INTF_HEADER
{
	char		szID[4]; // 'I' 'N' 'T' 'F' 식별자..
	int			nQt; // 인터페이스 갯수
	char		szRemark[64]; // 주석
	char		szAniFN[64]; // 에니메이션 파일 이름..
	char		szSprFN[64]; // 스프라이트 파일 이름.
	RECT		rcWhole; // 인터페이스 영역
	_INTF_HEADER const & operator = (_INTF_HEADER const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _INTF_HEADER;

typedef struct _INTF_DATA
{
	POINT		ptView;	// 찍기 위치
	RECT		rcRgn;	// 마우스 체크 영역
	int			nParentLayer; // 어느레이어에 속해있는지....
	int			nType; // 보통, 버튼, 스크롤바좌우, 스크롤바상하, 드래그, 다이얼, 컬러영역, 0,1,2,4,8,16 .....
	int			nFlags[4]; // 플래그 4개
	COLORREF	crFill;	// 채울 색... A,B,G,R
	int			nAnis[4]; // 에니메이션 번호.. 배열 순대로 보통상태, 마우스가 위로 올라감, 선택됨, 누름 상태
	int			nRenderTypes[4]; // 찍는 방법.. RENDER_NORMAL, _TRANS, _ALPHA_SOFT, _ALPHA_ADD, _SHADOW, _DARK
	int			nPos1; // 버튼일때 0 - 보통, 1 - 마우스 밑, 2 - 눌림, 3 - 눌렸음
	int			nPos2; // 버튼일때 0 - 보통, 1 - 마우스 밑, 2 - 눌림, 3 - 눌렸음 --- 주로 전의 상태를 나타낼때 씀..
	char		szName[32]; // 설명 - 나중에 팁으로 나타난다.
	int			nText; // 텍스트 데이터 길이
	char*		szText; //  텍스트 데이터
} _INTF_DATA;

const int INTF_TYPE_NORMAL = 0;
const int INTF_TYPE_BUTTON_NORMAL = 1;
const int INTF_TYPE_BUTTON_TOGGLE = 2;
const int INTF_TYPE_BUTTON_JUMP = 3;
const int INTF_TYPE_BUTTON_RADIO = 4;
const int INTF_TYPE_SCROLL_LR = 5;
const int INTF_TYPE_SCROLL_TB = 6;
const int INTF_TYPE_DIAL_LR = 7;
const int INTF_TYPE_DIAL_TB = 8;
const int INTF_TYPE_PROGRESS = 9;
const int INTF_TYPE_COLOR_BOX = 10;
const int INTF_TYPE_TEXT_IN = 11;
const int INTF_TYPE_TEXT_OUT = 12;
const int INTF_TYPE_LIST_BOX = 13;
const int INTF_TYPE_COMBO_BOX = 14;
const int INTF_TYPE_DRAG_RGN = 15;

// 마우스 플래그
const int MOUSE_LBCLICK = 1;
const int MOUSE_LBCLICKED = 2;
const int MOUSE_LBDOWN = 4;
const int MOUSE_MBCLICK = 8;
const int MOUSE_MBCLICKED = 16;
const int MOUSE_MBDOWN = 32;
const int MOUSE_RBCLICK = 64;
const int MOUSE_RBCLICKED = 128;
const int MOUSE_RBDOWN = 256;
const int MOUSE_LBDBLCLK = 512;
const int MOUSE_MBDBLCLK = 1024;
const int MOUSE_RBDBLCLK = 2048;

const int MAX_ITEM = 1024;
typedef struct _ITEM_HEADER
{
	char	szID[4]; // 'I' 'T' 'E' 'M' 식별자..
	char	szRemark[64]; // 주석
	int		nQt;
	char	szAniFN[64]; // 에니메이션 파일 이름..
	char	szSprFN[64]; // 스프라이트 파일 이름.
	_ITEM_HEADER const & operator = (_ITEM_HEADER const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _ITEM_HEADER;

typedef struct _ITEM_DATA
{
	char		szName[32];
	int			nAni; // 할당된 에니메이션
	short int	snWidth; // 아이템 너비
	short int	snHeight; // 아이템 높이
	short int	snRemark; // 설명 길이
	char*		szRemark; // 설명..
	short int	snRemarkNPC; // 점원 설명 길리
	char*		szRemarkNPC; // 점원의 설명..
	short int	snStrength; // 체력
	short int	snSpirit; // 근성
	short int	snCharm; // 매력
	short int	snIQ; // 
	short int	snEQ; // 
	short int	snLuck; // 행운
	short int	snExtra0; // 예비0
	short int	snExtra1; // 예비1
	short int	snExtra2; // 예비2
	short int	snExtra3; // 예비3
	short int	snSpecial0; // 특수능력0
	short int	snSpecial1; // 특수능력1
	short int	snSpecial2; // 특수능력2
	short int	snSpecial3; // 특수능력3
	short int	snDurability; // 지속시간 - mSec 단위
	
	short int	snStrengthNeed; // 체력
	short int	snSpiritNeed; // 근성
	short int	snCharmNeed; // 매력
	short int	snIQNeed; // 
	short int	snEQNeed; // 
	short int	snLuckNeed; // 행운
	short int	snExtra0Need; // 예비0
	short int	snExtra1Need; // 예비1
	short int	snExtra2Need; // 예비2
	short int	snExtra3Need; // 예비3
	_ITEM_DATA const & operator = (_ITEM_DATA const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _ITEM_DATA;


const int MAX_HOUSE_DATA = 512; // 집데이터 갯수...

typedef struct _HOUSE_HEADER // 캐릭터 헤더..
{
	char	szID[4]; // 'M' 'H' 'D' NULL
	char	szRemark[64]; // 주석
	int		nQt; // 스프라이트 파일 갯수
} _HOUSE_HEADER;

typedef struct _HOUSE_DATA
{
	RECT rcRgn; // 영역 - 32 단위 이다.
	int nNum;	// 집번호
} _HOUSE_DATA;
#endif // AFX_MYCONSTANDDEF_H_INCLUDED_