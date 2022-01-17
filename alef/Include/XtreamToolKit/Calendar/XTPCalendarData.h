// XTPCalendarData.h: interface for the CXTPCalendarData class.
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
#if !defined(_XTPCALENDARDATA_H__)
#define _XTPCALENDARDATA_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////
#include "Common/XTPNotifyConnection.h"

#include "XTPCalendarOptions.h"
#include "XTPCalendarEvent.h"

class CXTPCalendarRecurrencePattern;
class CXTPCalendarEventLabels;
class CXTPCalendarSchedules;
class CXTPNotifyConnection;
class CXTPCalendarOptions;

//===========================================================================
// Summary:
//     Cache table size.
// Remarks:
//    Should be a prime number!
//    like: 503, 1021, 1511, 2003, 3001, 4001, 5003, 6007, 8009, 12007, 16001,
//          32003, 48017, 64007
//===========================================================================
#define XTP_OBJECT_CACHE_HASH_TABLE_SIZE 1021

static const LPCTSTR cszProcess_RecurrenceState     = _T("process_RecurrenceState");    // String name for a corresponding property
static const LPCTSTR cszProcess_RecurrencePatternID = _T("process_RecurrencePatternID");// String name for a corresponding property

//===========================================================================
// Summary:
//     This class defines an abstract representation of the Calendar
//     control data with all necessary methods and some predefined
//     functionality added,
// Remarks:
//     This class is the definition of the XTPCalendarData abstract
//     class which represents a data portion of the Calendar control.
//
//     To implement a custom Data Provider solution you should create a class
//     derived from CXTPCalendarData and implement necessary virtual methods.
//     The first main set of methods to be implemented are Open, Close, Save,
//     Create, IsOpen, etc.
//     The second main set of methods to be implemented is starting from Do verb.
//     Those are: DoRetrieveDayEvents, DoRead_Event, DoUpdate_RPattern, etc.
//
//     To use a descendant of CXTPCalendarData in a regular way you first
//     construct an instance of CXTPCalendarData object, then use Open()
//     member function to establish connection to the data source and
//     initialize the object. Class provides a number of functions to
//     manipulate events and events related data. After finishing working
//     with data source you should call Close() member function to close
//     connection, de-initialize the object and free unused resources.
//
//     This class also contains an implementation of the data provider cache,
//     which is used by default and gets a great performance increase in case
//     when any slow connections are used (for example database connection).
// See Also:
//     CXTPCalendarData overview, CXTPCalendarEvents, CXTPCalendarEvent,
//     CXTPCalendarCustomDataProvider
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarData : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarRecurrencePattern;
	friend class CXTPCalendarEvent;
	friend class CXTPCalendarOptions;

	DECLARE_DYNAMIC(CXTPCalendarData)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	// Remarks:
	//     Constructs a Data object. To establish a connection to a
	//     specified data source and initialize the object you must
	//     call Open() member function immediately after construction.
	// See Also: Open()
	//-----------------------------------------------------------------------
	CXTPCalendarData();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default Destructor.
	// Remarks:
	//     Handles all deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarData();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to initialize a newly constructed
	//     Data object. Your data object must be created before you
	//     can call Open().
	// Returns:
	//     Boolean value represents succsess/failure result
	//-----------------------------------------------------------------------
	virtual BOOL Open();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to create a file for storing custom
	//     data provider data.
	// Remarks:
	//     It will be opened by default.
	// Returns:
	//     TRUE is successful. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL Create();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method saves data to the file.
	// Remarks:
	//     The most recent data from the data provider will be saved in
	//     the same external file as before.
	//     Note that all the data inside the data provider is already up to
	//     date, this method just allows it to be saved externally.
	// Returns:
	//     TRUE is successful. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL Save();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to close the connection to a data source.
	//-----------------------------------------------------------------------
	virtual void Close();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method determines whether a data source connection is opened..
	// Returns:
	//     TRUE when a connection is already opened. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsOpen();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method determines a type of the current Data Provider
	//     implementation.
	// Returns:
	//     Current Data Provider implementation type.
	// See also:
	//     XTPCalendarDataProvider enumeration.
	//-----------------------------------------------------------------------
	virtual XTPCalendarDataProvider GetType();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves day events for a specified day from the data source.
	// Parameters:
	//     dtDay - A specified day.
	// Remarks:
	//     This method retrieves all appointments for a specified day from
	//     the data source. It includes recurrence appointments occurrences,
	//     regular appointments for this day, multi day appointments which
	//     also have a part of this day.
	//     This method uses DoRetrieveDayEvents realization and implements
	//     some additional service processing.
	// Returns:
	//     A collection of events for a specified day.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr RetrieveDayEvents(COleDateTime dtDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new empty CXTPCalendarEvent object.
	// Parameters:
	//     dwEventID - ID for a new event
	// Remarks:
	//     This method creates a new CXTPCalendarEvent class and
	//     initializes it's ID.
	//
	//     It could be reimplemented in a descendant if it has its own
	//     ID's source (for example, database key field).
	// Returns:
	//     Created empty event.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr CreateNewEvent(DWORD dwEventID = XTP_CALENDAR_UNKNOWN_EVENT_ID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new empty CXTPCalendarRecurrencePattern object.
	// Parameters:
	//     dwPatternID - ID for a new Recurrence Pattern
	// Remarks:
	//     This method creates a new CXTPCalendarRecurrencePattern class and
	//     initializes it's ID.
	//
	//     It could be reimplemented in a descendant if it has its own
	//     ID's source (for example, database key field).
	// Returns:
	//     Created empty Recurrence Pattern.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarRecurrencePatternPtr CreateNewRecurrencePattern(DWORD dwPatternID = XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Read event from the data source.
	// Parameters:
	//     dwEventID - Event ID of the read target.
	// Remarks:
	//     This method retrieves all data for a specified event.
	//     This method uses DoRead_Event realization and implements
	//     some additional service processing.
	// Returns:
	//     A pointer to the new CXTPCalendarEvent object which fields
	//     are filled with a data from the data source.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr GetEvent(DWORD dwEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Read recurrence pattern from the data source
	// Parameters:
	//     dwPatternID - Pattern ID of the read target.
	// Remarks:
	//     This method retrieves all data for a specified recurrence pattern.
	//     This method uses DoRead_RPattern realization and implements
	//     some additional service processing.
	// Returns:
	//     A pointer to the new CXTPCalendarRecurrencePatternPtr object which
	//     fields are filled with a data from the data source.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarRecurrencePatternPtr GetRecurrencePattern(DWORD dwPatternID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new event in the data source
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of a newly created record.
	// Remarks:
	//     This method creates a data record corresponding with the
	//     specified event.
	//     This method uses DoCreate_Event realization and implements
	//     some additional service processing.
	// Returns:
	//     TRUE if event created successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL AddEvent(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a set of events in the data source
	// Parameters:
	//     pEvents - Pointer to the CXTPCalendarEvents collection which
	//               contains a number of CXTPCalendarEvent objects.
	// Remarks:
	//     This method adds a collection of events into the data source.
	// Returns:
	//     TRUE if all events were added successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL AddEvents(CXTPCalendarEvents* pEvents);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all events from the data source.
	// Remarks:
	//     This method removes all appointments from the data source.
	//     This method uses DoRemoveAllEvents realization and implements
	//     some additional service processing.
	//-----------------------------------------------------------------------
	virtual void RemoveAllEvents();

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes an event from the data source.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an event.
	// Remarks:
	//     This method deletes a data record corresponding with the
	//     specified event.
	//     This method uses DoDelete_Event realization and implements
	//     some additional service processing.
	// Returns:
	//     TRUE if an event deleted successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DeleteEvent(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates event properties in the data source.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an event.
	// Remarks:
	//     This method updates a data record corresponding with the
	//     specified event.
	//     This method uses DoUpdate_Event realization and implements
	//     some additional service processing.
	// Returns:
	//     TRUE if an event updated successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL ChangeEvent(CXTPCalendarEvent* pNewEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a connection pointer for the.data source.
	// Returns:
	//     A connection pointer.
	// See also;
	//     CXTPNotifyConnection overview.
	//-----------------------------------------------------------------------
	virtual CXTPNotifyConnection* GetConnection();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a custom properties collection.
	// Returns:
	//     A custom properties collection associated with the data source..
	// See also;
	//     CXTPCalendarCustomProperties overview.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarCustomProperties* GetCustomProperties();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a collection of Labels used in this Data source.
	// Returns:
	//     A collection of Labels used in this Data source.
	//     By default it has some predefined values.
	// See also;
	//     CXTPCalendarEventLabels overview, SetLabelList(),
	//     CXTPCalendarEventLabels::InitDefaultValues
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventLabels* GetLabelList() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Set custom collection of Labels to be used in this Data source.
	// Parameters:
	//     pLabelList - A custom collection of Labels to be used there.
	// See also;
	//     CXTPCalendarEventLabels overview, GetLabelList()
	//-----------------------------------------------------------------------
	virtual void SetLabelList(CXTPCalendarEventLabels* pLabelList);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a collection of schedules used in this data source.
	// Returns:
	//     A collection of schedules associated with the data source.
	// See also;
	//     CXTPCalendarSchedules overview.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarSchedules* GetSchedules() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a data provider connection string.
	// Parameters:
	//     lpszConnectionString - A connection string.
	// See also;
	//     GetConnectionString(), GetDataSource()
	//-----------------------------------------------------------------------
	virtual void SetConnectionString(LPCTSTR lpszConnectionString);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a data provider connection string.
	// Returns:
	//     Current connection string.
	// See also;
	//     SetConnectionString(), GetDataSource()
	//-----------------------------------------------------------------------
	virtual CString GetConnectionString();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a data provider data source.
	// Returns:
	//     String with current data source settings.
	// Remarks:
	//     This method parses a connection string and returns a data source
	//     as a part of it.
	// See also;
	//     GetConnectionString(), SetConnectionString()
	//-----------------------------------------------------------------------
	virtual CString GetDataSource();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines current cache mode.
	// See also:
	//     XTPCalendarDataProviderCacheMode enumeration,
	//     SetCacheMode().
	//-----------------------------------------------------------------------
	virtual int GetCacheMode() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Set common data provider cache mode.
	// Parameters:
	//     eCacheMode         - Cache mode. See XTPCalendarDataProviderCacheMode
	//     pCacheDataProvider - Data Provider object which you want to use
	//                          for cache purposes. By default it is
	//                          CXTPCalendarMemoryDataProvider.
	// Remarks:
	//     This method updates an internal cache mode.
	// See also:
	//     XTPCalendarDataProviderCacheMode enumeration.
	//-----------------------------------------------------------------------
	virtual void SetCacheMode(int eCacheMode, CXTPCalendarData* pCacheDataProvider = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Clears common data provider cache.
	//-----------------------------------------------------------------------
	virtual void ClearCache();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a collection of events expiring in the period from
	//     dtFrom during next spPeriod.
	// Parameters:
	//     dtFrom   - Start date and time of a specified period.
	//     spPeriod - A duration of a specified period.
	// Remarks:
	//     This method is used by a reminders manager.
	//     This method uses DoGetUpcomingEvents realization and implements
	//     some additional service processing.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     upcoming events.
	// See Also:
	//     CXTPCalendarRemindersManager overview
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr GetUpcomingEvents(COleDateTime dtFrom = (DATE)0, COleDateTimeSpan spPeriod = (DATE)0);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method Gets all events without generating recurrence
	//     occurrences.
	// Remarks:
	//     Simple events are included as is.
	//     For the recurrence events only master events are included,
	//     recurrence occurrences are not generated.
	//     This method uses DoGetAllEvents_raw realization and implements
	//     some additional service processing.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     all events.
	// See Also: _Save(), Serialize(), DoGetAllEvents_raw()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr GetAllEvents_raw();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves day events for a specified day from the data source.
	// Parameters:
	//     dtDay - A specified day.
	// Remarks:
	//     This method retrieves all appointments for a specified day from
	//     the data source. It includes recurrence appointments occurrences,
	//     regular appointments for this day, multi day appointments which
	//     also have a part of this day.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     A collection of events for a specified day.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoRetrieveDayEvents(COleDateTime dtDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all events from the data source.
	// Remarks:
	//     This method removes all appointments from the data source.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	//-----------------------------------------------------------------------
	virtual void DoRemoveAllEvents();

	//-----------------------------------------------------------------------
	// Summary:
	//     Read event from the data source.
	// Parameters:
	//     dwEventID - Event ID of the read target.
	// Remarks:
	//     This method retrieves all data for a specified event.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     A pointer to the new CXTPCalendarEvent object which fields
	//     are filled with a data from the data source.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr DoRead_Event(DWORD dwEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Read recurrence pattern from the data source
	// Parameters:
	//     dwPatternID - Pattern ID of the read target.
	// Remarks:
	//     This method retrieves all data for a specified recurrence pattern.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     A pointer to the new CXTPCalendarRecurrencePatternPtr object which
	//     fields are filled with a data from the data source.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarRecurrencePatternPtr DoRead_RPattern(DWORD dwPatternID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new event in the data source
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of a newly created record.
	//     rdwNewEventID - [out] EventID of a newly created record
	// Remarks:
	//     This method creates a data record corresponding with the
	//     specified event.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     TRUE if event created successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoCreate_Event(CXTPCalendarEvent* pEvent, DWORD& rdwNewEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates event properties in the data source.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an event.
	// Remarks:
	//     This method updates a data record corresponding with the
	//     specified event.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     TRUE if an event updated successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoUpdate_Event(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes an event from the data source.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an event.
	// Remarks:
	//     This method deletes a data record corresponding with the
	//     specified event.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     TRUE if an event deleted successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoDelete_Event(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new recurrence pattern in the data source
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object which
	//              describes all data fields of a newly created record.
	//     rdwNewPatternID - [out] ID of a newly created record
	// Remarks:
	//     This method creates a data record corresponding with the
	//     specified recurrence pattern.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     TRUE if recurrence pattern created successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoCreate_RPattern(CXTPCalendarRecurrencePattern* pPattern, DWORD& rdwNewPatternID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates a recurrence pattern in the database
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object.
	// Remarks:
	//     This method updates a data record corresponding with the
	//     specified recurrence pattern.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     TRUE if recurrence pattern updated successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoUpdate_RPattern(CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes a recurrence pattern from the database
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object
	//              which should be deleted.
	// Remarks:
	//     This method deletes a data record corresponding with the
	//     specified recurrence pattern.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     TRUE if recurrence pattern deleted successfully,
	//     FALSE in case of any error during the process.
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
	//     This method is used by a reminders manager.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     upcoming events.
	// See Also:
	//     CXTPCalendarRemindersManager overview
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoGetUpcomingEvents(COleDateTime dtFrom, COleDateTimeSpan spPeriod);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves all events without generating
	//     recurrence occurrences.
	// Remarks:
	//     Simple events are included as is.
	//     For the recurrence events only master events are included,
	//     recurrence occurrences are not generated.
	//     It has empty implementation in this class and should be
	//     implemented by any custom realization of CXTPCalendarData.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     all events.
	// See Also: GetAllEvents_raw()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoGetAllEvents_raw();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//    Sends a notification to all listeners.
	// Parameters:
	//    EventCode - Notification event code.
	//    wParam    - First notification parameter.
	//    lParam    - Second notification parameter.
	// See Also:
	//    CXTPNotifyConnection overview
	//-----------------------------------------------------------------------
	virtual void SendNotification(XTP_NOTIFY_CODE EventCode, WPARAM wParam , LPARAM lParam);

	//{{AFX_CODEJOCK_PRIVATE
	virtual BOOL _AddRPatternWithExceptions(CXTPCalendarRecurrencePattern* pPattern);
	virtual BOOL _RemoveRPatternWithExceptions(CXTPCalendarRecurrencePattern* pPattern);
	virtual BOOL _ChangeRPatternWithExceptions(CXTPCalendarRecurrencePattern* pRecurrencePattern);

	virtual BOOL _ChangeRExceptionOccurrence_nf(CXTPCalendarEvent* pExcOcc, BOOL bSend_EventDeleted);

	virtual void _PostProcessOccurrencesFromMaster(COleDateTime dtDay, CXTPCalendarEvents* pEvents);
	virtual void _PostProcessOccurrencesFromMaster2(COleDateTime dtDayFrom, COleDateTime dtDayTo, CXTPCalendarEvents* pEvents);

	virtual BOOL _PostProcessRecurrenceIfNeed(CXTPCalendarEvent* pEvent);
	virtual void _PostProcessRecurrenceIfNeed(CXTPCalendarEvents* pEvents);

	virtual void _FilterDayEventsInstancesByEndTime(COleDateTime dtDay, CXTPCalendarEvents* pEvents);

	virtual CXTPCalendarRecurrencePatternPtr _GetRecurrencePattern_raw(DWORD dwPatternID);
	virtual CXTPCalendarEventPtr _GetEvent_raw(DWORD dwEventID);


	//}}AFX_CODEJOCK_PRIVATE

protected:
	BOOL m_bOpened; // Stores a flag whether a data source connection is open

	CXTPNotifyConnection* m_pConnect;    // Connection object to send notifications.
	BOOL m_bDisableNotificationsSending; // Stores a flag whether notifications sending is disabled.

	CXTPCalendarEventLabels* m_pLabelList; // Event labels list.
	CXTPCalendarSchedules*   m_pSchedules; // Data source Schedules list

	CXTPCalendarCustomProperties* m_pCustomProperties; //Custom properties collection object.
protected:
	CString m_strConnectionString;         // default connection string;

	XTPCalendarDataProvider m_typeProvider;// Data provider type.

	//-----------------------------------------------------------------------
	// Remarks:
	//     Implementation of the data provider cache.
	//-----------------------------------------------------------------------
	class _XTP_EXT_CLASS CXTPDPCache
	{
	public:
		//-------------------------------------------------------------------
		// Summary:
		//     Default class constructor.
		//-------------------------------------------------------------------
		CXTPDPCache();

		//-------------------------------------------------------------------
		// Summary:
		//     Default class destructor.
		//-------------------------------------------------------------------
		virtual ~CXTPDPCache();

		//-------------------------------------------------------------------
		// Summary:
		//     Closes a cache.
		//-------------------------------------------------------------------
		void SafeClose();

		//-------------------------------------------------------------------
		// Summary:
		//     Safely removes all elements from cache.
		//-------------------------------------------------------------------
		void SafeRemoveAll();

		//-------------------------------------------------------------------
		// Summary:
		//     Checks whether events for this day are already stored in cache.
		// Parameters:
		//     dtDay - A day to check.
		// Returns:
		//     TRUE if day is already stored in cache,
		//     FALSE otherwise.
		//-------------------------------------------------------------------
		virtual BOOL IsDayInCache(COleDateTime dtDay);

		//-------------------------------------------------------------------
		// Summary:
		//     Sets a flag that this day is in cache.
		// Parameters:
		//     dtDay - A day to set.
		//     bSet  - Whether to set or clear flag.
		//-------------------------------------------------------------------
		virtual void SetDayInCache(COleDateTime dtDay, BOOL bSet = TRUE);

		//-------------------------------------------------------------------
		// Summary:
		//     Checks whether a specified event is already stored in cache.
		// Parameters:
		//     pEvent - An event to check.
		// Returns:
		//     TRUE if an event is already stored in cache,
		//     FALSE otherwise.
		//-------------------------------------------------------------------
		virtual BOOL IsEventInCache(CXTPCalendarEvent* pEvent);

		//-------------------------------------------------------------------
		// Summary:
		//     Adds a collection if events to the cache for a specified day.
		// Parameters:
		//     pEvents - An events collection.
		//     dtDay   - A specified day.
		//-------------------------------------------------------------------
		virtual void AddToCache(CXTPCalendarEvents* pEvents, COleDateTime dtDay);

		//-------------------------------------------------------------------
		// Summary:
		//     Checks whether a specified event is already stored in cache
		//     and adds it there if not.
		// Parameters:
		//     pEvent - An event to check / add.
		//-------------------------------------------------------------------
		virtual void AddToCacheIfNeed(CXTPCalendarEvent* pEvent);

		//-------------------------------------------------------------------
		// Summary:
		//     Checks whether a specified recurrence pattern is already
		//     stored in cache and adds it there if not.
		// Parameters:
		//     pPattern - An pattern to check / add.
		//-------------------------------------------------------------------
		virtual void _AddToCacheIfNeed(CXTPCalendarRecurrencePattern* pPattern);

		CXTPCalendarData* m_pCacheDP;   // Cache Data Provider.
		int               m_eCacheMode; // see XTPCalendarDataProviderCacheMode

	protected:
		CMap<DWORD, DWORD, BOOL, BOOL> m_mapIsDayInCache; // whether events for this day are already stored in cache
	};

protected:
	CXTPDPCache m_cache; // Common data provider cache.

	//-----------------------------------------------------------------------
	// Remarks:
	//     This template class is used as a map (see CMap) to store
	//     ID and object pairs. Using the map allows for quick access
	//     using the ID.
	// See Also: CMap
	//-----------------------------------------------------------------------
	template<class _TObject>
	class EventsMapByID_T
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Default class constructor.
		// Parameters:
		//     uHashTableSize - Size of the map hash table.
		// See Also: CMap, CMap::InitHashTable
		//-----------------------------------------------------------------------
		EventsMapByID_T(UINT uHashTableSize = XTP_OBJECT_CACHE_HASH_TABLE_SIZE)
		{
			m_mapID2Object.InitHashTable(uHashTableSize, FALSE);
		};

		//-----------------------------------------------------------------------
		// Summary:
		//     Default class destructor.
		// Remarks:
		//     Handles member items deallocation. Decreases references of
		//     stored objects.
		// See Also: CMap
		//-----------------------------------------------------------------------
		virtual ~EventsMapByID_T() {};

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to add new or to replace existing
		//     pairs of IDs and Objects.
		// Parameters:
		//     dwID - Object ID.
		//     pObj - Object Pointer.
		// Remarks:
		//     Call this method to add new or to replace existing
		//     elements in the map. InternalAddRef is called for the
		//     stored object.
		// See Also: Get, Remove, CMap::SetAt
		//-----------------------------------------------------------------------
		void Add(DWORD dwID, _TObject* pObj)
		{
			if (!pObj) {
				ASSERT(FALSE);
				return;
			}
			TObjPtr ptrObj(pObj, TRUE);
			m_mapID2Object.SetAt(dwID, ptrObj);
		};

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to obtain a pointer to an object
		//     using the specified ID.
		// Parameters:
		//     dwID - Object ID.
		// Remarks:
		//     Call this method to search for the object using the ID.
		//     InternalAddRef is called for the non-NULL returned object.
		// Returns:
		//     Object pointer or NULL.
		// See Also: Add, CMap::Lookup
		//-----------------------------------------------------------------------
		_TObject* Get(DWORD dwID) {
			TObjPtr ptrObj;
			if (m_mapID2Object.Lookup(dwID, ptrObj) && ptrObj) {
				return (_TObject*)(CCmdTarget*)ptrObj;
			}
			return NULL;
		};

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to determine if an object with the
		//     specified ID exists.
		// Parameters:
		//     dwID - Object ID.
		// Remarks:
		//     Similar to Get method, but calls InternalAddRef for
		//     the returned object.
		// Returns:
		//     TRUE if object with ID exists. Otherwise FALSE.
		// See Also: Get, CMap::Lookup
		//-----------------------------------------------------------------------
		BOOL IsExist(DWORD dwID) {
			TObjPtr ptrObj;
			return m_mapID2Object.Lookup(dwID, ptrObj) && ptrObj;
		};

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to remove an object with the
		//     specified ID.
		// Parameters:
		//     dwID - Object ID.
		// Remarks:
		//     Call this method to remove an element with the supplied ID
		//     from the map.
		//     InternalAddRef is called for the removed object.
		// Returns: Nonzero if the entry was found and successfully removed; otherwise 0.
		// See Also: Add, RemoveAll, CMap::RemoveKey
		//-----------------------------------------------------------------------
		BOOL Remove(DWORD dwID) {
			BOOL bRes = m_mapID2Object.RemoveKey(dwID);
			return bRes;
		};

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to remove all stored objects
		//     from the map.
		// Remarks:
		//     Call this method to cleanup the map.
		//     InternalAddRef is called for all objects.
		// See Also: Remove, CMap::RemoveAll
		//-----------------------------------------------------------------------
		void RemoveAll() {
			m_mapID2Object.RemoveAll();
		}

		//-----------------------------------------------------------------------
		// Summary:
		//     This member function is used to get start iteration position.
		// Returns: A POSITION value that indicates a starting position for
		//          iterating the map; or NULL if the map is empty.
		// See Also: GetNextElement, CMap::GetStartPosition
		//-----------------------------------------------------------------------
		POSITION GetStartPosition() {
			return m_mapID2Object.GetStartPosition();
		}

		//-----------------------------------------------------------------------
		// Summary:
		//      This member function is used to get next object pointer.
		// Parameters:
		//      rPos    - [in/out]  Iteration position of the element.
		//      rID     - [out]     Element ID.
		// Remarks:
		//      Call this method to iterate on elements stored in the map.
		//      InternalAddRef is not called for the returned object.
		//      rPos parameter value is set to the element next position
		//      or NULL if iterating is finished.
		// Returns:
		//      A pointer to the object which corresponds to the specified
		//      position (rPos parameter value).
		// See Also: GetStartPosition, CMap::GetNextAssoc
		//-----------------------------------------------------------------------
		_TObject* GetNextElement(POSITION& rPos, DWORD& rID)
		{
			TObjPtr ptrElement;
			m_mapID2Object.GetNextAssoc(rPos, rID, ptrElement);
			return (_TObject*)(CCmdTarget*)ptrElement;
		}

		//-----------------------------------------------------------------------
		// Summary:
		//      This member function is used to get object count.
		// Returns:
		//      Count of the objects in the map.
		// See Also: CMap::GetCount
		//-----------------------------------------------------------------------
		int GetCount()  {
			return (int)m_mapID2Object.GetCount();

		}

	protected:

		//typedef CXTPSmartPtrInternalT<_TObject> TObjPtr;       // Smart pointer object type.
		// to avoid warning C4786 CCmdTarget used.
		typedef CXTPSmartPtrInternalT<CCmdTarget> TObjPtr;       // Smart pointer object type.
		CMap<DWORD, DWORD, TObjPtr, TObjPtr&>  m_mapID2Object; // The map of ID to object.
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get a unique temporary event ID.
	// Returns:
	//     New value for event ID.
	//-----------------------------------------------------------------------
	static DWORD GetNextFreeTempID();

	static DWORD ms_dwNextFreeTempID; // Next unique event ID.

	CXTPCalendarOptions* m_pCalendarOptions;    // This member stores user's calendar view options.
private:
	//{{AFX_CODEJOCK_PRIVATE
	void SetOptionsToUpdate(CXTPCalendarOptions* pOptions);
	//}}AFX_CODEJOCK_PRIVATE
protected:

};

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE BOOL CXTPCalendarData::Open() {
	ASSERT(!m_bOpened);
	return m_bOpened = TRUE;
}

