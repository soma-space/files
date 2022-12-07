// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAIN_H__B26B4742_9495_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_RAIN_H__B26B4742_9495_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern int g_nAniFrame;

class CRain  
{
private:
	int m_nFrame;
	int m_nCurFrame;
	int m_nX;
	int m_nY;
	int m_nWidth;
	int m_nLength;
	DWORD m_dwDropsTime;
	BOOL m_bDropsEnabled;
	static int m_nState;
	int m_nCount;
	static int m_nRainNum;
	static DWORD m_dwTime;
	static DWORD m_dwStateTime;
	static BOOL m_bLightning;
	static int m_nAlphaGamma;
	static BOOL m_bReduce;
	static BOOL m_bIncrease;
	static BOOL m_bGammaApplyed;
	static CArray< WORD*, WORD* > m_arrPword;
	static CArray< _SPR_LSP *,_SPR_LSP *> m_arrSprLsp;

public:
	int m_nRainDropsX;
	int m_nRainDropsY;

	void SetPosition();
	void SetFrame(){ m_nFrame = g_nAniFrame ;}

	static BOOL Lightning( LPDIRECTDRAWSURFACE7 pSurface, COLORREF color );
	static BOOL LightningEx(LPDIRECTDRAWGAMMACONTROL lpDDGammaControl, DDGAMMARAMP* pDDGammaLightning, DDGAMMARAMP* pDDGammaRamp );
	static void SetLightning( BOOL bLightning ){ m_bLightning = bLightning;	}
	static BOOL GetLightning( ){ return m_bLightning; }
	static void SetState();
	static void SetRain(){ m_bReduce  = TRUE; m_bIncrease =FALSE; }
	static void SetEraseRain(){ m_bIncrease = TRUE; m_bReduce = FALSE; }
	static BOOL LoadRainDrops( TCHAR* strName );
	static int  GetRainNum(){ return m_nRainNum; }
//	static void PutPixelNormal(const WORD* pDest, const WORD* pSrc, int nCopyCount);
	static int  SetGammaRamp( LPDIRECTDRAWGAMMACONTROL lpDDGammaControl, DDGAMMARAMP* pDDGammaRamp, BOOL bGamma );
	BOOL RainMove( );
	BOOL RainDrops( BOOL bEarth, DDSURFACEDESC2 ddsd, CPoint dPos );
	void BltLSPNormal(DDSURFACEDESC2 ddsd, int x, int y, RECT* pRC, WORD* data);
	BOOL GetClippedRect(RECT *pRC, RECT* pRCClip);

	void RainDraw( DDSURFACEDESC2 ddsd );
	CRain();
	virtual ~CRain();
};
/*
inline void CRain::PutPixelNormal(const WORD* pDest, const WORD* pSrc, int nCopyCount)
{
	__asm
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;
MOV_LOOP:
		movsw;
		dec ecx;
		jnz MOV_LOOP;
	}
};
*/

#endif // !defined(AFX_RAIN_H__B26B4742_9495_11D4_956A_00D0B77259E8__INCLUDED_)
