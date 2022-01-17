// XTPCalendarRemindersDialog.cpp :

#include "stdafx.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPVC50Helpers.h"

#include "XTPCalendarRemindersDialog.h"
#include "XTPCalendarEventPropertiesDlg.h"
#include "XTPCalendarEvent.h"
#include "XTPCalendarNotifications.h"
#include "XTPCalendarControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define XTP_CALENDAR_RMD_REFRESH_TIMERID 1
#define XTP_CALENDAR_RMD_REFRESH_TIMEOUT 60*1000 // 1 minute

#define XTP_CALENDAR_RMD_REFRESH_ITEM_TIMERID 2
#define XTP_CALENDAR_RMD_REFRESH_ITEM_TIMEOUT 70 // 70 ms

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarRemindersDialog dialog

CXTPCalendarRemindersDialog::CXTPCalendarRemindersDialog(CWnd* pParent)
{
	InitModalIndirect(XTPResourceManager()->LoadDialogTemplate(IDD), pParent);

	m_pRemindersMan = NULL;
	m_pCalendar = NULL;
	m_nNextRefreshIndex = -1;

	m_bAutoPopup = TRUE;
	m_bShowInTaskBar = TRUE;
}

CXTPCalendarRemindersDialog::~CXTPCalendarRemindersDialog()
{
	DestroyWindow();

	m_Sink.UnadviseAll();

	CMDTARGET_RELEASE(m_pRemindersMan);
	CMDTARGET_RELEASE(m_pCalendar);
}

BOOL CXTPCalendarRemindersDialog::Create(CWnd* pParent)
{
	BOOL bCreated = CDialog::CreateIndirect(XTPResourceManager()->LoadDialogTemplate(IDD), pParent);
	if (bCreated)
	{
		if (m_bShowInTaskBar)
		{
			ModifyStyleEx(0, WS_EX_APPWINDOW);
		}
	}

	return bCreated;
}

BOOL CXTPCalendarRemindersDialog::IsShowInTaskBar()
{
	return m_bShowInTaskBar;
}

void CXTPCalendarRemindersDialog::SetShowInTaskBar(BOOL bShow)
{
	BOOL bChanged = m_bShowInTaskBar != bShow;
	m_bShowInTaskBar = bShow;
	if (m_hWnd)
	{
		BOOL bVisible = IsWindowVisible();
		if (bChanged)
			ShowWindow(SW_HIDE);

		ModifyStyleEx(m_bShowInTaskBar ? 0 : WS_EX_APPWINDOW, m_bShowInTaskBar ? WS_EX_APPWINDOW : 0);

		if (bChanged && bVisible)
			ShowWindow(SW_SHOWNORMAL);
	}
}

void CXTPCalendarRemindersDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, XTP_IDC_RMD_SNOZE_TIME_CB, m_ctrlSnoozeTimeCB);
	DDX_Control(pDX, XTP_IDC_RMD_SNOOZE, m_ctrlSnoozeBtn);
	DDX_Control(pDX, XTP_IDC_RMD_REMINDERS_LIST, m_ctrlRemindersList);
	DDX_Control(pDX, XTP_IDC_RMD_DIMISS_ALL, m_ctrlDimissAllBtn);
	DDX_Control(pDX, XTP_IDC_RMD_DIMISS, m_ctrlDimissBtn);
	DDX_Control(pDX, XTP_IDC_RMD_OPEN_ITEM, m_ctrlOpenItemBtn);
	DDX_Control(pDX, XTP_IDC_RMD_DESCRIPTION, m_ctrlDescription);

}

