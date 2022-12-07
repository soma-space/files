// CustomTile.h: interface for the CCustomTile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMTILE_H__A64CCB3D_1348_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_CUSTOMTILE_H__A64CCB3D_1348_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCustomTileVary;
typedef CTypedPtrArray <CPtrArray, CCustomTileVary*> AP_CUSTOMTILEVARY;

class CCustomTile  
{
public:
	void				DarkenProcess(COLORREF rgb);		//맵의 밝기 조절시.
	AP_CUSTOMTILEVARY&	GetApCustomTileVary(){return m_apCustomTileVary;}
	BOOL				Load(CArchive &ar);					//Map중에서 CustomTile Load처리부

	CCustomTile();
	virtual ~CCustomTile();

protected:
	void				RemoveAllData();

protected:
	CString				m_szTileName;						//Tile의 이름
	CString				m_szBaseTileName;					//Base Tile의 이름
	CSize				m_dimTile;							//CustomTile의 Dimension (32X32 단위 셀 기준)
	BYTE*				m_bypBitmapInfo;					//Bitmap Info
	DWORD				m_dwBitmapInfoSize;					//Bitmap Info 의 크기 : sizeof(m_bypBitmapInfo)

	AP_CUSTOMTILEVARY	m_apCustomTileVary;
};

#endif // !defined(AFX_CUSTOMTILE_H__A64CCB3D_1348_11D3_8422_0020AF9F40BD__INCLUDED_)
