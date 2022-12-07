// Direct3D.cpp: implementation of the CDirect3D class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D3Dutil.h"
#include "Direct3D.h"
#include "d3dtextr.h"
#include <time.h>

#define	SAFE_RELESE(x) { if(x) x->Release(); x = NULL;}
char* D3DErrorToString(HRESULT hErr);

static HRESULT WINAPI EnumZBufferCallback(DDPIXELFORMAT* DDP_Format ,VOID* DDP_Desired)
{
	if(DDP_Format->dwFlags == DDPF_ZBUFFER)
	{
		memcpy( DDP_Desired, DDP_Format,sizeof(DDPIXELFORMAT) );
		return D3DENUMRET_CANCEL;
	}
	return D3DENUMRET_OK;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDirect3D::CDirect3D()
{
	m_d3dLoginVertices = NULL;
	m_lpD3D7 = NULL;
	m_lpD3DD7 = NULL;
	m_lpDDS_ZBuffer = NULL;

	m_d3dSpriteVertices[0] = D3DTLVERTEX(D3DVECTOR(100, 100, 0), 1, 0xffffffff, 0, 0, 0);
	m_d3dSpriteVertices[1] = D3DTLVERTEX(D3DVECTOR(700, 100, 0), 1, 0xffffffff, 0, 0, 0);
	m_d3dSpriteVertices[2] = D3DTLVERTEX(D3DVECTOR(100, 450, 0), 1, 0xffffffff, 0, 0, 1);
	m_d3dSpriteVertices[3] = D3DTLVERTEX(D3DVECTOR(700, 450, 0), 1, 0xffffffff, 0, 0, 1);

	m_d3dDayVertices[0]	   = D3DTLVERTEX(D3DVECTOR(  0,   0, 0), 1, 0xffffffff, 0, 0, 0);
	m_d3dDayVertices[1]    = D3DTLVERTEX(D3DVECTOR(800,   0, 0), 1, 0xffffffff, 0, 1, 0);
	m_d3dDayVertices[2]    = D3DTLVERTEX(D3DVECTOR(  0, 495, 0), 1, 0xffffffff, 0, 0, 1);
	m_d3dDayVertices[3]    = D3DTLVERTEX(D3DVECTOR(800, 495, 0), 1, 0xffffffff, 0, 1, 1);

	m_d3dLoginVertices = NULL;//3D 9월 24일..
	m_Char1 = D3DVECTOR(280, 120, 0);
	m_Char2 = D3DVECTOR(480, 40, 0);
	m_Char3 = D3DVECTOR(80, 40, 0);
	m_Color1 = RGB(240,240,240);
	m_Color2 = RGB(70,70,70);
	m_Color3 = RGB(70,70,70);

}

CDirect3D::~CDirect3D()
{
	DestroyAllTexture(); //우석.
	DestoryLoginVertices(); //우석.
	DestoryMakeCharVertices();
	CleanUp3DEnvironment();
}

void CDirect3D::SetVAGType( int nVGAType )
{
	m_VGAType		= nVGAType;
	m_Alphaarg1		= D3DTA_DIFFUSE;
	switch( m_VGAType )
	{
	case 0:
		m_SorBlend = 9;
		m_DesBlend = 1;
	break;
	case 1:
		m_SorBlend = 9;
		m_DesBlend = 1;
	break;
	case 2:
		m_Alphaarg1 = D3DTA_TEXTURE;
		m_SorBlend = 9;
		m_DesBlend = 1;
	break;
	case 3:
		m_SorBlend = 8;
		m_DesBlend = 4;
	break;
	default:
		m_SorBlend = 9;
		m_DesBlend = 1;
	break;
	}

}
BOOL CDirect3D::Initialize3D(CDirectDraw *pDirectDraw,HWND hWnd, GUID* pDriverGUID, const GUID* pDeviceGUID)
{
//	DDPIXELFORMAT DDPF_ZBuffer;
	DDSCAPS2 ddscaps = { DDSCAPS_BACKBUFFER, 0, 0, 0 };
//	DDSURFACEDESC2 ddsd;
	//HRESULT hRet;
	
	LPDIRECTDRAW7			lpDD7;
	lpDD7 = pDirectDraw->GetInterface(); 

	if(lpDD7->QueryInterface (IID_IDirect3D7, (void **)&m_lpD3D7)!=DD_OK)
	{
		TRACE("Couldn't query D3D interface\n");
		return FALSE;
	}
	m_lpDDS_BackBuffer = pDirectDraw->GetBackBuffer()->GetInterface();

	if(m_lpD3D7->CreateDevice(*pDeviceGUID, m_lpDDS_BackBuffer, &m_lpD3DD7)!=D3D_OK)
	{
		TRACE("Couldn't find any D3D Device\n");
		return FALSE;
	}
/*
	m_lpD3D7->EnumZBufferFormats(*pDeviceGUID, EnumZBufferCallback, (VOID*)&DDPF_ZBuffer);

	if(sizeof(DDPIXELFORMAT)!=DDPF_ZBuffer.dwSize)
	{
		TRACE("couldn't enumerate ZBuffer formats for the device\n");
		return FALSE;
	}

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
	ddsd.dwWidth = 800;
	ddsd.dwHeight = 600;

	if(IsEqualIID(*pDeviceGUID, IID_IDirect3DHALDevice)) ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	else ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	memcpy(&ddsd.ddpfPixelFormat, &DDPF_ZBuffer, sizeof(DDPIXELFORMAT));

	hRet = lpDD7->CreateSurface(&ddsd, &m_lpDDS_ZBuffer, NULL);
	if( FAILED( hRet ) )
	{
		TRACE("Couldn't create zbuffer : %s\n", D3DErrorToString( hRet ) );
		return FALSE;
	}

	if(m_lpDDS_BackBuffer->AddAttachedSurface(m_lpDDS_ZBuffer)!=DD_OK)
	{
		TRACE("Couldn't attach zbuffer to BackBuffer\n");
		return FALSE;
	}

	if(m_lpD3DD7->SetRenderTarget(m_lpDDS_BackBuffer, 0)!=D3D_OK)
	{
		TRACE("Couldn't set render target\n");
		return FALSE;
	}
*/
	D3DVIEWPORT7 vp = { 0, 0, 800, 600, 0.0f, 1.0f };

	if(m_lpD3DD7->SetViewport(&vp)!=D3D_OK)
	{
		TRACE("Couldn't set viewport\n");
		return FALSE;
	}
	SetMaterial();
	SetLight();
	D3DMATRIX mat;
	mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
	mat._12 = mat._13 = mat._14 = mat._41 = 0.0f;
	mat._21 = mat._23 = mat._24 = mat._42 = 0.0f;
	mat._31 = mat._32 = mat._34 = mat._43 = 0.0f;
			
	D3DMATRIX matWorld = mat;
	m_lpD3DD7->SetTransform( D3DTRANSFORMSTATE_WORLD, &matWorld );

	D3DMATRIX matView = mat;
	matView._43 = 20.0f;
	m_lpD3DD7->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );

	D3DMATRIX matProj = mat;
	matProj._11 =  2.0f;
	matProj._22 =  2.0f;
	matProj._34 =  1.0f;
	matProj._43 = -1.0f;
	matProj._44 =  0.0f;
	m_lpD3DD7->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

	D3DCreateTexture(".\\MagicRes\\chat.bmp");
//	D3DCreateTexture(".\\MagicRes\\light2.bmp");

	return TRUE;
}

