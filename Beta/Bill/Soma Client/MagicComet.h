// MagicComet.h: interface for the CMagicComet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAGICCOMET_H__C728F163_A128_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_MAGICCOMET_H__C728F163_A128_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MagicManager.h"
typedef struct _TAG_ST_POSITION
{
	int m_nDir;
	CPoint m_ptPos;
	int m_nCurFrame;

}ST_POSITION;

class CMagicComet  
{
private:
	int m_nIndex;
	CPoint m_ptSor;
	CPoint m_ptDest;
	CString m_strCometName;
	int m_nFrame;
	int m_nFrameInterval;
	int m_nDrawCount;
	DWORD m_dwTime;
	CArray<ST_POSITION*,ST_POSITION*>m_arrRoutingCell;
public:
	static CMagicManager* pMagicManager;
	CMagicComet();
	CMagicComet(CPoint ptSor, CPoint ptDest);
	void SetPosition(CPoint ptSor, CPoint ptDest){ m_ptSor = ptSor; m_ptDest = ptDest;}
	BOOL DrawComet();
	int DirStartFrame(int nDir, int nResMaxFrame);	//지정한 방향의 처음 Frame Index 구하기
	BOOL Load( int nMagicIndex );

	virtual ~CMagicComet();
	int SetRoute();

};

#endif // !defined(AFX_MAGICCOMET_H__C728F163_A128_11D4_956A_00D0B77259E8__INCLUDED_)
