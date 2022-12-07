// TMapTool.cpp: implementation of the CTMapTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileSetTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "DDEngineTool4.h"
extern CDDEngineTool4* g_pTEng;

CTileSetTool::CTileSetTool()
{
	MAKE_ZERO(m_Datas);
	MAKE_ZERO(m_HTSs);
	MAKE_ZERO(m_lpDDSs);
}

CTileSetTool::~CTileSetTool()
{
	ReleaseAll();
}

void CTileSetTool::ReAlloc(int nTS, int nTSS, int width, int height)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;
	if(width <= 0 || height <= 0) return;

	// 지우고 새로 할당..
	char szTmp[256];
	lstrcpy(szTmp, m_Datas[nTS][nTSS].szRemark);
	_TILE_SUB_SET_DATA_TOOL *sub_set = DataGetSafe(nTS, nTSS);
	sub_set->Alloc(width, height);
	lstrcpy(m_Datas[nTS][nTSS].szRemark, szTmp);
}

_TILE_SUB_SET_DATA_TOOL* CTileSetTool::DataGetSafe(int nTS, int nTSS)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return NULL;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return NULL;
	return &m_Datas[nTS][nTSS];
}

void CTileSetTool::DataSetSafe(int nTS, int nTSS, _TILE_SUB_SET_DATA_TOOL *pTileSet)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;
	if(pTileSet == NULL) return;

	m_Datas[nTS][nTSS] = *pTileSet;
}

POINT *CTileSetTool::DataGetPos(int nTS, int nTSS, int nIndex, int nAniNum)
{
	_TILE_SUB_SET_DATA_TOOL *sub_set = DataGetSafe(nTS, nTSS);
	if(sub_set == NULL) return NULL;

	return sub_set->GetPos(nIndex, nAniNum);
}

POINT *CTileSetTool::DataGetPosCur(int nTS, int nTSS, int nIndex)
{
	_TILE_SUB_SET_DATA_TOOL *sub_set = DataGetSafe(nTS, nTSS);
	if(sub_set == NULL) return NULL;

	return sub_set->GetPosCur(nIndex);
}

int *CTileSetTool::DataGetFlagCur(int nTS, int nTSS, int nIndex)
{
	_TILE_SUB_SET_DATA_TOOL *sub_set = DataGetSafe(nTS, nTSS);
	if(sub_set == NULL) return NULL;

	return sub_set->GetFlagCur(nIndex);
}

void CTileSetTool::Save(const char *szFileName)
{
	CFile file;
	file.Open(szFileName, CFile::modeCreate | CFile::modeWrite);
	for(int i = 0; i < MAX_TILE_SET; i++)
	{
		m_HTSs[i].nQtSubSet = 0; // 서브 셋의 갯수를 센다.
		for(int j = 0; j < MAX_TILE_SUB_SET; j++)
		{
			if(m_Datas[i][j].nWidth > 0 && m_Datas[i][j].nHeight > 0)
			{
				m_HTSs[i].nQtSubSet++;
			}
			else break;
		}

		file.Write(&m_HTSs[i], sizeof(_TILE_SET_HEADER));
		for(j = 0; j < m_HTSs[i].nQtSubSet; j++)
		{
			m_Datas[i][j].Save(file);
		}
	}
	file.Close();
}


void CTileSetTool::TileSetImport(int nTS, const char *szFileName)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	
	CFile file;
	if(file.Open(szFileName, CFile::modeRead) == FALSE) return;

	file.Read(&this->m_HTSs[nTS], sizeof(m_HTSs[nTS]));

	for(int i = 0; i < MAX_TILE_SUB_SET; i++)
	{
		m_Datas[nTS][i].Load(file);
	}
	file.Close();

	g_pTEng->PathSetCur("BMP");
	RELEASE_SAFE(m_lpDDSs[nTS]);
	m_lpDDSs[nTS] = g_pTEng->SurfaceCreateWithBMP(m_HTSs[nTS].szBMPFN, FALSE, m_HTSs[nTS].crCK); // 비트맵 읽기..
}

void CTileSetTool::TileSetExport(int nTS, const char *szFileName)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	
	CFile file;
	if(file.Open(szFileName, CFile::modeCreate | CFile::modeWrite) == FALSE) return;

	file.Write(&this->m_HTSs[nTS], sizeof(m_HTSs[nTS]));

	for(int i = 0; i < MAX_TILE_SUB_SET; i++)
	{
		m_Datas[nTS][i].Save(file);
	}
	file.Close();
}

void CTileSetTool::ReleaseAll()
{
	for(int i = 0; i < MAX_TILE_SET; i++)
	{
		for(int j = 0; j < MAX_TILE_SUB_SET; j++)
		{
			m_Datas[i][j].Release();
			MAKE_ZERO(m_Datas[i][j]);
		}
		RELEASE_SAFE(m_lpDDSs[i]);
	}
	MAKE_ZERO(m_HTSs);
	MAKE_ZERO(m_Datas);
}

void CTileSetTool::Release(int nTS, int nTSS)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;

	m_Datas[nTS][nTSS].Release();

	MAKE_ZERO(m_Datas[nTS][nTSS]);
}

void CTileSetTool::Alloc(int nTS, int nTSS, int width, int height)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return;
	if(width <= 0 || height <= 0) return;

	m_Datas[nTS][nTSS].Alloc(width, height);
}

// 로딩후 서피스는 따로.... 로딩!!!!
void CTileSetTool::Load(const char *szFileName)
{
	CFile file;
	
	if(!file.Open(szFileName, CFile::modeRead))
	{ 
		OutputDebugString("타일셋 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}

	this->ReleaseAll(); // 모두 해제

	for(int i = 0; i < MAX_TILE_SET; i++)
	{
		file.Read(&m_HTSs[i], sizeof(_TILE_SET_HEADER));
		for(int j = 0; j < m_HTSs[i].nQtSubSet; j++)
		{
			m_Datas[i][j].Load(file);
		}

		RELEASE_SAFE(m_lpDDSs[i]); // 서피스 할당..
		if(lstrlen(m_HTSs[i].szBMPFN) > 0)
		{
			g_pTEng->PathSetCur("BMP");
			m_lpDDSs[i] = g_pTEng->SurfaceCreateWithBMP(m_HTSs[i].szBMPFN, FALSE, m_HTSs[i].crCK);
		}
	}
	file.Close();
}

_TILE_SUB_SET_DATA_TOOL* CTileSetTool::DataGet(int nTS, int nTSS)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return NULL;
	if(nTSS < 0 || nTSS >= MAX_TILE_SUB_SET) return NULL;

	return &m_Datas[nTS][nTSS];
}

void CTileSetTool::ResetAni()
{
	for(int i = 0; i < MAX_TILE_SET; i++)
	{
		for(int j = 0; j < MAX_TILE_SUB_SET; j++)
		{
			m_Datas[i][j].nCurAni = 0;
		}
	}
}

void CTileSetTool::Increase()
{
	for(int i = 0; i < MAX_TILE_SET; i++)
	{
		for(int j = 0; j < MAX_TILE_SUB_SET; j++)
		{
			m_Datas[i][j].Increase();
		}
	}
}

void CTileSetTool::Decrease()
{
	for(int i = 0; i < MAX_TILE_SET; i++)
	{
		for(int j = 0; j < MAX_TILE_SUB_SET; j++)
		{
			m_Datas[i][j].Decrease();
		}
	}
}