// UIRes.cpp: implementation of the CUIRes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"

#include "UIRes.h"
#include "BBDX_Editor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#include "DManage/BB_Direct.h"
#include "DManage/BB_Bitmap.h"
#include "DManage/BBDX_MouseInformation.h"

#define DLGLIB_HEADER		("DIALOG LIBRARY.")
#define DLGLIB_HEADERSIZE	15

extern CUIRes *g_pFocusDialog;
extern CUIRes *g_pCaptureDialog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIRes::CUIRes(CWnd* pParent)
{
	m_pDD = NULL;
	m_ptOffset = CPoint(0,0);
	m_vSpecCtrl = -1;
	m_vIndexCtrl = 0;

	m_bDragging = FALSE;
	m_ptDrag = m_ptOffset;
	m_bCapture = FALSE;

	m_szLibName = _T("");
	
	INIT_PTR(m_pDDSurface);
	m_bEnableFocus = FALSE;
	m_bTrans = FALSE;
	m_bModal = TRUE;
	m_pWnd = pParent;
	m_bLoad = FALSE;
	m_pParent = NULL;
	m_pChild = NULL;
	m_vPosMode = POS_NORMAL;
	m_bShow = FALSE;

	//INIT_PTR(m_pToolTips);
	INIT_PTR(m_pMainEditor);
}

CUIRes::~CUIRes()
{
	int size, i;

	delete m_pDDSurface;
	
	size = m_arrArea.GetSize();
	for(i = 0; i < size; i++)
	{
		SAFE_DELETE(m_arrArea[i]);
	}
	m_arrArea.RemoveAll();

	size = m_arrBtn.GetSize();
	for(i = 0; i < size; i++)
	{
		SAFE_DELETE(m_arrBtn[i]);
	}
	m_arrBtn.RemoveAll();
	
	size = m_arrBtnEx.GetSize();
	for(i = 0; i < size; i++)
	{
		SAFE_DELETE(m_arrBtnEx[i]);
	}
	m_arrBtnEx.RemoveAll();

	size = m_arrRectManager.GetSize();
	for(i = 0; i < size; i++)
	{
		SAFE_DELETE(m_arrRectManager[i]);
	}
	m_arrRectManager.RemoveAll();
	//SAFE_DELETE(m_pToolTips);
	SAFE_DELETE(m_pMainEditor);
}

BOOL CUIRes::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	m_pDD = pDD;
	ASSERT(m_pDD != NULL);

	m_vSpecCtrl = -1;
	m_vIndexCtrl = 0;

	m_ptDrag = m_ptOffset;
	m_bCapture = FALSE;

	ASSERT(szLibName.GetLength());
	m_szLibName = szLibName;//.Format("%sScreen\\%s", CDirectDraw::GetExecutionPath(), szLibName);
	
	m_pDDSurface = NULL;

	if (!LoadResource()) {
		return FALSE;
	}

	ASSERT(m_pDDSurface);
	m_pDDSurface->SetColorKey(RGB(255,0,255));

	ASSERT(nType>=0);
	m_nType = nType;
	m_bModal = bModal;
	m_bMoveable = bMoveable;
	m_bTrans = bTrans;
	m_bLoad = TRUE;

	return TRUE;
}


