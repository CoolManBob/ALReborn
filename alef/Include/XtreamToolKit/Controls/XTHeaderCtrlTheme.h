// XTHeaderCtrlTheme.h: interface for the CXTHeaderCtrlTheme class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#if !defined(__XTHEADERCTRLTHEME_H__)
#define __XTHEADERCTRLTHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTThemeManager.h"
#include "Common/XTPWinThemeWrapper.h"

class CXTHeaderCtrl;

DECLARE_THEME_FACTORY(CXTHeaderCtrlTheme)

// -----------------------------------------------------------------------
// Summary:
//     CXTHeaderCtrlTheme is a theme class used to draw windows classic style
//     theme for the header control.<p/>
// Remarks:
//     The CXTHeaderCtrlTheme class provides theme capability to the CXTHeaderCtrl
//     class. You can derive a class from CXTHeaderCtrlTheme to custom user
//     defined themes.<p/>
//
//     To use this class, first subclass your header control using the
//     Windows API SubclassWindow and a CXTHeaderCtrl base class. Once
//     you have done this you can call SetTheme and pass in a pointer to
//     the theme you wish to use for the header control.
// -----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTHeaderCtrlTheme : public CXTThemeManagerStyle
{
public:

	//--------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTHeaderCtrlTheme object using the specified
	//     header control.
	//--------------------------------------------------------------------
	CXTHeaderCtrlTheme();

	//--------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTHeaderCtrlTheme object, handles cleanup and
	//     deallocation.
	//--------------------------------------------------------------------
	virtual ~CXTHeaderCtrlTheme();

public:

	//--------------------------------------------------------------------
	// Summary:
	//     Call this member function to return a pointer to the Windows
	//     theme API wrapper for this control.
	// Returns:
	//     A CXTPWinThemeWrapper pointer.
	//--------------------------------------------------------------------
	CXTPWinThemeWrapper* GetThemeWrapper();

	// ---------------------------------------------------------------------------------
	// Summary:
	//     Call this member function from the header control's WM_PAINT
	//     message to perform themed drawing.
	// Parameters:
	//     pDC -          Pointer to a valid device context.
	//     pHeaderCtrl -  Points to a CXTHeaderCtrl object.
	// Remarks:
	//     Called by the CXTHeaderCtrlTheme class to render the header for the CXTHeaderCtrl
	//     \object.
	// ---------------------------------------------------------------------------------
	virtual void DrawHeader(CDC* pDC, CXTHeaderCtrl* pHeaderCtrl);

	//--------------------------------------------------------------------
	// Summary:
	//     Call this member whenever the system colors need to be updated.
	//--------------------------------------------------------------------
	virtual void RefreshMetrics();

	//--------------------------------------------------------------------
	// Summary:
	//     This member function can be called to determine if Windows XP
	//     themes can be displayed for the header control.
	// Parameters:
	//     pHeaderCtrl -  Points to a CXTHeaderCtrl object.
	// Returns:
	//     TRUE if Windows themes can be displayed, otherwise FALSE.
	//--------------------------------------------------------------------
	virtual BOOL UseWinXPThemes(CXTHeaderCtrl* pHeaderCtrl);

	// ---------------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the desired XTTHEME_ drawing styles
	//     for the header theme.
	// Parameters:
	//     dwDrawStyle -  The desired styles for the header control theme. Can be
	//                    one or more of the values listed in the remarks section.
	//     pHeaderCtrl -  Pointer to a CXTHeaderCtrl object.
	// Remarks:
	//     Does not affect the WS_ (window style) settings. Styles to be
	//     added or removed can be combined by using the bitwise OR (|)
	//     operator. It can be one or more of the following:
	//
	//     * <b>XTTHEME_WINXPTHEMES</b> Allows the header control to
	//           display Windows XP themes when they are available.
	//     * <b>XTTHEME_HOTTRACKING</b> Allows the header control to
	//           display mouse over effects for the header control.
	//     * <b>XTTHEME_SORTARROW</b> Allows the header control to
	//           display a sort arrow to indicate sort order.
	// See Also:
	//     GetDrawStyle
	// ---------------------------------------------------------------------------
	virtual void SetDrawStyle(DWORD dwDrawStyle, CXTHeaderCtrl* pHeaderCtrl);

	//--------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the current drawing style
	//     used by the header theme.
	// Returns:
	//     The current XTTHEME_ (xt theme) settings for the header control.
	//     See SetDrawStyle for the complete list of available styles.
	// See Also:
	//     SetDrawStyle
	//--------------------------------------------------------------------
	virtual DWORD GetDrawStyle() const;

	//--------------------------------------------------------------------
	// Summary:
	//     Retrieves the size and position of a header control within a
	//     given rectangle. This function is used to determine the
	//     appropriate dimensions for a new header control that is to
	//     occupy the given rectangle.
	// Parameters:
	//     lpHDL - Pointer to an HDLAYOUT structure, which contains
	//             information used to set the size and position of a
	//             header control.
	//     pHeaderCtrl -  Points to a CXTHeaderCtrl object.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//--------------------------------------------------------------------
	virtual BOOL Layout(LPHDLAYOUT lpHDL, CXTHeaderCtrl* pHeaderCtrl);

	// -------------------------------------------------------------------
	// Summary:
	//     Cleans up allocated GDI resources used by the theme.
	// Parameters:
	//     pHeaderCtrl -  Points to a CXTHeaderCtrl object.
	// Remarks:
	//     The CXTHeaderCtrlTheme class calls this member function to free GDI
	//     resources that have been allocated during draw routines.
	// -------------------------------------------------------------------
	virtual void CleanUp(CXTHeaderCtrl* pHeaderCtrl);

	//--------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the bitmap image for the
	//     specified header item.
	// Parameters:
	//     iCol      - Zero-based index of the column to set the sort
	//                 image for.
	//     uBitmapID - Resource ID of the bitmap to use.
	//     bRemove   - TRUE to remove a bitmap, FALSE if adding a bitmap.
	//     crMask    - Color used to generate the mask. Each pixel of
	//                 this color in the given bitmap is changed to black
	//                 and the corresponding bit in the mask is set to one.
	//     pHeaderCtrl -  Points to a CXTHeaderCtrl object.
	// Returns:
	//     TRUE if successful; otherwise FALSE.
	//--------------------------------------------------------------------
	virtual BOOL SetBitmap(int iCol, UINT uBitmapID, BOOL bRemove, COLORREF crMask, CXTHeaderCtrl* pHeaderCtrl);

	//--------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the header control displays
	//     a sort arrow.
	// Returns:
	//     TRUE if the header control displays a sort arrow, otherwise returns FALSE.
	//--------------------------------------------------------------------
	BOOL HasSortArrow();

protected:

	// -------------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme to draw the header
	//     control's background.
	// Parameters:
	//     lpDIS :  A long pointer to a DRAWITEMSTRUCT structure. The structure
	//              contains information about the item to be drawn and the type
	//              of drawing required.
	// -------------------------------------------------------------------------
	virtual void OnDrawBackground(LPDRAWITEMSTRUCT lpDIS);

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme to draw the header
	//     control's background using Windows themes.
	// Parameters:
	//     lpDIS - A long pointer to a DRAWITEMSTRUCT structure. The
	//             structure contains information about the item to be
	//             drawn and the type of drawing required.
	// Returns:
	//     TRUE if successful; otherwise FALSE.
	//--------------------------------------------------------------------
	virtual BOOL OnDrawBackgroundWinThemed(LPDRAWITEMSTRUCT lpDIS);

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme to draw the background
	//     for the header item specified by lpDIS->itemID.
	// Parameters:
	//     lpDIS - A long pointer to a DRAWITEMSTRUCT structure. The
	//             structure contains information about the item to be
	//             drawn and the type of drawing required.
	//--------------------------------------------------------------------
	virtual void OnDrawItemBackground(LPDRAWITEMSTRUCT lpDIS);

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme to draw the background
	//     using Windows themes for the header item specified by lpDIS->itemID.
	// Parameters:
	//     lpDIS - A long pointer to a DRAWITEMSTRUCT structure. The
	//             structure contains information about the item to be
	//             drawn and the type of drawing required.
	// Returns:
	//     TRUE if successful; otherwise FALSE.
	//--------------------------------------------------------------------
	virtual BOOL OnDrawItemBackgroundWinThemed(LPDRAWITEMSTRUCT lpDIS);

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme to draw the sort
	//     used to display sort order for header item specified by lpDIS->itemID.
	// Parameters:
	//     lpDIS      - A long pointer to a DRAWITEMSTRUCT structure. The
	//                  structure contains information about the item to be
	//                  drawn and the type of drawing required.
	//     bAscending - TRUE if the arrow is to be draw ascending.
	//--------------------------------------------------------------------
	virtual void OnDrawItemSortArrow(LPDRAWITEMSTRUCT lpDIS, BOOL bAscending);

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw the text for the header
	//     item specified by lpDIS->itemID.
	// Parameters:
	//     lpDIS - A long pointer to a DRAWITEMSTRUCT structure. The
	//             structure contains information about the item to be
	//             drawn and the type of drawing required.
	//     pHeaderCtrl -  Points to a CXTHeaderCtrl object.
	//--------------------------------------------------------------------
	virtual void OnDrawItemText(LPDRAWITEMSTRUCT lpDIS, CXTHeaderCtrl* pHeaderCtrl);

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw a bitmap for the header
	//     item specified by lpDIS->itemID.
	// Parameters:
	//     lpDIS  - A long pointer to a DRAWITEMSTRUCT structure. The
	//              structure contains information about the item to be
	//              drawn and the type of drawing required.
	//     rcText - Size of the area where the header item text is displayed.
	//     pHeaderCtrl -  Points to a CXTHeaderCtrl object.
	//--------------------------------------------------------------------
	virtual void OnDrawItemBitmap(LPDRAWITEMSTRUCT lpDIS, CRect& rcText, CXTHeaderCtrl* pHeaderCtrl);

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by CXTHeaderCtrlTheme::OnDrawItemSortArrow
	//     to draw the sort arrow.
	// Parameters:
	//     pDC        - Pointer to a valid device context.
	//     ptsArrow   - POINT array used to define the 3 points of the arrow drawn.
	//     bAscending - TRUE if the arrow is to be draw ascending.
	//--------------------------------------------------------------------
	virtual void DrawSortArrow(CDC* pDC, LPPOINT ptsArrow, BOOL bAscending);

protected:
	int                 m_iArrowPadding; // Spacing between text and sort arrow.
	DWORD               m_dwDrawStyle;   // Style flags used by theme, see CXTHeaderCtrlTheme::SetDrawStyle().
	BOOL                m_bUseWinThemes; // TRUE to use Windows XP themes when available.
	CSize               m_sizePadding;   // Leading and trailing padding for text display.
	CSize               m_sizeArrow;     // Width and height of sort arrow.
	COLORREF            m_crText;        // RGB value for text color.
	COLORREF            m_cr3DFace;      // RGB value for header background color.
	COLORREF            m_cr3DShadow;    // RGB value for border shadow color.
	COLORREF            m_cr3DDkShadow;  // RGB value for border dark shadow color.
	COLORREF            m_cr3DHighLight; // RGB value for border highlight color.
	CXTPWinThemeWrapper m_themeWrapper;  // Wraps the Windows XP theme API that is used to display Windows XP themes.
};

