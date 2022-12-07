// SpriteUnit.h: interface for the CSpriteUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITEUNIT_H__050A2F65_14EB_11D3_8422_00105A6B97E2__INCLUDED_)
#define AFX_SPRITEUNIT_H__050A2F65_14EB_11D3_8422_00105A6B97E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SM_MapManager;
class CSprite;
class CDDSurface;

#define NORMAL_SPRITE		0X00
#define	INNER_HOUSE_SPRITE	0X01
#define DOOR_HOUSE_SPRITE	0X02

class CSpriteUnit  
{
public:
	BOOL				IsPosBehindSprite2(int x, int y);
	CRect&				GetSpriteRect() {return m_rectSprite;}
//	int					GetSpriteX_Left(CRect r) {return m_rectSprite.left - r.left;}
	int					GetSpriteWide(CRect r) {return (m_rectSprite.right - r.left)-(m_rectSprite.left - r.left);}
	int					GetSpriteY(CRect r) {return m_rectSprite.bottom - r.top;}
	void				RotateAnimation();
	void				Draw(CRect r, CPoint MapStartPos, CPoint MapOffset, CDDSurface* destSurface, BOOL bTrans=FALSE);
	BOOL				Load(CArchive &ar, int indx_sprite=0);
	BOOL				IsPosBehindSprite(int x, int y);
    int                 GetSpriteSky() {return m_Sky;}
	CSpriteUnit();
	virtual ~CSpriteUnit();

	static	SM_MapManager*	m_bpMapManager;

protected:
	CRect			m_rectSprite;		// Screen ÁÂÇ¥, m_Pos, m_sizeSprite·Î ÀÌ·ç¾îÁü
	CSize			m_sizeSprite;
	CPoint			m_Pos;				// Screen ÁÂÇ¥
	CPoint			m_CellPos;			// Cell ÁÂÇ¥
	CSprite*		m_bpSprite;
	int				m_iAnimationOrder;
public:
	int m_House_In;
	int m_HouseIndex;
	short           m_House_Number;
	BYTE            m_House_Spec;
	int             m_Alpha_True;
	int             m_Sky;
};

#endif // !defined(AFX_SPRITEUNIT_H__050A2F65_14EB_11D3_8422_00105A6B97E2__INCLUDED_)
