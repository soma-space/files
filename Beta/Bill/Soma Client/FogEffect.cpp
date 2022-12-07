// FogEffect.cpp: implementation of the CFogEffect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FogEffect.h"
#include "DManage/AlphaProcess.h"

/*
#include "Soma.h"
#include "ChildView.h"
#include "DManage/BB_Direct.h"
#include "DManage/AlphaProcess.h"
#include "DManage/BBDX_MouseInformation.h"
#include "MainFrm.h"
#include "User.h"
#include "PacketParser.h"
#include "BBDX_Editor.h"
#include "BBDX_ChatOuter.h"
#include "Magic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
#define RELEASE(exp) if(exp!=NULL) { exp->Release(); exp=NULL; }
#define CLEAR(exp) memset(&exp, 0, sizeof(exp))

DirectXMng			CFogEffect::DirectXMng;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFogEffect::CFogEffect()
{
	m_lpSurface_Fog = NULL;
}

CFogEffect::~CFogEffect()
{
	SAFE_DELETE(m_lpSurface_Fog);
}

void CFogEffect::InitData(CDirectDraw *pDirectDraw)// 다이렉트 드로우를 가지고 온다..반드시 초기화 시켜줘야함..
{
	m_pDirectDraw = pDirectDraw;
	m_lpSurface_Fog = new CDDSurface;
	
	Load_Fog_image();
}

void CFogEffect::test(CDDSurface* pDestSurf)
{
	pDestSurf->DrawFogLayer3(8, 0);

}

void CFogEffect::test2(CDirectDraw *pDirectDraw)//IDirectDraw4 *lpDD)
{
	int del;
	del = 0;
	IDirectDraw4 *lptest = NULL;

	pDirectDraw->GetInterface();

//	lpDD = DirectXMng.m_pDD->GetDirectDraw();
	
//	LPDIRECTDRAWSURFACE4 test;
//	IDirectDrawSurface4 *test;

//	test = DDLoadBitmap(pDirectDraw->GetInterface(), "test.bmp", 0,0,0);
}

void CFogEffect::Load_Fog_image()
{
//	m_lpSurface_Fog = DDLoadBitmap(m_pDirectDraw->GetInterface(), "test.bmp", 0,0,0);

// 막아 놓았음...쓸때없이..비됴 메모리를 사용할려구 해서..냠냠..
//	m_lpSurface_Fog->CreateFromImage(m_pDirectDraw, "test.bmp", 1);
//	m_lpSurface_Fog->SetColorKey(RGB(0, 0, 0));
}



void CFogEffect::Draw(CDDSurface* pDestSurf)
{
	CRect rect;
//	CRect *rect2;
	CRect r(0, 0, 800, 600);
	CRect sr(0, 0, 800, 600);

	POINT pt;
	pt.x = 0;
	pt.y = 0;
	
	rect.top = 0;
	rect.left = 0;
	rect.bottom = 600;
	rect.right = 800;

//	CAlphaProcess::MixHalf(pDestSurf, r, m_lpSurface_Fog, sr);
	pDestSurf->Blt(&rect, m_lpSurface_Fog,&rect, DDBLT_KEYSRC);
}
