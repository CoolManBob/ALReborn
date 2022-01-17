// XTPDrawHelpers.h: interface for the CXTPDrawHelpers class.
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
#if !defined(__XTPDRAWHELPERS_H__)
#define __XTPDRAWHELPERS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPPaintManagerColorGradient;


#ifndef LAYOUT_BITMAPORIENTATIONPRESERVED
#define LAYOUT_BITMAPORIENTATIONPRESERVED 0x00000008
#endif

#ifndef LAYOUT_RTL
#define LAYOUT_RTL 0x00000001
#endif

//===========================================================================
// Summary:
//     CXTPTransparentBitmap is a helper class used to extract the
//     transparent color from a transparent BitMap.  Also, this class
//     is used to convert a transparent BitMap into a transparent icon.
//===========================================================================
class _XTP_EXT_CLASS CXTPTransparentBitmap
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructor.  Construct a new CXTPTransparentBitmap from a
	//     handle to an existing BitMap.
	// Parameters:
	//     hBitmap - Handle to an existing BitMap.
	//-----------------------------------------------------------------------
	CXTPTransparentBitmap(HBITMAP hBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the transparent color of the BitMap.
	// Returns:
	//     -1 if the BitMap is NULL.
	//     Otherwise, a COLORREF that contains the transparent color of the BitMap.
	//-----------------------------------------------------------------------
	COLORREF GetTransparentColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to create an icon based on the BitMap.
	// Returns:
	//     NULL if the BitMap is NULL.
	//     NULL if the width or height of the BitMap is 0.
	//     Otherwise, a handle to the icon created from the BitMap.
	//-----------------------------------------------------------------------
	HICON ConvertToIcon() const;

protected:

	HBITMAP m_hBitmap;  // A handle to a BtiMap.
};

//===========================================================================
// Summary:
//     CXTPClientCursorPos is a helper class used to retrieve the cursor
//     position in client coordinates.
//===========================================================================
class _XTP_EXT_CLASS CXTPClientCursorPos : public CPoint
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPClientCursorPos object used to retrieve the
	//     cursor position in client coordinates.
	// Parameters:
	//     pWnd - Pointer to the window that contains the client area to
	//            get the cursor position for.
	//-----------------------------------------------------------------------
	CXTPClientCursorPos(CWnd* pWnd);
};

//===========================================================================
// Summary:
//     CXTPEmptyRect is a helper class used to instantiate an empty
//     CRect object.
//===========================================================================
class _XTP_EXT_CLASS CXTPEmptyRect : public CRect
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPEmptyRect object used to instantiate an
	//     empty CRect object.
	//-----------------------------------------------------------------------
	CXTPEmptyRect();
};

//===========================================================================
// Summary:
//     CXTPEmptySize is a helper class used to instantiate an empty
//     CSize object.
//===========================================================================
class _XTP_EXT_CLASS CXTPEmptySize : public CSize
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPEmptySize object used to instantiate an
	//     empty CSize object.
	//-----------------------------------------------------------------------
	CXTPEmptySize();

	//-----------------------------------------------------------------------
	// Summary:
	//     Makes CXTPEmptySize a null size by setting all coordinates to zero.
	//-----------------------------------------------------------------------
	void SetSizeEmpty();

	//-----------------------------------------------------------------------
	// Summary:
	//      Assigns srcSize to CSize.
	// Parameters:
	//      Refers to a source size. Can be a SIZE or CSize.
	//-----------------------------------------------------------------------
	const SIZE& operator=(const SIZE& srcSize);
};

//===========================================================================
// Summary:
//     CXTPWindowRect is a helper class used to retrieve the screen
//     size for a specified window.
//===========================================================================
class _XTP_EXT_CLASS CXTPWindowRect : public CRect
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPWindowRect object used to retrieve the
	//     screen size for the specified window.
	// Parameters:
	//     hWnd - Handle to the window to retrieve the screen size for.
	//     pWnd - Points to the window to retrieve the screen size for.
	//-----------------------------------------------------------------------
	CXTPWindowRect(HWND hWnd);
	CXTPWindowRect(const CWnd* pWnd); // <combine CXTPWindowRect::CXTPWindowRect@HWND>
};

//===========================================================================
// Summary:
//     CXTPClientRect is a helper class used to retrieve the client
//     area for a specified window.
//===========================================================================
class _XTP_EXT_CLASS CXTPClientRect : public CRect
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPClientRect object used to retrieve the
	//     client area for the specified window.
	// Parameters:
	//     hWnd - Handle to the window to retrieve the client area for.
	//     pWnd - Points to the window to retrieve the client area for.
	//-----------------------------------------------------------------------
	CXTPClientRect(HWND hWnd);
	CXTPClientRect(const CWnd* pWnd); //<combine CXTPClientRect::CXTPClientRect@HWND>
};

//===========================================================================
// Summary:
//     CXTPBufferDC is a helper class used to create a memory device
//     context used to draw to an off-screen bitmap.  When destroyed, the
//     class selects the previous bitmap back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPBufferDC : public CDC
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBufferDC object used to create a memory
	//     device context used to draw to an off-screen bitmap.
	// Parameters:
	//     hDestDC - Handle to the destination device context the memory
	//               device is BitBlt to.
	//     rcPaint - Size of the area to paint.
	//-----------------------------------------------------------------------
	CXTPBufferDC(HDC hDestDC, const CRect& rcPaint);

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBufferDC object used to create a memory
	//     device context used to draw to an off-screen bitmap.
	// Parameters:
	//     paintDC - Handle to the destination device context the memory
	//-----------------------------------------------------------------------
	CXTPBufferDC(CPaintDC& paintDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPBufferDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBufferDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to set the valid flag to false
	//     so that the off screen device context will not be drawn.
	//-----------------------------------------------------------------------
	void Discard();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve a CDC pointer to the destination
	//     device context.
	//-----------------------------------------------------------------------
	CDC* GetDestDC();

protected:

	HDC     m_hDestDC;    // Handle to the destination device context.
	CBitmap m_bitmap;     // Bitmap in memory device context
	CRect   m_rect;       // Size of the area to paint.
	HGDIOBJ m_hOldBitmap; // Handle to the previously selected bitmap.
};

