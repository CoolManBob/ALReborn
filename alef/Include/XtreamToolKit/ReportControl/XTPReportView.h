// XTPReportView.h: interface for the CXTPReportView class.
//
// This file is a part of the XTREME REPORTCONTROL MFC class library.
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
#if !defined(__XTPREPORTVIEW_H__)
#define __XTPREPORTVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common/XTPDrawHelpers.h"

#include "XTPReportControl.h"
#include "resource.h"

//===========================================================================
// Summary:
//     Identifier of report control child window of CXTPReportView.
// Example:
// <code>
// int CXTPReportView::OnCreate(LPCREATESTRUCT lpCreateStruct)
// {
//     if (CView::OnCreate(lpCreateStruct) == -1)
//         return -1;
//
//     if (!m_wndReport.Create(WS_CHILD | WS_TABSTOP | WS_VISIBLE | WM_VSCROLL,
//         CRect(0, 0, 0, 0), this, XTP_ID_REPORT_CONTROL))
//     {
//         TRACE(_T("Failed to create tab control window\n"));
//         return -1;
//     }
//     return 0;
// }
// </code>
// See Also:
//     XTP_NM_REPORT_CHECKED, XTP_NM_REPORT_COLUMNORDERCHANGED, XTP_NM_REPORT_HEADER_RCLICK,
//     XTP_NM_REPORT_HYPERLINK, XTP_NM_REPORT_INPLACEBUTTONDOWN, XTP_NM_REPORT_SELCHANGED
//     XTP_NM_REPORT_SORTORDERCHANGED, XTP_NM_REPORT_VALUECHANGED
//     CXTPReportView, CXTPReportControl, CXTPReportHeader
//===========================================================================
const UINT XTP_ID_REPORT_CONTROL = 100;

class _XTP_EXT_CLASS CXTPReportViewPrintOptions : public CXTPPrintOptions
{
	DECLARE_DYNAMIC(CXTPReportViewPrintOptions)
	CXTPReportViewPrintOptions();

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
//     CXTPReportViewPrintOptions overview,
//     CPageSetupDialog overview
//===========================================================================
class _XTP_EXT_CLASS CXTPReportPageSetupDialog : public CPageSetupDialog
{
public:
	//{{AFX_CODEJOCK_PRIVATE
	enum {IDD = XTP_IDD_REPORT_PRINT_PAGE_SETUP};
	//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//     Class constructor.
	// Parameters:
	//     pOptions - A pointer to an associated CXTPReportViewPrintOptions
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
	CXTPReportPageSetupDialog(CXTPReportViewPrintOptions* pOptions,
		DWORD dwFlags = PSD_MARGINS | PSD_INWININIINTLMEASURE,
		CWnd* pParentWnd = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class constructor, handles members cleanup.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportPageSetupDialog();

protected:
	CXTPReportViewPrintOptions* m_pOptions; // Pointer to associated print options object.

	//-----------------------------------------------------------------------
	// Summary:
	//     Converts font name to a human readable string.
	// Parameters:
	//     lfFont - A reference to a font.
	// Returns:
	//     A string describing a provided font.
	//-----------------------------------------------------------------------
	//CString FormatFontName(const LOGFONT& lfFont);

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

	//{{AFX_CODEJOCK_PRIVATE
	CEdit       m_ctrlHeaderFormat;
	CEdit       m_ctrlFooterFormat;

	CButton     m_ctrlHeaderFormatBtn;
	CButton     m_ctrlFooterFormatBtn;

	afx_msg void OnBnClickedHeaderFormat();
	afx_msg void OnBnClickedFooterFormat();

	DECLARE_MESSAGE_MAP()
	//}}AFX_CODEJOCK_PRIVATE
};


//===========================================================================
// Summary:
//     The CXTPReportView class provides an implementation of
//     the Report control as view class. Use GetReportCtrl method to access methods
//     of CXTPReportControl
// See Also: CXTPReportControl
//===========================================================================
class _XTP_EXT_CLASS CXTPReportView : public CView
{
	DECLARE_DYNCREATE(CXTPReportView)
protected:

