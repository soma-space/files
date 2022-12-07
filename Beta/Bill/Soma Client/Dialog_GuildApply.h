// Dialog_GuildApply.h: interface for the CDialog_GuildApply class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_GUILDAPPLY_H__8850F5A6_9462_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_DIALOG_GUILDAPPLY_H__8850F5A6_9462_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"
//class BBDX_TextOuter;
class BBDX_MouseInformation;
//class CTWFont;

class CDialog_GuildApply : public CUIRes  
{
public:
	CDialog_GuildApply();
	virtual ~CDialog_GuildApply();

	void DialogOpen(PBYTE bypData);
	void RcvApplyGuild(PBYTE bypData);
	void SendApplyGuild();
	void SendAskGuildDesc();
	void RcvGuildDesc(PBYTE bypData);
	void AddGuild(CString m_name);

public:
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);

	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void InitData();
	void ShowWindow(int Id = 0, CUIRes *pParent = NULL);
	void HideWindow();
	void HideAllWindow();
	void DeleteContents();
	void SetScrollData();
	void DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index);
	void ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index);

	CUIScrollBar *m_pScrollBar;
	CStringArray	m_GuildName;
	CStringArray	m_GuildDesc;
	CByteArray		m_GuildDescFlag;
	CString			*m_pGuildName;
	CString			*m_pGuildDesc;
	BOOL			*m_pGuildDescFlag;
	CFont			*m_pFont;
	int				m_vGuildCount;
	int				m_vSelIndex, m_vProcessingIndex;
	BOOL			m_bProcessing;
	int				m_vId;
};

#endif // !defined(AFX_DIALOG_GUILDAPPLY_H__8850F5A6_9462_11D4_98B2_00C02615866B__INCLUDED_)
