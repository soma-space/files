// LightSource.cpp: implementation of the CLightSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightSource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightSource::CLightSource()
{
	m_pDatas = NULL;
}

CLightSource::~CLightSource()
{
}

void CLightSource::Release()
{
	memset(&m_Header, 0, sizeof(_LIGHT_SOURCE_HEADER));

	if(m_pDatas) delete [] m_pDatas;
	m_pDatas = NULL;
}

_LIGHT_SOURCE_DATA* CLightSource::DataGet(int index)
{
	if(index < 0 || index >= m_Header.nQt) return NULL;
	return &m_pDatas[index];
}

void CLightSource::Alloc(int nQt)
{
	if(nQt <= 0) return;

	this->Release();

	m_Header.nQt = nQt;
	m_pDatas = new _LIGHT_SOURCE_DATA[nQt];
}

void CLightSource::Load(const char* szFileName)
{
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
		OutputDebugString("광원 데이터 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}

	DWORD dwCount;
	ReadFile(hFile, &m_Header, sizeof(_LIGHT_SOURCE_HEADER), &dwCount, NULL);

	_LIGHT_SOURCE_HEADER Header;
	memcpy(&Header, &m_Header, sizeof(_LIGHT_SOURCE_HEADER));
	this->Alloc(m_Header.nQt);
	memcpy(&m_Header, &Header, sizeof(_LIGHT_SOURCE_HEADER));

	ReadFile(hFile, m_pDatas, sizeof(_LIGHT_SOURCE_DATA)*m_Header.nQt, &dwCount, NULL);

	CloseHandle(hFile);
}
