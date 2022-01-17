// XTToolkitPro.h
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4786) // Identifier was truncated to '255' characters in the debug information

#if (_MSC_VER > 1100)
#pragma warning(push)
#pragma pack(push, 8)
#endif

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>
#endif // _AFX_NO_OLE_SUPPORT

#ifdef _MFC_OVERRIDES_NEW
#define _INC_MALLOC
#endif

#include <afxpriv.h>    // MFC support for docking windows
#include <afxtempl.h>   // MFC template classes
#include <afxcview.h>   // MFC ListView / TreeView support
#include <shlobj.h>     // MFC support for shell extensions


#ifndef AFX_STATIC
#define AFX_STATIC static
#endif

//------------------------------------------------------------------------
// NOTE: If using the Xtreme Toolkit Pro as a static library linked to an
// application that is dynamically linked with MFC, you will need to do the
// following:
//
// Open the XTPToolkit_Lib project workspace and select one of the
// Win32 Dynamic build settings and build the library. Add the following
// lines of code to your stdafx.h file:
//
// #define _XTP_STATICLINK
// #include <XTToolkitPro.h>
//
// Add the following line of code to your *.rc2 file after the comment:
// "Add manually edited resources here...":
//
// #include "XTToolkitPro.rc"
//------------------------------------------------------------------------

#undef _XTP_EXT_CLASS

#if !defined(_AFXDLL) || defined(_XTP_STATICLINK)
#define _XTP_EXT_CLASS
#else
#define _XTP_EXT_CLASS  __declspec(dllimport)
#endif // !defined(_AFXDLL) || defined(_XTP_STATICLINK)

#if defined(_XTP_INCLUDE_DEPRECATED)
  #include "Common/XTPDeprecated.h"
#endif

#if !defined(_XTP_EXCLUDE_COMMON)
#include "Common/Includes.h"          // _XTP_INCLUDE_COMMON
#endif

#if !defined(_XTP_EXCLUDE_TABMANAGER)
#include "TabManager/Includes.h"      // _XTP_INCLUDE_TABMANAGER
#endif

#if !defined(_XTP_EXCLUDE_GRAPHICLIBRARY)
#include "GraphicLibrary/Includes.h"  // _XTP_INCLUDE_GRAPHICLIBRARY
#endif

#if !defined(_XTP_EXCLUDE_CONTROLS)
#include "Controls/Includes.h"        // _XTP_INCLUDE_CONTROLS
#endif

#if !defined(_XTP_EXCLUDE_COMMANDBARS)
#include "CommandBars/Includes.h"     // _XTP_INCLUDE_COMMANDBARS
#endif

#if !defined(_XTP_EXCLUDE_DOCKINGPANE)
#include "DockingPane/Includes.h"     // _XTP_INCLUDE_DOCKINGPANE
#endif

#if !defined(_XTP_EXCLUDE_PROPERTYGRID)
#include "PropertyGrid/Includes.h"    // _XTP_INCLUDE_PROPERTYGRID
#endif

#if !defined(_XTP_EXCLUDE_REPORTCONTROL)
#include "ReportControl/Includes.h"   // _XTP_INCLUDE_REPORTCONTROL
#endif

#if !defined(_XTP_EXCLUDE_CALENDAR)
#include "Calendar/Includes.h"        // _XTP_INCLUDE_CALENDAR
#endif

#if !defined(_XTP_EXCLUDE_TASKPANEL)
#include "TaskPanel/Includes.h"       // _XTP_INCLUDE_TASKPANEL
#endif

#if !defined(_XTP_EXCLUDE_SHORTCUTBAR)
#include "ShortcutBar/Includes.h"     // _XTP_INCLUDE_SHORTCUTBAR
#endif

#if !defined(_XTP_EXCLUDE_SKINFRAMEWORK)
#include "SkinFramework/Includes.h"   // _XTP_INCLUDE_SKINFRAMEWORK
#endif

#if !defined(_XTP_EXCLUDE_RIBBON)
#include "Ribbon/Includes.h"          // _XTP_INCLUDE_RIBBON
#endif

#if !defined(_XTP_EXCLUDE_SYNTAXEDIT)
#include "SyntaxEdit/Includes.h"      // _XTP_INCLUDE_SYNTAXEDIT
#endif

#define _XTPLIB_FILE_VERSION_STRING_(x) #x
#define _XTPLIB_FILE_VERSION_STRING(x) _XTPLIB_FILE_VERSION_STRING_(x)

#if !defined(_XTP_DEMOMODE) && !defined(_XTP_RETAIL)
#define _XTPLIB_VISUALSTUIDO_VERSION ""
#else
#if _MSC_VER < 1200
#define _XTPLIB_VISUALSTUIDO_VERSION "vc50"
#endif
#if _MSC_VER == 1200
#define _XTPLIB_VISUALSTUIDO_VERSION "vc60"
#endif
#if _MSC_VER == 1300
#define _XTPLIB_VISUALSTUIDO_VERSION "vc70"
#endif
#if _MSC_VER == 1310
#define _XTPLIB_VISUALSTUIDO_VERSION "vc71"
#endif
#if _MSC_VER == 1400
#define _XTPLIB_VISUALSTUIDO_VERSION "vc80"
#endif

#ifndef _XTPLIB_VISUALSTUIDO_VERSION
#error "Unknown Visual Studio version"
#endif
#endif

#define _XTPLIB_FILE_VERSION _XTPLIB_FILE_VERSION_STRING(_XTPLIB_VERSION_PREFIX)


//------------------------------------------------------
// Xtreme link to the appropriate dll or static library:
//------------------------------------------------------
#if defined(_AFXDLL) && defined(_XTP_STATICLINK)
	#if defined(_DEBUG)
		#if defined(_UNICODE)
			#define _XTPLIB_PROJECT_CONFIGURATION "DSUD"
		#else
			#define _XTPLIB_PROJECT_CONFIGURATION "DSD"
		#endif // _UNICODE
	#else
		#if defined(_UNICODE)
			#define _XTPLIB_PROJECT_CONFIGURATION "DSU"
		#else
			#define _XTPLIB_PROJECT_CONFIGURATION "DS"
		#endif // _UNICODE
	#endif // _DEBUG
	#define _XTPLIB_LINK_TYPE   "lib"
	#define _XTPLIB_LINK_IS_DLL 0
#elif !defined(_AFXDLL)
	#if defined(_DEBUG)
		#if defined(_UNICODE)
			#define _XTPLIB_PROJECT_CONFIGURATION "SUD"
		#else
			#define _XTPLIB_PROJECT_CONFIGURATION "SD"
		#endif // _UNICODE
	#else
		#if defined(_UNICODE)
			#define _XTPLIB_PROJECT_CONFIGURATION "SU"
		#else
			#define _XTPLIB_PROJECT_CONFIGURATION "S"
		#endif // _UNICODE
	#endif // _DEBUG
	#define _XTPLIB_LINK_TYPE   "lib"
	#define _XTPLIB_LINK_IS_DLL 0
#else
	#if defined(_DEBUG)
		#if defined(_UNICODE)
			#define _XTPLIB_PROJECT_CONFIGURATION "UD"
		#else
			#define _XTPLIB_PROJECT_CONFIGURATION "D"
		#endif // _UNICODE
	#else
		#if defined(_UNICODE)
			#define _XTPLIB_PROJECT_CONFIGURATION "U"
		#else
			#define _XTPLIB_PROJECT_CONFIGURATION ""
		#endif // _UNICODE
	#endif // _DEBUG
	#define _XTPLIB_LINK_TYPE   "dll"
	#define _XTPLIB_LINK_IS_DLL 1
#endif // !defined(_AFXDLL) || defined(_XTP_STATICLINK)

#if (_XTPLIB_LINK_IS_DLL == 0) && defined(_XTP_DEMOMODE)
	#pragma message(" ")
	#pragma message("----------------------------------------------------------------------------------------------")
	#pragma message(" The evaluation version of the toolkit only supports DLL configurations.")
	#pragma message(" To purchase the full version (with static link support) please visit http://www.codejock.com")
	#pragma message("----------------------------------------------------------------------------------------------")
	#pragma message(" ")
	#error This build configuration is not supported by the evaluation library
#endif

#if !(defined(_DEBUG)) && defined(_XTP_DEMOMODE)
	#pragma message(" ")
	#pragma message("----------------------------------------------------------------------------------------------")
	#pragma message(" The evaluation version of the toolkit only supports DEBUG configurations.")
	#pragma message(" To purchase the full version (with static link support) please visit http://www.codejock.com")
	#pragma message("----------------------------------------------------------------------------------------------")
	#pragma message(" ")
	#error This build configuration is not supported by the evaluation library
#endif


#if defined(_XTP_DEMOMODE)
	#define _XTPLIB_PROJECT_EVAL "Eval"
#else
	#define _XTPLIB_PROJECT_EVAL ""
#endif

#if defined(_WIN64)
	#define _XTPLIB_PLATFORM "x64"
#else
	#define _XTPLIB_PLATFORM ""
#endif

#ifndef _XTPLIB_NOAUTOLINK
	#pragma comment(lib, "ToolkitPro" _XTPLIB_PROJECT_EVAL _XTPLIB_FILE_VERSION _XTPLIB_VISUALSTUIDO_VERSION _XTPLIB_PLATFORM _XTPLIB_PROJECT_CONFIGURATION ".lib")
	#pragma message("Automatically linking with ToolkitPro" _XTPLIB_PROJECT_EVAL _XTPLIB_FILE_VERSION _XTPLIB_VISUALSTUIDO_VERSION _XTPLIB_PLATFORM _XTPLIB_PROJECT_CONFIGURATION "." _XTPLIB_LINK_TYPE)
#endif //_XTPLIB_NOAUTOLINK

#if !defined(_AFXDLL) && !defined(_XTPLIB_NOAUTOLIBIGNORE)
#ifdef _DEBUG
	#pragma comment(linker, "/NODEFAULTLIB:libcd.lib")
#else
	#pragma comment(linker, "/NODEFAULTLIB:libc.lib")
#endif
#endif

#undef _XTPLIB_FILE_VERSION
#undef _XTPLIB_PROJECT_CONFIGURATION
#undef _XTPLIB_PROJECT_EVAL
#undef _XTPLIB_LINK_TYPE
#undef _XTPLIB_LINK_IS_DLL

#if (_MSC_VER > 1100)
#pragma pack(pop)
#pragma warning(pop)
#endif

/////////////////////////////////////////////////////////////////////////////
