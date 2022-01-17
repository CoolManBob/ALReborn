// XTPPropertyGridPaintManager.h interface for the CXTPPropertyGridPaintManager class.
//
// This file is a part of the XTREME PROPERTYGRID MFC class library.
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
#if !defined(_XTPPROPERTYGRIDPAINTMANAGER_H__)
#define _XTPPROPERTYGRIDPAINTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPWinThemeWrapper.h"
#include "Common/XTPColorManager.h"

class CXTPPropertyGrid;
class CXTPPropertyGridInplaceButton;
class CXTPPropertyGridItem;
class CXTPPropertyGridItemConstraint;

//-----------------------------------------------------------------------
// Summary:
//     Visual theme enumeration
// Example:
//     <code> m_wndPropertyGrid.SetTheme(xtpGridThemeWhidbey); </code>
// See Also: CXTPPropertyGrid::SetTheme, CXTPPropertyGrid::GetCurrentTheme
//
// <KEYWORDS xtpGridThemeDefault, xtpGridThemeNativeWinXP, xtpGridThemeOffice2003, xtpGridThemeCool, xtpGridThemeSimple, xtpGridThemeDelphi, xtpGridThemeWhidbey>
//-----------------------------------------------------------------------
enum XTPPropertyGridPaintTheme
{
	xtpGridThemeDefault,     // Default theme.
	xtpGridThemeNativeWinXP, // WinXP style theme.
	xtpGridThemeOffice2003,  // Office 2003 style theme.
	xtpGridThemeCool,        // Cool theme.
	xtpGridThemeSimple,      // Visual Basic style theme.
	xtpGridThemeDelphi,      // Delphi style theme.
	xtpGridThemeWhidbey,     // Visual Studio 2005 "Whidbey" style theme.
	xtpGridThemeOfficeXP     // Office XP style theme.
};

//-----------------------------------------------------------------------
// Summary:
//     Border Style enumeration
// Example:
//     <code>m_wndPropertyGrid.SetBorderStyle(xtpGridBorderClientEdge);</code>
// See Also: CXTPPropertyGrid::SetBorderStyle
//-----------------------------------------------------------------------
enum XTPPropertyGridBorderStyle
{
	xtpGridBorderNone,      // Without borders
	xtpGridBorderFlat,      // Black flat borders
	xtpGridBorderStaticEdge,// Static edge
	xtpGridBorderClientEdge // Client edge
};

//-----------------------------------------------------------------------
// Summary:
//     InplaceButtons style
// Example:
//     pPaintManager->m_buttonsStyle = xtpGridButtonsOfficeXP;
// See Also: CXTPPropertyGrid::SetTheme, CXTPPropertyGrid::GetCurrentTheme
//-----------------------------------------------------------------------
enum XTPPropertyGridInplaceButtonsStyle
{
	xtpGridButtonsDefault,          // Default button style
	xtpGridButtonsThemed,           // Themed button style
	xtpGridButtonsOfficeXP          // Office XP button style
};


class CXTPPropertyGridPaintManager;

//===========================================================================
// Summary:
//     Standalone class used by CXTPPropertyGridPaintManager as holder of item metrics
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridItemMetrics : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridItemMetrics object.
	// Parameters:
	//     pPaintManager - Parent CXTPPropertyGridPaintManager object
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemMetrics(CXTPPropertyGridPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Resets all color values to its defaults.
	//-----------------------------------------------------------------------
	virtual void SetDefaultValues();

public:
	CFont                         m_fontNormal;         // Normal font.
	CFont                         m_fontBold;           // Bold font.
	int                           m_nImage;             // Image index.

	CXTPPaintManagerColor         m_clrHelpBack;        // Color of the description background.
	CXTPPaintManagerColor         m_clrHelpFore;        // Color of the description text.

	CXTPPaintManagerColor         m_clrLine;            // Line color.
	CXTPPaintManagerColor         m_clrFore;            // Color of the item's text.
	CXTPPaintManagerColor         m_clrCategoryFore;    // Color of the category text.
	CXTPPaintManagerColor         m_clrBack;            // Background color.
	CXTPPaintManagerColor         m_clrReadOnlyFore;    // Color of read-only text.
	CXTPPaintManagerColor         m_clrVerbFace;        // Text color of verbs.
	UINT                          m_uDrawTextFormat;    // Text formatting style for the property item.
	int                           m_nMaxLength;         // Maximum number of characters that can be entered into an editable item (Edit limit).

protected:
	CXTPPropertyGridPaintManager* m_pPaintManager;          // Pointer to the paint manager used to draw the property grid.

};


