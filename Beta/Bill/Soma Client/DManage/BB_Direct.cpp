// BB_Direct.cpp
//
// Copyright (c) Nigel Thompson 1996
//
// Implementation for:
// CDirectDraw
//

#include "stdafx.h"
#include "BB_Direct.h"
#include "BBDX_MouseInformation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define WM_MESSAGE_RESTOREALL	(WM_USER+2)

//#define dALPHA_MASK1 0x7c1f03e0
//#define dALPHA_MASK2 0x03e07c1f

#define dALPHA_MASK1 0xf81f07e0
#define dALPHA_MASK2 0x07e0f81f

const int PF_555 = 555;
const int PF_565 = 565;

int data=0;
int DataView[600][400];

#define CLEAR(exp) memset(&exp, 0, sizeof(exp))


// 재덕...
WORD RGB2(COLORREF color)
{
	int PixelFormat = PF_555;// 임시..나중에 바꿔줘야한다..*******************************************꼭...문제가 큼ㅂ

	if(PixelFormat == PF_555) // (5bit, 5bit, 5bit)
	{
		return (WORD)((((color&0x000000ff)*31/255)<<10)+((((color&0x0000ff00)>>8)*31/255)<<5)+(((color & 0x00ff0000) >> 16) * 31 / 255));
	}
	else if(PixelFormat == PF_565)
	{
		return (WORD)((((color&0x000000ff)*31/255)<<11)+((((color&0x0000ff00)>>8)*63/255)<<5)+(((color & 0x00ff0000) >> 16) * 31 / 255));
	}
	return 0;
}

//for FogLayer (FOR : 16bit mode color )
WORD HIRGB(WORD r, WORD g, WORD b, BOOL bIs555)
{
	b >>= 3;
	g >>= 2+ (bIs555 ? 1:0);
	g <<= 5;
	r >>= 3;
	r <<= 10+ (bIs555 ? 0:1);
	
	return (b|g|r);
}

BYTE GetHIRValue(WORD rgb, BOOL bIs555)
{
	if (bIs555)
	{
		rgb &= 0x7c00;
		rgb <<= 1;
	}
	else 
	{
		rgb &= 0xf800;
	}
	return HIBYTE(rgb);
}

BYTE GetHIGValue(WORD rgb, BOOL bIs555)
{
	if (bIs555)
	{
		rgb &= 0x03e0;
		rgb >>= 2;
	}
	else
	{
		rgb &= 0x07e0;
		rgb >>= 3;
	}
	return LOBYTE(rgb);
}

BYTE GetHIBValue(WORD rgb, BOOL bIs555)
{
	rgb &= 0x001f;
	rgb <<= 3;
	return LOBYTE(rgb);
}


//////////////////////////////////////////////////////////////////
// CDDObject

IMPLEMENT_DYNAMIC(CDDObject, CObject)

CDDObject::CDDObject()
{
	m_hr = 0;
}

CDDObject::~CDDObject()
{
}

//////////////////////////////////////////////////////////////////
// CDDEngine

IMPLEMENT_DYNAMIC(CDDEngine, CDDObject)

CDDEngine::CDDEngine()
{
}

CDDEngine::~CDDEngine()
{
}

// Device enumeration data structure
typedef struct _EnumDevInfo {
	int iDeviceCount;
	int iDevice;
	CString strName;
	CString strDesc;
} EnumDevInfo;

// callback function for device enumeration
static BOOL FAR PASCAL EnumDevFn(GUID FAR * pGuid,
								 LPSTR pszDriver,
								 LPSTR pszDesc,
								 LPVOID pArg)
{
	EnumDevInfo* pInfo = (EnumDevInfo*) pArg;
	ASSERT(pInfo);
	
	// see if it's the one wanted
	if (pInfo->iDevice == pInfo->iDeviceCount) {
		pInfo->strName = pszDriver;
		pInfo->strDesc = pszDesc;
		return DDENUMRET_CANCEL; // stop enumerating
	}

	pInfo->iDeviceCount++;
	return DDENUMRET_OK;
}

// Get the number of Direct draw devices
int CDDEngine::GetNumDevices()
{
	EnumDevInfo di;
	di.iDeviceCount = 0;
	di.iDevice = -1;
	m_hr = ::DirectDrawEnumerate(EnumDevFn, &di);
	ASSERT(m_hr==DD_OK);
	return di.iDeviceCount;
}

// Get info for a given device
BOOL CDDEngine::GetDeviceInfo(int iDev,
								CString& strName,
								CString& strDesc)
{
	int iNum = GetNumDevices();
	if (iDev >= iNum) return FALSE;

	EnumDevInfo di;
	di.iDeviceCount = 0;
	di.iDevice = iDev;
	m_hr = ::DirectDrawEnumerate(EnumDevFn, &di);
	ASSERT(m_hr==DD_OK);
	strName = di.strName;
	strDesc = di.strDesc;
	return TRUE;
}

HRESULT CDDEngine::CreateDirectDraw(IDirectDraw7** ppIDD)
{
	ASSERT(ppIDD);
	HRESULT hRet;
	hRet = DirectDrawCreateEx(NULL, (void **)&ppIDD, IID_IDirectDraw7, NULL);
	return hRet;
}

// the one and only DDEngine
//CDDEngine theDDEngine;

//////////////////////////////////////////////////////////////////
// CDirectDraw

IMPLEMENT_DYNAMIC(CDirectDraw, CDDObject)

CDirectDraw::CDirectDraw()
{
	m_pFrontBuffer = NULL;
	m_pBackBuffer = NULL;
	m_pClipper = NULL;
	m_pPalette = NULL;
	m_bSWRender = FALSE;
	m_pIDD = NULL;
	m_bRestore = FALSE;

	for(int i =0; i< 600; i++)
		for(int j=0;j<400;j++)
			DataView[i][j]=data++;// 240, 320
}

CDirectDraw::~CDirectDraw()
{
	// free all the buffers
	_ReleaseAll();

	// release the interface
	if (m_pIDD) {
		m_pIDD->Release();
		m_pIDD = NULL;
	}
}

void CDirectDraw::_ReleaseAll()
{
	if (m_bRestore) {
		RestoreMode();
	}
	if (m_pBackBuffer) {
		delete m_pBackBuffer;
		m_pBackBuffer = NULL;
	}
	if (m_pFrontBuffer) {
		delete m_pFrontBuffer;
		m_pFrontBuffer = NULL;
	}
	if (m_pClipper) {
		delete m_pClipper;
		m_pClipper = NULL;
	}
	if (m_pPalette) {
		delete m_pPalette;
		m_pPalette = NULL;
	}
	m_bSWRender = FALSE;
}

BOOL CDirectDraw::Create()
{
	_ReleaseAll();

	m_hr = DirectDrawCreateEx(NULL, (void **)&m_pIDD, IID_IDirectDraw7, NULL);
	if(FAILED(m_hr)) return FALSE;
	return TRUE;

}

BOOL CDirectDraw::GetCaps(DDCAPS* pDrvCaps, DDCAPS* pHelCaps)
{
	ASSERT(m_pIDD);
	if (pDrvCaps) {
		memset(pDrvCaps, 0, sizeof(DDCAPS));
		pDrvCaps->dwSize = sizeof(DDCAPS);
	}
	if (pHelCaps) {
		memset(pHelCaps, 0, sizeof(DDCAPS));
		pHelCaps->dwSize = sizeof(DDCAPS);
	}
	m_hr = m_pIDD->GetCaps(pDrvCaps, pHelCaps);
	return m_hr==DD_OK;
}

// Mode enumeration info structure
typedef struct _EnumModeInfo {
	int iModeCount;
	int iMode;
	DDSURFACEDESC2 ddSurf;
} EnumModeInfo;

// Callback function for device mode enumeration
static HRESULT FAR PASCAL EnumModesFn(LPDDSURFACEDESC2 psd,
						              LPVOID pArg)

{
	EnumModeInfo* pInfo = (EnumModeInfo*) pArg;
	ASSERT(pInfo);
	
	// See if it's the correct mode
	if (pInfo->iMode == pInfo->iModeCount) {
		pInfo->ddSurf = *psd;
		return DDENUMRET_CANCEL; // Stop enumerating
	}

	pInfo->iModeCount++;
	return DDENUMRET_OK;
}

// Get number of supported display modes
int CDirectDraw::GetNumModes()
{
	ASSERT(m_pIDD);
	EnumModeInfo mi;
	mi.iModeCount = 0;
	mi.iMode = -1;
	m_hr = m_pIDD->EnumDisplayModes(0, NULL, &mi,
		                            EnumModesFn);
	ASSERT(m_hr==DD_OK);
	return mi.iModeCount;
}

// Get data for a given mode
BOOL CDirectDraw::GetModeInfo(int iMode,
							  DDSURFACEDESC2* pDesc)
{
	int iModes =  GetNumModes();
	if (iMode >= iModes) return FALSE;
	ASSERT(m_pIDD);
	EnumModeInfo mi;
	mi.iModeCount = 0;
	mi.iMode = iMode;
	m_hr = m_pIDD->EnumDisplayModes(0, NULL, &mi,
		                            EnumModesFn);
	ASSERT(m_hr==DD_OK);
	*pDesc = mi.ddSurf; 
	return TRUE;
}

BOOL CDirectDraw::SetCooperativeLevel(HWND hWnd, DWORD dwFlags)
{
	ASSERT(m_pIDD);
	ASSERT(hWnd);
	m_hr = m_pIDD->SetCooperativeLevel(hWnd, dwFlags);
	return m_hr==DD_OK;
}

