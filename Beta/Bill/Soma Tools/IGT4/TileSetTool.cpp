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
	OnCreate();
}

CTileSetTool::~CTileSetTool()
{
	ReleaseAll();
	OnDestroy();
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

BOOL CTileSetTool::Delete(int nTS)
{
	if(nTS < 0 || nTS >= MAX_TILE_SET) return FALSE;
	int i, j;

	DeleteCore(nTS);
	for(i = nTS; i < MAX_TILE_SET-1; i++)
	{
		for(j = 0; j < MAX_TILE_SUB_SET; j++)
		{
			m_Datas[i][j].Release();
			MAKE_ZERO(m_Datas[i][j]);
		}
		m_lpDDSs[i] = NULL;
		m_HTSs[i].szName[0] = 0;
		m_HTSs[i].szBMPFN[0] = 0;
		m_HTSs[i].crCK = 0;
		m_HTSs[i].nQtSubSet = 0;
		for(j = 0; j < MAX_TILE_SUB_SET; j++)
		{
			if(m_Datas[i+1][j].nWidth > 0 && m_Datas[i+1][j].nHeight > 0)
			{
				m_HTSs[i].nQtSubSet++;
				m_Datas[i][j] = m_Datas[i+1][j];
			}
			else break;
		}
		if(m_HTSs[i].nQtSubSet == 0) break;

		m_lpDDSs[i] = m_lpDDSs[i+1];
		lstrcpy(m_HTSs[i].szName, m_HTSs[i+1].szName);
		lstrcpy(m_HTSs[i].szBMPFN, m_HTSs[i+1].szBMPFN);
		m_HTSs[i].crCK = m_HTSs[i+1].crCK;
	}

	return TRUE;
}

void CTileSetTool::DeleteCore(int nTS)
{
	int i;

	for(i = 0; i < MAX_TILE_SUB_SET; i++)
	{
		m_Datas[nTS][i].Release();
		MAKE_ZERO(m_Datas[nTS][i]);
	}
	RELEASE_SAFE(m_lpDDSs[nTS]);
	m_HTSs[nTS].szName[0] = 0;
	m_HTSs[nTS].szBMPFN[0] = 0;
	m_HTSs[nTS].crCK = 0;
	m_HTSs[nTS].nQtSubSet = 0;
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
	CString szBMPFN;
	
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
			if(m_lpDDSs[i] == NULL)
			{
				szBMPFN = m_HTSs[i].szBMPFN;
				szBMPFN = szBMPFN.Left(szBMPFN.GetLength() - 3);
				szBMPFN += "obm";
				m_lpDDSs[i] = g_pTEng->SurfaceCreateWithBMP(szBMPFN, FALSE, m_HTSs[i].crCK);
			}
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

void CTileSetTool::OnSaveSplFromTileSet(CString szFileName, int nTS)
{
	if( nTS < 0 || nTS > MAX_TILE_SET || m_pAniObj == NULL )
	{
		if( m_pAniObj == NULL ) 
		{
			AfxMessageBox("메모리가 부족합니다.\n현재 실행중인 프로그램들을 종료하시고\n다시 시도하십시요", MB_ICONINFORMATION|MB_OK);
			m_pAniObj = new CAniObjTool();
		}
		else
			AfxMessageBox("타일셋 범위 땜시 저장실패\n", MB_ICONINFORMATION|MB_OK);
		return;
	}

	RELEASE_SAFE(m_pAniObj->m_lpDDS);
	m_pAniObj->m_lpDDS = m_lpDDSs[nTS];
	lstrcpy(m_pAniObj->m_HSpr.szBMPFN, szFileName);

	if(m_pAniObj->m_lpDDS) 
	{
		CRect rc; g_pTEng->SurfaceGetSize(m_pAniObj->m_lpDDS, &rc);
		m_pAniObj->m_HSpr.nWidth = rc.Width();
		m_pAniObj->m_HSpr.nHeight = rc.Height();
	}
	else
	{
		AfxMessageBox("타일셋 서피스가 없음\n", MB_ICONINFORMATION|MB_OK);
		return;
	}
	m_pAniObj->m_HSpr.nQt = m_HTSs[nTS].nQtSubSet;

	m_pAniObj->m_HSpr.SrcCK = m_HTSs[nTS].crCK;
	DDCOLORKEY ddck;
	DWORD CK = m_HTSs[nTS].crCK;
	ddck.dwColorSpaceHighValue = CK;
	ddck.dwColorSpaceLowValue = CK;
	m_pAniObj->m_lpDDS->SetColorKey(DDCKEY_SRCBLT , &ddck);

	for(int j = 0; j < m_HTSs[nTS].nQtSubSet; j++)
	{
		int nWidth  = m_Datas[nTS][j].nWidth;
		int nHeight = m_Datas[nTS][j].nHeight;
		if( nWidth > 16 || nHeight > 16 )
		{
			CString szFail;
			szFail.Format("%s 서브셋이 넘 커요. 서브셋 크기를 \n16*16이내로(단위: 픽셀32) 조정바람\n그래도 계속 하시겠습니까?", m_HTSs[nTS].szName);
			if(AfxMessageBox(szFail, MB_ICONINFORMATION|MB_YESNO) == IDNO)
			{
				m_pAniObj->m_lpDDS = NULL;
				m_pAniObj->SprNew();
				return;
			}
		}
		CRect rcSubSet;
	
		POINT *t_pos	= DataGetPos( nTS, j, 0);
		rcSubSet.left	= t_pos->x * 32;
		rcSubSet.top	= t_pos->y * 32;
		rcSubSet.right	= (t_pos->x + nWidth) * 32;
		rcSubSet.bottom = (t_pos->y + nHeight)* 32;
		
		_SPR_TOOL spr;
		spr.rcV = rcSubSet;
		spr.ptCV.x = rcSubSet.left;
		spr.ptCV.y = rcSubSet.top;
		m_pAniObj->SprExSetData(j, &spr);
	}

	g_pTEng->PathSetCur("DATA"); // 경로 지정
	m_pAniObj->SprSave(szFileName);
	m_pAniObj->m_lpDDS = NULL;
	m_pAniObj->SprNew();
	AfxMessageBox("저장됨\n", MB_ICONINFORMATION|MB_OK);
}

void CTileSetTool::OnCreate()
{
	m_pAniObj = new CAniObjTool();
}

void CTileSetTool::OnDestroy()
{
	DELETE_SAFE(m_pAniObj);
}
