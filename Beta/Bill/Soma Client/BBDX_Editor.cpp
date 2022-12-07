// BBDX_Editor.cpp : implementation file
//

#include "stdafx.h"
#include "Soma.h"
#include "BBDX_Editor.h"
#include "ctype.h"
#include "DManage/BB_Direct.h"
#include "TWFont.h"
#include "UIRes.h"
#include "Dialog_Quit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWnd			*g_pFocusWnd;
extern CDialog_Quit	Dialog_Quit;
/////////////////////////////////////////////////////////////////////////////
// BBDX_Editor

BBDX_Editor::BBDX_Editor(CWnd *pParent, CUIRes *resParent, BOOL bModeMulti, CRect area, BOOL readOnly, int lineGap, DWORD moreStyle)
{
	m_vRealRect = area;
	m_vWndRect = area;
	m_vLineGap = lineGap;
	m_sizeFont.cx = 12;
	m_sizeFont.cy = 12;
	m_bReadOnly = readOnly;
	if(m_bReadOnly) moreStyle |= ES_READONLY;

	if(bModeMulti)
	{
//		m_vWndRect.right += 12; // 실제 에디터와 화면에 뿌리는 에디터의 글자를 맞추기 위해
		m_vWndRect.bottom = m_vWndRect.top + (area.Height() / (m_sizeFont.cy+lineGap)) * m_sizeFont.cy;
	}
	m_bModeMulti = bModeMulti;
	m_CaretBlinkTime = (DWORD)GetCaretBlinkTime();
	m_TickCount	= GetTickCount();
	m_CaretOn = FALSE;
	m_vWndRect.OffsetRect(-10000, -10000);
	if(moreStyle&ES_PASSWORD) m_bPassword = TRUE;
	else m_bPassword = FALSE;
	if (bModeMulti) Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL|moreStyle, m_vWndRect, pParent, 100);
	else Create(WS_CHILD|WS_VISIBLE|moreStyle, m_vWndRect, pParent, 100);

//	m_pBitmapFont = NULL;
	m_pEditorFont = NULL;
	FontSet();
	m_bIsFocused = FALSE;
	m_bChat = TRUE;
	m_pParent = resParent;
	m_bRecalcCaretTime = TRUE;
}

BBDX_Editor::~BBDX_Editor()
{
	if (GetSafeHwnd()) DestroyWindow();
//	SAFE_DELETE(m_pBitmapFont);
	SAFE_DELETE(m_pEditorFont);
}


BEGIN_MESSAGE_MAP(BBDX_Editor, CEdit)
	//{{AFX_MSG_MAP(BBDX_Editor)
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BBDX_Editor message handlers

