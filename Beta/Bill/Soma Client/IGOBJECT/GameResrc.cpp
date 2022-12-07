// GameResrc.cpp: implementation of the CGameResrc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameResrc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameResrc::CGameResrc()
{
	MAKE_ZERO(m_szMapFNs[MAX_MAP_RESRC][64]);
	MAKE_ZERO(m_szAvata0FNs[MAX_AVATA_RESRC][64]); // 첫번째는 에니메이션 나머지는 스프라이트
	MAKE_ZERO(m_szAvata1FNs[MAX_AVATA_RESRC][64]); // 첫번째는 에니메이션 나머지는 스프라이트
	MAKE_ZERO(m_szNPCAniFNs[MAX_NPC_RESRC][64]);
	MAKE_ZERO(m_szNPCSprFNs[MAX_NPC_RESRC][64]);
	MAKE_ZERO(m_szNPCSprShadowFNs[MAX_NPC_RESRC][64]);
}

CGameResrc::~CGameResrc()
{

}

// nType - 0:맵, 1:남자아바타, 2:여자아바타, 3:NPC에니메이션, 4:NPC스프라이트
// nIndex - 타입이 아바타일 경우 0은 에니메이션 파일 이름, 나머지는 순서대로 스프라이트
const char* CGameResrc::FileNameGet(int nType, int nIndex)
{
	if(nType < 0 || nType > 5) return NULL;
	if(nIndex < 0) return NULL;

	if(nType == 0 && nIndex < MAX_MAP_RESRC)		return m_szMapFNs[nIndex];
	else if(nType == 1 && nIndex < MAX_AVATA_RESRC) return m_szAvata0FNs[nIndex];
	else if(nType == 2 && nIndex < MAX_AVATA_RESRC) return m_szAvata1FNs[nIndex];
	else if(nType == 3 && nIndex < MAX_NPC_RESRC)	return m_szNPCAniFNs[nIndex];
	else if(nType == 4 && nIndex < MAX_NPC_RESRC)	return m_szNPCSprFNs[nIndex];
	else if(nType == 5 && nIndex < MAX_NPC_RESRC)	return m_szNPCSprShadowFNs[nIndex];

	return NULL;
}

void CGameResrc::FileNameSet(int nType, int nIndex, const char *szFN)
{
	if(nType < 0 || nType > 5) return;
	if(nIndex < 0) return;

	if(nType == 0 && nIndex < MAX_MAP_RESRC)		lstrcpy(m_szMapFNs[nIndex], szFN);
	else if(nType == 1 && nIndex < MAX_AVATA_RESRC) lstrcpy(m_szAvata0FNs[nIndex], szFN);
	else if(nType == 2 && nIndex < MAX_AVATA_RESRC) lstrcpy(m_szAvata1FNs[nIndex], szFN);
	else if(nType == 3 && nIndex < MAX_NPC_RESRC)	lstrcpy(m_szNPCAniFNs[nIndex], szFN);
	else if(nType == 4 && nIndex < MAX_NPC_RESRC)	lstrcpy(m_szNPCSprFNs[nIndex], szFN);
	else if(nType == 5 && nIndex < MAX_NPC_RESRC)	lstrcpy(m_szNPCSprShadowFNs[nIndex], szFN);
}
