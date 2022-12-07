// CustomTileVary.h: interface for the CCustomTileVary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMTILEVARY_H__831A048A_1378_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_CUSTOMTILEVARY_H__831A048A_1378_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//CTilePiece클래스에도 선언되어 있음
class CTilePiece;
typedef CTypedPtrArray<CPtrArray, CTilePiece*> AP_TILEPIECE;
typedef CTypedPtrArray<CPtrArray, AP_TILEPIECE*> APP_TILEPIECE;

class CCustomTileVary  
{
public:
	void				DarkenProcess(COLORREF rgb);
	AP_TILEPIECE&		GetApTilePiece(int index){return *m_appTilePiece[index];}
	APP_TILEPIECE&		GetAppTilePiece(){return m_appTilePiece;}
	BOOL				Load(CArchive &ar, CSize dimTile, BYTE* bypBitmapInfo, DWORD dwBitmapInfoSize);

	CCustomTileVary();
	virtual ~CCustomTileVary();

protected:
	void				RemoveAllData();

protected:
	APP_TILEPIECE		m_appTilePiece;			//배열의 배열
};

#endif // !defined(AFX_CUSTOMTILEVARY_H__831A048A_1378_11D3_8422_0020AF9F40BD__INCLUDED_)
