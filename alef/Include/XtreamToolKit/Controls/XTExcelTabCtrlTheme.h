// XTExcelTabCtrlTheme.h: interface for the CXTExcelTabCtrlTheme class.
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
#if !defined(__XTEXCELTABCTRLTHEME_H__)
#define __XTEXCELTABCTRLTHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPDrawHelpers.h"
#include "XTThemeManager.h"

class CXTTcbItem;
class CXTExcelTabCtrl;
class CXTExcelTabCtrlButtonState;

DECLARE_THEME_FACTORY(CXTExcelTabCtrlTheme)

//===========================================================================
// Summary:
//     CXTExcelTabCtrlTheme is used to draw the CXTExcelTabCtrl object.  All themes
//     used for CXTExcelTabCtrl should inherit from this base class.
//===========================================================================
class _XTP_EXT_CLASS CXTExcelTabCtrlTheme : public CXTThemeManagerStyle
{
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTExcelTabCtrlTheme object.
	//-------------------------------------------------------------------------
	CXTExcelTabCtrlTheme();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTExcelTabCtrlTheme object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	~CXTExcelTabCtrlTheme();

public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to refresh the flat tab control colors,
	//     position any scroll bars to their default positions, and then
	//     reset the control to the system defaults.
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

public:

	//-------------------------------------------------------------------------
	// Summary:
	//     This member function will draw a tab to the device context specified
	//     by 'pDC'.
	// Parameters:
	//     pDC          - A CDC pointer that represents the current device
	//                    context.
	//     pTabCtrl     - A pointer to a CXTExcelTabCtrl object.
	//     pt           - A CPoint object that specifies the position of
	//                    the XY location of the top left corner of the tab to draw.
	//     bSelected    - True if the tab is currently selected.
	//     pTcbItem     - Tab pointer to render.
	// Returns:
	//     The x position of the next tab to be drawn if successful, otherwise
	//     returns -1;.
	//-------------------------------------------------------------------------
	virtual int DrawTab(CDC* pDC, CXTExcelTabCtrl* pTabCtrl, const CPoint& pt, bool bSelected, CXTTcbItem* pTcbItem);

	//-------------------------------------------------------------------------
	// Summary:
	//     This member function is used by the flat tab control to draw an arrow
	//     button to the device context specified by 'pDC'.
	// Parameters:
	//     pDC          - A CDC pointer that represents the current device
	//                    context.
	//     pTabCtrl     - A pointer to a CXTExcelTabCtrl object.
	//     state - XY location of the top left corner of the tab to draw.
	//-------------------------------------------------------------------------
	virtual void DrawButton(CDC* pDC, CXTExcelTabCtrl* pTabCtrl, CXTExcelTabCtrlButtonState& state);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the background of the button.
	// Parameters:
	//      pDC   - A pointer to a valid device context.
	//      rect  - A CRect object that contains the location and the dimensions of the button.
	//      state - A CXTExcelTabCtrlButtonState that contains the current state of the button.
	//-----------------------------------------------------------------------
	virtual void DrawButtonBack(CDC* pDC, CRect& rect, CXTExcelTabCtrlButtonState& state);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function gets an RGB value that represents the background
	//     color of the tab.
	// Parameters:
	//     pTcbItem - The tab pointer to get background color for.
	// Returns:
	//     An RGB value that represents the tab background color if successful,
	//     otherwise returns COLORREF_NULL.
	//-----------------------------------------------------------------------
	virtual COLORREF GetTabBackColor(CXTTcbItem* pTcbItem) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function gets an RGB value that represents the text color
	//     of the tab.
	// Parameters:
	//     pTcbItem - The tab pointer to get text color for.
	// Returns:
	//     An RGB value that represents the tab text color if successful,
	//     otherwise returns COLORREF_NULL.
	//-----------------------------------------------------------------------
	virtual COLORREF GetTabTextColor(CXTTcbItem* pTcbItem) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function gets an RGB value that represents the background
	//     color for selected tabs.
	// Parameters:
	//     pTcbItem - The tab pointer to get selected background color for.
	// Returns:
	//     An RGB value that represents the selected tabs background color if
	//     successful, otherwise returns COLORREF_NULL.
	//-----------------------------------------------------------------------
	virtual COLORREF GetSelTabBackColor(CXTTcbItem* pTcbItem) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function gets an RGB value that represents the text color
	//     for selected tabs.
	// Parameters:
	//     pTcbItem - The tab pointer to get selected text color for.
	// Returns:
	//     An RGB value that represents the selected tab text color if
	//     successful, otherwise returns COLORREF_NULL.
	//-----------------------------------------------------------------------
	virtual COLORREF GetSelTabTextColor(CXTTcbItem* pTcbItem) const;

