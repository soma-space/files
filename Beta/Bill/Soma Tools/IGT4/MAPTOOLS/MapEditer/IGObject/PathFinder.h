#if !defined(AFX_ASTAR_H__42CDC7E1_CD91_11D2_B0A3_0060970DBAD5__INCLUDED_)
#define AFX_ASTAR_H__42CDC7E1_CD91_11D2_B0A3_0060970DBAD5__INCLUDED_

const int MAX_PATH_CHECK_COUNT = 1024;

class CPathFinder
{
private:
	typedef struct _NODE
	{     // node structure
		long f, h;
		int g, tmpg;
		int x, y;
		int NodeNum;
		_NODE *Parent;
		_NODE *Child[8]; // a node may have upto 8+(NULL) children.
		_NODE *NextNode;  // for filing purposes
	} NODE;
	
	struct _STACK // the stack structure
	{
		_NODE *NodePtr;
		_STACK *NextStackPtr;
	} STACK;

	_NODE*	OPEN;	// the node list pointers
	_NODE*	CLOSED;	
	_NODE*	PATH;	// 최단경로 pointer

	_STACK	*Stack;

	BOOL	isPath; // 올바른 경로일때만 참이다..  

	int		m_nWidth, m_nHeight; 			// 유효한 넓이 높이 (실제 맵은 상하좌우 1씩 크다..)
	int		m_nTileWidth, m_nTileHeight;	// 실제 타일 단위 넓이, 높이
	int		m_nPixelWidth, m_nPixelHeight; 	// 픽셀 단위 넓이 높이
	DWORD	m_dwTotalTiles; // 총 타일갯수.. - 단 주위에 벽이 둘러져 있다..
	int*	m_pTiles;				// 타일 맵 데이타 포인터

private:
	void	DeleteNodes(void);	// 모든 노드들을 삭제한다.
	int SetTile(int x, int y, int val) // 해당 픽셀 좌표의 타일값을 돌려준다.
	{
		if(x < 0 || y < 0 || x >= m_nPixelWidth || y >= m_nPixelHeight) return 0;
		int oldVal = m_pTiles[((y>>TILE_SHIFT)+1)*m_nTileWidth + (x>>TILE_SHIFT)+1];
		m_pTiles[((y>>TILE_SHIFT)+1)*m_nTileWidth + (x>>TILE_SHIFT)+1] = val;
		return oldVal;
	}

	// The A* Algorithm and it's stuff
	BOOL 	FindPath(int sx, int sy, int dx, int dy);
	_NODE*	ReturnBestNode(void);
	void	GenerateSuccessors(_NODE *BestNode, int dx, int dy);
	void	GenerateSucc(_NODE *BestNode,int x, int y, int dx, int dy);
	_NODE*	CheckOPEN(int tilenum);
	_NODE*	CheckCLOSED(int tilenum);
	void	Insert(_NODE *Successor);
	void	PropagateDown(_NODE *Old);
	void	Push(_NODE *Node); // stack functions
	_NODE*	Pop(void);

public:
	CPathFinder();
	~CPathFinder();
	
	int		TileNum(int x, int y) // 해당 타일 좌표의 몇번째 배열인지 돌려준다. // 주위에 1 두께의 장벽이 둘러져 있기 땜시 1 을 더해 준다..
	{
		if(x < 0 || y < 0 || x >= m_nPixelWidth || y >= m_nPixelHeight) return 0;
		return ((y>>TILE_SHIFT)+1)*m_nTileWidth + (x>>TILE_SHIFT)+1;
	}  

	int		GetTile(int x, int y) // 해당 픽셀 좌표의 타일값을 돌려준다.
	{
		if(x < 0 || y < 0 || x >= m_nPixelWidth || y >= m_nPixelHeight) return 0;
		return m_pTiles[((y>>TILE_SHIFT)+1)*m_nTileWidth + (x>>TILE_SHIFT)+1];
	} 

	void	Init(const int* pTiles, int width, int height); // 다른 맵형식을 넣을 수도 있다..
	BOOL	CalcNewPath(int sx, int sy, int dx, int dy);	// 새경로 계산.. - 경로가 없으면 거짓을 리턴

	BOOL	ReachedGoal(void); // 최종 목적지에 닿았는지 체크.. 길이 없어도 참을 리턴.
	POINT	GetNextWay(void); // 다음 길이 없다면 0,0 을 리턴..

	void SetCol(int x, int y, int nTile) // 해당 타일 좌표 충돌값을 세팅
	{
		if(x < 0 || y < 0 || x >= m_nWidth || y >= m_nHeight) return;
		m_pTiles[(y+1)*m_nTileWidth + (x+1)] |= nTile;
	}

	void ReleaseCol(int x, int y) // 해당 타일 좌표 충돌값 해제..
	{
		if(x < 0 || y < 0 || x >= m_nWidth || y >= m_nHeight) return;
		m_pTiles[(y+1)*m_nTileWidth + (x+1)] &= 1;
	}

	void SetChrCol(int x, int y, int nSN) // zebra - 인수는 픽셀 단위이다.
	{
		int x2 = (x>>TILE_SHIFT), y2 = (y>>TILE_SHIFT);
		if(x2 < 0 || y2 < 0 || x2 > m_nWidth || y2 > m_nHeight) return;
		x2+=2; y2+=2;
		for(y = y2-2; y <= y2; y++)
		{
			for(x = x2-2; x <= x2; x++)
			{
				if(!m_pTiles[y*m_nTileWidth + x])
					m_pTiles[y*m_nTileWidth + x] |= nSN; // 캐릭터 충돌은 2 를 or 연산한다.
			}
		}
	}

	void ReleaseChrCol(int x, int y, int nSN) // zebra - 인수는 픽셀 단위이다.
	{
		int x2 = (x>>TILE_SHIFT), y2 = (y>>TILE_SHIFT);
		if(x2 < 0 || y2 < 0 || x2 > m_nWidth || y2 > m_nHeight) return;
		x2+=2; y2+=2;
		for(y = y2-2; y <= y2; y++)
		{
			for(x = x2-2; x <= x2; x++)
			{
				if((m_pTiles[y*m_nTileWidth + x] & nSN) == nSN)
					m_pTiles[y*m_nTileWidth + x] &= 1; // 캐릭터 충돌은 2 를 or 연산한다.
			}
		}
	}

	int Get(int x, int y) // zebra - 타일 단위 좌표에서 값을 구한다..
	{
		if(x < 0 || y < 0 || x >= m_nWidth || y >= m_nHeight) return 0;
		return m_pTiles[(y+1)*m_nTileWidth + (x+1)];
	}
};

#endif // ASTAR_H

