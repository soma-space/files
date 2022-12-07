// DirectXMng.cpp: implementation of the DirectXMng class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Soma.h"
#include "DirectXMng.h"
#include "DManage/BB_Direct.h"
#include "DManage/BBDX_MouseInformation.h"
#include "DManage/AlphaProcess.h"
#include "DManage/DirectSound.h"
#include "DManage/Direct3D.h"
#include "DManage/Night.h"
#include "DxCursor.h"
#include "Item_Field.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CItem_Field Item_Field;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DirectXMng::DirectXMng()
{
	m_pDD			= NULL;
	m_bHalEnabled	= FALSE;
	m_colorTrans	= RGB(0, 0, 0);
	m_pDirectInput	= NULL;
	m_pMouseInfo	= NULL;
	m_hWnd			= NULL;
	m_pAlphaProcess = NULL;
	m_pGammaCtrl	= NULL;
	m_GammaRate		= -25;
	m_nAlphaGamma	= 0;
	m_sizeScreen.cx = 800;
	m_sizeScreen.cy = 600;
//	m_sizeScreen.cx = 640;
//	m_sizeScreen.cy = 480;
	m_Night			= NULL;
	INIT_PTR(m_pDxCursor);
	INIT_PTR(m_pDrawSurface);
	INIT_PTR(m_pDirectSound);
}

DirectXMng::~DirectXMng()
{
	SAFE_DELETE(m_Night);
	ReleaseDirectX();
}

void DirectXMng::SetGammaRate( long lGammaRate )
{
	m_GammaRate = lGammaRate;

	int nGamma = m_GammaRate + m_nAlphaGamma + 50;
	int nRed	= 0;
	int nGreen	= 0;
	int nBlue	= 0;

	DDGAMMARAMP GammaSetting;	

	for(int loop=0; loop<256 ; loop++)
	{
		nRed = m_orgGamma.red[loop] + 255*nGamma; 
		if( nRed >= 0xffff ) nRed = 0xffff;
		if( nRed <= 0x00 ) nRed = 0x00;

		GammaSetting.red[loop] = nRed;

		nGreen = m_orgGamma.green[loop] + 255*nGamma; 
		if( nGreen >= 0xffff ) nGreen = 0xffff;
		if( nGreen <= 0x00 ) nGreen = 0x00;

		GammaSetting.green[loop] = nGreen;

		nBlue = m_orgGamma.blue[loop] + 255*nGamma; 
		if( nBlue >= 0xffff ) nBlue = 0xffff;
		if( nBlue <= 0x00 ) nBlue = 0x00;

		GammaSetting.blue[loop] = nBlue;
	}
	
	m_pGammaCtrl->SetGammaRamp( 0, &GammaSetting );

}

void DirectXMng::SetInHouseGammaRate( )
{
	int nGamma = m_GammaRate;
	int nRed	= 0;
	int nGreen	= 0;
	int nBlue	= 0;

	DDGAMMARAMP GammaSetting;	

	for(int loop=0; loop<256 ; loop++)
	{
		nRed = m_orgGamma.red[loop] + 255*nGamma; 
		if( nRed >= 0xffff ) nRed = 0xffff;
		if( nRed <= 0x00 ) nRed = 0x00;

		GammaSetting.red[loop] = nRed;

		nGreen = m_orgGamma.green[loop] + 255*nGamma; 
		if( nGreen >= 0xffff ) nGreen = 0xffff;
		if( nGreen <= 0x00 ) nGreen = 0x00;

		GammaSetting.green[loop] = nGreen;

		nBlue = m_orgGamma.blue[loop] + 255*nGamma; 
		if( nBlue >= 0xffff ) nBlue = 0xffff;
		if( nBlue <= 0x00 ) nBlue = 0x00;

		GammaSetting.blue[loop] = nBlue;
	}
	
	m_pGammaCtrl->SetGammaRamp( 0, &GammaSetting );

}