void BBDX_Editor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
	case VK_ESCAPE:
		ClearTexts();
		if(m_pParent) m_pParent->OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	case VK_UP:
	case VK_LEFT:
	case VK_DOWN:
	case VK_RIGHT:
	case VK_HOME:
	case VK_END:
	case VK_PRIOR:
	case VK_NEXT:
		m_bRecalcCaretTime = TRUE;
		break;
	case VK_TAB:
		if(m_pParent) m_pParent->OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	if(m_pParent)
	{
		m_pParent->OnKeyDown(nChar, nRepCnt, nFlags);
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
	else
	{
		if((nChar >= VK_F1 && nChar <= VK_F12) || (nChar == 'S' && (GetKeyState(VK_CONTROL)&0x80)) || (nChar == 'Q' && (GetKeyState(VK_CONTROL)&0x80)) || nChar == VK_TAB)
			GetParent()->PostMessage(WM_KEYDOWN, (WPARAM)nChar, MAKELPARAM(nRepCnt, nFlags));
		else CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void BBDX_Editor::ClearTextBuffer()
{
	m_szEditorText.Empty();
}

void BBDX_Editor::ClearTexts()
{
	SetWindowText(""); 
	SetCaretPos(CPoint(0,0));
}

BOOL BBDX_Editor::IsShowCaret()
{
	DWORD curTickCount = GetTickCount();
	if(m_bRecalcCaretTime == TRUE)
	{
		m_CaretOn = TRUE;
		m_TickCount = curTickCount;
		m_bRecalcCaretTime = FALSE;
	}
	else if (curTickCount-m_TickCount>m_CaretBlinkTime)
	{
		m_CaretOn = !m_CaretOn;
		m_TickCount = curTickCount;
	}
	return m_CaretOn;
}

void BBDX_Editor::ReturnkeyProcess()
{
	if (m_bModeMulti) return;

	if(m_bChat == FALSE) return;

	GetWindowText(m_szEditorText);
	SetWindowText("");
	SetCaretPos(CPoint(0,0));
}

void BBDX_Editor::ChangeEditorMode(BOOL bModeMulti, CRect area)
{
	CWnd *pParent = GetParent();

	if (GetSafeHwnd()) DestroyWindow();

	m_bModeMulti = bModeMulti;
	m_vRealRect = area;
	m_vWndRect = area;
	if(bModeMulti) m_vWndRect.bottom = (area.Height() / (m_sizeFont.cy+m_vLineGap)) * m_sizeFont.cy;
	area.OffsetRect(-10000, -10000);
	if (m_bModeMulti) Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL, m_vWndRect, pParent, 100);
	else Create(WS_CHILD|WS_VISIBLE, m_vWndRect, pParent, 100);
	if (GetSafeHwnd()) SetFocus();

	m_szEditorText.Empty();
}

void BBDX_Editor::ChangeArea(CRect area)
{
	if (GetSafeHwnd())
	{
		m_vRealRect = area;
		area.OffsetRect(-10000, -10000);
		MoveWindow(area, FALSE);
	}
}

void BBDX_Editor::FontSet()
{
	SetFont (NULL);

	CFont *Font = new CFont;
	if(Font->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림")) SetFont(Font);
	else delete Font, Font = NULL;

	if (m_pEditorFont) delete m_pEditorFont, m_pEditorFont = NULL;
	m_pEditorFont = Font;
}

BOOL BBDX_Editor::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	SetFont(NULL);	
	return CEdit::DestroyWindow();
}

void BBDX_Editor::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	m_bIsFocused = TRUE;
	g_pFocusWnd = this;
}

void BBDX_Editor::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);

	CWnd *focus_wnd = CWnd::GetFocus();
	if(focus_wnd==NULL || focus_wnd->IsChild(this) == FALSE) g_pFocusWnd = this;
	else g_pFocusWnd = NULL;
	m_bIsFocused = FALSE;
}

BOOL BBDX_Editor::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(Dialog_Quit.IsVisible())
	{
		if(pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE))
		{
			Dialog_Quit.OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
			return TRUE;
		}
	}
	if (!m_bModeMulti && pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		ReturnkeyProcess();
		if(m_pParent) m_pParent->OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
		return TRUE;
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}

void BBDX_Editor::GetText()
{
	if (m_bModeMulti) return;
	GetWindowText(m_szEditorText);
}

