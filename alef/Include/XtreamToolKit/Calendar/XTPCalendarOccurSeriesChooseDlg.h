// XTPCalendarOccurSeriesChooseDlg.h interface for CXTPCalendarOccurSeriesChooseDlg class.
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
#ifndef _XTPCALENDAROCCURSERIESCHOOSEDLG_H__
#define _XTPCALENDAROCCURSERIESCHOOSEDLG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////

class CXTPCalendarEvent;
//===========================================================================
// Summary:
//      This class implements a dialog to choose between current occurrence and
//      whole recurrence event series (or pattern).
// Remarks:
//      This is useful in operations with events like edit or delete event.
//      The dialog items inscriptions are customizable and loaded from resources.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarOccurSeriesChooseDlg : public CDialog
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	// Parameters:
	//      pParent           - Pointer to the parent window.
	//      nIDResourceLabels - Dialog labels string resource ID.
	//      lpszResourceIcon  - Dialog icon resource ID.
	// Remarks:
	//      The string format for nIDResourceLabels is:
	//      "Title\\nOccurrence\\nSeries\\nMessage"
	//      Where:
	//          * Title      - Dialog window title text.
	//          * Occurrence - Occurrence radio button label.
	//          * Series     - Series radio button label.
	//          * Message    - Dialog message text. If message string has %s
	//                         format specifier - event subject is inserted
	//                         instead. See SetEvent() method.
	// See Also: ~CXTPCalendarEvent()
	//-----------------------------------------------------------------------
	CXTPCalendarOccurSeriesChooseDlg(CWnd* pParent, UINT nIDResourceLabels,
									LPCTSTR lpszResourceIcon = IDI_EXCLAMATION);

	//-----------------------------------------------------------------------
	// Summary:
	//      Default class destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarOccurSeriesChooseDlg();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      Update dialog information from the specified event.
	// Parameters:
	//      pEvent  - Pointer to the calendar event object.
	// Remarks:
	//      If message string has %s format specifier - event subject is
	//      inserted instead by default.
	//-----------------------------------------------------------------------
	virtual void SetEvent(CXTPCalendarEvent* pEvent);

	BOOL m_bOccur; // [in/out] Contain default/selected choice. If TRUE - occurrence is selected, otherwise series.

protected:
	//{{AFX_CODEJOCK_PRIVATE
	virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
	//}}AFX_CODEJOCK_PRIVATE

protected:
	CStatic m_ctrlMessage;  // Dialog message text control.
	CStatic m_ctrlIcon;     // Dialog icon control.
	CButton m_ctrlOccur;    // Occurrence radio button control.
	CButton m_ctrlSeries;   // Series radio button control.

	CString m_strTitle;     // Dialog title string.
	CString m_strMessage;   // Dialog message text prepared string.
	CString m_strOccur;     // Occurrence radio button label text string.
	CString m_strSeries;    // Series radio button label text string.

	UINT  m_nIDResourceLabels;  // Dialog labels string resource ID.
	HICON m_hIcon;              // Dialog icon resource ID.
};


#endif // _XTPCALENDAROCCURSERIESCHOOSEDLG_H__
