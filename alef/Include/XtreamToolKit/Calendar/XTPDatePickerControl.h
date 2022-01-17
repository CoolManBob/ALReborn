// XTPDatePickerControl.h: interface for the CXTPDatePickerControl class.
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
#if !defined(_XTPDATEPICKERCONTROL_H__)
#define _XTPDATEPICKERCONTROL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE
// XTPDatePickerControl.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CXTPDatePickerControl window

#include "Common/XTPNotifyConnection.h"

class CXTPDatePickerPaintManager;
class CXTPDatePickerItemMonth;
class CXTPDatePickerDaysCollection;
class CXTPDatePickerList;
class CXTPDatePickerControl;


//===========================================================================
// Summary:
//     This structure represents a set of parameters which are
//     used to display day items.
//     1. There could be customized foreground and background colors.
//     2. Also you can text font for the displayed day text.
//     3. And as the last thing you can customize a day picture displayed there,
//        using OLE PICTUREDISP interface and OleSetPicture method.
//===========================================================================
struct _XTP_EXT_CLASS XTP_DAYITEM_METRICS : public CXTPCmdTarget
{
	//-----------------------------------------------------------------------
	// Summary:
	//     Get day item font..
	// Remarks:
	//     Return a day font stored by the structure.
	//-----------------------------------------------------------------------
	CFont* GetFont();
	//-----------------------------------------------------------------------
	// Summary:
	//     Set a new font for the day item.
	// Parameters:
	//  pFont - Pointer to a CFont object.
	//-----------------------------------------------------------------------
	void SetFont(CFont* pFont);

	COLORREF clrForeground; // A COLORREF object that contains the item's foreground color.
	COLORREF clrBackground; // A COLORREF object that contains the item's background color.

//{{AFX_CODEJOCK_PRIVATE
	XTP_DAYITEM_METRICS();
	virtual ~XTP_DAYITEM_METRICS(){};

private:
	CFont m_fntText;        // A CFont object that contains the item's text font.

//}}AFX_CODEJOCK_PRIVATE

};


//===========================================================================
// Summary:
//     Structure used to send button notification messages.
// Remarks:
//     This structure represents a set of parameters which are sent
//     to the Main window in a WM_NOTIFY message from DatePicker control
//     and notify the main window when a button event occurs.
//===========================================================================
struct XTP_NC_DATEPICKER_BUTTON {
	NMHDR hdr;  // Contains information about a notification message.
	int nID;    // Date picker button ID (could be XTP_IDS_DATEPICKER_TODAY or XTP_IDS_DATEPICKER_NONE)
};

//---------------------------------------------------------------------------
// Summary:
//     Callback function definition for the IsITEMMETRICS function.
// Remarks:
//     First three arguments are required. The pUserData can be NULL.
//     pDatePicker     - is CXTPDatePickerControl*   type to provide DatePicker
//                       control pointer.
//     rDateTime       - is COleDateTime reference type to provide date time
//                       storage object.
//     pDayItemMetrics - is XTP_DAYITEM_METRICS* pointer type to provide display
//                       metrics.
//     pUserData       - is void* pointer to any additional user's defined parameter,
//                       this value can be NULL.
//---------------------------------------------------------------------------
typedef void (CALLBACK* PFNITEMMETRICS)(CXTPDatePickerControl* pDatePicker, const COleDateTime& rDateTime, XTP_DAYITEM_METRICS* pDayItemMetrics, void* pUserData);

//{{AFX_CODEJOCK_PROVATE
const UINT XTP_NC_DATEPICKER_BUTTON_CLICK      = (NM_FIRST - 50);  // Notify ID to parent window.
const UINT XTP_NC_DATEPICKER_SELECTION_CHANGED = (NM_FIRST - 51);  // Notify ID to parent window.
const UINT XTP_DATEPICKER_TIMERID              = 255;              // Date picker timer ID.
const UINT XTP_DATEPICKER_TIMER_INTERVAL       = 200;              // Date picker timer interval.
const UINT XTP_SELECTION_INFINITE              = (UINT)-1;         // Infinite number of selected days.
//}}AFX_CODEJOCK_PROVATE

// ----------------------------------------------------------------------
// Summary:
//     Border style of Date picker control
// ----------------------------------------------------------------------
enum XTPDatePickerBorderStyle
{
	xtpDatePickerBorderNone,             // No border.
	xtpDatePickerBorder3D,               // 3D border.
	xtpDatePickerBorderOffice,           // Office-like border
	xtpDatePickerBorderStatic            // Static border.
};

// ----------------------------------------------------------------------
// Summary:
//     This class represents a DatePicker button.
// Remarks:
//     CXTPDatePickerButton provides storage for all parameters needed to
//     display and handle a button.
// ----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPDatePickerButton
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default button constructor.
	// Remarks:
	//     Default button constructor handles all necessary
	//     initialization.
	//-----------------------------------------------------------------------
	CXTPDatePickerButton();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDatePickerButton object, handles cleanup
	//     and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDatePickerButton()
	{
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Get a button caption.
	// Remarks:
	//     Load from resources string using ID stored in m_nID member or
	//     value which was set by SetCaption() call.
	// Returns:
	//     Returns a button caption.
	// See Also: m_nID, SetCaption
	//-----------------------------------------------------------------------
	virtual CString GetCaption();

	//-----------------------------------------------------------------------
	// Summary:
	//     Set a button caption.
	// Parameters:
	//     pcszCaption - A caption string.
	// Remarks:
	//     If pcszCaption is empty string - GetCaption will the m_nID to load
	//     caption string from resources.
	// See Also: GetCaption, m_nID
	//-----------------------------------------------------------------------
	virtual void SetCaption(LPCTSTR pcszCaption);

public:
	int m_nID;              // -------------------------------------------------------------
	                        // This member variable is used to specify the identifier of the
	                        // button. Also it used as resource ID for a caption string.
	                        // -------------------------------------------------------------
	CRect m_rcButton;       // --------------------------------------------------------------
	                        // This member variable is used to specify the bounding rectangle
	                        // coordinates of a button.
	                        // --------------------------------------------------------------
	BOOL m_bVisible;        // ----------------------------------------------------------------
	                        // This member variable is used to indicate if a button is visible.
	                        // ----------------------------------------------------------------

	BOOL m_bPressed;        // ----------------------------------------------------------------
	                        // This member variable is used to indicate if a button is pressed.
	                        // ----------------------------------------------------------------
	BOOL m_bHighlight;      // -------------------------------------------------------
	                        // This member variable is used to indicate if a button is
	                        // highlighted.
	                        // -------------------------------------------------------
protected:
	CString m_strCaption;   // ------------------------------------------------------------------
	                        // This member variable is used to specify the caption of the button.
	                        // ------------------------------------------------------------------
};

// ----------------------------------------------------------------------
// Summary:
//     Class CXTPDatePickerButtons represents a collection of buttons.
// Remarks:
//     CXTPDatePickerButtons inherits most of the behavior of the
//     standard MFC CArray. The DatePicker control can display and handle
//     any number of buttons. CXTPDatePickerButtons provides the basic
//     functionality to handle the collection.<p/>
// Create a CXTPDatePickerButtons object by calling the default
//     constructor without parameters. Furthermore, add buttons to
//     collection using standard CArray member functions.
// See Also:
//     CArray
// ----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPDatePickerButtons : public CArray<CXTPDatePickerButton*, CXTPDatePickerButton*>
{
public:

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the current count of visible
	//     buttons.
	// Returns:
	//     Integer value that contains the current number of visible buttons
	//     that are in the collection.
	// ----------------------------------------------------------------------
	int GetVisibleButtonCount() const;

	// ------------------------------------------------------------------
	// Summary:
	//     This member function finds buttons by their ID property.
	// Remarks:
	//     Walks through the collection and looks for the button with the
	//     provided ID.
	// Returns:
	//     A pointer to the CXTPDatePickerButton structure. NULL if no
	//     matching button ID is found.
	// Parameters:
	//     nID :  An int that contains the ID to search for.
	// ------------------------------------------------------------------
	CXTPDatePickerButton* Find(int nID) const;

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function determines which button is affected by the
	//     mouse pointer.
	// Parameters:
	//     point :  Point to test.
	// Remarks:
	//     Call this member function to test a mouse event occurring on one
	//     of the buttons rectangle areas. If no buttons are found then NULL
	//     is returned.
	// Returns:
	//     The DatePicker button at the specified position, if any, otherwise
	//     NULL.
	// ----------------------------------------------------------------------
	CXTPDatePickerButton* HitTest(CPoint point) const;
};


