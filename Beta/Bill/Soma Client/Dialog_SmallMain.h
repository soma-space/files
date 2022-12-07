// Dialog_SmallMain.h: interface for the CDialog_SmallMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_SMALLMAIN_H__3EC77321_EB95_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_SMALLMAIN_H__3EC77321_EB95_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"

class BBDX_MouseInformation;

class CDialog_SmallMain : public CUIRes  
{
public:
	CDialog_SmallMain();
	virtual ~CDialog_SmallMain();

	void CDialog_SmallMain::InitData();
	void DrawTipRect(CDDSurface *pDest, CRect& t_rect, CRect standard_rect, int area_index);
	void ViewCaption(CDDSurface *pDest, CRect standard_rect, CString str, int area_index);

public:
	BOOL Draw(CDDSurface *pDest);
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans);

	CFont			*m_pFont;
	int				m_bViewCaption;
	CString			m_vStrCaption;
};

#endif // !defined(AFX_DIALOG_SMALLMAIN_H__3EC77321_EB95_11D4_98B2_00C02615866B__INCLUDED_)
