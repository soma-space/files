// TileVary.h: interface for the CTileVary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TILEVARY_H__831A0481_1378_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_TILEVARY_H__831A0481_1378_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTilePiece;
typedef CTypedPtrArray<CPtrArray, CTilePiece*> AP_TILEPIECE;

class CTileVary  
{
public:
	void			DarkenProcess(COLORREF rgb);			//어둡게 처리하기 SM_MapManager로 부터시작해서 불려짐
	AP_TILEPIECE&	GetApTilePiece(){return m_apTilePiece;}
	BOOL			Load(CArchive &ar, int nPiece, BYTE *bypBitmapInfo, DWORD dwBitmapInfoSize); //Bitmap Info를 정해놓고 Load함

	CTileVary();
	virtual ~CTileVary();

protected:
	void			RemoveAllData();

protected:
	AP_TILEPIECE	m_apTilePiece;	//TilePiece의 Array
	int				m_iLink;		// ?
};

#endif // !defined(AFX_TILEVARY_H__831A0481_1378_11D3_8422_0020AF9F40BD__INCLUDED_)
