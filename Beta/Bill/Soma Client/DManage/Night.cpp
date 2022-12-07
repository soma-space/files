// Night.cpp: implementation of the CNight class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ColorVertex.h"
#include "Night.h"
#include "d3dtextr.h"
#include "BB_Direct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNight::CNight(int PF_565)
{
	if( PF_565 == 565 )
	{
		m_dwALPHAMASK1=0xf81f07c0;
		m_dwALPHAMASK2=0x07c0f81f;
	}
	else
	{
		m_dwALPHAMASK1=0x7c1f03c0;
		m_dwALPHAMASK2=0x03c07c1f;
	}
	m_nAlpha		= 31;
	m_nVillageAlpha = 16;
	m_nNightType	= 0;
	m_bVEnabled		= FALSE;
	m_nPreDayState  = 0;
	InitLightMap();
}
CNight::CNight(int nWidthRate, int nHeightRate, float ScreenX, float ScreenY, LPDIRECT3DDEVICE7 lpD3DD7, int nVGAType)
{
	m_nWidthRate	= nWidthRate;
	m_nHeightRate	= nHeightRate;
	m_ScreenX		= ScreenX;
	m_ScreenY		= ScreenY;
	m_stColorV		= new CColorVertex[nWidthRate*nHeightRate];
	m_dwAlpha		= 0xff000000;
	m_lpD3DD7		= lpD3DD7;
	m_bNightEnabled = TRUE;
	m_VGAType		= nVGAType;
	m_Alphaarg1		= D3DTA_DIFFUSE;
	switch( m_VGAType )
	{
	case 0:
		m_SorBlend = 9;
		m_DesBlend = 5;
	break;
	case 1:
		m_SorBlend = 1;
		m_DesBlend = 5;
	break;
	case 2:
		m_Alphaarg1 = D3DTA_TEXTURE;
		m_SorBlend = 9;
		m_DesBlend = 5;
	break;
	default:
		m_SorBlend = 9;
		m_DesBlend = 5;
	break;
	}
	m_SpriteVertices[0] = D3DTLVERTEX(D3DVECTOR(0, 0, 0), 1, 0x000000, 0, 0, 0); 
	m_SpriteVertices[1] = D3DTLVERTEX(D3DVECTOR(0, 0, 0), 1, 0x000000, 0, 1, 0); 
	m_SpriteVertices[2] = D3DTLVERTEX(D3DVECTOR(0, 0, 0), 1, 0x000000, 0, 0, 1); 
	m_SpriteVertices[3] = D3DTLVERTEX(D3DVECTOR(0, 0, 0), 1, 0x000000, 0, 1, 1); 

	InitColorVertex();
}

CNight::~CNight()
{
//	if( m_stColorV ) delete [] m_stColorV;
//	m_stColorV = NULL;
}

void CNight::InitColorVertex()
{
	D3DTextr_CreateTextureFromFile("mask.bmp", 0, D3DTEXTR_16BITSPERPIXEL);
	D3DTextr_Restore("mask.bmp", m_lpD3DD7 );
	float width  = m_ScreenX / m_nWidthRate;
	float height = m_ScreenY / m_nHeightRate;
	for( int i = 0 ; i < m_nWidthRate; i++ )
		for( int j= 0; j < m_nHeightRate; j++ )
		{
			m_stColorV[j + i*m_nHeightRate].m_stVertex[0].m_dwColor	= 0;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[0].m_dwAlpha	= 0x000000;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[0].m_xPos 	= i*width;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[0].m_yPos	= j*height;

			m_stColorV[j + i*m_nHeightRate].m_stVertex[1].m_dwColor	= 0;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[1].m_dwAlpha	= 0x000000;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[1].m_xPos	= (i+1)*width;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[1].m_yPos	= j*height;
			
			m_stColorV[j + i*m_nHeightRate].m_stVertex[2].m_dwColor	= 0;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[2].m_dwAlpha	= 0x000000;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[2].m_xPos	= i*width;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[2].m_yPos	= (j+1)*height;
			
			m_stColorV[j + i*m_nHeightRate].m_stVertex[3].m_dwColor	= 0;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[3].m_dwAlpha	= 0x000000;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[3].m_xPos	= (i+1)*width;
			m_stColorV[j + i*m_nHeightRate].m_stVertex[3].m_yPos	= (j+1)*height;
		}
}
#define dRGB565ALPHAMASK1 0xf81f07c0
#define dRGB565ALPHAMASK2 0x07c0f81f
#define macGamma1(p) \
{\
	dwSrc1=*p;\
	dwSrc2=dwSrc1;\
	dwSrc1&=dRGB565ALPHAMASK1;\
	dwSrc1>>=5;\
	dwSrc1*=c;\
	dwSrc1&=dRGB565ALPHAMASK1;\
	dwSrc2&=dRGB565ALPHAMASK2;\
	dwSrc2*=c;\
	dwSrc2>>=5;\
	dwSrc2&=dRGB565ALPHAMASK2;\
	dwSrc2|=dwSrc1;\
	*p++=dwSrc2;\
}

