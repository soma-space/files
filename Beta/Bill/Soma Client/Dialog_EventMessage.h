// Dialog_EventMessage.h: interface for the CDialog_EventMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_EVENTMESSAGE_H__8E7A3604_938E_11D4_826D_00D0B769A081__INCLUDED_)
#define AFX_DIALOG_EVENTMESSAGE_H__8E7A3604_938E_11D4_826D_00D0B769A081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
class BBDX_MouseInformation;
class BBDX_TextOuter;
class CTWFont;

#define    MAX_SELECT_SIZE   4

class CDialog_EventMessage : public CUIRes
{
public:
		CDialog_EventMessage();
		virtual ~CDialog_EventMessage();

public:
		BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
		BOOL Draw(CDDSurface *pDest);
		BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
		BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
		BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
		BOOL OnRButtonDown(BBDX_MouseInformation *pMouseInfo);
		BOOL OnRButtonUp(BBDX_MouseInformation *pMouseInfo);
		BOOL OnLButtonUp2(BBDX_MouseInformation *pMouseInfo);
		int  MouseProcess(BBDX_MouseInformation *pMouseInfo);
		void AddString(PBYTE bypData);
		void ShowWindow();
		void HideWindow();
		void HideAllWindow();
		
		void SendClickNum(int num); //서버에 선택한 번호를 보낸다..
		CFont			*m_pFont;

		bool 			 m_LClick;
		bool 			 m_RClick;
		int              m_SelectStrNum; //실제 서버로부터 받은 선택문항수..
		int              m_SelStringNUm; //선택한 문자열 번호..
		CString			 m_StringMsg;
		CString			 m_SelectStr[MAX_SELECT_SIZE]; //선택문항 최대3개...
		int              m_StringViewMode;
		int              m_FocusSel;
		bool             m_bHaveMsg;

		int				 m_str_len;
		int				 m_charwidth;
		int				 m_charheight;
		int              m_DrawTxtHeight;
		int              m_StartLine;
};

#endif // !defined(AFX_DIALOG_EVENTMESSAGE_H__8E7A3604_938E_11D4_826D_00D0B769A081__INCLUDED_)
