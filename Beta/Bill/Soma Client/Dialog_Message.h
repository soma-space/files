// Dialog_Message.h: interface for the CDialog_Message class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_MESSAGE_H__D6602F21_774A_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_MESSAGE_H__D6602F21_774A_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
//class BBDX_TextOuter;
//class CTWFont;

class CDialog_Message : public CUIRes  
{
public:
	CDialog_Message();
	virtual ~CDialog_Message();

public:
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void SetMessage(CString& message);

	void ShowWindow(int Id = 0, CUIRes *pParent = NULL);
	void HideWindow();
	void HideAllWindow();

//	BBDX_TextOuter	*m_pOuter;
//	CTWFont			*m_pTWFont;
	CString			m_strMessage;
	CFont			*m_pFont;
	int				m_vId;
};

#endif // !defined(AFX_DIALOG_MESSAGE_H__D6602F21_774A_11D4_98B0_00C02615866B__INCLUDED_)
