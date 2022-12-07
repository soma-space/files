// Dialog_Status.h: interface for the CDialog_Status class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_STATUS_H__3C794361_69EE_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_STATUS_H__3C794361_69EE_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_TextOuter;
class BBDX_MouseInformation;
class CTWFont;

class CDialog_Status : public CUIRes  
{
public:
	CDialog_Status();
	virtual ~CDialog_Status();

	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void ShowWindow();
	void HideWindow();
	void HideAllWindow();
public:
//	BBDX_TextOuter *m_pOuter;
//	CTWFont *m_pTWFont;
	CFont *m_pFont;
};

#endif // !defined(AFX_DIALOG_STATUS_H__3C794361_69EE_11D4_98B0_00C02615866B__INCLUDED_)
