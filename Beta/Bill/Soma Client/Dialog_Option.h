// Dialog_Option.h: interface for the CDialog_Option class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_OPTION_H__128BEAE1_6DFD_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_OPTION_H__128BEAE1_6DFD_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_MouseInformation;
class CUIHorzScrollBar;

class CDialog_Option : public CUIRes  
{
public:
	CDialog_Option();
	virtual ~CDialog_Option();

public:
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void ShowWindow();
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = TRUE, BOOL bMoveable = TRUE, BOOL bTrans = TRUE);

	int		m_vAreaFocus;
	CUIHorzScrollBar *m_pSoundScrollBar, *m_pMusicScrollBar, *m_pGammaScrollBar;
	long	m_vMusicVolume;
	long	m_vSoundVolume;
	long	m_vGammaRate;//this
};

#endif // !defined(AFX_DIALOG_OPTION_H__128BEAE1_6DFD_11D4_98B0_00C02615866B__INCLUDED_)
