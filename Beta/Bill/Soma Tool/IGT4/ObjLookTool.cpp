// ObjLookTool.cpp: implementation of the CObjLookTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igt4.h"
#include "ObjLookTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjLookTool::CObjLookTool()
{
	this->Alloc(MAX_LOOK);
}

CObjLookTool::~CObjLookTool()
{
}

void CObjLookTool::Save(const char *szFileName)
{
	CFile file;
	if(file.Open(szFileName, CFile::modeWrite | CFile::modeCreate) == FALSE) return;
	
	m_Header.szID[0] = 'L'; m_Header.szID[1] = 'A'; m_Header.szID[2] = 'D'; m_Header.szID[3] = 'F';
	lstrcpy(m_Header.szRemark, "얼굴 표정 조합 데이터 파일");
	
	file.Write(&m_Header, sizeof(_LOOK_HEADER));

	for(int i = 0; i < m_Header.nQt; i++)
	{
		file.Write(m_pDatas[i], sizeof(_LOOK_DATA));
	}

	file.Close();
}
