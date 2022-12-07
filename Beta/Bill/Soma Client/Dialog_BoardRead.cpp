// Dialog_BoardRead.cpp: implementation of the CDialog_BoardRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"

#include "Dialog_BoardRead.h"

#include "Dialog_BoardCtrl.h"
#include "Dialog_BoardList.h"
#include "BBDX_Editor.h"
//#include "BBDX_TextOuterWithScroll.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CUIRes *g_pFocusDialog;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_BoardRead::CDialog_BoardRead(CWnd *pWnd)
{
	m_pWnd = pWnd;

	INIT_PTR(m_bpBoardCtrl);
	INIT_PTR(m_pScrollBar);

	m_nReadIndex = 0;
//	m_pReadOuter[0] = NULL;
	m_vLineGap = 0;
	m_vReadLineCount = 0;
	m_pFont = NULL;
	m_vTotalLine = 0;
	m_bProcessing = FALSE;
	m_bEnableFocus = TRUE;
}

CDialog_BoardRead::~CDialog_BoardRead()
{
	SAFE_DELETE(m_pScrollBar);

//	if(m_pReadOuter[0])
//	{
//		int i;

//		for(i = 0; i < 3; i++) delete m_pReadOuter[i];
//	}
	if(m_pFont) delete m_pFont;
}

BOOL CDialog_BoardRead::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex) {
		case 0:	// 이전
			ReadBack();
			break;
		case 1:	// 다음.
			ReadNext();
			break;
		case 2:	// 삭제.
			if(m_bpBoardCtrl->m_nBoardSpec == BOARD_NORMAL || m_bpBoardCtrl->m_nBoardSpec == BOARD_FULL) Delete();
			break;
		case 3:
			HideWindow();
			break;
		case 4:
			m_pScrollBar->ScrollUp();
			m_pMainEditor->LineScroll(-1);
			break;
		case 5:
			m_pScrollBar->ScrollDown();
			m_pMainEditor->LineScroll(1);
			break;
		}
	}

	return TRUE;
}

BOOL CDialog_BoardRead::Draw(CDDSurface *pDest)
{
	CUIRes::Draw(pDest);

	CRect rtDst;

	Draw3ButtonCtrl(pDest, 0, 5);

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	COLORREF old_color = pDC->SetTextColor(RGB(255,255,0));
	int mode = pDC->SetBkMode(TRANSPARENT);

	rtDst = m_arrArea[0]->m_vRect;		// 번호
	rtDst.OffsetRect(m_ptOffset);
	pDC->DrawText(m_ReadString[0], &rtDst, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL);
//	m_pReadOuter[0]->DrawTexts(pDest, rtDst, 0xffe0, LINE_GAP);

	rtDst = m_arrArea[1]->m_vRect;		// 제목
	rtDst.OffsetRect(m_ptOffset);
	pDC->DrawText(m_ReadString[1], &rtDst, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL);
//	m_pReadOuter[1]->DrawTexts(pDest, rtDst, 0xffe0, LINE_GAP);

/*	rtDst = m_arrArea[2]->m_vRect;		// 내용
	rtDst.OffsetRect(m_ptOffset);
	
	CRgn t_rgn;
	CRect t_rect = rtDst;
	int char_height;
	TEXTMETRIC Metrics;
	t_rgn.CreateRectRgnIndirect((LPCRECT)rtDst);
	pDC->SelectClipRgn(&t_rgn);
	pDC->GetTextMetrics(&Metrics);
	char_height = Metrics.tmHeight + Metrics.tmExternalLeading;
	t_rect.top -= (m_pScrollBar->GetStartLine() * char_height);
	pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(m_ReadString[2], &t_rect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EDITCONTROL|DT_EXTERNALLEADING);
//	m_pReadOuter[2]->DrawTextsFromIndexedLine(pDest, rtDst, m_pScrollBar->GetStartLine(), 0xffff, LINE_GAP); // 내용*/

	pDC->SetTextColor(old_color);
	pDC->SetBkMode(mode);
	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	m_pMainEditor->DCDrawOffsetedCurText(pDest, m_ptOffset);

	m_pScrollBar->Draw(pDest);

	return TRUE;
}

BOOL CDialog_BoardRead::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

/*	for (int i=0; i<3; i++) m_pReadOuter[i] = new BBDX_TextOuter(m_bpBoardCtrl->m_pTWFont, 14, 14);

	m_pReadOuter[2]->SetSplitRegion(m_arrArea[2]->m_vRect.Width());*/
	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	m_pMainEditor = new BBDX_Editor(m_pWnd, this, TRUE, m_arrArea[2]->m_vRect, TRUE, LINE_GAP);

	m_pScrollBar = new CUIScrollBar();
	m_pScrollBar->Create((CUIRes*)this, m_arrArea[3]->m_vRect, 0);
	
		
	m_nReadIndex = 0;
	return TRUE;

}

