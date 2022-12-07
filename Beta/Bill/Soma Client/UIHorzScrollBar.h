// UIHorzScrollBar.h: interface for the CUIHorzScrollBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIHORZSCROLLBAR_H__7408B4E2_96BD_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_UIHORZSCROLLBAR_H__7408B4E2_96BD_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DManage/BB_Direct.h"
#include "UIRes.h"

class BBDX_MouseInformation;

class CUIHorzScrollBar  
{
public:
	CUIHorzScrollBar();
	virtual ~CUIHorzScrollBar();

	void InitScroll();

	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL Create(CUIRes *pUIRes, CRect rtScroll, int nResRectIndex);

	void Draw(CDDSurface *pDest);
	
	int GetMaxLineCount() {return m_nMaxLineCount;};
	void SetMaxLineCount(int nMaxLineCount) {m_nMaxLineCount = nMaxLineCount;};

	int GetViewLineCount() {return m_nViewLineCount;};
	void SetViewLineCount(int nViewLineCount) {m_nViewLineCount = nViewLineCount;};

	int  GetStartLine() {return m_nStartLine;};
	void  SetStartLine(int nStartLine);
	void SetForceStartLine(int nStartLine) {m_nStartLine = nStartLine;};

	void ScrollDown()
	{
		if(m_nMaxLineCount <= m_nViewLineCount) return;

		if(m_nStartLine<(m_nMaxLineCount-m_nViewLineCount)) {
			m_nStartLine++;
		}
		SetStartLine(m_nStartLine);
	}
	void ScrollUp()
	{
		if(m_nMaxLineCount <= m_nViewLineCount) return;

		if (m_nStartLine>0) {
			m_nStartLine--;
		}
		SetStartLine(m_nStartLine);
	}

	inline BOOL IsEndScroll() {return m_nStartLine == (m_nMaxLineCount-m_nViewLineCount);};
	void SetEndPos();

protected:

	CUIRes *m_bpUIRes;		// Scroll영역에대한 정보를 얻어오기위한 Point
	
	CPoint *m_pPosOffset;		// ScrollBar Resource가 위치하는 Surface Dialog가 화면에서 위치하는 Offset
	CRect m_rtScroll;

	CRect m_rtThumb;		// Thumb이미지가 위치하게 되는 영역.
	
	MBtnCtrlEx *m_bpButton;

	BOOL m_bCapture;
	CPoint m_ptCapturePos;

	int m_nMaxLineCount;
	int m_nViewLineCount;
	int m_nStartLine;

	int m_nState;
};

#endif // !defined(AFX_UIHORZSCROLLBAR_H__7408B4E2_96BD_11D4_98B2_00C02615866B__INCLUDED_)
