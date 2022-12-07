// DDEngine.h: interface for the CDDEngineNew class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDENGINE_H__CF5FF875_6921_4E64_904F_C074E41AD179__INCLUDED_)
#define AFX_DDENGINE_H__CF5FF875_6921_4E64_904F_C074E41AD179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
//#include <DDraw.h>
//#include <windows.h>
#include "MyMacro.h"
#include "MyConstAndDef.h"

#include "ddraw.h"	// Added by ClassView

//int RENDER_NORMAL = 0; // 보통

class CDDEngineNew  
{
protected:
public:
	LPDIRECTDRAW			m_lpDD;
	LPDIRECTDRAWSURFACE7	m_lpDDSPri;
	LPDIRECTDRAWSURFACE7	m_lpDDSBB;
	LPDIRECTDRAWCLIPPER		m_lpDDClipper;
	
	LPDIRECTDRAW7			m_lpDD4;

//	DirectDrawCreate(NULL,&m_lpDD,NULL);
//	m_lpDD->QueryInterface(IID_IDirectDraw4, (void**)&m_pIDD4);

	HWND m_hWnd;
	BOOL m_bFullScreen;

	int m_nFrmRate; // 초당 프레임수..

	RECT m_rcScr; // 화면상에 나타날 영역
	WORD*	m_pwBB; // 백버퍼 포인터
	int		m_nPitchBB; // 백버퍼 피치 - 워드 단위 -> 주의 할 것..!!

	DDSURFACEDESC2 m_ddsdTmp1, m_ddsdTmp2; // 임시로만 쓰이는 서피스 구조체.. -> 정말 임시로만 쓰여야 한다..

	int m_nPF; // 픽셀 포맷
	DWORD m_dwRBMask, m_dwRMask, m_dwGMask, m_dwBMask; // 각 색상 마스크..

	char m_szPath[128]; // 현재 지정된 경로
	BOOL	m_bBBLocked; // 현재 백버퍼에 락이 걸려 있는지..

	WORD m_wRTbls[32]; // 레드 테이블
	WORD m_wGTbls[32]; // 그린 테이블
	WORD m_wBTbls[32]; // 블루 테이블
public:
	// 예비 버퍼 생성.
	LPDIRECTDRAWSURFACE7 m_lpDDSExtra; // 예비용 버퍼.. lsp 스프라이트를 작게 찍는 다거나 할때 쓴다.

public: // 인라인 함수만 모아두자..
	int		FrmRateGet() { return m_nFrmRate; } // 프레임 비율
	void	ScreenGetSize(RECT* pRC) { *pRC = m_rcScr; }
	const	LPDIRECTDRAW GetDirectDraw() const { return m_lpDD; }
	int		PixelFormatGet() { return m_nPF; }

	WORD RGB16(COLORREF crColor) // 컬러 레퍼런스 값을 현재 픽셀 포맷에 맞추어 16비트 값으로..
	{
		if(m_nPF == PF_555) return (WORD) (	(((crColor&0x000000ff)>>3)<<10) | ((((crColor&0x0000ff00)>>8)>>3)<<5) | (((crColor&0x00ff0000)>>16)>>3) );
		else if(m_nPF == PF_565) return (WORD) ( (((crColor&0x000000ff)>>3)<<11) | ((((crColor&0x0000ff00)>>8)>>2)<<5) | (((crColor&0x00ff0000)>>16)>>3) );
		else return -1;
	}
	
	WORD RGB16(WORD wR, WORD wG, WORD wB) // R,G,B 값을 현재 픽셀 포맷에 맞추어 16비트 값으로..
	{
		if(m_nPF == PF_555) return (((wR>>3)<<10) | ((wG>>3)<<5) | (wB>>3));
		else if(m_nPF == PF_565) return (((wR>>3)<<11) | ((wG>>2)<<5) | (wB>>3));
		else return -1;
	}
	
	COLORREF RGB24(WORD wColor) // 현재 픽셀 포맷16비트 값을 컬러 레퍼런스 값으로..
	{
		COLORREF crColor = wColor;
		if(m_nPF == PF_555) return ( (((crColor&0x00007c00)>>10)<<3) | ((((crColor&0x000003e0)>>5)<<3)<<8) | (((crColor&0x0000001f)<<3)<<16) );
		else if(m_nPF == PF_565) return ( (((crColor&0x0000f800)>>11)<<3) | ((((crColor&0x000007e0)>>5)<<2)<<8) | (((crColor&0x0000001f)<<3)<<16) );
		else return (crColor = -1);
	}

