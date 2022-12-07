// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__180690A7_F9A1_11D2_8421_0020AF9F40BD__INCLUDED_)
#define AFX_STDAFX_H__180690A7_F9A1_11D2_8421_0020AF9F40BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

#include <ddraw.h>
#include <dinput.h>
#include <mmsystem.h>
#include <dsound.h>
#include <afxtempl.h>

#include "IGObject\\myconstanddef.h"
#include "IGObject\\MyMacro.h"

const int MAX_TILE_SET = 100;
const int MAX_TILE_SUB_SET = 64;
const int MAX_TILE_SET_WIDTH = 128;
const int MAX_TILE_SET_HEIGHT = 128;

typedef struct _TILE_MAP_DATA_TOOL
{
	DWORD snTS		: 10;			// -1 이면 쓰지 않는다..
	DWORD snTSS		: 6;		// -1 이면 사용하지 않음.
	DWORD snNum		: 12;		// -1이면 사용하지 않음.
	DWORD snFlags	: 4;		// 0 - 아무속성 없음. 1 - 충돌 2 - 4 - 8 - 16 - 32 - ......
} _TILE_MAP_DATA_TOOL, *LPTILE_MAP_DATA_TOOL;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__180690A7_F9A1_11D2_8421_0020AF9F40BD__INCLUDED_)