BEGIN_MESSAGE_MAP(CXTPCalendarRemindersDialog, CDialog)

	ON_BN_CLICKED(XTP_IDC_RMD_DIMISS, OnDimiss)
	ON_BN_CLICKED(XTP_IDC_RMD_DIMISS_ALL, OnDimissAll)
	ON_BN_CLICKED(XTP_IDC_RMD_OPEN_ITEM, OnOpenItem)
	ON_BN_CLICKED(XTP_IDC_RMD_SNOOZE, OnSnooze)
	ON_NOTIFY(LVN_ITEMCHANGED, XTP_IDC_RMD_REMINDERS_LIST, OnRemindersList_ItemChanged)
	ON_NOTIFY(NM_DBLCLK, XTP_IDC_RMD_REMINDERS_LIST, OnRemindersList_DblClick)
	ON_WM_TIMER()

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarRemindersDialog message handlers

BOOL CXTPCalendarRemindersDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rcList(0,0,0,0);
	m_ctrlRemindersList.GetClientRect(&rcList);

	int nColWith0 = rcList.Width()*60/100;
	int nColWith1 = rcList.Width()*30/100;

	m_ctrlRemindersList.InsertColumn(0, CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_SUBJECT),
									 LVCFMT_LEFT, nColWith0, 0);

	m_ctrlRemindersList.InsertColumn(1, CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_DUEIN),
									 LVCFMT_LEFT, nColWith1, 1);

	ListView_SetExtendedListViewStyleEx(m_ctrlRemindersList.m_hWnd, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	InitSnoozeComboBox();

	UpdateFromManager();

	UpdateControlsBySelection();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CXTPCalendarRemindersDialog::InitSnoozeComboBox()
{
	m_ctrlSnoozeTimeCB.ResetContent();

	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_MINUTES_1));
	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_MINUTES_5));
	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_MINUTES_10));
	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_MINUTES_15));

	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_HOURS_1));
	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_HOURS_2));
	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_HOURS_4));
	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_HOURS_8));

	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_DAYS_0_5));
	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_DAYS_1));
	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_DAYS_2));
	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_DAYS_3));

	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_WEEKS_1));
	m_ctrlSnoozeTimeCB.AddString(CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_WEEKS_2));
}

void CXTPCalendarRemindersDialog::OnDimissAll()
{
	CWaitCursor _WC;

	ASSERT(m_pRemindersMan);
	if (m_pRemindersMan)
	{
		VERIFY(m_pRemindersMan->DismissAll());
	}
}

BOOL CXTPCalendarRemindersDialog::_RemoveReminderInfo(CXTPCalendarReminder* pReminder)
{
	int nRmdIdx = m_arActiveReminders.Find(pReminder);

	if (nRmdIdx >= 0 && nRmdIdx < m_ctrlRemindersList.GetItemCount())
	{
		m_ctrlRemindersList.DeleteItem(nRmdIdx);
		m_arActiveReminders.RemoveAt(nRmdIdx);

		UpdateControlsBySelection();

		SelectItem(nRmdIdx);
		UpdateTitle();

		return TRUE;
	}

	return FALSE;
}

void CXTPCalendarRemindersDialog::OnDimiss()
{
	int nSelIdx = m_ctrlRemindersList.GetNextItem(-1, LVNI_FOCUSED|LVNI_SELECTED);
	if (nSelIdx >= 0)
	{
		CXTPCalendarReminder* pReminder = m_arActiveReminders.GetAt(nSelIdx, FALSE);
		ASSERT(pReminder);
		if (pReminder)
		{
			VERIFY(pReminder->Dismiss());
		}
	}
}

void CXTPCalendarRemindersDialog::OnSnooze()
{
	int nSelIdx = m_ctrlRemindersList.GetNextItem(-1, LVNI_FOCUSED|LVNI_SELECTED);
	if (nSelIdx >= 0)
	{
		CXTPCalendarReminder* pReminder = m_arActiveReminders.GetAt(nSelIdx, FALSE);
		ASSERT(pReminder);
		if (pReminder)
		{
			CString strSnooze;
			m_ctrlSnoozeTimeCB.GetWindowText(strSnooze);
			int nSnoozeTime = CXTPCalendarUtils::ParceTimeDuration_min(strSnooze);

			if (nSnoozeTime == INT_MIN || nSnoozeTime < 1)
			{
				m_ctrlSnoozeTimeCB.SetFocus();
				XTPResourceManager()->ShowMessageBox(XTP_IDS_CALENDAR_WRONG_VALUE);
				return;
			}

			VERIFY(pReminder->Snooze(nSnoozeTime));
		}
	}
}

