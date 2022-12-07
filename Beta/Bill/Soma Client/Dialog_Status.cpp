// Dialog_Status.cpp: implementation of the CDialog_Status class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Status.h"
//#include "BBDX_TextOuter.h"
#include "User.h"
//#include "TWFont.h"
#include "ChildView.h"
#include "DManage/BBDX_MouseInformation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CUIRes *g_pShowDialog;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Status::CDialog_Status()
{
//	INIT_PTR(m_pOuter);
//	INIT_PTR(m_pTWFont);

	m_vPosMode = POS_LEFT;
	m_pFont = NULL;
}

CDialog_Status::~CDialog_Status()
{
//	SAFE_DELETE(m_pTWFont);
//	SAFE_DELETE(m_pOuter);
	if(m_pFont) delete m_pFont;
}

BOOL CDialog_Status::Draw(CDDSurface *pDest)
{
	if(m_bShow == FALSE) return TRUE;

	CUIRes::Draw(pDest);

	CUser *ptmp;
	CString t_str;
	CRect rtDst;
//	WORD t_color;
	COLORREF t_color;

	Draw3ButtonCtrl(pDest, 0, 0);
	ptmp = CChildView::UserManager.GetMe();

	CString str;
	CDC *pDC = pDest->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255,255,255));

	t_str = ptmp->m_strUserId;
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[0]->m_vRect;		// 이름
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER);

	if(ptmp->m_Designation.IsEmpty() == FALSE)
	{
		rtDst = m_arrArea[1]->m_vRect;		// 칭호
		rtDst.OffsetRect(m_ptOffset);
		pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER);
	}

	if(ptmp->m_Guild.IsEmpty() == FALSE)
	{
		rtDst = m_arrArea[2]->m_vRect;		// 소속
		rtDst.OffsetRect(m_ptOffset);
		pDC->DrawText(ptmp->m_Guild, &rtDst, DT_SINGLELINE|DT_VCENTER);
	}

	if(ptmp->m_Grade.IsEmpty() == FALSE)
	{
		rtDst = m_arrArea[3]->m_vRect;		// 지위
		rtDst.OffsetRect(m_ptOffset);
		pDC->DrawText(ptmp->m_Grade, &rtDst, DT_SINGLELINE|DT_VCENTER);
	}

	t_str.Format("%d", ptmp->m_Level);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[4]->m_vRect;		// 레벨
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