// Set a given mode. If hWnd is NULL, we try to go full screen
BOOL CDirectDraw::_SetMode(HWND hWnd, int cx, int cy,
						   int bpp, BOOL bFullScreen)
{
	ASSERT(m_pIDD);

	// Release any existing buffers
	_ReleaseAll();

	// Set cooperative level
	if (bFullScreen) {
		if (!SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | /*DDSCL_ALLOWMODEX |*/ DDSCL_ALLOWREBOOT |//dspring_0726_3d
			                     DDSCL_NOWINDOWCHANGES | DDSCL_FULLSCREEN)) {
			return FALSE;
		}
		m_hr = m_pIDD->SetDisplayMode(cx, cy, bpp, 0, 0);
		if (m_hr!=DD_OK) {
			return FALSE;
		}
		m_bRestore = TRUE;
	} else {
		if (!SetCooperativeLevel(hWnd, DDSCL_NORMAL)) {
			return FALSE;
		}
	}

	// Create front and back buffer surfaces

	m_iWidth = cx;
	m_iHeight = cy;
	
	DDSURFACEDESC2 sd;
	memset(&sd, 0, sizeof(sd));
	sd.dwSize = sizeof(sd);

	if (bFullScreen)
	{
//soma_0726
		// Create a complex flipping surface with front
		// and back buffers
/*		sd.dwFlags = DDSD_CAPS
				   | DDSD_BACKBUFFERCOUNT;
		sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE
						  | DDSCAPS_FLIP
						  | DDSCAPS_COMPLEX
						  | DDSCAPS_VIDEOMEMORY;
						  //| DDSCAPS_SYSTEMMEMORY;
		sd.dwBackBufferCount = 1;
    */
		sd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE | DDSCAPS_FLIP 
							| DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY;
		sd.dwBackBufferCount = 1;
//dspring_0726_3d
		// Create front and back buffer surfaces
		m_pFrontBuffer = new CDDSurface;
		if (!m_pFrontBuffer->Create(this, &sd)) {
			TRACE("비디오메모리 생성실패\n");
			sd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
			if (!m_pFrontBuffer->Create(this, &sd)) return FALSE;
		}
		ASSERT(m_pFrontBuffer);

		// Get pointer to attached back buffer
		DDSCAPS2 caps;
		memset(&caps, 0, sizeof(caps));
		caps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY; // | DDSCAPS_BACKBUFFER//dspring_0726_3d
		m_pBackBuffer = m_pFrontBuffer->GetAttachedSurface(&caps);

//		sd.ddsCaps.dwCaps = DDSCAPS_FLIP
//						  | DDSCAPS_COMPLEX
//						  | DDSCAPS_VIDEOMEMORY | DDSCAPS_BACKBUFFER;
//		m_pBackBuffer = m_pFrontBuffer->GetAttachedSurface(&sd.ddsCaps);
		if (!m_pBackBuffer) 
		{
			delete m_pFrontBuffer;
			m_pFrontBuffer = NULL;
			return FALSE;
		}

	} 
	else 
	{		// Windowed

		// Create two surfaces for windowed case,
		// a primary surface to share with GDI and
		// a back buffer to render into

		// Create the front buffer surface.
		// Note: Because the front buffer is the primary
		// (existing) surface, we don't specify height and
		// width.

		sd.dwFlags = DDSD_CAPS;
		sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
		m_pFrontBuffer = new CDDSurface;
		if (!m_pFrontBuffer->Create(this, &sd))
		{
			return FALSE;
		}

		// Create back buffer surface
		sd.dwFlags = DDSD_WIDTH 
				   | DDSD_HEIGHT
				   | DDSD_CAPS;
		sd.dwWidth = cx;
		sd.dwHeight = cy;

		// 메닉스 컴퓨터에서는 시스템 메모리로 ㅎ\안하면..문제가 생기네..위녿우 모드가 동작안함..
		// 내컴으로는 비디오메모리로 올려도 아무런 문제가 없음..
//		sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;
		m_pBackBuffer = new CDDSurface;
		if (!m_pBackBuffer->Create(this, &sd))
		{
			delete m_pFrontBuffer;
			m_pFrontBuffer = NULL;
			return FALSE;

		}

		// Create a clipper object for front buffer
		// so drawing is clipped to the window
		m_pClipper = new CDDClipper;
		if (!m_pClipper->Create(this, hWnd)) {
			return FALSE;
		}

		// Attach clipper to front buffer
		if (!m_pFrontBuffer->SetClipper(m_pClipper)) {
			return FALSE;
		}

	}//else

	// See if the back buffer is in video memory and if not
	// set the software-only rendering flag so we won't try to use
	// a hardware renderer on it.
	m_pBackBuffer->GetDescription(&sd);
	if (!(sd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY))	{
		m_bSWRender = TRUE;
	}

	// Create and setup a palette if we are less than 16 bpp
	// find the pixel format needed to access a locked buffer
#if 0
	memset(&sd, 0, sizeof(sd));
	sd.dwSize = sizeof(sd);
	m_hr = m_pBackBuffer->GetInterface()->Lock(NULL, &sd, DDLOCK_WAIT, NULL);
	if (m_hr!=DD_OK) return FALSE;
	m_hr = m_pBackBuffer->GetInterface()->Unlock(NULL);
#else
	m_pBackBuffer->GetDescription(&sd);
#endif
//	int i;
	if (sd.ddpfPixelFormat.dwRGBBitCount < 16) {

		// yes, we need a palette
		// get the current hardware palette
		PALETTEENTRY pe[256];
		HDC hdc = ::GetDC(NULL);
		::GetSystemPaletteEntries(hdc, 0, 256, pe);
		::ReleaseDC(NULL, hdc);

//		ZeroMemory(pe, sizeof(PALETTEENTRY)*256);

		m_pPalette = new CDDPalette;

		if (!m_pPalette->Create(this,
						   DDPCAPS_8BIT | DDPCAPS_INITIALIZE,
						   pe)) {
			return FALSE;
		}

		// Attach the palette to the buffers
		if (!m_pFrontBuffer->SetPalette(m_pPalette)) return FALSE;
		if (!m_pBackBuffer->SetPalette(m_pPalette)) return FALSE;

	}

	return TRUE;
}
void CDirectDraw::RestoreAllSurfaces()
{
	if( m_pIDD )
		m_pIDD->RestoreAllSurfaces();
}
void CDirectDraw::RestoreMode()
{
	if (m_bRestore) {
		ASSERT(m_pIDD);
		m_hr = m_pIDD->RestoreDisplayMode();
		ASSERT(m_hr==DD_OK);
		m_bRestore = FALSE;
	}
}

void CDirectDraw::OnActivate(BOOL bActive)
{
	if (bActive) {
		// the window is being activated
		// if we are using a palette, restore it to the
		// front buffer
		if (m_pPalette && m_pFrontBuffer) {
			m_pFrontBuffer->SetPalette(m_pPalette);
		}
	}
}
//실행디렉토리 가져오기 (Application의) ->실행path가 자주 필요하기 땜시 static으로 이곳에 묶어둠
CString	CDirectDraw::GetExecutionPath(BOOL bWithBkSlash)
{
	char exeName[50];
	CString exename = AfxGetApp()->m_pszExeName;
	strcpy(exeName, exename);
	strcat(exeName, ".EXE");
	char path[_MAX_PATH];
	CString tmpStr;
	if (!GetModuleFileName(GetModuleHandle(exeName), path, _MAX_PATH))
	{
		path[0] = 0;
	}
	else
	{
		tmpStr = path;
		int rfv = tmpStr.ReverseFind('\\');
		path[rfv]=0;
		if (bWithBkSlash) strcat(path, "\\");
	}

	tmpStr = path;
	return tmpStr;
}

