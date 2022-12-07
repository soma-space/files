// Item.cpp: implementation of the CItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Item.h"
#include "ChildView.h"

#define RELEASE(exp) if(exp!=NULL) { exp->Release(); exp=NULL; }
#define CLEAR(exp) memset(&exp, 0, sizeof(exp))

DirectXMng			CItem::DirectXMng;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CItem::CItem()
{
	m_lpSurface = NULL;
	m_pDirectDraw = NULL;
	m_SprData = NULL;
	m_lpSurf = NULL;
}

CItem::~CItem()
{
	if(m_lpSurf)
	{
		m_lpSurf->Release();
		m_lpSurf = NULL;
	}
	SAFE_DELETE(m_lpSurface);
	SAFE_DELETES(m_SprData);
}

void CItem::InitData(CDirectDraw *pDirectDraw)// 다이렉트 드로우를 가지고 온다..반드시 초기화 시켜줘야함..
{
	m_pDirectDraw = pDirectDraw;

//	m_lpSurface = new CDDSurface;

	m_Item_Flag = -1;

	m_Select_Item = -1;//현재 드래그 되고 있는 아이템을 갈켜준다..잡고있는...

	m_Count = 0; // 아이템 배열을 순차적으로 채우기 위해서..

	m_Old_Pos.x = 0;
	m_Old_Pos.y = 0;

	m_Current_Item = -1;
	
	for(int i=0; i<50; i++)
	{
		m_BoxRect[i] = CRect(0,0,0,0);
		m_Move_Flag[i] = FALSE;

		m_Item_pos[i].x = 35 + i%8*40;
		m_Item_pos[i].y = 395 + i/8*33;
		m_Item_Pic[i] = -1;
	}

	for(i=50;i<100;i++)
	{
		m_BoxRect[i] = CRect(0,0,0,0);
		m_Move_Flag[i] = FALSE;
	}	
//	Load_image("test.bmp");
//	LoadSpr("test.spr");

}

void CItem::Load_image(LPCSTR	szFileName)
{
	CString szTmpFileName;// = szFileName;
	BOOL test;

	const CString szExePath = CDirectDraw::GetExecutionPath();
	szTmpFileName.Format("%sitem\\%s", (LPCTSTR)szExePath ,szFileName);

	m_lpSurface = new CDDSurface;

	test = m_lpSurface->CreateFromImage(m_pDirectDraw, szTmpFileName, 0);
		
	m_lpSurface->SetColorKey(RGB(255, 0, 255));
}


void CItem::Draw(CDDSurface* pDestSurf, int count, int tmp_y,  int index)
{
	int i = index;

	if(m_Item_Pic[i]>0 &&m_Item_pos[i].x < 0)// 옮겨진 데이타라는 소리..
	{
		m_Item_pos[i] = m_Item_Fixpos[i];
	}


	int x = m_Item_pos[i].x;
	int y = m_Item_pos[i].y - tmp_y;

	if(x >= 0 && y >= 0 && m_Item_Pic[i] >=0)
		ViewItemBox(x, y, i, m_Item_Pic[i], pDestSurf);
}

void CItem::Draw(int x, int y, CDDSurface* pDestSurf, int count, int index)
{
	int i = index;

	if(x >= 0 && y >= 0 && m_Item_Pic[i] >=0)
		ViewItemBox(x, y, i, m_Item_Pic[i], pDestSurf);
}

void CItem::LoadSpr(LPCSTR	szFileName)
{
	int i;
	CString szTmpFileName;// = szFileName;

	const CString szExePath = CDirectDraw::GetExecutionPath();
	szTmpFileName.Format("%sspr\\%s", (LPCTSTR)szExePath ,szFileName);

	CFile* pFile = new CFile(szTmpFileName, CFile::modeRead);
	pFile->Read(&m_HSpr, sizeof(_SPR_HEADER)); // 헤더 읽기
	
	// 스프라이트 영역 - 툴에서만 쓰이는 부가적인 데이터.
	//_SPR_NORMAL spr;
	m_SprData = new _SPR_NORMAL[m_HSpr.nQt];
	for(i = 0; i < m_HSpr.nQt; i++)
	{
		//pFile->Read(&spr, sizeof(_SPR_NORMAL));
		pFile->Read(&m_SprData[i], sizeof(_SPR_NORMAL));
	}
	pFile->Close();
	delete pFile;
}

