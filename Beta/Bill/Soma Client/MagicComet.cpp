// MagicComet.cpp: implementation of the CMagicComet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MagicComet.h"
#include "illstruct.h"
#include "ChildView.h"
#include "DManage/Direct3D.h"
#include "MagicRes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMagicManager* CMagicComet::pMagicManager;

CMagicComet::CMagicComet()
{
	m_nIndex = 0;
	m_ptSor = CPoint(0,0);
	m_ptDest = CPoint(0,0);
	m_nFrame = 0;
	m_nFrameInterval = 0;
	m_dwTime = 0;
	m_nDrawCount = 1;
}

CMagicComet::CMagicComet(CPoint ptSor, CPoint ptDest)
{
	m_nIndex = 0;
	m_ptSor = ptSor;
	m_ptDest = ptDest;
	m_nFrame = 0;
	m_nFrameInterval = 0;
	m_dwTime = 0;
	m_nDrawCount = 1;
}

CMagicComet::~CMagicComet()
{
	int nArrSize = m_arrRoutingCell.GetSize();
	ST_POSITION* pStPosInfo = NULL;
	for( int i = 0 ; i < nArrSize ; i++ )
	{
		pStPosInfo = m_arrRoutingCell.GetAt(i);
		if( pStPosInfo )
		{
			delete pStPosInfo;
			pStPosInfo = NULL;
		}

	}
	m_arrRoutingCell.RemoveAll(); 

}

BOOL CMagicComet::Load( int nMagicIndex )
{
	if (pMagicManager->LoadMagicRes(nMagicIndex))
	{
		m_nIndex = nMagicIndex;
	}
	else return FALSE;

	CString szTmpFileName;
	szTmpFileName.Format(".\\MagicRes\\mgr_%03d.mrf", nMagicIndex);
	//CChildView::DirectXMng.m_p3D->D3DCreateTextureEx( szTmpFileName.GetBuffer(0) );  
	CFile file;
	if (!file.Open(szTmpFileName.GetBuffer(0), CFile::modeRead))
	{
		TRACE("마법 Resource : %s Load실패\n", (LPCTSTR)szTmpFileName.GetBuffer(0));
		return FALSE;
	}
	m_strCometName = szTmpFileName;
	_SPR_HEADER stHSpr;
	UINT nReadCount;
	nReadCount = file.Read(&stHSpr, sizeof(_SPR_HEADER));
	m_nFrame = stHSpr.nQt; 
	m_nFrameInterval = m_nFrame/8;
	ASSERT(nReadCount==sizeof(_SPR_HEADER));
	file.Close();
	return TRUE;

}
BOOL CMagicComet::DrawComet()
{
	DWORD dwNowTime = GetTickCount();
	ST_POSITION* pStPosInfo;
	int num = 0;
	int nStartFrame;
	int nArrSize = m_arrRoutingCell.GetSize();
	if( nArrSize == 0 ) return TRUE;
	CMagicRes* pMagicRes = NULL;
	pMagicRes = pMagicManager->GetIndexedMagic(m_nIndex);		  //배열의 Res포인터를 얻어와서 
	if( pMagicRes == NULL ) return TRUE;

	int nLoopCount;
	if( nArrSize > m_nDrawCount ) nLoopCount = m_nDrawCount;
	else						  nLoopCount = nArrSize;
	for( int i =0 ; i < nLoopCount ; i++ )
	{
		pStPosInfo = m_arrRoutingCell.GetAt(i); 
		if( pStPosInfo == NULL ) continue;

		nStartFrame = DirStartFrame(pStPosInfo->m_nDir, m_nFrame);	//지정한 방향의 처음 Frame Index 구하기
		CPoint ptMapOffset = CChildView::MapManager.GetMapStartPos();

		pMagicRes->DrawMagic( CChildView::DirectXMng.m_pDrawSurface, pStPosInfo->m_ptPos.x - ptMapOffset.x, pStPosInfo->m_ptPos.y - ptMapOffset.y,
			pStPosInfo->m_nCurFrame + nStartFrame );//this_fly
		CPoint pt(pStPosInfo->m_ptPos.x - ptMapOffset.x,pStPosInfo->m_ptPos.y - ptMapOffset.y);
		LIGHT_INFO LI;
		LI.m_kind = 1;
		LI.m_pt = pt;
		CChildView::m_ptLightPos.Add(LI);    
//		CChildView::m_ptLightPos.Add(pt);    

//		CChildView::DirectXMng.m_p3D->D3DRenderEx( m_strCometName.GetBuffer(0), 
//			pStPosInfo->m_ptPos.x - ptMapOffset.x, pStPosInfo->m_ptPos.y - ptMapOffset.y, pStPosInfo->m_nCurFrame + nStartFrame);
		
		if( dwNowTime - m_dwTime > 50 )
		{
			pStPosInfo->m_nCurFrame++; 
			num++;
			if( pStPosInfo->m_nCurFrame >= m_nFrameInterval )
			{
				delete pStPosInfo; pStPosInfo = NULL;
				m_arrRoutingCell.RemoveAt(i);
				i--; nLoopCount--;

			}
		}
	}
	if( num )
	{
		m_dwTime = dwNowTime;
		m_nDrawCount++;
	}
	return FALSE;
}

int CMagicComet::SetRoute()
{
	m_arrRoutingCell.RemoveAll();
	//직선일 경우로 가정
	int nDir = 0;

	int deltax = m_ptDest.x - m_ptSor.x;
	int deltay = m_ptDest.y - m_ptSor.y;

	if (deltax==0) 
	{
		if (m_ptDest.y>m_ptSor.y) nDir = DIR_DOWN;
		else nDir = DIR_UP;
	}
	else
	{
		double lean = (double)abs(deltay)/abs(deltax);

		if (m_ptDest.y>m_ptSor.y) nDir |= DIR_DOWN;			//X,Y값에 따른 방향 설정
		else if (m_ptDest.y < m_ptSor.y) nDir |= DIR_UP;
		if (m_ptDest.x>m_ptSor.x) nDir |= DIR_RIGHT;
		else if (m_ptDest.x < m_ptSor.x) nDir |= DIR_LEFT;

		if (lean<.5) nDir &= ~(DIR_UP|DIR_DOWN);			//기울기로 판단한 8방향 재설정
		else if (lean>2.) nDir &= ~(DIR_LEFT|DIR_RIGHT);
	}

	int	totalDistance = abs(deltax) + abs(deltay);
	int nStep = int(totalDistance / 40 + .5);

	if (nStep==0) return nDir;

	ST_POSITION* pStPosInfo = NULL;

	for (int i=0; i<=nStep; i++)
	{
		pStPosInfo = new ST_POSITION;
		if( pStPosInfo == NULL ) continue;
		pStPosInfo->m_nDir = nDir;
		pStPosInfo->m_nCurFrame = 0;
		pStPosInfo->m_ptPos.x = m_ptSor.x + deltax *i/nStep;
		pStPosInfo->m_ptPos.y = m_ptSor.y + deltay *i/nStep;
		m_arrRoutingCell.Add(pStPosInfo);
//		m_arrRoutingCell.InsertAt(0,pStPosInfo);
	}
	return nDir;
}

/*
void CMagicComet::SetRoute()
{
	m_arrRoutingCell.RemoveAll();
	//직선일 경우로 가정
	int nDir = 0;

	int deltax = m_ptSor.x - m_ptDest.x;
	int deltay = m_ptSor.y - m_ptDest.y;

	if (deltax==0) 
	{
		if (m_ptSor.y>m_ptDest.y) nDir = DIR_DOWN;
		else nDir = DIR_UP;
	}
	else
	{
		double lean = (double)abs(deltay)/abs(deltax);

		if (m_ptSor.y>m_ptDest.y) nDir |= DIR_DOWN;			//X,Y값에 따른 방향 설정
		else if (m_ptSor.y < m_ptDest.y) nDir |= DIR_UP;
		if (m_ptSor.x>m_ptDest.x) nDir |= DIR_RIGHT;
		else if (m_ptSor.x < m_ptDest.x) nDir |= DIR_LEFT;

		if (lean<.5) nDir &= ~(DIR_UP|DIR_DOWN);			//기울기로 판단한 8방향 재설정
		else if (lean>2.) nDir &= ~(DIR_LEFT|DIR_RIGHT);
	}

	int	totalDistance = abs(deltax) + abs(deltay);
	int nStep = int(totalDistance / 80 + .5);

	if (nStep==0) return;

	ST_POSITION* pStPosInfo = NULL;

	for (int i=0; i<=nStep; i++)
	{
		pStPosInfo = new ST_POSITION;
		if( pStPosInfo == NULL ) continue;
		pStPosInfo->m_nDir = nDir;
		pStPosInfo->m_nCurFrame = 0;
		pStPosInfo->m_ptPos.x = m_ptSor.x + deltax *i/nStep;
		pStPosInfo->m_ptPos.y = m_ptSor.y + deltay *i/nStep;
		m_arrRoutingCell.Add(pStPosInfo);
		m_arrRoutingCell.InsertAt(0,pStPosInfo);
	}
}
*/
int CMagicComet::DirStartFrame(int nDir, int nResMaxFrame)	//지정한 방향의 처음 Frame Index 구하기
{
	ASSERT(nResMaxFrame%8==0);//8방향을 가진 객체이어야함
	int DirMaxFrame = nResMaxFrame/8;
	int nStartFrame = 0;
	switch(nDir)
	{
	case DIR_DOWN:				nStartFrame = 0;				break;
	case DIR_LEFT | DIR_DOWN:	nStartFrame = DirMaxFrame;		break;
	case DIR_LEFT:				nStartFrame = DirMaxFrame*2;	break;
	case DIR_LEFT | DIR_UP:		nStartFrame = DirMaxFrame*3;	break;
	case DIR_UP:				nStartFrame = DirMaxFrame*4;	break;
	case DIR_RIGHT | DIR_UP:	nStartFrame = DirMaxFrame*5;	break;
	case DIR_RIGHT:				nStartFrame = DirMaxFrame*6;	break;
	case DIR_RIGHT | DIR_DOWN:	nStartFrame = DirMaxFrame*7;	break;
	}
	return nStartFrame;
}
