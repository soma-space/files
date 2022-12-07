// AniObjLSP.cpp: implementation of the CAniObjLSP class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AniObjLSP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAniObjLSP::CAniObjLSP()
{
	MAKE_ZERO(m_HSpr);
	MAKE_ZERO(m_pSprs);
}

CAniObjLSP::~CAniObjLSP()
{
	SprRelease();
}

void CAniObjLSP::SprRelease()
{
	MAKE_ZERO(m_HSpr);	// 스프라이트 헤더를 널로..
	for(int i=0; i < MAX_SPR; i++) // 스프라이트 영역 데이터 및 lsp data 삭제..
	{
		if(m_pSprs[i])
		{
			if(m_pSprs[i]->pwV) { delete [] m_pSprs[i]->pwV; m_pSprs[i]->pwV = NULL; }
			delete m_pSprs[i];
			m_pSprs[i] = NULL;
		}
	}
}

// Pixel Format - 555 or 565
void CAniObjLSP::SprLoad(const char* szFileName, int nPF)
{
	if(m_HSpr.nQt > 0) SprRelease(); // 해제...

	HANDLE hFile;
	DWORD dwCount;

	if(szFileName[0] == 0) return;
	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
#ifdef _DEBUG
		OutputDebugString("LSP 스프라이트 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
#endif
		return;
	}

	ReadFile(hFile, &m_HSpr, sizeof(_SPR_HEADER), &dwCount, NULL); // 스프라이트 헤더 읽기.
	if(m_HSpr.szID[0] != 'S' || m_HSpr.szID[1] != 'P' || m_HSpr.szID[2] != 'L' || m_HSpr.szID[3] != 'N')
	{
#ifdef _DEBUG
		OutputDebugString("LSP 스프라이트 파일 열기 실패 : 잘못된 파일 헤더");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
#endif
		CloseHandle(hFile);
		return;
	}

	SprAlloc(m_HSpr.nQt); // 스프라이트 데이터 할당.

	SetFilePointer(hFile, sizeof(_SPR_TOOL)*m_HSpr.nQt, NULL, FILE_CURRENT); // 툴에서만 쓰이는 부가적인 데이터.

	for(int i =0; i < m_HSpr.nQt; i++)
	{
		ReadFile(hFile, m_pSprs[i], sizeof(_SPR_LSP), &dwCount, NULL); // 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
		m_pSprs[i]->pwV = 0;
	}
	for(i =0; i < m_HSpr.nQt; i++)
	{
		if(m_pSprs[i]->nDataSize > 2560000)
		{
#ifdef _DEBUG
			OutputDebugString(szFileName);
			OutputDebugString(" : 비정상적인 스프라이트\n");
#endif
			break;
		}
		if(m_pSprs[i]->nDataSize > 0)
		{
			m_pSprs[i]->pwV = new WORD[m_pSprs[i]->nDataSize/2];
			ReadFile(hFile, m_pSprs[i]->pwV, m_pSprs[i]->nDataSize, &dwCount, NULL);
			if(nPF == 555) Decode2_555(m_pSprs[i]); // 픽셀 포맷 변환
		}
	}
	CloseHandle(hFile);
}

void CAniObjLSP::SprAlloc(int nQt) // 크기는 바이트 단위이다. 할당할때 워드 단위이 땜시 반으로 쪼갠다.
{
	if(nQt <= 0 || nQt > MAX_SPR) return;

	for(int i=0; i < nQt; i++)
	{
		if(m_pSprs[i] != NULL) continue;

		m_pSprs[i] = new _SPR_LSP;
		MAKE_ZERO(*m_pSprs[i]);
	}
	m_HSpr.nQt = nQt;
}

BOOL CAniObjLSP::SprGet(int index, const _SPR_LSP** ppSpr)
{
	if(index < 0 || index >= MAX_SPR)
	{
		*ppSpr = NULL;
		return FALSE;
	}

	*ppSpr = m_pSprs[index];
	return TRUE;
}

void CAniObjLSP::Decode2_555(_SPR_LSP* pSpr) // 565 LSP 를 555 LSP 로 바꾸어준다.. 
{
	WORD* tmp = pSpr->pwV;
	int node; int np; // 노드 및 유효 픽셀 수
	for(int y = 0; y < pSpr->nLine; y++)
	{
		node = *tmp; tmp++;
		for(;node;node--, tmp+=np)
		{
			tmp++; // 0 부분 skip... 
			np = *tmp; tmp++;
			for(;np; np--)
			{
				*tmp = ((*tmp&0xffc0)>>1) | (*tmp&0x001f);
				tmp++;
			}
		}
	}
}

void CAniObjLSP::Decode2_565(_SPR_LSP* pSpr) // 555 LSP 를 565 LSP 로 바꾸어준다.. 
{
	WORD* tmp = pSpr->pwV;
	int node; int np; // 노드 및 유효 픽셀 수
	for(int y = 0; y < pSpr->nLine; y++)
	{
		node = *tmp; tmp++;
		for(;node;node--, tmp+=np)
		{
			tmp++; // 0 부분 skip... 
			np = *tmp; tmp++;
			for(;np; np--)
			{
				*tmp = ((*tmp&0x7fe0)<<1) | (*tmp&0x001f);
				tmp++;
			}
		}
	}
}

int CAniObjLSP::SprGetAnimated(const _SPR_LSP** ppSpr, int nCtrl)
{
	int nFrm = this->FrmGetAnimated(nCtrl);
	if(nFrm < 0 || nFrm >= m_HSpr.nQt) { *ppSpr = NULL; return -1; }

	*ppSpr = this->m_pSprs[nFrm];
	return nFrm;
}

BOOL CAniObjLSP::Convert8PixelTo16Pixel(BYTE* p8Pixels, WORD *p16Pixels, RGBQUAD* pRGBTbls, int nQt)
{
	if(!p16Pixels || !p8Pixels || !pRGBTbls || nQt <= 0) return FALSE;
	memset(p16Pixels, 0, nQt*2);

	WORD wPixel;
	BYTE byIndex;
	for(int i = 0; i < nQt; i++)
	{
		byIndex = *p8Pixels; p8Pixels++;
		wPixel = RGB16(pRGBTbls[byIndex].rgbRed, pRGBTbls[byIndex].rgbGreen, pRGBTbls[byIndex].rgbBlue);
		*p16Pixels = wPixel; p16Pixels++;
	}
	return TRUE;
}
