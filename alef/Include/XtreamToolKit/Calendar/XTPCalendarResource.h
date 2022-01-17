// XTPCalendarResource.h: interface for the CXTPCalendarResource class.
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
#if !defined(_XTPCALENDARRESOURCE_H_)
#define _XTPCALENDARRESOURCE_H_
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPNotifyConnection.h"

#include "XTPCalendarDefines.h"
#include "XTPCalendarPtrCollectionT.h"
#include "XtpCalendarPtrs.h"

class CXTPCalendarControl;
class CXTPCalendarView;
class CXTPCalendarPaintManager;
class CXTPCalendarData;
class CXTPCalendarDayView;
class CXTPCalendarWeekView;
class CXTPCalendarMonthView;
class CXTPCalendarOptions;
class CXTPNotifyConnection;
class CXTPCalendarRemindersManager;
class CXTPCalendarCustomProperties;

//===========================================================================
// Summary:
//    Calendar Schedule is a characteristic of the Calendar Event, which
//    describes a belonging of the Event to a particular Schedule (which
//    could be people schedule, common resource schedule, etc).
//
//    This class implements base properties of the Schedule and operations
//    on it.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarSchedule : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarSchedule)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//     Construct CXTPCalendarSchedule object.
	// Parameters:
	//     uScheduleID      - A numeric positive ID of the schedule.
	//     pcszScheduleName - A string name of the schedule (could be a name
	//                        of the person, etc)
	// See Also: ~CXTPCalendarSchedule
	//-----------------------------------------------------------------------
	CXTPCalendarSchedule(UINT uScheduleID = 0, LPCTSTR pcszScheduleName = _T(""));

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	// See Also: CXTPCalendarSchedule
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarSchedule();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a unique schedule ID.
	// See Also: CXTPCalendarSchedule, SetID
	//-----------------------------------------------------------------------
	UINT GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the schedule name.
	// Returns:
	//     A CString object that contains the schedule name text.
	// See Also: SetName
	//-----------------------------------------------------------------------
	LPCTSTR GetName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set a new ID value for a schedule.
	// Parameters:
	//     uScheduleID - A DWORD that contains the id value.
	// See Also: GetID
	//-----------------------------------------------------------------------
	void SetID(UINT uScheduleID);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the schedule name text.
	// Parameters:
	//     pcszScheduleName - An LPCTSTR object that contains the string of text.
	// See Also: GetName
	//-----------------------------------------------------------------------
	void SetName(LPCTSTR pcszScheduleName);

	//-----------------------------------------------------------------------
	// Summary:
	//      Get custom (named, user defined) properties collection.
	// Returns:
	//      A pointer to CXTPCalendarCustomProperties object.
	// See Also:
	//      CXTPCalendarCustomProperties.
	//-----------------------------------------------------------------------
	CXTPCalendarCustomProperties* GetCustomProperties();

protected:
	UINT    m_uScheduleID;     // Stores numeric positive ID of the schedule
	CString m_strScheduleName; // Stores Schedule Name

	CXTPCalendarCustomProperties* m_pCustomProperties;  // Stores custom properties collection for this Schedule

protected:
};

