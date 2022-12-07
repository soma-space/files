// Intf.cpp: implementation of the CIntf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdAfx.h"
#include "Intf.h"
#include "AniObjLSP.h"

CIntf::CIntf()
{
	MAKE_ZERO(m_Header);
	MAKE_ZERO(m_pIntfs);
	m_pAniObj = new CAniObjLSP();
}

CIntf::~CIntf()
{
	DELETE_SAFE(m_pAniObj);
	this->IntfRelease();
}

void CIntf::IntfRelease()
{
	MAKE_ZERO(m_Header);
	for(int i = 0; i < MAX_INTF; i++)
	{
		if(m_pIntfs[i])
		{
			DELETE_ARRAY_SAFE(m_pIntfs[i]->szText);
			DELETE_SAFE(m_pIntfs[i]);
		}
	}
}

void CIntf::IntfAlloc(int nQt)
{
	if(nQt <= 0) return;
	for(int i = 0; i < nQt; i++)
	{
		if(m_pIntfs[i])
		{
			DELETE_ARRAY_SAFE(m_pIntfs[i]->szText);
			DELETE_SAFE(m_pIntfs[i]);
		}
		m_pIntfs[i] = new _INTF_DATA;
		memset(m_pIntfs[i], 0, sizeof(_INTF_DATA));
	}
	m_Header.nQt = nQt;
}

_INTF_DATA* CIntf::IntfGet(int index)
{
	if(index < 0 || index >= MAX_INTF) return NULL;
	return m_pIntfs[index];
}

_INTF_DATA* CIntf::IntfGet(int nLayer, int nSeq)
{
	int n = 0;
	for(int i = 0; i < MAX_INTF; i++)
	{
		if(m_pIntfs[i] == NULL) continue;
		if(m_pIntfs[i]->nParentLayer != nLayer) continue;
		if(n == nSeq) return m_pIntfs[i];
		n++;
	}

	return NULL;
}

void CIntf::Load(const char* szFileName, const char* szAniPath, const char* szSprPath, int nPF)
{
	HANDLE hFile;
	DWORD dwCount;

	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		OutputDebugString("파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}

	this->IntfRelease();

	ReadFile(hFile, &m_Header, sizeof(_INTF_HEADER), &dwCount, NULL); // 인터페이스 헤더 읽기.

	this->IntfAlloc(m_Header.nQt);

	int i;
	for(i = 0; i < m_Header.nQt; i++)
	{
		ReadFile(hFile, m_pIntfs[i], sizeof(_INTF_DATA), &dwCount, NULL); // 인터페이스 데이터 읽기.
		m_pIntfs[i]->szText = NULL;
		if(m_pIntfs[i]->nText > 0)
		{
			m_pIntfs[i]->szText = new char[m_pIntfs[i]->nText + 1];
			ReadFile(hFile, m_pIntfs[i]->szText, m_pIntfs[i]->nText, &dwCount, NULL);
			m_pIntfs[i]->szText[m_pIntfs[i]->nText] = NULL;
		}
	}
	CloseHandle(hFile);

	SetCurrentDirectory(szAniPath); // 에니메이션, 스프라이트 읽기...
	m_pAniObj->AniLoad(m_Header.szAniFN);
	m_pAniObj->AniCtrlAlloc(m_Header.nQt*4); // 갯수 만큼 에니메이션 제어 할당...
	for(i = 0; i < m_Header.nQt; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			m_pAniObj->AniSet(m_pIntfs[i]->nAnis[j], FL_ANI_LOOP, i*4+j); // 에니메이션 지정
		}
	}
	SetCurrentDirectory(szSprPath);
	m_pAniObj->SprLoad(m_Header.szSprFN, nPF);
}

int CIntf::HitTest(int nLayer, POINT pt)
{
	if(nLayer < 0 || nLayer >= MAX_LAYER) return -1;

	for(int i = 0; i < MAX_INTF; i++)
	{
		if(m_pIntfs[i] == NULL) continue;
		if(PtInRect(&m_pIntfs[i]->rcRgn, pt)) return i;
	}

	return -1;
}
