// XTPCalendarMAPIDataProvider.h: interface for the CXTPCalendarData class.
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
#if !defined(_XTPCalendarMAPIDataProvider_H__)
#define _XTPCalendarMAPIDataProvider_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarMAPIWrapper.h"
#include "XTPCalendarMemoryDataProvider.h"
#include "XTPCalendarData.h"

class CXTPCalendarRecurrencePattern;
class CXTPCalendarEventLabels;
class CXTPNotifyConnection;

//{{AFX_CODEJOCK_PRIVATE
// Common customizable properties
// DP props collection
static const LPCTSTR cszMAPIpropName_AppointmentMessageClass = _T("AppointmentMessageClass");
static const LPCTSTR cszMAPIpropVal_AppointmentMessageClass = _T("IPM.Appointment");

static const LPCTSTR cszMAPIpropName_OutlookInternalVersion  = _T("OutlookInternalVersion");
static const int cnMAPIpropVal_OutlookInternalVersionDef = 115608;

static const LPCTSTR cszMAPIpropName_OutlookVersion          = _T("OutlookVersion");
static const LPCTSTR cszMAPIpropVal_OutlookVersionDef = _T("11");

// event runtime custom properties
static const LPCTSTR cszMAPIpropVal_EntryID = _T("MAPIEntryID");
static const LPCTSTR cszMAPIpropVal_SearchKey = _T("MAPISearchKey");

// event customizable properties
// Event props collection

//static const LPCTSTR cszMAPIpropName_AppointmentIconIndex  = _T("AppointmentIconIndex");
static const int cnMAPIpropVal_AppointmentIcon       = 1024;
static const int cnMAPIpropVal_AppointmentIconRecurr = 1025;

//-------------------------------------------------------------------------
// Summary: Structure describes MAPI property ID, type, and guid.
//-------------------------------------------------------------------------
struct XTP_MAPI_PROP_NAME
{
	ULONG   m_ulID;         // ID
	ULONG   m_ulType;       // Type
	GUID    m_GuidPS;       // Guid


	//-------------------------------------------------------------------------
	// Summary: Default constructor
	//-------------------------------------------------------------------------
	XTP_MAPI_PROP_NAME(ULONG ulID, ULONG ulType, const GUID& guidPS)
	{
		m_ulID = ulID;
		m_ulType = ulType;
		m_GuidPS = guidPS;
	}
};

//===========================================================================
// Summary:
//      Undocumented MAPI
// See also:
//      http://support.microsoft.com/?kbid=171670
//      http://www.cdolive.com/cdo10.htm
//===========================================================================
#define XTP_TAG_ID_MAPI_CALENDAR_FOLDER         0x36D00102 // Used to query for the EntryID of the calendar folder off of the root folder

//#define XTP_PS_MAPI_EVENT                     Guid( "0x0220060000000000C000000000000046") //CdoPropSetID1
//#define XTP_PS_MAPI_EVENT_GUID                    "{00062002-0000-0000-C000-000000000046}"    // The GUID of the class of appointment item extended properties

//#define XTP_PS_MAPI_COMMON                        Guid( "0x0820060000000000C000000000000046") //CdoPropSetID4
//#define XTP_PS_MAPI_COMMON_GUID                   "{00062008-0000-0000-C000-000000000046}"    // The GUID of the class of common Outlook extended properties

//#define XTP_PS_MAPI_AGGREGATE_GUID                "{00020329-0000-0000-C000-000000000046}"    // First introduced to support 'keywords' on calendar items

