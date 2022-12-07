// Dialog_Crest.h: interface for the CDialog_Crest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_CREST_H__9349F7C1_94A5_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_CREST_H__9349F7C1_94A5_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_TextOuter;
class CTWFont;

class CDialog_Crest : public CUIRes  
{
public:
	CDialog_Crest();
	virtual ~CDialog_Crest();

public:
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void SetMessage(CString& message);
	BOOL ShowWindow(int Id, CUIRes *pParent);
	void HideWindow();
	void HideAllWindow();

	BBDX_TextOuter	*m_pOuter;
	CTWFont			*m_pTWFont;
	int				m_vId;
	LPDIRECTDRAWSURFACE7 m_pSymbolSurface;
	CDDSurface		*m_pDDSymbolSurface;
};

#endif // !defined(AFX_DIALOG_CREST_H__9349F7C1_94A5_11D4_98B2_00C02615866B__INCLUDED_)