BOOL CDirectDraw::ErrorHandle(HRESULT hr)
{
	PCHAR msg;
	switch(hr)
	{
	case DDERR_ALREADYINITIALIZED :
		msg = "The object has already been initialized. \n";
		break;
	case DDERR_BLTFASTCANTCLIP :
		msg = "A DirectDrawClipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface2::BltFast method. \n";
		break;
	case DDERR_CANNOTATTACHSURFACE :
		msg = "A surface cannot be attached to another requested surface. \n";
		break;
	case DDERR_CANNOTDETACHSURFACE :
		msg = "A surface cannot be detached from another requested surface. \n";
		break;
	case DDERR_CANTCREATEDC :
		msg = "Windows cannot create any more device contexts (DCs). \n";
		break;
	case DDERR_CANTDUPLICATE :
		msg = "Primary and 3D surfaces, or surfaces that are implicitly created, cannot be duplicated.  \n";
		break;
	case DDERR_CANTLOCKSURFACE :
		msg = "Access to this surface is refused because an attempt was made to lock the primary surface without DCI support.  \n";
		break;
	case DDERR_CANTPAGELOCK :
		msg = "An attempt to page lock a surface failed. Page lock will not work on a display-memory surface or an emulated primary surface.  \n";
		break;
	case DDERR_CANTPAGEUNLOCK :
		msg = "An attempt to page unlock a surface failed. Page unlock will not work on a display-memory surface or an emulated primary surface.  \n";
		break;
	case DDERR_CLIPPERISUSINGHWND :
		msg = "An attempt was made to set a clip list for a DirectDrawClipper object that is already monitoring a window handle.  \n";
		break;
	case DDERR_COLORKEYNOTSET :
		msg = "No source color key is specified for this operation.  \n";
		break;
	case DDERR_CURRENTLYNOTAVAIL :
		msg = "No support is currently available.  \n";
		break;
	case DDERR_DCALREADYCREATED :
		msg = "A device context (DC) has already been returned for this surface. Only one DC can be retrieved for each surface.  \n";
		break;
	case DDERR_DIRECTDRAWALREADYCREATED :
		msg = "A DirectDraw object representing this driver has already been created for this process.  \n";
		break;
	case DDERR_EXCEPTION :
		msg = "An exception was encountered while performing the requested operation.  \n";
		break;
	case DDERR_EXCLUSIVEMODEALREADYSET :
		msg = "An attempt was made to set the cooperative level when it was already set to exclusive.  \n";
		break;
	case DDERR_GENERIC :
		msg = "There is an undefined error condition.  \n";
		break;
	case DDERR_HEIGHTALIGN :
		msg = "The height of the provided rectangle is not a multiple of the required alignment.  \n";
		break;
	case DDERR_HWNDALREADYSET :
		msg = "The DirectDraw cooperative level window handle has already been set. It cannot be reset while the process has surfaces or palettes created.  \n";
		break;
	case DDERR_HWNDSUBCLASSED :
		msg = "DirectDraw is prevented from restoring state because the DirectDraw cooperative level window handle has been subclassed.  \n";
		break;
	case DDERR_IMPLICITLYCREATED :
		msg = "The surface cannot be restored because it is an implicitly created surface.  \n";
		break;
	case DDERR_INCOMPATIBLEPRIMARY :
		msg = "The primary surface creation request does not match with the existing primary surface.  \n";
		break;
	case DDERR_INVALIDCAPS :
		msg = "One or more of the capability bits passed to the callback function are incorrect.  \n";
		break;
	case DDERR_INVALIDCLIPLIST :
		msg = "DirectDraw does not support the provided clip list.  \n";
		break;
	case DDERR_INVALIDDIRECTDRAWGUID :
		msg = "The globally unique identifier (GUID) passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier. \n";
		break;
	case DDERR_INVALIDMODE :
		msg = "DirectDraw does not support the requested mode. \n";
		break;
	case DDERR_INVALIDOBJECT :
		msg = "DirectDraw received a pointer that was an invalid DirectDraw object. \n";
		break;
	case DDERR_INVALIDPARAMS :
		msg = "One or more of the parameters passed to the method are incorrect. \n";
		break;
	case DDERR_INVALIDPIXELFORMAT :
		msg = "The pixel format was invalid as specified. \n";
		break;
	case DDERR_INVALIDPOSITION :
		msg = "The position of the overlay on the destination is no longer legal. \n";
		break;
	case DDERR_INVALIDRECT :
		//msg = "The provided rectangle was invalid. \n";
		msg = "";
		break;
	case DDERR_INVALIDSURFACETYPE :
		msg = "The requested operation could not be performed because the surface was of the wrong type. \n";
		break;
	case DDERR_LOCKEDSURFACES :
		msg = "One or more surfaces are locked, causing the failure of the requested operation. \n";
		break;
	case DDERR_NO3D :
		msg = "No 3D hardware or emulation is present. \n";
		break;
	case DDERR_NOALPHAHW :
		msg = "No alpha acceleration hardware is present or available, causing the failure of the requested operation. \n";
		break;
	case DDERR_NOBLTHW :
		msg = "No blitter hardware is present. \n";
		break;
	case DDERR_NOCLIPLIST :
		msg = "No clip list is available. \n";
		break;
	case DDERR_NOCLIPPERATTACHED :
		msg = "No DirectDrawClipper object is attached to the surface object. \n";
		break;
	case DDERR_NOCOLORCONVHW :
		msg = "The operation cannot be carried out because no color-conversion hardware is present or available. \n";
		break;
	case DDERR_NOCOLORKEY :
		msg = "The surface does not currently have a color key. \n";
		break;
	case DDERR_NOCOLORKEYHW :
		msg = "The operation cannot be carried out because there is no hardware support for the destination color key. \n";
		break;
	case DDERR_NOCOOPERATIVELEVELSET :
		msg = "A create function is called without the IDirectDraw2::SetCooperativeLevel method being called. \n";
		break;
	case DDERR_NODC :
		msg = "No DC has ever been created for this surface. \n";
		break;
	case DDERR_NODDROPSHW :
		msg = "No DirectDraw raster operation (ROP) hardware is available. \n";
		break;
	case DDERR_NODIRECTDRAWHW :
		msg = "Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware. \n";
		break;
	case DDERR_NODIRECTDRAWSUPPORT :
		msg = "DirectDraw support is not possible with the current display driver. \n";
		break;
	case DDERR_NOEMULATION :
		msg = "Software emulation is not available. \n";
		break;
	case DDERR_NOEXCLUSIVEMODE :
		msg = "The operation requires the application to have exclusive mode, but the application does not have exclusive mode. \n";
		break;
	case DDERR_NOFLIPHW :
		msg = "Flipping visible surfaces is not supported. \n";
		break;
	case DDERR_NOGDI :
		msg = "No GDI is present. \n";
		break;
	case DDERR_NOHWND :
		msg = "Clipper notification requires a window handle, or no window handle has been previously set as the cooperative level window handle. \n";
		break;
	case DDERR_NOMIPMAPHW :
		msg = "The operation cannot be carried out because no mipmap texture mapping hardware is present or available. \n";
		break;
	case DDERR_NOMIRRORHW :
		msg = "The operation cannot be carried out because no mirroring hardware is present or available. \n";
		break;
	case DDERR_NOOVERLAYDEST :
		msg = "The IDirectDrawSurface2::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface2::UpdateOverlay method has not been called on to establish a destination. \n";
		break;
	case DDERR_NOOVERLAYHW :
		msg = "The operation cannot be carried out because no overlay hardware is present or available. \n";
		break;
	case DDERR_NOPALETTEATTACHED :
		msg = "No palette object is attached to this surface. \n";
		break;
	case DDERR_NOPALETTEHW :
		msg = "There is no hardware support for 16- or 256-color palettes. \n";
		break;
	case DDERR_NORASTEROPHW :
		msg = "The operation cannot be carried out because no appropriate raster operation hardware is present or available. \n";
		break;
	case DDERR_NOROTATIONHW :
		msg = "The operation cannot be carried out because no rotation hardware is present or available. \n";
		break;
	case DDERR_NOSTRETCHHW :
		msg = "The operation cannot be carried out because there is no hardware support for stretching. \n";
		break;
	case DDERR_NOT4BITCOLOR :
		msg = "The DirectDrawSurface object is not using a 4-bit color palette and the requested operation requires a 4-bit color palette. \n";
		break;
	case DDERR_NOT4BITCOLORINDEX :
		msg = "The DirectDrawSurface object is not using a 4-bit color index palette and the requested operation requires a 4-bit color index palette. \n";
		break;
	case DDERR_NOT8BITCOLOR :
		msg = "The DirectDrawSurface object is not using an 8-bit color palette and the requested operation requires an 8-bit color palette. \n";
		break;
	case DDERR_NOTAOVERLAYSURFACE :
		msg = "An overlay component is called for a non-overlay surface. \n";
		break;
	case DDERR_NOTEXTUREHW :
		msg = "The operation cannot be carried out because no texture-mapping hardware is present or available. \n";
		break;
	case DDERR_NOTFLIPPABLE :
		msg = "An attempt has been made to flip a surface that cannot be flipped. \n";
		break;
	case DDERR_NOTFOUND :
		msg = "The requested item was not found. \n";
		break;
	case DDERR_NOTINITIALIZED :
		msg = "An attempt was made to call an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized. \n";
		break;
	case DDERR_NOTLOCKED :
		msg = "An attempt is made to unlock a surface that was not locked. \n";
		break;
	case DDERR_NOTPAGELOCKED :
		msg = "An attempt is made to page unlock a surface with no outstanding page locks. \n";
		break;
	case DDERR_NOTPALETTIZED :
		msg = "The surface being used is not a palette-based surface. \n";
		break;
	case DDERR_NOVSYNCHW :
		msg = "The operation cannot be carried out because there is no hardware support for vertical blank synchronized operations. \n";
		break;
	case DDERR_NOZBUFFERHW :
		msg = "The operation to create a z-buffer in display memory or to perform a blit using a z-buffer cannot be carried out because there is no hardware support for z-buffers. \n";
		break;
	case DDERR_NOZOVERLAYHW :
		msg = "The overlay surfaces cannot be z-layered based on the z-order because the hardware does not support z-ordering of overlays. \n";
		break;
	case DDERR_OUTOFCAPS :
		msg = "The hardware needed for the requested operation has already been allocated. \n";
		break;
	case DDERR_OUTOFMEMORY :
		msg = "DirectDraw does not have enough memory to perform the operation. \n";
		break;
	case DDERR_OUTOFVIDEOMEMORY :
		msg = "DirectDraw does not have enough display memory to perform the operation. \n";
		break;
	case DDERR_OVERLAYCANTCLIP :
		msg = "The hardware does not support clipped overlays. \n";
		break;
	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE :
		msg = "An attempt was made to have more than one color key active on an overlay. \n";
		break;
	case DDERR_OVERLAYNOTVISIBLE :
		msg = "The IDirectDrawSurface2::GetOverlayPosition method is called on a hidden overlay. \n";
		break;
	case DDERR_PALETTEBUSY :
		msg = "Access to this palette is refused because the palette is locked by another thread. \n";
		break;
	case DDERR_PRIMARYSURFACEALREADYEXISTS :
		msg = "This process has already created a primary surface. \n";
		break;
	case DDERR_REGIONTOOSMALL :
		msg = "The region passed to the IDirectDrawClipper::GetClipList method is too small. \n";
		break;
	case DDERR_SURFACEALREADYATTACHED :
		msg = "An attempt was made to attach a surface to another surface to which it is already attached. \n";
		break;
	case DDERR_SURFACEALREADYDEPENDENT :
		msg = "An attempt was made to make a surface a dependency of another surface to which it is already dependent. \n";
		break;
	case DDERR_SURFACEBUSY :
		msg = "Access to the surface is refused because the surface is locked by another thread. \n";
		break;
	case DDERR_SURFACEISOBSCURED :
		msg = "Access to the surface is refused because the surface is obscured. \n";
		break;
	case DDERR_SURFACELOST :
		msg = "Access to the surface is refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have the IDirectDrawSurface2::Restore method called on it. \n";
		AfxGetMainWnd()->PostMessage(WM_MESSAGE_RESTOREALL);
		break;
	case DDERR_SURFACENOTATTACHED :
		msg = "The requested surface is not attached. \n";
		break;
	case DDERR_TOOBIGHEIGHT :
		msg = "The height requested by DirectDraw is too large. \n";
		break;
	case DDERR_TOOBIGSIZE :
		msg = "The size requested by DirectDraw is too large. However, the individual height and width are OK. \n";
		break;
	case DDERR_TOOBIGWIDTH :
		msg = "The width requested by DirectDraw is too large. \n";
		break;
	case DDERR_UNSUPPORTED :
		msg = "The operation is not supported. \n";
		break;
	case DDERR_UNSUPPORTEDFORMAT :
		msg = "The FourCC format requested is not supported by DirectDraw. \n";
		break;
	case DDERR_UNSUPPORTEDMASK :
		msg = "The bitmask in the pixel format requested is not supported by DirectDraw. \n";
		break;
	case DDERR_UNSUPPORTEDMODE :
		msg = "The display is currently in an unsupported mode. \n";
		break;
	case DDERR_VERTICALBLANKINPROGRESS :
		msg = "A vertical blank is in progress. \n";
		break;
	case DDERR_WASSTILLDRAWING :
		msg = "The previous blit operation that is transferring information to or from this surface is incomplete. \n";
		break;
	case DDERR_WRONGMODE :
		msg = "This surface cannot be restored because it was created in a different mode. \n";
		break;
	case DDERR_XALIGN :
		msg = "The provided rectangle was not horizontally aligned on a required boundary. 	\n";
		break;
	}
	TRACE(msg);
	return FALSE;//DEFAULT
}


//////////////////////////////////////////////////////////////////
// CDDSurface

IMPLEMENT_DYNAMIC(CDDSurface, CDDObject)

CDDSurface::CDDSurface()
{
	m_pISurf = NULL;
	m_bDontDestroy = FALSE;
    ZeroMemory(&m_SurfDesc, sizeof(m_SurfDesc));
    m_SurfDesc.dwSize = sizeof(m_SurfDesc);
    m_SurfDesc.ddpfPixelFormat.dwSize = sizeof(m_SurfDesc.ddpfPixelFormat);

/*	int data=0;
	for(int i =0; i< 480; i++)
		for(int j=0;j<320;j++)
		{
			m_DataView[i][j]=data++;// 240, 320
			
		}
*/
}

CDDSurface::CDDSurface(IDirectDrawSurface7* ps)
{
	ASSERT(ps);
	m_pISurf = ps;
	ASSERT(m_pISurf);
	// BUGBUG: AddRef'ing the surface doesn't stop it being
	// destroyed so just set a flag to say we don't destroy it in
	// the destructor
	//m_pISurf->AddRef();
	m_bDontDestroy = TRUE;
	_UpdateDescription();
}

CDDSurface::~CDDSurface()
{
	// release the interface
	if ((m_pISurf != NULL) && (!m_bDontDestroy)) {	//AttackMode가 아닌경우에만 파괴함 
		m_pISurf->Release();						//Attack란 IDirectDrawSurface를 인자로 생성한 경우
	}
}

BOOL CDDSurface::Create(CDirectDraw* pDD, DDSURFACEDESC2* pDesc)
{
	if (m_pISurf != NULL) {
		m_pISurf->Release();
		m_pISurf = NULL;
	}
	ASSERT(pDD);
	ASSERT(pDesc);
	m_hr = pDD->GetInterface()->CreateSurface(pDesc, &m_pISurf, NULL);
	if (m_hr!=DD_OK) {
		m_pISurf = NULL;
		return FALSE;
	}
	ASSERT(m_pISurf);
	_UpdateDescription();
	return TRUE;
}