//===========================================================================
// Summary:
//     CXTPBufferDCEx is a helper class used to create a memory device
//     context used to draw to an off-screen bitmap.  When destroyed, the
//     class selects the previous bitmap back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPBufferDCEx : public CDC
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBufferDCEx object used to create a memory
	//     device context used to draw to an off-screen bitmap.
	// Parameters:
	//     hDestDC - Handle to the destination device context the memory
	//               device is BitBlt to.
	//     rcPaint - Size of the area to paint.
	//-----------------------------------------------------------------------
	CXTPBufferDCEx(HDC hDestDC, const CRect rcPaint);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPBufferDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBufferDCEx();

protected:

	HDC     m_hDestDC;    // Handle to the destination device context.
	HBITMAP m_bitmap;     // Bitmap in memory device context
	CRect   m_rect;       // Size of the area to paint.
	HGDIOBJ m_hOldBitmap; // Handle to the previously selected bitmap.
};

//===========================================================================
// Summary:
//     CXTPBitmapDC is a helper class used to select a bitmap into the
//     device context specified by hDC.  When destroyed, the class
//     selects the previous bitmap back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPBitmapDC
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBitmapDC object and selects the specified bitmap
	//     into the device context specified by pDC.
	// Parameters:
	//     pDC     - Pointer to a valid device context.
	//     pBitmap - Pointer to a CBitmap object to select into the device
	//               context.
	//-----------------------------------------------------------------------
	CXTPBitmapDC(CDC* pDC, CBitmap* pBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPBitmapDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBitmapDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the bitmap selected by the
	//     device context.
	// Parameters:
	//     pBitmap - Pointer to a CBitmap object to select into the device
	//               context.
	//-----------------------------------------------------------------------
	void SetBitmap(CBitmap* pBitmap);

protected:

	HDC     m_hDC;        // Device context handle.
	HGDIOBJ m_hOldBitmap; // Handle to the previously selected bitmap.
};

//===========================================================================
// Summary:
//     CXTPFontDC is a helper class used to select a font into the
//     device context specified by pDC.  When destroyed, the class
//     selects the previous font back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPFontDC
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPFontDC object and selects the specified font
	//     into the device context specified by pDC.
	// Parameters:
	//     pDC   - Pointer to a valid device context.
	//     pFont - Pointer to a CFont object to select into the device
	//             context.
	//     clrTextColor - Text color to set in the DC.
	//-----------------------------------------------------------------------
	CXTPFontDC(CDC* pDC, CFont* pFont = NULL);
	CXTPFontDC(CDC* pDC, CFont* pFont, COLORREF clrTextColor); // <combine CXTPFontDC::CXTPFontDC@CDC*@CFont*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPFontDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPFontDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the font selected by the
	//     device context.
	// Parameters:
	//     pFont - Pointer to a CFont object to select into the device
	//             context.
	//-----------------------------------------------------------------------
	void SetFont(CFont* pFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the text color set in the
	//     device context.
	// Parameters:
	//     clrTextColor - Text color to set in the DC.
	//-----------------------------------------------------------------------
	void SetColor(COLORREF clrTextColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the font and color selected
	//     by the device context.
	// Parameters:
	//     pFont - Pointer to a CFont object to select into the device
	//             context.
	//     clrTextColor - Text color to set in the DC.
	//-----------------------------------------------------------------------
	void SetFontColor(CFont* pFont, COLORREF clrTextColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to select the initial font back into the
	//     device context.
	// Remarks:
	//     This method is also called from the destructor.
	//-----------------------------------------------------------------------
	void ReleaseFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the initial text color back into the
	//     device context.
	// Remarks:
	//     This method is also called from the destructor.
	//-----------------------------------------------------------------------
	void ReleaseColor();

protected:
	CDC*     m_pDC;             // Stored pointer to a device context.
	CFont*   m_pOldFont;        // Stored pointer to an initial font from the device context.
	COLORREF m_clrOldTextColor; // Stored an initial text color from the device context.
};

//===========================================================================
// Summary:
//     CXTPPenDC is a helper class used to create a pen using
//     the color specified by crColor.  The pen is then selected
//     into the device context specified by hDC.  When destroyed, the
//     class selects the previous pen back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPPenDC
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPenDC object using an existing CPen object.  The
	//     pen is then selected into the device context specified by hDC.
	// Parameters:
	//     pDC  - Pointer to a valid device context.
	//     pPen - Pointer to a CPen object to select into the device
	//             context.
	//     hDC     - Handle to a valid device context.
	//     crColor - RGB value used to create pen.
	// Remarks:
	//     The crColor version constructs a CXTPPenDC object and creates a pen using
	//     the color specified by crColor.  The pen is then selected
	//     into the device context specified by hDC.
	//-----------------------------------------------------------------------
	CXTPPenDC(CDC* pDC, CPen* pPen);
	CXTPPenDC(HDC hDC, COLORREF crColor); // <combine CXTPPenDC::CXTPPenDC@CDC*@CPen*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPenDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPenDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve or change the color used by the
	//     device context pen.
	// Parameters:
	//     crColor - RGB value to change the pen color to.
	// Remarks:
	//     The No argument version is used to retrieve the color used by the
	//     device context pen.
	// Returns:
	//     The no argument version returns an RGB value that represents the selected pen color.
	//-----------------------------------------------------------------------
	COLORREF Color();
	void Color(COLORREF crColor); // <combine CXTPPenDC::Color>

protected:

	CPen m_pen;     // Pen selected into device context.
	HDC  m_hDC;     // Device context handle.
	HPEN m_hOldPen; // Handle to the previously selected pen.
};

//===========================================================================
// Summary:
//     CXTPBrushDC is a helper class used to create a  brush using
//     the color specified by crColor.  The brush is then selected
//     into the device context specified by hDC.  When destroyed, the
//     class selects the previous brush back into the device context to
//     handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPBrushDC
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPBrushDC object and creates a brush using
	//     the color specified by crColor.  The brush is then selected
	//     into the device context specified by hDC.
	// Parameters:
	//     hDC     - Handle to a valid device context.
	//     crColor - RGB value used to create brush.
	//-----------------------------------------------------------------------
	CXTPBrushDC(HDC hDC, COLORREF crColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPBrushDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPBrushDC();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to change the color used by the
	//     device context brush.
	// Parameters:
	//     crColor - RGB value to change the brush color to.
	//-----------------------------------------------------------------------
	void Color(COLORREF crColor);

protected:

	CBrush m_brush;     // Brush selected into device context.
	HDC    m_hDC;       // Device context handle.
	HBRUSH m_hOldBrush; // Handle to the previously selected brush.
};

//===========================================================================
// Summary:
//     CXTPCompatibleDC is a helper class used to create a memory device
//     context (DC) compatible with the device specified by pDC.  When
//     destroyed, the class selects the previous bitmap back into the
//     device context to handle GDI resource cleanup.
//===========================================================================
class _XTP_EXT_CLASS CXTPCompatibleDC : public CDC
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCompatibleDC object and creates a memory
	//     device context (DC) compatible with the device specified by
	//     pDC.  The bitmap specified by pBitmap is then selected into
	//     the device context.
	// Parameters:
	//     pDC     - Points to a valid device context.
	//     pBitmap - Points to the previously selected bitmap.
	//     hBitmap - Points to the previously selected bitmap.
	//-----------------------------------------------------------------------
	CXTPCompatibleDC(CDC* pDC, CBitmap* pBitmap);
	CXTPCompatibleDC(CDC* pDC, HBITMAP hBitmap); // <combine CXTPCompatibleDC::CXTPCompatibleDC@CDC*@CBitmap*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCompatibleDC object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCompatibleDC();

protected:

	HBITMAP m_hOldBitmap; // Pointer to the previously selected bitmap.
};

