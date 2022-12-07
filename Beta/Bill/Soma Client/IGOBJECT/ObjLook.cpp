// ObjLook.cpp: implementation of the CObjLook class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ObjLook.h"
#include "AniObjLSP.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjLook::CObjLook()
{
	MAKE_ZERO(m_pDatas);
	MAKE_ZERO(m_Header);
	m_AniObj.AniCtrlAlloc(MAX_LOOK_PART);
}

CObjLook::~CObjLook()
{
	this->Release();
}

void CObjLook::Release()
{
	m_AniObj.SprRelease();
	m_AniObj.AniRelease();

	MAKE_ZERO(m_Header);
	for(int i = 0; i < MAX_LOOK; i++)
	{
		DELETE_SAFE(m_pDatas[i]);
	}
}

_LOOK_DATA* CObjLook::DataGet(int index)
{
	if(index < 0 || index >= MAX_LOOK) return NULL;
	
	return m_pDatas[index];
}

void CObjLook::Load(const char *szFileName, const char *szAniPath, const char *szSprPath, int nPF)
{
	HANDLE hFile;
	DWORD dwCount;

	hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
		OutputDebugString("얼굴 데이터 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}

	ReadFile(hFile, &m_Header, sizeof(_LOOK_HEADER), &dwCount, NULL); // 헤더 읽기

	if(	m_Header.szID[0] != 'L' ||
		m_Header.szID[1] != 'A' ||
		m_Header.szID[2] != 'D' ||
		m_Header.szID[3] != 'F')
	{
		OutputDebugString("얼굴 데이터 파일 읽기 실패 : 잘못된 파일 헤더 - ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		CloseHandle(hFile);
		return;
	}

	this->Alloc(m_Header.nQt);

	for(int i = 0; i < m_Header.nQt; i++)
	{
		ReadFile(hFile, m_pDatas[i], sizeof(_LOOK_DATA), &dwCount, NULL);
	}

	CloseHandle(hFile);

	// 에니메이션,  스프라이트 읽기.
	SetCurrentDirectory(szAniPath);
	m_AniObj.AniLoad(m_Header.szAniFN);
	m_AniObj.AniCtrlAlloc(MAX_LOOK_PART);

	SetCurrentDirectory(szSprPath);
	m_AniObj.SprLoad(m_Header.szSprFN, nPF);
}

void CObjLook::Alloc(int nQt)
{
	if(nQt <= 0 || nQt > MAX_LOOK) return;

	m_Header.nQt = nQt;
	for(int i = 0; i < m_Header.nQt; i++)
	{
		DELETE_SAFE(m_pDatas[i]);
		m_pDatas[i] = new _LOOK_DATA;
		MAKE_ZERO(*m_pDatas[i]);
	}
}
