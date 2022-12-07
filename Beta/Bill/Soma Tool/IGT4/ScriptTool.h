// ScriptTool.h: interface for the CScriptTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTTOOL_H__4F1571A4_BCDF_11D2_B0A3_0060970DBAD5__INCLUDED_)
#define AFX_SCRIPTTOOL_H__4F1571A4_BCDF_11D2_B0A3_0060970DBAD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\IGObject\\Script.h"

class CScriptTool : public CScript  
{
public:
	void Save(const char* szFileName);
	void Set(int index, const _SCT_DATA* pSct);	// 스프라이트의 데이터 바꿈.

public:
	CScriptTool();
	virtual ~CScriptTool();
};

#endif // !defined(AFX_SCRIPTTOOL_H__4F1571A4_BCDF_11D2_B0A3_0060970DBAD5__INCLUDED_)
