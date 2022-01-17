// XTPDockingPanePaintManager.h : interface for the CXTPDockingPanePaintManager class.
//
// This file is a part of the XTREME DOCKINGPANE MFC class library.
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
#if !defined(__XTPDOCKINGPANEPAINTMANAGER_H__)
#define __XTPDOCKINGPANEPAINTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPDockingPaneDefines.h"
#include "Common/XTPWinThemeWrapper.h"
#include "Common/XTPColorManager.h"

class CXTPDockingPaneTabsArray;
class CXTPDockingPaneTabbedContainer;
class CXTPDockingPaneAutoHidePanel;
class CXTPDockingPaneHiddenTabsArray;
class CXTPTabPaintManager;
class CXTPDockingPaneMiniWnd;
class CXTPDockingPaneSplitterWnd;
class CXTPDockingPaneCaptionButtons;


//-----------------------------------------------------------------------
// Summary:
//     Enumeration of the pin button state
// See Also:
//     CXTPDockingPaneCaptionButton::GetState,
//     CXTPDockingPaneCaptionButton::SetState
//
// <KEYWORDS xtpPanePinVisible, xtpPanePinPushed>
//-----------------------------------------------------------------------
enum XTPDockingPanePinState
{
	xtpPanePinVisible = 1, // Pin button is visible.
	xtpPanePinPushed = 2   // Pin button is pushed.
};

//-----------------------------------------------------------------------
// Summary:
//     Enumeration of the splitter styles
// Example:
//     m_paneManager.GetPaintManager()->SetSplitterStyle(xtpPaneSplitterSoft3D + xtpPaneSplitterGripperOffice2003);
// See Also:
//     CXTPDockingPanePaintManager::SetSplitterStyle
//
// <KEYWORDS xtpPaneSplitterFlat, xtpPaneSplitter3D, xtpPaneSplitterSoft3D, xtpPaneSplitterGripperOfficeXP, xtpPaneSplitterGripperOffice2003>
//-----------------------------------------------------------------------
enum XTPDockingPaneSplitterStyle
{
	xtpPaneSplitterFlat     = 0,            // Flat splitter style
	xtpPaneSplitter3D       = 1,            // 3D splitter style
	xtpPaneSplitterSoft3D   = 2,            // Soft splitter style
	xtpPaneSplitterGradient = 4,            // Gradient splitter style
	xtpPaneSplitterGradientVert = 8,            // Gradient splitter style
	xtpPaneSplitterGripperOfficeXP = 0x100,     // Splitter has OfficeXP gripper
	xtpPaneSplitterGripperOffice2003 = 0x200    // Splitter has Office2003 gripper
};

//-----------------------------------------------------------------------
// Summary:
//     Enumeration of the caption button styles
// Example:
//     m_paneManager.GetPaintManager()->SetCaptionButtonStyle(xtpPaneCaptionButtonThemedExplorerBar);
// See Also:
//     CXTPDockingPanePaintManager::SetCaptionButtonStyle
//-----------------------------------------------------------------------
enum XTPDockingPaneCaptionButtonStyle
{
	xtpPaneCaptionButtonDefault,            // Default caption button style
	xtpPaneCaptionButtonOffice,             // Office XP caption button style
	xtpPaneCaptionButtonOffice2003,         // Office 2003 caption button style
	xtpPaneCaptionButtonThemedExplorerBar,  // Explorer button style
	xtpPaneCaptionButtonThemedButton,       // Windows XP Button style
	xtpPaneCaptionButtonThemedToolBar,      // Windows XP Toolbar style
	xtpPaneCaptionButtonThemedToolWindow,   // Window XP Tool Window style
};


