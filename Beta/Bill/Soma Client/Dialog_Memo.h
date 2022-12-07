// Dialog_Memo.h: interface for the CDialog_Memo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_MEMO_H__5F6E05A2_71CC_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_MEMO_H__5F6E05A2_71CC_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"

//class BBDX_TextOuter;
//class BBDX_Editor;
class BBDX_MouseInformation;
class BBDX_ChatOuter;
//class CTWFont;

class CDialog_Memo : public CUIRes  
{
public:
	CDialog_Memo(CWnd *pWnd = NULL);
	virtual ~CDialog_Memo();

	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	BOOL OnRButtonDown(BBDX_MouseInformation *pMouseInfo);
	BOOL OnButtonAction(int nSpec, int nIndex);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	void ShowWindow(BOOL bIcon);
	void HideWindow();

	void SetScrollData();

	void SetUserName(CString user_name);
	void SendChat(CString chat);
	void AddMessage(BOOL bMe, CString chat);
	void SetFocus();
public:
//	CTWFont *m_pTWFont;
//	BBDX_TextOuter *m_pChatOuter, *m_pUserOuter;
	BBDX_ChatOuter *m_pChatOuter;
	CFont *m_pFont;
	CUIScrollBar *m_pScrollBar;
	CString m_vUserName;
	BOOL m_bIcon;
	int m_vIconIndex;
	CRect m_vMemoRect;
};

#endif // !defined(AFX_DIALOG_MEMO_H__5F6E05A2_71CC_11D4_98B0_00C02615866B__INCLUDED_)
