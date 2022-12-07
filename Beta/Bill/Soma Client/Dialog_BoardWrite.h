// Dialog_BoardWrite.h: interface for the CDialog_BoardWrite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_BOARDWRITE_H__6566C205_6153_11D4_A1F5_00D0B769A081__INCLUDED_)
#define AFX_DIALOG_BOARDWRITE_H__6566C205_6153_11D4_A1F5_00D0B769A081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"

class CDialog_BoardCtrl;
class BBDX_Editor;
class BBDX_MouseInformation;


class CDialog_BoardWrite : public CUIRes  
{
public:
	void	ShowWindow();
	void	HideWindow();
	void	HideAllWindow();

	void Write();
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void SetFocus();
	void InitWriter();
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	CDialog_BoardWrite(CWnd *pWnd = NULL);
	virtual ~CDialog_BoardWrite();

	void SetScrollData();

	BOOL OnButtonAction(int nSpec, int nIndex);

	
	CDialog_BoardCtrl *m_bpBoardCtrl;

	BBDX_Editor *m_pWriteEditor;

	CUIScrollBar *m_pScrollBar;

	int m_vWriteLineCount;
	int m_vLineGap;
};

#endif // !defined(AFX_DIALOG_BOARDWRITE_H__6566C205_6153_11D4_A1F5_00D0B769A081__INCLUDED_)
