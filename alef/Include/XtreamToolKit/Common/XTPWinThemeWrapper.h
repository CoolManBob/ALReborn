// XTPWinThemeWrapper.h: interface for the CXTPWinThemeWrapper class.
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
#if !defined(__XTPWINTHEMEWRAPPER_H__)
#define __XTPWINTHEMEWRAPPER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "uxtheme.h"
#include "schemadef.h"
#include "tmschema.h"

typedef HANDLE HPAINTBUFFER;

// BP_BUFFERFORMAT
enum XTP_UX_BUFFERFORMAT
{
	XTP_BPBF_COMPATIBLEBITMAP,    // Compatible bitmap
	XTP_BPBF_DIB,                 // Device-independent bitmap
	XTP_BPBF_TOPDOWNDIB,          // Top-down device-independent bitmap
	XTP_BPBF_TOPDOWNMONODIB       // Top-down monochrome device-independent bitmap
};

// BP_PAINTPARAMS
struct XTP_UX_PAINTPARAMS
{
	DWORD                       cbSize;
	DWORD                       dwFlags; // BPPF_ flags
	const RECT *                prcExclude;
	const BLENDFUNCTION *       pBlendFunction;
};


struct XTP_UX_DTTOPTS
{
	DWORD             dwSize;              // size of the struct
	DWORD             dwFlags;             // which options have been specified
	COLORREF          crText;              // color to use for text fill
	COLORREF          crBorder;            // color to use for text outline
	COLORREF          crShadow;            // color to use for text shadow
	int               iTextShadowType;     // TST_SINGLE or TST_CONTINUOUS
	POINT             ptShadowOffset;      // where shadow is drawn (relative to text)
	int               iBorderSize;         // Border radius around text
	int               iFontPropId;         // Font property to use for the text instead of TMT_FONT
	int               iColorPropId;        // Color property to use for the text instead of TMT_TEXTCOLOR
	int               iStateId;            // Alternate state id
	BOOL              fApplyOverlay;       // Overlay text on top of any text effect?
	int               iGlowSize;           // Glow radius around text
	VOID*             pfnDrawTextCallback; // Callback for DrawText
	LPARAM            lParam;              // Parameter for callback
};

struct _DTBGOPTS;
typedef _DTBGOPTS* PDTBGOPTS;

