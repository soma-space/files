// SLoadingView.cpp: implementation of the SLoadingView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "SLoadingView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define  PIC_COUNT  6;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SLoadingView::SLoadingView()
{

}

SLoadingView::~SLoadingView()
{

}

BOOL SLoadingView::Create(CDirectDraw *pDD, CString szLibName, int nType, BOOL bMoveable, BOOL bTrans)
{
	int pic_num = 0;
	CString  tmp;
	if(m_pDDSurface) { delete m_pDDSurface; m_pDDSurface =	NULL; }

	srand((unsigned)time(NULL));
	pic_num = rand() % PIC_COUNT;	

	tmp.Format("%s%d.lib",(LPCTSTR)szLibName,pic_num);
	if(!CUIRes::Create(pDD, tmp, nType, bMoveable, bTrans)) {
	   return FALSE;
	}

	return TRUE;
}

BOOL SLoadingView::Draw( CDDSurface* pDestSurf )
{
	CUIRes::Draw(pDestSurf);

	return TRUE;
}