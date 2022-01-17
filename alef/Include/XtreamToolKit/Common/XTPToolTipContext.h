// XTPToolTipContext.h: interface for the CXTPToolTipContext class.
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
#if !defined(__XTPTOOLTIPCONTEXT_H__)
#define __XTPTOOLTIPCONTEXT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CXTPImageManager;
class CXTPImageManagerIcon;

//{{AFX_CODEJOCK_PRIVATE
const UINT XTP_TTM_WINDOWFROMPOINT = (WM_USER + 9800);
const UINT XTP_TTM_SETIMAGE = (WM_USER + 9801);

#ifndef TTI_NONE
#define TTI_NONE                0
#define TTI_INFO                1
#define TTI_WARNING             2
#define TTI_ERROR               3
#endif

#define XTP_TTS_OFFICEFRAME         0x1000
#define XTP_TTS_NOSHADOW            0x2000


struct XTP_TOOLTIP_CONTEXT
{
	LPTSTR lpszDescription;
	LPTSTR lpszTitle;
	CXTPImageManager* pImageManager;
	RECT rcExclude;
	CCmdTarget* pObject;
};

struct XTP_TOOLTIP_TOOLINFO
{
	UINT cbSize;
	UINT uFlags;
	HWND hwnd;
	UINT_PTR uId;
	RECT rect;
	HINSTANCE hinst;
	LPTSTR lpszText;
};

struct XTP_TOOLTIP_TOOLINFO_EX : public XTP_TOOLTIP_TOOLINFO
{
	XTP_TOOLTIP_CONTEXT* pToolInfo;
};
//}}AFX_CODEJOCK_PRIVATE


//-----------------------------------------------------------------------
// Summary:
//     Enumeration used to determine styles for tooltips
// Example:
//     <code>pCommandBars->GetToolTipContext()->SetStyle(xtpToolTipBalloon);</code>
// See Also:
//     CXTPToolTipContext::SetStyle
//
// <KEYWORDS xtpToolTipBalloon, xtpToolTipBalloon, xtpToolTipOffice>
//-----------------------------------------------------------------------
enum XTPToolTipStyle
{
	xtpToolTipStandard,     // Standard tooltip window style
	xtpToolTipBalloon,      // Balloon tooltip window style
	xtpToolTipOffice,       // Office tooltip window style
	xtpToolTipRTF,          // RTF tooltip window style
	xtpToolTipLuna,         // Luna tooltip window style
	xtpToolTipOffice2007    // Office 2007 window style
};

class CXTPToolTipContext;

//-----------------------------------------------------------------------
// Summary:
//     CXTPToolTipContextToolTip is CWnd derived class, the base class for
//     custom tool tips.
//-----------------------------------------------------------------------
class CXTPToolTipContextToolTip : public CWnd
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPToolTipContextToolTip object
	//-----------------------------------------------------------------------
	CXTPToolTipContextToolTip(CXTPToolTipContext* pContext);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPToolTipContextToolTip object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPToolTipContextToolTip();


//{{AFX_CODEJOCK_PRIVATE
	struct TOOLITEM
	{
		CString strCaption;
		BOOL bAutoCaption;
		UINT uFlags;
		CRect rect;
		HINSTANCE hinst;
		HWND hwnd;
		UINT_PTR uId;
		CRect  rectExclude;

		BOOL IsEqual(TOOLITEM* pItem) {
			return (pItem && hwnd == pItem->hwnd && uId == pItem->uId) || (!pItem && !hwnd);
		}
		BOOL IsEmpty() {
			return hwnd == 0;
		}
		void Reset() {
			strCaption.Empty();
			uFlags = 0;
			hwnd = 0;
			uId = (UINT)-1;
			hinst = 0;
			rect.SetRectEmpty();
			rectExclude.SetRectEmpty();
		}
		void Assign(TOOLITEM* pItem) {
			if (pItem)
				*this = *pItem;
			else
				Reset();
		}
	};

public:
	BOOL Create(CWnd* pParentWnd, DWORD dwStyle);

