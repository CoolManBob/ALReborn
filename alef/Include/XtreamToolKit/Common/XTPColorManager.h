// XTPColorManager.h: interface for the CXTPColorManager class.
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
#if !defined(__XTPCOLORMANAGER_H__)
#define __XTPCOLORMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//-----------------------------------------------------------------------
// Summary:
//     Enumeration used to determine the current Windows XP theme in use
// See Also:
//     CXTPColorManager::GetCurrentSystemTheme
// Example:
//     <code>XTPCurrentSystemTheme systemTheme = XTPColorManager()->GetCurrentSystemTheme();</code>
//
// <KEYWORDS xtpSystemThemeUnknown, xtpSystemThemeBlue, xtpSystemThemeOlive, xtpSystemThemeSilver, xtpSystemThemeAuto, xtpSystemThemeDefault>
//-----------------------------------------------------------------------
enum XTPCurrentSystemTheme
{
	xtpSystemThemeUnknown,      // No known theme in use
	xtpSystemThemeBlue,         // Blue theme in use
	xtpSystemThemeOlive,        // Olive theme in use
	xtpSystemThemeSilver,       // Silver theme in use
	xtpSystemThemeAuto,         // Use OS theme.
	xtpSystemThemeDefault       // Default theme (xtpSystemThemeAuto)
};


//{{AFX_CODEJOCK_PRIVATE
#ifndef COLOR_GRADIENTACTIVECAPTION
#define COLOR_GRADIENTACTIVECAPTION           27
#endif

#ifndef COLOR_GRADIENTINACTIVECAPTION
#define COLOR_GRADIENTINACTIVECAPTION         28
#endif

#define COLORREF_NULL   (COLORREF)-1
//}}AFX_CODEJOCK_PRIVATE

const UINT XPCOLOR_BASE                     = 30; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_TOOLBAR_FACE             = XPCOLOR_BASE; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_HIGHLIGHT                = 31; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_HIGHLIGHT_BORDER         = 32; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_HIGHLIGHT_PUSHED         = 33; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_HIGHLIGHT_CHECKED        = 36; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_HIGHLIGHT_CHECKED_BORDER = 37; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_ICONSHADDOW              = 34; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_MENUBAR_GRAYTEXT         = 35; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_GRAYTEXT                 = XPCOLOR_MENUBAR_GRAYTEXT; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_TOOLBAR_GRIPPER          = 38; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_SEPARATOR                = 39; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_DISABLED                 = 40; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_MENUBAR_FACE             = 41; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_MENUBAR_EXPANDED         = 42; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_MENUBAR_BORDER           = 43; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_MENUBAR_TEXT             = 44; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_HIGHLIGHT_TEXT           = 45; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_TOOLBAR_TEXT             = 46; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_PUSHED_TEXT              = 47; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_TAB_INACTIVE_BACK        = 48; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_TAB_INACTIVE_TEXT        = 49; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_HIGHLIGHT_PUSHED_BORDER  = 50; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_CHECKED_TEXT             = XPCOLOR_HIGHLIGHT_TEXT; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_3DFACE                   = 51; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_3DSHADOW                 = 52; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_EDITCTRLBORDER           = 53; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_FRAME                    = 54; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_SPLITTER_FACE            = 55; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_LABEL                    = 56; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_STATICFRAME              = 57; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_HIGHLIGHT_DISABLED_BORDER = 58; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_SHADOW_FACTOR            = 59; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_TOOLBAR_GRAYTEXT         = 60; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_FLOATBAR_BORDER          = 61; //<ALIAS CXTPColorManager::GetColor>
const UINT XPCOLOR_LAST                     = 62; //<ALIAS CXTPColorManager::GetColor>

