// XTPScrollBar.h : interface for the CXTPScrollBar class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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
#if !defined(__XTPSCROLLBAR_H__)
#define __XTPSCROLLBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define XTP_HTSCROLLUP          60          // <combine CXTPScrollBase::HitTestScrollBar@POINT>
#define XTP_HTSCROLLDOWN        61          // <combine CXTPScrollBase::HitTestScrollBar@POINT>
#define XTP_HTSCROLLUPPAGE      62          // <combine CXTPScrollBase::HitTestScrollBar@POINT>
#define XTP_HTSCROLLDOWNPAGE    63          // <combine CXTPScrollBase::HitTestScrollBar@POINT>
#define XTP_HTSCROLLTHUMB       64          // <combine CXTPScrollBase::HitTestScrollBar@POINT>
#define XTP_HTSCROLLPOPUP       65          // <combine CXTPScrollBase::HitTestScrollBar@POINT>


//-------------------------------------------------------------------------
// Summary: Scroll Bar style
// See Also: CXTPScrollBase::SetScrollBarStyle
//-------------------------------------------------------------------------
enum XTPScrollBarStyle
{
	xtpScrollStyleDefault,          // Default style
	xtpScrollStyleStandard,         // standard window theme
	xtpScrollStyleFlat,             // Flat scrollbar style
	xtpScrollStyleSystem,           // WinXP themed style
	xtpScrollStyleOffice,           // Office scrollbar style
	xtpScrollStyleOffice2007Light,  // Office 2007 light style
	xtpScrollStyleOffice2007Dark,   // Office 2007 dark style
};

class CXTPControlGalleryPaintManager;


//===========================================================================
// Summary:
//     CXTPScrollBase is base class for all scrollbale objects
//===========================================================================
class _XTP_EXT_CLASS CXTPScrollBase
{
public:

	//===========================================================================
	// Summary:
	//     The SCROLLBARPOSINFO structure contains scroll bar information.
	//===========================================================================
	struct SCROLLBARPOSINFO
	{
		int    posMin;  // Minimum position
		int    posMax;  // Maximum position
		int    page;    // Page size
		int    pos;     // Position of thumb

		int    pxTop;   // Top bounding rectangle
		int    pxBottom;// Bottom bounding rectangle
		int    pxLeft;  // Left bounding rectangle
		int    pxRight; // Right bounding rectangle
		int    cpxThumb;// Size of thumb button
		int    pxUpArrow;   // Position of Up arrow
		int    pxDownArrow; // Position of Down arrow
		int    pxStart;     // Previous position of thumb button
		int    pxThumbBottom;   // Thumb bottom bounding rectangle
		int    pxThumbTop;      // Thumb top bounding rectangle
		int    pxPopup; // In-place scroll position
		int    cpx;     // position in pixels
		int    pxMin;   // Minimum position in pixels

		RECT   rc;      // Bounding rectangle
		int    ht;      // Hit test
		BOOL   fVert;   // TRUE if vertical
	};

	//===========================================================================
	// Summary:
	//     The SCROLLBARTRACKINFO structure contains tracking information.
	//===========================================================================
	struct SCROLLBARTRACKINFO
	{
		BOOL   fHitOld;     // Previous hittest
		RECT   rcTrack;     // Bounding rectangle of available thumb position
		UINT   cmdSB;       // Scroll command
		UINT_PTR  hTimerSB; // Timer identifier
		int    dpxThumb;    // Delta of thumb button
		int    pxOld;       // Previous position in pixels
		int    posOld;      // Previous position
		int    posNew;      // new position
		HWND   hWndTrack;   // Parent tracking window

