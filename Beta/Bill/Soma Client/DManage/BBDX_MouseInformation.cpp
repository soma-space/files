// BBDX_MouseInformation.cpp: implementation of the BBDX_MouseInformation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BBDX_MouseInformation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BBDX_MouseInformation::BBDX_MouseInformation()
{
	m_bLButtonPressed = FALSE;
	m_bRButtonPressed = FALSE;
	m_bLButtonDblClk = FALSE;
	m_bLButtonDblFlag = TRUE;
	m_bLButtonPressing = FALSE;
	m_bRButtonPressing = FALSE;
	m_bLButtonUp = FALSE;
	m_bRButtonUp = FALSE;
	m_posMouse.x = SCREEN_WIDTH/2;
	m_posMouse.y = SCREEN_HEIGHT/2;

	Backup();
	m_LTickCount = GetTickCount();
	m_pressingContCount = 0;
}

BBDX_MouseInformation::~BBDX_MouseInformation()
{
}

void BBDX_MouseInformation::Backup()
{
	m_p_bLButtonPressed = m_bLButtonPressed;
	m_p_bRButtonPressed = m_bRButtonPressed;
	m_p_posMouse = m_posMouse;
}

BOOL	BBDX_MouseInformation::IsLButtonDobleDown()
{
/*	if (!m_p_bLButtonPressed&&m_bLButtonPressed) 
	{
		DWORD	tmp_TickCount = GetTickCount();
		if((tmp_TickCount - m_TickCount) < 300)
		{
//			m_TickCount = GetTickCount();
			m_pressingContCount = 0;
			return TRUE;
		}
	}
	return FALSE;*/
	return m_bLButtonDblClk;
}


BOOL	BBDX_MouseInformation::IsLButtonDown()
{
/*	if (!m_p_bLButtonPressed&&m_bLButtonPressed) 
	{
//		DWORD	tmp_TickCount = GetTickCount();
//		if((tmp_TickCount - m_TickCount) >= 200)//300)
//		{
			m_TickCount = GetTickCount();
			m_pressingContCount = 0;
			return TRUE;
//		}
	}
	return FALSE;*/
	return m_bLButtonPressed;
}

BOOL BBDX_MouseInformation::IsLButtonUp()
{
/*	if (m_p_bLButtonPressed&&!m_bLButtonPressed) return TRUE;
	return FALSE;*/
	return m_bLButtonUp;
}

BOOL BBDX_MouseInformation::IsLButtonPressing()
{
/*	BOOL ret_val;
	ret_val = m_bLButtonPressed&&(m_bLButtonPressed==m_p_bLButtonPressed);
	if (ret_val)
	{
		DWORD curTick = GetTickCount();
		DWORD delay = MI_PRESSING_DELAY;
//		DWORD minusGap = m_pressingContCount*MI_PRESSING_DELAY_DECREASE_UNIT;
//		if (minusGap>MI_PRESSING_DELAY-MI_PRESSING_MININUMDELAY) minusGap = MI_PRESSING_DELAY-MI_PRESSING_MININUMDELAY;
//		delay -= minusGap;
		if (curTick-m_TickCount>delay) 
		{
			m_TickCount = curTick;
			m_pressingContCount++;
		}
		else ret_val = FALSE;
	}
	return ret_val;*/
	return m_bLButtonPressing;
}

BOOL BBDX_MouseInformation::IsLButtonPressing2()
{
/*	BOOL ret_val;
	ret_val = m_bLButtonPressed&&(m_bLButtonPressed==m_p_bLButtonPressed);
	if (ret_val)
	{
		DWORD curTick = GetTickCount();
		DWORD delay = 0;
//		DWORD minusGap = m_pressingContCount*MI_PRESSING_DELAY_DECREASE_UNIT;
//		if (minusGap>MI_PRESSING_DELAY-MI_PRESSING_MININUMDELAY) minusGap = MI_PRESSING_DELAY-MI_PRESSING_MININUMDELAY;
//		delay -= minusGap;
		if (curTick-m_TickCount>delay) 
		{
			m_TickCount = curTick;
			m_pressingContCount++;
		}
		else ret_val = FALSE;
	}
	return ret_val;*/
	return m_bLButtonPressing;
}

