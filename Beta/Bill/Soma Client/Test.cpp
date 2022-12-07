// Test.cpp: implementation of the CTest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Test.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// 다이얼 로그 박스 샘플이다..탬플릿으루..만들까나??음냐..
//////////////////////////////////////////////////////////////////////

CTest::CTest()
{
	m_Test_Del = 0;
	m_lpSurface = NULL;
}

CTest::~CTest()
{
	SAFE_DELETE(m_lpSurface);
}

void CTest::Draw(CDDSurface* pDestSurf)
{
	//예..1
/*	int number = 0;
	while(ViewItemBox(x, y, number, pDestSurf))
	{
		number ++;
	}
    //예2
    ViewItemBox(x, y, 0, spr_number, pDestSurf);
	if(.....)
	  ViewItemBox(x, y, 1, spr_number, pDestSurf);
	else
	  ViewItemBox(x, y, 2, spr_number, pDestSurf);
*/
	
	int spr_number = 3;
	
	if(m_Dialog_Box_Flag == 0)
		spr_number = 3;
	else
		spr_number = 4;

    ViewItemBox(0, 100, 0, spr_number, pDestSurf, m_Test_Del ++);

	if(m_Test_Del > 50)
		m_Test_Del = 0;
//	spr_number = 1;
//	ViewItemBox(0, 200, 1, spr_number, pDestSurf);
//	spr_number = 2;
//	ViewItemBox(100, 500, 2, spr_number, pDestSurf);
//	spr_number = 3;
//	ViewItemBox(50, 100, 3, spr_number, pDestSurf);
//	spr_number = 4;
//	ViewItemBox(50, 200, 4, spr_number, pDestSurf);

}

void CTest::InitData(CDirectDraw *pDirectDraw)
{
	m_pDirectDraw = pDirectDraw;
	m_lpSurface = new CDDSurface;

	m_Dialog_Box_Flag = -1;

	for(int i=0; i<50; i++)
	{
		m_BoxRect[i].top    = 0;
		m_BoxRect[i].bottom = 0;
		m_BoxRect[i].left   = 0;
		m_BoxRect[i].right  = 0;
	}
	
	Load_image("test.bmp");
	LoadSpr("test.spr");

}


