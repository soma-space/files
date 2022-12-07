#include "StdAfx.h"
#include "DirectInput.h"

static LPDIRECTINPUTDEVICE lpDID = NULL;

//////////////////////////////////////////////////////////////////////////////////
// Enumerate all the joystick devices on the system.
//////////////////////////////////////////////////////////////////////////////////
BOOL FAR PASCAL EnumJoystick(LPCDIDEVICEINSTANCE pdinst, LPVOID pvRef)
{
	LPDIRECTINPUT pDI = (LPDIRECTINPUT)pvRef;

	if(pDI->CreateDevice(pdinst->guidInstance, &lpDID, NULL) != DI_OK)
		return DIENUM_CONTINUE;

	return DIENUM_STOP;
}

//////////////////////////////////////////////////////////////////////////////////
// Default constructor. Simply initializes the member elements.
//////////////////////////////////////////////////////////////////////////////////
CDirectInput::CDirectInput(void)
{
	m_lpDI = NULL;
	m_lpDIDKeyboard = NULL;
	m_lpDIDMouse = NULL;

#if DIRECTINPUT_VERSION >= CDX_DIVER
//	LPDIRECTINPUTDEVICE2 m_lpDIDJoystick;
#endif
	m_hWnd = NULL;

	m_bMouse = FALSE;
	m_bKeyboard = FALSE;
//	BOOL m_bJoystick;

	m_nMouseFlag = 0;
	m_nMouseFlagOld = 0; // 마우스 버튼 눌림 플래그
	m_dwTickLBDown = 0; // 마우스 왼쪽 버튼 더블 클릭 감지용
	m_dwTickRBDown = 0; // 마우스 오른쪽 버튼 더블 클릭 감지용

	m_ptCurMouse.x = m_ptCurMouse.y = 0; // 현재 마우스 포인터
	m_ptOldMouse.x = m_ptOldMouse.y = 0; // 직전 마우스 포인터

	SetRect(&m_rcLBDrag, 0, 0, 0, 0); // 드래그 영역
	SetRect(&m_rcMBDrag, 0, 0, 0, 0); // 드래그 영역
	SetRect(&m_rcRBDrag, 0, 0, 0, 0); // 드래그 영역

//	DWORD m_JoystickType;

//	float m_joystickSensitivity;
	
	SetRect(&m_rcMLimit, 0, 0, 0, 0); // 마우스 움직임 제한 영역

///	int m_joystickMinX;
//	int m_joystickMinY;
//	int m_joystickMaxX;
//	int m_joystickMaxY;

//	int m_joystickX, m_joystickY;
//	int m_joystickFreeX, m_joystickFreeY;
//	int m_joystickDeltaX, m_joystickDeltaY;

	MAKE_ZERO(m_byCurKeys); // 현재 키 상태
	MAKE_ZERO(m_byOldKeys); // 직전 키 상태
	MAKE_ZERO(m_bKeyPresses); // 키를 누른 순간인지
	MAKE_ZERO(m_bKeyPresseds); // 키를 눌렀다 떼는 순간인지
	m_bNoKeyDown = FALSE; // 아무 키입력도 없는지
	
	MAKE_ZERO(m_dwTickKeyPress);
}	


//////////////////////////////////////////////////////////////////////////////////
// Default destructor. Releases all devices that accquired
//////////////////////////////////////////////////////////////////////////////////
CDirectInput::~CDirectInput(void)
{
	// shutdown keyboard
	if (m_lpDIDKeyboard)
	{
		UnacquireKeyboard();
		RELEASE_SAFE(m_lpDIDKeyboard);
	}
	
	// shutdown mouse
	if (m_lpDIDMouse)
	{
		UnacquireMouse();
		RELEASE_SAFE(m_lpDIDMouse);
	}
	
	// shutdown joystick
#if DIRECTINPUT_VERSION >= CDX_DIVER
//	if (m_lpDIDJoystick)
//	{
//		UnacquireJoystick();
//		RELEASE_SAFE(m_lpDIDJoystick);
//		m_lpDIDJoystick = NULL;
//	}
#endif

	// kill directinput
	RELEASE_SAFE(m_lpDI);
}

