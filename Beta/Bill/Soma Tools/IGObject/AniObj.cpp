// AniObj.cpp: implementation of the CAniObj class.
//
//////////////////////////////////////////////////////////////////////
#include "stdAfx.h"
#include "AniObj.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAniObj::CAniObj()
{
	m_dwTickFrms = NULL;
	MAKE_ZERO(m_HAni);
	m_nCtrlQt = NULL;
	m_nCurAnis = NULL;
	m_nCurDirs = NULL;
	m_nCurFrms = NULL;
	m_nDefaultAnis = NULL;
	m_nFlags = NULL;
	MAKE_ZERO(m_pAnis);
	m_fASpeed = 1.0f; // 기본적인 액션 스피드는 1.0

	AniCtrlAlloc(2); // 최소 두개 할당...
}

CAniObj::~CAniObj()
{
	this->AniRelease();
	AniCtrlRelease();
}

void CAniObj::AniRelease()
{
	MAKE_ZERO(m_HAni);	// 에니메이션 헤더를 널로..
	for(int i=0; i < MAX_ANI; i++) // 할당된 메모리 널로...
	{
		if(m_pAnis[i]) 
		{
			DELETE_ARRAY_SAFE(m_pAnis[i]->pnFrms);
			DELETE_SAFE(m_pAnis[i]);
			m_pAnis[i] = NULL;
		}
	}
}

void CAniObj::AniCtrlRelease()
{
	DELETE_ARRAY_SAFE(m_dwTickFrms);
	DELETE_ARRAY_SAFE(m_nDefaultAnis);
	DELETE_ARRAY_SAFE(m_nCurAnis);
	DELETE_ARRAY_SAFE(m_nCurDirs);
	DELETE_ARRAY_SAFE(m_nCurFrms);
	DELETE_ARRAY_SAFE(m_nFlags);
	m_nCtrlQt = 0;
}