//===========================================================================
// Summary:
//     CXTPSplitterTracker is a stand alone class. It is used
//     to track a splitter rectangle.
//===========================================================================
class _XTP_EXT_CLASS CXTPSplitterTracker
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSplitterTracker object.
	// Parameters:
	//     bSolid - TRUE to use solid tracker; FALSE for HalftoneBrush.
	//-----------------------------------------------------------------------
	CXTPSplitterTracker(BOOL bSolid = FALSE);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to track size of the splitter.
	// Parameters:
	//     pTrackWnd - Pointer to a CWnd object.
	//     rcAvail - Available rectangle of tracking.
	//     rectTracker - Current rectangle of tracking.
	//     point - Starting point.
	//     bHoriz - TRUE to track horizontally.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL Track(CWnd* pTrackWnd, CRect rcAvail, CRect& rectTracker, CPoint point, BOOL bHoriz);

	//-----------------------------------------------------------------------
	// Summary:
	//     Controls the accumulation of bounding-rectangle information for the specified device context.
	// Parameters:
	//     rcBoundRect - A CRect object that specifies the boundaries of the bounding rectangle.
	//-----------------------------------------------------------------------
	void SetBoundRect(CRect rcBoundRect);

private:
	//-----------------------------------------------------------------------
	// Summary:
	//     This function is called by the framework during the resizing of splitter windows.
	//     This function inverts the contents of the Splitter Tracker rectangle.
	//     Inversion is a logical NOT operation and flips the bits of each pixel.
	// Parameters:
	//     rc - A CRect object that specifies the XY position of the Splitter Tracker.
	//-----------------------------------------------------------------------
	void OnInvertTracker(CRect rc);

private:
	CDC* m_pDC;
	BOOL m_bSolid;
	CRect m_rcBoundRect;
};

//===========================================================================
// Summary:
//     The class CXTPMouseMonitor is a helper class that is used to
//     monitor mouse messages for in-place controls.  This class is used
//     in XTPPropertyGridInPlaceList and XTPReportInplaceControls.
//===========================================================================
class _XTP_EXT_CLASS CXTPMouseMonitor
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to set up a hook to monitor mouse messages.
	// Parameters:
	//     pWndMonitor - A pointer to a CWnd object.  Used to determine
	//                   which monitor currently contains the mouse cursor.
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetupHook(CWnd* pWndMonitor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to determine if mouse messages was hooked.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsMouseHooked();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     The MouseProc hook procedure is an application-defined or
	//     library-defined callback function used with the SetWindowsHookEx
	//     function. The system calls this function whenever an application
	//     calls the GetMessage or PeekMessage function and there is a mouse
	//     message to be processed.
	// Parameters:
	//     nCode  - Specifies a code the hook procedure uses to determine
	//          how to process the message. This parameter can be one of the
	//          following values: HC_ACTION and HC_NOREMOVE.
	//          HC_ACTION   - The wParam and lParam parameters contain information about a mouse message.
	//          HC_NOREMOVE - The wParam and lParam parameters contain information about a mouse message,
	//                        and the mouse message has not been removed from the message queue. (An
	//                        application called the PeekMessage function, specifying the PM_NOREMOVE flag.)
	//     wParam - Specifies the identifier of the mouse message.
	//     lParam - Pointer to a MOUSEHOOKSTRUCT structure.
	// Returns:
	//     If nCode is less than zero, the hook procedure must return the value returned by CallNextHookEx.
	//     If nCode is greater than or equal to zero, and the hook procedure did not process the message,
	//     it is highly recommended that you call CallNextHookEx and return the value it returns; otherwise,
	//     other applications that have installed WH_MOUSE hooks will not receive hook notifications and may
	//     behave incorrectly as a result. If the hook procedure processed the message, it may return a nonzero
	//     value to prevent the system from passing the message to the target window procedure.
	//-----------------------------------------------------------------------
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
	static HHOOK m_hHookMouse;      // A handle to a hook.
	static CWnd* m_pWndMonitor;     // A pointer to the in-place control.
};

