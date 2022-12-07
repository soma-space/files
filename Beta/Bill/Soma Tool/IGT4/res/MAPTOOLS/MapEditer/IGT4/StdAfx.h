// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BB99B4E3_7185_4C43_AA24_9D81EBF691E6__INCLUDED_)
#define AFX_STDAFX_H__BB99B4E3_7185_4C43_AA24_9D81EBF691E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>
#include <DDraw.h>

#include "..\\IGObject\\MyConstAndDef.h"
#include "..\\IGObject\\MyMacro.h"

const int MAX_TILE_SET = 100;
const int MAX_TILE_SUB_SET = 64;
const int MAX_TILE_SET_WIDTH = 128;
const int MAX_TILE_SET_HEIGHT = 128;

typedef struct _TILE_MAP_DATA_TOOL
{
	short int snTS;			// -1 이면 쓰지 않는다..
	short int snTSS;		// -1 이면 사용하지 않음.
	short int snNum;		// -1이면 사용하지 않음.
	short int snFlags;		// 0 - 아무속성 없음. 1 - 충돌 2 - 4 - 8 - 16 - 32 - ......
} _TILE_MAP_DATA_TOOL, *LPTILE_MAP_DATA_TOOL;

typedef struct _TILE_MAP_DATA_TOOL_FOR_UNDO
{
	RECT rc;
	_TILE_MAP_DATA_TOOL* pDatas[MAX_TILE_MAP_LAYER];
}  _TILE_MAP_DATA_TOOL_FOR_UNDO;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BB99B4E3_7185_4C43_AA24_9D81EBF691E6__INCLUDED_)
