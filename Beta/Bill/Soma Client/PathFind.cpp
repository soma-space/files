// PathFind.cpp: implementation of the CPathFind class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "PathFind.h"
#include "math.h"

#include "MainFrm.h"
#include "Packetparser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define LEVEL_ONE_FIND_CROSS			5
//#define LEVEL_ONE_FIND_DIAGONAL			7
#define LEVEL_ONE_FIND_CROSS			5
#define LEVEL_ONE_FIND_DIAGONAL			7

#define LEVEL_TWO_FIND_CROSS			11
#define LEVEL_TWO_FIND_DIAGONAL			10

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPathFind::CPathFind()
{
	m_pStack = (STACK *)calloc(1, sizeof(STACK));
	m_pOpen = NULL;
	m_pClosed = NULL;
	m_pMap = NULL;
	//m_pEventMap = NULL;
	m_pHouseMap = NULL;


#ifdef _DEBUG
	m_pDC = NULL;
#endif
}

CPathFind::~CPathFind()
{
	int i;

	if(m_pMap)
	{
		for(i = 0; i < m_vMapSize.cx; i++)
			SAFE_DELETES(m_pMap[i]);

		SAFE_DELETES(m_pMap);
	}

	if(m_pHouseMap)
	{
		for(i = 0; i < m_vMapSize.cx; i++)
			SAFE_DELETES(m_pHouseMap[i]);
		SAFE_DELETES(m_pHouseMap);
	}
	ClearData();
	free(m_pStack);
}

void CPathFind::ClearData()
{
	NODE *t_node1, *t_node2;

	if(m_pOpen)
	{
		t_node1 = m_pOpen->NextNode;
		while(t_node1)
		{
			t_node2 = t_node1->NextNode;
			free(t_node1);
			t_node1 = t_node2;
		}
		free(m_pOpen);
		m_pOpen = NULL;
	}

	if(m_pClosed)
	{
		t_node1 = m_pClosed->NextNode;
		while(t_node1)
		{
			t_node2 = t_node1->NextNode;
			free(t_node1);
			t_node1 = t_node2;
		}
		free(m_pClosed);
		m_pClosed = NULL;
	}
}
/*
void CPathFind::SetMap(int cx, int cy, BYTE **map)
// x = m_vDim.cx, y = m_vDim.cy
{
	int i;//, j;

	if(m_pMap)
	{
		for(i = 0; i < m_vMapSize.cx; i++)
		{
			SAFE_DELETES( m_pMap[i] );
		}
		SAFE_DELETES( m_pMap );
	}
	ClearData();

	m_vMapSize.cx = x;
	m_vMapSize.cy = y;

	m_pMap = new BYTE *[m_vMapSize.cx];
	for(i = 0; i < m_vMapSize.cx; i++)
	{
		m_pMap[i] = new BYTE[m_vMapSize.cy];
	}

	for(i = 0; i < m_vMapSize.cx; i++)
	{
		CopyMemory(m_pMap[i], map[i], m_vMapSize.cy);
	}
}
*/

//할당되어진 메모리사용 (사용후는 풀어줘야 함.)
void CPathFind::SetAllocedMap(int cx, int cy, BYTE **map)
{
	int i;

	if(m_pMap)
	{
		for(i = 0; i < m_vMapSize.cx; i++)
		{
			SAFE_DELETES( m_pMap[i] );
		}
		SAFE_DELETES( m_pMap );
	}
	ClearData();

////////////////////////////////////////////////////////////////////
// berserk (1999.11.10) 마름모 사각형.
//	메모리 할당.
//	int width = (cx+1)/2 + (cy+1)/2 - 1;

	m_pMap = new BYTE *[cx];
	for(i = 0; i < cx; i++)
	{
		m_pMap[i] = new BYTE[cy];
	}

//	초기화
	int j;
	for (i=0;i<cx;i++)
	{
		for (j=0;j<cy;j++)
		{
			m_pMap[i][j] = 0;
		}
	}

//  맵 컨버팅(직사각형->마름모)
/*	int tempx, tempy;
	int temph = cy / 2 - 1;

	for( int y = 0; y < cy - 1; y++)
	{
		for( int x = 0; x < cx - 1; x++)
		{
			if( (x+y)%2 == 0 )
			{
				tempx = temph - ( y / 2 ) + ( x / 2 );

				if( x % 2 ) tempy = ( y / 2 ) + ( ( x / 2 ) + 1 );
				else        tempy = ( y / 2 ) + ( x / 2 );

				m_pMap[tempx][tempy] = map[x][y];
			}
		}
	}
*/
	m_vMapSize.cx = cx;
	m_vMapSize.cy = cy;//width;

	for (i=0;i < cy-1;i++)
	{
		for (j=0;j < cx-1;j++)
		{
			m_pMap[j][i] = map[j][i];
		}
	}
///////////////////////////////////////////////////////////////////
}

