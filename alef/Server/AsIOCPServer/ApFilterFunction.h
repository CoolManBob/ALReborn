/*
	Notices: Copyright (c) NHN Studio 2003
	Created by: Bryan Jeong (2003/12/24)
 */

// ApFilterFunction.h: interface for the ApFilterFunction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APFILTERFUNCTION_H__02645486_8A1F_419A_804D_3022BB21D56C__INCLUDED_)
#define AFX_APFILTERFUNCTION_H__02645486_8A1F_419A_804D_3022BB21D56C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include <stdlib.h>
#include <imagehlp.h>

#pragma comment(lib, "imagehlp")

#define FILEEXT ".FilterCallStack.log"

extern BOOL ApFilterFunction(LPEXCEPTION_POINTERS pException, BOOL bDebugBreak = FALSE);

#endif // !defined(AFX_APFILTERFUNCTION_H__02645486_8A1F_419A_804D_3022BB21D56C__INCLUDED_)
