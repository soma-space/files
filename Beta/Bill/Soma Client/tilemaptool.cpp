// TileMapTool.cpp: implementation of the CTileMapTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileMapTool.h"
#include "IGObject\\Dib16.h"

//#include "GlobalVar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

typedef struct {
	DWORD tsn	:	10;		// 1023개
	DWORD tssn	:	6;		// 63개
	DWORD num	:	12;		// 4096개(64X64)
	DWORD flag	:	4;
} DARK_TMD_FORMAT;

//////////////////////////////////////////////////////////////////////
// ConstrsnSrction/DestrsnSrction
//////////////////////////////////////////////////////////////////////

CTileMapTool::CTileMapTool()
{
	for(int i = 0; i < 3; i++) // 왕창 할당..
	{
		m_hDatasTool[i] = NULL;
		m_pDatasTool[i] = NULL;
	}
	m_hDatasTool[4] = NULL;
	m_pDatasTool[4] = NULL;
}

CTileMapTool::~CTileMapTool()
{
	for(int i = 0; i < 3; i++) // 몽땅 해제..
	{
		if(m_hDatasTool[i])
		{
			GlobalUnlock(m_hDatasTool[i]);
			GlobalFree(m_hDatasTool[i]);
			m_hDatasTool[i] = NULL;
			m_pDatasTool[i] = NULL;
		}
	}
	if(m_hDatasTool[4])
	{
		GlobalUnlock(m_hDatasTool[4]);
		GlobalFree(m_hDatasTool[4]);
		m_hDatasTool[4] = NULL;
		m_pDatasTool[4] = NULL;
	}
}

_TILE_MAP_DATA_TOOL* CTileMapTool::ToolDataGet(int nLayer, int x, int y)
{
	if(nLayer < 0 || (nLayer >= 3 && nLayer != 4)) return NULL;
	if(x < 0 || x >= m_HTMap.nWidth) return NULL;
	if(y < 0 || y >= m_HTMap.nHeight) return NULL;

	return &m_pDatasTool[nLayer][y*m_HTMap.nWidth + x];
}

void CTileMapTool::New()
{
	this->Release();
	this->ReleaseEx();
}

void CTileMapTool::LoadFromToolData(const char* szFileName)
{
	this->Release();
	this->ReleaseEx();

	CFile file1, file2;
	CString szSprFileName;
	int map_size;
	DWORD table_count, core_size;
	DWORD offset, count;
	DWORD *st_buf;
	DWORD i, j, k;

	szSprFileName = szFileName;
	szSprFileName = szSprFileName.Left(szSprFileName.GetLength()-1);
	szSprFileName += '1';
	if(file1.Open(szFileName, CFile::modeRead) == FALSE) return;
	if(file2.Open(szSprFileName, CFile::modeRead) == FALSE)
	{
		file1.Close();
		return;
	}

	file1.Read(&m_HTMap, sizeof(TILE_MAP_HEADER));

	this->AllocTool(m_HTMap.nWidth, m_HTMap.nHeight); // 할당

	map_size = m_HTMap.nHeight*m_HTMap.nWidth;
	file1.Read(&core_size, sizeof(DWORD));
	st_buf = new DWORD[core_size];

	file1.Read(st_buf, sizeof(DWORD)*core_size);

	short int mts = 0, mtss = 0, mnum = 0;

	offset = 0;
	for(i = 0; i < 3; i++) // 툴에서 쓴 데이터 저장...
	{
		table_count = st_buf[offset++];
		k = 0;
		for(j = 0; j < table_count; j++)
		{
			count = st_buf[offset++];
			k += count;
			count = st_buf[offset++];
			CopyMemory(&m_pDatasTool[i][k], &st_buf[offset], count*4);
			k += count;
			offset += count;
		}
	}
	delete[] st_buf;
	file1.Close();

	file2.Read(&core_size, sizeof(DWORD));
	st_buf = new DWORD[core_size];

	file2.Read(st_buf, sizeof(DWORD)*core_size);

	offset = 0;
	// Skip Layer 3(Shadow Layer)
	table_count = st_buf[offset++];
	k = 0;
	for(j = 0; j < table_count; j++)
	{
		count = st_buf[offset++];
		k += count;
		count = st_buf[offset++];
		k += count;
		offset += count;
	}
	// Read Layer 4
	table_count = st_buf[offset++];
	k = 0;
	for(j = 0; j < table_count; j++)
	{
		count = st_buf[offset++];
		k += count;
		count = st_buf[offset++];
		CopyMemory(&m_pDatasTool[4][k], &st_buf[offset], count*4);
		k += count;
		offset += count;
	}
	delete[] st_buf;
	file2.Close();
}

