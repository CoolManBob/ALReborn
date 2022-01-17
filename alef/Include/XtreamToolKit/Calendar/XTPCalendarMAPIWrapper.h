// XTPCalendarMAPIWrapper.h: interface for the CXTPCalendarMAPIWrapper class.
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
#if !defined(__XTPCALENDARMAPIWRAPPER_H__)
#define __XTPCALENDARMAPIWRAPPER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Calendar/mapi/mapidefs.h"
#include "Calendar/mapi/mapicode.h"
#include "Calendar/mapi/mapiguid.h"
#include "Calendar/mapi/mapitags.h"
#include "Calendar/mapi/mapix.h"

//{{AFX_CODEJOCK_PRIVATE
#define TYPEDEF_WRAPPER_POINTER(val, type, proc)\
	const int xtpWrapper##proc = val;\
	const LPCSTR xtpWrapperProc##proc = #proc;\
	typedef type (__stdcall* PFN##proc)

#define TYPEDEF_WRAPPER_PTR_NUM(val, type, proc, num)\
	const int xtpWrapper##proc = val;\
	const LPCSTR xtpWrapperProc##proc = #proc"@"#num;\
	typedef type (__stdcall* PFN##proc)

TYPEDEF_WRAPPER_POINTER(0, HRESULT, MAPIInitialize)(LPVOID);
TYPEDEF_WRAPPER_POINTER(1, void, MAPIUninitialize)();
TYPEDEF_WRAPPER_POINTER(2, HRESULT, MAPILogonEx)(ULONG, LPTSTR, LPTSTR, FLAGS, LPMAPISESSION FAR *);
TYPEDEF_WRAPPER_POINTER(3, ULONG, MAPIFreeBuffer)(LPVOID);
TYPEDEF_WRAPPER_PTR_NUM(4, ULONG, UlRelease, 4)(LPVOID);
TYPEDEF_WRAPPER_PTR_NUM(5, void, FreeProws, 4)(LPSRowSet);
TYPEDEF_WRAPPER_PTR_NUM(6, HRESULT, HrQueryAllRows, 24)(LPMAPITABLE, LPSPropTagArray, LPSRestriction, LPSSortOrderSet, LONG, LPSRowSet FAR *);
TYPEDEF_WRAPPER_PTR_NUM(7, HRESULT, HrGetOneProp, 12)(LPMAPIPROP, ULONG, LPSPropValue FAR *);
TYPEDEF_WRAPPER_PTR_NUM(8, HRESULT, HrSetOneProp, 8)(LPMAPIPROP, LPSPropValue);
TYPEDEF_WRAPPER_POINTER(9, SCODE, MAPIAllocateBuffer)(ULONG, LPVOID FAR *);
TYPEDEF_WRAPPER_PTR_NUM(10, HRESULT, HrThisThreadAdviseSink, 8)(LPMAPIADVISESINK, LPMAPIADVISESINK FAR *);
TYPEDEF_WRAPPER_PTR_NUM(11, HRESULT, HrAllocAdviseSink, 12)(LPNOTIFCALLBACK, LPVOID, LPMAPIADVISESINK FAR *);
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     The CXTPCalendarMAPIWrapper class wraps the mapi32.dll API.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarMAPIWrapper
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCalendarMAPIWrapper object.
	//-----------------------------------------------------------------------
	CXTPCalendarMAPIWrapper();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCalendarMAPIWrapper object, handles cleanup and
	//     deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarMAPIWrapper();

	/////////////////////////////////////////////////////////////////////////
	// Wrapped methods

	//-----------------------------------------------------------------------
	// Summary:
	//     Increments the MAPI subsystem reference count and initializes
	//     global data for the MAPI DLL.
	// Parameters:
	//     lpMapiInit - [in] Pointer to a MAPIINIT_0 structure.
	//                  The lpMapiInit parameter can be set to NULL.
	// Returns:
	//     Returns S_OK if MAPI subsystem was initialized successfully,
	//     or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT MAPIInitialize(LPVOID lpMapiInit);

	//-----------------------------------------------------------------------
	// Summary:
	//     Decrements the reference count, cleans up, and deletes
	//     per-instance global data for the MAPI DLL.
	//-----------------------------------------------------------------------
	void MAPIUninitialize();

	//-----------------------------------------------------------------------
	// Summary:
	//     Logs a client application on to a session with the messaging system..
	// Parameters:
	//     ulUIParam       - [in] Handle to the window to which the logon dialog box is modal.
	//                       If no dialog box is displayed during the call,
	//                       the ulUIParam parameter is ignored. This parameter can be zero..
	//     lpszProfileName - [in] Pointer to a string containing the name of the profile to use
	//                       when logging on. This string is limited to 64 characters.
	//     lpszPassword    - [in] Pointer to a string containing the password of the profile.
	//                       The lpszPassword parameter can be NULL whether or not the
	//                       lpszProfileName parameter is NULL. This string is limited to 64 characters. .
	//     flFlags         - [in] Bitmask of flags used to control how logon is performed.
	//                       See API MAPILogonEx description for more info.
	//     lppSession      - [out] Pointer to a pointer to the MAPI session interface.
	// Returns:
	//     S_OK                - The logon succeeded.
	//     MAPI_E_LOGON_FAILED - The logon did not succeed, either because one or more of the
	//                           parameters to MAPILogonEx were invalid or because there were
	//                           too many sessions open already.
	//     MAPI_E_TIMEOUT      - MAPI serializes all logons through a mutex. This is returned if
	//                           the MAPI_TIMEOUT_SHORT flag was set and another thread held the mutex.
	//     MAPI_E_USER_CANCEL  - The user canceled the operation, typically by clicking
	//                           the Cancel button in a dialog box.
	//-----------------------------------------------------------------------
	HRESULT MAPILogonEx(ULONG ulUIParam, LPTSTR lpszProfileName, LPTSTR lpszPassword, FLAGS flFlags, LPMAPISESSION FAR * lppSession);

	//-----------------------------------------------------------------------
	// Summary:
	//     Frees a memory buffer allocated with a call to the MAPIAllocateBuffer
	//     function or the MAPIAllocateMore function.
	// Parameters:
	//     lpBuffer - [in] Pointer to a previously allocated memory buffer.
	//                If NULL is passed in the lpBuffer parameter, MAPIFreeBuffer does nothing. .
	// Returns:
	//     Returns S_OK if the call succeeded and freed the memory requested.
	//     MAPIFreeBuffer can also return S_OK on already freed locations or
	//     if memory block is not allocated with MAPIAllocateBuffer and MAPIAllocateMore.
	//-----------------------------------------------------------------------
	ULONG MAPIFreeBuffer(LPVOID lpBuffer);

	//-----------------------------------------------------------------------
	// Summary:
	//     Provides an alternative way to invoke the OLE method IUnknown::Release. .
	// Parameters:
	//     punk - [in] Pointer to an interface derived from the IUnknown interface,
	//            in other words any MAPI interface.
	// Returns:
	//     Returns S_OK if the call succeeded and has returned the expected value or values.
	//     MAPI_E_CALL_FAILED - An error of unexpected or unknown origin
	//                          prevented the operation from completing.
	//-----------------------------------------------------------------------
	ULONG UlRelease(LPVOID punk);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys an SRowSet structure and frees associated memory, including
	//     memory allocated for all member arrays and structures.
	// Parameters:
	//     prows - [in] Pointer to the SRowSet structure to be destroyed.
	//-----------------------------------------------------------------------
	void FreeProws(LPSRowSet prows);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves all rows of a table.
	// Parameters:
	//     ptable - [in] Pointer to the MAPI table from which rows are retrieved.
	//     ptaga  - [in] Pointer to an SPropTagArray structure containing an array of property tags
	//              indicating table columns. These tags are used to select the specific columns
	//              to be retrieved. If the ptaga parameter is NULL, HrQueryAllRows retrieves the
	//              entire column set of the current table view passed in the ptable parameter.
	//     pres   - [in] Pointer to an SRestriction structure containing retrieval restrictions.
	//              If the pres parameter is NULL, HrQueryAllRows makes no restrictions.
	//     psos   - [in] Pointer to an SSortOrderSet structure identifying the sort order of the
	//              columns to be retrieved. If the psos parameter is NULL,
	//              the default sort order for the table is used.
	//     crowsMax-[in] Maximum number of rows to be retrieved. If the value of the crowsMax
	//              parameter is zero, no limit on the number of rows retrieved is set.
	//     pprows - [out] Pointer to a pointer to the returned SRowSet structure containing
	//              an array of pointers to the retrieved table rows.
	// Returns:
	//     Returns S_OK if the call succeeded and retrieved the expected rows of a table.
	//     MAPI_E_TABLE_TOO_BIG - The number of rows in the table is larger than
	//                            the number passed for the crowsMax parameter.
	//-----------------------------------------------------------------------
	HRESULT HrQueryAllRows(LPMAPITABLE ptable, LPSPropTagArray ptaga, LPSRestriction pres, LPSSortOrderSet psos, LONG crowsMax, LPSRowSet FAR * pprows);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the value of a single property from a property interface,
	//     that is, an interface derived from IMAPIProp.
	// Parameters:
	//     pmp       - [in] Pointer to the IMAPIProp interface from which
	//                 the property value is to be retrieved.
	//     ulPropTag - [in] Property tag of the property to be retrieved.
	//     ppprop    - [out] Pointer to a pointer to the returned SPropValue
	//                 structure defining the retrieved property value.
	// Returns:
	//     MAPI_E_NOT_FOUND - The requested property is not available from
	//                        the specified interface.
	//-----------------------------------------------------------------------
	HRESULT HrGetOneProp(LPMAPIPROP pmp, ULONG ulPropTag, LPSPropValue FAR * ppprop);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets or changes the value of a single property on a property
	//     interface, that is, an interface derived from IMAPIProp.
	// Parameters:
	//     pmp   - [in] Pointer to an IMAPIProp interface on which the
	//             property value is to be set or changed.
	//     pprop - [in] Pointer to the SPropValue structure defining the
	//             property to be set or changed.
	// Returns:
	//     MAPI_E_NOT_FOUND - The requested property is not available from
	//                        the specified interface.
	//-----------------------------------------------------------------------
	HRESULT HrSetOneProp(LPMAPIPROP pmp, LPSPropValue pprop);

	//-----------------------------------------------------------------------
	// Summary:
	//     Allocates a memory buffer.
	// Parameters:
	//     cbSize    - [in] Size, in bytes, of the buffer to be allocated.
	//     lppBuffer - [out] Pointer to the returned allocated buffer.
	// Returns:
	//     Returns S_OK if the call succeeded and has returned
	//     the expected value or values.
	//-----------------------------------------------------------------------
	SCODE MAPIAllocateBuffer(ULONG cbSize, LPVOID FAR * lppBuffer);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates an advise sink that wraps an existing advise sink for
	//     thread safety.
	// Parameters:
	//     lpAdviseSink  - [in] Pointer to the advise sink to be wrapped.
	//     lppAdviseSink - [out] Pointer to a pointer to a new advise sink
	//                     that wraps the advise sink pointed to by the
	//                     lpAdviseSink parameter.
	//-----------------------------------------------------------------------
	HRESULT HrThisThreadAdviseSink(LPMAPIADVISESINK lpAdviseSink, LPMAPIADVISESINK FAR * lppAdviseSink);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates an advise sink object, given a context specified by the
	//     calling implementation and a callback function to be triggered by
	//     an event notification..
	// Parameters:
	//     lpfnCallback  - [in] Pointer to a callback function based on the
	//                     NOTIFCALLBACK prototype that MAPI is to call when
	//                     a notification event occurs for the newly created
	//                     advise sink. .
	//     lpvContext    - [in] Pointer to caller data passed to the callback
	//                     function when MAPI calls it. The caller data can
	//                     represent an address of significance to the client
	//                     or provider. Typically, for C++ code, the lpvContext
	//                     parameter represents a pointer to the address of
	//                     an object.
	//     lppAdviseSink - [out] Pointer to a pointer to an advise sink object.
	//-----------------------------------------------------------------------
	HRESULT HrAllocAdviseSink(LPNOTIFCALLBACK lpfnCallback, LPVOID lpvContext, LPMAPIADVISESINK FAR * lppAdviseSink);

private:
	LPVOID m_ptrWrappers[12];   // Wrapper pointer
	HMODULE m_hMapiDll;         // Handle to the mapi32 dll.

};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPCALENDARMAPIWRAPPER_H__)
