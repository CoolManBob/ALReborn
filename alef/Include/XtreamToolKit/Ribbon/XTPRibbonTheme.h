// XTPRibbonTheme.h: interface for the CXTPRibbonTheme class.
//
// This file is a part of the XTREME RIBBON MFC class library.
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
#if !defined(__XTPRIBBONTHEME_H__)
#define __XTPRIBBONTHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPRibbonGroup;
class CXTPRibbonBar;
class CXTPRibbonTab;
class CXTPOffice2007Image;
class CXTPOffice2007Images;
class CXTPTabPaintManager;
class CXTPRibbonTabContextHeaders;
class CXTPPopupToolBar;

#include "CommandBars/XTPPaintManager.h"
#include "CommandBars/XTPOffice2007Theme.h"

//===========================================================================
// Summary: Standalone class used by the CXTPRibbonBar as a paint manager.
//===========================================================================
class _XTP_EXT_CLASS CXTPRibbonTheme : public CXTPOffice2007Theme
{
private:
	class CRibbonAppearanceSet;
	class CRibbonColorSet;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPRibbonTheme object
	//-----------------------------------------------------------------------
	CXTPRibbonTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPRibbonTheme object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPRibbonTheme();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill background of a CXTPRibbonGroup.
	// Parameters:
	//               pDC - Pointer to a valid device context
	//               pActiveTab - Pointer to the tab that is currently active
	//               rcGroups - Bounding rectangle of the group
	// Remarks:
	//      Performs a gradient fill using the GROUP_COLOR::clrBackground for
	//      the main group background and GROUP_COLOR::clrCaption to fill the
	//      group caption rectangle.
	// See Also: CXTPRibbonGroup::GetRect, GROUP_COLOR
	//-----------------------------------------------------------------------
	virtual void FillGroupRect(CDC* pDC, CXTPRibbonTab* pActiveTab, CRect rcGroups);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill background of Ribbon Bar control.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pRibbonBar - Points to a CXTPRibbonBar object
	//-----------------------------------------------------------------------
	virtual void FillRibbonBar(CDC* pDC, CXTPRibbonBar* pRibbonBar);

