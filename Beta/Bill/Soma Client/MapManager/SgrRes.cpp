// SgrRes.cpp: implementation of the CSgrRes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SgrRes.h"
#include "Tile.h"
#include "Sprite.h"
#include "CustomTile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSgrRes::CSgrRes()
{
	m_iDarkLevel = 100;
	m_iTileCount = 0;// 타일 sgr들 수를 누적시키는 변수들...아래 3개.
	m_iSpriteCount = 0;
	m_iCustomTileCount = 0;
}

CSgrRes::~CSgrRes()
{
	RemoveAllData();
}

BOOL CSgrRes::Load(CString szFileName)
{
	RemoveAllData();

	CFile file;

	if (!file.Open(szFileName, CFile::modeRead|CFile::typeBinary))
	{
		TRACE("지정한 파일이 존재하지 않음\n");
		return FALSE;
	}
	CArchive ar(&file, CArchive::load);

	TRACE("sgr res 시작..\n");

	int				iTileCount;
	int				iSpriteCount;
	int				iCustomTileCount;
	CTile*			pTile = NULL;
	CSprite*		pSprite = NULL;
	CCustomTile*	pCustomTile = NULL;

	BOOL			bErrorOccur = FALSE;


//	ar >> iTileCount;					//타일의 갯수는?
///////////////////////////////////////////
	int tileCount;
	int vVersion;
//	int uniquenumber;
	
	ar >> vVersion;
//	ar >> uniquenumber;
	ar >> tileCount;

	
//////////////////////////////////////////////
	iTileCount = tileCount;
	m_iTileCount += tileCount;
		
	m_apTile.SetSize(m_iTileCount);
	for (int i=0; i<iTileCount; i++)
	{
		pTile = new CTile;
		if (pTile->Load(ar))// 여기가 문제점.....고쳐야 할부분...2월 29일...해결..
		{
			m_apTile.SetAt(i, pTile);
		}
		else
		{
			delete pTile;
			bErrorOccur = TRUE;
		}
	}

	TRACE("sgr res iSpriteCount 읽기전...\n");
	ar >> iSpriteCount;					//Sprite의 갯수는?
	
	m_iSpriteCount += iSpriteCount;// 누적되게 하려구..

	m_apSprite.SetSize(m_iSpriteCount);
	for (i=0; i<iSpriteCount; i++)
	{
		pSprite = new CSprite;
		if (pSprite->Load(ar))
		{
			m_apSprite.SetAt(i, pSprite);
		}
		else
		{
			delete pSprite;
			bErrorOccur = TRUE;
		}
	}

	TRACE("sgr res iCustomTileCount 읽기전...\n");
	ar >> iCustomTileCount;				//CustomTile의 갯수는?
	m_iCustomTileCount += iCustomTileCount;

	m_apCustomTile.SetSize(m_iCustomTileCount);
	for (i=0; i<iCustomTileCount; i++)
	{
		pCustomTile = new CCustomTile;
		if (pCustomTile->Load(ar))
		{
			m_apCustomTile.SetAt(i, pCustomTile);
		}
		else
		{
			delete pCustomTile;
			bErrorOccur = TRUE;
		}
	}

	ar.Close();
	file.Close();
	m_szFileName = szFileName;

	TRACE("sgr res 끝..\n");
	return bErrorOccur? FALSE : TRUE;
}

void CSgrRes::RemoveAllData()
{
	int size = m_apTile.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apTile[i]) delete m_apTile[i];
	}
	m_apTile.RemoveAll();			//Tile제거

	size = m_apSprite.GetSize();
	for (i=0; i<size; i++)
	{
		if (m_apSprite[i]) delete m_apSprite[i];
	}
	m_apSprite.RemoveAll();			//Sprite제거

	size = m_apCustomTile.GetSize();
	for (i=0; i<size; i++)
	{
		if (m_apCustomTile[i]) delete m_apCustomTile[i];
	}
	m_apCustomTile.RemoveAll();		//CustomTile제거
}

void CSgrRes::ReloadResource()
{
	BOOL bResult = Load(m_szFileName);		//다시 재로드
	if (!bResult)
	{
		TRACE("SGR Reload 실패\n");
	}
}

BOOL CSgrRes::SetDarkLevel(int iPercent, BOOL bAbsolute)	//Resource의 어둡기 설정 100Percent면 보통, 0Percent 이면 까맣게
{															//위의 경우는 : bAbsolute가 TRUE인 경우
	BOOL bReloadOccur = FALSE;								//bAbsolute가 False인 경우는 현재의 상태에서 밝기 조절

	if(iPercent == 0)
	{
		m_iDarkLevel = 100;
		ReloadResource();
		return TRUE;
	}

	if (bAbsolute)
	{
		if (iPercent==m_iDarkLevel) return bReloadOccur;

		ReloadResource();		//절대 밝기 이므로 Resource 재로드, 한번이라도 어두워지면 원본 이미지 손상
		bReloadOccur = TRUE;
		if (iPercent==100) return bReloadOccur;

		m_iDarkLevel = iPercent;
		DarkenProcess();
	}
	else
	{
		if (iPercent==100) return bReloadOccur;
		m_iDarkLevel = MulDiv(m_iDarkLevel, iPercent, 100);
		DarkenProcess();
	}
	return bReloadOccur;
}

//현재의 Resource에 m_iDarkLeve를 적용하여 어둡게 만듬
void CSgrRes::DarkenProcess()
{
	COLORREF rgb;
	int darkrgbtone = MulDiv(64, m_iDarkLevel, 100);
	rgb = RGB(darkrgbtone, darkrgbtone, darkrgbtone);
	
	int size = m_apTile.GetSize();
	for (int i=0; i<size; i++)
	{
		if (m_apTile.GetAt(i))
			m_apTile.GetAt(i)->DarkenProcess(rgb);
	}
	
	size = m_apSprite.GetSize();
	for (i=0; i<size; i++)
	{
		if (m_apSprite.GetAt(i))
			m_apSprite.GetAt(i)->DarkenProcess(rgb);
	}

	size = m_apCustomTile.GetSize();
	for (i=0; i<size; i++)
	{
		if (m_apCustomTile.GetAt(i))
			m_apCustomTile.GetAt(i)->DarkenProcess(rgb);
	}
}