void CDirect3D::D3DAlpha( BOOL bAlpha )
{
//////////////////////////////////////////////////////////////////////////////////////////
//이부분의 설정은 3D의 알파 블랜딩 가속기능만을 쓰기 위한 세팅임....
//다른 기능을 쓰기 원한다면 추후에 그 기능을 쓸때마다 다시 세팅을 해줘야함.....

	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_AMBIENT, 0xffffffff );
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ZENABLE, TRUE );//zbuffer를 만들어 놨지만 당장 쓸일이 없으므로 기능을 사용하지 않음
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ZENABLE, FALSE );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
    m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );

//	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
//	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    m_lpD3DD7->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_NONE);
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_EDGEANTIALIAS, FALSE);
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
	if( bAlpha )
		m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
	else
		m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);

//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHAREF, 0x08 );
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL );
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE, TRUE );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_COLORKEYBLENDENABLE, TRUE );

}

void CDirect3D::SetMaterial()
{
	HRESULT hRet;
	D3DMATERIAL7 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    hRet = m_lpD3DD7->SetMaterial( &mtrl );
	if( FAILED(hRet) )
		TRACE(D3DErrorToString(hRet));
}
void CDirect3D::SetLight()
{
	HRESULT hRet;
    D3DLIGHT7 light;
    D3DUtil_InitLight( light, D3DLIGHT_POINT, 0.0f, 0.0f, -10.0f );
	hRet = m_lpD3DD7->SetLight( 0, &light );
	if( FAILED(hRet) )
		TRACE("Couldn't set light : %s\n", D3DErrorToString( hRet ) );
    hRet = m_lpD3DD7->LightEnable( 0, TRUE );
	if( FAILED(hRet) )
		TRACE(D3DErrorToString(hRet));
    hRet = m_lpD3DD7->SetRenderState( D3DRENDERSTATE_LIGHTING, TRUE );
	if( FAILED(hRet) )
		TRACE(D3DErrorToString(hRet));
}
void CDirect3D::D3DCreateTextureSpl(TCHAR* strName)
{
	if( S_OK != D3DTextr_CreateTextureFromFileSpl(strName, 0, D3DTEXTR_16BITSPERPIXEL) )
		D3DTextr_RestoreSpl(strName, m_lpD3DD7 );
}
void CDirect3D::D3DCreateTexture(TCHAR* strName)
{
	D3DTextr_CreateTextureFromFile(strName, 0, D3DTEXTR_16BITSPERPIXEL);
	D3DTextr_Restore(strName, m_lpD3DD7 );
}
void CDirect3D::D3DCreateTextureEx(TCHAR* strName)
{
	LoadMagicRes(strName, 0, D3DTEXTR_16BITSPERPIXEL,m_lpD3DD7);
}

// 우석...
//-----------------------------------------------------------------------------
// Name: D3DTextr_CreateEmptyTexture()
// Desc: Creates an empty texture.
//-----------------------------------------------------------------------------
HRESULT CDirect3D::D3DCreateEmptyTexture( TCHAR* strName, DWORD dwWidth,
                                     DWORD dwHeight, DWORD dwStage, 
                                     DWORD dwFlags )
{
	HRESULT hr =  D3DTextr_CreateEmptyTexture(strName, dwWidth, dwHeight, dwStage,dwFlags);
	if(hr == D3D_OK)
		hr = D3DTextr_Restore(strName, m_lpD3DD7);
	return hr;
}

//-----------------------------------------------------------------------------
// Name: D3DTextr_GetSurface()
// Desc: Returns a pointer to a d3dSurface from the name of the texture
//-----------------------------------------------------------------------------
LPDIRECTDRAWSURFACE7 CDirect3D::D3DGetSurface( TCHAR* strName )
{
	return D3DTextr_GetSurface(strName);
}

void CDirect3D::CreateLoginVertices(int num)
{
	m_d3dLoginVertices = new D3DTLVERTEX[num];

	m_Char1 = D3DVECTOR(280, 120, 0);
	m_Char2 = D3DVECTOR(480, 40, 0);
	m_Char3 = D3DVECTOR(80, 40, 0);
	m_Color1 = RGB(240,240,240);
	m_Color2 = RGB(70,70,70);
	m_Color3 = RGB(70,70,70);

	ChangePosColor();
}

void CDirect3D::CreateMakeCharVertices(int num)
{
	m_d3dLoginVertices = new D3DTLVERTEX[num];

	m_Char1 = D3DVECTOR(100, 100, 0);
	m_Char2 = D3DVECTOR(110, 110, 0);
	m_Color1 = RGB(200,200,200);
	m_Color2 = RGB(0,0,0);

	m_SkinColor1 = RGB(200, 200, 200);
	m_SkinColor2 = RGB(0, 0, 0);
	m_SkinColor3 = RGB(0, 0, 0);

	m_d3dLoginVertices[0] = D3DTLVERTEX(m_Char1, 1, m_Color1, 0, 0, 0);
	m_d3dLoginVertices[1] = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX + 350, m_Char1.dvY,0), 1, m_Color1, 0, 1, 0);
	m_d3dLoginVertices[2] = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX, m_Char1.dvY + 350,0), 1, m_Color1, 0, 0, 1);
	m_d3dLoginVertices[3] = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX + 400, m_Char1.dvY + 400,0), 1, m_Color1, 0, 1, 1);

	m_d3dLoginVertices[4] = D3DTLVERTEX(m_Char2, 1, m_Color2, 0, 0, 0);
	m_d3dLoginVertices[5] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX + 350, m_Char2.dvY,0), 1, m_Color2, 0, 1, 0);
	m_d3dLoginVertices[6] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX, m_Char2.dvY + 350,0), 1, m_Color2, 0, 0, 1);
	m_d3dLoginVertices[7] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX + 350, m_Char2.dvY + 350,0), 1, m_Color2, 0, 1, 1);

	//남자.. 피부..
	m_d3dLoginVertices[8]  = D3DTLVERTEX(m_Char1, 1, m_SkinColor1, 0, 0, 0);
	m_d3dLoginVertices[9]  = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX + 350, m_Char1.dvY,0), 1, m_SkinColor1, 0, 1, 0);
	m_d3dLoginVertices[10] = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX, m_Char1.dvY + 350,0), 1, m_SkinColor1, 0, 0, 1);
	m_d3dLoginVertices[11] = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX + 400, m_Char1.dvY + 400,0), 1, m_SkinColor1, 0, 1, 1);

	m_d3dLoginVertices[12]  = D3DTLVERTEX(m_Char1, 1, m_SkinColor2, 0, 0, 0);
	m_d3dLoginVertices[13]  = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX + 350, m_Char1.dvY,0), 1, m_SkinColor2, 0, 1, 0);
	m_d3dLoginVertices[14] = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX, m_Char1.dvY + 350,0), 1, m_SkinColor2, 0, 0, 1);
	m_d3dLoginVertices[15] = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX + 400, m_Char1.dvY + 400,0), 1, m_SkinColor2, 0, 1, 1);

	//여자.. 피부..
	m_d3dLoginVertices[16] = D3DTLVERTEX(m_Char2, 1, m_SkinColor1, 0, 0, 0);
	m_d3dLoginVertices[17] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX + 350, m_Char2.dvY,0), 1, m_SkinColor1, 0, 1, 0);
	m_d3dLoginVertices[18] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX, m_Char2.dvY + 350,0), 1, m_SkinColor1, 0, 0, 1);
	m_d3dLoginVertices[19] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX + 350, m_Char2.dvY + 350,0), 1, m_SkinColor1, 0, 1, 1);

	m_d3dLoginVertices[20] = D3DTLVERTEX(m_Char2, 1, m_SkinColor2, 0, 0, 0);
	m_d3dLoginVertices[21] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX + 350, m_Char2.dvY,0), 1, m_SkinColor2, 0, 1, 0);
	m_d3dLoginVertices[22] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX, m_Char2.dvY + 350,0), 1, m_SkinColor2, 0, 0, 1);
	m_d3dLoginVertices[23] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX + 350, m_Char2.dvY + 350,0), 1, m_SkinColor2, 0, 1, 1);
}

