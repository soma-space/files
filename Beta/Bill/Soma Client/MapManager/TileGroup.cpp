// TileGroup.cpp: implementation of the CTileGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileGroup.h"
#include "TileVary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTileGroup::CTileGroup()
{
	m_nPiece = 0;
}

CTileGroup::~CTileGroup()
{
	RemoveAllData();
}

BOOL CTileGroup::Load(CArchive &ar, BYTE *bypBitmapInfo, DWORD dwBitmapInfoSize)
{
	RemoveAllData();

	ar >> m_nPiece;			//조각이 몇개?

	int			nVary;
	CTileVary	*pTileVary = NULL;
	BOOL		bErrorOccur = FALSE;

	ar >> nVary;			//변화가 몇개?
	m_apTileVary.SetSize(nVary);

	for (int i=0; i<nVary; i++)
	{
		pTileVary = new CTileVary;	
		if (pTileVary->Load(ar, m_nPiece, bypBitmapInfo, dwBitmapInfoSize))
		{
			m_apTileVary.SetAt(i, pTileVary);
		}
		else
		{
			delete pTileVary;
			bErrorOccur = TRUE;
		}
	}

	return bErrorOccur ? FALSE:TRUE;
}

void CTileGroup::RemoveAllData()
{
	int size = m_apTileVary.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apTileVary[i]) delete m_apTileVary[i];
	}
	m_apTileVary.RemoveAll();
}

void CTileGroup::DarkenProcess(COLORREF rgb)
{
	int size = m_apTileVary.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apTileVary.GetAt(i))
			m_apTileVary.GetAt(i)->DarkenProcess(rgb);
	}
}