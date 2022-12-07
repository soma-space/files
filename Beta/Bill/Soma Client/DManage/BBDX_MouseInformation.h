// BBDX_MouseInformation.h: interface for the BBDX_MouseInformation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BBDX_MOUSEINFORMATION_H__0F83B1E4_C33F_11D1_8BEE_0060979C5900__INCLUDED_)
#define AFX_BBDX_MOUSEINFORMATION_H__0F83B1E4_C33F_11D1_8BEE_0060979C5900__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MI_LBUTTONSTATE 0
#define MI_RBUTTONSTATE 1

#define MI_PRESSING_DELAY				700 //30
#define MI_PRESSING_DELAY_DECREASE_UNIT 0
#define MI_PRESSING_MININUMDELAY		20

#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600

//#define SCREEN_WIDTH	640
//#define SCREEN_HEIGHT	480

class AFX_EXT_CLASS BBDX_MouseInformation  
{
public:
	BBDX_MouseInformation();
	virtual ~BBDX_MouseInformation();

	void	SetHwndAndWindowed(HWND hwnd, BOOL bIsWindowed) {m_hwnd = hwnd; m_bWindowed = bIsWindowed;}

	BOOL	SetMouseState(DIMOUSESTATE *dims);
	BOOL	GetLButtonState() {return m_bLButtonPressed;}
	BOOL	GetRButtonState() {return m_bRButtonPressed;}
	CPoint&	GetPos() {return m_posMouse;}

	BOOL	IsLButtonDown();
	BOOL	IsLButtonUp();
	BOOL	IsLButtonPressing();
	BOOL	IsLButtonPressing2();
	BOOL    IsLButtonDobleDown();

	BOOL	IsRButtonDown();
	BOOL	IsRButtonUp();
	BOOL	IsRButtonPressing();

	BOOL	IsMouseMove();

	void	Backup();

//orginal data
	BOOL	m_bLButtonPressed;
	BOOL	m_bRButtonPressed;
	BOOL	m_bLButtonDblClk;
	BOOL	m_bLButtonDblFlag;
	BOOL	m_bLButtonPressing;
	BOOL	m_bRButtonPressing;
	BOOL	m_bLButtonUp;
	BOOL	m_bRButtonUp;
	CPoint	m_posMouse;

//previous data
	BOOL	m_p_bLButtonPressed;
	BOOL	m_p_bRButtonPressed;
	CPoint	m_p_posMouse;
protected:
	BOOL	m_bWindowed;
	HWND	m_hwnd;
private:
	DWORD	m_LTickCount, m_RTickCount;
	int		m_pressingContCount;
};

#endif // !defined(AFX_BBDX_MOUSEINFORMATION_H__0F83B1E4_C33F_11D1_8BEE_0060979C5900__INCLUDED_)
