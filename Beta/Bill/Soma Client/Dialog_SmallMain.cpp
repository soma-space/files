// Dialog_SmallMain.cpp: implementation of the CDialog_SmallMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_SmallMain.h"
#include "User.h"
#include "ChildView.h"
#include "DManage/BBDX_MouseInformation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern int			g_vMainMode;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_SmallMain::CDialog_SmallMain()
{
	InitData();
	INIT_PTR(m_pFont);
	m_bViewCaption = -1;
	m_vPosMode = POS_NOTEFFECT;
}

CDialog_SmallMain::~CDialog_SmallMain()
{
	if(m_pFont)
	{
		m_pFont->DeleteObject();
		delete m_pFont;
	}
}

void CDialog_SmallMain::InitData()
{
	m_bShow = FALSE;
	m_bEnableFocus = FALSE;
}

BOOL CDialog_SmallMain::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CUser *ptmp;
	int t_value;
	DWORD dwTrans;
	CRect src_rect, dst_rect;

	ptmp = CChildView::UserManager.GetMe();
	dwTrans = DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY;

	// 스태미너 표시
	t_value = m_arrArea[4]->m_vRect.Height();
	t_value = MulDiv(t_value, ptmp->m_Cur_Fa, ptmp->m_Max_Fa);
	src_rect = m_arrArea[1]->m_vRect;
	src_rect.top = src_rect.bottom - t_value;
	dst_rect = m_arrArea[4]->m_vRect;
	dst_rect.top = dst_rect.bottom - t_value;
	pDest->BltFast(m_ptOffset+dst_rect.TopLeft(), m_pDDSurface, &src_rect, dwTrans);

	// HP 표시
	t_value = m_arrArea[3]->m_vRect.Height();
	t_value = MulDiv(t_value, ptmp->m_Cur_Hp, ptmp->m_Max_Hp);
	src_rect = m_arrArea[0]->m_vRect;
	src_rect.top = src_rect.bottom - t_value;
	dst_rect = m_arrArea[3]->m_vRect;
	dst_rect.top = dst_rect.bottom - t_value;
	pDest->BltFast(m_ptOffset+dst_rect.TopLeft(), m_pDDSurface, &src_rect, dwTrans);

	// MP 표시
	t_value = m_arrArea[5]->m_vRect.Height();
	t_value = MulDiv(t_value, ptmp->m_Cur_Mp, ptmp->m_Max_Mp);
	src_rect = m_arrArea[2]->m_vRect;
	src_rect.top = src_rect.bottom - t_value;
	dst_rect = m_arrArea[5]->m_vRect;
	dst_rect.top = dst_rect.bottom - t_value;
	pDest->BltFast(m_ptOffset+dst_rect.TopLeft(), m_pDDSurface, &src_rect, dwTrans);

	if(m_bViewCaption >= 0)
	{
		CRect t_rect = m_arrArea[m_bViewCaption]->m_vRect;

		t_rect.OffsetRect(m_ptOffset);
		ViewCaption(pDest, t_rect, m_vStrCaption, 6);
	}

	return TRUE;
}

BOOL CDialog_SmallMain::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	if(CUIRes::OnMouseMove(pMouseInfo) == FALSE)
	{
		CPoint point, off_point;
		MAreaCtrl *area_ctrl;
		int i;

		point = pMouseInfo->GetPos();
		off_point = point - m_ptOffset;

		for(i = 3; i <= 5; i++)
		{
			area_ctrl = m_arrArea[i];
			if(area_ctrl->m_vRect.PtInRect(off_point)) break;
		}

		CUser *ptmp = CChildView::UserManager.GetMe();
		switch(i)
		{
		case 3:		// 체력
			m_vStrCaption.Format("체력 : %d/%d", ptmp->m_Cur_Hp, ptmp->m_Max_Hp);
			m_bViewCaption = i;
			break;
		case 5:		// 심력
			m_vStrCaption.Format("심력 : %d/%d", ptmp->m_Cur_Mp, ptmp->m_Max_Mp);
			m_bViewCaption = i;
			break;
		case 4:		// 스테미너
			m_vStrCaption.Format("지구력 : %d/%d", ptmp->m_Cur_Fa, ptmp->m_Max_Fa);
			m_bViewCaption = i;
			break;
		default:
			m_bViewCaption = -1;
			return FALSE;
		}
		return TRUE;
	}
	else return TRUE;
}

int CDialog_SmallMain::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	if(m_bShow && m_pChild == NULL)
	{
		if(GetCaptureStatus() || PtInUIRes(pMouseInfo->GetPos()))
		{
			if(pMouseInfo->IsLButtonDown()) {
				OnLButtonDown(pMouseInfo);
			}
			if(pMouseInfo->IsLButtonUp()) {
				OnLButtonUp(pMouseInfo);
			}
			if(pMouseInfo->IsMouseMove()) {
				OnMouseMove(pMouseInfo);
			}
			return 2;
		}
		else
		{
			m_bViewCaption = -1;
		}
		return IsModal(); // 다이얼로그가 떠 있을 때 캐릭터가 움직일 수 없음(TRUE), 캐릭터가 움직일 수 있음(FALSE)
	}

	return 0;
}