	HRESULT SurfaceLock(LPDIRECTDRAWSURFACE7 lpDDS, DDSURFACEDESC2* pDDSD, RECT* pRC = NULL) // 서피스 락.. DMA 이용은 반드시 시스템 메모리에 생성되었을 경우에만 한다..
	{
		MAKE_ZERO(*pDDSD);
		if(lpDDS == NULL) return -1;
		pDDSD->dwSize = sizeof(DDSURFACEDESC2);
		return lpDDS->Lock(pRC, pDDSD, DDLOCK_WAIT, NULL);
	}
	HRESULT SurfaceUnlock(LPDIRECTDRAWSURFACE7 lpDDS, RECT* pRC = NULL)
	{
		if(lpDDS == NULL) return -1;
		return lpDDS->Unlock(pRC);
	}

	void Conv565_0PixelTo555Pixel(WORD* pDest, const WORD* pSrc, int nCount) // 565 픽셀을 555 픽셀로 카운트 만큼 변환해준다.
	{
		for(int i = 0; i < nCount; i++, pDest++, pSrc++)
		{
			*pDest = ((*pSrc&0xffc0)>>1) | (*pSrc&0x001f);
		}
	}
	void Conv565_1PixelTo555Pixel(WORD* pDest, const WORD* pSrc, int nCount) // 565 픽셀을 555 픽셀로 카운트 만큼 변환해준다.
	{
		for(int i = 0; i < nCount; i++, pDest++, pSrc++)
		{
			if( *pDest == 0x00 || *pDest == 0xf81f )
				*pDest = ((*pSrc&0xffc0)>>1) | (*pSrc&0x001f);
			else
				*pDest = ((*pSrc&0xffc0)>>1) | (*pSrc&0x001f) | 0x8000;
		}
	}
	void Conv565_0SetPixel(WORD* pDest, const WORD* pSrc, int nCount) // 565 픽셀을 555 픽셀로 카운트 만큼 변환해준다.
	{
		for(int i = 0; i < nCount; i++, pDest++, pSrc++)
		{
			if( *pSrc & 0x20 )
				*pDest = *pSrc - 0x20;
			else *pDest = *pSrc;
		}
	}
	void Conv565_1SetPixel(WORD* pDest, const WORD* pSrc, int nCount) // 565 픽셀을 555 픽셀로 카운트 만큼 변환해준다.
	{
		for(int i = 0; i < nCount; i++, pDest++, pSrc++)
		{
			if( *pSrc == 0x00 || *pSrc == 0xf81f )
				*pDest = *pSrc;
			else if( *pSrc & 0x20 )
				*pDest = *pSrc;
			else *pDest = *pSrc + 0x20;
		}
	}
	void Conv555PixelTo565Pixel(WORD* pDest, const WORD* pSrc, int nCount) // 565 픽셀을 555 픽셀로 카운트 만큼 변환해준다.
	{
		for(int i = 0; i < nCount; i++, pDest++, pSrc++)
		{
			*pDest = ((*pSrc&0x7fe0)<<1) | (*pSrc&0x001f);
		}
	}

	void HWndSet(HWND hWnd) // 클리퍼에 핸들 설정..
	{
		m_hWnd = hWnd;
		if(m_bFullScreen == FALSE && m_lpDDClipper != NULL) m_lpDDClipper->SetHWnd(0, hWnd);
	}

	BOOL IsBBLocked() { return m_bBBLocked; } // 현재 백버퍼에 락이 걸려 있는지

public:
	void Blt(LPDIRECTDRAWSURFACE7 lpDDSDest, LPDIRECTDRAWSURFACE7 lpDDSSrc, RECT* pRCDest, RECT* pRCSrc, BOOL bUseColorKey);
	void PathSetWork(const char* szPath);
	void TextOut(LPDIRECTDRAWSURFACE7 lpDDS, int x, int y, const char* szText, COLORREF crText, UINT align = TA_TOP);
	void DrawBox(LPDIRECTDRAWSURFACE7 lpDDS, const RECT* pRC, COLORREF crColor);
	void DrawLine(LPDIRECTDRAWSURFACE7 lpDDS, int sx, int sy, int ex, int ey, COLORREF crColor);
	void DrawRhombus(LPDIRECTDRAWSURFACE7 lpDDS, int x, int y, COLORREF crColor);
	void DrawPolygon(LPDIRECTDRAWSURFACE7 lpDDS, POINT *point, int count, COLORREF crColor);