/*	t_str.Format("%ld", ptmp->m_Cur_exp);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[5]->m_vRect;		// 현재 경험치
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Max_exp);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[6]->m_vRect;		// 최대 경험치
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);*/

	t_str.Format("%ld", ptmp->m_Age);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[7]->m_vRect;		// 나이
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	if(ptmp->m_Gender == 0)	t_str = _T("F");
	else t_str = _T("M");
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[8]->m_vRect;		// 성별
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER);

	if(ptmp->m_Deposition <= -50)
	{
		t_str = _T("Devil");
//		t_color = 0x8000;
		t_color = RGB(255, 0, 0);
	}
	else if(ptmp->m_Deposition >= -49 && ptmp->m_Deposition <= -30)
	{
		t_str = _T("Wicked");
		t_color = RGB(255, 70, 70);
//		t_color = 0xf800;
	}
	else if(ptmp->m_Deposition >= -29 && ptmp->m_Deposition <= -11)
	{
		t_str = _T("Evil");
		t_color = RGB(255, 140, 140);
//		t_color = 0xf800;
	}
	else if(ptmp->m_Deposition >= -10 && ptmp->m_Deposition <= -4)
	{
		t_str = _T("Bad");
		t_color = RGB(255, 200, 200);
//		t_color = 0xfc00;
	}
	else if(ptmp->m_Deposition >= -3 && ptmp->m_Deposition <= 3)
	{
		t_str = _T("Neutral");
		t_color = RGB(211, 237, 251);
//		t_color = 0xffe0;
	}
	else if(ptmp->m_Deposition >= 4 && ptmp->m_Deposition <= 10)
	{
		t_str = _T("Good");
		t_color = RGB(187, 230, 244);
//		t_color = 0x07e0;
	}
	else if(ptmp->m_Deposition >= 11 && ptmp->m_Deposition <= 30)
	{
		t_str = _T("Moral");
		t_color = RGB(158, 208, 246);
//		t_color = 0x001f;
	}
	else if(ptmp->m_Deposition >= 30 && ptmp->m_Deposition <= 49)
	{
		t_str = _T("Virtuous");
		t_color = RGB(167, 190, 248);
//		t_color = 0x001f;
	}
	else if(ptmp->m_Deposition >= 50)
	{
		t_str = _T("Angel");
		t_color = RGB(150, 150, 255);
//		t_color = 0x07ff;
	}
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[9]->m_vRect;		// 성향
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, t_color, 0);
	pDC->SetTextColor(t_color);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER);
	pDC->SetTextColor(RGB(255,255,255));

	t_str.Format("%ld", ptmp->m_Cur_Hp);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[10]->m_vRect;		// 현재 HP
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Max_Hp);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[11]->m_vRect;		// 최대 HP
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Cur_Mp);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[12]->m_vRect;		// 현재 MP
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Max_Mp);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[13]->m_vRect;		// 최대 MP
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Cur_Fa);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[14]->m_vRect;		// 현재 스테미너
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Max_Fa);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[15]->m_vRect;		// 최대 스테미너
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%d", ptmp->m_Str);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[16]->m_vRect;		// 힘
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Int);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[17]->m_vRect;		// 지식
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Dex);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[18]->m_vRect;		// 민첩
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Wis);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[19]->m_vRect;		// 지혜
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Cha);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[20]->m_vRect;		// 매력
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%ld", ptmp->m_Con);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[21]->m_vRect;		// 체질
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_UsingSword/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[22]->m_vRect;		// 검술
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_UsingSpear/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[23]->m_vRect;		// 창술
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_UsingAxe/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[24]->m_vRect;		// 둔기
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_UsingKnuckle/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[25]->m_vRect;		// 권술
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_UsingBow/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[26]->m_vRect;		// 궁술
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_UsingCrook/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[27]->m_vRect;		// 지팡이
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_MakingArmor/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[28]->m_vRect;		// 방어구 제작술
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_MakingWeapon/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[29]->m_vRect;		// 무기 제작술
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_MakingAccessory/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[30]->m_vRect;		// 장신구 제작술
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_MakingPotion/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[31]->m_vRect;		// 시약 제작술
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_Cooking/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[32]->m_vRect;		// 요리
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_BlackMagicExp/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[33]->m_vRect;		// 흑마법
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_WhiteMagicExp/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[34]->m_vRect;		// 백마법
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	t_str.Format("%.1f", (double)ptmp->m_BlueMagicExp/10.0);
//	m_pOuter->SetString(t_str);
	rtDst = m_arrArea[35]->m_vRect;		// 청마법
	rtDst.OffsetRect(m_ptOffset);
//	m_pOuter->DrawTexts(pDest, rtDst, 0xffff, 0);
	pDC->DrawText(t_str, &rtDst, DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

	pDC->SelectObject(old_font);
	pDest->ReleaseDC(pDC);

	return TRUE;
}

BOOL CDialog_Status::OnButtonAction(int nSpec, int nIndex) 
{
	CUIRes::OnButtonAction(nSpec, nIndex);

	if(nSpec == BTN_CTRL)
	{
		if(nIndex == 0)
		{
			HideWindow();
		}
	}
	return TRUE;
}

BOOL CDialog_Status::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans)
{
	if (!CUIRes::Create(pDD, szLibName, nType, bModal, bMoveable, bTrans)) {
		return FALSE;
	}

/*	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		delete m_pTWFont; m_pTWFont=NULL;
		TRACE("####### Dialog Status Init 실패.. ############ \n");
		return FALSE;
	}

	m_pOuter = new BBDX_TextOuter(m_pTWFont, 14, 14);*/
	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	return TRUE;
}

BOOL CDialog_Status::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	return CUIRes::OnLButtonDown(pMouseInfo);
}

BOOL CDialog_Status::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	return CUIRes::OnLButtonUp(pMouseInfo);
}

BOOL CDialog_Status::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	return CUIRes::OnMouseMove(pMouseInfo);
}

int CDialog_Status::MouseProcess(BBDX_MouseInformation *pMouseInfo)
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

void CDialog_Status::ShowWindow()
{
	if(!IsLoading())
	{
		if(!Create(CChildView::DirectXMng.m_pDD, "Detail.lib", 0, FALSE, FALSE, FALSE))
		{
			return;
		}
	}

	if(g_pShowDialog) g_pShowDialog->HideAllWindow();
	g_pShowDialog = this;

	CUIRes::ShowWindow();
}

void CDialog_Status::HideWindow()
{
	g_pShowDialog = NULL;

	HideAllWindow();
}

void CDialog_Status::HideAllWindow()
{
	CUIRes::HideWindow();
}
