// MapObjTool.cpp: implementation of the CMapObjTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igt4.h"
#include "MapObjTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapObjTool::CMapObjTool()
{

}

CMapObjTool::~CMapObjTool()
{
	for(int i = 0; i < MAX_MAP_OBJ_REG; i++)
	{
		m_Objs[i].AniRelease();
		m_Objs[i].SprRelease();
	}
}

void CMapObjTool::Save(const char *szFileName)
{
	CFile file;
	int i;
	int count;
	SAVE_BUF *t_buf;

	file.Open(szFileName, CFile::modeWrite | CFile::modeCreate);

	m_Header.szID[0] = 'M';
	m_Header.szID[1] = 'O';
	m_Header.szID[2] = 'B';
	m_Header.szID[3] = 'J'; 

	wsprintf(m_Header.szRemark, "맵 오브젝트 배치 데이터 - 너비 : %d개 높이 %d개", m_Header.nWidth, m_Header.nHeight);
	file.Write(&m_Header, sizeof(m_Header));

	count = 0;
	for(i = 0; i < m_Header.nWidth*m_Header.nHeight; i++)
	{
		if(m_pnData[i] >= 0) count++;
	}
	file.Write(&count, sizeof(int));		// 개수
	t_buf = new SAVE_BUF[count]; // index, value
	count = 0;
	for(i = 0; i < m_Header.nWidth*m_Header.nHeight; i++)
	{
		if(m_pnData[i] >= 0)
		{
			t_buf[count].index = i;
			t_buf[count++].value = m_pnData[i];
		}
	}
	file.Write(t_buf, sizeof(SAVE_BUF)*count);
	delete[] t_buf;

	file.Close();
}

void CMapObjTool::Save(CFile& file)
{
	int i;
	int count;
	SAVE_BUF *t_buf;

	m_Header.szID[0] = 'M';
	m_Header.szID[1] = 'O';
	m_Header.szID[2] = 'B';
	m_Header.szID[3] = 'J'; 

	wsprintf(m_Header.szRemark, "맵 오브젝트 배치 데이터 - 너비 : %d개 높이 %d개", m_Header.nWidth, m_Header.nHeight);
	file.Write(&m_Header, sizeof(m_Header));

	count = 0;
	for(i = 0; i < m_Header.nWidth*m_Header.nHeight; i++)
	{
		if(m_pnData[i] >= 0) count++;
	}
	file.Write(&count, sizeof(int));		// 개수
	t_buf = new SAVE_BUF[count]; // index, value
	count = 0;
	for(i = 0; i < m_Header.nWidth*m_Header.nHeight; i++)
	{
		if(m_pnData[i] >= 0)
		{
			t_buf[count].index = i;
			t_buf[count++].value = m_pnData[i];
		}
	}
	file.Write(t_buf, sizeof(SAVE_BUF)*count);
	delete[] t_buf;
}

void CMapObjTool::Realloc(int nWidth, int nHeight)
{
	if(nWidth <= 0 || nWidth > MAX_TILE_MAP_WIDTH || nHeight <= 0 || nHeight > MAX_TILE_MAP_HEIGHT ) return;

	HGLOBAL		hTmpOld = NULL;
	short int*	pTmpOld = NULL;
	
	int nOldW = m_Header.nWidth;
	int nOldH = m_Header.nHeight;

	int i = 0;

	if(nOldW > 0 && nOldH > 0) // 전의 데이터 포인터를 저장해 놓는다.
	{
		hTmpOld = m_hData;
		pTmpOld = m_pnData;
		m_hData = NULL;
		m_pnData = NULL;
	}

	this->Alloc(nWidth, nHeight);

	int w = 0, h = 0;
	if(nOldW < nWidth) w = nOldW; else w = nWidth;
	if(nOldH < nHeight) h = nOldH; else h = nHeight;

	if(h > 0 && w > 0) // 전의 맵 데이터 복사
	{
		for(int y = 0; y < h; y++)
		{
			memcpy(&m_pnData[y*nWidth], &pTmpOld[y*nOldW], w*sizeof(short int));
		}

		GlobalUnlock(hTmpOld); // 해제
		GlobalFree(hTmpOld);
		hTmpOld = NULL;
		pTmpOld = NULL;
	}
}

void CMapObjTool::ReleaseEx()
{
	CMapObj::Release();
	for(int i = 0; i < MAX_MAP_OBJ_REG; i++)
	{
		m_Objs[i].AniRelease();
		m_Objs[i].SprRelease();
	}
}
