// Dialog_Login.cpp: implementation of the CDialog_Login class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "Dialog_Login.h"
#include "Item_Inv.h"
#include "ChildView.h"
#include "BBDX_Editor.h"
//#include "BBDX_TextOuter.h"
#include "DManage/BBDX_MouseInformation.h"
//#include "TWFont.h"
#include <math.h>
#include "IGobject/myconstanddef.h"

#include "DManage/Direct3D.h" // 3D

#define STEP_X		21
#define STEP_Y		3

extern   DirectXMng			DirectXMng; // 3D 9월 24일.

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialog_Login::CDialog_Login(CChildView *child_view)
{
	m_pFont = NULL;
	m_Dialog_Login_Del = 0;
//	m_lpSurface = NULL;
	m_pChildView = NULL;

	m_bChangingChar = 0;
	m_vRotateNum = 0;
//	m_vPos[0].x = 72; m_vPos[0].y = -108;
//	m_vPos[1].x = 303; m_vPos[1].y = -75;
//	m_vPos[2].x = 534; m_vPos[2].y = -108;

	m_pItem1 = NULL;
	m_pItem2 = NULL;
	m_pItem3 = NULL;

	m_pName = NULL;
//	for( int i = 0; i < 17; i++ )
//	{
//		m_pDataOut[i] = NULL;
//	}
//	m_pTWFont = NULL;
	m_vOk = -1;
	m_vViewMode = 0;
	m_bSex = 0; //남자로 초기화...
	m_bpreSex = 0;
	
	m_pChildView = child_view;

//우석.
	m_IsMovePixel = false;

	m_ViewCharNum = 0;
	m_ptSrcChar1.x = 284;
	m_ptSrcChar1.y = 150;
	m_ptSrcChar2.x = 490;
	m_ptSrcChar2.y = 80;
	m_ptSrcChar3.x = 90;
	m_ptSrcChar3.y = 80;

	m_ptMakeChar1.x = 120;
	m_ptMakeChar1.y = 90;
	m_ptMakeChar2.x = 120;
	m_ptMakeChar2.y = 90;

	// 새캐릭터 만들기에 쓸거...
	m_pNewCharItem01 = NULL;
	m_pNewCharItem02 = NULL;
	m_pNewCharItem03 = NULL;
	m_pNewCharItemWoman01 = NULL;
	m_pNewCharItemWoman02 = NULL;
	m_pNewCharItemWoman03 = NULL;

	//캐릭터의 능역치..
    m_Strength = 10;
    m_Dexterity = 10;
    m_Inteligence = 10;
    m_Charisma = 10;
    m_Wisdom = 10;
    m_Constitution = 10;
    m_Points = 3;

	m_SkinColorChange = 0;
	m_NextSkinColorChange = 0;
	m_bSkinColorChangeEnd = true;
	m_DrawMode = 0; //0:캐릭터 선택. 1:피부색 선택.

	m_frameRate = 0;
	m_HairColorIndex = 1;
	m_HairStyleIndex = 0;

	m_bCharExplain = false;
	m_CharExplainNum = -1;
	m_pCharExplainFont = NULL;
}

CDialog_Login::~CDialog_Login()
{
//	SAFE_DELETE( m_lpSurface );
	SAFE_DELETE( m_pName );
//	for(int i = 0; i < 17; i++)
//	{
//		SAFE_DELETE( m_pDataOut[i] );
//	}
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pCharExplainFont);
	// 새캐릭터 만들기에 쓸거...
	SAFE_DELETE(m_pNewCharItem01);
	SAFE_DELETE(m_pNewCharItem02);
	SAFE_DELETE(m_pNewCharItem03);
	SAFE_DELETE(m_pNewCharItemWoman01);
	SAFE_DELETE(m_pNewCharItemWoman02);
	SAFE_DELETE(m_pNewCharItemWoman03);
}