void CDialog_BoardRead::InitReader()
{
	CString t_str;

//	m_pReadOuter[0]->ClearString();
//	m_pReadOuter[1]->ClearString();
//	m_pReadOuter[2]->ClearString();
	m_ReadString[0].Empty();
	m_ReadString[1].Empty();
	m_ReadString[2].Empty();
	if(m_pMainEditor) m_pMainEditor->SetWindowText("");
	
	m_pScrollBar->InitScroll();

	m_nReadIndex = 0;
	SetScrollData();
}

void CDialog_BoardRead::SetScrollData()
{
	int max_count, rect_height, line_count;
	CSize t_size;

	line_count = m_pMainEditor->GetLineCount();
	if(m_vReadLineCount != line_count) 
	{
		m_vReadLineCount = line_count;
		t_size = m_pMainEditor->GetFontSize();
		m_vLineGap = t_size.cy + LINE_GAP;
		rect_height = m_arrArea[2]->m_vRect.Height();// 321-65; // 내용을 표시하는 영역의 높이
		max_count = rect_height / m_vLineGap;

		m_pScrollBar->SetMaxLineCount(m_pMainEditor->GetLineCount());
		m_pScrollBar->SetViewLineCount(max_count);
	}
	
	int nOffset = m_pMainEditor->GetFirstVisibleLine();

	int tmp = m_pScrollBar->GetStartLine();
	if (tmp != nOffset) {
		m_pScrollBar->SetStartLine(nOffset);
	}
}

BOOL CDialog_BoardRead::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	if (m_pScrollBar->OnLButtonDown(pMouseInfo) )
	{
		int nOffset = m_pScrollBar->GetStartLine();
		int nVisible = m_pMainEditor->GetFirstVisibleLine();
		if (nOffset != nVisible) {
			m_pMainEditor->LineScroll(nOffset-nVisible);
		}

		SetFocus();
		SetCapture();
		return FALSE;
	}

	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_BoardRead::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	ReleaseCapture();
	if (m_pScrollBar->OnLButtonUp(pMouseInfo) )
	{
		int nOffset = m_pScrollBar->GetStartLine();
		int nVisible = m_pMainEditor->GetFirstVisibleLine();
		if (nOffset != nVisible) {
			m_pMainEditor->LineScroll(nOffset-nVisible);
		}

		return FALSE;
	}

	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_BoardRead::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if (m_pScrollBar->OnMouseMove(pMouseInfo) ) {
		int nOffset = m_pScrollBar->GetStartLine();
		int nVisible = m_pMainEditor->GetFirstVisibleLine();
		if (nOffset != nVisible) {
			m_pMainEditor->LineScroll(nOffset-nVisible);
		}
	
		return FALSE;
	}

	return CUIRes::OnMouseMove(pMouseInfo);
}


void CDialog_BoardRead::SetRead(PBYTE bypData)
{
	int offset = 0;
	int nNum = CPacketParser::GetInt(bypData, offset); // 글 번호.

	if(nNum == -1) return;

	ARR_LIST *pArrList = &(m_bpBoardCtrl->m_arrList);

	int nSize = pArrList->GetSize();
	for (int i=0; i<nSize; i++) {
		if (pArrList->GetAt(i)->m_nNum == nNum) {
			m_nReadIndex = i;
			break;
		}
	}
	// 저장된 목록에 없을 경우...
	if (i>=nSize) {	
		ASSERT(FALSE);
		return;
	}
	// 저장된 목록에 있으면 ViewList를 Check해 본다.
	else {
		m_nReadIndex;
		int nStartViewIndex = m_bpBoardCtrl->m_pBoardList->m_nStartIndex;

		if (m_nReadIndex < nStartViewIndex) {
			m_bpBoardCtrl->m_pBoardList->ShowInList(nStartViewIndex-10);	// 내부에서 값은 조정한다.
		}
		else if (m_nReadIndex > (nStartViewIndex+9)) {
			m_bpBoardCtrl->m_pBoardList->ShowInList(nStartViewIndex+10);	// 내부에서 값은 조정한다.
		}

		// ListView의 SelectRow변경..
		nStartViewIndex = m_bpBoardCtrl->m_pBoardList->m_nStartIndex;
		m_bpBoardCtrl->m_pBoardList->m_nSelRow = m_nReadIndex-nStartViewIndex;

	}

	LIST_DATA *pData = pArrList->GetAt(i);
	CString t_str;

	m_ReadString[0].Format("%d", pData->m_nNum);
	m_ReadString[1] = pData->m_strData[2];
//	m_pReadOuter[0]->SetString(t_str);
//	m_pReadOuter[1]->SetString(pData->m_strData[2]);

	
	int nLen = CPacketParser::GetShort(bypData, offset);
	ASSERT(nLen>=0);
	CString str;
	LPSTR lpStr = str.GetBufferSetLength(nLen);
	CPacketParser::GetString(bypData, offset, (PBYTE)lpStr, nLen);
	str.ReleaseBuffer();
	t_str = _T(lpStr);

	CString strMsg;
	int nIndex = 0;
	BOOL bContinue = TRUE;

//	m_pReadOuter[2]->ClearString();
	m_ReadString[2].Empty();

/*	do {
		AfxExtractSubString(strMsg, t_str, nIndex, '\n');
		nIndex++;

		if (strMsg.IsEmpty() == FALSE) {
			m_pReadOuter[2]->AddString(strMsg);
		}
		else {
			bContinue = FALSE;
		}
	} while (bContinue);*/
	m_ReadString[2] = t_str;

	// 전체 줄 수를 구하기 위해서
/*	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	CRect t_rect = m_arrArea[2]->m_vRect;

	pDC->DrawText(m_ReadString[2], &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_CALCRECT|DT_WORDBREAK);
	m_vTotalLine = t_rect.Height() / 12;

	pDC->SelectObject(old_font);
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);*/

	m_pMainEditor->SetWindowText(m_ReadString[2]);

	SetScrollData();
}



