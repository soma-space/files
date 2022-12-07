// SgrRes.h: interface for the CSgrRes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SGRRES_H__A64CCB3A_1348_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_SGRRES_H__A64CCB3A_1348_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTile;
typedef CTypedPtrArray<CPtrArray, CTile*> AP_TILE;
class CSprite;
typedef CTypedPtrArray<CPtrArray, CSprite*> AP_SPRITE;
class CCustomTile;
typedef CTypedPtrArray<CPtrArray, CCustomTile*> AP_CUSTOMTILE;

//Sgr 파일의 Resource를 관리 (CSgrRes class 1개가 1파일에 해당)
class CSgrRes  
{
public:
	BOOL			SetDarkLevel(int iPercent, BOOL bAbsolute=TRUE);//Map의 Reload가 있으면 TRUE, 아니면 FALSE
	void			RemoveAllData();
	BOOL			Load(CString szFileName);

	AP_TILE&		GetApTile() {return m_apTile;}				//Tile의 정보보관
	AP_SPRITE&		GetApSprite() {return m_apSprite;}			//Sprite의 정보보관
	AP_CUSTOMTILE&	GetApCustomTile() {return m_apCustomTile;}	//CustomTile의 정보보관

	CSgrRes();
	virtual ~CSgrRes();

protected:
	void			DarkenProcess();		//어둡게 만듬
	void			ReloadResource();		//다시 로드함(필요한 경우에)

protected:
	CString			m_szFileName;			//Sgr파일의 이름
	AP_TILE			m_apTile;
	AP_SPRITE		m_apSprite;
	AP_CUSTOMTILE	m_apCustomTile;
	int				m_iDarkLevel;			//어둡기 정도 설정


public:
	int				m_iTileCount;
	int				m_iSpriteCount;
	int				m_iCustomTileCount;
};

#endif // !defined(AFX_SGRRES_H__A64CCB3A_1348_11D3_8422_0020AF9F40BD__INCLUDED_)
