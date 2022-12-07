// HouseInfo.cpp: implementation of the CHouseInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HouseInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HOUSECELL::HOUSECELL()
{
	m_vMove = -1;
	m_vInner = -1;
}

HOUSECELL::~HOUSECELL()
{
}

HOUSECELL& HOUSECELL::operator =(HOUSECELL& house)
{
	m_vMove = house.m_vMove;
	m_vInner = house.m_vInner;

	return *this;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHouseInfo::CHouseInfo()
{
	m_vSize.cx = m_vSize.cy = 0;
	m_ppData = NULL;
}

CHouseInfo::~CHouseInfo()
{
	DeleteData();
}

void CHouseInfo::DeleteData()
{
	if(m_ppData)
	{
		for(int i = 0; i < m_vSize.cx; i++)
		{
			delete[] m_ppData[i];
		}
		delete[] m_ppData;
		m_ppData = NULL;

		m_vSize.cx = m_vSize.cy = 0;
	}
}

void CHouseInfo::Initialize(int x, int y)
{
	if(x <= 0 || y <= 0) return;

	DeleteData();

	m_vSize.cx = x;
	m_vSize.cy = y;

	m_ppData = new HOUSECELL *[m_vSize.cx];
	for(int i = 0; i < m_vSize.cx; i++)
	{
		m_ppData[i] = new HOUSECELL[m_vSize.cy];
	}
}

void CHouseInfo::Initialize(CSize size)
{
	Initialize(size.cx, size.cy);
}

BOOL CHouseInfo::SetCellInfo(int x, int y, HOUSECELL cell_info)
{
	if(x < 0 || y < 0 || x >= m_vSize.cx || y >= m_vSize.cy) return FALSE;

	m_ppData[x][y] = cell_info;

	return TRUE;
}

void CHouseInfo::SetCellInfo(POINT *p, HOUSECELL cell_info)
{
	int i, j;
	int x, y, x_count, y_count;

	x_count = p[1].x - p[0].x;
	y_count = p[3].y - p[0].y;
	for(i = 0; i <= x_count; i++)
	{
		for(j = 0; j <= y_count; j++)
		{
			x = p[0].x+i+j+1; y = p[0].y-i+j+1;
			SetCellInfo(x, y, cell_info);
		}
	}
}

HOUSECELL *CHouseInfo::GetCellInfo(int x, int y)
{
	if(x < 0 || y < 0 || x >= m_vSize.cx || y >= m_vSize.cy) return NULL;

	return &m_ppData[x][y];
}

HOUSECELL *CHouseInfo::GetCellInfo(CPoint point)
{
	return GetCellInfo(point.x, point.y);
}

BOOL CHouseInfo::SetMoveInfo(int x, int y, char move_info)
{
	if(x < 0 || y < 0 || x >= m_vSize.cx || y >= m_vSize.cy) return FALSE;

	m_ppData[x][y].m_vMove = move_info;

	return TRUE;
}

void CHouseInfo::SetMoveInfo(POINT *p, char move_info)
{
	int i, j;
	int x, y, x_count, y_count;

	x_count = p[1].x - p[0].x;
	y_count = p[3].y - p[0].y;
	for(i = 0; i <= x_count; i++)
	{
		for(j = 0; j <= y_count; j++)
		{
			x = p[0].x+i+j+1; y = p[0].y-i+j+1;
			SetMoveInfo(x, y, move_info);
		}
	}
}

char CHouseInfo::GetMoveInfo(int x, int y)
{
	if(x < 0 || y < 0 || x >= m_vSize.cx || y >= m_vSize.cy) return -1;

	return m_ppData[x][y].m_vMove;
}

char CHouseInfo::GetMoveInfo(CPoint point)
{
	return GetMoveInfo(point.x, point.y);
}

BOOL CHouseInfo::SetInnerInfo(int x, int y, char inner_info)
{
	if(x < 0 || y < 0 || x >= m_vSize.cx || y >= m_vSize.cy) return FALSE;

	m_ppData[x][y].m_vInner = inner_info;

	return TRUE;
}

void CHouseInfo::SetInnerInfo(POINT *p, char inner_info)
{
	int i, j;
	int x, y, x_count, y_count;

	x_count = p[1].x - p[0].x;
	y_count = p[3].y - p[0].y;
	for(i = 0; i <= x_count; i++)
	{
		for(j = 0; j <= y_count; j++)
		{
			x = p[0].x+i+j+1; y = p[0].y-i+j+1;
			SetInnerInfo(x, y, inner_info);
		}
	}
}

char CHouseInfo::GetInnerInfo(int x, int y)
{
	if(x < 0 || y < 0 || x >= m_vSize.cx || y >= m_vSize.cy) return -1;

	return m_ppData[x][y].m_vInner;
}

char CHouseInfo::GetInnerInfo(CPoint point)
{
	return GetInnerInfo(point.x, point.y);
}

void CHouseInfo::Save(CFile& file)
{
	int count = m_vSize.cx * m_vSize.cy * 2;
	char *t_buf;
	int i;

	file.Write(&m_vSize.cx, 4);
	file.Write(&m_vSize.cy, 4);

	if(count == 0) return;

	i = 0;
	t_buf = new char[count];
	for(int x = 0; x < m_vSize.cx; x++)
	{
		for(int y = 0; y < m_vSize.cy; y++)
		{
			t_buf[i++] = m_ppData[x][y].m_vMove;
			t_buf[i++] = m_ppData[x][y].m_vInner;
		}
	}

	file.Write(t_buf, count);

	delete[] t_buf;
}

void CHouseInfo::Load(CFile& file)
{
	DeleteData();

	int count;
	char *t_buf;
	int i;

	file.Read(&m_vSize.cx, 4);
	file.Read(&m_vSize.cy, 4);

	count = m_vSize.cx * m_vSize.cy * 2;

	if(count == 0) return;

	Initialize(m_vSize);

	i = 0;
	t_buf = new char[count];
	file.Read(t_buf, count);
	for(int x = 0; x < m_vSize.cx; x++)
	{
		for(int y = 0; y < m_vSize.cy; y++)
		{
			m_ppData[x][y].m_vMove = t_buf[i++];
			m_ppData[x][y].m_vInner = t_buf[i++];
		}
	}

	delete[] t_buf;
}