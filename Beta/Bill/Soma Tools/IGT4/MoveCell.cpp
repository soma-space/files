// MoveCell.cpp: implementation of the MoveCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MoveCell.h"
#include "GlobalVar.h"

BYTE g_vBitMask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

struct T_STRUCT {
	short x;
	short y;
	short data;
};

#define MOVE_VER		400		// 100:(64,32) 200:(96,48) 300:Move Cell Size Fix 400:Added Area Cell

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MOVEINFO::MOVEINFO()
{
	m_bMovable = TRUE;
	m_nEventNumber = -1;
	m_nHouseNumber = -1;
	m_nAreaNumber = 0;
}

MoveCell::MoveCell()
{
	m_vDim.cx = 0;
	m_vDim.cy = 0;
	m_pMoveInfo = NULL;
	m_vMoveInfoVersion = MOVE_VER;
}

MoveCell::~MoveCell()
{
	RemoveData();
}

void MoveCell::RemoveData()
{
	if(m_pMoveInfo)
	{
		for(int i = 0; i < m_vDim.cx; i++) {
			delete[] m_pMoveInfo[i];
		}
		delete[] m_pMoveInfo;
		m_pMoveInfo = NULL;
	}
}

void MoveCell::Initialize(int cx, int cy)
{
	RemoveData();

	m_vDim.cx = cx;
	m_vDim.cy = cy;

	m_pMoveInfo = new MOVEINFO *[m_vDim.cx];
	for(int i = 0; i < m_vDim.cx; i++) {
		m_pMoveInfo[i] = new MOVEINFO[m_vDim.cy];
	}
}

BOOL MoveCell::GetMovable(int x, int y)
{
	if(x < 0 || x >= m_vDim.cx || y < 0 || y >= m_vDim.cy)
	{
		return FALSE;
	}

	return m_pMoveInfo[x][y].m_bMovable;
}

void MoveCell::SetMovable(int x, int y, BOOL movable)
{
	if(x < 0 || x >= m_vDim.cx || y < 0 || y >= m_vDim.cy)
	{
		return;
	}

	m_pMoveInfo[x][y].m_bMovable = movable;
}

void MoveCell::SetMovable(POINT *p)
{
	int i, j;
	int x, y, x_count, y_count;

	x_count = p[1].x - p[0].x;
	y_count = p[3].y - p[0].y;
	for(i = 0; i <= x_count; i++)
	{
		for(j = 0; j <= y_count; j++)
		{
			x = p[0].x+i+j; y = p[0].y-i+j;
			SetMovable(x, y, !GetMovable(x, y));
		}
	}
}

short MoveCell::GetEvent(int x, int y)
{
	if(x < 0 || x >= m_vDim.cx || y < 0 || y >= m_vDim.cy)
	{
		return -1;
	}

	return m_pMoveInfo[x][y].m_nEventNumber;
}

void MoveCell::SetEvent(int x, int y, short event)
{
	if(x < 0 || x >= m_vDim.cx || y < 0 || y >= m_vDim.cy)
	{
		return;
	}

	m_pMoveInfo[x][y].m_nEventNumber = event;
}

void MoveCell::SetEvent(POINT *p, short event)
{
	int i, j;
	int x, y, x_count, y_count;

	x_count = p[1].x - p[0].x;
	y_count = p[3].y - p[0].y;
	for(i = 0; i <= x_count; i++)
	{
		for(j = 0; j <= y_count; j++)
		{
			x = p[0].x+i+j; y = p[0].y-i+j;
			SetEvent(x, y, event);
		}
	}
}

short MoveCell::GetHouse(int x, int y)
{
	if(x < 0 || x >= m_vDim.cx || y < 0 || y >= m_vDim.cy)
	{
		return -1;
	}

	return m_pMoveInfo[x][y].m_nHouseNumber;
}

void MoveCell::SetHouse(int x, int y, short house)
{
	if(x < 0 || x >= m_vDim.cx || y < 0 || y >= m_vDim.cy)
	{
		return;
	}

	m_pMoveInfo[x][y].m_nHouseNumber = house;
}

void MoveCell::SetHouse(POINT *p, short house)
{
	int i, j;
	int x, y, x_count, y_count;

	x_count = p[1].x - p[0].x;
	y_count = p[3].y - p[0].y;
	for(i = 0; i <= x_count; i++)
	{
		for(j = 0; j <= y_count; j++)
		{
			x = p[0].x+i+j; y = p[0].y-i+j;
			SetHouse(x, y, house);
		}
	}
}

BYTE MoveCell::GetArea(int x, int y)
{
	if(x < 0 || x >= m_vDim.cx || y < 0 || y >= m_vDim.cy)
	{
		return -1;
	}

	return m_pMoveInfo[x][y].m_nAreaNumber;
}

