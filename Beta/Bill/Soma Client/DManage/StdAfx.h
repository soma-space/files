// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__08076628_57B7_11D2_8BEE_0060979C5900__INCLUDED_)
#define AFX_STDAFX_H__08076628_57B7_11D2_8BEE_0060979C5900__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <mmsystem.h>

// Include the headers from the DirectX SDK
//#include <d3drmwin.h>		// Reality Lab definitions
#define D3D_OVERLOADS
#include <d3d.h>
#include <ddraw.h>
#include <dinput.h>
#include <dsound.h>

#include <afxtempl.h>

#define _AFX_NO_OLE_SUPPORT
#define _AFX_NO_DB_SUPPORT
#define _AFX_NO_DAO_SUPPORT
#define _AFX_NO_AFXCMN_SUPPORT


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__08076628_57B7_11D2_8BEE_0060979C5900__INCLUDED_)
