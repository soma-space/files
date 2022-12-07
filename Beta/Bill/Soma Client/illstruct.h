#ifndef _ILLSTRUCT_H
#define _ILLSTRUCT_H

const unsigned long	DIR_LEFT =	0X00000001;
const unsigned long	DIR_RIGHT = 0X00000002;
const unsigned long	DIR_UP =	0X00000004;
const unsigned long	DIR_DOWN =	0X00000008;

#define MAX_RES_COUNT		19
#define MAX_ANI_COUNT		19

#endif
/**
#ifndef _ILLSTRUCT_H
#define _ILLSTRUCT_H

const int MAX_FRM = 32;		// 에니메이션당 최대 32 개 프레임
const int PF_555 = 555;
const int PF_565 = 565;
const int MAX_PART = 10; 	// 최대 10 개의 파트 스프라이트 처리

const unsigned long	DIR_LEFT =	0X00000001;
const unsigned long	DIR_RIGHT = 0X00000002;
const unsigned long	DIR_UP =	0X00000004;
const unsigned long	DIR_DOWN =	0X00000008;


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

typedef struct _ANI_DATA // 8방향 에니메이션 데이터
{
	char		szName[32];	// 에니메이션 이름
	int			nFlag;		// 에니메이션 플래그..
	float		fSpd;		// 스피드.. 단위는 Frame/Sec
	short int 	nMaxFrm;	// 방향별 최대 프레임
	short int*	pnFrms;		// 방향별 프레임 배열
} _ANI_DATA;

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
/*
typedef struct _SPR_NORMAL
{
	POINT	ptCV;			// 센터점
	POINT	ptCSH;			// 센터점 - 그림자
	POINT	ptCFX;			// 센터점 - 공격 효과
	RECT	rcV;			// 캐릭터 영역 - Part1
	RECT	rcSH;			// 그림자 영역
	RECT	rcFX;			// 공격 효과 영역
	_SPR_NORMAL const & operator = (_SPR_NORMAL const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}
} _SPR_NORMAL;
*/

/**
typedef struct _SPR_TOOL
{
	RECT	rcV;			// 캐릭터 영역 - 파트 1
	POINT	ptCV;			// 센터점 - 파트 1.
	_SPR_TOOL const & operator = (_SPR_TOOL const & other)
	{
		memcpy(this, &other, sizeof(*this));
		return *this;
	}

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




#endif
*/