// TileSurface.h: interface for the CTileSurface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILESURFACE_H__831A0483_1378_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_TILESURFACE_H__831A0483_1378_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDDSurface;

class CTileSurface  
{
public:
	CDDSurface*	GetSurface(){return m_pSurface;}
	BOOL		Load(CArchive &ar);	//Archive에 bitmapinfo 까지 포함하고 잇는 경우
	BOOL		Load(CArchive &ar, BYTE *bypBitmapInfo, DWORD dwBitmapInfoSize); //BitmapInfo를 따로 사용하는 경우

	CTileSurface();
	virtual ~CTileSurface();

protected:
	CDDSurface*	CreateSurfaceFromMemFile(BYTE *pMemFile);	//Bitmap 파일은 pMemFile에 올리고 pMemFile의 메모리로 부터 Surface를 생성
	void		RemoveAllData();

protected:
	CDDSurface	*m_pSurface;
};

#endif // !defined(AFX_TILESURFACE_H__831A0483_1378_11D3_8422_0020AF9F40BD__INCLUDED_)
