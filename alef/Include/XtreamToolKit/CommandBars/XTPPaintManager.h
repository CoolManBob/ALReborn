// XTPPaintManager.h : interface for the CXTPPaintManager class.
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
#if !defined(__XTPPAINTMANAGER_H__)
#define __XTPPAINTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPCommandBarsDefines.h"
#include "Common/XTPWinThemeWrapper.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPDrawHelpers.h"

class CXTPControlButton;
class CXTPControlPopup;
class CXTPControl;
class CXTPToolBar;
class CXTPCommandBar;
class CXTPCommandBars;
class CXTPPopupBar;
class CXTPControlComboBox;
class CXTPTabControl;
class CXTPDockBar;
class CXTPControlEdit;
class CXTPImageManagerIcon;
class CXTPStatusBar;
class CXTPStatusBarPane;
class CXTPDialogBar;
class CXTPCommandBarKeyboardTip;

class CXTPBufferDC;
class CXTPControlGalleryPaintManager;

//-----------------------------------------------------------------------
// Summary:
//     Special control with extended drawing.
// See Also: CXTPPaintManager::DrawSpecialControl
//
// <KEYWORDS xtpButtonExpandToolbar, xtpButtonExpandMenu, xtpButtonHideFloating, xtpButtonExpandFloating, xtpButtonCustomize>
//-----------------------------------------------------------------------
enum XTPSpecialControl
{
	xtpButtonExpandToolbar, // Control is option button of docking toolbar.
	xtpButtonExpandMenu,    // Control is expand button of popup bar.
	xtpButtonHideFloating,  // Control is hide button of floating toolbar.
	xtpButtonExpandFloating // Control is option button of floating toolbar.
};


//-------------------------------------------------------------------------
// Summary:
//     Shadow options of selected paint manager
// See Also: CXTPPaintManager::GetShadowOptions()
//-------------------------------------------------------------------------
enum XTPShadowOptions
{
	xtpShadowOfficeAlpha = 1,           // Office alpha shadow
	xtpShadowShowPopupControl = 2       // Draw shadow for popup controls
};

//------------------------------------------------------------------------
// Summary:
//     This structure collect icons options for command bars.
// Example:
//     <code>XTPPaintManager()->GetIconsInfo()->bIconsWithShadow = FALSE;</code>
// See Also: CXTPPaintManager::GetIconsInfo
//------------------------------------------------------------------------
struct XTP_COMMANDBARS_ICONSINFO
{
	BOOL bUseFadedIcons;    // If TRUE, Icons will appear faded.
	BOOL bUseDisabledIcons; // If TRUE, disabled icons will be used when the control is disabled.
	BOOL bIconsWithShadow;  // If TRUE, when the mouse pointer is moved over a command bar control, the icons will appear to "jump" away from the screen casting a shadow.
	BOOL bOfficeStyleDisabledIcons;  // If TRUE, Office 2003 style disabled icons will be drawn when using disabled icons.
};

//===========================================================================
// Summary:
//     PaintManagers used CFont wrapper.
//===========================================================================
class _XTP_EXT_CLASS CXTPPaintManagerFont : public CFont
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPaintManagerFont object.
	//-----------------------------------------------------------------------
	CXTPPaintManagerFont()
	{
		m_bStandardFont = TRUE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets application defined font
	// Parameters:
	//     lpLogFont - Application defined font
	//-----------------------------------------------------------------------
	BOOL SetCustomFont(const LOGFONT* lpLogFont)
	{
		m_bStandardFont = (lpLogFont == NULL);
		if (lpLogFont)
		{
			DeleteObject();
			return CreateFontIndirect(lpLogFont);
		}
		return TRUE;
	}


	//-----------------------------------------------------------------------
	// Summary:
	//      Call this method to set custom font
	// Parameters:
	//      bCustomFont - TRUE if custom font used
	// See Also: SetStandardFont
	//-----------------------------------------------------------------------
	void SetCustomFont(BOOL bCustomFont)
	{
		m_bStandardFont = !bCustomFont;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets paint manager's font
	// Parameters:
	//     lpLogFont - PaintManager's defined font
	//-----------------------------------------------------------------------
	BOOL SetStandardFont(const LOGFONT* lpLogFont)
	{
		if (lpLogFont && (m_bStandardFont || !GetSafeHandle()))
		{
			DeleteObject();
			return CreateFontIndirect(lpLogFont);
		}
		return TRUE;
	}

protected:
	BOOL m_bStandardFont; // TRUE to use standard font;
};

//===========================================================================
// Summary:
//     Paint manager of the command bars.
//===========================================================================
class _XTP_EXT_CLASS CXTPPaintManager : public CXTPCmdTarget
{
private:
	friend class CXTPPaintManager* XTPPaintManager();
	class CPaintManagerDestructor;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control.
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pButton - Points to a CXTPControl object
	//     bDraw   - TRUE to draw; FALSE to retrieve the size of the control.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	CSize DrawControl(CDC* pDC, CXTPControl* pButton, BOOL bDraw = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control placed on the PopupBar.
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pButton - Points to a CXTPControl object
	//     bDraw   - TRUE to draw; FALSE to retrieve the size of the control.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	virtual CSize DrawControlPopupParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control placed on the ToolBar.
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pButton - Points to a CXTPControl object
	//     bDraw   - TRUE to draw; FALSE to retrieve the size of the control.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	virtual CSize DrawControlToolBarParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the frame for the SplitButton
	//     control
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pButton - Points to a CXTPControl object
	//     rc      - Bounding rectangle to draw
	//-----------------------------------------------------------------------
	virtual void DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rc) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control.
	// Parameters:
	//     pDC    - Pointer to a valid device context
	//     pCombo - ComboBox to draw.
	//     bDraw  - TRUE to draw; FALSE to retrieve the size of the control.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	virtual CSize DrawControlComboBox(CDC* pDC, CXTPControlComboBox* pCombo, BOOL bDraw);

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
	//     This method is called to draw the control.
	// Parameters:
	//     pDC   - Pointer to a valid device context
	//     bDraw - TRUE to draw; FALSE to retrieve the size of the control.
	//     pEdit - Edit control to draw.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	virtual CSize DrawControlEdit(CDC* pDC, CXTPControlEdit* pEdit, BOOL bDraw);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control placed on the ListBox.
	// Parameters:
	//     pDC       - Pointer to a valid device context
	//     pButton   - Points to a CXTPControl object
	//     rc        - Bounding rectangle to draw.
	//     bSelected - TRUE if the control is selected.
	//     bDraw     - TRUE to draw; FALSE to retrieve the size of the control.
	//     pCommandBars - CommandBars object which metrics need to use.
	// Returns:
	//     Size of the control.
	//-----------------------------------------------------------------------
	virtual CSize DrawListBoxControl(CDC* pDC, CXTPControl* pButton, CRect rc, BOOL bSelected, BOOL bDraw, CXTPCommandBars* pCommandBars = 0) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill the command bar's face.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     pBar - Points to a CXTPCommandBar object to draw.
	//-----------------------------------------------------------------------
	virtual void FillCommandBarEntry(CDC* pDC, CXTPCommandBar* pBar) = 0;

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
	//     This method returns control text color to draw
	// Parameters:
	//     pButton - Points to a CXTPControl object to draw.
	//-----------------------------------------------------------------------
	virtual COLORREF GetControlTextColor(CXTPControl* pButton);

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
	virtual COLORREF GetRectangleTextColor(BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition) = 0;

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
	virtual void DrawRectangle(CDC* pDC, CRect rc, BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the command bar's gripper.
	// Parameters:
	//     pDC   - Pointer to a valid device context
	//     pBar  - Points to a CXTPCommandBar object
	//     bDraw - TRUE to draw; FALSE to retrieve the size of the gripper.
	// Returns:
	//     Size of the gripper.
	//-----------------------------------------------------------------------
	virtual CSize DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw = TRUE);

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
	virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw = TRUE) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to determine offset of popuped bar.
	// Parameters:
	//     rc        - Control's bounding rectangle.
	//     pControl  - Points to a CXTPControl object
	//     bVertical - TRUE if control docked vertically.
	//-----------------------------------------------------------------------
	virtual void AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL bVertical) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw a single pane text of the status bar.
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pPane - The status bar pane need to draw
	//     rcItem - Item rectangle
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarPaneEntry(CDC* pDC, CRect rcItem, CXTPStatusBarPane* pPane);

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
	//     This method is called to draw the status bar's gripper.
	// Parameters:
	//     pDC      - Pointer to a valid device context
	//     rcClient - Client rectangle of the status bar.
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarGripper(CDC* pDC, CRect rcClient);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill a dockbar.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     pBar - Points to a CXTPDockBar object
	//-----------------------------------------------------------------------
	virtual void FillDockBar(CDC* pDC, CXTPDockBar* pBar);

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
	//     This method is called to refresh the visual metrics of the manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines the kind of shadow to use.
	// Returns:
	//     TRUE to use alpha and solid shadow; otherwise returns FALSE
	//-----------------------------------------------------------------------
	int GetShadowOptions() const;

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

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to retrieve the command bar's borders.
	// Parameters:
	//     pBar - Points to a CXTPCommandBar object that the borders need to get.
	// Returns:
	//     Borders of the command bar.
	//-----------------------------------------------------------------------
	virtual CRect GetCommandBarBorders(CXTPCommandBar* pBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw a tear-off gripper of Popup Bar.
	// Parameters:
	//     pDC       - Pointer to a valid device context
	//     rcGripper - Bounding rectangle of the gripper.
	//     bSelected - True if the gripper is selected.
	//     bDraw     - TRUE to draw; FALSE to retrieve the size of the gripper.
	// Returns:
	//     Size of the gripper.
	//-----------------------------------------------------------------------
	virtual CSize DrawTearOffGripper(CDC* pDC, CRect rcGripper, BOOL bSelected, BOOL bDraw) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the image of the control.
	// Parameters:
	//     pDC           - Pointer to a valid device context
	//     pt            - Position to draw.
	//     sz            - Size of the image.
	//     pImage        - Points to a CXTPImageManagerIcon object
	//     bSelected     - TRUE if the control is selected.
	//     bPressed      - TRUE if the control is pushed.
	//     bEnabled      - TRUE if the control is enabled.
	//     bChecked      - TRUE if the control is checked.
	//     bPopuped      - TRUE if the control is popuped.
	//     bToolBarImage - TRUE if it is a toolbar image.
	//-----------------------------------------------------------------------
	virtual void DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBarImage = TRUE) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to draw gripper of dialog bar.
	// Parameters:
	//     pDC   - Points to a valid device context.
	//     pBar  - Dialog Bar pointer
	//     bDraw - TRUE to draw gripper, FALSE to calculate size.
	// Returns:
	//     Size of gripper to be drawn.
	//-----------------------------------------------------------------------
	virtual CSize DrawDialogBarGripper(CDC* pDC, CXTPDialogBar* pBar, BOOL bDraw) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines split button command rectangle
	// Parameters:
	//     pButton - Button to test
	// Returns:
	//     Bounding rectangle of command part of split button
	//-----------------------------------------------------------------------
	virtual CRect GetSplitButtonCommandRect(CXTPControl* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set bounding region for popup bars
	// Parameters:
	//     pCommandBar - Pointer to popup bar which region need to set
	//-----------------------------------------------------------------------
	virtual void SetCommandBarRegion(CXTPCommandBar* pCommandBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw check box mark area
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rc - Bounding rectangle
	//     bDraw - TRUE to draw; FALSE to find size
	//     bSelected - TRUE if control is selected
	//     bPressed - TRUE if control is pressed
	//     bChecked - TRUE if control is checked
	//     bEnabled -TRUE if control is enabled
	// Returns: Size of check box mark
	//-----------------------------------------------------------------------
	virtual CSize DrawControlCheckBoxMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw radio button mark area
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rc - Bounding rectangle
	//     bDraw - TRUE to draw; FALSE to find size
	//     bSelected - TRUE if control is selected
	//     bPressed - TRUE if control is pressed
	//     bChecked - TRUE if control is checked
	//     bEnabled -TRUE if control is enabled
	// Returns: Size of radio button mark
	//-----------------------------------------------------------------------
	virtual CSize DrawControlRadioButtonMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw popup bar right gripper.
	// Parameters:
	//     pDC       - Points to a valid device context.
	//     xPos         - Specifies the logical x-coordinate of the upper-left corner of the rectangle.
	//     yPos         - Specifies the logical y-coordinate of the upper-left corner of the destination rectangle.
	//     cx        - Specifies the width of the rectangle.
	//     cy        - Specifies the height of the rectangle.
	//     bExpanded - TRUE if expanded.gripper.
	//-----------------------------------------------------------------------
	virtual void DrawPopupBarGripper(CDC* pDC, int xPos, int yPos, int cx, int cy, BOOL bExpanded = FALSE);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates new PaintManager pointer.
	// Parameters:
	//     paintTheme - Theme to be created.
	// Returns:
	//     Pointer to new CXTPPaintManager object.
	// Example:
	//     <code>pCommandBars->SetPaintManager(CXTPPaintManager::CreateTheme(xtpThemeOfficeXP);</code>
	//-----------------------------------------------------------------------
	static CXTPPaintManager* AFX_CDECL CreateTheme(XTPPaintTheme paintTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to switch the visual theme of the Command Bars.
	// Parameters:
	//     paintTheme - Theme to be set. Can be any of the values listed in
	//                  the Remarks section.
	// Remarks:
	//     paintTheme can be one of following:
	//     * <b>xtpThemeOfficeXP</b> Enables Office XP theme.
	//     * <b>xtpThemeOffice2000</b> Enables Office 2000 theme.
	//     * <b>xtpThemeOffice2003</b> Enables Office 2003 theme.
	//     * <b>xtpThemeNativeWinXP</b> Enables Windows XP themes support.
	//     * <b>xtpThemeWhidbey</b> Enables Visual Studio 2005 theme.
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetTheme(XTPPaintTheme paintTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set a custom theme.
	// Parameters:
	//     pTheme - theme to be set.
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetCustomTheme(CXTPPaintManager* pTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the current visual theme.
	// Returns:
	//     The current theme.
	//-----------------------------------------------------------------------
	XTPPaintTheme GetCurrentTheme() { return m_themeCurrent;}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to destroy a paint manager object.
	//-----------------------------------------------------------------------
	static void AFX_CDECL Done();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the base theme of the manager.
	// Returns:
	//     Returns the base theme.
	//-----------------------------------------------------------------------
	virtual XTPPaintTheme BaseTheme() = 0;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the font of the command bar
	// Parameters:
	//     pBar - Points to a CXTPCommandBar object
	//     bBold - TRUE to retrieve BOLD font
	// Returns:
	//     A pointer to a CFont object
	//-----------------------------------------------------------------------
	virtual CFont* GetCommandBarFont(CXTPCommandBar* pBar, BOOL bBold = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws a triangle.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pt0 - Specifies the logical coordinates of the first point.
	//     pt1 - Specifies the logical coordinates of the second point.
	//     pt2 - Specifies the logical coordinates of the third point.
	//     clr - Color to fill.
	//-----------------------------------------------------------------------
	static void AFX_CDECL Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2, COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Refreshes the fonts to use.
	//-----------------------------------------------------------------------
	virtual void UpdateFonts();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set command bars font.
	// Parameters:
	//     pLogFont         - New LOGFONT to use for text.
	//     bUseStandardFont - When calling SetCommandBarsFontIndirect, always
	//                        use FALSE, this will indicate that pLogFont
	//                        should be used as the new font.
	//-----------------------------------------------------------------------
	void SetCommandBarsFontIndirect(LOGFONT* pLogFont, BOOL bUseStandardFont = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the general font.
	// Returns:
	//     A Pointer to a CFont object
	//-----------------------------------------------------------------------
	CFont* GetRegularFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the general bold font.
	// Returns:
	//     A Pointer to a CFont object
	//-----------------------------------------------------------------------
	CFont* GetRegularBoldFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the icon font.
	// Returns:
	//     A pointer to a CFont object
	//-----------------------------------------------------------------------
	CFont* GetIconFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve caption of floating mini frame.
	// Returns:
	//     A pointer to a CFont object
	//-----------------------------------------------------------------------
	CFont* GetSmCaptionFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to use Tahoma font despite system metrics.
	// Parameters:
	//     bUseOfficeFont - TRUE to use office font.
	//-----------------------------------------------------------------------
	void UseOfficeFont(bool bUseOfficeFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method returns the common controls height.
	// Returns:
	//     Controls height.
	//-----------------------------------------------------------------------
	int GetControlHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws a rectangle.
	// Parameters:
	//     pDC    - Pointer to a valid device context
	//     rc     - Specifies the rectangle in logical units.
	//     nPen   - Specifies the color used to paint the rectangle.
	//     nBrush - Specifies the color used to fill the rectangle.
	//-----------------------------------------------------------------------
	void Rectangle(CDC* pDC, CRect rc, int nPen, int nBrush);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a three-dimensional rectangle.
	// Parameters:
	//     pDC          - Pointer to a valid device context
	//     rc           - Specifies the rectangle in logical units.
	//     nTopLeft     - Specifies the color of the top and left sides
	//                    of the three-dimensional rectangle.
	//     nBottomRight - Specifies the color of the bottom and right sides
	//                    of the three-dimensional rectangle.
	//-----------------------------------------------------------------------
	void Draw3dRect(CDC* pDC, CRect rc, int nTopLeft, int nBottomRight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a check mark.
	// Parameters:
	//     pDC        - Pointer to a valid device context
	//     rcCheck    - Specifies the rectangle in logical units.
	//     clr        - Color to fill.
	//-----------------------------------------------------------------------
	virtual void DrawCheckMark(CDC* pDC, CRect rcCheck, COLORREF clr = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a radio button.
	// Parameters:
	//     pDC        - Pointer to a valid device context
	//     rcCheck    - Specifies the rectangle in logical units.
	//     clr        - Color to fill.
	//-----------------------------------------------------------------------
	virtual void DrawRadioMark(CDC* pDC, CRect rcCheck, COLORREF clr = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a combo box expand symbol
	// Parameters:
	//     pDC        - Pointer to a valid device context
	//     rc    - Specifies the rectangle in logical units.
	//     clr        - Color to fill.
	//-----------------------------------------------------------------------
	virtual void DrawComboExpandMark(CDC* pDC, CRect rc, COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws a line.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     p0  - Specifies the logical coordinates of the start position.
	//     p1  - Specifies the logical coordinates of the endpoint for the line.
	//     x0   - Specifies the logical x-coordinate of the start position.
	//     y0   - Specifies the logical y-coordinate of the start position.
	//     x1   - Specifies the logical x-coordinate of the endpoint for the line.
	//     y1   - Specifies the logical y-coordinate of the endpoint for the line.
	//     nPen - Specifies the color used to paint the line.
	//-----------------------------------------------------------------------
	void Line(CDC* pDC, CPoint p0, CPoint p1);
	void Line(CDC* pDC, int x0, int y0, int x1, int y1, int nPen); //<combine CXTPPaintManager::Line@CDC*@CPoint@CPoint>
	void HorizontalLine(CDC* pDC, int x0, int y, int x1, COLORREF clrPen); //<combine CXTPPaintManager::Line@CDC*@CPoint@CPoint>
	void VerticalLine(CDC* pDC, int x, int y0, int y1, COLORREF clrPen); //<combine CXTPPaintManager::Line@CDC*@CPoint@CPoint>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw a split button expand symbol
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     pControl    - Points to a CXTPControl object
	//     pt          - Position of expand symbol
	//     bSelected   - TRUE if the control is selected.
	//     bPopuped    - TRUE if the control is popuped.
	//     bEnabled    - TRUE if the control is enabled.
	//     bVert       - TRUE if control is vertical
	//-----------------------------------------------------------------------
	virtual void DrawDropDownGlyph(CDC* pDC, CXTPControl* pControl, CPoint pt, BOOL bSelected, BOOL bPopuped, BOOL bEnabled, BOOL bVert);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the pixel at the point.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     xPos    - Specifies the logical x-coordinate of the point to be set.
	//     yPos    - Specifies the logical y-coordinate of the point to be set.
	//     nPen -  Specifies the color used to paint the point
	//-----------------------------------------------------------------------
	void Pixel(CDC* pDC, int xPos, int yPos, int nPen);

	// -------------------------------------------------------------------------
	// Summary:
	//     Gradient fills the rectangle.
	// Parameters:
	//     pDC :         Pointer to a valid device context
	//     lpRect :      Specifies the rectangle in logical units.
	//     crFrom :      Start color.
	//     crTo :        Endpoint color.
	//     bHorz :       TRUE to fill horizontally.
	//     lpRectClip :  Pointer to a RECT structure that contains a clipping
	//                   rectangle. This parameter is optional and may be set to
	//                   NULL.
	// -------------------------------------------------------------------------
	void GradientFill(CDC* pDC, LPRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz, LPCRECT lpRectClip = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves specific icon's options.
	// See Also: XTP_COMMANDBARS_ICONSINFO
	//-----------------------------------------------------------------------
	XTP_COMMANDBARS_ICONSINFO* GetIconsInfo();

	//////////////////////////////////////////////////////////////////////////
	/// Animation Routines

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to animate the PopupBar.
	// Parameters:
	//     pDestDC       - Pointer to device context you must draw to.
	//     pSrcDC        - Device context that contains the bitmap you must
	//                     take.
	//     rc            - Bounding rectangle.
	//     animationType - Type of animation to perform.
	// See Also: SetAnimationDelay
	//-----------------------------------------------------------------------
	virtual void Animate(CDC* pDestDC, CDC* pSrcDC, CRect rc, XTPAnimationType animationType);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to animate the expanding process.
	// Parameters:
	//     pCommandBar - Command bar to animate.
	//     pDestDC     - Pointer to device context you must draw to.
	//     pSrcDC      - Device context that contains the bitmap you must
	//                   take.
	//     bExpandDown - TRUE to animate expanding top-to-bottom; FALSE to animate  bottom-to-top.
	//-----------------------------------------------------------------------
	virtual void AnimateExpanding(CXTPCommandBar* pCommandBar, CDC* pDestDC, CDC* pSrcDC, BOOL bExpandDown);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set animation delay.
	// Parameters:
	//     nAnimationSteps - Number of steps to take during animation.
	//     nAnimationTime  - Amount of time to rest, in milliseconds, between
	//                       each step.
	//-----------------------------------------------------------------------
	void SetAnimationDelay(int nAnimationSteps, int nAnimationTime);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function retrieves the current color of the specified XP
	//     display element. Display elements are the parts of a window
	//     and the display that appear on the system display screen.
	//     The function returns the red, green, blue (RGB) color value
	//     of the given element.  To provide a consistent look to all
	//     Windows applications, the operating system provides and
	//     maintains global settings that define the colors of various
	//     aspects of the display. These settings as a group are sometimes
	//     referred to as a "color scheme." To view the current color
	//     settings, click Control Panel, double-click the Display icon,
	//     and then click the "Appearance" tab.
	// Parameters:
	//     nIndex - Specifies the display element whose color is to be
	//              retrieved.  This parameter can be one of the
	//              following values displayed in the table below:
	// Remarks:
	//     <TABLE>
	//     <b>Constant</b>                   <b>Value</b>  <b>Description</b>
	//     --------------------------------  ============  ------------------------------------------------------------
	//     COLOR_SCROLLBAR                   0             Scroll bar color
	//     COLOR_BACKGROUND                  1             Desktop color
	//     COLOR_ACTIVECAPTION               2             Color of the title bar for the active window, Specifies the left side color in the color gradient of an active window's title bar if the gradient effect is enabled.
	//     COLOR_INACTIVECAPTION             3             Color of the title bar for the inactive window, Specifies the left side color in the color gradient of an inactive window's title bar if the gradient effect is enabled.
	//     COLOR_MENU                        4             Menu background color
	//     COLOR_WINDOW                      5             Window background color
	//     COLOR_WINDOWFRAME                 6             Window frame color
	//     COLOR_MENUTEXT                    7             Color of text on menus
	//     COLOR_WINDOWTEXT                  8             Color of text in windows
	//     COLOR_CAPTIONTEXT                 9             Color of text in caption, size box, and scroll arrow
	//     COLOR_ACTIVEBORDER                10            Border color of active window
	//     COLOR_INACTIVEBORDER              11            Border color of inactive window
	//     COLOR_APPWORKSPACE                12            Background color of multiple-document interface (MDI) applications
	//     COLOR_HIGHLIGHT                   13            Background color of items selected in a control
	//     COLOR_HIGHLIGHTTEXT               14            Text color of items selected in a control
	//     COLOR_BTNFACE                     15            Face color for three-dimensional display elements and for dialog box backgrounds.
	//     COLOR_BTNSHADOW                   16            Color of shading on the edge of command buttons
	//     COLOR_GRAYTEXT                    17            Grayed (disabled) text
	//     COLOR_BTNTEXT                     18            Text color on push buttons
	//     COLOR_INACTIVECAPTIONTEXT         19            Color of text in an inactive caption
	//     COLOR_BTNHIGHLIGHT                20            Highlight color for 3-D display elements
	//     COLOR_3DDKSHADOW                  21            Darkest shadow color for 3-D display elements
	//     COLOR_3DLIGHT                     22            Second lightest 3-D color after 3DHighlight, Light color for three-dimensional display elements (for edges facing the light source.)
	//     COLOR_INFOTEXT                    23            Color of text in ToolTips
	//     COLOR_INFOBK                      24            Background color of ToolTips
	//     COLOR_HOTLIGHT                    26            Color for a hot-tracked item. Single clicking a hot-tracked item executes the item.
	//     COLOR_GRADIENTACTIVECAPTION       27            Right side color in the color gradient of an active window's title bar. COLOR_ACTIVECAPTION specifies the left side color.
	//     COLOR_GRADIENTINACTIVECAPTION     28            Right side color in the color gradient of an inactive window's title bar. COLOR_INACTIVECAPTION specifies the left side color.
	//     XPCOLOR_TOOLBAR_FACE              30            XP toolbar background color.
	//     XPCOLOR_HIGHLIGHT                 31            XP menu item selected color.
	//     XPCOLOR_HIGHLIGHT_BORDER          32            XP menu item selected border color.
	//     XPCOLOR_HIGHLIGHT_PUSHED          33            XP menu item pushed color.
	//     XPCOLOR_HIGHLIGHT_CHECKED         36            XP menu item checked color.
	//     XPCOLOR_HIGHLIGHT_CHECKED_BORDER  37            An RGB value that represents the XP menu item checked border color.
	//     XPCOLOR_ICONSHADDOW               34            XP menu item icon shadow.
	//     XPCOLOR_GRAYTEXT                  35            XP menu item disabled text color.
	//     XPCOLOR_TOOLBAR_GRIPPER           38            XP toolbar gripper color.
	//     XPCOLOR_SEPARATOR                 39            XP toolbar separator color.
	//     XPCOLOR_DISABLED                  40            XP menu icon disabled color.
	//     XPCOLOR_MENUBAR_FACE              41            XP menu item text background color.
	//     XPCOLOR_MENUBAR_EXPANDED          42            XP hidden menu commands background color.
	//     XPCOLOR_MENUBAR_BORDER            43            XP menu border color.
	//     XPCOLOR_MENUBAR_TEXT              44            XP menu item text color.
	//     XPCOLOR_HIGHLIGHT_TEXT            45            XP menu item selected text color.
	//     XPCOLOR_TOOLBAR_TEXT              46            XP toolbar text color.
	//     XPCOLOR_PUSHED_TEXT               47            XP toolbar pushed text color.
	//     XPCOLOR_TAB_INACTIVE_BACK         48            XP inactive tab background color.
	//     XPCOLOR_TAB_INACTIVE_TEXT         49            XP inactive tab text color.
	//     XPCOLOR_HIGHLIGHT_PUSHED_BORDER   50            An RGB value that represents the XP border color for pushed in 3D elements.
	//     XPCOLOR_CHECKED_TEXT              45            XP color for text displayed in a checked button.
	//     XPCOLOR_3DFACE                    51            XP face color for three- dimensional display elements and for dialog box backgrounds.
	//     XPCOLOR_3DSHADOW                  52            XP shadow color for three-dimensional display elements (for edges facing away from the light source).
	//     XPCOLOR_EDITCTRLBORDER            53            XP color for the border color of edit controls.
	//     XPCOLOR_FRAME                     54            Office 2003 frame color.
	//     XPCOLOR_SPLITTER_FACE             55            XP splitter face color.
	//     XPCOLOR_LABEL                     56            Color for label control (xtpControlLabel)
	//     XPCOLOR_STATICFRAME               57            WinXP Static frame color
	//     </TABLE>
	// Returns:
	//     The red, green, blue (RGB) color value of the given element.
	//-----------------------------------------------------------------------
	COLORREF GetXtremeColor(UINT nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the colors for each element to the
	//     color in the specified color array.
	// Parameters:
	//     cElements    - Number of elements in array.
	//     lpaElements  - Array of elements.
	//     lpaRgbValues - Array of RGB values.
	//-----------------------------------------------------------------------
	void SetColors(int cElements, CONST INT* lpaElements, CONST COLORREF* lpaRgbValues);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw control text
	// Parameters:
	//      pDC - Pointer to valid device context
	//      pControl - Pointer to control to draw
	//      rcText - Bounding rectangle of the text
	//      bDraw - TRUE to draw text; FALSE to calculate size
	//      bVert - TRUE if text is vertical
	//      bCentered - TRUE if text is centered
	//      bTriangled - TRUE if triangle drawn
	// Returns: Size of the text
	//-----------------------------------------------------------------------
	CSize DrawControlText(CDC* pDC, CXTPControl* pControl, CRect rcText, BOOL bDraw, BOOL bVert, BOOL bCentered, BOOL bTriangled);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the font exists in the system.
	// Parameters:
	//     lpszFaceName - Font needed to test.
	// Returns:
	//     TRUE if font exists; otherwise returns FALSE
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL FontExists(LPCTSTR lpszFaceName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will search a string, strip off the mnemonic
	//     '&', and reformat the string.
	// Parameters:
	//     strClear - Text needed to strip.
	//-----------------------------------------------------------------------
	static void AFX_CDECL StripMnemonics(CString& strClear);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to prepare caption fro Right-To-Left draw
	// Parameters:
	//     strCaption - Caption to modify
	//-----------------------------------------------------------------------
	virtual void UpdateRTLCaption(CString& strCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine the current Windows XP
	//     theme in use.
	// Returns:
	//     A XTPCurrentSystemTheme enumeration that represents the
	//     current Windows theme in use, can be one of the following
	//     values:
	//     * <b>xtpSystemThemeUnknown</b> Indicates no known theme in use
	//     * <b>xtpSystemThemeBlue</b> Indicates blue theme in use
	//     * <b>xtpSystemThemeOlive</b> Indicates olive theme in use
	//     * <b>xtpSystemThemeSilver</b> Indicates silver theme in use
	// See Also: SetLunaTheme, RefreshLunaColors
	//-----------------------------------------------------------------------
	XTPCurrentSystemTheme GetCurrentSystemTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to force system theme.
	// Parameters:
	//     systemTheme - theme to use. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     theme can be one of the following:
	//     * <b>xtpSystemThemeUnknown</b> Disables <i>luna</i> theme support.
	//     * <b>xtpSystemThemeBlue</b> Enables blue theme
	//     * <b>xtpSystemThemeOlive</b> Enables olive theme
	//     * <b>xtpSystemThemeSilver</b> Enables silver theme
	//-----------------------------------------------------------------------
	void SetLunaTheme(XTPCurrentSystemTheme systemTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates gripper of popup bar.
	// Parameters:
	//     pBar - Popup bar.
	// Returns:
	//     Width of gripper to draw.
	//-----------------------------------------------------------------------
	virtual int GetPopupBarGripperWidth(CXTPCommandBar* pBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns paint manager used to draw galleries
	//-----------------------------------------------------------------------
	CXTPControlGalleryPaintManager* GetGalleryPaintManager() const;


//{{AFX_CODEJOCK_PRIVATE
public:
	// deprecated.
	void ShowKeyboardCues(BOOL bShow);

	CFont* GetIconBoldFont()
	{
		return &m_fontIconBold;
	}
	virtual void FillWorkspace(CDC* pDC, CRect rc, CRect rcExclude);

	virtual void DrawKeyboardTip(CDC* pDC, CXTPCommandBarKeyboardTip* pWnd, BOOL bSetRegion);

	CSize GetAutoIconSize(BOOL bLarge) const;

	static void AFX_CDECL FillCompositeAlpha(CDC* pDC, CRect rc);
	_XTP_DEPRECATE("This function or variable is no longer available. Please use 'DrawControlText' instead")
	CSize DrawTextEx(CDC*, CString, CRect, BOOL, BOOL, BOOL, BOOL, BOOL = FALSE, BOOL = FALSE) {
		return CSize(0, 0);
	}
	int GetSplitDropDownHeight() const;
	void AlphaEllipse(CDC* pDC, CRect rc, COLORREF clrBorder, COLORREF clrFace);

protected:
	void AlphaBlendU(PBYTE pDest, PBYTE pSrcBack, int cx, int cy, PBYTE pSrc, BYTE byAlpha);
	void _DrawCheckMark(CDC* pDC, CRect rcCheck, COLORREF clr = 0, BOOL bLayoutRTL = FALSE);

	CSize DrawControlText2(CDC* pDC, CXTPControl* pControl, CRect rcText, BOOL bDraw, BOOL bVert, BOOL bTrangled);
	void DrawControlText3(CDC* pDC, CXTPControl* pControl, const CString& str, const CString& strClear, CRect rcText, BOOL bVert, BOOL bCentered);
	void SplitString(const CString& str, CString& strFirstRow, CString& strSecondRow);

	CSize DrawControlText(CDC* pDC, CXTPControl* pControl, CRect rcText, BOOL bDraw, BOOL bTriangled, CSize szButton, BOOL bDrawImage);
	BOOL IsFlatToolBar(CXTPCommandBar* pCommandBar);
	CSize GetControlSize(CXTPControl* pControl, CSize sz, BOOL bVert);
	virtual CSize GetPopupBarImageSize(CXTPCommandBar* pBar);
	virtual void DrawPopupBarText(CDC* pDC, const CString& strText, CRect& rcText, UINT nFlags, BOOL bDraw, BOOL bSelected, BOOL bEnabled);

	CSize GetIconSize(CXTPControl* pButton);

//}}AFX_CODEJOCK_PRIVATE

public:
	COLORREF m_clrShadowFactor;             // Shadow color
	COLORREF m_clrStatusTextColor;          // Status bar test color
	CXTPPaintManagerColorGradient m_clrDisabledIcon;    // Disabled icons color
	CXTPPaintManagerColor m_clrFloatingGripper;     // Gripper color of floating toolbar.
	CXTPPaintManagerColor m_clrFloatingGripperText; // Gripper text color of floating toolbar.

	BOOL m_bThickCheckMark;             // TRUE to draw thick check mark
	BOOL m_bShowShadow;                 // TRUE to show shadow
	BOOL m_bThemedStatusBar;            // TRUE to draw status bar using WinXP theme.
	BOOL m_bThemedCheckBox;             // TRUE to draw check box using WinXP theme.
	BOOL m_bClearTypeTextQuality;       // TRUE to enable ClearType text for the font.

	BOOL m_bFlatToolBar;                // TRUE to draw toolbars flat
	BOOL m_bFlatMenuBar;                // TRUE to draw menubar flat
	int m_nPopupBarTextPadding;         // Distance between gripper and control text.
	BOOL m_bSelectImageInPopupBar;      // TRUE to select image as in explorer theme.
	BOOL m_bEnableAnimation;            // TRUE to enable animation for all toolbars

	BOOL m_bOffice2007Padding;          // TRUE if Office2007 metrics used
	BOOL m_bOffsetPopupLabelText;       // TRUE if labels in popups drawn with offset
	BOOL m_bWrapCaptionBelowText;       // Wrap text for xtpButtonIconAndCaptionBelow buttons

	CXTPPaintManagerFont m_fontSmCaption;   // Small caption font.
	CXTPPaintManagerFont m_fontIcon;        // Icon font.
	CXTPPaintManagerFont m_fontIconBold;    // Icon bold font.
	CXTPPaintManagerFont m_fontToolTip;     // Tooltip font

	int   m_nSplitButtonDropDownWidth;      // Split button drop down width in toolbars
	int   m_nSplitButtonPopupWidth;         // Split button drop down width in popups
	BOOL  m_bAutoResizeIcons;               // TRUE to automatically resize icons using current DPI

	CXTPControlGalleryPaintManager* m_pGalleryPaintManager;         // Gallery paint manager
	PVOID m_pfnSetLayeredWindowAttributes;  // point to Transparency proc in USER32.dll module

protected:
	CFont m_fontRegular;                // Regular font.
	CFont m_fontRegularBold;            // Regular bold font.
	CFont m_fontVert;                   // Vertical font.
	CFont m_fontVertBold;               // Vertical font.

	int   m_nTextHeight;                // Text height of the regular font.
	int   m_nEditHeight;                // Edit text height
	BOOL  m_bUseOfficeFont;             // TRUE to use Tahoma font.
	CString m_strOfficeFont;            // Office font
	BOOL  m_bUseStandardFont;           // TRUE to use system font.
	BOOL  m_bEmbossedDisabledText;      // TRUE to draw disabled text embossed
	int   m_nShadowOptions;             // Shadow options

	CXTPWinThemeWrapper m_themeStatusBar;   // StatusBar theme helper.
	CXTPWinThemeWrapper m_themeButton;      // Buttons theme helper.

	XTPPaintTheme m_themeCurrent;           // Current theme.
	static CXTPPaintManager* s_pInstance;   // Instance of the manager.

	XTP_COMMANDBARS_ICONSINFO m_iconsInfo;  // Specific icons options.


	int m_nAnimationSteps;                  // Steps of the animation.
	int m_nAnimationTime;                   // Time of the animation.

	BOOL m_bShowKeyboardCues;               // TRUE to show keyboard cues - deprecated.

	CXTPPaintManagerColor m_arrColor[XPCOLOR_LAST + 1];  // Self colors array.
	XTPCurrentSystemTheme m_systemTheme;   // Current system theme.

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPaintManager object
	//-----------------------------------------------------------------------
	CXTPPaintManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPaintManager object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPaintManager();

public:
	//---------------------------------------------------------------------------
	// Summary:
	//     CLogFont is a self initializing LOGFONT derived class. It
	//     will allow you to create or copy a LOGFONT object, and defines the
	//     attributes of a font.
	//---------------------------------------------------------------------------
	struct _XTP_EXT_CLASS CLogFont : public LOGFONT
	{
		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs an CLogFont object.
		//-----------------------------------------------------------------------
		CLogFont();
	};

	//---------------------------------------------------------------------------
	// Summary:
	//     CNonClientMetrics is a self initializing NONCLIENTMETRICS derived
	//     class. It contains the scalable metrics associated with the
	//     non-client area of a non-minimized window.  This class is used by
	//     the SPI_GETNONCLIENTMETRICS and SPI_SETNONCLIENTMETRICS actions of
	//     SystemParametersInfo.
	//---------------------------------------------------------------------------
	struct _XTP_EXT_CLASS CNonClientMetrics : public NONCLIENTMETRICS
	{
		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CNonClientMetrics object
		//-----------------------------------------------------------------------
		CNonClientMetrics();
	};


	friend class CCommandBarsCtrl;
	friend class CXTPCommandBarsOptions;
	friend class CXTPCommandBars;
};


//---------------------------------------------------------------------------
// Summary:
//     Call this function to access CXTPPaintManager members.
//     Since this class is designed as a single instance object you can
//     only access version info through this method. You <b>cannot</b>
//     directly instantiate an object of type CXTPPaintManager.
// Example:
//     <code>XTPPaintManager()->GetIconsInfo()->bIconsWithShadow = FALSE;</code>
//---------------------------------------------------------------------------
CXTPPaintManager* XTPPaintManager();

AFX_INLINE CXTPPaintManager* XTPPaintManager() {
	if (CXTPPaintManager::s_pInstance == NULL)
	{
		CXTPPaintManager::SetTheme(xtpThemeOffice2000);
	}
	return CXTPPaintManager::s_pInstance;
}


namespace XTPPaintThemes
{
	//===========================================================================
	// Summary:
	//     The CXTPOfficeTheme class is used to enable an Office XP style theme for Command Bars
	// See Also: CXTPPaintManager::SetTheme
	//===========================================================================
	class _XTP_EXT_CLASS CXTPOfficeTheme : public CXTPPaintManager
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPOfficeTheme object.
		//-----------------------------------------------------------------------
		CXTPOfficeTheme();

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
		//     This method is called to draw the command bar's gripper.
		// Parameters:
		//     pDC   - Pointer to a valid device context
		//     pBar  - Points to a CXTPCommandBar object
		//     bDraw - TRUE to draw; FALSE to retrieve the size of the gripper.
		// Returns:
		//     Size of the gripper.
		//-----------------------------------------------------------------------
		virtual CSize DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw = TRUE);

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
		virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw = TRUE);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw the control placed on the ListBox.
		// Parameters:
		//     pDC       - Pointer to a valid device context
		//     pButton   - Points to a CXTPControl object
		//     rc        - Bounding rectangle to draw.
		//     bSelected - TRUE if the control is selected.
		//     bDraw     - TRUE to draw; FALSE to retrieve the size of the control.
		//     pCommandBars - CommandBars object which metrics need to use.
		// Returns:
		//     Size of the control.
		//-----------------------------------------------------------------------
		virtual CSize DrawListBoxControl(CDC* pDC, CXTPControl* pButton, CRect rc, BOOL bSelected, BOOL bDraw, CXTPCommandBars* pCommandBars = 0);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to determine offset of popuped bar.
		// Parameters:
		//     rc       - Control's bounding rectangle.
		//     pControl - Points to a CXTPControl object
		//     bVertical - TRUE if control docked vertically.
		//-----------------------------------------------------------------------
		virtual void AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL bVertical);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw a tear-off gripper of Popup Bar.
		// Parameters:
		//     pDC       - Pointer to a valid device context
		//     rcGripper - Bounding rectangle of the gripper.
		//     bSelected - True if the gripper is selected.
		//     bDraw     - TRUE to draw; FALSE to retrieve the size of the gripper.
		// Returns:
		//     Size of the gripper.
		//-----------------------------------------------------------------------
		virtual CSize DrawTearOffGripper(CDC* pDC, CRect rcGripper, BOOL bSelected, BOOL bDraw);

		//-----------------------------------------------------------------------
		// Summary:
		//     Call this member to draw gripper of dialog bar.
		// Parameters:
		//     pDC   - Points to a valid device context.
		//     pBar  - Dialog Bar pointer
		//     bDraw - TRUE to draw gripper, FALSE to calculate size.
		// Returns:
		//     Size of gripper to be drawn.
		//-----------------------------------------------------------------------
		virtual CSize DrawDialogBarGripper(CDC* pDC, CXTPDialogBar* pBar, BOOL bDraw);

	protected:

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw popup bar right gripper.
		// Parameters:
		//     pDC       - Points to a valid device context.
		//     xPos         - Specifies the logical x-coordinate of the upper-left corner of the rectangle.
		//     yPos         - Specifies the logical y-coordinate of the upper-left corner of the destination rectangle.
		//     cx        - Specifies the width of the rectangle.
		//     cy        - Specifies the height of the rectangle.
		//     bExpanded - TRUE if expanded.gripper.
		//-----------------------------------------------------------------------
		virtual void DrawPopupBarGripper(CDC* pDC, int xPos, int yPos, int cx, int cy, BOOL bExpanded = FALSE);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw the control placed on the PopupBar.
		// Parameters:
		//     pDC     - Pointer to a valid device context
		//     pButton - Points to a CXTPControl object
		//     bDraw   - TRUE to draw; FALSE to retrieve the size of the control.
		// Returns:
		//     Size of the control.
		//-----------------------------------------------------------------------
		virtual CSize DrawControlPopupParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw);


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
		//     This method is called to draw the image of the control.
		// Parameters:
		//     pDC           - Pointer to a valid device context
		//     pt            - Position to draw.
		//     sz            - Size of the image.
		//     pImage        - Points to a CXTPImageManagerIcon object
		//     bSelected     - TRUE if the control is selected.
		//     bPressed      - TRUE if the control is pushed.
		//     bEnabled      - TRUE if the control is enabled.
		//     bChecked      - TRUE if the control is checked.
		//     bPopuped      - TRUE if the control is popuped.
		//     bToolBarImage - TRUE if it is a toolbar image.
		//-----------------------------------------------------------------------
		virtual void DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBarImage = TRUE);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw the control.
		// Parameters:
		//     pDC    - Pointer to a valid device context
		//     bDraw  - TRUE to draw; FALSE to retrieve the size of the control.
		//     pComboBox - CXTPControlComboBox pointer need to draw.
		// Returns:
		//     Size of the control.
		//-----------------------------------------------------------------------
		virtual CSize DrawControlComboBox(CDC* pDC, CXTPControlComboBox* pComboBox, BOOL bDraw);

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
		virtual CSize DrawControlEdit(CDC* pDC, CXTPControlEdit* pEdit, BOOL bDraw);

		//-----------------------------------------------------------------------
		// Summary:
		//     Draws intersect rectangle of popup bar and its control.
		// Parameters:
		//     pDC       - Points to a valid device context.
		//     pPopupBar - Popup bar pointer.
		//     clr       - COLORREF specifies RGB color value.
		//-----------------------------------------------------------------------
		void FillIntersectRect(CDC* pDC, CXTPPopupBar* pPopupBar, COLORREF clr);


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
		//     Retrieves the base theme of the manager.
		// Returns:
		//     Returns the base theme.
		//-----------------------------------------------------------------------
		XTPPaintTheme BaseTheme() { return xtpThemeOfficeXP; }

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
		//     This method is called to draw check box mark area
		// Parameters:
		//     pDC - Pointer to a valid device context
		//     rc - Bounding rectangle
		//     bDraw - TRUE to draw; FALSE to find size
		//     bSelected - TRUE if control is selected
		//     bPressed - TRUE if control is pressed
		//     bChecked - TRUE if control is checked
		//     bEnabled -TRUE if control is enabled
		// Returns: Size of check box mark
		//-----------------------------------------------------------------------
		virtual CSize DrawControlCheckBoxMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw radio button mark area
		// Parameters:
		//     pDC - Pointer to a valid device context
		//     rc - Bounding rectangle
		//     bDraw - TRUE to draw; FALSE to find size
		//     bSelected - TRUE if control is selected
		//     bPressed - TRUE if control is pressed
		//     bChecked - TRUE if control is checked
		//     bEnabled -TRUE if control is enabled
		// Returns: Size of radio button mark
		//-----------------------------------------------------------------------
		virtual CSize DrawControlRadioButtonMark(CDC* pDC, CRect rc, BOOL bDraw, BOOL bSelected, BOOL bPressed, BOOL bChecked, BOOL bEnabled);
	protected:
	//{{AFX_CODEJOCK_PRIVATE
		virtual void DrawSplitButtonPopup(CDC* pDC, CXTPControl* pButton);
		virtual void DrawControlPopupGlyph(CDC* pDC, CXTPControl* pButton);
		virtual void FillPopupLabelEntry(CDC* pDC, CRect rc);
	//}}AFX_CODEJOCK_PRIVATE
	};

	//===========================================================================
	// Summary:
	//     The CXTPDefaultTheme class is used to enable an Office 2000 style theme for Command Bars
	// See Also: CXTPPaintManager::SetTheme
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDefaultTheme : public CXTPPaintManager
	{
	public:
		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDefaultTheme object.
		//-------------------------------------------------------------------------
		CXTPDefaultTheme();

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the manager.
		//-----------------------------------------------------------------------
		virtual void RefreshMetrics();

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
		//     This method is called to draw the command bar's gripper.
		// Parameters:
		//     pDC   - Pointer to a valid device context
		//     pBar  - Points to a CXTPCommandBar object
		//     bDraw - TRUE to draw; FALSE to retrieve the size of the gripper.
		// Returns:
		//     Size of the gripper.
		//-----------------------------------------------------------------------
		virtual CSize DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw = TRUE);

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
		virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw = TRUE);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to determine offset of popuped bar.
		// Parameters:
		//     rc       - Control's bounding rectangle.
		//     pControl - Points to a CXTPControl object
		//     bVertical - TRUE if control docked vertically.
		//-----------------------------------------------------------------------
		virtual void AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL bVertical);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to retrieve the command bar's borders.
		// Parameters:
		//     pBar - Points to a CXTPCommandBar object that the borders need to get.
		// Returns:
		//     Borders of the command bar.
		//-----------------------------------------------------------------------
		virtual CRect GetCommandBarBorders(CXTPCommandBar* pBar);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw the control placed on the ListBox.
		// Parameters:
		//     pDC       - Pointer to a valid device context
		//     pButton   - Points to a CXTPControl object
		//     rc        - Bounding rectangle to draw.
		//     bSelected - TRUE if the control is selected.
		//     bDraw     - TRUE to draw; FALSE to retrieve the size of the control.
		//     pCommandBars - CommandBars object which metrics need to use.
		// Returns:
		//     Size of the control.
		//-----------------------------------------------------------------------
		virtual CSize DrawListBoxControl(CDC* pDC, CXTPControl* pButton, CRect rc, BOOL bSelected, BOOL bDraw, CXTPCommandBars* pCommandBars = 0);

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
		// See Also: XTPSpecialControl
		//-----------------------------------------------------------------------
		virtual CSize DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw a tear-off gripper of Popup Bar.
		// Parameters:
		//     pDC       - Pointer to a valid device context
		//     rcGripper - Bounding rectangle of the gripper.
		//     bSelected - True if the gripper is selected.
		//     bDraw     - TRUE to draw; FALSE to retrieve the size of the gripper.
		// Returns:
		//     Size of the gripper.
		//-----------------------------------------------------------------------
		virtual CSize DrawTearOffGripper(CDC* pDC, CRect rcGripper, BOOL bSelected, BOOL bDraw);

	protected:
		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw the control placed on the PopupBar.
		// Parameters:
		//     pDC     - Pointer to a valid device context
		//     pButton - Points to a CXTPControl object
		//     bDraw   - TRUE to draw; FALSE to retrieve the size of the control.
		// Returns:
		//     Size of the control.
		//-----------------------------------------------------------------------
		virtual CSize DrawControlPopupParent(CDC* pDC, CXTPControl* pButton, BOOL bDraw);

		//-----------------------------------------------------------------------
		// Summary:
		//     Draws split button frame
		// Parameters:
		//     pDC      - Points to a valid device context.
		//     pButton  - Pointer to split button to draw.
		//     rcButton - Bounding rectangle to draw
		//-----------------------------------------------------------------------
		virtual void DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rcButton);


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
		//     This method is called to draw the image of the control.
		// Parameters:
		//     pDC           - Pointer to a valid device context
		//     pt            - Position to draw.
		//     sz            - Size of the image.
		//     pImage        - Points to a CXTPImageManagerIcon object
		//     bSelected     - TRUE if the control is selected.
		//     bPressed      - TRUE if the control is pushed.
		//     bEnabled      - TRUE if the control is enabled.
		//     bChecked      - TRUE if the control is checked.
		//     bPopuped      - TRUE if the control is popuped.
		//     bToolBarImage - TRUE if it is a toolbar image.
		//-----------------------------------------------------------------------
		virtual void DrawImage(CDC* pDC, CPoint pt, CSize sz, CXTPImageManagerIcon* pImage, BOOL bSelected, BOOL bPressed, BOOL bEnabled = TRUE, BOOL bChecked = FALSE, BOOL bPopuped = FALSE, BOOL bToolBarImage = TRUE);

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
		virtual CSize DrawControlComboBox(CDC* pDC, CXTPControlComboBox* pComboBox, BOOL bDraw);

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
		virtual CSize DrawControlEdit(CDC* pDC, CXTPControlEdit* pEdit, BOOL bDraw);

		//-----------------------------------------------------------------------
		// Summary:
		//     Call this member to draw gripper of dialog bar.
		// Parameters:
		//     pDC   - Points to a valid device context.
		//     pBar  - Dialog Bar pointer
		//     bDraw - TRUE to draw gripper, FALSE to calculate size.
		// Returns:
		//     Size of gripper to be drawn.
		//-----------------------------------------------------------------------
		virtual CSize DrawDialogBarGripper(CDC* pDC, CXTPDialogBar* pBar, BOOL bDraw);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw frame of edit control.
		// Parameters:
		//     pDC - Points to a valid device context.
		//     rc - CRect object specifying size of area.
		//     bEnabled  - TRUE if control is enabled.
		//     bSelected - TRUE if control is selected.
		//-----------------------------------------------------------------------
		virtual void DrawControlEditFrame(CDC* pDC, CRect rc, BOOL bEnabled, BOOL bSelected);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw button od combo box control.
		// Parameters:
		//     pDC - Points to a valid device context.
		//     rcBtn     - Button bounding rectangle.
		//     bEnabled  - TRUE if combo box is enabled
		//     bSelected - TRUE if combo box is selected
		//     bDropped  - TRUE TRUE if combo box is dropped.
		//-----------------------------------------------------------------------
		virtual void DrawControlComboBoxButton(CDC* pDC, CRect rcBtn, BOOL bEnabled, BOOL bSelected, BOOL bDropped);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method draws edit control spin buttons
		// Parameters:
		//     pDC - Pointer to device context
		//     pControlEdit - Edit control
		//-----------------------------------------------------------------------
		virtual void DrawControlEditSpin(CDC* pDC, CXTPControlEdit* pControlEdit);

		//-----------------------------------------------------------------------
		// Summary:
		//     Retrieves the base theme of the manager.
		// Returns:
		//     Returns the base theme.
		//-----------------------------------------------------------------------
		XTPPaintTheme BaseTheme() { return xtpThemeOffice2000; }


	protected:
	//{{AFX_CODEJOCK_PRIVATE
		void DrawShadedRect(CDC* pDC, CRect& rect);
		virtual CSize GetPopupBarImageSize(CXTPCommandBar* pBar);
	//}}AFX_CODEJOCK_PRIVATE

	protected:
		int m_nPopupBarText;    // Popup bar text index.
	};

	//===========================================================================
	// Summary:
	//     The CXTPOffice2003Theme class is used to enable an Office 2003 style theme for Command Bars
	// See Also: CXTPPaintManager::SetTheme
	//===========================================================================
	class _XTP_EXT_CLASS CXTPOffice2003Theme : public CXTPOfficeTheme
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPOffice2003Theme object.
		//-----------------------------------------------------------------------
		CXTPOffice2003Theme();

		//-----------------------------------------------------------------------
		// Summary:
		//     Destroys a CXTPOffice2003Theme object, handles cleanup and deallocation
		//-----------------------------------------------------------------------
		virtual ~CXTPOffice2003Theme();

		//-----------------------------------------------------------------------
		// Summary:
		//     Loads bitmap specified by nIDResource and converts its colors.
		// Parameters:
		//     nIDResource - Specifies bitmap resource ID.
		// Returns:
		//     Handle to bitmap with loaded bitmap.
		//-----------------------------------------------------------------------
		static HBITMAP AFX_CDECL GetModernBitmap(UINT nIDResource);

		//-----------------------------------------------------------------------
		// Summary:
		//     Loads bitmap specified by nIDResource and adds it to ImageManager.
		// Parameters:
		//     nIDResource - Specifies bitmap resource ID.
		//     pCommands - Pointer to an array of IDs.
		//     nCount - Number of elements in the array pointed to by lpIDArray.
		//     szIcon - Size of the icons.
		// Returns:
		//     Returns TRUE if successful, FALSE otherwise
		// See also: GetModernBitmap, CXTPImageManager
		//-----------------------------------------------------------------------
		static BOOL AFX_CDECL LoadModernToolbarIcons(UINT nIDResource);
		static BOOL AFX_CDECL LoadModernToolbarIcons(UINT nIDResource, UINT* pCommands, int nCount, CSize szIcon = CSize(16, 15)); // <combine CXTPOffice2003Theme::LoadModernToolbarIcons@UINT>

		//-----------------------------------------------------------------------
		// Summary:
		//     This method creates a new clipping region that consists of the existing clipping region minus the corners rectangles.
		// Parameters:
		//     pDC - Pointer to a valid device context.
		//     rc - CRect object specifying size of area.
		//-----------------------------------------------------------------------
		void ExcludeCorners(CDC* pDC, CRect rc);

	protected:
		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the manager.
		//-----------------------------------------------------------------------
		virtual void RefreshMetrics();

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw popup bar right gripper.
		// Parameters:
		//     pDC       - Points to a valid device context.
		//     xPos         - Specifies the logical x-coordinate of the upper-left corner of the rectangle.
		//     yPos         - Specifies the logical y-coordinate of the upper-left corner of the destination rectangle.
		//     cx        - Specifies the width of the rectangle.
		//     cy        - Specifies the height of the rectangle.
		//     bExpanded - TRUE if expanded.gripper.
		//-----------------------------------------------------------------------
		virtual void DrawPopupBarGripper(CDC* pDC, int xPos, int yPos, int cx, int cy, BOOL bExpanded = FALSE);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to fill a dockbar.
		// Parameters:
		//     pDC  - Pointer to a valid device context
		//     pBar - Points to a CXTPDockBar object
		//-----------------------------------------------------------------------
		virtual void FillDockBar(CDC* pDC, CXTPDockBar* pBar);

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
		//     This method is called to draw a command bar's separator.
		// Parameters:
		//     pDC      - Pointer to a valid device context
		//     pBar     - Points to a CXTPCommandBar object
		//     pControl - Points to a CXTPControl object
		//     bDraw    - TRUE to draw; FALSE to retrieve the size of the separator.
		// Returns:
		//     This method is called to draw a command bar's separator.
		//-----------------------------------------------------------------------
		virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw = TRUE);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw the command bar's gripper.
		// Parameters:
		//     pDC   - Pointer to a valid device context
		//     pBar  - Points to a CXTPCommandBar object
		//     bDraw - TRUE to draw; FALSE to retrieve the size of the gripper.
		// Returns:
		//     Size of the gripper.
		//-----------------------------------------------------------------------
		virtual CSize DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw = TRUE);


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
		// See Also: XTPSpecialControl
		//-----------------------------------------------------------------------
		virtual CSize DrawSpecialControl(CDC* pDC, XTPSpecialControl controlType, CXTPControl* pButton, CXTPCommandBar* pBar, BOOL bDraw, LPVOID lpParam);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to retrieve the command bar's borders.
		// Parameters:
		//     pBar - Points to a CXTPCommandBar object that the borders need to get.
		// Returns:
		//     Borders of the command bar.
		//-----------------------------------------------------------------------
		virtual CRect GetCommandBarBorders(CXTPCommandBar* pBar);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw a tear-off gripper of Popup Bar.
		// Parameters:
		//     pDC       - Pointer to a valid device context
		//     rcGripper - Bounding rectangle of the gripper.
		//     bSelected - True if the gripper is selected.
		//     bDraw     - TRUE to draw; FALSE to retrieve the size of the gripper.
		// Returns:
		//     Size of the gripper.
		//-----------------------------------------------------------------------
		virtual CSize DrawTearOffGripper(CDC* pDC, CRect rcGripper, BOOL bSelected, BOOL bDraw);

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
		//     Call this member to draw gripper of dialog bar.
		// Parameters:
		//     pDC   - Points to a valid device context.
		//     pBar  - Dialog Bar pointer
		//     bDraw - TRUE to draw gripper, FALSE to calculate size.
		// Returns:
		//     Size of gripper to be drawn.
		//-----------------------------------------------------------------------
		virtual CSize DrawDialogBarGripper(CDC* pDC, CXTPDialogBar* pBar, BOOL bDraw);

		//-----------------------------------------------------------------------
		// Summary:
		//     Retrieves the base theme of the manager.
		// Returns:
		//     Returns the base theme.
		//-----------------------------------------------------------------------
		XTPPaintTheme BaseTheme() { return xtpThemeOffice2003; }

		//-----------------------------------------------------------------------
		// Summary:
		//     Generates expand bitmap used for CXTPPopupBar.
		//-----------------------------------------------------------------------
		void CreateGradientCircle();

		//-----------------------------------------------------------------------
		// Summary:
		//     Returns TRUE if Floating Toolbar drawn with gradient background
		// Parameters:
		//     pBar - Toolbar to check
		// Returns:
		//     TRUE if Toolbar has gradient background
		//-----------------------------------------------------------------------
		virtual BOOL HasFloatingBarGradientEntry(CXTPCommandBar* pBar);

	protected:
//{{AFX_CODEJOCK_PRIVATE
		void DrawExpandSymbols(CDC* pDC, BOOL bVertical, CRect rc, BOOL bHiddenExists, COLORREF clr);
		void RectangleEx(CDC* pDC, CRect rc, int nColorBorder, BOOL bHoriz, CXTPPaintManagerColorGradient& color);
//}}AFX_CODEJOCK_PRIVATE

	public:
		CXTPPaintManagerColorGradient m_clrDockBar;             // Color of dockbar face
		CXTPPaintManagerColorGradient m_clrCommandBar;          // Color of commandbar face
		CXTPPaintManagerColorGradient m_clrToolbarExpand;       // Toolbar Expand button color.
		CXTPPaintManagerColorGradient m_clrMenuExpand;          // MenuBar Expand button color.
		CXTPPaintManagerColorGradient m_clrMenuExpandedGripper; // Color of popup bar gripper
		CXTPPaintManagerColorGradient m_clrPopupControl;        // color of popup control

		CXTPPaintManagerColor m_clrToolbarShadow;    // Toolbar bottom shadow color
		CXTPPaintManagerColor m_clrStatusPane;       // Color of status bar cell..
		CXTPPaintManagerColor m_clrTearOffGripper;  // TearOff gripper color

		BOOL     m_bLunaTheme;                       // TRUE if luna theme is active.
		BOOL     m_bPlainSeparators;                 // TRUE to draw plain separators

		CImageList m_ilGradientCircle;               // Gradient circle holder.
		CXTPPaintManagerColorGradient m_grcLunaChecked;         // Background color of checked button
		CXTPPaintManagerColorGradient m_grcLunaPushed;          // Background color of pushed button
		CXTPPaintManagerColorGradient m_grcLunaSelected;        // Background color of selected button

	};

	//===========================================================================
	// Summary:
	//     The CXTPWhidbeyTheme class is used to enable a Visual Studio 2005 style theme for Command Bars
	// See Also: CXTPPaintManager::SetTheme
	//===========================================================================
	class _XTP_EXT_CLASS CXTPWhidbeyTheme : public CXTPOffice2003Theme
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPWhidbeyTheme object.
		//-----------------------------------------------------------------------
		CXTPWhidbeyTheme();

		//-----------------------------------------------------------------------
		// Summary:
		//     Destroys a CXTPWhidbeyTheme object, handles cleanup and deallocation
		//-----------------------------------------------------------------------
		virtual ~CXTPWhidbeyTheme();

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the manager.
		//-----------------------------------------------------------------------
		void RefreshMetrics();

	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Retrieves the base theme of the manager.
		// Returns:
		//     Returns the base theme.
		//-----------------------------------------------------------------------
		XTPPaintTheme BaseTheme() { return xtpThemeWhidbey; }

	};


	//===========================================================================
	// Summary:
	//     The CXTPNativeXPTheme class is used to enable a Windows XP style theme for Command Bars
	// See Also: CXTPPaintManager::SetTheme
	//===========================================================================
	class _XTP_EXT_CLASS CXTPNativeXPTheme : public CXTPDefaultTheme
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPNativeXPTheme object.
		//-----------------------------------------------------------------------
		CXTPNativeXPTheme();

		//-----------------------------------------------------------------------
		// Summary:
		//     Destroys a CXTPNativeXPTheme object, handles cleanup and deallocation
		//-----------------------------------------------------------------------
		~CXTPNativeXPTheme();

	protected:

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the manager.
		//-----------------------------------------------------------------------
		virtual void RefreshMetrics();

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
		//     This method is called to fill the control's face
		// Parameters:
		//     pDC         - Pointer to a valid device context
		//     pButton - Points to a CXTPControl object to draw.
		//-----------------------------------------------------------------------
		void DrawControlEntry(CDC* pDC, CXTPControl* pButton);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method returns control text color to draw
		// Parameters:
		//     pButton - Points to a CXTPControl object to draw.
		//-----------------------------------------------------------------------
		COLORREF GetControlTextColor(CXTPControl* pButton);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw the command bar's gripper.
		// Parameters:
		//     pDC   - Pointer to a valid device context
		//     pBar  - Points to a CXTPCommandBar object
		//     bDraw - TRUE to draw; FALSE to retrieve the size of the gripper.
		// Returns:
		//     Size of the gripper.
		//-----------------------------------------------------------------------
		virtual CSize DrawCommandBarGripper(CDC* pDC, CXTPCommandBar* pBar, BOOL bDraw = TRUE);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to fill a dockbar.
		// Parameters:
		//     pDC  - Pointer to a valid device context
		//     pBar - Points to a CXTPDockBar object
		//-----------------------------------------------------------------------
		virtual void FillDockBar(CDC* pDC, CXTPDockBar* pBar);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to fill a dockbar.
		// Parameters:
		//     pDC - Points to a valid device context.
		//     pWnd   - Client area to fill.
		//     pFrame - Parent frame window.
		//-----------------------------------------------------------------------
		void FillDockBarRect(CDC* pDC, CWnd* pWnd, CWnd* pFrame);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to determine offset of popuped bar.
		// Parameters:
		//     rc       - Control's bounding rectangle.
		//     pControl - Points to a CXTPControl object
		//     bVertical - TRUE if control docked vertically.
		//-----------------------------------------------------------------------
		void AdjustExcludeRect(CRect& rc, CXTPControl* pControl, BOOL bVertical);

		//-----------------------------------------------------------------------
		// Summary:
		//     Draws split button frame
		// Parameters:
		//     pDC      - Points to a valid device context.
		//     pButton  - Pointer to split button to draw.
		//     rcButton - Bounding rectangle to draw
		//-----------------------------------------------------------------------
		void DrawSplitButtonFrame(CDC* pDC, CXTPControl* pButton, CRect rcButton);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw frame of edit control.
		// Parameters:
		//     pDC - Points to a valid device context.
		//     rc - CRect object specifying size of area.
		//     bEnabled  - TRUE if control is enabled.
		//     bSelected - TRUE if control is selected.
		//-----------------------------------------------------------------------
		virtual void DrawControlEditFrame(CDC* pDC, CRect rc, BOOL bEnabled, BOOL bSelected);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw button od combo box control.
		// Parameters:
		//     pDC - Points to a valid device context.
		//     rcBtn     - Button bounding rectangle.
		//     bEnabled  - TRUE if combo box is enabled
		//     bSelected - TRUE if combo box is selected
		//     bDropped  - TRUE TRUE if combo box is dropped.
		//-----------------------------------------------------------------------
		virtual void DrawControlComboBoxButton(CDC* pDC, CRect rcBtn, BOOL bEnabled, BOOL bSelected, BOOL bDropped);

		//-----------------------------------------------------------------------
		// Summary:
		//     Call this member to draw gripper of dialog bar.
		// Parameters:
		//     pDC   - Points to a valid device context.
		//     pBar  - Dialog Bar pointer
		//     bDraw - TRUE to draw gripper, FALSE to calculate size.
		// Returns:
		//     Size of gripper to be drawn.
		//-----------------------------------------------------------------------
		virtual CSize DrawDialogBarGripper(CDC* pDC, CXTPDialogBar* pBar, BOOL bDraw);

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
		virtual CSize DrawCommandBarSeparator(CDC* pDC, CXTPCommandBar* pBar, CXTPControl* pControl, BOOL bDraw = TRUE);


		//-----------------------------------------------------------------------
		// Summary:
		//     Retrieves the base theme of the manager.
		// Returns:
		//     Returns the base theme.
		//-----------------------------------------------------------------------
		XTPPaintTheme BaseTheme() { return xtpThemeNativeWinXP; }

		//-----------------------------------------------------------------------
		// Summary:
		//     Determines if theme is enabled
		// Returns:
		//     TRUE if WinXP theme is enabled
		//-----------------------------------------------------------------------
		BOOL IsThemeEnabled() const;

		//-----------------------------------------------------------------------
		// Summary:
		//     This method draws edit control spin buttons
		// Parameters:
		//     pDC - Pointer to device context
		//     pControlEdit - Edit control
		//-----------------------------------------------------------------------
		virtual void DrawControlEditSpin(CDC* pDC, CXTPControlEdit* pControlEdit);

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
		CXTPWinThemeWrapper m_themeRebar;           // Rebar theme helper
		CXTPWinThemeWrapper m_themeToolbar;         // Toolbar theme helper
		CXTPWinThemeWrapper m_themeCombo;           // ComboBox theme helper
		CXTPWinThemeWrapper m_themeWindow;          // Window theme helper
		CXTPWinThemeWrapper m_themeSpin;          // Window theme helper
		COLORREF            m_clrEdgeShadowColor;   // Color of toolbar bottom line
		COLORREF            m_clrEdgeHighLightColor;// Color of toolbar top line
		BOOL                m_bFlatMenus;           // TRUE if OS flat menu option enabled
	};

	//===========================================================================
	// Summary:
	//     The CXTPReBarPaintManager template class is used to enable a ReBar theme for Command Bars
	// See Also: CXTPPaintManager::SetTheme
	//===========================================================================
	template <class TBase>
	class CXTPReBarPaintManager : public TBase
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPReBarPaintManager object.
		//-----------------------------------------------------------------------
		CXTPReBarPaintManager()
		{
			RefreshMetrics();
		}

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the manager.
		//-----------------------------------------------------------------------
		void RefreshMetrics()
		{
			TBase::RefreshMetrics();

			m_themeRebar.OpenThemeData(0, L"REBAR");
		}

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to fill the command bar's face.
		// Parameters:
		//     pDC  - Pointer to a valid device context
		//     pBar - Points to a CXTPCommandBar object to draw.
		//-----------------------------------------------------------------------
		void FillCommandBarEntry (CDC* pDC, CXTPCommandBar* pBar)
		{
			if (pBar->GetPosition() != xtpBarFloating && pBar->GetPosition() != xtpBarPopup)
			{
				if (m_themeRebar.IsAppThemed())
				{
					CXTPWindowRect rcClient(pBar->GetParent());
					pBar->ScreenToClient(&rcClient);

					m_themeRebar.DrawThemeBackground(pDC->GetSafeHdc(), 0, 0, &rcClient, 0);
				}
				else
					pDC->FillSolidRect(CXTPClientRect(pBar), GetSysColor(COLOR_3DFACE));
			}
			else
			{
				TBase::FillCommandBarEntry(pDC, pBar);
			}
		}

	protected:
		CXTPWinThemeWrapper m_themeRebar;  // ReBar theme helper.
	};

}
using namespace XTPPaintThemes;

//////////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTPPaintManager::SetAnimationDelay(int nAnimationSteps, int nAnimationTime) {
	m_nAnimationSteps = nAnimationSteps;
	m_nAnimationTime = nAnimationTime;
}
AFX_INLINE CFont* CXTPPaintManager::GetRegularFont() {
	return this == NULL ? CFont ::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)): &m_fontRegular;
}
AFX_INLINE CFont* CXTPPaintManager::GetRegularBoldFont() {
	return &m_fontRegularBold;
}
AFX_INLINE CFont* CXTPPaintManager::GetIconFont() {
	return &m_fontIcon;
}
AFX_INLINE CFont* CXTPPaintManager::GetSmCaptionFont() {
	return &m_fontSmCaption;
}
AFX_INLINE void CXTPPaintManager::ShowKeyboardCues(BOOL bShow) {
	m_bShowKeyboardCues = bShow;
}
AFX_INLINE int CXTPPaintManager::GetShadowOptions() const {
	return m_nShadowOptions;
}
AFX_INLINE CXTPControlGalleryPaintManager* CXTPPaintManager::GetGalleryPaintManager() const {
	return m_pGalleryPaintManager;
}

#endif // #if !defined(__XTPPAINTMANAGER_H__)
