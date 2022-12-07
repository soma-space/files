// TilePiece.cpp: implementation of the CTilePiece class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TilePiece.h"
#include "../DManage/BB_Direct.h"
#include "TileSurface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTilePiece::CTilePiece()
{
	m_nAnimation = 0;
}

CTilePiece::~CTilePiece()
{
	RemoveAllData();
}

BOOL CTilePiece::Load(CArchive &ar, BYTE *bypBitmapInfo, DWORD dwBitmapInfoSize)
{
	RemoveAllData();

//	int DummyPass;
//	ar >> DummyPass >> DummyPass >> DummyPass >> DummyPass;//이동 정보와 레이어정보, 맵에 저장되어있으므로 필요없음.

	CTileSurface*	pTileSurface = NULL;
	BOOL			bErrorOccur = FALSE;

	ar >> m_nAnimation;	//Animation 몇개?
	m_apTileSurface.SetSize(m_nAnimation);

	for (int i=0; i<m_nAnimation; i++)	//Animation갯수 만큼 로드
	{
		pTileSurface = new CTileSurface;
		if (pTileSurface->Load(ar, bypBitmapInfo, dwBitmapInfoSize))
		{
			m_apTileSurface.SetAt(i, pTileSurface);
		}
		else
		{
			delete pTileSurface;
			bErrorOccur = TRUE;
		}
	}

	FillApCDDSurface();

	return bErrorOccur ? FALSE:TRUE;
}

void CTilePiece::RemoveAllData()
{
	int size = m_apTileSurface.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apTileSurface[i]) delete m_apTileSurface[i];
	}
	m_apTileSurface.RemoveAll();

	m_apCDDSurface.RemoveAll();
}

void CTilePiece::FillApCDDSurface()
{
	int size = m_apTileSurface.GetSize();
	m_apCDDSurface.SetSize(size);
	for (int i=0; i<size; i++)
	{
		m_apCDDSurface[i] = m_apTileSurface[i]->GetSurface();
	}
}

void CTilePiece::Draw(CDDSurface* pDestSurface, CPoint &pos, int iAnimationIndex)
{
	ASSERT(m_nAnimation>iAnimationIndex);

	//그릴 영역 계산{
	CRect r(pos, CSize(64, 32));
	CRect sr(0, 0, 64, 32);
	if (pos.x<0)
	{
		r.left = 0;
		sr.left += -pos.x;
		r.right = sr.Width();
	}
//	else if (r.right>640)
	else if (r.right>800)
	{
		r.right = 800;
//		r.right = 640;
		sr.right = r.Width();
	}
	if (pos.y<0)
	{
		r.top = 0;
		sr.top += -pos.y;
		r.bottom = sr.Height();
	}
//	else if (r.bottom>480)
	else if (r.bottom>600)
	{
		//r.bottom = 480;
		r.bottom = 600;
		sr.bottom = r.Height();
	}
	//그릴 영역 계산}

//	static CRect screenRect(0, 0, 640, 480);
//	CRect tmpR;
//	if (!tmpR.IntersectRect(&r, &screenRect)) return;
	//pDestSurface->Blt(&r, m_apCDDSurface.GetAt(iAnimationIndex), &sr, DDBLT_ASYNC);
	CPoint point;
	point.x = r.left;
	point.y = r.top;

	pDestSurface->BltFast(point, m_apCDDSurface.GetAt(iAnimationIndex), &sr, DDBLTFAST_SRCCOLORKEY);
}

void CTilePiece::DarkenProcess(COLORREF rgb)
{
	int size = m_apCDDSurface.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apCDDSurface.GetAt(i))
			m_apCDDSurface.GetAt(i)->DrawFogLayer(NULL, rgb, TRUE);
	}
}