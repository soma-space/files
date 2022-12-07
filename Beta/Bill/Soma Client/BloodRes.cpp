// BloodRes.cpp: implementation of the CBloodRes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "soma.h"
#include "CharRes.h"
#include "CharManager.h"
#include "ChildView.h"
#include "User.h"
#include "DManage/BB_Direct.h"
#include "BloodRes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBloodRes::CBloodRes()
{
	m_RefCount = 0;
}

CBloodRes::~CBloodRes()
{
	_SPR_LSP* lsp	= NULL;
	WORD*	data	= NULL;

	int nArrSize = m_arrFxData.GetSize();
	for( int i = 0 ; i < nArrSize ; i++ )
	{
		data = m_arrFxData.GetAt( i );
		SAFE_DELETE( data );
	}
	m_arrFxData.RemoveAll();
	nArrSize = m_arrSprData.GetSize();
	for( i = 0 ; i < nArrSize; i++ )
	{
		lsp = m_arrSprData.GetAt( i );
		SAFE_DELETE( lsp );
	}
	m_arrSprData.RemoveAll();

}
void CBloodRes::AddRef()
{
	m_RefCount++;
}
int CBloodRes::Release()
{
	m_RefCount--;
	return m_RefCount;
}

BOOL CBloodRes::Draw(CDDSurface* pSurface,CPoint ptPos, int nCurFrame, BOOL bNext)
{
	RECT*	pRC		= NULL;
	WORD*	data	= NULL;

	if( m_HSpr.nQt <= nCurFrame ) return TRUE; 
	data = m_arrFxData.GetAt( nCurFrame );
	pRC = &m_arrSprData.GetAt( nCurFrame )->rcV;
//	if( nCurFrame < m_HSpr.nQt /2 )
		CUser::BltLSPNormal(pSurface, ptPos.x, ptPos.y, pRC, data);
//	else
//		CUser::BltLSPTrans(pSurface, ptPos.x, ptPos.y, pRC, data, 16 ); 
	return FALSE;
}
BOOL CBloodRes::LoadRes(int nBloodKind)
{
	CString szFileName;
	szFileName.Format(".\\magicres\\blood%03d.spl",nBloodKind);
	CFile file;
	if (!file.Open(szFileName, CFile::modeRead))
	{
		TRACE("%s Load실패\n", szFileName);
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
		nCount = m_arrSprData[i]->nDataSize;
		if (nCount)
		{
			pWordData = new WORD[nCount>>1];
			file.Read((PVOID)pWordData, nCount);
			if (CChildView::CharManager.GetPixelFormat()==PF_555) CCharRes::Decode2_555(m_arrSprData[i]->nLine, pWordData);
			m_arrFxData[i] = pWordData;	//실제 마법 Resource (LSP포맷임)
		}
	}
	nCount = file.GetPosition();

	file.Close();

	return TRUE;
}

BOOL CBloodRes::LoadRes(CString szFileName)
{
	CFile file;
	if (!file.Open(szFileName, CFile::modeRead))
	{
		TRACE("%s Load실패\n", szFileName);
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
		nCount = m_arrSprData[i]->nDataSize;
		if (nCount)
		{
			pWordData = new WORD[nCount>>1];
			file.Read((PVOID)pWordData, nCount);
			if (CChildView::CharManager.GetPixelFormat()==PF_555) CCharRes::Decode2_555(m_arrSprData[i]->nLine, pWordData);
			m_arrFxData[i] = pWordData;	//실제 마법 Resource (LSP포맷임)
		}
	}
	nCount = file.GetPosition();

	file.Close();

	return TRUE;
}
