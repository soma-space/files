// DDEngineTool4.h: interface for the CDDEngineTool4 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDENGINETOOL4_H__0474F7AE_BAB0_4826_93B1_12BBB6F6D151__INCLUDED_)
#define AFX_DDENGINETOOL4_H__0474F7AE_BAB0_4826_93B1_12BBB6F6D151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\IGObject\\DDEngineLSP.h"

class CDDEngineTool4 : public CDDEngineLSP
{
public:
	BOOL TransactMaskAndAnti(LPDIRECTDRAWSURFACE lpdds, WORD *pMaskTbl, COLORREF TColor);
	BOOL TransactSimilarTransColor(LPDIRECTDRAWSURFACE lpdds, COLORREF TColor, int nReinforceDelta);
	BOOL TransactPieceLSPSpr(const char* szFileName, int nRDelta, CPoint ptCenter, _SPR_LSP* pSpr);

	BOOL ConvertIndexedLSPFileToSImpleLSPFile(const char* szSrcFN, const char* szDestFN);
	BOOL ConvertSImpleLSPFileToIndexedLSPFile(const char* szSrcFN, const char* szDestFN, const char* szIndexFN);
	BOOL Convert16PixelTo8Pixel(WORD *p16Pixels, BYTE* p8Pixels, RGBQUAD* pRGBTbls, int nQt);
	BOOL Convert8PixelTo16Pixel(BYTE* p8Pixels, WORD *p16Pixels, RGBQUAD* pRGBTbls, int nQt);
//	BOOL CropBMPAndSaveTo24BMP(const char* szSrcFileName, const char* szDestFileName, CRect rcCrop);

	POINT	FindCenterAuto(LPDIRECTDRAWSURFACE lpDDS, CRect rc, COLORREF CColor); // 센터점을 자동으로 찾는다.
	RECT	FindRegionManual(LPDIRECTDRAWSURFACE lpDDS, CRect rc, COLORREF TColor); // 주어진 영역 안에서 최소 영역을 구한다.
	RECT	FindRegionAuto(LPDIRECTDRAWSURFACE lpDDS, CPoint pt, COLORREF TColor); // 주어진 점에서 확대를 해 나가 최소 영역을 구한다.
	BOOL 	EncodeToLSPData(LPDIRECTDRAWSURFACE lpDDS, COLORREF TColor, RECT rcRgn, POINT ptCenter, _SPR_LSP* pSpr); // LSP Encode

	CDDEngineTool4();
	virtual ~CDDEngineTool4();
};

#endif // !defined(AFX_DDENGINETOOL_H__0474F7AE_BAB0_4826_93B1_12BBB6F6D151__INCLUDED_)
