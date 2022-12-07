// Test.h: interface for the CTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEST_H__2FB2B681_D569_11D3_B175_00105A735718__INCLUDED_)
#define AFX_TEST_H__2FB2B681_D569_11D3_B175_00105A735718__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dialog_Box.h"

class CTest  : public CDialog_Box
{
public:
	CTest();
	virtual ~CTest();

public:
	int m_Test_Del;
	virtual void Draw(CDDSurface* pDestSurf);
	virtual void InitData(CDirectDraw *pDirectDraw);
};

#endif // !defined(AFX_TEST_H__2FB2B681_D569_11D3_B175_00105A735718__INCLUDED_)