//////////////////////////////////////////////////////////////////////////////////
// Try to accquire all devices. Use SetActiveDevices() if you do not want 
// some devices.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDirectInput::Init(HINSTANCE hInst, HWND hWnd , BOOL ExclusiveMouseAccess)
{
	HRESULT rval;

	m_hWnd = hWnd; // 윈도우 핸들 기억..

	//rval = DirectInputCreate(hInst, DIRECTINPUT_VERSION, &m_lpDI, NULL);
	rval = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                              IID_IDirectInput8, (VOID**)&m_lpDI, NULL );
	if(rval != DI_OK) return FALSE;

	// Create the keyboard device
	rval = m_lpDI->CreateDevice(GUID_SysKeyboard, &m_lpDIDKeyboard, NULL);
	if(rval == DI_OK)
	{
		m_lpDIDKeyboard->SetDataFormat(&c_dfDIKeyboard);
		m_lpDIDKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

		AcquireKeyboard();
		if(!m_bKeyboard) 
			return FALSE;
	}

	// Create the mouse device
	rval = m_lpDI->CreateDevice(GUID_SysMouse, &m_lpDIDMouse, NULL);
	if(rval == DI_OK)
	{
		m_lpDIDMouse->SetDataFormat(&c_dfDIMouse);
		if( ExclusiveMouseAccess == FALSE )
			m_lpDIDMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		else
			m_lpDIDMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

		AcquireMouse();
		if(!m_bMouse) 
			return FALSE;
	}

/*	// Enumerate the joystick device (DirectX 5 or higher)
#if DIRECTINPUT_VERSION >= CDX_DIVER
	rval = m_lpDI->EnumDevices(DIDEVTYPE_JOYSTICK, EnumJoystick, m_lpDI, DIEDFL_ATTACHEDONLY);
	if(lpDID != NULL)
	{
		rval = lpDID->QueryInterface(IID_IDirectInputDevice2, (LPVOID *)&m_lpDIDJoystick);
		if(rval != DI_OK) return FALSE;

		RELEASE_SAFE(lpDID);

		m_lpDIDJoystick->SetDataFormat(&c_dfDIJoystick);
		m_lpDIDJoystick->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

		// Set the X-axis range (-1000 to +1000)
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwObj = DIJOFS_X;
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = -1000;
		diprg.lMax = +1000;

		if(m_lpDIDJoystick->SetProperty(DIPROP_RANGE, &diprg.diph) != DI_OK)
			return FALSE;

		// And again for Y-axis range
		diprg.diph.dwObj = DIJOFS_Y;

		if(m_lpDIDJoystick->SetProperty(DIPROP_RANGE, &diprg.diph) != DI_OK)
			return FALSE;

		// Set X axis dead zone to 10%
		DIPROPDWORD dipdw;
		dipdw.diph.dwSize = sizeof(dipdw);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwObj = DIJOFS_X;
		dipdw.diph.dwHow = DIPH_BYOFFSET;
		dipdw.dwData = 1000;

		if(m_lpDIDJoystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph) != DI_OK)
			return FALSE;

		dipdw.diph.dwObj = DIJOFS_Y;

		// Set Y axis dead zone to 10%
		if(m_lpDIDJoystick->SetProperty(DIPROP_DEADZONE, &dipdw.diph) != DI_OK)
			return FALSE;

		AcquireJoystick();
		if(!m_bJoystick) 
			return FALSE;

        // get the joystick type
        DIDEVICEINSTANCE devinfo;

        ZeroMemory( &devinfo , sizeof( devinfo ) );
        devinfo.dwSize = sizeof( devinfo );
        m_lpDIDJoystick->GetDeviceInfo( &devinfo );
        m_JoystickType = GET_DIDEVICE_SUBTYPE( devinfo.dwDevType );
    }
#endif
*/
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Controls which devices you have accquired.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::SetActiveDevices(BOOL bKeyboard, BOOL bMouse, BOOL bJoystick)
{
	m_bMouse = bMouse;
	m_bKeyboard = bKeyboard;
//	m_bJoystick = bJoystick;

	if (bMouse)
		AcquireMouse();
	else
		UnacquireMouse();

	if (bKeyboard)
		AcquireKeyboard();
	else 
		UnacquireKeyboard();
/*
#if DIRECTINPUT_VERSION >= CDX_DIVER
	if (bJoystick)
		AcquireJoystick();
	else
		UnacquireJoystick();
#endif
*/

}

