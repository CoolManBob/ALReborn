// XTPCalendarMemoryDataProvider.h: interface for the CXTPCalendarMemoryDataProvider class.
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
#if !defined(_XTPCALENDARMEMORYDATAPROVIDER_H__)
#define _XTPCALENDARMEMORYDATAPROVIDER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE

#include "XTPCalendarData.h"

/////////////////////////////////////////////////////////////////////////////
class CXTPCalendarEvent;
class CXTPCalendarRecurrencePattern;

//===========================================================================
// Summary:
//     Define events storage hash table size. Should be a prime number!
//     like: 503, 1021, 1511, 2003, 3001, 4001, 5003, 6007, 8009, 12007,
//     16001, 32003, 48017, 64007
// See Also: CXTPCalendarData::EventsMapByID_T overview, CMap overview
//===========================================================================
#define XTP_EVENTS_STORAGE_HASH_TABLE_SIZE 4001

//===========================================================================
// Summary:
//     This class implements an abstract representation of the data used
//     in the XTPCalendarData class.
// Remarks:
//     This class is the implementation of the XTPCalendarData abstract
//     class which represents a data portion of the Calendar control.
//
//          This class provides a memory storage implementation of the
//          CXTPCalendarData class. It stores all elements using standard
//          CXTPCalendarEvents array collection and provides fast search
//          on it using search tree algorithms.
//
//          To use CXTPCalendarMemoryDataProvider you first construct an instance
//          of CXTPCalendarDatabaseDataProvider object then use Open() member function
//          to establish connection to the data source and initialize the object.
//          Class provides a number of functions to manipulate events and events
//          related data. After finishing working with data source you should call
//          Close() member function to close connection, de-initialize the object
//          and free unused resources.
// See Also: CXTPCalendarData overview, CXTPCalendarEvents, CXTPCalendarEvent
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarMemoryDataProvider : public CXTPCalendarData
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarMemoryDataProvider)
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
	CXTPCalendarMemoryDataProvider();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default Destructor.
	// Remarks:
	//     Handles all deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarMemoryDataProvider();

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
	//     Call this member function to create a file for storing memory
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
	//     the same file as before.
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
	//     Call this member function to get a collection of events for the specified date range.
	// Parameters:
	//     dtStartDay - A COleDateTime object without the time portion.
	//     dtEndDay - A COleDateTime object without the time portion.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of day events.
	// See Also: CXTPCalendarEvents
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr RetrieveEvents(COleDateTime dtStartDay, COleDateTime dtEndDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes data from or to an archive.
	// Parameters:
	//     ar - A CArchive object to serialize to or from.
	// Remarks:
	//     For the save case the most recent data from the data provider
	//     will be saved to archive.
	//     For the load case existing data is not removed from the data
	//     provider and a new data will be loaded from the specified
	//     archive and added to the existing data set.
	//     Call RemoveAllEvents() manually if you need a cleanup before load.
	//     For load and save operations _Load() and _Save() protected methods are used.
	//     Method can throw CArchiveException.
	// See Also: _Load(), _Save()
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes data from or to an storage.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	// Remarks:
	//     For the save case the most recent data from the data provider
	//     will be saved to archive.
	//     For the load case existing data is not removed from the data
	//     provider and a new data will be loaded from the specified
	//     archive and added to the existing data set.
	//     Call RemoveAllEvents() manually if you need a cleanup before load.
	//     For load and save operations _Load() and _Save() protected methods are used.
	//     Method can throw CArchiveException.
	// See Also: _Load(), _Save()
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads data from an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	// Remarks:
	//     The existing data is not removed from the data provider and
	//     a new data will be loaded from the specified archive and
	//     added to the existing data set.
	//     Call RemoveAllEvents() manually if you need a cleanup before load.
	//     Method can throw CArchiveException.
	// Returns:
	//     TRUE is successful. FALSE otherwise.
	// See Also: _Save(), Serialize()
	//-----------------------------------------------------------------------
	virtual BOOL _Load(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method writes data to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	// Remarks:
	//     The most recent data from the data provider will be saved to
	//     archive.
	//     Method can throw CArchiveException.
	// Returns:
	//     TRUE is successful. FALSE otherwise.
	// See Also: _Load(), Serialize()
	//-----------------------------------------------------------------------
	virtual BOOL _Save(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves day events for a specified day from the memory storage.
	// Parameters:
	//     dtDay - A specified day.
	// Remarks:
	//     This method retrieves all appointments for a specified day from
	//     the memory storage. It includes recurrence appointments occurrences,
	//     regular appointments for this day, multi day appointments which
	//     also have a part of this day.
	// Returns:
	//     A collection of events for a specified day.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr   DoRetrieveDayEvents(COleDateTime dtDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all events from the memory storage.
	// Remarks:
	//     This method removes all appointments from the memory storage.
	//-----------------------------------------------------------------------
	virtual void                    DoRemoveAllEvents();

	//-----------------------------------------------------------------------
	// Summary:
	//     Read event from the memory storage.
	// Parameters:
	//     dwEventID - Event ID of the read target.
	// Remarks:
	//     This method retrieves all data for a specified event.
	// Returns:
	//     A pointer to the new CXTPCalendarEvent object which fields
	//     are filled with a data from the corresponding memory storage.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr                DoRead_Event(DWORD dwEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Read recurrence pattern from the memory storage
	// Parameters:
	//     dwPatternID - Pattern ID of the read target.
	// Remarks:
	//     This method retrieves all data for a specified recurrence pattern.
	// Returns:
	//     A pointer to the new CXTPCalendarRecurrencePatternPtr object which
	//     fields are filled with a data from the corresponding memory storage.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarRecurrencePatternPtr    DoRead_RPattern(DWORD dwPatternID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new event in the memory storage
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of a newly created record.
	//     rdwNewEventID - [out] EventID of a newly created record
	// Remarks:
	//     This method creates a memory data record corresponding with the
	//     specified event.
	// Returns:
	//     TRUE if event created successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoCreate_Event (CXTPCalendarEvent* pEvent, DWORD& rdwNewEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates event properties in the memory storage.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an event.
	// Remarks:
	//     This method updates a memory data record corresponding with the
	//     specified event.
	// Returns:
	//     TRUE if an event updated successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoUpdate_Event (CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes an event from the memory storage.
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarEvent object which describes
	//              all data fields of an event.
	// Remarks:
	//     This method deletes a memory data record corresponding with the
	//     specified event.
	// Returns:
	//     TRUE if an event deleted successfully,
	//     FALSE in case of any errors during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoDelete_Event (CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new recurrence pattern in the memory storage
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object which
	//              describes all data fields of a newly created memory record.
	//     rdwNewPatternID - [out] ID of a newly created memory record
	// Remarks:
	//     This method creates a memory data record corresponding with the
	//     specified recurrence pattern.
	// Returns:
	//     TRUE if recurrence pattern created successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoCreate_RPattern  (CXTPCalendarRecurrencePattern* pPattern, DWORD& rdwNewPatternID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates a recurrence pattern in the memory storage
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object.
	// Remarks:
	//     This method updates a memory data record corresponding with the
	//     specified recurrence pattern.
	// Returns:
	//     TRUE if recurrence pattern updated successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoUpdate_RPattern  (CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     Deletes a recurrence pattern from the memory storage
	// Parameters:
	//     pEvent - Pointer to the CXTPCalendarRecurrencePattern object
	//              which should be deleted.
	// Remarks:
	//     This method deletes a memory data record corresponding with the
	//     specified recurrence pattern.
	// Returns:
	//     TRUE if recurrence pattern deleted successfully,
	//     FALSE in case of any error during the process.
	//-----------------------------------------------------------------------
	virtual BOOL DoDelete_RPattern  (CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method implements Getting all events from the memory storage
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
	//     This method returns a collection of upcoming events.
	//     It is used by a reminders manager.
	// Returns:
	//     A CXTPCalendarEvents pointer containing a collection of
	//     upcoming events.
	// See Also:
	//     CXTPCalendarRemindersManager overview
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr DoGetUpcomingEvents(COleDateTime dtFrom, COleDateTimeSpan spPeriod);

//{{AFX_CODEJOCK_PRIVATE
private:
	typedef CMap<DWORD, DWORD, CXTPCalendarEvent*, CXTPCalendarEvent*> CMapIDtoEvent;

	// Digital Search Tree Node
	struct DSTNode {

		DSTNode();
		~DSTNode();

		DSTNode* pLeft;
		DSTNode* pRight;

		void Mark(CXTPCalendarEvent* pEvent);
		void Unmark(CXTPCalendarEvent* pEvent);
		BOOL IsMarked(CXTPCalendarEvent* pEvent);

		void AppendAll(CMapIDtoEvent* pMapEvents);

	private:
		typedef CMap<CXTPCalendarEvent*,CXTPCalendarEvent*, BOOL, BOOL> EventsMap;
		EventsMap* m_pMapEvents;
	};
	// Digital Search Tree
	struct DST
	{
		DST();
		~DST();

		void Insert(CXTPCalendarEvent* pEvent);
		void Remove(CXTPCalendarEvent* pEvent);

		void Clear();

		void SearchForEvents(DWORD dwStart, DWORD dwEnd, CXTPCalendarEvents* pEvents);
	private:
		// properties
		DSTNode* pHead;

		// temp data storage
		CMapIDtoEvent tmp_mapEvents;
	private:
		enum DSTScanType {
			dstScanMark,
			dstScanUnmark,
			dstScanFind
		};
		// methods
		void ScanRange(DSTNode* const pNode, const int nDepth,
			const DWORD dwMinValue, const DWORD dwMaxValue,
			const DWORD dwStart, const DWORD dwEnd,
			CXTPCalendarEvent* const pEvent, CMapIDtoEvent* pMapEvents,
			const DSTScanType eScanType);

		void MarkRange(CXTPCalendarEvent* pEvent, const DSTScanType eScanType);
	};

private:
	BOOL IsProviderXML(BOOL bCheckXMLsupport, BOOL* pbXMLsupportError = NULL);
	CString GetXMLEncoding();

	DWORD _UniqueID_Event(DWORD dwID);
	DWORD _UniqueID_Patern(DWORD dwID);
private:
	EventsMapByID_T<CXTPCalendarEvent>              m_EventsStorage;
	EventsMapByID_T<CXTPCalendarRecurrencePattern>  m_mapPatterns;

	DST m_tree;

	//---------------------------------------------------------------------------
	CMap<DWORD, DWORD, BOOL, BOOL>  m_mapIsDayInCache;
//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(_XTPCALENDARMEMORYDATAPROVIDER_H__)