void CDirect3D::DestoryMakeCharVertices()
{
	if(m_d3dLoginVertices) { delete [] m_d3dLoginVertices;  m_d3dLoginVertices = NULL; }
}


void CDirect3D::ResetSkinColor(int index)
{
	switch(index)
	{
		case 1:
				m_SkinColor1 = RGB(200, 200, 200);
				m_SkinColor2 = RGB(0, 0, 0);
				m_SkinColor3 = RGB(0, 0, 0);
			break;
		case 2:
				m_SkinColor1 = RGB(0, 0, 0);
				m_SkinColor2 = RGB(200, 200, 200);
				m_SkinColor3 = RGB(0, 0, 0);
			break;
		case 3:
				m_SkinColor1 = RGB(0, 0, 0);
				m_SkinColor2 = RGB(0, 0, 0);
				m_SkinColor3 = RGB(200, 200, 200);
			break;
	}
}

bool CDirect3D::SkinColorChange(bool IsMan, int cur, int next, int skip_color)
{
	bool  IsOK1 = false, IsOK2 = false;

	D3DCOLOR *curColor = NULL;
	D3DCOLOR *nextColor = NULL;

	skip_color = 4;
	if(cur == next) return true;

	switch(cur)
	{
		case 0: curColor = &m_SkinColor1; break;
		case 1: curColor = &m_SkinColor2; break;
		case 2: curColor = &m_SkinColor3; break;
	}

	switch(next)
	{
		case 0: nextColor = &m_SkinColor1; break;
		case 1: nextColor = &m_SkinColor2; break;
		case 2: nextColor = &m_SkinColor3; break;
	}

	if( *curColor < RGB(5,5,5) )
	{
		*curColor = RGB(0,0,0);
		IsOK1 = true;
	}
	else
	{
		*curColor -= RGB(skip_color,skip_color,skip_color);
		IsOK1 = false;
	}
	if( *nextColor > RGB(255,255,255) )
	{
		*nextColor = RGB(255,255,255);
		IsOK2 = true;
	}
	else
	{
		*nextColor += RGB(skip_color,skip_color,skip_color);
		IsOK2 = false;
	}

	if(IsMan)
	{
		m_d3dLoginVertices[8].color = *curColor;
		m_d3dLoginVertices[9].color = *curColor;
		m_d3dLoginVertices[10].color = *curColor;
		m_d3dLoginVertices[11].color = *curColor;

		m_d3dLoginVertices[12].color = *nextColor;
		m_d3dLoginVertices[13].color = *nextColor;
		m_d3dLoginVertices[14].color = *nextColor;
		m_d3dLoginVertices[15].color = *nextColor;
	}
	else
	{
		m_d3dLoginVertices[16].color = *curColor;
		m_d3dLoginVertices[17].color = *curColor;
		m_d3dLoginVertices[18].color = *curColor;
		m_d3dLoginVertices[19].color = *curColor;

		m_d3dLoginVertices[20].color = *nextColor;
		m_d3dLoginVertices[21].color = *nextColor;
		m_d3dLoginVertices[22].color = *nextColor;
		m_d3dLoginVertices[23].color = *nextColor;
	}

	if( IsOK1 && IsOK2) return true;
	else return false;
}

bool CDirect3D::ChangeVerticesColor(int Sex, int skip_color)
{
	bool  IsOK1 = false, IsOK2 = false;

	skip_color = 4;

	switch(Sex)
	{
		case 0: 
			    if(m_Color1 > RGB(250,250,250) )
				{
					m_Color1 = RGB(250,250,250);
					IsOK1 = true;
				}
				else 
				{
					m_Color1 += RGB(skip_color,skip_color,skip_color);
					IsOK1 = false;
				}
			    if(m_Color2 < RGB(5,5,5) )
				{
					m_Color2 = RGB(0,0,0);
					IsOK2 = true;
				}
				else
				{
					m_Color2 -= RGB(skip_color,skip_color,skip_color);
					IsOK2 = false;
				}
			   break;
		case 1: 
			    if(m_Color1 < RGB(5,5,5) )
				{
					m_Color1 = RGB(0,0,0);
					IsOK1 = true;
				}
				else
				{
					m_Color1 -= RGB(skip_color,skip_color,skip_color);
					IsOK1 = false;
				}
			    if(m_Color2 > RGB(250,250,250) )
				{
					m_Color2 = RGB(250,250,250);
					IsOK2 = true;
				}
				else
				{
					m_Color2 += RGB(skip_color,skip_color,skip_color);
					IsOK2 = false;
				}
			   break;
	}
	m_d3dLoginVertices[0].color = m_Color1;
	m_d3dLoginVertices[1].color = m_Color1;
	m_d3dLoginVertices[2].color = m_Color1;
	m_d3dLoginVertices[3].color = m_Color1;

	m_d3dLoginVertices[4].color = m_Color2;
	m_d3dLoginVertices[5].color = m_Color2;
	m_d3dLoginVertices[6].color = m_Color2;
	m_d3dLoginVertices[7].color = m_Color2;

	if( IsOK1 && IsOK2) return true;
	else return false;
}

void CDirect3D::ChangePosColor()
{
	m_d3dLoginVertices[0] = D3DTLVERTEX(m_Char1, 1, m_Color1, 0, 0, 0);
	m_d3dLoginVertices[1] = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX + 300, m_Char1.dvY,0), 1, m_Color1, 0, 1, 0);
	m_d3dLoginVertices[2] = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX, m_Char1.dvY + 300,0), 1, m_Color1, 0, 0, 1);
	m_d3dLoginVertices[3] = D3DTLVERTEX(D3DVECTOR(m_Char1.dvX + 300, m_Char1.dvY + 300,0), 1, m_Color1, 0, 1, 1);

	m_d3dLoginVertices[4] = D3DTLVERTEX(m_Char2, 1, m_Color2, 0, 0, 0);
	m_d3dLoginVertices[5] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX + 300, m_Char2.dvY,0), 1, m_Color2, 0, 1, 0);
	m_d3dLoginVertices[6] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX, m_Char2.dvY + 300,0), 1, m_Color2, 0, 0, 1);
	m_d3dLoginVertices[7] = D3DTLVERTEX(D3DVECTOR(m_Char2.dvX + 300, m_Char2.dvY + 300,0), 1, m_Color2, 0, 1, 1);

	m_d3dLoginVertices[8] = D3DTLVERTEX(m_Char3, 1, m_Color3, 0, 0, 0);
	m_d3dLoginVertices[9] = D3DTLVERTEX(D3DVECTOR(m_Char3.dvX + 300, m_Char3.dvY,0), 1, m_Color3, 0, 1, 0);
	m_d3dLoginVertices[10] = D3DTLVERTEX(D3DVECTOR(m_Char3.dvX, m_Char3.dvY + 300,0), 1, m_Color3, 0, 0, 1);
	m_d3dLoginVertices[11] = D3DTLVERTEX(D3DVECTOR(m_Char3.dvX + 300, m_Char3.dvY + 300,0), 1, m_Color3, 0, 1, 1);
}