	CXTPPaintManagerColor m_clr3DFace;         // RGB value that represents the normal tab face color.
	CXTPPaintManagerColor m_clr3DFacePushed;   // RGB value for pushed background color.
	CXTPPaintManagerColor m_clr3DFaceHilite;   // RGB value for highlighted background color.
	CXTPPaintManagerColor m_clrBtnText;        // RGB value that represents the tab outline color.
	CXTPPaintManagerColor m_clrBtnTextGray;    // RGB value for disabled text color.
	CXTPPaintManagerColor m_clrBtnTextPushed;  // RGB value for highlighted text color.
	CXTPPaintManagerColor m_clrBtnTextHilite;  // RGB value for pushed text color.
	CXTPPaintManagerColor m_clrWindow;         // RGB value that represents the selected tab face color.
	CXTPPaintManagerColor m_clrWindowText;     // RGB value that represents the tab text color.
	CXTPPaintManagerColor m_clr3DHilight;      // RGB value that represents the tab highlight color.
	CXTPPaintManagerColor m_clr3DShadow;       // RGB value that represents the tab shadow color.
	CXTPPaintManagerColor m_clrBorder3DHilite; // RGB value for 3D border highlight color.
	CXTPPaintManagerColor m_clrBorder3DShadow; // RGB value for 3D border shadow color.

	int m_cx; // Width for each arrow button.
	int m_cy; // Height for each arrow button
};

//===========================================================================
// Summary:
//     CXTExcelTabCtrlThemeOfficeXP is used to draw the CXTExcelTabCtrl object.
//===========================================================================
class _XTP_EXT_CLASS CXTExcelTabCtrlThemeOfficeXP : public CXTExcelTabCtrlTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme manager to refresh
	//     the visual styles used by each components theme.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	///-----------------------------------------------------------------------
	// Summary: Call this member function to draw the background of the button.
	// Parameters:
	//      pDC   - A pointer to a valid device context.
	//      rect  - A CRect object that contains the location and the dimensions of the button.
	//      state - A CXTExcelTabCtrlButtonState that contains the current state of the button.
	//-----------------------------------------------------------------------
	virtual void DrawButtonBack(CDC* pDC, CRect& rect, CXTExcelTabCtrlButtonState& state);
};

//===========================================================================
// Summary:
//     CXTExcelTabCtrlThemeOffice2003 is used to draw the CXTExcelTabCtrl object.
//===========================================================================
class _XTP_EXT_CLASS CXTExcelTabCtrlThemeOffice2003 : public CXTExcelTabCtrlThemeOfficeXP
{
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme manager to refresh
	//     the visual styles used by each components theme.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the background of the button.
	// Parameters:
	//      pDC   - A pointer to a valid device context.
	//      rect  - A CRect object that contains the location and the dimensions of the button.
	//      state - A CXTExcelTabCtrlButtonState that contains the current state of the button.
	//-----------------------------------------------------------------------
	virtual void DrawButtonBack(CDC* pDC, CRect& rect, CXTExcelTabCtrlButtonState& state);
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTEXCELTABCTRLTHEME_H__)
