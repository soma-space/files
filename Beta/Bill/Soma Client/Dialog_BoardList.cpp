// Dialog_BoardList.cpp: implementation of the CDialog_BoardList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dialog_BoardList.h"

#include "Dialog_BoardCtrl.h"
//#include "BBDX_TextOuter.h"

#include "PacketParser.h"

extern CUIRes *g_pShowDialog;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_BoardList::CDialog_BoardList(CWnd *pWnd)
{
	INIT_PTR(m_bpBoardCtrl);
	m_pFont = NULL;
	m_nSelRow = -1;

	m_nStartIndex = 0;
	m_nVListCount = 0;
	m_bProcessing = FALSE;
//	m_pListOuter[0][0] = NULL;
}

CDialog_BoardList::~CDialog_BoardList()
{
/*	if(m_pListOuter[0][0])
	{
		int i, j;

		for(i = 0; i < 10; i++) {
			for(j = 0; j < 4; j++)	{
				delete m_pListOuter[i][j];
			}
		}
	}*/
	if(m_pFont) delete m_pFont;
}

BOOL CDialog_BoardList::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		switch(nIndex) {
		case 0:	// 이전.
			ListBack();
			break;
		case 1:	// 다음. 
			ListNext();
			break;
		case 2: // 읽기
			Read();
			break;
		case 3: // 쓰기
			if(m_bpBoardCtrl->m_nBoardSpec != BOARD_READONLY) m_bpBoardCtrl->DrawMode(BOARDDRAW_WRITE);
			break;
		case 4: // 종료
			HideWindow();
			break;
		}
	}
	else if(nSpec == AREA_CTRL)
	{
		m_nSelRow = nIndex;
	}

	return TRUE;
}

BOOL CDialog_BoardList::Draw(CDDSurface *pDest)
{
	CUIRes::Draw(pDest);

	int i;

	Draw3ButtonCtrl(pDest, 0, 4);

	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	COLORREF old_color = pDC->SetTextColor(RGB(255,255,255));
	int mode = pDC->SetBkMode(TRANSPARENT);

	for(i = 0; i < m_nVListCount; i++)
	{
		CRect rtDst = m_arrArea[i]->m_vRect;
		CPoint ptOffset;

		rtDst.OffsetRect(m_ptOffset);
		if(m_nSelRow == i)
		{
//			m_pListOuter[i][0]->DrawShadowTexts(pDest, rtDst, 0xffe0, 3);

//			rtDst.left += 45;
//			m_pListOuter[i][1]->DrawShadowTexts(pDest, rtDst, 0xffe0, 3);

//			rtDst.left += 90;
//			m_pListOuter[i][2]->DrawShadowTexts(pDest, rtDst, 0xffe0, 3);

//			rtDst.left += 65;
//			m_pListOuter[i][3]->DrawShadowTexts(pDest, rtDst, 0xffe0, 3);
			pDC->SetTextColor(RGB(255,255,0));
		}
		else
		{
//			m_pListOuter[i][0]->DrawTexts(pDest, rtDst, 0xffff, 3);

//			rtDst.left += 45;
//			m_pListOuter[i][1]->DrawTexts(pDest, rtDst, 0xffff, 3);

//			rtDst.left += 90;
//			m_pListOuter[i][2]->DrawTexts(pDest, rtDst, 0xffff, 3);

//			rtDst.left += 65;
//			m_pListOuter[i][3]->DrawTexts(pDest, rtDst, 0xffff, 3);
			pDC->SetTextColor(RGB(255,255,255));
		}

		pDC->DrawText(m_ListString[i][0], &rtDst, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL);
		rtDst.left += 45;
		pDC->DrawText(m_ListString[i][1], &rtDst, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL);
		rtDst.left += 90;
		pDC->DrawText(m_ListString[i][2], &rtDst, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL);
		rtDst.left += 65;
		pDC->DrawText(m_ListString[i][3], &rtDst, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_EDITCONTROL);
	}

	pDC->SetTextColor(old_color);
	pDC->SetBkMode(mode);
	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	return TRUE;
}



BOOL CDialog_BoardList::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}


	ASSERT(m_bpBoardCtrl);
/*	int i, j;

	for (i=0; i<10; i++) {
		for (j=0; j<4; j++)	{
			m_pListOuter[i][j] = new BBDX_TextOuter(m_bpBoardCtrl->m_pTWFont, 14, 14);
		}
	}*/
	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	return TRUE;
}

void CDialog_BoardList::InitLister()
{
	if(IsLoading())
	{
//		CString t_str;
		// 우선 초기화 하구...
//		t_str = _T("");
		for (int i=0; i<10; i++)
		{
			m_ListString[i][0].Empty();
			m_ListString[i][1].Empty();
			m_ListString[i][2].Empty();
			m_ListString[i][3].Empty();
//			m_pListOuter[i][0]->SetString(t_str);
//			m_pListOuter[i][1]->SetString(t_str);
//			m_pListOuter[i][2]->SetString(t_str);
//			m_pListOuter[i][3]->SetString(t_str);
		} 

		m_nSelRow = -1;
	}
}


void CDialog_BoardList::ListBack()
{
	m_nStartIndex -= 10;
	if (m_nStartIndex<0)
		m_nStartIndex = 0;

	ShowInList(m_nStartIndex);
}

void CDialog_BoardList::ListNext()
{
	ARR_LIST *pArrList = &(m_bpBoardCtrl->m_arrList);

	int nSize = pArrList->GetSize();

	if ((m_nStartIndex+10)<nSize) {
		m_nStartIndex+=10;
		ShowInList(m_nStartIndex);
	}
	else {
		// Server에서 받아옴.
		if(m_bProcessing == FALSE)
		{
			m_bProcessing = TRUE;

			BYTE buf[10];
			int offset = 0;
			CPacketParser::SetByte(buf, offset, BBS_NEXT); //최종점 요청
			CPacketParser::SetShort(buf, offset, (SHORT)m_bpBoardCtrl->GetBoardNum());

			CPacketParser::SetInt(buf, offset, pArrList->GetAt(nSize-1)->m_nNum);
			CChildView::SocketMng.Send(offset, buf);
		}
	}
}


BOOL CDialog_BoardList::SetList(PBYTE bypData)
{
	CString t_str;

	int offset = 0;
	int nLen = 0;
	short nListNum = CPacketParser::GetShort(bypData, offset); // 
	if(nListNum == -1) return FALSE;
	ASSERT(nListNum>=0 && nListNum <= 10);

	ARR_LIST *pArrList = &(m_bpBoardCtrl->m_arrList);

	m_bProcessing = FALSE;
	for (int i=0; i<nListNum; i++) {
		short num = CPacketParser::GetInt(bypData, offset);  //글번호.

		// 유일성 검사....
		if (m_bpBoardCtrl->IsUnique(num)==FALSE) {
			continue;
		}
		/////////////////////////////////////


		LIST_DATA *pData = new LIST_DATA();
		pData->m_nNum = num;
		
		// 글쓴이 ID. / 글쓴 시각 / 글 제목..
		for (int j=0; j<3; j++) {
			int nLen = CPacketParser::GetByte(bypData, offset);
			CString str;
			LPSTR lpStr = str.GetBufferSetLength(nLen);
			CPacketParser::GetString(bypData, offset, (PBYTE)lpStr, nLen);
			str.ReleaseBuffer();
			t_str = _T(lpStr);

			pData->m_strData[j] = t_str;
		}
		pArrList->Add(pData);
	}
	return TRUE;
}


