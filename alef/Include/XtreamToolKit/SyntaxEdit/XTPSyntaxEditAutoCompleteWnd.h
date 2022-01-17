// XTPSyntaxEditAutoCompleteWnd.h: interface for the CXTPSyntaxEditAutoCompleteWnd class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSYNTAXEDITAUTOCOMPLETEWND_H__)
#define __XTPSYNTAXEDITAUTOCOMPLETEWND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//===========================================================================
// Summary:
//     XTP_EDIT_ACDATA structure defines items displayed data. Provides text and
//     icon for each displayed line in Auto-complete window.
//===========================================================================
struct XTP_EDIT_ACDATA
{
	int m_nIcon;
	CString m_strText;

	XTP_EDIT_ACDATA(int nIcon, CString strText)
	{
		m_nIcon = nIcon;
		m_strText = strText;
	}

};

//===========================================================================
// Summary:
//     CXTPSyntaxEditACDataArray type defines a custom array to store AutoComplete items
//     displayed data.
// See Also:
//     PXTP_EDIT_ACDATA
//===========================================================================
typedef CArray<XTP_EDIT_ACDATA*, XTP_EDIT_ACDATA*> CXTPSyntaxEditACDataArray;

//===========================================================================
// Summary:
//     CXTPSyntaxEditACGrid type defines a custom array to store and manage visible
//     rectangle arrays that represented lines of data in AutoComplete window.
//===========================================================================
typedef CArray<CRect, CRect> CXTPSyntaxEditACGrid;


class CXTPSyntaxEditCtrl;