//우석....
BOOL CDialog_Login::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bMoveable, BOOL bTrans)
{
	if(m_pDDSurface) { delete m_pDDSurface; m_pDDSurface =	NULL; }
	if(!CUIRes::Create(pDD, szLibName, nType, bMoveable, bTrans)) {
	   return FALSE;
	}

	CRect t_rect;
	t_rect = GetTextOutRect(0); // id.
	m_pName = new BBDX_Editor(m_pChildView, NULL, FALSE, t_rect);
	m_pName->LimitText(12);

/*	m_pTWFont = new CTWFont();
	if (!m_pTWFont->CreateFontWithoutPath("Soma14", 14, 14))
	{
		SAFE_DELETE(m_pTWFont);
	}

	for(int i = 0; i < 17; i++)
	{
		m_pDataOut[i] = new BBDX_TextOuter(m_pTWFont, 14, 14);
	}*/
	m_pFont = new CFont();
	m_pFont->CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	m_pCharExplainFont = new CFont();
	m_pCharExplainFont->CreateFont(-17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");

	 
	// 3D 9월 24일..
	if(m_vViewMode == 0)
	{
//		CChildView::DirectXMng.m_p3D->DestoryLoginVertices(); // 3D 9월 24일.
//		CChildView::DirectXMng.m_p3D->CreateLoginVertices();

		//오류메세지의 위치지정.
		m_vMsgRect = GetTextOutRect(18);
	}
	else if(m_vViewMode == 1)
	{
//		CChildView::DirectXMng.m_p3D->DestoryMakeCharVertices();		
//		CChildView::DirectXMng.m_p3D->CreateMakeCharVertices();

		//오류메세지의 위치지정.
		m_vMsgRect = GetTextOutRect(9);
	}

	m_bDelCheck = FALSE;

	return TRUE;
}

BOOL CDialog_Login::MouseProcess(BBDX_MouseInformation *pMouseInfo)
{
	ASSERT(pMouseInfo);

	if(m_IsMovePixel) return TRUE;
	
	if(pMouseInfo->IsLButtonDown())	OnLButtonDown(pMouseInfo);
	if(pMouseInfo->IsMouseMove()) OnMouseMove(pMouseInfo);
	if(pMouseInfo->IsLButtonUp()) OnLButtonUp(pMouseInfo);

	return TRUE;
}

BOOL CDialog_Login::OnLButtonDown(BBDX_MouseInformation *pMouseInfo)
{
	CPoint point;
	
	point = pMouseInfo->GetPos();

	// 물방울 효과.
	m_ClickPt = point;
	m_water.InputData(point.x, point.y);

	int size, i;

	size = m_arrBtn.GetSize();

	m_vSpecCtrl = -1;
	for(i = 0; i < size; i++)
	{
		if(m_arrBtn[i]->m_vRect.PtInRect(CPoint(point-m_ptOffset)))
		{
			m_vSpecCtrl = m_arrBtn[i]->m_vSpec;
			m_vIndexCtrl = m_arrBtn[i]->m_vIndex;
			if(m_vSpecCtrl == BTN_CTRL)
			{
				m_arrBtn[i]->m_vState = BUTTON_DOWN;
			}
			break;
		}
	}

	return TRUE;
}

BOOL CDialog_Login::OnLButtonUp(BBDX_MouseInformation *pMouseInfo)
{
	CPoint point, off_point;

	point = pMouseInfo->GetPos();
	off_point = point - m_ptOffset;

	if(m_vSpecCtrl == BTN_CTRL)
	{
		MBtnCtrl *btn_ctrl = m_arrBtn[m_vIndexCtrl];
		if(btn_ctrl->m_vRect.PtInRect(off_point))
		{
			if(m_vViewMode == 0)
			{
				switch(m_vIndexCtrl)	
				{
					case 0: m_vOk = 0; break; // 프로그램 종료.
					case 1: if(m_pChildView->m_CharNum < 3) m_vOk = 1; break; // 새캐릭터 만들기.
					case 2: m_vOk = 2; break; // 들어가기.
					case 3: m_vOk = 3; break; // 나가기.
					case 4: m_vOk = 4; break; // 지우기.
					case 5: m_vOk = 5; m_bChangingChar = -1; SetCharNum(); break; // 캐릭터의 위치이동... 왼쪽...
					case 6: m_vOk = 6; m_bChangingChar = 1;  SetCharNum(); break; // 캐릭터의 위치이동... 오른쪽..
				}
				btn_ctrl->m_vState = BUTTON_NORMAL;
			}
			else if(m_vViewMode == 1)
			{
				if(!m_pChildView->m_bMakeProcess) //확인 버튼을 여러번 누르는걸 방지...
				{
					switch(m_vIndexCtrl)	
					{
						case 0: m_vOk = 0; break; // 확인.
						case 1: m_vOk = 1; break; // 취소.
						case 2: m_vOk = 2; m_bpreSex = m_bSex; m_bSex = 0;
							    //m_SkinColorChange = 1;
								//m_NextSkinColorChange = 1;
								if( m_bpreSex != m_bSex)
								{
									m_IsMovePixel = true;
									m_DrawMode = 0;
								}
								break; // 남자. 서버는 남자가 1 여자가 0...
						case 3: m_vOk = 3; m_bpreSex = m_bSex; m_bSex = 1;
							    //m_SkinColorChange = 1; 
								//m_NextSkinColorChange = 1; 
							    if( m_bpreSex != m_bSex)
								{
									m_IsMovePixel = true; 
									m_DrawMode = 0;
								}
								break; // 여자.
						case 4: m_vOk = 4; break; // strength. -
						case 5: m_vOk = 5; break; // Dexterity -
						case 6: m_vOk = 6; break; // inteligence -
						case 7: m_vOk = 7; break; // charisma -
						case 8: m_vOk = 8; break; // wisdom -
						case 9: m_vOk = 9; break; // constitution -
						case 10: m_vOk = 10; break; // constitution. +
						case 11: m_vOk = 11; break; // wisdom +
						case 12: m_vOk = 12; break; // charisma +
						case 13: m_vOk = 13; break; // inteligence +
						case 14: m_vOk = 14; break; // Dexterity +
						case 15: m_vOk = 15; break; // strength +

						case 16: m_vOk = 16; HairStyleChange(-1); break; // hair style -
						case 17: m_vOk = 17; HairStyleChange(1); break; // hair style +
						case 18: m_vOk = 18; HairColorChange(-1); break; // hair color -
						case 19: m_vOk = 19; HairColorChange(1);  break; // hair color +
						case 20: m_vOk = 20; SkinChange(-1); m_DrawMode = 1; break;
						case 21: m_vOk = 21; SkinChange(1); m_DrawMode = 1; break;
						case 22: SetCharTypeData(4);break;
						case 23: SetCharTypeData(1); break;
						case 24: SetCharTypeData(2);break;
						case 25: SetCharTypeData(3);break;						
					}
					btn_ctrl->m_vState = BUTTON_NORMAL;
				}//if
			}//else if
			else if(m_vViewMode == 3)
			{
				switch(m_vIndexCtrl)	
				{
					case 7: m_vOk = 4; m_vViewMode -= 3; m_bDelCheck = TRUE;  break; // ok.
					case 8: m_vOk = -1; m_vViewMode -= 3; m_bDelCheck = FALSE; break; // cancel.
				}
				btn_ctrl->m_vState = BUTTON_NORMAL;
			}
			else
			{
				switch(m_vIndexCtrl)	
				{
					case 26: m_vOk = -1; m_vViewMode -= 3;
						     m_pChildView->m_bMakeProcess = false;							 
						     break; // ok.
				}
			}
		}
	}

	m_vSpecCtrl = -1;
	m_vIndexCtrl = 0;

	return TRUE;
}

void CDialog_Login::HairStyleChange(int change)
{
	CItem_Inv  *CurChar = NULL;

	m_HairColorIndex = 1;

	switch(m_SkinColorChange)
	{
		case 0: if(m_bSex == 0) CurChar = m_pNewCharItem01; //남자..
				else            CurChar = m_pNewCharItemWoman01;
			   break;
		case 1: if(m_bSex == 0) CurChar = m_pNewCharItem02; //남자..
				else            CurChar = m_pNewCharItemWoman02;
			   break;
		case 2: if(m_bSex == 0) CurChar = m_pNewCharItem03; //남자..
				else            CurChar = m_pNewCharItemWoman03;
			   break;
	}

	switch(change)
	{
		case -1: if(m_HairStyleIndex > 0)
				 {	 
					 m_HairStyleIndex--;
					 //CurChar->Change_InvItem(0,m_HairColorIndex);
					 CurChar->HairStyleDraw(120,205,m_HairStyleIndex);
				 }
			     else
				 {
					 m_HairStyleIndex = 2;					 
					 CurChar->HairStyleDraw(120,205,m_HairStyleIndex);
				 }
			     break;
		case 1: if(m_HairStyleIndex < 2)
				{
					m_HairStyleIndex++;
					//CurChar->Change_InvItem(0,m_HairColorIndex);
				    CurChar->HairStyleDraw(120,205,m_HairStyleIndex);
				}
			    else
				{
					m_HairStyleIndex = 0;					
				    CurChar->HairStyleDraw(120,205,m_HairStyleIndex);
				}
			   break;
	}
}


void CDialog_Login::HairColorChange(int change)
{
 	CItem_Inv  *CurChar = NULL;

	switch(m_SkinColorChange)
	{
		case 0: if(m_bSex == 0) CurChar = m_pNewCharItem01; //남자..
				else            CurChar = m_pNewCharItemWoman01;
			   break;
		case 1: if(m_bSex == 0) CurChar = m_pNewCharItem02; //남자..
				else            CurChar = m_pNewCharItemWoman02;
			   break;
		case 2: if(m_bSex == 0) CurChar = m_pNewCharItem03; //남자..
				else            CurChar = m_pNewCharItemWoman03;
			   break;
	}

	switch(change)
	{
		case -1: if(m_HairColorIndex > 1)
				 {	 
					 m_HairColorIndex--;
					 //CurChar->Change_InvItem(0,m_HairColorIndex);
					 CurChar->HairDraw(120,205,m_HairStyleIndex,m_HairColorIndex);
				 }
			     else
				 {
					 m_HairColorIndex = 3;					 
					 CurChar->HairDraw(120,205,m_HairStyleIndex,m_HairColorIndex);
				 }
			     break;
		case 1: if(m_HairColorIndex < 3)
				{
					m_HairColorIndex++;
					//CurChar->Change_InvItem(0,m_HairColorIndex);
				    CurChar->HairDraw(120,205,m_HairStyleIndex,m_HairColorIndex);
				}
			    else
				{
					m_HairColorIndex = 1;					
				    CurChar->HairDraw(120,205,m_HairStyleIndex,m_HairColorIndex);
				}
			   break;
	}
}

void CDialog_Login::SkinChange(int change)
{
	//m_HairColorIndex = 1; m_HairStyleIndex = 0;

	if(m_bSkinColorChangeEnd)
	{
		m_bSkinColorChangeEnd = false;
		m_NextSkinColorChange += change;
		if(m_NextSkinColorChange < 0)
		{
			m_NextSkinColorChange = 2;
			//m_IsMovePixel = false;
			//return;
		}
		else if(m_NextSkinColorChange > 2)
		{
			m_NextSkinColorChange = 0;
			//m_IsMovePixel = false;
			//return;
		}

		m_IsMovePixel = true;
//		m_DrawMode = 1;
	}
}


//우석... 캐릭터 위치 이동...
void CDialog_Login::SetCharNum()
{
	if(!m_IsMovePixel)
	{
		m_IsMovePixel = true; /*initPixelData();*/
		if(m_bChangingChar == 1) //오른쪽..
		{
			switch(m_ViewCharNum)
			{
				case 0: m_ViewCharNum = 2;
					    m_pItem3->m_timegab = 110;
						m_pItem3->m_bCahrBlend = false;
						m_pItem1->m_timegab = 150; 
						m_pItem1->m_bCahrBlend = true;
						m_pItem2->m_timegab = 150; 
						m_pItem2->m_bCahrBlend = true;

						break;
				case 2: m_ViewCharNum = 1;
					    m_pItem2->m_timegab = 110;
						m_pItem2->m_bCahrBlend = false;
						m_pItem1->m_timegab = 150; 
						m_pItem1->m_bCahrBlend = true;
						m_pItem3->m_timegab = 150; 
						m_pItem3->m_bCahrBlend = true;
						break;
				case 1: m_ViewCharNum = 0;
					    m_pItem1->m_timegab = 110;
						m_pItem1->m_bCahrBlend = false;
						m_pItem2->m_timegab = 150; 
						m_pItem2->m_bCahrBlend = true;
						m_pItem3->m_timegab = 150; 
						m_pItem3->m_bCahrBlend = true;
						break;
			} // 캐릭터가 3개.
		}
		else if(m_bChangingChar == -1)
		{
			switch(m_ViewCharNum) //왼쪽....
			{
				case 0: m_ViewCharNum = 1;
					    m_pItem1->m_timegab = 150;
						m_pItem1->m_bCahrBlend = true;
						m_pItem2->m_timegab = 110; 
						m_pItem2->m_bCahrBlend = false;
						m_pItem3->m_timegab = 150; 
						m_pItem3->m_bCahrBlend = true;
						break;
				case 2: m_ViewCharNum = 0;
					    m_pItem1->m_timegab = 110;
						m_pItem1->m_bCahrBlend = false;
						m_pItem2->m_timegab = 150; 
						m_pItem2->m_bCahrBlend = true;
						m_pItem3->m_timegab = 150; 
						m_pItem3->m_bCahrBlend = true;
						break;
				case 1: m_ViewCharNum = 2;	
					    m_pItem1->m_timegab = 150;
						m_pItem1->m_bCahrBlend = true;
						m_pItem2->m_timegab = 150; 
						m_pItem2->m_bCahrBlend = true;
						m_pItem3->m_timegab = 110; 
						m_pItem3->m_bCahrBlend = false;
						break;
			} // 캐릭터가 3개.
		}
	}
}
BOOL CDialog_Login::OnMouseMove(BBDX_MouseInformation *pMouseInfo)
{
	CPoint point, off_point;
	int i, size;
	MBtnCtrl *btn_ctrl;
	MAreaCtrl *area_ctrl;

	point = pMouseInfo->GetPos();

//	m_water.InputData(point.x, point.y);

	off_point = point - m_ptOffset;

	if(pMouseInfo->IsLButtonPressing())
	{
		if(m_vSpecCtrl == BTN_CTRL)
		{
			btn_ctrl = m_arrBtn[m_vIndexCtrl];
			if(btn_ctrl->m_vRect.PtInRect(off_point))
			{
				btn_ctrl->m_vState = BUTTON_DOWN;
			}
			else
			{
//				if(btn_ctrl->m_vState == BUTTON_DOWN)
				btn_ctrl->m_vState = BUTTON_NORMAL;
			}
		}
	}
	else
	{		
		//=====================================
		// 새캐릭터 만들기창에서 각능력치설명..
		//=====================================
		if(m_vViewMode == 1)
		{
			size = m_arrArea.GetSize();
			for(i = 0; i < size; i++)
			{
				area_ctrl = m_arrArea[i];
				m_vAreaIndex = m_arrArea[i]->m_vIndex;
				if(area_ctrl->m_vRect.PtInRect(off_point))
				{					
					switch(m_vAreaIndex)	
					{
						case 11: case 12: case 13: case 14: case 15: 
						case 16: case 17: case 18: case 19: 
						case 20: m_bCharExplain = true; m_CharExplainNum = m_vAreaIndex; break;
                        default : m_bCharExplain = false; m_CharExplainNum = -1; break;
					}
					if(m_bCharExplain) break;
				}
				else { m_bCharExplain = false; m_CharExplainNum = -1; }
			}
		}
		//======================================
		if(m_vSpecCtrl == -1)
		{
			size = m_arrBtn.GetSize();
			for(i = 0; i < size; i++)
			{
				btn_ctrl = m_arrBtn[i];
				m_vIndexCtrl = m_arrBtn[i]->m_vIndex;
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
	}

	return TRUE;
}


void CDialog_Login::DrawNewCharExplain()
{
	CString  message;

	if(m_bCharExplain)
	{
		switch(m_CharExplainNum)	
		{
			case 11: message.LoadString(IDS_NEWCHAR_STR); break; 
			case 12: message.LoadString(IDS_NEWCHAR_DEX); break; 
			case 13: message.LoadString(IDS_NEWCHAR_INT); break; 
			case 14: message.LoadString(IDS_NEWCHAR_CHAR); break; 
			case 15: message.LoadString(IDS_NEWCHAR_WIS); break; 
			case 16: message.LoadString(IDS_NEWCHAR_CON); break;
			case 17: message.LoadString(IDS_NEWCHAR_POWERTYPE); break; 
			case 18: message.LoadString(IDS_NEWCHAR_SPEEDTYPE); break; 
			case 19: message.LoadString(IDS_NEWCHAR_MAGICTYPE); break; 
			case 20: message.LoadString(IDS_NEWCHAR_BALACETYPE); break; 
			default : return; break;
		}
	}
	else
	{
		message.LoadString(IDS_NEWCHAR_DEFAULT);
	}

	CDDSurface* pDestSurf = CChildView::DirectXMng.m_pDrawSurface;
	CRect t_rect;
	CDC *pDC = pDestSurf->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pCharExplainFont);
	COLORREF old_color = pDC->SetTextColor(RGB(255,255,255));
	int mode = pDC->SetBkMode(TRANSPARENT);

	t_rect = GetTextOutRect(10);
	pDC->DrawText(message, &t_rect, DT_LEFT|DT_TOP|DT_WORDBREAK|DT_EXTERNALLEADING);

	pDC->SetTextColor(old_color);
	pDC->SetBkMode(mode);
	pDC->SelectObject(old_font);
	pDestSurf->ReleaseDC(pDC);
}

void CDialog_Login::SetCharTypeData(int chartype) //num으로 능력치 구분..
{
	CString buff;

	for(int i = 0; i < 6; i++)
	{
		m_DataOut[i] = "10";
	}

    m_Strength = 10;
    m_Dexterity = 10;
    m_Inteligence = 10;
    m_Charisma = 10;
    m_Wisdom = 10;
    m_Constitution = 10;
	m_Points = 0;

	switch(chartype)
	{
		case 1: m_Strength = 13; m_Points = 0; //power type 
				wsprintf(m_DataOut[0].GetBuffer(11),"%d",m_Strength);
				m_DataOut[0].ReleaseBuffer();
				break;
		case 2: m_Dexterity = 13; m_Points = 0; // speed type
				wsprintf(m_DataOut[1].GetBuffer(11),"%d",m_Dexterity);
				m_DataOut[1].ReleaseBuffer();
				break;
		case 3:  m_Inteligence = 13; m_Points = 0; // magic type
				wsprintf(m_DataOut[2].GetBuffer(11),"%d",m_Inteligence);
				m_DataOut[2].ReleaseBuffer();
				break;
		case 4: m_Strength = 11; m_Dexterity = 11;m_Inteligence = 11;m_Points = 0; // balance type
				wsprintf(m_DataOut[0].GetBuffer(11),"%d",m_Strength);
				m_DataOut[0].ReleaseBuffer();
				wsprintf(m_DataOut[1].GetBuffer(11),"%d",m_Dexterity);
				m_DataOut[1].ReleaseBuffer();
				wsprintf(m_DataOut[2].GetBuffer(11),"%d",m_Inteligence);
				m_DataOut[2].ReleaseBuffer();
				break;
	}
	wsprintf(m_DataOut[6].GetBuffer(11),"%d",m_Points);
	m_DataOut[6].ReleaseBuffer();
}


// 우석...
void CDialog_Login::SetMakeCharData(int num) //num으로 능력치 구분..
{
	CString buff;

	switch(num)
	{
		case 4: if(m_Strength > 10) { m_Strength--; m_Points++; }
				wsprintf(m_DataOut[0].GetBuffer(11),"%d",m_Strength);
				m_DataOut[0].ReleaseBuffer();
//				m_pDataOut[0]->SetString(buff);  
				break;

		case 5: if(m_Dexterity > 10) { m_Dexterity--; m_Points++; }
				wsprintf(m_DataOut[1].GetBuffer(11),"%d",m_Dexterity);
				m_DataOut[1].ReleaseBuffer();
//				m_pDataOut[1]->SetString(buff); 
				break;
		case 6: if(m_Inteligence > 10) { m_Inteligence--; m_Points++; }
				wsprintf(m_DataOut[2].GetBuffer(11),"%d",m_Inteligence);
				m_DataOut[2].ReleaseBuffer();
//				m_pDataOut[2]->SetString(buff); 
				break;
		case 7: if(m_Charisma > 10) { m_Charisma--; m_Points++; }
				wsprintf(m_DataOut[3].GetBuffer(11),"%d",m_Charisma);
				m_DataOut[2].ReleaseBuffer();
//				m_pDataOut[3]->SetString(buff); 
				break;
		case 8: if(m_Wisdom > 10) { m_Wisdom--; m_Points++; }
				wsprintf(m_DataOut[4].GetBuffer(11),"%d",m_Wisdom);
				m_DataOut[4].ReleaseBuffer();
//				m_pDataOut[4]->SetString(buff);
				break;
		case 9: if(m_Constitution > 10) { m_Constitution--; m_Points++; }
				wsprintf(m_DataOut[5].GetBuffer(11),"%d",m_Constitution);
				m_DataOut[5].ReleaseBuffer();
//				m_pDataOut[5]->SetString(CString("0"));
				break;
		case 10: if(m_Points > 0) { m_Constitution++; m_Points--; }
				wsprintf(m_DataOut[5].GetBuffer(11),"%d",m_Constitution);
				m_DataOut[5].ReleaseBuffer();
//				m_pDataOut[5]->SetString(buff);
				break;
		case 11: if(m_Points > 0) { m_Wisdom++; m_Points--; }
				wsprintf(m_DataOut[4].GetBuffer(11),"%d",m_Wisdom);
				m_DataOut[4].ReleaseBuffer();
//				m_pDataOut[4]->SetString(buff);
				break;
		case 12: if(m_Points > 0) { m_Charisma++; m_Points--; }
				wsprintf(m_DataOut[3].GetBuffer(11),"%d",m_Charisma);
				m_DataOut[3].ReleaseBuffer();
//				m_pDataOut[3]->SetString(buff); 
				break;
		case 13: if(m_Points > 0) { m_Inteligence++; m_Points--; }
				wsprintf(m_DataOut[2].GetBuffer(11),"%d",m_Inteligence);
				m_DataOut[2].ReleaseBuffer();
//				m_pDataOut[2]->SetString(buff); 
				break;
		case 14: if(m_Points > 0) { m_Dexterity++; m_Points--; }
				wsprintf(m_DataOut[1].GetBuffer(11),"%d",m_Dexterity);
				m_DataOut[1].ReleaseBuffer();
//				m_pDataOut[1]->SetString(buff); 
				break;
		case 15: if(m_Points > 0) { m_Strength++; m_Points--; }
				wsprintf(m_DataOut[0].GetBuffer(11),"%d",m_Strength);
				m_DataOut[0].ReleaseBuffer();
//				m_pDataOut[0]->SetString(buff);  
				break;
	}
	wsprintf(m_DataOut[6].GetBuffer(11),"%d",m_Points);
	m_DataOut[6].ReleaseBuffer();
//	m_pDataOut[6]->SetString(buff); // points

//	buff.ReleaseBuffer();
}

BOOL CDialog_Login::Draw( CDDSurface *pDestSurf )
{
	CUIRes::Draw(pDestSurf);

	DWORD dwTick = GetTickCount();

	m_frameRate = dwTick - m_frameRate;

	//픽셀의 움직임.
	if(m_IsMovePixel)
	{
		if( m_vViewMode == 1 ) NewCharDraw( pDestSurf );
		else MovePixelDraw(pDestSurf);
	}
	else
	{
		if( m_vViewMode == 1 || m_vViewMode == 4) NewCharDraw( pDestSurf );
		else LoginDraw( pDestSurf );
	}

	m_frameRate = dwTick;

	// error message dlg...
	if( m_vViewMode > 1 )
	{
//		DDSURFACEDESC2 ddsd;
		CPoint point;
		if( m_vViewMode == 3)
		{
			point.x = 221; point.y = 189;
			pDestSurf->BltFast( point, m_pDDSurface, &m_arrArea[17]->m_vRect, DDBLTFAST_SRCCOLORKEY );
		}
		else				  
		{
			point.x = 109; point.y = 170; 
			pDestSurf->BltFast( point, m_pDDSurface, &m_arrArea[8]->m_vRect, DDBLTFAST_SRCCOLORKEY );
		}		
			
//		pDestSurf->Lock();
//		ddsd = pDestSurf->GetDescription();
		DrawErrorText(pDestSurf); //에러 메세지 출력.
//		pDestSurf->Unlock();
	}

	if(m_vViewMode == 0) Draw3ButtonCtrl(pDestSurf, 0, 6);
	else if(m_vViewMode == 1) Draw3ButtonCtrl(pDestSurf, 0, 25);
	else if(m_vViewMode == 3) Draw3ButtonCtrl(pDestSurf, 0, 8);
	else if(m_vViewMode == 4) Draw3ButtonCtrl(pDestSurf, 0, 26);

	return TRUE;
}

void CDialog_Login::MovePixelDraw(CDDSurface* pDestSurf)
{
	float time_gap = 0.0;

	time_gap = (float)m_frameRate / 10.0 + 1.0;

   if( LoginCharData(m_bChangingChar, m_ViewCharNum, time_gap) )
   {
	   m_IsMovePixel = false;
	   LoginDraw(pDestSurf);
   }
   else
   {
	   LoginDraw(pDestSurf);
   }
}


void CDialog_Login::LoginDraw(CDDSurface* pDestSurf)
{
		if(!CChildView::SocketMng.m_szLoginId2.IsEmpty())
		{
			if(m_pChildView->m_CharData[1] && m_ViewCharNum == 1)
			{
				if(m_IsMovePixel == false)
				{
					SetCharData(m_pItem2,m_ViewCharNum);
					DrawCharData(pDestSurf, 1);
				}
			}
//			m_pItem2->m_startPrimitive = 2;
			m_pItem2->DrawPrimitiveChar(m_ptSrcChar2.x, m_ptSrcChar2.y);
		}

		if(!CChildView::SocketMng.m_szLoginId3.IsEmpty())
		{   
			if(m_pChildView->m_CharData[2] && m_ViewCharNum == 2)
			{
				if(m_IsMovePixel == false)
				{
					SetCharData(m_pItem3, m_ViewCharNum);
					DrawCharData(pDestSurf, 2);
				}
			}
//			m_pItem3->m_startPrimitive = 3;
			m_pItem3->DrawPrimitiveChar(m_ptSrcChar3.x, m_ptSrcChar3.y);
		}

		if(!CChildView::SocketMng.m_szLoginId1.IsEmpty())
		{
			if(m_pChildView->m_CharData[0] && m_ViewCharNum == 0)
			{				
				if(m_IsMovePixel == false)
				{
					SetCharData(m_pItem1,m_ViewCharNum);
					DrawCharData(pDestSurf, 0);
				}
			}
//			m_pItem1->m_startPrimitive = 1;
			m_pItem1->DrawPrimitiveChar(m_ptSrcChar1.x, m_ptSrcChar1.y);
		}

/*
	if(m_IsMovePixel)
	{
		CRect   clientRect;
		clientRect.SetRect(5,5,795,595);
		if(clientRect.PtInRect(m_ClickPt))
		{
			int   sg;
			srand(time(NULL));
			sg = rand() % 2;
			if(sg) m_water.InputData(m_ClickPt.x + 1, m_ClickPt.y +1);
			else m_water.InputData(m_ClickPt.x - 1, m_ClickPt.y -1);
		}		
	}
*/
	DDSURFACEDESC2 ddsd;
	pDestSurf->Lock();
	ddsd = pDestSurf->GetDescription();
	m_water.Draw(ddsd); // 물방울 출력.
	pDestSurf->Unlock();
}

//우석.. 새캐릭터 만들기...
void CDialog_Login::MakeCharData(CDDSurface* pDestSurf)
{
	CRect t_rect;	

	CDC *pDC = pDestSurf->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	COLORREF old_color = pDC->SetTextColor(RGB(255,255,255));
	int mode = pDC->SetBkMode(TRANSPARENT);

	t_rect = GetTextOutRect(1); //strength
//	m_pDataOut[0]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[0], &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(2); //dexterity
//	m_pDataOut[1]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[1], &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(3); // inteligence
//	m_pDataOut[2]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[2], &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(4); // charisma
//	m_pDataOut[3]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[3], &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(5); // wisdom
//	m_pDataOut[4]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[4], &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(6); // constitution
//	m_pDataOut[5]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[5], &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(7); // points
//	m_pDataOut[6]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[6], &t_rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE);

	pDC->SetTextColor(old_color);
	pDC->SetBkMode(mode);
	pDC->SelectObject(old_font);
	pDestSurf->ReleaseDC(pDC);
}

//우석.. 
void CDialog_Login::DrawCharData(CDDSurface* pDestSurf, int i)
{
	COLORREF t_color;
	CRect	 t_rect;
	CDC *pDC = pDestSurf->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	COLORREF old_color = pDC->SetTextColor(RGB(255,255,255));
	int mode = pDC->SetBkMode(TRANSPARENT);

	t_rect = GetTextOutRect(0);
//	m_pDataOut[0]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[0], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(1);
//	m_pDataOut[1]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[1], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(2);
//	m_pDataOut[2]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[2], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(3);
//	m_pDataOut[3]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[3], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(4);
//	m_pDataOut[4]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[4], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(5);
//	m_pDataOut[5]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[5], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(6);
//	m_pDataOut[6]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[6], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(7);
//	m_pDataOut[7]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[7], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(8);
//	m_pDataOut[8]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[8], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(9);
//	m_pDataOut[9]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[9], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(10);
//	m_pDataOut[10]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[10], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(11);
//	m_pDataOut[11]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[11], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);



	t_rect = GetTextOutRect(12);
	//pDC->DrawText(m_DataOut[12], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	CString  t_str;
//================================================================================
	if(m_pChildView->m_CharData[i]->Moral <= -50)
	{
		t_str = _T("Devil");
		t_color = RGB(255, 0, 0);
	}
	else if(m_pChildView->m_CharData[i]->Moral >= -49 && m_pChildView->m_CharData[i]->Moral <= -30)
	{
		t_str = _T("Wicked");
		t_color = RGB(255, 70, 70);
	}
	else if(m_pChildView->m_CharData[i]->Moral >= -29 && m_pChildView->m_CharData[i]->Moral <= -11)
	{
		t_str = _T("Evil");
		t_color = RGB(255, 140, 140);
	}
	else if(m_pChildView->m_CharData[i]->Moral >= -10 && m_pChildView->m_CharData[i]->Moral <= -4)
	{
		t_str = _T("Bad");
		t_color = RGB(255, 200, 200);
	}
	else if(m_pChildView->m_CharData[i]->Moral >= -3 && m_pChildView->m_CharData[i]->Moral <= 3)
	{
		t_str = _T("Neutral");
		t_color = RGB(211, 237, 251);
	}
	else if(m_pChildView->m_CharData[i]->Moral >= 4 && m_pChildView->m_CharData[i]->Moral <= 10)
	{
		t_str = _T("Good");
		t_color = RGB(187, 230, 244);
	}
	else if(m_pChildView->m_CharData[i]->Moral >= 11 && m_pChildView->m_CharData[i]->Moral <= 30)
	{
		t_str = _T("Moral");
		t_color = RGB(158, 208, 246);
	}
	else if(m_pChildView->m_CharData[i]->Moral >= 30 && m_pChildView->m_CharData[i]->Moral <= 49)
	{
		t_str = _T("Virtuous");
		t_color = RGB(167, 190, 248);
	}
	else if(m_pChildView->m_CharData[i]->Moral >= 50)
	{
		t_str = _T("Angel");
		t_color = RGB(150, 150, 255);
	}
	pDC->SetTextColor(t_color);
	pDC->DrawText(t_str, &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	pDC->SetTextColor(RGB(255,255,255));
//=================================================================================

	t_rect = GetTextOutRect(13);
//	m_pDataOut[13]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[13], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(14);
//	m_pDataOut[14]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[14], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(15);
//	m_pDataOut[15]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[15], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	t_rect = GetTextOutRect(16);
//	m_pDataOut[16]->DrawTexts(pDestSurf, t_rect, 0xffff, 3);
	pDC->DrawText(m_DataOut[16], &t_rect, DT_RIGHT|DT_VCENTER|DT_SINGLELINE);

	pDC->SetTextColor(old_color);
	pDC->SetBkMode(mode);
	pDC->SelectObject(old_font);
	pDestSurf->ReleaseDC(pDC);
}

//우석
void CDialog_Login::SetCharData(CItem_Inv *pItem,int i)
{
	CString buff;
	int max_Exp;

//	m_pDataOut[0]->SetString((CString)m_pChildView->m_CharData[i]->userID); //id.
	m_DataOut[0] = m_pChildView->m_CharData[i]->userID;
	wsprintf(m_DataOut[1].GetBuffer(15),"%d / %d",m_pChildView->m_CharData[i]->Hp,m_pChildView->m_CharData[i]->MaxHp);
	m_DataOut[1].ReleaseBuffer();
//	m_pDataOut[1]->SetString(buff); //hp.
	wsprintf(m_DataOut[2].GetBuffer(15),"%d",m_pChildView->m_CharData[i]->Str);
	m_DataOut[2].ReleaseBuffer();
//	m_pDataOut[2]->SetString(buff); //str.
	m_DataOut[3] = m_pChildView->m_CharData[i]->Group;
//	m_pDataOut[3]->SetString(buff); //str.
	wsprintf(m_DataOut[4].GetBuffer(15),"%d / %d",m_pChildView->m_CharData[i]->Mp,m_pChildView->m_CharData[i]->MaxMp);
	m_DataOut[4].ReleaseBuffer();
//	m_pDataOut[4]->SetString(buff); //mp.
	wsprintf(m_DataOut[5].GetBuffer(15),"%d",m_pChildView->m_CharData[i]->Inteli);
	m_DataOut[5].ReleaseBuffer();
//	m_pDataOut[5]->SetString(buff); //int.
	wsprintf(m_DataOut[6].GetBuffer(15),"%d",m_pChildView->m_CharData[i]->Age);
	m_DataOut[6].ReleaseBuffer();
//	m_pDataOut[6]->SetString(buff); //Age.
	wsprintf(m_DataOut[7].GetBuffer(15),"%d / %d",m_pChildView->m_CharData[i]->Stm,m_pChildView->m_CharData[i]->MaxStm);
	m_DataOut[7].ReleaseBuffer();
//	m_pDataOut[7]->SetString(buff); //stm.
	wsprintf(m_DataOut[8].GetBuffer(15),"%d",m_pChildView->m_CharData[i]->Dex);
	m_DataOut[8].ReleaseBuffer();
//	m_pDataOut[8]->SetString(buff); //Dex.
	wsprintf(m_DataOut[9].GetBuffer(15),"%d",m_pChildView->m_CharData[i]->Fame);
	m_DataOut[9].ReleaseBuffer();
//	m_pDataOut[9]->SetString(buff); //Fame.


	if(m_pChildView->m_CharData[i]->SwordExp > m_pChildView->m_CharData[i]->SpearExp)
	{
	   max_Exp = m_pChildView->m_CharData[i]->SwordExp;
	   m_DataOut[10] = _T("검");
//	   wsprintf(buff.GetBuffer(15),"%s","검");
	}
	else
	{
	   max_Exp = m_pChildView->m_CharData[i]->SpearExp;
	   m_DataOut[10] = _T("창");
//		wsprintf(buff.GetBuffer(15),"%s","창");
	}
	if(max_Exp < m_pChildView->m_CharData[i]->BowExp)
	{
	   max_Exp = m_pChildView->m_CharData[i]->BowExp;
	   m_DataOut[10] = _T("활");
//	   wsprintf(buff.GetBuffer(15),"%s","활");
	}
	if(max_Exp < m_pChildView->m_CharData[i]->AxeExp)
	{
	   max_Exp = m_pChildView->m_CharData[i]->AxeExp;
	   m_DataOut[10] = _T("둔기");
//	   wsprintf(buff.GetBuffer(15),"%s","둔기");
	}
	if(max_Exp < m_pChildView->m_CharData[i]->KnuckleExp)
	{
	   max_Exp = m_pChildView->m_CharData[i]->KnuckleExp;
	   m_DataOut[10] = _T("주먹");
//	   wsprintf(buff.GetBuffer(15),"%s","주먹");
	}
	if(max_Exp < m_pChildView->m_CharData[i]->StaffExp)
	{
	   max_Exp = m_pChildView->m_CharData[i]->StaffExp;
	   m_DataOut[10] = _T("지팡이");
//	   wsprintf(buff.GetBuffer(15),"%s","지팡이");
	}
	if(max_Exp == 0 ) m_DataOut[10] = _T("없음");
//	m_pDataOut[10]->SetString(buff); //Weapon.

	wsprintf(m_DataOut[11].GetBuffer(15),"%d",m_pChildView->m_CharData[i]->Wis);
	m_DataOut[11].ReleaseBuffer();
//	m_pDataOut[11]->SetString(buff); //Wisdom.

	wsprintf(m_DataOut[12].GetBuffer(15),"%d",m_pChildView->m_CharData[i]->Moral);
	m_DataOut[12].ReleaseBuffer();
//	m_pDataOut[12]->SetString(buff); //Moral.

	if(m_pChildView->m_CharData[i]->WeaponMakeExp > m_pChildView->m_CharData[i]->AccMakeExp)
	{
		max_Exp = m_pChildView->m_CharData[i]->WeaponMakeExp;
		m_DataOut[13] = _T("무기제조");
//		wsprintf(buff.GetBuffer(15),"%s","무기제조");
	}
	else
	{
		max_Exp = m_pChildView->m_CharData[i]->AccMakeExp;
		m_DataOut[13] = _T("장신구제조");
//		wsprintf(buff.GetBuffer(15),"%s","장신구제조");
	}
	if(max_Exp < m_pChildView->m_CharData[i]->ArmorMakeExp)
	{
		max_Exp = m_pChildView->m_CharData[i]->ArmorMakeExp;
		m_DataOut[13] = _T("방어구제조");
//		wsprintf(buff.GetBuffer(15),"%s","방어구제조");
	}
	if(max_Exp < m_pChildView->m_CharData[i]->CookingExp)
	{
		max_Exp = m_pChildView->m_CharData[i]->CookingExp;
		m_DataOut[13] = _T("요리");
//		wsprintf(buff.GetBuffer(15),"%s","요리");
	}
	if(max_Exp < m_pChildView->m_CharData[i]->PotionMakeExp)
	{
		max_Exp = m_pChildView->m_CharData[i]->PotionMakeExp;
		m_DataOut[13] = _T("시약제조");
//		wsprintf(buff.GetBuffer(15),"%s","시약제조");
	}
	if(max_Exp == 0) m_DataOut[13] = _T("없음");
//	m_pDataOut[13]->SetString(buff); //Skill.

	wsprintf(m_DataOut[14].GetBuffer(15),"%d",m_pChildView->m_CharData[i]->Cha);
	m_DataOut[14].ReleaseBuffer();
//	m_pDataOut[14]->SetString(buff); //cha
	wsprintf(m_DataOut[15].GetBuffer(15),"%d",m_pChildView->m_CharData[i]->Level);
	m_DataOut[15].ReleaseBuffer();
//	m_pDataOut[15]->SetString(buff); //Level.
	wsprintf(m_DataOut[16].GetBuffer(15),"%d",m_pChildView->m_CharData[i]->Con);
	m_DataOut[16].ReleaseBuffer();
//	m_pDataOut[16]->SetString(buff); //Con.

//	buff.ReleaseBuffer();
}

CRect CDialog_Login::GetTextOutRect(int index)
{
	CRect rect;

	rect = m_arrArea[index]->m_vRect; 

	return rect;
}


void CDialog_Login::NewCharDraw(CDDSurface *pDestSurf)
{
	int skip_color =(int) m_frameRate / 10 ;
	skip_color /= 2;

 	CItem_Inv  *NewManChar = NULL;
	CItem_Inv  *NewWomanChar = NULL;
 	CItem_Inv  *NextNewManChar = NULL;
	CItem_Inv  *NextNewWomanChar = NULL;

 	CItem_Inv  *pSrcSkinColorChar = NULL;
	CItem_Inv  *pDstSkinColorChar = NULL;

	switch(m_SkinColorChange)
	{
		case 0: NewManChar = m_pNewCharItem01; //남자..
				NewWomanChar = m_pNewCharItemWoman01;
			   break;
		case 1: NewManChar = m_pNewCharItem02; //남자..
				NewWomanChar = m_pNewCharItemWoman02;
			   break;
		case 2: NewManChar = m_pNewCharItem03; //남자..
				NewWomanChar = m_pNewCharItemWoman03;
			   break;
	}

	if(m_bSex) pSrcSkinColorChar = NewWomanChar;
	else       pSrcSkinColorChar = NewManChar;

	switch(m_NextSkinColorChange)
	{
		case 0: NextNewManChar = m_pNewCharItem01; //남자..
				NextNewWomanChar = m_pNewCharItemWoman01;
			   break;
		case 1: NextNewManChar = m_pNewCharItem02; //남자..
				NextNewWomanChar = m_pNewCharItemWoman02;
			   break;
		case 2: NextNewManChar = m_pNewCharItem03; //남자..
				NextNewWomanChar = m_pNewCharItemWoman03;
			   break;
	}

	if(m_bSex) pDstSkinColorChar = NextNewWomanChar;
	else       pDstSkinColorChar = NextNewManChar;

    if(m_IsMovePixel)
	{
//===============================================
		switch(m_DrawMode)
		{
			case 0: //캐릭터 선택.
					if( ChangeVerticesColor(m_bSex, skip_color, NewManChar, NewWomanChar) )
					{
					   m_IsMovePixel = false;
					   m_HairColorIndex = 1;
 					   m_HairStyleIndex = 0;

					   	switch(m_bSex)
						{
						case 0:			
								//NewManChar->m_timegab = 110;
								//NewManChar->m_bCahrBlend = false;
								//NewManChar->DrawPrimitiveChar(m_ptMakeChar1.x, m_ptMakeChar1.y);
								NewWomanChar->HairDraw(120,205,m_HairStyleIndex,m_HairColorIndex);
							   break;
						case 1:
								//NewWomanChar->m_timegab = 110;					
								//NewWomanChar->m_bCahrBlend = false;
								//NewWomanChar->DrawPrimitiveChar(m_ptMakeChar2.x, m_ptMakeChar2.y);
								NewManChar->HairDraw(120,205,m_HairStyleIndex,m_HairColorIndex);
							   break;
						}

						//m_SkinColorChange = 0;
						//m_NextSkinColorChange = 0;												
					}
					else
					{
						switch(m_bSex)
						{
							case 0: NewManChar->m_bCahrBlend = true;
									NewWomanChar->m_bCahrBlend = true;
									break;
							case 1: NewManChar->m_bCahrBlend = true;
									NewWomanChar->m_bCahrBlend = true;
									break;
						}
						if(NewManChar)
						{
							NewManChar->m_timegab = 110;
							NewManChar->DrawPrimitiveChar(m_ptMakeChar1.x, m_ptMakeChar1.y);
						}
						if(NewWomanChar)
						{
							NewWomanChar->m_timegab = 110;
							NewWomanChar->DrawPrimitiveChar(m_ptMakeChar2.x, m_ptMakeChar2.y);
						}
					}
					break;

			case 1: // 피부선택
		 		  if( ChangeVerticesColor(m_bSex, skip_color, pSrcSkinColorChar, pDstSkinColorChar) )
				  {
						m_IsMovePixel = false;
						m_SkinColorChange = m_NextSkinColorChange;
						m_bSkinColorChangeEnd = true;
//						m_DrawMode = 0;

						//m_HairColorIndex = 1;
						//m_HairStyleIndex = 0;

					   	switch(m_bSex)
						{
						case 0:			
								//NewManChar->m_timegab = 110;
								//NewManChar->m_bCahrBlend = false;
								//NewManChar->DrawPrimitiveChar(m_ptMakeChar1.x, m_ptMakeChar1.y);
								NewManChar->HairDraw(120,205,0,1);
								NextNewManChar->HairDraw(120,205,m_HairStyleIndex,m_HairColorIndex);
							   break;
						case 1:
								//NewWomanChar->m_timegab = 110;					
								//NewWomanChar->m_bCahrBlend = false;
								//NewWomanChar->DrawPrimitiveChar(m_ptMakeChar2.x, m_ptMakeChar2.y);
								NewWomanChar->HairDraw(120,205,0,1);
								NextNewWomanChar->HairDraw(120,205,m_HairStyleIndex,m_HairColorIndex);
							   break;
						}									
				  }
				  else
				  {
						switch(m_bSex)
						{
							case 0: NewManChar->m_bCahrBlend = true;
									NextNewManChar->m_bCahrBlend = true;

									NewManChar->m_timegab = 110;
									NewManChar->DrawPrimitiveChar(m_ptMakeChar1.x, m_ptMakeChar1.y);

									NextNewManChar->m_timegab = 110;
									NextNewManChar->DrawPrimitiveChar(m_ptMakeChar1.x, m_ptMakeChar1.y);
									NextNewManChar->HairDraw(120,205,m_HairStyleIndex,m_HairColorIndex);
									break;
							case 1: NewWomanChar->m_bCahrBlend = true;
									NextNewWomanChar->m_bCahrBlend = true;

									NewWomanChar->m_timegab = 110;
									NewWomanChar->DrawPrimitiveChar(m_ptMakeChar2.x, m_ptMakeChar2.y);

									NextNewWomanChar->m_timegab = 110;
									NextNewWomanChar->DrawPrimitiveChar(m_ptMakeChar2.x, m_ptMakeChar2.y);
									NextNewWomanChar->HairDraw(120,205,m_HairStyleIndex,m_HairColorIndex);
									break;
						}
				  }
				  break;
		}		
	}
//============================================
	//else
	if(!m_IsMovePixel)
	{
		if(m_DrawMode == 0)
		{
			switch(m_bSex)
			{
			case 0:			
					NewManChar->m_timegab = 110;
					NewManChar->m_bCahrBlend = false;
					NewManChar->DrawPrimitiveChar(m_ptMakeChar1.x, m_ptMakeChar1.y);
				   break;
			case 1:
					NewWomanChar->m_timegab = 110;					
					NewWomanChar->m_bCahrBlend = false;
					NewWomanChar->DrawPrimitiveChar(m_ptMakeChar2.x, m_ptMakeChar2.y);
				   break;
			}
		}
		else
		{
			switch(m_bSex)
			{
			case 0:			
					NextNewManChar->m_timegab = 110;
					NextNewManChar->m_bCahrBlend = false;
					NextNewManChar->DrawPrimitiveChar(m_ptMakeChar1.x, m_ptMakeChar1.y);
				   break;
			case 1:
					NextNewWomanChar->m_timegab = 110;					
					NextNewWomanChar->m_bCahrBlend = false;
					NextNewWomanChar->DrawPrimitiveChar(m_ptMakeChar2.x, m_ptMakeChar2.y);
				   break;
			}
		}
	}

	m_pName->SetFocus();
	m_pName->GetText();

	if(m_pName) m_pName->DCDrawOffsetedCurText(pDestSurf);	//에디터에 설정된 글 그리기(Editor 는 원래 안보이는데 있음)

	MakeCharData(pDestSurf);
	DrawNewCharExplain();

	DDSURFACEDESC2 ddsd;
	pDestSurf->Lock();
	ddsd = pDestSurf->GetDescription();
	m_water.Draw(ddsd); // 물방울 출력.
	pDestSurf->Unlock();
}

void CDialog_Login::ResetData()
{
	m_SkinColorChange = 0;
	m_NextSkinColorChange = 0;
	m_bSkinColorChangeEnd = true;
	m_HairColorIndex = 1;
	m_HairStyleIndex = 0;
}

void CDialog_Login::ChangeCharacter(int rotate)
{
	m_bChangingChar = rotate;
}

void CDialog_Login::SetCharacter(CItem_Inv *item1, CItem_Inv *item2, CItem_Inv *item3)
{
	m_pItem1 = item1;
	m_pItem2 = item2;
	m_pItem3 = item3;

	m_ViewCharNum = 0;

	m_pItem1->m_bCahrBlend = false;
	m_pItem2->m_bCahrBlend = true;
	m_pItem3->m_bCahrBlend = true;
}

void CDialog_Login::LoginMouseProcess(BBDX_MouseInformation *pMouseInfo)
{
/*	if(m_vViewMode == 0)
	{
		if(pMouseInfo->IsLButtonDown())
		{
			m_vCurOk = m_Dialog_Box_Flag;
		}
		if(pMouseInfo->IsLButtonUp())
		{
			if(m_Dialog_Box_Flag == m_vCurOk) m_vOk = m_vCurOk;
			m_vCurOk = 0;
		}
	}
	else
	{
		if(pMouseInfo->IsLButtonDown())
		{
			m_vCurOk = m_Dialog_Box_Flag;
			if(m_vCurOk == 14) m_bSex = FALSE;
			else if(m_vCurOk == 15) m_bSex = TRUE;
		}
		if(pMouseInfo->IsLButtonUp())
		{
			if(m_Dialog_Box_Flag == m_vCurOk) m_vOk = m_vCurOk;
			m_vCurOk = 0;
		}
	}
*/
}

void CDialog_Login::InitLogin()
{
}

void CDialog_Login::InitNewChar()
{
	CString buff;
	
	wsprintf(m_DataOut[0].GetBuffer(11),"%d",m_Strength);
	m_DataOut[0].ReleaseBuffer();
//	m_pDataOut[0]->SetString(buff); 
	wsprintf(m_DataOut[1].GetBuffer(11),"%d",m_Dexterity);
	m_DataOut[1].ReleaseBuffer();
//	m_pDataOut[1]->SetString(buff); 
	wsprintf(m_DataOut[2].GetBuffer(11),"%d",m_Inteligence);
	m_DataOut[2].ReleaseBuffer();
//	m_pDataOut[2]->SetString(buff); 
	wsprintf(m_DataOut[3].GetBuffer(11),"%d",m_Charisma);
	m_DataOut[3].ReleaseBuffer();
//	m_pDataOut[3]->SetString(buff); 
	wsprintf(m_DataOut[4].GetBuffer(11),"%d",m_Wisdom);
	m_DataOut[4].ReleaseBuffer();
//	m_pDataOut[4]->SetString(buff);
	wsprintf(m_DataOut[5].GetBuffer(11),"%d",m_Constitution);
	m_DataOut[5].ReleaseBuffer();
//	m_pDataOut[5]->SetString(buff);
	wsprintf(m_DataOut[6].GetBuffer(11),"%d",m_Points);
	m_DataOut[6].ReleaseBuffer();
//	m_pDataOut[6]->SetString(buff); // points

//	buff.ReleaseBuffer();
}

void CDialog_Login::SetMode(int mode)
{
	m_vViewMode = mode;
}

void CDialog_Login::SetMessage( CString str )
{
	m_ErrorText = str;
}

//에러 문자열 출력.
void CDialog_Login::DrawErrorText(CDDSurface *pDestSurf)
{
//	if(m_pTWFont)
//	  m_pTWFont->IC_TextOutClip(m_vMsgRect, m_ErrorText, 0xffff, ddsd, 0, 0, -1, 5); //우석.
	CDC *pDC = pDestSurf->GetDC();
	CFont *old_font = (CFont *)pDC->SelectObject(m_pFont);
	COLORREF old_color = pDC->SetTextColor(RGB(255,255,255));
	int mode = pDC->SetBkMode(TRANSPARENT);

	pDC->DrawText(m_ErrorText, &m_vMsgRect, DT_LEFT|DT_TOP|DT_WORDBREAK);

	pDC->SetTextColor(old_color);
	pDC->SetBkMode(mode);
	pDC->SelectObject(old_font);
	pDestSurf->ReleaseDC(pDC);

	TRACE(".........................\n");
}

bool CDialog_Login::LoginCharData(int dir, int front_num, float time_gap)
{
	bool       IsOK1,IsOK2,IsOK3;
	POINT	   dstpt1;
	POINT      dstpt2;
	POINT      dstpt3;

	dstpt1.x = 284; dstpt1.y = 150;
	dstpt2.x = 490; dstpt2.y = 80;
	dstpt3.x = 90;  dstpt3.y = 80;

	switch(dir)
	{
		case 1:
				switch(front_num)
				{
					case 0: IsOK1 = SDrawLine(m_ptSrcChar1.x,m_ptSrcChar1.y, dstpt1.x,dstpt1.y, 1, time_gap);
							IsOK2 = SDrawLine(m_ptSrcChar2.x,m_ptSrcChar2.y, dstpt2.x,dstpt2.y, 1, time_gap);
							IsOK3 = SDrawLine(m_ptSrcChar3.x,m_ptSrcChar3.y, dstpt3.x,dstpt3.y, 2, time_gap);								
							break;
					case 1: IsOK1 = SDrawLine(m_ptSrcChar1.x,m_ptSrcChar1.y, dstpt3.x,dstpt3.y, 2, time_gap);
							IsOK2 = SDrawLine(m_ptSrcChar2.x,m_ptSrcChar2.y, dstpt1.x,dstpt1.y, 1, time_gap);
							IsOK3 = SDrawLine(m_ptSrcChar3.x,m_ptSrcChar3.y, dstpt2.x,dstpt2.y, 1, time_gap);
							break;
					case 2: IsOK1 = SDrawLine(m_ptSrcChar1.x,m_ptSrcChar1.y, dstpt2.x,dstpt2.y, 1, time_gap);
							IsOK2 = SDrawLine(m_ptSrcChar2.x,m_ptSrcChar2.y, dstpt3.x,dstpt3.y, 2, time_gap);
							IsOK3 = SDrawLine(m_ptSrcChar3.x,m_ptSrcChar3.y, dstpt1.x,dstpt1.y, 1, time_gap);
							break;
				}
			   break;

		case -1:
				switch(front_num)
				{
					case 0: IsOK1 = SDrawLine(m_ptSrcChar1.x,m_ptSrcChar1.y, dstpt1.x,dstpt1.y, 1, time_gap);
							IsOK2 = SDrawLine(m_ptSrcChar2.x,m_ptSrcChar2.y, dstpt2.x,dstpt2.y, 2, time_gap);
							IsOK3 = SDrawLine(m_ptSrcChar3.x,m_ptSrcChar3.y, dstpt3.x,dstpt3.y, 1, time_gap);								
							break;
					case 1: IsOK1 = SDrawLine(m_ptSrcChar1.x,m_ptSrcChar1.y, dstpt3.x,dstpt3.y, 1, time_gap);
							IsOK2 = SDrawLine(m_ptSrcChar2.x,m_ptSrcChar2.y, dstpt1.x,dstpt1.y, 1, time_gap);
							IsOK3 = SDrawLine(m_ptSrcChar3.x,m_ptSrcChar3.y, dstpt2.x,dstpt2.y, 2, time_gap);
							break;
					case 2: IsOK1 = SDrawLine(m_ptSrcChar1.x,m_ptSrcChar1.y, dstpt2.x,dstpt2.y, 2, time_gap);
							IsOK2 = SDrawLine(m_ptSrcChar2.x,m_ptSrcChar2.y, dstpt3.x,dstpt3.y, 1, time_gap);
							IsOK3 = SDrawLine(m_ptSrcChar3.x,m_ptSrcChar3.y, dstpt1.x,dstpt1.y, 1, time_gap);
							break;
				}
			   break;
		       break;
	}

	if( IsOK1 && IsOK2 && IsOK3)
		return true;
	else return false;
}

//버텍스 직선 이동 선긋기...
bool CDialog_Login::SDrawLine(long &x_start, long &y_start, long x_end, long y_end, int gap_step, float time_gap)
{
	float dx,dy;
	float x = 0.0, y = 0.0;
	float gap;


	dx = x_end - x_start;
	dy = y_end - y_start;

	time_gap += 2.5;

	gap = fabs(dx / time_gap);

	if(gap > 2.5)
	{
		gap /= gap_step; // 속도차이....준다..
		
		x = dx / gap;
		y = dy / gap;
		x_start += x;
		y_start += y;

		return false;
	}
	else
	{
		x_start = x_end;
		y_start = y_end;
		return true;
	}
}

bool CDialog_Login::ChangeVerticesColor(int Sex, int skip_color, CItem_Inv *ManChar, CItem_Inv *WomanChar)
{
	if(ManChar->m_alphastate) // +
	{
		ManChar->m_alpha += 1;
		if(ManChar->m_alpha == 32) ManChar->m_alphastate = false;
	}
	else // - 
	{
		ManChar->m_alpha -= 1;
		if(ManChar->m_alpha == 0) ManChar->m_alphastate = true;
	}
	

	if(WomanChar->m_alphastate) // +
	{
		WomanChar->m_alpha += 1;
		if(WomanChar->m_alpha == 32)
		{
			WomanChar->m_alphastate = false;
			return true;
		}
	}
	else // - 
	{
		WomanChar->m_alpha -= 1;
		if(WomanChar->m_alpha == 0)
		{
			WomanChar->m_alphastate = true;
			return true;
		}
	}	

	return false;
}
