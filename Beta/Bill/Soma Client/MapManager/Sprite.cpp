// Sprite.cpp: implementation of the CSprite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sprite.h"
#include "TileSurface.h"
#include "../DManage/BB_Direct.h"
#include "../DManage/AlphaProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSprite::CSprite()
{
	m_vCellSize.cx = m_vCellSize.cy = 0;
	m_sizeSprite.cx = 0;
	m_sizeSprite.cy = 0;
	m_nAnimation = 0;
	m_pSpriteCell = NULL;
}

CSprite::~CSprite()
{
	RemoveAllData();
}

BOOL CSprite::Load(CArchive &ar)
{
	RemoveAllData();

	LoadSpriteAttr(ar);
	//ar >> m_nAnimation;						//Animation 몇개?// 
	//m_apTileSurface.SetSize(m_nAnimation);

	/////////////////////광원정보..
	int count;
	ar >> count;
	if(count)
	{
		// 광원 정보를 읽는 부분 추가..지금은 없는거 같은데..



	}

////////////////////////////////////////////////////////////////
	// 나중에 물어 봐야지..

	CPoint *pDibPos;							// 각 DIB의 위치(여러 DIB로 묶여 있을 경우)
	CSize vDibCount;
	ar >> vDibCount;
	count = vDibCount.cx * vDibCount.cy;
	pDibPos = new CPoint[count];
	for(int i = 0; i < count; i++)
	{
		ar >> pDibPos[i];
	}
	delete[] pDibPos;
	/////////////////////////////////////////////////////////////


	ar >> m_nAnimation;						//Animation 몇개?// 
	m_apTileSurface.SetSize(m_nAnimation);
	///////////////////////////

	CTileSurface*	pTileSurface = NULL;
	BOOL			bErrorOccur = FALSE;
	for (i=0; i<m_nAnimation; i++)
	{
		pTileSurface = new CTileSurface;
		if (pTileSurface->Load(ar))
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
	if (!bErrorOccur)
	{
		m_sizeSprite.cx = m_apCDDSurface[0]->GetWidth();
		m_sizeSprite.cy = m_apCDDSurface[0]->GetHeight();
	}

	return bErrorOccur ? FALSE:TRUE;
}

void CSprite::RemoveAllData()
{
	if(m_pSpriteCell) {
		for(int i = 0; i < m_vCellSize.cx; i++) {
			delete[] m_pSpriteCell[i];
		}
		delete[] m_pSpriteCell;
		m_pSpriteCell = NULL;
	}
	m_vCellSize.cx = m_vCellSize.cy = 0;

	int size = m_apTileSurface.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apTileSurface[i]) delete m_apTileSurface[i];
	}
	m_apTileSurface.RemoveAll();

	m_apCDDSurface.RemoveAll();	//m_apCDDSurface는 실제 데이터 보관장소가 아니라 보관장소 Pointer만 가지고 있음(그러므로 메모리 삭제 과정 있으면 안됨)
}

void CSprite::LoadSpriteAttr(CArchive &ar)	//Load시 사용하지 않는 Sprite데이터 Pass함 나중에 사용할 것 생기면 이곳에도 빼야함
{	
	int iStrLen;
	ar >> iStrLen;									// 스프라이트 이름의 길이 Skip
//	BYTE *bypTmp = new BYTE[iStrLen];				// 스프라이트 이름 Skip
	LPTSTR buf = m_SpriteName.GetBuffer(iStrLen);
	ar.Read((void *)buf, iStrLen);
	m_SpriteName.ReleaseBuffer();
	
//	ar.Read((void *)buf, length);										// 스프라이트 이름
//	m_vName.ReleaseBuffer();

	int i;

	ar >> m_vCellSize.cx >> m_vCellSize.cy;
	if (m_vCellSize.cx==0&&m_vCellSize.cy==0) return;

	ar >> m_vPatternCount;						//Animation 몇개?// 밖에 있었는데 안으로 가지고 들어옴..

	if(m_vPatternCount)//에니메이션 패턴...
	{
		// 뭔가가 들어 가야 되는데..음냐..
		char start_frame;
		char end_frame;
		char direction;
		char repeat_count;
		for(i=0; i< m_vPatternCount;i++)
		{
			// 임시 나중에 수정해야된다..지금은 하나만 ㄷ르어오게되어있음..
			ar >> start_frame;
			ar >> end_frame;
			ar >> direction;
			ar >> repeat_count;
		}
	}

	m_pSpriteCell = new SPRITECELL *[m_vCellSize.cx];
	for(i = 0; i < m_vCellSize.cx; i++) {
		m_pSpriteCell[i] = new SPRITECELL[m_vCellSize.cy];
	}
	
	//////////////////////
	// 지붕때문에 나중에 추가할부분
	BYTE sky;
	ar >> sky;

	m_Sky = (int)sky;

	//l, t, r, b
	CRect SpriteRect;
	ar >> SpriteRect.left;
	ar >> SpriteRect.top;
	ar >> SpriteRect.right;
	ar >> SpriteRect.bottom;

	m_RectSprite = SpriteRect;

	int length;
	int x, y;
	CPoint p;

	ar.Read((void *)&length, sizeof(int));								// Hide Character Cell 갯수
	if(length > 0) {
		for(i = 0; i < length; i++) {
			ar.Read((void *)&(p.x), sizeof(int));						// Hide Character Cell의 X 좌표
			ar.Read((void *)&(p.y), sizeof(int));						// Hide Character Cell의 Y 좌표
			m_pSpriteCell[p.x][p.y].m_bHideCharacter = TRUE;
		}
	}

	for(y = 0; y < m_vCellSize.cy; y++) {								// 나머지 Cell의 정보
		for(x = 0; x < m_vCellSize.cx; x++) {
			ar.Read((void *)&(m_pSpriteCell[x][y].m_vLayer), sizeof(int));// 아래랑 위치를 바꿈..
			ar.Read((void *)&(m_pSpriteCell[x][y].m_bMovable), sizeof(BOOL));
			
		}
	}
}

