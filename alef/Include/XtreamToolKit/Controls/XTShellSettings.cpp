// XTShellSettings.cpp: implementation of the CXTShellSettings class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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

#include "stdafx.h"

#include "Common/XTPSystemHelpers.h"

#include "XTShellSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// CXTShellSettings

CXTShellSettings::CXTShellSettings()
{
	::ZeroMemory((SHELLFLAGSTATE*)this, sizeof(SHELLFLAGSTATE));

	CXTPModuleHandle lib(_T("shell32.dll"));

	typedef void (WINAPI* _SHGET)(LPSHELLFLAGSTATE lpsfs, DWORD dwMask);
	_SHGET pfn = NULL;

	if (lib.GetProcAddress((FARPROC*)&pfn, "SHGetSettings"))
	{
		pfn((SHELLFLAGSTATE*)this,
			SSF_DESKTOPHTML |
			SSF_DONTPRETTYPATH |
			SSF_DOUBLECLICKINWEBVIEW |
			SSF_HIDEICONS |
			SSF_MAPNETDRVBUTTON |
			SSF_NOCONFIRMRECYCLE |
			SSF_SHOWALLOBJECTS |
			SSF_SHOWATTRIBCOL |
			SSF_SHOWCOMPCOLOR |
			SSF_SHOWEXTENSIONS |
			SSF_SHOWINFOTIP |
			SSF_SHOWSYSFILES |
			SSF_WIN95CLASSIC);
	}
}

CXTShellSettings::~CXTShellSettings()
{

}

//////////////////////////////////////////////////////////////////////
//