BOOL CUIRes::LoadResource()
{
	ASSERT(m_szLibName.GetLength() !=0);
	ASSERT(m_szLibName.GetLength() !=0);

	CString szExec = CDirectDraw::GetExecutionPath();
	CFile f;
	try {
		BOOL bOpen = FALSE;
		bOpen = f.Open(szExec+"dlg\\"+m_szLibName, CFile::modeRead);
		//ASSERT(bOpen);
		if (!bOpen) {
			TRACE("File(\"Dlg\\%s\")를 Load할 수 없습니다. \n", m_szLibName);
			return FALSE;
		}
	}
	catch(CFileException *e) {
		ASSERT(FALSE);
		e->Delete();
		return FALSE;
	}

	CArchive ar(&f, CArchive::load);
	
	CFile *file = ar.GetFile();
	char buf[DLGLIB_HEADERSIZE+1];
	file->Read(buf, DLGLIB_HEADERSIZE);
	buf[DLGLIB_HEADERSIZE] = 0;
	if ( strcmp(buf, DLGLIB_HEADER) == 0)
	{	// Lib
		char buf[12];
		long bitmapsize;
		int count, i;
		MAreaCtrl *area_ctrl;
		MBtnCtrl *btn_ctrl;
		MBtnCtrlEx *btn_ctrl_ex;
		MRectManager *rect_manager;
		
		memset(buf, 0, 12);
		file->Read(buf, 10);
		bitmapsize = atol(buf);

		m_pDDSurface = BB_Bitmap::CreateSurfaceFromLib(m_pDD, *file, 11+DLGLIB_HEADERSIZE, bitmapsize);	
		ASSERT(m_pDDSurface);
		
		// Size of Dlg
		ar >> m_mainRect.left >> m_mainRect.top >> m_mainRect.right >> m_mainRect.bottom;

		ar >> count;				// Area
		for(i = 0; i < count; i++)
		{
			area_ctrl = new MAreaCtrl;
			area_ctrl->Load(ar);
			area_ctrl->m_vIndex = m_arrArea.Add(area_ctrl);

			rect_manager = new MRectManager;
			rect_manager->m_pRect = &area_ctrl->m_vRect;
			rect_manager->m_pCtrl = area_ctrl;
			rect_manager->m_vIndex = 0x01;
			m_arrRectManager.Add(rect_manager);
		}

		ar >> count;
		for(i = 0; i < count; i++)
		{
			btn_ctrl = new MBtnCtrl();
			btn_ctrl->Load(ar);
			btn_ctrl->m_vIndex = m_arrBtn.Add(btn_ctrl);
			
			rect_manager = new MRectManager;
			rect_manager->m_pRect = &btn_ctrl->m_vRect;
			rect_manager->m_pCtrl = btn_ctrl;
			rect_manager->m_vIndex = 0x01;
			m_arrRectManager.Add(rect_manager);
		}
		
		ar >> count;
		for(i = 0; i < count; i++)
		{
			btn_ctrl_ex = new MBtnCtrlEx();
			btn_ctrl_ex->Load(ar);
			btn_ctrl_ex->m_vIndex = m_arrBtnEx.Add(btn_ctrl_ex);
			
			rect_manager = new MRectManager;
			rect_manager->m_pRect = &btn_ctrl_ex->m_vRect;
			rect_manager->m_pCtrl = btn_ctrl_ex;
			rect_manager->m_vIndex = 0x01;
			m_arrRectManager.Add(rect_manager);
		}
	}	
	else
	{
		ar.Close();
		return FALSE;
	}

	ar.Close();
	return TRUE;
}


BOOL CUIRes::Draw(CDDSurface *pDest)
{
	CRect rtDst(m_ptOffset.x, m_ptOffset.y, m_ptOffset.x+m_mainRect.Width(), m_ptOffset.y+m_mainRect.Height());

	DWORD dwTrans = 0;
	if (m_bTrans) {
		dwTrans = DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT;
	}
	else {
		dwTrans = DDBLTFAST_WAIT;
	}

	pDest->BltFast(m_ptOffset, m_pDDSurface, &m_mainRect, dwTrans);

	return TRUE;
}

BOOL CUIRes::Draw3ButtonCtrl(CDDSurface *pDest, int nFrom, int nTo)
{
	ASSERT(nFrom>=0);
	if (nTo == -1) nTo = m_arrBtn.GetSize()-1;

	ASSERT(nTo<m_arrBtn.GetSize());
	ASSERT(nTo>=0);

	DWORD dwTrans = 0;

	for (int i=nFrom; i<=nTo; i++)
	{
		if(m_arrBtn[i]->m_bTransparent) {
			dwTrans = DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT;
		}
		else {
			dwTrans = DDBLTFAST_WAIT;
		}
		switch (m_arrBtn[i]->m_vState )
		{
		case BUTTON_FOCUS:
			pDest->BltFast(m_ptOffset + m_arrBtn[i]->m_vRect.TopLeft() - m_mainRect.TopLeft(), m_pDDSurface, &m_arrBtn[i]->m_vFocusRect, dwTrans);
			break;
		case BUTTON_DOWN:
			pDest->BltFast(m_ptOffset + m_arrBtn[i]->m_vRect.TopLeft() - m_mainRect.TopLeft(), m_pDDSurface, &m_arrBtn[i]->m_vDownRect, dwTrans);
			break;
		default:		
			break;
		}
	}

	return TRUE;
}

