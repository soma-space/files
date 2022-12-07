// Dialog_Repair.h: interface for the CDialog_Repair class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_REPAIR_H__56C06941_EACE_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_REPAIR_H__56C06941_EACE_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"
#include "DarkDef.h"

class BBDX_MouseInformation;

class CDialog_Repair : public CUIRes  
{
public:
	CDialog_Repair();
	virtual ~CDialog_Repair();

	void InitData();
	void SendRepairItem();
	void RcvRepairItem(PBYTE bypData);
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

	int				m_vDragItem;
	CPoint			m_vDragOffset;
	int				m_vCurArmItem, m_vCurInvItem;
	CUIScrollBar	*m_pScrollBar;
	CFont			*m_pFont;
	int				m_vTipIndex;
	CItemMeMap		m_mapItem;
	ITEM_ME			*m_pRepairItem;
	int				m_vRepairIndex;
	BOOL			m_bProcessing;
	DWORD			m_vRepairMoney;
	short			m_vRepairDur;
};

#endif // !defined(AFX_DIALOG_REPAIR_H__56C06941_EACE_11D4_98B2_00C02615866B__INCLUDED_)
