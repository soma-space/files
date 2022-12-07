// CustomTileVary.cpp: implementation of the CCustomTileVary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomTileVary.h"
#include "TilePiece.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomTileVary::CCustomTileVary()
{
}

CCustomTileVary::~CCustomTileVary()
{
	RemoveAllData();
}

BOOL CCustomTileVary::Load(CArchive &ar, CSize dimTile, BYTE* bypBitmapInfo, DWORD dwBitmapInfoSize)
{
	RemoveAllData();
	m_appTilePiece.SetSize(dimTile.cx);
	AP_TILEPIECE*	papTilePiece = NULL;
	CTilePiece*		pTilePiece = NULL;
	BOOL			bErrorOccur = FALSE;
	for (int i=0; i<dimTile.cx; i++)
	{
		papTilePiece = new AP_TILEPIECE;
		papTilePiece->SetSize(dimTile.cy);
		for (int j=0; j<dimTile.cy; j++)
		{
			pTilePiece = new CTilePiece;
			if (pTilePiece->Load(ar, bypBitmapInfo, dwBitmapInfoSize))
			{
				papTilePiece->SetAt(j, pTilePiece);
			}
			else
			{
				delete pTilePiece;
				bErrorOccur = TRUE;
			}
		}
		m_appTilePiece.SetAt(i, papTilePiece);
	}
	return bErrorOccur ? FALSE:TRUE;
}

void CCustomTileVary::RemoveAllData()
{
	int size = m_appTilePiece.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_appTilePiece[i])
		{
			AP_TILEPIECE &apTilePiece = *m_appTilePiece[i];
			int apSize = apTilePiece.GetSize();
			for (int j=0; j<apSize; j++)
			{
				if (apTilePiece.GetAt(j)) delete apTilePiece.GetAt(j);
			}
			apTilePiece.RemoveAll();
			delete m_appTilePiece[i];
		}
	}
	m_appTilePiece.RemoveAll();
}

void CCustomTileVary::DarkenProcess(COLORREF rgb)
{
	int size = m_appTilePiece.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_appTilePiece[i])
		{
			AP_TILEPIECE &apTilePiece = *m_appTilePiece[i];
			int apSize = apTilePiece.GetSize();
			for (int j=0; j<apSize; j++)
			{
				if (apTilePiece.GetAt(j)) apTilePiece.GetAt(j)->DarkenProcess(rgb);
			}
		}
	}
}