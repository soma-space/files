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
	HANDLE hFile;
	DWORD dwCount;

	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
		OutputDebugString("LSP 스프라이트 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}

	_SPR_HEADER HSpr;
	ReadFile(hFile, &HSpr, sizeof(_SPR_HEADER), &dwCount, NULL); // 스프라이트 헤더 읽기.
	if(HSpr.szID[0] != 'S' || HSpr.szID[1] != 'P' || HSpr.szID[2] != 'L' || HSpr.szID[3] != 'N')
	{
		OutputDebugString("LSP 스프라이트 파일 열기 실패 : 잘못된 파일 헤더");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		CloseHandle(hFile);
		return;
	}

	SprRelease(); // 해제...
	m_HSpr = HSpr;
	SprAlloc(m_HSpr.nQt); // 스프라이트 데이터 할당.

	SetFilePointer(hFile, sizeof(_SPR_TOOL)*m_HSpr.nQt, NULL, FILE_CURRENT); // 툴에서만 쓰이는 부가적인 데이터.


	for(int i =0; i < m_HSpr.nQt; i++)
	{
		ReadFile(hFile, m_pSprs[i], sizeof(_SPR_LSP), &dwCount, NULL); // 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
		m_pSprs[i]->pwV = 0;
	}
	if( nPF != 1 )//1인경우 비디오 메모리에 올린다.
	{
		for(i =0; i < m_HSpr.nQt; i++)
		{
			if(m_pSprs[i]->nDataSize > 2560000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
			if(m_pSprs[i]->nDataSize > 0)
			{
				m_pSprs[i]->pwV = new WORD[m_pSprs[i]->nDataSize/2];
				ReadFile(hFile, m_pSprs[i]->pwV, m_pSprs[i]->nDataSize, &dwCount, NULL);
				if(nPF == 565) Decode2_565_1(m_pSprs[i]);
//				if(nPF == 555) Decode2_555(m_pSprs[i]); // 픽셀 포맷 변환
				if(nPF == 555) Decode2_555_1(m_pSprs[i]); // 픽셀 포맷 변환
				if(nPF ==   0) Decode2_Shadow(m_pSprs[i]); // 검은색 그림자로......
			}
		}
	}
	CloseHandle(hFile);
}

// Pixel Format - 555 or 565
void CAniObjLSP::SprLoadEx(const char* szFileName, int nPF, int spl_index)
{
	HANDLE hFile;
	DWORD dwCount;

	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
		OutputDebugString("LSP 스프라이트 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}

	_SPR_HEADER HSpr;
	ReadFile(hFile, &HSpr, sizeof(_SPR_HEADER), &dwCount, NULL); // 스프라이트 헤더 읽기.
	if(HSpr.szID[0] != 'S' || HSpr.szID[1] != 'P' || HSpr.szID[2] != 'L' || HSpr.szID[3] != 'N')
	{
		OutputDebugString("LSP 스프라이트 파일 열기 실패 : 잘못된 파일 헤더");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		CloseHandle(hFile);
		return;
	}

	if( m_HSpr.nQt <= 0 )
		m_HSpr = HSpr;

	//SprAlloc(m_HSpr.nQt); // 스프라이트 데이터 할당.

	SetFilePointer(hFile, sizeof(_SPR_TOOL)*m_HSpr.nQt, NULL, FILE_CURRENT); // 툴에서만 쓰이는 부가적인 데이터.

	if(m_pSprs[spl_index])
	{
		if(m_pSprs[spl_index]->pwV) { delete [] m_pSprs[spl_index]->pwV; m_pSprs[spl_index]->pwV = NULL; }
		delete m_pSprs[spl_index];
		m_pSprs[spl_index] = NULL;
	}
	m_pSprs[spl_index] = new _SPR_LSP;
	if( m_pSprs[spl_index] == NULL ) 
	{
		OutputDebugString("OUTOFMEMORY\n");
		return;
	}
	MAKE_ZERO(*m_pSprs[spl_index]);

	_SPR_LSP* spr_lsp[MAX_SPR];

	for(int i=0; i < m_HSpr.nQt; i++)
	{ 
		spr_lsp[i] = new _SPR_LSP;
		MAKE_ZERO(*spr_lsp[i]);
	}
	
	for(i =0; i < m_HSpr.nQt; i++)
	{
		ReadFile(hFile, spr_lsp[i], sizeof(_SPR_LSP), &dwCount, NULL); // 스프라이트 영역 - 실제 게임에서만 쓰이는 데이터.
		spr_lsp[i]->pwV = 0;
	}
	
	memcpy(m_pSprs[spl_index],spr_lsp[spl_index],sizeof(_SPR_LSP));
	m_pSprs[spl_index]->pwV = 0;

	if( nPF != 1 )//1인경우 비디오 메모리에 올린다.
	{
		for(i =0; i < spl_index; i++)
		{
			if(spr_lsp[i]->nDataSize > 2560000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
			if(spr_lsp[i]->nDataSize > 0)
			{
				SetFilePointer(hFile, sizeof(WORD)*(spr_lsp[i]->nDataSize/2), NULL, FILE_CURRENT); 
			}
		}

		if(m_pSprs[spl_index]->nDataSize > 2560000) { MessageBox(NULL, szFileName, "비정상적인 스프라이트", MB_OK); exit(-1); }
		if(m_pSprs[spl_index]->nDataSize > 0)
		{
			m_pSprs[spl_index]->pwV = new WORD[m_pSprs[spl_index]->nDataSize/2];
			ReadFile(hFile, m_pSprs[spl_index]->pwV, m_pSprs[spl_index]->nDataSize, &dwCount, NULL);
			if(nPF == 565) Decode2_565_1(m_pSprs[spl_index]);
			if(nPF == 555) Decode2_555_1(m_pSprs[spl_index]); // 픽셀 포맷 변환
			if(nPF ==   0) Decode2_Shadow(m_pSprs[spl_index]); // 검은색 그림자로......
		}
	}
	
//	if( spr_lsp ) delete [] spr_lsp;
	for( i = 0; i < m_HSpr.nQt; i++)
		delete spr_lsp[i];

	CloseHandle(hFile);
}

void CAniObjLSP::SprAlloc(int nQt) // 크기는 바이트 단위이다. 할당할때 워드 단위이 땜시 반으로 쪼갠다.
{
	if(nQt <= 0 || nQt > MAX_SPR) return;

	SprRelease(); // 일단 해제
	for(int i=0; i < nQt; i++)
	{ 
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

	Map_iter pos = m_mapLSP.find( index );
	DWORD dwNowTime = GetTickCount();
	if( pos != m_mapLSP.end() )
	{
		(*pos).second = dwNowTime;
	}
	else
	{
		m_mapLSP.insert(Map_value(index,dwNowTime));
	}

	*ppSpr = m_pSprs[index];
	return TRUE;
}

void CAniObjLSP::RemoveUnUseSpr()
{
	DWORD dwNowTime = GetTickCount();
	for( Map_iter pos = m_mapLSP.begin(); pos != m_mapLSP.end(); )
	{
		if( dwNowTime > (*pos).second + 60000 )
		{
			if(m_pSprs[(*pos).first])
			{
				if(m_pSprs[(*pos).first]->pwV) { delete [] m_pSprs[(*pos).first]->pwV; m_pSprs[(*pos).first]->pwV = NULL; }
				delete m_pSprs[(*pos).first];
				m_pSprs[(*pos).first] = NULL;
				OutputDebugString("LSP 스프라이트 지움\n ");

			}
			m_mapLSP.erase(pos++);
		}
		else
			++pos;
	}
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
void CAniObjLSP::Decode2_555_1(_SPR_LSP* pSpr) // 565 LSP 를 555 LSP 로 바꾸어준다.. 
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
				*tmp = ((*tmp&0xffc0)>>1) | (*tmp&0x001f) | 0x8000;
				tmp++;
			}
		}
	}
}

void CAniObjLSP::Decode2_565_1(_SPR_LSP* pSpr) // 565 LSP 를 555 LSP 로 바꾸어준다.. 
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
				if( !(*tmp & 0x20) )
					*tmp = *tmp | 0x20;
				tmp++;
			}
		}
	}
}

void CAniObjLSP::Decode2_Shadow(_SPR_LSP* pSpr) // 565 LSP 를 555 LSP 로 바꾸어준다.. 
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
				*tmp = 0x00;//((*tmp&0xffc0)>>1) | (*tmp&0x001f);
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
