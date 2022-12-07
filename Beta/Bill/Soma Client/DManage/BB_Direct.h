// 3dDirect.h
//
// Copyright (c) Nigel Thompson 1996
//
// This file defines all the Direct Draw classes
//
// This file should be included in your application's main
// include file so that it is available to all modules that
// need access the the Direct Draw classes
//

#ifndef __BB_DIRECT_H_
#define __BB_DIRECT_H_

#ifdef __cplusplus
extern "C"
{            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

#ifdef _DIRECTDLLMODULE
#define EXP_IMP_DECL    __declspec( dllexport )
#else
#define EXP_IMP_DECL    __declspec( dllimport )
#endif

//DirectX의 Api모듬 (DirectX Sdk ddutil에 있음)
extern EXP_IMP_DECL void DDLoadPaletteToParam(IDirectDrawPalette *idp, LPCSTR szBitmap);
extern EXP_IMP_DECL IDirectDrawPalette * DDLoadPalette(IDirectDraw7 *pdd, LPCSTR szBitmap);
extern EXP_IMP_DECL IDirectDrawSurface7 * DDLoadBitmap(IDirectDraw7 *pdd, LPCSTR szBitmap, int dx, int dy, BOOL bUseVideoMem);
extern EXP_IMP_DECL HRESULT              DDReLoadBitmap(IDirectDrawSurface7 *pdds, LPCSTR szBitmap);
extern EXP_IMP_DECL HRESULT              DDCopyBitmap(IDirectDrawSurface7 *pdds, HBITMAP hbm, int x, int y, int dx, int dy);
extern EXP_IMP_DECL DWORD                DDColorMatch(IDirectDrawSurface7 *pdds, COLORREF rgb);
extern EXP_IMP_DECL HRESULT              DDSetColorKey(IDirectDrawSurface7 *pdds, COLORREF rgb);

#ifdef __cplusplus
}
#endif	/* __cplusplus */


//////////////////////////////////////////////////////////////////
// CDDObject
//
// All direct draw objects are derived from this class
//

class CDDObject : public CObject
{
public:
	DECLARE_DYNAMIC(CDDObject);
    CDDObject();
    virtual ~CDDObject();
	HRESULT GetResult() {return m_hr;}

protected:
	HRESULT m_hr; // result of last DD system call
};
	
///////////////////////////////////////////////////////////////////
// CDDEngine
//
// This object encapsulates the global Direct Draw APIs and supports
// a single display device.
// One of these objects is automatically included in your
// application - theDDEngine.
// This implementation does not support multiple display devices.
//

class CDDEngine : public CDDObject
{
public:
	DECLARE_DYNAMIC(CDDEngine);
    CDDEngine();
	virtual ~CDDEngine();
	int GetNumDevices();
	BOOL GetDeviceInfo(int iDev,
					   CString& strName,
					   CString& strDesc);
	HRESULT CreateDirectDraw(IDirectDraw7** ppIDD);

protected:
};

// the one and only DDEngine object
extern CDDEngine theDDEngine;   

///////////////////////////////////////////////////////////////////
// CDDClipper

class CDirectDraw;

class AFX_EXT_CLASS CDDClipper : public CDDObject
{
public:
	DECLARE_DYNAMIC(CDDClipper);
	CDDClipper();
	virtual ~CDDClipper();
	BOOL Create(CDirectDraw* pDD, HWND hWnd);
	IDirectDrawClipper* GetInterface() {return m_pIClip;}
	BOOL			SetClipList(LPCRECT r);
protected:
	IDirectDrawClipper* m_pIClip;
};

///////////////////////////////////////////////////////////////////
// CDDPalette

class CDDPalette : public CDDObject
{
public:
	DECLARE_DYNAMIC(CDDPalette);
	CDDPalette();
	CDDPalette(IDirectDrawPalette* pIPal);
	virtual ~CDDPalette();
	BOOL Create(CDirectDraw* pDD, DWORD dwFlags, PALETTEENTRY* pColorTable);
	IDirectDrawPalette* GetInterface() {return m_pIPal;}
	BOOL GetEntries(int iStart, int iCount, PALETTEENTRY* pColorTable);
	BOOL SetEntries(int iStart, int iCount, PALETTEENTRY* pColorTable);

protected:
	IDirectDrawPalette* m_pIPal;
};

///////////////////////////////////////////////////////////////////
// CDDSurface

#define DRAWFOGLAYER_HALF		RGB(32, 32, 32)
#define DRAWFOGLAYER_DOUBLE		RGB(128, 128, 128)

class AFX_EXT_CLASS CDDSurface : public CDDObject
{
public:
//	int m_DataView[480][320];
	void DrawFogLayer3(int alphaDeapth, WORD color);
	DECLARE_DYNAMIC(CDDSurface);
    CDDSurface();
	CDDSurface(IDirectDrawSurface7* ps);
    virtual ~CDDSurface();
	BOOL					Create(CDirectDraw* pDD, DDSURFACEDESC2* pDesc);

