// TileGroup.h: interface for the CTileGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILEGROUP_H__A64CCB40_1348_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_TILEGROUP_H__A64CCB40_1348_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTileVary;
typedef CTypedPtrArray<CPtrArray, CTileVary*> AP_TILEVARY;

class CTileGroup  
{
public:
	void			DarkenProcess(COLORREF rgb);			//어둡기 처리
	AP_TILEVARY&	GetApTileVary() {return m_apTileVary;}	//TileVary의 array
	BOOL			Load(CArchive &ar, BYTE *bypBitmapInfo, DWORD dwBitmapInfoSize); //Load시 bitmapinfo를 사용해서 이미지 생성

	CTileGroup();
	virtual ~CTileGroup();

protected:
	void			RemoveAllData();

protected:
	int				m_nPiece;								//Vary 당 몇개의 조각?
	AP_TILEVARY		m_apTileVary;							//Vary Array
};

#endif // !defined(AFX_TILEGROUP_H__A64CCB40_1348_11D3_8422_0020AF9F40BD__INCLUDED_)