void DirectXMng::SetRainGammaRate( int nAlphaGamma )
{
	m_nAlphaGamma = nAlphaGamma;
	int nGamma = m_nAlphaGamma + m_GammaRate + 50;
	TRACE("GAMMA %d\n", nGamma);
	int nRed	= 0;
	int nGreen	= 0;
	int nBlue	= 0;

	DDGAMMARAMP GammaSetting;	

	for(int loop=0; loop<256 ; loop++)
	{
		nRed = m_orgGamma.red[loop] + 255*nGamma; 
		if( nRed >= 0xffff ) nRed = 0xffff;
		if( nRed <= 0x00 ) nRed = 0x00;

		GammaSetting.red[loop] = nRed;

		nGreen = m_orgGamma.green[loop] + 255*nGamma; 
		if( nGreen >= 0xffff ) nGreen = 0xffff;
		if( nGreen <= 0x00 ) nGreen = 0x00;

		GammaSetting.green[loop] = nGreen;

		nBlue = m_orgGamma.blue[loop] + 255*nGamma; 
		if( nBlue >= 0xffff ) nBlue = 0xffff;
		if( nBlue <= 0x00 ) nBlue = 0x00;

		GammaSetting.blue[loop] = nBlue;
	}
	
	m_pGammaCtrl->SetGammaRamp( 0, &GammaSetting );

}

void DirectXMng::GetVGAName()
{
	DDDEVICEIDENTIFIER2 ddde2;
	m_pDD->GetInterface()->GetDeviceIdentifier(&ddde2,0); 
	m_VGAcardName = ddde2.szDescription;
	if( m_VGAcardName == "NVidia RIVA 128" )
		m_VGAType = 1;
	else if( m_VGAcardName == "Magnum/Xpert 128/Xpert 99 (한글)" )
		m_VGAType = 2;
	else if( m_VGAcardName == "Voodoo3 AGP" )
		m_VGAType = 3;
	else m_VGAType = 0;
}

int DirectXMng::InitMask()
{
	DWORD dwRBitMask,dwGBitMask,dwBBitMask,dwRBBitMask;
	m_bpBackSurf->GetRGBMasks(dwRBitMask, dwGBitMask, dwBBitMask);

	int nPixelFormat = 0;
	if (dwRBitMask == 0x7c00) // RGB_555
	{
		TRACE("PixelFormat : PF_555\n");
		dwRBBitMask = 0x00007c1f;

		m_dwHalf16Mask = 0x0000001e;
		m_dwHalf16Mask <<= 5;
		m_dwHalf16Mask |= 0x0000001e;
		m_dwHalf16Mask <<= 5;
		m_dwHalf16Mask |= 0x0000001e;

		m_dwHalf32Mask = (m_dwHalf16Mask<<16) | m_dwHalf16Mask;

		ASSERT(m_dwHalf16Mask == 0x00007bde);
	
		nPixelFormat = PF_555;
	}
	else// if (CUser::m_dwRBitMask == 0xf800) //565 // 1111 1000 0000 0000 
	{
		ASSERT(dwRBitMask == 0xf800); //RGB_565
												// BGR_565에서도 가능할 듯(test 불가)
		//  0000 0111 1110 0000  --->  0000 0111 1100 0000
		dwGBitMask = 0x07c0;
		TRACE("PixelFormat : PF_565\n");
		dwRBBitMask = 0x0000f81f;


		m_dwHalf16Mask = 0x0000001e;
		m_dwHalf16Mask <<= 5;
		m_dwHalf16Mask |= 0x0000001e;
		m_dwHalf16Mask <<= 6;
		m_dwHalf16Mask |= 0x0000001e;

		m_dwHalf32Mask = (m_dwHalf16Mask<<16) | m_dwHalf16Mask;

		nPixelFormat = PF_565;
	}

	return nPixelFormat;

}

