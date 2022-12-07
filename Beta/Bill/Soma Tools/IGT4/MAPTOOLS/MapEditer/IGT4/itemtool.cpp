// ItemTool.cpp: implementation of the CItemTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igt4.h"
#include "ItemTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemTool::CItemTool()
{

}

CItemTool::~CItemTool()
{

}

void CItemTool::Save(const char *szFileName)
{
	CFile file;
	if(file.Open(szFileName, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		OutputDebugString("파일 열기 실패 : ");
		OutputDebugString(szFileName);
		OutputDebugString("\n");
		return;
	}

	file.Write(&m_Header, sizeof(_ITEM_HEADER)); // 인터페이스 헤더 읽기.

	int i;
	for(i = 0; i < m_Header.nQt; i++)
	{
		file.Write(m_pDatas[i], sizeof(_ITEM_DATA)); // 인터페이스 데이터 읽기.
		if(m_pDatas[i]->snRemark > 0)
		{
			file.Write(m_pDatas[i]->szRemark, m_pDatas[i]->snRemark);
		}
		m_pDatas[i]->szRemarkNPC = NULL;
		if(m_pDatas[i]->snRemarkNPC > 0)
		{
			file.Write(m_pDatas[i]->szRemarkNPC, m_pDatas[i]->snRemarkNPC);
		}
	}
	file.Close();
}

void CItemTool::AniImport(const char *szFileName)
{
	lstrcpy(m_Header.szAniFN, szFileName);
	m_pAniObj->AniLoad(szFileName);
}

void CItemTool::SprImport(const char *szFileName, int nPF)
{
	lstrcpy(m_Header.szSprFN, szFileName);
	m_pAniObj->SprLoad(szFileName, nPF);
}
