// CustomTile.cpp: implementation of the CCustomTile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomTile.h"
#include "CustomTileVary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomTile::CCustomTile()
{
	m_bypBitmapInfo = NULL;
}

CCustomTile::~CCustomTile()
{
	RemoveAllData();
}

BOOL CCustomTile::Load(CArchive &ar)
{
	RemoveAllData();

	int iStrLen;
	ar >> iStrLen;
	if (iStrLen) 
	{
		ar.Read(m_szTileName.GetBufferSetLength(iStrLen), iStrLen);
		m_szTileName.ReleaseBuffer(iStrLen);
	}
	
	ar >> m_dimTile.cx >> m_dimTile.cy;
	
	ar >> iStrLen;
	if (iStrLen)
	{
		ar.Read(m_szBaseTileName.GetBufferSetLength(iStrLen), iStrLen);
		m_szBaseTileName.ReleaseBuffer(iStrLen);
	}

	ar >> m_dwBitmapInfoSize;
	m_bypBitmapInfo = new BYTE[m_dwBitmapInfoSize];
	ar.Read(m_bypBitmapInfo, m_dwBitmapInfoSize);

	int nVary;
	ar >> nVary;

	m_apCustomTileVary.SetSize(nVary);
	CCustomTileVary*	pCustomTileVary = NULL;
	BOOL				bErrorOccur = FALSE;
	for (int i=0; i<nVary; i++)
	{
		pCustomTileVary = new CCustomTileVary;
		if (pCustomTileVary->Load(ar, m_dimTile, m_bypBitmapInfo, m_dwBitmapInfoSize))
		{
			m_apCustomTileVary.SetAt(i, pCustomTileVary);
		}
		else
		{
			delete pCustomTileVary;
			bErrorOccur = TRUE;
		}

	}

	return bErrorOccur ? FALSE:TRUE;
}

void CCustomTile::RemoveAllData()
{
	m_szTileName.Empty();
	m_szBaseTileName.Empty();
	m_dimTile.cx = m_dimTile.cy = 0;
	if (m_bypBitmapInfo) delete m_bypBitmapInfo, m_bypBitmapInfo = NULL;
	m_dwBitmapInfoSize = 0L;

	int size = m_apCustomTileVary.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apCustomTileVary[i]) delete m_apCustomTileVary[i];
	}
	m_apCustomTileVary.RemoveAll();
}

void CCustomTile::DarkenProcess(COLORREF rgb)
{
	int size = m_apCustomTileVary.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apCustomTileVary[i]) 
			m_apCustomTileVary[i]->DarkenProcess(rgb);
	}
}