//===========================================================================
// Summary:
//     The CXTPPaintManagerColor class is used to manage COLORREF values for
//     determining default and application defined colors.
//===========================================================================
class _XTP_EXT_CLASS CXTPPaintManagerColor
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPaintManagerColor object and initializes default
	//     and application defined colors for the class
	// Parameters:
	//     clr - n RGB value that represents default color to set.
	// See Also:
	//     SetStandardValue, SetCustomValue
	//-----------------------------------------------------------------------
	CXTPPaintManagerColor();
	CXTPPaintManagerColor(COLORREF clr); // <combine CXTPPaintManagerColor::CXTPPaintManagerColor>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines if application defined colors are
	//     used.
	// Returns:
	//     TRUE if application defined colors are used, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsDefaultValue() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines if the standard color is set and
	//     used.
	// Returns:
	//     TRUE if standard color is set and custom color is not set,
	//     otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsStandardValue() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines if the custom color is set and
	//     used.
	// Returns:
	//     TRUE if custom color is set, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsCustomValue() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will retrieves the default color for the
	//     CXTPPaintManagerColor class.
	// Returns:
	//     An RGB value that represents the default color.
	//-----------------------------------------------------------------------
	COLORREF GetStandardColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will sets the application defined color for
	//     the CXTPPaintManagerColor class.
	// Parameters:
	//     clr - n RGB value that represents application defined color to set.
	//-----------------------------------------------------------------------
	void SetCustomValue(COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will sets the default color for
	//     the CXTPPaintManagerColor class.
	// Parameters:
	//     clr - n RGB value that represents default color to set.
	//-----------------------------------------------------------------------
	void SetStandardValue(COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will remove the application defined color (if any)
	//     that was defined for the CXTPPaintManagerColor class.
	//-----------------------------------------------------------------------
	void SetDefaultValue();

	//-----------------------------------------------------------------------
	// Summary:
	//     Operator used to retrieve an RGB color value that represents
	//     the current color defined for the CXTPPaintManagerColor class.
	// Returns:
	//     An RGB value that represents the current color.
	//-----------------------------------------------------------------------
	operator COLORREF() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator assigns a new COLORREF value for the application defined color
	// Parameters:
	//     clr    - RGB value that represents application defined color to set.
	//     refclr - Reference to CXTPPaintManagerColor value that represents application defined color to set.
	// See Also:
	//     SetStandardValue, SetCustomValue
	//-----------------------------------------------------------------------
	const CXTPPaintManagerColor& operator=(COLORREF clr);
	const CXTPPaintManagerColor& operator=(const CXTPPaintManagerColor& refclr); // <combine CXTPPaintManagerColor::=@COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes data from or to an archive.
	// Parameters:
	//     ar - A CArchive object to serialize to or from.
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar);
protected:
	COLORREF m_clrStandardValue;   // Standard color
	COLORREF m_clrCustomValue;     // Application defined color
};

//---------------------------------------------------------------------------

AFX_INLINE BOOL CXTPPaintManagerColor::IsDefaultValue() const {
	return (m_clrCustomValue == COLORREF_NULL) && (m_clrStandardValue == COLORREF_NULL);
}
AFX_INLINE BOOL CXTPPaintManagerColor::IsStandardValue() const {
	return (m_clrCustomValue == COLORREF_NULL) && (m_clrStandardValue != COLORREF_NULL);
}
AFX_INLINE BOOL CXTPPaintManagerColor::IsCustomValue() const {
	return m_clrCustomValue != COLORREF_NULL;
}
AFX_INLINE COLORREF CXTPPaintManagerColor::GetStandardColor() const {
	return m_clrStandardValue;
}
AFX_INLINE void CXTPPaintManagerColor::SetCustomValue(COLORREF clr) {
	m_clrCustomValue = clr;
}
AFX_INLINE void CXTPPaintManagerColor::SetStandardValue(COLORREF clr) {
	m_clrStandardValue = clr;
}
AFX_INLINE void CXTPPaintManagerColor::SetDefaultValue() {
	m_clrCustomValue = COLORREF_NULL;
}
AFX_INLINE CXTPPaintManagerColor::operator COLORREF() const{
	return  (m_clrCustomValue == COLORREF_NULL) ? m_clrStandardValue : m_clrCustomValue;
}
AFX_INLINE const CXTPPaintManagerColor& CXTPPaintManagerColor::operator=(COLORREF clr) {
	SetCustomValue(clr); return *this;
}
AFX_INLINE const CXTPPaintManagerColor& CXTPPaintManagerColor::operator=(const CXTPPaintManagerColor& clr) {
	m_clrCustomValue = clr.m_clrCustomValue;
	m_clrStandardValue = clr.m_clrStandardValue;
	return *this;
}

