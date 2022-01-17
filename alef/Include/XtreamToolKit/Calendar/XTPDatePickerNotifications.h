// XTPDatePickerNotifications.h: interface for the CXTPDatePickerData class.
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
#if !defined(_XTPDatePickerNotifications_H__)
#define _XTPDatePickerNotifications_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE

/////////////////////////////////////////////////////////////////////////////
#include "Common/XTPNotifyConnection.h"
#include "XTPCalendarDefines.h"

//==== CXTPDatePickerControl notification IDs ===============================



// ----------------------------------------------------------------------
// Summary:
//     This is a notification ID that is used to indicate mouse button
//     clicks on a control.
// Remarks:
//     There are two DWORD parameters that are reserved for user defined purposes.
//     An example is to use the first user defined parameter as a button ID.
//     wParam = ButtonID, lParam = 0;
//     The second parameter is not used.  This example is useful for sending
//     notifications when a button is clicked.
// Example:
// <code>
// // In the header file.
// // Declare the xtp sink macro.  This creates the sink and associates the sink
// // with the CXTPCalendarController object.
//
// DECLARE_XTPSINK(CXTPCalendarController, m_Sink);
//
// // Declare the member function that handles the event notification messages.
// void OnEvent_DatePicker(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);
//
// // In the source file.
// // Declare the message handler inside the sink macros.
//
// // m_Sink.Advise(ptrDPConn, XTP_NC_DATEPICKERBUTTONCLICKED, &CXTPCalendarController::OnEvent_DatePicker);
//
// // Somewhere in your program in a function that is used to capture mouse click events.
// // Send an XTP_NC_DATEPICKERBUTTONCLICKED notification message with
// // the accompanying button ID as a parameter.
//
// XTP_NC_DATEPICKER_BUTTON nm; // Structure used to send notification messages.
// nm.nID = nID;                // Get the ID of the button.
//
// SendNotification(XTP_NC_DATEPICKERBUTTONCLICKED, nID);
// </code>
// See Also:
//     DECLARE_XTPSINK
// ----------------------------------------------------------------------
static const XTP_NOTIFY_CODE    XTP_NC_DATEPICKERBUTTONCLICKED      = (WM_XTP_CALENDAR_BASE + 40);

// ----------------------------------------------------------------------
// Summary:
//     This is a notification ID that is used to indicate that the selection has changed.
// Remarks:
//     There are two DWORD parameters that are reserved for user defined purposes.
// Example:
// <code>
// // In the header file.
// // Declare the xtp sink macro.  This creates the sink and associates the sink
// // with the CXTPCalendarController object.
//
// DECLARE_XTPSINK(CXTPCalendarController, m_Sink);
//
// // Declare the member function that handles the event notification messages.
// void OnEvent_DatePicker(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);
//
// // In the source file.
// // Declare the message handler inside the sink macros.
//
// m_Sink.Advise(ptrDPConn, XTP_NC_DATEPICKERSELECTIONCHANGED, &CXTPCalendarController::OnEvent_DatePicker);
//
// // Somewhere in your program in a function that is used to capture mouse click events.
// // Send an XTP_NC_DATEPICKERSELECTIONCHANGED notification message.
//
// SendNotification(XTP_NC_DATEPICKERSELECTIONCHANGED);
// </code>
// See Also:
//     DECLARE_XTPSINK
// ----------------------------------------------------------------------
static const XTP_NOTIFY_CODE    XTP_NC_DATEPICKERSELECTIONCHANGED   = (WM_XTP_CALENDAR_BASE + 41);

