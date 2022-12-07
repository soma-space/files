// Arrow.cpp: implementation of the CArrow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Arrow.h"
#include "magic.h"
#include "DManage/BB_Direct.h"
#include <math.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
double CArrow::xrad[MAX_DIRECTION];
double CArrow::yrad[MAX_DIRECTION];
double CArrow::xgaprad[MAX_DIRECTION];
double CArrow::ygaprad[MAX_DIRECTION];

CArrow::CArrow()
{

}

CArrow::CArrow(const CArrow &arrow)
{
	m_ptSor = arrow.m_ptSor;
	m_ptDst = arrow.m_ptDst;
	m_ptCur = arrow.m_ptCur;
	m_nDirection = arrow.m_nDirection;
	m_nArrowKind = arrow.m_nArrowKind;
	m_bSuccess = arrow.m_bSuccess;
}

CArrow::CArrow(int nUid, CPoint pt_sor, CPoint pt_dst, BOOL bSuccess, int nArrowKind, int effect_index)
{
	m_ptSor			= pt_sor;
	m_ptDst			= pt_dst;
	m_ptCur			= pt_sor;
	m_ptLineEnd		= m_ptSor;
	m_ptCur.y		-= HEIGHT;
	m_ptLineEnd.y	-= HEIGHT;
	m_nDirection	= 0;
	m_nUid			= nUid;
	m_nArrowKind	= nArrowKind;
	m_nCurFrame		= 0;
	m_effect_index	= effect_index;
	m_bSuccess		= bSuccess;
	m_dwTickTime	= GetTickCount();
}

CArrow & CArrow::operator=(const CArrow & arrow)
{
	if( this == &arrow ) return *this;

	m_ptSor = arrow.m_ptSor;
	m_ptDst = arrow.m_ptDst;
	m_ptCur = arrow.m_ptCur;
	m_nDirection = arrow.m_nDirection;
	m_nArrowKind = arrow.m_nArrowKind;
	m_bSuccess = arrow.m_bSuccess;

	return *this;
}

CArrow::~CArrow()
{

}

void CArrow::DrawLine(CDDSurface* pSurface,CPoint ptLineEnd,CPoint ptDst,COLORREF line_color)
{
	CDC* pDC = pSurface->GetDC();
	CPen pen,*oldpen;
	pen.CreatePen(PS_GEOMETRIC,1,line_color);
	oldpen = pDC->SelectObject(&pen);
//	CPoint ptLineEnd = m_ptSor;
//	ptLineEnd.y -= HEIGHT;
//	ptLineEnd.y -= HEIGHT;
//	pDC->MoveTo(ptSor);
	pDC->MoveTo(ptLineEnd);
	pDC->LineTo(m_ptCur);
	pDC->SelectObject(oldpen);
	pen.DeleteObject();
	pSurface->ReleaseDC( pDC );
}

BOOL CArrow::Draw(CDDSurface* pSurface,CAniObjLSP* arrowres)
{
/*
	double distance = sqrt( (m_ptCur.x - m_ptLineEnd.x)*(m_ptCur.x - m_ptLineEnd.x)
				+ (m_ptCur.y - m_ptLineEnd.y)*(m_ptCur.y - m_ptLineEnd.y) );

	if (distance < 100)
	{
		DrawLine(pSurface,m_ptLineEnd,m_ptCur,RGB(127,127,127));
	}
	else
	{
		if( m_bSuccess )
		{
			m_ptLineEnd.x += int(m_dx*MOVE_RATE);
			m_ptLineEnd.y += int(m_dy*MOVE_RATE);
		}
		else
		{
			m_ptLineEnd.x += int(xrad[m_nDirection]*MOVE_RATE);
			m_ptLineEnd.y += int(yrad[m_nDirection]*MOVE_RATE);

		}
//		DrawLine(pSurface,m_ptLineEnd,m_ptCur,RGB(127,127,127));
		DrawLine(pSurface,m_ptLineEnd,m_ptCur,RGB(255,255,255));
	}
*/
	const _SPR_LSP* pSpr = NULL;

	int nSpr = arrowres->FrmGetByAniAndDirAndSeq(0, m_nDirection, m_nCurFrame);
	if( nSpr == -1 )
	{
		m_nCurFrame = 0;
		nSpr = arrowres->FrmGetByAniAndDirAndSeq(0, m_nDirection, m_nCurFrame);
	}
	arrowres->SprGet(nSpr, &pSpr);
	if( pSpr )
	{
		RECT rcV = pSpr->rcV;
		PWORD wpV = pSpr->pwV;
		CUser::BltLSPDrawObj(pSurface, m_ptCur.x, m_ptCur.y, &rcV, wpV);
	}
//	arrowres->Draw(pSurface,m_ptCur,m_nDirection,FALSE);
	return Move();
}
void CArrow::Setrad()
{
	double pi = 3.14159265358;
	double rad_degree = pi/180;
	double gap_degree = 360.0/MAX_DIRECTION;

	for( int i = 0; i < MAX_DIRECTION ; i++ )
	{
		yrad[i]		= -cos(gap_degree*i*rad_degree);
		xrad[i]		= sin(gap_degree*i*rad_degree);
		ygaprad[i]	= -cos(( gap_degree*i - gap_degree/2 )*rad_degree);
		xgaprad[i]	= sin(( gap_degree*i - gap_degree/2 )*rad_degree);
	}
}