void MoveCell::SetArea(int x, int y, BYTE area)
{
	if(x < 0 || x >= m_vDim.cx || y < 0 || y >= m_vDim.cy)
	{
		return;
	}

	m_pMoveInfo[x][y].m_nAreaNumber = area;
}

void MoveCell::SetArea(POINT *p, BYTE area)
{
	int i, j;
	int x, y, x_count, y_count;

	x_count = p[1].x - p[0].x;
	y_count = p[3].y - p[0].y;
	for(i = 0; i <= x_count; i++)
	{
		for(j = 0; j <= y_count; j++)
		{
			x = p[0].x+i+j; y = p[0].y-i+j;
			SetArea(x, y, area);
		}
	}
}

void MoveCell::Save(CArchive &ar)
{
	BYTE *t_buf;
	CPoint cell_limit;
	int load_count, read_count, tmp_int, roll;
	int x, y, i;

	ar << m_vDim.cx << m_vDim.cy;

	cell_limit.x = (m_vDim.cx+1)/2;
	cell_limit.y = (m_vDim.cy+1)/2;
	tmp_int = cell_limit.x*cell_limit.y+(cell_limit.x-1)*(cell_limit.y-1);
	load_count = tmp_int / 8;
	if(tmp_int % 8) load_count++;
	t_buf = new BYTE[load_count];
	// 이동정보를 저장한다.
	ZeroMemory(t_buf, load_count);
	roll = 0;
	read_count = 0;
	for(y = 0; y < m_vDim.cy-1; y++)
	{
		for(x = 0; x < m_vDim.cx-1; x++)
		{
			if((x+y)%2==0)
			{
				if(m_pMoveInfo[x][y].m_bMovable) t_buf[read_count] |= g_vBitMask[roll];
				roll++;
				if(roll == 8)
				{
					roll = 0;
					read_count++;
				}
			}
		}
	}
	ar.Write(t_buf, load_count);
	delete[] t_buf;
	// 이벤트정보를 저장한다.
	T_STRUCT t_struct;
	CArray<T_STRUCT, T_STRUCT&> t_dataarray;
	for(y = 0; y < m_vDim.cy; y++)
	{
		for(x = 0; x < m_vDim.cx; x++)
		{
			if((x+y)%2==0)
			{
				if(m_pMoveInfo[x][y].m_nEventNumber > -1)
				{
					t_struct.x = x;
					t_struct.y = y;
					t_struct.data = m_pMoveInfo[x][y].m_nEventNumber;
					t_dataarray.Add(t_struct);
				}
			}
		}
	}
	load_count = t_dataarray.GetSize();
	ar << load_count;
	for(i = 0; i < load_count; i++)
	{
		ar << t_dataarray[i].x;
		ar << t_dataarray[i].y;
		ar << t_dataarray[i].data;
	}
	t_dataarray.RemoveAll();

	// 건물정보를 저장한다.
	for(y = 0; y < m_vDim.cy; y++)
	{
		for(x = 0; x < m_vDim.cx; x++)
		{
			if((x+y)%2==0)
			{
				if(m_pMoveInfo[x][y].m_nHouseNumber > -1)
				{
					t_struct.x = x;
					t_struct.y = y;
					t_struct.data = m_pMoveInfo[x][y].m_nHouseNumber;
					t_dataarray.Add(t_struct);
				}
			}
		}
	}
	load_count = t_dataarray.GetSize();
	ar << load_count;
	for(i = 0; i < load_count; i++)
	{
		ar << t_dataarray[i].x;
		ar << t_dataarray[i].y;
		ar << t_dataarray[i].data;
	}
	t_dataarray.RemoveAll();

	// 영역정보를 저장한다.
	cell_limit.x = (m_vDim.cx+1)/2;
	cell_limit.y = (m_vDim.cy+1)/2;
	load_count = cell_limit.x*cell_limit.y+(cell_limit.x-1)*(cell_limit.y-1);
	t_buf = new BYTE[load_count];
	ZeroMemory(t_buf, load_count);
	read_count = 0;
	for(y = 0; y < m_vDim.cy-1; y++)
	{
		for(x = 0; x < m_vDim.cx-1; x++)
		{
			if((x+y)%2==0)
			{
				t_buf[read_count++] = m_pMoveInfo[x][y].m_nAreaNumber;
			}
		}
	}
	ar.Write(t_buf, load_count);
	delete[] t_buf;
}