void CPathFind::SetAllocedHouseMap(int cx, int cy, BYTE **map)
{
	int i;

	if(m_pHouseMap)
	{
		for(i = 0; i < m_vMapSize.cx; i++)
		{
			SAFE_DELETES( m_pHouseMap[i] );
		}
		SAFE_DELETES(m_pHouseMap);
	}
	ClearData();

////////////////////////////////////////////////////////////////////
// berserk (1999.11.10) 마름모 사각형.
//	메모리 할당.
	int width = (cx+1)/2 + (cy+1)/2 - 1;

	m_pHouseMap = new BYTE *[width];
	for(i = 0; i < width; i++)
	{
		m_pHouseMap[i] = new BYTE[width];
	}

//	초기화
	int j;
	for (i=0;i<width;i++)
	{
		for (j=0;j<width;j++)
		{
			m_pHouseMap[i][j] = 0;
		}
	}

//  맵 컨버팅(직사각형->마름모)
	int tempx, tempy;
	int temph = cy / 2 - 1;

	for( int y = 0; y < cy - 1; y++)
	{
		for( int x = 0; x < cx - 1; x++)
		{
			if( (x+y)%2 == 0 )
			{
				tempx = temph - ( y / 2 ) + ( x / 2 );

				if( x % 2 ) tempy = ( y / 2 ) + ( ( x / 2 ) + 1 );
				else        tempy = ( y / 2 ) + ( x / 2 );

				m_pHouseMap[tempx][tempy] = map[x][y];
			}
		}
	}
	m_vMapSize.cx = m_vMapSize.cy = width;

//  map 메모리 해제.
	if (map)
	{
		for (i=0;i<cx;i++)
		{
			SAFE_DELETE(map[i]);
		}
		SAFE_DELETE(map);
	}
////////////////////////////////////////////////////////////////////
}

NODE *CPathFind::FindPath(int start_x, int start_y, int dest_x, int dest_y)
// start_x, start_y->End, dest_x, dest_y->Start : 변환된 좌표.
{
//	FreeCell(CPoint(dest_x, dest_y));

	NODE *t_node, *r_node;
	
	if(!m_pMap) return NULL;

	m_rtRegion.SetRect(start_x, start_y, dest_x, dest_y);	
	m_rtRegion.NormalizeRect();
	m_rtRegion.InflateRect(10, 10);
	//m_rtRegion.NormalizeRect();

//	m_ptStart.x = start_x;		m_ptStart.y = start_x;
//	m_ptEnd.x = dest_x;			m_ptEnd.y = dest_x;	
	if (start_x < 0 || start_y <0 || dest_x <0 || dest_y <0)
	{
#ifdef USE_LOGFILE
		__GetMainWnd()->LogString("CPathFind::FindPath 좌표에  - 가 왔음");
#endif
		return NULL;
	}

	ClearData();
	m_pOpen = (NODE *)calloc(1, sizeof(NODE));
	m_pClosed = (NODE *)calloc(1, sizeof(NODE));
	
	t_node = (NODE *)calloc(1, sizeof(NODE));
	t_node->g = 0;
	t_node->h = (int)sqrt((start_x-dest_x)*(start_x-dest_x) + (start_y-dest_y)*(start_y-dest_y));
	t_node->f = t_node->g + t_node->h;
	t_node->x = start_x;
	t_node->y = start_y;
	
	m_pOpen->NextNode = t_node;
	while(1) {
		r_node = (NODE *)ReturnBestNode();
		if(r_node == NULL) break;
		if(r_node->x == dest_x && r_node->y == dest_y)
			break;
		FindChildPath(r_node, dest_x, dest_y);
	}
//	LockCell(CPoint(start_x, start_y));

	return r_node;
}