int CArrow::SetDirection()
{
	double distance = sqrt( (m_ptDst.x - m_ptSor.x)*(m_ptDst.x - m_ptSor.x)
				+ (m_ptDst.y - m_ptSor.y)*(m_ptDst.y - m_ptSor.y) );
	if (distance > 0)
	{
		m_dx = double(m_ptDst.x - m_ptSor.x) / distance;
		m_dy = double(m_ptDst.y - m_ptSor.y) / distance;
	}else return -1;

	if( m_dx >= 0 )
	{
		if( m_dy <= ygaprad[1] ) return 0;
		if( m_dy >= ygaprad[MAX_DIRECTION/2] ) return MAX_DIRECTION/2;
		for( int i = 0; i < MAX_DIRECTION/2 ; i++ )
		{
			if( m_dy < ygaprad[i+1] && m_dy >= ygaprad[i] )
			{
				TRACE("DIRECTION %d\n",i);
				return i;
			}
		}
	}

	if( m_dx <= 0 )
	{
		int ii = 0;
		if( m_dy >= ygaprad[MAX_DIRECTION/2] ) return MAX_DIRECTION/2;
		if( m_dy <= ygaprad[0] ) return 0;
		for( int i = MAX_DIRECTION/2; i < MAX_DIRECTION ; i++ )
		{
			if( (i+1) == MAX_DIRECTION ) ii = 0;
			else						 ii = i + 1;
			if( m_dy < ygaprad[i] && m_dy >= ygaprad[ii] )
			{
				TRACE("DIRECTION %d\n",i);
				return i;
			}
		}
	}
	return -1;

}
#include "UserManager.h"
#include "User.h"
#include "blood.h"
#include "ChildView.h"

BOOL CArrow::Move()
{
	DWORD dwNowTime = GetTickCount();
	if( dwNowTime > m_dwTickTime + 50 )
	{
		m_dwTickTime = dwNowTime;
		m_nCurFrame++;

		int nArrSize = m_arrRoute.GetSize();
		if( nArrSize == 0 )
		{
			if( m_bSuccess )
			{
				CRect rSurf(0,0,800,600);
				CUser* pUser = CChildView::UserManager.GetUserWithUid( m_nUid );
				if( pUser )
				{
					CPoint ptRenderingPos = pUser->GetPosition()-CChildView::MapManager.GetMapStartPos();
					if(CChildView::UserManager.IsInRange(rSurf, pUser, ptRenderingPos)) // 화면에 나타날 영역인가 확인
						CChildView::UserManager.m_Blood->Add( m_nUid,(int)pUser->m_BloodType,pUser->GetPosition(), TRUE );
				}

				CChildView::UserManager.ActionMotion(m_nUid, 1, 200);	// 맞는 동작을 뒤에 취한다.
				if( m_effect_index > 0 )
					CChildView::UserManager.SetMagic( m_nUid, m_effect_index, MAGIC_TYPE_UP );

			}

			return TRUE;
		}
		m_arrRoute.RemoveAt(0);
		if( nArrSize != 1 )
			m_ptCur = m_arrRoute.GetAt(0);
	}
	return FALSE;
}

