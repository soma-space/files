// Dialog_Login.h: interface for the CDialog_Login class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_LOGIN_H__34C32822_0A30_11D4_B175_00105A735718__INCLUDED_)
#define AFX_DIALOG_LOGIN_H__34C32822_0A30_11D4_B175_00105A735718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "Dialog_Box.h"

#include "UIRes.h"
//우석.
//#include "SParticle.h"

#include "SWater.h"


class CItem_Inv;
class CChildView;
class BBDX_Editor;
//class BBDX_TextOuter;
//class CTWFont;


//우석.
class CDialog_Login  :  public CUIRes
{
public:
	CDialog_Login(CChildView *child_view);
	virtual ~CDialog_Login();

//우석.

protected:
	CString         m_ErrorText; // 에러 메세지.
	CRect			m_vMsgRect;
	bool            m_bCharExplain;
	int             m_CharExplainNum;

	void SetCharData(CItem_Inv *pItem,int i);
	CRect GetTextOutRect(int index);
	void  DrawCharData(CDDSurface* pDestSurf, int i);


public:
	int			m_vOk, m_vCurOk;
	int			m_Dialog_Login_Del;
	int			m_bChangingChar;
	int			m_vRotateNum;
	CPoint		m_vPos[3];
	CItem_Inv  *m_pItem1, *m_pItem2, *m_pItem3;
	CChildView *m_pChildView;
	int			m_vViewMode;
	BOOL		m_bSex;
	BOOL        m_bpreSex;
	BOOL		m_bDelCheck;

	BBDX_Editor		*m_pName;
//	BBDX_TextOuter	*m_pDataOut[17];
//	CTWFont			*m_pTWFont;
	CString			m_DataOut[17];
	CFont		*m_pFont;
	CFont       *m_pCharExplainFont;

	CPoint			 m_vNewCharPos, m_vCharInfoPos;

	//우석
	bool	m_IsMovePixel;
	POINT	m_ptSrcChar1;
	POINT	m_ptSrcChar2;
	POINT	m_ptSrcChar3;
	POINT	m_ptMakeChar1;
	POINT   m_ptMakeChar2;

	SWater	m_water;
	CPoint  m_ClickPt;

	//새캐릭터 만들기에 쓸거....
	CItem_Inv *m_pNewCharItem01;
	CItem_Inv *m_pNewCharItem02;
	CItem_Inv *m_pNewCharItem03;
	CItem_Inv *m_pNewCharItemWoman01;
	CItem_Inv *m_pNewCharItemWoman02;
	CItem_Inv *m_pNewCharItemWoman03;

	int			m_SkinColorChange;
	int			m_NextSkinColorChange;
	bool		m_bSkinColorChangeEnd;
	int			m_DrawMode; //0:캐릭터 선택. 1:피부색 선택.

	//캐릭터의 능력치....
	int     m_Strength;
	int     m_Dexterity;
	int     m_Inteligence;
	int     m_Charisma;
	int     m_Wisdom;
	int     m_Constitution;
	int     m_Points;
	DWORD    m_frameRate;

	int		m_HairColorIndex;
	int		m_HairStyleIndex;

	//우석 3D..

	void HairColorChange(int change);
	void HairStyleChange(int change);
	void SetMakeCharData(int num);

	int     m_ViewCharNum;

//	void SetMovePixel();
//	void initPixelData();
	void MovePixelDraw(CDDSurface* pDestSurf);
	bool LoginCharData(int dir, int front_num, float time_gap);
	bool SDrawLine(long &x_start, long &y_start, long x_end, long y_end, int gap_step, float time_gap);
	bool ChangeVerticesColor(int Sex, int skip_color,  CItem_Inv *ManChar, CItem_Inv *WomanChar);
	//캐릭터의 이동....
	void SetCharNum();

	void SkinChange(int change);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw( CDDSurface* pDestSurf );
	BOOL MouseProcess(BBDX_MouseInformation *pMouseInfo);
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);

	//우석.

//	void Draw( CDDSurface *pDestSurf );
	void LoginDraw( CDDSurface* pDestSurf );
	void NewCharDraw( CDDSurface *pDestSurf );
	void MakeCharData(CDDSurface* pDestSurf);

	void LoginMouseProcess( BBDX_MouseInformation *pMouseInfo );

	void SetMode( int mode );
	void InitLogin();

	void InitNewChar();
	void InitData( CDirectDraw *pDirectDraw, CChildView *child_view );
	void ChangeCharacter( int rotate );
	void SetCharacter( CItem_Inv *item1, CItem_Inv *item2, CItem_Inv *item3 );

	void SetMessage( CString str );
	void DrawErrorText(CDDSurface *pDestSurf);

	void DrawNewCharExplain();
	void SetCharTypeData(int chartype);
	void ResetData();
};

#endif // !defined(AFX_DIALOG_LOGIN_H__34C32822_0A30_11D4_B175_00105A735718__INCLUDED_)
