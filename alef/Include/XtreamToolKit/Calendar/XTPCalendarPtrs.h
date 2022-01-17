// XtpCalendarPtrs.h:
//
// This file is a part of the XTREME CALENDAR MFC class library.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(_XTPCALENDARPTRS_H__)
#define _XTPCALENDARPTRS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPSmartPtrInternalT.h"

//===========================================================================
//{{AFX_CODEJOCK_PRIVATE
class CXTPCalendarRecurrencePattern;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarRecurrencePattern)

class CXTPCalendarEvent;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarEvent)

class CXTPCalendarEvents;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarEvents)

//===========================================================================
class CXTPCalendarResource;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarResource)

class CXTPCalendarResources;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarResources)

class CXTPCalendarSchedules;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarSchedules)

class CXTPCalendarViewGroup;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarViewGroup)

class CXTPCalendarViewGroups;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarViewGroups)

//////////////////////////////////////////////////////////////////////////

class CXTPCalendarViewEvent;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarViewEvent)

class CXTPCalendarViewEvents;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarViewEvents)

class CXTPCalendarTimeZone;
XTP_DEFINE_SMART_PTR_INTERNAL(CXTPCalendarTimeZone)

//}}AFX_CODEJOCK_PRIVATE

/////////////////////////////////////////////////////////////////////////////

#endif // _XTPCALENDARPTRS_H__
