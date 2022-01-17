// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A162F64B_662A_4CDA_82DD_E643B7C0DB70__INCLUDED_)
#define AFX_STDAFX_H__A162F64B_662A_4CDA_82DD_E643B7C0DB70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxtempl.h>

#include <atlbase.h>


#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "AgcmMap.h"
#include "AuList.h"
#include "Bmp.h"
#include "AuIniManager.h"
#include <afxdhtml.h>

enum
{
	REGIONTOOLMODE_REGION	,
	REGIONTOOLMODE_SPAWN	,
	REGIONTOOLMODE_SKY		,
	REGIONTOOLMODE_MAX
};

#define SPAWN_FILE_NAME	"RegionTool\\SpawnServerInfo.ini"
#define SKY_TEMPLATE_FORMAT "%03d , %s"

// 오프라인 작업 전용~
// 일반 작업시엔 이거 풀어줘야함.
#define OFFLINEWORK_ONLY

#include <ximage.h>

/*
#pragma comment ( lib , "png" )
#pragma comment ( lib , "jpeg" )
#pragma comment ( lib , "tiff" )
#pragma comment ( lib , "zlib" )
#pragma comment ( lib , "jasper" )
#pragma comment ( lib , "cximage" )
*/

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A162F64B_662A_4CDA_82DD_E643B7C0DB70__INCLUDED_)
