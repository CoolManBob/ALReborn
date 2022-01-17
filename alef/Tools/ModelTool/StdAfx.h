#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define  WINVER  0x0400

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <mmsystem.h>

#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <list>
#include <string>

using namespace std;

#include <d3d9.h>
#include <direct.h>

#include "TreeCtrlEx.h"

#include "RwUtil.h"
#include "Singleton.hpp"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(P)			{ if(P)   {	(P)->Release();		(P) = NULL;	} }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(P) 			{ if(P)   { delete (P);			(P) = NULL;	} }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(P) 	{ if(P)   { delete [] (P);		(P) = NULL;	} }
#endif

#include "AgcEngineChild.h"
#include "Util.h"

static void DebugString( const char* szFormat, ... )
{
	static char szBuffer[2048];

	va_list argument;
	va_start( argument, szFormat );
	_vsntprintf( szBuffer, 2048, szFormat, argument );
	va_end( argument );

	OutputDebugString( szBuffer );
}

#define _CRTDBG_MAP_ALLOC

//#define BitSet( (A), (B) )	A |= B
//#define BitDel( (A), (B) )	A &= ~B
