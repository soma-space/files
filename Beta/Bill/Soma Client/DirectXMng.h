// DirectXMng.h: interface for the DirectXMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTXMNG_H__010054CA_0204_11D3_8422_0020AF9F40BD__INCLUDED_)
#define AFX_DIRECTXMNG_H__010054CA_0204_11D3_8422_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDirectDraw;
class CDDSurface;
class CDirectInput;
class BBDX_MouseInformation;
class CDDClipper;
class CAlphaProcess;
class CDxCursor;
class CDirectSound;
class CDirect3D;
class CNight;

class DirectXMng  
{
public:
	void						DrawCursor();			//Cursor그리기
	void						Flip();					//화면 Flip
	BOOL						IsDirectInputValid() {return (NULL!=m_pDirectInput);}	//DirectInput의 유효성 검사
	BBDX_MouseInformation*		GetMouseInfoPtr() {return m_pMouseInfo;}	//마우스 정보
	CPoint&						GetMousePos();								//마우스의 정보중 좌표
	BOOL						MouseInfoUpdate();							//마우스 정보 갱신
	CSize&						GetScreenSize() {return m_sizeScreen;}
	void						SetRelatedHwnd(HWND hwnd) {m_hWnd = hwnd;}	//MainWindow의 Handle(다이렉트X 는 MainWindow Handle을 이용)
	BOOL						ReleaseDirectX();							//DirectX 의 종료
	BOOL						InitDirectX(int width, int height, int color_depth, BOOL bFullScreen);	//DirectX의 시작
	void						RestoreSurface();							//화면 깨졌을때 (다른 프로그램의 Active등에 의한) 복구
	long						GetGammaRate(){ return m_GammaRate;}
	void						SetGammaRate( long lGammaRate );
	void						SetRainGammaRate( int nAlphaGamma );
	void						SetInHouseGammaRate();
	void						GetVGAName();
	int 						InitMask();
	DWORD						m_dwHalf16Mask;
	DWORD						m_dwHalf32Mask;

	DirectXMng();
	virtual ~DirectXMng();
public:
	CDirectSound*				m_pDirectSound;				//DirectSound 객체

	CDDSurface*					m_bpFrontSurf;				//Front Surface(Primary Surface)
	CDDSurface*					m_bpBackSurf;				//Back Surface(Secondary Surface)
	CDDSurface*					m_pDrawSurface;				//Drawing Surface(System Memory사용) (Alpha때문)
	BOOL						m_bFullScreen;				//Full Screen인가?
	HWND						m_hWnd;						//MainWindow의 Handle
	CDirectDraw*				m_pDD;						//DirectDraw 객체
	COLORREF					m_colorTrans;				//투명으로 쓸 Color (Default: RGB(0,0,0))
	CDirectInput*				m_pDirectInput;				//DirectInput 객체
	BBDX_MouseInformation*		m_pMouseInfo;				//DirectInput에 의해 마우스 정보를 담고 있는곳
	CAlphaProcess*				m_pAlphaProcess;			//Alpha처리기 (사용?...)
	IDirectDrawGammaControl*	m_pGammaCtrl;				//Gamma Control(풀화면에서만 사용가능할 껄?....)
	DDGAMMARAMP					m_orgGamma;					//Gamma데이터 보관장소
	DDGAMMARAMP					m_GammaLightning;			//Gamma 번개 	

	long						m_GammaRate;
	int							m_nAlphaGamma;
	CDxCursor*					m_pDxCursor;				//Cursor그림 유지

	CSize						m_sizeScreen;				//화면의 크기
	CDirect3D*					m_p3D;
	BOOL						m_bHalEnabled;
	CNight*						m_Night;
	CString						m_VGAcardName;
	int							m_VGAType;

};

#endif // !defined(AFX_DIRECTXMNG_H__010054CA_0204_11D3_8422_0020AF9F40BD__INCLUDED_)
