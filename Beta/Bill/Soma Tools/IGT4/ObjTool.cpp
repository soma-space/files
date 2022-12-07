// ObjTool.cpp: implementation of the CObjTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igt4.h"
#include "ObjTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjTool::CObjTool()
{
	for(int i = 0; i < MAX_OBJ_ANI; i++)
	{
		m_pAniObjs[i] = new CAniObjLSP();
		m_pSprStands[i] = new _SPR_LSP;
		MAKE_ZERO(*m_pSprStands[i]);
	}
}

CObjTool::~CObjTool()
{
	for(int i = 0; i < MAX_OBJ_ANI; i++)
	{
		DELETE_SAFE(m_pAniObjs[i]);
		DELETE_ARRAY_SAFE(m_pSprStands[i]->pwV);
		DELETE_SAFE(m_pSprStands[i]);
	}
}

void CObjTool::New()
{
	MAKE_ZERO(m_Header);

	for(int i = 0; i < MAX_OBJ_ANI; i++)
	{
		m_pAniObjs[i]->SprRelease();
		m_pAniObjs[i]->AniRelease();
		m_pAniObjs[i]->AniCtrlRelease();

		DELETE_ARRAY_SAFE(m_pSprStands[i]->pwV);
		MAKE_ZERO(*m_pSprStands[i]);
	}
}

void CObjTool::Save(const char *szFileName, int nPF)
{
	m_Header.szID[0] = 'C'; m_Header.szID[1] = 'H'; m_Header.szID[2] = 'R'; m_Header.szID[3] = 'D';
	m_Header.nSprQt = 0;
	for(int i = 0; i < MAX_OBJ_ANI; i++)
	{
		if(lstrlen(m_Header.szSprFNs[i]) > 0) m_Header.nSprQt++;
	}

	wsprintf(m_Header.szRemark, "캐릭터 데이터 파일 : 스프라이트 %d개", m_Header.nSprQt);

	CFile file;
	file.Open(szFileName, CFile::modeCreate | CFile::modeWrite);
	file.Write(&m_Header, sizeof(_OBJ_HEADER));

	// 대표 이미지 저장..
	const _SPR_LSP* pSpr;
	for(i = 0; i < MAX_OBJ_ANI; i++)
	{
		DELETE_ARRAY_SAFE(m_pSprStands[i]->pwV);
		if(m_pAniObjs[i])
		{
			m_pAniObjs[i]->SprGet(0, &pSpr);
			if(pSpr)
			{
				*m_pSprStands[i] = *pSpr;
				m_pSprStands[i]->pwV = new WORD[m_pSprStands[i]->nDataSize/2];
				memcpy(m_pSprStands[i]->pwV, pSpr->pwV, m_pSprStands[i]->nDataSize);

				if(nPF == 555) m_pAniObjs[i]->Decode2_565(m_pSprStands[i]);
				file.Write(m_pSprStands[i], sizeof(_SPR_LSP));
				if(m_pSprStands[i]->nDataSize > 0) file.Write(m_pSprStands[i]->pwV, m_pSprStands[i]->nDataSize);
				if(nPF == 555) m_pAniObjs[i]->Decode2_555(m_pSprStands[i]);
			}
		}
	}

	file.Close();
}

void CObjTool::SprLoad(int index, const char *szFileName, int nPF)
{
	if(index < 0 || index >= MAX_OBJ_ANI)  return;
	m_pAniObjs[index]->SprLoad(szFileName, nPF);
}

void CObjTool::SprRelease(int index)
{
	if(index < 0 || index >= MAX_OBJ_ANI)  return;
	m_pAniObjs[index]->SprRelease();
}

void CObjTool::LoadHeaderAndStandImage(const char *szFileName, int nPF)
{
	this->New();

	CFile file;
	if(file.Open(szFileName, CFile::modeRead) == FALSE) return;
	file.Read(&m_Header, sizeof(_OBJ_HEADER));

	// 대표 이미지 부르기....
	for(int i = 0; i < MAX_OBJ_ANI; i++)
	{
		DELETE_ARRAY_SAFE(m_pSprStands[i]->pwV);
		file.Read(m_pSprStands[i], sizeof(_SPR_LSP));
		m_pSprStands[i]->pwV = new WORD[m_pSprStands[i]->nDataSize/2];
		if(m_pSprStands[i]->nDataSize > 0)
		{
			file.Read(m_pSprStands[i]->pwV, m_pSprStands[i]->nDataSize);
		}
		if(nPF == 555) m_pAniObjs[i]->Decode2_555(m_pSprStands[i]);
	}

	file.Close();
}

void CObjTool::Release()
{
	CObjBasic::Release();
	for(int i = 0; i < MAX_OBJ_ANI; i++)
	{
		m_pAniObjs[i]->SprRelease();
		m_pAniObjs[i]->AniRelease();
		DELETE_ARRAY_SAFE(m_pSprStands[i]->pwV);
		MAKE_ZERO(*m_pSprStands[i]);
	}
}
