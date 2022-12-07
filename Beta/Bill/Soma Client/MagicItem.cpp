// MagicItem.cpp: implementation of the CMagicItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MagicItem.h"
#include "ChildView.h"
#include "Item_Total.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//extern CItem_sArm    Item_sArm;
extern CItem_Total         Item_Total;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMagicItem::CMagicItem()
{
	m_Caption = 0;// 아이템 설명 캡션을 뜨게 하는 옵션 플레그..
	m_lpSurface2 = NULL;
	m_SprData2 = NULL;
	m_lpSurf = NULL;
}

CMagicItem::~CMagicItem()
{
	if(m_lpSurf)
	{
		m_lpSurf->Release();
		m_lpSurf = NULL;
	}
	SAFE_DELETE(m_lpSurface2);
	SAFE_DELETES(m_SprData2);
}

void CMagicItem::InitItem(CDirectDraw *pDirectDraw)
{
	InitData(pDirectDraw);
	for(int i=0;i<50;i++)
		m_Item_Fixpos[i] = m_Item_pos[i];// 자리는 고정시킨다.음냐..

	m_UpDown=0;
	m_UpDown_Shop=0;
	m_UpDown_Shopme=0;
	m_ShopSelect=-1;

	m_Item_Pic[0] = 0;// 임시  지워야함..
	m_Item_Pic[1] = 1;

	m_UserItem[0] = -1;
	m_UserItem[1] = -1;
	m_UserItem[2] = -1;

//	Load_image("Magic-B.bmp");// 임시로 갑옷을 한다..컴은 없으니깐..쩝..
	LoadObm("Magic-B.obm");
	LoadSpr("Magic-B.spr");
//	Load_image2("Magic-S.bmp");
	LoadObm2("Magic-S.obm");
	LoadSpr2("Magic-S.spr");
}

void CMagicItem::Load_image2(LPCSTR szFileName)
{
	CString szTmpFileName;// = szFileName;
	BOOL test;

	const CString szExePath = CDirectDraw::GetExecutionPath();
	szTmpFileName.Format("%sitem\\%s", (LPCTSTR)szExePath ,szFileName);

	m_lpSurface2 = new CDDSurface;

	test = m_lpSurface2->CreateFromImage(m_pDirectDraw, szTmpFileName, 0);

	m_lpSurface2->SetColorKey(RGB(255, 0, 255));
}

void CMagicItem::LoadSpr2(LPCSTR szFileName)
{
	int i;
	CString szTmpFileName;
	_SPR_HEADER hSpr;

	const CString szExePath = CDirectDraw::GetExecutionPath();
	szTmpFileName.Format("%sspr\\%s", (LPCTSTR)szExePath ,szFileName);

	CFile* pFile = new CFile(szTmpFileName, CFile::modeRead);
	pFile->Read(&hSpr, sizeof(_SPR_HEADER)); // 헤더 읽기
	
	// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터.
	//_SPR_NORMAL spr;
	m_SprData2 = new _SPR_NORMAL[hSpr.nQt];
	for(i = 0; i < hSpr.nQt; i++)
	{
		pFile->Read(&m_SprData2[i], sizeof(_SPR_NORMAL));
	}
	pFile->Close();
	delete pFile;
}

BOOL CMagicItem::MouseProcess(BBDX_MouseInformation *pMouseInfo, int Dlg_Pos, int Select_Box, int MoveFlag)
{
	// 고유한 플래그를 가진다.m_Item_Flag
	// 몇번 이상은...어떤 메뉴..몇번 있아은 몇번 메뉴..이런식으루..쩝..
	// 마우스가 움직일때 동작..
	int i;

/*	if(m_Select_Item >= 0)
	{
		i = m_Select_Item;
		if(m_Move_Flag[i] )// = TRUE;// 드래그를 할때..사용해야징..
		{
			// 이곳에서 아이템을 어떤 장소에다가 놓았는지 판단해야된다..
			if(pMouseInfo->IsLButtonDobleDown())
			{
				TRACE("매직 아이템 더블클릭..\n");
			}
			/*
			if(pMouseInfo->IsLButtonUp())
			{
				m_Move_Flag[i] = FALSE;
				m_Caption = 0;// 설명 창을 없애 버린다.
//				CString str;
//				str.Format("item을 %d번다이얼로그의 %d번 상자에 놓았다11111111.", Dlg_Pos, Select_Box);
//				CChildView::SocketMng.SendChat(str);		

				// 원래 자리로 돌아 오게 만든다...나중에 이동을 하게 하던가 해야겠쥐..음..
				// 놓는 위치가 어디냐에 따라서..
				m_Select_Item = -1;
				if(MoveFlag == 0) return TRUE;

				m_Item_pos[i] = m_Item_Fixpos[i];

				if(Dlg_Pos != 0)
				{
					//m_Item_Type[i] += 1;
					//Output(i);// 교환 시켜줘야 겠군.큰이미지 하나 작은이미지 하나..이런식으루...음냐..
				//	Item_sArm.Input(4);
				}
				else
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
			*/
//			return TRUE;
//		}
//	}


	for(i=0; i<50;i++)
	{
		if(InMousePos(pMouseInfo->m_posMouse, m_BoxRect[i]) || m_Move_Flag[i])
		{//메뉴바위에 있다..
			if(pMouseInfo->IsLButtonDobleDown())
			{
				m_Current_Item = i;
				
				if(i < 47)
				{
					if(m_UserItem[0] == -1)
						m_UserItem[0] = m_Item_Pic[i];
					else if(m_UserItem[1] == -1)
						m_UserItem[1] = m_Item_Pic[i];
					else if(m_UserItem[2] == -1)
						m_UserItem[2] = m_Item_Pic[i];
					else
						TRACE("아이템 박스가 다차있다.\n");
				}
				else// 장착 되어 있는...마법아이템.
				{
					m_UserItem[i-47] = -1;// 제거한다. 
				}

				//TRACE("매직 아이템 더블클릭..\n");
			}
			else if (pMouseInfo->IsLButtonDown()) 
			{
			}
/*				m_Move_Flag[i] = TRUE;// 드래그를 할때..사용해야징..
				m_Old_Pos = pMouseInfo->m_posMouse;c
				m_Current_Item = i;// 현재 선택되어있는..아이템..번호..
//				CString str;
//				str.Format(" %d번 item을 선택", m_Current_Item);
//				CChildView::SocketMng.SendChat(str);		
				m_Select_Item = i;// 마우스를 놓으면 초기화 됨.
				m_ShopSelect = i;// 상점에서 선택된 아이템이 유지되게 하기위해서..
				m_Caption = 1;// 설명 창이 뜨게 한다.

//				ViewCaption(pMouseInfo->m_posMouse, "연습이지롱 ^^");

			}
*/
			m_Item_Flag = i;
			return TRUE;
			
		}
		else if(m_Item_Flag == i)
			m_Item_Flag = -1;
	}
	return FALSE;
}