void CTileMapTool::SaveToToolData(const char *szFileName)
{
	UNREFERENCED_PARAMETER(szFileName);
}

void CTileMapTool::SaveToGameFormat(const char* szFileName, COLORREF crCK)
{
	UNREFERENCED_PARAMETER(szFileName);
	UNREFERENCED_PARAMETER(crCK);
}

void CTileMapTool::ReAlloc(int nWidth, int nHeight)
{
	if(nWidth <= 0 || nWidth > MAX_TILE_MAP_WIDTH || nHeight <= 0 || nHeight > MAX_TILE_MAP_HEIGHT ) return;

	HGLOBAL					hTmpOld[5];
	_TILE_MAP_DATA_TOOL*	pTmpOld[5];
	int nOldW = m_HTMap.nWidth;
	int nOldH = m_HTMap.nHeight;

	int i = 0;

	if(nOldW > 0 && nOldH > 0) // 전의 데이터 포인터를 저장해 놓는다.
	{
		for(i = 0; i < 5; i++)
		{
			hTmpOld[i] = m_hDatasTool[i];
			pTmpOld[i] = m_pDatasTool[i];
		}
	}

	this->AllocTool(nWidth, nHeight);

	int w = 0, h = 0;
	if(nOldW < nWidth) w = nOldW; else w = nWidth;
	if(nOldH < nHeight) h = nOldH; else h = nHeight;

	if(h > 0 && w > 0) // 전의 맵 데이터 복사
	{
		for(i = 0; i < 5; i++)
		{
			for(int y = 0; y < h; y++)
			{
				memcpy(&m_pDatasTool[i][y*nWidth], &pTmpOld[i][y*nOldW], w*sizeof(_TILE_MAP_DATA_TOOL));
			}

			GlobalUnlock(hTmpOld[i]); // 해제
			GlobalFree(hTmpOld[i]);
			hTmpOld[i] = NULL;
			pTmpOld[i] = NULL;
		}
	}
}

void CTileMapTool::ReleaseEx()
{
	for(int i = 0; i < 3; i++) // 왕창 할당..
	{
		if(m_hDatasTool[i])
		{
			GlobalUnlock(m_hDatasTool[i]);
			GlobalFree(m_hDatasTool[i]);
			m_hDatasTool[i] = NULL;
			m_pDatasTool[i] = NULL;
		}
	}
	if(m_hDatasTool[4])
	{
		GlobalUnlock(m_hDatasTool[4]);
		GlobalFree(m_hDatasTool[4]);
		m_hDatasTool[4] = NULL;
		m_pDatasTool[4] = NULL;
	}

	MAKE_ZERO(m_HTMap);
}

void CTileMapTool::AllocTool(int nWidth, int nHeight)
{
	if(nWidth <= 0 || nHeight <= 0) return;
	
	m_HTMap.nWidth = nWidth;
	m_HTMap.nHeight = nHeight;
	
	for(int i = 0; i < 3; i++) // 할당
	{
		int nAllocSize = nWidth * nHeight * sizeof(_TILE_MAP_DATA_TOOL);
		m_hDatasTool[i] = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, nAllocSize);
		m_pDatasTool[i] = (_TILE_MAP_DATA_TOOL*) GlobalLock(m_hDatasTool[i]);
		memset(m_pDatasTool[i], 0xff, nAllocSize);
	}
	int nAllocSize = nWidth * nHeight * sizeof(_TILE_MAP_DATA_TOOL);
	m_hDatasTool[4] = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, nAllocSize);
	m_pDatasTool[4] = (_TILE_MAP_DATA_TOOL*) GlobalLock(m_hDatasTool[4]);
	memset(m_pDatasTool[4], 0xff, nAllocSize);
}