//우석........
void CPathFind::FindPath(CList<CPoint, CPoint> *pRoute, int start_x, int start_y, int dest_x, int dest_y)
{
	NODE *t_node, *r_node, *tmp_node;
	
	if(!m_pMap)	return ;

//	m_rtRegion.SetRect(start_x, start_y, dest_x, dest_y);	
//	m_rtRegion.NormalizeRect();
//	m_rtRegion.InflateRect(10, 10);
	//m_rtRegion.NormalizeRect();

//	m_ptStart.x = start_x;		m_ptStart.y = start_x;
//	m_ptEnd.x = dest_x;			m_ptEnd.y = dest_x;	
	if (start_x < 0 || start_y <0 || dest_x <0 || dest_y <0)
	{
		return ;
	}

	ClearData();
	m_pOpen = (NODE *)calloc(1, sizeof(NODE));
	m_pClosed = (NODE *)calloc(1, sizeof(NODE));
	
	t_node = (NODE *)calloc(1, sizeof(NODE));
	t_node->g = 0;
	t_node->h = (start_x-dest_x)*(start_x-dest_x) + (start_y-dest_y)*(start_y-dest_y);
	t_node->f = t_node->g + t_node->h;
	t_node->x = start_x;
	t_node->y = start_y;
	
	if(!IsBlankMap(dest_x,dest_y))
	{
		TRACE("목적지가 갈 수 없는 길이다....\n");
		m_FindStep /= 2;
	}

	m_pOpen->NextNode = t_node;	
	while(m_FindStep) {
		r_node = (NODE *)ReturnBestNode();
		if(r_node == NULL) break;
		if(r_node->x == dest_x && r_node->y == dest_y)
			break;
		FindChildPath(r_node, dest_x, dest_y);
		m_FindStep--;
	}

	tmp_node = r_node;
	while(tmp_node)
	{
		pRoute->AddHead(CPoint(tmp_node->x,tmp_node->y));
		tmp_node = tmp_node->Parent;
	}
	if(pRoute->GetCount() != 0) pRoute->RemoveHead();
}


