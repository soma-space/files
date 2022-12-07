// MouseSelectManager.cpp: implementation of the MouseSelectManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Igt4.h"
#include "MouseSelectManager.h"
#include "WndTileMap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

POINT PosToCell(POINT p);
POINT CellToPos(POINT p);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MouseSelectManager::MouseSelectManager()
{
	m_bpView = NULL;
	m_bUsing = FALSE;
}


MouseSelectManager::~MouseSelectManager()
{

}

POINT *MouseSelectManager::GetSelectRgn()
{
	return m_vRealPos;
}

void MouseSelectManager::SetStartPos(CPoint pos)
{
	ASSERT(m_bpView);
	POINT t_p;

	m_bUsing = TRUE;
	t_p = PosToCell(pos);
	m_vPos[0] = t_p;
	t_p = CellToPos(t_p);
	t_p.x -= m_bpView->GetScrollPos(SB_HORZ)*TILE_SIZE;
	t_p.y -= m_bpView->GetScrollPos(SB_VERT)*TILE_SIZE;

	m_vCurPos[0].x = t_p.x - 48;	m_vCurPos[0].y = t_p.y;
	m_vCurPos[1].x = t_p.x;			m_vCurPos[1].y = t_p.y - 24;
	m_vCurPos[2].x = t_p.x + 48;	m_vCurPos[2].y = t_p.y;
	m_vCurPos[3].x = t_p.x;			m_vCurPos[3].y = t_p.y + 24;
	m_vCurPos[4].x = m_vCurPos[0].x;	m_vCurPos[4].y = m_vCurPos[0].y;

	m_bpView->Invalidate(FALSE);
}

void MouseSelectManager::SetEndPos(CPoint pos)
{
	POINT t_p;
	SIZE offset_size;
	int min, max;

	m_bUsing = FALSE;
	t_p = PosToCell(pos);
	offset_size.cx = -m_bpView->GetScrollPos(SB_HORZ)*TILE_SIZE;
	offset_size.cy = -m_bpView->GetScrollPos(SB_VERT)*TILE_SIZE;
	if(t_p.x == m_vPos[0].x && t_p.y == m_vPos[0].y)
	{
		m_vRealPos[0] = m_vRealPos[1] = m_vRealPos[2] = m_vRealPos[3] = t_p;
		t_p = CellToPos(t_p);
		t_p.x += offset_size.cx; t_p.y += offset_size.cy;

		m_vCurPos[0].x = t_p.x - 48;	m_vCurPos[0].y = t_p.y;
		m_vCurPos[1].x = t_p.x;			m_vCurPos[1].y = t_p.y - 24;
		m_vCurPos[2].x = t_p.x + 48;	m_vCurPos[2].y = t_p.y;
		m_vCurPos[3].x = t_p.x;			m_vCurPos[3].y = t_p.y + 24;
		m_vCurPos[4].x = m_vCurPos[0].x;	m_vCurPos[4].y = m_vCurPos[0].y;

		m_bpView->Invalidate(FALSE);
		return;
	}
	m_vPos[3] =  t_p;

	m_vPos[1].x = (m_vPos[0].x-m_vPos[0].y+m_vPos[3].x+m_vPos[3].y)/2;
	m_vPos[1].y = m_vPos[1].x-m_vPos[0].x+m_vPos[0].y;
	m_vPos[2].x = (m_vPos[0].x+m_vPos[0].y+m_vPos[3].x-m_vPos[3].y)/2;
	m_vPos[2].y = m_vPos[2].x-m_vPos[3].x+m_vPos[3].y;

	if(m_vPos[0].x < m_vPos[1].x)
	{
		min = 0; max = 1;
	}
	else
	{
		min = 1; max = 0;
	}
	if(m_vPos[2].x < m_vPos[min].x)	min = 2;
	else if(m_vPos[2].x > m_vPos[max].x) max = 2;
	if(m_vPos[3].x < m_vPos[min].x) min = 3;
	else if(m_vPos[3].x > m_vPos[max].x) max = 3;
	m_vRealPos[0] = m_vPos[min];
	m_vRealPos[2] = m_vPos[max];

	if(m_vPos[0].y < m_vPos[1].y)
	{
		min = 0; max = 1;
	}
	else
	{
		min = 1; max = 0;
	}
	if(m_vPos[2].y < m_vPos[min].y) min = 2;
	else if(m_vPos[2].y > m_vPos[max].y) max = 2;
	if(m_vPos[3].y < m_vPos[min].y) min = 3;
	else if(m_vPos[3].y > m_vPos[max].y) max = 3;
	m_vRealPos[1] = m_vPos[min];
	m_vRealPos[3] = m_vPos[max];

	m_vCurPos[0] = CellToPos(m_vRealPos[0]);
	m_vCurPos[0].x += offset_size.cx; m_vCurPos[0].y += offset_size.cy;
	m_vCurPos[1] = CellToPos(m_vRealPos[1]);
	m_vCurPos[1].x += offset_size.cx; m_vCurPos[1].y += offset_size.cy;
	m_vCurPos[2] = CellToPos(m_vRealPos[2]);
	m_vCurPos[2].x += offset_size.cx; m_vCurPos[2].y += offset_size.cy;
	m_vCurPos[3] = CellToPos(m_vRealPos[3]);
	m_vCurPos[3].x += offset_size.cx; m_vCurPos[3].y += offset_size.cy;

	m_vCurPos[0].x -= 48;
	m_vCurPos[1].y -= 24;
	m_vCurPos[2].x += 48;
	m_vCurPos[3].y += 24;
	m_vCurPos[4] = m_vCurPos[0];

	m_bpView->Invalidate(FALSE);
}

