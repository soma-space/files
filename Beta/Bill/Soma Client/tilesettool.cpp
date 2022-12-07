// TMapTool.cpp: implementation of the CTMapTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileSetTool.h"
#include "MainFrm.h"

//#include "IGObject\\DDEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

_TILE_SET_TOOL::_TILE_SET_TOOL()
{
	m_Datas = NULL;
	m_lpDDSs = NULL;
}

_TILE_SET_TOOL::~_TILE_SET_TOOL()
{
	if(m_Datas)
	{
		delete[] m_Datas;
	}
	if(m_lpDDSs)
	{
		m_lpDDSs->Release();
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//extern CDDEngine* g_pTEng = NULL;
//extern CDDEngine* test = NULL;

CTileSetTool::CTileSetTool()
{
}

CTileSetTool::~CTileSetTool()
{
	ReleaseAll();
}

_TILE_SUB_SET_DATA_TOOL* CTileSetTool::DataGetSafe(int nTS, int nTSS)
{
	_TILE_SET_TOOL *value;

	if(m_mapData.Lookup(nTS, value) == FALSE) return NULL;
	if(nTSS < 0 || nTSS >= value->m_HTSs.nQtSubSet) return NULL;
	return &value->m_Datas[nTSS];
}

_TILE_SET_TOOL *CTileSetTool::GetTileSet(int nTS)
{
	_TILE_SET_TOOL *value;

	if(m_mapData.Lookup(nTS, value) == FALSE) return NULL;
	else return value;
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

void CTileSetTool::ReleaseAll()
{
	POSITION pos = m_mapData.GetStartPosition();
	int key;
	_TILE_SET_TOOL *value;

	while(pos)
	{
		m_mapData.GetNextAssoc(pos, key, value);
		delete value;
	}
	m_mapData.RemoveAll();
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

	ReleaseAll(); // 모두 해제

	_TILE_SET_TOOL *value;
	_TILE_SET_HEADER t_hts;

	for(int i = 0; i < MAX_TILE_SET; i++)
	{
		file.Read(&t_hts, sizeof(_TILE_SET_HEADER));
		if(t_hts.nQtSubSet > 0)
		{
			value = new _TILE_SET_TOOL;
			CopyMemory(&value->m_HTSs, &t_hts, sizeof(_TILE_SET_HEADER));
			value->m_Datas = new _TILE_SUB_SET_DATA_TOOL[t_hts.nQtSubSet];
			for(int j = 0; j < t_hts.nQtSubSet; j++)
			{
				value->m_Datas[j].Load(file);
			}
			m_mapData[i] = value;
		}
	}
	file.Close();
}

void CTileSetTool::ResetAni()
{
	POSITION pos = m_mapData.GetStartPosition();
	int key, i;
	_TILE_SET_TOOL *value;

	while(pos)
	{
		m_mapData.GetNextAssoc(pos, key, value);
		for(i = 0; i < value->m_HTSs.nQtSubSet; i++)
		{
			value->m_Datas[i].nCurAni = 0;
		}
	}
}

void CTileSetTool::Increase()
{
	POSITION pos = m_mapData.GetStartPosition();
	int key, i;
	_TILE_SET_TOOL *value;

	while(pos)
	{
		m_mapData.GetNextAssoc(pos, key, value);
		for(i = 0; i < value->m_HTSs.nQtSubSet; i++)
		{
			value->m_Datas[i].Increase();
		}
	}
}

void CTileSetTool::Decrease()
{
	POSITION pos = m_mapData.GetStartPosition();
	int key, i;
	_TILE_SET_TOOL *value;

	while(pos)
	{
		m_mapData.GetNextAssoc(pos, key, value);
		for(i = 0; i < value->m_HTSs.nQtSubSet; i++)
		{
			value->m_Datas[i].Decrease();
		}
	}
}