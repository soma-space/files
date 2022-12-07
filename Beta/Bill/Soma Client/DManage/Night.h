// Night.h: interface for the CNight class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NIGHT_H__EE6270E2_A9C4_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_NIGHT_H__EE6270E2_A9C4_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define RED_MASK	0xff0000
#define GREEN_MASK	0xff00
#define BLUE_MASK	0xff

#ifndef LIGHTINFO
#define LIGHTINFO
typedef struct TAG_LIGHT_INFO
{
	int m_kind;
	CPoint m_pt;
}LIGHT_INFO;
#endif

class CColorVertex;
class CDDSurface;
class  AFX_EXT_CLASS CNight  
{
private:
	CColorVertex*			m_stColorV;
	DWORD					m_dwAlpha;//전에 쓰던거
	int						m_nAlpha;//지금 쓰는거
	short					m_gap;//일몰이나 일출중에 접속시 중간단계
	int						m_nVillageAlpha;
	int						m_nPreAlpha;
	BOOL					m_bVEnabled;
	int						m_nNightType;//1:일반 2:red 3:green 4:blue
	BOOL					m_bNightEnabled;
	int						m_nWidthRate;
	int						m_nHeightRate;
	float					m_ScreenX;
	float					m_ScreenY;
	LPDIRECT3DDEVICE7		m_lpD3DD7;
	D3DTLVERTEX				m_SpriteVertices[4];
	int						m_VGAType;
	DWORD					m_SorBlend;
	DWORD					m_DesBlend;
	DWORD					m_Alphaarg1;
	DWORD					m_dwALPHAMASK1;
	DWORD					m_dwALPHAMASK2;
	CPoint					m_ptPrePos;
	CPoint					m_ptMyPos;
	CPoint					m_ptLightStain;


	BYTE  m_aaiLightMap[600][800];
	BYTE  m_aaiLightMap1[600][800];
	BYTE  m_aaiLightMap2[600][800];
	

public:
	int						m_nPreDayState;
	CNight(int nWidthRate, int nHeightRate, float ScreenX, float ScreenY, LPDIRECT3DDEVICE7 lpD3DD7, int nVGAType);
	CNight(int PF_565);
	virtual ~CNight();

	void	SetDevice(LPDIRECT3DDEVICE7 lpD3DD7){ m_lpD3DD7 = lpD3DD7;}
	BOOL	DrawNight();
	void	SetNightState( int nDayState );
	void	SetNightStateEx( int nDayState, int type );
	BOOL	SetTransFormNight(int type);
	void	SetFirstNightState( short gap ){ m_gap = gap/2; m_nPreDayState = 0;}
	void	InitColorVertex();
	void	SetLight( CPoint pt, int illuminance, COLORREF color = 0x00 );
	BOOL	DrawNight( CDDSurface* pSurface, CArray< LIGHT_INFO, LIGHT_INFO>* ptLightPos, int y_screen, BOOL bRight );
	void	InitLightMap();
	CPoint	LightStain(BOOL bRight,CPoint pt,CPoint ptMapPos);
	void	SetLightStain();
	int		GetNightState();

	static void PutPixelNADark( const DWORD *pDest, int nCopyCount);
	static void PutPixelDark( const DWORD *pDest, int nAlphaSrc, int nCopyCount, DWORD ALPHAMASK1,DWORD ALPHAMASK2);
	static void PutPixelDark( const DWORD *pDest, BYTE* AlphaSrc, int nCopyCount, DWORD ALPHAMASK1,DWORD ALPHAMASK2);

};

inline void CNight::PutPixelNADark( const DWORD *pDest, int nCopyCount)
{
	__asm
	{
		mov edi, pDest;
		mov ecx, nCopyCount;
COPY_LOOP:
		mov dword ptr[edi], 0;
		add edi, 4;
		dec ecx;
		jnz COPY_LOOP;
	}
};
inline void CNight::PutPixelDark( const DWORD *pDest, int nAlphaSrc, int nCopyCount, DWORD ALPHAMASK1,DWORD ALPHAMASK2)
{
	__asm
	{
		mov edi, pDest;
		mov ecx, nCopyCount;//
		//mov ebx, 0;
COPY_LOOP:
		mov eax, dword ptr [edi];
		and eax, ALPHAMASK1;
		shr eax,5;
		mul nAlphaSrc;
		and eax, ALPHAMASK1;
		mov ebx, eax;

		mov eax, dword ptr [edi];
		and eax, ALPHAMASK2;
		mul nAlphaSrc;
		shr eax,5;
		and eax, ALPHAMASK2;
		or eax, ebx;
		mov dword ptr[edi], eax;

		add edi, 4;
		dec ecx;
		jnz COPY_LOOP;
	}
};

inline void CNight::PutPixelDark( const DWORD *pDest, BYTE* AlphaSrc, int nCopyCount, DWORD ALPHAMASK1,DWORD ALPHAMASK2)
{
	__asm
	{
		mov edi, pDest;
		mov esi, AlphaSrc;
		mov ecx, nCopyCount;//
		//mov ebx, 0;
COPY_LOOP:
		mov eax, dword ptr [edi];
		and eax, ALPHAMASK1;
		shr eax,5;
		mov dh, byte ptr [esi];
		mul dx;
		and eax, ALPHAMASK1;
		mov ebx, eax;

		mov eax, dword ptr [edi];
		and eax, ALPHAMASK2;
		mul dh;
		shr eax,5;
		and eax, ALPHAMASK2;
		or eax, ebx;
		mov dword ptr[edi], eax;

		add edi, 4;
		add esi, 1;
		dec ecx;
		jnz COPY_LOOP;
	}
};

#endif // !defined(AFX_NIGHT_H__EE6270E2_A9C4_11D4_956A_00D0B77259E8__INCLUDED_)
/*
inline void CNight::PutPixelDark( const DWORD *pDest, int nAlphaSrc, DWORD ALPHAMASK1,DWORD ALPHAMASK2)
{

	__asm
	{
		mov edi, pDest;
		//mov ebx, 0;
		mov eax, dword ptr [edi];
		and eax, ALPHAMASK1;
		shr eax,5;
		mul nAlphaSrc;
		and eax, ALPHAMASK1;
		mov ebx, eax;

		mov eax, dword ptr [edi];
		and eax, ALPHAMASK2;
		mul nAlphaSrc;
		shr eax,5;
		and eax, ALPHAMASK2;
//		and ebx, eax;
		or eax, ebx;
//		mov dword ptr[pDest], ebx;
		mov dword ptr[edi], eax;

	}
}
*/

