// XTPTaskDialogControls.h
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#if !defined(__XTPTASKLINKCTRL_H__)
#define __XTPTASKLINKCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTButtonTheme.h"


//===========================================================================
// Summary:
//     CXTPTaskDialogLinkCtrl is a CStatic derived class that is used to
//     display a text block that contains clickable links.
//===========================================================================
class _XTP_EXT_CLASS CXTPTaskDialogLinkCtrl : public CStatic
{
public:
	//===========================================================================
	// Summary:
	//     LINKITEM is Used to set and retrieve information about a
	//     link item.
	// See Also:
	//     CXTPTaskDialogLinkCtrl::HitTest
	//===========================================================================
	struct LINKITEM
	{
		DWORD       nStart;     // Starting character position,
		int         nIndex;     // Character index of the character nearest the specified point.
		CString     strLabel;   // Specifies the execute string for the link.
		CString     strUrl;     // Specifies the display string for the link.

		CArray<RECT, RECT&> arrParts;
	};

	typedef CArray<LINKITEM*, LINKITEM*> CTaskLinkItemArray;

	DECLARE_DYNCREATE(CXTPTaskDialogLinkCtrl)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//       Constructs a CXTPTaskDialogLinkCtrl object.
	//-----------------------------------------------------------------------
	CXTPTaskDialogLinkCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPTaskDialogLinkCtrl object, handles cleanup and
	//     deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPTaskDialogLinkCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to create a CXTPTaskDialogLinkCtrl
	//     text control. The control is used by CXTPTaskDialog to display a
	//     static text block that contains clickable link text.
	// Parameters:
	//     rect       - Specifies the edit contro's size and position. Can be
	//                  a CRect object or RECT structure.
	//     strBuffer  - NULL terminated string that contains link text
	//                  to be displayed.
	//     pFont      - Specifies the font to be used with the text control.
	//     pParentWnd - Specifies the edit contro's parent window (usually a
	//                  CXTPTaskDialog). It must not be NULL.
	// Remarks:
	//       When specified strBuffer can contain links in the form:
	// <code>
	// <A HREF="executablestring">Hyperlink Text</A>.
	// </code>
	// Returns:
	//     Nonzero if initialization is successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL Create(const CRect& rect, CString& strBuffer, CFont* pFont, CWnd* pParentWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the focused link.
	// Returns:
	//     The pointer of the currently focused link, otherwise NULL.
	//-----------------------------------------------------------------------
	LINKITEM* GetFocusedLink() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the focus to specified link
	// Parameters:
	//     iFocusChar - The index of the focused item or -1 to indicate
	//                  no focus.
	//-----------------------------------------------------------------------
	void FocusItem(int iFocusItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the link information from
	//     the specified cursor position.
	// Parameters:
	//     pt - Client coordinates.
	// Returns:
	//     Returns Pointer on success, NULL on failure.
	//-----------------------------------------------------------------------
	LINKITEM* HitTest(CPoint pt) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to return the character index of
	//     the first link located after the index specified by iPos.
	// Parameters:
	//     strBuffer - NULL terminated string to search for link text.
	//     iPos      - Starting position to begin search from.
	// Returns:
	//     The LOWORD value contains the starting position of the first
	//     link text block found, the HIWORD value contains the number
	//     of characters in the link text.
	//-----------------------------------------------------------------------
	DWORD GetLinkPos(CString strBuffer, int iPos = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to extract link information from
	//     the specified string buffer and store it internally. The string is
	//     then reformatted to remove the link tag information.
	// Parameters:
	//     strBuffer - NULL terminated string to search for link text.
	// Returns:
	//     TRUE if link text was located, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL ExtractLinks(CString& strBuffer);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the background color for the link
	//     control.
	// Parameters:
	//     crBack - RGB value representing the link control's background color.
	//-----------------------------------------------------------------------
	void SetBackColor(COLORREF crBack);
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the link control's background
	//     color.
	// Returns:
	//     RGB value representing the link control's background color.
	//-----------------------------------------------------------------------
	COLORREF GetBackColor() const;
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the text color for the link
	//     control.
	// Parameters:
	//     crText - RGB value representing the link control's text color.
	//-----------------------------------------------------------------------
	void SetTextColor(COLORREF crText);
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the link control's text
	//     color.
	// Returns:
	//     RGB value representing the link control's text color.
	//-----------------------------------------------------------------------
	COLORREF GetTextColor() const;
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the text link color for the link
	//     control.
	// Parameters:
	//     crTextLink - RGB value representing the link control's text link color.
	//-----------------------------------------------------------------------
	void SetTextLinkColor(COLORREF crTextLink);
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the link control's text link
	//     color.
	// Returns:
	//     RGB value representing the link control's text link color.
	//-----------------------------------------------------------------------
	COLORREF GetTextLinkColor() const;

private:
	void DrawText(CDC* pDC, CRect rClient);
	void DrawTextPart(CDC* pDC, int&x, int&y, int nWidth, CString strBuffer, LINKITEM* pItem);
	int DrawTextPartText(CDC* pDC, CString strBuffer, int x, int y, LINKITEM* pItem);
	void RemoveAllLinks();



protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPTaskDialogLinkCtrl)
	BOOL PreCreateWindow(CREATESTRUCT& cs);
	void PreSubclassWindow();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPTaskDialogLinkCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint /*point*/);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnUpdateUIState(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	int                         m_nFocused;         // Index of the currently focus link.
	CFont                       m_fontUL;           // Font used to render link text.
	CString                     m_strBuffer;        // Text buffer to be displayed in the control.
	HCURSOR                     m_hcurHand;         // Handle to text link cursor.
	COLORREF                    m_crBack;           // RGB background color value.
	COLORREF                    m_crText;           // RGB text color value.
	COLORREF                    m_crTextLink;       // RGB link text color value.
	CTaskLinkItemArray          m_arrLinks;         // Array of links for the display text.
	BOOL m_bPreSubclassWindow;
};


//===========================================================================
// CXTPTaskButtonTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPTaskButtonTheme : public CXTButtonTheme
{
public:
	CXTPTaskButtonTheme(BOOL bThemeReady);
	CXTPTaskButtonTheme();
	virtual void RefreshMetrics();
	void SetTitleFont(CFont* FontTitle);
	CFont* GetTitleFont();

protected:
	virtual BOOL DrawButtonThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton);
	virtual BOOL DrawWinThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton);
	virtual void DrawButtonText(CDC* pDC, UINT nState, CRect& rcItem, CXTButton* pButton);
	virtual COLORREF GetTextColor(UINT nState, CXTButton* pButton);
	virtual void DrawFocusRect(CDC* pDC, UINT nState, CRect& rcItem, CXTButton* pButton);
	virtual BOOL CanHilite(CXTButton* pButton);
	virtual CPoint CalculateImagePosition(CDC* pDC, UINT nState, CRect& rcItem, bool bHasPushedImage, CXTButton* pButton);
	virtual void GetButtonText(CXTButton* pButton);
	virtual CPoint GetTextPosition(UINT nState, CRect& rcItem, CSize& sizeText, CXTButton* pButton);
	virtual void DrawButtonIcon(CDC* pDC, UINT nState, CRect& rcItem, CXTButton* pButton);

	CFont* m_pFontTitle;
	CPoint m_ptMargin;
	CString m_strButton;
	CString m_strCaptText;
	CString m_strNoteText;
	CBitmap m_bmpBackClassic;
	CBitmap m_bmpBackThemed;
	CXTPPaintManagerColor m_crTextHilite;
	CXTPPaintManagerColor m_crBackSelected;
	CXTPPaintManagerColor m_crBorderActive;
	BOOL m_bThemeReady;
};

