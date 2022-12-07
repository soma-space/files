// MapObj.cpp: implementation of the CMapObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapObj.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMapObj::CMapObj()
{
	MAKE_ZERO(m_Header);
	m_hData = NULL;
	m_pnData = NULL;
}

CMapObj::~CMapObj()
{
	Release();
}

void CMapObj::Release()
{
	// 메모리 해제..
	MAKE_ZERO(m_Header);
	if(m_hData)
	{
		GlobalUnlock(m_hData);
		GlobalFree(m_hData);
	}
	m_hData = NULL;
	m_pnData = NULL;
}

void CMapObj::Alloc(int nWidth, int nHeight)
{
	if(nWidth <= 0 || nHeight <= 0) return;

	if(m_hData != NULL)
	{
		GlobalUnlock(m_hData);
		GlobalFree(m_hData);
	}

	m_Header.nWidth = nWidth;
	m_Header.nHeight = nHeight;

	// 할당하고 락..
	m_hData = GlobalAlloc(GMEM_FIXED, sizeof(short int)*nWidth*nHeight);
	m_pnData = (short int*)GlobalLock(m_hData);
	memset(m_pnData, -1, sizeof(short int)*nWidth*nHeight); // 초기화 값이어야 한다!!!
}

// 서피스는 따로 불러 주어야 한다..
void CMapObj::Load(const char *szFileName)
{
	HANDLE hFile;
	DWORD dwCount;
	int i, count;
	SAVE_BUF *t_buf;

	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
		OutputDebugString("타일맵 데이터 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}
	
	ReadFile(hFile, &m_Header, sizeof(_MAP_OBJ_HEADER), &dwCount, NULL);
	this->Alloc(m_Header.nWidth, m_Header.nHeight);
	
	SetAlphaObj(&m_Header);

	ReadFile(hFile, &count, sizeof(int), &dwCount, NULL);
	if(count > 0)
	{
		t_buf = new SAVE_BUF[count];
		ReadFile(hFile, t_buf, sizeof(SAVE_BUF)*count, &dwCount, NULL);
		for(i = 0; i < count; i++)
		{
			m_pnData[t_buf[i].index] = t_buf[i].value;
		}
		delete[] t_buf;
	}
	
	CloseHandle(hFile);
}

void CMapObj::Load(HANDLE& hFile)
{
	DWORD dwCount;
	int i, count;
	SAVE_BUF *t_buf;

	ReadFile(hFile, &m_Header, sizeof(_MAP_OBJ_HEADER), &dwCount, NULL);
	this->Alloc(m_Header.nWidth, m_Header.nHeight);

	SetAlphaObj(&m_Header);
	
	ReadFile(hFile, &count, sizeof(int), &dwCount, NULL);
	if(count > 0)
	{
		t_buf = new SAVE_BUF[count];
		ReadFile(hFile, t_buf, sizeof(SAVE_BUF)*count, &dwCount, NULL);
		for(i = 0; i < count; i++)
		{
			m_pnData[t_buf[i].index] = t_buf[i].value;
		}
		delete[] t_buf;
	}
}

void CMapObj::SetAlphaObj(_MAP_OBJ_HEADER* MOH)
{
	DeleteAlphaObj();	

	for (int i=0; i<MAX_MAP_OBJ_REG; i++)
	{
		if(MOH->szRegObjSprFNs[i][0] == 'c' && MOH->szRegObjSprFNs[i][1] == 'v')
			m_listHalfAlphaObj.insert( m_listHalfAlphaObj.end(), i );
		if(MOH->szRegObjSprFNs[i][0] == 'b' && MOH->szRegObjSprFNs[i][1] == 'l')
			m_listAddAlphaObj.insert( m_listAddAlphaObj.end(), i );
	}

}

BOOL CMapObj::GetHalfAlpha(int nSet)
{
	std::list<int>::iterator iter_Half;

	for ( iter_Half = m_listHalfAlphaObj.begin(); iter_Half != m_listHalfAlphaObj.end(); iter_Half++ )
	{
		if(*iter_Half == nSet) return TRUE;
	}
	return FALSE;

}

BOOL CMapObj::GetAddAlpha(int nSet)
{
	std::list<int>::iterator iter_Add;
	for ( iter_Add = m_listAddAlphaObj.begin(); iter_Add != m_listAddAlphaObj.end(); iter_Add++ )
	{
		if(*iter_Add == nSet) return TRUE;
	}
	return FALSE;
}

void CMapObj::DeleteAlphaObj()
{
	m_listHalfAlphaObj.erase(m_listHalfAlphaObj.begin(),m_listHalfAlphaObj.end());
	m_listAddAlphaObj.erase(m_listAddAlphaObj.begin(),m_listAddAlphaObj.end());
}