/////////////////////////////////////////////////////////////////////////////////////////////
// Sets the axis mode of the joystick.
/////////////////////////////////////////////////////////////////////////////////////////////
/*
void CDirectInput::SetJoystickAbs(void)
{
#if DIRECTINPUT_VERSION >= CDX_DIVER
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_ABS;
	m_lpDIDJoystick->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
#endif
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////
// Runs the DirectInput control panel associated with the mouse. If the device does not 
// have a control panel associated with it, the default device control panel is launched.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::RunMouseControlPanel(HWND hWnd)
{
	m_lpDIDMouse->RunControlPanel(hWnd, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Runs the DirectInput control panel associated with the joystick. If the device does not 
// have a control panel associated with it, the default device control panel is launched.
/////////////////////////////////////////////////////////////////////////////////////////////
/*
void CDirectInput::RunJoystickControlPanel(HWND hWnd)
{
#if DIRECTINPUT_VERSION >= CDX_DIVER
	m_lpDIDJoystick->RunControlPanel(hWnd, 0);
#endif
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////
// Wipes out the internal key data.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::KeyboardFlushData()
{
	memset(m_byOldKeys, 0, NUMDIKEYS);
	memset(m_byCurKeys, 0, NUMDIKEYS);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// This restricts the mouse to a defined area.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::MouseSetLimits(int x1, int y1, int x2, int y2)
{
	m_rcMLimit.left = x1;
	m_rcMLimit.top = y1;
	m_rcMLimit.right = x2;
	m_rcMLimit.bottom = y2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// This restricts the joystick to a defined area.
/////////////////////////////////////////////////////////////////////////////////////////////
/*
void CDirectInput::SetJoystickLimits(int x1, int y1, int x2, int y2)
{
	m_joystickMinX = x1;
	m_joystickMinY = y1;
	m_joystickMaxX = x2;
	m_joystickMaxY = y2;
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////
// Sets the mouse position. This restricts the position to the physical display.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::MouseSetPos(int x, int y)
{
	// clamp non-free mouse values to limits
	if ((m_ptCurMouse.x = x) >= m_rcMLimit.right)
		m_ptCurMouse.x = m_rcMLimit.right-1;

	if ((m_ptCurMouse.y = y) >= m_rcMLimit.bottom)
		m_ptCurMouse.y = m_rcMLimit.bottom-1;

	if ((m_ptCurMouse.x = x) <= m_rcMLimit.left)
		m_ptCurMouse.x = m_rcMLimit.left+1;

	if ((m_ptCurMouse.y = y) <= m_rcMLimit.top)
		m_ptCurMouse.y = m_rcMLimit.top+1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Sets the joystick position. This restricts the position to the physical display.
/////////////////////////////////////////////////////////////////////////////////////////////
/*
void CDirectInput::SetJoystickPos(int x, int y)
{
	// clamp non-free joystick values to limits
	if ((m_joystickX = x) >= m_joystickMaxX)
		m_joystickX = m_joystickMaxX-1;

	if ((m_joystickY = y) >= m_joystickMaxY)
		m_joystickY = m_joystickMaxY-1;

	if ((m_joystickX = x) <= m_joystickMinX)
		m_joystickX = m_joystickMinX+1;

	if ((m_joystickY = y) <= m_joystickMinY)
		m_joystickY = m_joystickMinY+1;

	// Zero out the Deltas
	m_joystickDeltaX = 0;
	m_joystickDeltaY = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Sets the joystick position. This allows you to set the joystick to anywhere (no limits).
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::SetJoystickFreePos(int x, int y)
{
	m_joystickFreeX  = x;
	m_joystickFreeY  = y;

	// Zero out the Deltas
	m_joystickDeltaX = 0;
	m_joystickDeltaY = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Sets the Joystick sensitivity factor.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::SetJoystickSensitivity(float factor)
{
	m_joystickSensitivity = factor;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Retrieves the current Joystick sensitivity factor.
/////////////////////////////////////////////////////////////////////////////////////////////
float CDirectInput::GetJoystickSensitivity()
{
	return(m_joystickSensitivity);
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////
// Retrieves the keystate of a given key.
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDirectInput::KeyboardGetKeyState(int nDIKey)
{
	if(nDIKey < 0 || nDIKey >= NUMDIKEYS) return FALSE;
	return(m_byCurKeys[nDIKey]);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Retrieves the joystick position and stores the x and y values in the passed parameters.
/////////////////////////////////////////////////////////////////////////////////////////////
/*
void CDirectInput::GetJoystickPos(LONG* x, LONG* y)
{
	*x = m_joystickX;
	*y = m_joystickY;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Retrieves the free joystick position and stores the x and y values in the passed parameters.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::GetJoystickFreePos(LONG* x, LONG* y)
{
	*x = m_joystickFreeX;
	*y = m_joystickFreeY;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Retrieves the joystick deltas and stores the x and y values in the passed parameters.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::GetJoystickDeltas(LONG* x, LONG* y)
{
	*x = m_joystickDeltaX;
	*y = m_joystickDeltaY;
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////
// Obtains access to the keyboard.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::AcquireKeyboard()
{
	if (m_lpDIDKeyboard)
	{
		HRESULT rval = m_lpDIDKeyboard->Acquire();
//		if(rval != DI_OK) MessageBox(NULL, "Acquire Keyboard Failed.", "DirectInput", MB_OK);
		if (rval == DI_OK || rval == S_FALSE)
		{
			m_bKeyboard = TRUE;
			KeyboardFlushData();
			return;
		}
	}	
//	m_bKeyboard = FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Releases access to the keyboard.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::UnacquireKeyboard()
{
	KeyboardFlushData();
	m_bKeyboard = FALSE;
	if (m_lpDIDKeyboard)
	{
		HRESULT rval = m_lpDIDKeyboard->Unacquire();
//		if(rval != DI_OK) MessageBox(NULL, "UnAcquire Keyboard Failed.", "DirectInput", MB_OK);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Obtains access to the mouse.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::AcquireMouse()
{
	if (m_lpDIDMouse)
	{
		HRESULT rval = m_lpDIDMouse->Acquire();
//		if(rval != DI_OK) MessageBox(NULL, "Acquire Mouse Failed.", "DirectInput", MB_OK);
		if (rval == DI_OK || rval == S_FALSE)
		{
			m_bMouse = TRUE;
			return;
		}
	}
//	m_bMouse = FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Releases access to the mouse.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::UnacquireMouse()
{
	m_bMouse = FALSE;
	if (m_lpDIDMouse)
	{
		HRESULT rval = m_lpDIDMouse->Unacquire();
//		if(rval != DI_OK) MessageBox(NULL, "UnAcquire Mouse Failed.", "DirectInput", MB_OK);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Obtains access to the joystick.
/////////////////////////////////////////////////////////////////////////////////////////////
/*
void CDirectInput::AcquireJoystick()
{
#if DIRECTINPUT_VERSION >= CDX_DIVER
	if (m_lpDIDJoystick)
	{
		HRESULT rval = m_lpDIDJoystick->Acquire();
//		if(rval != DI_OK) MessageBox(NULL, "Acquire Joystick Failed.", "DirectInput", MB_OK);
		if (rval == DI_OK || rval == S_FALSE)
		{
			m_bJoystick = TRUE;
			return;
		}
	}
//	m_bJoystick = FALSE;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Releases access to the joystick.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::UnacquireJoystick()
{
#if DIRECTINPUT_VERSION >= CDX_DIVER
	m_bJoystick = FALSE;
	if (m_lpDIDJoystick)
	{
		HRESULT rval = m_lpDIDJoystick->Unacquire();
//		if(rval != DI_OK) MessageBox(NULL, "UnAcquire Joystick Failed.", "DirectInput", MB_OK);
	}
#endif
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////
// Updates all devices. Call this before you check for input.
/////////////////////////////////////////////////////////////////////////////////////////////
// 되도록이면 전체 프로시저 돌때 한번씩만 도는게 좋다.. 여러번 하면 혼란이 올수도 있다.
void CDirectInput::Update(void)
{
	HRESULT err;
//	int oldx, oldy;
//	BOOL isPressed;
//	WORD i;
//	DWORD key;
/*
#if DIRECTINPUT_VERSION >= CDX_DIVER
	DIJOYSTATE JoyState;
#endif
*/
	///////////////////////
	//  KEYBOARD
	///////////////////////
	if(m_bKeyboard)
	{
		memcpy(m_byOldKeys, m_byCurKeys, NUMDIKEYS); // 전의 키 상태 기록
		err = m_lpDIDKeyboard->GetDeviceState(NUMDIKEYS, m_byCurKeys); // 현재 키 상태 기록
		if(err != DI_OK)
		{
			AcquireKeyboard();
			return;
		}
		
		m_bNoKeyDown = TRUE; // 첨엔 아무것도 안눌림
		for(int i = 0; i < NUMDIKEYS; i++)
		{
			if(!m_byOldKeys[i] && m_byCurKeys[i]) m_bKeyPresses[i] = TRUE; // 눌리는 순간
			else m_bKeyPresses[i] = FALSE;
			
			if(m_byOldKeys[i] && !m_byCurKeys[i]) m_bKeyPresseds[i] = TRUE; // 눌렀다 떼는 순간..
			else m_bKeyPresseds[i] = FALSE;
			
			if(m_byCurKeys[i]) m_bNoKeyDown = FALSE;
		}
	}

	///////////////////////
	//  MOUSE
	///////////////////////
	if(m_bMouse)
	{
//		DIMOUSESTATE DIMState;
//		err = m_lpDIDMouse->GetDeviceState(sizeof(DIMState), &DIMState); // 마우스 상태 얻기 - 이상타.. 자꾸.. 에러가..
//		if(err != DI_OK)
//		{
//			AcquireMouse();
//			return;
//		}

		// 마우스 위치 새로 가져오기
//		m_ptCurMouse.x += DIMState.lX;
//		m_ptCurMouse.y += DIMState.lY;

//		if((DIMState.rgbButtons[0] & 0x80)) m_bLBDown = TRUE; 
//		else m_bLBDown = FALSE;
//		if((DIMState.rgbButtons[1] & 0x80)) m_bRBDown = TRUE;
//		else m_bRBDown = FALSE;

		// clamp non-free mouse values to limits
//		if (m_ptCurMouse.x >= m_rcMLimit.right)
//			m_ptCurMouse.x = m_rcMLimit.right-1;
//
//		if (m_ptCurMouse.y >= m_rcMLimit.bottom)
//			m_ptCurMouse.y = m_rcMLimit.bottom-1;
//
//		if (m_ptCurMouse.x < m_rcMLimit.left)
//			m_ptCurMouse.x = m_rcMLimit.left;
//
//		if (m_ptCurMouse.y < m_rcMLimit.top)
//			m_ptCurMouse.y = m_rcMLimit.top;

		::GetCursorPos(&m_ptCurMouse); // 좀 이상해서... 그냥 시스템 마우스 커서 위치 가져오기
		::ScreenToClient(m_hWnd, &m_ptCurMouse); // 클라이언트 영역으로 변환

		// 마우스 버튼 상태 보관.
		m_nMouseFlagOld = m_nMouseFlag;
		m_nMouseFlag = 0;
		
		// 마우스 상태 가져오기
		if(_IsKeyDown(VK_LBUTTON)) m_nMouseFlag |= MOUSE_LBDOWN;
		if(_IsKeyDown(VK_MBUTTON)) m_nMouseFlag |= MOUSE_MBDOWN;
		if(_IsKeyDown(VK_RBUTTON)) m_nMouseFlag |= MOUSE_RBDOWN;

		// 버튼 클릭 직후..
		if((m_nMouseFlagOld & MOUSE_LBDOWN) == FALSE && (m_nMouseFlag & MOUSE_LBDOWN)) m_nMouseFlag |= MOUSE_LBCLICK;
		if((m_nMouseFlagOld & MOUSE_MBDOWN) == FALSE && (m_nMouseFlag & MOUSE_MBDOWN)) m_nMouseFlag |= MOUSE_MBCLICK;
		if((m_nMouseFlagOld & MOUSE_RBDOWN) == FALSE && (m_nMouseFlag & MOUSE_RBDOWN)) m_nMouseFlag |= MOUSE_RBCLICK;
		
		// 버튼에서 손을 떼면
		if((m_nMouseFlagOld & MOUSE_LBDOWN) && (m_nMouseFlag & MOUSE_LBDOWN) == FALSE) m_nMouseFlag |= MOUSE_LBCLICKED;
		if((m_nMouseFlagOld & MOUSE_MBDOWN) && (m_nMouseFlag & MOUSE_MBDOWN) == FALSE) m_nMouseFlag |= MOUSE_MBCLICKED;
		if((m_nMouseFlagOld & MOUSE_RBDOWN) && (m_nMouseFlag & MOUSE_RBDOWN) == FALSE) m_nMouseFlag |= MOUSE_RBCLICKED;
		
		static DWORD dwDblClk = GetDoubleClickTime(); // 윈도우의 더블 클릭시간을 가져오고..
		if(m_nMouseFlag & MOUSE_LBCLICKED) // 왼쪽 더블 클릭 감지
		{
			static DWORD dwCLicked = 0;
			if(GetTickCount() < dwCLicked + dwDblClk) m_nMouseFlag |= MOUSE_LBDBLCLK;
			dwCLicked = GetTickCount();
		}
		if(m_nMouseFlag & MOUSE_MBCLICKED) // 왼쪽 더블 클릭 감지
		{
			static DWORD dwCLicked = 0;
			if(GetTickCount() < dwCLicked + dwDblClk) m_nMouseFlag |= MOUSE_MBDBLCLK;
			dwCLicked = GetTickCount();
		}
		if(m_nMouseFlag & MOUSE_RBCLICKED) // 왼쪽 더블 클릭 감지
		{
			static DWORD dwCLicked = 0;
			if(GetTickCount() < dwCLicked + dwDblClk) m_nMouseFlag |= MOUSE_RBDBLCLK;
			dwCLicked = GetTickCount();
		}

		// 드래그 영역 처리
		if(m_nMouseFlag & MOUSE_LBDOWN) { m_rcLBDrag.right = m_ptCurMouse.x; m_rcLBDrag.bottom = m_ptCurMouse.y; }
		if(m_nMouseFlag & MOUSE_MBDOWN) { m_rcMBDrag.right = m_ptCurMouse.x; m_rcMBDrag.bottom = m_ptCurMouse.y; }
		if(m_nMouseFlag & MOUSE_RBDOWN) { m_rcRBDrag.right = m_ptCurMouse.x; m_rcRBDrag.bottom = m_ptCurMouse.y; }
		if(m_nMouseFlag & MOUSE_LBCLICK) { m_rcLBDrag.left = m_ptCurMouse.x; m_rcLBDrag.top = m_ptCurMouse.y; }
		if(m_nMouseFlag & MOUSE_MBCLICK) { m_rcMBDrag.left = m_ptCurMouse.x; m_rcMBDrag.top = m_ptCurMouse.y; }
		if(m_nMouseFlag & MOUSE_RBCLICK) { m_rcRBDrag.left = m_ptCurMouse.x; m_rcRBDrag.top = m_ptCurMouse.y; }
	} // end of mouse


	///////////////////////
	//  JOYSTICK
	///////////////////////