void MoveCell::Load(CArchive& ar)
{
	int cx, cy, roll;
	BYTE *t_buf;
	CPoint cell_limit;
	int load_count, read_count, tmp_int;
	int x, y, i;

	ar >> cx >> cy;

	if(m_vMoveInfoVersion == 100)
	{
		int t_x, t_y;

		t_x = cx * 32;
		t_y = cy * 16;
		Initialize(t_x/96*2 + ((t_x%96>=48)?1:0), t_y/48*2 + ((t_y%48>=24)?1:0));
		return;
	}
	else if(m_vMoveInfoVersion == 200)
	{
		int t_x, t_y;
		t_x = g_pTMap->m_HTMap.nWidth * 32;
		t_y = g_pTMap->m_HTMap.nHeight * 32;
		Initialize(t_x/96*2 + ((t_x%96>=48)?1:0), t_y/48*2 + ((t_y%48>=24)?1:0));

		cell_limit.x = (cx+1)/2;
		cell_limit.y = (cy+1)/2;
		tmp_int = cell_limit.x*cell_limit.y+(cell_limit.x-1)*(cell_limit.y-1);
		load_count = tmp_int / 8;
		if(tmp_int % 8) load_count++;
		t_buf = new BYTE[load_count];
		// 이동정보를 읽어온다.
		roll = 0;
		read_count = 0;
		ar.Read(t_buf, load_count);
		for(y = 0; y < cy-1; y++)
		{
			if(y < m_vDim.cy)
			{
				for(x = 0; x < cx-1; x++)
				{
					if((x+y)%2==0)
					{
						if(x < m_vDim.cx)
						{
							m_pMoveInfo[x][y].m_bMovable = (t_buf[read_count] & g_vBitMask[roll]) >> roll;
							roll++;
							if(roll == 8)
							{
								roll = 0;
								read_count++;
							}
						}
						else
						{
							roll++;
							if(roll == 8)
							{
								roll = 0;
								read_count++;
							}
						}
					}
				}
			}
			else
			{
				for(x = 0; x < cx-1; x++)
				{
					if((x+y)%2==0)
					{
						roll++;
						if(roll == 8)
						{
							roll = 0;
							read_count++;
						}
					}
				}
			}
		}
		delete[] t_buf;
	}
	else
	{
		Initialize(cx, cy);

		cell_limit.x = (cx+1)/2;
		cell_limit.y = (cy+1)/2;
		tmp_int = cell_limit.x*cell_limit.y+(cell_limit.x-1)*(cell_limit.y-1);
		load_count = tmp_int / 8;
		if(tmp_int % 8) load_count++;
		t_buf = new BYTE[load_count];
		// 이동정보를 읽어온다.
		roll = 0;
		read_count = 0;
		ar.Read(t_buf, load_count);
		for(y = 0; y < m_vDim.cy-1; y++)
		{
			for(x = 0; x < m_vDim.cx-1; x++)
			{
				if((x+y)%2==0)
				{
					m_pMoveInfo[x][y].m_bMovable = (t_buf[read_count] & g_vBitMask[roll]) >> roll;
					roll++;
					if(roll == 8)
					{
						roll = 0;
						read_count++;
					}
				}
			}
		}
		delete[] t_buf;
	}

	// 이벤트정보를 읽어온다.
	short data_x, data_y, data;
	ar >> load_count;
	for(i = 0; i < load_count; i++)
	{
		ar >> data_x;
		ar >> data_y;
		ar >> data;
		m_pMoveInfo[data_x][data_y].m_nEventNumber = data;
	}

	// 건물정보를 읽어온다.
	ar >> load_count;
	for(i = 0; i < load_count; i++)
	{
		ar >> data_x;
		ar >> data_y;
		ar >> data;
		m_pMoveInfo[data_x][data_y].m_nHouseNumber = data;
	}

	// 영역정보를 읽어온다.
	if(m_vMoveInfoVersion >= 400)
	{
		cell_limit.x = (cx+1)/2;
		cell_limit.y = (cy+1)/2;
		load_count = cell_limit.x*cell_limit.y+(cell_limit.x-1)*(cell_limit.y-1);
		t_buf = new BYTE[load_count];
		read_count = 0;
		ar.Read(t_buf, load_count);
		for(y = 0; y < m_vDim.cy-1; y++)
		{
			for(x = 0; x < m_vDim.cx-1; x++)
			{
				if((x+y)%2==0)
				{
					m_pMoveInfo[x][y].m_nAreaNumber = t_buf[read_count++];
				}
			}
		}
		delete[] t_buf;
	}
	m_vMoveInfoVersion = MOVE_VER;
}

void MoveCell::Save(LPCTSTR file_name)
{
	CFile newfile;

	if(newfile.Open(file_name, CFile::modeCreate|CFile::modeWrite) == 0)
	{
		AfxMessageBox("이동 정보를 저장하는 데 실패하였습니다.", MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		CArchive newar(&newfile, CArchive::store);
		newar << m_vMoveInfoVersion;
		Save(newar);
		newar.Close();
		newfile.Close();
	}
}

BOOL MoveCell::Load(LPCTSTR file_name)
{
	CFile newfile;

	if(newfile.Open(file_name, CFile::modeRead))
	{
		CArchive newar(&newfile, CArchive::load);
		newar >> m_vMoveInfoVersion;
		Load(newar);
		newar.Close();
		newfile.Close();
		return TRUE;
	}
	else return FALSE;
}