void CNight::InitLightMap()
{
	int illuminance;
	for( int y = 0 ; y < 600; y++ )
	{
		for( int x = 0 ; x < 800 ; x++ )
		{
			BYTE *pi=m_aaiLightMap[y]+x;
			int px = x;
			int py = y;
			if( y == 0 ) py = 1;
			if( x == 0 ) px = 1;
			//illuminance = (int)sqrt(180000 - px*px*7 - py*py*14);
			illuminance = (int)sqrt(50000 - px*px*1 - py*py*2);
			if( illuminance > 255 )	illuminance = 255;
			*pi=illuminance;

			pi=m_aaiLightMap1[y]+x;
			//illuminance = (int)sqrt(80000 - px*px*7 - py*py*14);
			illuminance = (int)sqrt(80000 - px*px*4 - py*py*8);
			if( illuminance > 255 ) illuminance = 255;
			*pi=illuminance;

			pi=m_aaiLightMap2[y]+x;
			illuminance = 10000000/(px*px/2+py*py);
			//illuminance = (int)sqrt(380000 - px*px*7 - py*py*14);
//			illuminance = (int)sqrt(90000 - px*px*1 - py*py*2);
			if( illuminance > 255 ) illuminance = 255;
			*pi=illuminance;
		}
	}
}

/*
void CNight::InitLightMap()
{
	int illuminance;
	for( int y = 0 ; y < 600; y++ )
	{
		for( int x = 0 ; x < 800 ; x++ )
		{
			BYTE *pi=m_aaiLightMap[y]+x;
			int px = x;
			int py = y;
			if( y == 0 ) py = 1;
			if( x == 0 ) px = 1;
			//illuminance = (int)sqrt(180000 - px*px*7 - py*py*14);
			illuminance = 3000000/(px*px/2+py*py);
			if( illuminance > 255 )	illuminance = 255;
			*pi=illuminance;

			pi=m_aaiLightMap1[y]+x;
			illuminance = 500000/(px*px/2+py*py);
			if( illuminance > 255 ) illuminance = 255;
			*pi=illuminance;

			pi=m_aaiLightMap2[y]+x;
			illuminance = 10000000/(px*px/2+py*py);
			if( illuminance > 255 ) illuminance = 255;
			*pi=illuminance;
		}
	}
}
*/

void CNight::SetLightStain()
{
	BYTE dir = 100;
	double distance = sqrt( (m_ptMyPos.x - m_ptLightStain.x)*
							(m_ptMyPos.x - m_ptLightStain.x)+
							(m_ptMyPos.y - m_ptLightStain.y)*
							(m_ptMyPos.y - m_ptLightStain.y));
	if( distance > 100 )
	{
		m_ptLightStain += m_ptMyPos - m_ptPrePos;
		return;
	}

	distance = sqrt( (m_ptMyPos.x - m_ptLightStain.x)*(m_ptMyPos.x - m_ptLightStain.x)
				+ (m_ptMyPos.y - m_ptLightStain.y)*(m_ptMyPos.y - m_ptLightStain.y) );
	double dx,dy;
	if (distance > 0)
	{
		dx = double(m_ptMyPos.x - m_ptLightStain.x) / distance;
		dy = double(m_ptMyPos.y - m_ptLightStain.y) / distance;
	}
	m_ptLightStain += CPoint( int(dx*10), int(dy*10) );
}

CPoint CNight::LightStain(BOOL bRight,CPoint ptMyPos,CPoint ptMapPos)
{
	SetLightStain();
	m_ptPrePos = ptMyPos;
	return m_ptLightStain;

}

BOOL CNight::DrawNight()
{
    m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAARG1, m_Alphaarg1 );
    m_lpD3DD7->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND, 9);