	//-------------------------------------------------------------------------
	// Summary:
	//      This method is called to refresh the visual metrics of manager.
	// Remarks:
	//      Refreshes all of the colors in the Ribbon Bar.  Many of the colors
	//      used are set with the GROUP_COLOR structure.
	// See Also:
	//     GROUP_COLOR
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the specified CXTPRibbonGroup.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pGroup - Pointer to the CXTPRibbonGroup to be drawn
	// Remarks:
	//     Draws the entire groups including background, caption and text.
	//     This will not draw the option button or group popup.
	// See Also: GROUP_COLOR, FillRibbonBar, FillGroupRect, RefreshMetrics
	//-----------------------------------------------------------------------
	virtual void DrawGroup(CDC* pDC, CXTPRibbonGroup* pGroup);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the colors used to draw the specified CXTPRibbonGroup.
	// Parameters:
	//     pGroup - Pointer to the CXTPRibbonGroup to retrieve the colors from
	// Returns:
	//     A GROUP_COLOR object containing the colors used to draw a CXTPRibbonGroup
	// See Also: CXTPRibbonTheme::GROUP_COLOR, CXTPRibbonTheme::DrawGroup
	//-----------------------------------------------------------------------
	//virtual GROUP_COLOR* GetGroupColor(CXTPRibbonGroup* pGroup);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the height of the group caption bar.
	// Returns:
	//     Integer containing the height of the group caption bar.
	// See Also: CXTPRibbonGroup::RepositionControls, CXTPRibbonGroup::SetRect
	//-----------------------------------------------------------------------
	int GetGroupCaptionHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the group popup button.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pControlGroupPopup - Pointer to a CXTPControlPopup object.
	//     bDraw - TRUE to draw group popup button; FALSE to retrieve the size of the control.
	// Remarks:
	//     A single group button will be displayed if there is not enough
	//     room to display the items in the group.  A group popup is displayed
	//     when the button is clicked.  The popup contains all the items in
	//     the group.  When a group button is clicked, a CXTPControlPopup
	//     is displayed that contains the items of the group.
	// Returns:
	//     If bDraw is TRUE, then the size of the group popup button is returned.
	// See Also: CXTPRibbonGroup::GetControlGroupPopup
	//-----------------------------------------------------------------------
	virtual CSize DrawControlGroupPopup(CDC* pDC, CXTPControlPopup* pControlGroupPopup, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the group option button.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pControlGroupOption - Pointer to a CXTPControl object.
	//     bDraw - TRUE to draw option button; FALSE to retrieve the size of the control.
	// Remarks:
	//     The option button is typically used to launch a dialog associated with the group.
	//     It is drawn on the right side of the group caption.
	// Returns:
	//     If bDraw is TRUE, then the size of the option button control is returned.
	// See Also: CXTPRibbonGroup::ShowOptionButton, CXTPRibbonGroup::GetControlGroupOption
	//-----------------------------------------------------------------------
	virtual CSize DrawControlGroupOption(CDC* pDC, CXTPControl* pControlGroupOption, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw single tab of ribbon bar
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pItem - Ribbon tab to draw
	//-----------------------------------------------------------------------
	virtual void DrawRibbonTab(CDC* pDC, CXTPRibbonTab* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw popup toolbar of reduced group
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pGroup - Parent group of popuped toolbar
	//     pCommandBar - popup toolbar to draw
	//-----------------------------------------------------------------------
	virtual void FillGroupPopupToolBar(CDC* pDC, CXTPRibbonGroup* pGroup, CXTPCommandBar* pCommandBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw background of minimzed popup bar
	// Parameters:
	//     pDC - Pointer to device context
	//     pPopupToolBar - Pointer to popup toolbar.
	//-----------------------------------------------------------------------
	void FillTabPopupToolBar(CDC* pDC, CXTPPopupToolBar* pPopupToolBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to Draw Ribbon caption
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pRibbonBar - Ribbon Bar to draw
	//     bActive - TRUE if ribbon frame is active
	//-----------------------------------------------------------------------
	virtual void DrawRibbonFrameCaption(CDC* pDC, CXTPRibbonBar* pRibbonBar, BOOL bActive);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw ribbon caption if EnableFrameTheme was not called
	// Parameters:
	//     pDC -  Pointer to a valid device context
	//     pRibbonBar - Ribbon Bar to draw
	//-----------------------------------------------------------------------
	virtual void DrawRibbonFrameCaptionBar(CDC* pDC, CXTPRibbonBar* pRibbonBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws top-left system button
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pControl - System button control
	//-----------------------------------------------------------------------
	virtual void DrawRibbonFrameSystemButton(CDC* pDC, CXTPControlPopup* pControl, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw ribbon context headers
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pContextHeaders - Collection of context header
	//-----------------------------------------------------------------------
	virtual void DrawRibbonFrameContextHeaders(CDC* pDC, CXTPRibbonBar* pRibbonBar, CXTPRibbonTabContextHeaders* pContextHeaders);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw quick access customize button
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pControl - Pointer to Quick Access customize button
	// See Also: DrawRibbonQuickAccessMoreButton
	//-----------------------------------------------------------------------
	virtual void DrawRibbonQuickAccessButton(CDC* pDC, CXTPControlPopup* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw quick access more button
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pControl - Pointer to Quick Access more button
	// See Also: DrawRibbonQuickAccessButton
	//-----------------------------------------------------------------------
	virtual void DrawRibbonQuickAccessMoreButton(CDC* pDC, CXTPControlPopup* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw scroll groups buttons
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pControl - Scroll button to draw
	//     bScrollLeft - TRUE to draw left button; FALSE - right
	//-----------------------------------------------------------------------
	virtual void DrawRibbonScrollButton(CDC* pDC, CXTPControl* pControl, BOOL bScrollLeft);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves edit control height
	//-----------------------------------------------------------------------
	int GetEditHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set font height used for Ribbon
	// Parameters:
	//     nFontHeight - New font height
	// See Also: GetFontHeight
	//-----------------------------------------------------------------------
	void SetFontHeight(int nFontHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get font height used for Ribbon
	// Returns:
	//     Font height used for Ribbon
	// See Also: SetFontHeight
	//-----------------------------------------------------------------------
	int GetFontHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw popup bar of more button
	// Parameters:
	//      pDC - Pointer to a valid device context
	//      pBar - ToolBar to fill
	//-----------------------------------------------------------------------
	virtual void FillMorePopupToolBarEntry(CDC* pDC, CXTPCommandBar* pBar);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get a pointer to the tab paint manager.
	//     The tab paint manager is used to customize the appearance of
	//     CXTPTabManagerItem objects and the tab manager.  I.e. Tab colors,
	//     styles, etc...  This member must be overridden in
	//     derived classes.
	// Returns:
	//     Pointer to CXTPTabPaintManager that contains the visual elements
	//     of the tabs.
	//-----------------------------------------------------------------------
	CXTPTabPaintManager* GetTabPaintManager() const;

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a CXTPTabPaintManager object used to
	//     store the color and appearance information of the tabs in the
	//     Ribbon Bar.
	//-----------------------------------------------------------------------
	virtual CXTPTabPaintManager* CreateTabPaintManager();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill the control's face
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     pButton - Points to a CXTPControl object to draw.
	//-----------------------------------------------------------------------
	virtual void DrawControlEntry(CDC* pDC, CXTPControl* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to determine offset of popuped bar.
	// Parameters:
	//     rc        - Control's bounding rectangle.
	//     pControl  - Points to a CXTPControl object
	//     bVertical - TRUE if control docked vertically.
	//-----------------------------------------------------------------------
	virtual void AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL bVertical);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the frame for the SplitButton
	//     control
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pButton - Points to a CXTPControl object
	//     rc      - Bounding rectangle to draw
	//-----------------------------------------------------------------------
	virtual void DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw popup bar right gripper.
	// Parameters:
	//     pDC       - Points to a valid device context.
	//     x         - Specifies the logical x-coordinate of the upper-left corner of the rectangle.
	//     y         - Specifies the logical y-coordinate of the upper-left corner of the destination rectangle.
	//     cx        - Specifies the width of the rectangle.
	//     cy        - Specifies the height of the rectangle.
	//     bExpanded - TRUE if expanded.gripper.
	//-----------------------------------------------------------------------
	virtual void DrawPopupBarGripper(CDC* pDC, int x, int y, int cx, int cy, BOOL bExpanded = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill the command bar's face.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     pBar - Points to a CXTPCommandBar object to draw.
	//-----------------------------------------------------------------------
	virtual void FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill the control's face
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     rc          - Rectangle to draw.
	//     bSelected   - TRUE if the control is selected.
	//     bPressed    - TRUE if the control is pushed.
	//     bEnabled    - TRUE if the control is enabled.
	//     bChecked    - TRUE if the control is checked.
	//     bPopuped    - TRUE if the control is popuped.
	//     barType     - Parent's bar type
	//     barPosition - Parent's bar position.
	//-----------------------------------------------------------------------
	virtual void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw a command bar's separator.
	// Parameters:
	//     pDC      - Pointer to a valid device context
	//     pBar     - Points to a CXTPCommandBar object
	//     pControl - Points to a CXTPControl object
	//     bDraw    - TRUE to draw; FALSE to retrieve the size of the separator.
	// Returns:
	//     This method is called to draw a command bar's separator.
	//-----------------------------------------------------------------------
	virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill a status bar.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     pBar - Points to a CXTPStatusBar object
	//-----------------------------------------------------------------------
	virtual void FillStatusBar(CDC* pDC, CXTPStatusBar* pBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the status bar's gripper.
	// Parameters:
	//     pDC      - Pointer to a valid device context
	//     rcClient - Client rectangle of the status bar.
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarGripper(CDC* pDC, CRect rcClient);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw frame of single cell of status bar.
	// Parameters:
	//     pDC - Points to a valid device context.
	//     rc - CRect object specifying size of area.
	//     bGripperPane - TRUE if pane is last cell of status bar
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarPaneBorder(CDC* pDC, CRect rc, BOOL bGripperPane);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves buttons text color
	// Parameters:
	//     bSelected   - TRUE if the control is selected.
	//     bPressed    - TRUE if the control is pushed.
	//     bEnabled    - TRUE if the control is enabled.
	//     bChecked    - TRUE if the control is checked.
	//     bPopuped    - TRUE if the control is popuped.
	//     barType     - Parent's bar type
	//     barPosition - Parent's bar position.
	//-----------------------------------------------------------------------
	virtual COLORREF GetRectangleTextColor(BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method returns control text color to draw
	// Parameters:
	//     pButton - Points to a CXTPControl object to draw.
	// Returns: Button text color
	//-----------------------------------------------------------------------
	virtual COLORREF GetControlTextColor(CXTPControl* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control.
	// Parameters:
	//     pDC   - Pointer to a valid device context
	//     bDraw - TRUE to draw; FALSE to retrieve the size of the control.
	//     pEdit - Edit control to draw.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	CSize DrawControlEdit(CDC* pDC, CXTPControlEdit* pControlEdit, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control.
	// Parameters:
	//     pDC       - Pointer to a valid device context
	//     pComboBox - ComboBox to draw.
	//     bDraw     - TRUE to draw; FALSE to retrieve the size of the control.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	CSize DrawControlComboBox(CDC* pDC, CXTPControlComboBox* pControlCombo, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method returns the edit control back color to draw.
	// Parameters:
	//     pControl - Points to a CXTPControl object to get the back color from.
	// Returns: Edit control back color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetControlEditBackColor(CXTPControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the base theme of the manager.
	// Returns:
	//     Returns the base theme.
	//-----------------------------------------------------------------------
	XTPPaintTheme BaseTheme() { return xtpThemeRibbon; }

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set bounding region for popup bars
	// Parameters:
	//     pCommandBar - Pointer to popup bar which region need to set
	//-----------------------------------------------------------------------
	virtual void SetCommandBarRegion(CXTPCommandBar* pCommandBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw menu split button dropdown rect
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pButton - Split button to draw
	//-----------------------------------------------------------------------
	virtual void DrawSplitButtonPopup(CDC* pDC, CXTPControl* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw menu popup Glyph
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pButton - Popup button to draw Glyph
	//-----------------------------------------------------------------------
	virtual void DrawControlPopupGlyph(CDC* pDC, CXTPControl* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw menu label background
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rc - Bounding rectangle of button
	//-----------------------------------------------------------------------
	virtual void FillPopupLabelEntry(CDC* pDC, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw caption text for Aero transparent caption
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rcCaptionText - Caption text rectangle
	//     strText - Caption text
	//     pSite - Frame to draw
	//     bActive - TRUE if frame is active
	//-----------------------------------------------------------------------
	void DrawDwmCaptionText(CDC* pDC, CRect rcCaptionText, CString strText, CWnd* pSite, BOOL bActive);

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh fonts
	//-------------------------------------------------------------------------
	void UpdateFonts();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set rounded rectangle region for window
	// Parameters:
	//     pWnd - Window to set region to
	//-----------------------------------------------------------------------
	void SetRoundRectRegion(CWnd* pWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw ribbon keyboard tip
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pWnd - Keyboard tip window
	//     bSetRegion - TRUE to set region
	//-----------------------------------------------------------------------
	virtual void DrawKeyboardTip(CDC* pDC, CXTPCommandBarKeyboardTip* pWnd, BOOL bSetRegion);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the specified controls.
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     controlType - Special control enumerator.
	//     pButton     - Points to a CXTPControl object to draw.
	//     pBar        - Parent CXTPCommandBar object.
	//     bDraw       - TRUE to draw; FALSE to retrieve the size of the
	//                   control.
	//     lpParam     - Specified parameter.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	virtual CSize DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if images is available
	// Returns:
	//     TRUE if images exist in application
	//-----------------------------------------------------------------------
	BOOL IsImagesAvailable();

//{{AFX_CODEJOCK_PRIVATE
	virtual void DrawDropDownGlyph(CDC* pDC, CXTPControl* pControl, CPoint pt, BOOL bSelected, BOOL bPopuped, BOOL bEnabled, BOOL bVert);
	virtual CSize DrawControlCheckBoxMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled);
	virtual CSize DrawControlRadioButtonMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled);
//}}AFX_CODEJOCK_PRIVATE


public:
	BOOL m_bHotTrackingGroups;              // TRUE to highlight group under cursor
	int m_nTabsHeight;                      // Tabs heights
	CXTPPaintManagerFont m_fontGroupCaption;   // Font used to draw text in the group caption


protected:
	int m_nGroupCaptionHeight;              // Group caption bar height
	int m_nFontHeight;                      // Ribbon font height

	CXTPTabPaintManager* m_pTabPaintManager;// Tab paint manager

protected:
	COLORREF m_clrRibbonFace;               // Ribbon face color
	COLORREF m_clrRibbonText;               // Ribbon text color
	COLORREF m_clrRibbonGroupCaptionText;   // Ribbon group captions color
	COLORREF m_clrControlEditNormal;        // Edit control color

	COLORREF m_clrMinimizedFrameEdgeHighLight;      // Minimzed bottom edge
	COLORREF m_clrMinimizedFrameEdgeShadow;         // Minimzed bottom edge shadow


protected:

	friend class CXTPRibbonBar;
};


AFX_INLINE int CXTPRibbonTheme::GetGroupCaptionHeight() const {
	return m_nGroupCaptionHeight;
}
AFX_INLINE CXTPTabPaintManager* CXTPRibbonTheme::GetTabPaintManager() const {
	return m_pTabPaintManager;
}
AFX_INLINE int CXTPRibbonTheme::GetEditHeight() const {
	return m_nEditHeight;
}
AFX_INLINE void CXTPRibbonTheme::SetFontHeight(int nFontHeight) {
	m_nFontHeight = nFontHeight;
	RefreshMetrics();
}
AFX_INLINE int CXTPRibbonTheme::GetFontHeight() const {
	return m_nFontHeight;
}

#endif // !defined(__XTPRIBBONTHEME_H__)
