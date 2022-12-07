// Dialog_Chat.h: interface for the CDialog_Chat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_CHAT_H__FBA48461_8312_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_CHAT_H__FBA48461_8312_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"

class BBDX_ChatOuter;
class BBDX_MouseInformation;
class CTWFont;

class CDialog_Chat : public CUIRes  
{
public:
	CDialog_Chat();
	virtual ~CDialog_Chat();

	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	BOOL OnButtonAction(int nSpec, int nIndex);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	void ShowWindow();
	void HideWindow();
	void HideAllWindow();

	void SetScrollData();
	void ScrollDown();
	void ScrollUp();

public:
	BBDX_ChatOuter *m_pChatOuter;
	CUIScrollBar *m_pScrollBar;
};

#endif // !defined(AFX_DIALOG_CHAT_H__FBA48461_8312_11D4_98B0_00C02615866B__INCLUDED_)
