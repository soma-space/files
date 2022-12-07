// Blood.cpp: implementation of the CBlood class.
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
#include "Blood.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBlood::CBlood()
{
}

CBlood::~CBlood()
{
	BLOOD_INFO* stBlood_info;
	int nArrSize = m_arrBlood.GetSize();
	for( int i = 0 ; i < nArrSize ; i++ )
	{
		stBlood_info = m_arrBlood.GetAt(i);
		if(stBlood_info)
		{
			delete stBlood_info;
			stBlood_info = NULL;
		}
	}

	m_arrBlood.RemoveAll(); 

	POSITION pos = m_mapBloodRes.GetStartPosition();
	int key;
	CBloodRes *value;

	while(pos)
	{
		m_mapBloodRes.GetNextAssoc(pos, key, value);
		delete value;
	}
	m_mapBloodRes.RemoveAll();
}

void CBlood::Add( int uid, int type, CPoint ptBlood ,BOOL bAttackBlood)
{
	BLOOD_INFO* stBlood_info = new BLOOD_INFO;
	int nBloodKind;
	if( stBlood_info )
	{
		if( bAttackBlood )
		{
			nBloodKind = type*10 + rand()%5 + 4;
		}
		else
		{
			nBloodKind = type*10 + rand()%3 + 1;
		}
		if( Load(nBloodKind) )
		{
			DWORD dwNowTime = GetTickCount();
			stBlood_info->m_nUid		= uid;
			stBlood_info->m_nCurFrame	= 0;
			stBlood_info->m_ptBlood		= ptBlood;
			stBlood_info->m_nBloodKind	= nBloodKind; 
			stBlood_info->m_TickTime	= dwNowTime;
			stBlood_info->m_bStained	= FALSE;
			stBlood_info->m_bAttackBlood= bAttackBlood; 
			m_arrBlood.Add(stBlood_info); 
		}
		else TRACE("BLOOD RES LOAD FAIL, BLOODKIND:%d \n",nBloodKind); 
	}
}

BOOL CBlood::Load( int nBloodKind )
{
	CBloodRes* bloodres;

	if(m_mapBloodRes.Lookup(nBloodKind, bloodres))
	{
		bloodres->AddRef();
		return TRUE;
	}

	bloodres = new CBloodRes;

	if (bloodres->LoadRes(nBloodKind))
	{
		m_mapBloodRes[nBloodKind] = bloodres;
		bloodres->AddRef(); 
	}
	else 
	{
		SAFE_DELETE(bloodres);
		return FALSE;
	}
	return TRUE;
}

CBloodRes* CBlood::GetIndexedBlood(int nBloodKind)
{
	CBloodRes* bloodres;
	if(m_mapBloodRes.Lookup(nBloodKind, bloodres) == TRUE) return bloodres;
	else return NULL;
}

BOOL CBlood::DrawBloodStain(CDDSurface *pSurface,CPoint ptMapPos)
{
	int nArrSize = m_arrBlood.GetSize();
	if( nArrSize == 0 ) return FALSE;

	DWORD	dwNowTime = GetTickCount();
	CPoint	ptPos;
	RECT*	pRC		= NULL;
	WORD*	data	= NULL;
	
	BLOOD_INFO* stBlood_info;
	CBloodRes* bloodres;

	for( int i = 0 ; i < nArrSize ; i++ )
	{
		stBlood_info = m_arrBlood.GetAt(i);
		if(stBlood_info)
		{
			if(stBlood_info->m_bStained == FALSE)	 continue; 
//			CUser* pDstUser = CChildView::UserManager.GetUserWithUid( stBlood_info->m_nUid );
//			if( pDstUser )
//			{
				ptPos = stBlood_info->m_ptBlood - ptMapPos; 
				bloodres = GetIndexedBlood(stBlood_info->m_nBloodKind); 
				if( bloodres == NULL ) return FALSE;
				if( bloodres->Draw(pSurface,ptPos, stBlood_info->m_nCurFrame , FALSE) )
				{
					if( bloodres->Release() <= 0 ) 
					{
						SAFE_DELETE( bloodres );
						m_mapBloodRes.RemoveKey(stBlood_info->m_nBloodKind); 
					}
					SAFE_DELETE( stBlood_info );
					//delete stBlood_info;
					m_arrBlood.RemoveAt(i);
					nArrSize--;i--;
					continue;
				}
				else
				{
					if( dwNowTime >= stBlood_info->m_TickTime + 1000 )
					{
						stBlood_info->m_TickTime = dwNowTime;
						stBlood_info->m_nCurFrame++;
					}
				}
//			}
//			else
//			{
//				delete stBlood_info;
//				m_arrBlood.RemoveAt(i);
//				nArrSize--;i--;
//				continue;
//			}
		}
	}
	return TRUE;
}

BOOL CBlood::Draw(CDDSurface *pSurface,CPoint ptMapPos,int nUid)
{
	int nArrSize = m_arrBlood.GetSize();
	if( nArrSize == 0 ) return FALSE;

	DWORD dwNowTime = GetTickCount();
	CPoint	ptPos;
	RECT*	pRC		= NULL;
	WORD*	data	= NULL;
	
	BLOOD_INFO* stBlood_info;
	CBloodRes* bloodres;
	for( int i = 0 ; i < nArrSize ; i++ )
	{
		stBlood_info = m_arrBlood.GetAt(i);
		if(stBlood_info)
		{
			if(stBlood_info->m_bStained)	 continue; 
			if(stBlood_info->m_nUid != nUid) continue;
			ptPos = stBlood_info->m_ptBlood - ptMapPos; 
			bloodres = GetIndexedBlood(stBlood_info->m_nBloodKind); 
			if( bloodres == NULL ) return FALSE;
			if( bloodres->Draw(pSurface,ptPos, stBlood_info->m_nCurFrame, FALSE) )
			{
				if( bloodres->Release() <= 0 ) 
				{
					SAFE_DELETE( bloodres );
					m_mapBloodRes.RemoveKey(stBlood_info->m_nBloodKind); 
				}
				SAFE_DELETE( stBlood_info );
				//delete stBlood_info;
				m_arrBlood.RemoveAt(i);
				nArrSize--;i--;
				continue;
			}
			
			if( dwNowTime >= stBlood_info->m_TickTime + 100 )
			{
				stBlood_info->m_TickTime = dwNowTime;
				stBlood_info->m_nCurFrame++;

				if( !stBlood_info->m_bAttackBlood && stBlood_info->m_nCurFrame >= bloodres->m_HSpr.nQt/2 ) 
				{
					stBlood_info->m_nCurFrame--; 
					stBlood_info->m_bStained = TRUE;
					continue;
				}

			}
		}
	}
	return TRUE;
}
