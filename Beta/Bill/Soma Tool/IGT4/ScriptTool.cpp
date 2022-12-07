// ScriptTool.cpp: implementation of the CScriptTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScriptTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptTool::CScriptTool()
{
}

CScriptTool::~CScriptTool()
{
}

void CScriptTool::Set(int index, const _SCT_DATA* pSct)
{
	if(pSct == NULL) return;
	if(index < 0 || index >= MAX_SCT) return;

	// 할당되어 있으면 해제한다...
	if(m_pDatas[index])
	{
		if(m_pDatas[index]->szText) delete [] m_pDatas[index]->szText; m_pDatas[index]->szText = NULL;
		delete m_pDatas[index]; m_pDatas[index] = NULL;
	}
	
	// 새로 할당
	m_pDatas[index] = new _SCT_DATA;
	MAKE_ZERO(*m_pDatas[index]);
	
	*m_pDatas[index] = *pSct;
	m_pDatas[index]->szText = NULL;

	if(pSct->szText != NULL)
	{
		m_pDatas[index]->szText = new char[lstrlen(pSct->szText) + 1];
		lstrcpy(m_pDatas[index]->szText, pSct->szText);
	}
}

void CScriptTool::Save(const char* szFileName)
{
	// 전체 몇개인지 확인
	for(int i = 0; i < MAX_SCT; i++)
	{
		if(m_pDatas[i] != NULL)
		{
			m_Header.nQt++;
			if(m_pDatas[i]->nCommand == 0)
			{
				break;
			}
		}
	}

	if(m_Header.nQt <= 0) return;
	
	// 헤더 쓰기
	m_Header.fVersion = 1.0f;
	lstrcpy(m_Header.szID, "소마신화전기 Script 파일");
	CFile file; if(file.Open(szFileName, CFile::modeWrite | CFile::modeCreate) == FALSE) { MessageBox(NULL, szFileName, "파일 열기 실패", MB_OK); return; }
	file.Write(&m_Header, sizeof(_SCT_HEADER));

	// 데이터 쓰기
	for(i = 0; i < m_Header.nQt; i++)
	{
		m_pDatas[i]->nText = lstrlen(m_pDatas[i]->szText);
		file.Write(m_pDatas[i], sizeof(_SCT_DATA));
		if(m_pDatas[i]->nText > 0)
		{
			file.Write(m_pDatas[i]->szText, m_pDatas[i]->nText); // 텍스트..
		}
	}
	file.Close();
}
