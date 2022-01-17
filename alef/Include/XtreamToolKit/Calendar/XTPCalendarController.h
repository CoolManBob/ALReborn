// XTPCalendarController.h: interface for the CXTPCalendarController class.
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
#if !defined(_XTPCALENDARCONTROLLER_H__)
#define _XTPCALENDARCONTROLLER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE



class CXTPDatePickerControl;
class CXTPCalendarControl;
class CXTPCalendarResourcesNf;

struct XTP_DAYITEM_METRICS;

//===========================================================================
// Summary:
//     Class CXTPCalendarController provides a connection between a
//     CXTPCalendarControl object and a CXTPDatePickerControl object.
// Remarks:
//     A CXTPCalendarController object provides a synchronization
//     facilities between CXTPCalendarControl and CXTPDatePickerControl
//     controls. It works as a "glue" between both controls and
//     reflects changes on one of them to the corresponded changes on the
//     second one.
// See Also: CXTPCalendarControl, CXTPDatePickerControl
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarController : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNCREATE(CXTPCalendarController)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCalendarController object.
	// Remarks:
	//     Construction of the CXTPCalendarController object defines
	//     a point of synchronization between CXTPCalendarControl and
	//     CXTPDatePickerControl controls. It can be constructed both
	//     in the same scope with controls which would provide us with
	//     full live objects time synchronization, and in the local scope
	//     for short time synchronization.
	//
	// Example:
	// <code>
	// Declare a local CXTPCalendarController object.
	// CXTPCalendarController myCalendarController;
	//
	// // Assigns CXTPCalendarControl instance to the object.
	// myCalendarController.SetCalendar(&m_wndCalendar);
	//
	// // Assigns CXTPDatePickerControl instance to the object.
	// myCalendarController.SetDatePicker(&m_wndDatePicker);
	// </code>
	// See Also: SetCalendar, SetDatePicker
	//-----------------------------------------------------------------------
	CXTPCalendarController();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCalendarController object, handles cleanup and
	//     de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarController();