#define TYPEDEF_WRAPPER_POINTER(val, type, proc)\
	const int xtpWrapper##proc = val;\
	const LPCSTR xtpWrapperProc##proc = #proc;\
	typedef type (__stdcall* PFN##proc)

TYPEDEF_WRAPPER_POINTER(0, HRESULT, DrawThemeBackground)(HTHEME, HDC, int, int, const RECT*, const RECT*);
TYPEDEF_WRAPPER_POINTER(1, HRESULT, CloseThemeData)(HTHEME);
TYPEDEF_WRAPPER_POINTER(2, HRESULT, SetWindowTheme)(HWND, LPCWSTR, LPCWSTR);
TYPEDEF_WRAPPER_POINTER(3, void, SetThemeAppProperties)(DWORD);
TYPEDEF_WRAPPER_POINTER(4, HTHEME, OpenThemeData)(HWND, LPCWSTR);
TYPEDEF_WRAPPER_POINTER(5, BOOL, IsThemePartDefined)(HTHEME, int, int);
TYPEDEF_WRAPPER_POINTER(6, BOOL, IsThemeDialogTextureEnabled)();
TYPEDEF_WRAPPER_POINTER(7, BOOL, IsThemeBackgroundPartiallyTransparent)(HTHEME, int, int);
TYPEDEF_WRAPPER_POINTER(8, BOOL, IsThemeActive)();
TYPEDEF_WRAPPER_POINTER(9, BOOL, IsAppThemed)();
TYPEDEF_WRAPPER_POINTER(10, HRESULT, HitTestThemeBackground)(HTHEME, HDC, int, int, DWORD, const RECT*, HRGN, POINT, WORD*);
TYPEDEF_WRAPPER_POINTER(11, HTHEME, GetWindowTheme)(HWND);
TYPEDEF_WRAPPER_POINTER(12, HRESULT, GetThemeSysString)(HTHEME, int, LPWSTR, int);
TYPEDEF_WRAPPER_POINTER(13, HRESULT, GetThemeTextMetrics)(HTHEME, HDC, int, int, TEXTMETRIC*);
TYPEDEF_WRAPPER_POINTER(14, HRESULT, GetThemeTextExtent)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, const RECT*, RECT*);
TYPEDEF_WRAPPER_POINTER(15, int, GetThemeSysSize)(HTHEME, int);
TYPEDEF_WRAPPER_POINTER(16, HRESULT, GetThemeSysInt)(HTHEME, int, int*);
TYPEDEF_WRAPPER_POINTER(17, HRESULT, GetThemeSysFont)(HTHEME, int, LOGFONT*);
TYPEDEF_WRAPPER_POINTER(18, HBRUSH, GetThemeSysColorBrush)(HTHEME, int);
TYPEDEF_WRAPPER_POINTER(19, COLORREF, GetThemeSysColor)(HTHEME, int);
TYPEDEF_WRAPPER_POINTER(20, BOOL, GetThemeSysBool)(HTHEME, int);
TYPEDEF_WRAPPER_POINTER(21, HRESULT, GetThemeString)(HTHEME, int, int, int, LPWSTR, int);
TYPEDEF_WRAPPER_POINTER(22, HRESULT, GetThemeRect)(HTHEME, int, int, int, RECT*);
TYPEDEF_WRAPPER_POINTER(23, HRESULT, GetThemePropertyOrigin)(HTHEME, int, int, int, PROPERTYORIGIN*);
TYPEDEF_WRAPPER_POINTER(24, HRESULT, GetThemePosition)(HTHEME, int, int, int, POINT*);
TYPEDEF_WRAPPER_POINTER(25, HRESULT, GetThemePartSize)(HTHEME, HDC, int, int, RECT*, THEMESIZE, SIZE*);
TYPEDEF_WRAPPER_POINTER(26, HRESULT, GetThemeMetric)(HTHEME, HDC, int, int, int, int*);
TYPEDEF_WRAPPER_POINTER(27, HRESULT, GetThemeMargins)(HTHEME, HDC, int, int, int, RECT*, MARGINS*);
TYPEDEF_WRAPPER_POINTER(28, HRESULT, GetThemeIntList)(HTHEME, int, int, int, INTLIST*);
TYPEDEF_WRAPPER_POINTER(29, HRESULT, GetThemeInt)(HTHEME, int, int, int, int*);
TYPEDEF_WRAPPER_POINTER(30, HRESULT, GetThemeFont)(HTHEME, HDC, int, int, int, LOGFONT*);
TYPEDEF_WRAPPER_POINTER(31, HRESULT, GetThemeFilename)(HTHEME, int, int, int, LPWSTR, int);
TYPEDEF_WRAPPER_POINTER(32, HRESULT, GetThemeEnumValue)(HTHEME, int, int, int, int*);
TYPEDEF_WRAPPER_POINTER(33, HRESULT, GetThemeDocumentationProperty)(LPCWSTR, LPCWSTR, LPWSTR, int);
TYPEDEF_WRAPPER_POINTER(34, HRESULT, GetThemeColor)(HTHEME, int, int, int, COLORREF*);
TYPEDEF_WRAPPER_POINTER(35, HRESULT, GetThemeBool)(HTHEME, int, int, int, BOOL*);
TYPEDEF_WRAPPER_POINTER(36, HRESULT, GetThemeBackgroundRegion)(HTHEME, HDC, int, int, const RECT*, HRGN*);
TYPEDEF_WRAPPER_POINTER(37, HRESULT, GetThemeBackgroundExtent)(HTHEME, HDC, int, int, const RECT*, RECT*);
TYPEDEF_WRAPPER_POINTER(38, HRESULT, GetThemeBackgroundContentRect)(HTHEME, HDC, int, int, const RECT*, RECT*);
TYPEDEF_WRAPPER_POINTER(39, DWORD, ThemeAppProperties)();
TYPEDEF_WRAPPER_POINTER(40, HRESULT, GetCurrentThemeName)(LPWSTR, int, LPWSTR, int, LPWSTR, int);
TYPEDEF_WRAPPER_POINTER(41, HRESULT, EnableTheming)(BOOL);
TYPEDEF_WRAPPER_POINTER(42, HRESULT, EnableThemeDialogTexture)(HWND, DWORD);
TYPEDEF_WRAPPER_POINTER(43, HRESULT, DrawThemeText)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, DWORD, const RECT*);
TYPEDEF_WRAPPER_POINTER(44, HRESULT, DrawThemeParentBackground)(HWND, HDC, RECT*);
TYPEDEF_WRAPPER_POINTER(45, HRESULT, DrawThemeIcon)(HTHEME, HDC, int, int, const RECT*, HIMAGELIST, int);
TYPEDEF_WRAPPER_POINTER(46, HRESULT, DrawThemeEdge)(HTHEME, HDC, int, int, const RECT*, UINT, UINT, RECT*);
TYPEDEF_WRAPPER_POINTER(47, HRESULT, DrawThemeBackgroundEx)(HTHEME, HDC, int, int, const RECT*, const PDTBGOPTS);
TYPEDEF_WRAPPER_POINTER(48, DWORD, GetThemeAppProperties)();

TYPEDEF_WRAPPER_POINTER(49, HPAINTBUFFER, BeginBufferedPaint)(HDC, const RECT*, XTP_UX_BUFFERFORMAT, XTP_UX_PAINTPARAMS*, HDC*);
TYPEDEF_WRAPPER_POINTER(50, HRESULT, EndBufferedPaint)(HPAINTBUFFER, BOOL);
TYPEDEF_WRAPPER_POINTER(51, HRESULT, DrawThemeTextEx)(HTHEME, HDC, int, int, LPCWSTR, int, DWORD, LPRECT, const XTP_UX_DTTOPTS*);


