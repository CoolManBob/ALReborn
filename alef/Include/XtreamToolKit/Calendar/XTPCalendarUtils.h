// XTPCalendarUtils.h: interface for the CXTPCalendarUtils class.
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
#if !defined(__XTPCALENDARUTILS_H_)
#define __XTPCALENDARUTILS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE

#include "Common/XTPVC50Helpers.h"
#include <math.h>
//////////////////////////////////////////////////////////////////////////
// Macros
//{{AFX_CODEJOCK_PRIVATE
#define XTPchSTR(x) #x
#define XTPchSTR2(x) XTPchSTR(x)

#define XTPNOTE(desc) message(__FILE__ "(" XTPchSTR2(__LINE__) ") : NOTE: " ##desc)
//How to use it: #pragma XTPNOTE("Message text")

//}}AFX_CODEJOCK_PRIVATE
//===========================================================================

//===========================================================================
// Remarks:
//     Half a second, expressed in days.
// See Also: COleDateTime overview.
//===========================================================================
#define XTP_HALF_SECOND  (1.0/172800.0)

//---------------------------------------------------------------------------
// Summary: Defines a lower bound of the time frame for the Calendar control.
// See Also: xtpCalendarDateTime_max
//---------------------------------------------------------------------------
static const COleDateTime xtpCalendarDateTime_min(100, 1, 1, 0, 0, 0);

//---------------------------------------------------------------------------
// Summary: Defines an upper bound of the time frame for the Calendar control.
// See Also: xtpCalendarDateTime_min
//---------------------------------------------------------------------------
static const COleDateTime xtpCalendarDateTime_max(9999, 12, 31, 0, 0, 0);

//{{AFX_CODEJOCK_PRIVATE
class CXTPCalendarEvent;
//}}AFX_CODEJOCK_PRIVATE

//////////////////////////////////////////////////////////////////////////

//===========================================================================
// Summary:
//      This template function is used to compare operands using operator >
//      for the operands type and return compare result from the set: {-1, 0, 1}.
// Parameters:
//     _Type    - Operands type.
//     e1       - First operand to compare.
//     e2       - Second operand to compare.
// Returns:
//     0 if e1 == e2; 1 if e1 > e2; -1 if e1 < e2.
//===========================================================================
template<class _Type>
AFX_INLINE int XTPCompare(_Type e1, _Type e2) {
	if (e1 > e2)
		return 1;
	if (e2 > e1)
		return -1;
	return 0;
}