//===========================================================================
// Summary:
//      This class represents a simple array collection of CXTPCalendarSchedule
//      objects.
// Remarks:
//      Array indexes always start at position 0.
//
//      As with a C array, the access time for indexed element of this
//      array is constant and is independent of the array size.
//
// See Also: CArray overview, CXTPCalendarSchedule overview.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarSchedules : public CXTPCalendarPtrCollectionT<CXTPCalendarSchedule>
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarSchedules)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection constructor.
	// See Also: ~CXTPCalendarSchedules()
	//-----------------------------------------------------------------------
	CXTPCalendarSchedules();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection destructor.
	// Remarks:
	//     Handles member item deallocation. Decreases reference of all
	//     stored CXTPCalendarSchedule objects.
	// See Also: RemoveAll()
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarSchedules();

	//-----------------------------------------------------------------------
	// Summary: Creates and adds a new schedule object by its name.
	// Parameters:
	//     pcszScheduleName - String with the schedule name.
	// Returns: TRUE if added successfully; FALSE otherwise.
	// See Also: RemoveSchedule
	//-----------------------------------------------------------------------
	virtual BOOL AddNewSchedule(LPCTSTR pcszScheduleName);

	//-----------------------------------------------------------------------
	// Summary: Removes a schedule object from the array by its ID.
	// Parameters:
	//     uScheduleID - An ID of the schedule.
	// Returns: TRUE if object was found and removed from the array;
	//          FALSE otherwise.
	// See Also: AddNewSchedule
	//-----------------------------------------------------------------------
	virtual BOOL RemoveSchedule(UINT uScheduleID);

	//-----------------------------------------------------------------------
	// Summary: Returns a name of the schedule by its numeric ID.
	// Parameters:
	//     uScheduleID - An ID of the schedule.
	// See Also: SetScheduleName
	//-----------------------------------------------------------------------
	virtual LPCTSTR GetScheduleName(UINT uScheduleID) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Finds schedule index in the internal array by its ID.
	// Parameters:
	//     uScheduleID - Label ID
	// Returns:
	//     Schedule index in the internal label array.
	//-----------------------------------------------------------------------
	virtual int FindIndex(UINT uScheduleID) const;

	//-----------------------------------------------------------------------
	// Summary: Sets a new schedule name by its numeric ID.
	// Parameters:
	//     uScheduleID - An ID of the schedule
	//     pcszNewName - String of text for the new name of the schedule
	// See Also: GetScheduleName
	//-----------------------------------------------------------------------
	virtual void SetScheduleName(UINT uScheduleID, LPCTSTR pcszNewName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load a schedule using the
	//     specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	// Remarks:
	//     This member function is used to store or load schedule data to or
	//     from a storage.
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

protected:

	//-----------------------------------------------------------------------
	// Summary: Finds next available ID which does not exist in the array.
	// Returns: Available ID which does not exist in the array.
	// Remarks: Function found max ID and returns it incremented by one.
	//-----------------------------------------------------------------------
	virtual UINT _GetNextID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Load a schedule from the
	//     specified data object.
	// Parameters:
	//     pPX - Source CXTPPropExchange data object reference.
	// Remarks:
	//     This member function is used to load schedule data
	//     from a specified storage.
	//-----------------------------------------------------------------------
	virtual void _Load(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store a schedule to the
	//     specified data object.
	// Parameters:
	//     pPX - Destination CXTPPropExchange data object reference.
	// Remarks:
	//     This member function is used to store schedule data to
	//     a storage.
	//-----------------------------------------------------------------------
	virtual void _Save(CXTPPropExchange* pPX);

};

//===========================================================================
// Summary:
//     Class CXTPCalendarResource represents a calendar for some subject like
//     a person, a shared resource, or even a group of persons or resources.
//
// Remarks:
//
//     Each resource is to be drawn in the separate column on the calendar view.
//     Each resource should have its own DataProvider. For example, events
//     for peoples could be stored inside the Memory data provider (somebody's
//     own or temporary events), and for other resources - inside a Database
//     data provider (those events are not changed by the user). So, there is
//     a possibility to use any combination of data providers in the same control.
//
// See Also: CXTPCalendarControl, CXTPCalendarData
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarResource : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNCREATE(CXTPCalendarResource)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//     Construct CXTPCalendarResource object.
	// Parameters:
	//     pCalendarCtrl - Pointer to the parent Calendar control.
	// See Also: ~CXTPCalendarResource
	//-----------------------------------------------------------------------
	CXTPCalendarResource(CXTPCalendarControl* pCalendarCtrl = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	// See Also: CXTPCalendarResource
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarResource();

	/////////////////////////////////////////////////////////////////////////
	// data provider related

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function obtains a pointer to the current data
	//     provider instance.
	// Remarks:
	//     Call this member function to obtain the data provider object
	//     currently used by this calendar control.
	// Returns:
	//     A CXTPCalendarData pointer to the associated data provider object.
	// See Also: CXTPCalendarData overview, SetDataProvider
	//-----------------------------------------------------------------------
	CXTPCalendarData* GetDataProvider();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the custom data provider for the control.
	// Parameters:
	//     pDataProvider - Pointer to the custom data provider object.
	//     lpszConnectionString - String containing the name and type of the data provider.
	// Remarks:
	//     Call this member function to set the custom data provider
	//     that is currently used by this calendar control. Note that
	//     custom data provider must be a descendant of CXTPCalendarData.
	// See Also: CXTPCalendarData overview, GetDataProvider
	//-----------------------------------------------------------------------
	void SetDataProvider(CXTPCalendarData* pDataProvider, BOOL bCloseDataProviderWhenDestroy = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the collection of the day's
	//     events for this resource. All events from the applied schedules
	//     will be included.
	// Parameters:
	//     dtDay - A COleDateTime object that contains the day's date.
	// Returns:
	//     A pointer to a CXTPCalendarEvents object that contains the
	//     collection of the day's events.
	// See Also: CXTPCalendarEvents
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr RetrieveDayEvents(COleDateTime dtDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the collection of all schedules
	//     ID's for this resource.
	// Returns:
	//     A pointer to CUIntArray collection which contains numeric ID's
	//     for all schedules existing in this resource.
	// See Also:
	//     CUIntArray overview, IsSchedulesSetEmpty, ExistsScheduleID
	//-----------------------------------------------------------------------
	virtual CUIntArray* GetSchedules();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines whether a schedule with the
	//     specified ID exists or not.
	// Parameters:
	//     uScheduleID - A numeric ID of the schedule to check.
	//     bReturnTrueIfSchedulesSetEmpty - TRUE by default.
	//          If there are no any schedules specified inside this resource,
	//          the Calendar will show (and process in other way) all events.
	// Returns: TRUE if a schedule ID exists in the Schedules collection for
	//          this resource; FALSE otherwise.
	// See Also: GetSchedules, IsSchedulesSetEmpty
	//-----------------------------------------------------------------------
	virtual BOOL ExistsScheduleID(UINT uScheduleID, BOOL bReturnTrueIfSchedulesSetEmpty = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether schedules set in this
	//     resource is empty or not.
	// Returns:
	//     TRUE when schedules set is empty for this resource;
	//     FALSE if at least 1 schedule exists.
	// See Also: GetSchedules, ExistsScheduleID
	//-----------------------------------------------------------------------
	virtual BOOL IsSchedulesSetEmpty() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns a string name of this resource object
	// Returns: A string with the name of the resource object.
	// See Also: SetName
	//-----------------------------------------------------------------------
	virtual LPCTSTR GetName();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function applies a new name to the resource object.
	// Parameters:
	//     pcszResourceName - A string with the new name for the resource
	// See Also: GetName
	//-----------------------------------------------------------------------
	virtual void SetName(LPCTSTR pcszResourceName);

protected:
	CString m_strResourceName;            // Text name of the resource
	CXTPCalendarControl* m_pCalendarCtrl; // Parent Calendar control
	CXTPCalendarData* m_pDataProvider;    // Associated data provider
	CUIntArray m_arScheduleIDs; // which schedules are used for this resource.

	BOOL m_bCloseDataProviderWhenDestroy;   // If TRUE - call Close() method if the data provider when control is destroyed.

	// Returns reminders manager associated with the main Calendar control
	//CXTPCalendarRemindersManager* GetRemindersManager();

protected:

};

//===========================================================================
// Summary:
//      This class represents a simple array collection of CXTPCalendarResource
//      objects.
// See Also: CXTPCalendarPtrCollectionT
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarResources : public CXTPCalendarPtrCollectionT<CXTPCalendarResource>
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNCREATE(CXTPCalendarResources)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection constructor.
	// See Also: ~CXTPCalendarResources()
	//-----------------------------------------------------------------------
	CXTPCalendarResources();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection destructor.
	// Remarks:
	//     Handles member item deallocation. Decreases reference of all
	//     stored CXTPCalendarResource objects.
	// See Also: RemoveAll()
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarResources();

protected:

};

//===========================================================================
// Summary:
//     This class implements CXTPCalendarResource functionality with addition
//     of notifications mechanism implemented by CXTPNotifySink.
// See also:
//     CXTPCalendarResource overview, CXTPNotifySink overview.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarResourcesNf : public  CXTPCalendarResources,
											   private CXTPNotifySink
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//     Construct CXTPCalendarResourcesNf object.
	// See Also: ~CXTPCalendarResourcesNf
	//-----------------------------------------------------------------------
	CXTPCalendarResourcesNf();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	// See Also: CXTPCalendarResourcesNf
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarResourcesNf();

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtain a pointer to the notification connection object.
	// Remarks:
	//     Used to subscribe (Advice) for notification events from the
	//     control.
	// Returns:
	//     Connection object pointer.
	// See Also: CXTPNotifyConnection overview,
	//           IXTPNotificationSink overview
	//-----------------------------------------------------------------------
	virtual CXTPNotifyConnection* GetConnection();

	//-----------------------------------------------------------------------
	// Summary:
	//     Rebuilds notification connection objects after changes in resources
	//     configuration.
	//     Call this member function after advising to notifications from
	//     connection pointers of the CXTPCalendarResourcesNf object
	// See Also: GetConnection, GetResourcesGroupedByDP
	//-----------------------------------------------------------------------
	virtual void ReBuildInternalData();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve a resources collection
	//     only for unique data providers.
	// Remarks:
	//     This method returns a collection of temporary resources objects
	//     having only unique data providers.
	//     It could be useful to retrieve a complete collection of events for
	//     further processing from all data providers in scope of resources
	//     collection, and in order not to retrieve duplicate events.
	// Returns:
	//     A pointer to the internal resources collection which is rebuilt
	//     during the ReBuildInternalData method execution.
	// See Also: ReBuildInternalData
	//-----------------------------------------------------------------------
	virtual CXTPCalendarResources* GetResourcesGroupedByDP();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function catches all the events from the associated
	//     data provider and forwards it to CXTPCalendarResourcesNf
	//     subscribers.
	// Parameters:
	//     dwNotifyCode - Events notification code
	//     wParam - First user's parameter.
	//     lParam - Second user's parameter.
	//     dwFlags - Unused parameter.
	//-----------------------------------------------------------------------
	virtual void OnEvent(XTP_NOTIFY_CODE dwNotifyCode, WPARAM wParam, LPARAM lParam,
						 DWORD dwFlags);
private:
	//{{AFX_CODEJOCK_PRIVATE
	static CXTPCalendarResource* AFX_CDECL FindByDataProvider(CXTPCalendarResources* pResources, CXTPCalendarData* pData);

	CXTPCalendarResources m_arResourcesGroupedByDP;

	class CXTPNotifyConnection_internal : public CXTPNotifyConnection
	{
		public:
			using CXTPNotifyConnection::CONNECTION_DESCRIPTOR;
			using CXTPNotifyConnection::m_arrConnections;
	};
	CXTPNotifyConnection_internal* m_pConnection;
	//}}AFX_CODEJOCK_PRIVATE
};

//////////////////////////////////////////////////////////////////////////
AFX_INLINE UINT CXTPCalendarSchedule::GetID() const {
	return m_uScheduleID;
}

AFX_INLINE LPCTSTR CXTPCalendarSchedule::GetName() const {
	return m_strScheduleName;
}

AFX_INLINE void CXTPCalendarSchedule::SetID(UINT uScheduleID) {
	m_uScheduleID = uScheduleID;
}

AFX_INLINE void CXTPCalendarSchedule::SetName(LPCTSTR pcszScheduleName) {
	ASSERT(pcszScheduleName);
	m_strScheduleName = pcszScheduleName ? pcszScheduleName : _T("");
}

AFX_INLINE CUIntArray* CXTPCalendarResource::GetSchedules() {
	return &m_arScheduleIDs;
}
AFX_INLINE BOOL CXTPCalendarResource::IsSchedulesSetEmpty() const {
	return m_arScheduleIDs.GetSize() == 0;
}

AFX_INLINE CXTPCalendarData* CXTPCalendarResource::GetDataProvider() {
	ASSERT(this);
	return this ? m_pDataProvider : NULL;
}

AFX_INLINE LPCTSTR CXTPCalendarResource::GetName() {
	return m_strResourceName;
}

AFX_INLINE void CXTPCalendarResource::SetName(LPCTSTR pcszResourceName) {
	ASSERT(pcszResourceName);
	m_strResourceName = pcszResourceName ? pcszResourceName : m_strResourceName;
}

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPNotifyConnection* CXTPCalendarResourcesNf::GetConnection() {
	return m_pConnection;
}

AFX_INLINE CXTPCalendarResources* CXTPCalendarResourcesNf::GetResourcesGroupedByDP() {
	return &m_arResourcesGroupedByDP;
}

AFX_INLINE CXTPCalendarCustomProperties* CXTPCalendarSchedule::GetCustomProperties() {
	return m_pCustomProperties;
}


#endif // !defined(_XTPCALENDARRESOURCE_H_)
