// LightSourceTool.cpp: implementation of the CLightSourceTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightSourceTool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightSourceTool::CLightSourceTool()
{
}

CLightSourceTool::~CLightSourceTool()
{
}

void CLightSourceTool::Save(const char *szFileName)
{
	CFile file;
	if(file.Open(szFileName, CFile::modeCreate | CFile::modeWrite, NULL) == FALSE) return;

	file.Write(&m_Header, sizeof(_LIGHT_SOURCE_HEADER));

	file.Write(m_pDatas, sizeof(_LIGHT_SOURCE_DATA)*m_Header.nQt);

	file.Close();
}