//===========================================================================
// Summary:
//     CXTPDockingPaneCaptionButton is a stand alone internal class.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneCaptionButton
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneCaptionButton object
	// Parameters:
	//     nID   - If of the caption button.
	//     pPane - Pane that the caption button controls.
	//-----------------------------------------------------------------------
	CXTPDockingPaneCaptionButton (int nID, CXTPDockingPaneBase* pPane)
	{
		m_nID = nID; m_pPane = pPane;
		m_bPressed = m_bSelected = FALSE;
		m_dwState = 0;
		m_rcButton.SetRectEmpty();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the button's rectangle.
	// Parameters:
	//     rc - Button's rectangle.
	//-----------------------------------------------------------------------
	void SetRect (CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to draw a pin button.
	// Parameters:
	//     pDC     - Pointer to the device context in which to draw.
	//     pt      - Center point.
	//     bPinned - TRUE if the button is pinned.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawPinnButton(CDC* pDC, CPoint pt, BOOL bPinned);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to draw the maximize\restore button.
	// Parameters:
	//     pDC     - Pointer to the device context in which to draw.
	//     pt      - Center point.
	//     bMaximize - TRUE if the button is to be drawn maximized, FALSE to
	//                 draw restore button.
	//     clr - Color of button.
	//-----------------------------------------------------------------------
	static void AFX_CDECL DrawMaximizeRestoreButton(CDC* pDC, CPoint pt, BOOL bMaximize, COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to click button.
	// Parameters:
	//     pWnd    - Parent window of button.
	//     pt      - Initial mouse position.
	//     bClient - TRUE if <i>pt</i> belongs client area of window.
	//-----------------------------------------------------------------------
	DWORD Click (CWnd* pWnd, CPoint pt, BOOL bClient = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the bounding rectangle
	// Returns:
	//     A CRect object
	//-----------------------------------------------------------------------
	CRect GetRect() { return m_rcButton;}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine the state of the caption button.
	//     This will tell you if the pin caption button is visible or
	//     if it is pushed.
	// Returns:
	//     1 if the the button is visible, 2 if it is pushed, 3 if it
	//     is both visible and pushed.
	// See Also: XTPDockingPanePinState
	//-----------------------------------------------------------------------
	DWORD GetState() const {
		return m_dwState;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the state of the pin caption button.
	// Parameters:
	//     dwState - XTPDockingPanePinState of pin button.
	// See Also: XTPDockingPanePinState
	//-----------------------------------------------------------------------
	void SetState(DWORD dwState) {
		m_dwState = dwState;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves button identifier
	// Returns:
	//     Button identifier. Standard values are
	//     * <b>XTP_IDS_DOCKINGPANE_CLOSE</b> Close button
	//     * <b>XTP_IDS_DOCKINGPANE_AUTOHIDE</b> Auto-hide button
	//-----------------------------------------------------------------------
	int GetID() const {
		return m_nID;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the pane that the pin button controls.
	// Returns:
	//     Pane that the pin button controls.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* GetPane() const {
		return m_pPane;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the point belongs to button.
	// Parameters:
	//     pt - Point to test.
	// Returns:
	//     TRUE if point belongs to the button; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL PtInRect (POINT pt) const {return m_rcButton.PtInRect (pt) != 0; }

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to refresh styles of the button.
	// Parameters:
	//     pt - Mouse cursor pointer.
	//-----------------------------------------------------------------------
	BOOL CheckForMouseOver (CPoint pt);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to redraw button
	//-----------------------------------------------------------------------
	void InvalidateRect();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the button is pushed.
	// Returns:
	//     TRUE is the button is pushed, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsPressed() const {
		return m_bPressed;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the button mouse hover the button
	// Returns:
	//     TRUE if mouse hover the button.
	//-----------------------------------------------------------------------
	BOOL IsSelected() const {
		return m_bSelected;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the pin caption button is visible.
	// Returns:
	//     TRUE if the pin button is visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsVisible();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if button is enabled
	// Returns:
	//     TRUE if button is enabled
	//-----------------------------------------------------------------------
	BOOL IsEnabled() const;

protected:
	CRect m_rcButton;             // Bounding rectangle of the button
	CXTPDockingPaneBase* m_pPane; // Parent pane
	int m_nID;                    // Identifier of the button.
	BOOL m_bPressed;              // TRUE if button is pressed by user.
	BOOL m_bSelected;             // TRUE if user move mouse cursor inside button rectangle
	DWORD m_dwState;              // XTPDockingPanePinState of caption button.

	friend class CXTPDockingPaneTabbedContainer;
	friend class CXTPDockingPaneMiniWnd;
};

//===========================================================================
// Summary:
//     CXTPDockingPanePaintManager is a stand alone class.
//     Override some virtual functions to get a new visual theme.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPanePaintManager : public CXTPCmdTarget
{
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPanePaintManager object
	//-----------------------------------------------------------------------
	CXTPDockingPanePaintManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDockingPanePaintManager object, handles cleanup
	//     and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPDockingPanePaintManager();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function to draw a tabbed container.
	// Parameters:
	//     dc    - Reference to the device context in which to draw.
	//     pPane - Pointer to the tabbed container.
	//     rc    - Client rectangle of the tabbed container.
	//-----------------------------------------------------------------------
	virtual void DrawPane(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function to draw a caption.
	// Parameters:
	//     dc - Reference to the device context in which to draw.
	//     pPane - Pointer to the tabbed container.
	//     rc - Client rectangle of the tabbed container.
	//-----------------------------------------------------------------------
	virtual void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc) = 0;

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function to draw buttons of tabbed caption.
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     pButtons    - Collection of buttons to draw.
	//     rcCaption   - Caption rectangle
	//     clr         - Color of button text.
	//     nButtonSize - Size of button
	//     nButtonGap  - Distance between buttons.
	//     bVertical   - TRUE if caption drawn vertically
	//-----------------------------------------------------------------------
	virtual void DrawCaptionButtons(CDC* pDC, CXTPDockingPaneCaptionButtons* pButtons, CRect& rcCaption, COLORREF clr, int nButtonSize, int nButtonGap, BOOL bVertical = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member function to draw button of tabbed caption.
	// Parameters:
	//     pDC         - Pointer to a valid device context
	//     pButton     - Button pointer need to draw.
	//     clrButton   - Color of button text.
	//-----------------------------------------------------------------------
	virtual void DrawCaptionButton(CDC* pDC, CXTPDockingPaneCaptionButton* pButton, COLORREF clrButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member to change the client rectangle of the child
	//     docking pane.
	// Parameters:
	//     pPane  - Pointer to the tabbed container.
	//     rect   - Client rectangle to be changed.
	//     bApply - TRUE to update tabs inside client area.
	//-----------------------------------------------------------------------
	virtual void AdjustClientRect(CXTPDockingPaneTabbedContainer* pPane, CRect& rect, BOOL bApply);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this member to change the caption rectangle of the child
	//     docking pane.
	// Parameters:
	//     pPane - Pointer to a CXTPDockingPaneTabbedContainer object.
	//     rc    - Caption rectangle to be changed.
	//-----------------------------------------------------------------------
	virtual void AdjustCaptionRect(const CXTPDockingPaneTabbedContainer* pPane, CRect& rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get caption gripper for tabbed container
	// Parameters:
	//     pPane - Tabbed container to test
	// Returns: Bounding rectangle of gripper to set size cursor
	//-----------------------------------------------------------------------
	virtual CRect GetCaptionGripperRect(const CXTPDockingPaneTabbedContainer* pPane);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws a line.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     x0 - Specifies the logical x-coordinate of the start position.
	//     y0 - Specifies the logical y-coordinate of the start position.
	//     x1 - Specifies the logical x-coordinate of the endpoint for the line.
	//     y1 - Specifies the logical y-coordinate of the endpoint for the line.
	//     nPen - Specifies the color used to paint the line.
	//-----------------------------------------------------------------------
	void Line(CDC* pDC, int x0, int y0, int x1, int y1, int nPen);
	void Line(CDC* pDC, int x0, int y0, int x1, int y1); // <combine CXTPDockingPanePaintManager::Line@CDC*@int@int@int@int@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the text of a pane caption.
	// Parameters:
	//     dc - Reference to a valid device context
	//     strTitle - Text to draw in caption.
	//     rc - Client rectangle of the tabbed container
	//     bVertical -  TRUE to draw caption vertically
	//     bCalcWidth - TRUE to calculate width
	// Returns:
	//     The width of the text that was drawn in the pane caption bar.
	//-----------------------------------------------------------------------
	int DrawCaptionText(CDC& dc, const CString& strTitle, CRect rc, BOOL bVertical, BOOL bCalcWidth = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the pixel at the point.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     xPos - Specifies the logical x-coordinate of the point to be set.
	//     yPos - Specifies the logical y-coordinate of the point to be set.
	//     nPen - Specifies the color used to paint the point
	//-----------------------------------------------------------------------
	void Pixel(CDC* pDC, int xPos, int yPos, int nPen);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws a rectangle.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rc - Specifies the rectangle in logical units.
	//     nPenEntry - Specifies the color used to paint the rectangle.
	//     nPenBorder - Specifies the color used to fill the rectangle.
	//-----------------------------------------------------------------------
	void Rectangle(CDC* pDC, CRect rc, int nPenEntry, int nPenBorder);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to use office (Tahoma) font.
	// Parameters:
	//     bUseOfficeFont - TRUE to use office font.
	//-----------------------------------------------------------------------
	void UseOfficeFont(BOOL bUseOfficeFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to change caption font
	// Parameters:
	//     pLogFont         - LOGFONT of caption to use
	//     bUseStandardFont - Must be FALSE
	//-----------------------------------------------------------------------
	void SetCaptionFontIndirect(LOGFONT* pLogFont, BOOL bUseStandardFont = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to draw frame of floating window
	// Parameters:
	//     dc    - Pointer to a valid device context
	//     pPane - Floating window pointer
	//     rc    - Client rectangle of floating frame
	//-----------------------------------------------------------------------
	virtual void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this method to draw splitter.
	// Parameters:
	//     dc        - Pointer to a valid device context
	//     pSplitter - Pointer to splitter window
	//-----------------------------------------------------------------------
	virtual void DrawSplitter(CDC& dc, CXTPDockingPaneSplitterWnd* pSplitter);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set Paint manager used for tabs of tabbed container.
	// Parameters:
	//     pManager - New CXTPTabPaintManager pointer
	// Returns:
	//     CXTPTabPaintManager pointer that will be used.
	// See Also: GetTabPaintManager, SetPanelPaintManager
	//-----------------------------------------------------------------------
	CXTPTabPaintManager* SetTabPaintManager(CXTPTabPaintManager* pManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set Paint manager used for tabs of panel.
	// Parameters:
	//     pManager - New CXTPTabPaintManager pointer
	// Returns:
	//     CXTPTabPaintManager pointer that will be used.
	// See Also: GetPanelPaintManager, SetTabPaintManager
	//-----------------------------------------------------------------------
	CXTPTabPaintManager* SetPanelPaintManager(CXTPTabPaintManager* pManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves pointer to CXTPTabPaintManager used to draw tabs of tabbed container.
	// Returns:
	//     Pointer used to draw tabs of tabbed container.
	// See Also: SetTabPaintManager, GetPanelPaintManager
	//-----------------------------------------------------------------------
	CXTPTabPaintManager* GetTabPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves pointer to CXTPTabPaintManager used to draw tabs of panel.
	// Returns:
	//     Pointer used to draw tabs of auto-hide panel.
	// See Also: SetPanelPaintManager, GetTabPaintManager
	//-----------------------------------------------------------------------
	CXTPTabPaintManager* GetPanelPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the current color of the specified XP display element.
	// Parameters:
	//     nIndex - Specifies the display element whose color is to be
	//              retrieved.  This parameter can be one of the
	//              following values displayed in the table below:
	// Remarks:
	//     Display elements are the parts of a window
	//     and the display that appear on the system display screen.
	//     The function returns the red, green, blue (RGB) color value
	//     of the given element.  To provide a consistent look to all
	//     Windows applications, the operating system provides and
	//     maintains global settings that define the colors of various
	//     aspects of the display. These settings as a group are sometimes
	//     referred to as a "color scheme." To view the current color
	//     settings, click Control Panel, double-click the Display icon,
	//     and then click the "Appearance" tab.
	//<TABLE>
	//  <b>Constant</b>                   <b>Value</b>  <b>Description</b>
	//  --------------------------------  ============  ------------------------------------------------------------
	//  COLOR_SCROLLBAR                   0             Scroll bar color
	//  COLOR_BACKGROUND                  1             Desktop color
	//  COLOR_ACTIVECAPTION               2             Color of the title bar for the active window, Specifies the left side color in the color gradient of an active window's title bar if the gradient effect is enabled.
	//  COLOR_INACTIVECAPTION             3             Color of the title bar for the inactive window, Specifies the left side color in the color gradient of an inactive window's title bar if the gradient effect is enabled.
	//  COLOR_MENU                        4             Menu background color
	//  COLOR_WINDOW                      5             Window background color
	//  COLOR_WINDOWFRAME                 6             Window frame color
	//  COLOR_MENUTEXT                    7             Color of text on menus
	//  COLOR_WINDOWTEXT                  8             Color of text in windows
	//  COLOR_CAPTIONTEXT                 9             Color of text in caption, size box, and scroll arrow
	//  COLOR_ACTIVEBORDER                10            Border color of active window
	//  COLOR_INACTIVEBORDER              11            Border color of inactive window
	//  COLOR_APPWORKSPACE                12            Background color of multiple-document interface (MDI) applications
	//  COLOR_HIGHLIGHT                   13            Background color of items selected in a control
	//  COLOR_HIGHLIGHTTEXT               14            Text color of items selected in a control
	//  COLOR_BTNFACE                     15            Face color for three-dimensional display elements and for dialog box backgrounds.
	//  COLOR_BTNSHADOW                   16            Color of shading on the edge of command buttons
	//  COLOR_GRAYTEXT                    17            Grayed (disabled) text
	//  COLOR_BTNTEXT                     18            Text color on push buttons
	//  COLOR_INACTIVECAPTIONTEXT         19            Color of text in an inactive caption
	//  COLOR_BTNHIGHLIGHT                20            Highlight color for 3-D display elements
	//  COLOR_3DDKSHADOW                  21            Darkest shadow color for 3-D display elements
	//  COLOR_3DLIGHT                     22            Second lightest 3-D color after 3DHighlight, Light color for three-dimensional display elements (for edges facing the light source.)
	//  COLOR_INFOTEXT                    23            Color of text in ToolTips
	//  COLOR_INFOBK                      24            Background color of ToolTips
	//  COLOR_HOTLIGHT                    26            Color for a hot-tracked item. Single clicking a hot-tracked item executes the item.
	//  COLOR_GRADIENTACTIVECAPTION       27            Right side color in the color gradient of an active window's title bar. COLOR_ACTIVECAPTION specifies the left side color.
	//  COLOR_GRADIENTINACTIVECAPTION     28            Right side color in the color gradient of an inactive window's title bar. COLOR_INACTIVECAPTION specifies the left side color.
	//  XPCOLOR_TOOLBAR_FACE              30            XP toolbar background color.
	//  XPCOLOR_HIGHLIGHT                 31            XP menu item selected color.
	//  XPCOLOR_HIGHLIGHT_BORDER          32            XP menu item selected border color.
	//  XPCOLOR_HIGHLIGHT_PUSHED          33            XP menu item pushed color.
	//  XPCOLOR_HIGHLIGHT_CHECKED         36            XP menu item checked color.
	//  XPCOLOR_HIGHLIGHT_CHECKED_BORDER  37            An RGB value that represents the XP menu item checked border color.
	//  XPCOLOR_ICONSHADDOW               34            XP menu item icon shadow.
	//  XPCOLOR_GRAYTEXT                  35            XP menu item disabled text color.
	//  XPCOLOR_TOOLBAR_GRIPPER           38            XP toolbar gripper color.
	//  XPCOLOR_SEPARATOR                 39            XP toolbar separator color.
	//  XPCOLOR_DISABLED                  40            XP menu icon disabled color.
	//  XPCOLOR_MENUBAR_FACE              41            XP menu item text background color.
	//  XPCOLOR_MENUBAR_EXPANDED          42            XP hidden menu commands background color.
	//  XPCOLOR_MENUBAR_BORDER            43            XP menu border color.
	//  XPCOLOR_MENUBAR_TEXT              44            XP menu item text color.
	//  XPCOLOR_HIGHLIGHT_TEXT            45            XP menu item selected text color.
	//  XPCOLOR_TOOLBAR_TEXT              46            XP toolbar text color.
	//  XPCOLOR_PUSHED_TEXT               47            XP toolbar pushed text color.
	//  XPCOLOR_TAB_INACTIVE_BACK         48            XP inactive tab background color.
	//  XPCOLOR_TAB_INACTIVE_TEXT         49            XP inactive tab text color.
	//  XPCOLOR_HIGHLIGHT_PUSHED_BORDER   50            An RGB value that represents the XP border color for pushed in 3D elements.
	//  XPCOLOR_CHECKED_TEXT              45            XP color for text displayed in a checked button.
	//  XPCOLOR_3DFACE                    51            XP face color for three- dimensional display elements and for dialog box backgrounds.
	//  XPCOLOR_3DSHADOW                  52            XP shadow color for three-dimensional display elements (for edges facing away from the light source).
	//  XPCOLOR_EDITCTRLBORDER            53            XP color for the border color of edit controls.
	//  XPCOLOR_FRAME                     54            Office 2003 frame color.
	//  XPCOLOR_SPLITTER_FACE             55            XP splitter face color.
	//  XPCOLOR_LABEL                     56            Color for label control (xtpControlLabel)
	//  XPCOLOR_STATICFRAME               57            WinXP Static frame color
	//</TABLE>
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
	// See Also: SetColor
	//-----------------------------------------------------------------------
	void SetColors(int cElements, CONST INT *lpaElements, CONST COLORREF* lpaRgbValues);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get caption height of tabbed container
	// See Also: GetTabsHeight
	//-----------------------------------------------------------------------
	int GetCaptionHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get tabs height of tabbed container
	// See Also: GetCaptionHeight
	//-----------------------------------------------------------------------
	int GetTabsHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set splitter style.
	// Parameters:
	//     dwStyle - New style of splitters
	// Remarks:
	//     dwStyle parameter can be one or more of the following:
	//     * <b>xtpPaneSplitterFlat</b>
	//     * <b>xtpPaneSplitter3D</b>
	//     * <b>xtpPaneSplitterSoft3D</b>
	//     * <b>xtpPaneSplitterGripperOfficeXP</b>
	//     * <b>xtpPaneSplitterGripperOffice2003</b>
	// Example:
	//     m_paneManager.GetPaintManager()->SetSplitterStyle(xtpPaneSplitterSoft3D + xtpPaneSplitterGripperOffice2003);
	//-----------------------------------------------------------------------
	void SetSplitterStyle(DWORD dwStyle);


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get splitter styles
	// See Also: XTPDockingPaneSplitterStyle, SetSplitterStyle
	//-----------------------------------------------------------------------
	DWORD GetSplitterStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set caption buttons style.
	// Parameters:
	//     nStyle - New style of caption buttons
	// Remarks:
	//     dwStyle parameter can be one or more of the following:
	//     * <b>xtpPaneCaptionButtonDefault</b>
	//     * <b>xtpPaneCaptionButtonOffice</b>
	//     * <b>xtpPaneCaptionButtonOffice2003</b>
	//     * <b>xtpPaneCaptionButtonThemeExplorerBar</b>
	//     * <b>xtpPaneCaptionButtonThemeButton</b>
	//     * <b>xtpPaneCaptionButtonThemeToolBar</b>
	// Example:
	//     m_paneManager.GetPaintManager()->SetCaptionButtonStyle(xtpPaneCaptionButtonOffice2003);
	// See Also: XTPDockingPaneCaptionButtonStyle, GetCaptionButtonStyle
	//-----------------------------------------------------------------------
	void SetCaptionButtonStyle(XTPDockingPaneCaptionButtonStyle nStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get caption button styles
	// See Also: XTPDockingPaneCaptionButtonStyle, SetCaptionButtonStyle
	//-----------------------------------------------------------------------
	XTPDockingPaneCaptionButtonStyle GetCaptionButtonStyle() const;

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
	XTPCurrentSystemTheme GetCurrentSystemTheme() const;

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

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Draws only the glyph of a button. For example, the cross of a Close Button.
	// Parameters:
	//     pDC       - Pointer to the device context in which to draw.
	//     pButton   - Button pointer need to draw.
	//     pt        - Center point.
	//     clrButton - Color of glyph.
	//-----------------------------------------------------------------------
	virtual void DrawCaptionButtonEntry(CDC* pDC, CXTPDockingPaneCaptionButton* pButton, CPoint pt, COLORREF clrButton);

//{{AFX_CODEJOCK_PRIVATE
	void DrawCaptionIcon(CDC& dc, CXTPDockingPaneBase* pPane, CRect& rcCaption, BOOL bVertical);
public:
	static CXTPDockingPane* AFX_CDECL GetSelectedPane(CXTPDockingPaneBase* pPane);
	BOOL IsCaptionEnabled(CXTPDockingPaneBase* pPane);
//}}AFX_CODEJOCK_PRIVATE
public:
	BOOL  m_bUseBoldCaptionFont;    // TRUE to use Bold font for caption
	BOOL  m_bHighlightActiveCaption; // TRUE to highlight active caption.
	int   m_nSplitterSize;          // Splitter size
	BOOL  m_bShowCaption;           // TRUE to draw captions of tabbed containers
	BOOL  m_bDrawSingleTab;         // TRUE to draw single tab.
	BOOL  m_bDrawCaptionIcon;       // TRUE to draw caption icon

	CXTPPaintManagerColor    m_clrSplitterGripper; // Splitter gripper color
	CXTPPaintManagerColor    m_clrSplitter;        // Splitter color
	CXTPPaintManagerColorGradient    m_clrSplitterGradient;        // Splitter color

protected:
	XTPDockingPaneCaptionButtonStyle m_nCaptionButtonStyle;        // Caption button style
	DWORD m_dwSplitterStyle;        // Splitter style
	int   m_nTitleHeight;           // Title's height.
	int   m_nCaptionFontGap;        // Additional gap of font.
	int   m_nTabsHeight;            // Tab bar's height.
	CFont m_fntTitle;               // Title font.
	CFont m_fntTitleVertical;       // Title vertical font.
	BOOL  m_bUseOfficeFont;         // TRUE to use Tahoma font.
	BOOL  m_bUseStandardFont;       // TRUE to use system icon font.

protected:
	CXTPTabPaintManager* m_pTabPaintManager;    // Paint Manager used to draw tabs for tabbed container
	CXTPTabPaintManager* m_pPanelPaintManager;  // Paint Manager used to draw tabs for auto-hide panel

	COLORREF m_arrColor[XPCOLOR_LAST + 1];      // Self colors array.
	XTPDockingPanePaintTheme m_themeCurrent;    // Current theme.
	CString                  m_strOfficeFont;   // Office font name.
	CRect                    m_rcCaptionMargin; // Margins of caption.
	XTPCurrentSystemTheme    m_systemTheme;     // Current system theme.

	CXTPWinThemeWrapper m_themeToolbar;         // Toolbar theme helper
	CXTPWinThemeWrapper m_themeExplorer;        // WinXP Explorer theme helper
	CXTPWinThemeWrapper m_themeButton;          // WinXP Button theme helper
	CXTPWinThemeWrapper m_themeWindow;          // WinXP Window Theme helper.

	BOOL     m_bLunaTheme;                      // TRUE if luna colors user


private:
	friend class CXTPDockingPaneManager;

};


namespace XTPDockingPanePaintThemes
{
	//===========================================================================
	// Summary:
	//     CXTPDockingPaneDefaultTheme is CXTPDockingPanePaintManager derived class, represents
	//     classic theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeDefault); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneDefaultTheme : public CXTPDockingPanePaintManager
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneDefaultTheme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneDefaultTheme();
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Call this method to draw frame of floating window
		// Parameters:
		//     dc    - Pointer to a valid device context
		//     pPane - Floating window pointer
		//     rc    - CLient rectangle of floating frame
		//-----------------------------------------------------------------------
		void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);

		//-----------------------------------------------------------------------
		// Summary:
		//     Override this member function to draw a caption.
		// Parameters:
		//     dc - Reference to the device context in which to draw.
		//     pPane - Pointer to the tabbed container.
		//     rc - Client rectangle of the tabbed container.
		//-----------------------------------------------------------------------
		virtual void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

	protected:
		//-----------------------------------------------------------------------
		// Summary:
		//     Draw common part of tabbed and floating caption.
		// Parameters:
		//     dc        - Reference to the device context in which to draw.
		//     pPane     - Container which caption need to draw.
		//     rcCaption - Caption bounding rectangle
		//     strTitle  - Caption text.
		//     bActive   - TRUE if caption is active.
		//     bVertical - TRUE to draw caption vertically
		//-----------------------------------------------------------------------
		virtual void DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical);

	};

	//===========================================================================
	// Summary:
	//     CXTPDockingPaneGripperedTheme is CXTPDockingPaneDefaultTheme derived class, represents
	//     gripper theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeGrippered); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneGripperedTheme : public CXTPDockingPaneDefaultTheme
	{
	public:
		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneGripperedTheme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneGripperedTheme();

	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Call this method to draw frame of floating window
		// Parameters:
		//     dc    - Pointer to a valid device context
		//     pPane - Floating window pointer
		//     rc    - CLient rectangle of floating frame
		//-----------------------------------------------------------------------
		void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);

		//-----------------------------------------------------------------------
		// Summary:
		//     Override this member function to draw a caption.
		// Parameters:
		//     dc - Reference to the device context in which to draw.
		//     pPane - Pointer to the tabbed container.
		//     rc - Client rectangle of the tabbed container.
		//-----------------------------------------------------------------------
		virtual void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

	protected:

		//-----------------------------------------------------------------------
		// Summary:
		//     Draw common part of tabbed and floating caption.
		// Parameters:
		//     dc        - Reference to the device context in which to draw.
		//     pPane     - Container which caption need to draw.
		//     rcCaption - Caption bounding rectangle
		//     strTitle  - Caption text.
		//     bActive   - TRUE if caption is active.
		//     bVertical - TRUE to draw caption vertically
		//-----------------------------------------------------------------------
		virtual void DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical);
	};

	//===========================================================================
	// Summary:
	//     CXTPDockingPaneExplorerTheme is CXTPDockingPaneDefaultTheme derived class, represents
	//     explorer theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeExplorer); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneExplorerTheme : public CXTPDockingPaneGripperedTheme
	{
	public:
		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneExplorerTheme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneExplorerTheme();

	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the pane.
		//-----------------------------------------------------------------------
		void RefreshMetrics();

	protected:

		//-----------------------------------------------------------------------
		// Summary:
		//     Draw common part of tabbed and floating caption.
		// Parameters:
		//     dc        - Reference to the device context in which to draw.
		//     pPane     - Container which caption need to draw.
		//     rcCaption - Caption bounding rectangle
		//     strTitle  - Caption text.
		//     bActive   - TRUE if caption is active.
		//     bVertical - TRUE to draw caption vertically
		//-----------------------------------------------------------------------
		virtual void DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical);

	protected:
		CXTPWinThemeWrapper m_themeRebar;           // Rebar theme helper
	};

	//===========================================================================
	// Summary:
	//     CXTPDockingPaneOfficeTheme is CXTPDockingPaneGripperedTheme derived class, represents
	//     Office XP theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeOffice); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneOfficeTheme : public CXTPDockingPaneGripperedTheme
	{
		class COfficePanelColorSet;

	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneOfficeTheme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneOfficeTheme();

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the pane.
		//-----------------------------------------------------------------------
		virtual void RefreshMetrics();

	protected:

		//-----------------------------------------------------------------------
		// Summary:
		//     Draw common part of tabbed and floating caption.
		// Parameters:
		//     dc        - Reference to the device context in which to draw.
		//     pPane     - Container which caption need to draw.
		//     rcCaption - Caption bounding rectangle
		//     strTitle  - Caption text.
		//     bActive   - TRUE if caption is active.
		//     bVertical - TRUE to draw caption vertically
		//-----------------------------------------------------------------------
		virtual void DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical);

		//-----------------------------------------------------------------------
		// Summary:
		//     Draws the background of the pane caption.
		// Parameters:
		//     dc        - Reference to the device context in which to draw.
		//     pPane     - Container of the caption needs to draw the background.
		//     rcCaption - Caption bounding rectangle
		//     strTitle  - Caption text.
		//     bActive   - TRUE if caption is active.
		//     bVertical - TRUE to draw caption vertically
		//-----------------------------------------------------------------------
		virtual COLORREF FillCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, BOOL bActive, BOOL bVertical);

	protected:
		BOOL m_bGradientCaption;                            // TRUE to draw gradient caption.

	protected:
		CXTPPaintManagerColorGradient m_clrNormalCaption;   // Color of pane caption when inactive.
		CXTPPaintManagerColorGradient m_clrActiveCaption;   // Color of pane caption when active.
		CXTPPaintManagerColor m_clrNormalCaptionText;                    // Color of pane caption text when the caption is inactive
		CXTPPaintManagerColor m_clrActiveCaptionText;                    // Active caption text
	};

	//===========================================================================
	// Summary:
	//     CXTPDockingPaneNativeXPTheme is CXTPDockingPaneOfficeTheme derived class, represents
	//     Win XP theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeNativeWinXP); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneNativeXPTheme : public CXTPDockingPaneOfficeTheme
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneNativeXPTheme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneNativeXPTheme();

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the pane.
		//-----------------------------------------------------------------------
		virtual void RefreshMetrics();

		//-----------------------------------------------------------------------
		// Summary:
		//     Override this member function to draw a caption.
		// Parameters:
		//     dc - Reference to the device context in which to draw.
		//     pPane - Pointer to the tabbed container.
		//     rc - Client rectangle of the tabbed container.
		//-----------------------------------------------------------------------
		virtual void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

		//-----------------------------------------------------------------------
		// Summary:
		//     Call this method to draw frame of floating window
		// Parameters:
		//     dc    - Pointer to a valid device context
		//     pPane - Floating window pointer
		//     rc    - CLient rectangle of floating frame
		//-----------------------------------------------------------------------
		void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);

	protected:
		//-----------------------------------------------------------------------
		// Summary:
		//     Draw common part of tabbed and floating caption.
		// Parameters:
		//     dc        - Reference to the device context in which to draw.
		//     pPane     - Container which caption need to draw.
		//     rcCaption - Caption bounding rectangle
		//     strTitle  - Caption text.
		//     bActive   - TRUE if caption is active.
		//     bVertical - TRUE to draw caption vertically
		//-----------------------------------------------------------------------
		virtual void DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical);

	protected:
	};

	//===========================================================================
	// Summary:
	//     CXTPDockingPaneVisioTheme is CXTPDockingPaneOfficeTheme derived class, represents
	//     Visio theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeVisio); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneVisioTheme : public CXTPDockingPaneOfficeTheme
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneVisioTheme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneVisioTheme();

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the pane.
		//-----------------------------------------------------------------------
		void RefreshMetrics();

		//-----------------------------------------------------------------------
		// Summary:
		//     Override this member function to draw a caption.
		// Parameters:
		//     dc - Reference to the device context in which to draw.
		//     pPane - Pointer to the tabbed container.
		//     rc - Client rectangle of the tabbed container.
		//-----------------------------------------------------------------------
		void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

		//-----------------------------------------------------------------------
		// Summary:
		//     Override this member function to draw a tabbed container.
		// Parameters:
		//     dc    - Reference to the device context in which to draw.
		//     pPane - Pointer to the tabbed container.
		//     rc    - Client rectangle of the tabbed container.
		//-----------------------------------------------------------------------
		void DrawPane(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

		//-----------------------------------------------------------------------
		// Summary:
		//     Override this member to change the caption rectangle of the child
		//     docking pane.
		// Parameters:
		//     pPane - Pointer to a CXTPDockingPaneTabbedContainer object.
		//     rc    - Caption rectangle to be changed.
		//-----------------------------------------------------------------------
		void AdjustCaptionRect(const CXTPDockingPaneTabbedContainer* pPane, CRect& rc);

		//-----------------------------------------------------------------------
		// Summary:
		//     Call this method to draw frame of floating window
		// Parameters:
		//     dc    - Pointer to a valid device context
		//     pPane - Floating window pointer
		//     rc    - Client rectangle of floating frame
		//-----------------------------------------------------------------------
		void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);
	};

	//===========================================================================
	// Summary:
	//     CXTPDockingPaneOffice2003Theme is CXTPDockingPaneOfficeTheme derived class, represents
	//     Office 2003 theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeOffice2003); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneOffice2003Theme : public CXTPDockingPaneOfficeTheme
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneOffice2003Theme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneOffice2003Theme();

		//-------------------------------------------------------------------------
		// Summary:
		//     Destroys a CXTPDockingPaneOffice2003Theme object, handles cleanup and deallocation.
		//-------------------------------------------------------------------------
		virtual ~CXTPDockingPaneOffice2003Theme();

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the pane.
		//-----------------------------------------------------------------------
		virtual void RefreshMetrics();

		//-----------------------------------------------------------------------
		// Summary:
		//     Override this member function to draw a caption.
		// Parameters:
		//     dc - Reference to the device context in which to draw.
		//     pPane - Pointer to the tabbed container.
		//     rc - Client rectangle of the tabbed container.
		//-----------------------------------------------------------------------
		virtual void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);

		//-----------------------------------------------------------------------
		// Summary:
		//     Call this method to draw frame of floating window
		// Parameters:
		//     dc    - Pointer to a valid device context
		//     pPane - Floating window pointer
		//     rc    - Client rectangle of floating frame
		//-----------------------------------------------------------------------
		virtual void DrawFloatingFrame(CDC& dc, CXTPDockingPaneMiniWnd* pPane, CRect rc);

		//-----------------------------------------------------------------------
		// Summary:
		//     Call this method to get caption gripper for tabbed container
		// Parameters:
		//     pPane - Tabbed container to test
		// Returns: Bounding rectangle of gripper to set size cursor
		//-----------------------------------------------------------------------
		virtual CRect GetCaptionGripperRect(const CXTPDockingPaneTabbedContainer* pPane);

	protected:
		//-----------------------------------------------------------------------
		// Summary:
		//     Draw common part of tabbed and floating caption.
		// Parameters:
		//     dc        - Reference to the device context in which to draw.
		//     pPane     - Container which caption need to draw.
		//     rcCaption - Caption bounding rectangle
		//     strTitle  - Caption text.
		//     bActive   - TRUE if caption is active.
		//     bVertical - TRUE to draw caption vertically
		//-----------------------------------------------------------------------
		virtual void DrawCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, const CString& strTitle, BOOL bActive, BOOL bVertical);


	protected:
		BOOL     m_bDrawGripper;            // TRUE to draw gripper
		BOOL     m_bRoundedCaption;         // TRUE to draw rounded caption
	};

	//===========================================================================
	// Summary:
	//     CXTPDockingPaneOffice2007Theme is CXTPDockingPaneOffice2003Theme derived class, represents
	//     Office 2007 theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeOffice2003); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneOffice2007Theme : public CXTPDockingPaneOffice2003Theme
	{
	public:
		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneOffice2007Theme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneOffice2007Theme();
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the pane.
		//-----------------------------------------------------------------------
		void RefreshMetrics();

		//-----------------------------------------------------------------------
		// Summary:
		//     Override this member function to draw a caption.
		// Parameters:
		//     dc - Reference to the device context in which to draw.
		//     pPane - Pointer to the tabbed container.
		//     rc - Client rectangle of the tabbed container.
		//-----------------------------------------------------------------------
		void DrawCaption(CDC& dc, CXTPDockingPaneTabbedContainer* pPane, CRect rc);
	};

	//===========================================================================
	// Summary:
	//     CXTPDockingPaneShortcutBar2003Theme is CXTPDockingPaneOffice2003Theme derived class, represents
	//     Office 2003 theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeShortcutBar2003); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneShortcutBar2003Theme : public CXTPDockingPaneOffice2003Theme
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneShortcutBar2003Theme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneShortcutBar2003Theme();

		//-------------------------------------------------------------------------
		// Summary:
		//     Destroys a CXTPDockingPaneShortcutBar2003Theme object, handles cleanup and deallocation.
		//-------------------------------------------------------------------------
		virtual ~CXTPDockingPaneShortcutBar2003Theme();

	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the pane.
		//-----------------------------------------------------------------------
		virtual void RefreshMetrics();

		//-----------------------------------------------------------------------
		// Summary:
		//     Call this method to get caption gripper for tabbed container
		// Parameters:
		//     pPane - Tabbed container to test
		// Returns: Bounding rectangle of gripper to set size cursor
		//-----------------------------------------------------------------------
		virtual CRect GetCaptionGripperRect(const CXTPDockingPaneTabbedContainer* pPane);

	};

	//===========================================================================
	// Summary:
	//     CXTPDockingPaneWhidbeyTheme is CXTPDockingPaneOfficeTheme derived class, represents
	//     Visual Studio 2005 theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeWhidbey); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneWhidbeyTheme : public CXTPDockingPaneOfficeTheme
	{
	public:
		class CColorSetWhidbey;
		class CColorSetVisualStudio2005;

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneWhidbeyTheme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneWhidbeyTheme();

	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the pane.
		//-------------------------------------------------------------------------
		void RefreshMetrics();

	};


	//===========================================================================
	// Summary:
	//     CXTPDockingPaneVisualStudio2005SecondTheme is CXTPDockingPaneWhidbeyTheme derived class, represents
	//     Visual Studio 2005 beta 2 theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeWhidbey); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneVisualStudio2005SecondTheme : public CXTPDockingPaneWhidbeyTheme
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneVisualStudio2005SecondTheme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneVisualStudio2005SecondTheme();

		//-------------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the pane.
		//-------------------------------------------------------------------------
		void RefreshMetrics();

		//-----------------------------------------------------------------------
		// Summary:
		//     Draws the background of the pane caption.
		// Parameters:
		//     dc        - Reference to the device context in which to draw.
		//     pPane     - Container of the caption needs to draw the background.
		//     rcCaption - Caption bounding rectangle
		//     strTitle  - Caption text.
		//     bActive   - TRUE if caption is active.
		//     bVertical - TRUE to draw caption vertically
		//-----------------------------------------------------------------------
		virtual COLORREF FillCaptionPart(CDC& dc, CXTPDockingPaneBase* pPane, CRect rcCaption, BOOL bActive, BOOL bVertical);

	protected:
		COLORREF m_clrCaptionBorder;    //Color used to draw the border of the pane caption.
	};

	//===========================================================================
	// Summary:
	//     CXTPDockingPaneVisualStudio2005SecondTheme is CXTPDockingPaneVisualStudio2005SecondTheme derived class, represents
	//     Visual Studio 2005 theme for docking panes.
	// Remarks:
	//     Call CXTPDockingPaneManager::SetTheme(xtpPaneThemeWhidbey); to set this theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPDockingPaneVisualStudio2005Theme : public CXTPDockingPaneVisualStudio2005SecondTheme
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPDockingPaneVisualStudio2005Theme object.
		//-------------------------------------------------------------------------
		CXTPDockingPaneVisualStudio2005Theme();
	};
}