//===========================================================================
// Summary:
//     CXTPDrawHelpers is a helper class used to perform specialized
//     drawing tasks.  You can use this class to perform such tasks as
//     gradient fills and blending colors.
//===========================================================================
class _XTP_EXT_CLASS CXTPDrawHelpers
{
protected:
	//===========================================================================
	// Summary:
	//     Function pointer used to access the Windows API GradientFill function.
	//===========================================================================
	typedef BOOL (__stdcall* PFNGRADIENTFILL)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDrawHelpers object.
	//-----------------------------------------------------------------------
	CXTPDrawHelpers();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXCSplitPath object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDrawHelpers();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to perform a gradient fill using
	//     the specified COLORREF values crFrom and crTo.
	// Parameters:
	//     pDC         - Points to the device context
	//     lpRect      - Size of area to fill
	//     crFrom      - RGB value to start from
	//     crTo        - RGB value to fill to
	//     grc         - CXTPPaintManagerColorGradient struct containing start from and
	//                   fill to RGB color values.
	//     bHorz       - TRUE if fill is horizontal.
	//     lpRectClip  - Pointer to a RECT structure that contains a clipping rectangle.
	//                   This parameter is optional and may be set to NULL.
	//     hdc         - Handle to a device context.
	//     pVertex     - Pointer to an array of TRIVERTEX structures that
	//                   each define a triangle vertex.
	//     dwNumVertex - The number of vertices.
	//     pMesh       - Array of GRADIENT_TRIANGLE structures in triangle
	//                   mode, or an array of GRADIENT_RECT structures in
	//                   rectangle mode.
	//     dwNumMesh   - The number of elements (triangles or rectangles)
	//                   in pMesh.
	//     dwMode      - Specifies gradient fill mode. For a list of possible
	//                   values, see GradientFill in the Platform SDK.
	// Remarks:
	//     Call this member function to fill rectangle and triangle structures
	//     with color that smoothly fades from one side to the other.
	// Returns:
	//     The HDC version TRUE if successful; otherwise FALSE.
	//-----------------------------------------------------------------------
	void GradientFill(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);
	void GradientFill(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz, LPCRECT lpRectClip); // <combine CXTPDrawHelpers::GradientFill@CDC*@LPCRECT@COLORREF@COLORREF@BOOL>
	void GradientFill(CDC* pDC, LPCRECT lpRect, const CXTPPaintManagerColorGradient& grc, BOOL bHorz, LPCRECT lpRectClip = NULL); // <combine CXTPDrawHelpers::GradientFill@CDC*@LPCRECT@COLORREF@COLORREF@BOOL>
	BOOL GradientFill(HDC hdc, PTRIVERTEX pVertex, ULONG dwNumVertex, PVOID pMesh, ULONG dwNumMesh, ULONG dwMode); // <combine CXTPDrawHelpers::GradientFill@CDC*@LPCRECT@COLORREF@COLORREF@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to exclude the pixels for each corner of the
	//     area specified by rc.
	// Parameters:
	//     pDC - Points to the device context
	//     rc  - Area to exclude corners from
	//-----------------------------------------------------------------------
	void ExcludeCorners(CDC* pDC, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will search a string, strip off the mnemonic
	//     '&', and reformat the string.
	// Parameters:
	//     strClear - Text needed to strip.
	//     lpszClear - Text needed to strip.
	//-----------------------------------------------------------------------
	static void AFX_CDECL StripMnemonics(CString& strClear);
	static void AFX_CDECL StripMnemonics(LPTSTR lpszClear); // <combine CXTPDrawHelpers::StripMnemonics@CString&>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to blur the color value for the
	//     points specified by pts
	// Parameters:
	//     pDC    - Points to the device context
	//     pts    - Array of points to blur
	//     nCount - Number of points in array.
	//-----------------------------------------------------------------------
	void BlurPoints(CDC* pDC, LPPOINT pts, int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to blend the colors specified by
	//     crA and crB.
	// Parameters:
	//     crA      - RGB value to blend
	//     crB      - RGB value to blend
	//     fAmountA - The amount that crA saturates crB.
	// Returns:
	//     An RGB value that represents the blended color.
	//-----------------------------------------------------------------------
	COLORREF BlendColors(COLORREF crA, COLORREF crB, float fAmountA);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get dark color from specified by lColor
	// Parameters:
	//      lScale - Scale value
	//      lColor - RGB value to get
	// See Also: LightenColor
	//-----------------------------------------------------------------------
	COLORREF DarkenColor(long lScale, COLORREF lColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get light color from specified by lColor
	// Parameters:
	//      lScale - Scale value
	//      lColor - RGB value to get
	// See Also: DarkenColor
	//-----------------------------------------------------------------------
	COLORREF LightenColor(long lScale, COLORREF lColor);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the specified font
	//     exists.
	// Parameters:
	//     lpszFaceName - A NULL terminated string that represents the
	//                   font face name.
	// Returns:
	//     TRUE if the font was found, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL FontExists(LPCTSTR lpszFaceName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves DEFAULT_GUI_FONT font name.
	// Returns:
	//     Face name of DEFAULT_GUI_FONT font.
	//-----------------------------------------------------------------------
	static CString AFX_CDECL GetDefaultFontName();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves vertical font name
	// Parameters:
	//     bUseOfficeFont - TRUE to use "Tahoma" font if exists.
	// Returns:
	//     Face name of vertical font.
	//-----------------------------------------------------------------------
	static CString AFX_CDECL GetVerticalFontName(BOOL bUseOfficeFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a triangle onto the
	//     specified device context.
	// Parameters:
	//     pDC - Points to the device context
	//     pt0 - First point of the triangle
	//     pt1 - Second point of the triangle
	//     pt2 - Third point of the triangle
	//     clr - An RGB value that represents the fill color.
	//-----------------------------------------------------------------------
	static void AFX_CDECL Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2, COLORREF clr);
	static void AFX_CDECL Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2); // <combine CXTPDrawHelpers::Triangle@CDC*@CPoint@CPoint@CPoint@COLORREF>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to draw a vertical or horizontal line using
	//     the specified device context.
	// Parameters:
	//     pDC     - Points to the device context
	//     x       - Specifies the logical x-coordinate of the start position.
	//     y       - Specifies the logical y-coordinate of the start position.
	//     nLength - Specifies the length of the line to draw, for vertical lines set this value to 0.
	//     nHeight - Specifies the height of the line to draw, for horizontal lines set this value to 0.
	//     crLine  - Specifies the RGB color value used to draw the line.
	// Returns:
	//     TRUE if the specified device context is valid, otherwise FALSE.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL DrawLine(CDC* pDC, int x, int y, int nLength, int nHeight, COLORREF crLine);

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
	static BOOL AFX_CDECL IsLowResolution(HDC hDC = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if GradientFill from msimg32.dll available.
	// Returns:
	//     TRUE if GradientFill can be used
	//-----------------------------------------------------------------------
	BOOL IsFastFillAvailable() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if top level parent is active for window handle
	// Parameters:
	//     hWnd        - Window handle to test
	// Returns:
	//     TRUE if top level parent is active.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsTopParentActive(HWND hWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to switch Left and Right keys if window
	//     in Right To Left layout
	// Parameters:
	//     pWnd        - Window to test
	//     nChar       - Virtual key, passed to OnKeyDown member.
	//-----------------------------------------------------------------------
	static void AFX_CDECL KeyToLayout(CWnd* pWnd, UINT& nChar);

	//-----------------------------------------------------------------------
	// Summary:
	//    Converts the screen coordinates of a given point on the display to window coordinates.
	// Parameters:
	//     pWnd - window whose area is used for the conversion
	//     lpPoint - Points to a CPoint object or POINT structure that contains the screen coordinates to be converted.
	//-----------------------------------------------------------------------
	static void AFX_CDECL ScreenToWindow(CWnd* pWnd, LPPOINT lpPoint);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if layout of a device context right to left.
	// Parameters:
	//     pDC - Device context to test
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL IsContextRTL(CDC* pDC);
	static BOOL AFX_CDECL IsContextRTL(HDC hDC); // <combine CXTPDrawHelpers::IsContextRTL@CDC*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Cal this method to set right to left layout for device context
	//     pDC - Device context to set layout
	//     bLayoutRTL - TRUE to set right to left layout
	//-----------------------------------------------------------------------
	static void AFX_CDECL SetContextRTL(CDC* pDC, BOOL bLayoutRTL);
	static void AFX_CDECL SetContextRTL(HDC hDC, BOOL bLayoutRTL);

	//-----------------------------------------------------------------------
	// Summary: Register class helper.
	//-----------------------------------------------------------------------
	static BOOL AFX_CDECL RegisterWndClass(HINSTANCE hInstance, LPCTSTR lpszClassName, UINT style, HICON hIcon = 0);

//{{AFX_CODEJOCK_PRIVATE
	_XTP_DEPRECATE("This function or variable is no longer available. Please use 'XTPSystemVersion()->GetComCtlVersion' instead")
	static DWORD GetComCtlVersion();
	_XTP_DEPRECATE("This function or variable is no longer available. Please use 'XTPMultiMonitor()->GetWorkArea' instead")
	static CRect GetWorkArea();
	_XTP_DEPRECATE("This function or variable is no longer available. Please use 'XTPMultiMonitor()->GetWorkArea' instead")
	static CRect GetWorkArea(const POINT& point);
	_XTP_DEPRECATE("This function or variable is no longer available. Please use 'XTPMultiMonitor()->GetWorkArea' instead")
	static CRect GetWorkArea(LPCRECT rect);
	_XTP_DEPRECATE("This function or variable is no longer available. Please use 'XTPMultiMonitor()->GetWorkArea' instead")
	static CRect GetWorkArea(const CWnd* pWnd);
	_XTP_DEPRECATE("This function or variable is no longer available. Please use 'XTPMultiMonitor()->GetScreenArea' instead")
	static CRect GetScreenArea(const CWnd* pWnd);
//}}AFX_CODEJOCK_PRIVATE

private:

	HMODULE m_hMsImgDll;  // Handle to MsImg32.dll.

	PFNGRADIENTFILL m_pfnFastGradientFill;

	// private members used for draw routines.
	void GradientFillFast(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);
	void GradientFillSlow(CDC* pDC, LPCRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);

	// singleton
	friend _XTP_EXT_CLASS CXTPDrawHelpers* AFX_CDECL XTPDrawHelpers();   // Used to access the CXTPDrawHelpers object.
};



//===========================================================================
// Summary:
//     CXTPNoFlickerWnd is a TBase derived general purpose template
//     helper class. CXTPNoFlickerWnd class is used for drawing flicker
//     free controls. To use, instantiate the template using any CWnd
//     derived class. For example, to create a tab control that is
//     flicker free you would use the following declaration:
// Example:
//      The following example demonstrates how to use CXTPNoFlickerWnd to create a flicker free object.
// <code>
// CXTPNoFlickerWnd <CTabCtrl> m_tabCtrl;
// </code>
//===========================================================================
template <class TBase>
class CXTPNoFlickerWnd : public TBase
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPNoFlickerWnd object.
	//-----------------------------------------------------------------------
	CXTPNoFlickerWnd()
	{
		m_crBack = ::GetSysColor(COLOR_3DFACE);
	}

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to set the background fill
	//     color for the flicker free control.
	// Parameters:
	//     crBack - An RGB value.
	//-----------------------------------------------------------------------
	void SetBackColor(COLORREF crBack)
	{
		m_crBack = crBack;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to retrieve the background
	//     fill color for the flicker free control.
	// Returns:
	//     An RGB value.
	//-----------------------------------------------------------------------
	COLORREF GetBackColor()
	{
		return m_crBack;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This method provides a CE procedure (WindowProc) for a CWnd
	//     object.  It dispatches messages through the window message
	//     map. The return value depends on the message.
	// Parameters:
	//     message - Specifies the Windows message to be processed.
	//     wParam  - Provides additional information used in processing
	//               the message. The parameter value depends on the
	//               message.
	//     lParam  - Provides additional information used in processing
	//               the message. The parameter value depends on the
	//               message.
	// Returns:
	//     An LRESULT object.
	//-----------------------------------------------------------------------
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_PAINT:
			{
				CPaintDC dc(this);

				// Get the client rect, and paint to a memory device context.
				// This will help reduce screen flicker.  Pass the memory
				// device context to the default window procedure to do
				// default painting.

				CRect rc;
				GetClientRect(&rc);

				CXTPBufferDC memDC(dc.GetSafeHdc(), rc);
				memDC.FillSolidRect(rc, m_crBack);

				return TBase::DefWindowProc(WM_PAINT,
					(WPARAM)memDC.m_hDC, 0);
			}

		case WM_ERASEBKGND:
			{
				return TRUE;
			}
		}

		return TBase::WindowProc(message, wParam, lParam);
	}

protected:
	COLORREF m_crBack; // An RGB value.

};

//===========================================================================
// Summary:
//     CXTPOfficeBorder is a template class used for windows that require
//     a thin border. This class can be used as the base class for any
//     CWnd derived class to display an Office style non-client border.
// Parameters:
//     TBase     - Object class name.
//     bCalcSize - 'true' to remove non-client edge and adjust borders to
//                  1 pixel, 'false' to only draw 1 pixel border.
// Example:
//     Sample code demonstrates how to use the CXTPOfficeBorder template
//     class with your base class.
// <code>
// class CTreeCtrlEx : public CXTPOfficeBorder<CXTTreeCtrl>
// {
//     ...
// };
// </code>
//     Sample code demonstrates how to use the CXTPOfficeBorder template
//     class with a member variable.
// <code>
// class CMyView : public CView
// {
//     ...
// protected:
//     CXTPOfficeBorder<CEdit,false> m_edit;
// };
// </code>
//========================================================================
template <class TBase, bool bCalcSize = true>
class CXTPOfficeBorder : public TBase
{
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to return the color used for drawing non-client
	//     Office style borders for the active current theme.
	// Parameters:
	//     bTopLeft - 'true' to return the top-left border color, 'false' to
	//                return the bottom-right border color.
	// Returns:
	//     An RGB color value that represents the non-client border color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetBorderColor(bool bTopLeft) const
	{

//{{AFX_CODEJOCK_PRIVATE
#if defined(_XTP_INCLUDE_COMMANDBARS)

		switch (XTPPaintManager()->BaseTheme())
		{
		case xtpThemeOfficeXP:
		case xtpThemeOffice2000:  return GetXtremeColor(bTopLeft? COLOR_3DSHADOW: COLOR_3DHIGHLIGHT);
		case xtpThemeOffice2003:  return GetXtremeColor(XPCOLOR_FRAME);
		case xtpThemeNativeWinXP: return GetXtremeColor(XPCOLOR_STATICFRAME);
		}

#elif defined(_XTP_INCLUDE_CONTROLS)

		switch (XTThemeManager()->GetTheme())
		{
		case xtThemeOfficeXP:
		case xtThemeDefault:     return GetXtremeColor(bTopLeft? COLOR_3DSHADOW: COLOR_3DHIGHLIGHT);
		case xtThemeOffice2003:  return GetXtremeColor(XPCOLOR_FRAME);
		default:                 return GetXtremeColor(XPCOLOR_STATICFRAME);
		}

#endif
//}}AFX_CODEJOCK_PRIVATE

		return GetXtremeColor(XPCOLOR_3DSHADOW);
	}

	//{{AFX_CODEJOCK_PRIVATE
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_NCPAINT:
			{
				TBase::WindowProc(message, wParam, lParam);

				CWindowDC dc(this);

				// convert to client coordinates
				CXTPWindowRect rWindow(this);
				ScreenToClient(rWindow);

				int nBorderWidth = -rWindow.top;

				rWindow.OffsetRect(-rWindow.left, -rWindow.top);

				// draw 1px. border.
				dc.Draw3dRect(&rWindow, GetBorderColor(true), GetBorderColor(false));

				if (nBorderWidth > 1)
				{
					rWindow.DeflateRect(1,1);
					dc.Draw3dRect(&rWindow,
						XTPColorManager()->GetColor(COLOR_WINDOW),
						XTPColorManager()->GetColor(COLOR_WINDOW));
				}

				return 0; // Handled.
			}

		case WM_CREATE:
			{
				if (TBase::WindowProc(message, wParam, lParam) == -1)
					return -1;

				if (bCalcSize)
				{
					// remove 3D borders.
					long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE) &~WS_BORDER;
					::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

					long lExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE) &~WS_EX_CLIENTEDGE;
					::SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle);

					// force non-client area to be recalculated.
					::SetWindowPos(m_hWnd, NULL, NULL, NULL, NULL, NULL,
						SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
				}

				return 0; // Handled.
			}
			break;

