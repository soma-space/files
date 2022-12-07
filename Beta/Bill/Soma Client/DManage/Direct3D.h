// Direct3D.h: interface for the CDirect3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECT3D_H__DBA3B162_6254_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_DIRECT3D_H__DBA3B162_6254_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d.h>
#include <ddraw.h>
#include "BB_Direct.h"
//계속 업그레이드를 해야 겠지 일단은 초기화와 3D기능만이라도 사용할수 있게만 했음
class AFX_EXT_CLASS CDirect3D  
{
public:
	CDirect3D();
	virtual ~CDirect3D();
	void SetVAGType( int nVGAType );
	void SetLight();
	void SetMaterial();
	BOOL Clip( RECT* pRC, float* lTu, float* rTu, float* tTv,float* bTu);
	LPDIRECT3DDEVICE7 GetDevice(){ return m_lpD3DD7;}
	BOOL Initialize3D(CDirectDraw *pDirectDraw,HWND hWnd, GUID* pDriverGUID, const GUID* pDeviceGUID);
	void CleanUp3DEnvironment();
	BOOL D3DTexturInfo( TCHAR* strName , int** pFrame, int** pCurFrame, float** pTu, 
						float** pTv, int** pWidth, int** pHeight, int** pTime, int** pX, int** pY );
	BOOL D3DRender(TCHAR* strName,int x, int y, int nCurFrame);
	BOOL D3DRenderEx(TCHAR* strName,int x, int y, int nCurFrame);
	BOOL D3DRenderShadow(TCHAR* strName,int x, int y, int nCurFrame);

	void DrawLightMask(CArray<int,int>* arr_Mask);
	BOOL D3DRenderDay( int Type, BOOL bSun_Set );
	void D3DRenderChat();
	void D3DAlpha( BOOL bAlpha );
	void DestroyAllTexture();
	void DestroyTexture(TCHAR *strName);
	void DestroySeriesTexture(TCHAR *strName, int nFrame);
	void D3DCreateTexture(TCHAR* strName);
	void D3DCreateTextureEx(TCHAR* strName);
	void D3DCreateTextureSpl(TCHAR* strName);
	//우석. 빈텍스쳐 서피스생성..
	HRESULT D3DCreateEmptyTexture( TCHAR* strName, DWORD dwWidth,
                                     DWORD dwHeight, DWORD dwStage = 0,
                                     DWORD dwFlags = 0 );
	LPDIRECTDRAWSURFACE7 D3DGetSurface( TCHAR* strName );
	void CreateLoginVertices(int num = 12);
	void DestoryLoginVertices();
	BOOL D3DLoginRender(char *strName, bool blend, int start);
	bool LoginCharData(int dir, int front_num, float time_gap);
	void ChangePosColor();
	void CreateSurfBmpToSurf(LPDIRECTDRAWSURFACE7 DstSurface, RECT Dst,
							   HBITMAP hbmBitmap, RECT Src);//우석.
	bool SDrawLine(float &x_start, float &y_start, float x_end, float y_end,
		           DWORD &SrcColor, DWORD DstColor, int gap_step = 1, float time_gap = 0.0);
	void CreateMakeCharVertices(int num = 24);
	void DestoryMakeCharVertices();
	bool ChangeVerticesColor(int Sex, int skip_color);
	bool SkinColorChange(bool IsMan, int cur, int next, int skip_color);
	void ResetSkinColor(int index);
	//우석.

	//우석
	D3DVECTOR                m_Char1;
	D3DVECTOR                m_Char2;
	D3DVECTOR                m_Char3;
	D3DCOLOR				 m_Color1;
	D3DCOLOR				 m_Color2;
	D3DCOLOR				 m_Color3;

	D3DCOLOR				 m_SkinColor1;
	D3DCOLOR				 m_SkinColor2;
	D3DCOLOR				 m_SkinColor3;
	//우석.

private:
	D3DTLVERTEX				m_d3dSpriteVertices[4];
	D3DTLVERTEX				m_d3dDayVertices[4];//xxx
	//우석
	D3DTLVERTEX				*m_d3dLoginVertices;
	//우석.

	LPDIRECT3D7				m_lpD3D7;
	LPDIRECT3DDEVICE7		m_lpD3DD7;
	D3DMATERIAL7			m_d3dMtrl;
    D3DLIGHT7				m_d3dLight;
	LPDIRECTDRAWSURFACE7	m_lpDDS_ZBuffer;
	LPDIRECTDRAWSURFACE7	m_lpDDS_BackBuffer;
	int						m_VGAType;
	DWORD					m_SorBlend;
	DWORD					m_DesBlend;
	DWORD					m_Alphaarg1;

};

#endif // !defined(AFX_DIRECT3D_H__DBA3B162_6254_11D4_956A_00D0B77259E8__INCLUDED_)