BOOL CDDSurface::CreateVideoSurface(CDirectDraw* pDD, int width, int height)
{
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
	if (Create(pDD, &ddsd)) return TRUE;

	ddsd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
	ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
	return Create(pDD, &ddsd);
}
//가장 사용이 용이하도록 만들어짐
BOOL CDDSurface::Create(CDirectDraw* pDD, int width, int height, BOOL bUseVideoMem)
{
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	if (bUseVideoMem) ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	else ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
	return Create(pDD, &ddsd);
}
//Image파일로 부터생성
BOOL CDDSurface::CreateFromImage(CDirectDraw *pDD, LPCSTR szBitmap, BOOL bUseVideoMem)
{
	if (m_pISurf) m_pISurf->Release(), m_pISurf = NULL;
	ASSERT(pDD);
	m_pISurf = DDLoadBitmap(pDD->GetInterface(), szBitmap, 0, 0, bUseVideoMem);
	if (!m_pISurf) return FALSE;
	_UpdateDescription();
	return TRUE;
}

BOOL CDDSurface::SurfaceFill(COLORREF color)
{
	ASSERT(m_pISurf);
	DDBLTFX ddbltfx;
	ZeroMemory(&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = DDColorMatch(m_pISurf, color);
	m_hr = m_pISurf->Blt(
		NULL,        // Destination
		NULL, NULL,  // Source rectangle
		DDBLT_COLORFILL|DDBLT_WAIT, &ddbltfx);
	if (m_hr!=DD_OK) return CDirectDraw::ErrorHandle(m_hr);
	return TRUE;
}

BOOL CDDSurface::CreateFromHBITMAP(CDirectDraw *pDD, HBITMAP hbmp, BOOL bUseVideoMem)
{
	if (m_pISurf) m_pISurf->Release(), m_pISurf = NULL;
	ASSERT(pDD);
	ASSERT(hbmp);

    BITMAP              bm;
    GetObject(hbmp, sizeof(bm), &bm);      // get size of bitmap

    //
    // create a DirectDrawSurface for this bitmap
    //
    DDSURFACEDESC2       ddsd;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	if (bUseVideoMem) ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	else ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = bm.bmWidth;
    ddsd.dwHeight = bm.bmHeight;

    if (pDD->GetInterface()->CreateSurface(&ddsd, &m_pISurf, NULL) != DD_OK)
	return FALSE;

    DDCopyBitmap(m_pISurf, hbmp, 0, 0, 0, 0);

	_UpdateDescription();
	return TRUE;
}

BOOL CDDSurface::CopyHBITMAP2Surface(HBITMAP hbmp)
{
    BITMAP              bm;
    GetObject(hbmp, sizeof(bm), &bm);      // get size of bitmap

	ASSERT(GetWidth()==bm.bmWidth);
	ASSERT(GetHeight()==bm.bmHeight);

	DDCopyBitmap(m_pISurf, hbmp, 0, 0, 0, 0);
	return TRUE;
}

void CDDSurface::_UpdateDescription()
{
	memset(&m_SurfDesc, 0, sizeof(m_SurfDesc));
	m_SurfDesc.dwSize = sizeof(DDSURFACEDESC2);
	ASSERT(m_pISurf);
	m_hr = m_pISurf->GetSurfaceDesc(&m_SurfDesc);
}

BOOL CDDSurface::GetDescription(DDSURFACEDESC2* pDesc)
{
	if (!m_pISurf) return FALSE;
	_UpdateDescription();
	ASSERT(pDesc);
	memcpy(pDesc, &m_SurfDesc, sizeof(DDSURFACEDESC2));
	return TRUE;
}

CDDSurface* CDDSurface::GetAttachedSurface(DDSCAPS2* pCaps)
{
	ASSERT(m_pISurf);
	ASSERT(pCaps);
	IDirectDrawSurface7* ps = NULL;
	m_hr = m_pISurf->GetAttachedSurface(pCaps, &ps);
	if (m_hr!=DD_OK) {
		return NULL;
	}
	ASSERT(ps);
	CDDSurface* pds = new CDDSurface(ps);
	return pds;
}

BOOL CDDSurface::SetClipper(CDDClipper* pClipper)
{
	ASSERT(m_pISurf);
	ASSERT(pClipper);
	m_hr = m_pISurf->SetClipper(pClipper->GetInterface());
	if (m_hr!=DD_OK) return FALSE;
	_UpdateDescription();
	return TRUE;
}

BOOL CDDSurface::SetPalette(CDDPalette* pPalette)
{
	ASSERT(m_pISurf);
	ASSERT(pPalette);
	m_hr = m_pISurf->SetPalette(pPalette->GetInterface());
	return m_hr==DD_OK;
}

CDDPalette* CDDSurface::GetPalette()
{
	ASSERT(m_pISurf);
	IDirectDrawPalette* pIPal = 0;
	m_hr = m_pISurf->GetPalette(&pIPal);
	if (m_hr!=DD_OK) return NULL;

	// Create a CDDPalette object to return
	ASSERT(pIPal);
	CDDPalette* pPal = new CDDPalette(pIPal);
	pIPal->Release();

	return pPal;
}

BOOL CDDSurface::AddAttachedSurface(CDDSurface* pSurface)
{
	ASSERT(m_pISurf);
	ASSERT(pSurface);
	m_hr = m_pISurf->AddAttachedSurface(pSurface->GetInterface());
	if (m_hr!=DD_OK) return FALSE;
	_UpdateDescription();
	return TRUE;
}

BOOL CDDSurface::Blt(CRect *prcTo, CDDSurface *pSorSurface, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
	ASSERT(m_pISurf);
	ASSERT(prcTo);

	//찍고자 하는 영역과 찍히는 영역의 공통부분찾기 {
	CRect myRect(0, 0, GetWidth(), GetHeight());
	CRect sorRect(0, 0, pSorSurface->GetWidth(), pSorSurface->GetHeight());
	CRect tmpRect = sorRect;
	CPoint offset = prcTo->TopLeft();
	tmpRect.OffsetRect(offset);
	tmpRect &= myRect;
	if (tmpRect.IsRectEmpty()) return TRUE;

	tmpRect.OffsetRect(-offset.x, -offset.y);	
	if (offset.x<0) prcTo->left = 0;
	if (offset.y<0) prcTo->top = 0;
	//찍고자 하는 영역과 찍히는 영역의 공통부분찾기 }

	m_hr = m_pISurf->Blt(prcTo, pSorSurface->GetInterface(), &tmpRect,
						dwFlags, lpDDBltFx);
#ifdef _DEBUG
	if (m_hr!=DD_OK) CDirectDraw::ErrorHandle(m_hr);
#endif
	return m_hr==DD_OK;
}

BOOL CDDSurface::Blt(RECT* prcTo, CDDSurface* pSurFrom, RECT* prcFrom,
			         DWORD dwFlags,	LPDDBLTFX lpDDBltFx)
{
	ASSERT(m_pISurf);
	ASSERT(prcTo);
	ASSERT(pSurFrom);
	ASSERT(prcFrom);
	m_hr = m_pISurf->Blt(prcTo, pSurFrom->GetInterface(), prcFrom,
						dwFlags, lpDDBltFx);
	if (m_hr!=DD_OK) CDirectDraw::ErrorHandle(m_hr);
	return m_hr==DD_OK;
}

BOOL CDDSurface::BltFast(CPoint point, CDDSurface *pSorSurface, DWORD dwTrans)
{
	ASSERT(m_pISurf);
	ASSERT(pSorSurface);

	//찍고자 하는 영역과 찍히는 영역의 공통부분찾기 {
	CRect myRect(0, 0, GetWidth(), GetHeight());
	CRect sorRect(0, 0, pSorSurface->GetWidth(), pSorSurface->GetHeight());
	CRect tmpRect = sorRect;
	tmpRect.OffsetRect(point);
	tmpRect &= myRect;
	if (tmpRect.IsRectEmpty()) return TRUE;

	tmpRect.OffsetRect(-point.x, -point.y);	
	if (point.x<0) point.x = 0;
	if (point.y<0) point.y = 0;
	//찍고자 하는 영역과 찍히는 영역의 공통부분찾기 }

	m_hr = m_pISurf->BltFast(point.x, point.y, pSorSurface->GetInterface(), &tmpRect, dwTrans);
#ifdef _DEBUG
	if (m_hr!=DD_OK) CDirectDraw::ErrorHandle(m_hr);
#endif
	return m_hr==DD_OK;
}

BOOL CDDSurface::BltFast(CPoint point, CDDSurface *pSorSurface, CRect *sourceRect, DWORD dwTrans)
{
	ASSERT(m_pISurf);
	ASSERT(pSorSurface);
	//찍고자 하는 영역과 찍히는 영역의 공통부분찾기 {
	CRect myRect(0, 0, GetWidth(), GetHeight());
	CRect sorRect(*sourceRect);
	CRect sorSurfaceRect(0, 0, pSorSurface->GetWidth(), pSorSurface->GetHeight());
	CRect tmpRect = sorRect;

	tmpRect &= sorSurfaceRect;
	if (tmpRect.IsRectEmpty()) return TRUE;
	tmpRect.OffsetRect(-sorRect.left, -sorRect.top);
	tmpRect.OffsetRect(point);
	if (!tmpRect.IntersectRect(tmpRect, myRect)) return TRUE;
	tmpRect &= myRect;
	tmpRect.OffsetRect(sorRect.left, sorRect.top);

	tmpRect.OffsetRect(-point.x, -point.y);	
	if (point.x<0) point.x = 0;
	if (point.y<0) point.y = 0;
	//찍고자 하는 영역과 찍히는 영역의 공통부분찾기 }
	m_hr = m_pISurf->BltFast(point.x, point.y, pSorSurface->GetInterface(), &tmpRect, dwTrans);
#ifdef _DEBUG
	if (m_hr!=DD_OK) CDirectDraw::ErrorHandle(m_hr);
#endif
	return m_hr==DD_OK;
}

DWORD CDDSurface::GetSemiBlack()
{
	DWORD r, g, b;
	GetRGBMasks(r, g, b);

	DWORD result = 0L;
	int count=0;
	while ((r!=0x80000000)&&count<32) //R값의 최하위 Pixel을 최상위로 Shift함 (상위 Pixel모두 0됨)
	{
		r<<=1;
		count++;
	}
	r >>= count;

	count = 0;
	while ((g!=0x80000000)&&count<32) //G값의 최하위 Pixel을 최상위로 Shift함 (상위 Pixel모두 0됨)
	{
		g<<=1;
		count++;
	}
	g >>= count;

	count = 0;
	while ((b!=0x80000000)&&count<32) //B값의 최하위 Pixel을 최상위로 Shift함 (상위 Pixel모두 0됨)
	{
		b<<=1;
		count++;
	}
	b >>= count;
	result = r|g|b;
	return result;
}
/////////////////////////////////////////////
//현재의 그림에 색깔톤으로 rect영역에 깐다.(64 원색: 128 두배밝게; 0 까맣게)
//bReserveBlack 이면 원래 black 만 black으로 유지된다.
/////////////////////////////////////////////
void CDDSurface::DrawFogLayer(CRect *pRect, COLORREF tcolor, BOOL bReserveBlack)
{
	CRect rr;
	GetRect(rr);
	if (pRect) rr &= *pRect;
	DWORD hiR = GetRValue(tcolor);
	DWORD hiG = GetGValue(tcolor);
	DWORD hiB = GetBValue(tcolor);

	if (rr.IsRectEmpty()) return;
	int width = rr.Width();
	int height = rr.Height();

	DWORD r, g, b;
	DWORD dwRTmp, dwGTmp, dwBTmp;
	GetRGBMasks(r, g, b);

	DWORD semiBlack = GetSemiBlack();

	if (!Lock(NULL, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT)) return;

	WORD *pwDest = (WORD *)((BYTE*)m_SurfDesc.lpSurface + (rr.left << 1) + rr.top * m_SurfDesc.lPitch);

    int dwDestModulo = m_SurfDesc.lPitch - (width<< 1);

	DWORD value;
	DWORD dwTmp;

	WORD *pwSrc = new WORD[width];
	int x = 0;
	if (bReserveBlack) //검정이 아닌색이 검정으로 바뀌면 안됨 (검정이 Transparent라 뚫릴 수 있음)
	{
		do
		{
			CopyMemory(pwSrc, pwDest, width<<1);
			x = width;
			do
			{
				dwTmp = pwSrc[width-x];
				if (dwTmp==0L) 
				{
					pwDest++; 
					continue;
				}
				dwRTmp = ((dwTmp & r) * hiR) >> 6;
				if (dwRTmp>r) dwRTmp = r;
				else dwRTmp &= r;
				dwGTmp = ((dwTmp & g) * hiG) >> 6;
				if (dwGTmp>g) dwGTmp = g;
				else dwGTmp &= g;
				dwBTmp = ((dwTmp & b) * hiB) >> 6;
				if (dwBTmp>b) dwBTmp = b;
				else dwBTmp &= b;

				value = dwRTmp | dwGTmp | dwBTmp;
				if (value==0L)
				{
					value = semiBlack;
				}
				*pwDest++ = (WORD)value;
			}while (--x);
			pwDest = (WORD *)((BYTE *)pwDest + dwDestModulo);
		}while (--height);
	}
	else	//검정아닌 색이 검정으로 되어도 상관없음
	{
		do
		{
			CopyMemory(pwSrc, pwDest, width<<1);
			x = width;
			do
			{
				dwTmp = pwSrc[width-x];

				dwRTmp = ((dwTmp & r) * hiR) >> 6;
				if (dwRTmp>r) dwRTmp = r;
				else dwRTmp &= r;
				dwGTmp = ((dwTmp & g) * hiG) >> 6;
				if (dwGTmp>g) dwGTmp = g;
				else dwGTmp &= g;
				dwBTmp = ((dwTmp & b) * hiB) >> 6;
				if (dwBTmp>b) dwBTmp = b;
				else dwBTmp &= b;

				value = dwRTmp | dwGTmp | dwBTmp;
				*pwDest++ = (WORD)value;
			}while (--x);
			pwDest = (WORD *)((BYTE *)pwDest + dwDestModulo);
		}while (--height);
	}
	delete[] pwSrc;
	Unlock(NULL);
}

/////////////////////////////////////////////
//현재의 그림에 색깔톤으로 rect영역에 깐다.(64 원색: 128 두배밝게; 0 까맣게)
//bReserveBlack 이면 원래 black 만 black으로 유지된다.
/////////////////////////////////////////////
void CDDSurface::DrawFogLayer2(CRect *pRect, COLORREF tcolor, BOOL bReserveBlack)
{
	CRect rr;
	GetRect(rr);
	if (pRect) rr &= *pRect;
	DWORD hiR = GetRValue(tcolor);
	DWORD hiG = GetGValue(tcolor);
	DWORD hiB = GetBValue(tcolor);

	if (rr.IsRectEmpty()) return;
	int width = rr.Width();
	int height = rr.Height();

	DWORD r, g, b;
	DWORD dwRTmp, dwGTmp, dwBTmp;
	GetRGBMasks(r, g, b);

	DWORD semiBlack = GetSemiBlack();

	if (!Lock(NULL, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT)) return;

	WORD *pwDest = (WORD *)((BYTE*)m_SurfDesc.lpSurface + (rr.left << 1) + rr.top * m_SurfDesc.lPitch);

    int dwDestModulo = m_SurfDesc.lPitch - (width<< 1);

	DWORD value;
	DWORD dwTmp;

	WORD *pwSrc = new WORD[width];
	int x = 0;
	if (bReserveBlack) //검정이 아닌색이 검정으로 바뀌면 안됨 (검정이 Transparent라 뚫릴 수 있음)
	{
		do
		{
			CopyMemory(pwSrc, pwDest, width<<1);
			x = width;
			do
			{
				dwTmp = pwSrc[width-x];
				if (dwTmp==0L) 
				{
					pwDest++; 
					continue;
				}
				dwRTmp = ((dwTmp & r) * hiR) >> 6;
				if (dwRTmp>r) dwRTmp = r;
				else dwRTmp &= r;
				dwGTmp = ((dwTmp & g) * hiG) >> 6;
				if (dwGTmp>g) dwGTmp = g;
				else dwGTmp &= g;
				dwBTmp = ((dwTmp & b) * hiB) >> 6;
				if (dwBTmp>b) dwBTmp = b;
				else dwBTmp &= b;

				value = dwRTmp | dwGTmp | dwBTmp;
				if (value==0L)
				{
					value = semiBlack;
				}
				*pwDest++ = (WORD)value;
			}while (--x);			
//			}while (x-= 2);

			pwDest = (WORD *)((BYTE *)pwDest + dwDestModulo);
		}while (--height);
//		}while (height-=2);
	}
	else	//검정아닌 색이 검정으로 되어도 상관없음
	{
		do
		{
			CopyMemory(pwSrc, pwDest, width<<1);
			x = width;
			do
			{
				dwTmp = pwSrc[width-x];

				dwRTmp = ((dwTmp & r) * hiR) >> 6;
				if (dwRTmp>r) dwRTmp = r;
				else dwRTmp &= r;
				dwGTmp = ((dwTmp & g) * hiG) >> 6;
				if (dwGTmp>g) dwGTmp = g;
				else dwGTmp &= g;
				dwBTmp = ((dwTmp & b) * hiB) >> 6;
				if (dwBTmp>b) dwBTmp = b;
				else dwBTmp &= b;

				value = dwRTmp | dwGTmp | dwBTmp;
				*pwDest++ = (WORD)value;
			}while (--x);
//			}while (x-=2);
			pwDest = (WORD *)((BYTE *)pwDest + dwDestModulo);
//		}while (--height);
		}while (height-= 3);
	}
	delete[] pwSrc;
	Unlock(NULL);
}

void CDDSurface::DrawFogLayer3(int alphaDeapth, WORD color)
{
	// 4바이트 계산 해서 어둡게 뿌리기..
	//HRESULT  rval;
	DDSURFACEDESC2 lpDDSD;

	ZeroMemory(&lpDDSD, sizeof(lpDDSD));
	lpDDSD.dwSize = sizeof(lpDDSD);

	if (!Lock(NULL, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT)) return;

	DWORD *p4_scr = (DWORD *)m_SurfDesc.lpSurface;

	DWORD first, second;

	DWORD dest_color = ((color<<16)|color);

	int destDepth = 32-alphaDeapth;

	first = dest_color&dALPHA_MASK1;
	first >>= 5;
	first *= destDepth;
	first &= dALPHA_MASK1;
	
	second = dest_color&dALPHA_MASK2;
	second >>= 5;
	second *= destDepth;
	second &= dALPHA_MASK2;

	dest_color = first|second;

	int i=0;
	int tmp = 0;

	for(int m=0; m< 10; m++)// 단위 별로..찍어 주기 위해서..
	for(int n=0; n< 10; n++)
		for(int k=0; k<60;k++)
			for(int j=0;j<40;j++)
			{
//				if(!(n>1&& n<8&& m>1&&m<8))
//				{
//					if(!(n>3&& n<6&& m>3&&m<6))
//						tmp = 0;
//					else
//						tmp = 5;

				tmp = m;

					i = DataView[k+60*m][j+40*n];

					first = p4_scr[i]&dALPHA_MASK1;
					first >>= 5;
					first *= alphaDeapth + tmp;
					first &= dALPHA_MASK1;
					
					second = p4_scr[i]&dALPHA_MASK2;
					second >>= 5;		
					second *= alphaDeapth + tmp;
					second &= dALPHA_MASK2;

					p4_scr[i] = (first|second)+dest_color;
//				}
			}
		


/*	int i=0;
	for(int k=0; k<480;k++)
		for(int j=0;j<320;j++)
		{
			i = m_DataView[k][j];

			first = p4_scr[i]&dALPHA_MASK1;
			first *= alphaDeapth;
			first >>= 5;
			first &= dALPHA_MASK1;
			
			second = p4_scr[i]&dALPHA_MASK2;
			second *= alphaDeapth;
			second >>= 5;		
			second &= dALPHA_MASK2;

			p4_scr[i] = (first|second)+dest_color;
		}
*/
/*	for(int i=0; i<800*600/2;i++)
	{
		first = p4_scr[i]&dALPHA_MASK1;
		first >>= 5;
		first *= alphaDeapth;
		first &= dALPHA_MASK1;
		
		second = p4_scr[i]&dALPHA_MASK2;
		second >>= 5;
		second *= alphaDeapth;
		second &= dALPHA_MASK2;

		p4_scr[i] = (first|second)+dest_color;
	}
*/
	Unlock(NULL);



}

BOOL CDDSurface::Flip(DWORD dwFlags)
{
	ASSERT(m_pISurf);
	m_hr = m_pISurf->Flip(NULL, dwFlags);
	if (FAILED(m_hr)) CDirectDraw::ErrorHandle(m_hr);
	return m_hr==DD_OK;
}

CDC* CDDSurface::GetDC()
{
	HDC hdc = NULL;
	ASSERT(m_pISurf);
	m_hr = m_pISurf->GetDC(&hdc);
	if (m_hr!=DD_OK) return NULL;

	CDC* pDC = new CDC;
	pDC->Attach(hdc);
	return pDC;
}

void CDDSurface::ReleaseDC(CDC* pDC)
{
	ASSERT(pDC);
	HDC hdc = pDC->Detach();
	delete pDC;
	ASSERT(m_pISurf);
	m_hr = m_pISurf->ReleaseDC(hdc);
	ASSERT(m_hr==DD_OK);
}

void CDDSurface::SetColorKey(DWORD dwFlags, DDCOLORKEY* pDDColorKey)
{
	ASSERT(m_pISurf);
	m_hr = m_pISurf->SetColorKey(dwFlags, pDDColorKey);
	ASSERT(m_hr==DD_OK);
	_UpdateDescription();
}

BOOL CDDSurface::SetColorKey(COLORREF rgb)
{
	ASSERT(m_pISurf);
    DDCOLORKEY          ddck;

    ddck.dwColorSpaceLowValue  = DDColorMatch(m_pISurf, rgb);
    ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
    m_hr = m_pISurf->SetColorKey(DDCKEY_SRCBLT, &ddck);
	return m_hr==DD_OK;
}

void CDDSurface::GetRect(RECT& rc)
{
	rc.left = rc.top = 0;
	rc.right = GetWidth();
	rc.bottom = GetHeight();
}

int CDDSurface::GetBitsPerPixel()
{
	// Note: only valid for RGB formats
	if (!(m_SurfDesc.ddpfPixelFormat.dwFlags & DDPF_RGB)) {
		return 0; // not RGB
	}

	return m_SurfDesc.ddpfPixelFormat.dwRGBBitCount;
}

void CDDSurface::GetRGBMasks(DWORD& r, DWORD& g, DWORD &b)
{
	// Note: only valid for RGB formats
	if (!(m_SurfDesc.ddpfPixelFormat.dwFlags & DDPF_RGB)) {
		r = g = b = 0;
		return; // not RGB
	}

	r = m_SurfDesc.ddpfPixelFormat.dwRBitMask;
	g = m_SurfDesc.ddpfPixelFormat.dwGBitMask;
	b = m_SurfDesc.ddpfPixelFormat.dwBBitMask;
}

void* CDDSurface::Lock(CRect *pRect, DWORD dwFlag)
{
	ASSERT(m_pISurf);

	if( m_pISurf->IsLost() == DDERR_SURFACELOST )
	{
		AfxGetMainWnd()->SendMessage(WM_MESSAGE_RESTOREALL);

//		m_hr = m_pISurf->Restore();
//		if( m_hr!=DD_OK )
//		{
//			CDirectDraw::ErrorHandle(m_hr);
//			return NULL;
//		}
//		else TRACE0("surface restore\n");
	}
	// Lock the entire surface
	m_hr = m_pISurf->Lock(pRect,
					      &m_SurfDesc,
						  dwFlag|DDLOCK_WAIT,
						  NULL);
	if (m_hr!=DD_OK) 
	{
		CDirectDraw::ErrorHandle(m_hr);
		return NULL;
	}
	return m_SurfDesc.lpSurface;
}

void CDDSurface::Unlock(CRect *pRect)
{
	if (!m_SurfDesc.lpSurface) return; // Not locked
	m_hr = m_pISurf->Unlock(pRect);

	if( m_hr == DDERR_SURFACELOST )
	{
		AfxGetMainWnd()->SendMessage(WM_MESSAGE_RESTOREALL);
//		m_hr = m_pISurf->Restore();
//		if( m_hr!=DD_OK )
//		{
//			CDirectDraw::ErrorHandle(m_hr);
//			return;
//		}
//		else TRACE0("surface restore\n");
	}

	ASSERT(m_hr==DD_OK);
	m_SurfDesc.lpSurface = NULL;
}

void* CDDSurface::Lock()
{
	ASSERT(m_pISurf);

	if( m_pISurf->IsLost() == DDERR_SURFACELOST )
	{
		AfxGetMainWnd()->SendMessage(WM_MESSAGE_RESTOREALL);
//		m_hr = m_pISurf->Restore();
//		if( m_hr!=DD_OK )
//		{
//			CDirectDraw::ErrorHandle(m_hr);
//			return NULL;
//		}
//		else TRACE0("surface restore\n");
	}
	// Lock the entire surface
	m_hr = m_pISurf->Lock(NULL,
					      &m_SurfDesc,
						  DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,
						  NULL);
	if (m_hr!=DD_OK) 
	{
		CDirectDraw::ErrorHandle(m_hr);
		return NULL;
	}
	return m_SurfDesc.lpSurface;
}

void CDDSurface::Unlock()
{
	if (!m_SurfDesc.lpSurface) return; // Not locked
	m_hr = m_pISurf->Unlock(NULL);
	if( m_hr == DDERR_SURFACELOST )
	{
		AfxGetMainWnd()->SendMessage(WM_MESSAGE_RESTOREALL);
//		m_hr = m_pISurf->Restore();
//		if( m_hr!=DD_OK )
//		{
//			CDirectDraw::ErrorHandle(m_hr);
//			return;
//		}
//		else TRACE0("surface restore\n");
	}
	ASSERT(m_hr==DD_OK);
	m_SurfDesc.lpSurface = NULL;
}
//////////////////////////////////////////////////////////////////
// CDDClipper

IMPLEMENT_DYNAMIC(CDDClipper, CDDObject)

CDDClipper::CDDClipper()
{
	m_pIClip = NULL;
}

CDDClipper::~CDDClipper()
{
	// release the interface
	if (m_pIClip != NULL) {
		m_pIClip->Release();
	}
}

BOOL CDDClipper::Create(CDirectDraw* pDD, HWND hWnd)
{
	ASSERT(!m_pIClip);
	ASSERT(pDD);
	ASSERT(hWnd);
	m_hr = pDD->GetInterface()->CreateClipper(0, &m_pIClip, NULL);
	if (m_hr!=DD_OK) return FALSE;
	ASSERT(m_pIClip);
	m_hr = m_pIClip->SetHWnd(0, hWnd);
	return m_hr==DD_OK;
}

BOOL CDDClipper::SetClipList(LPCRECT r)
{
	ASSERT(m_pIClip);
	LPRGNDATA	region_data;

	region_data = (LPRGNDATA)new BYTE[sizeof(RGNDATAHEADER) +
					sizeof(RECT)];
	memcpy(region_data->Buffer, &r, sizeof(RECT));
	region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
	region_data->rdh.iType = RDH_RECTANGLES;
	region_data->rdh.nCount = 1;
	region_data->rdh.nRgnSize = sizeof(RECT);

	region_data->rdh.rcBound = *r;

	m_hr = m_pIClip->SetClipList(region_data, 0);
	delete[] (BYTE*)region_data;
	return m_hr==DD_OK;
}

//////////////////////////////////////////////////////////////////
// CDDPalette

IMPLEMENT_DYNAMIC(CDDPalette, CDDObject)

CDDPalette::CDDPalette()
{
	m_pIPal = NULL;
}

CDDPalette::CDDPalette(IDirectDrawPalette* pIPal)
{
	m_pIPal = pIPal;
	if (m_pIPal) {
		m_pIPal->AddRef();
	}
}

CDDPalette::~CDDPalette()
{
	// release the interface
	if (m_pIPal != NULL) {
		m_pIPal->Release();
	}
}

BOOL CDDPalette::Create(CDirectDraw* pDD, DWORD dwFlags, LPPALETTEENTRY pColorTable)
{
	if (m_pIPal != NULL) {
		m_pIPal->Release();
		m_pIPal = NULL;
	}
	ASSERT(pDD);
	m_hr = pDD->GetInterface()->CreatePalette(dwFlags, pColorTable, &m_pIPal, NULL);
	if (m_hr!=DD_OK) return FALSE;
	ASSERT(m_pIPal);
	return TRUE;
}

BOOL CDDPalette::GetEntries(int iStart, int iCount, PALETTEENTRY* pColorTable)
{
	ASSERT(m_pIPal);
	m_hr = m_pIPal->GetEntries(0, iStart, iCount, pColorTable);
	return m_hr==DD_OK;
}

BOOL CDDPalette::SetEntries(int iStart, int iCount, PALETTEENTRY* pColorTable)
{
	ASSERT(m_pIPal);
	m_hr = m_pIPal->SetEntries(0, iStart, iCount, pColorTable);
	return m_hr==DD_OK;
}


//////////////////////////////////////////////////////////////////
// CDirectInputDevice
IMPLEMENT_DYNAMIC(CDirectInputDevice, CDDObject)

CDirectInputDevice::CDirectInputDevice(CDirectInput *pDirectInput, int deviceMode)
{
	ASSERT(pDirectInput);
	m_bpDirectInput = pDirectInput;
	m_pDirectInputDevice = NULL;
	m_iDeviceMode = deviceMode;
}

CDirectInputDevice::~CDirectInputDevice()
{
	if (m_pDirectInputDevice)
	{
        m_pDirectInputDevice->Unacquire();
        m_pDirectInputDevice->Release();
        m_pDirectInputDevice = NULL;
	}
}
//Windowed 이면 NONEXCLUSIVE 모드로 생성
BOOL CDirectInputDevice::Create(BOOL bIsWindowed)
{
	IDirectInput *piDirectInput = m_bpDirectInput->GetInterface();
	HWND hConnectedWnd = m_bpDirectInput->GetConnectedHwnd();
	HRESULT hr;
	switch(m_iDeviceMode)
	{
	case CDIRECTINPUT_DEVICE_MOUSE: //DirectInput 중 마우스 생성
		hr = piDirectInput->CreateDevice(GUID_SysMouse, &m_pDirectInputDevice, NULL);
		if (hr!=DI_OK) return CDirectInput::ErrorHandle(hr);

		ASSERT(m_pDirectInputDevice);

		hr = m_pDirectInputDevice->SetDataFormat(&c_dfDIMouse);
		if (hr!=DI_OK) return CDirectInput::ErrorHandle(hr);

	    if (bIsWindowed) hr = m_pDirectInputDevice->SetCooperativeLevel(hConnectedWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		else hr = m_pDirectInputDevice->SetCooperativeLevel(hConnectedWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		if (hr!=DI_OK) return CDirectInput::ErrorHandle(hr);

		break;
	case CDIRECTINPUT_DEVICE_KEYBOARD: //DirectInput중 키보드 생성
		hr = piDirectInput->CreateDevice(GUID_SysKeyboard, &m_pDirectInputDevice, NULL);
		if (hr!=DI_OK) return CDirectInput::ErrorHandle(hr);

		ASSERT(m_pDirectInputDevice);
		hr = m_pDirectInputDevice->SetDataFormat(&c_dfDIKeyboard);
		if (hr!=DI_OK) return CDirectInput::ErrorHandle(hr);

		hr = m_pDirectInputDevice->SetCooperativeLevel(hConnectedWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
//	    if (bIsWindowed) hr = m_pDirectInputDevice->SetCooperativeLevel(hConnectedWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
//		else hr = m_pDirectInputDevice->SetCooperativeLevel(hConnectedWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		if (hr!=DI_OK) return CDirectInput::ErrorHandle(hr);

		break;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////
// CDirectInput
IMPLEMENT_DYNAMIC(CDirectInput, CDDObject)

CDirectInput::CDirectInput(HWND hConnectedWnd)
{
	ASSERT(hConnectedWnd);
	m_hConnectedWnd = hConnectedWnd;
	m_pDirectInput = NULL;

	m_diMouse = NULL;
	m_diKeyboard = NULL;
}

CDirectInput::~CDirectInput()
{
	if (m_diMouse) 
	{
		delete m_diMouse;
		m_diMouse = NULL;
	}
	if (m_diKeyboard) 
	{
		delete m_diKeyboard;
		m_diKeyboard = NULL;
	}

	if (m_pDirectInput)
	{
		m_pDirectInput->Release();
		m_pDirectInput = NULL;
	}
}

BOOL CDirectInput::Create(HINSTANCE hInst)
{
	HRESULT hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pDirectInput, NULL);
	//HRESULT hr = DirectInputCreate(hInst, DIRECTINPUT_VERSION, &m_pDirectInput, NULL);
	if (hr!=DI_OK) return ErrorHandle(hr);
	return TRUE;
}

BOOL CDirectInput::MouseInputCreate(BOOL bIsWindowed)
{
	ASSERT(m_pDirectInput);
	ASSERT(!m_diMouse);
	m_diMouse = new CDirectInputDevice(this, CDIRECTINPUT_DEVICE_MOUSE);
	if (!m_diMouse->Create(bIsWindowed))
	{
		delete m_diMouse;
		m_diMouse = NULL;
		return FALSE;
	}
	m_diMouse->GetInterface()->Acquire();
	return TRUE;
}

BOOL CDirectInput::KeyboardInputCreate(BOOL bIsWindowed)
{
	ASSERT(m_pDirectInput);
	ASSERT(!m_diKeyboard);
	m_diKeyboard = new CDirectInputDevice(this, CDIRECTINPUT_DEVICE_KEYBOARD);
	if (!m_diKeyboard->Create(bIsWindowed))
	{
		delete m_diKeyboard;
		m_diKeyboard = NULL;
		return FALSE;
	}
	m_diKeyboard->GetInterface()->Acquire();
	return TRUE;
}

void CDirectInput::MouseInputCheck(BBDX_MouseInformation *pMouseInfo)
{
	ASSERT(m_pDirectInput);
	ASSERT(m_diMouse);
	ASSERT(pMouseInfo);
	HRESULT hr;
    DIMOUSESTATE dims;          /* DirectInput mouse state structure */
	static int struct_size = sizeof(DIMOUSESTATE);
//mouse_again:;
	hr = m_diMouse->GetInterface()->GetDeviceState(struct_size, &dims);
    if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
	{
        /*
         *  DirectInput is telling us that the input stream has
         *  been interrupted.  We aren't tracking any state
         *  between polls, so we don't have any special reset
         *  that needs to be done.  We just re-acquire and
         *  try again.
         */
        hr = m_diMouse->GetInterface()->Acquire();
//        if (SUCCEEDED(hr)) goto mouse_again;
		return;
	}
//  Mouse Information is valid
	if (SUCCEEDED(hr)) pMouseInfo->SetMouseState(&dims);
}

BOOL CDirectInput::KeyboardInputCheck()
{
	ASSERT(m_pDirectInput);
	ASSERT(m_diKeyboard);

    BYTE diks[256];             /* DirectInput keyboard state buffer */
    HRESULT hr;
keyboard_again:;
	hr = m_diKeyboard->GetInterface()->GetDeviceState(sizeof(diks), &diks);
    if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) 
	{
        /*
         *  DirectInput is telling us that the input stream has
         *  been interrupted.  We aren't tracking any state
         *  between polls, so we don't have any special reset
         *  that needs to be done.  We just re-acquire and
         *  try again.
         */
        hr = m_diKeyboard->GetInterface()->Acquire();
        if (SUCCEEDED(hr)) goto keyboard_again;
    }
//Keyboard Information is valid
	if (SUCCEEDED(hr))
	{
		for (int i=0; i<256; i++)
		{
			if (diks[i]&0x80)
			{
	//key pressed
			}
		}
	}
	return TRUE;
}

BOOL CDirectInput::OnActivate(BOOL bActive)
{
	//활성화 될때(Application이) -> Acquire해서 제어를 얻어야함
	//비활성화 될때 제어를 풀어줘야 함
	TRACE("Direct Input Active %d", bActive);
	if (m_diMouse)
	{
		TRACE("###Acquire %d\n", bActive);
		if (bActive) m_diMouse->GetInterface()->Acquire();
		else m_diMouse->GetInterface()->Unacquire();
		return TRUE;
	}
	if (m_diKeyboard)
	{
		if (bActive) m_diKeyboard->GetInterface()->Acquire();
		else m_diMouse->GetInterface()->Unacquire();
		return TRUE;
	}
	return FALSE;
}

BOOL CDirectInput::ErrorHandle(HRESULT hr)
{
	switch(hr)
	{
//	case DI_BUFFEROVERFLOW :
//	TRACE("The device buffer overflowed and some input was lost. This value is equal to the S_FALSE standard COM return value. \n");
//	break;
//	case DI_POLLEDDEVICE :
//	TRACE("The device is a polled device. As a result, device buffering will not collect any data and event notifications will not be signaled until the IDirectInputDevice::GetDeviceState method is called. \n");
//	break;
//	case DI_PROPNOEFFECT :
//	TRACE("The change in device properties had no effect. This value is equal to the S_FALSE standard COM return value. \n");
//	break;
	case DI_NOTATTACHED :
	TRACE("The device exists but is not currently attached. This value is equal to the S_FALSE standard COM return value. \n");
	break;
	case DIERR_ACQUIRED :
	TRACE("The operation cannot be performed while the device is acquired. \n");
	break;
	case DIERR_ALREADYINITIALIZED :
	TRACE("This object is already initialized \n");
	break;
	case DIERR_BADDRIVERVER :
	TRACE("The object could not be created due to an incompatible driver version or mismatched or incomplete driver components. \n");
	break;
	case DIERR_BETADIRECTINPUTVERSION :
	TRACE("The application was written for an unsupported prerelease version of DirectInput. \n");
	break;
	case DIERR_DEVICENOTREG :
	TRACE("The device or device instance is not registered with DirectInput. This value is equal to the REGDB_E_CLASSNOTREG standard COM return value. \n");
	break;
	case DIERR_GENERIC :
	TRACE("An undetermined error occurred inside the DirectInput subsystem. This value is equal to the E_FAIL standard COM return value. \n");
	break;
//	case DIERR_HANDLEEXISTS :
//	TRACE("The device already has an event notification associated with it. This value is equal to the E_ACCESSDENIED standard COM return value. \n");
//	break;
//	case DIERR_OTHERAPPHASPRIO :
//	TRACE("Another application has a higher priority level, preventing this call from succeeding. This value is equal to the E_ACCESSDENIED standard COM return value. \n");
//	break;
	case DIERR_READONLY :
	TRACE("The specified property cannot be changed. This value is equal to the E_ACCESSDENIED standard COM return value. \n");
	break;
	case DIERR_INPUTLOST :
	TRACE("Access to the input device has been lost. It must be re-acquired. \n");
	break;
	case DIERR_INVALIDPARAM :
	TRACE("An invalid parameter was passed to the returning function, or the object was not in a state that permitted the function to be called. This value is equal to the E_INVALIDARG standard COM return value. \n");
	break;
	case DIERR_NOAGGREGATION :
	TRACE("This object does not support aggregation. This value is equal to the CLASS_E_NOAGGREGATION standard COM return value. \n");
	break;
	case DIERR_NOINTERFACE :
	TRACE("The specified interface is not supported by the object. This value is equal to the E_NOINTERFACE standard COM return value. \n");
	break;
	case DIERR_NOTACQUIRED :
	TRACE("The operation cannot be performed unless the device is acquired. \n");
	break;
	case DIERR_NOTINITIALIZED :
	TRACE("This object has not been initialized. \n");
	break;
	case DIERR_OBJECTNOTFOUND :
	TRACE("The requested object does not exist. \n");
	break;
	case DIERR_OLDDIRECTINPUTVERSION :
	TRACE("The application requires a newer version of DirectInput. \n");
	break;
	case DIERR_OUTOFMEMORY :
	TRACE("The DirectInput subsystem couldn't allocate sufficient memory to complete the caller's request. This value is equal to the E_OUTOFMEMORY standard COM return value. \n");
	break;
	case DIERR_UNSUPPORTED :
	TRACE("The function called is not supported at this time. This value is equal tot the E_NOTIMPL standard COM return value. \n");
	break;
	case E_PENDING :
	TRACE("Data is not yet available. \n");
	break;

//	DirectInput joystick functions can return the following values. 

	case JOYERR_NOCANDO :
	TRACE("The joystick driver cannot update the device information from the registry. \n");
	break;
	case JOYERR_NOERROR :
	TRACE("The request completed successfully. \n");
	break;
//	case JOYERR_REGISTRYNOTVALID :
//	TRACE("One or more registry joystick entries contain invalid data. \n");
//	break;
	case JOYERR_UNPLUGGED :
	TRACE("The specified joystick is not connected to the computer. \n");
	break;
	case MMSYSERR_BADDEVICEID :
	TRACE("The specified joystick identifier is invalid. \n");
	break;
	case MMSYSERR_INVALPARAM :
	TRACE("An invalid parameter was passed. \n");
	break;
	case MMSYSERR_NODRIVER :
	TRACE("The joystick driver is not present. \n");
	break;
	}
	return FALSE;
}




//
//  DDLoadBitmap
//
//  create a DirectDrawSurface from a bitmap resource.
//

extern "C" IDirectDrawSurface7 * DDLoadBitmap(IDirectDraw7 *pdd, LPCSTR szBitmap, int dx, int dy, BOOL bUseVideoMem)
{
    HBITMAP             hbm;
    BITMAP              bm;
    DDSURFACEDESC2       ddsd;
    IDirectDrawSurface7 *pdds;

    //
    //  try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx, dy, LR_CREATEDIBSECTION);

    if (hbm == NULL)
	hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
	return NULL;

    //
    // get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);      // get size of bitmap

    //
    // create a DirectDrawSurface for this bitmap
    //
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	if (bUseVideoMem) ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	else ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = bm.bmWidth;
    ddsd.dwHeight = bm.bmHeight;

	TRACE("memory up 시도 =>  %s\n", szBitmap);

    if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)
	{
		if (bUseVideoMem) //Video Memory로 생성을 실패하면 SystemMemory로 다시시도
		{
			ddsd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

			TRACE("system memory up ^^=> %s\n", szBitmap);

			if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK) return NULL;
		}
		else return NULL;
	}

    DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);

    DeleteObject(hbm);

    return pdds;
}

// 재덕이가 사용할꺼..음..내방식이 좋지..쩝..
/*extern "C" BOOL SetColorKey(LPDIRECTDRAWSURFACE4 lpdds, COLORREF ColorKey)
{
	if(lpdds == NULL) return FALSE;

	// 컬러키 빼기
	DDCOLORKEY ddck;
	DWORD CK = RGB2(ColorKey);
	ddck.dwColorSpaceHighValue = CK;
	ddck.dwColorSpaceLowValue = CK;
	lpdds->SetColorKey(DDCKEY_SRCBLT , &ddck);

	return TRUE;
}

extern "C" LPDIRECTDRAWSURFACE4 CreateSurface(IDirectDraw4 *lpDD, int nWidth, int nHeight, BOOL bUseVideoMemory, COLORREF ColorKey)
{
	if(lpDD == NULL) return NULL;

	LPDIRECTDRAWSURFACE4 lpdds = NULL;

	DDSURFACEDESC2 ddsd;
	CLEAR(ddsd);
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
	ddsd.dwWidth = nWidth;
	ddsd.dwHeight = nHeight;

    if(bUseVideoMemory == TRUE) // 비디오 메모리에 생성할 꺼면...
	{
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
 
		if (lpDD->CreateSurface(&ddsd, &lpdds, NULL) != DD_OK)
		{
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			if (lpDD->CreateSurface(&ddsd, &lpdds, NULL) != DD_OK) return NULL;
		}
	}
	else
	{
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		lpDD->CreateSurface(&ddsd, &lpdds, NULL);
	}

	// 컬러키 빼기
	SetColorKey(lpdds, ColorKey);

	// 서피스 리턴
	return lpdds;
}

extern "C" LPDIRECTDRAWSURFACE4 DDLoadBitmap2(IDirectDraw4 *lpDD, LPCTSTR szFileName, BOOL bUseVideoMemory, COLORREF ColorKey)
{
	if(lpDD == NULL) return NULL;

	HBITMAP					hbm;
    BITMAP					bm;

    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hbm == NULL)
		hbm = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (hbm == NULL)
		hbm = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (hbm == NULL)
		hbm = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (hbm == NULL)
		hbm = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	if (hbm == NULL)
		hbm = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
	return NULL;

    GetObject(hbm, sizeof(bm), &bm);      // get size of bitmap

	// 서피스를 만든다.
	LPDIRECTDRAWSURFACE4 lpdds = NULL;
	lpdds = CreateSurface(lpDD, bm.bmWidth, bm.bmHeight, bUseVideoMemory, ColorKey);

	if (hbm == NULL || lpdds == NULL)
	return NULL;

	HDC                 hdcImage;
	HDC                 hdc;
	HRESULT             hr;

	hdcImage = CreateCompatibleDC(NULL);

	SelectObject(hdcImage, hbm);

	hr = lpdds->GetDC(&hdc);
	StretchBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcImage, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	lpdds->ReleaseDC(hdc);

	DeleteDC(hdcImage);

	DeleteObject(hbm);

	return lpdds;
}
*/
//
//  DDReLoadBitmap
// 
//  load a bitmap from a file or resource into a directdraw surface.
//  normaly used to re-load a surface after a restore.
// 
HRESULT DDReLoadBitmap(IDirectDrawSurface7 *pdds, LPCSTR szBitmap)
{
    HBITMAP             hbm;
    HRESULT             hr;

    //
    //  try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

    if (hbm == NULL)
	hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
    {
	OutputDebugString("handle is null\n");
	return E_FAIL;
    }

    hr = DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    if (hr != DD_OK)
    {
	OutputDebugString("ddcopybitmap failed\n");
    }


    DeleteObject(hbm);
    return hr;
}

//
//  DDCopyBitmap
//
//  draw a bitmap into a DirectDrawSurface
//

extern "C" HRESULT DDCopyBitmap(IDirectDrawSurface7 *pdds, HBITMAP hbm, int x, int y, int dx, int dy)
{
    HDC                 hdcImage;
    HDC                 hdc;
    BITMAP              bm;
    DDSURFACEDESC2       ddsd;
    HRESULT             hr;

    if (hbm == NULL || pdds == NULL)
	return E_FAIL;

    //
    // make sure this surface is restored.
    //
    pdds->Restore();

    //
    //  select bitmap into a memoryDC so we can use it.
    //
    hdcImage = CreateCompatibleDC(NULL);
    if (!hdcImage)
	OutputDebugString("createcompatible dc failed\n");
    SelectObject(hdcImage, hbm);

    //
    // get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);    // get size of bitmap
    dx = dx == 0 ? bm.bmWidth  : dx;    // use the passed size, unless zero
    dy = dy == 0 ? bm.bmHeight : dy;

    //
    // get size of surface.
    //
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);

    if ((hr = pdds->GetDC(&hdc)) == DD_OK)
    {
//	StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y, dx, dy, SRCCOPY);
	BitBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, 0, 0, SRCCOPY);
	pdds->ReleaseDC(hdc);
    }

    DeleteDC(hdcImage);

    return hr;
}

