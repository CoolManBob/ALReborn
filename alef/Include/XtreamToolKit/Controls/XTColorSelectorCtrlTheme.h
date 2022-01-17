// XTColorSelectorCtrlTheme.h: interface for the XTColorSelectorCtrlTheme class.
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

#if !defined(__XTCOLORSELECTORCTRLTHEME_H__)
#define __XTCOLORSELECTORCTRLTHEME_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTThemeManager.h"
#include "XTColorSelectorCtrl.h"

class CXTColorSelectorCtrl;

DECLARE_THEME_FACTORY(CXTColorSelectorCtrlTheme)

//===========================================================================
// Summary:
//     CXTColorSelectorCtrlTheme is derived from CXTThemeManagerStyle to
//     handle theme specific visualizations for the CXTColorSelectorCtrl
//     class.
//===========================================================================
class _XTP_EXT_CLASS CXTColorSelectorCtrlTheme : public CXTThemeManagerStyle
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTColorSelectorCtrlTheme object.
	//-----------------------------------------------------------------------
	CXTColorSelectorCtrlTheme();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme manager to refresh
	//     the visual styles used by each components theme.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the background of the
	//          ColorSelector control.
	// Parameters:
	//      pDC    - A pointer to a valid device context.
	//      rect   - A  CRect object that contains the dimensions of the area to fill.
	//      pOwner - A pointer to a valid CXTColorSelectorCtrl object.
	//-----------------------------------------------------------------------
	virtual void FillBackground(CDC* pDC, const CRect& rect, CXTColorSelectorCtrl* pOwner);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw the cell specified by
	//     'pColorCell'.
	// Parameters:
	//     pColorCell - An COLOR_CELL object.
	//     pDC        - A CDC pointer that represents the current device
	//                  context.
	//     bHilite    - TRUE if mouse over cell
	//     bPressed   - TRUE if cell pressed
	//-----------------------------------------------------------------------
	virtual void DrawColorCell(CXTColorSelectorCtrl::COLOR_CELL* pColorCell, CDC* pDC, BOOL bHilite, BOOL bPressed);

protected:
	COLORREF m_crBorderHilight; // RGB value for 3D border highlight color.
	COLORREF m_crBorderShadow; // RGB value for 3D border shadow color.
	COLORREF m_crBack; // RGB value for background color.
	COLORREF m_crPressed; // RGB value for pressed background color.
	COLORREF m_crHilight; // RGB value for hilight background color.
	COLORREF m_crChecked; // RGB value for checked background color.
	COLORREF m_crText; // RGB value for text color.
};

//===========================================================================
// Summary:
//     CXTColorSelectorCtrlThemeOfficeXP is derived from CXTColorSelectorCtrlTheme to
//     handle theme specific visualizations for the CXTColorSelectorCtrl
//     class.
//===========================================================================
class _XTP_EXT_CLASS CXTColorSelectorCtrlThemeOfficeXP : public CXTColorSelectorCtrlTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTColorSelectorCtrlThemeOfficeXP object.
	//-----------------------------------------------------------------------
	CXTColorSelectorCtrlThemeOfficeXP();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme manager to refresh
	//     the visual styles used by each components theme.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();
};

//===========================================================================
// Summary:
//     CXTColorSelectorCtrlThemeOffice2003 is derived from CXTColorSelectorCtrlThemeOfficeXP to
//     handle theme specific visualizations for the CXTColorSelectorCtrl
//     class.
//===========================================================================
class _XTP_EXT_CLASS CXTColorSelectorCtrlThemeOffice2003 : public CXTColorSelectorCtrlThemeOfficeXP
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTColorSelectorCtrlThemeOffice2003 object.
	//-----------------------------------------------------------------------
	CXTColorSelectorCtrlThemeOffice2003();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the theme manager to refresh
	//     the visual styles used by each components theme.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();
};

#endif // !defined(__XTCOLORSELECTORCTRLTHEME_H__)