BOOL	BBDX_MouseInformation::IsRButtonDown()
{
/*	if (!m_p_bRButtonPressed&&m_bRButtonPressed) 
	{
		m_TickCount = GetTickCount();
		m_pressingContCount=0;
		return TRUE;
	}
	return FALSE;*/
	return m_bRButtonPressed;
}

BOOL BBDX_MouseInformation::IsRButtonUp()
{
/*	if (m_p_bRButtonPressed&&!m_bRButtonPressed) return TRUE;
	return FALSE;*/
	return m_bRButtonUp;
}

BOOL BBDX_MouseInformation::IsRButtonPressing()
{
/*	BOOL ret_val;
	ret_val = m_bRButtonPressed&&(m_bRButtonPressed==m_p_bRButtonPressed);
	if (ret_val)
	{
		DWORD curTick = GetTickCount();
		DWORD delay = MI_PRESSING_DELAY;
		DWORD minusGap = m_pressingContCount*MI_PRESSING_DELAY_DECREASE_UNIT;
		if (minusGap>MI_PRESSING_DELAY-MI_PRESSING_MININUMDELAY) minusGap = MI_PRESSING_DELAY-MI_PRESSING_MININUMDELAY;
		delay -= minusGap;
		if (curTick-m_TickCount>delay)
		{
			m_TickCount = curTick;
			m_pressingContCount++;
		}
	}
	return ret_val;*/
	return m_bRButtonPressing;
}

BOOL BBDX_MouseInformation::IsMouseMove()
{
	if (m_posMouse!=m_p_posMouse) return TRUE;
	return FALSE;
}

BOOL BBDX_MouseInformation::SetMouseState(DIMOUSESTATE *dims)
{
	Backup();
	DWORD tick_count = GetTickCount();
	
	m_bLButtonDblClk = FALSE;
	if(dims->rgbButtons[MI_LBUTTONSTATE]&0x80)
	{
		if(m_bLButtonPressing == FALSE)
		{
			if(m_bLButtonDblFlag == FALSE)
			{
				if(tick_count - m_LTickCount < 300)
				{
					m_bLButtonDblFlag = TRUE;
					m_bLButtonDblClk = TRUE;
				}
			}
			else
			{
				m_bLButtonDblFlag = FALSE;
			}
			m_LTickCount = tick_count;
			m_bLButtonPressed = TRUE;
			m_bLButtonPressing = TRUE;
		}
		else
		{
			m_bLButtonPressed = FALSE;
		}
	}
	else
	{
		if(m_bLButtonPressing == TRUE)
		{
			m_bLButtonPressing = FALSE;
			m_bLButtonUp = TRUE;
		}
		else m_bLButtonUp = FALSE;
		m_bLButtonPressed = FALSE;
	}
	if(dims->rgbButtons[MI_RBUTTONSTATE]&0x80)
	{
		if(m_bRButtonPressing == FALSE)
		{
			m_RTickCount = tick_count;
			m_bRButtonPressed = TRUE;
			m_bRButtonPressing = TRUE;
		}
		else m_bRButtonPressed = FALSE;
	}
	else
	{
		if(m_bRButtonPressing == TRUE)
		{
			m_bRButtonPressing = FALSE;
			m_bRButtonUp = TRUE;
		}
		else m_bRButtonUp = FALSE;
		m_bRButtonPressed = FALSE;
	}

	if (dims->lX==0&&dims->lY==0) return TRUE;

	if (m_bWindowed)
	{
		GetCursorPos(&m_posMouse);
		::ScreenToClient(m_hwnd, &m_posMouse);
	}
	else
	{
		m_posMouse.x += dims->lX;// *2;//+ dims->lX/5*3;
		m_posMouse.y += dims->lY;// *2;//+ dims->lY/5*3;
	}

	if (m_posMouse.x<0) m_posMouse.x = 0;
	else if (m_posMouse.x>SCREEN_WIDTH) m_posMouse.x = SCREEN_WIDTH;
	if (m_posMouse.y<0) m_posMouse.y = 0;
	else if (m_posMouse.y>SCREEN_HEIGHT) m_posMouse.y = SCREEN_HEIGHT;

	return TRUE;
}
