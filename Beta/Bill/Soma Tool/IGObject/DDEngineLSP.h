// DDEngineLSP.h: interface for the CDDEngineLSP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDENGINELSP_H__0E150F5F_9D62_40D0_8FF2_CA85937751B6__INCLUDED_)
#define AFX_DDENGINELSP_H__0E150F5F_9D62_40D0_8FF2_CA85937751B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DDEngine.h"

class CDDEngineLSP : public CDDEngine  
{
public:
	void BltLSP(LPDIRECTDRAWSURFACE lpDDSDest, int x, int y, const _SPR_LSP* pSprLSP, int nRenderType = RENDER_NORMAL, int nAlpha = 16, COLORREF crColor = 0);
	void BltLSPOff(LPDIRECTDRAWSURFACE lpDDSDest, int x, int y, SIZE pSize, const _SPR_LSP* pSprLSP, int nRenderType = RENDER_NORMAL, int nAlpha = 16, COLORREF crColor = 0);
	BOOL GetClippedRect(RECT *pRC, RECT* pRCClip);
	
	CDDEngineLSP();
	virtual ~CDDEngineLSP();

protected:
};

#endif // !defined(AFX_DDENGINELSP_H__0E150F5F_9D62_40D0_8FF2_CA85937751B6__INCLUDED_)
