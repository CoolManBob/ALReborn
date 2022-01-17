// XTPCalendarDatabaseDataProvider.h: interface for
//                          the CXTPCalendarDatabaseDataProvider class.
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
#if !defined(_XTPCALENDARDATABASEDATAPROVIDER_H__)
#define _XTPCALENDARDATABASEDATAPROVIDER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "XTPCalendarData.h"
#include "XTPCalendarEvents.h"
#include "XTPCalendarEvent.h"
#include "XTPCalendarRecurrencePattern.h"
#include "XTPCalendarMemoryDataProvider.h"
#include "XTPCalendarDefines.h"
#include "XTPCalendarADO.h"

class CXTPCalendarSchedule;
class CXTPCalendarSchedules;

//===========================================================================
// Summary:
//     This class is the implementation of the XTPCalendarData abstract
//     class which represents the data portion of the Calendar control.
// Remarks:
//     This implementation uses MS Access database as the data source.
//     The Data source stores all Events data and provides a way to retrieve
//     and save CXTPCalendarEvent objects.
//
//          To use CXTPCalendarDatabaseDataProvider first construct an instance
//          of CXTPCalendarDatabaseDataProvider object then use Open() member function
//          to establish connection to the data source and initialize the object.
//          Class provides a number of functions to manipulate events and events
//          related data. After finished working with the data source must call
//          the Close() member function to close the connection and to de-initialize
//          the object and free any unused resources.
// See Also: CXTPCalendarData, CXTPCalendarMemoryDataProvider
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarDatabaseDataProvider : public CXTPCalendarData
{
	//{{AFX_CODEJOCK_PRIVATE
	class CADOCommand;

	DECLARE_DYNAMIC(CXTPCalendarDatabaseDataProvider)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	// Remarks:
	//     Constructs a Data object. To establish a connection to a
	//     specified data source and initialize the object the
	//     Open() member function must be called immediately after
	//     construction.
	// See Also: Open()
	//-----------------------------------------------------------------------
	CXTPCalendarDatabaseDataProvider();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default Destructor.
	// Remarks:
	//     Handles all deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarDatabaseDataProvider();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to initialize a newly constructed
	//     Data object.
	// Remarks:
	//     Data object must be created before you call Open.
	// Returns:
	//     TRUE is successful. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL Open();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to initialize data provider using existing
	//     ADO Connection object.
	// Parameters:
	//     pdispADOConnection - [in] An IDispatch interface of ADO Connection.
	// Remarks:
	//     Set Connection String as "Provider=ADO;" (or "Provider=Access;")
	//     Connection object must be opened before pass to OpenEx.
	//     Also calendar data provider do not close ADO Connection on Close method.
	//     You have to close it yourself.
	// Returns:
	//     TRUE is successful. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL OpenEx(LPDISPATCH pdispADOConnection);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to create an empty database.
	// Remarks:
	//     A database is created with the necessary tables structure.
	//     It is opened by default.
	// Returns:
	//     TRUE is successful. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL Create();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method saves data to the database.
	// Remarks:
	//     Actually, this method just always returns TRUE for the
	//     database data provider. The reason is that database data provider
	//     always keep consistency of its data and could be closed without
	//     saving. The database file would contain the most recent data.
	// Returns:
	//     TRUE is successful. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL Save();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to close the connection to a data source.
	//-----------------------------------------------------------------------
	virtual void Close();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieve day events for a specified day.
	// Parameters:
	//     dtDay - A specified day.
	// Remarks:
	//     This method creates and executes a database query on order to
	//     retrieve all appointments for a specified day.
	// Returns:
	//     A collection of events for a specified day.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoRetrieveDayEvents(COleDateTime dtDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Remove all events from the database.
	// Remarks:
	//     This method creates and executes a database query which
	//     removes all appointments from the database.
	//-----------------------------------------------------------------------
	virtual void DoRemoveAllEvents();

	//-----------------------------------------------------------------------
	// Summary:
	//     Read event from the database.
	// Parameters:
	//     dwEventID - Event ID of the read target.
	// Remarks:
	//     This method creates and executes a database query which
	//     retrieves all data for a specified event.
	// Returns:
	//     A pointer to the new CXTPCalendarEvent object which fields
	//     contains a data from the database.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr DoRead_Event(DWORD dwEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Read recurrence pattern from the database
	// Parameters:
	//     dwPatternID - Pattern ID of the read target.
	// Remarks:
	//     This method creates and executes a database query which
	//     retrieves all data for a specified recurrence pattern.
	// Returns:
	//     A pointer to the created CXTPCalendarRecurrencePattern object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarRecurrencePatternPtr DoRead_RPattern(DWORD dwPatternID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new event in the database
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of a newly created record.
	//     rdwNewEventID - [out] EventID of a newly created record,
	//                     it is an auto-generated numeric events table key.
	// Remarks:
	//     This method creates and executes a database query which
	//     adds a new record to the events table.
	// Returns:
	//     TRUE if event created successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoCreate_Event(CXTPCalendarEvent* pEvent, DWORD& rdwNewEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates event properties in the database.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an event.
	// Remarks:
	//     This method creates and executes a database query which updates
	//     a corresponding database record with properties from the specified
	//     event.
	// Returns:
	//     TRUE if an event updated successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoUpdate_Event(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes an event from the database.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an event.
	// Remarks:
	//     This method creates and executes a database query which deletes
	//     a corresponding database record.
	// Returns:
	//     TRUE if an event deleted successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoDelete_Event(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new recurrence pattern in the database
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object which
	//              describes all data fields of a newly created record.
	//     rdwNewPatternID - [out] ID of a newly created record,
	//                       it is an auto-generated numeric table key.
	// Remarks:
	//     This method creates and executes a database query which
	//     adds a new record to the recurrence patterns table.
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
	//     This method creates and executes a database query which
	//     updates a corresponding record in the recurrence patterns table.
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
	//     This method creates and executes a database query which
	//     deletes a corresponding record from the recurrence patterns table.
	// Returns:
	//     TRUE if recurrence pattern deleted successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoDelete_RPattern(CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method implements Getting all events from the database
	//     without generating recurrence occurrences.
	// Remarks:
	//     Simple events are included as is.
	//     For the recurrence events only master events are included,
	//     recurrence occurrences are not generated.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     all events.
	// See Also: GetAllEvents_raw()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoGetAllEvents_raw();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a collection of events expiring in the period from
	//     dtFrom during next spPeriod.
	// Parameters:
	//     dtFrom   - Start date and time of a specified period.
	//     spPeriod - A duration of a specified period.
	// Remarks:
	//     This method creates and executes a database query which
	//     returns a collection of upcoming events.
	//     It is used by a reminders manager.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     upcoming events.
	// See Also:
	//     CXTPCalendarRemindersManager overview
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoGetUpcomingEvents(COleDateTime dtFrom, COleDateTimeSpan spPeriod);

public:

	//{{AFX_CODEJOCK_PRIVATE
	static void AFX_CDECL TRACE_ProviderError(XTPADODB::_Connection* pConnDB);
	static void AFX_CDECL TRACE_ComError(_com_error &e);
	//}}AFX_CODEJOCK_PRIVATE

private:
	virtual CXTPCalendarEventPtr _ReadEvent_common(XTPADODB::_Recordset* pRS, BOOL bEcxception);

	virtual BOOL _ReadRExceptions(CXTPCalendarRecurrencePattern* pPattern);

	virtual BOOL _GetRPatternOptions(XTPADODB::_Recordset* pRS, XTP_CALENDAR_RECURRENCE_OPTIONS& rROptions);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to update options data in the database.
	// Parameters:
	//     strOptionsData - Options data in XML format.
	// Returns:
	//     TRUE if successful. Otherwise FALSE.
	// See Also: LoadOptions
	//-----------------------------------------------------------------------
	virtual BOOL SaveOptions(const CString& strOptionsData);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to load options data from the database.
	// Returns:
	//     TRUE if successful. Otherwise FALSE.
	// See Also: SaveOptions
	//-----------------------------------------------------------------------
	virtual BOOL LoadOptions();

	virtual CXTPCalendarSchedulesPtr LoadSchedules();
	virtual void UpdateSchedules();

	virtual void _AddSchedules(CXTPCalendarSchedules* pAddSet);
	virtual void _UpdateSchedules(CXTPCalendarSchedules* pUpdateSet, CXTPCalendarSchedules* pOrigSet);
	virtual void _DeleteSchedules(CUIntArray* pDeleteIDs);

	virtual CXTPCalendarSchedule* _ReadSchedule(XTPADODB::_Recordset* pRS);

protected:
	BOOL m_bTraceOptions; // Indicates whether there should be only one record
	                      // in the options table, or every save should add the new one.

private:
//{{AFX_CODEJOCK_PRIVATE
	BOOL _Open();

	void CreateEventTable(XTPADOX::_CatalogPtr ptrCatalog);
	void CreateRPatternTable(XTPADOX::_CatalogPtr ptrCatalog);
	void CreateOptionsTable(XTPADOX::_CatalogPtr ptrCatalog);
	void CreateSchedulesTable(XTPADOX::_CatalogPtr ptrCatalog);

	BOOL IsTableExist(XTPADOX::_CatalogPtr ptrCatalog, LPCWSTR pwszTableName);

	//void CreateDicTable(XTPADOX::_CatalogPtr ptrCatalog, LPCWSTR strTable,
	//                  LPCWSTR strNameID, bool bAuto = true);

	BOOL UpdateDBStructure(XTPADODB::_Connection* pconnDb);
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
	// members

	XTPADODB::_ConnectionPtr m_pconnDb;
	BOOL                     m_bCloseDbConnection;

	CADOCommand*  m_pcmdGetLastID;

	CADOCommand*  m_pcmdAddEvent;
	CADOCommand*  m_pcmdDelEvent;
	CADOCommand*  m_pcmdUpdEvent;

	CADOCommand*  m_pcmdGetDayEvents;
	CADOCommand*  m_pcmdGetRExceptions;
	CADOCommand*  m_pcmdGetEvent;
	CADOCommand*  m_pcmdUpdEventPatternID;

	CADOCommand*  m_pcmdAddRPattern;
	CADOCommand*  m_pcmdDelRPattern;
	CADOCommand*  m_pcmdUpdRPattern;
	CADOCommand*  m_pcmdGetRPattern;

	CADOCommand*  m_pcmdRemoveAllEvents;
	CADOCommand*  m_pcmdRemoveAllRPatterns;

	CADOCommand*  m_pcmdRemoveAllOptions;
	CADOCommand*  m_pcmdGetOptions;
	CADOCommand*  m_pcmdAddOptions;

	CADOCommand*  m_pcmdAddSchedule;
	CADOCommand*  m_pcmdUpdSchedule;
//}}AFX_CODEJOCK_PRIVATE
private:
	// internal only used member functions

//{{AFX_CODEJOCK_PRIVATE
	virtual BOOL _SetRPatternOptions(CADOCommand* pCmd, CXTPCalendarRecurrencePattern* pPattern);

	void DeleteAllDBCommands();

	void CreateGetLastIDCommand();
	void CreateAddEventCommand();
	void CreateDelEventCommand();
	void CreateUpdEventCommand();
	void CreateGetDayEventCommand();

	void CreateAddRPatternCommand();
	void CreateDelRPatternCommand();
	void CreateUpdRPatternCommand();
	void CreateGetRPatternCommand();
	void CreateGetRExceptionsCommand();
	void CreateGetEventCommand();
	void CreateUpdEventPatternIDCommand();

	void CreateRemoveAllCommands();

	void CreateGetOptionsCommand();
	void CreateAddOptionsCommand();

	void CreateAddScheduleCommand();
	void CreateUpdScheduleCommand();
//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(_XTPCALENDARDATABASEDATAPROVIDER_H__)
