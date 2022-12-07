// Sprite.h: interface for the CSprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITE_H__A64CCB3C_1348_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_SPRITE_H__A64CCB3C_1348_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//CTilePiece에도 선언되어있음
class CTileSurface;
typedef CTypedPtrArray <CPtrArray, CTileSurface*> AP_TILESURFACE;
class CDDSurface;
typedef CTypedPtrArray <CPtrArray, CDDSurface*> AP_CDDSURFACE;

class SPRITECELL {
public:
	SPRITECELL() {
		m_bMovable = FALSE;
		m_bHideCharacter = FALSE;
		m_vLayer = 0;
	};

public:
	BOOL m_bMovable;				// 이동 가능 플래그(가능(TRUE), 불가능(FALSE))
	BOOL m_bHideCharacter;			// 캐릭터를 가리는 속성(캐릭터를 가린다(TRUE), 캐릭터가 보인다(FALSE))
	int m_vLayer;					// 층 번호(0(1층), 1(1-2층), 2(2층))
};

class CSprite  
{
public:
	BYTE m_vPatternCount;
	void			DarkenProcess(COLORREF rgb);
	int				GetAnimationNum(){return m_nAnimation;}
	CSize			GetSpriteSize(){return m_sizeSprite;}
	void			FillApCDDSurface();
	void			Draw(CDDSurface* pDestSurface, 
						CPoint pos, 
						CPoint MapStartPos, 
						CPoint MapOffset, 
						int iAnimationIndex, 
						BOOL bTrans=FALSE);
	void			LoadSpriteAttr(CArchive &ar);	//사용하지 않는 Sprite정보 Pass함 Load함수에서 사용
	void			RemoveAllData();
	BOOL			Load(CArchive &ar);	//정보 로드
	BOOL			GetMovable(int x, int y);
	int				GetLayer(int x, int y);
	BOOL			GetHideCharacter(int x, int y);

	CSprite();
	virtual ~CSprite();

	CSize			m_vCellSize;		// 전체 32x32 셀의 갯수
	CString         m_SpriteName;
	int             m_Sky;
	CRect           m_RectSprite;

protected:
	SPRITECELL		**m_pSpriteCell;	// 각 셀의 속성(2차원 배열로 구성되어 있음)
	AP_TILESURFACE	m_apTileSurface;	//데이터 보관장소 CTileSurface를 Ptr Array로 가짐
	AP_CDDSURFACE	m_apCDDSurface;		//빠른 접근을 위해서 (직접 CDDSurface접근)
	CSize			m_sizeSprite;		//Sprite의 크기 (pixel단위)
	int				m_nAnimation;		//Animation할 수 있는 갯수는 몇개?
};

#endif // !defined(AFX_SPRITE_H__A64CCB3C_1348_11D3_8422_0020AF9F40BD__INCLUDED_)