	HWND HWndGet() { return m_hWnd; }

	void Flip(const RECT* pRCClip = NULL);
	
	LPDIRECTDRAWSURFACE7 SurfaceCreateWith16BMP(const char *szFileName, BOOL bUseVideoMemory, COLORREF crCK,BOOL bSetting = FALSE);
	LPDIRECTDRAWSURFACE7 SurfaceCreateWithBMP(const char* szFileName, BOOL bUseVideoMemory, COLORREF crColorKey,BOOL bSetting = FALSE);
	LPDIRECTDRAWSURFACE7 SurfaceCreate(int nWidth, int nHeight, BOOL bUseVideoMemory, COLORREF crColorKey);
	COLORREF SurfaceColorKeyGet(LPDIRECTDRAWSURFACE7 lpDDS);
	void SurfaceColorKeySet(LPDIRECTDRAWSURFACE7 lpdds, COLORREF ColorKey);
	void SurfaceFillRect(LPDIRECTDRAWSURFACE7 lpDDS = NULL, const RECT *pRC = NULL, COLORREF crFill = 0, int nAlpha = 32);
	void SurfaceGetSize(LPDIRECTDRAWSURFACE7 lpDDS, RECT* pRC);

	void BackBufferLock();
	void BackBufferUnlock();
	void Init(HWND hWnd, BOOL bFullScreen, int nWidth, int nHeight);

	void PathSetCur(const char* szSubPath);
	const char* PathGetCur(const char* szPath, int nPath = 0) const;

	void BltNormal(LPDIRECTDRAWSURFACE7 lpDDSSrc, int x, int y, const RECT* prcSrc, BOOL bUseColorKey, int nRenderType = 0, int nAlpha = 0, COLORREF crColor = 0, LPDIRECTDRAWSURFACE7 lpDDSDest = NULL);
	void BltCenter(LPDIRECTDRAWSURFACE7 lpDDSSrc, int x, int y, const RECT* prcSrc, POINT* pptCenter, BOOL bUseColorKey, int nRenderType = 0, int nAlpha = 0, COLORREF crColor = 0, LPDIRECTDRAWSURFACE7 lpDDSDest = NULL)
	{
		x -= pptCenter->x - prcSrc->left; y -= pptCenter->y - prcSrc->top;
		this->BltNormal(lpDDSSrc, x, y, prcSrc, bUseColorKey, nRenderType, nAlpha, crColor, lpDDSDest);
	}

	CDDEngineNew();
	virtual ~CDDEngineNew();

protected:
	LPDIRECTDRAWSURFACE7 SurfaceCreateWithGenericBMP(const char *szFileName, BOOL bUseVideoMemory, COLORREF crCK);

	// 엄청난 크기의 인라인 함수.. 속도에 매우 민감하다. - BltAlphaAdd.
	void PutPixelNormal(const WORD* pDest, const WORD* pSrc, int nCopyCount)
	{
/*		__asm // 32비트 카피...
		{
			mov edi, pDest;
			mov esi, pSrc;
			mov eax, nCopyCount;
			
			mov edx, 0;
			mov ecx, 2;
			div ecx; // 몫은 eax, 나머지 edx;
			cmp eax, 0;
			je WORD_COPY_LOOP;

DWORD_COPY_LOOP:
			dec eax;
			movsd; // 더블 원드 카피..
			jnz DWORD_COPY_LOOP; // 몫만큼 메모리 복사
			cmp edx, 0;
			je DONT_WORD_COPY; // 워드 카피 필요 없음.

WORD_COPY_LOOP:
			movsw;
			dec edx;
			jnz WORD_COPY_LOOP;
DONT_WORD_COPY:
		}
*/
		__asm
		{
			mov edi, pDest;
			mov esi, pSrc;
			
			mov ecx, nCopyCount;
			
//			movd mm0, nCopyCount;
//			movd nCopyCount, mm0;
//			EMMS

WORD_COPY_LOOP:
			movsw;
			dec ecx;
			jnz WORD_COPY_LOOP;
		}

	};