		case WM_NCCALCSIZE:
			{
				LRESULT lResult = TBase::WindowProc(message, wParam, lParam);

				if (bCalcSize)
				{
					NCCALCSIZE_PARAMS FAR* lpncsp = (NCCALCSIZE_PARAMS FAR*)lParam;

					// adjust non-client area for border space
					lpncsp->rgrc[0].left   += 1;
					lpncsp->rgrc[0].top    += 1;
					lpncsp->rgrc[0].right  -= 1;
					lpncsp->rgrc[0].bottom -= 1;
				}

				return lResult;
			}
		}

		return TBase::WindowProc(message, wParam, lParam);
	}
	//}}AFX_CODEJOCK_PRIVATE
};

//---------------------------------------------------------------------------
// Summary:
//     Call this function to access CXTPDrawHelpers members.
//     Since this class is designed as a single instance object you can
//     only access its members through this method. You <b>cannot</b>
//     directly instantiate an object of type CXTPDrawHelpers.
// Example:
//     <code>BOOL bLowRes = XTPDrawHelpers()->IsLowResolution();</code>
//---------------------------------------------------------------------------
_XTP_EXT_CLASS CXTPDrawHelpers* AFX_CDECL XTPDrawHelpers();


//---------------------------------------------------------------------------
// Summary:
//    Prepare a printer device context for printing and calculates
//    DEVMODE and DEVNAMES values.
// Parameters:
//    ref_hDevMode  - Reference to a handle of a movable global memory object
//                    that contains a DEVMODE structure.
//    ref_hDevNames - Reference to a handle of a movable global memory object
//                    that contains a DEVNAMES structure.
// Returns:
//    Nonzero if successful; otherwise 0.
// See Also:
//    CWinApp::GetPrinterDeviceDefaults, DEVMODE, DEVNAMES
//---------------------------------------------------------------------------
BOOL XTPGetPrinterDeviceDefaults(HGLOBAL& ref_hDevMode, HGLOBAL& ref_hDevNames);


