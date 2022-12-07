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
		this->Release();
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
	
	ReadFile(hFile, m_pnData, sizeof(short int)*m_Header.nWidth*m_Header.nHeight, &dwCount, NULL);
	
	CloseHandle(hFile);
}