AFX_INLINE BOOL CXTPCalendarData::Create() {
	ASSERT(!m_bOpened);
	return m_bOpened = TRUE;
}

AFX_INLINE BOOL CXTPCalendarData::IsOpen() {
	return m_bOpened;
}

AFX_INLINE BOOL CXTPCalendarData::Save() {
	ASSERT(m_bOpened);
	return TRUE;
}

AFX_INLINE void CXTPCalendarData::Close() {
	ASSERT(m_bOpened);

	m_cache.SafeClose();
	m_bOpened = FALSE;
}

AFX_INLINE CXTPNotifyConnection* CXTPCalendarData::GetConnection() {
	return m_pConnect;
}

AFX_INLINE DWORD CXTPCalendarData::GetNextFreeTempID() {
	return ms_dwNextFreeTempID--;
}

AFX_INLINE CXTPCalendarEventLabels* CXTPCalendarData::GetLabelList() const {
	return m_pLabelList;
}

AFX_INLINE CXTPCalendarSchedules* CXTPCalendarData::GetSchedules() const {
	return m_pSchedules;
}


AFX_INLINE void CXTPCalendarData::SetConnectionString(LPCTSTR lpszConnectionString) {
	ASSERT(lpszConnectionString);
	if (lpszConnectionString) {
		m_strConnectionString = lpszConnectionString;
	}
}

AFX_INLINE CString CXTPCalendarData::GetConnectionString() {
	return m_strConnectionString;
}

AFX_INLINE XTPCalendarDataProvider CXTPCalendarData::GetType() {
	return m_typeProvider;
}

AFX_INLINE int CXTPCalendarData::GetCacheMode() const {
	return m_cache.m_eCacheMode;
}

AFX_INLINE CXTPCalendarCustomProperties* CXTPCalendarData::GetCustomProperties() {
	return m_pCustomProperties;
}
#endif // !defined(_XTPCALENDARDATA_H__)
