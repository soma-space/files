// Item_Small.cpp: implementation of the CItem_Small class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Item_Small.h"
#include "ChildView.h"
#include "Item_Total.h"

//extern CItem_sArm    Item_sArm;
extern CItem_Total         Item_Total;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItem_Small::CItem_Small()
{
	m_Caption = 0;// 아이템 설명 캡션을 뜨게 하는 옵션 플레그..
}

CItem_Small::~CItem_Small()
{
}


void CItem_Small::InitItem(CDirectDraw *pDirectDraw)
{
	InitData(pDirectDraw);
	for(int i=0;i<50;i++)
	{
		m_Item_Fixpos[i] = m_Item_pos[i];// 자리는 고정시킨다.음냐..
		m_Shop_Pic[i] = -1;
		m_Shop_pos[i] = CPoint(0,0);		
		m_Shop_Name[i] = _T("");
		m_ItemTableNumber[i] = -1;
	}

	m_UpDown=0;
	m_UpDown_Shop=0;
	m_UpDown_Shopme=0;
	m_ShopSelect=-1;
	m_ShopMeSelect = -1;

	m_ShopNumber=-1;//상점번호

//	Load_image("item.bmp");// 임시로 갑옷을 한다..컴은 없으니깐..쩝..
	LoadObm("item.obm");
	LoadSpr("item.spr");
//	Load_image("item_test.bmp");// 임시로 갑옷을 한다..컴은 없으니깐..쩝..
//	LoadSpr("item_test.spr");

}

BOOL CItem_Small::MouseProcess(BBDX_MouseInformation *pMouseInfo, int Dlg_Pos, int Select_Box, int MoveFlag)
{
	// 고유한 플래그를 가진다.m_Item_Flag
	// 몇번 이상은...어떤 메뉴..몇번 있아은 몇번 메뉴..이런식으루..쩝..
	// 마우스가 움직일때 동작..
	int i;

	if(m_Select_Item >= 0)
	{
		i = m_Select_Item;
		if(m_Move_Flag[i] )// = TRUE;// 드래그를 할때..사용해야징..
		{
			// 이곳에서 아이템을 어떤 장소에다가 놓았는지 판단해야된다..
			if(pMouseInfo->IsLButtonUp())
			{
				m_Move_Flag[i] = FALSE;
				m_Caption = 0;// 설명 창을 없애 버린다.
//				CString str;
//				str.Format("item을 %d번다이얼로그의 %d번 상자에 놓았다11111111.", Dlg_Pos, Select_Box);
//				CChildView::SocketMng.SendChat(str);
				//Dlg_Pos : 1 메인 창 2 : 인벤토리창 3 : 

				// 원래 자리로 돌아 오게 만든다...나중에 이동을 하게 하던가 해야겠쥐..음..
				// 놓는 위치가 어디냐에 따라서..
				m_Select_Item = -1;
				if(MoveFlag == 0) return TRUE;

				m_Item_pos[i] = m_Item_Fixpos[i];

				if(Dlg_Pos == 2)
				{
					if(Select_Box ==0)
					{
						CChildView::ChangeItem(i);
						Output(i);
					}
				}
				else if(Dlg_Pos == 0)// 버린다. 
				{
//					CString str;
//					str.Format("바닥에 %d번item을 버렸다.", i);
//					CChildView::SocketMng.SendChat(str);	
					CPoint MyPos = CChildView::UserManager.GetMyCellPos();
					
					BYTE buf[10];
					int offset = 0;
					CPacketParser::SetByte(buf, offset, PKT_ITEM_THROW); //최종점 요청
					CPacketParser::SetShort(buf, offset, (short)i);
					CPacketParser::SetShort(buf, offset, (short)MyPos.x);
					CPacketParser::SetShort(buf, offset, (short)MyPos.y);
					CChildView::SocketMng.Send(offset, buf);

					Output(i);// 바닥에다 버릴때..음...
				}
			}
			else if (pMouseInfo->IsLButtonPressing())
			{
				if(m_Move_Flag[i])// = TRUE;// 드래그를 할때..사용해야징..
				{
					// 아이템 무브....
					int move_x, move_y;
					move_x = m_Old_Pos.x - pMouseInfo->m_posMouse.x;
					move_y = m_Old_Pos.y - pMouseInfo->m_posMouse.y;

					m_Item_pos[i].x -= move_x;
					m_Item_pos[i].y -= move_y;

					m_Old_Pos = pMouseInfo->m_posMouse;
				}
			}
			return TRUE;
		}
	}


	for(i=0; i<100;i++)
	{
		if(InMousePos(pMouseInfo->m_posMouse, m_BoxRect[i]) || m_Move_Flag[i])
		{//메뉴바위에 있다..
//			if(pMouseInfo->IsLButtonDobleDown()&& MoveFlag == 1)
//			{
//				CChildView::ChangeItem(i);
//				Output(i);
//			}
//			else
			
			if(pMouseInfo->IsRButtonDown())// 아이템 장착또는 사용할때..
			{
				CChildView::ChangeItem(i);
				Output(i);
			}

			if (pMouseInfo->IsLButtonDown()) 
			{
				m_Move_Flag[i] = TRUE;// 드래그를 할때..사용해야징..
				m_Old_Pos = pMouseInfo->m_posMouse;
				m_Current_Item = i;// 현재 선택되어있는..아이템..번호..
				m_Select_Item = i;// 마우스를 놓으면 초기화 됨.
				
				if(i < 50)	m_ShopMeSelect = i; 
				else m_ShopSelect = i;// 상점에서 선택된 아이템이 유지되게 하기위해서..

				m_Caption = 1;// 설명 창이 뜨게 한다.
			}
			m_Item_Flag = i;
			return TRUE;
			
		}
		else if(m_Item_Flag == i)
			m_Item_Flag = -1;
	}
	return FALSE;
}

