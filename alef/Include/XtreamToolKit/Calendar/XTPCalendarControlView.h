// XTPCalendarControlView.h: interface for the CXTPCalendarControlView class.
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
#if !defined(__XTPCALENDARCONTROLVIEW_H__)
#define __XTPCALENDARCONTROLVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarControl.h"
#include "XTPCalendarView.h"
#include "XTPCalendarDayView.h"
#include "XTPCalendarUtils.h"
#include "Resource.h"

class CXTPCalendarControlViewPrintOptions;
class CXTPCalendarPageSetupDialog;

//===========================================================================
// Summary:
//     Identifier of Calendar control child window of CXTPCalendarControlView.
//===========================================================================
const UINT XTP_ID_CALENDAR_CONTROL = 100;

//===========================================================================
// Summary:
//     Describes printing options for the Calendar Control.
// Remarks:
//     Use this class when you want to programmatically customize printing
//     options for the Calendar control.
//
//     See each property description to examine which customization options
//     are available.
// See Also:
//     CXTPPrintOptions, CXTPCalendarPageSetupDialog overview
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarControlViewPrintOptions : public CXTPPrintOptions
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarControlViewPrintOptions)
	//}}AFX_CODEJOCK_PRIVATE

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarControlViewPrintOptions();

	COleDateTime m_dtPrintFrom; // Top bound of the printing area for a day view.
	COleDateTime m_dtPrintTo;   // Bottom bound of the printing area for a day view.
	BOOL         m_bPrintFromToExactly; // Defines whether to cut printing
	                                    // area by showing only area between
	                                    // m_dtPrintFrom and m_dtPrintTo

	BOOL    m_bPrintDateHeader; // Defines whether to print a date header or not.
	LOGFONT m_lfDateHeaderFont; // Font for day header caption (Day 1 - Day 2)
	LOGFONT m_lfDateHeaderWeekDayFont; // Font for day header week day(Monday, ...)
	LOGFONT m_lfDateHeaderCalendarFont; // Font for day header small calendar.

	//BOOL m_bNoBackGround;
	//BOOL m_bGrayBackGround;
	//BOOL m_bGrayEventBackGround;

	//-----------------------------------------------------------------------
	// Summary:
	//     Copies all print options from a specified PrintOptions object.
	// Parameters:
	//     pSrc - A pointer to the object to copy options from.
	//-----------------------------------------------------------------------
	void Set(const CXTPCalendarControlViewPrintOptions* pSrc);

	virtual LCID GetActiveLCID();

protected:

};