void CDialog_BoardRead::ReadBack()
{
	if(m_bProcessing == TRUE) return;
	ARR_LIST *pArrList = &(m_bpBoardCtrl->m_arrList);

	int nSize = pArrList->GetSize();
	if (m_nReadIndex >0) {
		m_bProcessing = TRUE;
		int nIndexNum = pArrList->GetAt(m_nReadIndex-1)->m_nNum;

		BYTE buf[10];
		int offset = 0;
		CPacketParser::SetByte(buf, offset, BBS_READ); //최종점 요청
		CPacketParser::SetShort(buf, offset, (short)m_bpBoardCtrl->GetBoardNum()); //최종점 요청
		CPacketParser::SetInt(buf, offset, nIndexNum);

		CChildView::SocketMng.Send(offset, buf);
	}
}

void CDialog_BoardRead::ReadNext()
{
	if(m_bProcessing == TRUE) return;
	m_bProcessing = TRUE;
	ARR_LIST *pArrList = &(m_bpBoardCtrl->m_arrList);

	int nSize = pArrList->GetSize();
	if (m_nReadIndex<(nSize-1)) {
		int nIndexNum = pArrList->GetAt(m_nReadIndex+1)->m_nNum;

		BYTE buf[10];
		int offset = 0;
		CPacketParser::SetByte(buf, offset, BBS_READ); //최종점 요청
		CPacketParser::SetShort(buf, offset, (short)m_bpBoardCtrl->GetBoardNum()); //최종점 요청
		CPacketParser::SetInt(buf, offset, nIndexNum);

		CChildView::SocketMng.Send(offset, buf);
	}
	else {
		ASSERT(m_nReadIndex==(nSize-1));
		int nIndexNum = pArrList->GetAt(m_nReadIndex)->m_nNum;

		// Server에서 받아옴. 
		BYTE buf[10];
		int offset = 0;
		CPacketParser::SetByte(buf, offset, BBS_NEXT); //최종점 요청
		CPacketParser::SetShort(buf, offset, (SHORT)m_bpBoardCtrl->GetBoardNum());
		CPacketParser::SetInt(buf, offset, nIndexNum);

		CChildView::SocketMng.Send(offset, buf);
	}
}

void CDialog_BoardRead::Delete()
{
	KillFocus();		// List로 가기때문에 KillFocus를 해야한다.

	BYTE buf[10];
	int offset = 0;
	CPacketParser::SetByte(buf, offset, BBS_DELETE); //최종점 요청
	CPacketParser::SetShort(buf, offset, (SHORT)m_bpBoardCtrl->GetBoardNum());
	ARR_LIST *pArrList = &(m_bpBoardCtrl->m_arrList);
	int nIndexNum = pArrList->GetAt(m_nReadIndex)->m_nNum;
	CPacketParser::SetInt(buf, offset, nIndexNum);

	CChildView::SocketMng.Send(offset, buf);
}

void CDialog_BoardRead::ShowWindow()
{
	if(IsLoading() == FALSE)
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "B-Read.lib", 0, TRUE, FALSE, TRUE)) {
			return;
		}
	}
	m_bProcessing = FALSE;
	InitReader();

	CUIRes::ShowWindow();
}

void CDialog_BoardRead::HideWindow()
{
	KillFocus();
	m_bpBoardCtrl->DrawMode(BOARDDRAW_LIST);
}

void CDialog_BoardRead::HideAllWindow()
{
	KillFocus();
	m_bpBoardCtrl->DrawMode(BOARDDRAW_NONE);
}