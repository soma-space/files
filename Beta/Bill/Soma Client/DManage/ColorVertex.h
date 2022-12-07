// ColorVertex.h: interface for the CColorVertex class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORVERTEX_H__EE6270E3_A9C4_11D4_956A_00D0B77259E8__INCLUDED_)
#define AFX_COLORVERTEX_H__EE6270E3_A9C4_11D4_956A_00D0B77259E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct TAG_VERTEX
{
	DWORD m_dwAlpha;
	DWORD m_dwColor ;
	float m_xPos;
	float m_yPos;
}VERTEX;
class  AFX_EXT_CLASS CColorVertex  
{
public:
	VERTEX m_stVertex[4];

public:
	CColorVertex();
	virtual ~CColorVertex();

};

#endif // !defined(AFX_COLORVERTEX_H__EE6270E3_A9C4_11D4_956A_00D0B77259E8__INCLUDED_)
