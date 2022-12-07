// Tile.h: interface for the CTile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILE_H__A64CCB3B_1348_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_TILE_H__A64CCB3B_1348_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTileGroup;
typedef CTypedPtrArray<CPtrArray, CTileGroup*> AP_TILEGROUP;

class CTile  
{
public:
	void			DarkenProcess(COLORREF rgb);
	AP_TILEGROUP&	GetApTileGroup(){return m_apTileGroup;}
	BOOL			Load(CArchive &ar);

	CTile();
	virtual ~CTile();

protected:
	void			RemoveAllData();

protected:
	int				m_iType;			//Tile의 Type : type1, type2, type3
	CString			m_szTileName;		//타일의 이름
	CString			m_szBaseTileName;	//백 타일의 이름
	DWORD			m_dwBitmapInfoSize;	//sizeof (m_bypBitmapInfo)
	BYTE*			m_bypBitmapInfo;	//bitmap info
	AP_TILEGROUP	m_apTileGroup;		//TileGroup의 Array
};

#endif // !defined(AFX_TILE_H__A64CCB3B_1348_11D3_8422_0020AF9F40BD__INCLUDED_)