	//-------------------------------------------------------------------------
	// Summary:
	//     Protected constructor used by dynamic creation
	//-------------------------------------------------------------------------
	CXTPReportView();


public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to access Report control object.
	// Returns:
	//     Reference to child CXTPReportControl window
	//-----------------------------------------------------------------------
	virtual CXTPReportControl& GetReportCtrl() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom Report control object.
	// Parameters:
	//     pReport - Pointer to custom report object
	//-----------------------------------------------------------------------
	void SetReportCtrl(CXTPReportControl* pReport);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieved paint manager of the child report control.
	// Returns:
	//     Pointer to paint manager of report control.
	//-----------------------------------------------------------------------
	CXTPReportPaintManager* GetPaintManager() const;


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to access print options for this view.
	// Returns:
	//     Pointer to an object describing print options of this view.
	//-----------------------------------------------------------------------
	CXTPReportViewPrintOptions* GetPrintOptions();
protected:

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPReportView object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTPReportView();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to print single page of report control.
	// Parameters:
	//     pDC         - Pointer to a device context for page output.
	//     pInfo       - Points to a CPrintInfo structure that describes the
	//                   current print job.
	//     rcPage      - Page bounding rectangle
	//     nIndexStart - First row to print
	// Remarks:
	//     This method prints page header, page footer and call PrintReport method.
	// Returns:
	//     Index of last printed row
	//-----------------------------------------------------------------------
	virtual long PrintPage (CDC* pDC, CPrintInfo* pInfo, CRect rcPage, long nIndexStart);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to print report control (columns header and rows)
	//     on the page.
	// Parameters:
	//     pDC         - Pointer to a device context for page output.
	//     pInfo       - Points to a CPrintInfo structure that describes the current print job.
	//     rcPage      - Report bounding rectangle on the page
	//     nIndexStart - First row to print
	// Remarks:
	//     This method call PrintHeader, PrintRows methods.
	// Returns:
	//     Index of last printed row
	//-----------------------------------------------------------------------
	virtual long PrintReport (CDC* pDC, CPrintInfo* pInfo, CRect rcPage, long nIndexStart);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to print header of each page.
	// Parameters:
	//     pDC      - Pointer to a device context for page output.
	//     rcHeader - Header bounding rectangle
	//-----------------------------------------------------------------------
	virtual void PrintHeader(CDC* pDC, CRect rcHeader);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to print footer of each page.
	// Parameters:
	//     pDC      - Pointer to a device context for page output.
	//     rcFooter - Header bounding rectangle
	//-----------------------------------------------------------------------
	virtual void PrintFooter(CDC* pDC, CRect rcFooter);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw all rows inside bounding rectangle.
	// Parameters:
	//     pDC         - Pointer to a device context for page output.
	//     rcRows      - Bounding rectangle of rows
	//     nIndexStart - First row to print
	// Returns:
	//     Index of last printed row
	//-----------------------------------------------------------------------
	virtual int PrintRows(CDC* pDC, CRect rcRows, long nIndexStart);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called by PrintRows to print single row.
	// Parameters:
	//     pDC   - Pointer to a device context for page output.
	//     pRow  - Row to print.
	//     rcRow - Bounding rectangle of row
	//     nPreviewHeight - Height of preview
	//-----------------------------------------------------------------------
	virtual void PrintRow(CDC* pDC, CXTPReportRow* pRow, CRect rcRow, int nPreviewHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method calculates width of column need to print.
	// Parameters:
	//     pColumn     - Column need to print
	//     nTotalWidth - Total width of all columns.
	// Returns:
	//     Width of column.
	//-----------------------------------------------------------------------
	int GetColumnWidth(CXTPReportColumn* pColumn, int nTotalWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to set external scroll bar control.
	// Parameters:
	//      pScrollBar - Pointer to a scrollbar object.
	// See Also: CScrollBar
	//-----------------------------------------------------------------------
	void SetScrollBarCtrl(CScrollBar* pScrollBar);

protected:
//{{AFX_CODEJOCK_PRIVATE

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPReportView)
	virtual BOOL PaginateTo(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void _OnPrint2(CDC* pDC, CPrintInfo* pInfo, CRect rcPrint);
	CScrollBar* GetScrollBarCtrl(int nBar) const;
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPReportView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	//{{AFX_MSG(CXTPReportView)
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnFilePageSetup();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	CBitmap     m_bmpGrayDC;        // A temporary bitmap to convert colored report control image to 'gray' colors when printing in black&while mode.
	CUIntArray m_aPageStart;        // Printed indexes.
	CXTPReportControl m_wndReport;  // Child report control window.
	CXTPReportControl* m_pReport;   // Child report control pointer.
	BOOL m_bPrintSelection;         // TRUE if only printing the currently selected rows in the report, FALSE if printing the entire report,

	CXTPReportViewPrintOptions* m_pPrintOptions; // Printing options.
	CScrollBar* m_pScrollBar;           // Store pointer to external scrollbar control.

	friend class CReportControlCtrl;

public:
	BOOL    m_bAllowCut;                // If TRUE the cut operation is allowed.
	BOOL    m_bAllowPaste;              // If TRUE the paste operation is allowed.
	BOOL    m_bPrintDirect;             // if TRUE - the Print Dialog is bypassed. FALSE by default.
	BOOL    m_bResizeControlWithView;   // if FALSE - attached Report control will not be resized with view. TRUE by default.
};

/////////////////////////////////////////////////////////////////////////////
AFX_INLINE CXTPReportViewPrintOptions* CXTPReportView::GetPrintOptions() {
	return m_pPrintOptions;
}
#endif // !defined(__XTPREPORTVIEW_H__)