// ----------------------------------------------------------------------
// Summary:
//     This is a notification ID that is used to indicate that the
//     Date Picker is before run in modal mode (pop-up window),
//     window has already created.
// Remarks:
//     There are two DWORD parameters that are reserved for user defined purposes.
// Example:
// <code>
// // In the header file.
// // Declare the xtp sink macro.  This creates the sink and associates the sink
// // with the CXTPCalendarController object.
//
// DECLARE_XTPSINK(CXTPCalendarController, m_Sink);
//
// // Declare the member function that handles the event notification messages.
// void OnEvent_DatePicker(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);
//
// // In the source file.
// // Declare the message handler inside the sink macros.
//
// m_Sink.Advise(ptrDPConn, XTP_NC_DATEPICKERBEFOREGOMODAL, &CXTPCalendarController::OnEvent_DatePicker);
//
// // Somewhere in your program in a function that is used to capture mouse click events.
// // Send an XTP_NC_DATEPICKERSELECTIONCHANGED notification message.
//
// SendNotification(XTP_NC_DATEPICKERBEFOREGOMODAL);
// </code>
// See Also:
//     DECLARE_XTPSINK;
// ----------------------------------------------------------------------
static const XTP_NOTIFY_CODE    XTP_NC_DATEPICKERBEFOREGOMODAL      = (WM_XTP_CALENDAR_BASE + 42);

// ----------------------------------------------------------------------
// Summary:
//     This is a notification ID that is used to indicate mouse move under
//     a control.
// Remarks:
//     There are two DWORD parameters that are reserved for user defined purposes.
//     An example is to use the first user parameters as traced coordinates.
//     wParam = x, lParam = y;
//     The second parameter is not used.  This example is useful for sending
//     notifications when a button is clicked.
// Example:
// <code>
// // In the header file.
// // Declare the xtp sink macro.  This creates the sink and associates the sink
// // with the CXTPCalendarController object.
//
// DECLARE_XTPSINK(CXTPCalendarController, m_Sink);
//
// // Declare the member function that handles the event notification messages.
// void OnEvent_DatePicker(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);
//
// // In the source file.
// // Declare the message handler inside the sink macros.
//
// // m_Sink.Advise(ptrDPConn, XTP_NC_DATEPICKERMOUSEMOVE, &CXTPCalendarController::OnEvent_DatePicker);
//
// // Somewhere in your program in a function that is used to capture mouse move events.
// // Send an XTP_NC_DATEPICKERMOUSEMOVE notification message with
// // the accompanying mouse coordinates as parameters.
//
// </code>
// See Also:
//     DECLARE_XTPSINK
// ----------------------------------------------------------------------
static const XTP_NOTIFY_CODE XTP_NC_DATEPICKERMOUSEMOVE = (WM_XTP_CALENDAR_BASE + 43);

// ----------------------------------------------------------------------
// Summary:
//     This is a notification ID that is used to indicate that the
//     Date Picker is populated.
// Remarks:
//     Date picker is populating when months scrolled, month count changed, or
//     some other properties and settings where changed.
// Example:
// <code>
// // In the header file.
// // Declare the xtp sink macro.  This creates the sink and associates the sink
// // with the CXTPCalendarController object.
//
// DECLARE_XTPSINK(CXTPCalendarController, m_Sink);
//
// // Declare the member function that handles the event notification messages.
// void OnEvent_DatePicker(XTP_NOTIFY_CODE Event, WPARAM wParam, LPARAM lParam);
//
// // In the source file.
// // Declare the message handler inside the sink macros.
//
// m_Sink.Advise(ptrDPConn, XTP_NC_DATEPICKERMONTHCHANGED, &CXTPCalendarController::OnEvent_DatePicker);
//
// </code>
// See Also:
//     DECLARE_XTPSINK;
// ----------------------------------------------------------------------
static const XTP_NOTIFY_CODE XTP_NC_DATEPICKERMONTHCHANGED = (WM_XTP_CALENDAR_BASE + 44);

//---------------------------------------------------------------------------
// Summary:  This notification is used to get day item metrics.
//           <b>Sender</b> - CXTPDatePickerControl.
// Remarks:
//           It is sent before a CallBack function call.
//      <b>Parameters</b>
//           wParam - [in] A day date as XTP_DATE_VALUE;
//           lParam - [in/out] A pointer to day metrics as XTP_DAYITEM_METRICS*.
//
// See Also: CXTPDatePickerControl::GetDayMetrics,
//           CXTPDatePickerControl::SetCallbackDayMetrics.
//---------------------------------------------------------------------------
static const XTP_NOTIFY_CODE XTP_NC_DATEPICKERGETDAYMETRICS = (WM_XTP_CALENDAR_BASE + 45);
/////////////////////////////////////////////////////////////////////////////
#endif // !defined(_XTPDatePickerNotifications_H__)