// ----------------------------------------------------------------------
// Summary:
//     Class CXTPDatePickerControl encapsulates the functionality of a
//     date and time picker control.
// Remarks:
//     A CXTPDatePickerControl object implements a calendar-like user
//     interface. This provides the user with a very intuitive and
//     recognizable method of entering or selecting a date. The control
//     also provides the application with the means to obtain and set the
//     date information in the control using existing data types. By
//     default, the date picker control displays the current day and
//     month. However, the user is able to scroll to the previous and
//     next months and select a specific month and/or year. The user can
//     select a date or a range of dates. Also, the user can change the
//     display by:
//
//     * Scrolling backward and forward, from month to month.
//     * Clicking the Today text to select the current day.
//     * Clicking the None text to remove the selection.
//     * Picking a month or a year from a pop-up menu.
//
// The date picker control is customize by applying a variety of
//     additional settings. Look at the control's class members list to
//     see available customization options.<p/>
// The date picker control can display more than one month, and it
//     can indicate special days (such as holidays) by modifying the
//     date's style. Furthermore, CXTPDatePickerControl contains methods
//     similar to the MFC CMonthCalCtrl. Use the methods in the same way
//     as the MFC CMonthCalCtrl.
// See Also:
//     CMonthCalCtrl
// ----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPDatePickerControl : public CWnd
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPDatePickerItemMonth;
	friend class CXTPDatePickerDaysCollection;
	friend class CXTPDatePickerTheme;
	//}}AFX_CODEJOCK_PRIVATE

public:
	//===========================================================================
	// Summary:
	//     Enumeration of operational mouse modes.
	// Remarks:
	//     DatePickerControl handles several Mouse states.
	//     This enumeration helps to clearly identify each of these states.
	//===========================================================================
	enum DatePickerMouseMode
	{
		mouseNothing,            // No mouse activity by the user.
		mouseScrollingLeft,      // User pressed left scroll triangle and is holding the button.
		mouseScrollingRight,     // User pressed right scroll triangle and is holding the button.
		mouseSelecting,          // User pressed left mouse down on unselected day item and is now selecting a range of day items.
		mouseDeselecting,        // User pressed left mouse down on selected day item and is now de-selecting a range of day items.
		mouseTrackingHeaderList  // User is tracking pop-up month list window.
	};
// Construction
public:
	// -------------------------------------------------------------------------------------------------------------
	// Summary:
	//     Default constructor is used to construct a CXTPDatePickerControl
	//     \object.
	// Remarks:
	//     Construct a CXTPDatePickerControl object in two steps. First, call
	//     the constructor CXTPDatePickerControl and then call the Create
	//     method, which initializes the window.
	// Example:
	// <code>
	// // Declare a local CXTPDatePickerControl object.
	// CXTPDatePickerControl myDatePicker;
	//
	// // Declare a dynamic CXTPDatePickerControl object.
	// CXTPDatePickerControl* pMyDatePicker = new CXTPDatePickerControl();
	//
	// // Create a window
	// if (!myDatePicker.Create(WS_CHILD | WS_TABSTOP | WS_VISIBLE, CRect(0, 0, 200, 200), this, ID_DATEPICKER_CONTROL))
	// {
	//     TRACE(_T("Failed to create the date picker window\\n"));
	// }
	// </code>
	// See Also:
	//     Create
	// -------------------------------------------------------------------------------------------------------------
	CXTPDatePickerControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDatePickerControl object, handles cleanup
	//     and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDatePickerControl();

