// Dialog_EventMessage1.h: interface for the CDialog_EventMessage1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_EVENTMESSAGE1_H__FF189DE5_9468_11D4_826D_00D0B769A081__INCLUDED_)
#define AFX_DIALOG_EVENTMESSAGE1_H__FF189DE5_9468_11D4_826D_00D0B769A081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_MouseInformation;
class BBDX_TextOuter;
class CTWFont;

class CDialog_EventMessage1 : public CUIRes  
{
public:
	CDialog_EventMessage1();
	virtual ~CDialog_EventMessage1();

public:
		BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
		BOOL Draw(CDDSurface *pDest);
		BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
		BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
		BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
		BOOL OnRButtonDown(BBDX_MouseInformation *pMouseInfo);
		BOOL OnRButtonUp(BBDX_MouseInformation *pMouseInfo);
		int  MouseProcess(BBDX_MouseInformation *pMouseInfo);
		void AddString(PBYTE bypData);
		void ShowWindow();
		void HideWindow();
		void HideAllWindow();

		CFont			*m_pFont;
		CString			 m_StringMsg;

		bool             m_bHaveMsg;

//		int				 m_str_len;
		int				 m_charwidth;
		int				 m_charheight;
};

#endif // !defined(AFX_DIALOG_EVENTMESSAGE1_H__FF189DE5_9468_11D4_826D_00D0B769A081__INCLUDED_)
