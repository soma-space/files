// Dialog_MemoCtrl.h: interface for the CDialog_MemoCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_MEMOCTRL_H__5F6E05A3_71CC_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_MEMOCTRL_H__5F6E05A3_71CC_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDialog_Memo;
class CDirectDraw;
class CChildView;
class CDDSurface;
class BBDX_MouseInformation;
class CUIRes;

typedef CTypedPtrArray<CPtrArray, CDialog_Memo *> CDialog_MemoArray;
typedef CTypedPtrList<CPtrList, CDialog_Memo *> CDialog_MemoList;

class CDialog_MemoCtrl  
{
public:
	CDialog_MemoCtrl();
	virtual ~CDialog_MemoCtrl();

	void InitData(CDirectDraw *pDD, CChildView *pWnd);
	void AddMemoChat(CString user_name, CString chat);
	void AddEchoMemoChat(CString user_name, CString chat);
	void Draw(CDDSurface *pDest);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);
	void HideMemo(CUIRes *res);
	void DeleteContents();

public:
	CDialog_MemoList m_listDialogMemo;
	CDialog_MemoArray m_arrHiddenMemo;
	CWnd *m_pWnd;
	CDirectDraw *m_pDD;
};

#endif // !defined(AFX_DIALOG_MEMOCTRL_H__5F6E05A3_71CC_11D4_98B0_00C02615866B__INCLUDED_)
