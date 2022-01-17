// XTPCalendarTip.h: interface for the CXTPCalendarTip class.
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
#if !defined(__XTPCalendarTip_H__)
#define __XTPCalendarTip_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//===========================================================================
// Summary:
//     The CXTPCalendarTip class emulates the functionality of a "tool tip"
//     control.
// Remarks:
//     The CXTPCalendarTip class encapsulates the functionality of
//     a "tip control", a small pop-up window that displays a
//     single line of text describing the hidden text when hovering over
//     an item on the report area.
//
//          CXTPCalendarTip provides the functionality to control the
//          the tip text, the size of the tip window itself, and the text
//          font of the tip.
//
//          This class is used by the report control internally.
//
// See Also: CXTPCalendarControl
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarTip : public CWnd
{
// Construction
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCalendarTip object.
	// Remarks:
	//     Construct a CXTPCalendarTip object in two steps.
	//     Call the constructor object and then call
	//     Create, which initializes the window and
	//     attaches it to the parent window.
	//
	// Example:
	// <code>
	// Declare a local CXTPCalendarTip object.
	// CXTPCalendarTip wndTip;
	//
	// // Declare a dynamic CXTPCalendarTip object.
	// CXTPCalendarTip* pTipWnd = new CXTPCalendarTip;
	// </code>
	// See Also: CWnd, Create, Activate
	//-----------------------------------------------------------------------
	CXTPCalendarTip();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCalendarTip object and handles cleanup and de-allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarTip();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to create a report tip window.
	// Parameters:
	//     pParentWnd - A pointer to a CWnd that is the parent report
	//                  control window.
	// Remarks:
	//     Construct an object in two steps.
	//     Call the constructor object and then call
	//     Create(), which initializes the window and
	//     attaches it to the parent window.
	//
	// Example:
	// <code>
	// void MyDialog::ShowToolTip(CRect rcTip, CXTPCalendarTip* pTipWnd)
	// {
	//     if (!pTipWnd->GetSafeHwnd())
	//     {
	//         pTipWnd->Create(this);
	//     }
	//     pTipWnd->SetHoverRect(rcTip);
	//     pTipWnd->Activate(TRUE);
	// }
	// </code>
	// Returns:
	//     TRUE if tip window is created successfully, FALSE otherwise.
	// See Also: CXTPCalendarTip::CXTPCalendarTip, Activate
	//-----------------------------------------------------------------------
	virtual BOOL Create(CWnd* pParentWnd);

// Attributes
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the tooltip text string.
	// Returns:
	//     A CString that contains the current tooltip text.
	// See Also: SetTooltipText
	//-----------------------------------------------------------------------
	CString GetTooltipText();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new tooltip text.
	// Parameters:
	//     str - An LPCSTR string that contains the new tooltip caption.
	// Remarks:
	//     Call this member function before activating the tooltip window.
	// See Also: GetTooltipText
	//-----------------------------------------------------------------------
	void SetTooltipText(LPCTSTR str);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the tooltip text font.
	// Parameters:
	//     pFont - A pointer to a CFont object that contains the new font.
	//-----------------------------------------------------------------------
	void SetFont(CFont* pFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the coordinates of the
	//     tooltip hover rectangle.
	// Parameters:
	//     rc - A CRect object that contains the new tooltip hover rectangle
	//          in the parent window coordinates.
	// Remarks:
	//     Called this member function before activating the tooltip window.
	// See Also: GetHoverRect
	//-----------------------------------------------------------------------
	void SetHoverRect(CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function is used to obtain the coordinates of a tooltip hover rectangle.
	// Returns:
	//     The coordinates of the tooltip hover rectangle in parent window coordinates.
	// See Also: SetHoverRect
	//-----------------------------------------------------------------------
	CRect GetHoverRect();


// Operations
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to activate or deactivate a tool tip control.
	// Parameters:
	//     bActive - A BOOL. Specifies whether the tool tip control is to be activated or deactivated.
	// Remarks:
	//     If <i>bActivate</i> is <b>TRUE</b>, the control is activated.
	//     If <b>FALSE</b>, it is deactivated.
	//
	//          When a tool tip control is active, the tool tip information
	//          appears when the cursor is on a tool that is registered with
	//          the control. When it is inactive, the tool tip information
	//          does not appear, even when the cursor is on a tool.
	// Example: See Create method example.
	// See Also: Create, CXTPCalendarTip::CXTPCalendarTip, SetTooltipText
	//-----------------------------------------------------------------------
	void Activate(BOOL bActive);

	//-----------------------------------------------------------------------
	// Summary:
	//      This function is used to calculate tooltip rectangle.
	// Remarks:
	//      Standard window DC and stored font and tooltip text are used.
	// Returns:
	//     Size of the tooltip rectangle.
	// See Also: SetTooltipText, SetFont.
	//-----------------------------------------------------------------------
	CSize CalcToolTipRect();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPCalendarTip)
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnPaint();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

protected:
	CRect  m_rcHover;           // Hover window coordinates.
	CFont* m_pFont;             // Font for displaying tooltip text.
	CWnd * m_pParentWnd;        // Pointer to the parent report window.
	CString m_strTooltipText;   // Tooltip text to display.

};

AFX_INLINE BOOL CXTPCalendarTip::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
AFX_INLINE CString CXTPCalendarTip::GetTooltipText() {
	return m_strTooltipText;
}
AFX_INLINE void CXTPCalendarTip::SetTooltipText(LPCTSTR str) {
	m_strTooltipText = str;
}
AFX_INLINE void CXTPCalendarTip::SetFont(CFont* pFont) {
	m_pFont = pFont;
}
AFX_INLINE void CXTPCalendarTip::SetHoverRect(CRect rc) {
	m_rcHover = rc;
	MoveWindow(rc);
}
AFX_INLINE CRect CXTPCalendarTip::GetHoverRect() {
	return m_rcHover;
}


#endif // !defined(__XTPCalendarTip_H__)