//	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, 5);
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_SRCBLEND, m_SorBlend);
	m_lpD3DD7->SetRenderState( D3DRENDERSTATE_DESTBLEND, m_DesBlend);

	if(m_lpD3DD7->BeginScene()==D3D_OK)
	{

		m_lpD3DD7->SetTexture(0, D3DTextr_GetSurface("mask.bmp"));
		int nDrawNum = m_nWidthRate*m_nHeightRate;
		for( int i = 0 ; i < nDrawNum ; i++ )
		{
			m_SpriteVertices[0].color = m_stColorV[i].m_stVertex[0].m_dwAlpha + m_stColorV[i].m_stVertex[0].m_dwColor;  
			m_SpriteVertices[0].sx	  = m_stColorV[i].m_stVertex[0].m_xPos;   
			m_SpriteVertices[0].sy	  = m_stColorV[i].m_stVertex[0].m_yPos;   

			m_SpriteVertices[1].color = m_stColorV[i].m_stVertex[1].m_dwAlpha + m_stColorV[i].m_stVertex[1].m_dwColor;  
			m_SpriteVertices[1].sx	  = m_stColorV[i].m_stVertex[1].m_xPos;   
			m_SpriteVertices[1].sy    = m_stColorV[i].m_stVertex[1].m_yPos;   

			m_SpriteVertices[2].color = m_stColorV[i].m_stVertex[2].m_dwAlpha + m_stColorV[i].m_stVertex[2].m_dwColor;  
			m_SpriteVertices[2].sx    = m_stColorV[i].m_stVertex[2].m_xPos;   
			m_SpriteVertices[2].sy    = m_stColorV[i].m_stVertex[2].m_yPos;   

			m_SpriteVertices[3].color = m_stColorV[i].m_stVertex[3].m_dwAlpha + m_stColorV[i].m_stVertex[3].m_dwColor;  
			m_SpriteVertices[3].sx    = m_stColorV[i].m_stVertex[3].m_xPos;   
			m_SpriteVertices[3].sy    = m_stColorV[i].m_stVertex[3].m_yPos;   

			if(m_lpD3DD7->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX, m_SpriteVertices, 4, 0)!=DD_OK)
			{
				m_lpD3DD7->EndScene();
				return FALSE;
			}

			m_stColorV[i].m_stVertex[0].m_dwAlpha = m_dwAlpha;
			m_stColorV[i].m_stVertex[1].m_dwAlpha = m_dwAlpha;
			m_stColorV[i].m_stVertex[2].m_dwAlpha = m_dwAlpha;
			m_stColorV[i].m_stVertex[3].m_dwAlpha = m_dwAlpha;
			m_stColorV[i].m_stVertex[0].m_dwColor = 0;
			m_stColorV[i].m_stVertex[1].m_dwColor = 0;
			m_stColorV[i].m_stVertex[2].m_dwColor = 0;
			m_stColorV[i].m_stVertex[3].m_dwColor = 0;

		}
	}
	else return FALSE;
	
	m_lpD3DD7->EndScene();

	return TRUE;
}

