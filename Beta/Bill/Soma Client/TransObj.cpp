// TransObj.cpp: implementation of the CTransObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TransObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransObjData::CTransObjData()
{
	m_TickTime		= GetTickCount();
	m_trans_degree  = 31;
}

CTransObjData::~CTransObjData()
{

}

BOOL CTransObjData::TransDegree(int* trans_degree,BOOL* bHalfed, BOOL bObj_In)
{
	DWORD dwNowTime = GetTickCount();

	if( dwNowTime > m_TickTime + TRANS_OBJ_GAP_TIME )
	{
		m_TickTime = dwNowTime;
		if( bObj_In ) 
		{
			m_trans_degree -= TRANS_OBJ_STEP;
			if( m_trans_degree < TRANS_OBJ_LOW){ m_trans_degree = TRANS_OBJ_LOW; }
		}
		else
		{
			m_trans_degree += TRANS_OBJ_STEP;
			if( m_trans_degree > TRANS_OBJ_HIGH){ m_trans_degree = TRANS_OBJ_HIGH; }
		}
	}
	if( m_trans_degree == TRANS_OBJ_LOW ) *bHalfed = TRUE;
	*trans_degree = m_trans_degree;
	if( m_trans_degree == TRANS_OBJ_HIGH ) return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransObj::CTransObj()
{

}

CTransObj::~CTransObj()
{
	POSITION pos = m_mapTransObj.GetStartPosition();
	int key;
	CTransObjData *value;

	while(pos)
	{
		m_mapTransObj.GetNextAssoc(pos, key, value);
		delete value;
	}
	m_mapTransObj.RemoveAll();

}

BOOL CTransObj::TransDegree(int key, int* trans_degree,BOOL* bHalfed, BOOL bObj_In)
{
	CTransObjData* trans_obj_data;

	if(m_mapTransObj.Lookup(key, trans_obj_data))
	{
		if( trans_obj_data->TransDegree(trans_degree, bHalfed,bObj_In) )
		{
			if( trans_obj_data ) delete trans_obj_data; trans_obj_data = NULL;
			m_mapTransObj.RemoveKey(key); 
		}
		return TRUE;
	}
	else if( !bObj_In ) return FALSE;

	trans_obj_data = new CTransObjData;

	if (trans_obj_data)
	{
		m_mapTransObj[key] = trans_obj_data;
		if( trans_obj_data->TransDegree(trans_degree, bHalfed, bObj_In) )
		{
			if( trans_obj_data ) delete trans_obj_data; trans_obj_data = NULL;
			m_mapTransObj.RemoveKey(key); 
		}
		return TRUE;
	}
	else 
	{
		if( trans_obj_data ) delete trans_obj_data; trans_obj_data = NULL;
		return FALSE;
	}
	return FALSE;
}