//{{AFX_CODEJOCK_PRIVATE
#define CXTHeaderTheme CXTHeaderCtrlTheme // deprecated
//}}AFX_CODEJOCK_PRIVATE


AFX_INLINE CXTPWinThemeWrapper* CXTHeaderCtrlTheme::GetThemeWrapper() {
	return &m_themeWrapper;
}

// -------------------------------------------------------------------------------
// Summary:
//     CXTHeaderCtrlThemeOfficeXP is a CXTHeaderCtrlTheme derived theme class
//     used to draw an Office XP style theme for the header control.<p/>
// Remarks:
//     The CXTHeaderCtrlThemeOfficeXP class provides theme capability to the CXTHeaderCtrl
//     class. You can derive a class from CXTHeaderCtrlThemeOfficeXP to
//     custom user defined themes.<p/>
//
//     To use this class, first subclass your header control using the
//     Windows API SubclassWindow and a CXTHeaderCtrl base class. Once
//     you have done this you can call SetTheme and pass in a pointer to
//     the theme you wish to use for the header control.
// -------------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTHeaderCtrlThemeOfficeXP : public CXTHeaderCtrlTheme
{
public:

	//--------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTHeaderCtrlThemeOfficeXP object using the specified
	//     header control.
	// Parameters:
	//     pHeaderCtrl - Pointer to the header control to be themed.
	//--------------------------------------------------------------------
	CXTHeaderCtrlThemeOfficeXP();

	//--------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTHeaderCtrlThemeOfficeXP object, handles cleanup and
	//     deallocation.
	//--------------------------------------------------------------------
	virtual ~CXTHeaderCtrlThemeOfficeXP();

protected:

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme to draw the header
	//     control's background.
	// Parameters:
	//     lpDIS - A long pointer to a DRAWITEMSTRUCT structure. The
	//             structure contains information about the item to be
	//             drawn and the type of drawing required.
	//--------------------------------------------------------------------
	virtual void OnDrawBackground(LPDRAWITEMSTRUCT lpDIS);

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme to draw the background
	//     for the header item specified by lpDIS->itemID.
	// Parameters:
	//     lpDIS - A long pointer to a DRAWITEMSTRUCT structure. The
	//             structure contains information about the item to be
	//             drawn and the type of drawing required.
	//--------------------------------------------------------------------
	virtual void OnDrawItemBackground(LPDRAWITEMSTRUCT lpDIS);
};

