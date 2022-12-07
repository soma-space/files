// Dialog_MemoCtrl.cpp: implementation of the CDialog_MemoCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_MemoCtrl.h"
#include "Dialog_Memo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_MemoCtrl::CDialog_MemoCtrl()
{
	m_pWnd = NULL;
}

CDialog_MemoCtrl::~CDialog_MemoCtrl()
{
	DeleteContents();
}

void CDialog_MemoCtrl::DeleteContents()
{
	int i, count;
	POSITION pos;

	pos = m_listDialogMemo.GetHeadPosition();
	while(pos)
	{
		delete m_listDialogMemo.GetNext(pos);
	}
	m_listDialogMemo.RemoveAll();

	count = m_arrHiddenMemo.GetSize();
	for(i = 0; i < count; i++)
	{
		delete m_arrHiddenMemo[i];
	}
	m_arrHiddenMemo.RemoveAll();
}

void CDialog_MemoCtrl::AddMemoChat(CString user_name, CString chat)
{
	CDialog_Memo *dialog_memo;
	POSITION pos;

	pos = m_listDialogMemo.GetHeadPosition();
	while(pos)
	{
		dialog_memo = m_listDialogMemo.GetNext(pos);
		if(!dialog_memo->m_vUserName.CompareNoCase((LPCTSTR)user_name))
		{
			dialog_memo->AddMessage(FALSE, chat);
			return;
		}
	}

	if(m_arrHiddenMemo.GetSize() > 0)
	{
		dialog_memo = m_arrHiddenMemo[0];
		m_arrHiddenMemo.RemoveAt(0);
	}
	else
	{
		dialog_memo = new CDialog_Memo(m_pWnd);
		dialog_memo->Create(m_pDD, "Memo.lib", 0, FALSE, TRUE, FALSE);
	}
	dialog_memo->SetUserName(user_name);
	dialog_memo->AddMessage(FALSE, chat);
	dialog_memo->ShowWindow(TRUE);
	m_listDialogMemo.AddTail(dialog_memo);
}

void CDialog_MemoCtrl::AddEchoMemoChat(CString user_name, CString chat)
{
	CDialog_Memo *dialog_memo;
	POSITION pos;

	pos = m_listDialogMemo.GetHeadPosition();
	while(pos)
	{
		dialog_memo = m_listDialogMemo.GetNext(pos);
		if(!dialog_memo->m_vUserName.CompareNoCase((LPCTSTR)user_name))
		{
			dialog_memo->AddMessage(TRUE, chat);
			return;
		}
	}

	if(m_arrHiddenMemo.GetSize() > 0)
	{
		dialog_memo = m_arrHiddenMemo[0];
		m_arrHiddenMemo.RemoveAt(0);
	}
	else
	{
		dialog_memo = new CDialog_Memo(m_pWnd);
		dialog_memo->Create(m_pDD, "Memo.lib", 0, FALSE, TRUE, FALSE);
	}
	dialog_memo->SetUserName(user_name);
	dialog_memo->AddMessage(TRUE, chat);
	dialog_memo->ShowWindow(FALSE);
	m_listDialogMemo.AddTail(dialog_memo);
/*	int count, i;

	count = m_arrDialogMemo.GetSize();
	for(i = 0; i < count; i++)
	{
		if(!m_arrDialogMemo[i]->m_vUserName.CompareNoCase((LPCTSTR)user_name))
		{
			m_arrDialogMemo[i]->AddMessage(TRUE, chat);
			return;
		}
	}*/
}

void CDialog_MemoCtrl::InitData(CDirectDraw *pDD, CChildView *pWnd)
{
	m_pDD = pDD;
	m_pWnd = (CWnd *)pWnd;
}

void CDialog_MemoCtrl::Draw(CDDSurface *pDest)
{
	int pos_x, size_y, i = 0;
	CDialog_Memo *t_memo;
	CSize icon_size;
	POSITION pos;

	pos = m_listDialogMemo.GetHeadPosition();

	if(pos)
	{
		t_memo = m_listDialogMemo.GetNext(pos);
		size_y = t_memo->m_arrArea[4]->m_vRect.Height();
		pos_x = 800 - (12*8 + t_memo->m_arrArea[4]->m_vRect.Width())/2;
		if(t_memo->m_bIcon)
		{
			t_memo->m_ptOffset.x = pos_x;
			t_memo->m_ptOffset.y = (size_y+16)*i++;
		}
		t_memo->Draw(pDest);
		while(pos)
		{
			t_memo = m_listDialogMemo.GetNext(pos);
			if(t_memo->m_bIcon)
			{
				t_memo->m_ptOffset.x = pos_x;
				t_memo->m_ptOffset.y = (size_y+16)*i++;
			}
			t_memo->Draw(pDest);
		}
	}
/*	count = m_arrDialogMemo.GetSize();
	if(count > 0)
	{
		pos_x = 800 - (12*8 + m_arrDialogMemo[0]->m_arrArea[4]->m_vRect.Width())/2;
		for(i = 0; i < count; i++)
		{
			t_memo = m_arrDialogMemo[i];
			if(t_memo->m_bIcon)
			{
				t_memo->m_ptOffset.x = pos_x;
				t_memo->m_ptOffset.y = (t_memo->m_arrArea[4]->m_vRect.Height()+16)*i;
			}
			t_memo->Draw(pDest);
		}
	}*/
}

int CDialog_MemoCtrl::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	CDialog_Memo *t_memo;
	int retVal = 0;
	POSITION pos, old_pos;

	pos = m_listDialogMemo.GetTailPosition();

	while(pos)
	{
		old_pos = pos;
		t_memo = m_listDialogMemo.GetPrev(pos);
		retVal = t_memo->MouseProcess(pMouseInfo);
		if(retVal > 0)
		{
			if(retVal == 3)
			{
				m_listDialogMemo.RemoveAt(old_pos);
				m_listDialogMemo.AddTail(t_memo);
			}
			return retVal;
		}
	}

	return 0;
}

void CDialog_MemoCtrl::HideMemo(CUIRes *res)
{
	POSITION pos;

	pos = m_listDialogMemo.Find(res);
	if(pos)
	{
		res->HideWindow();
		m_arrHiddenMemo.Add((CDialog_Memo *)res);
		m_listDialogMemo.RemoveAt(pos);
	}
}