void CXTPCalendarRemindersDialog::OnOpenItem()
{
	ASSERT(m_pCalendar);
	if (!m_pCalendar) {
		return;
	}

	int nSelIdx = m_ctrlRemindersList.GetNextItem(-1, LVNI_FOCUSED|LVNI_SELECTED);
	if (nSelIdx >= 0)
	{
		CXTPCalendarReminder* pReminder = m_arActiveReminders.GetAt(nSelIdx, FALSE);
		ASSERT(pReminder);
		if (pReminder)
		{
			CXTPCalendarEventPtr ptrEvent = pReminder->GetEvent();

			CXTPCalendarEventPropertiesDlg dlg(m_pCalendar, FALSE, this, ptrEvent);
			dlg.DoModal();
		}
	}
}


void CXTPCalendarRemindersDialog::OnRemindersList_ItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	UpdateControlsBySelection();
}

void CXTPCalendarRemindersDialog::OnRemindersList_DblClick(NMHDR* , LRESULT* )
{
	if (m_pCalendar) {
		OnOpenItem();
	}
}

void CXTPCalendarRemindersDialog::UpdateControlsBySelection()
{
	CString strDescription;
	BOOL bEnableControls = FALSE;

	int nSelIdx = m_ctrlRemindersList.GetNextItem(-1, LVNI_FOCUSED|LVNI_SELECTED);
	if (nSelIdx < 0)
	{
		if (m_ctrlRemindersList.GetItemCount())
		{
			strDescription = CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_SEL_0_RMD);
		}
		else
		{
			strDescription = CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_NO_REMINDERS);
		}
	}
	else
	{
		CXTPCalendarReminder* pReminder = m_arActiveReminders.GetAt(nSelIdx, FALSE);
		CXTPCalendarEventPtr ptrEvent = pReminder ? pReminder->GetEvent() : NULL;

		ASSERT(ptrEvent);
		if (ptrEvent)
		{
			bEnableControls = TRUE;

			CString strSubject = ptrEvent->GetSubject();
			SYSTEMTIME stStart;
			GETASSYSTEMTIME_DT(ptrEvent->GetStartTime(), stStart);

			CString strFormatDate = CXTPCalendarUtils::GetLocaleString(LOCALE_SLONGDATE, 81);
			CString strFormatTime = CXTPCalendarUtils::GetTimeFormatString_HrMin();

			CString strDate = CXTPCalendarUtils::GetDateFormat(&stStart, strFormatDate);
			CString strTime = CXTPCalendarUtils::GetTimeFormat(&stStart, strFormatTime);

			strDescription.Format(_T("%s \n\n%s: %s %s"), (LPCTSTR)strSubject,
							(LPCTSTR)CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_START_TIME),
							(LPCTSTR)strDate, (LPCTSTR)strTime);
		}
	}

	//===================================================================
	m_ctrlDescription.SetWindowText(strDescription);

	CString strSnooze = CXTPCalendarUtils::FormatTimeDuration(15, FALSE);
	m_ctrlSnoozeTimeCB.SetWindowText(strSnooze);

	m_ctrlSnoozeTimeCB.EnableWindow(bEnableControls);
	m_ctrlSnoozeBtn.EnableWindow(bEnableControls);
	m_ctrlDimissBtn.EnableWindow(bEnableControls);

	int nItemsCount = m_ctrlRemindersList.GetItemCount();
	m_ctrlDimissAllBtn.EnableWindow(nItemsCount > 0);

	m_ctrlOpenItemBtn.EnableWindow(bEnableControls && m_pCalendar != NULL);
}