void MouseSelectManager::MouseMoving(CPoint pos)
{
	ASSERT(m_bpView);
	CPoint t_p;
	CSize offset_size;
	int min, max;

	t_p = PosToCell(pos);
	offset_size.cx = -m_bpView->GetScrollPos(SB_HORZ)*TILE_SIZE;
	offset_size.cy = -m_bpView->GetScrollPos(SB_VERT)*TILE_SIZE;

	if(t_p == m_vPos[0])
	{
		t_p = CellToPos(t_p);
		t_p.x += offset_size.cx; t_p.y += offset_size.cy;

		m_vCurPos[0].x = t_p.x - 48;	m_vCurPos[0].y = t_p.y;
		m_vCurPos[1].x = t_p.x;			m_vCurPos[1].y = t_p.y - 24;
		m_vCurPos[2].x = t_p.x + 48;	m_vCurPos[2].y = t_p.y;
		m_vCurPos[3].x = t_p.x;			m_vCurPos[3].y = t_p.y + 24;
		m_vCurPos[4].x = m_vCurPos[0].x;	m_vCurPos[4].y = m_vCurPos[0].y;

		m_bpView->Invalidate(FALSE);
		return;
	}

	m_vPos[3] =  t_p;

	m_vPos[1].x = (m_vPos[0].x-m_vPos[0].y+m_vPos[3].x+m_vPos[3].y)/2;
	m_vPos[1].y = m_vPos[1].x-m_vPos[0].x+m_vPos[0].y;
	m_vPos[2].x = (m_vPos[0].x+m_vPos[0].y+m_vPos[3].x-m_vPos[3].y)/2;
	m_vPos[2].y = m_vPos[2].x-m_vPos[3].x+m_vPos[3].y;

	if(m_vPos[0].x < m_vPos[1].x)
	{
		min = 0; max = 1;
	}
	else
	{
		min = 1; max = 0;
	}
	if(m_vPos[2].x < m_vPos[min].x)	min = 2;
	else if(m_vPos[2].x > m_vPos[max].x) max = 2;
	if(m_vPos[3].x < m_vPos[min].x) min = 3;
	else if(m_vPos[3].x > m_vPos[max].x) max = 3;
	m_vCurPos[0] = m_vPos[min];
	m_vCurPos[2] = m_vPos[max];

	if(m_vPos[0].y < m_vPos[1].y)
	{
		min = 0; max = 1;
	}
	else
	{
		min = 1; max = 0;
	}
	if(m_vPos[2].y < m_vPos[min].y) min = 2;
	else if(m_vPos[2].y > m_vPos[max].y) max = 2;
	if(m_vPos[3].y < m_vPos[min].y) min = 3;
	else if(m_vPos[3].y > m_vPos[max].y) max = 3;
	m_vCurPos[1] = m_vPos[min];
	m_vCurPos[3] = m_vPos[max];

	m_vCurPos[0] = CellToPos(m_vCurPos[0]);
	m_vCurPos[0].x += offset_size.cx; m_vCurPos[0].y += offset_size.cy;
	m_vCurPos[1] = CellToPos(m_vCurPos[1]);
	m_vCurPos[1].x += offset_size.cx; m_vCurPos[1].y += offset_size.cy;
	m_vCurPos[2] = CellToPos(m_vCurPos[2]);
	m_vCurPos[2].x += offset_size.cx; m_vCurPos[2].y += offset_size.cy;
	m_vCurPos[3] = CellToPos(m_vCurPos[3]);
	m_vCurPos[3].x += offset_size.cx; m_vCurPos[3].y += offset_size.cy;

	m_vCurPos[0].x -= 48;
	m_vCurPos[1].y -= 24;
	m_vCurPos[2].x += 48;
	m_vCurPos[3].y += 24;
	m_vCurPos[4] = m_vCurPos[0];

	m_bpView->Invalidate(FALSE);
}