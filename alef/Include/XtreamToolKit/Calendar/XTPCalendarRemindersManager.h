// XTPCalendarRemindersManager.h: interface for the
// CXTPCalendarRemindersManager class.
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
#if !defined(_XTP_CALENDAR_REMINDERS_MANAGER_H__)
#define _XTP_CALENDAR_REMINDERS_MANAGER_H__
//}}AFX_CODEJOCK_PRIVATE


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////
#include "Common/XTPNotifyConnection.h"

#include "XtpCalendarPtrs.h"
#include "XTPCalendarUtils.h"
#include "XTPCalendarPtrCollectionT.h"

class CXTPCalendarData;
class CXTPCalendarResources;
class CXTPCalendarResourcesNf;
class CXTPCalendarRemindersManager;


//===========================================================================
// Summary:
//     This class describes a reminder for calendar event and implements base
//     properties and operations on reminder.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarReminder : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarReminder)

	friend class CXTPCalendarRemindersManager;
	friend class CXTPCalendarReminders;
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pOwnerMan - A pointer to the reminders manager which own this object.
	// See Also: ~CXTPCalendarReminder()
	//-----------------------------------------------------------------------
	CXTPCalendarReminder(CXTPCalendarRemindersManager* pOwnerMan = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	// See Also: CXTPCalendarReminder()
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarReminder();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieve calendar event for which this reminder is related.
	// Returns:
	//     A pointer to calendar event object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr GetEvent();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve date and time when reminder
	//     is scheduled to be activated.
	// Remarks:
	//     This value for non snoozed event is:
	//          EventStartTime - MinutesBeforeStart.
	//     For snoozed event this value is set when Snooze() method is called.
	// Returns:
	//     Reminder activation date and time.
	// See Also: GetMinutesBeforeStart, Snooze, GetEvent.
	//-----------------------------------------------------------------------
	virtual COleDateTime GetNextReminderTime();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine the number of minutes
	//     before the start of an event.
	// Returns:
	//     An int that contains the number of Minutes before the start of
	//     an event.
	// See Also: CXTPCalendarEvent::GetReminderMinutesBeforeStart, GetEvent.
	//-----------------------------------------------------------------------
	virtual int GetMinutesBeforeStart();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Snooze active reminder.
	// Parameters:
	//     nMinutesAfterNow - A value in minutes to activate reminder again.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also:
	//     Dismiss, GetNextReminderTime, GetEvent,
	//     CXTPCalendarRemindersManager::Snooze
	//-----------------------------------------------------------------------
	virtual BOOL Snooze(int nMinutesAfterNow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Dismiss active reminder.
	// Remarks:
	//     Reminder flag for corresponding event object is set to FALSE.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also:
	//     Snooze, GetEvent, CXTPCalendarRemindersManager::Dismiss
	//-----------------------------------------------------------------------
	virtual BOOL Dismiss();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine are reminders objects have
	//     equal IDs (or are they represent the same reminder).
	// Parameters:
	//     pReminder2 - A pointer to reminder object to compare.
	// Returns:
	//     TRUE if reminders objects have equal IDs, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsEqualID(const CXTPCalendarReminder* pReminder2) const;

protected:
	COleDateTime    m_dtNextReminderTime;   // Store reminder activation date and time.
	int             m_nMinutesBeforeStart;  // Store minutes before start event to activate reminder.

	//-----------------------------------------------------------------------
	// Summary:
	//     Set calendar event for which this reminder is related.
	// Parameters:
	//     pEvent - A pointer to calendar event.
	// See Also: GetEvent
	//-----------------------------------------------------------------------
	virtual void SetEvent(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     This enum define the event type: recurrence or non-recurrence.
	//-----------------------------------------------------------------------
	enum EventType {
		evtNormal = 1,      // event type is recurrence.
		evtRecurrence = 2,  // event type is non-recurrence.
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     This structure contains data members to unambiguously define related
	//     recurrence event occurrence.
	// See Also: EventType
	//-----------------------------------------------------------------------
	struct RecurrenceEventInfo
	{
		DWORD   m_dwMasterEventID;          // Recurrence master event ID.
		DATE    m_dtOccurrenceStartTime;    // Recurrence occurrence start time or recurrence exception original start time.
		DATE    m_dtOccurrenceEndTime;      // Recurrence occurrence end time or recurrence exception original end time.
	};

	COleDateTime    m_dtEventStartTime;     // Store related event start time.
	CXTPCalendarData* m_pEventDataProvider; // Store related event data provider.

	EventType   m_eEventType;               // Define related event type: recurrence or non-recurrence.

	//-----------------------------------------------------------------------
	// Summary:
	//     This union contains data members to unambiguously define related
	//     event.
	// See Also: RecurrenceEventInfo, m_eEventType
	//-----------------------------------------------------------------------
	union {
		DWORD               m_dwNormalEventID;          // Non-recurrence event ID.
		RecurrenceEventInfo m_RecurrenceEventInfo;      // Recurrence event information (instead of ID).
	};

private:
	CXTPCalendarRemindersManager* m_pOwnerMan;

protected:
};

//===========================================================================
// Summary:
//      This class represents a simple array collection of CXTPCalendarReminder
//      objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarReminders : public CXTPCalendarPtrCollectionT<CXTPCalendarReminder>
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Finds a specified reminder object in the collection.
	// Remarks:
	//     CXTPCalendarReminder::IsEqualID method is used to compare reminders.
	// Parameters:
	//     pReminder - A pointer to reminder object to find.
	// Returns:
	//     Index of the equal reminder object in the collection or -1 if such
	//     object is not present.
	// See Also: Find@CXTPCalendarEvent
	//-----------------------------------------------------------------------
	virtual int Find(const CXTPCalendarReminder* pReminder) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Finds a reminder object in the collection which is related to
	//     the event specified.
	// Parameters:
	//     pEvent - A pointer to event.
	// Returns:
	//     Index of the related reminder object in the collection or
	//     -1 if such object is not present.
	// See Also: Find@CXTPCalendarReminder
	//-----------------------------------------------------------------------
	virtual int Find(CXTPCalendarEvent* pEvent) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Finds a reminder object in the collection which is related to
	//     the specified event ID.
	// Parameters:
	//     dwEventID - Event ID of normal event or Master event.
	// Returns:
	//     Index of the related reminder object in the collection or
	//     -1 if such object is not present.
	// See Also: Find@CXTPCalendarReminder, Find@CXTPCalendarEvent
	//-----------------------------------------------------------------------
	virtual int Find(DWORD dwEventID) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Try to find and remove reminder object which is related to
	//     the event specified.
	// Parameters:
	//     pEvent - A pointer to event.
	// Returns:
	//     TRUE if the related reminder object is removed from the collection,
	//     FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL RemoveDataForEvent(CXTPCalendarEvent* pEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sort reminders by the event start time field.
	//-----------------------------------------------------------------------
	virtual void Sort();
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Try to find and remove reminder(s) object(s) which are related to
	//     the recurrence master event specified.
	// Parameters:
	//     pEvent - A pointer to master event.
	// Returns:
	//     TRUE if the related reminder(s) object(s) was removed from
	//     the collection, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL _RemoveDataForMasterEvent(CXTPCalendarEvent* pEvent);
};

//===========================================================================
// Summary:
//     This class is used to monitor and manage calendar events reminders.
//     It will send notifications when active reminders list
//     is changed - new reminder(s) come or some reminder(s) goes away.
// Remarks:
//     When Reminder manager starts it check all events which have Reminder
//     flag set.
//     Reminder for event is added to active reminders list when:
//         Event->StartTime - Event->ReminderMinutesBeforeStart <= Now
//
//     There are 2 actions with reminder:
//         Dismiss - reset Reminder flag for the event and remove reminder.
//                   Reminder never comes back;
//
//         Snooze - set new NextReminderTime value and remove reminder.
//                  Reminder will come back when NextReminderTime <= Now.
//
//     Some Reminder manager data stored in CalendarEvent custom properties
//     (or RecurrencePattern custom properties) - this means if own events
//     storage is used you have to save/load custom properties too.
//
//     StartMonitoring/StopMonitoring are used to start/stop reminders manager.
//     These methods are called by the calendar control automatically when
//     CXTPCalendarControl::EnableReminders() method is called.
//
//     GetActiveReminders is used to retrieve collection of active reminders.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarRemindersManager : public CWnd
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarRemindersManager)

	friend class CXTPCalendarReminder;
	//}}AFX_CODEJOCK_PRIVATE

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// See Also: ~CXTPCalendarRemindersManager()
	//-----------------------------------------------------------------------
	CXTPCalendarRemindersManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also: CXTPCalendarRemindersManager()
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarRemindersManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve reminders count in the active
	//     reminders collection.
	// Remarks:
	//     Can be useful to quick determine is active reminders collection
	//     empty without copy collection.
	// Returns:
	//     Active reminders count.
	// See Also:
	//     GetActiveReminders
	//-----------------------------------------------------------------------
	virtual int GetActiveRemindersCount();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve active reminders collection.
	// Parameters:
	//     rarActiveReminders - A reference to an array to retrieve active
	//                          reminders.
	// See Also:
	//     GetActiveRemindersCount
	//-----------------------------------------------------------------------
	virtual void GetActiveReminders(CXTPCalendarReminders& rarActiveReminders);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Snooze specified reminder.
	// Parameters:
	//     pReminder        - A pointer to reminder object
	//     nMinutesAfterNow - A value in minutes to activate reminder again.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also:
	//     Dismiss, CXTPCalendarReminder::Snooze, CXTPCalendarReminder::Dismiss,
	//     CXTPCalendarReminder::GetNextReminderTime
	//-----------------------------------------------------------------------
	virtual BOOL Snooze(CXTPCalendarReminder* pReminder, int nMinutesAfterNow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Dismiss specified reminder.
	// Remarks:
	//     Reminder flag for corresponding event object is set to FALSE.
	// Parameters:
	//     pReminder    - A pointer to reminder object
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also:
	//     Snooze, CXTPCalendarReminder::Snooze, CXTPCalendarReminder::Dismiss
	//-----------------------------------------------------------------------
	virtual BOOL Dismiss(CXTPCalendarReminder* pReminder);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Dismiss all active reminders.
	// Remarks:
	//     Reminder flag for each event object is set to FALSE.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also:
	//     Snooze, Dismiss, CXTPCalendarReminder::Snooze, CXTPCalendarReminder::Dismiss
	//-----------------------------------------------------------------------
	virtual BOOL DismissAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Obtain a pointer to the notification connection object.
	// Remarks:
	//     Used to subscribe (Advice) for notification events from the
	//     reminders manager.
	// Returns:
	//     Connection object pointer.
	// See Also:
	//     CXTPNotifyConnection overview, IXTPNotificationSink overview
	//-----------------------------------------------------------------------
	virtual CXTPNotifyConnection* GetConnection();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used to attach to calendar resources (data provider)
	//     and start reminders monitoring.
	// Parameters:
	//     pResources     - A pointer to calendar resources collection.
	//     spPeriod2Cache - A time period for which reminder manager will read
	//                      reminders from calendar resources (data provider).
	//  Remarks:
	//     To retrieve reminders CXTPCalendarData::GetUpcomingEvents() method
	//     is used. This can be a slow operation and using spPeriod2Cache
	//     parameter value we can control how often this method will be called.
	//
	//     For example calendar control use spPeriod2Cache = 2 hours by default.
	//     This means than GetUpcomingEvents will be called only once per 2 hours.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also:
	//     StopMonitoring
	//-----------------------------------------------------------------------
	virtual BOOL StartMonitoring(CXTPCalendarResources* pResources, COleDateTimeSpan spPeriod2Cache);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used to detach from calendar resources (data provider)
	//     and and clear all reminders in reminders manager.
	// Returns:
	//     TRUE if operation is successful, FALSE otherwise.
	// See Also:
	//     StartMonitoring
	//-----------------------------------------------------------------------
	virtual BOOL StopMonitoring();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine is StartMonitoring was successfully
	//     called and reminders manager monitoring reminders.
	// Returns:
	//     TRUE if reminders manager active (started/running), FALSE otherwise.
	// See Also:
	//     StartMonitoring, StopMonitoring
	//-----------------------------------------------------------------------
	virtual BOOL IsMonitoringRunning() const;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used to send XTP_NC_CALENDAR_ON_REMINDERS notification
	//     via connection object.
	// Parameters:
	//     eAction - A reminders action. Value from enum XTPCalendarRemindersAction.
	//               lParam - Reserved. Set to 0.
	// See Also:
	//     XTP_NC_CALENDAR_ON_REMINDERS, XTPCalendarRemindersAction.
	//-----------------------------------------------------------------------
	virtual void NotifyReminders(int eAction, LPARAM lParam = 0);

	//-------------------------------------------------------------------------
	// Summary:
	//     Schedule to call NotifyReminders(xtpCalendarRemindersFire) in 500 ms.
	// See Also:
	//     NotifyReminders, XTP_NC_CALENDAR_ON_REMINDERS, XTPCalendarRemindersAction.
	//-------------------------------------------------------------------------
	virtual void PostNotify_RemindersFire();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method clear all reminders in reminders manager collections.
	// See Also:
	//     StopMonitoring
	//-----------------------------------------------------------------------
	virtual void RemoveAll();

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_XTP_SINK(CXTPCalendarRemindersManager, m_Sink)

	afx_msg void OnTimeChange();
	afx_msg void OnTimer(UINT_PTR uTimerID);

	virtual BOOL _Dismiss(CXTPCalendarReminder* pReminder);

	virtual BOOL UpdateDataFromDP(COleDateTime dtFrom, COleDateTimeSpan spPeriod);
	virtual BOOL GetUpcomingEventsAll(COleDateTime dtFrom, COleDateTimeSpan spPeriod, CXTPCalendarEventsPtr& rptrEvents);

	virtual BOOL ProcessActiveReminders(COleDateTime dtTime);


	virtual void OnEventChanged(XTP_NOTIFY_CODE Event, WPARAM wParam , LPARAM lParam);
	virtual BOOL ProcessNewEvent(CXTPCalendarEvent* pEvent, COleDateTime dtFrom, COleDateTimeSpan spPeriod);
	virtual BOOL ProcessChangedEvent(CXTPCalendarEvent* pEvent);
	virtual BOOL _ProcessNewSingleEvent(CXTPCalendarEvent* pEvent, COleDateTime dtFrom, COleDateTimeSpan spPeriod);
	virtual BOOL _ProcessNewMasterEvent(CXTPCalendarEvent* pEvent, COleDateTime dtFrom, COleDateTimeSpan spPeriod);
	virtual BOOL _GetMaxExceptionReminder(CXTPCalendarRecurrencePattern* pPattern,int& rnMinutes);

	virtual BOOL _RemoveSnoozeData(CXTPCalendarReminder* pRmd);

	virtual BOOL _CreateWnd();
	virtual void OnFinalRelease();
//}}AFX_CODEJOCK_PRIVATE

	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()
	//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPNotifyConnection* m_pConnection;        // A pointer to the notification connection object.

	CXTPCalendarResourcesNf* m_pResourcesNf;    // A collection of calendar resources to be monitored.
	COleDateTimeSpan        m_spPeriod2Cache;   // Time period to cache data from the calendar resources.

	COleDateTime            m_dtLastUpdateTime; // Store last time when reminders data was loaded from the calendar resources.

	CXTPCalendarReminders   m_arWaitingReminders;  // Waiting reminders collection.
	CXTPCalendarReminders   m_arActiveReminders;   // Active reminders collection.

	BOOL m_bMonitoringRunning;          // Flag indicate that reminders manager is started;

	//{{AFX_CODEJOCK_PRIVATE
	BOOL m_bSkipOnEventChanged;
	//}}AFX_CODEJOCK_PRIVATE

protected:

};

////////////////////////////////////////////////////////////////////////////
AFX_INLINE COleDateTime CXTPCalendarReminder::GetNextReminderTime() {
	return m_dtNextReminderTime;
}
AFX_INLINE int  CXTPCalendarReminder::GetMinutesBeforeStart() {
	return m_nMinutesBeforeStart;
}
AFX_INLINE BOOL CXTPCalendarReminder::Snooze(int nMinutesAfterNow) {
	ASSERT(m_pOwnerMan);
	return m_pOwnerMan ? m_pOwnerMan->Snooze(this, nMinutesAfterNow) : FALSE;
}
AFX_INLINE BOOL CXTPCalendarReminder::Dismiss() {
	ASSERT(m_pOwnerMan);
	return m_pOwnerMan ? m_pOwnerMan->Dismiss(this) : FALSE;
}

//===========================================================================
AFX_INLINE CXTPNotifyConnection* CXTPCalendarRemindersManager::GetConnection() {
	return m_pConnection;
}

AFX_INLINE BOOL CXTPCalendarRemindersManager::IsMonitoringRunning() const {
	return m_bMonitoringRunning;
}

#endif // !defined(_XTP_CALENDAR_REMINDERS_MANAGER_H__)
