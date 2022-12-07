// Item_Field.h: interface for the CItem_Field class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Item_Field_H__8A2074A6_981E_11D3_B175_00105A735718__INCLUDED_)
#define AFX_Item_Field_H__8A2074A6_981E_11D3_B175_00105A735718__INCLUDED_

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

class ITEM_PIECE
{
public:
	ITEM_PIECE() {};
	~ITEM_PIECE() {};

public:
	int		m_vPicNum;
	CString m_vName;
};

typedef CTypedPtrArray<CPtrArray, ITEM_PIECE *> CItemPieceArray;

class ITEM_FIELD
{
public:
	ITEM_FIELD();
	~ITEM_FIELD();

public:
	CPoint m_pos;
	CItemPieceArray		m_arrPiece;
	int m_vInHouse;
};

typedef CMap<int, int, ITEM_FIELD *, ITEM_FIELD *&> CItemFieldMap;

class CItem_Field  
{
public:
	CItem_Field();
	virtual ~CItem_Field();

	void RemoveAll();
public:
	void DrawLabel(CDDSurface *pDest, CPoint point, int in_house);
	void Draw(CDDSurface* pDestSurf,CPoint ViewPos, int in_house);
	void InitData(CDirectDraw *pDirectDraw);
	BOOL MouseProcess(CPoint mouse_pos);
	BOOL IsExistItem(CPoint cell_pos, int in_house, CPoint ViewPos,CDDSurface *pDestSurf );

public:
	BOOL ViewItem(int x, int y, int number, int spr_number, CDDSurface* pDestsurf, int ani_value=0);
	BOOL InMousePos(CPoint pos, CRect rect);
	void LoadSpr(LPCSTR	szFileName);
	void Load_image(LPCSTR	szFileName);
	void LoadObm(LPCTSTR szFileName);
	
public:
	void Del_F_item(CPoint pos, int picnum);
	void Add_F_item(CPoint pos, int picnum, CString t_name);
	CDirectDraw *m_pDirectDraw;// 가져와야 한다..
	CDDSurface*  m_lpSurface;

	_SPR_HEADER			m_HSpr;
	_SPR_NORMAL         *m_SprData;//100은 임시로 잡은값...내맘..

//	int m_Select_Item;
//	int m_Current_Item;
//	CPoint m_Old_Pos;
//	int m_Count;

	CItemFieldMap m_MapItemField;

//	BOOL m_Move_Flag[50];
//	CPoint m_Item_Field_Fixpos[50];
	
	int m_Item_Field_Flag;

public:	
	WORD m_colorkey;
	WORD m_linekey;
	CDDSurface*	 m_lpISurface;
	LPDIRECTDRAWSURFACE7 m_lpSurf;
	void SetPixelFormat(int nPixelFormat);
	void DrawFocusItem(int x, int y, int number, int spr_number, CDDSurface *pDestSurf, int ani_value);
	void GetClippedRect(RECT *pRC, RECT* pRCClip);
	inline int PutPixelLineAndItem( WORD* pDest, WORD* pSrc, int nCopyCount, WORD colorkey, WORD linekey );
	inline void PutPixelTopBottomLine( WORD* pDest, WORD* pSrc, int nCopyCount, WORD colorkey, WORD linekey );
};

inline int CItem_Field::PutPixelLineAndItem( WORD* pDest, WORD* pSrc, int nCopyCount, WORD colorkey, WORD linekey )
{
	__asm
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;
		mov edx, 0;
		mov ebx, 1;
		jmp COPY_LOOP;
NO_COPY:
		cmp edx, 0;
		jne PUTLINEKEY;
		add edi, 2;
		add esi, 2;
		inc ebx;
		dec ecx;
		jnz COPY_LOOP;
		jz END_LOOP;
PUTLINEKEY:
		mov eax, dword ptr linekey;
		mov [edi],eax;
		mov edx, 0;
		add edi, 2;
		add esi, 2;
		dec ecx;
		jnz	COPY_LOOP;
		jz END_LOOP;
PUTLINEKEY1:
		mov eax, dword ptr linekey;
		mov [edi],eax;
		mov edx, 1;
		add edi, 2;
		add esi, 2;
		dec ecx;
		jnz	COPY_LOOP;
		jz END_LOOP;
COPY_LOOP:
		mov eax, [esi];
		cmp ax, colorkey;
		je NO_COPY;
		cmp edx, 1;
		jne PUTLINEKEY1;
		mov [edi],eax;
		add edi, 2;
		add esi, 2;
		dec ecx;
		jnz COPY_LOOP;
END_LOOP:
		mov eax,ebx;
	}
	return TRUE;
}

inline void CItem_Field::PutPixelTopBottomLine( WORD* pDest, WORD* pSrc, int nCopyCount, WORD colorkey, WORD linekey )
{
	__asm
	{
		mov edi, pDest;
		mov esi, pSrc;
		mov ecx, nCopyCount;
		jmp COPY_LOOP;

PUT_LINE:
		mov eax, dword ptr linekey;
		mov [edi],eax;
		add edi, 2;
		add esi, 2;
		dec ecx;
		jnz	COPY_LOOP;
		jz END_LOOP;

COPY_LOOP:
		mov eax, [esi];
		cmp ax, colorkey;
		jne PUT_LINE;
		add edi, 2;
		add esi, 2;
		dec ecx;
		jnz COPY_LOOP;
END_LOOP:
	}
}

#endif // !defined(AFX_Item_Field_H__8A2074A6_981E_11D3_B175_00105A735718__INCLUDED_)