//===========================================================================
// Summary:
//     This class implements AutoComplete window. It provides facilities
//     to display predefined strings of data in the sorted order. Then
//     user can navigate over list of strings and choose desired text to
//     complete his input. AutoComplete window provides some ways to
//     navigate by using vertical scroll bar, up/down keys, mouse.
//     If user types alphanumeric keys after AutoComplete window was
//     appeared it uses user input as search criteria and tries to find
//     text from its predefined strings of data that are like user input.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditAutoCompleteWnd : public CWnd
{

	friend class CXTPSyntaxEditCtrl;
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default class constructor.
	//-----------------------------------------------------------------------
	CXTPSyntaxEditAutoCompleteWnd();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSyntaxEditAutoCompleteWnd object, handles cleanup
	//     and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditAutoCompleteWnd();

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates AutoComplete window and sets predefined properties.
	// Parameters:
	//     pParentWnd: [in] Pointer to the parent window.
	// Remarks:
	//     Before create Auto-complete window you should construct
	//     CXTPSyntaxEditAutoCompleteWnd object by calling constrictor.
	// Returns:
	//     TRUE if success; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL Create(CWnd* pParentWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets list of data to be displayed in AutoComplete window.
	// Parameters:
	//     parrData: [in] reference to array of data.
	// Remarks:
	//     AutoComplete window displays list of data for user choice.If
	//     AutoComplete window has already had a list of displayed data it will
	//     be replaced by new one. Before add to the AutoComplete window new
	//     data is sorted.
	// See Also:
	//    CXTPSyntaxEditACDataArray
	//-----------------------------------------------------------------------
	void SetList(CXTPSyntaxEditACDataArray& parrData);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets list of open tags for AutoComplete window.
	// Parameters:
	//     strOpenTags: [in] string with new open tags.
	// Remarks:
	//     Open Tags are treated by AutoComplete window as command to be
	//     displayed and activated. Each tag may consists of one or more symbols.
	//     Tags must be separated by "~" sign. If AutoComplete window has
	//     already had a list of tags it will be replaced by new one.
	//-----------------------------------------------------------------------
	void SetOpenTags(CString strOpenTags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if user input is open tag.
	// Parameters:
	//     strToTest: [in] string to test.
	// Remarks:
	//     Call this function to determine if user enters open tag.
	//     Function uses incremental search and temporary stores input
	//     that resembles initial part of open tags that consist of
	//     number of symbols.
	// Returns:
	//     TRUE if open tag detected; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsOpenTag(CString strToTest);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets list of close tags for AutoComplete window.
	// Parameters:
	//     strCloseTags: [in] string with new close tags.
	// Remarks:
	//     Close Tags are treated by AutoComplete window as command to be
	//     hidden and deactivated. Each tag may consists of one or more symbols.
	//     Tags must be separated by "~" sign. If AutoComplete window has
	//     already had a list of tags it will be replaced by new one.
	//-----------------------------------------------------------------------
	void SetCloseTags(CString strCloseTags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if user input is Close tag.
	// Parameters:
	//      strToTest: [in] String to test.
	// Remarks:
	//     Call this function to determine if user enters close tag.
	//     Function uses incremental search and temporary stores input that
	//     resembles initial part of close tags that consist of number of
	//     symbols.
	// Returns:
	//     TRUE if close tag detected; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsCloseTag(CString strToTest);

	//-----------------------------------------------------------------------
	// Summary:
	//     Shows AutoComplete window.
	// Parameters:
	//     pt:        [in] Point where left upper corner of AutoComplete
	//                window should appears.
	//     strSearch: [in] Search string.
	// Remarks:
	//     Shows AutoComplete window at specified point and try to filter
	//     dataset by previously typed piece of word and highlighted line
	//     according search string.
	//-----------------------------------------------------------------------
	void Show(CPoint pt, CString strSearch = _T(""));

	//-----------------------------------------------------------------------
	// Summary:
	//     Hides AutoComplete window.
	//-----------------------------------------------------------------------
	void Hide();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns active flag for AutoComplete window.
	// Returns:
	//     TRUE if window is visible and active; FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsActive();

	//-----------------------------------------------------------------------
	// Summary:
	//     Updates display settings from system settings.
	//-----------------------------------------------------------------------
	void RefreshMetrics();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns text line where user click mouse.
	// Parameters:
	//     ptTest: [in] tested point.
	// Returns:
	//     Integer identifier of affected text line.
	//-----------------------------------------------------------------------
	int HitTest(CPoint ptTest);

	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	// message handlers
	void OnLButtonDown(UINT nFlags, CPoint point);

	void OnRButtonDown(UINT nFlags, CPoint point);

	void OnLButtonDblClk( UINT nFlags, CPoint point );

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );

	void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void OnPaint();

	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_CODEJOCK_PRIVATE

protected: // Data Members

	CString     m_strSearch;    // Temporary stores chain of chars from user input to perform
	                            // incremental search
	CString     m_strCloseTags; // Stores set of closed tags divided by ~
	                            // input closed tags lead to close AutoComplete window
	CString     m_strTmpCloseTag;// Temporary stores chain of chars from user input to
								// recognize close tags
	CString     m_strOpenTags;  // Stores open tags

	CString     m_strTmpOpenTag;// Temporary stores chain of chars from user input to
	                            // recognize Open tags

	CXTPSyntaxEditCtrl* m_pParentWnd;   // Pointer to the parent window

	CFont m_fontBasic;          // Font to display text
	COLORREF m_clrWindow;       // Standard window color
	COLORREF m_clrHighLight;    // Standard window highlight color
	COLORREF m_clrWindowText;   // Standard windows text color
	COLORREF m_clrHighLightText;// Standard windows highlight text color

	CXTPSyntaxEditACDataArray m_arrACData;// Stores list of choices (line of text and image)

	CXTPSyntaxEditACDataArray m_arrACDataFiltered; // Stores filtered list of choices (line of text and image)

	CXTPSyntaxEditACGrid m_arrGrid;     // Stores list of rectangle areas where were
								// displayed text\image strings from m_arrACData

	int m_nBordersHeight;       // Stores summary thickness of vertical window borders
	int m_nLineHeight;          // Stores height of line
	int m_nLines;               // Number of visible lines
	int m_nWndHeight;           // Height of window

	int m_nHighLightLine;       // Current highlight line identifier
	int m_nFirstDisplayedStr;   // Current first visible (upper) line

	BOOL m_bActive;             // Active flag

	BOOL m_bFilteredMode;       // Flag to identify is current mode (filtered/full);

	BOOL m_bHighLight;          // This flag determines if selected string is highlighted.

	int m_nStartReplacePos;     // Position of first char in the Edit Control which will be replaced
	                            // by the chosen text
	int m_nEndReplacePos;       // Position of last char in the Edit Control which will be replaced
	                            // by the chosen text
	CImageList m_ilACGlyphs;    // Images for AutoComplete list

	CString m_strDelims;        // stores char list that are tag delimiters


private:
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

	BOOL AdjusLayout();

	void RemoveAll();

	BOOL ScrollTo(int nNewLine);

	void ReturnSelected(BOOL bAdjust = FALSE);

	void Sort();

	static int _cdecl CompareACData(const XTP_EDIT_ACDATA** p1, const XTP_EDIT_ACDATA** p2);

	int Search(CString strSearch = _T(""));

	int Filter(CString strSearch = _T(""));

	static int _cdecl CompareACDataToSearch(const XTP_EDIT_ACDATA** ppKey, const XTP_EDIT_ACDATA** ppElem);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPSYNTAXEDITAUTOCOMPLETEWND_H__)