BOOL CUIRes::Draw4ButtonCtrl(CDDSurface *pDest, int nFrom, int nTo)
{
	ASSERT(nFrom>=0);
	if (nTo == -1) nTo = m_arrBtnEx.GetSize()-1;

	ASSERT(nTo<m_arrBtnEx.GetSize());
	ASSERT(nTo>=0);

	DWORD dwTrans = 0;
	for (int i=nFrom; i<=nTo; i++)
	{
		if(m_arrBtnEx[i]->m_bTransparent) {
			dwTrans = DDBLTFAST_SRCCOLORKEY|DDBLTFAST_WAIT;
		}
		else {
			dwTrans = DDBLTFAST_WAIT;
		}
		switch (m_arrBtnEx[i]->m_vState )
		{
		case BUTTON_NORMAL:
			pDest->BltFast(m_ptOffset + m_arrBtnEx[i]->m_vRect.TopLeft() - m_mainRect.TopLeft(), m_pDDSurface, &m_arrBtnEx[i]->m_vNormalRect, dwTrans);
			break;
		case BUTTON_FOCUS:
			pDest->BltFast(m_ptOffset + m_arrBtnEx[i]->m_vRect.TopLeft() - m_mainRect.TopLeft(), m_pDDSurface, &m_arrBtnEx[i]->m_vFocusRect, dwTrans);
			break;
		case BUTTON_DOWN:
			pDest->BltFast(m_ptOffset + m_arrBtnEx[i]->m_vRect.TopLeft() - m_mainRect.TopLeft(), m_pDDSurface, &m_arrBtnEx[i]->m_vDownRect, dwTrans);
			break;
		case BUTTON_DISABLE:
			break;
		default:
			TRACE1("State = %d\n", m_arrBtnEx[i]->m_vState);
			break;
		}
	}

	return TRUE;
}

BOOL CUIRes::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CRect rtDlg;
	CPoint point, off_point;
	
	SetFocus();
	point = pMouseInfo->GetPos();
	off_point = point - m_ptOffset;
	rtDlg.SetRect(0, 0, m_mainRect.Width(), m_mainRect.Height());
	if(!PtInUIRes(point)) {
		return FALSE;
	}
	
	int size, i;
	MBtnCtrl *btn_ctrl;
	MBtnCtrlEx *btn_ctrl_ex;
	MRectManager *rect_manager;

	size = m_arrRectManager.GetSize();

	m_vSpecCtrl = -1;
	for(i = 0; i < size; i++)
	{
		rect_manager = m_arrRectManager[i];
		if(rect_manager->m_pRect->PtInRect(off_point))
		{
			m_vSpecCtrl = rect_manager->m_pCtrl->m_vSpec;
			m_vIndexCtrl = rect_manager->m_pCtrl->m_vIndex;
			if(m_vSpecCtrl == BTN_CTRL)
			{
				btn_ctrl = (MBtnCtrl *)rect_manager->m_pCtrl;
				if(btn_ctrl->m_vState != BUTTON_DISABLE)
				{
					btn_ctrl->m_vState = BUTTON_DOWN;
					SetCapture();
				}
				else continue;
			}
			else if(m_vSpecCtrl == BTN_CTRL_EX)
			{
				btn_ctrl_ex = (MBtnCtrlEx *)rect_manager->m_pCtrl;
				if(btn_ctrl_ex->m_vState != BUTTON_DISABLE)
				{
					btn_ctrl_ex->m_vState = BUTTON_DOWN;
					SetCapture();
				}
				else continue;
			}
			break;
		}
	}

	if(m_vSpecCtrl < 1 && IsMoveable()) {
		m_ptDrag = point;
		SetCapture();
		m_bDragging = TRUE;
	}

	/* // ToolTip 표현을 위해서.
	if (m_nFocusedButton != -1) {
		m_nFocusedButton = -1;
		OnButtonFocus(-1);
	} */

	return TRUE;
}