#define XTP_UXTHEME_HANDLE (HMODULE)0x21FFFF1

//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     The CXTPWinThemeWrapper class wraps the UxTheme.dll visual theme
//     API.  You can use this class for rendering Windows XP themes for
//     owner drawn controls.
//===========================================================================
class _XTP_EXT_CLASS CXTPWinThemeWrapper
{
private:
	//-----------------------------------------------------------------------
	// Summary:
	//     Shared data of wrappers
	//-----------------------------------------------------------------------
	class CSharedData
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CSharedData object.
		//-------------------------------------------------------------------------
		CSharedData();

	public:
		LPVOID m_ptrWrappers[52];   // Wrapper pointer
		HMODULE m_hThemeDll;        // Handle to the theme dll.
		DWORD m_dwComCtlVersion;    // ComCtl version.
	};

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPWinThemeWrapper object.
	//-----------------------------------------------------------------------
	CXTPWinThemeWrapper();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPWinThemeWrapper object, handles cleanup and
	//     deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPWinThemeWrapper();

	//-----------------------------------------------------------------------
	// Summary:
	//     Open the theme data for the specified HWND and semi-colon
	//     separated list of class names.  OpenThemeData() will try
	//     each class name, one at a time, and use the first matching
	//     theme info found.
	// Parameters:
	//     hWnd         - Window handle of the control/window to be
	//                    themed.
	//     pszClassList - Class name (or list of names) to match to
	//                    theme data section.  if the list contains
	//                    more than one name, the names are tested one
	//                    at a time for a match.  If a match is found,
	//                    OpenThemeData() returns a theme handle
	//                    associated with the matching class.  This
	//                    param is a list (instead of just a single
	//                    class name) to provide the class an
	//                    opportunity to get the "best" match between
	//                    the class and the current theme.  For example,
	//                    a button might pass L"OkButton, Button" if its
	//                    ID=ID_OK.  If the current theme has an entry
	//                    for OkButton, that will be used.  Otherwise,
	//                    we fall back on the normal Button entry.
	//-----------------------------------------------------------------------
	void OpenThemeData(HWND hWnd, LPCWSTR pszClassList);

	//-----------------------------------------------------------------------
	// Summary:
	//     Closes the theme data handle.  CXTPWinThemeWrapper will do
	//     this automatically when destroyed or when OpenThemeData is
	//     called.  Your window should handle the WM_THEMECHANGED
	//     message when called followed by a call to OpenThemeData.
	//-----------------------------------------------------------------------
	void CloseThemeData();

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the theme-specified border and fill for the "iPartId"
	//     and "iStateId".  This could be based on a bitmap file, a
	//     border and fill, or other image description.
	// Parameters:
	//     hDC       - HDC to draw into
	//     iPartID   - Part number to draw
	//     iStateID  - State number (of the part) to draw
	//     pRect     - Defines the size/location of the part
	//     pClipRect - Optional clipping rect (don't draw outside it)
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeBackground(HDC hDC, int iPartID, int iStateID, const RECT *pRect, const RECT* pClipRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the text using the theme-specified color and font for
	//     the "iPartID" and "iStateID".
	// Parameters:
	//     hDC          - HDC to draw into
	//     iPartID      - Part number to draw
	//     iStateID     - State number (of the part) to draw
	//     pszText      - Actual text to draw
	//     iCharCount   - Number of chars to draw (-1 for all)
	//     dwTextFlags  - Same as DrawText() "uFormat" param
	//     dwTextFlags2 - Additional drawing options
	//     pRect        - Defines the size/location of the part
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeText(HDC hDC, int iPartID, int iStateID, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT* pRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the size of the content for the theme-defined
	//     background.  This is usually the area inside the borders or
	//     Margins.
	// Parameters:
	//     hDC           - (optional) device content to be used for
	//                     drawing
	//     iPartID       - Part number to draw
	//     iStateID      - State number (of the part) to draw
	//     pBoundingRect - The outer RECT of the part being drawn
	//     pContentRect  - RECT to receive the content area
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeBackgroundContentRect(HDC hDC, int iPartID, int iStateID, const RECT *pBoundingRect, RECT* pContentRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates the size/location of the theme-specified
	//     background based on the "pContentRect".
	// Parameters:
	//     hDC           - (optional) device content to be used for
	//                     drawing
	//     iPartID       - Part number to draw
	//     iStateID      - State number (of the part) to draw
	//     pContentRect  - RECT that defines the content area
	//     pExtentRect   - RECT to receive the overall size/location of
	//                     part
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeBackgroundExtent(HDC hDC, int iPartID, int iStateID, const RECT *pContentRect, RECT* pExtentRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the theme-specified border and fill for the "iPartId"
	//     and "iStateId".  This could be based on a bitmap file, a
	//     border and fill, or other image description.
	// Parameters:
	//     hDC      - HDC to draw into
	//     iPartID  - Part number to draw
	//     iStateID - State number (of the part) to draw
	//     pRect    - Defines the size/location of the part
	//     pOptions - Pointer to optional parameters.
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeBackgroundEx(HDC hDC, int iPartID, int iStateID, const RECT *pRect, const PDTBGOPTS pOptions);

	//-----------------------------------------------------------------------
	// Summary:
	//     Similar to the DrawEdge() API, but uses part colors and is
	//     high-DPI aware
	// Parameters:
	//     hDC          - HDC to draw into
	//     iPartID      - Part number to draw.
	//     iStateID     - State number of part
	//     pDestRect    - The RECT used to draw the line(s)
	//     uEdge        - Same as DrawEdge() API
	//     uFlags       - Same as DrawEdge() API
	//     pContentRect - Receives the interior rect if (uFlags &
	//                    BF_ADJUST)
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeEdge(HDC hDC, int iPartID, int iStateID, const RECT *pDestRect, UINT uEdge, UINT uFlags, RECT* pContentRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws an image within an imagelist based on a (possible)
	//     theme-defined effect.
	// Parameters:
	//     hDC          - HDC to draw into
	//     iPartID      - Part number to draw.
	//     iStateID     - State number of part
	//     pRect        - The RECT to draw the image within
	//     himl         - Handle to IMAGELIST
	//     iImageIndex  - Index into IMAGELIST (which icon to draw)
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeIcon(HDC hDC, int iPartID, int iStateID, const RECT* pRect, HIMAGELIST himl, int iImageIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Used by partially-transparent or alpha-blended child controls
	//     to draw the part of their parent that they appear in front of.
	// Parameters:
	//     hWnd  - Handle of the child control
	//     hDC   - HDC of the child control
	//     pRect - (optional) rect that defines the area to be
	//             Drawn (CHILD coordinates)
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeParentBackground(HWND hWnd, HDC hDC, RECT* pRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables/disables dialog background theme.  This method can be
	//     used to tailor dialog compatibility with child windows and
	//     controls that may or may not coordinate the rendering of
	//     their client area backgrounds with that of their parent
	//     dialog in a manner that supports seamless background
	//     texturing.
	// Parameters:
	//     hWnd    - The window handle of the target dialog
	//     dwFlags - ETDT_ENABLE to enable the theme-defined dialog
	//               background texturing,
	//               ETDT_DISABLE to disable background texturing,
	//               ETDT_ENABLETAB to enable the theme-defined
	//               background texturing using the Tab texture
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT EnableThemeDialogTexture(HWND hWnd, DWORD dwFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Enables or disables theming for the current user in the
	//     current and future sessions.
	// Parameters:
	//     bEnable - If FALSE, disable theming & turn themes off.
	//               if TRUE, enable theming and, if user
	//               previously had a theme active, make it active
	//               now.
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT EnableTheming(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the name of the current theme in-use.  Optionally, return
	//     the ColorScheme name and the Size name of the theme.
	// Parameters:
	//     pszThemeFileName - Receives the theme path & filename
	//     dwMaxNameChars   - Max chars allowed in pszNameBuff
	//     pszColorBuff     - (optional) receives the canonical color
	//                        scheme name
	//                        (not the display name)
	//     cchMaxColorChars - Max chars allowed in pszColorBuff
	//     pszSizeBuff      - (optional) receives the canonical size
	//                        name (not the display name)
	//     cchMaxSizeChars  - Max chars allowed in pszSizeBuff
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetCurrentThemeName(LPWSTR pszThemeFileName, int dwMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR pszSizeBuff, int cchMaxSizeChars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the property flags that control how visual styles
	//     are applied in the current application.
	// Returns:
	//     The following return values are bit flags combined with a
	//     logical OR operator.
	//     STAP_ALLOW_NONCLIENT  - Specifies that the non-client areas of
	//                             application windows have visual
	//                             styles applied.
	//     STAP_ALLOW_CONTROLS   - Specifies that controls in
	//                             application windows have visual
	//                             styles applied.
	//     STAP_ALLOW_WEBCONTENT - Specifies that all web content
	//                             displayed in an application is
	//                             rendered using visual styles.
	//-----------------------------------------------------------------------
	DWORD ThemeAppProperties();

	//-----------------------------------------------------------------------
	// Summary:
	//     Computes the region for a regular or partially transparent
	//     theme-specified background that is bound by the specified
	//     "pRect".  If the rectangle is empty, sets the hRgn to NULL
	//     and return S_FALSE.
	// Parameters:
	//     hDC      - Optional hDC to draw into (DPI scaling)
	//     iPartID  - Part number to draw.
	//     iStateID - State number (of the part)
	//     pRect    - The RECT used to draw the part
	//     pRegion  - Receives handle to calculated region
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeBackgroundRegion(HDC hDC, int iPartID, int iStateID, const RECT *pRect, HRGN* pRegion);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified BOOL property
	// Parameters:
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to get the value for
	//     pfVal    - Receives the value of the property
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeBool(int iPartID, int iStateID, int iPropID, BOOL* pfVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified COLOR property
	// Parameters:
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to get the value for
	//     pColor   - Receives the value of the property
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeColor(int iPartID, int iStateID, int iPropID, COLORREF* pColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified property name from the
	//     [documentation] section of the themes.ini file for the
	//     specified theme.  If the property has been localized in the
	//     theme files string table, the localized version of the
	//     property value is returned.
	// Parameters:
	//     pszThemeName     - Filename of the theme file to query
	//     pszPropertyName  - Name of the string property to retrieve a
	//                        value for
	//     pszValueBuff     - Receives the property string value
	//     cchMaxValChars   - Max chars allowed in pszValueBuff
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeDocumentationProperty(LPCWSTR pszThemeName, LPCWSTR pszPropertyName, LPWSTR pszValueBuff, int cchMaxValChars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified ENUM property
	// Parameters:
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to get the value for
	//     piVal    - Receives the value of the enum (cast to int*)
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeEnumValue(int iPartID, int iStateID, int iPropID, int* piVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified FILENAME property.
	// Parameters:
	//     iPartID          - Part number
	//     iStateID         - State number of part
	//     iPropID          - The property number to search for
	//     pszThemeFilename - Output buffer to receive the filename
	//     cchMaxBuffChars  - The size of the return buffer, in chars
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeFilename(int iPartID, int iStateID, int iPropID, LPWSTR pszThemeFilename, int cchMaxBuffChars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified font property
	// Parameters:
	//     hDC      - (optional) hDC to be drawn to (DPI scaling)
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to get the value for
	//     pFont    - Receives the value of the LOGFONT property
	//                (scaled for the current logical screen dpi)
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeFont(HDC hDC, int iPartID, int iStateID, int iPropID, LOGFONT* pFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified int property
	// Parameters:
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to get the value for
	//     piVal    - Receives the value of the property
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeInt(int iPartID, int iStateID, int iPropID, int* piVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified INTLIST struct
	// Parameters:
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to get the value for
	//     pIntList - Receives the value of the INTLIST property
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeIntList(int iPartID, int iStateID, int iPropID, INTLIST* pIntList);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified MARGINS property
	// Parameters:
	//     hDC      - (optional) hDC to be used for drawing
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to get the value for
	//     pRect    - RECT for area to be drawn into
	//     pMargins - Receives the value of the MARGINS property
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeMargins(HDC hDC, int iPartID, int iStateID, int iPropID, RECT *pRect, MARGINS* pMargins);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified metric/size property
	// Parameters:
	//     hDC      - (optional) hDC to be drawn into (DPI scaling)
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to get the value for
	//     piVal    - Receives the value of the property
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeMetric(HDC hDC, int iPartID, int iStateID, int iPropID, int* piVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the specified size of the theme part
	// Parameters:
	//     hDC      - HDC to select font into & measure against
	//     iPartID  - Part number to retrieve size for
	//     iStateID - State number (of the part)
	//     pRect    - (optional) rect for part drawing destination
	//     eSize    - The type of size to be retrieved
	//     pSize    - Receives the specified size of the part
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemePartSize(HDC hDC, int iPartID, int iStateID, RECT *pRect, THEMESIZE eSize, SIZE* pSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified position property
	// Parameters:
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to get the value for
	//     pPoint   - Receives the value of the position property
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemePosition(int iPartID, int iStateID, int iPropID, POINT* pPoint);

	//-----------------------------------------------------------------------
	// Summary:
	//     Searches for the specified theme property and sets "pOrigin"
	//     to indicate where it was found (or not found)
	// Parameters:
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to search for
	//     pOrigin  - Receives the value of the property origin
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemePropertyOrigin(int iPartID, int iStateID, int iPropID, PROPERTYORIGIN* pOrigin);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified RECT property
	// Parameters:
	//     iPartID  - Part number
	//     iStateID - State number of part
	//     iPropID  - The property number to get the value for
	//     pRect    - Receives the value of the RECT property
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeRect(int iPartID, int iStateID, int iPropID, RECT* pRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified string property
	// Parameters:
	//     iPartID         - Part number
	//     iStateID        - State number of part
	//     iPropID         - The property number to get the value for
	//     pszBuff         - Receives the string property value
	//     cchMaxBuffChars - Max. number of chars allowed in pszBuff
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeString(int iPartID, int iStateID, int iPropID, LPWSTR pszBuff, int cchMaxBuffChars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the boolean value of specified System metric.
	// Parameters:
	//     iBoolID - The TMT_XXX BOOL number (first BOOL is
	//               TMT_FLATMENUS)
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	BOOL GetThemeSysBool(int iBoolID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value of the specified System color.
	// Parameters:
	//     iColorID - The system color index defined in winuser.h
	// Returns:
	//     The value of the specified system color.
	//-----------------------------------------------------------------------
	COLORREF GetThemeSysColor(int iColorID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the brush for the specified System color.
	// Parameters:
	//     iColorID - The system color index defined in winuser.h
	// Returns:
	//     Handle to brush data.
	//-----------------------------------------------------------------------
	HBRUSH GetThemeSysColorBrush(int iColorID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the LOGFONT for the specified System font.
	// Parameters:
	//     iFontID - The TMT_XXX font number (first font
	//               is TMT_CAPTIONFONT)
	//     pLF     - Ptr to LOGFONT to receive the font value.
	//               (scaled for the current logical screen dpi)
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeSysFont(int iFontID, LOGFONT* pLF);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value of specified System int.
	// Parameters:
	//     iIntID  - Must be one of the following values:
	//               TMT_DPIX
	//               TMT_DPIY
	//               TMT_MINCOLORDEPTH
	//     piValue - Ptr to int to receive value
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeSysInt(int iIntID, int* piValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value of the specified System size metric.  (scaled
	//     for the current logical screen dpi)
	// Parameters:
	//     iSizeID - The following values are supported:
	//               SM_CXBORDER   (border width)
	//               SM_CXVSCROLL  (scrollbar width)
	//               SM_CYHSCROLL  (scrollbar height)
	//               SM_CXSIZE     (caption width)
	//               SM_CYSIZE     (caption height)
	//               SM_CXSMSIZE   (small caption width)
	//               SM_CYSMSIZE   (small caption height)
	//               SM_CXMENUSIZE (menubar width)
	//               SM_CYMENUSIZE (menubar height)
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	int GetThemeSysSize(int iSizeID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value of specified System string metric.
	// Parameters:
	//     iStringID         - Must be one of the following values:
	//                         TMT_CSSNAME
	//                         TMT_XMLNAME
	//     pszStringBuff     - The buffer to receive the string value
	//     cchMaxStringChars - Max. number of chars that pszStringBuff
	//                         can hold
	// Returns:
	//     The size in dots per inch (DPI) scaled for the current
	//     logical screen.
	//-----------------------------------------------------------------------
	HRESULT GetThemeSysString(int iStringID, LPWSTR pszStringBuff, int cchMaxStringChars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates the size/location of the specified text when
	//     rendered in the Theme Font.
	// Parameters:
	//     hDC             - HDC to select font & measure into
	//     iPartID         - Part number to draw.
	//     iStateID        - State number (of the part)
	//     pszText         - The text to be measured
	//     iCharCount      - Number of chars to draw (-1 for all)
	//     dwTextFlags     - Same as DrawText() "uFormat" param
	//     pBoundingRect   - Optional: to control layout of text
	//     pExtentRect     - Receives the RECT for text size/location
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeTextExtent(HDC hDC, int iPartID, int iStateID, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, const RECT *pBoundingRect, RECT* pExtentRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns info about the theme-specified font for the part/
	//     state passed in.
	// Parameters:
	//     hDC      - Optional: hDC for screen context
	//     iPartID  - Part number to draw.
	//     iStateID - State number (of the part)
	//     ptm      - Receives the font info
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT GetThemeTextMetrics(HDC hDC, int iPartID, int iStateID, TEXTMETRIC* ptm);

	//-----------------------------------------------------------------------
	// Summary:
	//     If window is themed, returns its most recent HTHEME from
	//     OpenThemeData() - Otherwise, returns NULL.
	// Parameters:
	//     hWnd - The window to get the HTHEME of
	// Returns:
	//     The most recent theme handle from OpenThemeData.
	//-----------------------------------------------------------------------
	HTHEME GetWindowTheme(HWND hWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a HitTestCode (a subset of the values returned by
	//     WM_NCHITTEST) for the point "ptTest" within the theme-
	//     specified background (bound by pRect).  "pRect" and "ptTest"
	//     should both be in the same coordinate system (client, screen,
	//     etc).
	// Parameters:
	//     hDC           - HDC to draw into
	//     iPartID       - Part number to test against
	//     iStateID      - State number (of the part)
	//     pRect         - The RECT used to draw the part
	//     hRgn          - Optional region to use; must be in same
	//                     coordinates as
	//                     pRect and pTest.
	//     ptTest        - The hit point to be tested
	//     dwOptions     - HTTB_xxx constants
	//     pwHitTestCode - Receives the returned hit test code - One of:
	//                     HTNOWHERE, HTLEFT, HTTOPLEFT, HTBOTTOMLEFT,
	//                     HTRIGHT, HTTOPRIGHT, HTBOTTOMRIGHT,
	//                     HTTOP, HTBOTTOM, HTCLIENT
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT HitTestThemeBackground(HDC hDC, int iPartID, int iStateID, DWORD dwOptions, const RECT *pRect, HRGN hRgn, POINT ptTest, WORD* pwHitTestCode);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reports whether the current application's user interface
	//     displays using visual styles.
	// Returns:
	//     Returns one of the following values.
	//     TRUE  - The application has a visual style applied.
	//     FALSE - The application does not have a visual style applied.
	//-----------------------------------------------------------------------
	BOOL IsAppThemed() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Can be used to test if a system theme is active for the
	//     current user session.
	//     use the API "IsAppThemed()" to test if a theme is active for
	//     the calling process.
	// Returns:
	//     Returns one of the following values:
	//     TRUE  - A visual style is enabled, and windows with visual
	//             styles applied should call OpenThemeData to start
	//             using theme drawing services.
	//     FALSE - A visual style is not enabled, and the window message
	//             handler does not need to make another call to
	//             IsThemeActive until it receives a WM_THEMECHANGED
	//             message.
	//-----------------------------------------------------------------------
	BOOL IsThemeActive() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the application should
	//     display Windows XP themes.  IsAppThemeReady() will first check
	//     to see if the operating system supports Windows XP theme display,
	//     then checks to see if the application has any controls that are
	//     able to display Windows XP themes, then finally checks to see if
	//     the application has included a manifest file.
	// Returns:
	//     TRUE if the application can display Windows XP themes, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL IsAppThemeReady() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if the theme specified background for the part/
	//     state has transparent pieces or alpha-blended pieces.
	// Parameters:
	//     iPartID  - Part number
	//     iStateID - State number of part
	// Returns:
	//     Returns one of the following values.
	//     TRUE  - The theme-specified background for a particular
	//             iPartId and iStateId has transparent pieces or alpha-
	//             blended pieces.
	//     FALSE - The theme-specified background for a particular
	//             iPartId and iStateId does not have transparent pieces
	//             or alpha-blended pieces.
	//-----------------------------------------------------------------------
	BOOL IsThemeBackgroundPartiallyTransparent(int iPartID, int iStateID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reports whether the dialog supports background texturing.
	// Returns:
	//     Returns one of the following values.
	//     TRUE  - Background texturing is supported.
	//     FALSE - Background texturing is not supported.
	//-----------------------------------------------------------------------
	BOOL IsThemeDialogTextureEnabled();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns TRUE if the theme has defined parameters for the
	//     specified "iPartID" and "iStateID".
	// Parameters:
	//     iPartID  - Part number to find definition for
	//     iStateID - State number of part
	// Returns:
	//     Returns one of the following values.
	//     TRUE  - The theme has defined parameters for the specified
	//             iPartId and iStateId
	//     FALSE - The theme does not have defined parameters for the
	//             specified iPartId and iStateId
	//-----------------------------------------------------------------------
	BOOL IsThemePartDefined(int iPartID, int iStateID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the flags that control theming within the app
	// Parameters:
	//     dwFlags - The flag values to be set
	//-----------------------------------------------------------------------
	void SetThemeAppProperties(DWORD dwFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Redirects an existing Window to use a different section of
	//     the current theme information than its class normally asks
	//     for.
	// Parameters:
	//     hWnd          - The handle of the window (cannot be NULL)
	//     pszSubAppName - App (group) name to use in place of the
	//                     calling app's name.  If NULL, the actual
	//                     calling app name will be used.
	//     pszSubIdList  - Semicolon separated list of class Id names to
	//                     use in place of actual list passed by the
	//                     window's class.  if NULL, the id list from
	//                     the calling class is used.
	// Remarks: The Theme Manager will remember the "pszSubAppName" and the
	//          "pszSubIdList" associations thru the lifetime of the window
	//          (even if themes are subsequently changed).  The window is
	//          sent a "WM_THEMECHANGED" msg at the end of this call, so that
	//          the new theme can be found and applied.
	//          When "pszSubAppName" or "pszSubIdList" are NULL, the Theme
	//          Manager removes the previously remember association.  To turn
	//          off theme-ing for the specified window, you can pass an empty
	//          string (L"") so it won't match any section entries.
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT SetWindowTheme(HWND hWnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if OpenThemeData() was
	//     called to open theme information for a particular window.
	// Returns:
	//     TRUE if theme information was loaded, otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL ThemeDataOpen() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the property flags that control how visual styles are applied in the current application.
	//-----------------------------------------------------------------------
	DWORD GetThemeAppProperties();


//{{AFX_CODEJOCK_PRIVATE
	static BOOL m_bDllDetached;

	//for compatibility with older versions.
	void CloseTheme() {
		CloseThemeData();
	}
	void OpenTheme(HWND hWnd, LPCWSTR pszClassList) {
		OpenThemeData(hWnd, pszClassList);
	}
	void DrawBackground(HDC hdc, int iPartId, int iStateId, const RECT *pRect,  const RECT* pClipRect) {
		DrawThemeBackground(hdc, iPartId, iStateId, pRect, pClipRect);
	}
//}}AFX_CODEJOCK_PRIVATE

	//-----------------------------------------------------------------------
	// Summary:
	//     Begins a buffered paint operation
	// Parameters:
	//     hdcTarget - Handle of the target device context (DC) on which the buffer will be painted.
	//     prcTarget - Pointer to a RECT structure that specifies the area of the target DC in which to paint.
	//     dwFormat - A member of the BP_BUFFERFORMAT enumeration that specifies the format of the buffer.
	//     pPaintParams - [in] Pointer to a BP_PAINTPARAMS structure that defines the paint operation parameters. This value can be NULL.
	//     phdc - [out] When this function returns, this value points to the handle of the new device context.
	// Returns: A handle to the buffered paint context
	// See Also: EndBufferedPaint
	//-----------------------------------------------------------------------
	HPAINTBUFFER BeginBufferedPaint(HDC hdcTarget, const RECT* prcTarget, XTP_UX_BUFFERFORMAT dwFormat, XTP_UX_PAINTPARAMS *pPaintParams, HDC *phdc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Completes a buffered paint operation and frees the associated buffered paint handle
	// Parameters:
	//     hBufferedPaint - Handle of the buffered paint context, obtained through BeginBufferedPaint
	//     fUpdateTarget - TRUE to copy the buffer to the target device context (DC)
	// See Also: BeginBufferedPaint
	//-----------------------------------------------------------------------
	void EndBufferedPaint(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the text using the theme-specified color and font for
	//     the "iPartID" and "iStateID".
	// Parameters:
	//     hDC          - HDC to draw into
	//     iPartID      - Part number to draw
	//     iStateID     - State number (of the part) to draw
	//     pszText      - Actual text to draw
	//     iCharCount   - Number of chars to draw (-1 for all)
	//     dwTextFlags  - Same as DrawText() "uFormat" param
	//     pOptions     - Additional drawing options
	//     pRect        - Defines the size/location of the part
	// Returns:
	//     Returns S_OK if successful, or an error value otherwise.
	//-----------------------------------------------------------------------
	HRESULT DrawThemeTextEx(HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const XTP_UX_DTTOPTS *pOptions);

	//-----------------------------------------------------------------------
	// Parameters:
	//     nThemeWrapper - Wrapper id.
	//     lpWrapperPtr  - Wrapper pointer
	// Summary:
	//     This method is used to set user defined wrappers
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetThemeWrapperPtr(int nThemeWrapper, LPVOID lpWrapperPtr);

private:
	void ReloadLibrary() const;

private:
	HTHEME  m_hTheme;     // Handle to the theme data.
	static CSharedData& AFX_CDECL GetSharedData();
};

//{{AFX_CODEJOCK_PRIVATE

struct XTP_DWM_MARGINS
{
	int cxLeftWidth;      // width of left border that retains its size
	int cxRightWidth;     // width of right border that retains its size
	int cyTopHeight;      // height of top border that retains its size
	int cyBottomHeight;   // height of bottom border that retains its size
};

TYPEDEF_WRAPPER_POINTER(0, HRESULT, DwmDefWindowProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
TYPEDEF_WRAPPER_POINTER(1, HRESULT, DwmIsCompositionEnabled)( BOOL*);
TYPEDEF_WRAPPER_POINTER(2, HRESULT, DwmExtendFrameIntoClientArea)(HWND, const XTP_DWM_MARGINS*);

//===========================================================================
// Summary:
//     The CXTPWinDwmWrapper class wraps the dwp api.
//===========================================================================
class _XTP_EXT_CLASS CXTPWinDwmWrapper
{
private:
	//-----------------------------------------------------------------------
	// Summary:
	//     Shared data of wrappers
	//-----------------------------------------------------------------------
	class CSharedData
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CSharedData object.
		//-------------------------------------------------------------------------
		CSharedData();

	public:
		LPVOID m_ptrWrappers[3];   // Wrapper pointer
		HMODULE m_hDwmDll;        // Handle to the theme dll.
	};

public:

	CXTPWinDwmWrapper();
	~CXTPWinDwmWrapper();

public:

	HRESULT DefWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
	BOOL IsCompositionEnabled();
	HRESULT ExtendFrameIntoClientArea(HWND hWnd, int cxLeftWidth, int cyTopHeight, int cxRightWidth, int cyBottomHeight);

public:
	void LogicalToPhysicalPoint(HWND hWnd, LPPOINT lpPoint);
	void SetProcessDPIAware();

private:
	static CSharedData& AFX_CDECL GetSharedData();
};

//}}AFX_CODEJOCK_PRIVATE

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPWINTHEMEWRAPPER_H__)
