// Intf.h: Intf for the CIntf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTF_H__79758CC5_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_)
#define AFX_INTF_H__79758CC5_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAniObjLSP;

class CIntf
{
public:
	_INTF_HEADER	m_Header;

protected:
	_INTF_DATA*	m_pIntfs[MAX_INTF];
	CAniObjLSP* m_pAniObj;

	void	IntfAlloc(int nQt);
	void	IntfRelease();

public:
	CAniObjLSP* AniObjGet() { return m_pAniObj; }
	int		HitTest(int nLayer, POINT pt);
	void	Load(const char* szFileName, const char* szAniPath, const char* szSprPath, int nPF);
	_INTF_DATA* IntfGet(int index);
	_INTF_DATA* IntfGet(int nLayer, int nSeq);

	CIntf();
	virtual ~CIntf();
};

#endif // !defined(AFX_INTF_H__79758CC5_12AF_11D3_B0A3_0060970DBAD5__INCLUDED_)
