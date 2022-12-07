// Dialog_KrBoard.cpp: implementation of the CDialog_BoardCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_BoardCtrl.h"

#include "Dialog_BoardList.h"
#include "Dialog_BoardRead.h"
#include "Dialog_BoardWrite.h"
#include "PacketParser.h"

extern CUIRes *g_pShowDialog;
extern CUIRes *g_pFocusDialog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_BoardCtrl::CDialog_BoardCtrl()
{
	INIT_PTR(m_pBoardList);
	INIT_PTR(m_pBoardRead);
	INIT_PTR(m_pBoardWrite);

	INIT_PTR(m_pDD);
	INIT_PTR(m_pWnd);

	INIT_PTR(m_pDrawBoard);

	m_nBoardNum = 0;
	m_nBoardSpec = BOARD_NORMAL;
	m_nDrawMode = 0;

	m_bInit = FALSE;
}

CDialog_BoardCtrl::~CDialog_BoardCtrl()
{
	SAFE_DELETE(m_pBoardList);
	SAFE_DELETE(m_pBoardRead);
	SAFE_DELETE(m_pBoardWrite);

	RemoveAllSaveList();
}


void CDialog_BoardCtrl::Draw(CDDSurface *pDest)
{
	if (m_pDrawBoard) {
		m_pDrawBoard->Draw(pDest);
	}
}



BOOL CDialog_BoardCtrl::InitData(CDirectDraw *pDD, CChildView *pWnd)
{
	m_pDD = pDD;
	m_pWnd = pWnd;

	m_pBoardList = new CDialog_BoardList((CWnd *)m_pWnd);
	m_pBoardList->m_bpBoardCtrl = this;
	m_pBoardList->SetOffset(CPoint(150,100));

	m_pBoardRead = new CDialog_BoardRead((CWnd *)m_pWnd);
	m_pBoardRead->m_bpBoardCtrl = this;
	m_pBoardRead->SetOffset(CPoint(150,100));

	m_pBoardWrite = new CDialog_BoardWrite((CWnd *)m_pWnd);
	m_pBoardWrite->m_bpBoardCtrl = this;
	m_pBoardWrite->SetOffset(CPoint(150,100));

	DrawMode(BOARDDRAW_NONE);
	m_bInit = TRUE;

	TRACE("####### BoardCtrl Init 성공.. ############ \n");
	return TRUE;
}


int CDialog_BoardCtrl::BoardMouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	if (m_pDrawBoard) {
		if(m_pDrawBoard->GetCaptureStatus() || m_pDrawBoard->PtInUIRes(pMouseInfo->GetPos()))
		{
			if(pMouseInfo->IsLButtonDobleDown())
			{
				m_pDrawBoard->OnLButtonDblClk(pMouseInfo);
			}
			if(pMouseInfo->IsLButtonDown()) {
				m_pDrawBoard->OnLButtonDown(pMouseInfo);
			}
			if(pMouseInfo->IsMouseMove()) {
				m_pDrawBoard->OnMouseMove(pMouseInfo);
			}
			if(pMouseInfo->IsLButtonUp()) {
				m_pDrawBoard->OnLButtonUp(pMouseInfo);
			}
			return 2;
		}
		return 1; // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

void CDialog_BoardCtrl::DrawMode(int nMode)
{
	m_nDrawMode = nMode;
	switch(nMode){
	case BOARDDRAW_NONE:
		m_pDrawBoard = NULL;
		m_pBoardList->InitLister();
		RemoveAllSaveList();
		m_pBoardWrite->KillFocus();
		break;
	case BOARDDRAW_LIST:
		//m_pBoardList->InitLister();
		m_pBoardList->ShowWindow();		// 순서 지킬 것
		g_pFocusDialog = g_pShowDialog = m_pDrawBoard = m_pBoardList;
		break;
	case BOARDDRAW_READ:
		m_pBoardRead->ShowWindow();
		m_pBoardRead->InitReader();
		g_pFocusDialog = m_pDrawBoard = m_pBoardRead;
		break;
	case BOARDDRAW_WRITE:
		m_pBoardWrite->ShowWindow();
		m_pBoardWrite->InitWriter();
		g_pFocusDialog = m_pDrawBoard = m_pBoardWrite;
		break;
	default:
		ASSERT(FALSE);
		m_pDrawBoard = NULL;
		break;
	}

}

void CDialog_BoardCtrl::RemoveAllSaveList()
{
	int nSize = m_arrList.GetSize();
	for (int i=0; i<nSize; i++) {
		delete m_arrList.GetAt(i);
	}
	
	m_arrList.RemoveAll();

}

void CDialog_BoardCtrl::BBS_PKT_Process(PBYTE bypData)
{
	if (!m_bInit) {
		return;
	}
	ASSERT(m_pBoardList);
	ASSERT(m_pBoardRead);
	ASSERT(m_pBoardWrite);

	int offset = 0;
	switch(bypData[offset++]) {
	case BBS_OPEN:
		RemoveAllSaveList();
		BBS_SetOpen(bypData+offset);
		DrawMode(BOARDDRAW_LIST);
		m_pBoardList->ShowInList(0);
		break;
	case BBS_NEXT:
		if(BBS_SetList(bypData+offset) == TRUE)
		{
			if (m_nDrawMode == BOARDDRAW_LIST) {
				// ShowInList에서 StartIndex는 Check한다.
				m_pBoardList->ShowInList(m_pBoardList->m_nStartIndex+10);
			}
			else if (m_nDrawMode == BOARDDRAW_READ) {
				m_pBoardRead->m_bProcessing = FALSE;
				m_pBoardRead->ReadNext();
			}
		}
		else
		{
			if(m_nDrawMode == BOARDDRAW_READ) m_pBoardRead->m_bProcessing = FALSE;
		}
		break;
	case BBS_READ:
		BBS_SetRead(bypData+offset);

		break;
	case BBS_WRITE:
		break;
	case BBS_EDIT:
		break;
	case BBS_DELETE:
		break;
	default:
		ASSERT(FALSE);
	}
}

void CDialog_BoardCtrl::BBS_SetOpen(PBYTE bypData)
{
	int offset=0;
	int nNum = CPacketParser::GetShort(bypData, offset); // bbs 번호
	m_nBoardSpec = CPacketParser::GetByte(bypData, offset); // bbs 권한

	if (nNum != m_nBoardNum) {
		m_nBoardNum = nNum;

		m_pBoardList->InitLister();
		RemoveAllSaveList();

	}

	m_pBoardList->SetList(bypData+offset);
}

BOOL CDialog_BoardCtrl::BBS_SetList(PBYTE bypData)
{
	int offset=0;
	int nNum = CPacketParser::GetShort(bypData, offset); // bbs 번호

	if (nNum != m_nBoardNum) {
		m_nBoardNum = nNum;

		m_pBoardList->InitLister();
		RemoveAllSaveList();

	}

	return m_pBoardList->SetList(bypData+offset);
}

void CDialog_BoardCtrl::BBS_SetRead(PBYTE bypData)
{
	int offset=0;
	int nNum = CPacketParser::GetShort(bypData, offset); // bbs 번호, 
	ASSERT(nNum == m_nBoardNum);

	DrawMode(BOARDDRAW_READ);

	m_pBoardRead->m_bProcessing = FALSE;
	m_pBoardRead->SetRead(bypData+offset);
}




BOOL CDialog_BoardCtrl::IsUnique(int nNum)
{
	int nSize = m_arrList.GetSize();
	for (int count=0; count<nSize; count++) {
		if (m_arrList.GetAt(count)->m_nNum == nNum) {
			return FALSE;
		}
	}

	return TRUE;
}