//{{AFX_CODEJOCK_PRIVATE
#define CXTHeaderThemeOfficeXP CXTHeaderCtrlThemeOfficeXP // deprecated
//}}AFX_CODEJOCK_PRIVATE

// -----------------------------------------------------------------------
// Summary:
//     CXTHeaderCtrlThemeOffice2003 is a CXTHeaderCtrlTheme derived theme class
//     used to draw an Office 2003 style theme for the header control.<p/>
// Remarks:
//     The CXTHeaderCtrlThemeOffice2003 class provides theme capability to
//     the CXTHeaderCtrl class. You can derive a class from
//     CXTHeaderCtrlThemeOffice2003 to custom user defined themes.<p/>
//
//     To use this class, first subclass your header control using the
//     Windows API SubclassWindow and a CXTHeaderCtrl base class. Once
//     you have done this you can call SetTheme and pass in a pointer to
//     the theme you wish to use for the header control.
// -----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTHeaderCtrlThemeOffice2003 : public CXTHeaderCtrlTheme
{
public:

	//--------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTHeaderCtrlThemeOffice2003 object using the specified
	//     header control.
	// Parameters:
	//     pHeaderCtrl - Pointer to the header control to be themed.
	//--------------------------------------------------------------------
	CXTHeaderCtrlThemeOffice2003();

	//--------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTHeaderCtrlThemeOffice2003 object, handles cleanup and
	//     deallocation.
	//--------------------------------------------------------------------
	virtual ~CXTHeaderCtrlThemeOffice2003();

	//--------------------------------------------------------------------
	// Summary:
	//     Retrieves the size and position of a header control within a
	//     given rectangle. This function is used to determine the
	//     appropriate dimensions for a new header control that is to
	//     occupy the given rectangle.
	// Parameters:
	//     lpHDL - Pointer to anHDLAYOUT structure, which contains
	//             information used to set the size and position of a
	//             header control.
	//     pHeaderCtrl -  Points to a CXTHeaderCtrl object.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//--------------------------------------------------------------------
	virtual BOOL Layout(LPHDLAYOUT lpHDL, CXTHeaderCtrl* pHeaderCtrl);

protected:

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme to draw the header
	//     control's background.
	// Parameters:
	//     lpDIS - A long pointer to a DRAWITEMSTRUCT structure. The
	//             structure contains information about the item to be
	//             drawn and the type of drawing required.
	//--------------------------------------------------------------------
	virtual void OnDrawBackground(LPDRAWITEMSTRUCT lpDIS);

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme to draw the background
	//     for the header item specified by lpDIS->itemID.
	// Parameters:
	//     lpDIS - A long pointer to a DRAWITEMSTRUCT structure. The
	//             structure contains information about the item to be
	//             drawn and the type of drawing required.
	//--------------------------------------------------------------------
	virtual void OnDrawItemBackground(LPDRAWITEMSTRUCT lpDIS);

private:
	COLORREF MixColor(COLORREF clrLight, COLORREF clrDark, double dFactor);
};