//===========================================================================
// CXTPTaskButtonThemeExpando
//===========================================================================
class _XTP_EXT_CLASS CXTPTaskButtonThemeExpando : public CXTButtonTheme
{
public:
	CXTPTaskButtonThemeExpando();

	void DrawItem(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton);
};

//===========================================================================
// CXTPTaskDialogProgressCtrl
//===========================================================================
class _XTP_EXT_CLASS CXTPTaskDialogProgressCtrl : public CProgressCtrl
{
public:
	CXTPTaskDialogProgressCtrl();


protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPTaskDialogProgressCtrl)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPTaskDialogProgressCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG


protected:
	int m_nState;
};


AFX_INLINE void CXTPTaskDialogLinkCtrl::SetBackColor(COLORREF crBack) {
	m_crBack = crBack;
}
AFX_INLINE COLORREF CXTPTaskDialogLinkCtrl::GetBackColor() const {
	return m_crBack;
}
AFX_INLINE void CXTPTaskDialogLinkCtrl::SetTextColor(COLORREF crText) {
	m_crText = crText;
}
AFX_INLINE COLORREF CXTPTaskDialogLinkCtrl::GetTextColor() const {
	return m_crText;
}
AFX_INLINE void CXTPTaskDialogLinkCtrl::SetTextLinkColor(COLORREF crTextLink) {
	m_crTextLink = crTextLink;
}
AFX_INLINE COLORREF CXTPTaskDialogLinkCtrl::GetTextLinkColor() const {
	return m_crTextLink;
}



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPTASKLINKCTRL_H__)