void CNight::SetLight( CPoint pt, int illuminance, COLORREF color )
{
	float xPos;
	float yPos;
	DWORD dwAlpha;
	DWORD dwDelta;

	DWORD R = color & RED_MASK;
	DWORD G = color & GREEN_MASK;
	DWORD B = color & BLUE_MASK;
//	COLORREF Vcolor; 

	int nDrawNum = m_nWidthRate*m_nHeightRate;
	for( int i = 0 ; i < nDrawNum ; i++ )
	{
/////////////////////////////////////////////////////////////
//0
		xPos = m_stColorV[i].m_stVertex[0].m_xPos - pt.x; 
		yPos = m_stColorV[i].m_stVertex[0].m_yPos - pt.y;
		if( xPos == 0 && yPos == 0 )
			dwAlpha = 0xff;
		else
			dwAlpha = (DWORD)( illuminance / (xPos*xPos + yPos*yPos) ); 
/*
		if( dwAlpha >= 128)
		{
			Vcolor = m_stColorV[i].m_stVertex[0].m_dwColor;
			m_stColorV[i].m_stVertex[0].m_dwColor = Vcolor | R + Vcolor | G + Vcolor | B;
		}
*/
//		if( dwAlpha <= 0xff )
//			m_stColorV[i].m_stVertex[0].m_dwColor = dwAlpha;
//		else  
//			m_stColorV[i].m_stVertex[0].m_dwColor = 0xff;

		dwDelta = m_stColorV[i].m_stVertex[0].m_dwAlpha;
		dwDelta = dwDelta>>24;
		dwDelta += dwAlpha;
		if( dwDelta >= 0xff ) dwDelta = 0xff;
		if( m_dwAlpha <= ( dwDelta << 24 ) )
			m_stColorV[i].m_stVertex[0].m_dwAlpha = dwDelta << 24;


/////////////////////////////////////////////////////////////
//1
		xPos = m_stColorV[i].m_stVertex[1].m_xPos - pt.x; 
		yPos = m_stColorV[i].m_stVertex[1].m_yPos - pt.y;
		if( xPos == 0 && yPos == 0 )
			dwAlpha = 0xff;
		else
			dwAlpha = (DWORD)( illuminance / (xPos*xPos + yPos*yPos) ); 
/*
		if( dwAlpha >= 128)
		{
			Vcolor = m_stColorV[i].m_stVertex[1].m_dwColor;
			m_stColorV[i].m_stVertex[1].m_dwColor = Vcolor | R + Vcolor | G + Vcolor | B;
		}
*/
//		if( dwAlpha <= 0xff )
//			m_stColorV[i].m_stVertex[1].m_dwColor = dwAlpha;
//		else
//			m_stColorV[i].m_stVertex[1].m_dwColor = 0xff;

		dwDelta = m_stColorV[i].m_stVertex[1].m_dwAlpha;
		dwDelta = dwDelta>>24;
		dwDelta += dwAlpha;
		if( dwDelta >= 0xff ) dwDelta = 0xff;
		if( m_dwAlpha <= ( dwDelta << 24 ) )
			m_stColorV[i].m_stVertex[1].m_dwAlpha = dwDelta << 24;

/////////////////////////////////////////////////////////////
//2
		xPos = m_stColorV[i].m_stVertex[2].m_xPos - pt.x; 
		yPos = m_stColorV[i].m_stVertex[2].m_yPos - pt.y;
		if( xPos == 0 && yPos == 0 )
			dwAlpha = 0xff;
		else
			dwAlpha = (DWORD)( illuminance / (xPos*xPos + yPos*yPos) ); 
/*		
		if( dwAlpha >= 128)
		{
			Vcolor = m_stColorV[i].m_stVertex[2].m_dwColor;
			m_stColorV[i].m_stVertex[2].m_dwColor = Vcolor | R + Vcolor | G + Vcolor | B;
		}
*/
//		if( dwAlpha <= 0xff )
//			m_stColorV[i].m_stVertex[2].m_dwColor = dwAlpha;
//		else  
//			m_stColorV[i].m_stVertex[2].m_dwColor = 0xff;

		dwDelta = m_stColorV[i].m_stVertex[2].m_dwAlpha;
		dwDelta = dwDelta>>24;
		dwDelta += dwAlpha;
		if( dwDelta >= 0xff ) dwDelta = 0xff;
		if( m_dwAlpha <= ( dwDelta << 24 ) )
			m_stColorV[i].m_stVertex[2].m_dwAlpha = dwDelta << 24;

/////////////////////////////////////////////////////////////
//3
		xPos = m_stColorV[i].m_stVertex[3].m_xPos - pt.x; 
		yPos = m_stColorV[i].m_stVertex[3].m_yPos - pt.y;
		if( xPos == 0 && yPos == 0 )
			dwAlpha = 0xff;
		else
			dwAlpha = (DWORD)( illuminance / (xPos*xPos + yPos*yPos) ); 
/*		
		if( dwAlpha >= 128)
		{
			Vcolor = m_stColorV[i].m_stVertex[3].m_dwColor;
			m_stColorV[i].m_stVertex[3].m_dwColor = Vcolor | R + Vcolor | G + Vcolor | B;
		}
*/
//		if( dwAlpha <= 0xff )
//			m_stColorV[i].m_stVertex[3].m_dwColor = dwAlpha;
//		else
//			m_stColorV[i].m_stVertex[3].m_dwColor = 0xff;

		dwDelta = m_stColorV[i].m_stVertex[3].m_dwAlpha;
		dwDelta = dwDelta>>24;
		dwDelta += dwAlpha;
		if( dwDelta >= 0xff ) dwDelta = 0xff;
		if( m_dwAlpha <= ( dwDelta << 24 ) )
			m_stColorV[i].m_stVertex[3].m_dwAlpha = dwDelta << 24;
	}
}

