//XTPCalendarRemindersDialog.h: interface for the CXTPCalendarDayViewTimeScale class.
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
#if !defined(_XTP_CALENDAR_REMINDERS_DIALOG_H__)
#define _XTP_CALENDAR_REMINDERS_DIALOG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include "XTPCalendarRemindersManager.h"

class CXTPCalendarRemindersManager;
class CXTPCalendarControl;
/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarRemindersDialog dialog

class _XTP_EXT_CLASS CXTPCalendarRemindersDialog : public CDialog
{
// Construction
public:
	enum { IDD = XTP_IDD_CALENDAR_REMINDERS};

	CXTPCalendarRemindersDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CXTPCalendarRemindersDialog();

	virtual BOOL Create(CWnd* pParent = NULL);

	virtual void SetRemindersManager(CXTPCalendarControl* pCalendar);

	virtual void UpdateFromManager();

	virtual void Detach();

	virtual BOOL IsShowInTaskBar();
	virtual void SetShowInTaskBar(BOOL bShow = TRUE);

	BOOL m_bAutoPopup;
protected:

	CXTPCalendarRemindersManager*   m_pRemindersMan;
	CXTPCalendarReminders           m_arActiveReminders;

	CXTPCalendarControl* m_pCalendar;

	int m_nNextRefreshIndex;

	DECLARE_XTP_SINK(CXTPCalendarRemindersDialog, m_Sink)

	virtual void OnReminders(XTP_NOTIFY_CODE Event, WPARAM wParam , LPARAM lParam);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void InitSnoozeComboBox();

	virtual void UpdateControlsBySelection();
	virtual void SelectItem(int nSelItemIndex);
	virtual void UpdateTitle();
	virtual BOOL _RemoveReminderInfo(CXTPCalendarReminder* pReminder);

	virtual void OnOK();
	afx_msg void OnDimiss();
	afx_msg void OnDimissAll();
	afx_msg void OnOpenItem();
	afx_msg void OnSnooze();
	afx_msg void OnRemindersList_ItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRemindersList_DblClick(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl   m_ctrlRemindersList;
	CStatic     m_ctrlDescription;

	CComboBox   m_ctrlSnoozeTimeCB;
	CButton     m_ctrlSnoozeBtn;

	CButton m_ctrlDimissBtn;
	CButton m_ctrlDimissAllBtn;
	CButton m_ctrlOpenItemBtn;

	BOOL m_bShowInTaskBar;
};

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE void CXTPCalendarRemindersDialog::Detach() {
	SetRemindersManager(NULL);
}

#endif // !defined(_XTP_CALENDAR_REMINDERS_DIALOG_H__)