BOOL CDialog_SmallMain::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

	m_pFont = new CFont;
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");
	if(g_vMainMode == 1) m_bShow = TRUE;
	else m_bShow = FALSE;

	return TRUE;
}

void CDialog_SmallMain::DrawTipRect(CDDSurface *pDest, CRect& t_rect, CRect standard_rect, int area_index)
{
	int width, height, i, j;
	DWORD dwTrans;
	CPoint start_pos;
	CSize rect_size;
	int r_width[3], r_height[3];

	r_width[0] = m_arrArea[area_index]->m_vRect.Width();
	r_width[1] = m_arrArea[area_index+1]->m_vRect.Width();
	r_width[2] = m_arrArea[area_index+2]->m_vRect.Width();
	r_height[0] = m_arrArea[area_index]->m_vRect.Height();
	r_height[1] = m_arrArea[area_index+3]->m_vRect.Height();
	r_height[2] = m_arrArea[area_index+6]->m_vRect.Height();

	width = t_rect.Width() / r_width[1] + ((t_rect.Width()%r_width[1])?1:0);
	height = t_rect.Height() / r_height[1] + ((t_rect.Height()%r_height[1])?1:0);

	rect_size.cx = r_width[0]+r_width[1]*width+r_width[2];
	rect_size.cy = r_height[0]+r_height[1]*height+r_height[2];
	start_pos.x = (standard_rect.Width() - rect_size.cx) / 2 + standard_rect.left;
	start_pos.y = standard_rect.top - rect_size.cy;
	if(start_pos.x < 0) start_pos.x = 0;
	if(start_pos.y < 0) start_pos.y = 0;
	if(start_pos.x + rect_size.cx > 800) start_pos.x = 800 - rect_size.cx;
	if(start_pos.y + rect_size.cy > 600) start_pos.y = 600 - rect_size.cy;
	t_rect.OffsetRect(start_pos);

	dwTrans = DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY;

	start_pos = t_rect.TopLeft();
	pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index]->m_vRect, dwTrans);
	start_pos.x += r_width[0];
	for(j = 0; j < width; j++)
	{
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+1]->m_vRect, dwTrans);
		start_pos.x += r_width[1];
	}
	pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+2]->m_vRect, dwTrans);
	start_pos.y += r_height[0];

	for(i = 0; i < height; i++)
	{
		start_pos.x = t_rect.left;
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+3]->m_vRect, dwTrans);
		start_pos.x += r_width[0];
		for(j = 0; j < width; j++)
		{
			pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+4]->m_vRect, dwTrans);
			start_pos.x += r_width[1];
		}
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+5]->m_vRect, dwTrans);
		start_pos.y += r_height[1];
	}

	start_pos.x = t_rect.left;
	pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+6]->m_vRect, dwTrans);
	start_pos.x += r_width[0];
	for(j = 0; j < width; j++)
	{
		pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+7]->m_vRect, dwTrans);
		start_pos.x += r_width[1];
	}
	pDest->BltFast(start_pos, m_pDDSurface, &m_arrArea[area_index+8]->m_vRect, dwTrans);

	rect_size.cx = (rect_size.cx - t_rect.Width()) / 2;
	rect_size.cy = (rect_size.cy - t_rect.Height()) / 2;

	t_rect.OffsetRect(rect_size.cx, rect_size.cy);
}

void CDialog_SmallMain::ViewCaption(CDDSurface *pDest, CRect standard_rect, CString str, int area_index)
{
	int def_width = 200;
	CRect t_rect;
	CFont pFont, *old_font;
	CDC *pDC = pDest->GetDC();

	old_font = pDC->SelectObject(m_pFont);
	t_rect.SetRect(0, 0, def_width, 14);
	pDC->DrawText(str, &t_rect, DT_LEFT|DT_TOP|DT_EDITCONTROL|DT_WORDBREAK|DT_CALCRECT);
	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

//	t_rect.OffsetRect(pos);

	DrawTipRect(pDest, t_rect, standard_rect, area_index);

	pDC = pDest->GetDC();
	pDC->SetBkMode(TRANSPARENT);

	old_font = pDC->SelectObject(m_pFont);

	// 검은색
	pDC->SetTextColor(RGB(5,5,5));
	pDC->DrawText(str, &t_rect, DT_CENTER|DT_TOP|DT_EDITCONTROL|DT_WORDBREAK);
	
	t_rect.OffsetRect(-1, -1);

	pDC->SetTextColor(RGB(255,255,255));
	pDC->DrawText(str, &t_rect, DT_CENTER|DT_TOP|DT_EDITCONTROL|DT_WORDBREAK);

	pDC->SelectObject(old_font);

	pDest->ReleaseDC(pDC);
}