void CDirect3D::DestoryLoginVertices()
{
	if(m_d3dLoginVertices) { delete [] m_d3dLoginVertices;  m_d3dLoginVertices = NULL; }
}

BOOL CDirect3D::D3DLoginRender(char *strName, bool blend, int start)
{
	HRESULT hRet;
	static DWORD dwFrameTime = 0;
	static DWORD dwTime = 0;


/*
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DITHERENABLE, FALSE );
    m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, FALSE );

	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    m_lpD3DD7->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
	
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_NONE);
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_EDGEANTIALIAS, FALSE);
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHAREF, 0x08 );
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL );
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE, TRUE );
*/

	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_POINT );
    m_lpD3DD7->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_POINT );

	if(blend) m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);//
	else
	{
		//m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
		m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
	}
	
	//m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);//

	LPDIRECTDRAWSURFACE7  surface = D3DTextr_GetSurface( strName );
	DDCOLORKEY colorkey;
		colorkey.dwColorSpaceHighValue = 0x00000000;
		colorkey.dwColorSpaceLowValue  = 0x00000000;

		surface->SetColorKey(DDCKEY_DESTBLT, &colorkey);
	hRet = m_lpD3DD7->SetTexture(0, surface);
	if(hRet != D3D_OK)
	{
		TRACE("Settexture error!!\n");
		return FALSE;
	}

	if(m_lpD3DD7->BeginScene()!=D3D_OK)
	{
		TRACE("Couldn't begin scene\n");
	}

	m_lpD3DD7->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE,TRUE);		

	if( FAILED(hRet) )
	{
		TRACE( "SetTexture Error : %s\n",D3DErrorToString(hRet) );
	}
	else
	{		
		switch(start)
		{
			case 1:hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_d3dLoginVertices, 4, D3DDP_WAIT );
				   break;
			case 2:hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, &m_d3dLoginVertices[4], 4, D3DDP_WAIT );
				   break;
			case 3:hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, &m_d3dLoginVertices[8], 4, D3DDP_WAIT );
				   break;
			case 4:hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, &m_d3dLoginVertices[12], 4, D3DDP_WAIT );
				   break;
			case 5:hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, &m_d3dLoginVertices[16], 4, D3DDP_WAIT );
				   break;
			case 6:hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, &m_d3dLoginVertices[20], 4, D3DDP_WAIT );
				   break;
		}
		if( FAILED(hRet) )
		{
			TRACE("Couldn't draw primitive\n");
		}

	}
	m_lpD3DD7->EndScene();

	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);//

	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
    m_lpD3DD7->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );

	return TRUE;
}

bool CDirect3D::LoginCharData(int dir, int front_num, float time_gap)
{
/*	D3DVECTOR  temp;
	D3DCOLOR  ctemp;

	switch(dir)
	{
		case 1:	temp = m_Char1; m_Char1 = m_Char2; m_Char2 = m_Char3; m_Char3 = temp; 
				ctemp = m_Color1; m_Color1 = m_Color2; m_Color2 = m_Color3; m_Color3 = ctemp;
				break;
		case -1: temp = m_Char1; m_Char1 = m_Char3; m_Char3 = m_Char2; m_Char2 = temp; 
				ctemp = m_Color1; m_Color1 = m_Color3; m_Color3 = m_Color2; m_Color2 = ctemp;
				break;
	}	

	ChangePosColor();
*/
	D3DVECTOR  dstpt1 = D3DVECTOR(280, 120, 0);
	D3DVECTOR  dstpt2 = D3DVECTOR(480, 40, 0);
	D3DVECTOR  dstpt3 = D3DVECTOR(80, 40, 0);
//	D3DVECTOR  curpt1;// = D3DVECTOR(300, 150, 0);
//	D3DVECTOR  curpt2;// = D3DVECTOR(500, 70, 0);
//	D3DVECTOR  curpt3;// = D3DVECTOR(100, 70, 0);
	
	D3DCOLOR   color1 = RGB(240,240,240);
	D3DCOLOR   color2 = RGB(70,70,70);
	D3DCOLOR   color3 = RGB(70,70,70);
	
	bool       IsOK1,IsOK2,IsOK3;

	switch(dir)
	{
		case 1:
				switch(front_num)
				{
					case 0: IsOK1 = SDrawLine(m_Char1.x,m_Char1.y, dstpt1.x,dstpt1.y, m_Color1,color1, 1, time_gap);
							IsOK2 = SDrawLine(m_Char2.x,m_Char2.y, dstpt2.x,dstpt2.y, m_Color2,color2, 1, time_gap);
							IsOK3 = SDrawLine(m_Char3.x,m_Char3.y, dstpt3.x,dstpt3.y, m_Color3,color3, 2, time_gap);								
							break;
					case 1: IsOK1 = SDrawLine(m_Char1.x,m_Char1.y, dstpt3.x,dstpt3.y, m_Color1,color3, 2, time_gap);
							IsOK2 = SDrawLine(m_Char2.x,m_Char2.y, dstpt1.x,dstpt1.y, m_Color2,color1, 1, time_gap);
							IsOK3 = SDrawLine(m_Char3.x,m_Char3.y, dstpt2.x,dstpt2.y, m_Color3,color2, 1, time_gap);
							break;
					case 2: IsOK1 = SDrawLine(m_Char1.x,m_Char1.y, dstpt2.x,dstpt2.y, m_Color1,color2, 1, time_gap);
							IsOK2 = SDrawLine(m_Char2.x,m_Char2.y, dstpt3.x,dstpt3.y, m_Color2,color3, 2, time_gap);
							IsOK3 = SDrawLine(m_Char3.x,m_Char3.y, dstpt1.x,dstpt1.y, m_Color3,color1, 1, time_gap);
							break;
				}
			   break;

		case -1:
				switch(front_num)
				{
					case 0: IsOK1 = SDrawLine(m_Char1.x,m_Char1.y, dstpt1.x,dstpt1.y, m_Color1,color1, 1, time_gap );
							IsOK2 = SDrawLine(m_Char2.x,m_Char2.y, dstpt2.x,dstpt2.y, m_Color2,color2, 2, time_gap );
							IsOK3 = SDrawLine(m_Char3.x,m_Char3.y, dstpt3.x,dstpt3.y, m_Color3,color3, 1, time_gap);								
							break;
					case 1: IsOK1 = SDrawLine(m_Char1.x,m_Char1.y, dstpt3.x,dstpt3.y, m_Color1,color3, 1, time_gap);
							IsOK2 = SDrawLine(m_Char2.x,m_Char2.y, dstpt1.x,dstpt1.y, m_Color2,color1, 1, time_gap);
							IsOK3 = SDrawLine(m_Char3.x,m_Char3.y, dstpt2.x,dstpt2.y, m_Color3,color2, 2, time_gap);
							break;
					case 2: IsOK1 = SDrawLine(m_Char1.x,m_Char1.y, dstpt2.x,dstpt2.y, m_Color1,color2, 2, time_gap);
							IsOK2 = SDrawLine(m_Char2.x,m_Char2.y, dstpt3.x,dstpt3.y, m_Color2,color3, 1, time_gap);
							IsOK3 = SDrawLine(m_Char3.x,m_Char3.y, dstpt1.x,dstpt1.y, m_Color3,color1, 1, time_gap);
							break;
				}
			   break;
		       break;
	}

	ChangePosColor();
	
	if( IsOK1 && IsOK2 && IsOK3)
		return true;
	else return false;
}