public:
	// -----------------------------------------------------------------------
	// Summary:
	//     This member function is used to create the Date Picker Control
	//     Window.
	// Parameters:
	//     dwStyle :     Specifies the window style attributes.
	//     rect :        A RECT object that contains the coordinates of the
	//                   window, in the client coordinates of pParentWnd.
	//     pParentWnd :  A CWnd pointer to the parent window.
	//     nID :         A UINT that contains the ID of the child window.
	//     pContext :    A CCreateContext pointer that contains the create context
	//                   of the window.
	// Returns:
	//     Nonzero if successful. Otherwise 0.
	// -----------------------------------------------------------------------
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function implements pop-up window behavior for the
	//     Date Picker control.
	// Parameters:
	//     rect :        Rectangle that contains the size and position of the
	//                   window, in client coordinates of pParentWnd.
	//     pParentWnd :  Pointer to the parent window.
	// Returns:
	//     TRUE if successful, FALSE otherwise.
	// See Also:
	//     IsModal
	// ----------------------------------------------------------------------
	virtual BOOL GoModal(const RECT& rect, CWnd* pParentWnd = NULL);

	// ----------------------------------------------------------------------
	// Summary:
	//      Is the control runs in modal mode.
	// Returns:
	//      TRUE when the control is used as a pop-up window,
	//      FALSE otherwise.
	// See Also:
	//     GoModal
	// ----------------------------------------------------------------------
	virtual BOOL IsModal() const;

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function returns a pointer to the associated paint
	//     manager.
	// Remarks:
	//     Call this member function to obtain a pointer to the paint manager
	//     \object. The paint manager object is used for drawing the date
	//     picker window.
	// Returns:
	//     Pointer to the paint manager object.
	// See Also:
	//     SetTheme
	// ----------------------------------------------------------------------
	virtual CXTPDatePickerPaintManager* GetPaintManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function redraws the control windows.
	// Remarks:
	//     Call this member function to redraw all control windows
	//     according to the stored parameter values. The window will be
	//     updated immediately: CWnd::UpdateWindow() will be called.
	// See Also:
	//     _RedrawControl
	//-----------------------------------------------------------------------
	virtual void RedrawControl();

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function sets the new control drawing theme.
	// Parameters:
	//     pPaintManager :  Pointer of the new paint manager object or derived
	//                      class like CXTPDatePickerThemeOffice2007.
	//                      if NULL - the default paint manager object is set.
	// Remarks:
	//     Call this member function to set the paint manager object that is
	//     used for drawing a date picker window.
	// See Also:
	//     GetPaintManager
	// ---------------------------------------------------------------------
	virtual void SetTheme(CXTPDatePickerPaintManager* pPaintManager = NULL);

	// ---------------------------------------------------------------------
	// Summary:
	//     This function is called to determine if AutoSize mode is set.
	// Remarks:
	//     Call this member function to determine whether the control's auto
	//     size mode is enabled.
	// Returns:
	//     A boolean value that specifies is AutoSize is enabled.<P/>
	//     TRUE if AutoSize is enabled.<P/>
	//     Otherwise return FALSE.
	// See Also:
	//     SetAutoSize
	// ---------------------------------------------------------------------
	virtual BOOL IsAutoSize();

	//-----------------------------------------------------------------------
	// Summary:
	//     This function sets the AutoSize mode.
	// Parameters:
	//     bAuto - Boolean value. If bAuto is TRUE, then AutoSize
	//             is enabled. If this parameter is FALSE, then
	//             AutoSize is disabled.
	// Remarks:
	//     Set AutoSize mode to TRUE if you want the control to automatically
	//     calculate the number of month items according to control's size.
	//     When this mode is enabled and there is enough space on control's
	//     client area, the control adds a row or a column of month items
	//     and centers them both horizontally and vertically.
	// See Also: IsAutoSize
	//-----------------------------------------------------------------------
	virtual void SetAutoSize(BOOL bAuto = TRUE);

	// -------------------------------------------------------------------
	// Summary:
	//     This member function sets the user defined day metrics callback
	//     function.
	// Parameters:
	//     pFunc :   Pointer to the callback function.
	//     pParam :  Pointer to any user defined parameter provided to the
	//               callback function.
	// Remarks:
	//     Call this member function to set up the user defined callback
	//     function to manipulate the 'special day's attributes'.
	// See Also:
	//     PFNITEMMETRICS, GetCallbackDayMetrics, GetDayMetrics
	// -------------------------------------------------------------------
	virtual void SetCallbackDayMetrics(PFNITEMMETRICS pFunc, void* pParam = NULL);

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function returns the day metrics callback function.
	// Remarks:
	//     Call this member function to obtain a pointer to the user defined
	//     day metrics callback function.
	// Returns:
	//     Pointer to the callback function.
	// See Also:
	//     PFNITEMMETRICS, SetCallbackDayMetrics, GetDayMetrics
	// ---------------------------------------------------------------------
	virtual PFNITEMMETRICS GetCallbackDayMetrics();

	// --------------------------------------------------------------------
	// Summary:
	//     This member function returns the day item drawing metrics.
	// Parameters:
	//     dtDay :      Reference to the DateTime value to check.
	//     pDayMetics : Pointer to the day metrics structure.
	// Remarks:
	//     Determines whether or not a given date is special. This function
	//     calls a callback function to make this determination.
	// See Also:
	//     GetCallbackDayMetrics, SetCallbackDayMetrics
	// --------------------------------------------------------------------
	virtual void GetDayMetrics(COleDateTime& dtDay, XTP_DAYITEM_METRICS* pDayMetics);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function sets the day of the week that is displayed in
	//     the leftmost column of the calendar.
	// Parameters:
	//     nDay :  An integer value representing which day is set as the first day
	//             of the week. This value must be one of the day numbers. See
	//             GetFirstDayOfWeek for a description of the day numbers.
	// See Also:
	//     GetFirstDayOfWeek
	// ----------------------------------------------------------------------
	virtual void SetFirstDayOfWeek(int nDay);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function obtains the first day of the week that is
	//     displayed in the leftmost column of the calendar.
	// Returns:
	//     An integer value that represents the first day of the week. The
	//     days of the week are represented as integers, as follows.
	//     * 1 - Sunday
	//     * 2 - Monday
	//     * 3 - Tuesday
	//     * 4 - Wednesday
	//     * 5 - Thursday
	//     * 6 - Friday
	//     * 7 - Saturday
	// See Also:
	//     SetFirstDayOfWeek
	// ----------------------------------------------------------------------
	virtual int GetFirstDayOfWeek();

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function sets the required number of this year days
	//     in the first week of the year.
	// Parameters:
	//     nDays : An integer value representing the required number of
	//             days of the new year in the first week of the year.
	//             The value should be between 1 (default) and 7 (full week).
	// See Also:
	//     GetFirstWeekOfYearDay
	// Example:
	// <code>
	// // Setup German style of week numbers calculation.
	// pMyDatePicker->SetFirstWeekOfYearDays(4);
	// </code>
	// ----------------------------------------------------------------------
	virtual void SetFirstWeekOfYearDays(int nDays);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function obtains the current required number of year days
	//     in the first week of the year.
	// Returns:
	//     An integer value that represents the required number of
	//     days of the new year in the first week of this year.
	//     The value should be between 1 (by default) and 7 (full week).
	// See Also:
	//     SetFirstWeekOfYearDays
	// ----------------------------------------------------------------------
	virtual int GetFirstWeekOfYearDays();

	// ----------------------------------------------------------------------
	// Summary:
	//     This function sets visibility for the "Today" and "None" buttons.
	// Parameters:
	//     bShowToday :  Provide TRUE to show the "Today" button, or FALSE to
	//                   hide it.
	//     bShowNone :   Provide TRUE to show the "None" button, or FALSE to
	//                   hide it.
	// Remarks:
	//     Call this member function to hide or show any of the control's
	//     service buttons.
	// See Also:
	//     IsTodayButtonVisible, IsNoneButtonVisible
	// ----------------------------------------------------------------------
	virtual void SetButtonsVisible(BOOL bShowToday, BOOL bShowNone);

	// ------------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the value of the visibility
	//     flag for the "Today" button.
	// Remarks:
	//     Call this member function to determine the visibility of the
	//     "Today" service button.
	// Returns:
	//     A boolean value that specifies if the "Today" button is visible.<p/>
	// TRUE if the "Today" button is visible.<p/>
	// Otherwise FALSE.
	// See Also:
	//     IsNoneButtonVisible, SetButtonsVisible
	// ------------------------------------------------------------------------
	virtual BOOL IsTodayButtonVisible();

	// -----------------------------------------------------------------------
	// Summary:
	//     This function returns the boolean visibility flag of the "None"
	//     button.
	// Remarks:
	//     Call this member function to determine the visibility of the
	//     "None" service button.
	// Returns:
	//     A boolean value that specifies if the "None" button is visible.<p/>
	//     TRUE when the "None" button is visible.<p/>
	//     Otherwise FALSE.
	// See Also:
	//     IsTodayButtonVisible, SetButtonsVisible
	// -----------------------------------------------------------------------
	virtual BOOL IsNoneButtonVisible();

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to enable or disable highlighting of the
	//     "Today" cell.
	// Parameters:
	//     bValue :  Boolean value determines if the "Today" cell is
	//               highlighted.
	// Remarks:
	//     The "Today" cell is determined and changed using the SetToday and
	//     GetToday member functions. When highlighting is enabled, the cell
	//     is surrounded by a rectangle frame of the specific color.
	// See Also:
	//     GetHighlightToday, SetToday, GetToday,
	//     CXTPDatePickerPaintManager::GetHighlightTodayColor
	// ----------------------------------------------------------------------
	virtual void SetHighlightToday(BOOL bValue);

	// -------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine whether the "Today" cell
	//     highlighting is enabled or disabled.
	// Remarks:
	//     This member function returns a boolean flag that specifies the
	//     "Today" button highlighting mode.
	// Returns:
	//     A BOOL that indicates if the "Today" cell is highlighted.<p/>
	//     TRUE if the "Today" cell is highlighted. Otherwise FALSE.
	// See Also:
	//     GetHighlightToday, SetToday, GetToday,
	//     CXTPDatePickerPaintManager::GetHighlightTodayColor
	// -------------------------------------------------------------------
	virtual BOOL GetHighlightToday();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set right-to-left mode.
	// Parameters:
	//     bRightToLeft - TRUE to set right-to-left reading-order properties.
	//-----------------------------------------------------------------------
	virtual void SetLayoutRTL(BOOL bRightToLeft);

	// ------------------------------------------------------------------------
	// Summary:
	//     Call this member function to show or hide the week numbers.
	// Parameters:
	//     bValue :  Boolean value determines whether week numbers are visible.
	// Remarks:
	//     Week numbers are shown for each week in each month at the very
	//     left column of the month, separated by the vertical line from the
	//     \other week days. This member function controls the visibility
	//     mode of the week numbers.
	// See Also:
	//     GetShowWeekNumbers,
	//     CXTPDatePickerPaintManager::GetWeekNumbersBackColor,
	//     CXTPDatePickerPaintManager::GetWeekNumbersTextColor,
	//     CXTPDatePickerPaintManager::GetWeekNumbersTextFont
	// ------------------------------------------------------------------------
	virtual void SetShowWeekNumbers(BOOL bValue);

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the week numbers are
	//     visible.
	// Remarks:
	//     The week numbers are shown for each week in each month as the very
	//     left column of the month, separated by the vertical line from the
	//     \other week days. This member function determines if the week
	//     numbers are shown.
	// Returns:
	//     Boolean value that determines if the week numbers are set to be
	//     shown.<p/>
	// TRUE - If the week numbers are visible.<p/>
	// FALSE - If the week numbers are not visible.
	// See Also:
	//     SetShowWeekNumbers,
	//     CXTPDatePickerPaintManager::GetWeekNumbersBackColor,
	//     CXTPDatePickerPaintManager::GetWeekNumbersTextColor,
	//     CXTPDatePickerPaintManager::GetWeekNumbersTextFont
	// ----------------------------------------------------------------------
	virtual BOOL GetShowWeekNumbers();

	// -------------------------------------------------------------------
	// Summary:
	//     Call this member function to show or hide non-month days.
	// Parameters:
	//     bShow :  Boolean value determines if the non month days are
	//              shown.
	//              TRUE  If the non\-month days are visible.
	//              FALSE If the non\-month days are not visible.
	// Remarks:
	//     Non-month days are shown in a special color or font. Non-month
	//     days are shown just before and after regular days of a specific
	//     month.
	// Note:
	//     For a control showing more than 1 month, non-month days will be
	//     shown before the first month, and after the last one.
	// See Also:
	//     GetShowNonMonthDays,
	//     CXTPDatePickerPaintManager::GetNonMonthDayBackColor,
	//     CXTPDatePickerPaintManager::GetNonMonthDayTextColor,
	//     CXTPDatePickerPaintManager::GetNonMonthDayTextFont
	// -------------------------------------------------------------------
	virtual void SetShowNonMonthDays(BOOL bShow);

	// ---------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the non-month days are
	//     visible.
	// Remarks:
	//     Non-month days are shown in a special color or font just before
	//     and after regular days of a specific month.
	// Note:
	//     For a control showing more than 1 month, non-month days are shown
	//     before the first month, and after the last month.
	// Returns:
	//     A boolean value that specifies if the "non-month days" are
	//     visible.<p/>
	//     TRUE if non-month days are visible.<p/>
	//     FALSE otherwise.
	// See Also:
	//     SetShowNonMonthDays,
	//     CXTPDatePickerPaintManager::GetNonMonthDayBackColor,
	//     CXTPDatePickerPaintManager::GetNonMonthDayTextColor,
	//     CXTPDatePickerPaintManager::GetNonMonthDayTextFont
	// ---------------------------------------------------------------------
	virtual BOOL GetShowNonMonthDays();

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set borders style
	//     around the control.
	// Parameters:
	//     borderStyle - XTPDatePickerBorderStyle value determines whether border style
	// See Also:
	//     GetBorderStyle, CXTPDatePickerPaintManager::DrawBorder, XTPDatePickerBorderStyle
	// ----------------------------------------------------------------------
	virtual void SetBorderStyle(XTPDatePickerBorderStyle borderStyle);

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the border is
	//     visible.
	// Returns:
	//     XTPDatePickerBorderStyle value that specifies the border style.<p/>
	// See Also:
	//     SetBorderStyle, CXTPDatePickerPaintManager::DrawBorder, XTPDatePickerBorderStyle
	// ----------------------------------------------------------------------
	virtual XTPDatePickerBorderStyle GetBorderStyle() const;

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the number of months in the
	//     control's grid.
	// Parameters:
	//     nRows :  An int that contains the new rows count value.
	//     nCols :  An int that contains the new columns count value.
	// Remarks:
	//     This member function is called in the AutoSize mode. It sets the
	//     new number of columns and rows in the control's months grid. Also,
	//     the function redraws each month within the existing control's
	//     coordinates, reducing or enlarging the size of each month item as
	//     necessary.
	// Note:
	//     This function does nothing when AutoSize mode is enabled.
	// See Also:
	//     GetRows, GetCols
	// ----------------------------------------------------------------------
	virtual void SetGridSize(int nRows, int nCols);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the number of rows in the
	//     months grid.
	// Returns:
	//     The number of rows in the control's months grid.
	// See Also: GetCols, SetGridSize
	//-----------------------------------------------------------------------
	virtual int GetRows();

	// ----------------------------------------------------------------------
	// Summary:
	//     Call this member function to obtain the number of columns in the
	//     months grid.
	// Returns:
	//     An int that contains the number of columns in the control's months
	//     grid.
	// See Also:
	//     GetRows, SetGridSize
	// ----------------------------------------------------------------------
	virtual int GetCols();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function finds the month item at the provided point.
	// Parameters:
	//     ptMouse - The mouse coordinates to test.
	//     pMCHitTest - A pointer to a <b>MCHITTESTINFO</b> structure
	//                  containing hit testing points for the
	//                  date picker control.
	// Remarks:
	//     Call this function to retrieve a pointer to the month item
	//          at the specified position.
	//
	//     This HitTest(PMCHITTESTINFO pMCHitTest) function determines which date portion of the
	//     picker control, if any, is at a specified position.  It implements the behavior of the
	//     CMonthCalCtrl::HitTest function.
	// Returns:
	//     A pointer to the month item where the mouse was clicked.
	//     Returns NULL if the mouse was clicked outside the months grid area.
	//
	//     HitTest(PMCHITTESTINFO pMCHitTest) returns a DWORD value. Equal to the uHit member of the MCHITTESTINFO structure.
	// See Also: HitTest, CMonthCalCtrl::HitTest, MCHITTESTINFO
	//-----------------------------------------------------------------------
	virtual CXTPDatePickerItemMonth* HitTest(CPoint ptMouse);
	virtual DWORD HitTest(PMCHITTESTINFO pMCHitTest); // <combine CXTPDatePickerControl::HitTest@CPoint>

	// --------------------------------------------------------------------------
	// Summary:
	//     This member function scrolls the control's months grid to the left
	//     by the specified number of months.
	// Parameters:
	//     nMonthCount :  Integer value that specifies how many months to scroll.
	//                    The default value is 1.
	// Remarks:
	//     Call this member function to scroll the months grid to the left by
	//     nMonthCount positions.
	// See Also:
	//     ScrollRight
	// --------------------------------------------------------------------------
	virtual void ScrollLeft(int nMonthCount = 1);

	// --------------------------------------------------------------------------
	// Summary:
	//     This member function scrolls the control's months grid to the
	//     right by the specified amount of months.
	// Parameters:
	//     nMonthCount :  Integer value that specifies how many months to scroll.
	//                    The default value is 1.
	// Remarks:
	//     Call this member function to scroll the months grid to the right
	//     by nMonthCount positions.
	// See Also:
	//     ScrollLeft
	// --------------------------------------------------------------------------
	virtual void ScrollRight(int nMonthCount = 1);

	// -----------------------------------------------------------------------------------
	// Summary:
	//     Call this member function to send notifications to the parent
	//     window.
	// Parameters:
	//     nMessage :  An int that contains the identifier of the message. See remarks
	//                 section for a list of valid values.
	// pNMHDR :    Pointer to the XTP_NC_DATEPICKER_BUTTON notification
	//                 structure.
	// Remarks:
	//     The following values are valid for nMessage:
	//     * XTP_NC_DATEPICKER_BUTTON_CLICK - Notifies that one of date
	//           picker buttons was clicked.
	//     * XTP_NC_DATEPICKER_SELECTION_CHANGED - Notifies that the date
	//           picker selection has changed.
	// This function notifies the parent window that something happened.
	//     For the example of how to catch these messages see below.
	// Example:
	// <code>
	// // First, add the message handler to your message map.
	// ON_NOTIFY(XTP_NC_DATEPICKER_BUTTON_CLICK, 1000, OnButtonPressed)
	//
	// // Then implement the message handler like the example below
	// void YourDlg::OnButtonPressed(NMHDR* pNotifyStruct, LRESULT*)
	// {
	//     XTP_NC_DATEPICKER_BUTTON* pNMButton = (XTP_NC_DATEPICKER_BUTTON*)pNotifyStruct;
	//
	//     switch (pNMButton-\>nID)
	//     {
	//         case XTP_IDS_DATEPICKER_TODAY:
	//             TRACE(_T("TODAY BUTTON PRESSEDn"));
	//             break;
	//         case XTP_IDS_DATEPICKER_NONE:
	//             TRACE(_T("NONE BUTTON PRESSEDn"));
	//             break;
	//     }
	// }
	// </code>
	// Returns:
	//     An LRESULT that is used to indicate the success of the function. Non-zero
	//     if the message is sent successfully, zero otherwise.
	// -----------------------------------------------------------------------------------
	virtual LRESULT SendMessageToParent(int nMessage, NMHDR* pNMHDR = NULL);

	// ---------------------------------------------------------------------
	// Summary:
	//     This function checks a specific date to determine if it is
	//     contained in the control's selection.
	// Parameters:
	//     dtDay :  A date to check.
	// Remarks:
	//     Call this member function to determine whether dtDay is selected.
	// Returns:
	//     A boolean value that specifies if the date is contained in the
	//     current selection.<p/>
	//     TRUE when the day is selected.<p/>
	//     Otherwise FALSE.
	// See Also:
	//     Select, Deselect
	// ---------------------------------------------------------------------
	virtual BOOL IsSelected(const COleDateTime& dtDay) const;

	// ---------------------------------------------------------------------
	// Summary:
	//     This function checks a specific date to determine if it is
	//     focused.
	// Parameters:
	//     dtDay :  A date to check.
	// Remarks:
	//     Focused item is used only for the keyboard navigation and selecting.
	//     It is initialized by keyboard using first day of the current
	//     selection or if selection is empty the first day of the first
	//     visible month is used.
	//     Focused item is cleared by mouse or by set selection externally.
	// Returns:
	//     TRUE when the day is focused, FALSE otherwise.
	// See Also:
	//     ClearFocus
	// ---------------------------------------------------------------------
	virtual BOOL IsFocused(const COleDateTime& dtDay) const;

	// ---------------------------------------------------------------------
	// Summary:
	//     This function clear focused attribute if it is set.
	// Remarks:
	//     Focused item is used only for the keyboard navigation and selecting.
	//     It is initialized by keyboard using first day of the current
	//     selection or if selection is empty the first day of the first
	//     visible month is used.
	//     Focused item is cleared by mouse or by set selection externally.
	// See Also:
	//     IsFocused
	// ---------------------------------------------------------------------
	virtual void ClearFocus();

	// -----------------------------------------------------------------
	// Summary:
	//     Call this member function to select a specific day.
	// Parameters:
	//     dtDay :  Date to be selected.
	// Remarks:
	//     This function determines if one more item can be added to the
	//     collection of selected days. If yes, then the selected day is
	//     added to the list of selected items.
	// See Also:
	//     Deselect, IsSelected
	// -----------------------------------------------------------------
	virtual void Select(const COleDateTime& dtDay);

	// --------------------------------------------------------------------
	// Summary:
	//     Call this member function to de-select the specified day.
	// Parameters:
	//     dtDay :  Date to be deselected.
	// Remarks:
	//     This member function removes the provided day from the control's
	//     selected days collection.
	// See Also:
	//     Select, IsSelected
	// --------------------------------------------------------------------
	virtual void Deselect(const COleDateTime& dtDay);

	// ---------------------------------------------------------------------
	// Summary:
	//     Call this member function to ensure that the COleDateTime item is
	//     visible.
	// Parameters:
	//     dtDate :  The date to test.
	// Remarks:
	//     If necessary, the function scrolls the month grid on the control
	//     to make the day visible.
	// See Also:
	//     EnsureVisibleSelection
	// ---------------------------------------------------------------------
	virtual void EnsureVisible(const COleDateTime& dtDate);

	// ---------------------------------------------------------------------
	// Summary:
	//     Call this member function to ensure that the selection range is
	//     visible.
	// Remarks:
	//     If necessary, the function scrolls the month grid on the control
	//     to make all selected days visible.
	// See Also:
	//     EnsureVisible, EnsureVisibleFocus
	// ---------------------------------------------------------------------
	virtual void EnsureVisibleSelection();

	// Summary:
	//     Call this member function to ensure that the focused item is visible.
	// Remarks:
	//     Focused item is used only for the keyboard navigation and selecting.
	//     It is initialized by keyboard using first day of the current
	//     selection or if selection is empty the first day of the first
	//     visible month is used.
	//     Focused item is cleared by mouse or by set selection externally.
	// See Also:
	//     EnsureVisible, EnsureVisibleSelection
	// ---------------------------------------------------------------------
	virtual void EnsureVisibleFocus();

	// ------------------------------------------------------------------------------
	// Summary:
	//     This member function sets the user-defined month name.
	// Parameters:
	//     nMonth :    An int that contains the Month number corresponding to the
	//                 month name. i.e. 1 = January, 2 = February, 3 = March, ...
	//                 Valid values are from 1 to 12.
	// strMonthName :  A CString that contains the New month name.
	// Remarks:
	//     Use this function to change the shown month name to any preferred
	//     \or localized values.
	// See Also:
	//     GetMonthName, SetDayOfWeekName, GetDayOfWeekName
	// ------------------------------------------------------------------------------
	void SetMonthName(int nMonth, LPCTSTR strMonthName);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function returns the month name of the given integer
	//     value.
	// Parameters:
	//     nMonth :  Month number. The valid range is 1 \<= nMonth \>= 12.
	// Remarks:
	//     Call this member function to retrieve the name of the month which
	//     was previously set by SetMonthName function. If month name was not
	//     set by SetMonthName function, it will return the default value,
	//     which is the default localized month name.
	// Returns:
	//     If nMonth is a valid integer value, then the function returns the
	//     name of the month. If nMonth is not valid, then the function
	//     \returns an empty string.
	// See Also:
	//     SetMonthName, SetDayOfWeekName, GetDayOfWeekName
	// ----------------------------------------------------------------------
	CString GetMonthName(int nMonth) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the user-defined DayOfWeek name.
	// Parameters:
	//     nDayOfWeek       - DayOfWeek number to set new name for.
	//     strDayOfWeekName - New DayOfWeek name.
	// Remarks:
	//     Valid day-of-week range is between 1 and 7, where
	//          1 = Sunday, 2 = Monday, and so on.
	// See Also: GetDayOfWeekName, SetMonthName, GetMonthName
	//-----------------------------------------------------------------------
	void SetDayOfWeekName(int nDayOfWeek, LPCTSTR strDayOfWeekName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns the DayOfWeek name associated
	//     with the integer value of nDayOfWeek.
	// Parameters:
	//     nDayOfWeek - DayOfWeek number.
	// Remarks:
	//     Valid day-of-week range is between 1 and 7, where
	//          1 = Sunday, 2 = Monday, and so on.
	// Returns:
	//     If nDayOfWeek is a valid integer value, then the function
	//     returns the name of the week associated with the integer value
	//     of nDayOfWeek.  If nDayOfWeek is not a valid integer value, then
	//     the function returns an empty string.
	// See Also: SetDayOfWeekName, SetMonthName, GetMonthName
	//-----------------------------------------------------------------------
	CString GetDayOfWeekName(int nDayOfWeek) const;

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves date information representing the
	//     upper and lower limits of the date range currently selected by the
	//     user.
	// Parameters:
	//     refMinRange :  A reference to a COleDateTime object containing the
	//                    minimum date allowed.
	//     refMaxRange :  A reference to a COleDateTime object containing the
	//                    maximum date allowed.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::GetSelRange function.
	// Returns:
	//     Nonzero if successful. Otherwise 0.
	// See Also:
	//     SetSelRange, CMonthCalCtrl::GetSelRange
	// ----------------------------------------------------------------------
	virtual BOOL GetSelRange(COleDateTime& refMinRange, COleDateTime& refMaxRange) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the selection for a date picker
	//     control to a given date range.
	// Parameters:
	//     refMinRange - A reference to a COleDateTime object containing the
	//                 date at the lowest end of the range.
	//     refMaxRange - A reference to a COleDateTime object containing the
	//                 date at the highest end of the range.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::SetSelRange function.
	//
	//     Please note that this method doesn't refresh the control, and in
	//     order to update control's picture you'll have to call RedrawControl
	//     method after changing a selection.
	// Returns:
	//     Nonzero if successful. Otherwise 0.
	// See Also: GetSelRange, CMonthCalCtrl::SetSelRange
	//-----------------------------------------------------------------------
	virtual BOOL SetSelRange(const COleDateTime& refMinRange, const COleDateTime& refMaxRange);

	// --------------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the minimum size required to show a
	//     full month in the date picker control or a
	//     specified number of full month items in a date picker control.
	// Parameters:
	//     pRect :  A pointer to a RECT structure that receives the bounding
	//              rectangle information. This parameter must be a valid address
	//              and cannot be NULL.
	//     nRows :  A number of months in a row inside the calculated rectangle.
	//     nCols :  A number of months in a column inside the calculated rectangle.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::GetMinReqRect function.
	//
	//     GetMinReqRect(RECT* pRect, int nRows, int nCols) retrieves the minimum
	//     size required to show a specified number of full month items in
	//     a date picker control.  The functionality implemented by this function is similar to the
	//     CMonthCalCtrl::GetMinReqRect, but instead of calculating a
	//     rectangle size for 1 month only, it can also calculate a rectangle
	//     size for any specified number of month items.
	// Returns:
	//     If successful, this member function returns nonzero and <i>pRect</i>
	//     receives the applicable bounding rectangle information. If
	//     unsuccessful, the member function returns 0.
	// See Also:
	//     CMonthCalCtrl::GetMinReqRect
	// --------------------------------------------------------------------------
	virtual BOOL GetMinReqRect(RECT* pRect) const;
	virtual BOOL GetMinReqRect(RECT* pRect, int nRows, int nCols) const; // <combine CXTPDatePickerControl::GetMinReqRect@RECT*@const>

	// ----------------------------------------------------------------------------
	// Summary:
	//     This member function adjusts the date picker control to the
	//     minimum size required to display one month.
	// Parameters:
	//     bRepaint :  A BOOL that specifies whether the control is to be
	//                 repainted. By default, TRUE. If FALSE, no repainting occurs.
	// Remarks:
	//     Calling SizeMinReq successfully displays the entire date picker
	//     control for one month's calendar.
	// Returns:
	//     Nonzero if the date picker control is sized to its minimum.
	//     Otherwise 0.
	// See Also:
	//     GetMinReqRect, CMonthCalCtrl::SizeMinReq
	// ----------------------------------------------------------------------------
	virtual BOOL SizeMinReq(BOOL bRepaint = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the scroll rate for the date
	//     picker control.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::GetMonthDelta function.
	//     The scroll rate is the number of months that the control
	//     moves its display when the user clicks a scroll button once.
	// Returns:
	//     The scroll rate for the date picker control.
	// See Also: SetMonthDelta, CMonthCalCtrl::GetMonthDelta
	//-----------------------------------------------------------------------
	virtual int GetMonthDelta() const;

	// ----------------------------------------------------------------------------
	// Summary:
	//     This member function sets the scroll rate for the date picker
	//     control.
	// Parameters:
	//     iDelta :  The number of months set as the control's scroll rate. If this
	//               value is zero, the month delta is reset to the default, which
	//               is the number of months displayed in the control.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::SetMonthDelta function.
	// See Also:
	//     GetMonthDelta, CMonthCalCtrl::SetMonthDelta
	// ----------------------------------------------------------------------------
	virtual void SetMonthDelta(int iDelta);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the system time as indicated by the
	//     currently-selected date.
	// Parameters:
	//     refDateTime :  A reference to a COleDateTime object. Receives the
	//                    currently selected time.
	// Remarks:
	//     This member function fails if more than 1 date is selected.
	// Returns:
	//     A BOOL that indicates the success of the function.<p/>
	//     TRUE if the function is successfully able to retrieve the system
	//     time.<p/>
	//     FALSE if more than one date is selected.
	// See Also:
	//     SetCurSel, CMonthCalCtrl::GetCurSel
	// ----------------------------------------------------------------------
	virtual BOOL GetCurSel(COleDateTime& refDateTime) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the currently selected date for the
	//     date picker control.
	// Parameters:
	//     refDateTime - A reference to a COleDateTime object, which
	//                   contains a date to select.
	// Remarks:
	//     This member function clears the selection of a date picker
	//     control and selects a specified date.
	// Returns:
	//     Nonzero if successful. Otherwise 0.
	// See Also: GetCurSel, CMonthCalCtrl::GetCurSel
	//-----------------------------------------------------------------------
	virtual BOOL SetCurSel(const COleDateTime& refDateTime);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the current maximum number of days
	//     that can be selected in a date picker control.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::GetMaxSelCount function.
	// Returns:
	//     An integer value that represents the total number of days that can
	//     be selected for the control. XTP_SELECTION_INFINITE if selection
	//     is not bounded.
	// See Also:
	//     SetMaxSelCount, CMonthCalCtrl::GetMaxSelCount, AllowNoncontinuousSelection
	// ----------------------------------------------------------------------
	virtual int GetMaxSelCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the maximum number of days that can be
	//     selected in a date picker control.
	// Parameters:
	//     nMax :  The value that is set to represent the maximum number of
	//             selectable days. XTP_SELECTION_INFINITE if selection is not
	//             bounded.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::SetMaxSelCount function.
	// See also:
	//     GetMaxSelCount, AllowNoncontinuousSelection, CMonthCalCtrl::SetMaxSelCount
	//-----------------------------------------------------------------------
	virtual void SetMaxSelCount(int nMax = XTP_SELECTION_INFINITE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the flag which shows whether control
	//     allows non-continuous days selection or not.
	// Parameters:
	//     bAllow - TRUE to allow non-continuous days selection (default value),
	//              FALSE to disallow.
	// Remarks:
	//     Allowing continuous days selection means that user will be able
	//     to select only a single days interval between 2 dates, where all
	//     dates will be selected inside.
	//     Non-continuous days selection means that user will be able to
	//     select any specific dates totally up to MaxSelCount count.
	// See Also:
	//     IsAllowNoncontinuousSelection, SetMaxSelCount, GetMaxSelCount
	//-----------------------------------------------------------------------
	virtual void AllowNoncontinuousSelection(BOOL bAllow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns the flag which shows whether control
	//     allows non-continuous days selection or not.
	// Remarks:
	//     Allowing continuous days selection means that user will be able
	//     to select only a single days interval between 2 dates, where all
	//     dates will be selected inside.
	//     Non-continuous days selection means that user will be able to
	//     select any specific dates totally up to MaxSelCount count.
	// Returns:
	//     TRUE when non-continuous days selection is allowed,
	//     FALSE otherwise.
	// See Also:
	//     AllowNoncontinuousSelection, SetMaxSelCount, GetMaxSelCount
	//-----------------------------------------------------------------------
	virtual BOOL IsAllowNoncontinuousSelection();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the date information for the date
	//     specified as "Today" for the date picker control.
	// Parameters:
	//     refDateTime :  A reference to a COleDateTime object indicating the
	//                    current day.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::GetToday function.
	// Returns:
	//     Nonzero if successful. Otherwise 0.
	// See Also:
	//     GetHighlightToday, SetHighlightToday, SetToday,
	//     CMonthCalCtrl::GetToday
	//-----------------------------------------------------------------------
	virtual BOOL GetToday(COleDateTime& refDateTime) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the calendar control to the current day.
	// Parameters:
	//     refDateTime - A reference to a COleDateTime object that
	//                   contains the current date.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::SetToday function.
	// See Also: GetHighlightToday, SetHighlightToday, SetToday, GetToday,
	//           CMonthCalCtrl::SetToday
	//-----------------------------------------------------------------------
	virtual void SetToday(const COleDateTime& refDateTime);

	// --------------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the current minimum and maximum
	//     dates set in a date picker control.
	// Parameters:
	//     pMinRange :  A pointer to a COleDateTime object containing the date at
	//                  the lowest end of the range.
	//     pMaxRange :  A pointer to a COleDateTime object containing the date at
	//                  the highest end of the range.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::GetRange function.
	// Returns:
	//     A DWORD that can be zero (no limits are set) or a combination of
	//     the following values that specify limit information.
	//
	//     * <b>GDTR_MAX</b> - A maximum limit is set for the control.
	//           pMaxRange is valid and contains the applicable date information.
	//     * <b>GDTR_MIN</b> - A minimum limit is set for the control.
	//           pMinRange is valid and contains the applicable date information.
	// Example:
	//     See CMonthCalCtrl::GetRange documentation for the example.
	// See Also:
	//     SetRange, CMonthCalCtrl::GetRange
	// --------------------------------------------------------------------------
	virtual DWORD GetRange(COleDateTime* pMinRange, COleDateTime* pMaxRange) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the minimum and maximum allowable
	//     dates for a date picker control.
	// Parameters:
	//     pMinRange - A pointer to a COleDateTime object containing the
	//                 date at the lowest end of the range.
	//     pMaxRange - A pointer to a COleDateTime object containing the
	//                 date at the highest end of the range.
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::GetRange function.
	// Returns:
	//     Nonzero if successful. Otherwise 0.
	// See Also: GetRange, CMonthCalCtrl::SetRange
	//-----------------------------------------------------------------------
	virtual BOOL SetRange(const COleDateTime* pMinRange, const COleDateTime* pMaxRange);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves date information representing
	//     the high and low limits of a date picker control's display.
	// Parameters:
	//     refMinRange - A reference to a COleDateTime object containing
	//                   the minimum date allowed.
	//     refMaxRange - A reference to a COleDateTime object containing
	//                   the maximum date allowed.
	//     dwFlags     - Value specifying the scope of the range limits
	//                   to be retrieved. This value must be one of the
	//                   following
	//
	//                      * <b>GMR_DAYSTATE</b> - Include preceding and
	//                      trailing months of visible range that are only
	//                      partially displayed.
	//                      * <b>GMR_VISIBLE</b> - Include only those months
	//                      that are entirely displayed.
	//
	// Remarks:
	//     This member function implements the behavior of the
	//     CMonthCalCtrl::GetMonthRange function.
	// Returns:
	//     An integer that represents the range, in months, spanned by the
	//     two limits indicated by refMinRange and refMaxRange.
	// See Also: CMonthCalCtrl::GetMonthRange
	//-----------------------------------------------------------------------
	virtual int GetMonthRange(COleDateTime& refMinRange, COleDateTime& refMaxRange, DWORD dwFlags) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves first and last displayed days dates.
	// Parameters:
	//     refFirstVisibleDay   - A first displayed day date.
	//     refLastVisibleDay    - A last displayed day date.
	// Returns:
	//     TRUE if succeeded, otherwise FALSE;
	// See Also: GetMonthRange
	//-----------------------------------------------------------------------
	virtual BOOL GetVisibleRange(COleDateTime& refFirstVisibleDay,
						 COleDateTime& refLastVisibleDay) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns the number of CXTPDatePickerButton
	//     items shown on the control.
	// Remarks:
	//     Call this member function to determine the number of buttons
	//     currently used on the control.
	// Returns:
	//     A number of CXTPDatePickerButton items shown on the control.
	// See Also: GetButton, AddButton
	//-----------------------------------------------------------------------
	virtual int GetButtonCount() const;

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function returns a pointer to the CXTPDatePickerButton
	//     \object using its numeric index.
	// Parameters:
	//     nIndex :  The index of the CXTPDatePickerButton to retrieve.
	// Remarks:
	//     Call this member function to retrieve a pointer to the button
	//     \object using its numeric index.
	// Returns:
	//     A pointer to the CXTPDatePickerButton object.
	// See Also:
	//     GetButtonCount, AddButton
	// ----------------------------------------------------------------------
	virtual CXTPDatePickerButton* GetButton(int nIndex) const;

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function adds one more buttons to the control.
	// Parameters:
	//     nID :  ID of the string resource item. This is the caption of the
	//            new button.
	// Remarks:
	//     Call this member function to add one more CXTPDatePickerButton
	//     \objects to the control's collection.
	// See Also:
	//     GetButton, GetButtonCount
	// ---------------------------------------------------------------------
	virtual void AddButton(UINT nID);

	// -------------------------------------------------------------------------
	// Summary:
	//     This member function obtains a notification connection object
	//     pointer.
	// Remarks:
	//     Used to subscribe (Advice) for notifications events from the control.
	// Returns:
	//     A CXTPNotifyConnection pointer to the connection object.
	// See Also:
	//     CXTPNotifyConnectionoverview, IXTPNotificationSink overview
	// -------------------------------------------------------------------------
	virtual CXTPNotifyConnection* GetConnection();

	// -----------------------------------------------------------------------
	// Summary:
	//     Returns a collection of the days, selected in the date picker.
	// Remarks:
	//     Call this member function to return a CXTPDatePickerDaysCollection
	//     collection of the days, selected in the date picker control.
	// Returns:
	//     Pointer to the collection of the selected days.
	//     If you want to use pointer to this collection lately in your code,
	//     you should increment and decrement references to the object manually.
	// See Also:
	//     m_pSelectedDays
	// -----------------------------------------------------------------------
	virtual CXTPDatePickerDaysCollection* GetSelectedDays();

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function adjusts the layout of all sub-items depending
	//     \on the current window client size.
	// Parameters:
	//     rcClient :  A CRect that contains the coordinates of the control.
	// Remarks:
	//     AdjustLayout depends on the AutoSize flag (changing control size
	//     \or item fonts correspondingly).
	// See Also:
	//     AdjustLayout(CRect rcClient), IsAutoSize
	// ----------------------------------------------------------------------
	virtual void AdjustLayout();

	// -----------------------------------------------------------------
	// Summary:
	//     This member function registers the window class if it has not
	//     already been registered.
	// Parameters:
	//     hInstance - Instance of resource where control is located
	// Returns:
	//     A boolean value that specifies if the window is successfully
	//     registered.<p/>
	//     TRUE if the window class is successfully registered.<p/>
	//     Otherwise FALSE.
	// -----------------------------------------------------------------
	virtual BOOL RegisterWindowClass(HINSTANCE hInstance = NULL);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sends a notification to all control's
	//     event subscribers.
	// Parameters:
	//     EventCode - The specific code of the event.
	//     wParam   - First custom parameter. Depends on the event type.
	//                See specific event description for details.
	//     lParam   - Second custom parameter. Depends on the event type.
	//                See specific event description for details.
	// Remarks:
	//     This member function is called internally from inside the
	//     control when a notification is sent to all notification
	//     listeners.
	// See Also: XTP_NOTIFICATIONCODE, GetConnection
	//-----------------------------------------------------------------------
	virtual void SendNotification(XTP_NOTIFY_CODE  EventCode, WPARAM wParam = 0, LPARAM lParam = 0);


protected:
	// ----------------------------------------------------------------------
	// Summary:
	//     This member function performs control population, creating the
	//     view from the data.
	// Remarks:
	//     Creates a new month collection for the control, populating it with
	//     the new objects created according to the stored properties and
	//     settings. Call this member function if the number of months to
	//     show needs adjusting.
	// See Also:
	//     ClearMonths
	// ----------------------------------------------------------------------
	virtual void Populate();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function redraws the control windows.
	// Parameters:
	//     bUpdateNow - if TRUE the CWnd::UpdateWindow() will be called,
	//                   otherwise only CWnd::Invalidate() will be called.
	// Remarks:
	//     Call this member function to redraw all control windows
	//     according to the stored parameter values.
	// See Also:
	//     RedrawControl
	//-----------------------------------------------------------------------
	virtual void _RedrawControl(BOOL bUpdateNow);

	// --------------------------------------------------------------
	// Summary:
	//     This member function is used to cleanup the month's array.
	// See Also:
	//     Populate
	// --------------------------------------------------------------
	virtual void ClearMonths();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to perform all drawing logic.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	// See Also: DrawButtons
	//-----------------------------------------------------------------------
	virtual void OnDraw(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw the calendar's buttons.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	// See Also: OnDraw
	//-----------------------------------------------------------------------
	virtual void DrawButtons(CDC* pDC);

	virtual void AdjustLayout(CRect rcClient);  // <combine CXTPDatePickerControl::AdjustLayout>

	// -----------------------------------------------------------
	// Summary:
	//     This member function calculates the size of the button.
	// Returns:
	//     CSize object with button size.
	// See Also:
	//     SetButtonRect, CalcButtonBandRect
	// -----------------------------------------------------------
	virtual CSize CalcButtonSize() const;

	// --------------------------------------------------------------
	// Summary:
	//     Call this member function to calculate and set the buttons
	//     bounding rectangle.
	// See Also:
	//     CalcButtonSize, CalcButtonBandRect
	// --------------------------------------------------------------
	virtual void SetButtonRect();

	// -------------------------------------------------------------------
	// Summary:
	//     Call this member function to calculate and set the buttons band
	//     rectangle.
	// Remarks:
	//     The band rectangle is a box that is drawn around the button to
	//     highlight the button.
	// See Also:
	//     SetButtonRect, CalcButtonSize
	// -------------------------------------------------------------------
	virtual void CalcButtonBandRect();

	// ------------------------------------------------------------------
	// Summary:
	//     This member function is used to implement the button behavior.
	// Parameters:
	//     point :  Current mouse position.
	// Remarks:
	//     This member function is called from the following member
	//     functions: OnLButtonDown, OnLButtonDblClk, OnLButtonUp,
	//     OnMouseLeave, and OnMouseMove.
	// ------------------------------------------------------------------
	virtual void ProcessButtons(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function shows a CXTPDatePickerList control at the
	//     coordinates specified by rcHeader.
	// Parameters:
	//     rcHeader - The coordinates of the list window.
	//     dtMonth  - The month date to start from.
	// Remarks:
	//     Call this member function to force showing of the months
	//     pop-up list control.
	// See Also: CXTPDatePickerList overview
	//-----------------------------------------------------------------------
	virtual void ShowListHeader(CRect rcHeader, COleDateTime dtMonth);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function initializes the month and day names depending
	//     \on localization.
	// Remarks:
	//     This is an internal initialization function that reads the system
	//     user locale defaults and fills the month and day of the week
	//     arrays with the locale defaults.
	// See Also:
	//     GetMonthName, SetMonthName, GetDayOfWeekName, SetDayOfWeekName
	// ----------------------------------------------------------------------
	virtual void InitNames();

	// -------------------------------------------------------------------------
	// Summary:
	//     This function shifts the specified date for the specified number
	//     \of months.
	// Parameters:
	//     refDate :      Reference to the COleDateTime object to shift the date
	//                    on.
	//     nMonthCount :  Number of months used for shifting. Can be both positive
	//                    and negative numbers.
	// Remarks:
	//     This is a utility function that is used to shift the specified
	//     date for a specific number of months.
	// Returns:
	//     A BOOL value that specifies if the function is successful.<p/>
	//     Return zero if the value of this COleDateTime object is set
	//     successfully.<p/>
	// Otherwise return 1.
	// See Also:
	//     COleDateTime overview
	// -------------------------------------------------------------------------
	static BOOL ShiftDate(COleDateTime &refDate, int nMonthCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets new dimensions for the month grid.
	// Parameters:
	//     rcGrid - New rectangle coordinates of the Grid area.
	// Remarks:
	//     Calculates and sets Grid dimensions (rows and columns count) for
	//     the grid size.
	// See Also: AdjustLayout
	//-----------------------------------------------------------------------
	virtual CSize SetGridDimentions(CRect rcGrid);

	// ------------------------------------------------------------------
	// Summary:
	//     This member function creates the internal month array.
	// Remarks:
	//     Fills the internal month array with the new DatPickerItemMonth
	//     \objects, using the values of the initialized data members.
	// See Also:
	//     Populate
	// ------------------------------------------------------------------
	virtual void CreateMonthArray();

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function is called by the framework to allow other
	//     necessary sub-classing to occur before the window is sub-classed.
	// Remarks:
	//     This member function is used to call some internal initialization
	//     functions like AdjustLayout() immediately after instantiating the
	//     DatePickerControl object.
	// See Also:
	//     Create, CXTPDatePickerControl::CXTPDatePickerControl
	// ---------------------------------------------------------------------
	virtual void PreSubclassWindow();

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function is used to process the control's button
	//     clicks.
	// Parameters:
	//     nID :  Date picker button ID (could be XTP_IDS_DATEPICKER_TODAY
	//            or XTP_IDS_DATEPICKER_NONE)
	// Remarks:
	//     This member function performs internal button click processing and
	//     sends a XTP_NC_DATEPICKER_BUTTON_CLICK notification to the parent
	//     window.
	// See Also:
	//     SendMessageToParent
	// ----------------------------------------------------------------------
	virtual void OnButtonClick(UINT nID);

protected:
	DatePickerMouseMode m_mouseMode;    // This member variable is used to specify the current Mouse operating mode.

	int m_nLockUpdateCount;         // This member variable is used as a counter that is used to count
	                                // the update locks. An image will be redrawn only when the lock
	                                // counter is equal to zero.
	int m_nTimerID;                 // This member variable is used to specify the control timer ID.

	CRect m_rcControl;  // This data member is used to store the control drawing coordinates.

	CXTPDatePickerPaintManager* m_pPaintManager;    // This member variable is a pointer to the paint manager.

	BOOL m_bAutoSize;   // This member variable is used as the AutoSize flag.
	BOOL m_bIsModal;    // This member variable is used to specify if the control is used as
	                    // a pop-up window. TRUE when the control is used as a pop-up. FALSE
	                    // \otherwise.
	BOOL m_bChanged;    // This member variable is used to determine if the control requires
	                    // redrawing.

	CBitmap m_bmpCache; // This member variable is a cached window bitmap.

	int m_nRows;        // This member variable is used to specify the number of rows in the
	                    // column month's grid.
	int m_nColumns;     // This member variable is used to specify the number of columns in
	                    // the column month's grid.

	CXTPNotifyConnection* m_pConnect; // This member variable is a connection object that is used to send
	                                  // notifications.


	COleDateTime m_dtMinRange;      // This member variable specifies the minimum allowable date for a
	                                // date picker control.
	COleDateTime m_dtMaxRange;      // This member variable specifies the maximum allowable date for a
	                                // date picker control.
	COleDateTime m_dtToday;         // This member variable is used to specify the "Today" date for a
	                                // date picker control.
	COleDateTime m_dtFirstMonth;    // This member variable is used to specify the first month in the
	                                // grid.
	int m_nFirstDayOfWeek;          // This member variable is used to specify the first day of the week
	                                // to display (1-Sunday, 2-Monday ... etc).
	int m_nFirstWeekOfYearDays;     // This member variable is used to specify the number of days of
	                                // the new year in the first week of this year.

	PFNITEMMETRICS m_pfnCallback;   // This member variable is a pointer to the user's IsSpecialDay
	                                // function.
	void* m_pCallbackParam;         // This member variable is a pointer to the user's additional
	                                // parameter for the callback function.

	CArray<CXTPDatePickerItemMonth*, CXTPDatePickerItemMonth*> m_arrMonths; // This member variable is an internal storage for the month items.

	CXTPDatePickerDaysCollection* m_pSelectedDays;  // This member variable is a collection of selected days.

	BOOL m_bHighlightToday;     // This member variable is used as a flag to determine if the "Today"
	                            // date is highlighted.
	BOOL m_bShowWeekNumbers;    // This member variable is a flag that determines if the "week
	                            // numbers" are displayed.
	BOOL m_bShowNonMonthDays;   // This member variable is a flag that determines if the "non month
	                            // days" are displayed.
	XTPDatePickerBorderStyle m_borderStyle;   // This member variable is a flag that determines if the control
	                                          // displays a 3D border.
	int m_nMaxSelectionDays;    // This member variable is used to specify the maximum number of
	                            // selected days.
	int m_nMonthDelta;          // This member variable is used to specify the number of months set
	                            // as the control's scroll rate. If this value is zero, then the
	                            // month delta is reset to the default rate, which is the number of
	                            // months displayed in the control.

	COleDateTime m_dtFirstClicked;  // This member variable is used to specify the day that was clicked
	                                // before starting the mouse selection.
	COleDateTime m_dtLastClicked;   // This member variable is used to specify the day that was clicked
	                                // last in the mouse selection.
	BOOL m_bSelectWeek;         // TRUE when user is started selecting the whole week.

	COleDateTime m_dtFocused;   // This member variable is used to specify the focused day. Used only for keyboard navigation and selecting.
	COleDateTime m_dtFSelBase;  // This member variable is used to specify start day of the continuous selection. Used only for keyboard navigation and selecting.

	CXTPDatePickerList* m_pListControl; // This member variable is used to specify the "header month list"
	                                    // pop-up control.

	CString* m_arMonthNames;    // This member variable contains the localized month names values.
	CString* m_arDayOfWeekNames;// This member variable contains the day abbreviation for drawing in
	                            // the month header.

	CRect m_rcGrid;             // Months grid rect.

	CXTPDatePickerButtons m_arrButtons; // This member variable is an array of control buttons.

	BOOL m_bAllowNoncontinuousSelection;  // TRUE to allow Noncontinuous selection

	CXTPDatePickerButton* m_pButtonCaptured;  // Pointer to a date picker button
	BOOL m_bRightToLeft;            // TRUE to display calendar components in Right-To-Left (RTL) format

//{{AFX_CODEJOCK_PRIVATE
	LCID    m_lcidActiveLocale;

	enum XTPEnumMovmentStep
	{
		stepDay     = 1,
		stepWeek    = 2,
		stepMonth   = 3,
		stepYear    = 4,

		stepWeekBE  = 10,
		stepMonthBE = 11
	};
	enum XTPEnumMovmentDirection
	{
		dirNext = 1,
		dirPrev = -1
	};

	class CXTPSelectionHelper
	{
	public:
		CXTPSelectionHelper(CXTPDatePickerControl* pControl);

		void RemoveFocus();

		void MoveFocus(int eStep, int eDirection, BOOL bContinuouse, BOOL bSaveSel);
		void SelUnselFocus();

		void _TmpSaveFocus();
		void _TmpRestoreFocus();

	protected:
		void InitFocusIfNeed();

		void _MoveFocus(int eStep, int eDirection);

		CXTPDatePickerControl* m_pDP;

		COleDateTime m_dtFocusedTmp;
		COleDateTime m_dtFSelBaseTmp;
	};
	friend class CXTPSelectionHelper;
//}}AFX_CODEJOCK_PRIVATE
protected:

//{{AFX_CODEJOCK_PRIVATE
	// AFX_MSG(CXTPDatePickerControl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lp);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnCancelMode();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimeChange();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysColorChange();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus (CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnDestroy();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG

	virtual void OnFinalRelease();

	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE

private:
//{{AFX_CODEJOCK_PRIVATE
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	virtual void _EndModalIfNeed();
//}}AFX_CODEJOCK_PRIVATE

public:
	BOOL m_bDeleteOnFinalRelease;   // Delete self OnFinalRelease() call.
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPDatePickerPaintManager* CXTPDatePickerControl::GetPaintManager() {
	return m_pPaintManager;
}

AFX_INLINE BOOL CXTPDatePickerControl::IsModal() const {
	return m_bIsModal;
}

AFX_INLINE BOOL CXTPDatePickerControl::IsAutoSize() {
	return m_bAutoSize;
}

AFX_INLINE void CXTPDatePickerControl::SetCallbackDayMetrics(PFNITEMMETRICS pFunc, void* pParam) {
	m_pfnCallback = pFunc;
	m_pCallbackParam = pParam;
}

AFX_INLINE PFNITEMMETRICS CXTPDatePickerControl::GetCallbackDayMetrics() {
	return m_pfnCallback;
}

AFX_INLINE void CXTPDatePickerControl::SetFirstDayOfWeek(int nDay) {
	ASSERT(nDay >= 1 && nDay <= 7);
	if (nDay >= 1 && nDay <= 7 && nDay != m_nFirstDayOfWeek)
	{
		m_nFirstDayOfWeek = nDay;
		Populate();
	}
}

AFX_INLINE int CXTPDatePickerControl::GetFirstDayOfWeek() {
	return m_nFirstDayOfWeek;
}

AFX_INLINE void CXTPDatePickerControl::SetFirstWeekOfYearDays(int nDays) {
	ASSERT(nDays >= 1 && nDays <= 7);
	if (nDays >= 1 && nDays <= 7 && nDays != m_nFirstWeekOfYearDays)
	{
		m_nFirstWeekOfYearDays = nDays;
		RedrawControl();
	}
}

AFX_INLINE int CXTPDatePickerControl::GetFirstWeekOfYearDays() {
	return m_nFirstWeekOfYearDays;
}

AFX_INLINE BOOL CXTPDatePickerControl::GetHighlightToday() {
	return m_bHighlightToday;
}

AFX_INLINE BOOL CXTPDatePickerControl::GetShowWeekNumbers() {
	return m_bShowWeekNumbers;
}

AFX_INLINE int CXTPDatePickerControl::GetRows() {
	return m_nRows;
}
AFX_INLINE int CXTPDatePickerControl::GetCols() {
	return m_nColumns;
}

AFX_INLINE void CXTPDatePickerControl::SetMonthName(int nMonth, LPCTSTR strMonthName) {
	ASSERT(nMonth >= 1 && nMonth <= 12);
	m_arMonthNames[nMonth - 1] = strMonthName;
}

AFX_INLINE CString CXTPDatePickerControl::GetMonthName(int nMonth) const {
	return nMonth >= 1 && nMonth <= 12 ? m_arMonthNames[nMonth - 1] : _T("");
}

AFX_INLINE void CXTPDatePickerControl::SetDayOfWeekName(int nDayOfWeek, LPCTSTR strDayOfWeekName) {
	ASSERT(nDayOfWeek >= 1 && nDayOfWeek <= 7);
	m_arDayOfWeekNames[nDayOfWeek - 1] = strDayOfWeekName;
}

AFX_INLINE CString CXTPDatePickerControl::GetDayOfWeekName(int nDayOfWeek) const {
	return nDayOfWeek >= 1 && nDayOfWeek <= 7 ? m_arDayOfWeekNames[nDayOfWeek - 1] : _T("");
}

AFX_INLINE BOOL CXTPDatePickerControl::GetShowNonMonthDays() {
	return m_bShowNonMonthDays;
}

AFX_INLINE XTPDatePickerBorderStyle CXTPDatePickerControl::GetBorderStyle() const{
	return m_borderStyle;
}

AFX_INLINE int CXTPDatePickerControl::GetMaxSelCount() const {
	return m_nMaxSelectionDays;
}

AFX_INLINE int CXTPDatePickerControl::GetMonthDelta() const {
	return m_nMonthDelta ? m_nMonthDelta : 1;
}

AFX_INLINE void CXTPDatePickerControl::SetMonthDelta(int iDelta) {
	m_nMonthDelta = iDelta;
}

AFX_INLINE BOOL CXTPDatePickerControl::GetToday(COleDateTime& refDateTime) const {
	refDateTime = m_dtToday;
	return TRUE;
}
AFX_INLINE int CXTPDatePickerControl::GetButtonCount() const{
	return (int)m_arrButtons.GetSize();
}
AFX_INLINE CXTPDatePickerButton* CXTPDatePickerControl::GetButton(int nIndex) const {
	return m_arrButtons[nIndex];
}
AFX_INLINE CXTPNotifyConnection* CXTPDatePickerControl::GetConnection() {
	return m_pConnect;
}

AFX_INLINE void CXTPDatePickerControl::SendNotification(XTP_NOTIFY_CODE EventCode, WPARAM wParam , LPARAM lParam) {
	m_pConnect->SendEvent(EventCode, wParam, lParam);
}
AFX_INLINE BOOL CXTPDatePickerControl::IsAllowNoncontinuousSelection() {
	return m_bAllowNoncontinuousSelection;
}
AFX_INLINE CXTPDatePickerDaysCollection* CXTPDatePickerControl::GetSelectedDays() {
	return m_pSelectedDays;
}

AFX_INLINE void CXTPDatePickerControl::AdjustLayout() {
	InitNames();
	AdjustLayout(m_rcControl);
}
AFX_INLINE BOOL CXTPDatePickerControl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

#endif // !defined(_XTPDATEPICKERCONTROL_H__)