//{{AFX_CODEJOCK_PRIVATE
#define CXTHeaderThemeOffice2003 CXTHeaderCtrlThemeOffice2003 // deprecated
//}}AFX_CODEJOCK_PRIVATE

// -------------------------------------------------------------------------------
// Summary:
//     CXTHeaderCtrlThemeExplorer is a CXTHeaderCtrlThemeOffice2003 derived theme
//     class used to draw an Office XP style theme for the header
//     control.<p/>
// Remarks:
//     The CXTHeaderCtrlThemeExplorer class provides theme capability to the CXTHeaderCtrl
//     class. You can derive a class from CXTHeaderCtrlThemeExplorer to
//     custom user defined themes.<p/>
//
//     To use this class, first subclass your header control using the
//     Windows API SubclassWindow and a CXTHeaderCtrl base class. Once
//     you have done this you can call SetTheme and pass in a pointer to
//     the theme you wish to use for the header control.
// -------------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTHeaderCtrlThemeExplorer : public CXTHeaderCtrlThemeOffice2003
{
public:

	//--------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTHeaderCtrlThemeExplorer object using the specified
	//     header control.
	// Parameters:
	//     pHeaderCtrl - Pointer to the header control to be themed.
	//--------------------------------------------------------------------
	CXTHeaderCtrlThemeExplorer();

	//--------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTHeaderCtrlThemeExplorer object, handles cleanup and
	//     deallocation.
	//--------------------------------------------------------------------
	virtual ~CXTHeaderCtrlThemeExplorer();

	//--------------------------------------------------------------------
	// Summary:
	//     Retrieves the size and position of a header control within a
	//     given rectangle. This function is used to determine the
	//     appropriate dimensions for a new header control that is to
	//     occupy the given rectangle.
	// Parameters:
	//     lpHDL - Pointer to anHDLAYOUT structure, which contains
	//             information used to set the size and position of a
	//             header control.
	//     pHeaderCtrl -  Points to a CXTHeaderCtrl object.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//--------------------------------------------------------------------
	virtual BOOL Layout(LPHDLAYOUT lpHDL, CXTHeaderCtrl* pHeaderCtrl);

protected:

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme to draw the background
	//     using Windows themes for the header item specified by lpDIS->itemID.
	// Parameters:
	//     lpDIS - A long pointer to a DRAWITEMSTRUCT structure. The
	//             structure contains information about the item to be
	//             drawn and the type of drawing required.
	// Returns:
	//     TRUE if successful; otherwise FALSE.
	//--------------------------------------------------------------------
	virtual BOOL OnDrawItemBackgroundWinThemed(LPDRAWITEMSTRUCT lpDIS);

	//--------------------------------------------------------------------
	// Summary:
	//     This member function is called by CXTHeaderCtrlTheme::OnDrawItemSortArrow
	//     to draw the sort arrow.
	// Parameters:
	//     pDC        - Pointer to a valid device context.
	//     ptsArrow   - POINT array used to define the 3 points of the arrow drawn.
	//     bAscending - TRUE if the arrow is to be draw ascending.
	//--------------------------------------------------------------------
	virtual void DrawSortArrow(CDC* pDC, LPPOINT ptsArrow, BOOL bAscending);
};

