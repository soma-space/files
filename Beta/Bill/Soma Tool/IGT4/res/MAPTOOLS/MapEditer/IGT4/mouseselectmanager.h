// MouseSelectManager.h: interface for the MouseSelectManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MouseSelectManager_H)
#define _MouseSelectManager_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWndTileMap;

class MouseSelectManager
{
public:
	POINT m_vCurPos[5];

	void	SetViewPtr(CWndTileMap *pView){m_bpView = pView;}
	void	MouseMoving(CPoint pos);
	POINT	*GetSelectRgn();
	void	SetEndPos(CPoint pos);
	void	SetStartPos(CPoint pos);
	BOOL	IsUsing() {return m_bUsing;};

	MouseSelectManager();
	virtual ~MouseSelectManager();

protected:
	BOOL m_bUsing;
	POINT m_vPos[4], m_vRealPos[4];
	CWndTileMap *m_bpView;
};

#endif // !defined(AFX_MouseSelectManager_H__488C9C01_06B8_11D3_8422_0020AF9F40BD__INCLUDED_)
