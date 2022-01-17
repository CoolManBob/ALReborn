// XTPCalendarCustomDataProvider.h: interface for the CXTPCalendarCustomProperties class.
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
#if !defined(_XTP_CALENDAR_CUSTOM_DATA_PROVIDER_H__)
#define _XTP_CALENDAR_CUSTOM_DATA_PROVIDER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendardata.h"

//===========================================================================
// Summary:
//     Custom implementation of XTPCalendarData based on notifications.
// Remarks:
//     This class is the implementation of the XTPCalendarData abstract
//     class which represents a data portion of the Calendar control.
//
//     This implementation is based on a notifications mechanism.
//     Each significant method sends a custom notification, which could be
//     handled by a user and where a custom data access code should be
//     implemented.
// See Also:
//     CXTPCalendarData overview
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarCustomDataProvider : public CXTPCalendarData
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarCustomDataProvider)
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
	CXTPCalendarCustomDataProvider();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default Destructor.
	// Remarks:
	//     Handles all deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarCustomDataProvider();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to close the connection to a data source.
	//-----------------------------------------------------------------------
	virtual void Close();

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
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoRetrieveDayEvents which a user can handle and
	//     implement a custom method.
	// Returns:
	//     A collection of events for a specified day.
	// See also:
	//     XTP_NC_CALENDAR_DoRetrieveDayEvents notification
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr   DoRetrieveDayEvents(COleDateTime dtDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all events from the data source.
	// Remarks:
	//     This method removes all appointments from the data source.
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoRemoveAllEvents which a user can handle and
	//     implement a custom method.
	// See also:
	//     XTP_NC_CALENDAR_DoRemoveAllEvents notification
	//-----------------------------------------------------------------------
	virtual void                    DoRemoveAllEvents();

	//-----------------------------------------------------------------------
	// Summary:
	//     Read event from the data source.
	// Parameters:
	//     dwEventID - Event ID of the read target.
	// Remarks:
	//     This method retrieves all data for a specified event.
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoRead_Event which a user can handle and
	//     implement a custom method.
	// Returns:
	//     A pointer to the new CXTPCalendarEvent object which fields
	//     are filled with a data from the data source.
	// See also:
	//     XTP_NC_CALENDAR_DoRead_Event notification
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr                DoRead_Event(DWORD dwEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Read recurrence pattern from the data source
	// Parameters:
	//     dwPatternID - Pattern ID of the read target.
	// Remarks:
	//     This method retrieves all data for a specified recurrence pattern.
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoRead_RPattern which a user can handle and
	//     implement a custom method.
	// Returns:
	//     A pointer to the new CXTPCalendarRecurrencePatternPtr object which
	//     fields are filled with a data from the data source.
	// See also:
	//     XTP_NC_CALENDAR_DoRead_RPattern notification
	//-----------------------------------------------------------------------
	virtual CXTPCalendarRecurrencePatternPtr    DoRead_RPattern(DWORD dwPatternID);

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
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoCreate_Event which a user can handle and
	//     implement a custom method.
	// Returns:
	//     TRUE if event created successfully,
	//     FALSE in case of any error during the process.
	// See also:
	//     XTP_NC_CALENDAR_DoCreate_Event notification
	//-----------------------------------------------------------------------
	virtual BOOL DoCreate_Event (CXTPCalendarEvent* pEvent, DWORD& rdwNewEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates event properties in the data source.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an event.
	// Remarks:
	//     This method updates a data record corresponding with the
	//     specified event.
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoUpdate_Event which a user can handle and
	//     implement a custom method.
	// Returns:
	//     TRUE if an event updated successfully,
	//     FALSE in case of any errors during the process.
	// See also:
	//     XTP_NC_CALENDAR_DoUpdate_Event notification
	//-----------------------------------------------------------------------
	virtual BOOL DoUpdate_Event (CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes an event from the data source.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an event.
	// Remarks:
	//     This method deletes a data record corresponding with the
	//     specified event.
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoDelete_Event which a user can handle and
	//     implement a custom method.
	// Returns:
	//     TRUE if an event deleted successfully,
	//     FALSE in case of any errors during the process.
	// See also:
	//     XTP_NC_CALENDAR_DoDelete_Event notification
	//-----------------------------------------------------------------------
	virtual BOOL DoDelete_Event (CXTPCalendarEvent* pEvent);

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
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoCreate_RPattern which a user can handle and
	//     implement a custom method.
	// Returns:
	//     TRUE if recurrence pattern created successfully,
	//     FALSE in case of any error during the process.
	// See also:
	//     XTP_NC_CALENDAR_DoCreate_RPattern notification
	//-----------------------------------------------------------------------
	virtual BOOL DoCreate_RPattern  (CXTPCalendarRecurrencePattern* pPattern, DWORD& rdwNewPatternID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates a recurrence pattern in the database
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object.
	// Remarks:
	//     This method updates a data record corresponding with the
	//     specified recurrence pattern.
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoUpdate_RPattern which a user can handle and
	//     implement a custom method.
	// Returns:
	//     TRUE if recurrence pattern updated successfully,
	//     FALSE in case of any error during the process.
	// See also:
	//     XTP_NC_CALENDAR_DoUpdate_RPattern notification
	//-----------------------------------------------------------------------
	virtual BOOL DoUpdate_RPattern  (CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes a recurrence pattern from the database
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object
	//              which should be deleted.
	// Remarks:
	//     This method deletes a data record corresponding with the
	//     specified recurrence pattern.
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoDelete_RPattern which a user can handle and
	//     implement a custom method.
	// Returns:
	//     TRUE if recurrence pattern deleted successfully,
	//     FALSE in case of any error during the process.
	// See also:
	//     XTP_NC_CALENDAR_DoDelete_RPattern notification
	//-----------------------------------------------------------------------
	virtual BOOL DoDelete_RPattern  (CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method implements Getting all events without generating
	//     recurrence occurrences.
	// Remarks:
	//     This method actually implements working with the data source.
	//     Simple events are included as is.
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoGetAllEvents_raw which a user can handle and
	//     implement a custom method.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     all events.
	// See also:
	//     XTP_NC_CALENDAR_DoGetAllEvents_raw notification, GetAllEvents_raw()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoGetAllEvents_raw();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method implements getting a collection of events expiring in
	//     the period from dtFrom during next spPeriod.
	// Parameters:
	//     dtFrom   - Start date and time of a specified period.
	//     spPeriod - A duration of a specified period.
	// Remarks:
	//     This method is used by a reminders manager.
	//
	//     This implementation of the method sends a notification
	//     XTP_NC_CALENDAR_DoGetUpcomingEvents which a user can handle and
	//     implement a custom method.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     upcoming events.
	// See Also:
	//     CXTPCalendarRemindersManager overview,
	//     XTP_NC_CALENDAR_DoGetUpcomingEvents notification.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoGetUpcomingEvents(COleDateTime dtFrom, COleDateTimeSpan spPeriod);
};

///////////////////////////////////////////////////////////////////
#endif // !defined(_XTP_CALENDAR_CUSTOM_DATA_PROVIDER_H__)