//
//  DDLoadPalette
//
//  Create a DirectDraw palette object from a bitmap resoure
//
//  if the resource does not exist or NULL is passed create a
//  default 332 palette.
//
extern "C" IDirectDrawPalette * DDLoadPalette(IDirectDraw7 *pdd, LPCSTR szBitmap)
{
    IDirectDrawPalette* ddpal;
    int                 i;
    int                 n;
    int                 fh;
    HRSRC               h;
    LPBITMAPINFOHEADER  lpbi;
    PALETTEENTRY        ape[256];
    RGBQUAD *           prgb;

    //
    // build a 332 palette as the default.
    //
    for (i=0; i<256; i++)
    {
	ape[i].peRed   = (BYTE)(((i >> 5) & 0x07) * 255 / 7);
	ape[i].peGreen = (BYTE)(((i >> 2) & 0x07) * 255 / 7);
	ape[i].peBlue  = (BYTE)(((i >> 0) & 0x03) * 255 / 3);
	ape[i].peFlags = (BYTE)0;
    }

    //
    // get a pointer to the bitmap resource.
    //
    if (szBitmap && (h = FindResource(NULL, szBitmap, RT_BITMAP)))
    {
	lpbi = (LPBITMAPINFOHEADER)LockResource(LoadResource(NULL, h));
	if (!lpbi)
	    OutputDebugString("lock resource failed\n");
	prgb = (RGBQUAD*)((BYTE*)lpbi + lpbi->biSize);

	if (lpbi == NULL || lpbi->biSize < sizeof(BITMAPINFOHEADER))
	    n = 0;
	else if (lpbi->biBitCount > 8)
	    n = 0;
	else if (lpbi->biClrUsed == 0)
	    n = 1 << lpbi->biBitCount;
	else
	    n = lpbi->biClrUsed;

	//
	//  a DIB color table has its colors stored BGR not RGB
	//  so flip them around.
	//
	for(i=0; i<n; i++ )
	{
	    ape[i].peRed   = prgb[i].rgbRed;
	    ape[i].peGreen = prgb[i].rgbGreen;
	    ape[i].peBlue  = prgb[i].rgbBlue;
	    ape[i].peFlags = 0;
	}
    }
    else if (szBitmap && (fh = _lopen(szBitmap, OF_READ)) != -1)
    {
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	_lread(fh, &bf, sizeof(bf));
	_lread(fh, &bi, sizeof(bi));
	_lread(fh, ape, sizeof(ape));
	_lclose(fh);

	if (bi.biSize != sizeof(BITMAPINFOHEADER))
	    n = 0;
	else if (bi.biBitCount > 8)
	    n = 0;
	else if (bi.biClrUsed == 0)
	    n = 1 << bi.biBitCount;
	else
	    n = bi.biClrUsed;

	//
	//  a DIB color table has its colors stored BGR not RGB
	//  so flip them around.
	//
	for(i=0; i<n; i++ )
	{
	    BYTE r = ape[i].peRed;
	    ape[i].peRed  = ape[i].peBlue;
	    ape[i].peBlue = r;
	}
    }

    pdd->CreatePalette(DDPCAPS_8BIT, ape, &ddpal, NULL);

    return ddpal;
}