	BOOL					CreateVideoSurface(CDirectDraw* pDD, int width, int height);	//VideoMemory로 생성
	BOOL					Create(CDirectDraw* pDD, int width, int height, BOOL bUseVideoMem=TRUE); //생성시 주로 사용하는 함수
	BOOL					CreateFromHBITMAP(CDirectDraw *pDD, HBITMAP hbmp, BOOL bUseVideoMem=TRUE);
	BOOL					CopyHBITMAP2Surface(HBITMAP hbmp); //HBITMAP을 Surface로 복사
	BOOL					CreateFromImage(CDirectDraw *pDD, LPCSTR szBitmap, BOOL bUseVideoMem=TRUE); //파일로 부터 Surface생성
	DWORD					GetMatchColor(COLORREF color){return DDColorMatch(m_pISurf, color);} //RGB에 맞는 현재설정(보통 16bit) Color값구하기

	IDirectDrawSurface7*	GetInterface() {return m_pISurf;}
	BOOL					GetDescription(DDSURFACEDESC2* pDesc);
	DDSURFACEDESC2&			GetDescription(){return m_SurfDesc;}
	CDDSurface*				GetAttachedSurface(DDSCAPS2* pCaps);
	BOOL					SetClipper(CDDClipper* pClipper);
	BOOL					SetPalette(CDDPalette* pPalette);
	CDDPalette*				GetPalette();
	BOOL					AddAttachedSurface(CDDSurface* pSurface);

	//소스 Surface를 자신의 Surface에 찍음
	BOOL					Blt(CRect *prcTo, CDDSurface *pSorSurface, 
								DWORD dwFlags = DDBLT_WAIT, LPDDBLTFX lpDDBltFx = NULL);
	BOOL					Blt(RECT* prcTo, CDDSurface* pSurFrom, RECT* prcFrom,
								DWORD dwFlags = DDBLT_WAIT, LPDDBLTFX lpDDBltFx = NULL);

	//BltFast : 가속이 없는 경우 10%의 향상효과, Window모드에서 Front Surface에 사용불가
	//가속이 있는 경우 이득이 없음
	BOOL					BltFast(CPoint point, CDDSurface *pSorSurface, DWORD dwTrans);
	BOOL					BltFast(CPoint point, CDDSurface *pSorSurface, CRect *sourceRect, DWORD dwTrans);

	void CDDSurface::BltNormal(CDDSurface *pSorSurface, int x, int y, const RECT *prcSrc, BOOL bUseColorKey, int nRenderType, int nAlpha, COLORREF crColor);

	//Surface의 지정한 Rect영역에 tcolor 톤으로 색깔을 입힘(Alpha효과 이용)
	void					DrawFogLayer(CRect *pRect, COLORREF tcolor=RGB(32,32,32), BOOL bReserveBlack=FALSE);
	void                    DrawFogLayer2(CRect *pRect, COLORREF tcolor=RGB(32,32,32), BOOL bReserveBlack=FALSE);
	BOOL					Flip(DWORD dwFlags = DDFLIP_WAIT);
	CDC*					GetDC();	//Surface를 DC처럼사용하기 TrueType폰트이용시 많이 사용됨(GDI function Call 사용시)
	void					ReleaseDC(CDC* pDC);	//GetDC하면 반드시 ReleaseDC
	void					SetColorKey(DWORD dwFlags, DDCOLORKEY* pDDColorKey);	//ColorKey (Transparent)설정
	BOOL					SetColorKey(COLORREF rgb);								//ColorKey (Transparent)설정
	int						GetWidth() {return m_SurfDesc.dwFlags & DDSD_WIDTH ? m_SurfDesc.dwWidth : 0;}		//Surface의 Width
	int						GetHeight() {return m_SurfDesc.dwFlags & DDSD_HEIGHT ? m_SurfDesc.dwHeight : 0;}	//Surface의 Height
	int						GetPitch() {return m_SurfDesc.dwFlags & DDSD_PITCH ? m_SurfDesc.lPitch : 0;}
	void					GetRect(RECT& rc);	//Surfac의 Rect(0,0,width,height)로 return
	int						GetBitsPerPixel();	
	void					GetRGBMasks(DWORD& r, DWORD& g, DWORD &b);
	void*					Lock(CRect *pRect, DWORD dwFlag);	//surface메모리 잠금 (surface의 MemoryPtr를 가져와서 사용할때)
	void					Unlock(CRect *pRect);				//Surface메모리 풀음
	void*					Lock();								//전체 영역 잠글때
	void					Unlock();							//전체 영역 풀때

	BOOL					SurfaceFill(COLORREF color);		//지정한 색으로 Surface칠하기
	DWORD					GetSemiBlack();						//검정이 아닌 가장 가까운 검정색 가져오기 (Black은 Transparent로 쓰기땜시)

protected:
	DDSURFACEDESC2			m_SurfDesc;		// cached surface description
	IDirectDrawSurface7*	m_pISurf;	// interface to the Direct Draw Surface object
	BOOL					m_bDontDestroy;