void CDialog_BoardList::Read()
{
	if (m_nSelRow>=0 && m_nSelRow<10) {
//		CString strListNum = m_pListOuter[m_nSelRow][0]->GetString(0);
//		if (strListNum != _T("")) {
		if(m_ListString[m_nSelRow][0].IsEmpty() == FALSE)
		{
			BYTE buf[10];
			int offset = 0;
			CPacketParser::SetByte(buf, offset, BBS_READ); //최종점 요청
			CPacketParser::SetShort(buf, offset, (short)m_bpBoardCtrl->GetBoardNum()); //최종점 요청
//			int nIndexNum = atoi(strListNum);
			int nIndexNum = atoi(m_ListString[m_nSelRow][0]);
			CPacketParser::SetInt(buf, offset, nIndexNum);

			CChildView::SocketMng.Send(offset, buf);
		}
	}
}


void CDialog_BoardList::ShowInList(int nStartIndex)
{
	ASSERT(m_bpBoardCtrl);

	//m_bpBoardCtrl->DrawMode(BOARDDRAW_LIST);

	ARR_LIST *pArrList = &(m_bpBoardCtrl->m_arrList);

	int nSize = pArrList->GetSize();


	// m_nStartIndex를 nSize에 맟추어 보정한다.
	if (nStartIndex<(nSize-10)) {
		m_nStartIndex = nStartIndex;
	}
	else {
		m_nStartIndex = nSize-10;
		if (m_nStartIndex<0)
			m_nStartIndex = 0;
	}

		
	InitLister();
	m_nVListCount = 0;
	for (int i=m_nStartIndex; (i<nSize && (i-m_nStartIndex)<10); i++) {
		CString t_str;
		LIST_DATA *pData = pArrList->GetAt(i);

//		t_str.Format("%d", pData->m_nNum);
//		m_pListOuter[i-m_nStartIndex][0]->SetString(t_str);
		m_ListString[i-m_nStartIndex][0].Format("%d", pData->m_nNum);
		
		// 글쓴이 ID. / 글쓴 시각 / 글 제목..
		for (int j=1; j<4; j++) {
//			t_str = pData->m_strData[j-1];
//			m_pListOuter[i-m_nStartIndex][j]->SetString(t_str);
			m_ListString[i-m_nStartIndex][j] = pData->m_strData[j-1];
		}
		m_nVListCount++;
	}
}

void CDialog_BoardList::ShowWindow()
{
	if(!IsLoading())
	{
		if (!Create(CChildView::DirectXMng.m_pDD, "B-List.lib", 0, TRUE, FALSE, TRUE)) {
			return;
		}
	}

	if(g_pShowDialog && g_pShowDialog != this) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;
	
	m_bProcessing = FALSE;
	CUIRes::ShowWindow();
}

void CDialog_BoardList::HideWindow()
{
	g_pShowDialog = NULL;
	HideAllWindow();
}

void CDialog_BoardList::HideAllWindow()
{
	m_bpBoardCtrl->DrawMode(BOARDDRAW_NONE);

	CUIRes::HideWindow();
}

int CDialog_BoardList::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	if(m_bShow && m_pChild == NULL)
	{
		if(GetCaptureStatus() || PtInUIRes(pMouseInfo->GetPos()))
		{
			if(pMouseInfo->IsLButtonDobleDown())
			{
				OnLButtonDblClk(pMouseInfo);
			}
			if(pMouseInfo->IsLButtonDown()) {
				OnLButtonDown(pMouseInfo);
			}
			if(pMouseInfo->IsMouseMove()) {
				OnMouseMove(pMouseInfo);
			}
			if(pMouseInfo->IsLButtonUp()) {
				OnLButtonUp(pMouseInfo);
			}
			return 2;
		}
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

BOOL CDialog_BoardList::OnLButtonDblClk(BBDX_MouseInformation *pMouseInfo)
{
	OnLButtonDown(pMouseInfo);
	if(m_vSpecCtrl == AREA_CTRL && m_vIndexCtrl >= 0 && m_vIndexCtrl <= 9)
	{
		m_nSelRow = m_vIndexCtrl;
		Read();
		return TRUE;
	}
	return FALSE;
}