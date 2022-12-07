// IntfTool.h: interface for the CIntfTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IntfTool_H__79758CC6_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_)
#define AFX_IntfTool_H__79758CC6_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\\IGObject\\Intf.h"
#include "..\\IGObject\\AniObjLSP.h"

class CIntfTool : public CIntf
{
protected:
	_INTF_DATA* m_pIntfClips[MAX_INTF];

public:
	int ClipBoardGetQt();
	void ClipBoardPaste(int nDestIntf, int nSeq, int nParentLayer);
	void ClipBoardClear();
	void ClipBoardAdd(int index);

	int	 Add();
	void Delete(int index);
	
	void New();
	void Save(const char* szFileName);

	void ImportSpr(const char* szFileName, int nPF);
	void ImportAni(const char* szFileName);

	CIntfTool();
	virtual ~CIntfTool();
};

#endif // !defined(AFX_IntfTool_H__79758CC6_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_)