//===========================================================================
// Summary:
//     Helper class with static member functions to perform common
//     tasks with dates, events and other objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarUtils
{
private:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor
	// Remarks:
	//     Do not create an instance of this class.
	//     Use CXTPCalendarUtils::<MemberFunction> form.
	//-----------------------------------------------------------------------
	CXTPCalendarUtils();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to retrieve information about a locale.
	// Parameters:
	//     LCType - An LCTYPE that specifies the type of locale information to
	//              retrieve. (for the LOCALE_USER_DEFAULT)
	// Returns:
	//     Integer value that contains the information about the locale.
	// See Also: GetLocaleInfo
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetLocaleLong(LCTYPE LCType);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to retrieve information about a locale.
	// Parameters:
	//     LCType     - An LCTYPE that specifies the type of locale information to
	//                  retrieve. (for the LOCALE_USER_DEFAULT)
	//     nMaxLength - An int that contains the maximum string size to
	//                  allocate for the corresponding buffer.
	// Returns:
	//     String value.
	// See Also: GetLocaleInfo
	//-----------------------------------------------------------------------
	static CString AFX_CDECL GetLocaleString(LCTYPE LCType, int nMaxLength);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to format a date as a date string
	//     for a LOCALE_USER_DEFAULT locale.
	// Parameters:
	//     lpDate   - Pointer to a SYSTEMTIME structure that contains
	//                the date information to be formatted. If this
	//                pointer is NULL, the function uses the current
	//                local system date.
	//
	//     dtDate   - A date to be formatted.
	//
	//     lpFormat - Pointer to a format picture string that is used
	//                to form the date string. The format picture
	//                string must be zero terminated. If lpFormat is
	//                NULL, the function uses the date format of the
	//                specified locale.
	// Remarks:
	//     The function formats either a specified date or the local system date.
	// Returns:
	//     A CString that contains the formatted date string.
	// See Also: GetDateFormat
	//-----------------------------------------------------------------------
	static CString AFX_CDECL GetDateFormat(CONST SYSTEMTIME* lpDate, LPCTSTR lpFormat);
	static CString AFX_CDECL GetDateFormat(CONST COleDateTime& dtDate, LPCTSTR lpFormat); // <COMBINE GetDateFormat>

	// ---------------------------------------------------------------------------
	// Summary:
	//     This member function is used to format a time as a time string for
	//     a LOCALE_USER_DEFAULT locale.
	// Parameters:
	//     lpTime   - Pointer to a SYSTEMTIME structure that contains the time
	//                 information to be formatted. If this pointer is NULL, the
	//                 function uses the current local system time.
	//
	//     dtTime   - A time to be formatted.
	//
	//     nHour    - Hour of time to be formatted.
	//     nMinutes - Minutes of time to be formatted.
	//
	//     lpFormat - Pointer to a format picture string that is used to form the
	//                 time string. The format picture string must be zero
	//                 terminated. If lpFormat is NULL, the function uses the time
	//                 format of the specified locale.
	// Remarks:
	//     The function formats either a specified time or the local system
	//     time.
	// Returns:
	//     A CString that contains the formatted time string.
	// See Also:
	//     GetTimeFormat
	// ---------------------------------------------------------------------------
	static CString AFX_CDECL GetTimeFormat(CONST SYSTEMTIME* lpTime, LPCTSTR lpFormat);
	static CString AFX_CDECL GetTimeFormat(CONST COleDateTime& dtTime, LPCTSTR lpFormat); // <COMBINE GetTimeFormat>
	static CString AFX_CDECL GetTimeFormat(int nHour, int nMinutes, LPCTSTR lpFormat); // <COMBINE GetTimeFormat>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate the maximum month
	//     day number for a specified Year and Month in the dtDate parameter.
	// Parameters:
	//     dtDate   - A COleDateTime date value.
	// Returns:
	//     An int that contains the maximum month day number. (30, 31, 28, 29)
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetMaxMonthDay(COleDateTime dtDate);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to shift the specified date for
	//     the specified number of months.
	// Parameters:
	//     refDate     - A reference to the COleDateTime object to shift
	//                   the date on.
	//     nMonthCount - An int that contains the number of months shifted.
	//                   Can be both positive and negative numbers.
	//     nMonthDay   - An int that contains the day of the month to set
	//                   for a resulting date. If nMonthDay is grater then the
	//                   maximum month day in the result month, then the maximum
	//                   possible day is set.
	// Remarks:
	//     This is a utility function that is used to shift the specified date
	//     for the specific number of months.
	// Returns:
	//     A BOOL. TRUE if the value of this COleDateTime object was set
	//     successfully. FALSE otherwise.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL ShiftDate_Month(COleDateTime &refDate, int nMonthCount);
	static BOOL AFX_CDECL ShiftDate_Month(COleDateTime &refDate, int nMonthCount, int nMonthDay); // <combine CXTPCalendarUtils::ShiftDate_Month@COleDateTime &@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to shift the specified date for the
	//     specified number of years.
	// Parameters:
	//     refDate     - Reference to the COleDateTime object to shift the date on.
	//     nYearCount  - An int that contains the number of years to shift. Can be both
	//                   positive and negative numbers.
	//     nMonthDay   - An int that contains the day of the month to set
	//                   for the resulting date. If nMonthDay is grater
	//                   then the maximum month day in the result month,
	//                   then the maximum possible day is set.
	// Remarks:
	//     This is a utility function that is used to shift the specified date
	//     the specified number of years.
	// Returns:
	//     A BOOL. TRUE if the value of this COleDateTime object is set
	//     successfully. FALSE otherwise.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL ShiftDate_Year(COleDateTime &refDate, int nYearCount);
	static BOOL AFX_CDECL ShiftDate_Year(COleDateTime &refDate, int nYearCount, int nMonthDay); // <combine CXTPCalendarUtils::ShiftDate_Year@COleDateTime &@int>

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine the number of minutes
	//     between two DateTime objects.
	// Parameters:
	//      dtStart :      A COleDateTime object that contains the beginning
	//                     date.
	//      dtEnd :        A COleDateTime object that contains the end date.
	//      bAllDayEvent : Calculate duration for all day event.
	// Remarks:
	//     This is a utility function that is used to retrieve the number of
	//     minutes between two COleDateTime objects.
	// Returns:
	//     An int that contains the number of minutes between two DateTime
	//     \objects.
	// ---------------------------------------------------------------------
	static int AFX_CDECL GetDurationMinutes(const COleDateTime& dtStart, const COleDateTime& dtEnd, BOOL bAllDayEvent);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to safely update a month day in
	//     the specified date.
	// Parameters:
	//     refDate     - Reference to the COleDateTime object to shift
	//                   the date on.
	//     nMonthDay   - An int that contains the day of the month to set
	//                   for the resulting date.
	//                   If nMonthDay is grater then the maximum month day in
	//                   the result month, then the maximum possible day is
	//                   set.
	// Remarks:
	//     This is utility function is used to update the month day in the
	//     specified date.
	// Returns:
	//     A BOOL. TRUE if the value of this COleDateTime object was set
	//     successfully. FALSE otherwise.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL UpdateMonthDay(COleDateTime &refDate, int nMonthDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to update the time part for the
	//     specified DateTime value.
	// Parameters:
	//     dtDate   - A COleDateTime object that contains the value that
	//                is used to update the time part.
	//     nHour    - An int that contains the new Hour in the time part.
	//     nMinutes - An int that contains the new Minutes in the time part.
	//     nSeconds - An int that contains the new Seconds in the time part.
	// Returns:
	//     A COleDateTime object that contains the updated time part.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static const COleDateTime AFX_CDECL UpdateTime(const COleDateTime& dtDate, int nHour, int nMinutes, int nSeconds);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to update the time part for the
	//     specified DateTime value.
	// Parameters:
	//     dtDate   - A COleDateTime object that contains the value used
	//                to update the time part.
	//     dtTime   - A COleDateTime object that contains the new time part value.
	// Returns:
	//     A COleDateTime object that contains the updated time part.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static const COleDateTime AFX_CDECL UpdateTime(const COleDateTime& dtDate, const COleDateTime& dtTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to update the time part to 23:59.
	// Parameters:
	//     dtDateTime - A COleDateTime object that contains the value used
	//                  to update the time part.
	// Returns:
	//     A COleDateTime object that contains the updated time part.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static const COleDateTime AFX_CDECL SetTime_235959(const COleDateTime& dtDateTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to update the date part of the
	//     specified DateTime value.
	// Parameters:
	//     dtTime   - A COleDateTime object that contains the value used
	//                to update the date part.
	//     nYear    - An int that contains the new Year used in the date part.
	//     nMonth   - An int that contains the new Minutes used in the the date part.
	//     nDay     - An int that contains the new Day used in the date part.
	//     dtDate   - A COleDateTime object that contains the new date part.
	// Returns:
	//     A COleDateTime object that contains the updated date part.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static const COleDateTime AFX_CDECL UpdateDate(const COleDateTime& dtTime, const COleDateTime& dtDate);
	static const COleDateTime AFX_CDECL UpdateDate(const COleDateTime& dtTime, int nYear, int nMonth, int nDay); // <combine CXTPCalendarUtils::UpdateDate@const COleDateTime&@const COleDateTime&>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to reset (set to zero) the time
	//     part for the specified DateTime value.
	// Parameters:
	//     dtDateTime   - A COleDateTime object that contains the time
	//                    value to reset.
	// Returns:
	//     A COleDateTime object with the time part set equal to zero.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static const COleDateTime AFX_CDECL ResetTime(const COleDateTime& dtDateTime);
	static const COleDateTime AFX_CDECL ResetTime(const DATE dtDateTime); // <COMBINE CXTPCalendarUtils::ResetTime@const COleDateTime&>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to reset (set to zero) the date
	//     part for the specified DateTime value.
	// Parameters:
	//     dtDateTime   - A COleDateTime DateTime value that contains the
	//                    DateTime value to reset.
	// Returns:
	//     A COleDateTime object with the date part set equal to zero.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static const COleDateTime AFX_CDECL ResetDate(const COleDateTime& dtDateTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to compare two dates (and times).
	// Parameters:
	//     dt1             - A COleDateTime object that contains the first
	//                       DateTime value.
	//     dt2             - A COleDateTime object that contains the second
	//                       DateTime value.
	//     bOneSecTimeDiff - A BOOL.  Used to set the precision of the compare operation.
	//                       If TRUE, then one second precision is used.
	//                       If FALSE, then half a second precision is used.
	// Returns:
	//     A BOOL. TRUE if the dates differ by less than or equal to the specified
	//     precision. FALSE otherwise.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsEqual(const COleDateTime& dt1, const COleDateTime& dt2, BOOL bOneSecTimeDiff = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine if the time part is
	//     equal to zero.
	// Parameters:
	//     dtDateTime   - A COleDateTime object that contains the value to check.
	// Returns:
	//     A BOOL. TRUE if the time part is equal to zero. FALSE otherwise.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsZeroTime(const COleDateTime& dtDateTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate differences between 2 dates
	//     in months.
	// Parameters:
	//     dt1             - A COleDateTime object that contains the first
	//                       DateTime value.
	//     dt2             - A COleDateTime object that contains the second
	//                       DateTime value.
	// Returns:
	//     Months difference for dt1 - dt2.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetDiff_Months(const COleDateTime& dt1, const COleDateTime& dt2);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get event end date (date part only) value.
	// Parameters:
	//     pEvent - Event object.
	//     dtStart - Event start time.
	//     dtEnd   - Event end time.
	// Remarks:
	//     Useful when end DateTime has a zero time part and/or event
	//     start equal end.
	// Returns:
	//     Date part of the event end DateTime.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static COleDateTime AFX_CDECL RetriveEventEndDate(const CXTPCalendarEvent* pEvent);
	static COleDateTime AFX_CDECL RetriveEventEndDate(const COleDateTime& dtStart, const COleDateTime& dtEnd); //<COMBINE CXTPCalendarUtils::RetriveEventEndDate@CXTPCalendarEvent*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get an event duration as a number
	//     of days.
	// Parameters:
	//     dtStart - Event start time.
	//     dtEnd   - Event end time.
	// Returns:
	//     Integer value of an event duration in days.
	// See Also:
	//     CXTPCalendarEvent::GetDurationMinutes(), CXTPCalendarEvent::GetDuration()
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetEventPeriodDays(const COleDateTime& dtStart, const COleDateTime& dtEnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate the day of the month
	//     from the provided COleDateTime object.
	// Parameters:
	//     rdtDate       - A COleDateTime object that contains the date to
	//                     update with the valid year and month parts.
	//     nWhichDay     - An int that contains the value from the enum
	//                     XTPCalendarWeek. This parameter specifies
	//                     the sequential number of the day For example:
	//                     First Friday.
	//     nWhichDayMask - An int that contains the combination of values
	//                     from the enums XTPCalendarWeekDay or the
	//                     XTPCalendarWeekDay. This parameter, together
	//                     with the nWhichDay parameter are used to
	//                     specify the following logical values:
	//                     Which Day of Month, Which WeekDay,
	//                     Which WeekEndDay, Which day of week
	//                     Su, Mo, ..., St. For example: Last day of Month,
	//                     Second WeekEndDay, Fourth Monday.
	// Returns:
	//     A BOOL. TRUE if the value of this COleDateTime object was set
	//     successfully. FALSE otherwise.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL CalcDayOfMonth(COleDateTime& rdtDate, int nWhichDay, int nWhichDayMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to convert a sequential week day
	//     number (from 1 to 7) to a value from the enum XTPCalendarWeekDay.
	// Parameters:
	//     nWeekDay - An int that contains the sequential week day number.
	//     1-Sunday, 2-Monday, ...
	// Returns:
	//     An int that contains the corresponding value from the enum
	//     XTPCalendarWeekDay.
	// See Also: GetDayOfWeekIndex, enum XTPCalendarWeekDay
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetDayOfWeekMask(int nWeekDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to convert an XTPCalendarWeekDay
	//     enum value to a sequential week day number (from 1 to 7).
	// Parameters:
	//     nDayOfWeekMask - An int that contains the value from the enum
	//     XTPCalendarWeekDay.
	// Returns:
	//     An int that contains the sequential week day number.
	//     1-Sunday, 2-Monday, ...
	// See Also: GetDayOfWeekMask, enum XTPCalendarWeekDay
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetDayOfWeekIndex(int nDayOfWeekMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to count the bits in the specified
	//     bit mask.
	// Parameters:
	//     nDayOfWeekMask - An int that contains the mask from the enum
	//                      XTPCalendarWeekDay.
	// Returns:
	//     An int that contains the integer value of the day 0f the week
	//     contained in the bit mask.
	// See Also: GetDayOfWeekMask, enum XTPCalendarWeekDay
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetDayOfWeekCount(int nDayOfWeekMask);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to make the days mask from Monday
	//     until the specified day.
	// Parameters:
	//     nEndWDay - An int that contains the sequential week day number.
	//     1-Sunday, 2-Monday, ...
	// Returns:
	//     An int that contains the values set from the enum XTPCalendarWeekDay.
	// See Also: GetDayOfWeekIndex, enum XTPCalendarWeekDay
	//-----------------------------------------------------------------------
	static int AFX_CDECL MakeDaysOfWeekMask_Mo_(int nEndWDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to shift the specified date to
	//     the begin of the week.
	// Parameters:
	//     dtDate               - A COleDateTime object that contains the value
	//                            to shift to the beginning of the week.
	//     nFirstDayOfWeekIndex - Index of the first day of week. Default value 1.
	//                            1-Sunday, 2-Monday, ...
	// Returns:
	//     A COleDateTime object that contains the date of the first day of week
	//     before the specified date.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static COleDateTime AFX_CDECL ShiftDateToWeekBegin(COleDateTime dtDate,
											 int nFirstDayOfWeekIndex = 1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate a week number of year.
	// Parameters:
	//     dtDate               - A COleDateTime object that contains the date
	//                            to calculate the week of year.
	//     nFirstDayOfWeekIndex - Index of the first day of week. Default value 1.
	//                            1-Sunday, 2-Monday, ...
	// Returns:
	//     A week number of year for the specified date.
	// See Also: COleDateTime overview
	//-----------------------------------------------------------------------
	static int AFX_CDECL GetWeekOfYear(COleDateTime dtDate, int nFirstDayOfWeekIndex = 1);

	//-----------------------------------------------------------------------
	// Summary:
	//      The SystemTimeToTzSpecificLocalTime function converts a time in
	//      Coordinated Universal Time (UTC) to a specified time zone's
	//      corresponding local time.
	// Parameters:
	//      pTZI    - [in] Pointer to a TIME_ZONE_INFORMATION structure
	//                that specifies the time zone of interest.
	//      pUTime  - [in] Pointer to a SYSTEMTIME structure that specifies
	//                a UTC. The function converts this universal time to
	//                the specified time zone's corresponding local time.
	//      pTzTime - [out] Pointer to a SYSTEMTIME structure that receives
	//                the local time information.
	// Returns:
	//      If the function succeeds, the return value is nonzero,
	//      and the function sets the members of the SYSTEMTIME structure
	//      pointed to by pTzTime to the appropriate local time values.
	// See Also:
	//      SystemTimeToTzSpecificLocalTime() windows API function.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL SystemTimeToTzSpecificLocalTime(const TIME_ZONE_INFORMATION* pTZI,
												const SYSTEMTIME* pUTime,
												SYSTEMTIME* pTzTime);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is used to determine that program running on
	//      Windows 9x operation system family.
	// Returns:
	//      TRUE for Windows 9x operation system, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsWin9x();

	//-----------------------------------------------------------------------
	// Summary:
	//      Converts a number of minutes to the corresponding COleDateTimeSpan
	//      object.
	// Parameters:
	//      nMinutes - A number of minutes in the time span.
	// Returns: Calculated COleDateTimeSpan object.
	//-----------------------------------------------------------------------
	static COleDateTimeSpan AFX_CDECL Minutes2Span(int nMinutes);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns current local time.
	// Returns:
	//      COleDateTime object with the calculated local time value.
	//-----------------------------------------------------------------------
	static COleDateTime AFX_CDECL GetCurrentTime();

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns current time format string for hours and minutes.
	// Returns:
	//      A time format string like: 'HH:mm', 'hh:mm tt', 'h:m t', ...
	//-----------------------------------------------------------------------
	static CString AFX_CDECL GetTimeFormatString_HrMin();

	//-----------------------------------------------------------------------
	// Summary:
	//      Parse a time string like "18:20" or "7:15 pm" and returns time
	//      as total minutes. Active locale setting are used.
	// Parameters:
	//      pcszTime_HrMin  - time string like "18:20" or "7:15 pm"
	// Returns:
	//      Time as total minutes.
	//-----------------------------------------------------------------------
	static int AFX_CDECL ParceTimeString_min(LPCTSTR pcszTime_HrMin);

	//-----------------------------------------------------------------------
	// Summary:
	//      Format time duration like "3 minutes" or "1 week".
	//      Active locale setting are used.
	// Parameters:
	//      nMinutes - Duration in minutes
	//      bRoundly - If TRUE duration will be rounded to biggest scale,
	//                 like 65 minutes will be formatted as "1 hour".
	//                 If False duration will formatted exactly:
	//                 65 minutes as "65 minutes; 120 minutes as "2 hours".
	// Returns:
	//      Time duration as string like "5 minutes", "2 hours", "1 day", "3 weeks".
	//-----------------------------------------------------------------------
	static CString AFX_CDECL FormatTimeDuration(int nMinutes, BOOL bRoundly);

	//-----------------------------------------------------------------------
	// Summary:
	//      Parse a time duration string like "5 minutes", "2 hours", "1 day"
	//      or "3 weeks" and returns duration as total minutes.
	//      Active locale setting are used.
	// Parameters:
	//      pcszTimeDuration - time duration string like "5 minutes", "2 hours"
	// Returns:
	//      Time duration as total minutes.
	//-----------------------------------------------------------------------
	static int AFX_CDECL ParceTimeDuration_min(LPCTSTR pcszTimeDuration);

	//-----------------------------------------------------------------------
	// Summary:
	//      Loads string using XTPResourceManager.
	// Parameters:
	//      nIDResource - ID of the string in resource.
	// Returns:
	//      Loaded string or an empty string.
	//-----------------------------------------------------------------------
	static CString AFX_CDECL LoadString(UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//      Loads string using XTPResourceManager.
	// Parameters:
	//      nIDResource - ID of the string in resource.
	//      rstrString  - [out] Reference to CString object to load string to.
	// Remarks:
	//      If string with such ID is not present in the resource - rstrString
	//      is not changed.
	// Returns:
	//      TRUE if string loaded, FALSE otherwise.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL LoadString2(CString& rstrString, UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//      Determine which locale is used active locale: current user locale
	//      or resource file locale.
	// Returns:
	//      TRUE if resource file locale is used active locale, FALSE otherwise.
	// See Also:
	//      SetUseResourceFileLocale, CXTPResourceManager::GetResourcesLangID,
	//      GetActiveLCID, LOCALE_USER_DEFAULT
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsUseResourceFileLocale();

	//-----------------------------------------------------------------------
	// Summary:
	//      Used to set which locale is used active locale: current user locale
	//      or resource file locale.
	// Parameters:
	//      bUseResourceFileLocale - If TRUE resource file locale will be used,
	//                               if FALSE current user locale will be used.
	// See Also:
	//      IsUseResourceFileLocale, CXTPResourceManager::GetResourcesLangID,
	//      GetActiveLCID, LOCALE_USER_DEFAULT
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetUseResourceFileLocale(BOOL bUseResourceFileLocale);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns active locale ID (current user locale or resource file locale)
	// Returns:
	//      LOCALE_USER_DEFAULT or resource file locale ID.
	// See Also:
	//      IsUseResourceFileLocale, SetUseResourceFileLocale,
	//      CXTPResourceManager::GetResourcesLangID, LOCALE_USER_DEFAULT
	//-----------------------------------------------------------------------
	static LCID AFX_CDECL GetActiveLCID();

	//-----------------------------------------------------------------------
	// Summary:
	//      This function used to determine a font with maximum symbols height
	//      in an array of fonts.
	// Parameters:
	//      arFonts     - [in] A reference to array of fonts.
	//      pDC         - [in] A pointer to valid device context or NULL.
	//      pnMaxHeight - [out] A pointer to int variable to store maximum font
	//                          height or NULL.
	// Remarks:
	//      if pDC is not NULL - the font height in device logical units is used.
	//      Otherwise the LOGFONT.lfHeight member is used as font height.
	// Returns:
	//      A font from the provided array with maximum symbols height.
	//-----------------------------------------------------------------------
	static CFont* AFX_CDECL GetMaxHeightFont(CArray<CFont*, CFont*>& arFonts,
											 CDC* pDC, int* pnMaxHeight = NULL);

private:
	static BOOL AFX_CDECL IsInDst(const TIME_ZONE_INFORMATION* pTZI, COleDateTime dtTime);

	struct CXTPTimesResourceIDs
	{
		typedef CMap<int, int, UINT, UINT> CMapInt2UINT;

		CMapInt2UINT s_mapMinutes;
		CMapInt2UINT s_mapHours;
		CMapInt2UINT s_mapDays;
		CMapInt2UINT s_mapWeeks;

		CXTPTimesResourceIDs();
	};

	static BOOL s_bUseResourceFileLocale;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      This function used to round date time value by add (subtract)
	//      half-second double value.
	// Parameters:
	//      dblDT - [in] A date time value in COLEDateTime double representation.
	// Returns:
	//      Rounded date time value in COLEDateTime double representation.
	//-----------------------------------------------------------------------
	static double AFX_CDECL _RoundDateTime_dbl(double dblDT);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function used to reset time part.
	// Parameters:
	//      dblDT - [in] A date time value in COLEDateTime double representation.
	// Returns:
	//      Date value (with zero time) in COLEDateTime double representation.
	//-----------------------------------------------------------------------
	static double AFX_CDECL _ResetTime_dbl(double dblDT);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function used to reset date part.
	// Parameters:
	//      dblDT - [in] A date time value in COLEDateTime double representation.
	// Returns:
	//      Time value (with zero date) in COLEDateTime double representation.
	//-----------------------------------------------------------------------
	static double AFX_CDECL _ResetDate_dbl(double dblDT);
};

//===========================================================================

//===========================================================================
// Summary:
//      Helper class template provides functionality to temporary set some
//      to variable and reset to initial or other value on destroy.
//      Can be useful to lock some data or actions via flags and unlock
//      automatically.
//===========================================================================
template<class _TValue>
class CXTPAutoResetValue
{
	_TValue     m_valReset;
	_TValue&    m_rData;
public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Object constructor.
	// Parameters:
	//      rData - A reference to managing variable.
	// Remarks:
	//      The original value of rData is stored in separate member and it
	//      will be restored on destroy.
	//-----------------------------------------------------------------------
	CXTPAutoResetValue(_TValue& rData) :
		m_rData(rData)
	{
		m_valReset = rData;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Object constructor.
	// Parameters:
	//      rData    - A reference to managing variable.
	//      valReset - A value to set when destroy.
	// Remarks:
	//      The original value of rData is ignored.
	//      valReset value will be used on destroy.
	//-----------------------------------------------------------------------
	CXTPAutoResetValue(_TValue& rData, const _TValue& valReset): m_rData(rData) {
		m_valReset = valReset;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Object destructor.
	// Remarks:
	//      Managing variable value will be restored to the original one or to
	//      value specified as second constructor parameter.
	//-----------------------------------------------------------------------
	virtual ~CXTPAutoResetValue() {
		m_rData = m_valReset;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to access managing variable.
	// Returns:
	//     Managing variable reference.
	//-----------------------------------------------------------------------
	operator _TValue&() {
		return m_rData;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function overloads the assignment operator for the
	//     CXTPAutoResetValue class.
	// Parameters:
	//     rSrc - A new value.
	// Returns:
	//     A reference to this object.
	//-----------------------------------------------------------------------
	const _TValue& operator =(const _TValue& rSrc) {
		return m_rData = rSrc;
	}
};

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE double CXTPCalendarUtils::_RoundDateTime_dbl(double dblDT)
{
	return (dblDT >= 0 ? dblDT + XTP_HALF_SECOND : dblDT - XTP_HALF_SECOND);
}

AFX_INLINE double CXTPCalendarUtils::_ResetTime_dbl(double dblDT)
{
	return (double)((LONGLONG)_RoundDateTime_dbl(dblDT));
}

AFX_INLINE double CXTPCalendarUtils::_ResetDate_dbl(double dblDT)
{
	double dblDate = _ResetTime_dbl(dblDT);
	double dblResult = dblDT > dblDate ? (dblDT - dblDate) : (dblDate - dblDT);

	double dblDate_raw = (double)((LONGLONG)dblDT);

	if (dblDate_raw != dblDate)
	{
		dblResult = _RoundDateTime_dbl(dblResult);
	}
	return dblResult;
}
AFX_INLINE const COleDateTime CXTPCalendarUtils::ResetTime(const COleDateTime& dtDateTime)
{
	return (DATE)_ResetTime_dbl((double)dtDateTime);
}

AFX_INLINE const COleDateTime CXTPCalendarUtils::ResetTime(const DATE dtDateTime)
{
	return (DATE)_ResetTime_dbl((double)dtDateTime);
}

AFX_INLINE const COleDateTime CXTPCalendarUtils::ResetDate(const COleDateTime& dtDateTime)
{
	return (DATE)_ResetDate_dbl((double)dtDateTime);
}
AFX_INLINE const COleDateTime CXTPCalendarUtils::SetTime_235959(const COleDateTime& dtDateTime)
{
	double dDate = _ResetTime_dbl((double)dtDateTime);
	return (DATE)(dDate + 1 - XTP_HALF_SECOND * 2);
}


AFX_INLINE int CXTPCalendarUtils::GetDayOfWeekMask(int nWeekDay) {
	ASSERT(nWeekDay >= 1 && nWeekDay <= 7);

	int nRes = 1 << (nWeekDay-1);
	return nRes;
}

//return 1 = Sunday, 2 = Monday ...
AFX_INLINE int CXTPCalendarUtils::GetDayOfWeekIndex(int nDayOfWeekMask) {
	for (int i = 1; i <= 7; i++)
	{
		int nTestMask = 1 << (i-1);
		if (nTestMask == nDayOfWeekMask){
			return i;
		}
	}
	ASSERT(FALSE);
	return 0;
}

AFX_INLINE int CXTPCalendarUtils::GetMaxMonthDay(COleDateTime dtDate)
{
	SYSTEMTIME sysDate;
	if (!GETASSYSTEMTIME_DT(dtDate, sysDate))
		return 0;

	// days per month  -     _  jan feb mar apr may jun   jul aug sep oct nov dec
	static int arDays[13] = {0, 31, 28, 31, 30, 31, 30,   31, 31, 30, 31, 30, 31};
	if (sysDate.wMonth == 2 && 0 == (sysDate.wYear % 4))
	{
		return 29;
	}
	else
	{
		return arDays[sysDate.wMonth];
	}
}

AFX_INLINE BOOL CXTPCalendarUtils::UpdateMonthDay(COleDateTime &refDate, int nMonthDay)
{
	if (nMonthDay > 28)
	{
		int nMaxDay = GetMaxMonthDay(refDate);
		if (!nMaxDay)
		{
			return FALSE;
		}
		nMonthDay = min(nMonthDay, nMaxDay);
	}

	SYSTEMTIME sysDate;
	if (!GETASSYSTEMTIME_DT(refDate, sysDate))
		return FALSE;

	int nRes = refDate.SetDate(sysDate.wYear, sysDate.wMonth, nMonthDay);
	return nRes == 0;
}

AFX_INLINE BOOL CXTPCalendarUtils::ShiftDate_Month(COleDateTime &refDate, int nMonthCount)
{
	SYSTEMTIME sysDate;
	if (!GETASSYSTEMTIME_DT(refDate, sysDate))
		return FALSE;

	int nYearNew = sysDate.wYear + nMonthCount / 12;
	int nMonthNew = sysDate.wMonth + nMonthCount % 12;

	if (nMonthNew > 12)
	{
		nMonthNew -= 12 * (nMonthCount / abs(nMonthCount));
		nYearNew++;
	}
	else if (nMonthNew <= 0)
	{
		nYearNew--;
		nMonthNew += 12;

		ASSERT(nMonthNew >= 1 && nMonthNew <= 12);
	}

	return COleDateTime::valid == refDate.SetDate(nYearNew, nMonthNew, 1);
}

AFX_INLINE BOOL CXTPCalendarUtils::ShiftDate_Month(COleDateTime &refDate, int nMonthCount, int nMonthDay)
{
	if (ShiftDate_Month(refDate, nMonthCount))
	{
		BOOL bRes = UpdateMonthDay(refDate, nMonthDay);
		return bRes;
	}
	return FALSE;
}

AFX_INLINE BOOL CXTPCalendarUtils::ShiftDate_Year(COleDateTime &refDate, int nYearCount)
{
	SYSTEMTIME sysDate;
	if (!GETASSYSTEMTIME_DT(refDate, sysDate))
	{
		return FALSE;
	}
	int nYearNew = sysDate.wYear + nYearCount;
	return COleDateTime::valid == refDate.SetDate(nYearNew, sysDate.wMonth, 1);
}

AFX_INLINE BOOL CXTPCalendarUtils::ShiftDate_Year(COleDateTime &refDate, int nYearCount, int nMonthDay)
{
	if (ShiftDate_Year(refDate, nYearCount))
	{
		BOOL bRes = UpdateMonthDay(refDate, nMonthDay);
		return bRes;
	}
	return FALSE;
}

AFX_INLINE int CXTPCalendarUtils::GetDiff_Months(const COleDateTime& dt1, const COleDateTime& dt2)
{
	int nYear1 = dt1.GetYear();
	int nMonth1 = dt1.GetMonth();

	int nYear2 = dt2.GetYear();
	int nMonth2 = dt2.GetMonth();

	int nYearDiff = nYear1 - nYear2;
	int nMonthDiff = nYearDiff * 12;

	nMonthDiff += nMonth1 - nMonth2;

	return nMonthDiff;
}

AFX_INLINE int CXTPCalendarUtils::GetDurationMinutes(const COleDateTime& dtStart, const COleDateTime& dtEnd, BOOL bAllDayEvent)
{
	COleDateTimeSpan spDuration;
	if (bAllDayEvent)
	{
		spDuration = SetTime_235959(dtEnd) - ResetTime(dtStart);
	}
	else
	{
		spDuration = dtEnd - dtStart;
	}
	return (int)GETTOTAL_MINUTES_DTS(spDuration);
}

AFX_INLINE const COleDateTime CXTPCalendarUtils::UpdateTime(const COleDateTime& dtDate, int nHour, int nMinutes, int nSeconds)
{
	SYSTEMTIME stResult;
	if (!GETASSYSTEMTIME_DT(dtDate, stResult))
	{
		ASSERT(FALSE);
		return dtDate;
	}

	stResult.wHour   = (WORD)nHour;
	stResult.wMinute = (WORD)nMinutes;
	stResult.wSecond = (WORD)nSeconds;

	return COleDateTime(stResult);
}

AFX_INLINE const COleDateTime CXTPCalendarUtils::UpdateTime(const COleDateTime& dtDate, const COleDateTime& dtTime)
{
	double dDate = _ResetTime_dbl((double)dtDate);
	double dTime = _ResetDate_dbl((double)dtTime);
	return (DATE)(dDate + dTime);
}

AFX_INLINE const COleDateTime CXTPCalendarUtils::UpdateDate(const COleDateTime& dtTime, int nYear, int nMonth, int nDay)
{
	SYSTEMTIME stResult;
	if (!GETASSYSTEMTIME_DT(dtTime, stResult))
	{
		ASSERT(FALSE);
		return dtTime;
	}

	stResult.wYear  = (WORD)nYear;
	stResult.wMonth = (WORD)nMonth;
	stResult.wDay   = (WORD)nDay;

	return COleDateTime(stResult);
}

AFX_INLINE const COleDateTime CXTPCalendarUtils::UpdateDate(const COleDateTime& dtTime, const COleDateTime& dtDate)
{
	double dDate = _ResetTime_dbl((double)dtDate);
	double dTime = _ResetDate_dbl((double)dtTime);
	return (DATE)(dDate + dTime);
}

AFX_INLINE BOOL CXTPCalendarUtils::IsZeroTime(const COleDateTime& dtDateTime)
{
	double dTime = _ResetDate_dbl((double)dtDateTime);
	return fabs(dTime) < XTP_HALF_SECOND;
}

AFX_INLINE BOOL CXTPCalendarUtils::IsEqual(const COleDateTime& dt1, const COleDateTime& dt2, BOOL bOneSecTimeDiff)
{
	double dTimeDiff = bOneSecTimeDiff ? XTP_HALF_SECOND * 2 : XTP_HALF_SECOND;
	return dTimeDiff >= fabs(dt1 - dt2);
}

AFX_INLINE COleDateTime CXTPCalendarUtils::RetriveEventEndDate(const COleDateTime& dtStart, const COleDateTime& dtEnd)
{
	COleDateTime dtEndDay = ResetTime(dtEnd);

	if (IsZeroTime(dtEnd))
	{
		COleDateTime dtStartDay = ResetTime(dtStart);

		if (dtStartDay < dtEndDay)
		{
			const COleDateTimeSpan spDay(1, 0, 0, 0);
			dtEndDay -= spDay;
		}
	}
	return dtEndDay;
}

AFX_INLINE int CXTPCalendarUtils::GetEventPeriodDays(const COleDateTime& dtStart, const COleDateTime& dtEnd)
{
	COleDateTime dtStartDay = ResetTime(dtStart);
	COleDateTime dtEndDay = RetriveEventEndDate(dtStart, dtEnd);

	COleDateTimeSpan spDuration = dtEndDay - dtStartDay;
	int nDays = (int)GETTOTAL_DAYS_DTS(spDuration) + 1;

	return nDays;
}

AFX_INLINE COleDateTimeSpan CXTPCalendarUtils::Minutes2Span(int nMunutes)
{
	double dblSpan =  ((double)nMunutes) / (24 * 60);
	return COleDateTimeSpan(dblSpan);
}

AFX_INLINE COleDateTime CXTPCalendarUtils::ShiftDateToWeekBegin(COleDateTime dtDate, int nFirstDayOfWeekIndex)
{
	dtDate = CXTPCalendarUtils::ResetTime(dtDate);

	int nShift = (dtDate.GetDayOfWeek() - nFirstDayOfWeekIndex + 7) % 7;
	if (nShift)
	{
		COleDateTimeSpan spShift(nShift, 0, 0, 0);
		dtDate -= spShift;
	}
	return dtDate;
}

AFX_INLINE int CXTPCalendarUtils::GetDayOfWeekCount(int nDayOfWeekMask)
{
	int nCount = 0;
	for (int i = 1; i <= 7; i++)
	{
		int nTestMask = 1 << (i-1);
		if (nTestMask & nDayOfWeekMask)
		{
			nCount++;
		}
	}
	return nCount;
}

AFX_INLINE int CXTPCalendarUtils::MakeDaysOfWeekMask_Mo_(int nEndWDay)
{
	int nMask = 0;
	for (int i = 1; i < nEndWDay; i++)
	{
		int nWDMask = CXTPCalendarUtils::GetDayOfWeekMask(i);
		nMask |= nWDMask;
	}
	return nMask;
}



/////////////////////////////////////////////////////////////////////////////
#endif // !defined(__XTPCALENDARUTILS_H_)