void CNight::SetNightState( int nDayState )
{
	static DWORD dwFrameTime = GetTickCount();
	DWORD dwNowTime = GetTickCount();
	static DWORD dwAlpha = 0xff000000;

	static int nSun_set = 2;
	static int nPreDayState = 0;
	if( nPreDayState != nDayState )
	{
		switch( nDayState )
		{
		case 1:
			dwAlpha = 0x55000000;
			nSun_set = 0;
			if( nPreDayState == 0 )
			{
				m_dwAlpha = 0xaa000000;
			}
			break;
		case 2:
			nSun_set = 0;
			dwAlpha = 0xaa000000;
			if( nPreDayState == 0 )
			{
				m_dwAlpha = 0xfe000000;
			}
			break;
		case 3:
			nSun_set = 0;
			dwAlpha = 0xfe000000;
			if( nPreDayState == 0 )
			{
				m_dwAlpha = 0xfe000000;
			}
			break;
		case 4://이값은 들어 오지 않음
			break;
		case 5:
			nSun_set = 1;
			dwAlpha = 0xaa000000;
			if( nPreDayState == 0 )
			{
				m_dwAlpha = 0xaa000000;
			}
			break;
		case 6:
			nSun_set = 1;
			dwAlpha = 0x55000000;
			if( nPreDayState == 0 )
			{
				m_dwAlpha = 0x55000000;
			}
			break;
		case 7:
			nSun_set = 1;
			dwAlpha = 0x10000000;
			if( nPreDayState == 0 )
			{
				m_dwAlpha = 0x10000000;
			}
			break;
		case 8:
			nSun_set = 1;
			dwAlpha = 0x10000000;
			if( nPreDayState == 0 )
			{
				m_dwAlpha = 0x10000000;
			}
			break;
		}

		nPreDayState = nDayState;
	}

	if( dwNowTime - dwFrameTime > 7140 )//10분에 대한 
	{
		dwFrameTime = dwNowTime;

		if( nSun_set == 1 )
		{
			m_dwAlpha -= 0x01000000;
			if( m_dwAlpha <= dwAlpha)
			{
				m_dwAlpha = dwAlpha;
			}

		}
		else if( nSun_set == 0 )
		{
			m_dwAlpha += 0x01000000;
			if( m_dwAlpha >= dwAlpha)
			{
				m_dwAlpha = dwAlpha;
			}

		}
	
	}

}

BOOL CNight::SetTransFormNight(int type)
{
	switch( type )
	{
	case 1:
		m_nNightType	= 1;//일반
		m_bVEnabled		= TRUE;
		break;
/*	case 12:
		m_nNightType	= 2;//RED
		m_bVEnabled		= TRUE;
		break;
	case 13:
		m_nNightType	= 3;//GREEN
		m_bVEnabled		= TRUE;
		break;
	case 14:
		m_nNightType	= 4;//BLUE
		m_bVEnabled		= TRUE;
		break;
*/	default:
		m_nNightType	= 0;
		m_bVEnabled		= FALSE;
		break;
	}

	return TRUE;
}
int CNight::GetNightState()
{
	if( m_bVEnabled )
	{
		if( m_nAlpha <= m_nVillageAlpha)
			return m_nVillageAlpha;
	}
	return m_nAlpha;
}

