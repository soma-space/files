// SpriteUnit.cpp: implementation of the CSpriteUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SpriteUnit.h"
#include "SM_MapManager.h"
#include "SgrRes.h"
#include "Sprite.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
SM_MapManager* CSpriteUnit::m_bpMapManager = NULL;

CSpriteUnit::CSpriteUnit()
{
	m_bpSprite = NULL;
	m_iAnimationOrder = 0;
	m_Alpha_True = 0;
	m_House_In = 0;// 캐릭터가 집안으로 들어 오면..
	m_HouseIndex = -1;// 내가 들어간 집의 인덱스 번호
}

CSpriteUnit::~CSpriteUnit()
{

}

BOOL CSpriteUnit::Load(CArchive &ar, int indx_sprite)
{
	static	short	sizeSprite_cx;
	static	short	sizeSprite_cy;
	static	short	posPut_x; //Sprite의 X좌표는 32단위로 되어있음 (주의 : tile은 64단위임)
	static	short	posPut_y;
	static	short	idxSgr;
	static	short	idxSprite;
	static	short	House_number;

	int unique_number;
	CString sprite_name;

	BYTE  House_Spec;

	CString test;

	//로드후에는 모든 데이터가 Pixel단위로 됨

	ar >> sizeSprite_cx;
	ar >> sizeSprite_cy;
	ar >> posPut_x;
	ar >> posPut_y;
	ar >> idxSgr;
	ar >> unique_number;
	ar >> sprite_name;
	ar >> House_number;// 임시...막아 놓은것..새로운 맵 데이타때문에..
	ar >> House_Spec;

	idxSprite = 0;//(short) indx_sprite;
//////////////////////////////////////////////
//이름을 비교하여...스프라이트 인덱스 번호를 지정해준다...
//
	CSgrRes* testSgrRes;
	testSgrRes = m_bpMapManager->GetSgrRes(idxSgr);
//	TRACE("idxSgr ok! \n");

	int sprite_size = testSgrRes->GetApSprite().GetSize();
//	TRACE(" sprite_size = %d  \n", sprite_size);

	//int sprite_size = m_bpMapManager->GetSgrRes(idxSgr)->GetApSprite().GetSize();// 스프라이트 데이타..음냐..여기서 이름을 배교해서..전체가 몇개인지?
//	TRACE("어디서 스톱 되나? sprite_size = %d \n", sprite_size);

	for(int i=0; i < sprite_size; i++)
	{
		test = m_bpMapManager->GetSgrRes(idxSgr)->GetApSprite().GetAt(i)->m_SpriteName;
		if(!test.Compare((LPCTSTR)sprite_name))
		{
			idxSprite = i;
			break;
		}
	}
//////////////////////////////////////////////

	m_sizeSprite.cx = sizeSprite_cx;
	m_sizeSprite.cy = sizeSprite_cy;
	m_CellPos.x = posPut_x-1;
	m_CellPos.y = posPut_y-1;
	m_Pos.x = (posPut_x)*32;	//Sprite는 32X32단위로 찍혀있음
	m_Pos.y = (posPut_y)*16;

	m_House_Number = House_number;// 임시
	m_House_Spec = House_Spec;
	

	ASSERT(m_bpMapManager);
	m_bpSprite = m_bpMapManager->GetSgrRes(idxSgr)->GetApSprite().GetAt(idxSprite);

	//m_bpSprite->m_RectSprite;

	m_Sky = m_bpSprite->m_Sky;

	//m_rectSprite.SetRect(m_Pos.x, m_Pos.y, m_Pos.x+m_sizeSprite.cx, m_Pos.y+m_sizeSprite.cy);
	m_rectSprite.SetRect(m_Pos.x, m_Pos.y, m_Pos.x+m_sizeSprite.cx, m_Pos.y+m_bpSprite->m_RectSprite.bottom);

	return TRUE;
}

void CSpriteUnit::Draw(CRect r, CPoint MapStartPos, CPoint MapOffset, CDDSurface* pDestSurface, BOOL bTrans)
{
	if (!r.IntersectRect(&r, m_rectSprite)) return;
	// 나를 제외한 따른 사람은 투명하게 보이게 하기위해서...
//	if(bTrans)
//		if(m_Alpha_True == 1) bTrans = TRUE;// 이곳에서 조절..나만 투명한 오브젝트로 만들기 위해서..
//		else bTrans = FALSE;
	
	m_bpSprite->Draw(pDestSurface, m_Pos, MapStartPos, MapOffset, m_iAnimationOrder, bTrans);
}

void CSpriteUnit::RotateAnimation()
{
	m_iAnimationOrder ++;
	m_iAnimationOrder %= m_bpSprite->GetAnimationNum();
}

BOOL CSpriteUnit::IsPosBehindSprite(int x, int y)
{
	if(!m_bpSprite) return FALSE;

	int ab_posX, ab_posY;

	ab_posX = x - m_CellPos.x;
	ab_posY = y - m_CellPos.y;

	if(ab_posX < 0 || ab_posY < 0 || ab_posX >= m_bpSprite->m_vCellSize.cx || ab_posY >= m_bpSprite->m_vCellSize.cy) return FALSE;

	return m_bpSprite->GetHideCharacter(ab_posX, ab_posY);
}

BOOL CSpriteUnit::IsPosBehindSprite2(int x, int y)
{
	// 투명해지는 오브젝트 처리..
	if(!m_bpSprite) return FALSE;

	int pos_X, pos_Y;

	pos_X = (x+1)*32;
	pos_Y = (y+1)*16;

	int ab_posX, ab_posY;

	ab_posX = x - m_CellPos.x;
	ab_posY = y - m_CellPos.y;

	CRect rectIntersect = CRect(pos_X-0, pos_Y-30, pos_X+10, pos_Y+0);// 캐릭터 영역..
	//CRect rectIntersect = CRect(pos_Y-100, pos_Y+50, pos_X-50, pos_X+50);// 캐릭터 영역..
	CRect rSprite = GetSpriteRect();//스프라이트 영역..

//	if(ab_posX < 0 || ab_posY < 0 || ab_posX >= m_bpSprite->m_vCellSize.cx || ab_posY >= m_bpSprite->m_vCellSize.cy) 
//		return FALSE;

//	if(rSprite.top > rectIntersect.top)
	if (rSprite.IntersectRect(&rSprite, rectIntersect)) 
		return TRUE;//m_bpSprite->GetHideCharacter(ab_posX, ab_posY);
//	if (!r.IntersectRect(&r, rSprite)) return TRUE;
	return FALSE;
}
