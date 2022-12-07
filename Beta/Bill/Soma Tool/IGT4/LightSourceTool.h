// LightSourceTool.h: interface for the CLightSourceTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTSOURCETOOL_H__4560A6E4_49D3_438C_994D_F7A0B8763512__INCLUDED_)
#define AFX_LIGHTSOURCETOOL_H__4560A6E4_49D3_438C_994D_F7A0B8763512__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LightSource.h"

class CLightSourceTool : public CLightSource  
{
public:
	void Save(const char* szFileName);
	CLightSourceTool();
	virtual ~CLightSourceTool();

};

#endif // !defined(AFX_LIGHTSOURCETOOL_H__4560A6E4_49D3_438C_994D_F7A0B8763512__INCLUDED_)