public:
	BOOL AddTool(LPTOOLINFO lpToolInfo);
	void Activate(BOOL bActivate);
	void DelTool(LPTOOLINFO lpToolInfo);
	void RelayEvent(LPMSG lpMsg);
//}}AFX_CODEJOCK_PRIVATE
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to retrieve size of tool tip to show
	// Returns:
	//     Size of new tool tip.
	// See Also: DrawEntry
	//-----------------------------------------------------------------------
	virtual CSize GetToolSize(TOOLITEM* lpToolInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw face of tool tip.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     rc   - Client rectangle
	//-----------------------------------------------------------------------
	virtual void DrawEntry(CDC* pDC, TOOLITEM* lpToolInfo, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw background of tool tip.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     rc   - Client rectangle
	//-----------------------------------------------------------------------
	virtual void DrawBackground(CDC* pDC, TOOLITEM* lpToolInfo, CRect rc);

//{{AFX_CODEJOCK_PRIVATE
protected:
	BOOL IsVisible() const {
		return GetSafeHwnd() && IsWindowVisible();
	}
	TOOLITEM* FindTool();

	CString GetToolText(TOOLITEM* lpToolInfo);
	void SetVisibleTool(TOOLITEM* lpToolInfo);

	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	void HookMouseMove(BOOL bSetupHook);
	void EnsureVisible(CRect& rcToolTip);
	int GetCursorHeight() const;
	int GetMaxTipWidth() const;
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPToolTipContextToolTip)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPToolTipContextToolTip)
	afx_msg LRESULT OnAddTool(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnActivate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDelTool(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRelayEvent(WPARAM, LPARAM lParam);
	afx_msg LRESULT OnSetTitle(WPARAM, LPARAM lParam);
	afx_msg LRESULT OnUpdateTipText(WPARAM, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnWindowFromPoint(WPARAM, LPARAM);
	afx_msg LRESULT OnSetImage(WPARAM, LPARAM);
	afx_msg LRESULT OnSetDelayTime(WPARAM, LPARAM lParam);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE
private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

protected:
	CArray<TOOLITEM*, TOOLITEM*> m_arrTools;            // Tool tips item array
	BOOL m_bActive;                                     // TRUE if tracking is active
	CXTPToolTipContext* m_pContext;                     // Pointer to parent context class
	static HHOOK m_hHookMouse;                          // Mouse hook
	static CXTPToolTipContextToolTip* m_pWndMonitor;    // Mouse monitor pointer

	TOOLITEM m_toolVisible;                             // Currently visible tip
	TOOLITEM m_toolDisabled;                            // Currently ignored tip
	TOOLITEM m_toolDelay;                               // Currently delayed tip
	UINT_PTR m_nDelayTimer;                             // Hover timer
	DWORD m_dwLastTip;                                  // Last tool tip time.
	UINT m_nDelayInitial;                               // The length of time the pointer must remain stationary within a tool's bounding rectangle before the tool tip window appears.
	UINT m_nDelayReshow;                                // The length of time it takes for subsequent tool tip windows to appear as the pointer moves from one tool to another.
	CString m_strTitle;                                 // Title of tooltip if enabled.
	CXTPImageManagerIcon* m_pIcon;                      // Image
};



//===========================================================================
// Summary:
//     CXTPToolTipContext is a CCmdTarget derived class. It allows to change options for
//     tool tips of parent object.
//===========================================================================
class _XTP_EXT_CLASS CXTPToolTipContext : public CXTPCmdTarget
{
public:
	class CRichEditToolTip;
	class CStandardToolTip;
	class CLunaToolTip;
	class COffice2007ToolTip;

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPToolTipContext object.
	//-------------------------------------------------------------------------
	CXTPToolTipContext();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPToolTipContext object, handles cleanup and deallocation
	//-------------------------------------------------------------------------
	virtual ~CXTPToolTipContext();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this method to show title and description part of tooltip window.
	// Parameters:
	//     bShowTitleAndDescription - TRUE to show title and description part
	//     nIconTitle               - Icon to be set
	// Remarks:
	//     Allow only if Internet Explorer 5.0 installed.
	//-------------------------------------------------------------------------
	void ShowTitleAndDescription(BOOL bShowTitleAndDescription = TRUE, int nIconTitle = TTI_INFO);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if TitleAndDescription option was set
	// Returns:
	//     TRUE if description is visible for tooltips
	// See Also: ShowTitleAndDescription
	//-----------------------------------------------------------------------
	BOOL IsShowTitleAndDescription() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show image in tooltip
	// Parameters:
	//     bShowImage - TRUE to show image
	//     nImageBase - Image base
	// See Also: ShowTitleAndDescription
	//-----------------------------------------------------------------------
	void ShowImage(BOOL bShowImage, int nImageBase = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the style of the tooltip.
	// Parameters:
	//     toolStyle - The style to be set. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     toolStyle parameter can be one of the following:
	//         * <b>xtpToolTipStandard</b> Standard tooltip window style
	//         * <b>xtpToolTipBalloon</b> Balloon tooltip window style
	//         * <b>xtpToolTipOffice</b> Office tooltip window style
	// See Also: GetStyle, XTPToolTipStyle
	//-----------------------------------------------------------------------
	void SetStyle(XTPToolTipStyle toolStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the style of the tooltip.
	// See Also: SetStyle, XTPToolTipStyle
	//-----------------------------------------------------------------------
	XTPToolTipStyle GetStyle() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if Balloon tooltips supported by the system.
	//-------------------------------------------------------------------------
	BOOL IsBalloonStyleSupported() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the maximum width for a tooltip window.
	// Parameters:
	//     iWidth - Maximum tooltip window width to be set.
	//-----------------------------------------------------------------------
	void SetMaxTipWidth(int iWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the maximum width for a tooltip window.
	// Returns:
	//     Returns an INT value that represents the maximum tooltip width, in pixels
	//-----------------------------------------------------------------------
	int GetMaxTipWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the background color in a tooltip window.
	// Returns:
	//     Returns a COLORREF value that represents the background color.
	//-----------------------------------------------------------------------
	COLORREF GetTipBkColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the background color in a tooltip window.
	// Parameters:
	//     clr - New background color.
	//-----------------------------------------------------------------------
	void SetTipBkColor(COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the text color in a tooltip window.
	// Returns:
	//     Returns a COLORREF value that represents the text color.
	//-----------------------------------------------------------------------
	COLORREF GetTipTextColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the top, left, bottom, and right margins for a tooltip window
	// Parameters:
	//     lprc - Address of a RECT structure that contains the margin information to be set.
	//-----------------------------------------------------------------------
	void SetMargin(LPCRECT lprc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the top, left, bottom, and right margins set for a tooltip window
	//-----------------------------------------------------------------------
	CRect GetMargin() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the text color in a tooltip window.
	// Parameters:
	//     clr - New text color.
	//-----------------------------------------------------------------------
	void SetTipTextColor(COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Modifies the styles of the tool tip.
	// Parameters:
	//     dwRemove - Styles to remove.
	//     dwAdd - Styles to add.
	// Example:
	//     <code>m_wndToolBox.GetToolTipContext()->ModifyToolTipStyle(TTS_NOPREFIX, 0);</code>
	//-----------------------------------------------------------------------
	void ModifyToolTipStyle(DWORD dwRemove, DWORD dwAdd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves control style
	// See Also:
	//     ModifyToolTipStyle
	//-----------------------------------------------------------------------
	DWORD GetControlStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to set the delay time for a tool tip control. The delay time is the length of time the cursor must remain on a tool before the tool tip window appears. The default delay time is 500 milliseconds.
	// Parameters:
	//     dwDuration - Flag that specifies which duration value will be retrieved. See CToolTipCtrl::GetDelayTime for a description of the valid values.
	//     iTime      - The specified delay time, in milliseconds.
	//-----------------------------------------------------------------------
	void SetDelayTime(DWORD dwDuration, int iTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to remove a tooltip from the screen if a tooltip is currently displayed.
	//-----------------------------------------------------------------------
	void CancelToolTips();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom font for tooltips
	// Parameters:
	//     pFont - Font to be set
	//-----------------------------------------------------------------------
	void SetFont(CFont* pFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom font for tooltips
	// Parameters:
	//     lpLogFont - Font to be set
	//-----------------------------------------------------------------------
	void SetFontIndirect(LOGFONT* lpLogFont);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to create tooltip window. You can override it to create custom tool tip
	//     derived from CXTPToolTipContextToolTip
	// Parameters:
	//     pOwner - Specifies the tool tip control's parent window.
	// Returns:
	//     New tool tip window.
	//-----------------------------------------------------------------------
	virtual CWnd* CreateToolTip(CWnd* pOwner);


public:
//{{AFX_CODEJOCK_PRIVATE
	static void AFX_CDECL FillInToolInfo(TOOLINFO* pTI, HWND hWnd, CRect rect, INT_PTR nHit,
		const CString& strToolTip);
	static void AFX_CDECL FillInToolInfo(TOOLINFO* pTI, HWND hWnd, CRect rect, INT_PTR nHit,
		const CString& strToolTip, const CString& strTitle, const CString& strDescription, CXTPImageManager* pImageManager = 0);


	void FilterToolTipMessage(CWnd* pWndHost, MSG* pMsg);
	void FilterToolTipMessage(CWnd* pWndHost, UINT message, WPARAM wParam, LPARAM lParam);
	void FilterToolTipMessageHelper(CWnd* pWndHost, MSG* pMsg, BOOL bIgnoreFlags);

	void RelayToolTipMessage(CWnd* pToolTip, MSG* pMsg);

	void SetModuleToolTipContext();
	static void PASCAL FilterToolTipMessageStatic(MSG* pMsg, CWnd* pWnd);
//}}AFX_CODEJOCK_PRIVATE

private:
	friend class CXTPToolTipContextToolTip;
	friend class CRichEditToolTip;

protected:
	BOOL m_bShowTitleAndDescription;    // TRUE to show title and description for tooltip.
	BOOL m_bShowImage;                  // TRUE to show images in tooltip
	int m_nImageBase;                   // This value will be added to tool identifier to retrieve image for tip.
	XTPToolTipStyle m_toolStyle;        // Tooltip style
	DWORD m_dwComCtlVersion;            // CommCtrl.dll version
	int m_nIconTitle;                   // Icon title for extended tooltip
	int m_nMaxTipWidth;                 // Maximum width of tooltip window
	DWORD m_dwStyle;                    // Tooltip style
	CRect m_rcMargin;                   // Margin or "padding" placed around the tooltip text.  The sides of the CRect indicate the amount of padding for the left, top, bottom, and right sides of the tooltip window.

	int m_nDelayInitial;                // The length of time the pointer must remain stationary within a tool's bounding rectangle before the tool tip window appears.
	int m_nDelayReshow;                 // The length of time it takes for subsequent tool tip windows to appear as the pointer moves from one tool to another.
	int m_nDelayAutoPop;                // the length of time a tooltip window remains visible if the pointer is stationary within a tool's bounding rectangle.

	CWnd* m_pToolTip;                   // Owned Tooltip  window
	XTP_TOOLTIP_TOOLINFO_EX m_lastInfo; // Last tooltip info.
	static CXTPToolTipContext* m_pModuleContext;  // Tooltip for current module

	COLORREF m_clrTipBkColor;           // Background color in a tooltip window
	COLORREF m_clrTipTextColor;         // Text color in a tooltip window.

	CFont m_fnt;                        // Font of the tool tip
	CFont m_fntTitle;                   // Font of the tool tip

};


AFX_INLINE CRect CXTPToolTipContext::GetMargin() const {
	return m_rcMargin;
}
AFX_INLINE BOOL CXTPToolTipContextToolTip::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
AFX_INLINE DWORD CXTPToolTipContext::GetControlStyle() const {
	return m_dwStyle;
}
AFX_INLINE BOOL CXTPToolTipContext::IsShowTitleAndDescription() const {
	return m_bShowTitleAndDescription;
}

#endif // !defined(__XTPTOOLTIPCONTEXT_H__)