// Attributes
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function associates a custom calendar control
	//     with the controller.
	// Parameters:
	//     pCalendarCtrl - A pointer to the user's CXTPCalendarControl object.
	// Remarks:
	//     Call this member function to associate your CXTPCalendarControl
	//     object with this calendar controller.
	// See Also: CXTPCalendarControl overview, SetDatePicker
	//-----------------------------------------------------------------------
	virtual void SetCalendar(CXTPCalendarControl* pCalendarCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function associates a custom date picker control
	//     with the controller.
	// Parameters:
	//     pDatePickerCtrl - Pointer to the user's CXTPDatePickerControl object.
	// Remarks:
	//     Call this member function to associate your CXTPDatePickerControl
	//     object with this calendar controller.
	// See Also: CXTPDatePickerControl overview, SetCalendar
	//-----------------------------------------------------------------------
	virtual void SetDatePicker(CXTPDatePickerControl* pDatePickerCtrl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get a calendar control object associated with the controller.
	// Remarks:
	//     A pointer to calendar control object associated with the
	//     controller or NULL.
	// See Also: SetCalendar, SetDatePicker, GetDatePicker
	//-----------------------------------------------------------------------
	virtual CXTPCalendarControl* GetCalendar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Get a date picker control object associated with the controller.
	// Remarks:
	//     A pointer to date picker control object associated with the
	//     controller or NULL.
	// See Also: SetCalendar, SetDatePicker, GetCalendar
	//-----------------------------------------------------------------------
	virtual CXTPDatePickerControl* GetDatePicker() const;


public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function used to determine whether Bold font is used
	//     for days in DatePicker which have corresponding Calendar events.
	// Returns:
	//     TRUE when Bold font is used, FALSE otherwise.
	// See Also:
	//     SetBoldDaysWithEvents
	//-----------------------------------------------------------------------
	virtual BOOL IsBoldDaysWithEvents() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function enables or disables Bold font usage for
	//     displaying days in DatePicker which have corresponding Calendar
	//     events.
	// Parameters:
	//     bBold - TRUE to enable; FALSE to disable.
	// Remarks:
	//     Disabling this parameter could be useful in cases when determining
	//     whether a particular day has events takes a long time.
	// See Also:
	//     IsBoldDaysWithEvents
	//-----------------------------------------------------------------------
	virtual void SetBoldDaysWithEvents(BOOL bBold);

	//-----------------------------------------------------------------------
	// Summary:
	//     Is BoldDaysOnIdle option enabled or disabled.
	// Returns:
	//     TRUE if enabled retrieve days state (bold or regular)
	//     on idle (by timer) for the associated DatePicker.
	// See Also:
	//     SetBoldDaysOnIdle, GetBoldDaysPerIdleStep, GetBoldDaysIdleStepTime_ms,
	//     SetBoldDaysPerIdleStep, SetBoldDaysIdleStepTime_ms.
	//-----------------------------------------------------------------------
	virtual BOOL IsBoldDaysOnIdle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Enable or Disable retrieve days state (bold or regular)
	//     on idle (by timer) for the associated DatePicker.
	// See Also:
	//     IsBoldDaysOnIdle, GetBoldDaysPerIdleStep, GetBoldDaysIdleStepTime_ms,
	//     SetBoldDaysPerIdleStep, SetBoldDaysIdleStepTime_ms.
	//-----------------------------------------------------------------------
	virtual void SetBoldDaysOnIdle(BOOL bBoldDaysOnIdle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns Amount of days for the associated DatePicker to update state
	//     in one idle step.
	// Returns:
	//     Amount of days for the associated DatePicker to update state in one
	//     idle step.
	// See Also:
	//     SetBoldDaysOnIdle, IsBoldDaysOnIdle, GetBoldDaysIdleStepTime_ms,
	//     SetBoldDaysPerIdleStep, SetBoldDaysIdleStepTime_ms.
	//-----------------------------------------------------------------------
	virtual long GetBoldDaysPerIdleStep() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns time between idle steps to update days state (bold or regular)
	//     for the associated DatePicker.
	// Returns:
	//     Time between idle steps in milliseconds.
	// See Also:
	//     SetBoldDaysOnIdle, IsBoldDaysOnIdle, GetBoldDaysPerIdleStep,
	//     SetBoldDaysPerIdleStep, SetBoldDaysIdleStepTime_ms.
	//-----------------------------------------------------------------------
	virtual long GetBoldDaysIdleStepTime_ms() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Set amount of days for the associated DatePicker to update state
	//     in one idle step.
	// See Also:
	//     SetBoldDaysOnIdle, IsBoldDaysOnIdle, GetBoldDaysIdleStepTime_ms,
	//     GetBoldDaysPerIdleStep, SetBoldDaysIdleStepTime_ms.
	//-----------------------------------------------------------------------
	virtual void SetBoldDaysPerIdleStep(long nDaysPerStaep);

	//-----------------------------------------------------------------------
	// Summary:
	//     Set time (in milliseconds) between idle steps to update days state
	//     (bold or regular) for the associated DatePicker.
	// See Also:
	//     SetBoldDaysOnIdle, IsBoldDaysOnIdle, GetBoldDaysPerIdleStep,
	//     SetBoldDaysPerIdleStep, GetBoldDaysIdleStepTime_ms.
	//-----------------------------------------------------------------------
	virtual void SetBoldDaysIdleStepTime_ms(long nStepTime_ms);

// Implementation
protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_XTP_SINK(CXTPCalendarController, m_Sink)
//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function subscribes itself for all necessary notifications.
	// Remarks:
	//     This member function is called when controller object wants to
	//     reinitialize its event subscriptions from associated controls.
	// See Also: CXTPCalendarController overview
	//-----------------------------------------------------------------------
	virtual void AdviseToNotifications();

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function get first day of week from the calendar control
	//      and set the same value to the date picker control.
	// See Also: OnEvent_CalendarOptionsChanged(), CXTPCalendarController overview
	//-----------------------------------------------------------------------
	virtual void SetFirstDayOfWeekToDatePicker();

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function used to determine is Compressed Weekend Day -
	//      Sunday, used as first day of week in the active calendar view.
	// Remarks:
	//      It means that first day of week is automatically shifted from
	//      Sunday to Monday because Sunday is compressed with Saturday and
	//      it cannot be shown as first day of week in the active calendar view.
	// See Also: CXTPCalendarWeekView, CXTPCalendarMonthView,
	//           CXTPCalendarControl::MonthView_IsCompressWeekendDays(),
	//           CXTPCalendarControl::GetFirstDayOfWeek()
	//-----------------------------------------------------------------------
	virtual BOOL IsFirstDayOfWeekShifted_FromSun2Mon();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function catches all the events from the associated
	//     calendar control and its view.
	// Parameters:
	//     Event - Events code
	//     wParam - First user's parameter.
	//     lParam - Second user's parameter.
	// Remarks:
	//     This member function is called when an event is fired from the
	//     associated CXTPCalendarControl object or from its view.
	// See Also: AdviseToNotifications
	//-----------------------------------------------------------------------
	virtual void OnEvent_CalendarView(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function catches all the events when calendar control
	//     options were changed.
	// Parameters:
	//     Event - Events code
	//     wParam - First user's parameter.
	//     lParam - Second user's parameter.
	// Remarks:
	//     This member function is called when an event is fired when calendar
	//     control options where changed..
	// See Also: AdviseToNotifications
	//-----------------------------------------------------------------------
	virtual void OnEvent_CalendarOptionsChanged(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function catches all the events when resource
	//     configuration was changed in the CalendarControl.
	// Parameters:
	//     Event - Events code
	//     wParam - First user's parameter.
	//     lParam - Second user's parameter.
	// See Also: AdviseToNotifications
	//-----------------------------------------------------------------------
	virtual void OnEvent_CalendarResourcesChanged(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function catches all the events from the associated
	//     date picker control.
	// Parameters:
	//     Event - Events code.
	//     wParam - First user's parameter.
	//     lParam - Second user's parameter.
	// Remarks:
	//     This member function is called when an event is fired from the
	//     associated CXTPDatePickerControl object.
	// See Also: AdviseToNotifications
	//-----------------------------------------------------------------------
	virtual void OnEvent_DatePicker(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);
	virtual void OnEvent_DtPickGetItemMetrics(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam); //<COMBINE OnEvent_DatePicker>

protected:
	CXTPCalendarControl* m_pCalendarCtrl;       // Pointer to the associated Calendar control.
	CXTPDatePickerControl* m_pDatePickerCtrl;   // Pointer to the associated DatePicker control.

	BOOL m_bBoldDaysWithEvents;           // TRUE to use Bold for days with events in the associated DatePicker.
	BOOL m_bUseActiveViewResoucesForBold; // If true - active view resources are used, if FALSE - calendar control Resources are used.

	BOOL m_bBoldDaysOnIdle;          // TRUE to retrieve days state (bold or regular) on idle (by timer) for the associated DatePicker.
	long m_nBoldDaysPerIdleStep;     // Amount of days for the associated DatePicker to update state in one idle step.
	long m_nBoldDaysIdleStepTime_ms; // Time between idle steps to update days state (bold or regular) for the associated DatePicker.

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function used to determine has the specified day events or not.
	// Returns:
	//     TRUE if the specified day has events, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL _HasEvents(COleDateTime dtDay);

private:
	void _AdviseToDataChanged();
	void _UnadviseFromDataChanged();

	virtual void OnEvent_CalendarDataChanged(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);

	BOOL m_bAdjustingView;

	CXTPCalendarResourcesNf* m_pResourcesNf;
	int m_eLastActiveView;

	XTP_CONNECTION_ID m_cnidEventAdded;
	XTP_CONNECTION_ID m_cnidEventChanged;
	XTP_CONNECTION_ID m_cnidEventDeleted;

	//
	class CXTPDayInfoCache
	{
		struct XTPDayInfo
		{
			BOOL bHasEvents;
			DATE dtLastAccessTime;
		};
		CMap<long, long, XTPDayInfo, XTPDayInfo&>   m_mapDaysInfo;
		CMap<long, long, UINT, UINT>                m_mapDaysToRefresh;

		CXTPCalendarController* m_pOwner;
		UINT_PTR                m_uTimerID;
		DWORD                   m_dwLastRedrawTime;
		DWORD                   m_dwLastSelfClearTime;
		DWORD                   m_dwWaitingDataTime;

		UINT                    m_uActivePriority;
	public:
		CXTPDayInfoCache();
		virtual ~CXTPDayInfoCache();

		BOOL HasEvents(DATE dtDay);

		void Init(CXTPCalendarController* pOwner);
		void Clear();

		void ClearDays(COleDateTime dtDayFrom, COleDateTime dtDayTo);
		void RequestToRefreshDays(COleDateTime dtDayFrom, COleDateTime dtDayTo);

		UINT UpActivePriority();
	protected:
		void UpdateDayInfo(DATE dtDay, BOOL bHasEvents);
		void KillTimer();

		void _RequestToRefreshDays(COleDateTime dtDayFrom, COleDateTime dtDayTo, UINT uPriority = 0);

		void OnRefreshDays(int nDaysCountToRefresh = 1);
		void OnSelfClearOld();

		static VOID CALLBACK OnTimerCallback(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

		typedef CMap<UINT_PTR, UINT_PTR, CXTPDayInfoCache*, CXTPDayInfoCache*> CXTPMapTimers;
		static CXTPMapTimers m_mapTimers;

	};
	friend class CXTPDayInfoCache;

	CXTPDayInfoCache m_DayInfoCache;


};

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE BOOL CXTPCalendarController::IsBoldDaysWithEvents() const {
	return m_bBoldDaysWithEvents;
}

AFX_INLINE BOOL CXTPCalendarController::IsBoldDaysOnIdle() const {
	return m_bBoldDaysOnIdle;
}

AFX_INLINE void CXTPCalendarController::SetBoldDaysOnIdle(BOOL bBoldDaysOnIdle) {
	m_bBoldDaysOnIdle = bBoldDaysOnIdle;
	m_DayInfoCache.Clear();
}

AFX_INLINE long CXTPCalendarController::GetBoldDaysPerIdleStep() const {
	return m_nBoldDaysPerIdleStep;
}

AFX_INLINE void CXTPCalendarController::SetBoldDaysPerIdleStep(long nDaysPerStaep) {
	m_nBoldDaysPerIdleStep = nDaysPerStaep;
}

AFX_INLINE long CXTPCalendarController::GetBoldDaysIdleStepTime_ms() const {
	return m_nBoldDaysIdleStepTime_ms;
}

AFX_INLINE void CXTPCalendarController::SetBoldDaysIdleStepTime_ms(long nStepTime_ms) {
	m_nBoldDaysIdleStepTime_ms = nStepTime_ms;
}

#endif // !defined(_XTPCALENDARCONTROLLER_H__)