//===========================================================================
// Summary:
//      Helper class which provide currently selected printer information.
//===========================================================================
class _XTP_EXT_CLASS CXTPPrinterInfo : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPPrinterInfo)
public:

	//-------------------------------------------------------------------------
	// Summary: Default object constructor.
	// See Also: ~CXTPPrinterInfo
	//-------------------------------------------------------------------------
	CXTPPrinterInfo();

	//-------------------------------------------------------------------------
	// Summary: Default object destructor.
	// See Also: CXTPPrinterInfo
	//-------------------------------------------------------------------------
	virtual ~CXTPPrinterInfo();


	//-------------------------------------------------------------------------
	// Summary:
	//      This enum defines some printer properties IDs.
	// See Also: GetName
	//-------------------------------------------------------------------------
	enum XTPEnumDeviceName
	{
		xtpDevName_Driver = 0,  // Define Driver name property.
		xtpDevName_Device = 1,  // Define Device name property.
		xtpDevName_Port   = 2   // Define Port name property.
	};

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to retrieve currently selected printer
	//      Driver name, Device name or Port name.
	// Parameters:
	//      eNameID - [in] A value from XTPEnumDeviceName enum which specify
	//                property.
	// Returns:
	//      A string with requested name.
	// See Also: XTPEnumDeviceName
	//-----------------------------------------------------------------------
	CString GetName(XTPEnumDeviceName eNameID);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function returns name offset by name ID.
	// Parameters:
	//      pDevNames - [in] A pointer to DEVNAMES structure.
	//      eNameID   - [in] Name ID.
	// Returns:
	//      name Offset in chars.
	// See Also: XTPEnumDeviceName, DEVNAMES
	//-----------------------------------------------------------------------
	WORD _GetNameOffset(LPDEVNAMES pDevNames, XTPEnumDeviceName eNameID);

};


