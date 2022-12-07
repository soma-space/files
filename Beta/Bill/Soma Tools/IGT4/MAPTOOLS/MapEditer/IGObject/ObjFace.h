// ObjFace.h: interface for the CObjFace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJFACE_H__36777AC3_B533_4977_8573_B86520EBAAAE__INCLUDED_)
#define AFX_OBJFACE_H__36777AC3_B533_4977_8573_B86520EBAAAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AniObjLSP.h"

class CObjFace
{
protected:
	_FACE_DATA* m_pDatas[MAX_FACE_PART];

public:
	_FACE_HEADER m_Header;
	CAniObjLSP m_AniObj;

public:
	void Alloc(int nQt);
	void Release();
	void Load(const char* szFileName, const char* szAniPath, const char* szSprPath, int nPF);
	const _FACE_DATA* DataGet(int index);
	CObjFace();
	virtual ~CObjFace();

};

#endif // !defined(AFX_OBJFACE_H__36777AC3_B533_4977_8573_B86520EBAAAE__INCLUDED_)