BOOL CUIRes::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	CPoint point, off_point;

	point = pMouseInfo->GetPos();
	off_point = point - m_ptOffset;

	if(m_bDragging == TRUE)
	{
		ReleaseCapture();
		if (IsMoveable()) {
			m_ptOffset += point-m_ptDrag;
		}
		m_ptDrag = point;
		m_bDragging = FALSE;
	}
	else 
	{
		if(m_vSpecCtrl == BTN_CTRL)
		{
			ReleaseCapture();
			MBtnCtrl *btn_ctrl = m_arrBtn[m_vIndexCtrl];
			if(btn_ctrl->m_vState != BUTTON_DISABLE)
			{
				if(btn_ctrl->m_vRect.PtInRect(off_point))
				{
					btn_ctrl->m_vState = BUTTON_FOCUS;
					OnButtonAction(m_vSpecCtrl, m_vIndexCtrl);
				}
				else
				{
					btn_ctrl->m_vState = BUTTON_NORMAL;
				}
			}
		}
		else if(m_vSpecCtrl == BTN_CTRL_EX)
		{
			ReleaseCapture();
			MBtnCtrlEx *btn_ctrl_ex = m_arrBtnEx[m_vIndexCtrl];
			if(btn_ctrl_ex->m_vState != BUTTON_DISABLE)
			{
				if(btn_ctrl_ex->m_vRect.PtInRect(off_point))
				{
					btn_ctrl_ex->m_vState = BUTTON_FOCUS;
					OnButtonAction(m_vSpecCtrl, m_vIndexCtrl);
				}
				else
				{
					btn_ctrl_ex->m_vState = BUTTON_NORMAL;
				}
			}
		}
		else if(m_vSpecCtrl == AREA_CTRL)
		{
			ReleaseCapture();
			MAreaCtrl *area_ctrl = m_arrArea[m_vIndexCtrl];
			if(area_ctrl->m_vRect.PtInRect(off_point))
			{
				OnButtonAction(m_vSpecCtrl, m_vIndexCtrl);
			}
		}
		m_vSpecCtrl = -1;
		m_vIndexCtrl = 0;
	}

	return TRUE;
}

BOOL CUIRes::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	CPoint point, off_point;
	int i, size;
	MBtnCtrl *btn_ctrl;
	MBtnCtrlEx *btn_ctrl_ex;

	point = pMouseInfo->GetPos();
	off_point = point - m_ptOffset;

	if(pMouseInfo->IsLButtonPressing2())
	{
		if(m_bDragging == TRUE) {
			// 이동가능한 UI일 경우에는 시작위치를 변경하여 그 위치를 이동시킴.
			if (IsMoveable()) {
				m_ptOffset += point-m_ptDrag;
			}
			m_ptDrag = point;
		}
		else if(m_vSpecCtrl == BTN_CTRL)
		{
			btn_ctrl = m_arrBtn[m_vIndexCtrl];
			if(btn_ctrl->m_vState != BUTTON_DISABLE)
			{
				if(btn_ctrl->m_vRect.PtInRect(off_point))
				{
					btn_ctrl->m_vState = BUTTON_DOWN;
				}
				else
				{
					btn_ctrl->m_vState = BUTTON_FOCUS;
				}
			}
		}
		else if(m_vSpecCtrl == BTN_CTRL_EX)
		{
			btn_ctrl_ex = m_arrBtnEx[m_vIndexCtrl];
			if(btn_ctrl_ex->m_vState != BUTTON_DISABLE)
			{
				if(btn_ctrl_ex->m_vRect.PtInRect(off_point))
				{
					btn_ctrl_ex->m_vState = BUTTON_DOWN;
				}
				else
				{
					btn_ctrl_ex->m_vState = BUTTON_FOCUS;
				}
			}
		}
		return TRUE;
	}
	else
	{
		size = m_arrBtn.GetSize();
		for(i = 0; i < size; i++)
		{
			btn_ctrl = m_arrBtn[i];
			if(btn_ctrl->m_vState != BUTTON_DISABLE)
			{
				if(btn_ctrl->m_vRect.PtInRect(off_point))
				{
					btn_ctrl->m_vState = BUTTON_FOCUS;
				}
				else
				{
					btn_ctrl->m_vState = BUTTON_NORMAL;
				}
			}
		}
		size = m_arrBtnEx.GetSize();
		for(i = 0; i < size; i++)
		{
			btn_ctrl_ex = m_arrBtnEx[i];
			if(btn_ctrl_ex->m_vState != BUTTON_DISABLE)
			{
				if(btn_ctrl_ex->m_vRect.PtInRect(off_point))
				{
					btn_ctrl_ex->m_vState = BUTTON_FOCUS;
				}
				else
				{
					btn_ctrl_ex->m_vState = BUTTON_NORMAL;
				}
			}
		}
		return FALSE;
	}
}

