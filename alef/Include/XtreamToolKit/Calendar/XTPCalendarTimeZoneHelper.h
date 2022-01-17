// XTPCalendarTimeZoneHelper.h: interfaces for Time Zone Helper classes.
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
#if !defined(_XTPCALENDARTIMEZONEHELPER_H__)
#define _XTPCALENDARTIMEZONEHELPER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarPtrCollectionT.h"
#include "XTPCalendarPtrs.h"

//===========================================================================
// Summary:
//      This class extend TIME_ZONE_INFORMATION structure.
// Remarks:
//      It has additional data members to represent full information about
//      time zone.
//      Also it implements some helper functions and OLE interface methods.
// See Also:
//      TIME_ZONE_INFORMATION, CXTPCalendarTimeZones.
//       MSDN Articles:
//          INFO: Retrieving Time-Zone Information
//          KB115231, Q115231
//
//          HOWTO: Change Time Zone Information Using Visual Basic
//          KB221542, Q221542
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarTimeZone : public CXTPCmdTarget,
											public TIME_ZONE_INFORMATION
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarTimeZones;
	DECLARE_DYNAMIC(CXTPCalendarTimeZone)
	//}}AFX_CODEJOCK_PRIVATE

public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	// Parameters:
	//      pTZInfo - Pointer to a TIME_ZONE_INFORMATION data to initialize
	//                created object data.
	//      pTZInfoEx - Pointer to a source data object.
	// See Also: ~CXTPCalendarTimeZone(), TIME_ZONE_INFORMATION
	//-----------------------------------------------------------------------
	CXTPCalendarTimeZone(const TIME_ZONE_INFORMATION* pTZInfo = NULL);
	CXTPCalendarTimeZone(const CXTPCalendarTimeZone* pTZInfoEx); // <combine CXTPCalendarTimeZone::CXTPCalendarTimeZone@const TIME_ZONE_INFORMATION*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also: CXTPCalendarTimeZone()
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarTimeZone();

	//-----------------------------------------------------------------------
	// Summary:
	//      Get a time zone display string value.
	// Returns:
	//      A string value like: "(GMT+02:00) Athens, Beirut, Istanbul, Minsk"
	//-----------------------------------------------------------------------
	CString GetDisplayString() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Get a time zone order index value.
	// Remarks:
	//      This index value is used to sort time zones in the right order.
	// Returns:
	//      A time zone order index value.
	//-----------------------------------------------------------------------
	DWORD   GetIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Compare 2 time zones data.
	// Parameters:
	//      pTZI2 - Points to a TIME_ZONE_INFORMATION object.
	// Returns:
	//      TRUE - if time zones data related to the same time zone,
	//      otherwise - FALSE.
	//-----------------------------------------------------------------------
	BOOL IsEqual(const TIME_ZONE_INFORMATION* pTZI2) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Copy operator.
	// Parameters:
	//      rTZInfo - Pointer to a source data object.
	// Returns:
	//      A Constant reference to updated object
	// See Also:
	//      CXTPCalendarTimeZone()
	//-----------------------------------------------------------------------
	const CXTPCalendarTimeZone& operator=(const CXTPCalendarTimeZone& rTZInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//      Get full information about the specified time zone.
	// Parameters:
	//      pTZIdata - Pointer to a time zone data object.
	// Remarks:
	//      Retrieve additional information from the registry.
	// Returns:
	//      A smart pointer to CXTPCalendarTimeZone object.
	// See Also:
	//      CXTPCalendarTimeZones
	//-----------------------------------------------------------------------
	static CXTPCalendarTimeZonePtr AFX_CDECL GetTimeZoneInfo(const TIME_ZONE_INFORMATION* pTZIdata);

protected:
	CString m_strDisplayString; // A time zone display string stored value.
	DWORD   m_dwIndex;          // A time zone order index stored value.

};