//버텍스 직선 이동 선긋기...
bool CDirect3D::SDrawLine(float &x_start, float &y_start, float x_end, float y_end, 
						  DWORD &SrcColor, DWORD DstColor, int gap_step, float time_gap)
{
	float dx,dy;
	float x = 0.0, y = 0.0;
	float gap;

	int   color_gap;

	dx = x_end - x_start;
	dy = y_end - y_start;

	time_gap += 2.5;

	gap = (float)fabs(dx / time_gap);

	if(gap > 2.5)
	{
		gap /= gap_step; // 속도차이....준다..
		
		x = dx / gap;
		y = dy / gap;
		x_start += x;
		y_start += y;

		color_gap = 5;

		if(SrcColor > DstColor) SrcColor -= RGB(color_gap,color_gap,color_gap);
		else if(SrcColor < DstColor) SrcColor += RGB(color_gap,color_gap,color_gap);
		else SrcColor = DstColor;

		return false;
	}
	else
	{
		x_start = x_end;
		y_start = y_end;
	    SrcColor = DstColor; 
		return true;
	}
}




void CDirect3D::CreateSurfBmpToSurf(LPDIRECTDRAWSURFACE7 DstSurface, RECT Dst,
							  HBITMAP hbmBitmap, RECT Src)
{
	SurfaceBitmapToSurface(DstSurface, Dst, hbmBitmap, Src);// 우석..
}

//우석...
void CDirect3D::DestroyAllTexture()
{
	D3DTextr_InvalidateAllTextures();

}
void CDirect3D::DestroyTexture(TCHAR *strName)
{
	D3DTextr_Invalidate( strName );
	D3DTextr_DestroyTexture( strName );

}
void CDirect3D::DestroySeriesTexture(TCHAR *strName, int nFrame)
{
	TCHAR buffer[5];
	TCHAR strAddName[100];

	for( int i = 0 ; i < nFrame ; i++ )
	{
		itoa(i,buffer,10);
		strcpy(strAddName,strName);
		strcat(strAddName,buffer);
		D3DTextr_Invalidate( strAddName );
		D3DTextr_DestroyTexture( strAddName );
	}

}
void CDirect3D::CleanUp3DEnvironment()
{
	D3DTextr_InvalidateAllTextures();
	D3DTextr_DestroyAllTextures();
	if( m_lpDDS_ZBuffer )
	{
		m_lpDDS_BackBuffer->DeleteAttachedSurface( 0, m_lpDDS_ZBuffer );
		SAFE_RELEASE( m_lpDDS_ZBuffer );
	}
	SAFE_RELESE(m_lpD3D7);
	SAFE_RELESE(m_lpD3DD7);
}

BOOL CDirect3D::D3DTexturInfo( TCHAR* strName , int** pFrame, int** pCurFrame, float** pTu, 
							 float** pTv, int** pWidth, int** pHeight, int** pTime, int** pX, int** pY )
{
	HRESULT hRet;
	hRet = D3DTextr_TextureInfo( strName, pFrame, pCurFrame, pTu, pTv, pWidth, pHeight, pTime, pX, pY );
	if( FAILED(hRet) )
	{
		TRACE("Couldn't get textureinfo : %s\n", D3DErrorToString( hRet ) );
		return FALSE;
	}
	return TRUE;

}
void CDirect3D::D3DRenderChat()
{
/*
	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ONE );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVDESTCOLOR );
*/
	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
//1101
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND,  9 );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, 1 );
	HRESULT hRet;
	m_d3dSpriteVertices[0] = D3DTLVERTEX(D3DVECTOR(21 ,457, 0), 1, 0xffffffff, 0, 0, 0);
	m_d3dSpriteVertices[1] = D3DTLVERTEX(D3DVECTOR(779,457, 0), 1, 0xffffffff, 0, 1, 0);
	m_d3dSpriteVertices[2] = D3DTLVERTEX(D3DVECTOR(21 ,478, 0), 1, 0xffffffff, 0, 0, 1);
	m_d3dSpriteVertices[3] = D3DTLVERTEX(D3DVECTOR(779,478, 0), 1, 0xffffffff, 0, 1, 1);

	if(m_lpD3DD7->BeginScene()!=D3D_OK)
	{
		TRACE("Couldn't begin scene\n");
	}
	hRet = m_lpD3DD7->SetTexture(0, D3DTextr_GetSurface( ".\\MagicRes\\chat.bmp" ));
	if( FAILED(hRet) )
	{
		TRACE( "SetTexture Error : %s\n",D3DErrorToString(hRet) );
	}
	else
	{
		hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_d3dSpriteVertices, 4, D3DDP_WAIT );
		if( FAILED(hRet) )
		{
			TRACE("Couldn't draw primitive\n");
		}

	}
	m_lpD3DD7->EndScene();
}

BOOL CDirect3D::D3DRender(TCHAR* strName,int x, int y, int nCurFrame)
{
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ONE );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
	HRESULT hRet;
	static DWORD dwFrameTime = 0;
	static DWORD dwTime = 0;

//MRF TEST를 위한것
	int		*pFrame		= NULL;
	float	*pTu		= NULL;
	float	*pTv		= NULL;
	int		*pCurFrame	= NULL;
	int		*pWidth		= NULL;
	int		*pHeight	= NULL;
	int		*pTime		= NULL;
	int		*pX			= NULL;
	int		*pY			= NULL;

	hRet = D3DTextr_TextureInfo( strName, &pFrame, &pCurFrame, &pTu, &pTv, &pWidth, &pHeight, &pTime, &pX, &pY);
	if( FAILED(hRet) )
	{
		TRACE("Couldn't get d3dtextr_textureinfo : %s\n", D3DErrorToString( hRet ) );
//		return;
	}
		*pTu = float(nCurFrame) / *pFrame;

	float fTv = 0.0;
	float fTvB = 1.0;
	float fTopY = float(y - *pY);
	float fBottomY;

	if( y + *pHeight - *pY < 0 ) return FALSE;
	if( y - *pY > 600 ) return FALSE;

	fBottomY = float(y + *pHeight - *pY);

	if( y - *pY < 0 )
	{
		fTv = float( *pY - y ) / *pHeight;
		fTopY = 0.0;
	}
	
	if( fBottomY >= 600 )
	{
		fTvB = 1 - (( fBottomY - 600 ) / *pHeight);
		fBottomY = 599.0;
	}

	float fTu = 0.0;
	float fTuL = 0.0;
	float fLeftX = float(x - *pX);
	float fRightX = float(x + *pWidth - *pX);

	if( fLeftX  > 800 ) return FALSE;
	if( fRightX <   0 ) return FALSE; 

	if( fRightX >= 800 )
	{
		fTu = ( fRightX - 800 ) / *pWidth;
		fRightX = 799.0;
	}
	if( fLeftX <= 0 )
	{
		fTuL = (-fLeftX) / *pWidth;
		fLeftX = 0.0;
	}
	m_d3dSpriteVertices[0] = D3DTLVERTEX(D3DVECTOR(fLeftX ,    fTopY, 0), 1, 0xffffffff, 0, 0, fTv);
	m_d3dSpriteVertices[1] = D3DTLVERTEX(D3DVECTOR(fRightX,    fTopY, 0), 1, 0xffffffff, 0, 0, fTv);
	m_d3dSpriteVertices[2] = D3DTLVERTEX(D3DVECTOR(fLeftX , fBottomY, 0), 1, 0xffffffff, 0, 0, fTvB);
	m_d3dSpriteVertices[3] = D3DTLVERTEX(D3DVECTOR(fRightX, fBottomY, 0), 1, 0xffffffff, 0, 0, fTvB);

	m_d3dSpriteVertices[0].tu = *pTu + float(1.0 / *pFrame)*fTuL;
	m_d3dSpriteVertices[2].tu = *pTu + float(1.0 / *pFrame)*fTuL;
	m_d3dSpriteVertices[1].tu = *pTu + float(1.0 / *pFrame)*(1 - fTu);
	m_d3dSpriteVertices[3].tu = *pTu + float(1.0 / *pFrame)*(1 - fTu);

	if(*pTu >= 1.0) *pTu = 0.0;