	void _UpdateDescription();
};

///////////////////////////////////////////////////////////////////
// CDirectDraw

class AFX_EXT_CLASS CDirectDraw : public CDDObject
{
public:
	DECLARE_DYNAMIC(CDirectDraw);
    CDirectDraw();
    virtual ~CDirectDraw();

	IDirectDraw7*			GetInterface() {return m_pIDD;}
	BOOL					Create();
	BOOL					GetCaps(DDCAPS* pDrvCaps, DDCAPS* pHelCaps = NULL);
	int						GetNumModes();
	BOOL					GetModeInfo(int iMode, DDSURFACEDESC2* pDesc);
	BOOL					SetCooperativeLevel(HWND hWnd, DWORD dwFlags);
	BOOL					SetWindowedMode(HWND hWnd, int cx, int cy)
								{ return _SetMode(hWnd, cx, cy, 0, FALSE);}
	BOOL					SetFullScreenMode(HWND hWnd, int cx, int cy, int bpp)
								{return _SetMode(hWnd, cx, cy, bpp, TRUE);}
	BOOL					IsSWRenderOnly() {return m_bSWRender;}
	void					RestoreMode();
	void					RestoreAllSurfaces();
	CDDSurface*				GetBackBuffer() {return m_pBackBuffer;}
	CDDSurface*				GetFrontBuffer() {return m_pFrontBuffer;}


	void					OnActivate(BOOL bActive);
	CPalette*				GrabPalette();
	CBitmap*				GrabImage();
	HBITMAP					GrabImage(BITMAPINFO** ppBMI, void** ppBits);
	CDDPalette*				GetPalette() {return m_pPalette;}


	static	BOOL			ErrorHandle(HRESULT hr);
	static CString			GetExecutionPath(BOOL bWithBkSlash=TRUE);
protected:
public:
	IDirectDraw7*			m_pIDD;
	CDDSurface*				m_pFrontBuffer;
	CDDSurface*				m_pBackBuffer;
	CDDClipper*				m_pClipper;
	CDDPalette*				m_pPalette;
	BOOL					m_bSWRender;		// TRUE if not all bufs in video mem
	int						m_iWidth;
	int						m_iHeight;
	BOOL					m_bRestore;		// TRUE if mode needs to be restored

	BOOL _SetMode(HWND hWnd, int cx, int cy, int bpp, BOOL bFullScreen);
	void _ReleaseAll();

private:
};

///////////////////////////////////////////////////////////////////
//DirectInput Classes
class CDirectInput;
class CDirectInputDevice;
class BBDX_MouseInformation;

///////////////////////////////////////////////////////////////////
// CDirectInput
class AFX_EXT_CLASS CDirectInput : public CDDObject
{
public:
	DECLARE_DYNAMIC(CDirectInput);
	CDirectInput(HWND hConnectedWnd);
	virtual ~CDirectInput();

	IDirectInput*		GetInterface() {return m_pDirectInput;}
	BOOL				Create(HINSTANCE hInst);
	HWND				GetConnectedHwnd(){return m_hConnectedWnd;}
	BOOL				MouseInputCreate(BOOL bIsWindowed);	
	BOOL				KeyboardInputCreate(BOOL bIsWindowed);

	void	__fastcall	MouseInputCheck(BBDX_MouseInformation *pMouseInfo=NULL); //Update 루틴에서 사용할 함수(Mouse 검사)
	BOOL				KeyboardInputCheck(); //Update 루틴에서 사용할 함수(Keyboard 검사)

	BOOL				OnActivate(BOOL bActive);

	static	BOOL		ErrorHandle(HRESULT hr);
protected:
	IDirectInput		*m_pDirectInput;
	HWND				m_hConnectedWnd;
	CDirectInputDevice	*m_diMouse;
	CDirectInputDevice	*m_diKeyboard;
};


///////////////////////////////////////////////////////////////////
// CDirectInputDevice
#define CDIRECTINPUT_DEVICE_MOUSE		0
#define CDIRECTINPUT_DEVICE_KEYBOARD	1
#define CDIRECTINPUT_DEVICE_JOYSTICK	2//RESERVED : not yet implemented
class AFX_EXT_CLASS CDirectInputDevice : public CDDObject
{
public:
	DECLARE_DYNAMIC(CDirectInputDevice);
	CDirectInputDevice(CDirectInput *pDirectInput, int deviceMode = CDIRECTINPUT_DEVICE_MOUSE);
	virtual ~CDirectInputDevice();

	int					GetDeviceMode() {return m_iDeviceMode;}
	IDirectInputDevice*	GetInterface() {return m_pDirectInputDevice;}
	BOOL				Create(BOOL bIsWindowed);//mouse or keyboard
protected:
	IDirectInputDevice	*m_pDirectInputDevice;
	int					m_iDeviceMode;
	CDirectInput		*m_bpDirectInput;
};

#endif