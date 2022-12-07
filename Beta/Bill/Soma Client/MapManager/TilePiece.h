// TilePiece.h: interface for the CTilePiece class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILEPIECE_H__831A0482_1378_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_TILEPIECE_H__831A0482_1378_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTileSurface;
typedef CTypedPtrArray <CPtrArray, CTileSurface*> AP_TILESURFACE;
class CDDSurface;
typedef CTypedPtrArray <CPtrArray, CDDSurface*> AP_CDDSURFACE;

class CTilePiece  
{
public:
	void			DarkenProcess(COLORREF rgb);
	void			Draw(CDDSurface* pDestSurface, CPoint &pos, int iAnimationIndex);	//그리기 주어진 Index활용
	int				GetAnimationNum(){return m_nAnimation;}								//Animation몇개인가?
	BOOL			Load(CArchive &ar, BYTE *bypBitmapInfo, DWORD dwBitmapInfoSize);	//Load
	int				m_nAnimation;

	CTilePiece();
	virtual ~CTilePiece();

protected:
	void			RemoveAllData();
	void			FillApCDDSurface();	//Load후 m_apCDDSurface 채우기

protected:
	//int				m_nAnimation;		
	AP_TILESURFACE	m_apTileSurface;	//CTileSurface의 Array
	AP_CDDSURFACE	m_apCDDSurface;		//빠른 접근을 위해서(그릴때 TileSurface의 CDDSurface를 사용하므로, CDDSurface의 포인터를 받아두고 직접 접근함)
};

#endif // !defined(AFX_TILEPIECE_H__831A0482_1378_11D3_8422_0020AF9F40BD__INCLUDED_)