//===========================================================================
// Summary:
//      This class used to define printed page header or footer.
//      Also some calculation and drawing functionality are provided.
//      Used as member of printing options.
// See Also:
//      CXTPPrintOptions
//===========================================================================
class _XTP_EXT_CLASS CXTPPrintPageHeaderFooter : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPPrintOptions;
	DECLARE_DYNAMIC(CXTPPrintPageHeaderFooter)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//      Object constructor.
	// Parameters:
	//      pOwner  - A pointer to owner object.
	//      bHeader - If TRUE object represents a Header, otherwise footer.
	// See Also:
	//      CXTPPrintOptions
	//-----------------------------------------------------------------------
	CXTPPrintPageHeaderFooter(CXTPPrintOptions* pOwner, BOOL bHeader);

	//-------------------------------------------------------------------------
	// Summary: Default object destructor.
	// See Also: CXTPPrintPageHeaderFooter
	//-------------------------------------------------------------------------
	virtual ~CXTPPrintPageHeaderFooter();

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to format footer or header using
	//      format string specified in m_strFormatString member.
	//      The formatted text is copied in following members:
	//          m_strLeft, m_strCenter, m_strRight.
	//      If m_strFormatString is empty - FormatTexts do nothing.
	// Parameters:
	//      pInfo        - [in] Printing context.
	//      pcszWndTitle - [in] Pointer to window title (for &w format string specifier).
	// Remarks:
	//      FormatTexts should be called before Draw call.
	//      Format specifiers the same as in Internet Explorer:
	//  [ul]
	//  [li]<b>&d</b>  Date in short format (as specified by Regional Settings in Control Panel)
	//  [li]<b>&D</b>  Date in long format (as specified by Regional Settings in Control Panel)
	//
	//  [li]<b>&t</b>  Time in the format specified by Regional Settings in Control Panel
	//  [li]<b>&T</b>  Time in 24-hour format
	//
	//  [li]<b>&p</b>  Current page number
	//  [li]<b>&P</b>  Total number of pages
	//
	//  [li]<b>&b</b>  The text immediately following these characters as centered.
	//  [li]<b>&b&b</b>    The text immediately following the first "&b" as centered, and the text following the second "&b" as right-justified.
	//
	//  [li]<b>&w</b>  Window title
	//
	//  [li]<b>&&</b>  A single ampersand (&)
	//  [li]<b>\n</b>  new line marker
	//  [/ul]
	// See Also: m_strFormatString, m_strLeft, m_strCenter, m_strRight
	//-----------------------------------------------------------------------
	virtual void FormatTexts(CPrintInfo* pInfo, LPCTSTR pcszWndTitle);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to draw header or footer on the device
	//      context.
	// Parameters:
	//      pDC             - [in] A pointer to device context.
	//      rcRect          - [in, out] A reference to bounding rectangle.
	//                        After function call this member contains rect
	//                        necessary to draw header or footer. Header is
	//                        aligned to top of passed bounding rectangle,
	//                        footer to bottom.
	//      bCalculateOnly  - If this parameter TRUE - the only rectangle
	//                        calculated, without drawing.
	// See Also: FormatTexts, m_strLeft, m_strCenter, m_strRight
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC, CRect& rcRect, BOOL bCalculateOnly = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to copy members from specified source object.
	// Parameters:
	//      pSrc - A pointer to source object to copy data.
	// See Also: Clear
	//-----------------------------------------------------------------------
	virtual void Set(const CXTPPrintPageHeaderFooter* pSrc);

	//-------------------------------------------------------------------------
	// Summary:
	//      Call this member function to clear all members values.
	// See Also: Set
	//-------------------------------------------------------------------------
	virtual void Clear();

	LOGFONT m_lfFont;           // Font details
	COLORREF m_clrColor;        // Text color
	CString m_strFormatString;  // Text as Format string

	CString m_strLeft;    // A string to show with left align
	CString m_strCenter;  // A string to show with center align
	CString m_strRight;   // A string to show with right align

	//{{AFX_CODEJOCK_PRIVATE
	static CString GetParentFrameTitle(CWnd* pWnd);
	static void DoInsertHFFormatSpecifierViaMenu(CWnd* pParent, CEdit* pEdit, CButton* pButton);
	//}}AFX_CODEJOCK_PRIVATE

