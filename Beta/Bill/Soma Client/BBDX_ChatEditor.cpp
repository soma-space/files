// BBDX_ChatEditor.cpp : implementation file
//

#include "stdafx.h"
#include "soma.h"
#include "ChildView.h"
#include "BBDX_ChatEditor.h"
#include "Dialog_Quit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CDialog_Quit Dialog_Quit;
/////////////////////////////////////////////////////////////////////////////
// BBDX_ChatEditor

BBDX_ChatEditor::BBDX_ChatEditor(CWnd *pParent, CUIRes *resParent, BOOL bModeMulti, CRect area, int lineGap, DWORD moreStyle)
:BBDX_Editor(pParent, resParent, bModeMulti, area, lineGap, moreStyle)
{
}

BBDX_ChatEditor::~BBDX_ChatEditor()
{
}


BEGIN_MESSAGE_MAP(BBDX_ChatEditor, BBDX_Editor)
	//{{AFX_MSG_MAP(BBDX_ChatEditor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BBDX_ChatEditor message handlers

BOOL BBDX_ChatEditor::PreTranslateMessage(MSG* pMsg) 
{
	if(Dialog_Quit.IsVisible())
	{
		if(pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE))
		{
			Dialog_Quit.OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
			return TRUE;
		}
		return TRUE;
	}
	if (!m_bModeMulti && pMsg->message==WM_KEYDOWN)
	{
		CString str;
		SHORT value;
		switch(pMsg->wParam)
		{
		case VK_RETURN:
			ReturnkeyProcess();

			str = GetCurEditorText();
			CChildView::SocketMng.SendChat(str, 1);// 타입을 보내줘야한다.
			ClearTextBuffer();
			GetParent()->SetFocus();
			return TRUE;
		case VK_ESCAPE:
			ClearTextBuffer();
			SetWindowText("");
			GetParent()->SetFocus();
			return TRUE;
		case 'V':
		case 'v':
			value = ::GetKeyState(VK_CONTROL);
			if(value & 0x8000)
			{
				return TRUE;
			}
			break;
		case VK_INSERT:
			value = ::GetKeyState(VK_SHIFT);
			if(value & 0x8000)
			{
				return TRUE;
			}
			break;
		}
	}
	else if(pMsg->message == WM_COPY || pMsg->message == WM_CUT || pMsg->message == WM_PASTE || pMsg->message == WM_CONTEXTMENU)
	{
		return TRUE;
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}