// Dialog_BoardList.h: interface for the CDialog_BoardList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_BOARDLIST_H__6566C203_6153_11D4_A1F5_00D0B769A081__INCLUDED_)
#define AFX_DIALOG_BOARDLIST_H__6566C203_6153_11D4_A1F5_00D0B769A081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"

class CDialog_BoardCtrl;
//class BBDX_TextOuter;
class LIST_DATA;
class BBDX_MouseInformation;

class CDialog_BoardList : public CUIRes  
{
public:
	void	ShowWindow();
	void	HideWindow();
	void	HideAllWindow();

	void ShowInList(int nStartIndex);
	void Read();
	void RemoveSaveList();
	BOOL SetList(PBYTE bypData);

	void ListNext();
	void ListBack();
	void InitLister();
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	CDialog_BoardList(CWnd *pWnd = NULL);
	virtual ~CDialog_BoardList();

	BOOL OnButtonAction(int nSpec, int nIndex);

	BOOL OnLButtonDblClk(BBDX_MouseInformation *pMouseInfo);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);
	
	CDialog_BoardCtrl *m_bpBoardCtrl;

//	BBDX_TextOuter* m_pListOuter[10][4];
	CFont			*m_pFont;
	CString			m_ListString[10][4];
	int				m_nVListCount;

	int m_nSelRow;

	int m_nStartIndex;
	BOOL m_bProcessing;
};

#endif // !defined(AFX_DIALOG_BOARDLIST_H__6566C203_6153_11D4_A1F5_00D0B769A081__INCLUDED_)