//===========================================================================
// Summary:
//     Customized Calendar control page setup dialog.
// Remarks:
//     Use this class when you want to allow user to customize printing
//     options for the Calendar control.
//
//     This class enhances standard MFC page setup dialog, which encapsulates
//     the services provided by the Windows common OLE Page Setup dialog box
//     with additional support for setting and modifying print margins.
//     This class is designed to take the place of the Print Setup dialog box.
//
// See Also:
//     CXTPCalendarControlViewPrintOptions overview,
//     CPageSetupDialog overview
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarPageSetupDialog : public CPageSetupDialog
{
public:
//{{AFX_CODEJOCK_PRIVATE
	enum {IDD = XTP_IDD_CALENDAR_PRINT_PAGE_SETUP};
//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//     Class constructor.
	// Parameters:
	//     pOptions - A pointer to an associated CXTPCalendarControlViewPrintOptions
	//                object for storing user's interaction results.
	//     dwFlags  - One or more flags you can use to customize the settings
	//                of the dialog box. The values can be combined using the
	//                bitwise-OR operator. For more details see CPageSetupDialog::CPageSetupDialog
	//     pParentWnd - Pointer to the dialog box's parent or owner.
	// Remarks:
	//     Use the DoModal function to display the dialog box.
	// See also:
	//      CPageSetupDialog::CPageSetupDialog()
	//-----------------------------------------------------------------------
	CXTPCalendarPageSetupDialog(CXTPCalendarControlViewPrintOptions* pOptions,
								DWORD dwFlags = PSD_MARGINS | PSD_INWININIINTLMEASURE,
								CWnd* pParentWnd = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class constructor, handles members cleanup.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarPageSetupDialog();

protected:
	CXTPCalendarControlViewPrintOptions* m_pOptions; // Pointer to associated print options object.

	CComboBox   m_ctrlPrintFromCmb; // A combo for choosing CXTPCalendarControlViewPrintOptions::m_dtPrintFrom value
	CComboBox   m_ctrlPrintToCmb;   // A combo for choosing CXTPCalendarControlViewPrintOptions::m_dtPrintTo value

	CButton     m_ctrlPrintFromTo;  // A check box for editing CXTPCalendarControlViewPrintOptions::m_bPrintFromToExactly value

	CButton     m_ctrlPrintDateHeader;  // A check box for editing CXTPCalendarControlViewPrintOptions::m_bPrintDateHeader value
	CStatic     m_ctrlDateHeaderFontLabel;  // A label for displaying CXTPCalendarControlViewPrintOptions::m_lfDateHeaderFont caption
	CEdit       m_ctrlDateHeaderFont;       // A label for displaying CXTPCalendarControlViewPrintOptions::m_lfDateHeaderFont value
	CButton     m_ctrlDateHeaderFontButton; // A button for choosing CXTPCalendarControlViewPrintOptions::m_lfDateHeaderFont value

	CButton     m_ctrlBlackWhitePrinting;  // A check box for editing CXTPCalendarControlViewPrintOptions::m_bBlackWhitePrinting value

	LOGFONT     m_lfDateHeader;     // Date header font temporary storage

	CEdit       m_ctrlHeaderFormat;         // An edit for header format string
	CEdit       m_ctrlFooterFormat;         // An edit for footer format string

	CButton     m_ctrlHeaderFormatBtn;          // A button to show header specifiers menu
	CButton     m_ctrlFooterFormatBtn;          // A button to show footer specifiers menu

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts font name to a human readable string.
	// Parameters:
	//     lfFont - A reference to a font.
	// Returns:
	//     A string describing a provided font.
	//-----------------------------------------------------------------------
	CString FormatFontName(const LOGFONT& lfFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Dialog-box initialization.
	// Returns:
	//     FALSE because it has explicitly set the input focus to one of the
	//     controls in the dialog box.
	// See also:
	//     CPageSetupDialog::OnInitDialog().
	//-----------------------------------------------------------------------
	virtual BOOL OnInitDialog();

	//-----------------------------------------------------------------------
	// Summary:
	//     Called when the user clicks the OK button (the button with an ID of IDOK).
	// Remarks:
	//     Saves all values into m_pOptions structure.
	// See also:
	//     CPageSetupDialog::OnOK().
	//-----------------------------------------------------------------------
	virtual void OnOK();

	//-----------------------------------------------------------------------
	// Summary:
	//     Called when the user clicks the Print Date Header check box.
	// Remarks:
	//     Enables/disables all controls related to configuring a Date Header.
	//-----------------------------------------------------------------------
	afx_msg void OnBnClickedPrintDateHeader();

	//-----------------------------------------------------------------------
	// Summary:
	//     Called when the user clicks the Date Header font button.
	// Remarks:
	//     Shows standard Font dialog in order to customize a Date Header font.
	//-----------------------------------------------------------------------
	afx_msg void OnBnClickedDateHeaderFont();

//{{AFX_CODEJOCK_PRIVATE
	afx_msg void OnBnClickedHeaderFormat();
	afx_msg void OnBnClickedFooterFormat();
	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE
};

//===========================================================================
// Summary:
//     The CXTPCalendarControlView class provides an implementation of
//     the Calendar control as view class. Use GetCalendarCtrl() method
//     to access methods of CXTPCalendarControl
// See Also: CXTPCalendarControl
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarControlView : public CView
{
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNCREATE(CXTPCalendarControlView)
//}}AFX_CODEJOCK_PRIVATE
protected:

	//-------------------------------------------------------------------------
	// Summary:
	//     Protected constructor used by dynamic creation
	//-------------------------------------------------------------------------
	CXTPCalendarControlView();


public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to access Calendar control object.
	// Returns:
	//     Reference to child CXTPCalendarControl window
	//-----------------------------------------------------------------------
	virtual CXTPCalendarControl& GetCalendarCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom Calendar control object.
	// Parameters:
	//     pCalendar - Pointer to custom report object
	//-----------------------------------------------------------------------
	void SetCalendarCtrl(CXTPCalendarControl* pCalendar, BOOL bReleaseWhenDestroy = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to access print options for this view.
	// Returns:
	//     Pointer to an object describing print options of this view.
	//-----------------------------------------------------------------------
	CXTPCalendarControlViewPrintOptions* GetPrintOptions();

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to set external scroll bar control.
	// Parameters:
	//      pScrollBar - Pointer to a scrollbar object.
	// See Also: CScrollBar
	//-----------------------------------------------------------------------
	void SetScrollBarCtrl(CScrollBar* pScrollBar);

protected:

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCalendarControlView object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTPCalendarControlView();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws main (central) date header area.
	// Parameters:
	//     bCalculate - If TRUE, then only calculate rectangles and do not
	//                  performs drawing itself.
	//     pDC        - Pointer to a valid device context.
	//     pOptions   - Pointer to calendar Print Options object.
	//     rrcHeader  - [out] Drawn rectangle area.
	//-----------------------------------------------------------------------
	virtual void DrawDateHeader(BOOL bCalculate, CDC* pDC,
							   CXTPCalendarControlViewPrintOptions* pOptions,
							   CRect& rrcHeader);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws month calendar in the right part of the central date header
	//     area.
	// Parameters:
	//     bCalculate - If TRUE, then only calculate rectangles and do not
	//                  performs drawing itself.
	//     pDC        - Pointer to a valid device context.
	//     pOptions   - Pointer to calendar Print Options object.
	//     dtYearMonth- Which month's calendar to draw.
	//     rrcHeaderCalendar - [out] Drawn rectangle area.
	//-----------------------------------------------------------------------
	virtual void _DrawDateHeaderCalendar(BOOL bCalculate, CDC* pDC,
									CXTPCalendarControlViewPrintOptions* pOptions,
									COleDateTime dtYearMonth,
									CRect& rrcHeaderCalendar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates a number of shown months and captions for them to be
	//     drawn on the date header.
	// Parameters:
	//     dtYearMonth1 - Bottom boundary of the date period.
	//     dtYearMonth1 - Top boundary of the date period.
	//     strHeader1       - [out] Bottom boundary caption.
	//     strHeader2       - [out] Top boundary caption.
	//     strHeaderWeekDay - [out] Week day name (for a single day view only).
	// Returns:
	//     A number of months between bottom and top boundaries, including them.
	//-----------------------------------------------------------------------
	virtual int _GetDateHeaderMonths(COleDateTime& dtYearMonth1, COleDateTime& dtYearMonth2,
									 CString& strHeader1, CString& strHeader2,
									 CString& strHeaderWeekDay);


	//-----------------------------------------------------------------------
	// Summary:
	//     Adjusts calendar printing rectangle by adding space for a header,
	//     footer and similar areas.
	// Parameters:
	//     pDC        - Pointer to a valid device context.
	//     rcCalendar - Calendar printing rectangle.
	//     pOptions   - Pointer to calendar Print Options object.
	// Returns:
	//     Adjusted calendar printing rectangle.
	//-----------------------------------------------------------------------
	virtual CRect AdjustCalendarForPrint(CDC* pDC, CRect rcCalendar,
										 CXTPCalendarControlViewPrintOptions* pOptions);

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts a rectangle of HIMETRIC units into logical units.
	// Parameters:
	//     pDC        - Pointer to a valid device context.
	//     rcHimetric - A rectangle to convert in HIMETRIC units.
	// Remarks:
	//     Use this function when you get HIMETRIC sizes from OLE and wish to
	//     convert them to your application's natural mapping mode.
	//     The conversion is accomplished by first converting the HIMETRIC
	//     units into pixels and then converting these units into logical
	//     units using the device context's current mapping units. Note that
	//     the extents of the device's window and viewport will affect the result.
	// Returns:
	//     Converted rectangle in logical units.
	// See also:
	//     CDC::HIMETRICtoLP()
	//-----------------------------------------------------------------------
	static CRect _HimetricToLP(CDC* pDC, const CRect rcHimetric);

protected:

//{{AFX_CODEJOCK_PRIVATE
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPCalendarControlView)
	virtual void OnDraw(CDC* pDC);
	CScrollBar* GetScrollBarCtrl(int nBar) const;

	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);

	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void _OnPrint2(CDC* pDC, CPrintInfo* pInfo, CRect rcPrint);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPCalendarControlView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	//}}AFX_MSG

	//{{AFX_MSG(CXTPCalendarControlView)
	afx_msg void OnUndo();
	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnUpdateCommand(CCmdUI* pCmdUI);
	afx_msg void OnFilePageSetup();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:

//{{AFX_CODEJOCK_PRIVATE
	class CXTPCalendarPritAdjustContext
	{
	public:
		CXTPCalendarPritAdjustContext() {
			Reset();
		}

		void Reset()
		{
			m_bAdjust = TRUE;
			m_rcCalendar.SetRectEmpty();
			m_rectDraw_prev.SetRectEmpty();
		}


		BOOL    m_bAdjust;

		CRect   m_rcHeader;
		CRect   m_rcDateHeader;
		CRect   m_rcCalendar;
		CRect   m_rcFooter;

		CRect   m_rectDraw_prev;
	};

	class CXTPCalendarBeforePrintState
	{
	public:
		CXTPCalendarBeforePrintState()
		{
			ClearData();
		}

		BOOL m_bDataValid;
		COleDateTimeSpan m_spScale_orig;
		int m_nTopRow;

		void ClearData()
		{
			m_bDataValid = FALSE;

			m_spScale_orig = 0;
			m_nTopRow = 0;
		}

		void Save(CXTPCalendarControlView* pView)
		{
			m_bDataValid = FALSE;
			m_spScale_orig = 0;
			m_nTopRow = 0;

			int nViewType = pView->GetCalendarCtrl().GetActiveView()->GetViewType();
			if(nViewType == xtpCalendarDayView || nViewType == xtpCalendarWorkWeekView)
			{
				CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, pView->GetCalendarCtrl().GetDayView());
				ASSERT(pDayView);
				if(!pDayView) {
					return;
				}
				m_spScale_orig = pDayView->GetScaleInterval();
				m_nTopRow = pDayView->GetTopRow();

				m_bDataValid = TRUE;
			}
		}

		void Restore(CXTPCalendarControlView* pView)
		{
			if (!m_bDataValid) {
				return;
			}

			int nViewType = pView->GetCalendarCtrl().GetActiveView()->GetViewType();
			if(nViewType == xtpCalendarDayView || nViewType == xtpCalendarWorkWeekView)
			{
				CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, pView->GetCalendarCtrl().GetDayView());
				ASSERT(pDayView);
				if(!pDayView) {
					return;
				}
				pDayView->SetScaleInterval(m_spScale_orig);
				pView->CalendarDayView_ScrolV(m_nTopRow);
			}

			pView->GetCalendarCtrl().AdjustLayout();
		}
	};


	CXTPCalendarBeforePrintState         m_beforePrintState;
	CXTPCalendarPritAdjustContext        m_pritAdjustContext;
	CXTPCalendarControl::CUpdateContext* m_pUpdateContect;
	CBitmap                              m_bmpGrayDC;
//}}AFX_CODEJOCK_PRIVATE

	//------------------------------------------------------------------------
//{{AFX_CODEJOCK_PRIVATE
	friend class CCalendarControlCtrl;
	friend class CXTPCalendarBeforePrintState;
//}}AFX_CODEJOCK_PRIVATE

protected:

//{{AFX_CODEJOCK_PRIVATE
	void CalendarDayView_ScrolV(int nTopRow)
	{
		CXTPCalendarDayView* pDayView = DYNAMIC_DOWNCAST(CXTPCalendarDayView, GetCalendarCtrl().GetDayView());
		ASSERT(pDayView);
		if(pDayView) {
			pDayView->_ScrollV(nTopRow, nTopRow);
		}
	}
//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPCalendarControlViewPrintOptions* m_pPrintOptions; // Printing options.

private:
	CXTPCalendarControl  m_wndCalendar; // Child Calendar control window.

	CXTPCalendarControl* m_pCalendar;   // Child Calendar control pointer (used to set external calendar control).
	BOOL                 m_bReleaseCalendarWhenDestroy;
	CScrollBar*          m_pScrollBar;

public:
	BOOL    m_bPrintDirect;             // if TRUE - the Print Dialog is bypassed. FALSE by default.
	BOOL    m_bResizeControlWithView;   // if FALSE - attached Calendar control will not be resized with view. TRUE by default.
};

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE CXTPCalendarControlViewPrintOptions* CXTPCalendarControlView::GetPrintOptions() {
	return m_pPrintOptions;
}


#endif // !defined(__XTPCALENDARCONTROLVIEW_H__)
