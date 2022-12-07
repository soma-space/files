// MagicRes.cpp: implementation of the CMagicRes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "MagicRes.h"
#include "CharRes.h"
#include "CharManager.h"
#include "ChildView.h"
#include "User.h"
#include "DManage/BB_Direct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


DWORD CMagicRes::dwMagicDelay = 50L;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMagicRes::CMagicRes()
{
}

CMagicRes::~CMagicRes()
{
	int size = m_arrFxData.GetSize();
//	TRACE("10\n");//del
	for (int i=0; i<size; i++)
	{
		SAFE_DELETES(m_arrFxData[i]);
	}
	m_arrFxData.RemoveAll();

	size = m_arrSprData.GetSize();
//	TRACE("11\n");//del
	for (i=0; i<size; i++)
	{
		SAFE_DELETE(m_arrSprData[i]);
	}
	m_arrSprData.RemoveAll();//dspring_0720a

}

BOOL CMagicRes::LoadMagic(CString &path)
{
	CFile file;
	if (!file.Open(path, CFile::modeRead))
	{
		TRACE("마법 Resource : %s Load실패\n", (LPCTSTR)path);
		return FALSE;
	}

	UINT nReadCount;
	nReadCount = file.Read(&m_HSpr, sizeof(_SPR_HEADER));
	ASSERT(nReadCount==sizeof(_SPR_HEADER));
	file.Seek(sizeof(_SPR_TOOL)*m_HSpr.nQt, CFile::current);

	m_arrFxData.SetSize(m_HSpr.nQt);
	m_arrSprData.SetSize(m_HSpr.nQt);

	_SPR_LSP *pSprData;		//데이터 설정에 관한 데이터

	for (int i=0; i<m_HSpr.nQt; i++)
	{
		pSprData = new _SPR_LSP;
		nReadCount = file.Read((PVOID)pSprData, sizeof(_SPR_LSP));
		pSprData->pwV = NULL;
		m_arrSprData[i] = pSprData;
	}

	PWORD	pWordData;
	UINT	nCount;
	for (i=0; i<m_HSpr.nQt; i++)
	{
//		nReadCount = file.Read((PVOID)&nCount, sizeof(UINT));
		nCount = m_arrSprData[i]->nDataSize;
		if (nCount)
		{
			pWordData = new WORD[nCount>>1];
			file.Read((PVOID)pWordData, nCount);
			if (CChildView::CharManager.GetPixelFormat()==PF_555) CCharRes::Decode2_555(m_arrSprData[i]->nLine, pWordData);
			m_arrFxData[i] = pWordData;	//실제 마법 Resource (LSP포맷임)
		}
//		file.Seek(sizeof(UINT)*3, CFile::current);
//		file.Seek(sizeof(UINT)*MAX_PART, CFile::current);
	}
	nCount = file.GetPosition();

	file.Close();
	return TRUE;
}

void CMagicRes::DrawMagic(CDDSurface *pSurface, int x, int y, int nFrame, int Alpha)
{
	//주어진 nFrame을 이용하여 그린다.
	ASSERT(nFrame<m_HSpr.nQt);

	if(Alpha == 1)
	{
		CUser::BltLSPAlphaAdd(pSurface, x, y, &m_arrSprData[nFrame]->rcV, m_arrFxData[nFrame]);
	}
	else 
	{
		CUser::BltLSPNormal(pSurface, x, y, &m_arrSprData[nFrame]->rcV, m_arrFxData[nFrame]);
	}
}