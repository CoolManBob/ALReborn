// XTPCalendarRecurrencePattern.h: interface for the CXTPCalendarRecurrencePattern class.
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
#if !defined(_XTPCALENDARRECURRENCEPATTERN_H__)
#define _XTPCALENDARRECURRENCEPATTERN_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "XTPCalendarDefines.h"
#include "XTPCalendarEvent.h"
#include "XTPCalendarPtrs.h"
#include "XTPCalendarEvents.h"

class CXTPCalendarData;
class CXTPPropExchange;
class CXTPCalendarCustomProperties;


//===========================================================================
// Summary:
//     This structure describes a reminder for an occurrence of a
//     recurrence event.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarReminderForOccurrence
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// See Also: ~CXTPCalendarReminderForOccurrence()
	//-----------------------------------------------------------------------
	CXTPCalendarReminderForOccurrence();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarReminderForOccurrence();

	DATE    m_dtOccurrenceStartTime; // Start time of the described occurrence.
	DATE    m_dtOccurrenceEndTime;   // End time of the described occurrence.

	int     m_nbIsReminder;          // XTPEnumCalendarReminderForOccurrenceParameters
	DATE    m_dtNextReminderTime_Snoozed; // Date/time of the next reminder event.

	//-----------------------------------------------------------------------
	// Summary:
	//      Copy operator.
	// Parameters:
	//      rSrc - Pointer to a source data object.
	// Returns:
	//      A Constant reference to updated object
	// See Also:
	//      CXTPCalendarReminderForOccurrence
	//-----------------------------------------------------------------------
	const CXTPCalendarReminderForOccurrence& operator=(const CXTPCalendarReminderForOccurrence& rSrc);
};