using namespace XTPDockingPanePaintThemes;

AFX_INLINE int CXTPDockingPanePaintManager::GetCaptionHeight() const {
	return m_nTitleHeight;
}
AFX_INLINE int CXTPDockingPanePaintManager::GetTabsHeight() const{
	return m_nTabsHeight;
}

AFX_INLINE CXTPTabPaintManager* CXTPDockingPanePaintManager::GetTabPaintManager() const {
	return m_pTabPaintManager;
}
AFX_INLINE CXTPTabPaintManager* CXTPDockingPanePaintManager::GetPanelPaintManager() const {
	return m_pPanelPaintManager;
}
AFX_INLINE void CXTPDockingPanePaintManager::DrawFloatingFrame(CDC& /*dc*/, CXTPDockingPaneMiniWnd* /*pPane*/, CRect /*rc*/) {
}

AFX_INLINE void CXTPDockingPanePaintManager::SetCaptionButtonStyle(XTPDockingPaneCaptionButtonStyle nStyle) {
	m_nCaptionButtonStyle = nStyle;
}
AFX_INLINE XTPDockingPaneCaptionButtonStyle CXTPDockingPanePaintManager::GetCaptionButtonStyle() const {
	return m_nCaptionButtonStyle;
}


#endif // #if !defined(__XTPDOCKINGPANEPAINTMANAGER_H__)