void CAniObj::AniLoad(const char* szFileName)
{
	if(m_HAni.nQt > 0) this->AniRelease(); // 기존의 데이터 삭제.

	HANDLE hFile;
	DWORD dwCount;

	////////////////////////////////////////////////////////////////////////////////
	// 에니메이션...................................................................
	if(szFileName[0] == 0) return;
	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{ 
#ifdef _DEBUG
		OutputDebugString("에니메이션 파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
#endif
		return;
	}

	ReadFile(hFile, &m_HAni, sizeof(_ANI_HEADER), &dwCount, NULL); // 에니메이션 헤더 읽기
	if(m_HAni.nDirQt <= 0 || m_HAni.szID[0] != 'A' || m_HAni.szID[1] != 'N' || m_HAni.szID[2] != 'I')
	{
#ifdef _DEBUG
		OutputDebugString("에니메이션 파일 열기 실패 : 잘못된 파일 헤더 - ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
#endif
		CloseHandle(hFile);
		return;
	}

	AniAlloc(m_HAni.nQt); // 할당..
	for(int i = 0; i < m_HAni.nQt; i++)
	{
		ReadFile(hFile, m_pAnis[i], sizeof(_ANI_DATA), &dwCount, NULL); // 에니메이션 데이터 읽기
		if(m_pAnis[i]->fSpd == 0.0f) m_pAnis[i]->fSpd = 10.0f;
		if(m_pAnis[i]->nMaxFrm > 0)
		{
			m_pAnis[i]->pnFrms = new short int[m_pAnis[i]->nMaxFrm*m_HAni.nDirQt];
			ReadFile(hFile, m_pAnis[i]->pnFrms, m_pAnis[i]->nMaxFrm*m_HAni.nDirQt*2, &dwCount, NULL);
		}
	}
	CloseHandle(hFile);
	// 에니메이션...................................................................
	////////////////////////////////////////////////////////////////////////////////
}

void CAniObj::AniAlloc(int nQt)
{
	if(nQt <= 0 || nQt > MAX_ANI) return;

	for(int i=0; i < nQt; i++)
	{ 
		if(m_pAnis[i] != NULL) continue;

		m_pAnis[i] = new _ANI_DATA;
		memset(m_pAnis[i], 0, sizeof(_ANI_DATA));
	}
	m_HAni.nQt = nQt;
}

// 에니메이션 컨트롤 수 지정..
void CAniObj::AniCtrlAlloc(int nQt)
{
	if(nQt <= 0 || nQt > MAX_ANI_CTRL) return;
	
	AniCtrlRelease(); // 일단 컨트롤 모두 해제..

	m_nCtrlQt = nQt;
	m_dwTickFrms = new DWORD[nQt]; memset(m_dwTickFrms, 0, nQt*4);
	m_nDefaultAnis = new int[nQt]; memset(m_nDefaultAnis, 0, nQt*4);
	m_nCurAnis = new int[nQt]; memset(m_nCurAnis, 0, nQt*4);
	m_nCurDirs = new int[nQt]; memset(m_nCurDirs, 0, nQt*4);
	m_nCurFrms = new int[nQt]; memset(m_nCurFrms, 0, nQt*4);
	m_nFlags = new int[nQt]; memset(m_nFlags, 0, nQt*4);
}

_ANI_DATA* CAniObj::AniDataGet(int index)
{
	if(index < 0 || index >= m_HAni.nQt) return NULL;
	return m_pAnis[index];
}

void CAniObj::AniSet(int nAni, int nFlag, int nCtrl)
{
	if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return; // 컨트롤 범위 검사
	if(nAni < 0 || nAni >= m_HAni.nQt) return;
	if(nFlag == FL_ANI_LOOP)
	{
		m_nDefaultAnis[nCtrl] = nAni;
	}
	
	if(nAni != m_nCurAnis[nCtrl]) m_nCurFrms[nCtrl] = 0; // 다른 에니메이션이 들어오면 프레임을 0 으로 돌린다.
	m_nFlags[nCtrl] = nFlag;
	m_nCurAnis[nCtrl] = nAni;
}

void CAniObj::FrmSet(int nFrm, int nCtrl)
{
	if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return; // 컨트롤 범위 검사
	if(nFrm < 0 || nFrm >= MAX_FRM) return;
	m_nCurFrms[nCtrl] = nFrm;
}

void CAniObj::DirSet(int nDir, int nCtrl)
{
	if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return; // 컨트롤 범위 검사
	if(nDir < 0 || nDir >= m_HAni.nDirQt || nDir == m_nCurDirs[nCtrl]) return;
	m_nCurDirs[nCtrl] = nDir;
	m_nCurFrms[nCtrl] = 0;
}

int CAniObj::FrmGetMax(int nCtrl)
{
	if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return 0;
	if(m_pAnis[m_nCurAnis[nCtrl]] == NULL) return 0;
	return m_pAnis[m_nCurAnis[nCtrl]]->nMaxFrm;
}

float CAniObj::ActionSpeedSet(float fSpeed)
{
	float f = m_fASpeed;
	if(fSpeed <= 0)
		fSpeed = 1.00f;
	m_fASpeed = fSpeed;
	return f;
}

int CAniObj::FrmGetByAniAndDirAndSeq(int nAni, int nDir, int nFrm)
{
	if(nAni < 0 || nAni >= m_HAni.nQt) return -1;
	int nMaxDir = m_HAni.nDirQt;
	if(nDir < 0 || nDir >= nMaxDir) return -1;
	int nMaxFrm = m_pAnis[nAni]->nMaxFrm;
	if(nFrm < 0 || nFrm >= nMaxFrm) return -1;

	if(m_pAnis[nAni] == NULL) return -1; // 에러 체크
	return m_pAnis[nAni]->pnFrms[nDir*nMaxFrm + nFrm];
}

int CAniObj::FrmGetAnimated(int nCtrl)
{
	if(nCtrl < 0 || nCtrl >= m_nCtrlQt) return -1; // 컨트롤 범위 검사
	if(m_pAnis[m_nCurAnis[nCtrl]] == NULL) return -1;
	int nMaxFrm = m_pAnis[m_nCurAnis[nCtrl]]->nMaxFrm;
	if(nMaxFrm <= 0) return -1;
	
	if(GetTickCount() > m_dwTickFrms[nCtrl] + (1000 / m_pAnis[m_nCurAnis[nCtrl]]->fSpd * m_fASpeed)) // 시간에 따라 제어...
	{
		m_dwTickFrms[nCtrl] = GetTickCount();
		m_nCurFrms[nCtrl]++;
	}
	
	if(m_nCurFrms[nCtrl] >= nMaxFrm) // 프레임의 마지막이면..
	{
		if(m_nFlags[nCtrl] == FL_ANI_ONCE)
		{
			m_nCurAnis[nCtrl] = m_nDefaultAnis[nCtrl]; // 기본 에니메이션으로 돌아간다..
			m_nCurFrms[nCtrl] = 0;
		}
		else if(m_nFlags[nCtrl] == FL_ANI_ONCE_FIX)
		{
			m_nCurFrms[nCtrl] = nMaxFrm - 1;
		}
		else // if(m_nFlags[nCtrl] == FL_ANI_LOOP)
		{
			m_nCurFrms[nCtrl] = 0;
		}
	}

	int nDir = m_nCurDirs[nCtrl];
	int nFrm = m_nCurFrms[nCtrl];
	return m_pAnis[m_nCurAnis[nCtrl]]->pnFrms[nDir*nMaxFrm + nFrm];
}
