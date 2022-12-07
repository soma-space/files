// AniObj.h: interface for the CAniObjLSP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIOBJLSP_H_INCLUDED_)
#define AFX_ANIOBJLSP_H_INCLUDED_

#include "AniObj.h"

#pragma warning(disable:4786)
#include <map>
#include <algorithm>
typedef int key_type;
typedef DWORD value_type;

typedef	std::map<key_type,value_type> MAP_OBJLSP;
typedef	MAP_OBJLSP::iterator Map_iter;
typedef	MAP_OBJLSP::value_type Map_value;

class CAniObjLSP : public CAniObj
{
public:
	int					m_RefCount;
	_SPR_HEADER			m_HSpr;

	MAP_OBJLSP			m_mapLSP;
protected:
	_SPR_LSP*	m_pSprs[MAX_SPR];

	virtual void	SprAlloc(int nQt);
	BOOL	Convert8PixelTo16Pixel(BYTE* p8Pixels, WORD *p16Pixels, RGBQUAD* pRGBTbls, int nQt); // 8비트 인덱스 픽셀을 16비트 픽셀로 바꾼다.
	WORD	RGB16(WORD r, WORD g, WORD b) { return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3); } // 565 포맷일때의 인라인 함수로 선언

public:
	void AddRef(){ m_RefCount++; }
	int Release(){ m_RefCount--; return m_RefCount; }
	void RemoveUnUseSpr();

	void		SprRelease();
	COLORREF	SrcCKGet() { return m_HSpr.SrcCK; }
	void	Decode2_555(_SPR_LSP* pSpr); // 565 LSP 를 555 LSP 로 바꾸어준다..
	void	Decode2_565(_SPR_LSP* pSpr); // 555 LSP 를 565 LSP 로 바꾸어준다..
	void	Decode2_Shadow(_SPR_LSP* pSpr);
	void	Decode2_565_1(_SPR_LSP* pSpr); // 565 LSP 를 555 LSP 로 바꾸어준다.. 
	void	Decode2_555_1(_SPR_LSP* pSpr); // 565 LSP 를 555 LSP 로 바꾸어준다.. 



	BOOL	SprGet(int index, const _SPR_LSP** ppSpr);
	int		SprGetAnimated(const _SPR_LSP** ppSpr, int nCtrl = 0); // 에니메이션 되고 있는 스프라이트를 가져온다.
	virtual void SprLoad(const char* szFileName, int nPF);

	virtual void SprLoadEx(const char* szFileName, int nPF, int spl_index);

	
	CAniObjLSP();
	virtual ~CAniObjLSP();
};

#endif // !defined(AFX_ANIOBJLSP_H_INCLUDED_)
