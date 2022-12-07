// TileSurface.cpp: implementation of the CTileSurface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TileSurface.h"
#include "../DManage/BB_Direct.h"
#include "SM_MapManager.h"
#include "Dib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTileSurface::CTileSurface()
{
	m_pSurface = NULL;
}

CTileSurface::~CTileSurface()
{
	RemoveAllData();
}

BOOL CTileSurface::Load(CArchive &ar, BYTE *bypBitmapInfo, DWORD dwBitmapInfoSize)
{
	RemoveAllData();

	//Bitmap Info를 이용한 Load
	int iCoreSize;
	ar >> iCoreSize;
	BYTE *pLoadMem = new BYTE[dwBitmapInfoSize+iCoreSize];
	CopyMemory(pLoadMem, bypBitmapInfo, dwBitmapInfoSize);
	ar.Read(pLoadMem+dwBitmapInfoSize, iCoreSize);

	m_pSurface = CreateSurfaceFromMemFile(pLoadMem);
	m_pSurface->SetColorKey(RGB(255, 0, 255));// 새로운맵
//	m_pSurface->SetColorKey(RGB(0, 0, 0));

	delete[] pLoadMem;
	return TRUE;
}

BOOL CTileSurface::Load(CArchive &ar)
{
	RemoveAllData();


//	DWORD dwDibSize;
//	ar >> dwDibSize;// 바뀐거 같은데..음냐..
	CSize vSize;
	CSize vPieceSize;
	CSize vPieceCount;

	ar >> vSize;
	ar >> vPieceSize;
	ar >> vPieceCount;

//	CDib* pDib;
//	pDib = new CDib[vPieceCount.cx*vPieceCount.cy];
	
//	LPSTR bitmap_info;
//	DWORD bitmap_info_size;
	
	for(int i = 0; i < 1; i++)//vPieceCount.cx*vPieceCount.cy; i++)
	{

		//dib...대신하는 부분...
		DWORD dwDibSize=0;
		ar.Read((void *)&dwDibSize, sizeof(DWORD));
		if(!dwDibSize) 
			return FALSE;

		BYTE *pLoadMem = new BYTE [dwDibSize];
		ar.Read(pLoadMem, dwDibSize);

		m_pSurface = CreateSurfaceFromMemFile(pLoadMem); //비트맵 정보에서 직접 Surface생성해서 로드해 놓음
		m_pSurface->SetColorKey(RGB(255, 0, 255));// 새로운맵
	//	m_pSurface->SetColorKey(RGB(0, 0, 0));

		delete[] pLoadMem;
		//////////////////////////////////////////////

	}
	return TRUE;
}

void CTileSurface::RemoveAllData()
{
	if (m_pSurface) delete m_pSurface, m_pSurface = NULL;
}

CDDSurface*	CTileSurface::CreateSurfaceFromMemFile(BYTE *pMemFile)
{
	BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)pMemFile;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)pMemFile;

	CSize imageSize(bmiHeader.biWidth, bmiHeader.biHeight);

	CDDSurface *pSurface = new CDDSurface;
	pSurface->Create(SM_MapManager::sm_bpDirectDraw, imageSize.cx, imageSize.cy, FALSE);
	CDC *pDC = pSurface->GetDC();
	
	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 
	1 << bmiHeader.biBitCount;
	
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);
	
	::SetDIBitsToDevice(pDC->GetSafeHdc(), 0, 0, imageSize.cx, imageSize.cy,
		0, 0, 0, imageSize.cy,	lpDIBBits,
		(LPBITMAPINFO)pMemFile,	DIB_RGB_COLORS);

	pSurface->ReleaseDC(pDC);
	return pSurface;
}