void CItem_Small::ViewCaption(CPoint pos, CString str, int tmp_y)
{
	DWORD cur = GetTickCount();
	int DEFAULT_BUBBLECHAT_WIDTH = 300;
	int len = str.GetLength();

	pos.x += 20;
	pos.y += 10;

	pos.y -= tmp_y;

//	CPoint pos;
//	pos.x = m_Position.x - bgpos.x - 50;
//	pos.y = m_Position.y - bgpos.y - 80;

	len = len/14 + 1;
	//ASSERT (line >= 1);			
	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
	
	pDC->SetBkMode(TRANSPARENT);
	CFont pFont, *old_font;
	CRect r;

	////////////////////////
	pFont.CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");
	old_font = pDC->SelectObject(&pFont);
	/////////////////////////

	CSize size = pDC->GetTextExtent(str);
	int line = size.cx / DEFAULT_BUBBLECHAT_WIDTH + 1;
	
	if(size.cx < DEFAULT_BUBBLECHAT_WIDTH)
		DEFAULT_BUBBLECHAT_WIDTH = size.cx;

	r.SetRect(pos.x , pos.y, pos.x+DEFAULT_BUBBLECHAT_WIDTH, pos.y+line*size.cy);
	
	//r.OffsetRect(-DEFAULT_BUBBLECHAT_WIDTH/2 , -100 - (line)*size.cy);
	r.OffsetRect(0 , - (line)*size.cy);

	// 검은색
	pDC->SetTextColor(RGB(5,5,5));
	//pDC->DrawText(m_szChatString, &r, DT_WORDBREAK|DT_CENTER|DT_TOP);
	pDC->DrawText(str, &r, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	
	// 노란색		
	//pDC->SetTextColor(RGB(255,255,0));
	//pDC->SetTextColor(RGB(181,181, 255));

	

	CRect temp = r;
	CRect temp2 = r;
	
	temp.OffsetRect(-1, -1);

	temp2.OffsetRect(-3, -4);
	temp2.right +=6;
	temp2.bottom += 7;

	//pDC->DrawText(m_szChatString, &temp, DT_WORDBREAK|DT_CENTER|DT_TOP);
	
	//pDC->FillSolidRect(temp2, RGB(181,181, 255));// 박스를 채워서 그린다.
	pDC->FillSolidRect(temp2, RGB(247,202,26));// 박스를 채워서 그린다.

	//pDC->DrawEdge(temp2, EDGE_BUMP, BF_RECT);
	//pDC->DrawEdge(temp2, EDGE_SUNKEN, BF_RECT);
	pDC->DrawText(str, &temp, DT_SINGLELINE|DT_CENTER|DT_VCENTER);//|DT_CENTER);
	//::DrawText(pDC->m_hDC , m_szChatString, strlen(m_szChatString), &temp, DT_WORDBREAK);

	pDC->SelectObject(old_font);
	pFont.DeleteObject();
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);
	
/////////////////////////////////////////////////////////
}

void CItem_Small::Draw(CDDSurface* pDestSurf, int count, int tmp_y,  int index)
{
	CItem::Draw(pDestSurf, count, tmp_y, index);
	
	if(m_Caption == 1)
	{
		CString str = _T("이 아이템은"); str += m_Item_Name[m_Select_Item]; str += _T("이다.");
		ViewCaption(m_Item_pos[m_Select_Item], str, tmp_y);
	}
}

// 상점이 가지고 있는 아이템을 보여준다.
void CItem_Small::DrawShop(int x, int y, CDDSurface* pDestSurf, int count,  int index)
{
//	CItem::Draw(x, y, pDestSurf, count, index);

	int i = index;
	CPoint pos;
	pos = CPoint(x,y);

	if(x >= 0 && y >= 0 && m_Shop_Pic[i] >=0)
	{
		ViewItemBox(x, y, i+50, m_Shop_Pic[i], pDestSurf);// 상점 아이템이므로...
		ShopItemNumber(pos, i);
	}

	if(m_ShopSelect-50 == index)
		ViewSelectBox(pos,0);


	if(m_Caption == 1 && (m_Select_Item-50) == index)
	{
		CString str = _T("이 아이템은"); str += m_Shop_Name[m_Select_Item-50]; str += _T("이다.");
		ViewCaption(pos, str, 0);
	}
}

