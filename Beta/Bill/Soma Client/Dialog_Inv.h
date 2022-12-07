// Dialog_Inv.h: interface for the CDialog_Inv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_INV_H__7DC42601_6D13_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_INV_H__7DC42601_6D13_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"
#include "DarkDef.h"
class BBDX_MouseInformation;
//class BBDX_TextOuter;
//class CTWFont;

#define ARM_COUNT		10

class CDialog_Inv : public CUIRes  
{
public:
	CDialog_Inv();
	virtual ~CDialog_Inv();

	void CallBack_MessageBox(int nId, int nValue);
	void SendDumpMoney(DWORD money);
	void ChangeItem(short fromindex, short toindex);
	void SendChangeItem(short index);
	void SendChangeItem(short from, short to);
	void SendThrowItem(short index, short count);
	void SendItemToBelt(short fromindex, short toindex);
	void RcvItemToBelt(PBYTE bypData);
	void SendUseItem(short index);

public:
	BOOL OnLButtonDblClk(BBDX_MouseInformation *pMouseInfo);
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	BOOL OnRButtonDown(BBDX_MouseInformation *pMouseInfo);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL DragDraw(CDDSurface *pDest);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void SetScrollData();

	void ShowWindow();
	void HideWindow();
	void HideAllWindow();
	void RefreshData();
	void DeleteContents();

	void DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index);
	void ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index);
	void TransParentItem(CDDSurface *pDestSur, CPoint ptDst, CRect rcSrc);
	static	void	PutPixelHalf(const WORD* pDest, const DWORD* pColor, int nCopyCount, DWORD dwHalf16Mask, DWORD dwHalf32Mask);

	DWORD m_TransParentColor;

//	CTWFont			*m_pTWFont;
//	BBDX_TextOuter	*m_pMoneyOuter;
	CUIScrollBar	*m_pScrollBar;
	CItemInvMap		m_mapItem;
	ITEM_INV		*m_pArmItem[ARM_COUNT];
	int				m_vCurEquipItem[ARM_COUNT];
	int				m_vCurArmItem, m_vCurInvItem;
	int				m_vDragItem;
	CPoint			m_vDragOffset;
	short			m_vFromItem, m_vToItem;
	BOOL			m_bChanging;
	CFont			*m_pFont;
	int				m_vTipIndex;
};

//inline void CUser::PutPixelHalf(const WORD* pDest, const WORD* pSrc, int nCopyCount, DWORD dwHalf16Mask, DWORD dwHalf32Mask) // nAlphaSrc - 0 이 젤 시커멓다.. 32 가 제일 밝다.
inline void CDialog_Inv::PutPixelHalf(const WORD* pDest, const DWORD* pColor, int nCopyCount, DWORD dwHalf16Mask, DWORD dwHalf32Mask) // nAlphaSrc - 0 이 젤 시커멓다.. 32 가 제일 밝다.
{
	__asm
	{
		mov edi, pDest;
//		mov esi, pSrc;
		mov esi, pColor;
		mov ecx, nCopyCount;

		// nCopyCount가 홀수인지 짝수인지를 판단. 
		mov ebx, ecx;
		and ebx, 1;
		jz	DWORD_HALF_MIX;

		//push ecx; // 카운트 저장..
	
		mov eax, [esi]; // Spr Pixel
		mov edx, [edi]; // Dest Spr Pixel
		and eax, dwHalf16Mask;
		and edx, dwHalf16Mask;
		shr eax, 1;
		shr edx, 1;
		add eax, edx;
		//and eax, wMask;
		mov [edi], ax;		// ax는 16bit Register.
		
		add edi, 2;
//		add esi, 2;
		//pop ecx; // 카운트 복구..
		dec ecx;
		jz  END_LOOP;

		// 여기서부터는 32bit(DWORD)계산.
DWORD_HALF_MIX: 
		shr ecx, 1;
		
COPY_LOOP:
		//push ecx; // 카운트 저장..
		//mov ebx, 0; // 최종 색깔 초기화
		
		mov eax, [esi]; // Spr Pixel
		//jecxz DONT_COPY; // 스프라이트 색깔이 0 이면... 처리 안함..
		mov edx, [edi]; // 
		and eax, dwHalf32Mask;
		and edx, dwHalf32Mask;
		shr eax, 1;
		shr edx, 1;
		add eax, edx;
		//and eax, dwMask;
		mov [edi], eax;

//DONT_COPY:
		add edi, 4;
//		add esi, 4;
		
		//pop ecx; // 카운트 복구..
		dec ecx;
		jnz COPY_LOOP;
END_LOOP:
	}
};

#endif // !defined(AFX_DIALOG_INV_H__7DC42601_6D13_11D4_98B0_00C02615866B__INCLUDED_)