		BOOL   bTrackThumb; // TRUE if thumb is tracking
		SCROLLBARPOSINFO* pSBInfo;  // SCROLLBARPOSINFO pointer
	};

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPScrollBase object
	//-----------------------------------------------------------------------
	CXTPScrollBase();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPScrollBase object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPScrollBase();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set style of scrollbar
	// Parameters:
	//     scrollStyle - Style to be set
	// See Also: GetScrollBarStyle
	//-----------------------------------------------------------------------
	void SetScrollBarStyle(XTPScrollBarStyle scrollStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get style of scrollbar
	// See Also: SetScrollBarStyle
	//-----------------------------------------------------------------------
	XTPScrollBarStyle GetScrollBarStyle() const;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get SCROLLBARTRACKINFO information
	// See Also: GetScrollBarPosInfo
	//-----------------------------------------------------------------------
	SCROLLBARTRACKINFO* GetScrollBarTrackInfo();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get SCROLLBARPOSINFO information
	// See Also: GetScrollBarTrackInfo
	//-----------------------------------------------------------------------
	SCROLLBARPOSINFO* GetScrollBarPosInfo();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if scrollbar is enabled
	//-----------------------------------------------------------------------
	virtual BOOL IsScrollBarEnabled() const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves parent window of scrollbar
	// Returns:
	//     Pointer to parent window
	//-----------------------------------------------------------------------
	virtual CWnd* GetParentWindow() const = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to check ScrollBar part under pointer.
	// Parameters:
	//     pt - Mouse pointer to test
	//-----------------------------------------------------------------------
	int HitTestScrollBar(POINT pt);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to start scrollbar tracking
	// Parameters:
	//     hWnd - Parent Window handle
	//     point - Starting cursor position
	//     pSBInfo - Scrollbar information
	//     bDirect - TRUE to scroll to specified position
	//-----------------------------------------------------------------------
	void PerformTrackInit(HWND hWnd, CPoint point, SCROLLBARPOSINFO* pSBInfo, BOOL bDirect);


protected:
//{{AFX_CODEJOCK_PRIVATE
	void EndScroll(BOOL fCancel);
	void MoveThumb(int px);
	void TrackThumb(UINT message, CPoint pt);
	void TrackBox(UINT message, CPoint point);
	void ContScroll();
//}}AFX_CODEJOCK_PRIVATE

protected:

//{{AFX_CODEJOCK_PRIVATE
	void SetupScrollInfo();
	virtual CRect GetScrollBarRect() = 0;
	virtual void GetScrollInfo(SCROLLINFO* psi) = 0;
	virtual void DoScroll(int cmd, int pos) = 0;
	virtual void RedrawScrollBar() = 0;
	virtual void CalcScrollBarInfo(LPRECT lprc, SCROLLBARPOSINFO* pSBInfo, SCROLLINFO* pSI);
	virtual CXTPControlGalleryPaintManager* GetGalleryPaintManager() const = 0;
	void CalcTrackDragRect(SCROLLBARTRACKINFO* pSBTrack) const;
//}}AFX_CODEJOCK_PRIVATE
protected:
	SCROLLBARPOSINFO m_spi;                 // ScrollBar position.
	SCROLLBARTRACKINFO* m_pSBTrack;         // ScrollBar tracking
	XTPScrollBarStyle m_scrollBarStyle;     // ScrollBar style

};

//===========================================================================
// Summary:
//     CXTPScrollBar is a CScrollBar derived class.
//===========================================================================
class _XTP_EXT_CLASS CXTPScrollBar : public CScrollBar, public CXTPScrollBase
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPScrollBar object
	//-----------------------------------------------------------------------
	CXTPScrollBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPScrollBar object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPScrollBar();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the command bar's object.
	// Parameters:
	//     pCommandBars - CXTPCommandBars to be set.
	//-----------------------------------------------------------------------
	void SetCommandBars(CXTPCommandBars* pCommandBars);

protected:

//{{AFX_CODEJOCK_PRIVATE
	virtual CRect GetScrollBarRect();
	virtual void GetScrollInfo(SCROLLINFO* psi);
	virtual BOOL IsScrollBarEnabled() const;
	virtual CXTPControlGalleryPaintManager* GetGalleryPaintManager() const;
	virtual CWnd* GetParentWindow() const;
	virtual void DoScroll(int cmd, int pos);
	virtual void RedrawScrollBar();
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPScrollBar)
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSetScrollInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetScrollInfo(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPCommandBars* m_pCommandBars;            // Parent CommandBars object
};

AFX_INLINE CXTPScrollBase::SCROLLBARTRACKINFO* CXTPScrollBase::GetScrollBarTrackInfo() {
	return m_pSBTrack;
}
AFX_INLINE CXTPScrollBase::SCROLLBARPOSINFO* CXTPScrollBase::GetScrollBarPosInfo() {
	return &m_spi;
}
AFX_INLINE void CXTPScrollBase::SetScrollBarStyle(XTPScrollBarStyle scrollStyle) {
	m_scrollBarStyle = scrollStyle;
}

AFX_INLINE void CXTPScrollBar::SetCommandBars(CXTPCommandBars* pCommandBars) {
	m_pCommandBars = pCommandBars;
}

#endif //#if !defined(__XTPSTATUSBAR_H__)