//===========================================================================
// Summary:
//     Standalone class used by CXTPPropertyGrid as the paint manager.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridPaintManager
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridPaintManager object.
	// Parameters:
	//     pGrid - Points to a CXTPPropertyGrid object
	//-----------------------------------------------------------------------
	CXTPPropertyGridPaintManager(CXTPPropertyGrid* pGrid);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropertyGridPaintManager object, handles cleanup
	//     and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridPaintManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get item text color
	// Parameters:
	//     pItem - Item which text color need to get
	//     bValuePart - TRUE if value part color needed
	// Returns:
	//     COLORREF of item text.
	//-----------------------------------------------------------------------
	virtual COLORREF GetItemTextColor(CXTPPropertyGridItem* pItem, BOOL bValuePart);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get item back color
	// Parameters:
	//     pItem - Item which back color need to get
	//     bValuePart - TRUE if value part color needed
	// Returns:
	//     COLORREF of item back.
	//-----------------------------------------------------------------------
	virtual COLORREF GetItemBackColor(CXTPPropertyGridItem* pItem, BOOL bValuePart);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get item font
	// Parameters:
	//     pItem - Item which font color need to get
	//     bValuePart - TRUE if value part font needed
	// Returns:
	//     Pointer to CFont object containing font of the item
	//-----------------------------------------------------------------------
	virtual CFont* GetItemFont(CXTPPropertyGridItem* pItem, BOOL bValuePart);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw property grid background.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//-----------------------------------------------------------------------
	virtual void FillPropertyGrid(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw view background.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//-----------------------------------------------------------------------
	virtual void FillPropertyGridView(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw grid borders
	// Parameters:
	//      pDC - Pointer to a valid device context
	//      rcBorder - Bounding rectangle
	//      bAdjustRect - TURE to adjust bounding rectangle
	// See Also: FillPropertyGridView
	//-----------------------------------------------------------------------
	virtual void DrawPropertyGridBorder(CDC* pDC, RECT& rcBorder, BOOL bAdjustRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw single item of the grid.
	// Parameters:
	//     lpDrawItemStruct - A long pointer to a DRAWITEMSTRUCT structure
	//     that contains information about the type of drawing required.
	//-----------------------------------------------------------------------
	virtual void DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw specified constraint in in-place list.
	// Parameters:
	//     pDC       - Pointer to a valid device context
	//     rc        - Bounding rectangle of the constraint
	//     bSelected - TRUE if constraint is currently selected.
	//     pConstraint - Constraint to draw.
	//-----------------------------------------------------------------------
	virtual void DrawInplaceListItem(CDC* pDC, CXTPPropertyGridItemConstraint* pConstraint, CRect rc, BOOL bSelected);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get rectangle of value part of the item.
	// Parameters:
	//     pItem - Property Grid item
	//     rcValue - Rectangle of the value part
	//-----------------------------------------------------------------------
	virtual void AdjustItemValueRect(CXTPPropertyGridItem* pItem, CRect& rcValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get rectangle of caption part of the item.
	// Parameters:
	//     pItem - Property Grid item
	//     rcCaption - Rectangle of the caption part
	//-----------------------------------------------------------------------
	virtual void AdjustItemCaptionRect(CXTPPropertyGridItem* pItem, CRect& rcCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this method and fill in the MEASUREITEMSTRUCT structure
	//     to inform Windows of the list-box dimensions.
	// Parameters:
	//     lpMeasureItemStruct - Specifies a long pointer to a MEASUREITEMSTRUCT
	//     structure.
	//-----------------------------------------------------------------------
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw in-place button of the grid.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     pButton - Points to a CXTPPropertyGridInplaceButton object
	//-----------------------------------------------------------------------
	virtual void FillInplaceButton(CDC* pDC, CXTPPropertyGridInplaceButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if a verb is at a specific point.
	// Parameters:
	//     pDC     - Reference to a valid device context
	//     rcVerbs - Bounding rectangle of all verbs.
	//     pt      - Point to test.
	// Returns:
	//     Index of verb within collection if the point was on a verb,
	//     otherwise -1.
	//-----------------------------------------------------------------------
	int HitTestVerbs(CDC* pDC, CRect rcVerbs, CPoint pt);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves default item metrics
	// Returns:
	//     Call this method to get default item metrics
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemMetrics* GetItemMetrics() const;


public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw expand buttons of the grid.
	// Parameters:
	//     dc - Reference to a valid device context
	//     pItem - Points to a CXTPPropertyGridItem object
	//     rcCaption - Caption button rectangle.
	//-----------------------------------------------------------------------
	virtual void DrawExpandButton(CDC& dc, CXTPPropertyGridItem* pItem, CRect rcCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to draw the bounding rectangle for a
	//     category caption.
	// Parameters:
	//     pDC - Reference to a valid device context
	//     rc  - Bounding rectangle of Category's caption.
	//-----------------------------------------------------------------------
	virtual void DrawCategoryCaptionBackground(CDC* pDC, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw value part of the property grid item
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pItem   - Property Grid item to draw
	//     rcValue - Bounding rectangle of the value part
	//-----------------------------------------------------------------------
	virtual void DrawItemValue(CDC* pDC, CXTPPropertyGridItem* pItem, CRect rcValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw caption part of the property grid item
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pItem   - Property Grid item to draw
	//     rcCaption - Bounding rectangle of the caption part
	//-----------------------------------------------------------------------
	virtual void DrawItemCaption(CDC* pDC, CXTPPropertyGridItem* pItem, CRect rcCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw inplace buttons if the item
	// Parameters:
	//     pDC     - Pointer to a valid device context
	//     pItem   - Property Grid item to draw
	//     rcValue - Value bounding rectangle
	//-----------------------------------------------------------------------
	virtual void DrawInplaceButtons(CDC* pDC, CXTPPropertyGridItem* pItem, CRect rcValue);

public:
	XTPPropertyGridInplaceButtonsStyle m_buttonsStyle;   // TRUE to use WinXP themes.

	CXTPWinThemeWrapper m_themeTree;                // Themes for Tree controls.
	CXTPWinThemeWrapper m_themeButton;              // Themes for button controls
	CXTPWinThemeWrapper m_themeCombo;               // Themes for combo button.

protected:
	CXTPPropertyGrid* m_pGrid;                      // Parent grid class.
	CFont m_fntVerbUnderline;                       // Font used to display the caption of verbs with underline
	CFont m_fntVerbNormal;                          // Font used to display the caption of verbs without underline

	CXTPPropertyGridItemMetrics* m_pMetrics;        // Default metrics of the item

	COLORREF m_clrFace;                             // Face color.
	COLORREF m_clrShadow;                           // Shadow color.

	friend class CXTPPropertyGrid;
};

namespace XTPPropertyGridPaintThemes
{

	//===========================================================================
	// Summary:
	//     CXTPPropertyGridOffice2003Theme is a CXTPPropertyGridPaintManager derived
	//     class.  This represents an Office 2003 style theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPPropertyGridOffice2003Theme : public CXTPPropertyGridPaintManager
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPPropertyGridOffice2003Theme object.
		// Parameters:
		//     pGrid - Points to a CXTPPropertyGrid object
		//-----------------------------------------------------------------------
		CXTPPropertyGridOffice2003Theme(CXTPPropertyGrid* pGrid);

		//-------------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the
		//     property grid.
		//-------------------------------------------------------------------------
		virtual void RefreshMetrics();


	protected:
		BOOL m_bLunaTheme;  // TRUE to use luna colors, FALSE otherwise
	};

	//===========================================================================
	// Summary:
	//     CXTPPropertyGridNativeXPTheme is a CXTPPropertyGridPaintManager derived
	//     class.  This represents a Native Windows XP style theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPPropertyGridNativeXPTheme : public CXTPPropertyGridPaintManager
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPPropertyGridNativeXPTheme object.
		// Parameters:
		//     pGrid - Points to a CXTPPropertyGrid object
		//-----------------------------------------------------------------------
		CXTPPropertyGridNativeXPTheme(CXTPPropertyGrid* pGrid);

		//-------------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the
		//     property grid.
		//-------------------------------------------------------------------------
		virtual void RefreshMetrics();
	};

	//===========================================================================
	// Summary:
	//     CXTPPropertyGridOfficeXP is a CXTPPropertyGridPaintManager derived
	//     class.  This represents a Office XP style theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPPropertyGridOfficeXP : public CXTPPropertyGridPaintManager
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPPropertyGridNativeXPTheme object.
		// Parameters:
		//     pGrid - Points to a CXTPPropertyGrid object
		//-----------------------------------------------------------------------
		CXTPPropertyGridOfficeXP(CXTPPropertyGrid* pGrid);

		//-------------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the
		//     property grid.
		//-------------------------------------------------------------------------
		void RefreshMetrics();
	};

	//===========================================================================
	// Summary:
	//     CXTPPropertyGridCoolTheme is a CXTPPropertyGridPaintManager derived
	//     class.  This represents a Cool style theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPPropertyGridCoolTheme : public CXTPPropertyGridPaintManager
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPPropertyGridCoolTheme object.
		// Parameters:
		//     pGrid - Points to a CXTPPropertyGrid object
		//-----------------------------------------------------------------------
		CXTPPropertyGridCoolTheme(CXTPPropertyGrid* pGrid);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw single item of the grid.
		// Parameters:
		//     lpDrawItemStruct - A long pointer to a DRAWITEMSTRUCT structure
		//     that contains information about the type of drawing required.
		//-----------------------------------------------------------------------
		virtual void DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct);

		//-------------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the
		//     property grid.
		//-------------------------------------------------------------------------
		void RefreshMetrics();
	};


	//===========================================================================
	// Summary:
	//     CXTPPropertyGridSimpleTheme is a CXTPPropertyGridPaintManager derived
	//     class.  This represents a Simple style theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPPropertyGridSimpleTheme : public CXTPPropertyGridPaintManager
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPPropertyGridSimpleTheme object.
		// Parameters:
		//     pGrid - Points to a CXTPPropertyGrid object
		//-----------------------------------------------------------------------
		CXTPPropertyGridSimpleTheme(CXTPPropertyGrid* pGrid);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw single item of the grid.
		// Parameters:
		//     lpDrawItemStruct - A long pointer to a DRAWITEMSTRUCT structure
		//     that contains information about the type of drawing required.
		//-----------------------------------------------------------------------
		virtual void DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct);

		//-------------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the
		//     property grid.
		//-------------------------------------------------------------------------
		void RefreshMetrics();
	};


	//===========================================================================
	// Summary:
	//     CXTPPropertyGridDelphiTheme is a CXTPPropertyGridPaintManager derived
	//     class.  This represents a Delphi style theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPPropertyGridDelphiTheme : public CXTPPropertyGridPaintManager
	{
	public:
		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPPropertyGridDelphiTheme object.
		// Parameters:
		//     pGrid - Points to a CXTPPropertyGrid object
		//-----------------------------------------------------------------------
		CXTPPropertyGridDelphiTheme(CXTPPropertyGrid* pGrid);

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw single item of the grid.
		// Parameters:
		//     lpDrawItemStruct - A long pointer to a DRAWITEMSTRUCT structure
		//     that contains information about the type of drawing required.
		//-----------------------------------------------------------------------
		virtual void DrawItem(PDRAWITEMSTRUCT lpDrawItemStruct);

		//-------------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the
		//     property grid.
		//-------------------------------------------------------------------------
		virtual void RefreshMetrics();

	};


	//===========================================================================
	// Summary:
	//     CXTPPropertyGridWhidbeyTheme is a CXTPPropertyGridPaintManager derived
	//     class.  This represents a Visual Studio 2005 "Whidbey" style theme.
	//===========================================================================
	class _XTP_EXT_CLASS CXTPPropertyGridWhidbeyTheme : public CXTPPropertyGridPaintManager
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CXTPPropertyGridWhidbeyTheme object.
		// Parameters:
		//     pGrid - Points to a CXTPPropertyGrid object
		//-----------------------------------------------------------------------
		CXTPPropertyGridWhidbeyTheme(CXTPPropertyGrid* pGrid);

	protected:

		//-----------------------------------------------------------------------
		// Summary:
		//     This member is called to draw the bounding rectangle for a
		//     category caption.
		// Parameters:
		//     pDC - Reference to a valid device context
		//     rc  - Bounding rectangle of Category's caption.
		//-----------------------------------------------------------------------
		void DrawCategoryCaptionBackground(CDC* pDC, CRect rc);

		//-------------------------------------------------------------------------
		// Summary:
		//     This method is called to refresh the visual metrics of the
		//     property grid.
		//-------------------------------------------------------------------------
		void RefreshMetrics();

		//-----------------------------------------------------------------------
		// Summary:
		//     This method is called to draw expand buttons of the grid.
		// Parameters:
		//     dc        - Reference to a valid device context
		//     pItem     - Points to a CXTPPropertyGridItem object
		//     rcCaption - Caption button rectangle.
		//-----------------------------------------------------------------------
		void DrawExpandButton(CDC& dc, CXTPPropertyGridItem* pItem, CRect rcCaption);
	};

}
using namespace XTPPropertyGridPaintThemes;

AFX_INLINE CXTPPropertyGridItemMetrics* CXTPPropertyGridPaintManager::GetItemMetrics() const {
	return m_pMetrics;
}


#endif // !defined(_XTPPROPERTYGRIDPAINTMANAGER_H__)
