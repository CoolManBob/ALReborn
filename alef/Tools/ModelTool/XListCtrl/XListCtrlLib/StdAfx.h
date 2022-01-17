// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef STDAFX_H
#define STDAFX_H

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


///////////////////////////////////////////////////////////////////////////////
// XLISTCTRLLIB_EXPORTS - defined to export the classes in this dll
#ifndef XLISTCTRLLIB_EXPORTS
	#define XLISTCTRLLIB_EXPORTS
#endif


///////////////////////////////////////////////////////////////////////////////
// Automatic library selection macro - will not be defined for the dll compile.
// For app compiles, this will ensure that the correct export library is 
// automatically linked to the application.
#define XLISTCTRLLIB_NOAUTOLIB


///////////////////////////////////////////////////////////////////////////////
// uncomment following line to remove support for comboboxes
//#define DO_NOT_INCLUDE_XCOMBOLIST


///////////////////////////////////////////////////////////////////////////////
// uncomment following line to remove support for tooltips
//#define NO_XLISTCTRL_TOOL_TIPS


#include "..\XListCtrl\XTrace.h"


///////////////////////////////////////////////////////////////////////////////
// uncomment the following line to enable TRACE statements within the 
// XListCtrl library
#define ENABLE_XLISTCTRL_TRACE

#ifdef ENABLE_XLISTCTRL_TRACE
	#define XLISTCTRL_TRACE TRACE
#else
	#define XLISTCTRL_TRACE __noop
#endif


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //STDAFX_H
