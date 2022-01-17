// XTPCalendarEventRecurrenceDlg.h interface for the CXTPCalendarEventPropertiesDlg.
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
#if !defined(_XTP_CALENDAR_EVENT_RECURRENCE_DLG_H__)
#define _XTP_CALENDAR_EVENT_RECURRENCE_DLG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarPtrs.h"
#include "XTPCalendarUtils.h"

//===========================================================================
// Summary:
//      This constant defines a value which returned from
//      CXTPCalendarEventRecurrenceDlg::DoModal() when "Remove Recurrence"
//      button pressed.
// See Also:
//      CXTPCalendarEventRecurrenceDlg
//===========================================================================
static const int XTP_CALENDAR_DLGRESULT_REMOVE_RECURRENCE = 103;

//===========================================================================
// Summary:
//      This class used for manipulations with recurrence state of the events.
// See Also:
//      CXTPCalendarEventPropertiesDlg
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarEventRecurrenceDlg : public CDialog
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarEventRecurrenceDlg)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//{{AFX_CODEJOCK_PRIVATE
	enum { IDD = XTP_IDD_CALENDAR_RECURRENCE_PROPERTIES };
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//     Dialog class constructor.
	// Parameters:
	//     pMasterEvent - [in] Master event pointer to edit recurrence.
	//     pParent      - [in] Pointer to parent window. Can be NULL.
	// See Also:
	//     ~CXTPCalendarEventRecurrenceDlg()
	//-----------------------------------------------------------------------
	CXTPCalendarEventRecurrenceDlg(CXTPCalendarEvent* pMasterEvent, CWnd* pParent = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarEventRecurrenceDlg();

	BOOL m_bDisableRemove;  // Set TRUE to disable button remove recurrence. FALSE by default.

protected:


	//-----------------------------------------------------------------------
	// Summary:
	//      Initialize data for Start and End combobox controls.
	// Remarks:
	//      Time formatted using active locale.
	//-----------------------------------------------------------------------
	virtual void InitStartEndCB();

	//-----------------------------------------------------------------------
	// Summary:
	//      Initialize data for Duration combobox control.
	// Remarks:
	//      Time duration formatted using active resource file.
	//-----------------------------------------------------------------------
	virtual void InitDurationCB();

	//-----------------------------------------------------------------------
	// Summary:
	//      Initialize data for days of week names.
	// Parameters:
	//      wndCB - A CComboBox object reference to set data.
	// Remarks:
	//      Values formatted using active locale.
	//-----------------------------------------------------------------------
	virtual void InitDayOfWeekCBs(CComboBox& wndCB);

	//-----------------------------------------------------------------------
	// Summary:
	//      Initialize data for months names.
	// Parameters:
	//      wndCB - A CComboBox object reference to set data.
	// Remarks:
	//      Values formatted using active resource file.
	//-----------------------------------------------------------------------
	virtual void InitMonthCBs(CComboBox& wndCB);

	//-----------------------------------------------------------------------
	// Summary:
	//      Initialize data for which day (First, Second, ...).
	// Parameters:
	//      wndCB - A CComboBox object reference to set data.
	// Remarks:
	//      Values formatted using active resource file.
	//-----------------------------------------------------------------------
	virtual void InitWhichDayCBs(CComboBox& wndCB);

	//-----------------------------------------------------------------------
	// Summary:
	//      Shows error message if the user's input is incorrect.
	// Parameters:
	//      pWnd    - A pointer to control with wrong value (to set focus).
	//      nCtrlID - A dialog control ID with wrong value (to set focus).
	//      nMin    - A minimum value of allowed range.
	//      nMax    - A maximum value of allowed range.
	// Remarks:
	//      Message formatted using active resource file.
	//-----------------------------------------------------------------------
	virtual void MsgBox_WrongValue(CWnd *pWnd);
	virtual void MsgBox_WrongValueRange(UINT nCtrlID, int nMin, int nMax); //<COMBINE CXTPCalendarEventRecurrenceDlg::MsgBox_WrongValue@CWnd *>

	//{{AFX_CODEJOCK_PRIVATE
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();

	afx_msg void OnStartComboChanged();
	afx_msg void OnEndComboChanged();
	afx_msg void OnDurationComboChanged();

	afx_msg void OnStartComboEdited();
	afx_msg void OnEndComboEdited();
	afx_msg void OnDurationComboEdited();

	afx_msg void OnBnClickedButtonRemoveRecurrence();

	// Update visible state of controls, according to the selected mode:
	//day/week/month/year. wparam - ID of the selected radio
	afx_msg void OnBnClickedRadioDailyYearly(UINT wparam);

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual void _MoveControlsBy_Y();
	virtual int _AddString(CComboBox& wndCB, UINT nStrResID, DWORD_PTR dwItemData);
	virtual int _AddLocaleString(CComboBox& wndCB, LCTYPE lcidStr, DWORD_PTR dwItemData);

	virtual void OnStartChanged();
	virtual void _OnStartChanged();

	virtual void OnEndChanged();
	virtual void _OnEndChanged();

	virtual void OnDurationChanged();
	virtual void _OnDurationChanged();

	//Gets Time in Minutes, shown in ComboBox
	virtual int GetChangedComboTimeInMin(CComboBox &wndCb);
	virtual int GetDurationComboInMin();

	// Shows or hides control on the dialog with specified id
	virtual void ShowWindow(int nID, BOOL bShow = TRUE);
	virtual void MoveWindow_Y(int nID, int nYOffset);
	virtual CRect GetCtrlRect(int nID);

	DECLARE_MESSAGE_MAP()
	//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPCalendarEventPtr m_ptrMasterEvent;          //Pointer to master event
	CXTPCalendarRecurrencePatternPtr m_ptrPattern;  //recurrence pattern

	COleDateTime m_dtStart;     // Store StartTime editor value.
	COleDateTime m_dtEnd;       // Store EndTime editor value.
	COleDateTime m_dtStartDate; // Store StartDate editor value. UpdateData method used to Exchange value with dialog control.
	COleDateTime m_dtEndDate;   // Store EndDate editor value. UpdateData method used to Exchange value with dialog control.

	CComboBox m_wndCbStart;     // StartTime combobox object.
	CComboBox m_wndCbEnd;       // EndTime combobox object.
	CComboBox m_wndCbDuration;  // Duration combobox object.

	int m_nOccurNum;            // Store Number of occurrences editor value. UpdateData method used to Exchange value with dialog control.

	// Daly recurrence case
	int m_nDayInterval;         // Store days interval editor value. UpdateData method used to Exchange value with dialog control.

	// Weekly recurrence case
	int m_nWeeklyInterval;      // Store weeks interval editor value. UpdateData method used to Exchange value with dialog control.

	BOOL m_bMonday;     // Store Monday checkbox state. UpdateData method used to Exchange value with dialog control.
	BOOL m_bTuesday;    // Store Tuesday checkbox state. UpdateData method used to Exchange value with dialog control.
	BOOL m_bWednesday;  // Store Wednesday checkbox state. UpdateData method used to Exchange value with dialog control.
	BOOL m_bThursday;   // Store Thursday checkbox state. UpdateData method used to Exchange value with dialog control.
	BOOL m_bFriday;     // Store Friday checkbox state. UpdateData method used to Exchange value with dialog control.
	BOOL m_bSaturday;   // Store Saturday checkbox state. UpdateData method used to Exchange value with dialog control.
	BOOL m_bSunday;     // Store Sunday checkbox state. UpdateData method used to Exchange value with dialog control.

	// Monthly recurrence case
	int m_MonthDate;        // Store month day editor value. UpdateData method used to Exchange value with dialog control.
	int m_nMonthInterval;   // Store month interval editor value. UpdateData method used to Exchange value with dialog control.
	int m_nMonthInterval2;  // Store month interval editor value (for second case). UpdateData method used to Exchange value with dialog control.

	CComboBox m_wndCbMonthDay;   // Day of week combobox object for monthly recurrence.
	CComboBox m_wndCbMonthWhich; // Which Day combobox object for monthly recurrence.

	// Yearly recurrence case
	int m_nDayOfMonth;              // Store month day editor value for Yearly recurrence. UpdateData method used to Exchange value with dialog control.

	CComboBox m_wndYearMonth;       // Month combobox object for Yearly recurrence.
	CComboBox m_wndComboYearMonth2; // Month combobox object for Yearly recurrence (for second case).
	CComboBox m_wndCbYearWhich;     // Which Day combobox object for Yearly recurrence.
	CComboBox m_wndCbYearDay;       // Day of week combobox object for Yearly recurrence.
};

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE void CXTPCalendarEventRecurrenceDlg::ShowWindow(int nID, BOOL bShow)
{
	if (GetDlgItem(nID))
		GetDlgItem(nID)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

#endif // !defined(_XTP_CALENDAR_EVENT_RECURRENCE_DLG_H__)