//	if(m_lpD3DD7->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color0x00, 1.0f, 0)!=D3D_OK)
//	{
//		TRACE("Couldn't clear the target\n");
//	} 

	if(m_lpD3DD7->BeginScene()!=D3D_OK)
	{
		TRACE("Couldn't begin scene\n");
	}
	hRet = m_lpD3DD7->SetTexture(0, D3DTextr_GetSurface( strName ));
	if( FAILED(hRet) )
	{
		TRACE( "SetTexture Error : %s\n",D3DErrorToString(hRet) );
	}
	else
	{
//		if(m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_d3dSpriteVertices, 4, 0)!=DD_OK)
		hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_d3dSpriteVertices, 4, D3DDP_WAIT );
		if( FAILED(hRet) )
		{
			TRACE("Couldn't draw primitive\n");
		}

	}
	m_lpD3DD7->EndScene();
	if(nCurFrame >= (*pFrame-1)) {*pCurFrame = 0;*pTv = 0; return TRUE;}
	return FALSE;
}

void CDirect3D::DrawLightMask(CArray<int,int>* arr_Mask)
{
/*
	HRESULT hRet;
	RECT rc;
	RECT rcSour;
	RECT rcDest;

	DDCOLORKEY colorkey;
	colorkey.dwColorSpaceHighValue = 0x00000000;
	colorkey.dwColorSpaceLowValue  = 0x00000000;
	D3DTextr_GetSurface("fire.bmp")->SetColorKey( DDCKEY_SRCBLT, &colorkey );



	int nArrSize = arr_Mask->GetSize();
	for( int i = 0 ; i < nArrSize ; i++ )
	{
		CPoint pt;
		int nWidth;
		int nHeight;
		SetRect(&rc, -(nWidth>>1),-(nHeight>>1),nWidth>>1,nHeight>>1);
		rc.left   = pt.x - rc.left   ;
		rc.right  = pt.x + rc.right  ;
		rc.top    = pt.y - rc.top    ;
		rc.bottom = pt.y + rc.bottom ;

//각각의 마스크를 클립핑
		SetRect(&rcSour,0,0,128,128);
		if( rc.left    <=   0 ) rcSour.left   = int( (-rc.left)/float(nWidth)*128 );
		if( rc.right   >= 800 ) rcSour.right  = 128 - int( ( rc.right - 800 )/float(nWidth)*128 );
		if( rc.top     <=   0 ) rcSour.top    = int( (-rc.top)/float(nHeight)*128 );
		if( rc.bottom  >= 600 ) rcSour.bottom = 128 - int( ( rc.bottom - 600 )/float(nHeight)*128 );

		rcDest.left   = int( rc.left   * 256/800.0);
		rcDest.right  = int( rc.right  * 256/800.0);
		rcDest.top	  = int( rc.top    * 256/600.0);
		rcDest.bottom = int( rc.bottom * 256/600.0);
		if( rcDest.left   <=   0 ) rcDest.left   =   0;
		if( rcDest.right  >= 256 ) rcDest.right  = 256;
		if( rcDest.top    <=   0 ) rcDest.top    =   0;
		if( rcDest.bottom >= 256 ) rcDest.bottom = 256;

		//D3DTextr_GetSurface(strName[picnum])->Blt
		//( &rcDest, D3DTextr_GetSurface("fire.bmp"), &rcSour, DDBLT_WAIT, NULL);

	}
*/
}
BOOL CDirect3D::D3DRenderDay( int nType, int nDayState )
{
//	return FALSE;
    m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
    m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND,  9 );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, 5 );

	HRESULT hRet;
	
//	static BOOL bConverted = TRUE;
	static DWORD dwFrameTime = GetTickCount();
	DWORD dwNowTime = GetTickCount();

	if( nType )
	{
		m_d3dDayVertices[0].sx =  400;
		m_d3dDayVertices[0].tu = 0.25;
		m_d3dDayVertices[1].tu = 0.75;
		m_d3dDayVertices[2].sx =  400;
		m_d3dDayVertices[2].tu = 0.25;
		m_d3dDayVertices[3].tu = 0.75;
	}
	else
	{
		m_d3dDayVertices[0].sx =   0;
		m_d3dDayVertices[0].tu =   0;
		m_d3dDayVertices[1].tu =   1;
		m_d3dDayVertices[2].sx =   0;
		m_d3dDayVertices[2].tu =   0;
		m_d3dDayVertices[3].tu =   1;
	}

	static DWORD dwAlpha = 0xffffffff;
	static int nSun_set = 2;
	static int nPreDayState = 0;
	if( nPreDayState != nDayState )
	{
		int j;
		switch( nDayState )
		{
		case 1:
//			dwAlpha = 0x01ffffff;
			dwAlpha = 0x55ffffff;
			nSun_set = 0;
			if( nPreDayState == 0 )
			{
				for( j = 0; j < 4; j++ )
				{
					m_d3dDayVertices[j].dcColor = 0xaaffffff;
				}
			}
			break;
		case 2:
			nSun_set = 0;
//			dwAlpha = 0x55ffffff;
			dwAlpha = 0xaaffffff;
			if( nPreDayState == 0 )
			{
				for( j = 0; j < 4; j++ )
				{
					m_d3dDayVertices[j].dcColor = 0xfeffffff;
				}
			}
			break;
		case 3:
			nSun_set = 0;
//			dwAlpha = 0xaaffffff;
			dwAlpha = 0xfeffffff;
			if( nPreDayState == 0 )
			{
				for( j = 0; j < 4; j++ )
				{
					m_d3dDayVertices[j].dcColor = 0xfeffffff;
				}
			}
			break;
		case 4://이값은 들어 오지 않음
//			dwAlpha = 0xfeffffff;
			break;
		case 5:
			nSun_set = 1;
//			dwAlpha = 0xfeffffff;
			dwAlpha = 0xaaffffff;
			if( nPreDayState == 0 )
			{
				for( j = 0; j < 4; j++ )
				{
					m_d3dDayVertices[j].dcColor = 0xaaffffff;
				}
			}
			break;
		case 6:
			nSun_set = 1;
			dwAlpha = 0x55ffffff;
			if( nPreDayState == 0 )
			{
				for( j = 0; j < 4; j++ )
				{
					m_d3dDayVertices[j].dcColor = 0x55ffffff;
				}
			}
			break;
		case 7:
			nSun_set = 1;
//			dwAlpha = 0xaaffffff;
			dwAlpha = 0x01ffffff;
			if( nPreDayState == 0 )
			{
				for( j = 0; j < 4; j++ )
				{
					m_d3dDayVertices[j].dcColor = 0x01ffffff;
				}
			}
			break;
		case 8:
			nSun_set = 1;
			dwAlpha = 0x01ffffff;
			if( nPreDayState == 0 )
			{
				for( j = 0; j < 4; j++ )
				{
					m_d3dDayVertices[j].dcColor = 0x01ffffff;
				}
			}
			break;
		}

		nPreDayState = nDayState;
	}
	if( dwNowTime - dwFrameTime > 100 )
	{
		dwFrameTime = dwNowTime;
		for( int i = 0; i < 4 ; i++ )
		{
			if( nSun_set == 1 )
			{
				m_d3dDayVertices[i].dcColor -= 0x01000000;//010101;
				if( m_d3dDayVertices[i].dcColor <= dwAlpha)//0x01ffffff )
				{
					m_d3dDayVertices[i].dcColor = dwAlpha;//0x01ffffff;
	//				m_d3dDayVertices[i].dcColor = 0xffffffff;
				}

			}
			else if( nSun_set == 0 )
			{
				m_d3dDayVertices[i].dcColor += 0x01000000;//010101;
				if( m_d3dDayVertices[i].dcColor >= dwAlpha)//0xfeffffff )
				{
					m_d3dDayVertices[i].dcColor = dwAlpha;//0xfeffffff;
	//				m_d3dDayVertices[i].dcColor = 0xffffffff;
				}

			}
		}
	}

	if(m_lpD3DD7->BeginScene()!=D3D_OK)
	{
		TRACE("Couldn't begin scene\n");
	}