void BBDX_Editor::DCDrawCurText(CDDSurface *pSurface, COLORREF color, BOOL emptyTextCurSorDraw, BOOL bPassword)
{
	if(!m_bModeMulti)		// 한줄일 경우
	{
		CString text;
		int sel_start, sel_end;
		GetSel(sel_start, sel_end);		// 선택 영역을 구한다

		// 커서 위치를 구한다
		int caretPos = -1;
		if(m_bReadOnly == FALSE)
		{
			if (m_bIsFocused&&IsShowCaret()) caretPos = sel_start;
			if (text.IsEmpty()&&!emptyTextCurSorDraw) caretPos = -1;
		}

		GetWindowText(text);			// 에디터에서 글자를 읽어 온다
		if(m_bPassword)
		{
			int count = text.GetLength();
			text = _T("");
			for(int i = 0; i < count; i++) text += '*';
		}

		// 패스워드 입력일 경우 모두 '*'로 표시하도록 바꾼다
/*		if(bPassword)
		{
			int count = text.GetLength();
			text = _T("");
			for(int i = 0; i < count; i++) text += '*';
		}*/

		CDC *pDC = pSurface->GetDC();
		CFont *old_font = (CFont *)pDC->SelectObject(m_pEditorFont);
		COLORREF old_color = pDC->SetTextColor(color);
		int mode = pDC->SetBkMode(TRANSPARENT);
		
		if(sel_start == sel_end)		// 선택 영역이 없는 경우
		{
			// 문장 출력
			pDC->DrawText(text, &m_vRealRect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL);
		}
		else			// 선택 영역이 있는 경우
		{
			CString str1;
			CRect t_rect;

			t_rect = m_vRealRect;
			if(sel_start > 0)		// 선택 영역 바로 전까지 출력
			{
				str1 = text.Left(sel_start);
				pDC->DrawText(str1, &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL|DT_CALCRECT);
				pDC->DrawText(str1, &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL);

				t_rect.left = t_rect.right;
				t_rect.right = m_vRealRect.right;
			}

			// 선택 영역 출력
			pDC->SetTextColor(~color);
			pDC->SetBkColor(color);
			pDC->SetBkMode(OPAQUE);
			str1 = text.Mid(sel_start, sel_end-sel_start);
			pDC->DrawText(str1, &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL|DT_CALCRECT);
			pDC->DrawText(str1, &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL);

			// 선택 영역 이후 출력
			if(sel_end < text.GetLength())
			{
				t_rect.left = t_rect.right;
				t_rect.right = m_vRealRect.right;

				str1 = text.Right(text.GetLength() - sel_end);
				pDC->SetTextColor(color);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(str1, &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL);
			}
		}

		// 커서를 표시한다
		if(caretPos != -1)
		{
			CPoint caret_pos = GetCaretPos();
			caret_pos.Offset(m_vRealRect.left, m_vRealRect.top);
			CPen pen(PS_SOLID, 3, color), *old_pen;
			
			old_pen = (CPen *)pDC->SelectObject(&pen);
			pDC->MoveTo(caret_pos.x, caret_pos.y+1);
			pDC->LineTo(caret_pos.x, caret_pos.y+m_sizeFont.cy+1);
			pDC->SelectObject(old_pen);
		}

		pDC->SetTextColor(color);
		pDC->SetBkMode(mode);
		pDC->SelectObject(old_font);
		pSurface->ReleaseDC(pDC);
	}
	else			// 여러줄일 경우
	{
		CString text;
		char t_text[100];
		int count = GetLineCount();
		int sel_start, sel_end;
		int i;
		int visible_line, str_count_line;
		CRect t_rect;

		// 선택 영역을 가져온다
		GetSel(sel_start, sel_end);
		// 화면에 출력될 첨라인을 가져온다
		visible_line = GetFirstVisibleLine();
		int firstCharPos = LineIndex(visible_line);

		// 커서 출력 여부를 결정한다
		BOOL caretPos = FALSE;
		if (m_bReadOnly == FALSE && m_bIsFocused && IsShowCaret()) caretPos = TRUE;

		CDC *pDC = pSurface->GetDC();
		CFont *old_font = (CFont *)pDC->SelectObject(m_pEditorFont);
		COLORREF old_color = pDC->SetTextColor(color);
		int mode = pDC->SetBkMode(TRANSPARENT);

		int select_line = LineFromChar();

		if(sel_start == sel_end)	// 선택 영역이 없을 경우
		{
			t_rect = m_vRealRect;

			// 문장을 출력한다
			for(i = visible_line; i < count; i++)
			{
				str_count_line = GetLine(i, t_text, 100);
				if(str_count_line != 0)
				{
					t_text[str_count_line] = 0;
					text = t_text;
					pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
					pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);
				}
				else
				{
					text = "a";
					pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
				}

				t_rect.left = m_vRealRect.left;
				t_rect.top = t_rect.bottom+m_vLineGap;
				t_rect.right = m_vRealRect.right;
				t_rect.bottom = m_vRealRect.bottom;
				if(t_rect.top > t_rect.bottom-12) break;
			}
		}
		else			// 선택 영역이 있을 경우
		{
			int select_line1 = LineFromChar(sel_start);
			int select_line2 = LineFromChar(sel_end);
			t_rect = m_vRealRect;
			if(select_line2 < visible_line)
			{
				for(i = visible_line; i < count; i++)
				{
					str_count_line = GetLine(i, t_text, 100);
					if(str_count_line != 0)
					{
						t_text[str_count_line] = 0;
						text = t_text;
						pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
						pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);
					}
					else
					{
						text = _T(" ");
						pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
					}

					t_rect.left = m_vRealRect.left;
					t_rect.top = t_rect.bottom+m_vLineGap;
					t_rect.right = m_vRealRect.right;
					t_rect.bottom = m_vRealRect.bottom;
					if(t_rect.top > t_rect.bottom-(m_sizeFont.cy+m_vLineGap)) break;
				}
			}
			else
			{
				for(i = visible_line; i < select_line1; i++)
				{
					str_count_line = GetLine(i, t_text, 100);
					if(str_count_line != 0)
					{
						t_text[str_count_line] = 0;
						text = t_text;
						pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
						pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);
					}
					else
					{
						text = _T(" ");
						pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
					}

					t_rect.left = m_vRealRect.left;
					t_rect.top = t_rect.bottom+m_vLineGap;
					t_rect.right = m_vRealRect.right;
					t_rect.bottom = m_vRealRect.bottom;
					if(t_rect.top > t_rect.bottom-(m_sizeFont.cy+m_vLineGap)) goto TEXTOUT_END;
				}

				str_count_line = GetLine(i, t_text, 100);
				CString str1;
				int block_start, block_end, line_start;
				if(str_count_line != 0)
				{
					t_text[str_count_line] = 0;
					text = t_text;
				}
				else text = _T("");
				line_start = LineIndex(i);
				if(i == select_line1)
				{
					block_start = sel_start-line_start;
					if(block_start > 0)
					{
						str1 = text.Left(block_start);
						pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
						pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);

						t_rect.left = t_rect.right;
						t_rect.right = m_vRealRect.right;
					}
				}
				else block_start = 0;

				pDC->SetTextColor(~color);
				pDC->SetBkColor(color);
				pDC->SetBkMode(OPAQUE);

				if(select_line1 == select_line2)
				{
					if(text.IsEmpty() == FALSE)
					{
						block_end = sel_end - line_start;

						str1 = text.Mid(block_start, block_end-block_start);
						pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
						pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);

						pDC->SetTextColor(color);
						pDC->SetBkMode(TRANSPARENT);
						if(block_end < text.GetLength())
						{
							t_rect.left = t_rect.right;
							t_rect.right = m_vRealRect.right;
							
							str1 = text.Right(text.GetLength() - block_end);
							pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);
						}
					}
					else
					{
						str1 = _T(" ");
						pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
					}
				}
				else
				{
					if(text.IsEmpty() == FALSE)
					{
						str1 = text.Right(text.GetLength() - block_start);
						pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
						pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);
					}
					else
					{
						str1 = _T(" ");
						pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
					}

					t_rect.left = m_vRealRect.left;
					t_rect.top = t_rect.bottom+m_vLineGap;
					t_rect.right = m_vRealRect.right;
					t_rect.bottom = m_vRealRect.bottom;
					if(t_rect.top > t_rect.bottom-(m_sizeFont.cy+m_vLineGap)) goto TEXTOUT_END;

					i++;
					for(; i < select_line2; i++)
					{
						str_count_line = GetLine(i, t_text, 100);
						if(str_count_line != 0)
						{
							t_text[str_count_line] = 0;
							text = t_text;
							pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
							pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);
						}
						else
						{
							text = _T(" ");
							pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
						}

						t_rect.left = m_vRealRect.left;
						t_rect.top = t_rect.bottom+m_vLineGap;
						t_rect.right = m_vRealRect.right;
						t_rect.bottom = m_vRealRect.bottom;
						if(t_rect.top > t_rect.bottom-(m_sizeFont.cy+m_vLineGap)) goto TEXTOUT_END;
					}

					str_count_line = GetLine(i, t_text, 100);
					if(str_count_line != 0)
					{
						t_text[str_count_line] = 0;
						text = t_text;

						line_start = LineIndex(select_line2);
						block_end = sel_end - line_start;

						if(block_end > 0)
						{
							str1 = text.Left(block_end);
							pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
							pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);
						}
						else
						{
							str1 = _T(" ");
							pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
							t_rect.right = m_vRealRect.left;
						}
						pDC->SetTextColor(color);
						pDC->SetBkMode(TRANSPARENT);
						if(block_end < text.GetLength())
						{
							t_rect.left = t_rect.right;
							t_rect.right = m_vRealRect.right;
							
							str1 = text.Right(text.GetLength() - block_end);
							pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);
						}
					}
					else
					{
						pDC->SetTextColor(color);
						pDC->SetBkMode(TRANSPARENT);
						str1 = _T(" ");
						pDC->DrawText(str1, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
					}
				}

				t_rect.left = m_vRealRect.left;
				t_rect.top = t_rect.bottom+m_vLineGap;
				t_rect.right = m_vRealRect.right;
				t_rect.bottom = m_vRealRect.bottom;
				if(t_rect.top > t_rect.bottom-(m_sizeFont.cy+m_vLineGap)) goto TEXTOUT_END;

				for(i = select_line2+1; i < count; i++)
				{
					str_count_line = GetLine(i, t_text, 100);
					if(str_count_line != 0)
					{
						t_text[str_count_line] = 0;
						text = t_text;
						pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
						pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL);
					}
					else
					{
						text = _T(" ");
						pDC->DrawText(text, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT);
					}

					t_rect.left = m_vRealRect.left;
					t_rect.top = t_rect.bottom+m_vLineGap;
					t_rect.right = m_vRealRect.right;
					t_rect.bottom = m_vRealRect.bottom;
					if(t_rect.top > t_rect.bottom-(m_sizeFont.cy+m_vLineGap))  goto TEXTOUT_END;
				}
			}
		}

