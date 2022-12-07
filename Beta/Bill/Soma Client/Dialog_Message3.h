// Dialog_Message3.h: interface for the CDialog_Message3 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_MESSAGE3_H__721EE1A1_7781_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_MESSAGE3_H__721EE1A1_7781_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
//class BBDX_TextOuter;
//class CTWFont;

class CDialog_Message3 : public CUIRes  
{
public:
	CDialog_Message3();
	virtual ~CDialog_Message3();

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

#endif // !defined(AFX_DIALOG_MESSAGE3_H__721EE1A1_7781_11D4_98B0_00C02615866B__INCLUDED_)
