#include "StdAfx.h"
#include "ObjectClass.h"

MAreaCtrl::MAreaCtrl()
{
	m_vRect.SetRectEmpty();
	m_bFocus = FALSE;
	m_vSpec = AREA_CTRL;
	m_vIndex = -1;
}

void MAreaCtrl::Save(CArchive& ar)
{
	ar << m_vRect.left << m_vRect.top << m_vRect.right << m_vRect.bottom;
}

void MAreaCtrl::Load(CArchive& ar)
{
	ar >> m_vRect.left >> m_vRect.top >> m_vRect.right >> m_vRect.bottom;
}

MBtnCtrl::MBtnCtrl()
{
	m_vFocusRect.SetRectEmpty();
	m_vDownRect.SetRectEmpty();
	m_bTransparent = FALSE;
	m_vStyle = 1;
	m_vState = 0;
	m_vSpec = BTN_CTRL;
}

void MBtnCtrl::Save(CArchive& ar)
{
	MAreaCtrl::Save(ar);

	ar << m_vFocusRect.left << m_vFocusRect.top << m_vFocusRect.right << m_vFocusRect.bottom;
	ar << m_vDownRect.left << m_vDownRect.top << m_vDownRect.right << m_vDownRect.bottom;
	ar << m_bTransparent << m_vStyle;
}

void MBtnCtrl::Load(CArchive& ar)
{
	MAreaCtrl::Load(ar);

	ar >> m_vFocusRect.left >> m_vFocusRect.top >> m_vFocusRect.right >> m_vFocusRect.bottom;
	ar >> m_vDownRect.left >> m_vDownRect.top >> m_vDownRect.right >> m_vDownRect.bottom;
	ar >> m_bTransparent >> m_vStyle;
}

MBtnCtrlEx::MBtnCtrlEx()
{
	m_vNormalRect.SetRectEmpty();
	m_vFocusRect.SetRectEmpty();
	m_vDownRect.SetRectEmpty();
	m_bTransparent = FALSE;
	m_vStyle = 1;
	m_vState = 0;
	m_vSpec = BTN_CTRL_EX;
}

void MBtnCtrlEx::Save(CArchive& ar)
{
	MAreaCtrl::Save(ar);

	ar << m_vNormalRect.left << m_vNormalRect.top << m_vNormalRect.right << m_vNormalRect.bottom;
	ar << m_vFocusRect.left << m_vFocusRect.top << m_vFocusRect.right << m_vFocusRect.bottom;
	ar << m_vDownRect.left << m_vDownRect.top << m_vDownRect.right << m_vDownRect.bottom;
	ar << m_bTransparent << m_vStyle;
}

void MBtnCtrlEx::Load(CArchive& ar)
{
	MAreaCtrl::Load(ar);

	ar >> m_vNormalRect.left >> m_vNormalRect.top >> m_vNormalRect.right >> m_vNormalRect.bottom;
	ar >> m_vFocusRect.left >> m_vFocusRect.top >> m_vFocusRect.right >> m_vFocusRect.bottom;
	ar >> m_vDownRect.left >> m_vDownRect.top >> m_vDownRect.right >> m_vDownRect.bottom;
	ar >> m_bTransparent >> m_vStyle;
}

MRectManager::MRectManager()
{
	m_pRect = NULL;
	m_pCtrl = NULL;
	m_vIndex = 0x01;
}