//===========================================================================
// Summary:
//     This class implements a collection of CXTPCalendarReminderForOccurrence
//     objects as well as some common operations on it.
//===========================================================================
class CXTPCalendarReminderForOccurrenceArray : public
	CArray<CXTPCalendarReminderForOccurrence, const CXTPCalendarReminderForOccurrence&>
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection constructor.
	// See Also: ~CXTPCalendarReminderForOccurrenceArray()
	//-----------------------------------------------------------------------
	CXTPCalendarReminderForOccurrenceArray();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection destructor.
	// Remarks:
	//     Handles member item deallocation. Decreases reference of all
	//     stored CXTPCalendarReminderForOccurrence objects.
	// See Also: RemoveAll()
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarReminderForOccurrenceArray();

	//-----------------------------------------------------------------------
	// Summary:
	//     Finds an index of the occurrence event.
	// Parameters:
	//     pOccEvent - Pointer to the occurrence event object.
	// Returns:
	//     An index of the occurrence event object in the array if found;
	//     or -1 if not found.
	// See Also: Find()
	//-----------------------------------------------------------------------
	int Find(CXTPCalendarEvent* pOccEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Finds an index of the occurrence event by its start and end time.
	// Parameters:
	//     dtOccStartTime - Occurrence start time.
	//     dtOccEndTime   - Occurrence end time.
	// Returns:
	//     An index of the occurrence event object in the array if found;
	//     or -1 if not found.
	// See Also: Find()
	//-----------------------------------------------------------------------
	int Find(DATE dtOccStartTime, DATE dtOccEndTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates occurrence custom properties with the values from the
	//     array if necessary one is found there.
	// Parameters:
	//     pOccEvent - Pointer to the occurrence event object.
	//     pPattern  - Pointer to the parent Recurrence Pattern object for
	//                 those occurrences.
	// Returns:
	//     Non-zero if updated successfully; zero if update failed.
	// See Also:
	//     UpdateOccIfNeedEx
	//-----------------------------------------------------------------------
	int UpdateOccIfNeed(CXTPCalendarEvent* pOccEvent,
						CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates occurrence custom properties with the values from the
	//     provided CXTPCalendarReminderForOccurrence structure.
	// Parameters:
	//     pOccEvent   - Pointer to the occurrence event object.
	//     pOccRmdData - Pointer to the Reminder data for the occurrence.
	//     pPattern    - Pointer to the parent Recurrence Pattern object for
	//                   those occurrences.
	// Returns:
	//     Non-zero if updated successfully; zero if update failed.
	// See Also:
	//     CXTPCalendarReminderForOccurrence overview, UpdateOccIfNeed
	//-----------------------------------------------------------------------
	static int UpdateOccIfNeedEx(CXTPCalendarEvent* pOccEvent,
								 const CXTPCalendarReminderForOccurrence* pOccRmdData,
								 CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     Loads an array of occurrences information from Calendar
	//     Custom Properties object.
	// Parameters:
	//     pProps - Pointer to a source CXTPCalendarCustomProperties object.
	// Returns:
	//     TRUE when loaded successfully; FALSE otherwise.
	// See Also:
	//     Save()
	//-----------------------------------------------------------------------
	BOOL Load(CXTPCalendarCustomProperties* pProps);

	//-----------------------------------------------------------------------
	// Summary:
	//     Saves an array of occurrences information to the Calendar
	//     Custom Properties object.
	// Parameters:
	//     pProps   - Pointer to a destination CXTPCalendarCustomProperties
	//                object.
	//     pPattern - Pointer to the parent Recurrence Pattern object for
	//                those occurrences.
	// Returns:
	//     TRUE when saved successfully; FALSE otherwise.
	// See Also:
	//     Load()
	//-----------------------------------------------------------------------
	BOOL Save(CXTPCalendarCustomProperties* pProps, CXTPCalendarRecurrencePattern* pPattern);

	//-----------------------------------------------------------------------
	// Summary:
	//     An utility method which clears all occurrences custom properties
	//     which was previously set by Save method.
	// Parameters:
	//     pProps - Pointer to CXTPCalendarCustomProperties object to be
	//              cleared.
	// See Also:
	//     Save(), Load()
	//-----------------------------------------------------------------------
	static void ClearProperties(CXTPCalendarCustomProperties* pProps);

private:
	static void _PropName(LPCTSTR pcszProp, int nIndex, CString& rstrPropNameI);
};

//===========================================================================
// Summary:
//     This structure defines a set of Recurrence Pattern options and is used
//     by a CXTPRecurrencePattern to easily manage and transfer recurrence
//     pattern data.
// See Also:
//     XTPCalendarEventRecurrenceType,
//     XTPCalendarWeekDay,
//     XTPCalendarWeekDay_Masks,
//     XTPCalendarWeekDay,
//     XTPCalendarWeek,
//     CXTPRecurrencePattern
//===========================================================================
struct XTP_CALENDAR_RECURRENCE_OPTIONS
{
public:
	//=======================================================================
	// Summary:
	//     Defines a group of parameters to describe the Daily pattern.
	//=======================================================================
	struct PATTERN_DAILY
	{
		BOOL    bEveryWeekDayOnly;  // Flag determines if the event occurs every week day.
		int     nIntervalDays;      // Event occurs every nIntervalDays days
		                            // beginning from a start date.
	};
	//=======================================================================
	// Summary:
	//     Defines a group of parameters to describe the Weekly pattern.
	//=======================================================================
	struct PATTERN_WEEKLY
	{
		int nIntervalWeeks; // Event occurs every nIntervalWeeks with the
		                    // first week beginning from a start date.
		int nDayOfWeekMask; // Determines the days of the week when an event occurs.
		                    // See Also: XTPCalendarWeekDay, XTPCalendarWeekDay_Masks.

	};
	//=======================================================================
	// Summary:
	//     Defines a group of parameters to describe the Month pattern.
	//=======================================================================
	struct PATTERN_MONTHLY
	{
		int nIntervalMonths;// Event occurs every nIntervalMonths with the
		                    // first month beginning from the start date.
		int nDayOfMonth;    // Determines the day of the month when an event occurs.
		                    // See Also: XTPCalendarWeek.
	};
	//=======================================================================
	// Summary:
	//     Defines a group of parameters to describe the Month pattern.
	//=======================================================================
	struct PATTERN_MONTHNTH
	{
		int nIntervalMonths;// Event occurs every nIntervalMonths month
		                    // beginning from start date.
		int nWhichDay;      // Determines the day of the month or nth order of day.
		int nWhichDayMask;  // Weekday code.
	};
	//=======================================================================
	// Summary:
	//     Defines a group of parameters to describe the Year pattern.
	//=======================================================================
	struct PATTERN_YEARLY
	{
		int nMonthOfYear;   // The month of every year that the event occurs on.
		int nDayOfMonth;    // The day of month that the event occurs on.
	};
	//=======================================================================
	// Summary:
	//     Defines a group of parameters to describe the Year pattern.
	//=======================================================================
	struct PATTERN_YEARNTH
	{
		int nWhichDay;      // Determines the day of the month or the nth order of the day.
		int nWhichDayMask;  // Weekday code.
		int nMonthOfYear;   // Month of the year.
	};


public:
//{{AFX_CODEJOCK_PRIVATE
	//-----------------------------------------------------------------------
	// Summary:
	//     Default class constructor.
	//-----------------------------------------------------------------------
	XTP_CALENDAR_RECURRENCE_OPTIONS() {
		memset(this, 0, sizeof(*this));
	};

	// --------------------------
	// Summary:
	//     Copy operator.
	// Parameters:
	//     rSrc :  The source object refferens;
	// Returns:
	//     This object reference.
	// --------------------------
	const XTP_CALENDAR_RECURRENCE_OPTIONS& operator=(const XTP_CALENDAR_RECURRENCE_OPTIONS& rSrc) {
		memcpy(this, &rSrc, sizeof(*this));
		return *this;
	}
//}}AFX_CODEJOCK_PRIVATE

public:
	XTPCalendarEventRecurrenceType m_nRecurrenceType; // Recurrence type, See Also: XTPCalendarEventRecurrenceType.

	// union stores active set of pattern parameters.
	union
	{
		PATTERN_DAILY       m_Daily;    // Options for Daily Recurrence type.
		PATTERN_WEEKLY      m_Weekly;   // Options for Weekly Recurrence type.
		PATTERN_MONTHLY     m_Monthly;  // Options for Monthly Recurrence type.
		PATTERN_MONTHNTH    m_MonthNth; // Options for MonthNth Recurrence type.
		PATTERN_YEARLY      m_Yearly;   // Options for Yearly Recurrence type.
		PATTERN_YEARNTH     m_YearNth;  // Options for YearNth Recurrence type.
	};

	//-----------------------------------------------------------------------
	// Summary:
	//      Saves this object data as 4 long values.
	// Parameters:
	//      rnData1 - [out] Reference to data variable number 1
	//      rnData2 - [out] Reference to data variable number 2
	//      rnData3 - [out] Reference to data variable number 3
	//      rnData4 - [out] Reference to data variable number 4
	// Remarks:
	//      Useful to save data in data storage;
	// See Also:
	//     SetData()
	//-----------------------------------------------------------------------
	void GetData(long& rnData1, long& rnData2, long& rnData3, long& rnData4)
	{
		rnData1 = m_nRecurrenceType;
		rnData2 = m_YearNth.nWhichDay;
		rnData3 = m_YearNth.nWhichDayMask;
		rnData4 = m_YearNth.nMonthOfYear;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Restore this object data from 4 long values previously retrieved
	//      using GetData() member function.
	// Parameters:
	//      rnData1 - [in] data value number 1
	//      rnData2 - [in] data value number 2
	//      rnData3 - [in] data value number 3
	//      rnData4 - [in] data value number 4
	// Remarks:
	//      Useful to load data from data storage;
	// See Also:
	//      GetData()
	//-----------------------------------------------------------------------
	void SetData(long nData1, long nData2, long nData3, long nData4)
	{
		ASSERT(nData1 >= 0 && nData1 <= 6); // just to check.

		m_nRecurrenceType = (XTPCalendarEventRecurrenceType)nData1;
		m_YearNth.nWhichDay = nData2;
		m_YearNth.nWhichDayMask = nData3;
		m_YearNth.nMonthOfYear = nData4;
	}

};

//===========================================================================
// Summary:
//     This class represents a recurrence pattern and stores data to build
//     schedule for recurrence events.
// See Also: CXTPCalendarEvent overview.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarRecurrencePattern : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarData;
	friend class CXTPCalendarEvent;
	DECLARE_DYNAMIC(CXTPCalendarRecurrencePattern)
	//}}AFX_CODEJOCK_PRIVATE
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Protected pattern constructor.
	// Parameters:
	//     pDataProvider - Pointer to a CXTPCalendarData object.
	// See Also:
	//     ~CXTPCalendarEvent()
	//-----------------------------------------------------------------------
	CXTPCalendarRecurrencePattern(CXTPCalendarData* pDataProvider);
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation. Decreases reference of
	//     stored objects: CXTPCalendarEventLabels and
	//     CXTPCalendarRecurrencePattern.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarRecurrencePattern();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a unique pattern ID.
	// See Also: CXTPCalendarRecurrencePattern()
	//-----------------------------------------------------------------------
	virtual DWORD GetPatternID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get start date-time.
	// Returns:
	//     COleDateTime object containing start date-time.
	// See Also: SetStartTime(COleDateTime dtStart)
	//-----------------------------------------------------------------------
	virtual COleDateTime GetStartTime() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new start date and time.
	// Parameters:
	//     dtStart - A COleDateTime object that contains the new start date value.
	// See Also: GetStartTime()
	//-----------------------------------------------------------------------
	virtual void SetStartTime(COleDateTime dtStart);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get a duration in minutes.
	// Returns:
	//     Integer value of a duration in minutes
	// See Also: SetDurationMinutes(int nMinutes), GetDuration()
	//-----------------------------------------------------------------------
	virtual int GetDurationMinutes() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new duration value.
	// Parameters:
	//     nMinutes - An int that contains the new duration value in minutes.
	// See Also: GetDurationMinutes()
	//-----------------------------------------------------------------------
	virtual void SetDurationMinutes(int nMinutes);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the duration as
	//     a COleDateTimeSpan object.
	// Returns:
	//     A COleDateTimeSpan object with a duration interval.
	// See Also: SetDuration(COleDateTimeSpan spDuration, BOOL bKeepStart = TRUE)
	//-----------------------------------------------------------------------
	virtual COleDateTimeSpan GetDuration() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the occurrence start date-time.
	// Parameters:
	//  dtOccurrenceStartDate - COleDateTime representing the start date.
	// Returns:
	//     A COleDateTime object that contains the parent's start date-time.
	// See Also: SetStartOccurrenceTime(COleDateTime dtStart)
	//-----------------------------------------------------------------------
	virtual COleDateTime GetStartOccurrenceTime(COleDateTime dtOccurrenceStartDate) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the occurrence end date-time
	//      by its start time.
	// Parameters:
	//     dtOccurrenceStartDate   - A COleDateTime object that contains
	//                               the date-time occurrence to identify.
	// Returns:
	//     A COleDateTime object containing the occurrence end date-time.
	// See Also: SetStartTime(COleDateTime dtStart)
	//-----------------------------------------------------------------------
	virtual COleDateTime GetEndOccurrenceTime(COleDateTime dtOccurrenceStartDate) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the value of the recurrence type flag.
	// Returns:
	//     An int that contains the previous value of the Recurrence type flag.
	// See Also: XTPCalendarEventRecurrenceType, XTP_CALENDAR_RECURRENCE_OPTIONS,
	//           SetRecurrenceOptions(const XTP_CALENDAR_RECURRENCE_OPTIONS& Options)
	//-----------------------------------------------------------------------
	virtual int GetRecurrenceType() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a set of recurrence options.
	// Returns:
	//     Reference to an XTP_CALENDAR_RECURRENCE_OPTIONS struct.
	// See Also: XTP_CALENDAR_RECURRENCE_OPTIONS,
	//           SetRecurrenceOptions(const XTP_CALENDAR_RECURRENCE_OPTIONS& Options)
	//-----------------------------------------------------------------------
	virtual const XTP_CALENDAR_RECURRENCE_OPTIONS& GetRecurrenceOptions() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the recurrence options.
	// Parameters:
	//     Options - Reference to a XTP_CALENDAR_RECURRENCE_OPTIONS struct.
	// See Also: XTP_CALENDAR_RECURRENCE_OPTIONS,
	//           SetRecurrenceOptions(const XTP_CALENDAR_RECURRENCE_OPTIONS& Options)
	//-----------------------------------------------------------------------
	virtual void SetRecurrenceOptions(const XTP_CALENDAR_RECURRENCE_OPTIONS& Options);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the start pattern DateTime.
	// Returns:
	//     A COleDateTime object containing the parent's start DateTime.
	// See Also: SetPatternStartTime(COleDateTime dtStart)
	//-----------------------------------------------------------------------
	virtual COleDateTime GetPatternStartDate() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new start pattern date and time.
	// Parameters:
	//     dtStart - A COleDateTime object that contains the new date value.
	// See Also: GetPatternStartTime()
	//-----------------------------------------------------------------------
	virtual void SetPatternStartDate(COleDateTime dtStart);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the end pattern DateTime.
	// Returns:
	//     A COleDateTime object that contains the parent's end DateTime.
	// See Also: SetPatternEndDate(COleDateTime dtEnd)
	//-----------------------------------------------------------------------
	virtual COleDateTime GetPatternEndDate() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new end pattern date.
	// Parameters:
	//     dtEnd   - A COleDateTime object that contains the new date value.
	// See Also: COleDateTime GetPatternEndDate()
	//-----------------------------------------------------------------------
	virtual void SetPatternEndDate(COleDateTime dtEnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the "End After occurrences"
	//     recurrence pattern option.
	// Returns:
	//     An int that contains the occurrence count for the pattern end.
	// See Also: SetEndAfterOccurrences, GetPatternEndDate, IsNoEndDate
	//-----------------------------------------------------------------------
	virtual int GetEndAfterOccurrences() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the "End After occurrences"
	//     recurrence pattern option.
	// Parameters:
	//     nOccurrences - An int that contains the occurrence count for the
	//                    pattern end.
	// See Also: GetEndAfterOccurrences, GetPatternEndDate, IsNoEndDate
	//-----------------------------------------------------------------------
	virtual void SetEndAfterOccurrences(int nOccurrences);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determines if the pattern has
	//     an end date.
	// Returns:
	//     A BOOL. TRUE if the pattern end is not set. FALSE otherwise.
	// See Also: SetNoEndDate(), GetEndAfterOccurrences(), GetPatternEndDate()
	//-----------------------------------------------------------------------
	virtual BOOL IsNoEndDate() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to reset the pattern end date.
	// See Also: IsNoEndDate(), GetEndAfterOccurrences(), GetPatternEndDate()
	//-----------------------------------------------------------------------
	virtual void SetNoEndDate();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the value of the "use end" flag.
	// Returns:
	//     An int that contains the "use end" flag value.
	// See Also:
	//     XTPCalendarPatternEnd.
	//-----------------------------------------------------------------------
	virtual XTPCalendarPatternEnd GetUseEndMethod() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the end pattern value.
	// Returns:
	//     A reference of the associated pattern end structure.
	// See Also:
	//     XTP_CALENDAR_PATTERN_END, XTPCalendarPatternEnd
	//-----------------------------------------------------------------------
	virtual const XTP_CALENDAR_PATTERN_END& GetPatternEnd() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the end pattern value.
	// Parameters:
	//     patternEnd - A reference to the source pattern end structure.
	// See Also:
	//     GetPatternEnd()
	//-----------------------------------------------------------------------
	virtual void SetPatternEnd(const XTP_CALENDAR_PATTERN_END& patternEnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the occurrence dates
	//     from the pattern in the given interval.
	// Parameters:
	//     rArDates    - A CDatesArray reference to an array of dates to be returned.
	//     dtStart     - A COleDateTime object that contains the interval's begin date.
	//     dtEnd       - A COleDateTime object that contains the interval's end date.
	// See Also: CDatesArray
	//-----------------------------------------------------------------------
	virtual void GetOccurrencesDates(CXTPCalendarDatesArray& rArDates, COleDateTime dtStart,
									COleDateTime dtEnd) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the occurrence dates
	//     from the pattern in the given interval.
	// Parameters:
	//     pEventsArray- Pointer to a CXTPCalendarEvents object that contains
	//                   the returned collection of slave events (occurrences).
	//     dtStart     - A COleDateTime object that contains the interval's begin date.
	//     dtEnd       - A COleDateTime object that contains the interval's end date.
	//     pMasterEvent- Pointer to a CXTPCalendarEvent object that contains the
	//                   master event.
	// See Also: CXTPCalendarEvents, CXTPCalendarEvent
	//-----------------------------------------------------------------------
	virtual BOOL GetOccurrences(CXTPCalendarEvents* pEventsArray, COleDateTime dtStart,
									COleDateTime dtEnd, CXTPCalendarEvent* pMasterEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a pointer to the read-write
	//     copy of the pattern object.
	// Returns:
	//     Smart pointer to CXTPCalendarRecurrencePattern object.
	// See Also: CXTPCalendarRecurrencePattern.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarRecurrencePatternPtr ClonePattern();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to update the pattern data from
	//     the given pattern.
	// Parameters:
	//     pNewData - A pointer to a CXTPCalendarRecurrencePattern object
	//                that contains the pattern to update.
	// Returns:
	//     A pointer to a CXTPCalendarRecurrencePattern object that contains
	//     the updated pattern.
	// See Also: CXTPCalendarRecurrencePattern.
	//-----------------------------------------------------------------------
	virtual BOOL Update(CXTPCalendarRecurrencePattern* pNewData);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the master event ID value.
	// Returns:
	//     A DWORD that contains the master event ID value.
	// See Also: SetMasterEventID(DWORD dwEventID).
	//-----------------------------------------------------------------------
	virtual DWORD GetMasterEventID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the master event ID value.
	// Parameters:
	//  dwEventID - A DWORD that contains the new master event ID value.
	// See Also: GetMasterEventID().
	//-----------------------------------------------------------------------
	virtual void SetMasterEventID(DWORD dwEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a pointer to the read-only
	//     interface of the master event.
	// Returns:
	//     Smart pointer to CXTPCalendarEvent object.
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventPtr GetMasterEvent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to add an exception to the master event.
	// Parameters:
	//     pException  - A pointer to a CXTPCalendarEvent object which
	//                   represents the exception.
	//     pMasterEventToUpdate    - A pointer to a CXTPCalendarEvent object
	//                               that is updated with the exception.
	// Returns:
	//     A BOOL. TRUE if the update is successful. FALSE otherwise.
	// See Also: CXTPCalendarEvent, CXTPCalendarEvent
	//-----------------------------------------------------------------------
	virtual BOOL SetException(CXTPCalendarEvent* pException, CXTPCalendarEvent* pMasterEventToUpdate = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to remove an exception from the
	//     master event.
	// Parameters:
	//     pException  - A pointer to a CXTPCalendarEvent object that
	//                   represents the exception.
	//     pMasterEventToUpdate - A pointer to the CXTPCalendarEvent object
	//                            from which the exception is removed.
	// Returns:
	//     TRUE is success; FALSE otherwise.
	// See Also: CXTPCalendarEvent, CXTPCalendarEvent, RemoveAllExceptions()
	//-----------------------------------------------------------------------
	virtual BOOL RemoveException(CXTPCalendarEvent* pException, CXTPCalendarEvent* pMasterEventToUpdate = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to remove all of the exceptions
	//     from the master event.
	// See Also: RemoveException(CXTPCalendarEvent* pException, CXTPCalendarEvent* pMasterEventToUpdate = NULL),
	//           SetException(CXTPCalendarEvent* pException, CXTPCalendarEvent* pMasterEventToUpdate = NULL)
	//-----------------------------------------------------------------------
	virtual void RemoveAllExceptions();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to return the read-only collection
	//     with the exception events.
	// Returns:
	//     Smart pointer to a CXTPCalendarEvents object that contains the read-only
	//     collection of exception events.
	// See Also: CXTPCalendarEvent, CXTPCalendarEvent, RemoveAllExceptions()
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEventsPtr GetExceptions();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load a recurrence pattern
	//     using the specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	// Remarks:
	//     This member function is used to store or load pattern data to or
	//     form an storage.
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

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to assign an ID to a pattern.
	// Parameters:
	//  dwID - Identifier to set.
	// See Also: GetPatternID()
	//-----------------------------------------------------------------------
	void SetPatternID(DWORD dwID);

private:
	void _GetOccurrenceStartEnd(const COleDateTime& dtOccStartDate, COleDateTime& rdtOccStart, COleDateTime& rdtOccEnd) const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to find the index of the exception
	//     event by its original start and end dates.
	// Parameters:
	//     dtStartOrig - A COleDateTime object that contains the start date.
	//     dtEndOrig   - A COleDateTime object that contains the end date.
	// Returns:
	//     An int that contains the value of the index in the collection.
	//     -1 if the exception is not found in the collection.
	//-----------------------------------------------------------------------
	virtual int FindExceptionIndex(COleDateTime dtStartOrig, COleDateTime dtEndOrig) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to find the exception event by its
	//     original start and end dates.
	// Parameters:
	//     dtStartOrig - A COleDateTime object that contains the start date.
	//     dtEndOrig   - A COleDateTime object that contains the end date.
	// Returns:
	//     If the exception event is found, then the function returns a pointer
	//     to a CXTPCalendarEvent object that contains the exception event.
	//     If the exception event is not found, then the function returns NULL.
	// See Also: CXTPCalendarEvent
	//-----------------------------------------------------------------------
	virtual CXTPCalendarEvent* FindException(COleDateTime dtStartOrig, COleDateTime dtEndOrig) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to find an exception event by its
	//     original start and end dates.
	// Parameters:
	//     pEvents - A CXTPCalendarEvents pointer that points to the collection of events to be found.
	//     dtStart - A COleDateTime object that contains the start date of the search interval.
	//     dtEnd   - A COleDateTime object that contains the end date of search interval.
	// Returns:
	//     A BOOL. The base class implementation alway returns TRUE.
	//     This function should return TRUE if successful.  FALSE otherwise.
	// See Also: CXTPCalendarEvents
	//-----------------------------------------------------------------------
	virtual BOOL FindExceptions(CXTPCalendarEvents* pEvents, COleDateTime dtStart, COleDateTime dtEnd) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to retrieve a collection of reminders
	//     settings for occurrences of this recurrence pattern.
	// Returns:
	//     An array of CXTPCalendarReminderForoccurrence objects for every
	//     occurrence with configured Reminders settings.
	// See Also:
	//     CXTPCalendarReminderForoccurrenceArray overview
	//-----------------------------------------------------------------------
	CXTPCalendarReminderForOccurrenceArray* GetOccReminders();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to setup an occurrence with new
	//     Reminder settings.
	// Parameters:
	//     pOccEvent - A CXTPCalendarEvent pointer that points to the occurrence
	//                 event which is to be modified.
	//     nbIsReminder - Trigger value showing whether to change enabled
	//                    reminder flag for this occurrence or not.
	//     dtNextReminderTime_Snoozed - Reminder snooze time.
	// See Also:
	//     GetOccReminders(),
	//     XTPEnumCalendarReminderForoccurrenceParameters overview
	//-----------------------------------------------------------------------
	void SetOccReminder(CXTPCalendarEvent* pOccEvent,
						int nbIsReminder = xtpCalendarRmdPrm_DontChange,
						DATE dtNextReminderTime_Snoozed = xtpCalendarRmdPrm_DontChange);

//  void ResetReminderForOccFromThePast(CXTPCalendarEvent* pMasterEvent);


private:
//{{AFX_CODEJOCK_PRIVATE
	DWORD m_dwPatternID;
	DWORD m_dwMasterEventID;

	CXTPCalendarReminderForOccurrenceArray m_arOccReminders;
	CXTPCalendarEvents  m_arExceptionEvents;
	CXTPCalendarData* m_ptrDataProvider;

	COleDateTime m_dtStartTime;
	int m_nDurationMinutes;

	XTP_CALENDAR_RECURRENCE_OPTIONS m_Options;

	COleDateTime m_dtPatternStartDate;

	XTP_CALENDAR_PATTERN_END m_PatternEnd;

	CXTPCalendarCustomProperties* m_pCustomProperties; //Custom properties collection object.
//}}AFX_CODEJOCK_PRIVATE

};
//===========================================================================



/////////////////////////////////////////////////////////////////////////////
AFX_INLINE DWORD CXTPCalendarRecurrencePattern::GetPatternID() const {
	return m_dwPatternID;
}

AFX_INLINE COleDateTime CXTPCalendarRecurrencePattern::GetStartTime() const {
	return m_dtStartTime;
}

AFX_INLINE void CXTPCalendarRecurrencePattern::SetStartTime(COleDateTime dtStart) {
	m_dtStartTime = dtStart;
}

AFX_INLINE int CXTPCalendarRecurrencePattern::GetDurationMinutes() const {
	return m_nDurationMinutes;
}

AFX_INLINE COleDateTimeSpan CXTPCalendarRecurrencePattern::GetDuration() const {
	return CXTPCalendarUtils::Minutes2Span(m_nDurationMinutes);
}

AFX_INLINE void CXTPCalendarRecurrencePattern::SetDurationMinutes(int nMinutes) {
	m_nDurationMinutes = nMinutes;
}


AFX_INLINE int CXTPCalendarRecurrencePattern::GetRecurrenceType() const {
	return m_Options.m_nRecurrenceType;
}

AFX_INLINE const XTP_CALENDAR_RECURRENCE_OPTIONS& CXTPCalendarRecurrencePattern::GetRecurrenceOptions() const {
	return m_Options;
}

AFX_INLINE const XTP_CALENDAR_PATTERN_END& CXTPCalendarRecurrencePattern::GetPatternEnd() const {
	return m_PatternEnd;
}

AFX_INLINE COleDateTime CXTPCalendarRecurrencePattern::GetPatternStartDate() const {
	return m_dtPatternStartDate;
}

AFX_INLINE COleDateTime CXTPCalendarRecurrencePattern::GetPatternEndDate() const {
	return m_PatternEnd.m_dtPatternEndDate;
}

AFX_INLINE int CXTPCalendarRecurrencePattern::GetEndAfterOccurrences() const {
	return m_PatternEnd.m_nEndAfterOccurrences;
}

AFX_INLINE void CXTPCalendarRecurrencePattern::SetEndAfterOccurrences(int nOccurrences) {
	m_PatternEnd.m_nEndAfterOccurrences = nOccurrences;
	m_PatternEnd.m_nUseEnd = xtpCalendarPatternEndAfterOccurrences;
}

AFX_INLINE BOOL CXTPCalendarRecurrencePattern::IsNoEndDate() const {
	return (m_PatternEnd.m_nUseEnd == xtpCalendarPatternEndNoDate);
}

AFX_INLINE void CXTPCalendarRecurrencePattern::SetNoEndDate() {
	m_PatternEnd.m_nUseEnd = xtpCalendarPatternEndNoDate;
}

AFX_INLINE XTPCalendarPatternEnd CXTPCalendarRecurrencePattern::GetUseEndMethod() const {
	return m_PatternEnd.m_nUseEnd;
}

AFX_INLINE DWORD CXTPCalendarRecurrencePattern::GetMasterEventID() const {
	return m_dwMasterEventID;
}

AFX_INLINE void CXTPCalendarRecurrencePattern::SetMasterEventID(DWORD dwEventID) {
	m_dwMasterEventID = dwEventID;
}

AFX_INLINE CXTPCalendarCustomProperties* CXTPCalendarRecurrencePattern::GetCustomProperties() {
	return m_pCustomProperties;
}

AFX_INLINE CXTPCalendarReminderForOccurrenceArray* CXTPCalendarRecurrencePattern::GetOccReminders() {
	return &m_arOccReminders;
}


#endif // !defined(_XTPCALENDARRECURRENCEPATTERN_H__)
