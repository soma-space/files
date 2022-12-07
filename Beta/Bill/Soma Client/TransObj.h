// TransObj.h: interface for the CTransObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSOBJ_H__51D3F602_E92E_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_TRANSOBJ_H__51D3F602_E92E_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TRANS_OBJ_GAP_TIME	50
#define TRANS_OBJ_STEP		3
#define TRANS_OBJ_HIGH		32
#define TRANS_OBJ_LOW		16


class CTransObjData
{
private:
	DWORD	m_TickTime;
	int		m_trans_degree;
public:
	CTransObjData();
	virtual ~CTransObjData();
	BOOL TransDegree(int* trans_degree,BOOL* bHalfed, BOOL bObj_In);
};

typedef CMap<int, int, CTransObjData *, CTransObjData *> MAP_TRANSOBJ;

class CTransObj  
{
	MAP_TRANSOBJ m_mapTransObj;

public:
	CTransObj();
	virtual ~CTransObj();
	BOOL TransDegree(int key, int* trans_degree,BOOL* bHalfed, BOOL bObj_In);

};

#endif // !defined(AFX_TRANSOBJ_H__51D3F602_E92E_11D4_956A_00D0B77259E8__INCLUDED_)
