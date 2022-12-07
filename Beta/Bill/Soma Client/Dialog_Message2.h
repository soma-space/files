// Dialog_Message2.h: interface for the CDialog_Message2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_MESSAGE2_H__D6602F22_774A_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_MESSAGE2_H__D6602F22_774A_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
//class BBDX_TextOuter;
//class CTWFont;

class CDialog_Message2 : public CUIRes  
{
public:
	CDialog_Message2();
	virtual ~CDialog_Message2();

public:
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void SetMessage(CString& message);
	void ShowWindow(int Id, CUIRes *pParent);
	void HideWindow();
	void HideAllWindow();

//	BBDX_TextOuter	*m_pOuter;
//	CTWFont			*m_pTWFont;
	CString			m_strMessage;
	CFont			*m_pFont;
	int				m_vId;
};

#endif // !defined(AFX_DIALOG_MESSAGE2_H__D6602F22_774A_11D4_98B0_00C02615866B__INCLUDED_)
