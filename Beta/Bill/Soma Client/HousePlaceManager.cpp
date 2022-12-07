// HousePlaceManager.cpp: implementation of the CHousePlaceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "HousePlaceManager.h"
#include "BuildHouseManager.h"
#include "PacketParser.h"
#include "ChildView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CBuildHouseManager	BuildHouseManager;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHousePlaceManager::CHousePlaceManager()
{
	m_bEnableHousePlace = FALSE;
	m_pPlaceRect = NULL;
	m_vTileSet = 0;
	m_vSelPlace = 0;
}

CHousePlaceManager::~CHousePlaceManager()
{
	DeleteHousePlace();
}

void CHousePlaceManager::DeleteHousePlace()
{
	if(m_pPlaceRect)
	{
		delete m_pPlaceIndex;
		delete m_pPlaceRect;
		m_pPlaceIndex = NULL;
		m_pPlaceRect = NULL;
	}
	m_bEnableHousePlace = FALSE;
}

void CHousePlaceManager::SetHousePlace(PBYTE bypData)
{
	short *sValue = (short *)bypData;
	int count, i, j;
	CRect t_rect;

	j = 0;
	m_vTileSet = sValue[j++];
	count = sValue[j++];
	if(count == 0) return;

	DeleteHousePlace();
	m_vPlaceCount = count;
	m_pPlaceIndex = new int[count];
	m_pPlaceRect = new CRect[count];
	for(i = 0; i < count; i++)
	{
		m_pPlaceIndex[i] = sValue[j++];
		m_pPlaceRect[i].left = sValue[j++];
		m_pPlaceRect[i].top = sValue[j++];
		m_pPlaceRect[i].right = sValue[j++];
		m_pPlaceRect[i].bottom = sValue[j++];
	}
	m_bEnableHousePlace = TRUE;
}

BOOL CHousePlaceManager::PtInPlace(CPoint pos)
{
	int i;

	for(i = 0; i < m_vPlaceCount; i++)
	{
		if(m_pPlaceRect[i].PtInRect(pos))
		{
			m_vSelPlace = i;
			SendHousePlace();
			return TRUE;
		}
	}

	return FALSE;
}

void CHousePlaceManager::SendHousePlace()
{
	BYTE buf[10];
	int offset = 0;
	CPoint s_pos;
	CPacketParser::SetByte(buf, offset, PKT_SELHOUSEPLACE);
	
	CPacketParser::SetShort(buf, offset, m_pPlaceIndex[m_vSelPlace]);
	CPacketParser::SetShort(buf, offset, m_vTileSet);
	s_pos = BuildHouseManager.GetInnerTopLeft(m_vTileSet, m_pPlaceRect[m_vSelPlace]);
	CPacketParser::SetShort(buf, offset, (short)s_pos.x);
	CPacketParser::SetShort(buf, offset, (short)s_pos.y);
	CChildView::SocketMng.Send(offset, buf);
}

void CHousePlaceManager::RcvBuildHouse(PBYTE bypData)
{
	int offset = 0;
	BYTE success = CPacketParser::GetByte(bypData, offset);

	if(success == 1) // ¼º°ø
	{
		BuildHouseManager.BuildHouse(m_pPlaceRect[m_vSelPlace], m_vTileSet);
	}
	DeleteHousePlace();
}