// GameResrc.h: interface for the CGameResrc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMERESRC_H__DAD18118_AF4A_47F1_A106_B461B59B9F30__INCLUDED_)
#define AFX_GAMERESRC_H__DAD18118_AF4A_47F1_A106_B461B59B9F30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const int MAX_MAP_RESRC = 32;
const int MAX_AVATA_RESRC = 32;
const int MAX_NPC_RESRC = 128;

class CGameResrc  
{
protected:
	char m_szMapFNs[MAX_MAP_RESRC][64];
	char m_szAvata0FNs[MAX_AVATA_RESRC][64]; // 첫번째는 에니메이션 나머지는 스프라이트
	char m_szAvata1FNs[MAX_AVATA_RESRC][64]; // 첫번째는 에니메이션 나머지는 스프라이트
	char m_szNPCAniFNs[MAX_NPC_RESRC][64];
	char m_szNPCSprFNs[MAX_NPC_RESRC][64];
	char m_szNPCSprShadowFNs[MAX_NPC_RESRC][64];

public:
	void FileNameSet(int nType, int nIndex, const char* szFN);
	const char* FileNameGet(int nType, int nIndex);
	
	CGameResrc();
	virtual ~CGameResrc();

};

#endif // !defined(AFX_GAMERESRC_H__DAD18118_AF4A_47F1_A106_B461B59B9F30__INCLUDED_)
