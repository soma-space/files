// MagicRes.h: interface for the CMagicRes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAGICRES_H__652BC4EE_5F86_11D3_8424_00105A6B96E7__INCLUDED_)
#define AFX_MAGICRES_H__652BC4EE_5F86_11D3_8424_00105A6B96E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "illstruct.h"
typedef CTypedPtrArray<CPtrArray, PWORD>			ARR_WORDDATA;
typedef CTypedPtrArray<CPtrArray, _SPR_LSP*>		ARR_SPRDATA;

class CDDSurface;
class CMagicRes  
{
public:
	CMagicRes();
	virtual ~CMagicRes();

	void				DrawMagic(CDDSurface *pSurface, int x, int y, int nFrame, int Alpha = 1);	//단순히 주어진 위치에 nFrame번째의 그림을 그림
	BOOL				LoadMagic(CString &path);			//마법 Resource를 로드함
	int					GetMagicFrameNum(){return m_HSpr.nQt;}	//마법 Resource의 Frame수

	static	DWORD		dwMagicDelay;				//마법 Frame변화의 Delay
protected:
	CString				m_szMagicResFileName;		//마법 Resource파일 이름

	_SPR_HEADER			m_HSpr;						//CharRes와 동일한 구조(CharRes참고할것)
	ARR_WORDDATA		m_arrFxData;
	ARR_SPRDATA			m_arrSprData;
};

#endif // !defined(AFX_MAGICRES_H__652BC4EE_5F86_11D3_8424_00105A6B96E7__INCLUDED_)