void CNight::SetNightStateEx( int nDayState, int type )
{
	static DWORD dwFrameTime = GetTickCount();
	DWORD dwNowTime = GetTickCount();
	static int nAlpha = 31;

	static int nSun_set = 2;
	static int nPreDayState = 0;

	SetTransFormNight(type);
	
	if( m_nPreDayState != nDayState )
	{
		switch( nDayState )
		{
		case 1://일몰
			nAlpha = 8;
			nSun_set = 1;
			if( m_nPreDayState == 0 )
			{
				if( m_gap > 23 ) m_gap = 23;
				m_nPreAlpha = m_nAlpha = 31 - m_gap;
			}
			break;
		case 2://밤
			nSun_set = 1;
			nAlpha = 8;
			if( m_nPreDayState == 0 )
			{
				m_nPreAlpha = m_nAlpha = 8;
			}
			break;
		case 3://일출
			nSun_set = 0;
			nAlpha = 31;
			if( m_nPreDayState == 0 )
			{
				if( m_gap < 8 ) m_gap = 8;
				m_nPreAlpha = m_nAlpha = m_gap;
			}
			break;
		case 4://낮//이값은 들어 오지 않음
			break;
		}

		m_nPreDayState = nDayState;
	}

	if( dwNowTime - dwFrameTime > 18700 )//10분에 대한 
	{
		dwFrameTime = dwNowTime;

		if( nSun_set == 1 )
		{
			m_nAlpha -= 1;
			if( m_nAlpha <= nAlpha)
			{
				m_nAlpha = nAlpha;
			}

		}
		else if( nSun_set == 0 )
		{
			m_nAlpha += 1;
			if( m_nAlpha >= nAlpha)
			{
				m_nAlpha = nAlpha;
			}
		
		}
	}
}

//경계선이 작은 밤

BOOL CNight::DrawNight( CDDSurface* pSurface, CArray< LIGHT_INFO, LIGHT_INFO>* ptLightArr, int y_screen,BOOL bRight )
{
	LIGHT_INFO LI;
	if(pSurface->Lock() == NULL)
	{
		TRACE("밤 그려주기 실패\n");
		return FALSE;
	}
	DDSURFACEDESC2& ddsd = pSurface->GetDescription();
	LONG lPitch = ddsd.lPitch/2;
	WORD* lpSurface = (WORD*)ddsd.lpSurface;

	DWORD* pdw1;
	DWORD* pdw2;
	DWORD* pdw3;
	DWORD* pdw4;

	int			 xstart =	0;
	if( bRight ) xstart = 400;
	else		 xstart =	0;

	int nAlpha = GetNightState();

	int nLightNum = ptLightArr->GetSize(); 
	CPoint ptLightPos;
	for( int y = 0 ; y < y_screen; y+=4 )
	{
		pdw1=(DWORD*)(lpSurface+y*lPitch+xstart);
		pdw2=(DWORD*)(lpSurface+(y+1)*lPitch+xstart);
		pdw3=(DWORD*)(lpSurface+(y+2)*lPitch+xstart);
		pdw4=(DWORD*)(lpSurface+(y+3)*lPitch+xstart);

		for( int x = xstart ; x < 800 ; x+=4 )
		{
			int c = 0;int c1 = 0;
			for( int ii = 0 ; ii < nLightNum ; ii++ )
			{
				LI = ptLightArr->GetAt( ii ); 
				ptLightPos = LI.m_pt;
				int dy = ptLightPos.y - y;
				if(dy<0) dy=-dy;
				if(dy>=600) dy = 599;

				int dx = ptLightPos.x - x;
				if(dx<0) dx=-dx;
				if(dx>=800) dx = 799;
		
				switch( LI.m_kind )
				{
				case 0:
					c1=m_aaiLightMap[dy][dx];
					break;
				case 1:
					c1=m_aaiLightMap1[dy][dx];
					break;
				case 2:
					c1=m_aaiLightMap2[dy][dx];
					break;
				default:
					c1=m_aaiLightMap[dy][dx];
					break;
				}
				c1>>=3;
				//if( c1 <= m_nAlpha ) c1 = m_nAlpha;
				if( c1 <= nAlpha ) c1 = nAlpha;
				if( c < c1 ) c = c1;
				if(c1>=255) break;
			}

			if( c>= 31 ) 
			{
				c=31;
				pdw1+=2;
				pdw2+=2;
				pdw3+=2;
				pdw4+=2;
				continue;
			}
			if( c < 2 )
			{
				PutPixelNADark( pdw1, 2);pdw1+=2;
				PutPixelNADark( pdw2, 2);pdw2+=2;
				PutPixelNADark( pdw3, 2);pdw3+=2;
				PutPixelNADark( pdw4, 2);pdw4+=2;
				continue;
			}

			PutPixelDark( pdw1, c, 2,m_dwALPHAMASK1,m_dwALPHAMASK2);pdw1+=2;
			PutPixelDark( pdw2, c, 2,m_dwALPHAMASK1,m_dwALPHAMASK2);pdw2+=2;	
			PutPixelDark( pdw3, c, 2,m_dwALPHAMASK1,m_dwALPHAMASK2);pdw3+=2;	
			PutPixelDark( pdw4, c, 2,m_dwALPHAMASK1,m_dwALPHAMASK2);pdw4+=2;	
		}
	}

	pSurface->Unlock();

	return TRUE;
}