/*
#if DIRECTINPUT_VERSION >= CDX_DIVER
	if(m_bJoystick)
	{
		m_lpDIDJoystick->Poll();

		err = m_lpDIDJoystick->GetDeviceState(sizeof(JoyState), &JoyState);
		if(err != DI_OK)
		{
			m_lpDIDJoystick->Acquire();
			return;
		}
		else
		{
			// get new joystick position status
			oldx			= m_joystickFreeX;
			oldy			= m_joystickFreeY;
			m_joystickFreeX += (LONG)((float)JoyState.lX * m_joystickSensitivity);
			m_joystickFreeY += (LONG)((float)JoyState.lY * m_joystickSensitivity);
			m_joystickX		+= (LONG)((float)JoyState.lX * m_joystickSensitivity);
			m_joystickY		+= (LONG)((float)JoyState.lY * m_joystickSensitivity);

			// clamp non-free joystick values to limits
			if (m_joystickX >= m_joystickMaxX)
				m_joystickX = m_joystickMaxX-1;

			if (m_joystickY >= m_joystickMaxY)
				m_joystickY = m_joystickMaxY-1;

			if (m_joystickX <= m_joystickMinX)
				m_joystickX = m_joystickMinX;

			if (m_joystickY <= m_joystickMinY)
				m_joystickY = m_joystickMinY;

			m_joystickDeltaX = m_joystickFreeX - oldx;
			m_joystickDeltaY = m_joystickFreeY - oldy;
	
			// check the joystick buttons
			for(i=0; i<10; i++)
			{
				key = i;

				// check current key state (bit 8 flipped on means key is pressed)
				if (JoyState.rgbButtons[i] & (BYTE)0x80)
					isPressed = TRUE;
				else
					isPressed = FALSE;

			}
		}
	}
#endif
*/
}

