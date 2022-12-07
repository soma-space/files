// Dialog_Storage.h: interface for the CDialog_Storage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIALOG_STORAGE_H__128BEAE4_6DFD_11D4_98B0_00C02615866B__INCLUDED_)
#define AFX_DIALOG_STORAGE_H__128BEAE4_6DFD_11D4_98B0_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIRes.h"
#include "UIScrollBar.h"
#include "DarkDef.h"
class BBDX_Editor;
//class BBDX_TextOuter;
class BBDX_MouseInformation;
//class CTWFont;

class CDialog_Storage : public CUIRes  
{
public:
	CDialog_Storage();
	virtual ~CDialog_Storage();

public:
	void CallBack_MessageBox(int nId, int nValue);
	BOOL OnMouseMove(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonUp(BBDX_MouseInformation *pMouseInfo);
	BOOL OnLButtonDown(BBDX_MouseInformation *pMouseInfo);
	int MouseProcess(BBDX_MouseInformation *pMouseInfo);

	void InitData();
	void SetWnd(CWnd *wnd) {m_pWnd = wnd;};
	BOOL Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bModal = FALSE, BOOL bMoveable = FALSE, BOOL bTrans = FALSE);
	BOOL Draw(CDDSurface *pDest);
	BOOL OnButtonAction(int nSpec, int nIndex);

	void SetScrollData();

	void SendToStorage(short count = 1);
	void GetFromStorage(short count = 1);
	void DepositMoney(DWORD money);
	void DrawMoney(DWORD money);

	void ShowWindow();
	void HideWindow();
	void HideAllWindow();
	void RefreshData();
	void DeleteContents();

	void StorageOpen(PBYTE bypData);
	void SaveItemToStorage(PBYTE bypData);
	void TakebackItemFromStorage(PBYTE bypData);
	void SaveMoney(PBYTE bypData);
	void TakebackMoney(PBYTE bypData);

	void DrawTipRect(CDDSurface *pDest, CRect& t_rect, int area_index);
	void ViewCaption(CDDSurface *pDest, CPoint pos, CString str, int area_index);

	CUIScrollBar		*m_pHandScrollBar, *m_pStorageScrollBar;
//	BBDX_TextOuter		*m_pOuter;
	CItemMeArray		m_arrItemMe;
	CItemStorageArray	m_arrItemStorage;
	int					m_vCurMeItem, m_vCurStorageItem;
	DWORD				m_vStorageMoney;
//	CTWFont				*m_pTWFont;
	CFont				*m_pFont;
	int					m_vTipIndex;
};

#endif // !defined(AFX_DIALOG_STORAGE_H__128BEAE4_6DFD_11D4_98B0_00C02615866B__INCLUDED_)