//	hRet = m_lpD3DD7->SetTexture(0, D3DTextr_GetSurface( ".\\MagicRes\\light2.bmp" ));

	//if( FAILED(hRet) )
	//{
	//	TRACE( "SetTexture Error : %s\n",D3DErrorToString(hRet) );
	//}
	//else
	//{
		hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_d3dDayVertices, 4, D3DDP_WAIT );
		if( FAILED(hRet) )
		{
			TRACE("Couldn't draw primitive\n");
		}

	//}
	m_lpD3DD7->EndScene();

	return TRUE;
}
BOOL CDirect3D::Clip( RECT* pRC, float* lTu, float* rTu, float* tTv,float* bTv)
{
	if( pRC->left >= 800 ) return FALSE;
	if( pRC->right < 0 )  return FALSE;
	if( pRC->bottom < 0 ) return FALSE;
//	if( pRC->top >= 600 )  return FALSE;
	if( pRC->top >= 495 )  return FALSE;
	float fWidth =(float)(pRC->right - pRC->left);
	float fHeight = (float)(pRC->bottom - pRC->top);

	if( pRC->left < 0 )
	{
		*lTu = -(pRC->left)/fWidth; 
		pRC->left = 1;
	}
	if( pRC->top < 0 )
	{
		*tTv = -(pRC->top)/fHeight;
		pRC->top = 1;
	}
	if( pRC->right > 799 )
	{
		*rTu = 1.0f - (pRC->right - 800)/fWidth;
		pRC->right = 799;
	}
//	if( pRC->bottom > 599 )
//	{
//		*bTv = 1.0f - (pRC->bottom - 600)/fHeight;
//		pRC->bottom = 599;
//	}
	if( pRC->bottom > 495 )
	{
		*bTv = 1.0f - (pRC->bottom - 495)/fHeight;
		pRC->bottom = 495;
	}
	return TRUE;
}
BOOL CDirect3D::D3DRenderShadow(TCHAR* strName,int x, int y, int nCurFrame)
{
	HRESULT hRet;
	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);//
	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND,  9 );
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, 1 );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND,  m_SorBlend );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, m_DesBlend );
	static DWORD dwFrameTime = 0;
	static DWORD dwTime = 0;

//MRF TEST를 위한것
	int		*pFrame		= NULL;
	int		*pX			= NULL;
	int		*pY			= NULL;
	RECT	*pRC		= NULL;
	TCHAR buffer[5];
	itoa(nCurFrame,buffer,10);
	TCHAR strAddName[100];
	strcpy(strAddName,strName);
	strcat(strAddName,buffer);

	hRet = D3DTextr_TextureInfoEx( strAddName, &pFrame, &pRC);
	if( FAILED(hRet) )
	{
//		TRACE("Couldn't get textureinfoex : %s\n", D3DErrorToString( hRet ) );
		return TRUE;
	}
	RECT rc;
	float lTu=0.0f,rTu=1.0f,tTv=0.0f,bTv=1.0f;
	rc.left   = x + pRC->left;
	rc.right  = x + pRC->right;
	rc.top    = y + pRC->top;
	rc.bottom = y + pRC->bottom;

	if( Clip( &rc, &lTu, &rTu, &tTv,&bTv ) )
	{
		m_d3dSpriteVertices[0] = D3DTLVERTEX(D3DVECTOR(float(rc.left) , float(rc.top)   , 0), 1, 0xffffffff, 0, lTu, tTv);
		m_d3dSpriteVertices[1] = D3DTLVERTEX(D3DVECTOR(float(rc.right), float(rc.top)   , 0), 1, 0xffffffff, 0, rTu, tTv);
		m_d3dSpriteVertices[2] = D3DTLVERTEX(D3DVECTOR(float(rc.left) , float(rc.bottom), 0), 1, 0xffffffff, 0, lTu, bTv);
		m_d3dSpriteVertices[3] = D3DTLVERTEX(D3DVECTOR(float(rc.right), float(rc.bottom), 0), 1, 0xffffffff, 0, rTu, bTv);

		if(m_lpD3DD7->BeginScene()!=D3D_OK)
		{
			TRACE("Couldn't begin scene\n"); return FALSE;
		}
		hRet = m_lpD3DD7->SetTexture(0, D3DTextr_GetSurface( strAddName ));
		if( FAILED(hRet) )
		{
			TRACE( "SetTexture Error : %s\n",D3DErrorToString(hRet) ); return FALSE;
		}
		else
		{
			hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_d3dSpriteVertices, 4, D3DDP_WAIT );
			if( FAILED(hRet) )
			{
				TRACE("Couldn't draw primitive\n"); return FALSE;
			}

		}
		m_lpD3DD7->EndScene();
	}
	if( nCurFrame >= *pFrame)  return TRUE;
	return FALSE;
}

BOOL CDirect3D::D3DRenderEx(TCHAR* strName,int x, int y, int nCurFrame)
{
	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ONE );
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
	HRESULT hRet;
	static DWORD dwFrameTime = 0;
	static DWORD dwTime = 0;

