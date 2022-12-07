// Notice.h: interface for the CNotice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTICE_H__9C7D0CC1_D8DD_11D4_826D_00D0B769A081__INCLUDED_)
#define AFX_NOTICE_H__9C7D0CC1_D8DD_11D4_826D_00D0B769A081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"



class CNotice : public CUIRes  
{
public:
	CNotice();
	virtual ~CNotice();

public:
		BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
		BOOL Draw(CDDSurface *pDest);
		BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
		BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
		BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
		int  MouseProcess(BBDX_MouseInformation *pMouseInfo);
		void AddString(PBYTE bypData);
		void ShowWindow();
		void HideWindow();
		void HideAllWindow();

		void KillFocus();
		void SetFocus();

		CFont			*m_pFont;
		CString			 m_StringMsg;

		bool             m_bHaveMsg;
		bool             m_bExit;

		int				 m_charwidth;
		int				 m_charheight;
};

#endif // !defined(AFX_NOTICE_H__9C7D0CC1_D8DD_11D4_826D_00D0B769A081__INCLUDED_)
