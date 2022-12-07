// MapObjTool.h: interface for the CMapObjTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPOBJTOOL_H__513ED186_DAD9_4189_8CC2_7DD4DC0FA6A6__INCLUDED_)
#define AFX_MAPOBJTOOL_H__513ED186_DAD9_4189_8CC2_7DD4DC0FA6A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\IGObject\\MapObj.h"
#include "ObjTool.h"

class CMapObjTool : public CMapObj
{
public:
	CAniObjLSP m_Objs[MAX_MAP_OBJ_REG]; // 에니메이션, 스프라이트 객체

public:
	void ReleaseEx();
	void Realloc(int nWidth, int nHeight);
	void Save(const char* szFileName);
	void Save(CFile& file);
	CMapObjTool();
	virtual ~CMapObjTool();

};

#endif // !defined(AFX_MAPOBJTOOL_H__513ED186_DAD9_4189_8CC2_7DD4DC0FA6A6__INCLUDED_)
