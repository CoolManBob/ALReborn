// XTPCalendarMAPIWrapper.cpp: implementation of the CXTPCalendarMAPIWrapper class.
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

#include "stdafx.h"

#include "XTPCalendarMAPIWrapper.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#pragma warning(disable: 4571) // warning C4571: catch(...) blocks compiled with /EHs do not catch or re-throw Structured Exceptions


#define TRY_POINTER_CALL(proc)\
	try\
	{\
		if (m_hMapiDll != NULL && m_ptrWrappers[xtpWrapper##proc] == NULL)\
		{\
			m_ptrWrappers[xtpWrapper##proc] = ::GetProcAddress(\
				m_hMapiDll, xtpWrapperProc##proc);\
		}\
		PFN##proc ptr = (PFN##proc)m_ptrWrappers[xtpWrapper##proc];\
		if (ptr)\
		{
#define POINTER_CALL (*ptr)
#define CATCH_POINTER_CALL()\
		}\
	}\
	catch (...)\
	{\
	}


//===========================================================================
// CXTPCalendarMAPIWrapper class
//===========================================================================

CXTPCalendarMAPIWrapper::CXTPCalendarMAPIWrapper()
{
	ZeroMemory(&m_ptrWrappers, sizeof(m_ptrWrappers));
	m_hMapiDll = ::LoadLibrary(_T("mapi32.dll"));
}

CXTPCalendarMAPIWrapper::~CXTPCalendarMAPIWrapper()
{
	if (m_hMapiDll)
	{
		FreeLibrary(m_hMapiDll);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Wrapped methods
//

HRESULT CXTPCalendarMAPIWrapper::MAPIInitialize(LPVOID lpMapiInit)
{
	TRY_POINTER_CALL(MAPIInitialize)
		return POINTER_CALL(lpMapiInit);
	CATCH_POINTER_CALL()

	return E_FAIL;
}

void CXTPCalendarMAPIWrapper::MAPIUninitialize()
{
	TRY_POINTER_CALL(MAPIUninitialize)
		POINTER_CALL();
	CATCH_POINTER_CALL()
}

HRESULT CXTPCalendarMAPIWrapper::MAPILogonEx(ULONG ulUIParam, LPTSTR lpszProfileName, LPTSTR lpszPassword, FLAGS flFlags, LPMAPISESSION FAR * lppSession)
{
	TRY_POINTER_CALL(MAPILogonEx)
		return POINTER_CALL(ulUIParam, lpszProfileName, lpszPassword, flFlags, lppSession);
	CATCH_POINTER_CALL()

	return E_FAIL;
}

ULONG CXTPCalendarMAPIWrapper::MAPIFreeBuffer(LPVOID lpBuffer)
{
	TRY_POINTER_CALL(MAPIFreeBuffer)
		return POINTER_CALL(lpBuffer);
	CATCH_POINTER_CALL()

	return 1;
}

ULONG CXTPCalendarMAPIWrapper::UlRelease(LPVOID punk)
{
	TRY_POINTER_CALL(UlRelease)
		return POINTER_CALL(punk);
	CATCH_POINTER_CALL()

	return 1;
}

void CXTPCalendarMAPIWrapper::FreeProws(LPSRowSet prows)
{
	TRY_POINTER_CALL(FreeProws)
		POINTER_CALL(prows);
	CATCH_POINTER_CALL()
}

HRESULT CXTPCalendarMAPIWrapper::HrQueryAllRows(LPMAPITABLE ptable, LPSPropTagArray ptaga, LPSRestriction pres, LPSSortOrderSet psos, LONG crowsMax, LPSRowSet FAR * pprows)
{
	TRY_POINTER_CALL(HrQueryAllRows)
		return POINTER_CALL(ptable, ptaga, pres, psos, crowsMax, pprows);
	CATCH_POINTER_CALL()

	return E_FAIL;
}

HRESULT CXTPCalendarMAPIWrapper::HrGetOneProp(LPMAPIPROP pmp, ULONG ulPropTag, LPSPropValue FAR * ppprop)
{
	TRY_POINTER_CALL(HrGetOneProp)
		return POINTER_CALL(pmp, ulPropTag, ppprop);
	CATCH_POINTER_CALL()

	return E_FAIL;
}

HRESULT CXTPCalendarMAPIWrapper::HrSetOneProp(LPMAPIPROP pmp, LPSPropValue pprop)
{
	TRY_POINTER_CALL(HrSetOneProp)
		return POINTER_CALL(pmp, pprop);
	CATCH_POINTER_CALL()

	return E_FAIL;
}

SCODE CXTPCalendarMAPIWrapper::MAPIAllocateBuffer(ULONG cbSize, LPVOID FAR * lppBuffer)
{
	TRY_POINTER_CALL(MAPIAllocateBuffer)
		return POINTER_CALL(cbSize,lppBuffer);
	CATCH_POINTER_CALL()

	return E_FAIL;
}

HRESULT CXTPCalendarMAPIWrapper::HrThisThreadAdviseSink(LPMAPIADVISESINK lpAdviseSink, LPMAPIADVISESINK FAR * lppAdviseSink)
{
	TRY_POINTER_CALL(HrThisThreadAdviseSink)
		return POINTER_CALL(lpAdviseSink, lppAdviseSink);
	CATCH_POINTER_CALL()

	return E_FAIL;
}

HRESULT CXTPCalendarMAPIWrapper::HrAllocAdviseSink(LPNOTIFCALLBACK lpfnCallback, LPVOID lpvContext, LPMAPIADVISESINK FAR * lppAdviseSink)
{
	TRY_POINTER_CALL(HrAllocAdviseSink)
		return POINTER_CALL(lpfnCallback, lpvContext, lppAdviseSink);
	CATCH_POINTER_CALL()

	return E_FAIL;
}