//경계선이 큰 밤
/*
BOOL CNight::DrawNight( CDDSurface* pSurface, CArray< LIGHT_INFO, LIGHT_INFO>* ptLightArr, BOOL bRight )
{
	LIGHT_INFO LI;
	pSurface->Lock();
	DDSURFACEDESC2& ddsd = pSurface->GetDescription();
	LONG lPitch = ddsd.lPitch/2;
	WORD* lpSurface = (WORD*)ddsd.lpSurface;

	DWORD* pdw1;
	DWORD* pdw2;
	DWORD* pdw3;
	DWORD* pdw4;

	int			 xstart =	0;
	if( bRight ) xstart = 400;
	else		 xstart =	0;

	int nLightNum = ptLightArr->GetSize(); 
	CPoint ptLightPos;
	for( int y = 0 ; y < 495; y+=4 )
	{
		pdw1=(DWORD*)(lpSurface+y*lPitch+xstart);
		pdw2=(DWORD*)(lpSurface+(y+1)*lPitch+xstart);
		pdw3=(DWORD*)(lpSurface+(y+2)*lPitch+xstart);
		pdw4=(DWORD*)(lpSurface+(y+3)*lPitch+xstart);

		for( int x = xstart ; x < 800 ; x+=4 )
		{
			int c = 0;
			for( int ii = 0 ; ii < nLightNum ; ii++ )
			{
				LI = ptLightArr->GetAt( ii ); 
				ptLightPos = LI.m_pt;
				int dy = ptLightPos.y - y;
				if(dy<0) dy=-dy;
				if(dy>=600) dy = 599;

				int dx = ptLightPos.x - x;
				if(dx<0) dx=-dx;
				if(dx>=800) dx = 799;
		
				switch( LI.m_kind )
				{
				case 0:
					c+=m_aaiLightMap[dy][dx];
					break;
				case 1:
					c+=m_aaiLightMap1[dy][dx];
					break;
				case 2:
					c+=m_aaiLightMap2[dy][dx];
					break;
				default:
					c+=m_aaiLightMap[dy][dx];
					break;
				}
				if(c>=255) break;
			}

			c>>=3;
			if( c <= m_nAlpha ) c = m_nAlpha;
			if( c>= 31 ) 
			{
				c=31;
				pdw1+=2;
				pdw2+=2;
				pdw3+=2;
				pdw4+=2;
				continue;
			}

			if( c < 2 )
			{
				PutPixelNADark( pdw1, 2);pdw1+=2;
				PutPixelNADark( pdw2, 2);pdw2+=2;
				PutPixelNADark( pdw3, 2);pdw3+=2;
				PutPixelNADark( pdw4, 2);pdw4+=2;
				continue;
			}

			PutPixelDark( pdw1, c, 2,m_dwALPHAMASK1,m_dwALPHAMASK2);pdw1+=2;
			PutPixelDark( pdw2, c, 2,m_dwALPHAMASK1,m_dwALPHAMASK2);pdw2+=2;	
			PutPixelDark( pdw3, c, 2,m_dwALPHAMASK1,m_dwALPHAMASK2);pdw3+=2;	
			PutPixelDark( pdw4, c, 2,m_dwALPHAMASK1,m_dwALPHAMASK2);pdw4+=2;	
		}
	}

	pSurface->Unlock();

	return TRUE;
}
*/