void CXTPCalendarRemindersDialog::SetRemindersManager(CXTPCalendarControl* pCalendar)
{
	m_Sink.UnadviseAll();
	m_ctrlRemindersList.DeleteAllItems();

	CMDTARGET_ADDREF(pCalendar);
	CMDTARGET_RELEASE(m_pCalendar);
	m_pCalendar = pCalendar;

	CMDTARGET_RELEASE(m_pRemindersMan);

	if (!m_pCalendar)
	{
		return;
	}

	CXTPNotifyConnection* pConn = m_pCalendar->GetConnection();
	ASSERT(pConn);
	if (pConn)
	{
		m_Sink.Advise(pConn, XTP_NC_CALENDAR_ON_REMINDERS, &CXTPCalendarRemindersDialog::OnReminders);
	}

	m_pRemindersMan = m_pCalendar->GetRemindersManager();
	CMDTARGET_ADDREF(m_pRemindersMan);

	UpdateFromManager();
}

void CXTPCalendarRemindersDialog::UpdateFromManager()
{
	CWaitCursor _WC;

	if (!m_pRemindersMan || !m_ctrlRemindersList.m_hWnd) {
		return;
	}

	m_pRemindersMan->GetActiveReminders(m_arActiveReminders);
	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();

	int nSelIdx = m_ctrlRemindersList.GetNextItem(-1, LVNI_FOCUSED|LVNI_SELECTED);
	m_ctrlRemindersList.DeleteAllItems();

	int nCount = m_arActiveReminders.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarReminder* pReminder = m_arActiveReminders.GetAt(i, FALSE);
		CXTPCalendarEventPtr ptrEvent = pReminder ? pReminder->GetEvent() : NULL;

		if (!ptrEvent) {
			ASSERT(FALSE);
			continue;
		}
		CString strText = ptrEvent->GetSubject();

		m_ctrlRemindersList.InsertItem(i, strText);

		//-----------------------------------------
		COleDateTimeSpan spDueIn = ptrEvent->GetStartTime() - dtNow;

		int nMunutes = GETTOTAL_MINUTES_DTS(spDueIn);
		strText = CXTPCalendarUtils::FormatTimeDuration(labs(nMunutes), TRUE);

		if (nMunutes < 0) {
			strText += _T(" ") + CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_OVERDUE);
		}

		m_ctrlRemindersList.SetItem(i, 1, LVIF_TEXT, strText, 0, 0, 0, 0);
	}

	SelectItem(nSelIdx);

	UpdateControlsBySelection();
	UpdateTitle();

	KillTimer(XTP_CALENDAR_RMD_REFRESH_ITEM_TIMERID);
	m_nNextRefreshIndex = -1;
	SetTimer(XTP_CALENDAR_RMD_REFRESH_TIMERID, XTP_CALENDAR_RMD_REFRESH_TIMEOUT, NULL);
}

void CXTPCalendarRemindersDialog::UpdateTitle()
{
	int nItemsCount = m_ctrlRemindersList.GetItemCount();

	CString strTitle;
	strTitle.Format((LPCTSTR)CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_REMINDERS), nItemsCount);
	SetWindowText(strTitle);
}

void CXTPCalendarRemindersDialog::SelectItem(int nSelItemIndex)
{
	int nItemsCount = m_ctrlRemindersList.GetItemCount();
	if (nSelItemIndex >= 0 && nItemsCount)
	{
		if (nSelItemIndex >= nItemsCount) {
			nSelItemIndex = nItemsCount - 1;
		}
		UINT uState = LVNI_FOCUSED|LVNI_SELECTED;
		m_ctrlRemindersList.SetItemState(nSelItemIndex, uState, uState);

		m_ctrlRemindersList.EnsureVisible(nSelItemIndex, FALSE);
	}
}

void CXTPCalendarRemindersDialog::OnReminders(XTP_NOTIFY_CODE Event, WPARAM wParam , LPARAM lParam)
{
	if (Event != XTP_NC_CALENDAR_ON_REMINDERS)
	{
		ASSERT(FALSE);
		return;
	}

	int eAction = (int)wParam;

	if (eAction == xtpCalendarRemindersFire ||
		eAction == xtpCalendarReminderDismissedAll)
	{
		UpdateFromManager();

		if (m_bAutoPopup && m_pRemindersMan && m_pRemindersMan->GetActiveRemindersCount())
		{
			ShowWindow(SW_SHOWNORMAL);
		}
	}
	else if (eAction == xtpCalendarReminderSnoozed ||
			 eAction == xtpCalendarReminderDismissed)
	{
		CXTPCalendarReminder* pReminder = (CXTPCalendarReminder*)lParam;
		ASSERT(pReminder);

		if (!_RemoveReminderInfo(pReminder))
		{
			ASSERT(FALSE);
			UpdateFromManager();
		}
	}
	else if (eAction == xtpCalendarRemindersMonitoringStarted)
	{
		ASSERT(!m_pRemindersMan);
		ASSERT(m_pCalendar);

		CMDTARGET_RELEASE(m_pRemindersMan);
		m_pRemindersMan = m_pCalendar ? m_pCalendar->GetRemindersManager() : NULL;
		CMDTARGET_ADDREF(m_pRemindersMan);

		UpdateFromManager();
	}
	else if (eAction == xtpCalendarRemindersMonitoringStopped)
	{
		m_ctrlRemindersList.DeleteAllItems();
		m_arActiveReminders.RemoveAll();

		UpdateControlsBySelection();

		KillTimer(XTP_CALENDAR_RMD_REFRESH_TIMERID);
		KillTimer(XTP_CALENDAR_RMD_REFRESH_ITEM_TIMERID);

		CMDTARGET_RELEASE(m_pRemindersMan);
	}

	if (!m_arActiveReminders.GetCount())
	{
		ShowWindow(SW_HIDE);
	}
}

void CXTPCalendarRemindersDialog::OnOK()
{
	OnSnooze();
}

void CXTPCalendarRemindersDialog::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);

	if (nIDEvent == XTP_CALENDAR_RMD_REFRESH_TIMERID)
	{
		if (m_nNextRefreshIndex < 0)
		{
			m_nNextRefreshIndex = 0;
			SetTimer(XTP_CALENDAR_RMD_REFRESH_ITEM_TIMERID, XTP_CALENDAR_RMD_REFRESH_ITEM_TIMEOUT, NULL);
		}
		return;
	}

	//=======================================================================
	if (nIDEvent != XTP_CALENDAR_RMD_REFRESH_ITEM_TIMERID)
	{
		return;
	}

	if (!m_pRemindersMan || !m_ctrlRemindersList.m_hWnd)
	{
		ASSERT(FALSE);
		return;
	}

	m_pRemindersMan->GetActiveReminders(m_arActiveReminders);
	COleDateTime dtNow = CXTPCalendarUtils::GetCurrentTime();

	int nCount = m_arActiveReminders.GetCount();
	if (m_nNextRefreshIndex < nCount)
	{
		int nIndex = m_nNextRefreshIndex;
		m_nNextRefreshIndex++;

		CXTPCalendarReminder* pReminder = m_arActiveReminders.GetAt(nIndex, FALSE);
		CXTPCalendarEventPtr ptrEvent = pReminder ? pReminder->GetEvent() : NULL;

		if (!ptrEvent) {
			ASSERT(FALSE);
			return;
		}

		//-----------------------------------------
		COleDateTimeSpan spDueIn = ptrEvent->GetStartTime() - dtNow;
		int nMunutes = GETTOTAL_MINUTES_DTS(spDueIn);
		CString strText = CXTPCalendarUtils::FormatTimeDuration(labs(nMunutes), TRUE);
		if (nMunutes < 0)
		{
			strText += _T(" ") + CXTPCalendarUtils::LoadString(XTP_IDS_CALENDAR_OVERDUE);
		}

		m_ctrlRemindersList.SetItem(nIndex, 1, LVIF_TEXT, strText, 0, 0, 0, 0);
	}
	else
	{
		m_nNextRefreshIndex = -1;
		KillTimer(XTP_CALENDAR_RMD_REFRESH_ITEM_TIMERID);
	}
}
