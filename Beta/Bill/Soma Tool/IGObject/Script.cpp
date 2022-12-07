// Script.cpp: implementation of the CScript class.
//
//////////////////////////////////////////////////////////////////////

#include "stdAfx.h"
#include "Script.h"

CScript::CScript()
{
	m_dwTickDelay = 0;
	MAKE_ZERO(m_Header);
	m_nIndex = 0;
	MAKE_ZERO(m_pDatas);
	MAKE_ZERO(m_szFileName);
}

CScript::~CScript()
{
	this->Release();
}

const _SCT_DATA* CScript::Get()
{
	return m_pDatas[m_nIndex];
}

const _SCT_DATA* CScript::Get(int index)
{
	if(index < 0 || index >= m_Header.nQt) return NULL; // 할당된 범위를 벗어나면..
	return m_pDatas[index];
}

void CScript::Load(const char* szFileName)
{
	HANDLE hFile;
	DWORD dwReadCount;
	
	// 전의 것들을 날려준다.
	Release();

	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
		MessageBox(NULL, szFileName, "파일 열기 실패", MB_OK);
		return;
	}

	lstrcpy(m_szFileName, szFileName); // 디버깅을 위해 최근 로딩한 스크립트파일이름을 기억해 둔다.

	SetFilePointer(hFile, 0, 0, FILE_BEGIN);

	// 헤더 읽기
	ReadFile(hFile, &m_Header, sizeof(_SCT_HEADER), &dwReadCount, NULL);

	// 숫자 만큼 메모리 새로 할당하고..
	Alloc(m_Header.nQt);
	
	//  읽는다.
	for(int i = 0; i < m_Header.nQt; i++)
	{
		ReadFile(hFile, m_pDatas[i], sizeof(_SCT_DATA), &dwReadCount, NULL);
		if(m_pDatas[i]->nText > 0)
		{
			m_pDatas[i]->szText = new char[m_pDatas[i]->nText+1];
			ReadFile(hFile, m_pDatas[i]->szText, m_pDatas[i]->nText, &dwReadCount, NULL);
			m_pDatas[i]->szText[m_pDatas[i]->nText] = NULL;
		}
	}
	CloseHandle(hFile);

	m_nIndex = 0; // 인덱스를 0 으로..
}

void CScript::Release()
{
	for(int i=0; i < MAX_SCT; i++)
	{
		if(m_pDatas[i] != NULL)
		{
			if(m_pDatas[i]->szText) delete [] m_pDatas[i]->szText; m_pDatas[i]->szText = NULL;
			delete m_pDatas[i];
			m_pDatas[i] = NULL;
		}
	}
	MAKE_ZERO(m_Header);	// 스프라이트 헤더를 널로..
	this->m_nIndex = 0;
}

void CScript::Alloc(int nQt)
{
	if(nQt <= 0 || nQt > MAX_SCT) return;

	m_Header.nQt = nQt;
	
	// 할당 및 초기화...
	for(int i = 0; i < nQt; i++)
	{
		if(m_pDatas[i])
		{
			if(m_pDatas[i]->szText) { delete m_pDatas[i]->szText; m_pDatas[i]->szText = NULL; }
			delete m_pDatas[i];
			m_pDatas[i] = NULL;
		}
		m_pDatas[i] = new _SCT_DATA;
		MAKE_ZERO(*m_pDatas[i]);
	}
}
