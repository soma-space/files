// ObjLookTool.h: interface for the CObjLookTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJLOOKTOOL_H__72C020A9_56E1_4714_A411_BB7C57B5971F__INCLUDED_)
#define AFX_OBJLOOKTOOL_H__72C020A9_56E1_4714_A411_BB7C57B5971F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\IGObject\\ObjLook.h"

class CObjLookTool : public CObjLook 
{
public:
	void Save(const char* szFileName);
	CObjLookTool();
	virtual ~CObjLookTool();

};

#endif // !defined(AFX_OBJLOOKTOOL_H__72C020A9_56E1_4714_A411_BB7C57B5971F__INCLUDED_)
