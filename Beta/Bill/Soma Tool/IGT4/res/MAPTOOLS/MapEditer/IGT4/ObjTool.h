// ObjTool.h: interface for the CObjTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJTOOL_H__0BE0631D_625F_4D58_A9A5_C697E1B6DF0B__INCLUDED_)
#define AFX_OBJTOOL_H__0BE0631D_625F_4D58_A9A5_C697E1B6DF0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\\IGObject\\ObjBasic.h"
#include "..\\IGObject\\AniObjLSP.h"

class CObjTool : public CObjBasic
{
protected:
	_SPR_LSP* m_pSprStands[MAX_OBJ_ANI];
public:
	void Save(const char* szFileName, int nPF);
	CObjTool();
	virtual ~CObjTool();

public:
	void Release();
	const _SPR_LSP* StandLSPGet(int index) { if(index < 0 || index >= MAX_OBJ_ANI) return NULL; return m_pSprStands[index]; }
	CAniObjLSP* AniObjGet(int index) { if(index < 0 || index >= MAX_OBJ_ANI) return NULL; return m_pAniObjs[index]; }
	
	void LoadHeaderAndStandImage(const char* szFileName, int nPF);
	
	void SprLoad(int index, const char* szFileName, int nPF);
	void SprRelease(int index);

	void New();
};

#endif // !defined(AFX_OBJTOOL_H__0BE0631D_625F_4D58_A9A5_C697E1B6DF0B__INCLUDED_)