TEXTOUT_END:
		// 커서를 표시한다
		if(caretPos == TRUE)
		{
			CPoint caret_pos = GetCaretPos();
			caret_pos.y = (caret_pos.y / m_sizeFont.cy) * (m_sizeFont.cy+m_vLineGap);
			caret_pos.Offset(m_vRealRect.left, m_vRealRect.top);
			CPen pen(PS_SOLID, 3, color), *old_pen;

			if(m_vRealRect.PtInRect(caret_pos))
			{
				old_pen = (CPen *)pDC->SelectObject(&pen);
				pDC->MoveTo(caret_pos.x, caret_pos.y-1);
				pDC->LineTo(caret_pos.x, caret_pos.y+m_sizeFont.cy-1);
				pDC->SelectObject(old_pen);
			}
		}

		pDC->SetTextColor(color);
		pDC->SetBkMode(mode);
		pDC->SelectObject(old_font);
		pSurface->ReleaseDC(pDC);
	}
}

void BBDX_Editor::DCDrawOffsetedCurText(CDDSurface *pSurface, CPoint offset, COLORREF color, BOOL bPassword)
{
	CRect r = m_vRealRect;

	m_vRealRect.OffsetRect(offset);
	DCDrawCurText(pSurface, color, TRUE, bPassword);

	m_vRealRect = r;
}

