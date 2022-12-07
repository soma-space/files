// TileMap.cpp: implementation of the CTileMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTileMap::CTileMap()
{
	MAKE_ZERO(m_HTMap);
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		m_hDatas[i] = NULL;
		m_pnDatas[i] = NULL;
		m_lpDDSs[i] = NULL;
	}
}

CTileMap::~CTileMap()
{
	Release();
}

void CTileMap::Release()
{
	// 메모리 해제..
	MAKE_ZERO(m_HTMap);
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		if(m_hDatas[i])
		{
			GlobalUnlock(m_hDatas[i]);
			GlobalFree(m_hDatas[i]);
		}
		m_hDatas[i] = NULL;
		m_pnDatas[i] = NULL;
		RELEASE_SAFE(m_lpDDSs[i]);
	}
}

void CTileMap::Alloc(int nWidth, int nHeight)
{
	if(nWidth < 0 || nWidth >= MAX_TILE_MAP_WIDTH || nHeight < 0 || nHeight >= MAX_TILE_MAP_HEIGHT) return;

	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		if(m_hDatas[i] != NULL)
		{
			_TILE_MAP_HEADER HMapOld = m_HTMap; // 헤더 기억해둔다...
			this->Release();
			m_HTMap = HMapOld;
			break;
		}
	}

	m_HTMap.nWidth = nWidth;
	m_HTMap.nHeight = nHeight;

	for(i = 0; i < MAX_TILE_MAP_LAYER; i++) // 할당하고 락..
	{
		m_hDatas[i] = GlobalAlloc(GMEM_FIXED, sizeof(_TILE_MAP_DATA)*nWidth*nHeight);
		m_pnDatas[i] = (short int*)GlobalLock(m_hDatas[i]);
		FillMemory(m_pnDatas[i], sizeof(_TILE_MAP_DATA)*nWidth*nHeight, 0xff);
	}
}

// 서피스는 따로 불러 주어야 한다..
void CTileMap::Load(const char *szFileName)
{
	HANDLE hFile;
	DWORD dwCount;
	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
		OutputDebugString("타일맵 데이터 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}
	
	ReadFile(hFile, &m_HTMap, sizeof(TILE_MAP_HEADER), &dwCount, NULL);
	this->Alloc(m_HTMap.nWidth, m_HTMap.nHeight);
	for(int i = 0; i < MAX_TILE_MAP_LAYER; i++)
	{
		for(int y = 0; y < m_HTMap.nHeight; y++)
		{
			ReadFile(hFile, &m_pnDatas[i][y*m_HTMap.nWidth], sizeof(short int)*m_HTMap.nWidth, &dwCount, NULL);
		}
	}
	
	CloseHandle(hFile);
}