// The GUID of the class of appointment item extended properties
static const GUID xtpGUID_PS_MAPI_Event     = {0x00062002,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
// The GUID of the class of common Outlook extended properties
static const GUID xtpGUID_PS_MAPI_Common    = {0x00062008,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};

//===========================================================================
#define XTP_PR_MAPI_EVENT_ICON_INDEX            PROP_TAG(PT_LONG, 0x1080) // Event icon index

// PT_LONG - Busy status. See also XTPCalendarEventBusyStatus - note that enum values are the same as Outlook values
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_BusyStatus(0x8205, PT_LONG, xtpGUID_PS_MAPI_Event);

// PT_STRING - Event location string
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_Location(0x8208, PT_STRING8, xtpGUID_PS_MAPI_Event);

// PT_SYSTIME - Event start-time
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_StartTime(0x820D, PT_SYSTIME, xtpGUID_PS_MAPI_Event);

// PT_SYSTIME - Event end-time
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_EndTime(0x820E, PT_SYSTIME, xtpGUID_PS_MAPI_Event);

// PT_LONG - Event duration
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_Duration(0x8213, PT_LONG, xtpGUID_PS_MAPI_Event);

// PT_LONG - Event color
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_Color(0x8214, PT_LONG, xtpGUID_PS_MAPI_Event);

// PT_BOOLEAN - Is all day
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_AllDay(0x8215, PT_BOOLEAN, xtpGUID_PS_MAPI_Event);

// PT_LONG - Meeting status enumeration
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_MeetingStatus(0x8217, PT_LONG, xtpGUID_PS_MAPI_Event);

// PT_BINARY - Recurrence binary data
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_RecurrenceState(0x8216, PT_BINARY, xtpGUID_PS_MAPI_Event);

// PT_BOOLEAN - Whether event is recurring
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_IsRecuring(0x8223, PT_BOOLEAN, xtpGUID_PS_MAPI_Event);

// PT_SYSTIME - Recurrence base time
//static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_RecurrenceBase(0x8228, PT_SYSTIME, xtpGUID_PS_MAPI_Event);

// PT_LONG - Recurrence type. See also XTPCalendarEventRecurrenceType - note that enum values are the same as Outlook values
//static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_RecurrenceType(0x8231, PT_LONG, xtpGUID_PS_MAPI_Event);

// PT_TSTRING
//static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_RecurrencePattern(0x8232, PT_STRING8, xtpGUID_PS_MAPI_Event);

// PT_BINARY Event time zone binaries -- for recurrence events only
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_TimeZone(0x8233, PT_BINARY, xtpGUID_PS_MAPI_Event);

//#define XTP_PRID_MAPI_EVENT_TIMEZONE_DESCRIPTION 0x8234 // PT_TSTRING
//static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_TimeZoneDescription(0x8234, PT_STRING8, xtpGUID_PS_MAPI_Event);

//#define XTP_PRID_MAPI_EVENT_RECURRENCE_START  0x8235 // PT_SYSTIME
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_RecurrenceStart(0x8235, PT_SYSTIME, xtpGUID_PS_MAPI_Event);

//#define XTP_PRID_MAPI_EVENT_RECURRENCE_END        0x8236 // PT_SYSTIME
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_RecurrenceEnd(0x8236, PT_SYSTIME, xtpGUID_PS_MAPI_Event);

//#define XTP_PRID_MAPI_EVENT_RESPONSE_STATUS       0x8218 // PT_TSTRING
//static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_(0x8218, PT_STRING8, xtpGUID_PS_MAPI_Event);

//#define XTP_PRID_MAPI_EVENT_REPLY_TIME            0x8222 // PT_TSTRING
//static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_(, , xtpGUID_PS_MAPI_Event);
//#define XTP_PRID_MAPI_EVENT_ORGANIZER         0x822E // PT_TSTRING
//static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_(, , xtpGUID_PS_MAPI_Event);


// PT_LONG - Outlook Internal Version
static const XTP_MAPI_PROP_NAME xtpMAPIpropCommon_OutlookInternalVersion(0x8552, PT_LONG, xtpGUID_PS_MAPI_Common);

// PT_STRING - Outlook Version
static const XTP_MAPI_PROP_NAME xtpMAPIpropCommon_OutlookVersion(0x8554, PT_STRING8, xtpGUID_PS_MAPI_Common);

// PT_LONG - Reminder Minutes Before
static const XTP_MAPI_PROP_NAME xtpMAPIpropCommon_ReminderMinutesBefore(0x8501, PT_LONG, xtpGUID_PS_MAPI_Common);

// PT_SYSTIME - Reminder Date
static const XTP_MAPI_PROP_NAME xtpMAPIpropCommon_ReminderDate(0x8502, PT_SYSTIME, xtpGUID_PS_MAPI_Common);

// PT_BOOLEAN - Is reminder set
static const XTP_MAPI_PROP_NAME xtpMAPIpropCommon_ReminderSet(0x8503, PT_BOOLEAN, xtpGUID_PS_MAPI_Common);

// PT_BOOLEAN - Event private flag
static const XTP_MAPI_PROP_NAME xtpMAPIpropCommon_IsPrivate(0x8506, PT_BOOLEAN, xtpGUID_PS_MAPI_Common);

//#define XTP_PRID_MAPI_COMMON_CONTEXT_MENU_FLAGS        0x8510 // PT_LONG
//static const XTP_MAPI_PROP_NAME xtpMAPIpropCommon_(0x8510, PT_LONG, xtpGUID_PS_MAPI_Common);

// PT_STRING - Event custom properties XML string
static const XTP_MAPI_PROP_NAME xtpMAPIpropEvent_CustomProperties(0x8581, PT_STRING8, xtpGUID_PS_MAPI_Event);

/////////////////////////////////////////////////////////////////////////////

//===========================================================================
// Summary: MAPI recurrence type undocumented enum.
//===========================================================================
enum XTPCalendarMAPIRcType
{
	xtpMAPIRcType_Unknown   = 0,            // Recurrence Type unknown.

	xtpMAPIRcType_Daily     = 0x200A,       // Daily Recurrence
	xtpMAPIRcType_Weekly    = 0x200B,       // Weekly Recurrence
	xtpMAPIRcType_Monthly   = 0x200C,       // Monthly Recurrence
	xtpMAPIRcType_Yearly    = 0x200D,       // Yearly Recurrence
};


//===========================================================================
// Summary: MAPI recurrence type undocumented enum - second version.
//===========================================================================
enum XTPCalendarMAPIRcType2
{
//  xtpMAPIRcType_Unknown       = -1,

	xtpMAPIRcType2_First            = 0,    // First enum value

	xtpMAPIRcType2_Daily            = 0,    // Daily Recurrence
	xtpMAPIRcType2_Weekly           = 1,    // Weekly Recurrence
	xtpMAPIRcType2_MonYearly        = 2,    // Monthly/Yearly Recurrence
	xtpMAPIRcType2_MonYearly_Nth    = 3,    // Monthly/Yearly every Nth item

	xtpMAPIRcType2_Last             = 3,    // Last enum value
};


//===========================================================================
// Summary: Describes recurrence end type
//===========================================================================
enum XTPCalendarMAPIRcEnd
{
	xtpMAPIRcEnd_Date   = 0x2021,           // End by date
	xtpMAPIRcEnd_Number = 0x2022,           // End after a number of occurrences
	xtpMAPIRcEnd_Never  = 0x2023            // Never end
};

//===========================================================================
// Summary: Constants for parsing of changes of the
//          single occurrence in recurrence data.
//          Comment is the size of extra data
//===========================================================================
enum XTPCalendarMAPIRcExceptionData
{
	xtpMAPIRcED_Subject         = 0x001, // [w:buf-size][w:str-len][ascii-char: count=len]
	xtpMAPIRcED_IsMeeting       = 0x002, // [dw] - bool(0|1)
	xtpMAPIRcED_ReminderTime    = 0x004, // [dw] minutes before start
	xtpMAPIRcED_IsReminder      = 0x008, // [dw] - bool(0|1)
	xtpMAPIRcED_Location        = 0x010, // [w:buf-size][w:str-len][ascii-char: count=len]
	xtpMAPIRcED_BusyStatus      = 0x020, // [dw] - busy status [0..3] = (free, busy, tent, oof)
	xtpMAPIRcED_Reserved        = 0x040, // unknown flag from MAPI
	xtpMAPIRcED_IsAllDay        = 0x080, // [dw] - bool(0|1)
	xtpMAPIRcED_Label           = 0x100, // [dw] - label ID
};

//===========================================================================
// Summary: This structure is used by CDO/MAPI to store timezone information
// See also: TIME_ZONE_INFORMATION overview in platform SDK.
//===========================================================================
struct XTP_MAPI_TIME_ZONE_INFORMATION
{
	LONG        Bias;           // Current bias for local time translation on this computer, in minutes.
	LONG        StandardBias;   // Bias value to be used during local time translations that occur during standard time.
	LONG        DaylightBias;   // Bias value to be used during local time translations that occur during daylight saving time.
	WORD        Reserved1;      // Not used
	SYSTEMTIME  StandardDate;   // A SYSTEMTIME structure that contains a date and local time when the transition from daylight saving time to standard time occurs on this operating system.
	WORD        Reserved2;      // Not used
	SYSTEMTIME  DaylightDate;   // A SYSTEMTIME structure that contains a date and local time when the transition from standard time to daylight saving time occurs on this operating system.
};

//===========================================================================
// Summary:
//     This is an internal class which represents MAPI Binary data structure
//     and implements main operations under it.
//===========================================================================
class CXTPMAPIBinary
{
public:
	// Default constructor.
	CXTPMAPIBinary();
	// Construct an object from MAPI pointers.
	CXTPMAPIBinary(ULONG cbEntryID, LPENTRYID lpEntryID);
	// Copy constructor.
	CXTPMAPIBinary(const CXTPMAPIBinary& eid);
	// Destructor
	virtual ~CXTPMAPIBinary();

	// The operator= method sets this CXTPMAPIBinary object equal to the
	// specified CXTPMAPIBinary object.
	CXTPMAPIBinary& operator=(const CXTPMAPIBinary& eid);

	// Returns a size of MAPI entry structure,
	ULONG GetBinarySize() const;
	// Returns a pointer to the MAPI EntryID binary data.
	LPBYTE GetBinaryData() const;

	// Initializes an object from MAPI pointers.
	void Set(ULONG cbSize, LPBYTE lpData);
	// Initializes an object from MAPI PropValue structure.
	void Set(LPSPropValue pPropEntryID);

	// Binary comparison of those objects.
	static BOOL IsBinaryEqual(const CXTPMAPIBinary& eid1, const CXTPMAPIBinary& eid2);

#ifdef _DEBUG
	void DebugPrint();
#endif

//private:
	ULONG m_cb; // Bytes count in the MAPI structure.
	CByteArray m_arBytes; // Binary storage for Entry ID data.

private:
	friend class CXTPCalendarMAPIDataProvider;
	friend class CXTP_ID_Collection;
};

// Define EventID to EntryID map association.
typedef CMap<DWORD, DWORD, CXTPMAPIBinary, CXTPMAPIBinary&> CMap_EventID_EntryIDIndex;
// Define EntryID to EventID map association.
typedef CMap<CXTPMAPIBinary, CXTPMAPIBinary&, DWORD, DWORD> CMap_EntryIDIndex_EventID;
// Define EventID's collection with fast search.
typedef CMap<DWORD, DWORD, BOOL, BOOL> CMap_EventIDs;

// Implements Hash method for MAPI Entry ID items.
template<> AFX_INLINE
UINT AFXAPI HashKey(CXTPMAPIBinary& key)
{
	UINT nRes = 0;
	for (UINT i = 0; (i < 4) && (key.m_cb > i); i++)
	{
		nRes *= 256;
		nRes += key.m_arBytes.GetAt(i);
	}
	return nRes;
}

// Compare method helper for MAPI entry ID items
template<> AFX_INLINE BOOL AFXAPI CompareElements(
	const CXTPMAPIBinary* lpt1, const CXTPMAPIBinary* lpt2)
{
	return CXTPMAPIBinary::IsBinaryEqual(*lpt1, *lpt2);
}

//===========================================================================
// Summary:
//     This internal class implements a collection of events' MAPI Entry ID's
//     and events' XTP Event ID's with the possibility to quickly search for
//     each other.
//===========================================================================
class CXTP_ID_Collection
{
public:
	// Default constructor.
	CXTP_ID_Collection();

	// Lookup for Entry ID by corresponding EventID.
	BOOL Lookup(const DWORD dwEventID, CXTPMAPIBinary& obEntryID);
	// Lookup for EventID by corresponding EntryID.
	BOOL Lookup(CXTPMAPIBinary& obEntryID, DWORD& dwEventID);

	// Adds a pair EventID/EntryID into the collection.
	BOOL Add(const DWORD dwEventID, CXTPMAPIBinary& obEntryID);

	// Iterate all internal EventID's and look for them in the mapEntries
	void FindMissing(CMap_EventIDs& mapExisting, CMap_EventIDs& mapMissed);

protected:
	CMap_EventID_EntryIDIndex m_mapEventID; // index from EventID to EntryID
	CMap_EntryIDIndex_EventID m_mapEntryID; // index from EntryID to EventID
};
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     This class is the implementation of the XTPCalendarData abstract
//     class which represents the data portion of the Calendar control.
// Remarks:
//     This implementation uses Extended MAPI as the data source.
//     The Data source stores all Events data and provides a way to retrieve
//     and save CXTPCalendarEvent objects.
//
//     To use CXTPCalendarMAPIDataProvider first construct an instance
//     of CXTPCalendarMAPIDataProvider object then use Open() member function
//     to establish connection to the data source and initialize the object.
//     Class provides a number of functions to manipulate events and events
//     related data. After finished working with the data source must call
//     the Close() member function to close the connection and to de-initialize
//     the object and free any unused resources.
//
//     MAPI data provider automatically opens default message store and default
//     user's calendar folder and reads all appointments there. MAPI data provider
//     uses Memory Data Provider internally as a private cache.
//     Also it uses a MAPI notifications mechanism in order to have MAPI
//     appointments always updated. When event is updated in MAPI storage,
//     it will be automatically reloaded in the data provider and updated on
//     the screen. It also works in the same way reverse: when an event is
//     updated in the Calendar control, it will be automatically updated in
//     the MAPI storage.
//
// See Also: CXTPCalendarData, CXTPCalendarMemoryDataProvider
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarMAPIDataProvider : public CXTPCalendarData, public CXTPCalendarMAPIWrapper
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarMAPIDataProvider)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	// Remarks:
	//     Constructs a CXTPCalendarMAPIDataProvider object. To establish a
	//     connection to a specified data source and initialize the object the
	//     Open() member function must be called immediately after construction.
	// See Also: Open()
	//-----------------------------------------------------------------------
	CXTPCalendarMAPIDataProvider();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default Destructor.
	// Remarks:
	//     Handles all deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarMAPIDataProvider();

	//-----------------------------------------------------------------------
	// Summary:
	//     Open user's message store, default calendar folder and reads all
	//     events there.
	// Remarks:
	//     Call this member function to initialize a newly constructed
	//     CXTPCalendarMAPIDataProvider object.
	//     Data object must be created before you call Open.
	//     This method firstly initializes MAPI, login with specified user's
	//     credentials and then reads all appointments.
	// Returns:
	//     TRUE is successful. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL Open();

	//-----------------------------------------------------------------------
	// Summary:
	//     For MAPI Data Provider this methods does the same sequence of
	//     actions as Open method.
	// Returns:
	//     TRUE is successful. FALSE otherwise.
	// See also:
	//     Open
	//-----------------------------------------------------------------------
	virtual BOOL Create();

	//-----------------------------------------------------------------------
	// Summary:
	//     For MAPI Data Provider this methods does nothing.
	// Returns:
	//     TRUE.
	//-----------------------------------------------------------------------
	virtual BOOL Save();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to close the connection to a data source.
	// Remarks:
	//     This method closes the connection to the MAPI data source and
	//     uninitializes MAPI libraries.
	//-----------------------------------------------------------------------
	virtual void Close();

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks whether a connection to the MAPI data source is already opened.
	// Returns:
	//     TRUE is connection is opened. FALSE otherwise.
	// See also:
	//     Open
	//-----------------------------------------------------------------------
	virtual BOOL IsOpen();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieve day events from memory cache.
	// Parameters:
	//     dtDay - A specified day.
	// Remarks:
	//     As far as we are getting cache synchronized with the MAPI storage,
	//     this method works from memory cache.
	// Returns:
	//     A collection of events for a specified day.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoRetrieveDayEvents(COleDateTime dtDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Remove all events from the MAPI storage.
	// Remarks:
	//     This method works with memory cache only and removes all data there.
	//-----------------------------------------------------------------------
	virtual void DoRemoveAllEvents();

	//-----------------------------------------------------------------------
	// Summary:
	//     Read event from the MAPI storage
	// Parameters:
	//     dwEventID - Event ID of the read target.
	// Remarks:
	//     As far as we are getting cache synchronized with the MAPI storage,
	//     this method reads an event from the memory cache.
	// Returns:
	//     A pointer to the created CXTPCalendarEvent object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr DoRead_Event(DWORD dwEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Read recurrence pattern from the MAPI storage
	// Parameters:
	//     dwPatternID - Pattern ID of the read target.
	// Remarks:
	//     As far as we are getting cache synchronized with the MAPI storage,
	//     this method reads a recurrence pattern from the memory cache.
	// Returns:
	//     A pointer to the created CXTPCalendarRecurrencePattern object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarRecurrencePatternPtr DoRead_RPattern(DWORD dwPatternID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new appointment in the MAPI storage.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of a newly created appointment.
	//     rdwNewEventID - [out] EventID of a newly created record.
	// Remarks:
	//     This method creates a new message of the appointment type in the
	//     MAPI data storage and saves all event properties there.
	// Returns:
	//     TRUE if an appointment created successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoCreate_Event(CXTPCalendarEvent* pEvent, DWORD& rdwNewEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates appointment properties in the MAPI storage.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an appointment.
	// Remarks:
	//     This method updates a corresponding MAPI appointment with
	//     properties from the specified event.
	// Returns:
	//     TRUE if an appointment updated successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoUpdate_Event(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes an appointment from the MAPI storage.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an appointment.
	// Remarks:
	//     This method deletes a corresponding MAPI appointment from the
	//     MAPI storage.
	// Returns:
	//     TRUE if an appointment deleted successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoDelete_Event(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new recurrence pattern
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object.
	//     rdwNewPatternID - [out] Pattern ID.
	// Remarks:
	//     As far as we are getting cache synchronized with the MAPI storage,
	//     this method just assigns rdwNewPatternID to the provided pattern ID..
	// Returns:
	//     Always returns TRUE.
	//-----------------------------------------------------------------------
	virtual BOOL DoCreate_RPattern(CXTPCalendarRecurrencePattern* pPattern, DWORD& rdwNewPatternID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates a recurrence pattern
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object.
	// Remarks:
	//     As far as we are getting cache synchronized with the MAPI storage,
	//     this method actually does nothing..
	// Returns:
	//     Always returns TRUE.
	//-----------------------------------------------------------------------
	virtual BOOL DoUpdate_RPattern(CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes a recurrence pattern
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object.
	// Remarks:
	//     As far as we are getting cache synchronized with the MAPI storage,
	//     this method actually does nothing..
	// Returns:
	//     Always returns TRUE.
	//-----------------------------------------------------------------------
	virtual BOOL DoDelete_RPattern(CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a collection of events expiring in the period from
	//     dtFrom during next spPeriod.
	// Parameters:
	//     dtFrom   - Start date and time of a specified period.
	//     spPeriod - A duration of a specified period.
	// Remarks:
	//     As far as we are getting cache synchronized with the MAPI storage,
	//     this method actually works from memory cache.
	//     It is used by a reminders manager.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     upcoming events.
	// See Also:
	//     CXTPCalendarRemindersManager overview
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoGetUpcomingEvents(COleDateTime dtFrom, COleDateTimeSpan spPeriod);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method implements Getting all events from the MAPI storage
	//     without generating recurrence occurrences.
	// Remarks:
	//     Simple events are included as is.
	//     For the recurrence events only master events are included,
	//     recurrence occurrences are not generated.
	//     As far as we are getting cache synchronized with the MAPI storage,
	//     this method actually works from memory cache.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     all events.
	// See Also: GetAllEvents_raw()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoGetAllEvents_raw();

protected:
	// Connection string items (for the next version)
	//CString m_strProfileName; // Name of the MAPI profile
	//CString m_strUsername;    // Login name to this profile
	//CString m_strPassword;    // User's password for login to the profile.
	//CString m_strMsgStoreGuid;// Message store
	//CString m_strFolderGUID;  // Folder with appointment items
	//BOOL m_bShowFolderChooser;// Whether to choose a folder using a buit-in dialog.

//{{AFX_CODEJOCK_PRIVATE

//private:
	CXTP_ID_Collection m_mapID; // A map object with ID's referencing each other...

	// Compares eid1 and eid2.
	// Returns TRUE if IDs are equal and FALSE otherwise.
	BOOL Equal(const CXTPMAPIBinary& eid1, const CXTPMAPIBinary& eid2);

//private:
	// Finds event SearchKey its MAPI EntryID.
	CXTPMAPIBinary GetSearchKey(ULONG cbEntryID, LPENTRYID lpEntryID);

	// Finds Calendar event ID by its MAPI SearchKey.
	// Adds new event ID / SearchKey pair if not exists.
	DWORD GetEventID(ULONG cbSearchKey, LPENTRYID lpSearchKey, BOOL bAddNew = FALSE);

	// Finds MAPI EntryID of the event by its MAPI Search Key
	CXTPMAPIBinary GetEntryID(CXTPMAPIBinary& eSearchKey);

	// Calculates next unique EventID, which does not exists in ID's collection.
	DWORD GetNextUniqueEventID(DWORD dwEventID = XTP_CALENDAR_UNKNOWN_EVENT_ID);

//private:
	// Opens default MAPI Message Store.
	static STDMETHODIMP OpenDefaultMessageStore(LPMAPISESSION lpMAPISession, LPMDB * lpMDB, CXTPCalendarMAPIDataProvider* pProvider);
	// Opens default MAPI Inbox folder.
	static STDMETHODIMP OpenInbox(LPMDB lpMDB, LPMAPIFOLDER *lpInboxFolder, LPMAPIFOLDER *lpCalendarFolder, CXTPCalendarMAPIDataProvider* pProvider);

	//static LPSRestriction BuildDayRestriction(const COleDateTime dtDay);
	static LPSRestriction BuildBinaryRestriction(ULONG cbSize, LPBYTE lpData, ULONG ulPropTag);

	// Imports all events (by a restriction) to the events collection and returns it.
	CXTPCalendarEventsPtr ImportAllEvents(LPSRestriction lpRestriction = NULL);

	// Scans events table (by a restriction),
	//   adds new events into the specified collection,
	//   and deletes removed events from this collection.
	// Returns: TRUE if ptrEvents collection was changed during the processing,
	//          FALSE if ptrEvents wasn't changed, what means that collections are equal.
	//BOOL ImportNewEvents(CXTPCalendarEventsPtr ptrEvents, LPSRestriction lpRestriction = NULL);
	BOOL ImportNewEvents();

	// Creates and returns an Event object, imports all its known fields from lpRow.
	CXTPCalendarEventPtr ImportEvent(LPSRow lpRow);
	// Creates and returns an Event object, imports all its known fields by EntryID.
	CXTPCalendarEventPtr ImportEvent(ULONG cbEntryID, LPENTRYID lpEntryID);
	// Creates and returns an Event object, imports all its known fields from pMessage.
	CXTPCalendarEventPtr ImportEvent(LPMESSAGE pMessage);

	CXTPCalendarEventPtr ImportEvent_FromCalendarFolderOnly(ULONG cbEntryID, LPENTRYID lpEntryID);

	// Import Recurrence structure from the binary arRCData into the associated
	// recurrence pattern for pMasterEvent.
	void ImportRecurrence(CXTPCalendarEvent* pMasterEvent, const CByteArray& arRCData);

	// Updates MAPI event data from source Calendar Event structure.
	BOOL UpdateMAPIEvent(LPMESSAGE pMessage, CXTPCalendarEvent* pEvent);
	// Set Event global MAPI properties if those are not already exist.
	void _SetMAPIEventGlobalPropsIfNeed(LPMESSAGE pMessage, CXTPCalendarEvent* pEvent);

	void _SetEventRuntimeProps(CXTPCalendarCustomProperties* pEventProps, LPMESSAGE pMessage);
	void _MoveMAPIEventRuntimeProps(CXTPCalendarCustomProperties* pDest, CXTPCalendarCustomProperties* pSrc);

	HRESULT _getPropVal(LPMESSAGE pMessage, ULONG ulPropTag, CByteArray& rData);

	CString _getPropVal_str(LPMESSAGE pMessage, ULONG ulPropTag);
	int     _getPropVal_int(LPMESSAGE pMessage, ULONG ulPropTag);
	SYSTEMTIME _getPropVal_UtcTime(LPMESSAGE pMessage, ULONG ulPropTag);

	CString _getPropVal_str(LPMESSAGE pMessage, const XTP_MAPI_PROP_NAME& propNameEx);
	int     _getPropVal_int(LPMESSAGE pMessage, const XTP_MAPI_PROP_NAME& propNameEx);
	SYSTEMTIME _getPropVal_UtcTime(LPMESSAGE pMessage, const XTP_MAPI_PROP_NAME& propNameEx);

	BOOL _setPropTagVal_int(LPMESSAGE pMessage, ULONG ulPropTag, int nValue);
	BOOL _setPropTagVal_str(LPMESSAGE pMessage, ULONG ulPropTag, LPCTSTR pcszValue);
	BOOL _setPropTagVal_UtcTime(LPMESSAGE pMessage, ULONG ulPropTag, const SYSTEMTIME& stTime);
	BOOL _setPropTagVal_bin(LPMESSAGE pMessage, ULONG ulPropTag, const CByteArray& arData);

	HRESULT _deletePropTag(LPMESSAGE pMessage, ULONG ulPropTag);
	HRESULT _deletePropsTags(LPMESSAGE pMessage, CUIntArray& arPropsTags);

	int GetEventGlobalPropVal_int(LPCTSTR pcszPropName, int nDefault);
	CString GetEventGlobalPropVal_str(LPCTSTR pcszPropName, LPCTSTR pcszDefault);

	static int _getSimpleMAPITypeSize(int nType);
	static UINT _getStreamSize(IStream* pStream);

	static LONG STDAPICALLTYPE MAPICallBack_OnNotify(LPVOID lpvContext,
													 ULONG  cNotif,
													 LPNOTIFICATION  lpNotif);

	// This class implements a collection of property tags and their IDs
	// with the possibility to quickly find each corresponding other.
	class CMAPIPropIDMap
	{
	public:
		CMAPIPropIDMap();

		ULONG GetPropTagByID(LPMESSAGE pMessage, const XTP_MAPI_PROP_NAME& propNameEx);

		ULONG GetPropIDByTag(LPMESSAGE pMessage, ULONG ulPropTag);

	protected:
		typedef CMap<ULONG, ULONG, ULONG, ULONG> CMapULONG;

		// Read all properties existing in the pMessage
		void _UpdateMaps(LPMESSAGE pMessage);

		// Try Read requested property tag from the pMessage
		ULONG _GetPropTagFromID_ex(LPMESSAGE pMessage, const XTP_MAPI_PROP_NAME& propNameEx);

		CMapULONG m_mapPropID2Tag; // Map Property ID to Property Tag
		CMapULONG m_mapPropTag2ID; // Map Property Tag to Property ID

	//private:
		CXTPCalendarMAPIDataProvider* m_pProvider; // MAPI Wrapper

		friend class CXTPCalendarMAPIDataProvider;
	};

	CMAPIPropIDMap m_MapiHelper; // MAPI Properties ID/Tag helper

//private:
	BOOL m_bMAPIInitialized; // MAPI library is initialized or not.

	LPMAPISESSION m_lpMAPISession;  // MAPI Session
	LPMDB         m_lpMDB;          // MAPI message store

	LPMAPIFOLDER    m_lpCalendarFolder;  // Calendar folder handler
	CXTPMAPIBinary m_eidCalendarFolder; // Calendar folder Entry ID

	ULONG         m_ulMAPIConID0;
	ULONG         m_ulMAPIConID1;

	LPMAPIADVISESINK m_lpAdviseSink;
	LPMAPIADVISESINK m_lpAdviseSink_ThrSafe;


//private:
	CXTPCalendarData*   m_pMemDP;
//}}AFX_CODEJOCK_PRIVATE

protected:
	friend class CXTPCalendarMemDPInternal;
	friend class CMAPIPropIDMap;
	using CXTPCalendarData::SendNotification;


	//=======================================================================
	// Summary: Internal extension of the CXTPCalendarMemoryDataProvider
	//          used for handling all notifications.
	//=======================================================================
	class CXTPCalendarMemDPInternal : public CXTPCalendarMemoryDataProvider
	{

		CXTPCalendarData* m_pDPExternal; // External data provider.
	public:

		//-------------------------------------------------------------------
		// Summary: Default constructor.
		// Parameters:
		//     pDPExternal - Pointer to the external data provider to be
		//                   initialized with.
		//-------------------------------------------------------------------
		CXTPCalendarMemDPInternal(CXTPCalendarData* pDPExternal);

		//-------------------------------------------------------------------
		// Summary:
		//     This member function sends a notification to all subscribers.
		// Parameters:
		//     EventCode - A specific code identifying the event.
		//     wParam - First custom parameter. Depends on the event type.
		//              See specific event description for details.
		//     lParam - Second custom parameter. Depends on the event type.
		//              See specific event description for details.
		//-------------------------------------------------------------------
		virtual void SendNotification(XTP_NOTIFY_CODE EventCode, WPARAM wParam , LPARAM lParam);
	};
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(_XTPCalendarMAPIDataProvider_H__)