//entry만 bitmap에서 뽑아서 팔레트를 셋팅함.
extern "C" void DDLoadPaletteToParam(IDirectDrawPalette *idp, LPCSTR szBitmap)
{
//    IDirectDrawPalette* ddpal;
    int                 i;
    int                 n;
    int                 fh;
    HRSRC               h;
    LPBITMAPINFOHEADER  lpbi;
    PALETTEENTRY        ape[256];
    RGBQUAD *           prgb;

    //
    // build a 332 palette as the default.
    //
    for (i=0; i<256; i++)
    {
	ape[i].peRed   = (BYTE)(((i >> 5) & 0x07) * 255 / 7);
	ape[i].peGreen = (BYTE)(((i >> 2) & 0x07) * 255 / 7);
	ape[i].peBlue  = (BYTE)(((i >> 0) & 0x03) * 255 / 3);
	ape[i].peFlags = (BYTE)0;
    }

    //
    // get a pointer to the bitmap resource.
    //
    if (szBitmap && (h = FindResource(NULL, szBitmap, RT_BITMAP)))
    {
	lpbi = (LPBITMAPINFOHEADER)LockResource(LoadResource(NULL, h));
	if (!lpbi)
	    OutputDebugString("lock resource failed\n");
	prgb = (RGBQUAD*)((BYTE*)lpbi + lpbi->biSize);

	if (lpbi == NULL || lpbi->biSize < sizeof(BITMAPINFOHEADER))
	    n = 0;
	else if (lpbi->biBitCount > 8)
	    n = 0;
	else if (lpbi->biClrUsed == 0)
	    n = 1 << lpbi->biBitCount;
	else
	    n = lpbi->biClrUsed;

	//
	//  a DIB color table has its colors stored BGR not RGB
	//  so flip them around.
	//
	for(i=0; i<n; i++ )
	{
	    ape[i].peRed   = prgb[i].rgbRed;
	    ape[i].peGreen = prgb[i].rgbGreen;
	    ape[i].peBlue  = prgb[i].rgbBlue;
	    ape[i].peFlags = 0;
	}
    }
    else if (szBitmap && (fh = _lopen(szBitmap, OF_READ)) != -1)
    {
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	_lread(fh, &bf, sizeof(bf));
	_lread(fh, &bi, sizeof(bi));
	_lread(fh, ape, sizeof(ape));
	_lclose(fh);

	if (bi.biSize != sizeof(BITMAPINFOHEADER))
	    n = 0;
	else if (bi.biBitCount > 8)
	    n = 0;
	else if (bi.biClrUsed == 0)
	    n = 1 << bi.biBitCount;
	else
	    n = bi.biClrUsed;

	//
	//  a DIB color table has its colors stored BGR not RGB
	//  so flip them around.
	//
	for(i=0; i<n; i++ )
	{
	    BYTE r = ape[i].peRed;
	    ape[i].peRed  = ape[i].peBlue;
	    ape[i].peBlue = r;
	}
    }
	TRACE("Palette SetEntry : %d palettes\n", n);
	idp->SetEntries(0, 0, n, ape);
//    pdd->CreatePalette(DDPCAPS_8BIT, ape, &ddpal, NULL);

//    return ddpal;
}

