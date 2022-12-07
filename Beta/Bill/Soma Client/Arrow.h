// Arrow.h: interface for the CArrow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARROW_H__208C3C41_E241_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_ARROW_H__208C3C41_E241_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "User.h"
#include "DManage/BB_Direct.h"
#include "IGObject\\AniObjLsp.h"
#define MAX_DIRECTION 32
#define MOVE_RATE	35
//#define HEIGHT	50
#define HEIGHT	0
#define SCREEN_XSIZE 800
#define SCREEN_YSIZE 600

class CDDSurface;

class CArrow
{
public:
//private:
	CPoint m_ptSor;
	CPoint m_ptDst;
	CPoint m_ptCur;
	CPoint m_ptLineEnd;
	BOOL   m_bSuccess; 
	CArray< CPoint , CPoint > m_arrRoute;
	int m_nArrowKind;
	int m_effect_index;
	int m_nDirection;
	int m_nUid;
	int m_nCurFrame;
	double m_dx,m_dy;//¡ı∞°¿≤
	DWORD m_dwTickTime;

	static double xrad[MAX_DIRECTION];
	static double yrad[MAX_DIRECTION];
	static double xgaprad[MAX_DIRECTION];
	static double ygaprad[MAX_DIRECTION];

public:
	static void Setrad();
	BOOL Draw(CDDSurface* pSurface,CAniObjLSP* arrowres);
	void DrawLine(CDDSurface* pSurface,CPoint ptLineEnd,CPoint ptDst,COLORREF line_color);

	int GetArrowKind(){return m_nArrowKind;}
	int GetDirection(){return m_nDirection;}
	int SetDirection();
	BOOL GetRoute( BOOL bSuccess );
	BOOL Move();

	CArrow();
	CArrow & operator=(const CArrow &);
	CArrow(const CArrow &);
	CArrow(int nUid, CPoint pt_sor, CPoint pt_dst, BOOL bSuccess, int nArrowKind, int effect_index);
	virtual ~CArrow();

};

typedef CMap<int, int, CAniObjLSP *, CAniObjLSP *> MAP_ARROWRES;

class CArrowManager
{
private:
	CArray< CArrow*, CArrow* > m_arrArrow;
	MAP_ARROWRES m_mapArrowRes;

public:
	void Add( int nUid ,CPoint pt_sor, CPoint pt_dst, int arrow_kind, int effect_index, BOOL bSuccess );
	BOOL Load( int arrow_kind, int nDirection );
	CAniObjLSP* GetIndexedBlood(int nArrowKind);

	BOOL DrawArrow(CDDSurface* pSurface);
	CArrowManager();
	virtual ~CArrowManager();

};

#endif // !defined(AFX_ARROW_H__208C3C41_E241_11D4_956A_00D0B77259E8__INCLUDED_)