void CSprite::Draw(CDDSurface* pDestSurface, CPoint pos, CPoint MapStartPos, CPoint MapOffset, int iAnimationIndex, BOOL bTrans)
{
	ASSERT(m_nAnimation>iAnimationIndex);

//	CRect sRect(0, 0, 640, 480);
	CRect sRect(0, 0, 800, 600);
	sRect.OffsetRect(MapStartPos.x*64+ MapOffset.x, MapStartPos.y*32+MapOffset.y);
	CRect sr(0, 0, m_sizeSprite.cx, m_sizeSprite.cy);
	CRect r = sr;
	r.OffsetRect(pos);
	r &= sRect;

	if (r.IsRectEmpty()) //그릴 자리가 없음
	{
		TRACE("보호코드 걸림: Error\n");
		return;
	}

	sr = r;
	sr.OffsetRect(-pos.x, -pos.y);
	r.OffsetRect(-MapStartPos.x*64-MapOffset.x, -MapStartPos.y*32-MapOffset.y);

	if (bTrans) 
	{
		//반투명 상태로 함(배경과 Sprite가 반반씩 섞임)
		CAlphaProcess::MixHalf(pDestSurface, r, m_apCDDSurface.GetAt(iAnimationIndex), sr);
	}
	else 
	{
		CPoint point;
		point.x = r.left;
		point.y = r.top;

		pDestSurface->BltFast(point, m_apCDDSurface.GetAt(iAnimationIndex), &sr, DDBLTFAST_SRCCOLORKEY);
		//pDestSurface->Blt(&r, m_apCDDSurface.GetAt(iAnimationIndex), &sr, DDBLT_KEYSRC|DDBLT_ASYNC);
	}
}

void CSprite::FillApCDDSurface()
{
	int size = m_apTileSurface.GetSize();
	m_apCDDSurface.SetSize(size);
	for (int i=0; i<size; i++)
	{
		m_apCDDSurface[i] = m_apTileSurface[i]->GetSurface(); //Pointer만 받아둠
	}
}

void CSprite::DarkenProcess(COLORREF rgb)
{
	int size = m_apTileSurface.GetSize();
	m_apCDDSurface.SetSize(size);
	for (int i=0; i<size; i++)
	{
		if (m_apCDDSurface.GetAt(i))
			m_apCDDSurface.GetAt(i)->DrawFogLayer(NULL, rgb, TRUE);
	}
}

BOOL CSprite::GetMovable(int x, int y)
{
	ASSERT(x >= 0 && x < m_vCellSize.cx);
	ASSERT(y >= 0 && y < m_vCellSize.cy);

	return m_pSpriteCell[x][y].m_bMovable;
}

int CSprite::GetLayer(int x, int y)
{
	ASSERT(x >= 0 && x < m_vCellSize.cx);
	ASSERT(y >= 0 && y < m_vCellSize.cy);

	return m_pSpriteCell[x][y].m_vLayer;
}

BOOL CSprite::GetHideCharacter(int x, int y)
{
	ASSERT(x >= 0 && x < m_vCellSize.cx);
	ASSERT(y >= 0 && y < m_vCellSize.cy);

	return m_pSpriteCell[x][y].m_bHideCharacter;
}
