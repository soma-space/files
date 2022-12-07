// DxCursor.h: interface for the CDxCursor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXCURSOR_H__62502769_3A93_11D3_8422_00105A6B97E2__INCLUDED_)
#define AFX_DXCURSOR_H__62502769_3A93_11D3_8422_00105A6B97E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _SPR_DATA
#define _SPR_DATA
class SPR_DATA
{
public:
	_ANI_HEADER		m_HAni;
	_ANI_DATA		m_AniData;
	_SPR_HEADER		m_HSpr;
	_SPR_LSP		*m_arrSprData;
	WORD			**m_arrVData;

public:
	SPR_DATA();
	~SPR_DATA();
};
#endif

class CDDSurface;

#define CURSOR_COUNT		3

#define NORMAL_CURSOR		0
#define ATTACK_CURSOR		1
#define PICKUP_CURSOR		2

class CDxCursor  
{
public:
	void				InitCursor();
	void				Draw(CDDSurface *pSurface, CPoint ptPos);			//커서 그리기
	void				SetCursorIndex(int index);	//커서 교체
	void				RemoveAllData();
	void				BltLSPNormal(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data);

	CDxCursor();
	virtual ~CDxCursor();
protected:
	SPR_DATA			*m_arrCursor[CURSOR_COUNT];	//Cursor그림의 보관장소(Surface로 관리)
	int					m_iCursorIndex;		//사용하는 Cursor의 Index
	int					m_iSpriteIndex;
	DWORD				m_dwTick;
	CPoint				m_ptHotSpot;		//32X32크기의 Cursor그림에서의 HotSpot
};

#endif // !defined(AFX_DXCURSOR_H__62502769_3A93_11D3_8422_00105A6B97E2__INCLUDED_)