//Return False일 경우 프로그램 종료
BOOL DirectXMng::InitDirectX(int width, int height, int color_depth, BOOL bFullScreen)
{
	m_sizeScreen.cx = width;
	m_sizeScreen.cy = height;

	ASSERT(m_hWnd);
	m_bFullScreen = bFullScreen;
	m_pDD = new CDirectDraw;
	if (!m_pDD->Create()) return FALSE;	//DirectDraw 생성 실패하면 프로그램 종료

	if (bFullScreen) //풀화면으로 설정할 경우
	{
		if (!m_pDD->SetFullScreenMode(AfxGetMainWnd()->GetSafeHwnd(),
			    width,
			    height, 
				color_depth)) return FALSE;
//soma_3dxx
		m_pDrawSurface = new CDDSurface;
		if (!m_pDrawSurface->Create(m_pDD, width, height, FALSE))	//비디오 메모리를 사용하지 않는 surface생성(그리기용)
			return FALSE;

		// 임시 속도 문제 때문에 비디오 메모리에다가 생성을 한번 해보려고...
//		if (!m_pDrawSurface->Create(m_pDD, width, height, FALSE))	//비디오 메모리를 사용하지 않는 surface생성(그리기용)
//			return FALSE;

	}
	else
	{
		m_pDrawSurface = new CDDSurface;
		if (!m_pDD->SetWindowedMode(AfxGetMainWnd()->GetSafeHwnd(), width, height)) return FALSE;
		if (!m_pDrawSurface->Create(m_pDD, width, height, FALSE))	//비디오 메모리를 사용하지 않는 surface생성(그리기용)
			return FALSE;
	}
	
	m_bpFrontSurf = m_pDD->GetFrontBuffer();
	m_bpBackSurf = m_pDD->GetBackBuffer();

	m_bpFrontSurf->SetColorKey(m_colorTrans);	//TransParent로 처리할 색 설정
	m_bpBackSurf->SetColorKey(m_colorTrans);

//	m_pDrawSurface = m_pDD->GetBackBuffer(); 

	//DirectInput 생성시의 Window Handle은 반드시 MainWindow의 Handle이어야 한다.
	m_pDirectInput = new CDirectInput(AfxGetMainWnd()->GetSafeHwnd());
	if (!m_pDirectInput->Create(AfxGetInstanceHandle()))  
	{
		SAFE_DELETE(m_pDirectInput);
	}
	else if (!m_pDirectInput->MouseInputCreate(!bFullScreen)) 
	{
		SAFE_DELETE(m_pDirectInput);
	}

	m_pMouseInfo = new BBDX_MouseInformation;
	m_pMouseInfo->SetHwndAndWindowed(m_hWnd, !bFullScreen);

	m_bpFrontSurf->SurfaceFill(RGB(0, 0, 0));	//초기 까맣게..

//	m_pAlphaProcess = new CAlphaProcess(m_pDD);
	m_p3D = new CDirect3D;
	if( !m_p3D->Initialize3D(m_pDD, AfxGetMainWnd()->GetSafeHwnd(),NULL,&IID_IDirect3DHALDevice) )//dspring_0726_3D
	{
		TRACE("HAL로 3D를 사용할수가 없습니다.\n");
		m_p3D->CleanUp3DEnvironment(); 
		if( !m_p3D->Initialize3D(m_pDD, AfxGetMainWnd()->GetSafeHwnd(),NULL,&IID_IDirect3DMMXDevice) )//dspring_0726_3D
		{
			TRACE("MMX로 3D를 사용할수가 없습니다.\n");
			m_p3D->CleanUp3DEnvironment(); 

			if( !m_p3D->Initialize3D(m_pDD, AfxGetMainWnd()->GetSafeHwnd(),NULL,&IID_IDirect3DRGBDevice) )//dspring_0726_3D
			{
				TRACE("HEL로 3D를 사용할수가 없습니다.\n");
				m_p3D->CleanUp3DEnvironment(); 
			}
			else
				TRACE("HEL로 3D를 초기화했습니다..\n");

		}
		else
			TRACE("MMX로 3D를 초기화했습니다..\n");
	}
	else
	{
		m_bHalEnabled = TRUE;
		TRACE("HAL로 3D를 초기화했습니다..\n");
	}
	m_bHalEnabled = FALSE;
	int nPixelFormat = InitMask();
	m_p3D->D3DAlpha( m_bHalEnabled ); 
	Item_Field.SetPixelFormat( nPixelFormat );

	if (FAILED(m_bpFrontSurf->GetInterface()->QueryInterface(IID_IDirectDrawGammaControl, (void**)&m_pGammaCtrl))) //감마 컨트롤 생성
	{
		m_pGammaCtrl = NULL;
	}
	else
	{
		int nGamma = 70;
		m_pGammaCtrl->GetGammaRamp(0, &m_orgGamma);

		int nRed	= 0;
		int nGreen	= 0;
		int nBlue	= 0;

		for(int loop=0; loop<256 ; loop++)
		{
			nRed = m_orgGamma.red[loop] + 255*nGamma; 
			if( nRed >= 0xffff ) nRed = 0xffff;
			if( nRed <= 0x00 ) nRed = 0x00;

			m_GammaLightning.red[loop] = nRed;

			nGreen = m_orgGamma.green[loop] + 255*nGamma; 
			if( nGreen >= 0xffff ) nGreen = 0xffff;
			if( nGreen <= 0x00 ) nGreen = 0x00;

			m_GammaLightning.green[loop] = nGreen;

			nBlue = m_orgGamma.blue[loop] + 255*nGamma; 
			if( nBlue >= 0xffff ) nBlue = 0xffff;
			if( nBlue <= 0x00 ) nBlue = 0x00;

			m_GammaLightning.blue[loop] = nBlue;
		}
		
	}
	
	GetVGAName();
	m_p3D->SetVAGType( m_VGAType ); 
	
//	if( m_bHalEnabled )
//		m_Night = new CNight(10,10,799.0,495.0, m_p3D->GetDevice(), m_VGAType );
		m_Night = new CNight(nPixelFormat);

	//Cursor 만들기
	//m_pDxCursor = new CDxCursor();
	//m_pDxCursor->InitCursor();
//	m_pDxCursor->AddCursor("NRes\\cursor.bmp");	//커서 여러개 등록가능
	//m_pDxCursor->SetCursorIndex(0);

	m_pDirectSound = new CDirectSound;			//DirectSound 생성 (주의 : PCM포맷만 지원함)

	if (FALSE == m_pDirectSound->InitDirectSound(m_hWnd, bFullScreen)) 
	{
//		SAFE_DELETE(m_pDirectSound);//dspring_0721 CDirectSound내부적으로 안전장치를 해주었기 때문에 실패시 지워줄 필요가 없습니다....
	}
	else
	{
		//주의: PCM 방식의 wave만 재생가능

		//Test용 Sample code
/*		char *fnames[10] =
		{
			"Everland.wav",
			"CHIMES.WAV",
			"CHORD.WAV",
			"DING.WAV",
			"HOVERSEL.WAV",
			"LOGOFF.WAV",
			"NOTIFY.WAV",
			"RECYCLE.WAV",
			"TADA.WAV",
			"Notify.wav"
		};
		CString filename;
		CString path = "C:\\Windows\\Media\\";
		for (int i=0; i<10; i++)
		{
			filename = path;
			filename += fnames[i];
			m_pDirectSound->SoundPlay(filename, FALSE);
		}*/
	}

	return TRUE;
}

