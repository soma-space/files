// Item_Field.cpp: implementation of the CItem_Field class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Item_Field.h"
#include "ChildView.h"

#define RELEASE(exp) if(exp!=NULL) { exp->Release(); exp=NULL; }
#define CLEAR(exp) memset(&exp, 0, sizeof(exp))

#define POINT2KEY(pos)		(pos.x << 16 | pos.y)
#define KEY2POINT(value)	CPoint(value>>16, value&0xffff)

extern SIZE g_vDispPixSize, g_vDispCellSize;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ITEM_FIELD::ITEM_FIELD()
{
	m_pos.x = m_pos.y = -1;
}

ITEM_FIELD::~ITEM_FIELD()
{
	int count = m_arrPiece.GetSize();

	for(int i = 0; i < count; i++)
	{
		delete m_arrPiece[i];
	}
	m_arrPiece.RemoveAll();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CItem_Field::CItem_Field()
{
	m_lpSurf = NULL;
	m_lpSurface = NULL;
	m_lpISurface = NULL;
	m_pDirectDraw = NULL;
	m_SprData = NULL;
}

CItem_Field::~CItem_Field()
{
	SAFE_RELEASE(m_lpSurf);
	SAFE_DELETE(m_lpSurface);
	SAFE_DELETE(m_lpISurface);
	SAFE_DELETES(m_SprData);

	RemoveAll();
}

void CItem_Field::RemoveAll()
{
	POSITION pos = m_MapItemField.GetStartPosition();
	int key;
	ITEM_FIELD *value;

	while(pos)
	{
		m_MapItemField.GetNextAssoc(pos, key, value);
		delete value;
	}
	m_MapItemField.RemoveAll();
}

void CItem_Field::SetPixelFormat(int nPixelFormat)
{
	if( nPixelFormat == 565 )
	{
		m_colorkey = 0xf81f;
		m_linekey = 0x07e0;
	}
	else
	{
		m_colorkey = 0x7c1f;
		m_linekey = 0x07e0;
	}

}

void CItem_Field::InitData(CDirectDraw *pDirectDraw)// 다이렉트 드로우를 가지고 온다..반드시 초기화 시켜줘야함..
{
	m_pDirectDraw = pDirectDraw;

	m_lpISurface = new CDDSurface;
	m_lpISurface->Create(m_pDirectDraw, 48, 48, FALSE);
	m_lpISurface->SetColorKey(RGB(255, 0, 255));
	
	LoadObm("item.obm");
//	Load_image("item.bmp");
	LoadSpr("item.spr");
}

void CItem_Field::Load_image(LPCSTR	szFileName)
{
	CString szTmpFileName;// = szFileName;
	BOOL test;

	const CString szExePath = CDirectDraw::GetExecutionPath();
	szTmpFileName.Format("%sItem\\%s", (LPCTSTR)szExePath ,szFileName);

	m_lpSurface = new CDDSurface;
	test = m_lpSurface->CreateFromImage(m_pDirectDraw, szTmpFileName, 0);

	m_lpSurface->SetColorKey(RGB(255, 0, 255));
}

void CItem_Field::Draw(CDDSurface* pDestSurf,CPoint ViewPos, int in_house)
{	
	int real_x, real_y;
	CPoint start_pos;
	CPoint i_pos;
	ITEM_FIELD *item_field;
	int count;
	int i;

	start_pos = CChildView::MapManager.PixelToCell(ViewPos);

//	for(i_pos.y = start_pos.y; i_pos.y < start_pos.y+21; i_pos.y++)
//	{
//		for(i_pos.x = start_pos.x; i_pos.x < start_pos.x+17; i_pos.x++)
	for(i_pos.y = start_pos.y-1; i_pos.y < start_pos.y+25; i_pos.y++)
	{
		for(i_pos.x = start_pos.x-1; i_pos.x < start_pos.x+18; i_pos.x++)
		{
			if(((i_pos.x+i_pos.y)&0x1) == 0 && m_MapItemField.Lookup(POINT2KEY(i_pos), item_field) == TRUE)
			{
				if(item_field->m_vInHouse == in_house)
				{
					real_x = i_pos.x * 48 + 48 - ViewPos.x;
					real_y = i_pos.y * 24 + 24 - ViewPos.y;
					count = item_field->m_arrPiece.GetSize();
					for(i = 0; i < count; i++)
					{
						ViewItem(real_x, real_y, 0, item_field->m_arrPiece[i]->m_vPicNum, pDestSurf);
					}
				}
			}
		}
	}
}

void CItem_Field::LoadSpr(LPCSTR	szFileName)
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

BOOL CItem_Field::MouseProcess(CPoint mouse_pos)
{
	CPoint pos;
	ITEM_FIELD *item_field;
	CPoint MyCellPos;

	MyCellPos = CChildView::UserManager.GetMyCellPos();
	pos = CChildView::PixelToCellUnit(mouse_pos);
	if(abs(MyCellPos.x - pos.x)+abs(MyCellPos.y - pos.y) <= 2)
	{
		if(m_MapItemField.Lookup(POINT2KEY(pos), item_field) == TRUE)
		{
			BYTE buf[20];
			int offset = 0;
			CPacketParser::SetByte(buf, offset, PKT_ITEM_PICKUP);
			CPacketParser::SetShort(buf, offset, (short)pos.x);
			CPacketParser::SetShort(buf, offset, (short)pos.y);
			CChildView::SocketMng.Send(offset, buf);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CItem_Field::InMousePos(CPoint pos, CRect rect)
{
	if(pos.x > rect.left && pos.x < rect.right && pos.y > rect.top && pos.y < rect.bottom)
		return 1;
	else
		return 0;
}

BOOL CItem_Field::ViewItem(int x, int y, int number, int spr_number, CDDSurface *pDestSurf, int ani_value)
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

	rect.top = y-16;
	rect.left = x-16;
	rect.bottom = rect.top + 32;
	rect.right = rect.left + 32;

	if(rect.right < 0 || rect.bottom < 0 || rect.left >= g_vDispPixSize.cx || rect.top >= g_vDispPixSize.cy) return FALSE; // 화면을 완전히 벗어남..

	CRect rcClip;
	GetClippedRect(&rect,&rcClip);
	if( rcClip != rect )
	{
		if( rect.left < 0 )
		{
			srect.left += int( float( - rect.left )*(srect.right - srect.left)/( rect.right - rect.left ) );
		}
		if( rect.right >= g_vDispPixSize.cx )
		{
			srect.right -= int( float( rect.right - g_vDispPixSize.cx )*(srect.right - srect.left)/( rect.right - rect.left ) );
		}
		if( rect.top < 0 )
		{
			srect.top += int( float( - rect.top )*(srect.bottom - srect.top)/( rect.bottom - rect.top) );
		}
		if( rect.bottom >= g_vDispPixSize.cy )
		{
			srect.bottom -= int( float( rect.bottom - g_vDispPixSize.cy )*(srect.bottom - srect.top)/( rect.bottom - rect.top ) );
		}
	}
	pDestSurf->Blt(&rcClip, m_lpSurface,&srect, DDBLT_WAIT|DDBLT_KEYSRC);
//	pDestSurf->Blt(&rect, m_lpSurface,&srect, DDBLT_WAIT|DDBLT_KEYSRC);
	return TRUE;
}

void CItem_Field::GetClippedRect(RECT *pRC, RECT* pRCClip)
{
	int ScreenX = g_vDispPixSize.cx;
	int ScreenY = g_vDispPixSize.cy;

	*pRCClip = *pRC;

	if(pRC->left < 0)
	{ 
		pRCClip->left = 0;
	}
	else if(pRC->right > ScreenX)
	{ 
		pRCClip->right = ScreenX;
	}
	if(pRC->top < 0)
	{
		pRCClip->top = 0;
	}
	else if(pRC->bottom > ScreenY)
	{
		pRCClip->bottom = ScreenY;
	}
}

void CItem_Field::DrawFocusItem(int x, int y, int number, int spr_number, CDDSurface *pDestSurf, int ani_value)
{
	CRect rect;
	CRect srect;
	int ani_flag = 0;//에니플래그..

	srect = m_SprData[spr_number].rcV;

	if(ani_value == 1)
		ani_flag = 1;		
	else if(ani_value > 1)
		srect.right -= ani_value;// 에너지 바에 사용...

	if(srect.right < 0) return ;// 임의 값으로 초기화 시켜야 됨..0..

	rect.top	= 0;
	rect.left	= 0;
	rect.bottom = 32;
	rect.right	= 32;

	m_lpISurface->Blt(&rect, m_lpSurface,&srect, DDBLT_WAIT);

	rect.OffsetRect(x,y);
	if(rect.right < 0 || rect.bottom < 0 || rect.left >= g_vDispPixSize.cx || rect.top >= g_vDispPixSize.cy) return; // 화면을 완전히 벗어남..

	CRect rcClip = rect;
	GetClippedRect(&rect, &rcClip);
//	int width = 32;
//	int height = 32;
	int width = rcClip.right - rcClip.left;
	int height = rcClip.bottom - rcClip.top;

	if( width == 0 || height == 0 ) return;
	
	int xlgap = - rect.left;
	int ytgap = - rect.top;
	if( ytgap <= 0 ) ytgap = 0;
	if( x < 0 ) x = 0; if( y < 0 ) y = 0;

	WORD* pSrcOrg;
	m_lpISurface->Lock();
	DDSURFACEDESC2& ddsdSrc = m_lpISurface->GetDescription();
	if( xlgap > 0 )
	{
		pSrcOrg = (PWORD)((PBYTE)ddsdSrc.lpSurface+xlgap*2+ytgap*ddsdSrc.lPitch);
	}
	else
		pSrcOrg = (PWORD)((PBYTE)ddsdSrc.lpSurface+ytgap*ddsdSrc.lPitch);
	int pitchSrc = ddsdSrc.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.

	pDestSurf->Lock();
	DDSURFACEDESC2& ddsdDest = pDestSurf->GetDescription();
	WORD* pDestOrg = (PWORD)((PBYTE)ddsdDest.lpSurface+x*2+y*ddsdDest.lPitch);
	int pitchDest = ddsdDest.lPitch/2; // 워드 픽셀단위이기땜에 항상 /2 를 해준다.

	WORD* pDest = pDestOrg;
	WORD* pSrc = pSrcOrg;
	
	int nNoCopyCount = 0;
	BOOL bConverted = FALSE;
	for( int j = 0; j < height; j++ )
	{
		WORD* pDest = pDestOrg+pitchDest*j;
		WORD* pSrc = pSrcOrg+pitchSrc*j;
		nNoCopyCount = PutPixelLineAndItem( pDest, pSrc, width, m_colorkey, m_linekey );
		if( nNoCopyCount != width )
		{
			if( bConverted == FALSE )
			{
				bConverted = TRUE;
				pDest = pDestOrg+pitchDest*(j-1);
				PutPixelTopBottomLine( pDest, pSrc, width, m_colorkey, m_linekey );
			}
			else
			{
				pDest = pDestOrg+pitchDest*(j+1);
				PutPixelTopBottomLine( pDest, pSrc, width, m_colorkey, m_linekey );
			}
		}
	}

	pDestSurf->Unlock();
	m_lpISurface->Unlock();

}

void CItem_Field::Add_F_item(CPoint pos, int picnum, CString t_name)
{
	ITEM_FIELD *item_field;
	ITEM_PIECE *item_piece;

	item_piece = new ITEM_PIECE;
	item_piece->m_vName = t_name;
	item_piece->m_vPicNum = picnum;

	if(m_MapItemField.Lookup(POINT2KEY(pos), item_field) == TRUE)
	{
		item_field->m_arrPiece.Add(item_piece);
	}
	else
	{
		item_field = new ITEM_FIELD;
		item_field->m_vInHouse = CChildView::MapManager.IsHouse(pos);
		item_field->m_arrPiece.Add(item_piece);
		m_MapItemField[POINT2KEY(pos)] = item_field;
	}
}

void CItem_Field::Del_F_item(CPoint pos, int picnum)
{
	ITEM_FIELD *item_field;

	if(m_MapItemField.Lookup(POINT2KEY(pos), item_field) == TRUE)
	{
		int count;

		count = item_field->m_arrPiece.GetSize();
		for(int i = count-1; i >= 0; i--)
		{
			if(item_field->m_arrPiece[i]->m_vPicNum == picnum)
			{
				delete item_field->m_arrPiece[i];
				item_field->m_arrPiece.RemoveAt(i);
				break;
			}
		}
		if(item_field->m_arrPiece.GetSize() == 0)
		{
			m_MapItemField.RemoveKey(POINT2KEY(pos));
			delete item_field;
		}
	}
	return;
}

BOOL CItem_Field::IsExistItem(CPoint cell_pos, int in_house, CPoint ViewPos,CDDSurface *pDestSurf )
{
	ITEM_FIELD *item_field;
	int real_x, real_y,count;

	if(((cell_pos.x+cell_pos.y)&0x1) == 0 && m_MapItemField.Lookup(POINT2KEY(cell_pos), item_field) == TRUE)
	{
		if(item_field->m_vInHouse == in_house)
		{
			real_x = cell_pos.x * 48 + 48 - ViewPos.x - 16;
			real_y = cell_pos.y * 24 + 24 - ViewPos.y - 16;
			count = item_field->m_arrPiece.GetSize();
			if( count >= 1 )
				DrawFocusItem(real_x, real_y, 0, item_field->m_arrPiece[count-1]->m_vPicNum, pDestSurf,0);
		}
	}

	if(m_MapItemField.Lookup(POINT2KEY(cell_pos), item_field) == TRUE && item_field->m_vInHouse == in_house) return TRUE;
	else return FALSE;


}

void CItem_Field::DrawLabel(CDDSurface *pDest, CPoint point, int in_house)
{
	ITEM_FIELD *item_field;
	if(m_MapItemField.Lookup(POINT2KEY(point), item_field) == TRUE)
	{
		if(item_field->m_vInHouse == in_house)
		{
			CDC *pDC = pDest->GetDC();
			CFont *old_font = (CFont *)pDC->SelectObject(CChildView::pAppFont);
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255,255,255));

			int count = item_field->m_arrPiece.GetSize();
			if(count > 0)
			{
				CRect t_rect;
				CPoint mouse_pos = CChildView::DirectXMng.GetMouseInfoPtr()->m_posMouse;
				CString t_name;

				t_rect.left = mouse_pos.x;
				t_rect.top = mouse_pos.y - 20;
				t_rect.right = mouse_pos.x + 20;
				t_rect.bottom = t_rect.top + 14;
				t_name = item_field->m_arrPiece[count-1]->m_vName;
				pDC->DrawText(t_name, &t_rect, DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_CALCRECT);
				pDC->DrawText(t_name, &t_rect, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
			}

			pDC->SelectObject(old_font);
			pDest->ReleaseDC(pDC);
		}
	}
}

void CItem_Field::LoadObm(LPCTSTR szFileName)
{
	CString szTmpFileName;

	const CString szExePath = CDirectDraw::GetExecutionPath();
	szTmpFileName.Format("%sitem\\%s", (LPCTSTR)szExePath ,szFileName);

	m_lpSurf = CChildView::g_pTEng.SurfaceCreateWith16BMP(szTmpFileName, FALSE, RGB(255, 0, 255), TRUE);

	m_lpSurface = new CDDSurface(m_lpSurf);
}