// MagicItem.h: interface for the CMagicItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MagicItem_H__870BC321_DF02_11D3_B175_00105A735718__INCLUDED_)
#define AFX_MagicItem_H__870BC321_DF02_11D3_B175_00105A735718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"

class CMagicItem  : public CItem
{
public:
	LPDIRECTDRAWSURFACE7	m_lpSurf2;
	CDDSurface*  m_lpSurface2;
	_SPR_NORMAL  *m_SprData2;
	int m_ShopSelect;
	int m_UserItem[3];
	void ViewSelectBox(CPoint pos, int collar );
	void ShopMeItemDown();
	void ShopMeItemUp();
	void ShopItemDown();
	void ShopItemUp();
	int m_UpDown;// 인벤토리 스롤바
	int m_UpDown_Shop;//상점 스크롤바
	int m_UpDown_Shopme;//상점 나의 스크롤바

	void Load_image2(LPCSTR szFileName);
	void LoadSpr2(LPCSTR szFileName);
	void LoadObm2(LPCTSTR szFileName);
	void InvItemDown();
	void InvItemUp();
	void InvWindowDraw(CDDSurface* pDrawSurface);
	void ShopMeWindowDraw(CDDSurface* pDrawSurface);
	void ShopWindowDraw(CDDSurface* pDrawSurface);
	int m_Caption;
	void ViewCaption(CPoint pos, CString str, int tmp_y = 0);
	virtual BOOL MouseProcess(BBDX_MouseInformation* pMouseInfo, int Dlg_Pos = 0, int Select_Box = 0, int MoveFlag = 1);
	virtual void Draw(CDDSurface* pDestSurf, int count=0, int tmp_y=0, int index=0);
	virtual void DrawShop(int x, int y, CDDSurface* pDestSurf, int count=0, int index=0);
	void InitItem(CDirectDraw *pDirectDraw);
	CMagicItem();
	virtual ~CMagicItem();


};

#endif // !defined(AFX_MagicItem_H__870BC321_DF02_11D3_B175_00105A735718__INCLUDED_)
