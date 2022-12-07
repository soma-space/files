// _TILE_SUB_SET_DATA_TOOL.cpp: implementation of the _TILE_SUB_SET_DATA_TOOL class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "igt4.h"
#include "_TILE_SUB_SET_DATA_TOOL.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_TILE_SUB_SET_DATA_TOOL::_TILE_SUB_SET_DATA_TOOL()
{
	Init();
}

_TILE_SUB_SET_DATA_TOOL::~_TILE_SUB_SET_DATA_TOOL()
{
	Release();
}

void _TILE_SUB_SET_DATA_TOOL::Init()
{
	szRemark[0] = 0;
	nWidth = nHeight = 0;
	nAniCount = 0;
	nCurAni = 0;
	pAniTSSN = NULL;
}

void _TILE_SUB_SET_DATA_TOOL::Save(CFile &file)
{
/*	file.Write(szRemark, 64);
	file.Write(&nWidth, sizeof(int));
	file.Write(&nHeight, sizeof(int));
	file.Write(&nAniCount, sizeof(int));
	for(int i = 0; i < nAniCount; i++)
	{
		file.Write(pnFlags[i], sizeof(int)*nWidth*nHeight);
		file.Write(pptSrcs[i], sizeof(POINT)*nWidth*nHeight);
	}
*/
	file.Write(szRemark, 64);
	file.Write(&nWidth, sizeof(int));
	file.Write(&nHeight, sizeof(int));
	file.Write(&nAniCount, sizeof(int));

	file.Write(pnFlags[0], sizeof(int)*nWidth*nHeight);
	file.Write(pptSrcs[0], sizeof(POINT)*nWidth*nHeight);

	if( nAniCount > 1 )
		file.Write(pAniTSSN, sizeof(BYTE)*(nAniCount));
}

void _TILE_SUB_SET_DATA_TOOL::Load(CFile &file)
{
/*	Release();

	file.Read(szRemark, 64);
	file.Read(&nWidth, sizeof(int));
	file.Read(&nHeight, sizeof(int));
	file.Read(&nAniCount, sizeof(int));
	for(int i = 0; i < nAniCount; i++)
	{
		pnFlags[i] = new int[nWidth*nHeight];
		pptSrcs[i] = new POINT[nWidth*nHeight];
		file.Read(pnFlags[i], sizeof(int)*nWidth*nHeight);
		file.Read(pptSrcs[i], sizeof(POINT)*nWidth*nHeight);
	}
	nCurAni = 0;
*/
	Release();

	file.Read(szRemark, 64);
	file.Read(&nWidth, sizeof(int));
	file.Read(&nHeight, sizeof(int));
	file.Read(&nAniCount, sizeof(int));

	pnFlags[0] = new int[nWidth*nHeight];
	pptSrcs[0] = new POINT[nWidth*nHeight];
	file.Read(pnFlags[0], sizeof(int)*nWidth*nHeight);
	file.Read(pptSrcs[0], sizeof(POINT)*nWidth*nHeight);
	if( nAniCount > 1 )
	{
		pAniTSSN = new BYTE[nAniCount - 1];
		file.Read(pAniTSSN, sizeof(BYTE)*(nAniCount));
	}

	nCurAni = 0;

}

int _TILE_SUB_SET_DATA_TOOL::Alloc(int width, int height)
{
	if(width <= 0 || height <= 0) return -1;

	if(nAniCount > 0)
	{
		if(nWidth != width || nHeight != height) return -2;
		if(nAniCount >= MAX_ANI_COUNT) return -3;
	}
	else
	{
		nWidth = width;
		nHeight = height;
	}

	if(pnFlags[nAniCount] == NULL) pnFlags[nAniCount] = new int[nWidth*nHeight];
	if(pptSrcs[nAniCount] == NULL) pptSrcs[nAniCount] = new POINT[nWidth*nHeight];

	ZeroMemory(pnFlags[nAniCount], sizeof(int)*width*height);
	FillMemory(pptSrcs[nAniCount], sizeof(POINT)*width*height, 0xff);

	nAniCount++;

	return nAniCount-1;
}

void _TILE_SUB_SET_DATA_TOOL::Release()
{
/*
	if(nAniCount > 0)
	{
		for(int i = 0; i < nAniCount; i++)
		{
			delete[] pnFlags[i]; pnFlags[i] = NULL;
			delete[] pptSrcs[i]; pptSrcs[i] = NULL;
		}
		nAniCount = 0;
		nCurAni = 0;
	}
*/
	if(nAniCount > 0)
	{
		delete[] pnFlags[0]; pnFlags[0] = NULL;
		delete[] pptSrcs[0]; pptSrcs[0] = NULL;
		nAniCount = 0;
		nCurAni = 0;
		delete[] pAniTSSN;   pAniTSSN = NULL;
	}

}

_TILE_SUB_SET_DATA_TOOL& _TILE_SUB_SET_DATA_TOOL::operator =(_TILE_SUB_SET_DATA_TOOL& other)
{
/*	Release();

	CopyMemory(szRemark, other.szRemark, 64);
	nWidth = other.nWidth;
	nHeight = other.nHeight;
	nAniCount = other.nAniCount;

	for(int i = 0; i < nAniCount; i++)
	{
		pnFlags[i] = new int[nWidth*nHeight];
		pptSrcs[i] = new POINT[nWidth*nHeight];
		CopyMemory(pnFlags[i], other.pnFlags[i], sizeof(int)*nWidth*nHeight);
		CopyMemory(pptSrcs[i], other.pptSrcs[i], sizeof(POINT)*nWidth*nHeight);
	}
	nCurAni = 0;

	return *this;
*/

	Release();

	CopyMemory(szRemark, other.szRemark, 64);
	nWidth = other.nWidth;
	nHeight = other.nHeight;
	nAniCount = other.nAniCount;

	pnFlags[0] = new int[nWidth*nHeight];
	pptSrcs[0] = new POINT[nWidth*nHeight];
	CopyMemory(pnFlags[0], other.pnFlags[0], sizeof(int)*nWidth*nHeight);
	CopyMemory(pptSrcs[0], other.pptSrcs[0], sizeof(POINT)*nWidth*nHeight);
	if( nAniCount > 1 )
	{
		pAniTSSN = new BYTE[nAniCount - 1];
		CopyMemory(pAniTSSN, other.pAniTSSN,sizeof(BYTE)*(nAniCount));
	}
	nCurAni = 0;

	return *this;

}

int _TILE_SUB_SET_DATA_TOOL::Increase(int index)
{
	index++;
	if(index >= nAniCount - 1) index = 0;

	return index;
}

int _TILE_SUB_SET_DATA_TOOL::Decrease(int index)
{
	index--;

	if(index < 0) index = nAniCount - 1;

	return index;
}

void _TILE_SUB_SET_DATA_TOOL::Increase()
{
	nCurAni = Increase(nCurAni);
}

void _TILE_SUB_SET_DATA_TOOL::Decrease()
{
	nCurAni = Decrease(nCurAni);
}