void CMagicItem::ViewCaption(CPoint pos, CString str, int tmp_y)
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

void CMagicItem::Draw(CDDSurface* pDestSurf, int count, int tmp_y,  int index)
{
	CItem::Draw(pDestSurf, count, tmp_y, index);
	
	if(m_Caption == 1)
	{
		CString str = _T("이 아이템은"); str += m_Item_Name[m_Select_Item]; str += _T("이다.");
		ViewCaption(m_Item_pos[m_Select_Item], str, tmp_y);
	}
}

void CMagicItem::DrawShop(int x, int y, CDDSurface* pDestSurf, int count,  int index)
{
	CItem::Draw(x, y, pDestSurf, count, index);
	
//	CPoint pos = CPoint(x,y);
//	if(m_ShopSelect == index)

//	ViewSelectBox(pos,0);

/*	if(m_Caption == 1 && m_Select_Item == index)
	{

		CString str = _T("이 아이템은"); str += m_Item_Name[m_Select_Item]; str += _T("이다.");
		ViewCaption(pos, str, 0);
	}
*/
}

void CMagicItem::InvWindowDraw(CDDSurface *pDrawSurface)
{
	// 배열을 만들어..우선 순위를 정해서 여기서 뿌려준다..음냐..
	int count = 0;//m_UpDown;
	int tmp = 33;
	for(int i=0;i<50;i++)
		m_BoxRect[i] = CRect(0,0,0,0);// 초기화..
	for(i=4*count;i<8+count*4;i++)// 임시...
	{
		Draw(pDrawSurface,0, count*40+15, i);
	}
}

void CMagicItem::ShopMeWindowDraw(CDDSurface *pDrawSurface)
{
	int x, y, count=0;
	for(int i=0;i<50;i++)
		m_BoxRect[i] = CRect(0,0,0,0);// 초기화..
	for(i=m_UpDown_Shopme;i<m_UpDown_Shopme+5;i++)
	{
		x = 85;
		y = 199+count*46;
		DrawShop(x, y, pDrawSurface,0,  i);
		count++;
	}
}

void CMagicItem::ShopWindowDraw(CDDSurface *pDrawSurface)
{
	int x, y, count=0;
	for(int i=m_UpDown_Shop;i<m_UpDown_Shop+5;i++)
	{
		x = 50;
		y = 153+count*75;
		DrawShop(x, y, pDrawSurface,0,  i);
		count++;
	}

	for(i=0;i<3;i++)
	{
		x = 474 + 47*i;
		y = 542;
		if(m_UserItem[i] >= 0)
			ViewItemBox(x, y, 47+i, m_UserItem[i], pDrawSurface);
		//m_UserItem[i]
	}
}

void CMagicItem::InvItemUp()
{
	m_UpDown--;
	if(m_UpDown < 0) m_UpDown = 0;
}

void CMagicItem::InvItemDown()
{
	m_UpDown++;
	if(m_UpDown >= 4) m_UpDown = 3;
}

void CMagicItem::ShopItemUp()
{
	m_UpDown_Shop--;
	if(m_UpDown_Shop < 0) m_UpDown_Shop = 0;
}

void CMagicItem::ShopItemDown()
{
	m_UpDown_Shop++;
	if(m_UpDown_Shop < 0) m_UpDown_Shop = 0;
}

void CMagicItem::ShopMeItemUp()
{
	m_UpDown_Shopme--;
	if(m_UpDown_Shopme < 0) m_UpDown_Shopme = 0;
}

void CMagicItem::ShopMeItemDown()
{
	m_UpDown_Shopme++;
	if(m_UpDown_Shopme > 45) m_UpDown_Shopme = 45;
}

void CMagicItem::ViewSelectBox(CPoint pos, int collar)
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

void CMagicItem::LoadObm2(LPCTSTR szFileName)
{
	CString szTmpFileName;

	const CString szExePath = CDirectDraw::GetExecutionPath();
	szTmpFileName.Format("%sitem\\%s", (LPCTSTR)szExePath ,szFileName);

	m_lpSurf2 = CChildView::g_pTEng.SurfaceCreateWith16BMP(szTmpFileName, FALSE, RGB(255, 0, 255), TRUE);

	m_lpSurface2 = new CDDSurface(m_lpSurf2);
}