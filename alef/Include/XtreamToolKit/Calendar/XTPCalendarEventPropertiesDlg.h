// XTPCalendarEventPropertiesDlg.h: interface for the CXTPCalendarEventPropertiesDlg.
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
#if !defined(_XTP_CALENDAR_EVENT_PROPERTIS_DLG_H__)
#define _XTP_CALENDAR_EVENT_PROPERTIS_DLG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarPtrs.h"
#include "XTPCalendarUtils.h"

class CXTPCalendarViewEvent;
class CXTPCalendarControl;

//===========================================================================
// Summary:
//      This class used to edit calendar event properties. This is a default
//      implementation which can be used as an example or as a base class
//      for custom dialogs.
//      It designed to be localized - XTPResourceManager is used to load
//      resources.
// See Also:
//      CXTPCalendarEventRecurrenceDlg
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarEventPropertiesDlg : public CDialog
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarEventPropertiesDlg)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//{{AFX_CODEJOCK_PRIVATE
	enum { IDD = XTP_IDD_CALENDAR_EVENT_PROPERTIES };
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//     Dialog class constructor.
	// Parameters:
	//     pControl      - [in] Pointer to control.
	//     bAddEvent     - [in] If TRUE DataProvider->AddEvent() will be called
	//                          on OK button pressed, otherwise ChangeEvent
	//                          will be called.
	//     pParent       - [in] Pointer to parent window. Can be NULL.
	//
	//     pEditingEvent - [in, out] Pointer to editing event object. If NULL
	//                               a new event will be created.
	// Remarks:
	//     If pEditingEvent is NULL the new event will be created and StartTime,
	//     EndTime, AllDayEvent, ScheduleID properties will be initialized using
	//     current view selection.
	//     If OK button pressed event will be added or changed and control updated
	//     automatically.
	// See Also:
	//     ~CXTPCalendarEventRecurrenceDlg()
	//-----------------------------------------------------------------------
	CXTPCalendarEventPropertiesDlg(CXTPCalendarControl* pControl, BOOL bAddEvent,
					CWnd* pParent = NULL, CXTPCalendarEvent* pEditingEvent = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarEventPropertiesDlg(){};

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Update dialog controls values using editing event properties.
	//-----------------------------------------------------------------------
	virtual void UpdateControlsFromEvent();

	//-----------------------------------------------------------------------
	// Summary:
	//     Update dialog controls state (enabled and visible) using editing
	//     event properties.
	//-----------------------------------------------------------------------
	virtual void UpdateControlsState();

	//-----------------------------------------------------------------------
	// Summary:
	//     Initialize Reminder combobox.
	// Remarks:
	//      Time duration formatted using active resource file.
	//-----------------------------------------------------------------------
	virtual void InitReminderComboBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Initialize Label combobox.
	//-----------------------------------------------------------------------
	virtual void InitLabelComboBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Initialize Busy Status combobox.
	//-----------------------------------------------------------------------
	virtual void InitBusyStatusComboBox();

	// Generated message map functions
	//{{AFX_CODEJOCK_PRIVATE
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckReminder();
	afx_msg void OnBnClickedButtonRecurrecnce();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedCheckAlldayEvent();

	virtual CString LoadString(UINT nIDResource);
	virtual void AddEndDate_ResetTime(COleDateTime dtStartDate, COleDateTime& rdtEndDate, int nDays);

	DECLARE_MESSAGE_MAP()
	//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPCalendarEventPtr    m_ptrEditingEvent;  // Pointer to editing event object.

	CXTPCalendarControl*    m_ptrControl;       // Pointer to Calendar control.
	BOOL                    m_bAddEvent;        // Store edit mode: Add or Change.

	//------------------------------------------------------------------------
	CString         m_strSubject;   // Store subject editor value. UpdateData method used to Exchange value with dialog control.
	CString         m_strLocation;  // Store location editor value. UpdateData method used to Exchange value with dialog control
	CString         m_strBody;      // Store body editor value. UpdateData method used to Exchange value with dialog control.
	COleDateTime    m_dtStartDate;  // Store StartDate editor value. UpdateData method used to Exchange value with dialog control.
	COleDateTime    m_dtStartTime;  // Store StartTime editor value. UpdateData method used to Exchange value with dialog control.
	COleDateTime    m_dtEndDate;    // Store EndDate editor value. UpdateData method used to Exchange value with dialog control.
	COleDateTime    m_dtEndTime;    // Store EndTime editor value. UpdateData method used to Exchange value with dialog control.

	int m_nBusyStatus;      // Store BusyStatus checkbox value. UpdateData method used to Exchange value with dialog control.

	BOOL m_bAllDayEvent;    // Store AllDayEvent checkbox value. UpdateData method used to Exchange value with dialog control.

	BOOL m_bPrivate;        // Store Private checkbox value. UpdateData method used to Exchange value with dialog control.
	BOOL m_bMeeting;        // Store Meeting checkbox value. UpdateData method used to Exchange value with dialog control.
	BOOL m_bReminder;       // Store Reminder checkbox value. UpdateData method used to Exchange value with dialog control.

protected:
	int m_nAllDayEventSet;  // If value < 0 - AllDayEvent control initialized from editing event, otherwise it is initialized from m_nAllDayEventSet value.
	BOOL m_bOccurrence;     // If TRUE the not recurring event or recurrence exception (ocurrence) is editing, otherwise master event is editing.

	CComboBox m_ctrlLabelCB;        // Label combobox control instance.
	CComboBox m_ctrlReminderCB;     // Reminder combobox control instance.
	CComboBox m_ctrlBusyStatusCB;   // BusyStatus combobox control instance.

	CComboBox m_ctrlScheduleIDCB;   // ScheduleID combobox control instance.
	CStatic   m_ctrlScheduleLabel;  // Label window for ScheduleID combobox control.
};

AFX_INLINE CString CXTPCalendarEventPropertiesDlg::LoadString(UINT nIDResource) {
	return CXTPCalendarUtils::LoadString(nIDResource);
}

#endif // !defined(_XTP_CALENDAR_EVENT_PROPERTIS_DLG_H__)
