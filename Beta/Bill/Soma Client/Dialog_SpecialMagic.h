// Dialog_SpecialMagic.h: interface for the CDialog_SpecialMagic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_SPECIALMAGIC_H__56D161E2_742D_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_SPECIALMAGIC_H__56D161E2_742D_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"
class BBDX_MouseInformation;
class CDialog_MagicCtrl;
//class BBDX_TextOuter;
//class CTWFont;

class CDialog_SpecialMagic : public CUIRes  
{
public:
	CDialog_SpecialMagic(CWnd *wnd);
	virtual ~CDialog_SpecialMagic();

public:
	void ShowWindow();
	void HideWindow();
	void HideAllWindow();

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL DragDraw(CDDSurface *pDest);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void SetScrollData();

	void InitData();

	int m_vMagicCount;
	CUIScrollBar *m_pScrollBar;
	CDialog_MagicCtrl *m_pDialogCtrl;
//	BBDX_TextOuter	*m_pOuter;
//	CTWFont			*m_pTWFont;
	CFont			*m_pFont;
	int				m_vDragItem;
	CPoint			m_vDragOffset;
};

#endif // !defined(AFX_DIALOG_SPECIALMAGIC_H__56D161E2_742D_11D4_98B0_00C02615866B__INCLUDED_)
