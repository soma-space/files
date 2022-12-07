// Dialog_BoardRead.h: interface for the CDialog_BoardRead class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_BOARDREAD_H__6566C204_6153_11D4_A1F5_00D0B769A081__INCLUDED_)
#define AFX_DIALOG_BOARDREAD_H__6566C204_6153_11D4_A1F5_00D0B769A081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"
class CDialog_BoardCtrl;
//class BBDX_TextOuter;
class BBDX_MouseInformation;


class CDialog_BoardRead : public CUIRes
{
public:
	void	ShowWindow();
	void	HideWindow();
	void	HideAllWindow();

	void Delete();
	void ReadNext();
	void ReadBack();
	void SetRead(PBYTE bypData);
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	void InitReader();
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	CDialog_BoardRead(CWnd *pWnd = NULL);
	virtual ~CDialog_BoardRead();
	BOOL OnButtonAction(int nSpec, int nIndex);

	void SetScrollData();

	CDialog_BoardCtrl *m_bpBoardCtrl;

//	BBDX_TextOuter *(m_pReadOuter[3]);
	CFont			*m_pFont;
	CString			m_ReadString[3];
	int				m_vTotalLine;

	int m_vLineGap, m_vOffsetCount;

	CUIScrollBar *m_pScrollBar;

	int m_nReadIndex;	// 목록에서 현재 읽고있는 글의 Index;
	BOOL m_bProcessing;
	int m_vReadLineCount;
};

#endif // !defined(AFX_DIALOG_BOARDREAD_H__6566C204_6153_11D4_A1F5_00D0B769A081__INCLUDED_)
