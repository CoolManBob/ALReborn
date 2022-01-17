// XTPSyntaxEditToolTipCtrl.h: interface for the CXTPSyntaxEditToolTipCtrl class.
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
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSYNTAXEDITTOOLTIPCTRL_H__)
#define __XTPSYNTAXEDITTOOLTIPCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Internal class

//===========================================================================
// Summary:
//      The CXTPSyntaxEditToolTipCtrl class encapsulates the functionality of
//      a "tip control," a small pop-up window that displays a
//      single line of text describing the hidden text when hovering an
//      item on the report area.
//
//      CXTPSyntaxEditToolTipCtrl provides the functionality to control the
//      the tip text, the size of the tip window itself, and the text
//      font of the tip.
//
//      This class is used by the report control internally.
//
// See also:
//      CXTPSyntaxEditCtrl
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditToolTipCtrl : public CWnd
{
// Construction
public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Constructs a CXTPSyntaxEditToolTipCtrl object.
	// Remarks:
	//      You construct a object object in two steps. First, call the constructor
	//      object and then call Create, which initializes the window and attaches
	//      it to the parent window.
	// Example:
	//      // Declare a local CXTPSyntaxEditToolTipCtrl object.
	//      CXTPSyntaxEditToolTipCtrl wndTip;
	//
	//      // Declare a dynamic CXTPSyntaxEditToolTipCtrl object.
	//      CXTPSyntaxEditToolTipCtrl* pTipWnd = new CXTPSyntaxEditToolTipCtrl;
	//
	// See also:
	//      Create, Activate
	//-----------------------------------------------------------------------
	CXTPSyntaxEditToolTipCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//      Destroys a CXTPSyntaxEditToolTipCtrl object, handles cleanup and de-allocation
	//-----------------------------------------------------------------------
	virtual ~CXTPSyntaxEditToolTipCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//      Creates smart edit tip window.
	// Parameters:
	//      pParentWnd : [in] Parent report control window.
	// Returns:
	//      TRUE if tip window created successfully, FALSE otherwise.
	// See also:
	//      CXTPSyntaxEditCtrl::ShowCollapsedToolTip
	//-----------------------------------------------------------------------
	virtual BOOL Create(CWnd *pParentWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets time to display the tip.
	// Parameters:
	//      nDelay : [in] Display delay value for ToolTip.
	// See also:
	//      int GetDelay();
	//-----------------------------------------------------------------------
	void SetDelay(int nDelay);

	//-----------------------------------------------------------------------
	// Summary:
	//      Returns the delay time.
	// Returns:
	//      Integer value of current delay.
	// See also:
	//      void SetDelay(int nDelay);
	//-----------------------------------------------------------------------
	int GetDelay();

	//-----------------------------------------------------------------------
	// Summary:
	//      Hides the ToolTip Window.
	//-----------------------------------------------------------------------
	void Hide();

	//-----------------------------------------------------------------------
	// Summary:
	//      Activates the tooltip window.
	// Parameters:
	//      strText : [in] Text to be shown in ToolTip window.
	//-----------------------------------------------------------------------
	void Activate(const CString& strText);

	//-----------------------------------------------------------------------
	// Summary:
	//      Sets coordinates of the tooltip hover rectangle.
	// Parameters:
	//      rc : [in] New tooltip hover rectangle in parent window coordinates.
	// Remarks:
	//      Should be called before activating tooltip window.
	// See also:
	//      GetHoverRect()
	//-----------------------------------------------------------------------
	void SetHoverRect(CRect rc);

	//{{AFX_VIRTUAL(CXTPSyntaxEditToolTipCtrl)
	//}}AFX_VIRTUAL

protected:
	int     m_nDelayTime;       // Delay before showing the tooltip in milliseconds.
	CWnd*   m_pParentWnd;       // Pointer to the parent report window.
	CRect   m_rcHover;          // Hover window coordinates.
	CString m_strToolTipText;   // ToolTip text to display.

protected:
	//{{AFX_MSG(CXTPSyntaxEditToolTipCtrl)
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ReCalcToolTipRect();
	BOOL RegisterWindowClass(HINSTANCE hInstance = NULL);
};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTPSyntaxEditToolTipCtrl::SetDelay(int nDelay) {
	m_nDelayTime = nDelay;
}
AFX_INLINE int CXTPSyntaxEditToolTipCtrl::GetDelay() {
	return m_nDelayTime;
}
AFX_INLINE void CXTPSyntaxEditToolTipCtrl::SetHoverRect(CRect rc) {
	m_rcHover = rc;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPSYNTAXEDITTOOLTIPCTRL_H__)
