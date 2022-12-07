// Dialog_Main.h: interface for the CDialog_Main class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_MAIN_H__53DA6201_6AB6_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_MAIN_H__53DA6201_6AB6_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_MouseInformation;
class CMyMagicData;

#define NORMAL_MODE		0
#define BATTLE_MODE		1
#define PK_MODE			2

class CDialog_Main : public CUIRes  
{
public:
	CDialog_Main();
	virtual ~CDialog_Main();

	void SendUseItem(short index);
	void SendThrowItem(short index, short count);
	void SendItemToInv(short fromindex, short toindex);
	void RcvItemToInv(PBYTE bypData);
public:
	BOOL OnLButtonDblClk(BBDX_MouseInformation *pMouseInfo);
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	BOOL OnRButtonDown(BBDX_MouseInformation *pMouseInfo);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	BOOL DragDraw(CDDSurface *pDest);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = TRUE, BOOL bMoveable = TRUE, BOOL bTrans = TRUE);

	void HideWindow();
	void InitData();
	void ResetStatus();

	void SetMagicData(int index, CMyMagicData *magic_data);
	void LoadMagicData(CMyMagicData *magic_item[]);

	void SetSelMagic(int index);
	void SetBattleMode(int b_mode);

	void DrawTipRect(CDDSurface *pDest, CRect& t_rect, CRect standard_rect, int area_index);
	void ViewCaption(CDDSurface *pDest, CRect standard_rect, CString str, int area_index);

	CMyMagicData *m_pMagicData[4];
	int m_vSelMagicIndex;
	int				m_vDragSpec; // 1 : 아이템 2 : 마법
	int				m_vDragItem;
	CPoint			m_vDragOffset;
	short			m_vFromItem, m_vToItem;
	BOOL			m_bChanging;
	int				m_bViewCaption;
	CString			m_vStrCaption;
	int				m_vLBtnDown;
	CFont			*m_pFont;
	double			m_vShowHP, m_vHPFactor;
	double			m_vOldHP, m_vNewHP, m_vCurHP;
	DWORD			m_vOldHPTickCount;
	double			m_vShowMP, m_vMPFactor;
	double			m_vOldMP, m_vNewMP, m_vCurMP;
	DWORD			m_vOldMPTickCount;
};

#endif // !defined(AFX_DIALOG_MAIN_H__53DA6201_6AB6_11D4_98B0_00C02615866B__INCLUDED_)