void BBDX_Editor::DCDrawPosCurText(CDDSurface *pSurface, CPoint pos, COLORREF color, BOOL bPassword)
{
	CRect r = m_vRealRect;

	m_vRealRect.OffsetRect(pos.x-m_vRealRect.left, pos.y-m_vRealRect.top);
	DCDrawCurText(pSurface, color, TRUE, bPassword);

	m_vRealRect = r;
}

void BBDX_Editor::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == VK_TAB) return;
	m_bRecalcCaretTime = TRUE;
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void BBDX_Editor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(m_pParent)
	{
		m_pParent->OnKeyUp(nChar, nRepCnt, nFlags);
		CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
	}
	else
	{
		if((nChar >= VK_F1 && nChar <= VK_F12) || (nChar == 'S' && (GetKeyState(VK_CONTROL)&0x80)) || (nChar == 'Q' && (GetKeyState(VK_CONTROL)&0x80)) || nChar == VK_TAB)
		{
			GetParent()->PostMessage(WM_KEYUP, (WPARAM)nChar, MAKELPARAM(nRepCnt, nFlags));
		}
		else CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
	}
}

void BBDX_Editor::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetParent()->PostMessage(WM_SYSKEYDOWN, (WPARAM)nChar, MAKELPARAM(nRepCnt, nFlags));
}

void BBDX_Editor::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetParent()->PostMessage(WM_SYSKEYUP, (WPARAM)nChar, MAKELPARAM(nRepCnt, nFlags));
}
