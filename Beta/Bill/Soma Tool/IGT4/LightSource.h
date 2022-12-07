// LightSource.h: interface for the CLightSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTSOURCE_H__55486AB7_9AB4_4BEF_8DB2_35374B773F0F__INCLUDED_)
#define AFX_LIGHTSOURCE_H__55486AB7_9AB4_4BEF_8DB2_35374B773F0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _LIGHT_SOURCE_HEADER
{
	char szID[4]; // "LSDF"
	char szRemark[64];
	int nQt;
} LIGHT_SOURCE_HEADER;

typedef struct _LIGHT_SOURCE_DATA
{
	COLORREF crColor;
	RECT rc;
} LIGHT_SOURCE_DATA;

class CLightSource  
{
public:
	_LIGHT_SOURCE_HEADER m_Header;
	_LIGHT_SOURCE_DATA* m_pDatas;

public:
	void Load(const char* szFileName);
	void Alloc(int nQt);
	_LIGHT_SOURCE_DATA* DataGet(int index);
	void Release();
	CLightSource();
	virtual ~CLightSource();

};

#endif // !defined(AFX_LIGHTSOURCE_H__55486AB7_9AB4_4BEF_8DB2_35374B773F0F__INCLUDED_)
