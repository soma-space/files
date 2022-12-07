// BBDX_ChatOuter.h: interface for the BBDX_ChatOuter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BBDX_CHATOUTER_H__985208E2_1719_11D2_8BEE_0060979C5900__INCLUDED_)
#define AFX_BBDX_CHATOUTER_H__985208E2_1719_11D2_8BEE_0060979C5900__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDirectDraw;

#define COCK_NORMAL			1
#define COCK_FAMILY			2
#define COCK_GLOBAL			3
#define COCK_TOSOMEONE		4
#define COCK_MESSAGE		5
#define COCK_NOTICE			6

class BBDX_ChatOuter
{
public:
	BBDX_ChatOuter(CString fontname, int height, CRect area, int maxLine, int lineGap=0);

	virtual ~BBDX_ChatOuter();

	void			DrawChat(CDDSurface *dest, CPoint offset=CPoint(0,0), CDC *pdc = NULL);
	int				AddChatWithKind(CString &str, int chatKind);//Return값은 maxline넘었을경우 그만큼 제외시킴
	int				AddMemo(BOOL bMe, CString chat);

	void			ChatAreaChange(CRect r);
	int				CalcShowLineNum();
	int				GetRealShowLineNum() {return m_vRealShowLineNum;};

	void			SetScrollPos(int pos);
	int				GetLineCount() {return m_ChatList.GetCount();}
	int				GetShowLineNum() {return m_iShowLineNum;}
	int				GetScrollPos() {return m_iScrollPos;}
	void			ScrollDown();
	void			ScrollUp();
	BOOL			ScrollPageDown();
	void			ScrollPageUp();
	void			ClearString();
	void			ShowMode(int show_mode);

protected:
	void			ResetStrLists();
	void			ResetIntLists();

protected:
	CFont			*m_pMyFont;
	int				m_iShowLineNum;
	int				m_iScrollPos;
	CRect			m_vAreaRect;
	int				m_vCharHeight;
	int				m_vMaxLine;
	int				m_vLineGap;
	int				m_vEndLine;
	int				m_vRealShowLineNum;
	int				m_iCount;
	CStringList		m_ChatList;
	CList<int,int>	m_ChatKindList;
	CArray<int,int>	m_ChatLineCount;
	COLORREF		m_ChatColor[7];
	DWORD			m_vChatTime;
	int				m_vShowMode;
	int				m_vOldScrollPos;
};

#endif // !defined(AFX_BBDX_CHATOUTER_H__985208E2_1719_11D2_8BEE_0060979C5900__INCLUDED_)
