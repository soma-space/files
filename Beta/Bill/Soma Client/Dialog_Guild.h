// Dialog_Guild.h: interface for the CDialog_Guild class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_GUILD_H__152D4C27_947D_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_GUILD_H__152D4C27_947D_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"
//class BBDX_TextOuter;
class BBDX_MouseInformation;
//class CTWFont;

class CDialog_Guild : public CUIRes  
{
public:
	CDialog_Guild();
	virtual ~CDialog_Guild();

	void InitData();
	void DeleteContents();
	void ShowWindow();
	void HideWindow();
	void HideAllWindow();
	void AddMember(CString m_name);

	void SetScrollData();

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal, BOOL bMoveable, BOOL bTrans);
	BOOL Draw(CDDSurface *pDest);

	BOOL OnButtonAction(int nSpec, int nIndex);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnRButtonDown(BBDX_MouseInformation *pMouseInfo);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	void CallBack_MessageBox(int nId, int nValue);

	void SendRemoveMember();
	void RcvRemoveMember(PBYTE bypData);
	void SendChangeTaxRate(int tax_rate);
	void RcvChangeTaxRate(PBYTE bypData);
	void SendChangePos(CString pos_name);
	void RcvChangePos(PBYTE bypData);
	void SendChangePosName(int from, int to);
	void RcvChangePosName(PBYTE bypData);
	void SendDissolveGuild();
	void RcvDissolveGuild(PBYTE bypData);
	void SendDonation(DWORD money);
	void RcvDonation(PBYTE bypData);
	void OpenGuild(PBYTE bypData);
public:
//	BBDX_TextOuter	*m_pGuildNameOuter, *m_pPersonCountOuter, *m_pTaxValueOuter, *m_pGuildMoneyOuter;
//	BBDX_TextOuter	*(m_pPosOuter[10]), *(m_pPosNameOuter[10]), *m_pTextOuter;
	int				m_vPersonCount;
	short			m_vTaxRate;
	DWORD			m_vGuildMoney;
	CString			m_strGuildName;
	CString			m_strGuildPos[9], m_strGuildPosName[9];
	CUIScrollBar	*m_pScrollBar;
//	CTWFont			*m_pTWFont;
	CFont			*m_pFont;
	int				m_vSelIndex, m_vProcessIndex, m_vProcessIndex2;
	CString			m_vProcessStr;
	int				m_vDragIndex, m_vDragFlag;
	CPoint			m_vDragOffset;
	CString			m_vDragName;
	CStringList		m_listGuildPersonName;
	int				m_vGuildIndex;			// 길드 고유 번호
	BYTE			m_vRank;
	BOOL			m_bProcessing, m_bQuit;
	int				m_vFromIndex, m_vToIndex;
	int				m_vMeIndex;
};

#endif // !defined(AFX_DIALOG_GUILD_H__152D4C27_947D_11D4_98B2_00C02615866B__INCLUDED_)
