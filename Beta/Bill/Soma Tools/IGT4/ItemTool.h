// ItemTool.h: interface for the CItemTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMTOOL_H__14E698B5_BFB6_4B36_894C_CBDDEDEF8068__INCLUDED_)
#define AFX_ITEMTOOL_H__14E698B5_BFB6_4B36_894C_CBDDEDEF8068__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\IGObject\\Item.h"

class CItemTool : public CItem  
{
public:
	void SprImport(const char* szFileName, int nPF);
	void AniImport(const char* szFileName);
	void Save(const char* szFileName);
	CItemTool();
	virtual ~CItemTool();

};

#endif // !defined(AFX_ITEMTOOL_H__14E698B5_BFB6_4B36_894C_CBDDEDEF8068__INCLUDED_)
