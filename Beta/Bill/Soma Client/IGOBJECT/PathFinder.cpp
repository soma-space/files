/////////////////////////////////////////////////////////////////////////////
// pathfinde.h

#include "stdAfx.h"
#include "PathFinder.h"

CPathFinder::CPathFinder()
{
	OPEN = NULL;	// the node list pointers
	CLOSED = NULL;	
	PATH = NULL;	// 최단경로 pointer

	Stack = NULL;

	BOOL	isPath = FALSE; // 올바른 경로일때만 참이다..  

	m_nWidth = 0; m_nHeight = 0; 			// 유효한 넓이 높이 (실제 맵은 상하좌우 1씩 크다..)
	m_nTileWidth = 0; m_nTileHeight = 0;	// 실제 타일 단위 넓이, 높이
	m_nPixelWidth = 0; m_nPixelHeight = 0; 	// 픽셀 단위 넓이 높이
	m_dwTotalTiles = 0; // 총 타일갯수.. - 단 주위에 벽이 둘러져 있다..
	m_pTiles = NULL;				// 타일 맵 데이타 포인터

}

////////////////////////////////////////////////////////////////////////////////

CPathFinder::~CPathFinder()
{
	DeleteNodes();
	if(Stack) { delete Stack; Stack = NULL; }
	if(m_pTiles) { delete [] m_pTiles; m_pTiles = NULL; }
}

////////////////////////////////////////////////////////////////////////////////
//                             Public Member Functions                        //
////////////////////////////////////////////////////////////////////////////////

void CPathFinder::Init(const int* pTiles, int width, int height)
{
	Stack = new _STACK; memset(Stack, 0, sizeof(_STACK));
	isPath = FALSE;
	OPEN = NULL;
	CLOSED = NULL;
	PATH = NULL;

	// 넓이, 높이, 총 갯수를 계산 - 주위에 둘러싼 타일이 있으므로 각각 2를 더해준다..
	m_nPixelWidth = width * TILE_SIZE;
	m_nPixelHeight = height * TILE_SIZE;

	m_nWidth = width;
	m_nHeight = height;

	m_nTileWidth = width+2;
	m_nTileHeight = height+2;
	m_dwTotalTiles = m_nTileWidth*m_nTileHeight;
						   
	// 타일 할당... 그리고 타일 내용을 0 으로 비운다..
	m_pTiles = new int[m_dwTotalTiles];
	memset(m_pTiles, 0, m_dwTotalTiles*4);

	// 타일 주위를 1로 세팅한다.. - 막는다.
	int x, y; 
	for (x = 0; x < m_nTileWidth; x++)
		m_pTiles[x] = 1;
	for (y = 1; y < m_nTileHeight-1; y++)
	{
		m_pTiles[y*m_nTileWidth] = 1;
		m_pTiles[y*m_nTileWidth + m_nTileWidth-1] = 1;
	}
	for (x = 0; x < m_nTileWidth; x++)
		m_pTiles[(m_nTileHeight-1)*m_nTileWidth+x]=1;
	
	// 타일 값을 채운다..
	for (y = 0; y < height; y++) 
		for (x = 1; x < width; x++) 
			m_pTiles[(y+1)*m_nTileWidth + (x+1)] = pTiles[y*width+x];

}

////////////////////////////////////////////////////////////////////////////////

BOOL CPathFinder::CalcNewPath(int sx,int sy, int dx,int dy)
{
	int oldVal = GetTile(sx,sy);
	if(oldVal != 0) SetTile(sx, sy, 0); // 현재 꼼짝도 못하면..

	if ((GetTile(dx,dy) == 0) && (GetTile(sx,sy) == 0) && ((TileNum(sx,sy) != TileNum(dx,dy))) )
	{
		DeleteNodes(); // clear node lists
		isPath = FindPath(sx,sy,dx,dy);
	}
	else isPath = FALSE;

	SetTile(sx, sy, oldVal); // 원래대로 돌린다..

	return isPath;
}

