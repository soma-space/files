// Item.h: interface for the CItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Item_H__8A2074A6_981E_11D3_B175_00105A735718__INCLUDED_)
#define AFX_Item_H__8A2074A6_981E_11D3_B175_00105A735718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Soma.h"
#include "ChildView.h"
#include "DManage/BB_Direct.h"
#include "DManage/AlphaProcess.h"
#include "illstruct.h"
#include "DManage/AlphaProcess.h"
#include "DManage/BBDX_MouseInformation.h"

class CItem  
{
public:
	CItem();
	virtual ~CItem();

public:
	virtual void Draw(CDDSurface* pDestSurf, int count=0, int tmp_y=0, int index=0);
	virtual void Draw(int x, int y, CDDSurface* pDestSurf, int count, int index);
	virtual void InitData(CDirectDraw *pDirectDraw);
	virtual BOOL MouseProcess(BBDX_MouseInformation* pMouseInfo, int Dlg_Pos = 0);// 아이템을 집으면..참을 리턴하여..캐릭터가 다른 행동을 안하게 한다.

public:
	BOOL ViewItemBox(int x, int y, int number, int spr_number, CDDSurface* pDestsurf, int ani_value=0);
	BOOL InMousePos(CPoint pos, CRect rect);
	void LoadSpr(LPCSTR	szFileName);
	void Load_image(LPCSTR	szFileName);
	void LoadObm(LPCTSTR szFileName);

public:
	void SetInv_Data();
	int m_Select_Item;
	int m_Current_Item;
	CPoint m_Old_Pos;
	int m_Count;
	int  Output(int type);
	BOOL Input(int index, int type, int x=100, int y=100);

	int m_Item_Pic[50];
	CPoint m_Item_pos[50];
	CPoint m_Item_Fixpos[50];
	CString m_Item_Name[50];
	BOOL m_Move_Flag[100];
	CRect m_BoxRect[100];// 임의 로 50개로 잡았다.. 다이얼로그 박스 안에 들어가는 그림 기타등등..

	CDirectDraw *m_pDirectDraw;// 가져와야 한다..
	CDDSurface*  m_lpSurface;
	static DirectXMng		DirectXMng;
	_SPR_HEADER			m_HSpr;
	_SPR_NORMAL         *m_SprData;//100은 임시로 잡은값...내맘..
	LPDIRECTDRAWSURFACE7	m_lpSurf;
	
	int m_Item_Flag;

};

#endif // !defined(AFX_Item_H__8A2074A6_981E_11D3_B175_00105A735718__INCLUDED_)