//{{AFX_CODEJOCK_PRIVATE
#define CXTHeaderThemeExplorer CXTHeaderCtrlThemeExplorer // deprecated
//}}AFX_CODEJOCK_PRIVATE

//========================================================================

//{{AFX_CODEJOCK_PRIVATE
#ifndef ODS_HOTLIGHT
#define ODS_HOTLIGHT        0x0040
#endif//ODS_HOTLIGHT
//}}AFX_CODEJOCK_PRIVATE

const DWORD XTTHEME_WINXPTHEMES = 0x0001; //<ALIAS CXTHeaderCtrlTheme::SetDrawStyle@DWORD@CXTHeaderCtrl*>
const DWORD XTTHEME_HOTTRACKING = 0x0002; //<ALIAS CXTHeaderCtrlTheme::SetDrawStyle@DWORD@CXTHeaderCtrl*>
const DWORD XTTHEME_SORTARROW   = 0x0004; //<ALIAS CXTHeaderCtrlTheme::SetDrawStyle@DWORD@CXTHeaderCtrl*>

//========================================================================

AFX_INLINE BOOL CXTHeaderCtrlTheme::HasSortArrow() {
	return ((m_dwDrawStyle & XTTHEME_SORTARROW) == XTTHEME_SORTARROW);
}
AFX_INLINE COLORREF CXTHeaderCtrlThemeOffice2003::MixColor(COLORREF clrLight, COLORREF clrDark, double dFactor) {
	return RGB(
		GetRValue(clrLight) - dFactor * (GetRValue(clrLight) - GetRValue(clrDark)),
		GetGValue(clrLight) - dFactor * (GetGValue(clrLight) - GetGValue(clrDark)),
		GetBValue(clrLight) - dFactor * (GetBValue(clrLight) - GetBValue(clrDark)));
}


//========================================================================

#endif // !defined(__XTHEADERCTRLTHEME_H__)
