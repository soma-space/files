// ObjLook.h: interface for the CObjLook class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJ_LOOK_H__36777AC3_B533_4977_8573_B86520EBAAAE__INCLUDED_)
#define AFX_OBJLOOK_H__36777AC3_B533_4977_8573_B86520EBAAAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AniObjLSP.h"

class CObjLook
{
protected:
	_LOOK_DATA*			m_pDatas[MAX_LOOK];

public:
	_LOOK_HEADER m_Header;
	CAniObjLSP m_AniObj;

public:
	void Alloc(int nQt);
	void Release();
	void Load(const char* szFileName, const char* szAniPath, const char* szSprPath, int nPF);
	_LOOK_DATA* DataGet(int index);
	CObjLook();
	virtual ~CObjLook();

};

#endif // !defined(AFX_OBJLOOK_H__36777AC3_B533_4977_8573_B86520EBAAAE__INCLUDED_)