/////////////////////////////////////////////////////////////////////////////////////////////
// returns the number of buttons the joystick has
/////////////////////////////////////////////////////////////////////////////////////////////
/*
int CDirectInput::GetNumberOfJoystickButtons( void )
{
#if DIRECTINPUT_VERSION >= CDX_DIVER

    DIDEVCAPS Caps;

    // check if joystick exists
	if(m_bJoystick)
	{
        ZeroMemory( &Caps , sizeof( Caps ) );
        Caps.dwSize = sizeof( Caps );    
        if( m_lpDIDJoystick->GetCapabilities( &Caps ) == DI_OK )
            return Caps.dwButtons ;
        else
            return 0;
    }
    else
        return 0;
#else
    return 0;
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////
// returns TRUE, if the control is a gamepad
// this is useful if you want to know if the joystick is analog or digital
// so you can check for the correct axes values
/////////////////////////////////////////////////////////////////////////////////////////////
BOOL CDirectInput::IsGamePad( void )
{
    if( m_JoystickType == DIDEVTYPEJOYSTICK_GAMEPAD )
        return TRUE;
    else
        return FALSE;
}
*/
// 마우스 버튼 입력 몽땅 무효화한다.
void CDirectInput::MouseClearClick()
{
	m_nMouseFlag = m_nMouseFlagOld = 0;
}

void CDirectInput::KeyboardClearKeyInput()
{
	MAKE_ZERO(m_byOldKeys);
	MAKE_ZERO(m_byCurKeys);
}