//
// DDColorMatch
//
// convert a RGB color to a pysical color.
//
// we do this by leting GDI SetPixel() do the color matching
// then we lock the memory and see what it got mapped to.
//
extern "C" DWORD DDColorMatch(IDirectDrawSurface7 *pdds, COLORREF rgb)
{
    COLORREF rgbT;
    HDC hdc;
    DWORD dw = CLR_INVALID;
    DDSURFACEDESC2 ddsd;
    HRESULT hres;

    //
    //  use GDI SetPixel to color match for us
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
	rgbT = GetPixel(hdc, 0, 0);             // save current pixel value
	SetPixel(hdc, 0, 0, rgb);               // set our value
	pdds->ReleaseDC(hdc);
    }

    //
    // now lock the surface so we can read back the converted color
    //
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
	;

    if (hres == DD_OK)
    {
	dw  = *(DWORD *)ddsd.lpSurface;                     // get DWORD
        if(ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
            dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1;  // mask it to bpp
	pdds->Unlock(NULL);
    }

    //
    //  now put the color that was there back.
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
	SetPixel(hdc, 0, 0, rgbT);
	pdds->ReleaseDC(hdc);
    }

    return dw;
}


//
// DDSetColorKey
//
// set a color key for a surface, given a RGB.
// if you pass CLR_INVALID as the color key, the pixel
// in the upper-left corner will be used.
//
extern "C" HRESULT DDSetColorKey(IDirectDrawSurface7 *pdds, COLORREF rgb)
{
    DDCOLORKEY          ddck;

    ddck.dwColorSpaceLowValue  = DDColorMatch(pdds, rgb);
    ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
    return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}