//===========================================================================
// Summary:
//     The CXTPPaintManagerColorGradient class is used to manage COLORREF values
//     for determining light and dark gradient colors.
// See Also: CXTPPaintManagerColor
//===========================================================================
class _XTP_EXT_CLASS CXTPPaintManagerColorGradient
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPaintManagerColorGradient object and initializes
	//     default dark and light colors equal to the specified COLORREF value.
	// Parameters:
	//     clr - An RGB COLORREF value representing the dark and light colors.
	//     clrLight - An RGB COLORREF value representing the light color.
	//     clrDark - An RGB COLORREF value representing the dark color.
	// See Also:
	//     SetStandardValue, SetCustomValue
	//-----------------------------------------------------------------------
	CXTPPaintManagerColorGradient();
	CXTPPaintManagerColorGradient(const COLORREF clr); // <combine CXTPPaintManagerColorGradient::CXTPPaintManagerColorGradient>
	CXTPPaintManagerColorGradient(const CXTPPaintManagerColor clr); // <combine CXTPPaintManagerColorGradient::CXTPPaintManagerColorGradient>
	CXTPPaintManagerColorGradient(COLORREF clrLight, COLORREF clrDark); // <combine CXTPPaintManagerColorGradient::CXTPPaintManagerColorGradient>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function sets the value of the default
	//     dark and light colors equal to the specified COLORREF value.
	// Parameters:
	//     clr      - An RGB COLORREF value representing the dark and light colors.
	//     clrref   - An reference to  CXTPPaintManagerColorGradient value
	//                representing the dark and light colors.
	//     clrLight - An RGB COLORREF value representing the light color.
	//     clrDark  - An RGB COLORREF value representing the dark color.
	//     fGradientFactor - Gradient divider factor
	//-----------------------------------------------------------------------
	void SetStandardValue(const COLORREF clr);
	void SetStandardValue(CXTPPaintManagerColorGradient& clrref); // <combine CXTPPaintManagerColorGradient::SetStandardValue@const COLORREF>
	void SetStandardValue(COLORREF clrLight, COLORREF clrDark, float fGradientFactor = 0.5f); // <combine CXTPPaintManagerColorGradient::SetStandardValue@const COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will sets the application defined 4 color gradient values for
	//     the CXTPPaintManagerColorGradient class.
	// Parameters:
	//     clr      - An RGB COLORREF value representing the dark and light colors.
	//     clrref   - An reference to  CXTPPaintManagerColorGradient value
	//                representing the dark and light colors.
	//     clrLight - An RGB COLORREF value representing the light color.
	//     clrDark  - An RGB COLORREF value representing the dark color.
	//-----------------------------------------------------------------------
	void SetCustomValue(const COLORREF clr);
	void SetCustomValue(CXTPPaintManagerColorGradient& clrref); // <combine CXTPPaintManagerColorGradient::SetCustomValue@const COLORREF>
	void SetCustomValue(COLORREF clrLight, COLORREF clrDark); // <combine CXTPPaintManagerColorGradient::SetCustomValue@const COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//     Operator used to retrieve an RGB color value that represents
	//     the dark gradient color used for the CXTPPaintManagerColorGradient class.
	// Returns:
	//     An RGB value that represents the dark gradient color.
	//-----------------------------------------------------------------------
	operator COLORREF () const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves a reference to the CXTPPaintManagerColor dark gradient color
	//     member.
	// Returns:
	//     A reference to the CXTPPaintManagerColor dark gradient color used
	//     for the CXTPPaintManagerColorGradient class.
	//-----------------------------------------------------------------------
	operator CXTPPaintManagerColor& ();

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator copy all values from the specified source object;
	// Parameters:  //
	//     refSrc - Reference to CXTPPaintManagerColorGradient source object.
	// See Also:
	//     SetStandardValue, SetCustomValue
	//-----------------------------------------------------------------------
	const CXTPPaintManagerColorGradient& operator=(const CXTPPaintManagerColorGradient& refSrc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the CXTPPaintManagerColorGradient object's colors
	//     have been initialized.
	// Returns:
	//     TRUE if both light and dark gradient colors have not been initialized,
	//     otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsNull() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes data from or to an archive.
	// Parameters:
	//     ar - A CArchive object to serialize to or from.
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar);

public:
	CXTPPaintManagerColor clrLight;     // Light color.
	CXTPPaintManagerColor clrDark;      // Dark color.
	float fGradientFactor;              // Gradient divider factor
};

//---------------------------------------------------------------------------

AFX_INLINE CXTPPaintManagerColorGradient::operator COLORREF () const {
	return clrDark;
}
AFX_INLINE CXTPPaintManagerColorGradient::operator CXTPPaintManagerColor& () {
	return clrDark;
}
AFX_INLINE BOOL CXTPPaintManagerColorGradient::IsNull() const {
	return ((clrDark == COLORREF_NULL) && (clrLight == COLORREF_NULL));
}
AFX_INLINE const CXTPPaintManagerColorGradient& CXTPPaintManagerColorGradient::operator=(const CXTPPaintManagerColorGradient& refSrc) {
	clrLight = refSrc.clrLight;
	clrDark = refSrc.clrDark;
	fGradientFactor = refSrc.fGradientFactor;
	return *this;
}

