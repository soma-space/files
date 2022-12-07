// Dialog_MagicCtrl.h: interface for the CDialog_MagicCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_MAGICCTRL_H__56D161E1_742D_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_MAGICCTRL_H__56D161E1_742D_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DRAW_SPECIALMAGIC		1
#define DRAW_WHITEMAGIC			2
#define DRAW_BLUEMAGIC			3
#define DRAW_BLACKMAGIC			4

class CDialog_SpecialMagic;
class CDialog_WhiteMagic;
class CDialog_BlueMagic;
class CDialog_BlackMagic;
class CChildView;
class BBDX_MouseInformation;
class CDirectDraw;
class CDDSurface;
class CUIRes;

class CDialog_MagicCtrl
{
public:
	CDialog_MagicCtrl();
	virtual ~CDialog_MagicCtrl();

	void DrawMode(int nMode);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);
	BOOL InitData(CDirectDraw *pDD, CChildView *pWnd);
	void Draw(CDDSurface *pDest);

	void ShowWindow();
	void HideWindow();
	BOOL IsVisible() {return m_bShow;};

	CUIRes *m_pDrawDialog;		// 그려주기 위한 포인터
	BOOL m_bShow;

	CDialog_SpecialMagic *m_pDialog_Special;
	CDialog_WhiteMagic *m_pDialog_White;
	CDialog_BlueMagic *m_pDialog_Blue;
	CDialog_BlackMagic *m_pDialog_Black;

protected:
	CDirectDraw *m_pDD;
	CChildView *m_pWnd;

	int m_nDrawMode;

private:
	BOOL m_bInit;
};

#endif // !defined(AFX_DIALOG_MAGICCTRL_H__56D161E1_742D_11D4_98B0_00C02615866B__INCLUDED_)