	void PutPixelAlphaAdd(const WORD* pDest, const WORD* pSrc, int nCopyCount, DWORD dwRMask, DWORD dwGMask, DWORD dwBMask)
	{
		__asm
		{
			mov edi, pDest;	// 백그라운드 주소 저장.
			mov esi, pSrc;	// 스프라이트 주소 저장.

			mov ecx, nCopyCount; // 가로 길이.

COPY_LOOP:
			push ecx;
			mov ecx, [esi]; // Spr

			jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..

			mov ebx, 0; // 최종 픽셀

			mov eax, ecx; // Spr - R
			mov edx, [edi]; // Bkg - R
			and eax, dwRMask;
			and edx, dwRMask;
			add edx, eax;
			cmp edx, dwRMask;
			ja COPY_R1;
			jmp ADD_R1;
COPY_R1:
			mov edx, dwRMask;
ADD_R1:
			or ebx, edx;

			mov eax, ecx; // Spr - G
			mov edx, [edi]; // Bkg - G
			and eax, dwGMask;
			and edx, dwGMask;
			add edx, eax;
			cmp edx, dwGMask;
			ja COPY_G1;
			jmp ADD_G1;
COPY_G1:
			mov edx, dwGMask;
ADD_G1:
			or ebx, edx;

			mov eax, ecx; // Spr - B
			mov edx, [edi]; // Bkg - B
			and eax, dwBMask;
			and edx, dwBMask;
			add edx, eax;
			cmp edx, dwBMask;
			ja COPY_B1;
			jmp ADD_B1;
COPY_B1:
			mov edx, dwBMask;
ADD_B1:
			or ebx, edx;

			mov [edi], bx; // 백버퍼에 씀
			
DONT_COPY:
			add edi, 2;
			add esi, 2;

			pop ecx;
			dec ecx;
			jnz COPY_LOOP;
		}
	};

	void PutPixelAlphaSoft(const WORD* pDest, const WORD* pSrc, int nCopyCount, DWORD dwRMask, DWORD dwGMask, DWORD dwBMask)
	{
		__asm
		{
			mov edi, pDest;	// 백그라운드 주소 저장.
			mov esi, pSrc;	// 스프라이트 주소 저장.

			mov ecx, nCopyCount; // 가로 길이.

COPY_LOOP:
			push ecx; // 카운트 기억

			mov ecx, [esi]; // 스프라이트 픽셀

			jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..
			
			mov ebx, 0; // 최종 픽셀 값..

			mov ecx, [esi]; // Spr 픽셀
			
			mov edx, [edi]; // Bkg - R
			mov eax, ecx // Spr - R
			and edx, dwRMask;
			and eax, dwRMask;
			cmp eax, edx;
			jb COPY_R1_BKG;
			mov edx, eax
COPY_R1_BKG:
			or ebx, edx;

			mov edx, [edi]; // Bkg - R
			mov eax, ecx // Spr - R
			and edx, dwGMask;
			and eax, dwGMask;
			cmp eax, edx;
			jb COPY_G1_BKG;
			mov edx, eax
COPY_G1_BKG:
			or ebx, edx;

			mov edx, [edi]; // Bkg - R
			mov eax, ecx // Spr - R
			and edx, dwBMask;
			and eax, dwBMask;
			cmp eax, edx;
			jb COPY_B1_BKG;
			mov edx, eax
COPY_B1_BKG:
			or ebx, edx;

			mov [edi], bx; // 백버퍼에 씀
		
DONT_COPY:
			add edi, 2;
			add esi, 2;

			pop ecx; // 카운트 복구
			dec ecx;
			jnz COPY_LOOP;
		}
	};

	void PutPixelTrans(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaDest, int nAlphaSrc, DWORD dwRBMask, DWORD dwGMask) // nAlphaSrc 와 nAlphaDest 의 합은 반드시 32 이어야 한다..
	{
/*		__asm // RGB 값마다 강도 계산..
		{
			mov edi, pDest;
			mov esi, pSrc;
			mov ecx, nCopyCount;
COPY_LOOP:
			push ecx; // 카운트 저장..

			mov ebx, 0; // 최종 색깔 초기화

			mov ecx, [esi]; // Spr Pixel

			jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..

			mov eax, ecx; // Spr - R B
			and eax, dwRBMask;
			mul nAlphaSrc;
			shr eax, 5;
			and eax, dwRBMask;
			add ebx, eax;

			mov eax, ecx; // Spr - G
			and eax, dwGMask;
			mul nAlphaSrc;
			shr eax, 5;
			and eax, dwGMask;
			add ebx, eax;
			
			mov ecx, [edi]; // Bkg Pixel

			mov eax, ecx; // Bkg - R B
			and eax, dwRBMask;
			mul nAlphaDest;
			shr eax, 5;
			and eax, dwRBMask;
			add ebx, eax;
			
			mov eax, ecx; // Bkg - G
			and eax, dwGMask;
			mul nAlphaDest;
			shr eax, 5;
			and eax, dwGMask;
			add ebx, eax;
			
			mov [edi], bx;
DONT_COPY:
			add edi, 2;
			add esi, 2;
			
			pop ecx; // 카운트 복구..
			dec ecx;
			jnz COPY_LOOP;
		}
*/	};