//===========================================================================
// Summary:
//     CXTPColorManager is a stand alone class. This object is used for manipulation of
//     RGB values.
// See Also: XTPColorManager
//===========================================================================
class _XTP_EXT_CLASS CXTPColorManager : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Application-defined callback function used with the SetGetSysColorPtr function.
	// See Also: SetGetSysColorPtr
	//-----------------------------------------------------------------------
	typedef DWORD (__stdcall* PFNGETSYSCOLOR)(int nIndex);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This function will refresh the display element colors defined
	//     in the CXTPColorManager object.
	//-----------------------------------------------------------------------
	void RefreshColors();

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
	//  XPCOLOR_MENUBAR_FACE              41            XP menu item57- text background color.
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
	COLORREF GetColor(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to get the Luna color specified by nIndes.
	// Parameters:
	//     nIndex - Specifies the display element whose color is to be
	//              retrieved.
	// Returns:
	//      An RGB color value representing the Luna color.
	//-----------------------------------------------------------------------
	COLORREF GetLunaColor(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to blend the light and dark colors
	//     by the amount specified in nDelta.
	// Parameters:
	//     clrLight - RGB light color
	//     clrDark  - RGB dark color
	//     nDelta   - Amount saturation to blend colors.
	// Returns:
	//     An RGB color value representing the two blended colors.
	//-----------------------------------------------------------------------
	COLORREF LightColor(COLORREF clrLight, COLORREF clrDark, int nDelta) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to disable the luna (Blue, Olive
	//     and Green) colors for Office 2003 theme for Windows XP and
	//     later operating systems.
	// Parameters:
	//     bDisable - TRUE to disable luna colors, otherwise FALSE.
	//-----------------------------------------------------------------------
	void DisableLunaColors(BOOL bDisable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if Luna colors for Office 2003 style is disabled.
	// Returns:
	//     TRUE if the luna colors are disabled, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsLunaColorsDisabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the color for the specified index.
	//     See GetColor for a complete list of index values.
	// Parameters:
	//     nIndex - Color index.
	//     clrValue - RGB color value to set.
	// See Also: SetColors
	//-----------------------------------------------------------------------
	void SetColor(int nIndex, COLORREF clrValue);

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
	void SetColors(int cElements, const int* lpaElements, const COLORREF* lpaRgbValues);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the Office 2003 Luna colors for each element to correspond with
	//     the current Windows XP visual style.
	// See Also: SetColor, SetColors
	//-----------------------------------------------------------------------
	void RefreshLunaColors();

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
	//-----------------------------------------------------------------------
	XTPCurrentSystemTheme GetWinThemeWrapperTheme() const;

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
	//     This method replace standard GetSysColor to application defined.
	// Parameters:
	//     pfnGetSysColor - Pointer to the PFNGETSYSCOLOR procedure
	//-----------------------------------------------------------------------
	void SetGetSysColorPtr(PFNGETSYSCOLOR pfnGetSysColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the system display
	//     is set to low resolution.
	// Parameters:
	//     hDC - Handle to a device context, if NULL the desktop window
	//           device context is used.
	// Returns:
	//     TRUE if the system display is set to low resolution,
	//     otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsLowResolution(HDC hDC = 0);

private:
	CXTPColorManager();
	void RefreshSysColors();
	void RefreshXPColors();
	void RefreshGradientColors();
	float ColorWidth(int nLength, int nWidth);
	float ColorDelta(COLORREF clrA, COLORREF clrB);
	float Length(COLORREF clrA, COLORREF clrB);
	BOOL LongColor(COLORREF clrMain, COLORREF clrSub, BOOL bCalcLength, float fDistance);
	COLORREF MixColor(COLORREF clrMain, COLORREF clrSub, float fDistance);
	COLORREF AdjustColor(COLORREF clrMain, COLORREF clrSub, float fDistance);
	double GetRDelta(COLORREF clr);
	double GetGDelta(COLORREF clr);
	double GetBDelta(COLORREF clr);
	XTPCurrentSystemTheme _GetWinThemeWrapperTheme();

	// singleton, instantiate on demand.
	static CXTPColorManager& AFX_CDECL Instance();

public:
	CXTPPaintManagerColorGradient grcCaption;            // Office 2003 Caption gradient color.
	CXTPPaintManagerColorGradient grcDockBar;            // Office 2003 Dockbar gradient color.
	CXTPPaintManagerColorGradient grcShortcutBarGripper; // Office 2003 Shortcut bar gripper.

	CXTPPaintManagerColorGradient grcToolBar;            // Office 2003 ToolBar gradient color.
	CXTPPaintManagerColorGradient grcMenu;               // Office 2003 Menu sidebar gradient color.
	CXTPPaintManagerColorGradient grcMenuExpanded;       // Office 2003 Expanded menu sidebar gradient color.
	CXTPPaintManagerColorGradient grcMenuItemPopup;      // Office 2003 Menu item popup gradient color.

	CXTPPaintManagerColorGradient grcLunaChecked;        // Office 2003 checked button gradient color.
	CXTPPaintManagerColorGradient grcLunaPushed;         // Office 2003 pushed button gradient color.
	CXTPPaintManagerColorGradient grcLunaSelected;       // Office 2003 selected button gradient color.

	BOOL m_bEnableLunaBlueForRoyaleTheme;                // TRUE if Luna Blue color will be used if the current system theme is set to Royale.

private:
	BOOL m_bDisableLunaColors;
	COLORREF m_arrStandardColor[XPCOLOR_LAST + 1];
	COLORREF m_arrCustomColor[XPCOLOR_LAST + 1];
	COLORREF m_arrLunaColor[XPCOLOR_LAST + 1];
	PFNGETSYSCOLOR m_pfnGetSysColor;
	XTPCurrentSystemTheme m_systemTheme;
	XTPCurrentSystemTheme m_winThemeWrapperTheme;


	friend CXTPColorManager* XTPColorManager();
};


//---------------------------------------------------------------------------
// Summary:
//     Call this function to access CXTPColorManager members.
//     Since this class is designed as a single instance object you can
//     only access version info through this method. You <b>cannot</b>
//     directly instantiate an object of type CXTPColorManager.
// Example:
// <code>
// COLORREF cr = XTPColorManager()->GetColor(nIndex);
// </code>
//---------------------------------------------------------------------------
CXTPColorManager* XTPColorManager();

//---------------------------------------------------------------------------
// Summary:
//     Call this member as a convenient way to call
//     XTPColorManager()->GetColor, see CXTPColorManager::GetColor for
//     more details.
// Parameters:
//     nIndex - Specifies the display element whose color is to be
//              retrieved.
// Returns:
//     The red, green, blue (RGB) color value of the given element.
// Example:
// <code>
// if (eState == xtMouseNormal)
// {
//     pDC->Draw3dRect(rcItem,
//         GetXtremeColor(COLOR_3DFACE),
//         GetXtremeColor(COLOR_3DFACE));
// }
// </code>
// See Also:
//     CXTPColorManager::GetColor
//---------------------------------------------------------------------------
COLORREF GetXtremeColor(UINT nIndex);

//-----------------------------------------------------------------------
// Summary:
//      Call this function to get the MS Office color represented by nIndex.
// Parameters:
//     nIndex - Specifies the display element whose color is to be
//              retrieved.
// Returns:
//      An RGB color value representing the color value of the MS Office color.
// Example:
// <code>
// switch (XTPColorManager()->GetCurrentSystemTheme())
// {
//      case xtpSystemThemeBlue:
//      case xtpSystemThemeOlive:
//      case xtpSystemThemeSilver:
//           m_clr3DFacePushed.SetStandardValue(GetMSO2003Color(XPCOLOR_HIGHLIGHT_PUSHED));
//           m_clr3DFaceHilite.SetStandardValue(GetMSO2003Color(XPCOLOR_HIGHLIGHT));
//           m_clrBtnTextPushed.SetStandardValue(GetMSO2003Color(XPCOLOR_PUSHED_TEXT));
//           m_clrBtnTextHilite.SetStandardValue(GetMSO2003Color(XPCOLOR_HIGHLIGHT_TEXT));
//           m_clrBorder3DShadow.SetStandardValue(GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER));
//           m_clrBorder3DHilite.SetStandardValue(GetMSO2003Color(XPCOLOR_HIGHLIGHT_BORDER));
//           break;
// }
// </code>
// See Also:
//      CXTPColorManager::GetColor
//-----------------------------------------------------------------------
COLORREF GetMSO2003Color(UINT nIndex);

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE CXTPColorManager* XTPColorManager() {
	return &CXTPColorManager::Instance();
}
AFX_INLINE COLORREF GetXtremeColor(UINT nIndex) {
	return XTPColorManager()->GetColor(nIndex);
}
AFX_INLINE COLORREF GetMSO2003Color(UINT nIndex) {
	return XTPColorManager()->GetLunaColor(nIndex);
}

//-----------------------------------------------------------------------
// Summary:
//     This global function will refresh the display element
//     colors defined in the CXTPColorManager class.
// See Also:
//     CXTPColorManager::RefreshColors
//-----------------------------------------------------------------------
_XTP_EXT_CLASS void AFX_CDECL RefreshXtremeColors();

#endif // !defined(__XTPCOLORMANAGER_H__)
