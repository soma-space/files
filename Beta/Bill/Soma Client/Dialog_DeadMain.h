// Dialog_DeadMain.h: interface for the CDialog_DeadMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_DEADMAIN_H__AF0718E1_6C6D_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_DEADMAIN_H__AF0718E1_6C6D_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_MouseInformation;

class CDialog_DeadMain : public CUIRes  
{
public:
	CDialog_DeadMain();
	virtual ~CDialog_DeadMain();

public:
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void ShowWindow();

	void StartCountDown();
	void CountDown();

	void SendAliveFromHere();
	void SendAliveFromTown();

	int m_vCountDown;
};

#endif // !defined(AFX_DIALOG_DEADMAIN_H__AF0718E1_6C6D_11D4_98B0_00C02615866B__INCLUDED_)
