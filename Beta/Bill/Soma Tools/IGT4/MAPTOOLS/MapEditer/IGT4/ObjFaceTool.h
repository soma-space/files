// ObjFaceTool.h: interface for the CObjFaceTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJFACETOOL_H__72C020A9_56E1_4714_A411_BB7C57B5971F__INCLUDED_)
#define AFX_OBJFACETOOL_H__72C020A9_56E1_4714_A411_BB7C57B5971F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjFace.h"

class CObjFaceTool : public CObjFace 
{
public:
	void Save(const char* szFileName);
	CObjFaceTool();
	virtual ~CObjFaceTool();

};

#endif // !defined(AFX_OBJFACETOOL_H__72C020A9_56E1_4714_A411_BB7C57B5971F__INCLUDED_)