BOOL CUIRes::OnLButtonPressing(CPoint point)
{
	int size = m_arrBtn.GetSize();
	CPoint ptReal = point - m_ptOffset;
	for (int i =0;i<size;i++)
	{
		if (m_arrBtn[i]->m_vState == BUTTON_DOWN && m_arrBtn[i]->m_vRect.PtInRect(ptReal))
		{
			OnButtonActionContinue(i);
			break;
		}
	}

	return TRUE;
}


BOOL CUIRes::OnButtonAction(int nSpec, int nIndex)
{
	TRACE ("CScreen::OnButtonAction nID = %d, %d\n", nSpec, nIndex);	

	return TRUE;
}

BOOL CUIRes::OnButtonActionContinue(int nID)
{
	TRACE ("CScreen::OnButtonActionContinue nID = %d\n", nID);
//	OnButtonAction(nID);

	return TRUE;
}

void CUIRes::InitButton()
{
	int size = m_arrBtn.GetSize();
	for (int i =0;i<size;i++)
	{
		if( m_arrBtn[i]->m_vStyle == STYLE_PUSH ) {
			m_arrBtn[i]->m_vState = BUTTON_NORMAL;
		}
	}
	ReleaseCapture();
}

BOOL CUIRes::PtInUIRes(CPoint point)
{
	CRect rtRes = CRect(0, 0, m_mainRect.Width(), m_mainRect.Height()) + m_ptOffset;

	if (!rtRes.PtInRect(point)) return FALSE;


	ASSERT(m_pDDSurface);
	BOOL ret_val = TRUE;

	// 주어진 점의 위치가 Surface의 ColorKey이면 FALSE, 아닐 경우 TRUE를 리턴.
	CDC *pDC = m_pDDSurface->GetDC();
	if (pDC) {
		CPoint ptPixel = point-m_ptOffset;
		ASSERT(ptPixel.x >=0 && ptPixel.x<m_mainRect.Width());
		ASSERT(ptPixel.y >=0 && ptPixel.y<m_mainRect.Height());
		
		if (m_bTrans && pDC->GetPixel(ptPixel)==RGB(255,0,255)) {
			ret_val = FALSE;
		}
		m_pDDSurface->ReleaseDC(pDC);
	}
	else {
		ASSERT(FALSE);
	}

	return	ret_val;
}

BOOL CUIRes::SetToolTips(CString strMsg, CPoint pos)
{
	/* if (!m_pToolTips) {
		m_pToolTips = new CUIToolTips();
	}
	m_pToolTips->SetToolTips(strMsg, pos); */

	return TRUE;
}

void CUIRes::ShowWindow()
{
	m_bShow = TRUE;
	if(m_bEnableFocus)
	{
		SetFocus();
	}
}

void CUIRes::HideWindow()
{
	m_bShow = FALSE;
	if(m_bEnableFocus)
	{
		KillFocus();
	}
}

void CUIRes::SetFocus()
{
	if(m_bEnableFocus)
	{
		if(m_pMainEditor)
		{
			g_pFocusDialog = this;
			m_pMainEditor->SetFocus();
		}
	}
}

void CUIRes::KillFocus()
{
	g_pFocusDialog = NULL;
	if(m_bEnableFocus && m_pMainEditor)
	{
		m_pWnd->SetFocus();
	}
}

BOOL CUIRes::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_ESCAPE)
	{
		HideWindow();
		return TRUE;
	}

	return FALSE;
}

void CUIRes::SetCapture()
{
	m_bCapture = TRUE;
	g_pCaptureDialog = this;
}

void CUIRes::ReleaseCapture()
{
	m_bCapture = FALSE;
	g_pCaptureDialog = NULL;
}

int CUIRes::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	if(m_bShow && m_pChild == NULL)
	{
		if(GetCaptureStatus() || PtInUIRes(pMouseInfo->GetPos()))
		{
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

int CUIRes::GetPosMode()
{
	if(m_pParent && m_pParent->IsVisible()) return m_pParent->GetPosMode();
	else return m_vPosMode;
}