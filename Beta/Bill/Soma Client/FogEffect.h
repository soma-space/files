// FogEffect.h: interface for the CFogEffect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOGEFFECT_H__8A2074A6_981E_11D3_B175_00105A735718__INCLUDED_)
#define AFX_FOGEFFECT_H__8A2074A6_981E_11D3_B175_00105A735718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Soma.h"
#include "ChildView.h"
#include "DManage/BB_Direct.h"
#include "DManage/AlphaProcess.h"

class CFogEffect  
{
public:
	void test(CDDSurface* pDestSurf);
	void test2(CDirectDraw *pDirectDraw);//IDirectDraw4 *lpDD);
	CFogEffect();
	virtual ~CFogEffect();

public:
	void Draw(CDDSurface* pDestSurf);
	void InitData(CDirectDraw *pDirectDraw);
	void Load_Fog_image();

//	LPDIRECTDRAWSURFACE4 m_lpSurface_Fog;
	CDirectDraw *m_pDirectDraw;// 가져와야 한다..
	CDDSurface*  m_lpSurface_Fog;
	static DirectXMng		DirectXMng;

};

#endif // !defined(AFX_FOGEFFECT_H__8A2074A6_981E_11D3_B175_00105A735718__INCLUDED_)