/*
void CNight::SetNightStateEx( int nDayState )
{
	static DWORD dwFrameTime = GetTickCount();
	DWORD dwNowTime = GetTickCount();
	static int nAlpha = 31;

	static int nSun_set = 2;
	static int nPreDayState = 0;
	if( nPreDayState != nDayState )
	{
		switch( nDayState )
		{
		case 1:
			nAlpha = 10;
			nSun_set = 0;
			if( nPreDayState == 0 )
			{
				m_nAlpha = 0;
			}
			break;
		case 2:
			nSun_set = 0;
			nAlpha = 20;
			if( nPreDayState == 0 )
			{
				m_nAlpha = 10;
			}
			break;
		case 3:
			nSun_set = 0;
			nAlpha = 31;
			if( nPreDayState == 0 )
			{
				m_nAlpha = 20;
			}
			break;
		case 4://이값은 들어 오지 않음
			break;
		case 5:
			nSun_set = 1;
			nAlpha = 20;
			if( nPreDayState == 0 )
			{
				m_nAlpha = 31;
			}
			break;
		case 6:
			nSun_set = 1;
			nAlpha = 10;
			if( nPreDayState == 0 )
			{
				m_nAlpha = 20;
			}
			break;
		case 7:
			nSun_set = 1;
			nAlpha = 0;
			if( nPreDayState == 0 )
			{
				m_nAlpha = 10;
			}
			break;
		case 8:
			nSun_set = 1;
			nAlpha = 0;
			if( nPreDayState == 0 )
			{
				m_nAlpha = 0;
			}
			break;
		}

		nPreDayState = nDayState;
	}

	if( dwNowTime - dwFrameTime > 300)//59900 )//10분에 대한 
	{
		dwFrameTime = dwNowTime;

		if( nSun_set == 1 )
		{
			m_nAlpha -= 1;
			if( m_nAlpha <= nAlpha)
			{
				m_nAlpha = nAlpha;
			}

		}
		else if( nSun_set == 0 )
		{
			m_nAlpha += 1;
			if( m_nAlpha >= nAlpha)
			{
				m_nAlpha = nAlpha;
			}
		
		}
	}
}

*/

/*
void CNight::SetNightStateEx( int nDayState )
{
	static DWORD dwFrameTime = GetTickCount();
	DWORD dwNowTime = GetTickCount();
	static int nAlpha = 31;

	static int nSun_set = 2;
	static int nPreDayState = 0;

	if( nPreDayState != nDayState )
	{
		switch( nDayState )
		{
		case 1://일몰
			nAlpha = 0;
			nSun_set = 1;
			if( nPreDayState == 0 )
			{
				m_nAlpha = 31 - m_gap;
			}
			break;
		case 2://밤
			nSun_set = 1;
			nAlpha = 0;
			if( nPreDayState == 0 )
			{
				m_nAlpha = 0;
			}
			break;
		case 3://일출
			nSun_set = 0;
			nAlpha = 31;
			if( nPreDayState == 0 )
			{
				m_nAlpha = m_gap;
			}
			break;
		case 4://낮//이값은 들어 오지 않음
			break;
		}

		nPreDayState = nDayState;
	}

//	if( dwNowTime - dwFrameTime > 300)//10분에 대한 
	if( dwNowTime - dwFrameTime > 18700 )//10분에 대한 
	{
		dwFrameTime = dwNowTime;

		if( nSun_set == 1 )
		{
			m_nAlpha -= 1;
			if( m_nAlpha <= nAlpha)
			{
				m_nAlpha = nAlpha;
			}

		}
		else if( nSun_set == 0 )
		{
			m_nAlpha += 1;
			if( m_nAlpha >= nAlpha)
			{
				m_nAlpha = nAlpha;
			}
		
		}
	}
}
*/
/*
BOOL CNight::SetTransFormNight(int type)
{
	static int pre_type = 1000;
	switch( type )
	{
	case 1:
		m_nNightType	= 1;//일반
		m_bVEnabled		= TRUE;
		break;
	case 12:
		m_nNightType	= 2;//RED
		m_bVEnabled		= TRUE;
		break;
	case 13:
		m_nNightType	= 3;//GREEN
		m_bVEnabled		= TRUE;
		break;
	case 14:
		m_nNightType	= 4;//BLUE
		m_bVEnabled		= TRUE;
		break;
	default:
		m_nNightType	= 0;
		m_bVEnabled		= FALSE;
		break;
	}

	static DWORD dwFrameTime = GetTickCount();
	DWORD dwNowTime = GetTickCount();

	if( m_bVEnabled )
	{
		if( dwNowTime - dwFrameTime > 300 )
		{
			dwFrameTime = dwNowTime;
			if( m_nPreAlpha <= m_nVillageAlpha )
			{
				m_nPreAlpha++;
			}
		}
	}
	else
	{
		if( dwNowTime - dwFrameTime > 300 )
		{
			dwFrameTime = dwNowTime;
			if( m_nPreAlpha <= m_nAlpha )
			{
				m_nPreAlpha++;
			}
			else
			{
				m_nPreAlpha--;
			}
		}
	}

	if( pre_type == type ) return FALSE;
	pre_type = type;
	return TRUE;
	
}
*/