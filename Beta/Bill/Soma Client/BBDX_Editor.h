#if !defined(AFX_BBDX_EDITOR_H__B74F9824_D5EC_11D1_8BEE_0060979C5900__INCLUDED_)
#define AFX_BBDX_EDITOR_H__B74F9824_D5EC_11D1_8BEE_0060979C5900__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BBDX_Editor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// BBDX_Editor window

//class CTWFont;
class CDDSurface;
class CUIRes;

class BBDX_Editor : public CEdit
{
// Construction
public:
	BBDX_Editor(CWnd *pParent, CUIRes *resParent, BOOL bModeMulti, CRect area, BOOL readOnly = FALSE, int lineGap = 0, DWORD moreStyle=0L);

// Attributes
public:
	
// Operations
public:
	CSize		GetFontSize() {return m_sizeFont;}
	void		ClearTextBuffer();
	void		ClearTexts();
	void		FontSet();
	void		ChangeEditorMode(BOOL bModeMulti, CRect area);
	void		ChangeArea(CRect area);
	CRect&		GetEditorRect(){return m_vWndRect;}
	const char*	GetCurEditorText(){return (const char*)m_szEditorText;}
	virtual	void	DCDrawCurText(CDDSurface *pSurface, COLORREF color = RGB(255,255,255), BOOL emptyTextCurSorDraw=TRUE, BOOL bPassword = FALSE);
	void			DCDrawOffsetedCurText(CDDSurface *pSurface, CPoint offset=CPoint(0,0), COLORREF color = RGB(255,255,255), BOOL bPassword = FALSE);
	void			DCDrawPosCurText(CDDSurface *pSurface, CPoint pos, COLORREF color = RGB(255,255,255), BOOL bPassword = FALSE);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BBDX_Editor)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetText();
	virtual ~BBDX_Editor();

	BOOL	IsShowCaret();
	virtual void	ReturnkeyProcess();

	BOOL	GetFocusStatus(){return m_bIsFocused;}
	BOOL    m_bChat;
protected:
	BOOL	m_bIsFocused;
	DWORD	m_CaretBlinkTime;
	DWORD	m_TickCount;
	BOOL	m_CaretOn;
	BOOL	m_bRecalcCaretTime;
	BOOL	m_bPassword;

	BOOL	m_bModeMulti;
	CString	m_szEditorText;
	CRect	m_vWndRect, m_vRealRect;
	int		m_vLineGap;
	BOOL	m_bReadOnly;

	CFont	*m_pEditorFont;
	CSize	m_sizeFont;
	CUIRes	*m_pParent;
	// Generated message map functions
protected:
	//{{AFX_MSG(BBDX_Editor)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BBDX_EDITOR_H__B74F9824_D5EC_11D1_8BEE_0060979C5900__INCLUDED_)
