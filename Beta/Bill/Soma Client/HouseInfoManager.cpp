// HouseInfoManager.cpp: implementation of the CHouseInfoManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HouseInfoManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void _HOUSE_INFO_HEADER::Save(CFile &file)
{
	file.Write(szID, 4);
	file.Write(szRemark, 64);
	file.Write(&nQt, 4);
}

BOOL _HOUSE_INFO_HEADER::Load(CFile &file)
{
	char t_buf[4];
	char t_remark[64];
	int t_qt;

	file.Read(t_buf, 4);
	file.Read(t_remark, 64);
	file.Read(&t_qt, 4);

	if(lstrcmp(t_buf, "HIF")) return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHouseInfoManager::CHouseInfoManager()
{
	lstrcpy(m_HouseInfoHeader.szID, "HIF");
	ZeroMemory(m_HouseInfoHeader.szRemark, 64);
	m_HouseInfoHeader.nQt = 0;
}

CHouseInfoManager::~CHouseInfoManager()
{
	DeleteAllHouse();
}

void CHouseInfoManager::SetHouse(int number, int cx, int cy)
{
	CHouseInfo *pHouseInfo;

	if(m_MapHouseInfo.Lookup(number, pHouseInfo) == FALSE)
	{
		pHouseInfo = new CHouseInfo;
		m_MapHouseInfo[number] = pHouseInfo;
	}

	pHouseInfo->Initialize(cx, cy);
}

void CHouseInfoManager::SetHouse(int number, CSize size)
{
	CHouseInfo *pHouseInfo;

	if(m_MapHouseInfo.Lookup(number, pHouseInfo) == FALSE)
	{
		pHouseInfo = new CHouseInfo;
		m_MapHouseInfo[number] = pHouseInfo;
	}

	pHouseInfo->Initialize(size);
}

void CHouseInfoManager::DeleteHouse(int number)
{
	CHouseInfo *pHouseInfo;

	if(m_MapHouseInfo.Lookup(number, pHouseInfo))
	{
		m_MapHouseInfo.RemoveKey(number);
		delete pHouseInfo;
	}
}

void CHouseInfoManager::DeleteAllHouse()
{
	POSITION pos;
	int index;
	CHouseInfo *pHouseInfo;

	pos = m_MapHouseInfo.GetStartPosition();
	while (pos != NULL)
	{
		m_MapHouseInfo.GetNextAssoc(pos, index, pHouseInfo);
		delete pHouseInfo;
	}
	m_MapHouseInfo.RemoveAll();
}

void CHouseInfoManager::Save(CString file_name)
{
	int total_count;
	CFile file;
	POSITION pos;
	int index;
	CHouseInfo *pHouseInfo;

	file.Open(file_name, CFile::modeWrite | CFile::modeCreate);

	m_HouseInfoHeader.Save(file);

	total_count = m_MapHouseInfo.GetCount();

	file.Write(&total_count, sizeof(int));

	pos = m_MapHouseInfo.GetStartPosition();
	while(pos!=NULL)
	{
		m_MapHouseInfo.GetNextAssoc(pos, index, pHouseInfo);
		file.Write(&index, sizeof(int));
		pHouseInfo->Save(file);
	}

	file.Close();
}

void CHouseInfoManager::Load(CString file_name)
{
	DeleteAllHouse();

	int i, total_count, index;
	CFile file;
	int file_size;
	CHouseInfo *pHouseInfo;

	if(!file.Open(file_name, CFile::modeRead)) return;

	file_size = file.GetLength();
	if(file_size < 4+64+4+4 || m_HouseInfoHeader.Load(file) == FALSE)
	{
		file.Close();
		AfxMessageBox("HIF 포맷과 맞지 않습니다.", MB_OK|MB_ICONSTOP);
		return;
	}

	file.Read(&total_count, sizeof(int));

	for(i = 0; i < total_count; i++)
	{
		file.Read(&index, sizeof(int));
		pHouseInfo = new CHouseInfo;
		pHouseInfo->Load(file);
		m_MapHouseInfo[index] = pHouseInfo;
	}

	file.Close();
}