BOOL DirectXMng::ReleaseDirectX()
{
	if (m_pGammaCtrl) 
	{
		m_pGammaCtrl->SetGammaRamp(0, &m_orgGamma);
		SAFE_RELEASE(m_pGammaCtrl);
	}
	SAFE_DELETE(m_pDirectSound);
	SAFE_DELETE(m_pAlphaProcess);
	SAFE_DELETE(m_pMouseInfo);
	SAFE_DELETE(m_pDirectInput);
	SAFE_DELETE(m_pDxCursor);
	SAFE_DELETE(m_pDrawSurface);
	SAFE_DELETE(m_p3D);
	//if (m_bFullScreen) SAFE_DELETE(m_pDrawSurface);
	SAFE_DELETE(m_pDD);
	return TRUE;
}
//그리기 Surface복구
void DirectXMng::RestoreSurface()
{
	CDDSurface *pSurfs[2];
	pSurfs[0] = m_bpFrontSurf;
	pSurfs[1] = m_bpBackSurf;
	for (int i=0; i<2; i++)
	{
		if (pSurfs[i]->GetInterface()->IsLost()!=DD_OK)
		{
			pSurfs[i]->GetInterface()->Restore();
		}
	}
}

BOOL DirectXMng::MouseInfoUpdate()
{
	if (m_pDirectInput) 
	{
		m_pDirectInput->MouseInputCheck(m_pMouseInfo);
		return TRUE;
	}
	return FALSE;
}

void DirectXMng::Flip()
{
	DrawCursor();	

	CPoint offset(0, 0);
	CRect sr(offset, m_sizeScreen);
	if (m_bFullScreen) //Back Buffer에 그린후 Flip함
	{
		m_bpBackSurf->Blt(&sr, m_pDrawSurface, &sr, DDBLT_ASYNC);
		m_bpFrontSurf->Flip(0L);
	}
	else
	{	//그냥 Front에 그림
		MapWindowPoints(GetParent(m_hWnd), NULL, &offset, 1);
		CRect r(offset, m_sizeScreen);
		m_bpFrontSurf->Blt(&r, m_pDrawSurface, &sr, DDBLT_WAIT);
	}
}

void DirectXMng::DrawCursor()
{
	CPoint ptCursorPos = m_pMouseInfo->m_posMouse;
	//Cursor는 항상 DrawSurface에 그린다.
	if (m_pDxCursor) m_pDxCursor->Draw(m_pDrawSurface, ptCursorPos);
}

//현재 마우스의 위치
CPoint&	DirectXMng::GetMousePos()
{
	static CPoint pos(320, 240);
	if (m_pMouseInfo) return m_pMouseInfo->m_posMouse;
	return pos;
}