#ifdef _DEBUG
NODE *CPathFind::FindPath(CDC& dc, int cellx, int celly, int start_x, int start_y, int dest_x, int dest_y)
{
	NODE *t_node, *r_node;
	
	if(!m_pMap) return NULL;

	CFont *old_font, font;
	m_pDC = &dc;
	m_vCell.cx = cellx;
	m_vCell.cy = celly;
	font.CreateFont(-20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림체");
	old_font = m_pDC->SelectObject(&font);

	ClearData();
	m_pOpen = (NODE *)calloc(1, sizeof(NODE));
	m_pClosed = (NODE *)calloc(1, sizeof(NODE));
	
	t_node = (NODE *)calloc(1, sizeof(NODE));
	t_node->g = 0;
	t_node->h = (int)sqrt((start_x-dest_x)*(start_x-dest_x) + (start_y-dest_y)*(start_y-dest_y));
	t_node->f = t_node->g + t_node->h;
	t_node->x = start_x;
	t_node->y = start_y;
	
	m_pOpen->NextNode = t_node;
	while(1) {
		r_node = (NODE *)ReturnBestNode();
		if(r_node == NULL) break;
		if(r_node->x == dest_x && r_node->y == dest_y)
			break;
		FindChildPath(r_node, dest_x, dest_y);
	}

	m_pDC->SelectObject(old_font);
	font.DeleteObject();
	m_pDC = NULL;

	return r_node;
}

void CPathFind::DisplayData(int x, int y, int value)
{
	if(!m_pDC) return;

	CRect t_rect(x*m_vCell.cx, y*m_vCell.cy, (x+1)*m_vCell.cx, (y+1)*m_vCell.cy);
	CString str;

	str.Format("%d", value);
	m_pDC->DrawText(str, &t_rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}
#endif

NODE *CPathFind::ReturnBestNode()
{
	NODE *tmp;
	
	if(m_pOpen->NextNode == NULL) {
		return NULL;
	}
	
	tmp=m_pOpen->NextNode;   // point to first node on m_pOpen
	m_pOpen->NextNode=tmp->NextNode;    // Make m_pOpen point to nextnode or NULL.
	
	tmp->NextNode=m_pClosed->NextNode;
	m_pClosed->NextNode=tmp;
	
	return(tmp);
}

void CPathFind::FindChildPath(NODE *node, int dx, int dy)
{
	int x, y;


	// Upper
	if(IsBlankMap(x=node->x,y=node->y-2))
		FindChildPathSub(node, x, y, dx, dy,1);
	// Right
	if(IsBlankMap(x=node->x+2,y=node->y))
		FindChildPathSub(node, x, y, dx, dy,1);
	// Lower
	if(IsBlankMap(x=node->x,y=node->y+2))
		FindChildPathSub(node, x, y, dx, dy,1);
	// Left
	if(IsBlankMap(x=node->x-2,y=node->y))
		FindChildPathSub(node, x, y, dx, dy,1);

	// UpperRight
	if(IsBlankMap(x=node->x+1,y=node->y-1))
		FindChildPathSub(node, x, y, dx, dy,1);	
	
	// LowerRight
	if(IsBlankMap(x=node->x+1,y=node->y+1))
		FindChildPathSub(node, x, y, dx, dy,1);
	
	// LowerLeft
	if(IsBlankMap(x=node->x-1,y=node->y+1))
		FindChildPathSub(node, x, y, dx, dy,1);

	// UpperLeft
	if(IsBlankMap(x=node->x-1,y=node->y-1))
		FindChildPathSub(node, x, y, dx, dy,1);


/*
	// Upper
	if(IsBlankMap(x=node->x,y=node->y-1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_ONE_FIND_CROSS		);
	// Right
	if(IsBlankMap(x=node->x+1,y=node->y))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_ONE_FIND_CROSS		);
	// Lower
	if(IsBlankMap(x=node->x,y=node->y+1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_ONE_FIND_CROSS		);
	// Left
	if(IsBlankMap(x=node->x-1,y=node->y))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_ONE_FIND_CROSS		);

	// UpperRight
	if(IsBlankMap(x=node->x+1,y=node->y-1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_ONE_FIND_DIAGONAL	);	
	
	// LowerRight
	if(IsBlankMap(x=node->x+1,y=node->y+1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_ONE_FIND_DIAGONAL	);
	
	// LowerLeft
	if(IsBlankMap(x=node->x-1,y=node->y+1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_ONE_FIND_DIAGONAL	);

	// UpperLeft
	if(IsBlankMap(x=node->x-1,y=node->y-1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_ONE_FIND_DIAGONAL);

*/
/*
	// UpperLeft
	if(IsBlankMap(x=node->x-1,y=node->y-1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_TWO_FIND_CROSS		);
	// Upper
	if(IsBlankMap(x=node->x,y=node->y-1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_TWO_FIND_DIAGONAL	);
	// UpperRight
	if(IsBlankMap(x=node->x+1,y=node->y-1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_TWO_FIND_CROSS		);
	// Right
	if(IsBlankMap(x=node->x+1,y=node->y))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_TWO_FIND_DIAGONAL	);
	// LowerRight
	if(IsBlankMap(x=node->x+1,y=node->y+1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_TWO_FIND_CROSS		);
	// Lower
	if(IsBlankMap(x=node->x,y=node->y+1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_TWO_FIND_DIAGONAL	);
	// LowerLeft
	if(IsBlankMap(x=node->x-1,y=node->y+1))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_TWO_FIND_CROSS		);
	// Left
	if(IsBlankMap(x=node->x-1,y=node->y))
		FindChildPathSub(node, x, y, dx, dy, LEVEL_TWO_FIND_DIAGONAL	);
*/
}

void CPathFind::FindChildPathSub(NODE *node, int x, int y, int dx, int dy, int arg)
{
	int g, c=0;
	NODE *old_node,*t_node;
	
	g = node->g + arg;
	
	if((old_node = CheckOpen(x, y)) != NULL)
	{
		for(c = 0; c < 8; c++)
		{
			if(node->Child[c] == NULL)
			{
				break;
			}
		}
		node->Child[c] = old_node;
		if(g < old_node->g)
		{
			old_node->Parent = node;
			old_node->g = g;
			old_node->f = g + old_node->h;
#ifdef _DEBUG
			DisplayData(old_node->x, old_node->y, old_node->g);
#endif
		}
	}
	else if((old_node = CheckClosed(x, y)) != NULL)
	{
		for(c = 0; c < 8; c++)
		{
			if(node->Child[c] == NULL)
			{
				break;
			}
		}
		node->Child[c] = old_node;
		if(g < old_node->g)
		{
			old_node->Parent = node;
			old_node->g = g;
			old_node->f = g + old_node->h;
#ifdef _DEBUG
			DisplayData(old_node->x, old_node->y, old_node->g);
#endif
			PropagateDown(old_node);
		}
	}
	else
	{
		t_node = (NODE *)calloc(1, sizeof(NODE));
		t_node->Parent = node;
		t_node->g = g;
		t_node->h = (int)sqrt((x-dx)*(x-dx) + (y-dy)*(y-dy));
		t_node->f = g + t_node->h;
		t_node->x = x;
		t_node->y = y;
#ifdef _DEBUG
		DisplayData(t_node->x, t_node->y, t_node->g);
#endif
		Insert(t_node);
		for(c = 0; c < 8; c++)
		{
			if(node->Child[c] == NULL)
			{
				break;
			}
		}
		node->Child[c] = t_node;
	}
}

NODE *CPathFind::CheckOpen(int x, int y)
{
	NODE *tmp;
	
	tmp = m_pOpen->NextNode;
	while(tmp != NULL)
	{
		if(tmp->x == x && tmp->y == y)
		{
			return tmp;
		}
		else
		{
			tmp = tmp->NextNode;
		}
	}

	return NULL;
}

NODE *CPathFind::CheckClosed(int x, int y)
{
	NODE *tmp;
	
	tmp = m_pClosed->NextNode;
	
	while(tmp != NULL)
	{
		if(tmp->x == x && tmp->y == y)
		{
			return tmp;
		}
		else
		{
			tmp = tmp->NextNode;
		}
	}

	return NULL;
}

void CPathFind::Insert(NODE *node)
{
	NODE *tmp1, *tmp2;
	int f;
	
	if(m_pOpen->NextNode == NULL)
	{
		m_pOpen->NextNode = node;
		return;
	}
	
	f = node->f;
	tmp1 = m_pOpen;
	tmp2 = m_pOpen->NextNode;
	while((tmp2 != NULL) && (tmp2->f < f))
	{
		tmp1 = tmp2;
		tmp2 = tmp2->NextNode;
	}
	node->NextNode = tmp2;
	tmp1->NextNode = node;
}

void CPathFind::PropagateDown(NODE *old)
{
	int c, g;
	NODE *child, *parent;
	
	g = old->g;
	for(c = 0; c < 8; c++)
	{
		if((child = old->Child[c]) == NULL)
		{
			break;
		}
		if(g+1 < child->g)
		{
			child->g = g+1;
			child->f = child->g + child->h;
			child->Parent = old;
			Push(child);
		}
	}
	
	while (m_pStack->NextStackPtr != NULL)
	{
		parent = Pop();
		for(c = 0; c < 8; c++)
		{
			if((child = parent->Child[c])==NULL)
			{
				break;
			}
			if(parent->g+1 < child->g)
			{
				child->g = parent->g+1;
				child->f = parent->g + parent->h;
				child->Parent = parent;
				Push(child);
			}
		}
	}
}

void CPathFind::Push(NODE *node)
{
	STACK *tmp;
	
	tmp = (STACK *)calloc(1, sizeof(STACK));
	tmp->NodePtr = node;
	tmp->NextStackPtr = m_pStack->NextStackPtr;
	m_pStack->NextStackPtr = tmp;
}

NODE *CPathFind::Pop()
{
	NODE *t_node;
	STACK *t_stack;
	
	t_stack = m_pStack->NextStackPtr;
	t_node = t_stack->NodePtr;
	
	m_pStack->NextStackPtr = t_stack->NextStackPtr;
	free(t_stack);

	return t_node;
}

BOOL CPathFind::IsBlankMap(int x, int y)
// return value 1: 갈수 있는곳, 0:갈수 없는곳.
{
	if(x < 0 || y < 0 || x >= m_vMapSize.cx || y >= m_vMapSize.cy) return FALSE;
// berserk(일정한 맵 범위에서 찾기 위해서)
//	if (!m_rtRegion.PtInRect(CPoint(x,y)))	return FALSE;
	if(m_pMap[x][y]) return TRUE;
	else return FALSE;
}

BOOL CPathFind::IsMovable(int x, int y)
// m_pMap 0 : 움직일수 있다. 1: 움직일수 없다. => 반대넹...바뀌었나??
{
	if(x < 0 || y < 0 || x >= m_vMapSize.cx || y >= m_vMapSize.cy) 
	{
		TRACE("영역을 벗어 났다..\n");
		return FALSE;
	}
//	TRACE("m_pMap[%d][%d] = %d\n", x, y, (int)m_pMap[x][y]);
	if(m_pMap[x][y]) 
		return TRUE;
	else 
		return FALSE;
}

void CPathFind::LockCell(CPoint &ptCell)
{
//	CPoint p = CPacketParser::Conv2ServerMapPos(ptCell);
//	m_pMap[p.x][p.y] = 1;

	m_pMap[ptCell.x][ptCell.y] = 1;
	TRACE ("Lock [%d, %d]\n", ptCell.x, ptCell.y);
}

void CPathFind::FreeCell(CPoint &ptCell)
{
//	CPoint p = CPacketParser::Conv2ServerMapPos(ptCell);
//	m_pMap[p.x][p.y] = 0;

	m_pMap[ptCell.x][ptCell.y] = 0;
	TRACE ("Free [%d, %d]\n", ptCell.x, ptCell.y);
}

int CPathFind::GetMapData(CPoint ptCell)
{
//	TRACE ("MapData [%d, %d] = %d \n", ptCell.x, ptCell.y, (int)m_pMap[ptCell.x][ptCell.y]);
	return (int)m_pMap[ptCell.x][ptCell.y];
}

int CPathFind::GetHouseMapData(CPoint ptCell)
{
	return (int)m_pHouseMap[ptCell.x][ptCell.y];
}

//우석.. 10월 11일..
void CPathFind::FindPath2(CList<CPoint, CPoint> *pRoute, int start_x, int start_y, int dest_x, int dest_y, int pre_x, int pre_y)
{
	if(!pRoute) return;
	if(m_FindStep < 0) return;
	m_FindStep--;

	CPoint  NextPt;
	int		x_tab = 0, y_tab = 0;
	int		distance = -1, distance1 = -1, distance2 = -1, distance3 = -1;


	if(dest_x == start_x && dest_y < start_y) //0 direction..
	{
		if( IsMovable(start_x, start_y-2) && (pre_x != start_x || pre_y != start_y-2))
		{
			x_tab = 0; y_tab = -2;
			distance1 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
			distance = distance1;			
		}
		else if(start_x == dest_x && start_y-2 == dest_y)	return;

		if( IsMovable(start_x-1, start_y-1) && (pre_x != start_x-1 || pre_y != start_y-1) )
		{
			x_tab = -1; y_tab = -1;
			distance2 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance2);
			else distance = distance2;
		}
		else if(start_x-1 == dest_x && start_y-1 == dest_y)	return;

		if( IsMovable(start_x+1, start_y-1) && (pre_x != start_x+1 || pre_y != start_y-1) )
		{
			x_tab = 1; y_tab = -1;
			distance3 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance3);
			else distance = distance3;
		}
		else if(start_x+1 == dest_x && start_y-1 == dest_y)	return;

		if( distance < 0) return ;

		if( distance == distance1)		{ x_tab = 0; y_tab = -2; }
		else if( distance == distance2) { x_tab = -1; y_tab = -1; }
		else                            { x_tab = 1; y_tab = -1; }

		NextPt.x = start_x + x_tab;
		NextPt.y = start_y + y_tab;

		if(pRoute->IsEmpty()) { pre_x = 0; pre_y = 0; }
		else				  { pre_x = pRoute->GetTail().x; pre_y = pRoute->GetTail().y; }

		pRoute->AddTail(NextPt);

		FindPath2(pRoute, NextPt.x, NextPt.y, dest_x, dest_y, pre_x, pre_y);		
	}
	else if(dest_x > start_x && dest_y < start_y) //1 direction..
	{
		if( IsMovable(start_x, start_y-2) && (pre_x != start_x || pre_y != start_y-2) )
		{
			x_tab = 0; y_tab = -2;
			distance1 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
			distance = distance1;			
		}
		else if(start_x == dest_x && start_y-2 == dest_y)	return;

		if( IsMovable(start_x+1, start_y-1) && (pre_x != start_x+1 || pre_y != start_y-1) )
		{
			x_tab = 1; y_tab = -1;
			distance2 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance2);
			else distance = distance2;
		}
		else if(start_x+1 == dest_x && start_y-1 == dest_y)	return;

		if( IsMovable(start_x+2, start_y) && (pre_x != start_x+2 || pre_y != start_y) )
		{
			x_tab = 2; y_tab = 0;
			distance3 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance3);
			else distance = distance3;
		}
		else if(start_x+2 == dest_x && start_y == dest_y)	return;

		if( distance < 0) return ;

		if( distance == distance1)		{ x_tab = 0; y_tab = -2; }
		else if( distance == distance2) { x_tab = 1; y_tab = -1; }
		else                            { x_tab = 2; y_tab = 0; }

		NextPt.x = start_x + x_tab;
		NextPt.y = start_y + y_tab;

		if(pRoute->IsEmpty()) { pre_x = 0; pre_y = 0; }
		else				  { pre_x = pRoute->GetTail().x; pre_y = pRoute->GetTail().y; }

		pRoute->AddTail(NextPt);

		FindPath2(pRoute, NextPt.x, NextPt.y, dest_x, dest_y, pre_x, pre_y);		
	}
	else if(dest_x > start_x && dest_y == start_y) //2 direction..
	{
		if( IsMovable(start_x+2, start_y) && (pre_x != start_x+2 || pre_y != start_y) )
		{
			x_tab = 2; y_tab = 0;
			distance1 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
			distance = distance1;			
		}
		else if(start_x+2 == dest_x && start_y == dest_y)	return;

		if( IsMovable(start_x+1, start_y-1) && (pre_x != start_x+1 || pre_y != start_y-1) )
		{
			x_tab = 1; y_tab = -1;
			distance2 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance2);
			else distance = distance2;
		}
		else if(start_x+1 == dest_x && start_y-1 == dest_y)	return;

		if( IsMovable(start_x+1, start_y+1) && (pre_x != start_x+1 || pre_y != start_y+1) )
		{
			x_tab = 1; y_tab = 1;
			distance3 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance3);
			else distance = distance3;
		}
		else if(start_x+1 == dest_x && start_y+1 == dest_y)	return;

		if( distance < 0) return ;

		if( distance == distance1)		{ x_tab = 2; y_tab = 0; }
		else if( distance == distance2) { x_tab = 1; y_tab = -1; }
		else                            { x_tab = 1; y_tab = 1; }

		NextPt.x = start_x + x_tab;
		NextPt.y = start_y + y_tab;

		if(pRoute->IsEmpty()) { pre_x = 0; pre_y = 0; }
		else				  { pre_x = pRoute->GetTail().x; pre_y = pRoute->GetTail().y; }

		pRoute->AddTail(NextPt);

		FindPath2(pRoute, NextPt.x, NextPt.y, dest_x, dest_y, pre_x, pre_y);		
	}
	else if(dest_x > start_x && dest_y > start_y) //3 direction..
	{
		if( IsMovable(start_x+2, start_y) && (pre_x != start_x+2 || pre_y != start_y) )
		{
			x_tab = 2; y_tab = 0;
			distance1 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
			distance = distance1;			
		}
		else if(start_x+2 == dest_x && start_y == dest_y)	return;

		if( IsMovable(start_x+1, start_y+1) && (pre_x != start_x+1 || pre_y != start_y+1) )
		{
			x_tab = 1; y_tab = 1;
			distance2 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance2);
			else distance = distance2;
		}
		else if(start_x+1 == dest_x && start_y+1 == dest_y)	return;

		if( IsMovable(start_x, start_y+2) && (pre_x != start_x || pre_y != start_y+2) )
		{
			x_tab = 0; y_tab = 2;
			distance3 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance3);
			else distance = distance3;
		}
		else if(start_x == dest_x && start_y+2 == dest_y)	return;

		if( distance < 0) return ;

		if( distance == distance1)		{ x_tab = 2; y_tab = 0; }
		else if( distance == distance2) { x_tab = 1; y_tab = 1; }
		else                            { x_tab = 0; y_tab = 2; }

		NextPt.x = start_x + x_tab;
		NextPt.y = start_y + y_tab;

		if(pRoute->IsEmpty()) { pre_x = 0; pre_y = 0; }
		else				  { pre_x = pRoute->GetTail().x; pre_y = pRoute->GetTail().y; }

		pRoute->AddTail(NextPt);
		FindPath2(pRoute, NextPt.x, NextPt.y, dest_x, dest_y, pre_x, pre_y);		
	}
	else if(dest_x == start_x && dest_y > start_y) //4 direction..
	{
		if( IsMovable(start_x, start_y+2) && (pre_x != start_x || pre_y != start_y+2) )
		{
			x_tab = 0; y_tab = 2;
			distance1 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
			distance = distance1;			
		}
		else if(start_x == dest_x && start_y+2 == dest_y)	return;

		if( IsMovable(start_x+1, start_y+1) && (pre_x != start_x+1 || pre_y != start_y+1) )
		{
			x_tab = 1; y_tab = 1;
			distance2 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance2);
			else distance = distance2;
		}
		else if(start_x+1 == dest_x && start_y+1 == dest_y)	return;

		if( IsMovable(start_x-1, start_y+1) && (pre_x != start_x-1 || pre_y != start_y+1) )
		{
			x_tab = -1; y_tab = 1;
			distance3 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance3);
			else distance = distance3;
		}
		else if(start_x-1 == dest_x && start_y+1 == dest_y)	return;

		if( distance < 0) return ;

		if( distance == distance1)		{ x_tab = 0; y_tab = 2; }
		else if( distance == distance2) { x_tab = 1; y_tab = 1; }
		else                            { x_tab = -1; y_tab = 1; }

		NextPt.x = start_x + x_tab;
		NextPt.y = start_y + y_tab;

		if(pRoute->IsEmpty()) { pre_x = 0; pre_y = 0; }
		else				  { pre_x = pRoute->GetTail().x; pre_y = pRoute->GetTail().y; }

		pRoute->AddTail(NextPt);
		FindPath2(pRoute, NextPt.x, NextPt.y, dest_x, dest_y, pre_x, pre_y);
	}
	else if(dest_x < start_x && dest_y > start_y) //5 direction..
	{
		if( IsMovable(start_x, start_y+2) && (pre_x != start_x || pre_y != start_y+2) )
		{
			x_tab = 0; y_tab = 2;
			distance1 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
			distance = distance1;			
		}
		else if(start_x == dest_x && start_y+2 == dest_y)	return;

		if( IsMovable(start_x-1, start_y+1) && (pre_x != start_x-1 || pre_y != start_y+1) )
		{
			x_tab = -1; y_tab = 1;
			distance2 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance2);
			else distance = distance2;
		}
		else if(start_x-1 == dest_x && start_y+1 == dest_y)	return;

		if( IsMovable(start_x-2, start_y) && (pre_x != start_x-2 || pre_y != start_y) )
		{
			x_tab = -2; y_tab = 0;
			distance3 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance3);
			else distance = distance3;
		}
		else if(start_x-2 == dest_x && start_y == dest_y)	return;

		if( distance < 0) return ;

		if( distance == distance1)		{ x_tab = 0; y_tab = 2; }
		else if( distance == distance2) { x_tab = -1; y_tab = 1; }
		else                            { x_tab = -2; y_tab = 0; }

		NextPt.x = start_x + x_tab;
		NextPt.y = start_y + y_tab;

		if(pRoute->IsEmpty()) { pre_x = 0; pre_y = 0; }
		else				  { pre_x = pRoute->GetTail().x; pre_y = pRoute->GetTail().y; }

		pRoute->AddTail(NextPt);
		FindPath2(pRoute, NextPt.x, NextPt.y, dest_x, dest_y, pre_x, pre_y);
	}
	else if(dest_x < start_x && dest_y == start_y) //6 direction..
	{
		if( IsMovable(start_x-2, start_y) && (pre_x != start_x-2 || pre_y != start_y) )
		{
			x_tab = -2; y_tab = 0;
			distance1 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
			distance = distance1;			
		}
		else if(start_x-2 == dest_x && start_y == dest_y)	return;

		if( IsMovable(start_x-1, start_y+1) && (pre_x != start_x-1 || pre_y != start_y+1) )
		{
			x_tab = -1; y_tab = 1;
			distance2 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance2);
			else distance = distance2;
		}
		else if(start_x-1 == dest_x && start_y+1 == dest_y)	return;

		if( IsMovable(start_x-1, start_y-1) && (pre_x != start_x-1 || pre_y != start_y-1) )
		{
			x_tab = -1; y_tab = -1;
			distance3 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance3);
			else distance = distance3;
		}
		else if(start_x-1 == dest_x && start_y-1 == dest_y)	return;

		if( distance < 0) return ;

		if( distance == distance1)		{ x_tab = -2; y_tab = 0; }
		else if( distance == distance2) { x_tab = -1; y_tab = 1; }
		else                            { x_tab = -1; y_tab = -1; }

		NextPt.x = start_x + x_tab;
		NextPt.y = start_y + y_tab;

		if(pRoute->IsEmpty()) { pre_x = 0; pre_y = 0; }
		else				  { pre_x = pRoute->GetTail().x; pre_y = pRoute->GetTail().y; }

		pRoute->AddTail(NextPt);
		FindPath2(pRoute, NextPt.x, NextPt.y, dest_x, dest_y, pre_x, pre_y);
	}
	else if(dest_x < start_x && dest_y < start_y) //7 direction..
	{
		if( IsMovable(start_x, start_y-2) && (pre_x != start_x || pre_y != start_y-2) )
		{
			x_tab = 0; y_tab = -2;
			distance1 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
			distance = distance1;			
		}
		else if(start_x == dest_x && start_y-2 == dest_y)	return;

		if( IsMovable(start_x-1, start_y-1) && (pre_x != start_x-1 || pre_y != start_y-1) )
		{
			x_tab = -1; y_tab = -1;
			distance2 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance2);
			else distance = distance2;
		}
		else if(start_x-1 == dest_x && start_y-1 == dest_y)	return;

		if( IsMovable(start_x-2, start_y) && (pre_x != start_x-2 || pre_y != start_y) )
		{
			x_tab = -2; y_tab = 0;
			distance3 =  (dest_x - start_x - x_tab) * (dest_x - start_x - x_tab) +
						 (dest_y - start_y - y_tab) * (dest_y - start_y - y_tab);
		    if(distance >= 0) distance = min(distance,distance3);
			else distance = distance3;
		}
		else if(start_x-2 == dest_x && start_y == dest_y)	return;

		if( distance < 0) return ;

		if( distance == distance1)		{ x_tab = 0; y_tab = -2; }
		else if( distance == distance2) { x_tab = -1; y_tab = -1; }
		else                            { x_tab = -2; y_tab = 0; }

		NextPt.x = start_x + x_tab;
		NextPt.y = start_y + y_tab;

		if(pRoute->IsEmpty()) { pre_x = 0; pre_y = 0; }
		else				  { pre_x = pRoute->GetTail().x; pre_y = pRoute->GetTail().y; }

		pRoute->AddTail(NextPt);
		FindPath2(pRoute, NextPt.x, NextPt.y, dest_x, dest_y, pre_x, pre_y);
	}
	else return;

	return;
}

BOOL CPathFind::SetMovable(CPoint pt, bool bMove)
{
	if(pt.x < 0 || pt.y < 0 || pt.x >= m_vMapSize.cx || pt.y >= m_vMapSize.cy) return FALSE;

	m_pMap[pt.x][pt.y] = bMove;

	return TRUE;
}