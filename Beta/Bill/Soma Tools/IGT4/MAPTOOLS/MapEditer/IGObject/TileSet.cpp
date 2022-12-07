// TileSet.cpp: implementation of the CTileSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileSet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTileSet::CTileSet()
{
	MAKE_ZERO(m_Datas);
	MAKE_ZERO(m_HTSs);
	MAKE_ZERO(m_lpDDSs);
}

CTileSet::~CTileSet()
{
	ReleaseAll();
}

void CTileSet::ReleaseAll()
{
	for(int i = 0; i < MAX_TILE_SET; i++)
	{
		for(int j = 0; j < MAX_TILE_SUB_SET; j++)
		{
			DELETE_ARRAY_SAFE(m_Datas[i][j].pptSrcs);
			MAKE_ZERO(m_Datas[i][j]);
		}
		RELEASE_SAFE(m_lpDDSs[i]);
	}
	MAKE_ZERO(m_HTSs);
	MAKE_ZERO(m_Datas);
}

void CTileSet::Release(int nTS, int nTSS)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;

	if(m_Datas[nTS][nTSS].pptSrcs) delete [] m_Datas[nTS][nTSS].pptSrcs;
	MAKE_ZERO(m_Datas[nTS][nTSS]);
}

void CTileSet::Alloc(int nTS, int nTSS, int width, int height)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;
	if(width <= 0 || height <= 0) return;

	// 혹시나 
	if(m_Datas[nTS][nTSS].pptSrcs)
	{
		DELETE_ARRAY_SAFE(m_Datas[nTS][nTSS].pptSrcs);
		OutputDebugString("타일셋 데이터 할당시 전의 데이터를 지우지 않았습니다.");
		return;
	}

	MAKE_ZERO(m_Datas[nTS][nTSS]);

	m_Datas[nTS][nTSS].nWidth = width;
	m_Datas[nTS][nTSS].nHeight = height;

	m_Datas[nTS][nTSS].pptSrcs = new POINT[width*height];
	memset(m_Datas[nTS][nTSS].pptSrcs, 0, sizeof(POINT)*width*height);
}

// 로딩후 서피스는 따로.... 로딩!!!!
void CTileSet::Load(const char *szFileName)
{
	HANDLE hFile;
	DWORD dwCount;
	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
		OutputDebugString("타일셋 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}

	for(int i = 0; i < MAX_TILE_SET; i++)
	{
		ReadFile(hFile, &m_HTSs[i], sizeof(_TILE_SET_HEADER), &dwCount, NULL);
		if(m_HTSs[i].bUse)
		{
			for(int j = 0; j < MAX_TILE_SUB_SET; j++)
			{
				ReadFile(hFile, &m_Datas[i][j], sizeof(_TILE_SUB_SET_DATA_TOOL), &dwCount, NULL);
				m_Datas[i][j].pptSrcs = NULL;
				if(m_Datas[i][j].nWidth > 0 && m_Datas[i][j].nHeight > 0)
				{
					int nAlloc = m_Datas[i][j].nWidth * m_Datas[i][j].nHeight;
					m_Datas[i][j].pptSrcs = new POINT[nAlloc];
					ReadFile(hFile, m_Datas[i][j].pptSrcs, sizeof(POINT)*nAlloc, &dwCount, NULL);
				}
			}
		}
	}
	CloseHandle(hFile);
}