BOOL CArrow::GetRoute( BOOL bSuccess )
{
	m_nDirection = SetDirection();
	TRACE("direction %d\n",m_nDirection);
	if( m_nDirection == -1 ) return FALSE;
	CPoint pt;
	double distance;
	int i = 0;
	if( bSuccess )
	{
		while(1)
		{
			pt.x = m_ptSor.x + int(m_dx*MOVE_RATE*i);
			pt.y = m_ptSor.y + int(m_dy*MOVE_RATE*i);
			distance = sqrt( (m_ptDst.x - pt.x)*(m_ptDst.x - pt.x)
							+ (m_ptDst.y - pt.y)*(m_ptDst.y - pt.y) );
			if( distance < 20 )
			{
				TRACE("S:%d 경로를 구함\n",i);
				break;
			}
			pt.y -= HEIGHT;
			m_arrRoute.Add( pt );
			i++;
		}
	}
	else
	{
		while(1)
		{
			pt.x = m_ptSor.x + int(xrad[m_nDirection]*MOVE_RATE*i);
			pt.y = m_ptSor.y + int(yrad[m_nDirection]*MOVE_RATE*i);

			if( xrad[m_nDirection] > 0 && pt.x > SCREEN_XSIZE )
			{
				TRACE("S:%d 경로를 구함\n",i);
				break;
			}
			if( xrad[m_nDirection] < 0 && pt.x < 0 )
			{
				TRACE("S:%d 경로를 구함\n",i);
				break;
			}

			if( yrad[m_nDirection] > 0 && pt.y > SCREEN_YSIZE )
			{
				TRACE("S:%d 경로를 구함\n",i);
				break;
			}
			if( yrad[m_nDirection] < 0 && pt.y < 0 )
			{
				TRACE("S:%d 경로를 구함\n",i);
				break;
			}

			pt.y -= HEIGHT;
			m_arrRoute.Add( pt );
			i++;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArrowManager::CArrowManager()
{
	CArrow::Setrad();
}

CArrowManager::~CArrowManager()
{
	CArrow* arrow = NULL;
	int nArrSize = m_arrArrow.GetSize();
	for( int i = 0 ; i < nArrSize ; i++ )
	{
		arrow = m_arrArrow.GetAt(i);
		if( arrow )
		{
			delete arrow; arrow = NULL;
		}
	}

	POSITION pos = m_mapArrowRes.GetStartPosition();
	int key;
	CAniObjLSP *value;

	while(pos)
	{
		m_mapArrowRes.GetNextAssoc(pos, key, value);
		delete value;
	}
	m_mapArrowRes.RemoveAll();

}

void CArrowManager::Add( int nUid ,CPoint pt_sor, CPoint pt_dst, int arrow_kind, int effect_index, BOOL bSuccess )
{
	CArrow* arrow = new CArrow( nUid, pt_sor , pt_dst, bSuccess, arrow_kind, effect_index );

	if( !arrow->GetRoute( bSuccess ) )
	{
		delete arrow; arrow = NULL;
		return;
	}
	if( !Load( arrow_kind, arrow->GetDirection() ) )
	{
		delete arrow; arrow = NULL;
		return;
	}
	m_arrArrow.Add( arrow );
}

CAniObjLSP* CArrowManager::GetIndexedBlood(int nArrowKind)
{
	CAniObjLSP* arrowres;
	if(m_mapArrowRes.Lookup(nArrowKind, arrowres) == TRUE) return arrowres;
	else return NULL;
}

BOOL CArrowManager::Load( int arrow_kind, int nDirection )
{
	CAniObjLSP* arrowres;

	if(m_mapArrowRes.Lookup(arrow_kind, arrowres))
	{
		arrowres->AddRef();
		return TRUE;
	}

	arrowres = new CAniObjLSP;

	int nSpr;
	const _SPR_LSP* pSpr = NULL;
	CString szFileName;
	szFileName.Format(".\\magicres\\arrow%03d.ani",arrow_kind);

	arrowres->AniLoad(szFileName);
	arrowres->AniCtrlAlloc(256);
	szFileName.Format(".\\magicres\\arrow%03d.spl",arrow_kind);
	
	arrowres->SprLoad(szFileName, CChildView::CharManager.GetPixelFormat()); // 스프라이트 파일 읽기.
	nSpr = arrowres->FrmGetByAniAndDirAndSeq(0, nDirection, 0);
	arrowres->SprGet(nSpr, &pSpr);

	if(pSpr != NULL)
	{
		m_mapArrowRes[arrow_kind] = arrowres;
		arrowres->AddRef(); 
	}
	else 
	{
		TRACE("LOADFAIL\n");
		if(arrowres) delete arrowres; arrowres = NULL;
		return FALSE;
	}
	return TRUE;

}

BOOL CArrowManager::DrawArrow(CDDSurface* pSurface)
{
	CArrow* arrow;
	CAniObjLSP* arrowres;

	int arrow_size = m_arrArrow.GetSize();
	for( int i = 0; i < arrow_size ; i++ )
	{
		arrow = m_arrArrow.GetAt(i);
		if( arrow == NULL ) continue;
		
		arrowres = GetIndexedBlood(arrow->GetArrowKind()); 
		if( arrowres == NULL ) 
		{
			if( arrow ) delete arrow; arrow = NULL;
			m_arrArrow.RemoveAt(i);
			arrow_size--; i--;
			continue;
		}

		if(arrow->Draw(pSurface,arrowres))
		{
			if( arrowres->Release() <= 0 ) 
			{
				if( arrowres ) delete arrowres; arrowres = NULL;
				m_mapArrowRes.RemoveKey(arrow->GetArrowKind()); 
			}
			if( arrow ) delete arrow; arrow = NULL;
			m_arrArrow.RemoveAt(i);
			arrow_size--; i--;
		}
		else
		{

		}
	}
	return TRUE;
}