protected:
	BOOL m_bHeader;             // If TRUE object represents a Header, otherwise footer.
	CXTPPrintOptions* m_pOwner; // Store pointer to owner object.

	//{{AFX_CODEJOCK_PRIVATE
	virtual int Calc3ColSizes(CDC* pDC, int nW, const CString& str1, const CString& str2,
					  const CString& str3, CSize& rsz1, CSize& rsz2, CSize& rsz3);
	virtual int _Calc3ColSizesIfSingleCol(CDC* pDC, int nW,
					  const CString& str1, const CString& str2, const CString& str3,
					  CSize& rsz1, CSize& rsz2, CSize& rsz3);

	virtual void _SplitFormatLCR(CString strFormat);
	virtual void _FormatDateTime(CString& strFormat, LCID lcidLocale);
	//}}AFX_CODEJOCK_PRIVATE


private:
	int fnYi(int Xi, int Wi);
};

//-------------------------------------------------------------------------
// Summary:
//      This class used as a base class to store printing options.
//      It contains a base properties and operations for printing tasks.
// See Also: CXTPPrintPageHeaderFooter
//-------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPPrintOptions : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPPrintOptions)

public:

	//-------------------------------------------------------------------------
	// Summary: Default Object constructor.
	//-------------------------------------------------------------------------
	CXTPPrintOptions();

	//-------------------------------------------------------------------------
	// Summary: Default Object destructor.
	//-------------------------------------------------------------------------
	virtual ~CXTPPrintOptions();

	CRect        m_rcMargins;   // Margins (mm*100 or inches*1000)

	BOOL    m_bBlackWhitePrinting; // Store printing mode: Color or Black&White.
	int     m_nBlackWhiteContrast; // Black&White printing contrast: 0 ... 255. (default value is 0)

	//-------------------------------------------------------------------------
	// Summary:
	//      Returns a page Header properties.
	// See Also: GetPageFooter
	//-------------------------------------------------------------------------
	CXTPPrintPageHeaderFooter* GetPageHeader();

	//-------------------------------------------------------------------------
	//      Returns a page Footer properties.
	// See Also: GetPageHeader
	//-------------------------------------------------------------------------
	CXTPPrintPageHeaderFooter* GetPageFooter();

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to copy members from specified source object.
	// Parameters:
	//      pSrc - A pointer to source object to copy data.
	//-----------------------------------------------------------------------
	void Set(const CXTPPrintOptions* pSrc);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to determine margins measure:
	//      inches or millimeters (mm*100 or inches*1000).
	// Returns:
	//      Returns TRUE if margins measure in inches, FALSE otherwise.
	// See Also:
	//      m_rcMargins, GetMarginsHimetric, GetMarginsLP
	//-----------------------------------------------------------------------
	virtual BOOL IsMarginsMeasureInches();

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to get margins in Himetric units.
	// Returns:
	//      Returns margins in Himetric units.
	// See Also:
	//      m_rcMargins, GetMarginsLP
	//-----------------------------------------------------------------------
	virtual CRect GetMarginsHimetric();

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to get margins in Logical points.
	// Parameters:
	//      pDC - [in] A pointer to device context.
	// Returns:
	//      Returns margins in Logical units.
	// See Also:
	//      m_rcMargins, GetMarginsHimetric
	//-----------------------------------------------------------------------
	virtual CRect GetMarginsLP(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//      Override this member function in derived class to specify active
	//      locale ID.
	// Returns:
	//      Locale ID. Base implementation returns LOCALE_USER_DEFAULT;
	//-----------------------------------------------------------------------
	virtual LCID GetActiveLCID();

protected:

	CXTPPrintPageHeaderFooter* m_pPageHeader; // A page Header properties.
	CXTPPrintPageHeaderFooter* m_pPageFooter; // A page Footer properties.

};


//{{AFX_CODEJOCK_PRIVATE
/////////////////////////////////////////////////////////////////////////////
// Printing Dialog

class CXTPPrintingDialog : public CDialog
{
public:

	enum { IDD = AFX_IDD_PRINTDLG };

	CXTPPrintingDialog(CWnd* pParent);
	virtual ~CXTPPrintingDialog() { }

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
};
//}}AFX_CODEJOCK_PRIVATE

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CXTPDrawHelpers::IsFastFillAvailable() const {
	return m_pfnFastGradientFill != NULL;
}
AFX_INLINE void CXTPDrawHelpers::Triangle(CDC* pDC, CPoint pt0, CPoint pt1, CPoint pt2) {
	CPoint pts[] = {pt0, pt1, pt2};
	pDC->Polygon(pts, 3);
}
AFX_INLINE void CXTPSplitterTracker::SetBoundRect(CRect rcBoundRect) {
	m_rcBoundRect = rcBoundRect;
}

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPDRAWHELPERS_H__)
