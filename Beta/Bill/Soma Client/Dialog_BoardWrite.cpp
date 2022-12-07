// Dialog_BoardWrite.cpp: implementation of the CDialog_BoardWrite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dialog_BoardWrite.h"

#include "Dialog_BoardCtrl.h"
#include "BBDX_Editor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CUIRes *g_pFocusDialog;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_BoardWrite::CDialog_BoardWrite(CWnd *pWnd)
{
	m_pWnd = pWnd;
	
	INIT_PTR(m_bpBoardCtrl);
	INIT_PTR(m_pScrollBar);

	m_vWriteLineCount = 0;
	m_vLineGap = 0;
	m_pWriteEditor = NULL;
	m_bEnableFocus = TRUE;
}

CDialog_BoardWrite::~CDialog_BoardWrite()
{
	SAFE_DELETE(m_pScrollBar);
	if(m_pWriteEditor) delete m_pWriteEditor;
}

BOOL CDialog_BoardWrite::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex) {
		case 0: // 여기서 올려 보내자..
			Write();
			break;
		case 1: // 이건 취소고...
			HideWindow();
			break;
		case 2:
			m_pScrollBar->ScrollUp();// Scroll Up
			m_pWriteEditor->LineScroll(-1);
			break;
		case 3:
			m_pScrollBar->ScrollDown();// Scroll Down
			m_pWriteEditor->LineScroll(1);
			break;
		}
	}
	else if(nSpec == AREA_CTRL)
	{
		switch(nIndex)
		{
		case 0:
			m_pMainEditor->SetFocus();
			break;
		case 1:
			m_pWriteEditor->SetFocus();
			break;
		}
	}


	return TRUE;
}

BOOL CDialog_BoardWrite::Draw(CDDSurface *pDest)
{
	CUIRes::Draw(pDest);

	Draw3ButtonCtrl(pDest, 0, 3);

	SetScrollData();

	m_pMainEditor->DCDrawOffsetedCurText(pDest, m_ptOffset);
	m_pWriteEditor->DCDrawOffsetedCurText(pDest, m_ptOffset);


	m_pScrollBar->Draw(pDest);

	return TRUE;
}

BOOL CDialog_BoardWrite::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	CRect t_rect;

	m_pMainEditor = new BBDX_Editor(m_pWnd, this, FALSE, m_arrArea[0]->m_vRect);
	m_pMainEditor->m_bChat = FALSE;
	m_pWriteEditor = new BBDX_Editor(m_pWnd, this, TRUE, m_arrArea[1]->m_vRect, FALSE, LINE_GAP);


	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create(this, m_arrArea[2]->m_vRect, 0);
	

	return TRUE;
}

void CDialog_BoardWrite::InitWriter()
{
	m_pMainEditor->ClearTexts();
	m_pWriteEditor->ClearTexts();

	m_pMainEditor->SetFocus();

	m_pScrollBar->InitScroll();
}


void CDialog_BoardWrite::SetScrollData()
{
	int max_count, rect_height, line_count;
	CSize t_size;

	line_count = m_pWriteEditor->GetLineCount();
	if(m_vWriteLineCount != line_count) 
	{
		m_vWriteLineCount = line_count;
		t_size = m_pWriteEditor->GetFontSize();
		m_vLineGap = t_size.cy + LINE_GAP;
		rect_height = m_arrArea[1]->m_vRect.Height();// 321-65; // 내용을 표시하는 영역의 높이
		max_count = rect_height / m_vLineGap;

		m_pScrollBar->SetMaxLineCount(m_pWriteEditor->GetLineCount());
		m_pScrollBar->SetViewLineCount(max_count);
	}
	
	int nOffset = m_pWriteEditor->GetFirstVisibleLine();

	int tmp = m_pScrollBar->GetStartLine();
	if (tmp != nOffset) {
		m_pScrollBar->SetStartLine(nOffset);
	}
}


BOOL CDialog_BoardWrite::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if (m_pScrollBar->OnLButtonDown(pMouseInfo) ) {
		int nOffset = m_pScrollBar->GetStartLine();
		int nVisible = m_pWriteEditor->GetFirstVisibleLine();
		if (nOffset != nVisible) {
			m_pWriteEditor->LineScroll(nOffset-nVisible);
		}

		SetFocus();
		SetCapture();
		return FALSE;
	}

	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_BoardWrite::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();
	if (m_pScrollBar->OnLButtonUp(pMouseInfo) ) {
		int nOffset = m_pScrollBar->GetStartLine();
		int nVisible = m_pWriteEditor->GetFirstVisibleLine();
		if (nOffset != nVisible) {
			m_pWriteEditor->LineScroll(nOffset-nVisible);
		}

		return FALSE;
	}

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_BoardWrite::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if (m_pScrollBar->OnMouseMove(pMouseInfo) ) {
		int nOffset = m_pScrollBar->GetStartLine();
		int nVisible = m_pWriteEditor->GetFirstVisibleLine();
		if (nOffset != nVisible) {
			m_pWriteEditor->LineScroll(nOffset-nVisible);
		}
	
		return FALSE;
	}

	return CUIRes::OnMouseMove(pMouseInfo);
}


void CDialog_BoardWrite::Write()
{
	CString strTitle;
	CString strContent;

	m_pMainEditor->GetWindowText(strTitle);
	m_pWriteEditor->GetWindowText(strContent);

	if (strTitle.IsEmpty() || strContent.IsEmpty()) 
		return;

	int nLenTitle = strTitle.GetLength();
	int nLenContent = strContent.GetLength();


	BYTE *buf = new BYTE[nLenTitle+nLenContent+20];
	int offset = 0;

	CPacketParser::SetByte(buf, offset, BBS_WRITE); 
	CPacketParser::SetShort(buf, offset, (SHORT)m_bpBoardCtrl->GetBoardNum());


	CPacketParser::SetByte(buf, offset, (BYTE)nLenTitle);
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)strTitle, nLenTitle); 

	CPacketParser::SetShort(buf, offset, (short)nLenContent);
	CPacketParser::SetString(buf, offset, (PBYTE)(LPCTSTR)strContent, nLenContent); 

		
	CChildView::SocketMng.Send(offset, buf);

	delete []buf;

	HideWindow();
}

void CDialog_BoardWrite::ShowWindow()
{
	if(!IsLoading())
	{
		if (!Create(CChildView::DirectXMng.m_pDD, "B-Write.lib", 0, TRUE, FALSE, TRUE)) {
			return;
		}
	}

	CUIRes::ShowWindow();
}

void CDialog_BoardWrite::HideWindow()
{
	KillFocus();
	m_bpBoardCtrl->DrawMode(BOARDDRAW_LIST);
}

void CDialog_BoardWrite::HideAllWindow()
{
	m_bpBoardCtrl->DrawMode(BOARDDRAW_NONE);

	CUIRes::HideWindow();
}

BOOL CDialog_BoardWrite::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_TAB)
	{
		if(m_pMainEditor->GetFocusStatus()) m_pWriteEditor->SetFocus();
		else m_pMainEditor->SetFocus();

		return TRUE;
	}
	else if(nChar == VK_RETURN)
	{
		if(m_pMainEditor->GetFocusStatus()) m_pWriteEditor->SetFocus();
		return TRUE;
	}

	return FALSE;
}

void CDialog_BoardWrite::SetFocus()
{
	if(m_bEnableFocus)
	{
		if(m_pWriteEditor)
		{
			g_pFocusDialog = this;
			m_pWriteEditor->SetFocus();
		}
	}
}
