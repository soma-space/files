// TileVary.cpp: implementation of the CTileVary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileVary.h"
#include "TilePiece.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTileVary::CTileVary()
{
	m_iLink = -1;
}

CTileVary::~CTileVary()
{
	RemoveAllData();
}

BOOL CTileVary::Load(CArchive &ar, int nPiece, BYTE *bypBitmapInfo, DWORD dwBitmapInfoSize)
{
	RemoveAllData();

	ar >> m_iLink;		//SgrEditor에 있는 정보인데 기억나지 않음 : SgrEditor참조
	m_apTilePiece.SetSize(nPiece);

	BOOL		bErrorOccur = FALSE;
	CTilePiece*	pTilePiece = NULL;

	for (int i=0; i<nPiece; i++)	//조각 수 만큼, 조각을 로드함
	{
		pTilePiece = new CTilePiece;
		if (pTilePiece->Load(ar, bypBitmapInfo, dwBitmapInfoSize))
		{
			m_apTilePiece.SetAt(i, pTilePiece);
		}
		else
		{
			delete pTilePiece;
			bErrorOccur = TRUE;
		}
	}

	return bErrorOccur ? FALSE:TRUE;
}

void CTileVary::RemoveAllData()
{
	int size = m_apTilePiece.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apTilePiece[i]) delete m_apTilePiece[i];
	}
	m_apTilePiece.RemoveAll();
}

void CTileVary::DarkenProcess(COLORREF rgb)
{
	int size = m_apTilePiece.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apTilePiece.GetAt(i))	
			m_apTilePiece.GetAt(i)->DarkenProcess(rgb);
	}
}