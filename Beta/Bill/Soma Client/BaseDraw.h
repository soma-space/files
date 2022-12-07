// BaseDraw.h: interface for the CBaseDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEDRAW_H__6B7869C3_B494_11D4_98B2_00C02615866B__INCLUDED_)
#define AFX_BASEDRAW_H__6B7869C3_B494_11D4_98B2_00C02615866B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDDSurface;

class CBaseDraw  
{
public:
	CBaseDraw();
	virtual ~CBaseDraw();

	void InitSurface();
	void DrawBaseTile(CPoint cell_offset);
	void DrawChangeTile(CPoint cell_offset);
	BOOL IsChangePos(CPoint cell_offset);
	void DrawSurface(CDDSurface *pDestSurface, CPoint map_offset);
	void DrawShadow(CPoint cell_offset);
	void DrawPartShadow(CPoint cell_offset);
	void BltLSPShadow(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, DWORD dwHalf16Mask, DWORD dwHalf32Mask);
	void BltLSPPartShadow(CDDSurface *pSurface, int x, int y, RECT* pRC, WORD* data, DWORD dwHalf16Mask, DWORD dwHalf32Mask);

	BOOL GetClippedRect(RECT *pRC, RECT* pRCClip);


public:
	CRect m_rcBaseCenter;
	CDDSurface *m_pBaseSurface;
	int m_vWidth, m_vHeight;
	CPoint m_vMapPos, m_vOrgPos;			// Cell ¥‹¿ß(32X32)
	CPoint m_vCurPos;
};

inline BOOL CBaseDraw::IsChangePos(CPoint cell_offset)
{
	if(cell_offset.x != m_vMapPos.x || cell_offset.y != m_vMapPos.y) return TRUE;
	else return FALSE;
};

#endif // !defined(AFX_BASEDRAW_H__6B7869C3_B494_11D4_98B2_00C02615866B__INCLUDED_)
