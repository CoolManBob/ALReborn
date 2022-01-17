// XTPCalendarEvent.h: interface for the CXTPCalendarEvent class.
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
#if !defined(_XTPCALENDAREVENT_H__)
#define _XTPCALENDAREVENT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarDefines.h"
#include "XTPCalendarRecurrencePattern.h"
#include "XTPCalendarPtrs.h"
#include "XTPCalendarPtrCollectionT.h"

/////////////////////////////////////////////////////////////////////////////
class CXTPCalendarEvent;
class CXTPCalendarRecurrencePattern;

class CXTPCalendarEventLabel;
class CXTPCalendarEventLabels;

class CXTPPropExchange;
class CXTPCalendarCustomProperties;

class CXTPCalendarIconIDs;
typedef CXTPCalendarIconIDs CXTPCalendarCustomIconIDs;

//===========================================================================
// Summary:
//     This class represents the data of a single Event used in the Calendar
//     control.
// Remarks:
//     Event can be recurring or non-recurring: see GetRecurrenceState()
//     method and XTPCalendarEventRecurrenceState. If Event is
//     recurring use GetRecurrencePattern() method to retrieve recurrence
//     properties.
//     Recurrence States (XTPCalendarEventRecurrenceState):
//
//
//     * <b>xtpCalendarRecurrenceMaster</b>     - Main event which is used for
//                                                creating Occurrences.
//     * <b>xtpCalendarRecurrenceOccurrence</b> - Event is one Occurrence of the
//                                                Recurrence series.
//     * <b>xtpCalendarRecurrenceException</b>  - Event is Occurrence which
//                                                properties differ than in the
//                                                Recurrence pattern or this
//                                                Occurrence is deleted (for
//                                                example: Start/End time or
//                                                Location are different).
//
//
// See Also: CXTPCalendarRecurrencePattern overview,
//           CXTPCalendarEventLabels overview,
//           CXTPCalendarEventLabel overview,
//           XTPCalendarEventRecurrenceState
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarEvent : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarData;
	friend class CXTPCalendarRecurrencePattern;
	friend class CXTPCalendarView;

	DECLARE_DYNAMIC(CXTPCalendarEvent)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//     pDataProvider - Pointer to CXTPCalendarData object.
	// See Also: ~CXTPCalendarEvent()
	//-----------------------------------------------------------------------
	CXTPCalendarEvent(CXTPCalendarData* pDataProvider);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation. Decreases reference of
	//     stored objects: CXTPCalendarEventLabels and
	//     CXTPCalendarRecurrencePattern.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarEvent();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a unique event ID.
	// See Also: CXTPCalendarEvent, SetEventID
	//-----------------------------------------------------------------------
	virtual DWORD GetEventID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set a new ID value for an event.
	// Parameters:
	//     dwEventID - A DWORD that contains the event id value.
	//-----------------------------------------------------------------------
	virtual void SetEventID(DWORD dwEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a schedule ID of the event.
	// Remarks:
	//     String with the schedule ID allows to identify schedules for different
	//     resources, for example Schedule ID could contain the name of the
	//     employee, who is the owner of this event.
	// See Also: CXTPCalendarEvent, SetScheduleID
	//-----------------------------------------------------------------------
	virtual UINT GetScheduleID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set a new schedule ID of the event.
	// Parameters:
	//     uScheduleID - An UINT that contains the ScheduleID.
	//-----------------------------------------------------------------------
	virtual void SetScheduleID(UINT uScheduleID);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the event subject.
	// Returns:
	//     A CString object that contains the subject text.
	//-----------------------------------------------------------------------
	virtual CString GetSubject() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the subject text for the event.
	// Parameters:
	//     pcszSubject - An LPCTSTR object that contains the string of text.
	//-----------------------------------------------------------------------
	virtual void SetSubject(LPCTSTR pcszSubject);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the event location text.
	// Returns:
	//     A CString object that contains the location text.
	// See Also: SetLocation
	//-----------------------------------------------------------------------
	virtual CString GetLocation() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the location text for the event.
	// Parameters:
	//     pcszLocation- An LPCTSTR object that contains the string of text.
	// See Also: GetLocation
	//-----------------------------------------------------------------------
	virtual void SetLocation(LPCTSTR pcszLocation);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the event label.
	// Returns:
	//     Pointer to an CXTPCalendarEventLabel object that contains
	//     the event label.
	// See Also: CXTPCalendarEventLabel, SetLocation
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventLabel* GetLabel() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the event label ID.
	// Returns:
	//     An int that contains the event label ID value.
	// See Also: CXTPCalendarEventLabel, SetLabelID
	//-----------------------------------------------------------------------
	virtual int GetLabelID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new label ID for the event.
	// Parameters:
	//     nLabelID - An int that contains the label ID value.
	// See Also: GetLabelID
	//-----------------------------------------------------------------------
	virtual void SetLabelID(int nLabelID);


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if an event is
	//     an "All Day" event.
	// Returns:
	//     A BOOL. TRUE if events is "All Day". FALSE otherwise.
	// See Also: SetAllDayEvent
	//-----------------------------------------------------------------------
	virtual BOOL IsAllDayEvent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the "All Day" flag
	//     for an event.
	// Parameters:
	//     bSet - A BOOL. Boolean value used to set the "All Day" flag
	//            to TRUE or FALSE.
	//            TRUE - Event is an "All day" event.
	//            FALSE - Event is not an "All day" event.
	// See Also: IsAllDayEvent
	//-----------------------------------------------------------------------
	virtual void SetAllDayEvent(BOOL bSet = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the event's start DateTime.
	// Returns:
	//     A COleDateTime object containing the event's start DateTime.
	// See Also: SetStartTime
	//-----------------------------------------------------------------------
	virtual COleDateTime GetStartTime() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new start date and
	//     time for the event.
	//     The time is accepted as is, and no any conversions is performed.
	// Parameters:
	//     dtStart - A COleDateTime object that contains the new start
	//               date value.
	// See Also: GetStartTime
	//-----------------------------------------------------------------------
	virtual void SetStartTime(COleDateTime dtStart);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the event's end DateTime.
	// Returns:
	//     A COleDateTime object that contains the event's end DateTime.
	// See Also: SetEndTime
	//-----------------------------------------------------------------------
	virtual COleDateTime GetEndTime() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new end date and time
	//     for the event.
	// Parameters:
	//     dtEnd - new date value.
	// See Also: GetEndTime
	//-----------------------------------------------------------------------
	virtual void SetEndTime(COleDateTime dtEnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get an event duration in minutes.
	// Returns:
	//     Integer value of an event duration in minutes.
	// See Also: GetDuration
	//-----------------------------------------------------------------------
	virtual int GetDurationMinutes() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get an event duration as
	//     an COleDateTimeSpan object.
	// Returns:
	//     An COleDateTimeSpan object that contains an event duration interval.
	// See Also: SetDuration
	//-----------------------------------------------------------------------
	virtual COleDateTimeSpan GetDuration() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new duration for the event.
	// Parameters:
	//     spDuration  - An COleDateTimeSpan object that contains the
	//                   new date interval value.
	//     bKeepStart  - TRUE -  Expanded flag and expand end DateTime.
	//                   FALSE - Expand start DateTime.
	// See Also: GetDuration
	//-----------------------------------------------------------------------
	virtual void SetDuration(COleDateTimeSpan spDuration, BOOL bKeepStart = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to move the start date event to
	//     the provided DateTime, duration of the event does not change.
	// Parameters:
	//     dtNewStartTime  - An COleDateTime object that contains the new
	//                       start date for the event.
	//-----------------------------------------------------------------------
	virtual void MoveEvent(COleDateTime dtNewStartTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get an event duration as a number
	//     of days.
	// Returns:
	//     Integer value of an event duration in days.
	// See Also: GetDurationMinutes(), GetDuration()
	//-----------------------------------------------------------------------
	virtual int GetEventPeriodDays() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if a reminder is set
	//     for event.
	// Returns:
	//     TRUE if a reminder is set. FALSE otherwise.
	// See also:
	//     SetReminder()
	//-----------------------------------------------------------------------
	virtual BOOL IsReminder() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to setup a reminder for this event.
	// Parameters:
	//     bReminder - TRUE to enable reminder. FALSE to disable.
	// Remarks:
	//     This function allows to enable/disable firing reminders
	//     notifications for this event.
	//     This boolean setting corresponds to a checkbox near "Reminder"
	//     word in Event details dialog.
	// See also:
	//     IsReminder()
	//-----------------------------------------------------------------------
	virtual void SetReminder(BOOL bReminder);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if sound file is configured
	//      for the event reminder.
	// Returns:
	//     TRUE if sound file is configured. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsSound() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine the number of minutes
	//     before the start of an event.
	// Returns:
	//     An int that contains the number of Minutes before the start of
	//     an event.
	// See Also: SetReminderMinutesBeforeStart(int nMinutes)
	//-----------------------------------------------------------------------
	virtual int GetReminderMinutesBeforeStart() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the number of minutes
	//     before the start of an event.
	// Parameters:
	//     nMinutes - An int that contains the number of minutes before
	//                the start of an event.
	// See Also: GetReminderMinutesBeforeStart()
	//-----------------------------------------------------------------------
	virtual void SetReminderMinutesBeforeStart(int nMinutes);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine a full path to the
	//     sound file assigned to the event reminder.
	// Returns:
	//     A CString object with full path to the sound file for the
	//     event reminder.
	// See Also: SetReminderSoundFile(LPCTSTR pcszFile)
	//-----------------------------------------------------------------------
	virtual CString GetReminderSoundFile() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the sound file for the
	//     event reminder.
	// Parameters:
	//     pcszFile - An LPCTSTR object.  A pointer to a string with full
	//                path to the sound file.
	// See Also: GetReminderSoundFile()
	//-----------------------------------------------------------------------
	virtual void SetReminderSoundFile(LPCTSTR pcszFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the body
	//     text for the event.
	// Returns:
	//     A CString object with the body text for the event.
	// See Also: SetBody
	//-----------------------------------------------------------------------
	virtual CString GetBody() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the body text for the event.
	// Parameters:
	//     pcszDesc - An LPCTSTR Pointer to the string with the body text.
	// See Also: GetBody
	//-----------------------------------------------------------------------
	virtual void SetBody(LPCTSTR pcszDesc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get a busy status value for the event.
	// Returns:
	//     Busy status value for the event as an XTPCalendarEventBusyStatus flag.
	// See Also: SetBusyStatus, XTPCalendarEventBusyStatus enum
	//-----------------------------------------------------------------------
	virtual int GetBusyStatus() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the busy status value for
	//     the event.
	// Parameters:
	//     Status - An int that contains the busy status value.
	// See Also: GetBusyStatus, XTPCalendarEventBusyStatus enum
	//-----------------------------------------------------------------------
	virtual void SetBusyStatus(int Status);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get an importance level for an event.
	// Returns:
	//     Importance level for event as an XTPCalendarEventImportance enum.
	// See Also: SetImportance, XTPCalendarEventImportance enum
	//-----------------------------------------------------------------------
	virtual int GetImportance() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the importance level for
	//     the event.
	// Parameters:
	//     nImportance - An int that contains the importance level value.
	// See Also: GetImportance, XTPCalendarEventImportance enum
	//-----------------------------------------------------------------------
	virtual void SetImportance(int nImportance);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the creation time for an event.
	// Returns:
	//     Creation time for event as COleDateTime object.
	// See Also: SetCreationTime
	//-----------------------------------------------------------------------
	virtual COleDateTime GetCreationTime() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the creation time for an event.
	// Parameters:
	//     dtTime - A COleDateTime object that contains the new creation time.
	// See Also: GetCreationTime
	//-----------------------------------------------------------------------
	virtual void SetCreationTime(COleDateTime dtTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the last modification time
	//     for an event.
	// Returns:
	//     A COleDateTime object that contains the last modification time
	//     for the event.
	// See Also: SetLastModificationTime
	//-----------------------------------------------------------------------
	virtual COleDateTime GetLastModificationTime() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the modification time for
	//     the event.
	// Parameters:
	//     dtTime - A COleDateTime object that contains the new modification time.
	// See Also: GetLastModificationTime
	//-----------------------------------------------------------------------
	virtual void SetLastModificationTime(COleDateTime dtTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get a recurrence state for the event.
	// Returns:
	//     Recurrence state for the event as an XTPCalendarEventRecurrenceState enum.
	// See Also: MakeEventAsRecurrence, XTPCalendarEventRecurrenceState enum
	//-----------------------------------------------------------------------
	virtual XTPCalendarEventRecurrenceState GetRecurrenceState() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the recurrence state for the event.
	// Returns:
	//     TRUE if recurrence state successfully changed. FALSE otherwise.
	// See Also: GetRecurrenceState, XTPCalendarEventRecurrenceState enum
	//-----------------------------------------------------------------------
	virtual BOOL MakeEventAsRecurrence();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the recurrence pattern for the event.
	// Parameters:
	//     pRPattern - A pointer to a CXTPCalendarRecurrencePattern object that
	//                 contains the updated recurrence pattern.
	// Returns:
	//     TRUE if new pattern changed successfully. FALSE otherwise.
	// See Also: CXTPCalendarRecurrencePattern, RemoveRecurrence
	//-----------------------------------------------------------------------
	virtual BOOL UpdateRecurrence(CXTPCalendarRecurrencePattern* pRPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function removes the recurrence pattern for the event.
	// Returns:
	//     TRUE if the operation is successful. FALSE otherwise.
	// See Also: UpdateRecurrence(CXTPCalendarRecurrencePattern* pRPattern)
	//-----------------------------------------------------------------------
	virtual BOOL RemoveRecurrence();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the recurrence pattern ID.
	// Returns:
	//     A DWORD containing the recurrence pattern ID.
	//-----------------------------------------------------------------------
	virtual DWORD GetRecurrencePatternID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the event's recurrence pattern
	//     object.
	// Returns:
	//     Pointer to a CXTPCalendarRecurrencePattern object that contains
	//     the event's recurrence pattern.
	// See Also: CXTPCalendarRecurrencePattern
	//-----------------------------------------------------------------------
	virtual CXTPCalendarRecurrencePatternPtr GetRecurrencePattern() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the event's recurrence pattern
	//     as an exception occurrence.
	// Parameters:
	//     dwPatternID - Recurrence pattern ID.
	// Returns:
	//     TRUE if successful. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL MakeAsRException(DWORD dwPatternID = XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if an exception occurrence
	//     is marked as deleted.
	// Returns:
	//     TRUE if an exception occurrence is marked as deleted. FALSE otherwise.
	// See Also: SetRExceptionDeleted(BOOL bDeleted = TRUE)
	//-----------------------------------------------------------------------
	virtual BOOL IsRExceptionDeleted() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to mark an exception occurrence as
	//     deleted/undeleted.
	// Parameters:
	//     bDeleted - A BOOL. Exception deleted flag value.
	//                TRUE  - To mark the exception recurrence as deleted.
	//                FALSE - To mark the exception recurrence as un-deleted.
	// Returns:
	//     TRUE if successful. FALSE otherwise.
	// See Also: IsRExceptionDeleted()
	//-----------------------------------------------------------------------
	virtual void SetRExceptionDeleted(BOOL bDeleted = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to update the event's data.
	// Parameters:
	//     pNewData - Pointer to a CXTPCalendarEvent object that contains
	//                the new data.
	// Returns:
	//     TRUE if successful. FALSE otherwise.
	// See Also: CXTPCalendarEvent
	//-----------------------------------------------------------------------
	virtual BOOL Update(CXTPCalendarEvent* pNewData);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain a pointer to the read-write
	//     interface of the event.
	// Returns:
	//     Pointer to a CXTPCalendarEvent object that contains the
	//     read-write interface of the event.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr CloneEvent();

	//-----------------------------------------------------------------------
	// Summary:
	//     Clones current event properties into a provided object instance.
	// Remarks:
	//     This member function is used by CloneEvent() implementation.
	// Returns:
	//     TRUE if cloned successfully; FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL CloneEventTo(CXTPCalendarEvent* pEventDest);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to build an occurrence event by
	//     duplicating the master event.
	// Parameters:
	//      dtStart :       Start occurrence event date time.
	//      dtEnd :         End occurrence event date time.
	//      dwNewEventID :  Occurrence event ID. If specified value is
	//                      XTP_CALENDAR_UNKNOWN_EVENT_ID - a new occurrence
	//                      event ID will be generated automatically.
	// Remarks:
	//     Call this member function to get an occurrence event object.
	// Returns:
	//     Pointer to a CXTPCalendarEvent object that contains the cloned
	//     \occurrence event.
	// See Also:
	//     CXTPCalendarEvent
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr CloneForOccurrence(COleDateTime dtStart, COleDateTime dtEnd,
									DWORD dwNewEventID = XTP_CALENDAR_UNKNOWN_EVENT_ID);


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to compare IDs of different events.
	// Parameters:
	//     pEvent - Pointer to a CXTPCalendarEvent object that contains
	//              an event to compare.
	// Returns:
	//     TRUE if the provided event ID is equal the "this" ID. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsEqualIDs(CXTPCalendarEvent* pEvent) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to compare start dates of different
	//     events.
	// Parameters:
	//     pEvent - Pointer to a CXTPCalendarEvent object that contains
	//              an event to compare.
	// Returns:
	//     TRUE if the provided event start date is equal to the "this" date.
	//     FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsEqualStartEnd(CXTPCalendarEvent* pEvent) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the state for the meeting flag.
	// Parameters:
	//     bMeeting - A BOOL that contains the new state for the meeting flag.
	// Remarks:
	//     Call this member function to set meeting state flag.
	//-----------------------------------------------------------------------
	virtual void SetMeeting(BOOL bMeeting = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the value of the
	//     Meeting state flag.
	// Returns:
	//     TRUE if the event is a meeting. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsMeeting();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the state for the private flag.
	// Parameters:
	//     bPrivate - A BOOL that contains the new state for the private flag.
	// Remarks:
	//     Call this member function to set the private state flag. The
	//     private flag is used to indicate that a meeting event is private.
	//-----------------------------------------------------------------------
	virtual void SetPrivate(BOOL bPrivate = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the value of the
	//     private state flag.
	// Returns:
	//     TRUE if the event is private. FALSE otherwise. The private flag
	//     is used to indicate that a meeting event is private.
	//-----------------------------------------------------------------------
	virtual BOOL IsPrivate();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain collection of custom icons IDs
	//     associated with event object.
	// Returns:
	//     Pointer to CXTPCalendarCustomIconIDs object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarCustomIconIDs* GetCustomIcons();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load an event using the
	//     specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	// Remarks:
	//     This member function is used to store or load event data to or
	//     from a storage.
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//      Get custom (named, user defined) properties collection.
	// Returns:
	//      A pointer to CXTPCalendarCustomProperties object.
	// See Also:
	//      CXTPCalendarCustomProperties.
	//-----------------------------------------------------------------------
	CXTPCalendarCustomProperties* GetCustomProperties();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the original start
	//     time of an exception event.
	// Returns:
	//     A COleDateTime object with the start original date.
	// See Also: SetRException_StartTimeOrig, GetRException_EndTimeOrig
	//-----------------------------------------------------------------------
	virtual COleDateTime GetRException_StartTimeOrig() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the original start time
	//     of an exception event.
	// Parameters:
	//     dtStartOrig - A COleDateTime object that contains the original
	//                   start time of the exception event.
	// See Also: GetRException_StartTimeOrig
	//-----------------------------------------------------------------------
	virtual void SetRException_StartTimeOrig(COleDateTime dtStartOrig);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the original end time
	//     of an exception event.
	// Returns:
	//     A COleDateTime object that contains the original date.
	// See Also: GetRException_StartTimeOrig, SetRException_EndTimeOrig
	//-----------------------------------------------------------------------
	virtual COleDateTime GetRException_EndTimeOrig() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the original end time of
	//     an exception event.
	// Parameters:
	//     dtEndOrig - A COleDateTime object that contains the original
	//                 end time of an exception event.
	// See Also: GetRException_EndTimeOrig, SetRException_StartTimeOrig
	//-----------------------------------------------------------------------
	virtual void SetRException_EndTimeOrig(COleDateTime dtEndOrig);

	//void SetPermanentlyROccurrence_Reminder(
	//      int nbIsReminder = xtpCalendarRmdPrm_DontChange,
	//      int nMinutesBeforeStart = xtpCalendarRmdPrm_DontChange );
	//      //, int nMinutesBeforeStart2_Snoozed = xtpCalendarRmdPrm_DontChange);


	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new ID value for the
	//     event recurrence pattern.
	// Parameters:
	//     dwRecurrencePatternID - An DWORD that contains the event
	//                             recurrence pattern id value.
	//-----------------------------------------------------------------------
	virtual void SetRecurrencePatternID(DWORD dwRecurrencePatternID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns event data provider.
	// Returns:
	//     A pointer to the data provider which stores this event object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarData* GetDataProvider();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set all event properties to the
	//     empty or default state. The same as after new object creation.
	//-----------------------------------------------------------------------
	virtual void Clear(); // Clear current event properties

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate the date-time
	//     values of the start and end dates for occurrences of recurrent events.
	// Returns:
	//     TRUE if successful. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL CalcStartEndDatesFromPattern();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to change the date-time values
	//     of the start and end dates of the master event using exception
	//     occurrences of recurrent events.
	// Returns:
	//     TRUE if successful. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL UpdateStartEndDatesFromPatternExceptions();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to assign a new RecurrencePattern
	//     to an event and set the Master flag.
	// Parameters:
	//     pPattern - Pointer to a CXTPCalendarRecurrencePattern object that
	//                contains the new recurrence pattern for the event.
	// Returns:
	//     TRUE - If the provided event start dates are equal. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL SetRecurrenceState_Master(CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the exception recurrence.
	// Parameters:
	//     nState          - An int that contains the new state value.
	//     dwNewPatternID  - A DWORD that contains the new pattern ID.
	// Returns:
	//     TRUE if the provided event start dates are equal. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL SetRecurrenceState_ExceptionOccurrence(XTPCalendarEventRecurrenceState nState, DWORD dwNewPatternID = XTP_CALENDAR_UNKNOWN_RECURRENCE_PATTERN_ID);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function is used to reset the recurrence state for an event.
	// Returns:
	//     Always returns TRUE.
	//-----------------------------------------------------------------------
	virtual BOOL SetRecurrenceState_NotRecurring();

	//-----------------------------------------------------------------------
	// Summary:
	//      Clear intermediate data members which where set when operations
	//      with recurrence properties where made.
	// Remarks:
	//      This method is automatically called by the date provider
	//      when event is updated or added.
	//-----------------------------------------------------------------------
	virtual void ClearIntermediateData();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a pointer to the
	//     internally stored Recurrence Pattern object.
	// Returns:
	//     A CXTPCalendarRecurrencePattern pointer that contains the
	//     Recurrence Pattern object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarRecurrencePattern* GetRPatternRef();


protected:
	CXTPCalendarData*  m_pDataProvider; // Pointer to the owner data provider.

	BOOL m_bMeeting; // Boolean flag that is used to indicate if an event is a meeting.
	BOOL m_bPrivate; // Boolean flag that is used to indicate if an event is a private meeting.

	DWORD   m_dwEventID;    // ID of this event object.
	int     m_nLabelID;     // Label ID.
	CString m_strSubject;   // Subject value.
	CString m_strLocation;  // Location value.

	UINT    m_uScheduleID;  // ID of the corresponding Schedule.

	BOOL m_bAllDayEvent;        // Is this an all day event value.
	COleDateTime m_dtStartTime; // Start event time value.
	COleDateTime m_dtEndTime;   // End event time value.

	BOOL    m_bReminder;                    // This flag value indicates is the Reminder enabled or not.
	int     m_nReminderMinutesBeforeStart;  // Reminder time value. (in minutes)
	CString m_strReminderSoundFile;         // Reminder sound file path.

	CString m_strBody;   // Body value.

	int     m_nBusyStatus;      // Event Busy Status value.
	int     m_nImportance;      // Event Importance value.
	XTPCalendarEventRecurrenceState m_nRecurrenceState; // Event Recurrence State.

	COleDateTime m_dtCreationTime;          // Creation event time value.
	COleDateTime m_dtLastModificationTime;  // Last Modification event time value.

	CXTPCalendarRecurrencePatternPtr m_ptrRPattern; // Recurrence Pattern object for a master event only.
	DWORD m_dwRecurrencePatternID;                  // Recurrence Pattern ID.

	BOOL m_bRExceptionDeleted;                  // Is Recurrence Exception deleted.

	COleDateTime m_dtRException_StartTimeOrig;  // Original start time of Recurrence exception event.
	COleDateTime m_dtRException_EndTimeOrig;    // Original end time of Recurrence exception event.

	DWORD m_dwRecurrencePatternID_ToUse;        // Previously used pattern ID.

	CXTPCalendarCustomProperties* m_pCustomProperties; //Custom properties collection object.

	CXTPCalendarCustomIconIDs*  m_pCustomIconsIDs; // An array to store custom icons id's assigned to event.

protected:

};

//===========================================================================
// Summary:
//     This class represents the collection of custom icons IDs.
// Remarks:
//     It also implements and some useful operations on custom icons
//     IDs collection.
//===========================================================================
class CXTPCalendarIconIDs : public CXTPArrayT<UINT, UINT>
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarIconIDs)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// See Also: ~CXTPCalendarIconIDs()
	//-----------------------------------------------------------------------
	CXTPCalendarIconIDs();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	// See Also: CXTPCalendarIconIDs()
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarIconIDs();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to find ID in the collection.
	// Parameters:
	//     uID - ID to find.
	// Returns:
	//     Zero based index in the collection or -1 if element does not find.
	//-----------------------------------------------------------------------
	virtual int Find(UINT uID) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Add ID to collection only once.
	// Parameters:
	//     uID - ID to add.
	// Remarks:
	//     If specified ID already present in the collection - method do nothing.
	// See Also: CArray::Add
	//-----------------------------------------------------------------------
	virtual void AddIfNeed(UINT uID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to remove ID from the collection.
	// Parameters:
	//     uID - ID to remove.
	// Returns:
	//     TRUE if specified ID find and removed, FALSE otherwise.
	// See Also: CArray::RemoveAt, CArray::RemoveAll
	//-----------------------------------------------------------------------
	virtual BOOL RemoveID(UINT uID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to store collection data as comma separated
	//     string.
	// Returns:
	//     Comma separated string with collection elements.
	// See Also: LoadFromString
	//-----------------------------------------------------------------------
	virtual CString SaveToString() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to read collection data from comma separated
	//     string.
	// Parameters:
	//     pcszData - String with collection data.
	// See Also: SaveToString
	//-----------------------------------------------------------------------
	virtual void LoadFromString(LPCTSTR pcszData);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load an collection data using the
	//     specified data object.
	// Parameters:
	//     pPX         - Source or destination CXTPPropExchange data object reference.
	//     pcszSection - Section name for elements set.
	//     pcszElement - Element value property name.
	// Remarks:
	//     This member function is used to store or load event data to or
	//     from a storage.
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX, LPCTSTR pcszSection = _T("CustomIcons"), LPCTSTR pcszElement = _T("IconID"));

protected:
};

//===========================================================================
///////////////////////////////////////////////////////////////////////
AFX_INLINE DWORD CXTPCalendarEvent::GetEventID() const {
	return m_dwEventID;
}

AFX_INLINE CString CXTPCalendarEvent::GetSubject() const {
	return m_strSubject;
}

AFX_INLINE void CXTPCalendarEvent::SetSubject(LPCTSTR pcszSubject) {
	m_strSubject = pcszSubject;
}

AFX_INLINE UINT CXTPCalendarEvent::GetScheduleID() const {
	return m_uScheduleID;
}

AFX_INLINE void CXTPCalendarEvent::SetScheduleID(UINT uScheduleID) {
	m_uScheduleID = uScheduleID;
}

AFX_INLINE CString CXTPCalendarEvent::GetLocation() const {
	return m_strLocation;
}

AFX_INLINE void CXTPCalendarEvent::SetLocation(LPCTSTR pcszLocation) {
	m_strLocation = pcszLocation;
}

AFX_INLINE int CXTPCalendarEvent::GetLabelID() const {
	return m_nLabelID;
}

AFX_INLINE void CXTPCalendarEvent::SetLabelID(int nLabelID) {
	m_nLabelID = nLabelID;
}

AFX_INLINE BOOL CXTPCalendarEvent::IsAllDayEvent() const {
	return m_bAllDayEvent;
}

AFX_INLINE void CXTPCalendarEvent::SetAllDayEvent(BOOL bSet) {
	ASSERT(m_nRecurrenceState != xtpCalendarRecurrenceOccurrence);
	m_bAllDayEvent = bSet;
}

AFX_INLINE COleDateTime CXTPCalendarEvent::GetStartTime() const {
	return m_dtStartTime;
}

AFX_INLINE COleDateTime CXTPCalendarEvent::GetEndTime() const {
	return m_dtEndTime;
}

AFX_INLINE BOOL CXTPCalendarEvent::IsReminder() const {
	return m_bReminder;
}

AFX_INLINE void CXTPCalendarEvent::SetReminder(BOOL bReminder) {
	m_bReminder = bReminder;
}

AFX_INLINE BOOL CXTPCalendarEvent::IsSound() const {
	return !m_strReminderSoundFile.IsEmpty();
}

AFX_INLINE int CXTPCalendarEvent::GetReminderMinutesBeforeStart() const {
	return m_nReminderMinutesBeforeStart;
}
AFX_INLINE void CXTPCalendarEvent::SetReminderMinutesBeforeStart(int nMinutes) {
	m_nReminderMinutesBeforeStart = nMinutes;
}

AFX_INLINE CString CXTPCalendarEvent::GetReminderSoundFile() const {
	return m_strReminderSoundFile;
}

AFX_INLINE void CXTPCalendarEvent::SetReminderSoundFile(LPCTSTR pcszFile) {
	m_strReminderSoundFile = pcszFile;
}

AFX_INLINE CString CXTPCalendarEvent::GetBody() const {
	return m_strBody;
}

AFX_INLINE void CXTPCalendarEvent::SetBody(LPCTSTR pcszDesc) {
	m_strBody = pcszDesc;
}

AFX_INLINE int CXTPCalendarEvent::GetBusyStatus() const {
	return m_nBusyStatus;
}

AFX_INLINE void CXTPCalendarEvent::SetBusyStatus(int nStatus) {
	m_nBusyStatus = nStatus;
}

AFX_INLINE int CXTPCalendarEvent::GetImportance() const {
	return m_nImportance;
}

AFX_INLINE void CXTPCalendarEvent::SetImportance(int nImportance) {
	m_nImportance = nImportance;
}

AFX_INLINE XTPCalendarEventRecurrenceState CXTPCalendarEvent::GetRecurrenceState() const {
	return m_nRecurrenceState;
}

AFX_INLINE COleDateTime CXTPCalendarEvent::GetCreationTime() const {
	return m_dtCreationTime;
}

AFX_INLINE void CXTPCalendarEvent::SetCreationTime(COleDateTime dtTime) {
	m_dtCreationTime = dtTime;
}

AFX_INLINE COleDateTime CXTPCalendarEvent::GetLastModificationTime() const {
	return m_dtLastModificationTime;
}

AFX_INLINE void CXTPCalendarEvent::SetLastModificationTime(COleDateTime dtTime) {
	m_dtLastModificationTime = dtTime;
}

AFX_INLINE DWORD CXTPCalendarEvent::GetRecurrencePatternID() const {
	return m_dwRecurrencePatternID;
}

//AFX_INLINE void CXTPCalendarEvent::SetRecurrencePatternID(DWORD dwRecurrencePatternID) {
//  m_dwRecurrencePatternID = dwRecurrencePatternID;
//}

AFX_INLINE BOOL CXTPCalendarEvent::IsRExceptionDeleted() const {
	return m_bRExceptionDeleted;
}

AFX_INLINE void CXTPCalendarEvent::SetRExceptionDeleted(BOOL bDeleted) {
	m_bRExceptionDeleted = bDeleted;
}
AFX_INLINE COleDateTime CXTPCalendarEvent::GetRException_StartTimeOrig() const {
	return m_dtRException_StartTimeOrig;
}

AFX_INLINE void CXTPCalendarEvent::SetRException_StartTimeOrig(COleDateTime dtStartOrig) {
	m_dtRException_StartTimeOrig = dtStartOrig;
}

AFX_INLINE COleDateTime CXTPCalendarEvent::GetRException_EndTimeOrig() const {
	return m_dtRException_EndTimeOrig;
}

AFX_INLINE void CXTPCalendarEvent::SetRException_EndTimeOrig(COleDateTime dtEndOrig) {
	m_dtRException_EndTimeOrig = dtEndOrig;
}

AFX_INLINE BOOL CXTPCalendarEvent::MakeAsRException(DWORD dwPatternID) {
	ASSERT(m_nRecurrenceState == xtpCalendarRecurrenceNotRecurring ||
		m_nRecurrenceState == xtpCalendarRecurrenceOccurrence ||
		m_nRecurrenceState == xtpCalendarRecurrenceException);
	return SetRecurrenceState_ExceptionOccurrence(xtpCalendarRecurrenceException, dwPatternID);
}

AFX_INLINE void CXTPCalendarEvent::SetMeeting(BOOL bMeeting) {
	m_bMeeting = bMeeting;
}
AFX_INLINE BOOL CXTPCalendarEvent::IsMeeting() {
	return m_bMeeting;
}
AFX_INLINE void CXTPCalendarEvent::SetPrivate(BOOL bPrivate) {
	m_bPrivate = bPrivate;
}
AFX_INLINE BOOL CXTPCalendarEvent::IsPrivate() {
	return m_bPrivate;
}
AFX_INLINE CXTPCalendarCustomProperties* CXTPCalendarEvent::GetCustomProperties() {
	return m_pCustomProperties;
}

AFX_INLINE CXTPCalendarData* CXTPCalendarEvent::GetDataProvider() {
	return m_pDataProvider;
}

AFX_INLINE CXTPCalendarCustomIconIDs* CXTPCalendarEvent::GetCustomIcons() {
	return m_pCustomIconsIDs;
}

#endif // !defined(_XTPCALENDAREVENT_H__)