// 상점에서 내가 가진 아이템을 보여준다.
void CItem_Small::DrawShopMe(int x, int y, CDDSurface* pDestSurf, int count,  int index)
{
	CItem::Draw(x, y, pDestSurf, count, index);
	
	CPoint pos = CPoint(x,y);

	if(m_ShopMeSelect == index)
		ViewSelectBox(pos,0);

	if(m_Caption == 1 && m_Select_Item == index)
	{
		CString str = _T("이 아이템은"); str += m_Item_Name[m_Select_Item]; str += _T("이다.");
		ViewCaption(pos, str, 0);
	}
}

void CItem_Small::InvWindowDraw(CDDSurface *pDrawSurface)
{
	// 배열을 만들어..우선 순위를 정해서 여기서 뿌려준다..음냐..
	int count =m_UpDown;
	int tmp = 33;
	for(int i=0;i<50;i++)
		m_BoxRect[i] = CRect(0,0,0,0);
		//Item_Total.m_Item_Small->m_BoxRect[i] = CRect(0,0,0,0);// 초기화..
	for(i=8*count;i<24+count*8;i++)// 임시...
	{
		Draw(pDrawSurface,0, count*30+15, i);
		//Item_Total.m_Item_Small->Draw(pDrawSurface,0, count*33+15, i);
	}
}

void CItem_Small::ShopMeWindowDraw(CDDSurface *pDrawSurface)
{
	int x, y, count=0;
	for(int i=0;i<50;i++)
		m_BoxRect[i] = CRect(0,0,0,0);
		//Item_Total.m_Item_Small->m_BoxRect[i] = CRect(0,0,0,0);// 초기화..
	for(i=m_UpDown_Shopme;i<m_UpDown_Shopme+5;i++)
	{
		x = 85;
		y = 199+count*46;
		DrawShopMe(x, y, pDrawSurface,0,  i);
		count++;
	}
}

void CItem_Small::ShopWindowDraw(CDDSurface *pDrawSurface)
{
	int x, y, count=0;
	for(int i=m_UpDown_Shop;i<m_UpDown_Shop+5;i++)
	{
		x = 255;
		y = 199+count*46;
		DrawShop(x, y, pDrawSurface,0,  i);
		count++;
	}
}

void CItem_Small::InvItemUp()
{
	m_UpDown--;
	if(m_UpDown < 0) m_UpDown = 0;
}

void CItem_Small::InvItemDown()
{
	m_UpDown++;
	if(m_UpDown >= 4) m_UpDown = 3;
}

void CItem_Small::ShopItemUp()
{
	m_UpDown_Shop--;
	if(m_UpDown_Shop < 0) m_UpDown_Shop = 0;
}

void CItem_Small::ShopItemDown()
{
	m_UpDown_Shop++;
	if(m_UpDown_Shop < 0) m_UpDown_Shop = 0;
}

void CItem_Small::ShopMeItemUp()
{
	m_UpDown_Shopme--;
	if(m_UpDown_Shopme < 0) m_UpDown_Shopme = 0;
}

void CItem_Small::ShopMeItemDown()
{
	m_UpDown_Shopme++;
	if(m_UpDown_Shopme > 45) m_UpDown_Shopme = 45;
}

void CItem_Small::ViewSelectBox(CPoint pos, int collar)
{
	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();
	
	pDC->SetBkMode(TRANSPARENT);
	CRect r = CRect(pos.x-7, pos.y-5, pos.x+50, pos.y+33);
	//검은색
	//pDC->SetTextColor(RGB(5,5,5));
	//노란색		
	pDC->SetTextColor(RGB(255,255,0));
	//pDC->SetTextColor(RGB(181,181, 255));
	//pDC->FillSolidRect(temp2, RGB(181,181, 255));// 박스를 채워서 그린다.
	//pDC->FillSolidRect(temp2, RGB(247,202,26));// 박스를 채워서 그린다.

	pDC->DrawEdge(r, EDGE_BUMP, BF_RECT);
	//pDC->DrawEdge(temp2, EDGE_SUNKEN, BF_RECT);
	//::DrawText(pDC->m_hDC , m_szChatString, strlen(m_szChatString), &temp, DT_WORDBREAK);
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);
}

void CItem_Small::ShopItemNumber(CPoint pos, int index)
{
	CDC *pDC = CChildView::DirectXMng.m_pDrawSurface->GetDC();

	CString str;//
	str.Format("%3d", m_Shop_Number[index]);

	pDC->SetBkMode(TRANSPARENT);
	CFont pFont, *old_font;
	CRect r;
	////////////////////////
	pFont.CreateFont(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "굴림");
	old_font = pDC->SelectObject(&pFont);
	/////////////////////////

	pDC->SetTextColor(RGB(255,255,0));
	pDC->TextOut(pos.x+30, pos.y+20,str);

	pDC->SelectObject(old_font);
	pFont.DeleteObject();
	CChildView::DirectXMng.m_pDrawSurface->ReleaseDC(pDC);


}