	void PutPixelDark(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaSrc, DWORD dwRBMask, DWORD dwGMask) // nAlphaSrc - 0 이 젤 시커멓다.. 32 가 제일 밝다.
	{
		__asm // RGB 값마다 강도 계산..
		{
			mov edi, pDest;
			mov esi, pSrc;
			mov ecx, nCopyCount;
COPY_LOOP:
			push ecx; // 카운트 저장..

			mov ebx, 0; // 최종 색깔 초기화

			mov ecx, [esi]; // Spr Pixel

			jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..

			mov eax, ecx; // Spr - R B
			and eax, dwRBMask;
			mul nAlphaSrc;
			shr eax, 5;
			and eax, dwRBMask;
			add ebx, eax;
			
			mov eax, ecx; // Spr - G
			and eax, dwGMask;
			mul nAlphaSrc;
			shr eax, 5;
			and eax, dwGMask;
			add ebx, eax;
			
			mov [edi], bx;

DONT_COPY:

			add edi, 2;
			add esi, 2;
			
			pop ecx; // 카운트 복구..
			dec ecx;
			jnz COPY_LOOP;
		}
	};

	void PutPixelShadow(const WORD* pDest, const WORD* pSrc, int nCopyCount, DWORD dwRBMask, DWORD dwGMask) // 0 시커먼 그림자.. 32 그림자 없음..
	{
		__asm // RGB 값마다 강도 계산..
		{
			mov edi, pDest;
			mov esi, pSrc;

			mov ecx, nCopyCount;
COPY_LOOP:
			push ecx; // 카운트 저장..

			mov cx, [esi]; // 알파값 계산
			jcxz DONT_COPY; // 소스가 검은 색이면 넘어간다.
			and ecx, 0x001f; // 
			mov ebx, 0x001f;
			sub ebx, ecx;
			mov ecx, ebx;

			mov ebx, 0; // 최종 색깔 초기화

			mov eax, [edi]; // Bkg - R B
			and eax, dwRBMask;
			mul ecx;
			shr eax, 5;
			and eax, dwRBMask;
			add ebx, eax;
			
			mov eax, [edi]; // Bkg - G
			and eax, dwGMask;
			mul ecx;
			shr eax, 5;
			and eax, dwGMask;
			add ebx, eax;
			
			mov [edi], bx;

DONT_COPY:
			add edi, 2;
			add esi, 2;
			
			pop ecx; // 카운트 복구..
			dec ecx;
			jnz COPY_LOOP;
		}
	};
	
	void PutPixelColor(const WORD* pDest, const WORD* pSrc, int nCopyCount, int nAlphaSrc, DWORD dwDestColor, DWORD dwRBMask, DWORD dwGMask) // nAlphaSrc - 스프라이트의 밝기 값. dwDestColor - 컬러의 32비트 값. (반드시 "32-nAlphaSrc"의 비율과 일치하여야 한다.
	{
		__asm // RGB 값마다 강도 계산..
		{
			mov edi, pDest;
			mov esi, pSrc;
			mov ecx, nCopyCount;
COPY_LOOP:
			push ecx; // 카운트 저장..

			mov ebx, 0; // 최종 색깔 초기화

			mov ecx, [esi]; // Spr Pixel

			jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..

			mov eax, ecx; // Spr - R B
			and eax, dwRBMask;
			mul nAlphaSrc;
			shr eax, 5;
			and eax, dwRBMask;
			add ebx, eax;
			
			mov eax, ecx; // Spr - G
			and eax, dwGMask;
			mul nAlphaSrc;
			shr eax, 5;
			and eax, dwGMask;
			add ebx, eax;
			
			add ebx, dwDestColor; // 컬러 값을 더해준다.
			
			mov [edi], bx;

DONT_COPY:

			add edi, 2;
			add esi, 2;
			
			pop ecx; // 카운트 복구..
			dec ecx;
			jnz COPY_LOOP;
		}
	};
};

#endif // !defined(AFX_DDENGINE_H__CF5FF875_6921_4E64_904F_C074E41AD179__INCLUDED_)