BOOL CItem::MouseProcess(BBDX_MouseInformation *pMouseInfo, int Dlg_Pos)
{
	// 고유한 플래그를 가진다.m_Item_Flag
	// 몇번 이상은...어떤 메뉴..몇번 있아은 몇번 메뉴..이런식으루..쩝..
	// 마우스가 움직일때 동작..
	if(1)//pMouseInfo->IsMouseMove())
	{
		for(int i=0; i<50;i++)
		{
			// 끝났으면..중간에 멈추게 해야되겠쥐...
			if(m_BoxRect[i].right <= 0) return FALSE;

			if(InMousePos(pMouseInfo->m_posMouse, m_BoxRect[i]) || m_Move_Flag[i])
			{//메뉴바위에 있다..
				if (pMouseInfo->IsLButtonDown()) 
				{
					m_Move_Flag[i] = TRUE;// 드래그를 할때..사용해야징..

					m_Old_Pos = pMouseInfo->m_posMouse;
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

				if(m_Move_Flag[i])// = TRUE;// 드래그를 할때..사용해야징..
				{
					// 이곳에서 아이템을 어떤 장소에다가 놓았는지 판단해야된다..
					if(pMouseInfo->IsLButtonUp())
						m_Move_Flag[i] = FALSE;
				}

				m_Item_Flag = i;
				return TRUE;
				
			}
			else if(m_Item_Flag == i)
				m_Item_Flag = -1;
		}

	}
	return FALSE;
}

BOOL CItem::InMousePos(CPoint pos, CRect rect)
{
	if(pos.x > rect.left && pos.x < rect.right && pos.y > rect.top && pos.y < rect.bottom)
		return 1;
	else
		return 0;
}

BOOL CItem::ViewItemBox(int x, int y, int number, int spr_number, CDDSurface *pDestSurf, int ani_value)
{
	//x, y 위치.. spr_number : 뿌릴 종류.. number 뿌리는 종류..
	CRect rect;
	CRect srect;
	int ani_flag = 0;//에니플래그..

	srect = m_SprData[spr_number].rcV;

	if(ani_value == 1)
		ani_flag = 1;		
	else if(ani_value > 1)
		srect.right -= ani_value;// 에너지 바에 사용...

	if(srect.right < 0) return FALSE;// 임의 값으로 초기화 시켜야 됨..0..

	rect.top = y;
	rect.left = x;
	rect.bottom = rect.top + srect.bottom - srect.top;
	rect.right = rect.left + srect.right - srect.left;

	m_BoxRect[number] = rect;
	pDestSurf->Blt(&rect, m_lpSurface,&srect, DDBLT_KEYSRC);
	return TRUE;
}

BOOL CItem::Input(int index, int type, int x, int y)
{
//	int i = m_Count;
	int i = index;
	
	m_Count ++;
	if(m_Count > 50)
		m_Count = 0;
//	srand( (unsigned)time( NULL ) );

//	int tmp1 = rand()%6;
//	int tmp2 = rand()%6;

	m_Item_pos[i].x = x; //100*tmp1;
	m_Item_pos[i].y = y; //100*tmp2;

	if(m_Count > 50) return FALSE;

	m_Item_Pic[i] = type;

	//if(m_Item_Type[i] < 0) return; 
	return TRUE;
}

int CItem::Output(int type)
{
	int i = type;

	m_BoxRect[i].top    = 0;
	m_BoxRect[i].bottom = 0;
	m_BoxRect[i].left   = 0;
	m_BoxRect[i].right  = 0;

	m_Item_pos[i].x = -1;
	m_Item_pos[i].y = -1;
	
	m_Item_Pic[i] = -1;

	m_Move_Flag[i] = FALSE;

	return -1;
}
 

void CItem::SetInv_Data()
{
	
}

void CItem::LoadObm(LPCTSTR szFileName)
{
	CString szTmpFileName;

	const CString szExePath = CDirectDraw::GetExecutionPath();
	szTmpFileName.Format("%sitem\\%s", (LPCTSTR)szExePath ,szFileName);

	m_lpSurf = CChildView::g_pTEng.SurfaceCreateWith16BMP(szTmpFileName, FALSE, RGB(255, 0, 255), TRUE);

	m_lpSurface = new CDDSurface(m_lpSurf);
}