//MRF TEST를 위한것
	int		*pFrame		= NULL;
	int		*pX			= NULL;
	int		*pY			= NULL;
	RECT	*pRC		= NULL;
	TCHAR buffer[5];
	itoa(nCurFrame,buffer,10);
	TCHAR strAddName[100];
	strcpy(strAddName,strName);
	strcat(strAddName,buffer);

	hRet = D3DTextr_TextureInfoEx( strAddName, &pFrame, &pRC);
	if( FAILED(hRet) )
	{
//		TRACE("Couldn't get textureinfoex : %s\n", D3DErrorToString( hRet ) );
		return TRUE;
	}
	RECT rc;
	float lTu=0.0f,rTu=1.0f,tTv=0.0f,bTv=1.0f;
	rc.left   = x + pRC->left;
	rc.right  = x + pRC->right;
	rc.top    = y + pRC->top;
	rc.bottom = y + pRC->bottom;

	if( Clip( &rc, &lTu, &rTu, &tTv,&bTv ) )
	{
		m_d3dSpriteVertices[0] = D3DTLVERTEX(D3DVECTOR(float(rc.left) , float(rc.top)   , 0), 1, 0xffffffff, 0, lTu, tTv);
		m_d3dSpriteVertices[1] = D3DTLVERTEX(D3DVECTOR(float(rc.right), float(rc.top)   , 0), 1, 0xffffffff, 0, rTu, tTv);
		m_d3dSpriteVertices[2] = D3DTLVERTEX(D3DVECTOR(float(rc.left) , float(rc.bottom), 0), 1, 0xffffffff, 0, lTu, bTv);
		m_d3dSpriteVertices[3] = D3DTLVERTEX(D3DVECTOR(float(rc.right), float(rc.bottom), 0), 1, 0xffffffff, 0, rTu, bTv);

		if(m_lpD3DD7->BeginScene()!=D3D_OK)
		{
			TRACE("Couldn't begin scene\n"); return FALSE;
		}
		hRet = m_lpD3DD7->SetTexture(0, D3DTextr_GetSurface( strAddName ));
		if( FAILED(hRet) )
		{
			TRACE( "SetTexture Error : %s\n",D3DErrorToString(hRet) ); return FALSE;
		}
		else
		{
			hRet = m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_d3dSpriteVertices, 4, D3DDP_WAIT );
			if( FAILED(hRet) )
			{
				TRACE("Couldn't draw primitive\n"); return FALSE;
			}

		}
		m_lpD3DD7->EndScene();
	}
	if( nCurFrame >= (*pFrame - 1))  return TRUE;
	return FALSE;
}

//----------------------------------------------------------------------
// 
// Function     : D3DErrorToString(HRESULT hErr)
//
// Purpose      : Traces an error (Direct3D)
//
//----------------------------------------------------------------------

char*
D3DErrorToString(HRESULT hErr)
{       
    switch (hErr)
    {
        case D3DERR_BADMAJORVERSION : return("D3DERR_BADMAJORVERSION\n"); 
        case D3DERR_BADMINORVERSION : return("D3DERR_BADMINORVERSION\n"); 
        case D3DERR_EXECUTE_CREATE_FAILED : return("D3DERR_EXECUTE_CREATE_FAILED\n"); 
        case D3DERR_EXECUTE_DESTROY_FAILED : return("D3DERR_EXECUTE_DESTROY_FAILED\n"); 
        case D3DERR_EXECUTE_LOCK_FAILED : return("D3DERR_EXECUTE_LOCK_FAILED\n"); 
        case D3DERR_EXECUTE_UNLOCK_FAILED : return("D3DERR_EXECUTE_UNLOCK_FAILED\n"); 
        case D3DERR_EXECUTE_LOCKED : return("D3DERR_EXECUTE_LOCKED\n"); 
        case D3DERR_EXECUTE_NOT_LOCKED : return("D3DERR_EXECUTE_NOT_LOCKED\n"); 
        case D3DERR_EXECUTE_FAILED : return("D3DERR_EXECUTE_FAILED\n"); 
        case D3DERR_EXECUTE_CLIPPED_FAILED : return("D3DERR_EXECUTE_CLIPPED_FAILED\n"); 
        case D3DERR_TEXTURE_NO_SUPPORT : return("D3DERR_TEXTURE_NO_SUPPORT\n"); 
        case D3DERR_TEXTURE_CREATE_FAILED : return("D3DERR_TEXTURE_CREATE_FAILED\n"); 
        case D3DERR_TEXTURE_DESTROY_FAILED : return("D3DERR_TEXTURE_DESTROY_FAILED\n"); 
        case D3DERR_TEXTURE_LOCK_FAILED : return("D3DERR_TEXTURE_LOCK_FAILED\n"); 
        case D3DERR_TEXTURE_UNLOCK_FAILED : return("D3DERR_TEXTURE_UNLOCK_FAILED\n"); 
        case D3DERR_TEXTURE_LOAD_FAILED : return("D3DERR_TEXTURE_LOAD_FAILED\n"); 
        case D3DERR_TEXTURE_SWAP_FAILED : return("D3DERR_TEXTURE_SWAP_FAILED\n"); 
        case D3DERR_TEXTURE_LOCKED : return("D3DERR_TEXTURE_LOCKED\n"); 
        case D3DERR_TEXTURE_NOT_LOCKED : return("D3DERR_TEXTURE_NOT_LOCKED\n"); 
        case D3DERR_TEXTURE_GETSURF_FAILED : return("D3DERR_TEXTURE_GETSURF_FAILED\n"); 
        case D3DERR_MATRIX_CREATE_FAILED : return("D3DERR_MATRIX_CREATE_FAILED\n"); 
        case D3DERR_MATRIX_DESTROY_FAILED : return("D3DERR_MATRIX_DESTROY_FAILED\n"); 
        case D3DERR_MATRIX_SETDATA_FAILED : return("D3DERR_MATRIX_SETDATA_FAILED\n"); 
        case D3DERR_MATRIX_GETDATA_FAILED : return("D3DERR_MATRIX_GETDATA_FAILED\n"); 
        case D3DERR_SETVIEWPORTDATA_FAILED : return("D3DERR_SETVIEWPORTDATA_FAILED\n"); 
        case D3DERR_MATERIAL_CREATE_FAILED : return("D3DERR_MATERIAL_CREATE_FAILED\n"); 
        case D3DERR_MATERIAL_DESTROY_FAILED : return("D3DERR_MATERIAL_DESTROY_FAILED\n"); 
        case D3DERR_MATERIAL_SETDATA_FAILED : return("D3DERR_MATERIAL_SETDATA_FAILED\n"); 
        case D3DERR_MATERIAL_GETDATA_FAILED : return("D3DERR_MATERIAL_GETDATA_FAILED\n"); 
        case D3DERR_LIGHT_SET_FAILED : return("D3DERR_LIGHT_SET_FAILED\n"); 
        case D3DERR_SCENE_IN_SCENE : return("D3DERR_SCENE_IN_SCENE\n"); 
        case D3DERR_SCENE_NOT_IN_SCENE : return("D3DERR_SCENE_NOT_IN_SCENE\n"); 
        case D3DERR_SCENE_BEGIN_FAILED : return("D3DERR_SCENE_BEGIN_FAILED\n"); 
        case D3DERR_SCENE_END_FAILED : return("D3DERR_SCENE_END_FAILED\n"); 

        default : return("Unknown Error\n"); 
    }
}