//===========================================================================
// Summary:
//      This class represents a simple array collection of CXTPCalendarTimeZone
//      objects.
// Remarks:
//      Array indexes always start at position 0.
//      An InitFromRegistry() method is used to read all time zones from the
//      registry and add them to the collection.
//      Also it implements some helper functions and OLE interface methods.
// See Also:
//      TIME_ZONE_INFORMATION, CXTPCalendarTimeZone.
//       MSDN Articles:
//          INFO: Retrieving Time-Zone Information
//          KB115231, Q115231
//
//          HOWTO: Change Time Zone Information Using Visual Basic
//          KB221542, Q221542
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarTimeZones : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarTimeZones)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	// See Also: ~CXTPCalendarTimeZones()
	//-----------------------------------------------------------------------
	CXTPCalendarTimeZones();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also: CXTPCalendarTimeZones()
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarTimeZones();

	//-----------------------------------------------------------------------
	// Summary:
	//      Read all time zones from the registry and add them to the
	//      collection.
	// Returns:
	//      TRUE - if all time zones where successfully read,
	//      otherwise - FALSE.
	//-----------------------------------------------------------------------
	BOOL InitFromRegistry();

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to obtain the number of elements in
	//      this collection.
	// Returns:
	//     An int that contains the number of items in the collection.
	// See Also: GetAt()
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get an element at the specified numeric index.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetCount.
	// Returns:
	//     The pointer to the CXTPCalendarTimeZone element currently at this
	//     index.
	// See Also: GetCount()
	//-----------------------------------------------------------------------
	CXTPCalendarTimeZone* GetAt(long nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      This method is used to find a time zone information using the
	//      specified time zone data.
	// Parameters:
	//      pTZI2 - Pointer to a time zone data object.
	// Returns:
	//     The pointer to the first object in the collection
	//     that matches the requested time zone.
	//     NULL if the such object is not found.
	//-----------------------------------------------------------------------
	CXTPCalendarTimeZone* Find(const TIME_ZONE_INFORMATION* pTZI2) const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      This structure represents format of binary data stored in the
	//      "TZI" registry key.
	// See also:
	//      Microsoft KB115231, TIME_ZONE_INFORMATION,
	//      XTP_CALENDAR_TZIRegValName_DATA,
	//      XTP_CALENDAR_TIMEZONESKEY_NT, XTP_CALENDAR_TIMEZONESKEY_9X
	//-----------------------------------------------------------------------
	struct REGISTRY_TIMEZONE_INFORMATION
	{
		LONG       Bias;         // Current bias for local time translation on this computer, in minutes.
		LONG       StandardBias; // Bias value to be used during local time translations that occur during standard time.
		LONG       DaylightBias; // Bias value to be used during local time translations that occur during daylight saving time.
		SYSTEMTIME StandardDate; // A SYSTEMTIME structure that contains a date and local time when the transition from daylight saving time to standard time occurs on this operating system.
		SYSTEMTIME DaylightDate; // A SYSTEMTIME structure that contains a date and local time when the transition from standard time to daylight saving time occurs on this operating system.
	};

	//-----------------------------------------------------------------------
	// Summary:
	//      This method is used to read a string value from the registry.
	// Parameters:
	//      hKey          - [in] Parent key handle.
	//      pcszValueName - [in] Value key name.
	//      rstrValue     - [out] CString object reference to store value.
	// Returns:
	//      TRUE - if successful, otherwise - FALSE.
	// See Also: GetRegBSTR()
	//-----------------------------------------------------------------------
	static BOOL GetRegStr(HKEY hKey, LPCTSTR pcszValueName, CString& rstrValue);

	//-----------------------------------------------------------------------
	// Summary:
	//      This method is used to read a UNICODE string value from the registry.
	// Parameters:
	//      hKey           - [in] Parent key handle.
	//      pcszValueNameW - [in] Value key name in the UNICODE format.
	//      rbstrValue     - [out] BSTR object reference to store value.
	// Remarks:
	//      It is used to get UNICODE strings without conversion to MBCS.
	//      If rbstrValue parameter is not NULL the SysFreeString()
	//      API function is called to erase value.
	// Returns:
	//      TRUE - if successful, otherwise - FALSE.
	// See Also: GetRegStr()
	//-----------------------------------------------------------------------
	static BOOL GetRegBSTR(HKEY hKey, LPCWSTR pcszValueNameW, BSTR& rbstrValue);

	//-----------------------------------------------------------------------
	// Summary:
	//      This method is used to read a DWORD value from the registry.
	// Parameters:
	//      hKey          - [in] Parent key handle.
	//      pcszValueName - [in] Value key name.
	//      rdwValue      - [out] DWORD variable reference to store value.
	// Returns:
	//      TRUE - if successful, otherwise - FALSE.
	//-----------------------------------------------------------------------
	static BOOL GetRegDWORD(HKEY hKey, LPCTSTR pcszValueName, DWORD& rdwValue);

	//-----------------------------------------------------------------------
	// Summary:
	//      This method is used to read a time zone information stored as
	//      binary value in the registry.
	// Parameters:
	//      hKey          - [in] Parent key handle.
	//      pcszValueName - [in] Value key name.
	//      rRegTZI       - [out] REGISTRY_TIMEZONE_INFORMATION object
	//                      reference to store value.
	// Returns:
	//      TRUE - if successful, otherwise - FALSE.
	//-----------------------------------------------------------------------
	static BOOL GetRegTZI(HKEY hKey, LPCTSTR pcszValueName, REGISTRY_TIMEZONE_INFORMATION& rRegTZI);

protected:
	//--------------------------------------------------------------
	CXTPCalendarPtrCollectionT<CXTPCalendarTimeZone> m_arTZInfo; // Collection to store CXTPCalendarTimeZone object pointers;

private:
	int CompareTZI(const CXTPCalendarTimeZone* pTZI1,
					const CXTPCalendarTimeZone* pTZI2,
					BOOL bUseIndex) const;

	void ParseDisplayStr(LPCTSTR str, int& rnBias, CString& rstrPlace) const;
	int GetDigit(TCHAR ch) const;

protected:

};


////////////////////////////////////////////////////////////////////////////
#endif // !defined(_XTPCALENDARTIMEZONEHELPER_H__)
