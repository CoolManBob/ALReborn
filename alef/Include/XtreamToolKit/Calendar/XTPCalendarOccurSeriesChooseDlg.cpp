// XTPCalendarOccurSeriesChooseDlg.cpp: implementation of the
// CXTPCalendarOccurSeriesChooseDlg class.
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

#include "stdafx.h"
#include "Resource.h"

#include "Common/XTPResourceManager.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPCalendarEvent.h"
#include "XTPCalendarOccurSeriesChooseDlg.h"

HICON XTPLoadIconEx(LPCTSTR lpszResourceIcon)
{
	if (!lpszResourceIcon)
		return NULL;

	HICON hIcon = AfxGetApp()->LoadIcon(lpszResourceIcon);
	if (!hIcon)
		hIcon = ::LoadIcon(NULL, lpszResourceIcon);

	return hIcon;
}

// CXTPCalendarOccurSeriesChooseDlg dialog
CXTPCalendarOccurSeriesChooseDlg::CXTPCalendarOccurSeriesChooseDlg(CWnd* pParent,
							UINT nIDResourceLabels, LPCTSTR lpszResourceIcon)
{
	InitModalIndirect(XTPResourceManager()->LoadDialogTemplate(XTP_IDD_CALENDAR_DIALOG_OCURR_SERIES), pParent);

	m_bOccur = TRUE;
	m_nIDResourceLabels = nIDResourceLabels;
	m_hIcon = XTPLoadIconEx(lpszResourceIcon);

	SetEvent(NULL);
}

CXTPCalendarOccurSeriesChooseDlg::~CXTPCalendarOccurSeriesChooseDlg()
{
}

void CXTPCalendarOccurSeriesChooseDlg::SetEvent(CXTPCalendarEvent* pEvent)
{
	CString strDialog;
	VERIFY(XTPResourceManager()->LoadString(&strDialog, m_nIDResourceLabels));

	m_strTitle = strDialog.Left(strDialog.Find(_T('\n')));
	VERIFY(!m_strTitle.IsEmpty());
	DELETE_S(strDialog, 0, m_strTitle.GetLength() + 1);

	m_strOccur = strDialog.Left(strDialog.Find(_T('\n')));
	VERIFY(!m_strOccur.IsEmpty());
	DELETE_S(strDialog, 0, m_strOccur.GetLength() + 1);

	m_strSeries = strDialog.Left(strDialog.Find(_T('\n')));
	VERIFY(!m_strSeries.IsEmpty());
	DELETE_S(strDialog, 0, m_strSeries.GetLength() + 1);

	m_strMessage.Format(strDialog, pEvent ? (LPCTSTR)pEvent->GetSubject() : _T(""));
}

void CXTPCalendarOccurSeriesChooseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, XTP_IDC_CALENDAR_STATIC_OCCUR_MESSAGE, m_ctrlMessage);
	DDX_Control(pDX, XTP_IDC_CALENDAR_RADIO_OCCUR, m_ctrlOccur);
	DDX_Control(pDX, XTP_IDC_CALENDAR_RADIO_SERIES, m_ctrlSeries);
	DDX_Control(pDX, XTP_IDC_CALENDAR_STATIC_ICON, m_ctrlIcon);
}


BEGIN_MESSAGE_MAP(CXTPCalendarOccurSeriesChooseDlg, CDialog)
END_MESSAGE_MAP()

// CXTPCalendarOccurSeriesChooseDlg message handlers

BOOL CXTPCalendarOccurSeriesChooseDlg::OnInitDialog()
{
	#define OFFSETWINDOW(ID, OffsetTop, OffsetHeight)\
		GetDlgItem(ID)->GetWindowRect(rcCtrl);\
		ScreenToClient(&rcCtrl);\
		GetDlgItem(ID)->MoveWindow(rcCtrl.left, rcCtrl.top + OffsetTop, rcCtrl.Width(), rcCtrl.Height() + OffsetHeight);

	CDialog::OnInitDialog();

	m_ctrlOccur.SetCheck(m_bOccur ? 1 : 0);
	m_ctrlSeries.SetCheck(m_bOccur ? 0 : 1);

	if (!m_strTitle.IsEmpty())
	{
		SetWindowText(m_strTitle);
	}

	if (!m_strMessage.IsEmpty())
	{
		m_ctrlMessage.SetWindowText(m_strMessage);
	}

	if (!m_strOccur.IsEmpty())
	{
		m_ctrlOccur.SetWindowText(m_strOccur);
	}
	if (!m_strSeries.IsEmpty())
	{
		m_ctrlSeries.SetWindowText(m_strSeries);
	}
	if (m_hIcon)
	{
		m_ctrlIcon.SetIcon(m_hIcon);
	}

	CClientDC dc(&m_ctrlMessage);
	CFont* pOldFont = dc.SelectObject(m_ctrlMessage.GetFont());

	CXTPClientRect rcMessageCtrl(&m_ctrlMessage);

	CRect rcMessage(0, 0, rcMessageCtrl.Width(), 0);
	dc.DrawText(m_strMessage, rcMessage, DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK);

	int nOffset = rcMessage.Height() - rcMessageCtrl.Height();

	if (nOffset > 0)
	{
		CRect rcCtrl;
		GetWindowRect(rcCtrl);
		MoveWindow(rcCtrl.left, rcCtrl.top, rcCtrl.Width(), rcCtrl.Height() + nOffset);

		OFFSETWINDOW(XTP_IDC_CALENDAR_STATIC_OCCUR_MESSAGE, 0, nOffset);
		OFFSETWINDOW(XTP_IDC_CALENDAR_RADIO_OCCUR, nOffset, 0);
		OFFSETWINDOW(XTP_IDC_CALENDAR_RADIO_SERIES, nOffset, 0);
		OFFSETWINDOW(IDOK, nOffset, 0);
		OFFSETWINDOW(IDCANCEL, nOffset, 0);
	}

	dc.SelectObject(pOldFont);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CXTPCalendarOccurSeriesChooseDlg::OnOK()
{
	m_bOccur = m_ctrlOccur.GetCheck() != 0;

	CDialog::OnOK();
}
