// Tile.cpp: implementation of the CTile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tile.h"
#include "TileGroup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTile::CTile()
{
	m_bypBitmapInfo = NULL;
}

CTile::~CTile()
{
	RemoveAllData();
}

BOOL CTile::Load(CArchive &ar)
{
	RemoveAllData();
////////////////////////////////////
/*	short width;
	short height;
	short map_posx;
	short map_posy;
	short sgrindex;
	int   uniquenumber;
	CString SprName;
	
	ar >> width;
	ar >> height;
	ar >> map_posx;
	ar >> map_posy;
	ar >> sqrindex;
	ar >> uniquenumber;
	ar >> SprName;
*/
////////////////////////////////////////////////////////
	ar >> m_iType;		//tile의 type: type1, type2, type3

	int iStrLen;
	ar >> iStrLen;
	if (iStrLen)
	{
		ar.Read(m_szTileName.GetBufferSetLength(iStrLen), iStrLen);
		m_szTileName.ReleaseBuffer(iStrLen);
	}
////////////////////////////////////////////////////



////////////////////////////////////////////////////

	ar >> iStrLen;
	if (iStrLen)
	{
		ar.Read(m_szBaseTileName.GetBufferSetLength(iStrLen), iStrLen);
		m_szBaseTileName.ReleaseBuffer(iStrLen);
	}

	ar >> m_dwBitmapInfoSize;		//bitmapinfo의 크기
	m_bypBitmapInfo = new BYTE[m_dwBitmapInfoSize];	//bitmap info load할 메모리생성
	ar.Read(m_bypBitmapInfo, m_dwBitmapInfoSize);

	int			iGroupCount;
	CTileGroup*	pTileGroup = NULL;
	BOOL		bErrorOccur = FALSE;

	ar >> iGroupCount;			//그룹이 몇개?
	m_apTileGroup.SetSize(iGroupCount);
	for (int i=0; i<iGroupCount; i++)
	{
		pTileGroup = new CTileGroup;
		if (pTileGroup->Load(ar, m_bypBitmapInfo, m_dwBitmapInfoSize))
		{
			m_apTileGroup.SetAt(i, pTileGroup);
		}
		else 
		{
			delete pTileGroup;
			bErrorOccur = TRUE;
		}
	}
////////////////////////////////////
/*	ar >> m_iType;		//tile의 type: type1, type2, type3

	int iStrLen;
	ar >> iStrLen;
	if (iStrLen)
	{
		ar.Read(m_szTileName.GetBufferSetLength(iStrLen), iStrLen);
		m_szTileName.ReleaseBuffer(iStrLen);
	}

	ar >> iStrLen;
	if (iStrLen)
	{
		ar.Read(m_szBaseTileName.GetBufferSetLength(iStrLen), iStrLen);
		m_szBaseTileName.ReleaseBuffer(iStrLen);
	}

	ar >> m_dwBitmapInfoSize;		//bitmapinfo의 크기
	m_bypBitmapInfo = new BYTE[m_dwBitmapInfoSize];	//bitmap info load할 메모리생성
	ar.Read(m_bypBitmapInfo, m_dwBitmapInfoSize);

	int			iGroupCount;
	CTileGroup*	pTileGroup = NULL;
	BOOL		bErrorOccur = FALSE;

	ar >> iGroupCount;			//그룹이 몇개?
	m_apTileGroup.SetSize(iGroupCount);
	for (int i=0; i<iGroupCount; i++)
	{
		pTileGroup = new CTileGroup;
		if (pTileGroup->Load(ar, m_bypBitmapInfo, m_dwBitmapInfoSize))
		{
			m_apTileGroup.SetAt(i, pTileGroup);
		}
		else 
		{
			delete pTileGroup;
			bErrorOccur = TRUE;
		}
	}
*/
	return bErrorOccur? FALSE:TRUE;
}

void CTile::RemoveAllData()
{
	if (m_bypBitmapInfo) delete[] m_bypBitmapInfo, m_bypBitmapInfo = NULL;
	m_dwBitmapInfoSize = 0;
	m_szTileName.Empty();
	m_szBaseTileName.Empty();

	int iSize = m_apTileGroup.GetSize();
	for (int i=0; i<iSize; i++)
	{
		if (m_apTileGroup[i]) delete m_apTileGroup[i];
	}
	m_apTileGroup.RemoveAll();
}

void CTile::DarkenProcess(COLORREF rgb)
{
	int size = m_apTileGroup.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apTileGroup.GetAt(i))
			m_apTileGroup.GetAt(i)->DarkenProcess(rgb);
	}
}