POINT CPathFinder::GetNextWay()
{
	POINT ptNext = {0, 0};
	if ( !isPath ) return ptNext;
	if ( PATH == NULL)  return ptNext;
	if ( PATH->Parent == NULL ) return ptNext;
	PATH = PATH->Parent;
	if ( PATH == NULL)  return ptNext;

	ptNext.x = PATH->x;
	ptNext.y = PATH->y;
	return ptNext;
}

BOOL CPathFinder::ReachedGoal(void)		// check it's return value before getting
{										// the node entries
   if ( !isPath ) return TRUE;
   if ( PATH->Parent != NULL ) return FALSE;
   else return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//								      Private Member Functions                        //
////////////////////////////////////////////////////////////////////////////////

void CPathFinder::DeleteNodes(void)
{
	_NODE *Node, *OldNode;

	if ( OPEN != NULL )
	{
		Node = OPEN->NextNode;
		while ( Node != NULL )
		{
			OldNode = Node;
			Node = Node->NextNode;
			delete OldNode; OldNode = NULL;
		}
	}

	if ( CLOSED != NULL )
	{
		Node = CLOSED->NextNode;
		while ( Node != NULL )
		{
			OldNode = Node;
			Node = Node->NextNode;
			delete OldNode; OldNode = NULL;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//                               A* Algorithm                                 //
////////////////////////////////////////////////////////////////////////////////

BOOL CPathFinder::FindPath(int sx, int sy, int dx, int dy)
{
	_NODE *Node, *BestNode;
   int TileNumDest;

   TileNumDest = TileNum(sx, sy);

   OPEN = new _NODE; memset(OPEN, 0, sizeof(_NODE));
   CLOSED = new _NODE; memset(CLOSED, 0, sizeof(_NODE));

   Node = new _NODE; memset(Node, 0, sizeof(_NODE));
   Node->g = 0;
   Node->h = (dx-sx)*(dx-sx) + (dy-sy)*(dy-sy);  // should really use sqrt().
   Node->f = Node->g+Node->h;
   Node->NodeNum = TileNum(dx, dy);
   Node->x = dx;
   Node->y = dy;

   OPEN->NextNode=Node;        // make Open List point to first node
   DWORD count = 0; // zebra
   for (;;)
   {
		BestNode = ReturnBestNode();
		if(count >= MAX_PATH_CHECK_COUNT) // zebra
			return FALSE;
		count++; // zebra
		if(BestNode == NULL)
		{
			return FALSE;
		}
		if (BestNode->NodeNum == TileNumDest)    // if we've found the end, break and finish
	   	break;
		GenerateSuccessors(BestNode,sx,sy);
   }
   PATH = BestNode;
   return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

CPathFinder::_NODE* CPathFinder::ReturnBestNode(void)
{
	_NODE *tmp;

	if ( OPEN->NextNode == NULL )
	{
		return NULL;
	}

	// Pick Node with lowest f, in this case it's the first node in list
	// because we sort the OPEN list wrt lowest f. Call it BESTNODE.

	tmp = OPEN->NextNode;   // point to first node on OPEN
	OPEN->NextNode = tmp->NextNode;    // Make OPEN point to nextnode or NULL.

	// Next take BESTNODE (or temp in this case) and put it on CLOSED

	tmp->NextNode = CLOSED->NextNode;
	CLOSED->NextNode = tmp;

	return(tmp);
}

////////////////////////////////////////////////////////////////////////////////

void CPathFinder::GenerateSuccessors(CPathFinder::_NODE *BestNode, int dx, int dy)
{
   int x, y;

	// Upper-Left
	if ( GetTile(x=BestNode->x-TILE_SIZE, y=BestNode->y-TILE_SIZE) == 0) GenerateSucc(BestNode,x,y,dx,dy);
	// Upper
	if ( GetTile(x=BestNode->x, y=BestNode->y-TILE_SIZE) == 0) GenerateSucc(BestNode,x,y,dx,dy);
	// Upper-Right
	if ( GetTile(x=BestNode->x+TILE_SIZE, y=BestNode->y-TILE_SIZE) == 0) GenerateSucc(BestNode,x,y,dx,dy);
	// Right
	if ( GetTile(x=BestNode->x+TILE_SIZE, y=BestNode->y) == 0) GenerateSucc(BestNode,x,y,dx,dy);
	// Lower-Right
	if ( GetTile(x=BestNode->x+TILE_SIZE, y=BestNode->y+TILE_SIZE) == 0) GenerateSucc(BestNode,x,y,dx,dy);
	// Lower
	if ( GetTile(x=BestNode->x, y=BestNode->y+TILE_SIZE) == 0) GenerateSucc(BestNode,x,y,dx,dy);
	// Lower-Left
	if ( GetTile(x=BestNode->x-TILE_SIZE, y=BestNode->y+TILE_SIZE) == 0) GenerateSucc(BestNode,x,y,dx,dy);
	// Left
	if ( GetTile(x=BestNode->x-TILE_SIZE, y=BestNode->y) == 0) GenerateSucc(BestNode,x,y,dx,dy);
}

////////////////////////////////////////////////////////////////////////////////

void CPathFinder::GenerateSucc(CPathFinder::_NODE *BestNode,int x, int y, int dx, int dy)
{
	int g, TileNumS, c = 0;
	_NODE *Old, *Successor;

	g = BestNode->g+1;	    // g(Successor)=g(BestNode)+cost of getting from BestNode to Successor
	TileNumS = TileNum(x,y);  // identification purposes

	if ( (Old=CheckOPEN(TileNumS)) != NULL ) // if equal to NULL then not in OPEN list, else it returns the Node in Old
	{
		for( c = 0; c < 8; c++)
	   	if( BestNode->Child[c] == NULL ) // Add Old to the list of BestNode's Children (or Successors).
	   		break;
		BestNode->Child[c] = Old;

		if ( g < Old->g )  // if our new g value is < Old's then reset Old's parent to point to BestNode
		{
			Old->Parent = BestNode;
	   	Old->g = g;
	   	Old->f = g + Old->h;
		}
	}
	else if ( (Old=CheckCLOSED(TileNumS)) != NULL ) // if equal to NULL then not in OPEN list, else it returns the Node in Old
	{
		for( c = 0; c< 8; c++)
		if ( BestNode->Child[c] == NULL ) // Add Old to the list of BestNode's Children (or Successors).
	   		break;
		BestNode->Child[c] = Old;

		if ( g < Old->g )  // if our new g value is < Old's then reset Old's parent to point to BestNode
		{
	   	    Old->Parent = BestNode;
	   	    Old->g = g;
	   	    Old->f = g + Old->h;
	   	    PropagateDown(Old);  // Since we changed the g value of Old, we need
					   	        // to propagate this new value downwards, i.e.
					 	          // do a Depth-First traversal of the tree!
		}
	}
	else
	{
		Successor = new _NODE; memset(Successor, 0, sizeof(_NODE));
		Successor->Parent = BestNode;
		Successor->g = g;
		Successor->h = (x-dx)*(x-dx) + (y-dy)*(y-dy);  // should do sqrt(), but since we don't really
		Successor->f = g+Successor->h;     // care about the distance but just which branch looks
		Successor->x = x;                 // better this should suffice. Anyayz it's faster.
		Successor->y = y;
		Successor->NodeNum = TileNumS;
		Insert(Successor);     // Insert Successor on OPEN list wrt f
		for( c =0; c < 8; c++)
			if ( BestNode->Child[c] == NULL ) // Add Old to the list of BestNode's Children (or Successors).
	  		    break;
		BestNode->Child[c] = Successor;
	}
}

////////////////////////////////////////////////////////////////////////////////

CPathFinder::_NODE *CPathFinder::CheckOPEN(int tilenum)
{
	_NODE *tmp;

	DWORD count = 0; // zebra
	tmp = OPEN->NextNode;
	while ( tmp != NULL )
	{
		count++; // zebra
		if(count >= MAX_PATH_CHECK_COUNT)
			return NULL; // zebra;
		if ( tmp->NodeNum == tilenum )
		return (tmp);
		else
	  		tmp = tmp->NextNode;
	}
	return(NULL);
}

////////////////////////////////////////////////////////////////////////////////

CPathFinder::_NODE *CPathFinder::CheckCLOSED(int tilenum)
{
	_NODE *tmp;

	tmp = CLOSED->NextNode;

	DWORD count = 0;
	while ( tmp != NULL )
	{
		count++; // zebra
		if(count >= MAX_PATH_CHECK_COUNT)
			return NULL; // zebra;
		if ( tmp->NodeNum == tilenum )
		return(tmp);
		else
		tmp = tmp->NextNode;
	}
	return(NULL);
}

////////////////////////////////////////////////////////////////////////////////

void CPathFinder::Insert(CPathFinder::_NODE *Successor)
{
   _NODE *tmp1, *tmp2;
   int f;

   if ( OPEN->NextNode == NULL )
   {
		OPEN->NextNode = Successor;
		return;
   }

       // insert into OPEN successor wrt f
   f = Successor->f;
   tmp1 = OPEN;
   tmp2 = OPEN->NextNode;

   while ( (tmp2 != NULL) && (tmp2->f < f) )
   {
   	 tmp1 = tmp2;
       tmp2 = tmp2->NextNode;
   }

   Successor->NextNode = tmp2;
   tmp1->NextNode = Successor;
}

////////////////////////////////////////////////////////////////////////////////

void CPathFinder::PropagateDown(CPathFinder::_NODE *Old)
{
   int c, g;
   _NODE *Child, *Father;

   g = Old->g;            // alias.
   for ( c = 0; c < 8; c++)
   {
   	if ( (Child=Old->Child[c]) == NULL )   // create alias for faster access.
          break;
		if ( g+1 < Child->g)
		{
	   	 Child->g = g+1;
	   	 Child->f = Child->g + Child->h;
	  	    Child->Parent = Old;        // reset parent to new path.
	       Push(Child);                 // Now the Child's branch need to be
		}								// checked out. Remember the new cost must be propagated down.
   }

   while ( Stack->NextStackPtr != NULL )
   {
		Father = Pop();
		for (c = 0; c<8; c++)
		{
	   	if ( (Child=Father->Child[c]) == NULL )  // we may stop the propagation 2 ways: either
	  			break;
	      if ( Father->g+1 < Child->g ) // there are no children, or that the g value of
	  		{                          // the child is equal or better than the cost we're propagating
	      	Child->g = Father->g+1;
	    		Child->f = Child->g+Child->h;
	    		Child->Parent = Father;
	    		Push(Child);
	      }
	   }
   }
}


////////////////////////////////////////////////////////////////////////////////
//                              STACK FUNCTIONS                               //
////////////////////////////////////////////////////////////////////////////////

void CPathFinder::Push(CPathFinder::_NODE *Node)
{
   _STACK *tmp;

   tmp = new _STACK; memset(tmp, 0, sizeof(_STACK));
   tmp->NodePtr = Node;
   tmp->NextStackPtr = Stack->NextStackPtr;
   Stack->NextStackPtr = tmp;
}

////////////////////////////////////////////////////////////////////////////////

CPathFinder::_NODE *CPathFinder::Pop(void)
{
	_NODE *tmp;
	_STACK *tmpSTK;

	tmpSTK = Stack->NextStackPtr;
	tmp = tmpSTK->NodePtr;

	Stack->NextStackPtr = tmpSTK->NextStackPtr;
	delete tmpSTK; tmpSTK = NULL;
	return(tmp);
}

