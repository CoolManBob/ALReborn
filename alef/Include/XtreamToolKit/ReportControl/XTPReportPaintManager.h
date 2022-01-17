// XTPReportPaintManager.h: interface for the CXTPReportPaintManager class.
//
// This file is a part of the XTREME REPORTCONTROL MFC class library.
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
#if !defined(__XTPREPORTPAINTMANAGER_H__)
#define __XTPREPORTPAINTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPReportDefines.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPWinThemeWrapper.h"


class CXTPReportControl;
class CXTPReportRow;
class CXTPReportHeader;
class CXTPReportColumn;
class CXTPReportGroupRow;
class CXTPReportInplaceButton;
class CXTPReportHyperlink;

struct XTP_REPORTRECORDITEM_DRAWARGS;
struct XTP_REPORTRECORDITEM_METRICS;

//===========================================================================
// Summary:
//     Represents predefined grid line styles:
// Example:
//     <code>m_wndReport.SetGridStyle(FALSE, xtpReportGridLargeDots);</code>
// See Also: CXTPReportControl::SetGridStyle
//
// <KEYWORDS xtpReportGridNoLines, xtpReportGridSmallDots, xtpReportGridLargeDots, xtpReportGridDashes, xtpReportGridSolid>
//===========================================================================
enum XTPReportGridStyle
{
	xtpReportGridNoLines,       // empty line
	xtpReportGridSmallDots,     // line is drawn with small dots
	xtpReportGridLargeDots,     // line is drawn with large dots
	xtpReportGridDashes,        // line is drawn with dashes
	xtpReportGridSolid          // draws solid line
};

//===========================================================================
// Summary:
//     Represents predefined freeze column line styles:
//===========================================================================
enum XTPReportFreezeColsDividerStyle
{
	xtpReportFreezeColsDividerThin      = 0x01,  // thin line style
	xtpReportFreezeColsDividerBold      = 0x02,  // bold line style
	xtpReportFreezeColsDividerHeader    = 0x04,  // header line style
	xtpReportFreezeColsDividerShade     = 0x08,  // shade line style
};

//{{AFX_CODEJOCK_PRIVATE

// deprecated
#define xtpGridNoLines      xtpReportGridNoLines
#define xtpGridSmallDots    xtpReportGridSmallDots
#define xtpGridLargeDots    xtpReportGridLargeDots
#define xtpGridDashes       xtpReportGridDashes
#define xtpGridSolid        xtpReportGridSolid

//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     Represents predefined column styles.
// Example:
//     <code>m_wndReport.GetPaintManager()->m_columnStyle = xtpReportColumnFlat;</code>
// See Also: CXTPReportPaintManager::m_columnStyle, CXTPReportPaintManager
//
// <KEYWORDS xtpReportColumnShaded, xtpReportColumnFlat>
//===========================================================================
enum XTPReportColumnStyle
{
	xtpReportColumnShaded,      // Columns are gray shaded.
	xtpReportColumnFlat,        // Flat style for drawing columns.
	xtpReportColumnExplorer,    // Explorer column style
	xtpReportColumnOffice2003,  // Gradient column style
	xtpReportColumnOffice2007   // Office 2007 column style
};

//{{AFX_CODEJOCK_PRIVATE

// deprecated
#define xtpColumnShaded xtpReportColumnShaded
#define xtpColumnFlat   xtpReportColumnFlat

//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------
// Summary:
//     Represent the style of lines used to draw a hierarchical tree structure.
// Example:
//     <code>m_wndReport.GetPaintManager()->m_treeStructureStyle = xtpReportTreeStructureSolid;</code>
// See Also: CXTPReportPaintManager::m_treeStructureStyle, CXTPReportPaintManager
//
// <KEYWORDS xtpReportTreeStructureNone, xtpReportTreeStructureSolid>
//-----------------------------------------------------------------------
enum XTPReportTreeStructureStyle
{
	xtpReportTreeStructureNone,     // No lines will be drawn from the parent node to each child node.
	xtpReportTreeStructureSolid,    // Lines will be drawn from the parent node to each child node at the next level.
	xtpReportTreeStructureDots      // Dots will be drawn from the parent node to each child node at the next level.
};

//===========================================================================
// Summary:
//     Utility class, handling most of the drawing activities. It stores
//     all settings, needed by a control to perform drawing operations:
//     fonts, colors, styles for all other classes of control.
//     It also implements all functions for drawing typical graphical
//     primitives, functions that directly work with device context.
// Remarks:
//     Could be overridden to provide another look and feel for the control.
//     Thus you have an easy way to change "skin" of your control; just provide
//     your own implementation of CXTPReportPaintManager and there's no need to touch
//     functionality all others classes of control
//
//     You create ReportPaintManager by calling its constructor,
//     further you can call getter and setter functions to change
//     every setting as you wish.
// See Also: CXTPReportControl overview, CXTPReportRow, CXTPReportGroupRow,
//           CXTPReportColumn, CXTPReportHeader
//===========================================================================
class _XTP_EXT_CLASS CXTPReportPaintManager : public CXTPCmdTarget
{
	friend class CXTPReportControl;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager constructor, handles properties initialization.
	// See Also: RefreshMetrics
	//-----------------------------------------------------------------------
	CXTPReportPaintManager();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default paint manager destructor, handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportPaintManager();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Initializes all drawings defaults (fonts, colors, etc.). Most
	//     of them are system defaults.
	// See Also: CXTPReportPaintManager
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns default row height for print mode.
	// Parameters:
	//     pDC   - Pointer to the used Device Context.
	//     pRow  - Pointer to CXTPReportRow object.
	//     nTotalWidth - Width of the row
	// Returns:
	//     The height of the default rectangle where row's items will draw.
	// Example:
	//     <code>int nRowHeight = GetRowHeight(pDC, pRow)</code>
	//-----------------------------------------------------------------------
	virtual int GetRowHeight(CDC* pDC, CXTPReportRow* pRow);
	virtual int GetRowHeight(CDC* pDC, CXTPReportRow* pRow, int nTotalWidth); // <combine CXTPReportPaintManager::GetRowHeight@CDC*@CXTPReportRow*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the default column header height.
	// Returns:
	//     The current height of the rectangle where column's headers draw.
	//-----------------------------------------------------------------------
	virtual int GetHeaderHeight();

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculate optimal header height.
	// Parameters:
	//     pDC          - Pointer to device context.
	//     pControl     - Pointer to a report control.
	//     nTotalWidth  - Total header with. Used for printing only.
	// Returns:
	//     Optimal header height.
	//-----------------------------------------------------------------------
	virtual int GetHeaderHeight(CXTPReportControl* pControl, CDC* pDC, int nTotalWidth = 0);

	//-----------------------------------------------------------------------
	// Summary:
	// Summary:
	//     Calculate optimal footer height.
	// Parameters:
	//     pDC          - Pointer to device context.
	//     pControl     - Pointer to a report control.
	//     nTotalWidth  - Total header with. Used for printing only.
	// Returns:
	//     Optimal footer height.
	//-----------------------------------------------------------------------
	virtual int GetFooterHeight(CXTPReportControl* pControl, CDC* pDC, int nTotalWidth = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns control background color.
	// Parameters:
	//     pControl - Pointer to the parent report control object.
	// Returns:
	//     Color that is used for filling client rect.
	//-----------------------------------------------------------------------
	virtual COLORREF GetControlBackColor(CXTPReportControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Fills the provided row with its background color.
	// Parameters:
	//     pDC     - Pointer to the used Device Context.
	//     pRow    - Pointer to the Row.
	//     rcRow   - Row area rectangle coordinates.
	// Example:
	// <code>
	// // paint row background
	// pPaintManager->FillRow(pDC, this, rcRow);
	// </code>
	//-----------------------------------------------------------------------
	virtual void FillRow(CDC* pDC, CXTPReportRow* pRow, CRect rcRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws control's grid.
	// Parameters:
	//     pDC       - Pointer to the used Device Context.
	//     bVertical - boolean value determines orientation of grid
	//     rc        - Row area rectangle coordinates.
	// Example:
	// <code>
	// // draw vertical grid in Row
	// CRect rcGridItem(rcRowRect);
	// pPaintManager->DrawGrid(pDC, TRUE, rcGridItem);
	//
	// // draw horizontal grid in Row
	// CPaintDC pDC(this);
	// CRect rcGridItem(rcRowRect);
	// pPaintManager->DrawGrid(pDC, FALSE, rcGridItem);
	// </code>
	//-----------------------------------------------------------------------
	virtual void DrawGrid(CDC* pDC, BOOL bVertical, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the freeze column divider.
	// Parameters:
	//     pDC   - Pointer to the used Device Context.
	//     rc - Rectangle coordinates.
	//     pControl - Pointer to a report control.
	//     pRow  - Pointer to the Group Row.
	//-----------------------------------------------------------------------
	virtual void DrawFreezeColsDivider(CDC* pDC, const CRect& rc, CXTPReportControl* pControl,
									   CXTPReportRow* pRow = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws group row.
	// Parameters:
	//     pDC   - Pointer to the used Device Context.
	//     pRow  - Pointer to the Group Row.
	//     rcRow - Group Row area rectangle coordinates.
	//     pMetrics - Pointer to a XTP_REPORTRECORDITEM_METRICS object.
	// Example:
	// <code>
	// CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	// pDC->SetBkMode(TRANSPARENT);
	// pPaintManager->DrawGroupRow(pDC, this, rcRow);
	// </code>
	//-----------------------------------------------------------------------
	virtual void DrawGroupRow(CDC* pDC, CXTPReportGroupRow* pRow, CRect rcRow, XTP_REPORTRECORDITEM_METRICS* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//      Fill font and colors members of the metrics structure
	//      for the specified group row.
	// Parameters:
	//      pRow        - Pointer to the Group Row.
	//      pMetrics    - Pointer to the metrics structure to be filled.
	//      bPrinting   - Is printing mode.
	//-----------------------------------------------------------------------
	virtual void FillGroupRowMetrics(CXTPReportGroupRow* pRow, XTP_REPORTRECORDITEM_METRICS* pMetrics, BOOL bPrinting);

	//-----------------------------------------------------------------------
	// Summary:
	//     Fills indent area by its background.
	// Parameters:
	//     pDC   - Pointer to the used Device Context.
	//     rcRow - Row area rectangle coordinates.
	// Example:
	// <code>
	// CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	// CRect rcRow(rcClient);
	// // Get default row height
	// int nRowHeight = m_pControl->GetRowHeight(FALSE)
	// rcRow.bottom = rcRow.top + nRowHeight;
	// pPaintManager->FillIndent(pDC, rcRow);
	// </code>
	//-----------------------------------------------------------------------
	virtual void FillIndent(CDC* pDC, CRect rcRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Fills item shade by its background if necessary.
	// Parameters:
	//     pDC    - Pointer to the used Device Context.
	//     rcItem - Item area rectangle coordinates.
	// Example:
	// <code>
	// CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	// CRect rcItem(m_rcRow);
	// pPaintManager->FillItemShade(pDC, rcItem);
	// </code>
	//-----------------------------------------------------------------------
	virtual void FillItemShade(CDC* pDC, CRect rcItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Fills column header control area by its background.
	// Parameters:
	//     pDC      -  Pointer to the used Device Context.
	//     rcHeader -  Header area rectangle coordinates.
	// Example:
	// <code>
	// CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	// CRect rcHeader(rcClient);
	// int nHeaderHeight = m_pControl->GetHeaderHeight();
	// rcHeader.bottom = rcHeader.top + nHeaderHeight;
	// pPaintManager->FillHeaderControl(pDC, rcHeader);
	// </code>
	//-----------------------------------------------------------------------
	virtual void FillHeaderControl(CDC* pDC, CRect rcHeader);

	//-----------------------------------------------------------------------
	// Summary:
	//     Fills column footer control area by its background.
	// Parameters:
	//     pDC      -  Pointer to the used Device Context.
	//     rcFooter -  Footer area rectangle coordinates.
	//-----------------------------------------------------------------------
	virtual void FillFooter(CDC* pDC, CRect rcFooter);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws column header with all related attributes (sort order, icon, etc).
	// Parameters:
	//     pDC           - Pointer to the used Device Context.
	//     pColumn       - Column header area rectangle coordinates.
	//     pHeader       - Pointer to report header.
	//     rcColumn      - Column area rectangle coordinates.
	//     bDrawExternal - whether draw Column on Header area, default is FALSE.
	// Example:
	// <code>
	// CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	// CXTPReportColumn* pColumn = CXTPReportColumn* pColumn = m_pColumns->GetAt(1);
	// CRect rcHeader(rcClient);
	// int nHeaderHeight = m_pControl->GetHeaderHeight();
	// rcHeader.bottom = rcHeader.top + nHeaderHeight;
	// GetPaintManager()->DrawColumn(pDC, pColumn, this, rcHeader);
	// </code>
	//-----------------------------------------------------------------------
	virtual void DrawColumn(CDC* pDC, CXTPReportColumn* pColumn, CXTPReportHeader* pHeader,
							CRect rcColumn, BOOL bDrawExternal = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws column footer with all related attributes (footer text).
	// Parameters:
	//     pDC           - Pointer to the used Device Context.
	//     pColumn       - Column header area rectangle coordinates.
	//     pHeader       - Pointer to report header.
	//     rcColumn      - Column area rectangle coordinates.
	//-----------------------------------------------------------------------
	virtual void DrawColumnFooter(CDC* pDC, CXTPReportColumn* pColumn, CXTPReportHeader* pHeader, CRect rcColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if hot tracking is enabled for current column style.
	// Returns:
	//     TRUE if enabled; FALSE otherwise
	// Remarks:
	//     Hot Tracking can be used only for xtpReportColumnOffice2003 and xtpReportColumnExplorer styles.
	// See Also:
	//     m_columnStyle, m_bHotTracking
	//-----------------------------------------------------------------------
	BOOL IsColumHotTrackingEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws rows that were focused.
	// Parameters:
	//     pDC     - Pointer to the used Device Context.
	//     pHeader - Pointer to report header.
	//     rcRow   - Area rectangle coordinates.
	// Example:
	// <code>
	// CRect rcClient(GetParent()->GetClientRect());
	// CXTPReportHeader* pHeader = m_pColumns->GetReportHeader();
	// CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	// GetPaintManager()->DrawResizingRect(pDC, pHeader, rcClient);
	// </code>
	//-----------------------------------------------------------------------
	virtual void DrawFocusedRow(CDC* pDC, CRect rcRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws Group By box.
	// Parameters:
	//     pDC       - Pointer to the used Device Context.
	//     rcGroupBy - Reference to Group By box area rectangle coordinates.
	// Example:
	// <code>
	// CXTPReportPaintManager* pPaintManager = m_pControl->GetPaintManager();
	// pPaintManager->FillGroupByControl(pDC, rcGroupBy);
	// </code>
	//-----------------------------------------------------------------------
	virtual void FillGroupByControl(CDC* pDC, CRect& rcGroupBy);


	//-----------------------------------------------------------------------
	// Summary:
	//     Returns point to additional image list with Glyphs
	// Remarks:
	//     Additional image list usually used for bitmaps of collapsed icons, etc
	//     You call this member function to get a pointer to list and manipulate
	//     its content as you wish
	// Returns:
	//     Pointer to the instantiated object of CImageList class
	//-----------------------------------------------------------------------
	CImageList* GetGlyphsImageList();

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws Horizontal line.
	// Parameters:
	//     pDC - Pointer to the used Device Context.
	//     xPos   - Horizontal coordinate of the beginning of line.
	//     yPos   - Vertical coordinate of the beginning of line.
	//     cx  - Length of line.
	//     clr - Color of line.
	// Remarks:
	//     This member function is called in the control everywhere we need to
	//     draw a simple horizontal line
	//-----------------------------------------------------------------------
	void DrawHorizontalLine(CDC* pDC, int xPos, int yPos, int cx, COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws vertical line.
	// Parameters:
	//     pDC - Pointer to the used Device Context.
	//     xPos   - Horizontal coordinate of the beginning of line.
	//     yPos   - Vertical coordinate of the beginning of line.
	//     cy  - Length of line.
	//     clr - Color of line.
	// Remarks:
	//     This member function is called in the control everywhere we need to
	//     draw a simple vertical line
	//-----------------------------------------------------------------------
	void DrawVerticalLine(CDC* pDC, int xPos, int yPos, int cy, COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws triangle that shows the sort order of column.
	// Parameters:
	//     pDC        - Pointer to the used Device Context.
	//     rcTriangle - Triangle area rectangle coordinates.
	//     bToDown    - If TRUE the top of triangle turn downward, otherwise upward.
	// Remarks:
	//     You use this function to draw a triangle image on the header of the column
	//     that is sorted. Triangle represents the direction of sorting: ascending or descending.
	//     Sizes of the triangle depend on the rectangle area that is provided to draw
	// Example:
	// <code>
	// // draw ascendant triangle
	// CRect rcTriangle;
	// rcTriangle.CopyRect(rcHeader);
	// rcTriangle.DeflateRect(40, 5, 5, 5);
	// DrawTriangle(pDC, rcTriangle, TRUE);
	//
	// // draw descendant triangle
	// CRect rcTriangle;
	// rcTriangle.CopyRect(rcHeader);
	// rcTriangle.DeflateRect(40, 5, 5, 5);
	// DrawTriangle(pDC, rcTriangle, FALSE);
	// </code>
	//-----------------------------------------------------------------------
	void DrawTriangle(CDC* pDC, CRect rcTriangle, BOOL bToDown);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws a column connector for Group By box.
	// Parameters:
	//     pDC    - Pointer to the used Device Context.
	//     ptFrom - Source point of the connector.
	//     ptTo   - Target point of the connector.
	// Remarks:
	//     You use this function to draw a line that represents a
	//     chain of columns in GroupBy box. It connects columns
	//     in GroupBy box altogether
	// Example:
	//     <code>DrawConnector(pDC, CPoint(rcItem.right - 5, rcItem.bottom), CPoint(x, rcItem.bottom + 4));</code>
	//-----------------------------------------------------------------------
	void DrawConnector(CDC* pDC, CPoint ptFrom, CPoint ptTo);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws collapsed bitmap.
	// Parameters:
	//     pDC      - Pointer to the used Device Context.
	//     pRow     - Pointer to the Row.
	//     rcBitmap - Bitmap area rectangle coordinates.
	// Returns:
	//     Intended value (right bound of drawn bitmap).
	//-----------------------------------------------------------------------
	int DrawCollapsedBitmap(CDC* pDC, const CXTPReportRow* pRow, CRect& rcBitmap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws image of Report Control.
	// Parameters:
	//     pDC      - Pointer to the used Device Context.
	//     pControl - Pointer to report control which image will be drawn.
	//     rcColumn - Bitmap area rectangle coordinates.
	//     iImage   - Image index.
	// Remarks
	//     You use this function to draw a bitmap in the column rectangle.
	//     If the provided rectangle is too small, the bitmap will not be drawn.
	//     Generally, the column rectangle must be big enough to contain
	//     text (if presented) and bitmap
	// Returns:
	//     Width of drawn bitmap.
	// Example:
	// <code>
	// // draws bitmap with id ID_READ
	// pPaintManager->DrawBitmap(pDC, pControl, rcColumn, ID_READ);
	// </code>
	//-----------------------------------------------------------------------
	virtual int DrawBitmap(CDC* pDC, CXTPReportControl* pControl, CRect rcColumn, int iImage);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws image for a column.
	// Parameters:
	//      pDC      - Pointer to the used Device Context.
	//      pColumn  - Pointer to a column object.
	//      rcColumn - Column rectangle.
	//      rcIcon   - Image bounding rectangle.
	//      iIcon    - Image index.
	// Returns:
	//      Width of drawn bitmap.
	//-----------------------------------------------------------------------
	virtual int DrawColumnIcon(CDC* pDC, CXTPReportColumn* pColumn, CRect rcColumn, CRect rcIcon, int iIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw standard glyphs of report control
	// Parameters:
	//     pDC      - Pointer to the used Device Context.
	//     rcColumn - Bitmap area rectangle coordinates.
	//     iImage   - Image index.
	// Returns:
	//     Width of drawn bitmap.
	//-----------------------------------------------------------------------
	virtual int DrawGlyph(CDC* pDC, CRect rcColumn, int iImage);

	//-----------------------------------------------------------------------
	// Summary:
	//     Changes report control grid lines color.
	// Parameters:
	//     clrGridLine - New grid color.
	// Returns:
	//     Previous grid color
	// Example:
	// <code>
	// // Set new color, save old one
	// COLORREF clrNew;
	// clrNew = RGB(0, 255, 0);
	// COLORREF clrOld;
	// clrOld = SetGridColor(clrNew);
	// </code>
	//-----------------------------------------------------------------------
	COLORREF SetGridColor(COLORREF clrGridLine);

	//-----------------------------------------------------------------------
	// Summary:
	//     Changes the preview mode state for the control.
	// Parameters:
	//     bIsPreviewMode - New preview mode (TRUE or FALSE).
	// Remarks:
	//     Preview mode has two states: enable and disabled. When preview is
	//     enabled, the control tries to show additional bands with preview text
	// Example:
	// <code>
	// // enable preview mode
	// EnablePreviewMode(TRUE);
	//
	// // disable preview mode
	// EnablePreviewMode(FALSE);
	// </code>
	//-----------------------------------------------------------------------
	void EnablePreviewMode(BOOL bIsPreviewMode);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns preview mode state for the control.
	// Remarks:
	//     You use this member function to programmatically identify
	//     in which mode the control is
	// Returns:
	//     Current preview mode state.
	//-----------------------------------------------------------------------
	BOOL IsPreviewMode() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the default text font for the control items.
	// Returns:
	//     Pointer to the current font used for drawing test.
	//-----------------------------------------------------------------------
	CFont* GetTextFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     Set count of max lines of the preview text.
	// Parameters:
	//     nMaxLines - New value of max lines of text for a preview item.
	// Remarks:
	//     You use this member function to bound the height of a textPreview item
	//     by setting the maximum rows of text for it
	// Returns:
	//     Previous value of max lines of text for a preview item.
	// Example:
	//     <code>int nOldMaxLines = SetMaxPreviewLines(4);</code>
	//-----------------------------------------------------------------------
	int SetMaxPreviewLines(int nMaxLines);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the count of max lines of  preview text.
	// Returns:
	//     Current value of max lines of text for a preview item.
	//-----------------------------------------------------------------------
	int GetMaxPreviewLines();

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates the line count for preview text, takes into account max lines.
	// Parameters:
	//     pDC     - Pointer to the used Device Context.
	//     rcText  - Reference to text area rectangle coordinates.
	//     strText - String of text.
	// Remarks:
	//     This function calculates the count of preview text lines based on the length
	//     of preview text string, preview text font and width of provided rectangle.
	//     If default max lines count less than count of calculate lines,
	//     returns default max lines
	// Returns:
	//     Returns the count of lines needed to draw preview text.
	//-----------------------------------------------------------------------
	int GetPreviewLinesCount(CDC* pDC, CRect& rcText, const CString& strText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Return visibility of grid line
	// Parameters:
	//     bVertical - determines which grid lines will be tested,
	//                 vertical or horizontal.
	// Returns:
	//     boolean value which identifies the visibility of the line
	//-----------------------------------------------------------------------
	BOOL IsGridVisible(BOOL bVertical) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to set a header style.
	// Parameters:
	//      columnStyle - A value from XTPReportColumnStyle enum.
	// See Also: GetColumnStyle, XTPReportColumnStyle
	//-----------------------------------------------------------------------
	void SetColumnStyle(XTPReportColumnStyle columnStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//      Use this member function to get a header style.
	// Returns:
	//      A value from XTPReportColumnStyle enum.
	// See Also: SetColumnStyle, XTPReportColumnStyle
	//-----------------------------------------------------------------------
	XTPReportColumnStyle GetColumnStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets one of the predefined grid line styles
	// Parameters:
	//     bVertical - Determines which grid lines settings,
	//                 vertical or horizontal
	//     gridStyle - Grid style. Can be one of the values listed
	//                 in the remarks section.
	// Remarks:
	//     Grid style can be one of the following:
	//          * <b>xtpReportGridNoLines</b>   Empty line
	//          * <b>xtpReportGridSmallDots</b> Line drawn by small dots
	//          * <b>xtpReportGridLargeDots</b> Line drawn by large dots
	//          * <b>xtpReportGridDashes</b>    Line drawn by dashes
	//          * <b>xtpReportGridSolid</b>     Draws solid line
	// See Also: XTPReportGridStyle
	//-----------------------------------------------------------------------
	void SetGridStyle(BOOL bVertical, XTPReportGridStyle gridStyle);

	//--------------------------------------------------------------------------
	// Summary:
	//     Creates and sets default text font
	// Parameters:
	//     lf - Reference to LOGFONT structure
	// See Also: LOGFONT, SetCaptionFont
	//-----------------------------------------------------------------------
	void SetTextFont(LOGFONT& lf);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates and sets the default caption font
	// Parameters:
	//     lf - Reference to LOGFONT structure
	// See Also: LOGFONT, SetTextFont
	//-----------------------------------------------------------------------
	void SetCaptionFont(LOGFONT& lf);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the preview item indent.
	// Parameters:
	//     nLeft   - Left indent to be used
	//     nTop    - Top indent to be used
	//     nRight  - Right indent to be used
	//     nBottom - Bottom indent to be used
	//-----------------------------------------------------------------------
	void SetPreviewIndent(int nLeft, int nTop, int nRight, int nBottom);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw in-place button of the report
	// Parameters:
	//     pDC     - Pointer to the device context
	//     pButton - Pointer to in-place button to draw
	//-----------------------------------------------------------------------
	virtual void DrawInplaceButton(CDC* pDC, CXTPReportInplaceButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw tree elements of report control.
	// Parameters:
	//     pDrawArgs - pointer to structure with drawing arguments
	//     pMetrics  - Metrics of the item
	//     rcItem    - Item area rectangle coordinates.
	//-----------------------------------------------------------------------
	virtual void DrawTreeStructure(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pMetrics, CRect rcItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draw Item Bitmap. Override this method to draw custom bitmap.
	// Parameters:
	//     pDrawArgs - pointer to structure with drawing arguments
	//     rcItem    - Item rectangle.
	//     nImage    - Image index.
	//-----------------------------------------------------------------------
	virtual void DrawItemBitmap(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, CRect& rcItem, int nImage);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draw Item Caption. Override this method to draw custom caption.
	// Parameters:
	//     pDrawArgs - pointer to structure with drawing arguments
	//     pMetrics  - Metrics of the item
	//-----------------------------------------------------------------------
	virtual void DrawItemCaption(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to determines if paint manager supports
	//     variable height of row
	// Returns:
	//     TRUE if GetRowHeight retrieves same value for each row.
	// See Also: GetRowHeight
	//-----------------------------------------------------------------------
	BOOL IsFixedRowHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set if paint manager supports
	//     variable height of row
	// Parameters:
	//     bFixedRowHeight - TRUE to set Fixed Row height, FALSE to set variable
	// See Also: GetRowHeight
	//-----------------------------------------------------------------------
	void SetFixedRowHeight(BOOL bFixedRowHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to determines if paint manager supports
	//     variable height of inplace buttons.
	// Returns:
	//     TRUE if height of inplace buttons is fixed, FALSE if it is the same
	//     as row height.
	// See Also: SetInplaceButtonHeight
	//-----------------------------------------------------------------------
	BOOL IsFixedInplaceButtonHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set if paint manager supports
	//     fixed height of inplace buttons.
	// Parameters:
	//     bFixedInplaceButtonHeight -  TRUE to set Fixed button height,
	//                                  FALSE to set variable (the same as row height).
	// See Also: IsFixedInplaceButtonHeight
	//-----------------------------------------------------------------------
	void SetInplaceButtonHeight(BOOL bFixedInplaceButtonHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//      Get style of Freeze Columns Divider line.
	// Returns:
	//      A value from enum XTPReportFreezeColsDividerStyle.
	// See Also:
	//      XTPReportFreezeColsDividerStyle
	//-----------------------------------------------------------------------
	int GetFreezeColsDividerStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Set style of Freeze Columns Divider line.
	// Parameters:
	//      nStyle - A value from enum XTPReportFreezeColsDividerStyle.
	// See Also:
	//      XTPReportFreezeColsDividerStyle
	//-----------------------------------------------------------------------
	void SetFreezeColsDividerStyle(int nStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Override this method to do custom activities for the item before processing it.
	// Parameters:
	//     pDrawArgs - pointer to structure with drawing arguments
	// Returns:
	//     boolean value -  TRUE means that the item needs further processing
	//-----------------------------------------------------------------------
	virtual BOOL OnDrawAction(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs);

	void DrawNoGroupByText(CDC* pDC, CRect rcItem);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Draw hyperlink string using hyperlinks text settings.
	// Parameters:
	//     pnCurrDrawPos - pointer to current drawing position (is updated after each call of function)
	//     pDrawArgs     - pointer to structure with drawing arguments
	//     pHyperlink    - pointer to hyperlink object
	//     strText       - string of text
	//     rcLink        - link hot spot area rectangle coordinates
	//     nFlag         - Flag can allow next values:
	//                     DT_END_ELLIPSIS - truncate text by ellipses
	//                     DT_WORDBREAK - - truncate text by word end
	//     bSelected     - if Row is selected
	//
	// Remarks:
	//     Its member function is internally used by DrawTextLine and draws hyperlink
	//     chunk of text string, updates hyperlink's hot spot.
	// Returns:
	//     Position in CString where drawing of one link text was stopped
	//-----------------------------------------------------------------------
	virtual int DrawLink(int* pnCurrDrawPos, XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, CXTPReportHyperlink* pHyperlink,
		CString strText, CRect rcLink, int nFlag);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draw a line of text with respect to the drawing rect
	// Parameters:
	//     pDrawArgs        - Pointer to structure with drawing arguments.
	//     strText          - String of text.
	//     rcItem           - Item area rectangle coordinates.
	//     nFlag            - Flag can allow next values:
	//                        DT_END_ELLIPSIS - truncate text by ellipses
	//                        DT_WORDBREAK - - truncate text by word end
	//     nCharCounter     - Reference to printed char counted, accepts value before, returns value after drawing
	//     nHyperlikCounter - Reference to printed hyperlinks counted, accepts value before, returns value after drawing
	//     bSelected        - If Row is selected
	// Remarks:
	//     Its member function is internally used by DrawText() and draws one line of text.
	//     It processes plain text/ hyperlink text for one line. For multi-line text
	//     process last line - the truncate by ellipses, otherwise truncate by word
	// Returns:
	//     Position in CString where drawing of one line of text was stopped
	//-----------------------------------------------------------------------
	virtual void DrawTextLine(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, const CString& strText,
		CRect rcItem, int nFlag, int& nCharCounter, int& nHyperlikCounter);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draw simple string into rectangle
	// Parameters:
	//     pnCurrDrawPos - Pointer to current drawing position (is updated after each call of function)
	//     pDC           - Point to used Device Context
	//     strDraw       - String of text
	//     rcDraw        - Text area rectangle coordinates
	//     nFormat       - Flag can allow next values:
	//                     DT_END_ELLIPSIS - truncate text by ellipses
	//                     DT_WORDBREAK - truncate text by word end
	// Remarks:
	//     Its member function is internally used by DrawTextLine and draws a plain chunk of
	//     text string.
	// Returns:
	//     Position in CString where drawing of one string of text line was stopped
	//-----------------------------------------------------------------------
	virtual int DrawString(int* pnCurrDrawPos, CDC* pDC, const CString& strDraw, CRect rcDraw, UINT nFormat);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw background of the column using current column style
	// Parameters:
	//     pDC - Point to used Device Context
	//     pColumn - Column needs to draw
	//     rcColumn - Bounding rectangle of the column
	//     bColumnPressed - TRUE if column is pressed by user.
	//-----------------------------------------------------------------------
	virtual void DrawColumnBackground(CDC* pDC, CXTPReportColumn* pColumn, CRect rcColumn, BOOL& bColumnPressed);
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Generates a custom grid pattern
	// Parameters:
	//     pDC      -  Pointer to device context.
	//     pBrush   -  Pointer to brush object.
	//     rc       -  Rectangle area.
	//     pPattern -  Points to a short-integer array that contains the
	//                 initial bitmap bit values. If it is NULL, the new
	//                 bitmap is left uninitialized.
	//     clr      -  COLORREF object representing the pattern color.
	//-----------------------------------------------------------------------
	virtual void DrawGridPat(CDC* pDC, CBrush* pBrush, CRect rc, CONST VOID*pPattern, COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws in-place button frame
	// Parameters:
	//     pDC     - Pointer to device context.
	//     pButton - Pointer to in-place button
	//-----------------------------------------------------------------------
	virtual void DrawInplaceButtonFrame(CDC* pDC, CXTPReportInplaceButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculate optimal height for specified column.
	// Parameters:
	//     pDC          - Pointer to device context.
	//     pColumn      - Pointer to a column object.
	//     nTotalWidth  - Total header with. Used for printing only.
	// Returns:
	//     Optimal height for specified column.
	//-----------------------------------------------------------------------
	virtual int CalcColumnHeight(CDC* pDC, CXTPReportColumn* pColumn, int nTotalWidth = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws multi-line text (keeping hyperlinks active).
	// Parameters:
	//     pDrawArgs     - Pointer to structure with drawing arguments.
	//     strText       - String of text.
	//     rcItem        - Item area rectangle coordinates.
	//-----------------------------------------------------------------------
	virtual void DrawMultiLineText(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, const CString& strText,
								   CRect rcItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws single-line text (keeping hyperlinks active).
	// Parameters:
	//     pDrawArgs     - Pointer to structure with drawing arguments.
	//     strText       - String of text.
	//     rcItem        - Item area rectangle coordinates.
	//     nStartPos     - Start position in strText.
	//     nEndPos       - End position in strText.
	//     nActualWidth  - Width of text to be drawn.
	//-----------------------------------------------------------------------
	virtual void DrawSingleLineText(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs,const CString& strText,
									CRect rcItem, int nStartPos, int nEndPos,int nActualWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates height of the rectangle, bounding the multiline text, when the width is fixed.
	// Parameters:
	//     pDC        - Pointer to device context.
	//     strText    - String of text.
	//     nMaxWidth  - Max width of the rectangle, bounding the text.
	// Returns:
	//     Necessary height to draw the multiline text.
	//-----------------------------------------------------------------------
	virtual int CalculateRowHeight(CDC* pDC, const CString& strText, int nMaxWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draw hyperlink string using hyperlinks text settings (removing blanks).
	// Parameters:
	//     pnCurrDrawPos - pointer to current drawing position (is updated after each call of function)
	//     pDrawArgs     - pointer to structure with drawing arguments
	//     pHyperlink    - pointer to hyperlink object
	//     strText       - string of text
	//     rcLink        - link hot spot area rectangle coordinates
	//     nFlag         - Flag can allow next values:
	//                     DT_END_ELLIPSIS - truncate text by ellipses
	//                     DT_WORDBREAK - - truncate text by word end
	//     bTrim         - whether to trim the hyperlink
	// Remarks:
	//     Its member function is internally used by DrawTextLine and draws hyperlink
	//     chunk of text string, updates hyperlink's hot spot.
	//     If the hyperlink begins\ends with blanks, they are removed.
	// Returns:
	//     Position in CString where drawing of one link text was stopped
	//-----------------------------------------------------------------------
	virtual int DrawLink2(int* pnCurrDrawPos, XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, CXTPReportHyperlink* pHyperlink,
		CString strText, CRect rcLink, int nFlag, BOOL bTrim = FALSE);

//private:
	//{{AFX_CODEJOCK_PRIVATE
	COLORREF MixColor(COLORREF clrLight, COLORREF clrDark, double dFactor);
	void Line(CDC* pDC, int x, int y, int cx, int cy, CPen* pPen);
	virtual void DrawTreeStructureLine(CDC* pDC, int x, int y, int cx, int cy, COLORREF clr);
	//}}AFX_CODEJOCK_PRIVATE

public:
	CFont m_fontText;           // Report items default text.
	CFont m_fontBoldText;       // Report items default bold text.
	CFont m_fontCaption;        // Column header caption font.
	CFont m_fontPreview;        // Column header caption font.

public:
	CXTPPaintManagerColor m_clrHighlight;       // Background color of the highlighted row.
	CXTPPaintManagerColor m_clrGridLine;        // Grid lines color.
	CXTPPaintManagerColor m_clrWindowText;      // Report items default text color.
	CXTPPaintManagerColor m_clrHighlightText;   // Text color of the highlighted text.
	CXTPPaintManagerColor m_clrHeaderControl;   // Background color of the report header.
	CXTPPaintManagerColor m_clrCaptionText;     // Column header text color.
	CXTPPaintManagerColor m_clrControlBack;     // Background color of the control report area.
	CXTPPaintManagerColor m_clrGroupRowText;    // Row text color.
	CXTPPaintManagerColor m_clrGroupShadeBack;  // Group row background color when indentation shade is enabled.
	CXTPPaintManagerColor m_clrGroupShadeText;  // Group row foreground color when indentation shade is enabled.
	CXTPPaintManagerColor m_clrGroupShadeBorder;// Color for group border's shade.

	CXTPPaintManagerColor m_clrGroupBoxBack;    // Color of group box.
	CXTPPaintManagerColor m_clrControlDark;     // Dark background color of the report control (used on Group By area).
	CXTPPaintManagerColor m_clrControlLightLight;// Light background color used for drawing shades (used on column header area).
	CXTPPaintManagerColor m_clrHotDivider;      // Color of the column hot divider window (2 arrows).
	CXTPPaintManagerColor m_clrHyper;           // Hyperlink color.
	CXTPPaintManagerColor m_clrIndentControl;   // Color of the tree indentation area.
	CXTPPaintManagerColor m_clrItemShade;       // Color of the shade on sorted by column items.
	CXTPPaintManagerColor m_clrBtnFace;         // Standard button face color.
	CXTPPaintManagerColor m_clrBtnText;         // Standard button text color.
	CXTPPaintManagerColor m_clrPreviewText;     // Preview text color.
	CXTPPaintManagerColor m_clrSelectedRow;     // Selected row background color.
	CXTPPaintManagerColor m_clrSelectedRowText; // Selected row text color.

	CXTPPaintManagerColorGradient m_grcGradientColumn;          // Color of column used with xtpReportColumnOffice2003 style
	CXTPPaintManagerColorGradient m_grcGradientColumnHot;       // HotTracking Color of column used with xtpReportColumnOffice2003 style
	CXTPPaintManagerColorGradient m_grcGradientColumnPushed;    // Pushed Color of column used with xtpReportColumnOffice2003 style
	CXTPPaintManagerColor m_clrGradientColumnShadow;            // Bottom shadow of column used with xtpReportColumnOffice2003 style
	CXTPPaintManagerColor m_clrGradientColumnSeparator;         // Separator of column used with xtpReportColumnOffice2003 style
	CXTPPaintManagerColor m_crlNoGroupByText;           // A color for "No items" text in GroupBy area (see m_strNoGroupBy member)
	CXTPPaintManagerColor m_clrFreezeColsDivider;       // Freeze Columns Divider color.

	BOOL m_bGroupRowTextBold;       // TRUE to draw group rows with bold text.
	BOOL m_bShadeGroupHeadings;     // Show or not show the indentation shade.
	BOOL m_bShadeSortColumn;        // Show or not show the items shade in the sorted by column.
	BOOL m_bHotTracking;            // TRUE to allow column hot tracking
	BOOL m_bInvertColumnOnClick;    // TRUE to invert column on click

	CString m_strNoItems;       // A string which contains customized "No Items" text for displaying in view when there are no visible rows.
	CString m_strSortBy;        // A string which contains customized "Sort By" text for displaying in tooltip
	CString m_strNoGroupBy;     // A string which contains customized "No items" text message for displaying in Group By area when there are no items inside it.
	CString m_strNoFieldsAvailable; // A string which contains customized "No Fields Available" text message for displaying in Field Chooser.
	BOOL m_bHideSelection;      // TRUE when hiding report control's selection by drawing selected items as others, FALSE otherwise.
	BOOL m_bDrawSortTriangleAlways; // If TRUE, the sort triangle displayed in column headers when a column is sorted will always be displayed as long as the column size is large enough to drawn the triangle.  If FALSE, the triangle will be removed when the column size is too small to display the caption and triangle.
	BOOL m_bUseColumnTextAlignment; // TRUE to draw caption text accordingly to its alignment

	BOOL m_bRevertAlignment;       // TRUE to revert column alignments
	BOOL m_bUseEditTextAlignment;  //  TRUE to automatically apply edit alignment using column alignment

	XTPReportTreeStructureStyle m_treeStructureStyle;   // Tree structure style
	CRect m_rcPreviewIndent;    // Preview indentation.
	int m_nTreeIndent;          // Tree indentation.

	BOOL m_bThemedInplaceButtons; // Draw in-place buttons using current theme settings.

protected:
	int m_nRowHeight;           // Report row default height.
	int m_nHeaderHeight;        // Column header height.
	int m_nFooterHeight;        // Column footer height.
	int m_nGroupGridLineHeight; // The height of the group grid line.

	CImageList m_ilGlyphs;      // Contains an additional image list for report control

	BOOL m_bIsPreviewMode;      // Show or not show the item preview.

	int m_nResizingWidth;       // The width of the column resizing marker area.

	int m_nMaxPreviewLines;     // Restrict maximum lines for preview text

	CSize m_szGlyph;            // Glyph size

	CBrush m_brushVeriticalGrid;    // Stores brush to draw vertical grid lines
	CBrush m_brushHorizontalGrid;   // Stores brush to draw horizontal grid lines
	CBrush m_brushTreeStructure;    // Stores brush to draw tree nodes

	int m_nFreezeColsDividerStyle;  // A set of flags from XTPReportFreezeColsDividerStyle

	BOOL m_bFixedRowHeight;     // TRUE if fixed rows used.
	BOOL m_bFixedInplaceButtonHeight; // TRUE if fixed Inplace Buttons height used.

	XTPReportGridStyle m_verticalGridStyle;     // Stores current style for vertical grid lines
	XTPReportGridStyle m_horizontalGridStyle;   // Stores current style for horizontal grid lines
	XTPReportColumnStyle m_columnStyle; // Contains column drawing style.

	CXTPWinThemeWrapper m_themeWrapper;  // Header theme wrapper.
	CXTPWinThemeWrapper m_themeButton;   // In-place button theme wrapper.
	CXTPWinThemeWrapper m_themeCombo;    // Combobox in-place button theme wrapper.

};

AFX_INLINE int CXTPReportPaintManager::GetHeaderHeight() {
	return m_nHeaderHeight;
}
AFX_INLINE BOOL CXTPReportPaintManager::IsGridVisible(BOOL bVertical) const {
	return bVertical ? (m_verticalGridStyle != xtpReportGridNoLines) : (m_horizontalGridStyle != xtpReportGridNoLines);
}

AFX_INLINE void CXTPReportPaintManager::EnablePreviewMode(BOOL bIsPreviewMode) {
	m_bIsPreviewMode = bIsPreviewMode;
}
AFX_INLINE BOOL CXTPReportPaintManager::IsPreviewMode() const {
	return m_bIsPreviewMode;
}
AFX_INLINE COLORREF CXTPReportPaintManager::SetGridColor(COLORREF clrGridLine){
	COLORREF clrOldColor = m_clrGridLine;
	m_clrGridLine.SetCustomValue(clrGridLine);
	return clrOldColor;
}

AFX_INLINE CFont* CXTPReportPaintManager::GetTextFont() {
	return &m_fontText;
}
AFX_INLINE int CXTPReportPaintManager::SetMaxPreviewLines(int nMaxLines) {
	int nOldMaxLines = m_nMaxPreviewLines;
	m_nMaxPreviewLines = nMaxLines;
	return nOldMaxLines;
}
AFX_INLINE int CXTPReportPaintManager::GetMaxPreviewLines() {
	return m_nMaxPreviewLines;
}
AFX_INLINE CImageList* CXTPReportPaintManager::GetGlyphsImageList() {
	return &m_ilGlyphs;
}
AFX_INLINE BOOL CXTPReportPaintManager::IsFixedRowHeight() const {
	return m_bFixedRowHeight;
}
AFX_INLINE void CXTPReportPaintManager::SetFixedRowHeight(BOOL bFixedRowHeight){
	m_bFixedRowHeight = bFixedRowHeight;
}
AFX_INLINE BOOL CXTPReportPaintManager::IsFixedInplaceButtonHeight() const {
	return m_bFixedInplaceButtonHeight;
}
AFX_INLINE void CXTPReportPaintManager::SetInplaceButtonHeight(BOOL bFixedInplaceButtonHeight){
	m_bFixedInplaceButtonHeight= bFixedInplaceButtonHeight;
}
AFX_INLINE int  CXTPReportPaintManager::GetFreezeColsDividerStyle() const {
	return m_nFreezeColsDividerStyle;
}
AFX_INLINE void CXTPReportPaintManager::SetFreezeColsDividerStyle(int nStyle) {
	m_nFreezeColsDividerStyle = nStyle;
}

AFX_INLINE COLORREF CXTPReportPaintManager::MixColor(COLORREF clrLight, COLORREF clrDark, double dFactor) {
	return RGB(GetRValue(clrLight) - dFactor * (GetRValue(clrLight) - GetRValue(clrDark)),
		GetGValue(clrLight) - dFactor * (GetGValue(clrLight) - GetGValue(clrDark)),
		GetBValue(clrLight) - dFactor * (GetBValue(clrLight) - GetBValue(clrDark)));
}
AFX_INLINE void CXTPReportPaintManager::Line(CDC* pDC, int x, int y, int cx, int cy, CPen* pPen) {
	CPen* pOldPen = pDC->SelectObject(pPen);
	pDC->MoveTo(x, y);
	pDC->LineTo(x + cx, y + cy);
	pDC->SelectObject(pOldPen);
}

AFX_INLINE BOOL CXTPReportPaintManager::OnDrawAction(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs){
	UNREFERENCED_PARAMETER(pDrawArgs);
	return TRUE;
}

AFX_INLINE void CXTPReportPaintManager::SetColumnStyle(XTPReportColumnStyle columnStyle) {
	m_columnStyle = columnStyle;
	RefreshMetrics();
}
AFX_INLINE XTPReportColumnStyle CXTPReportPaintManager::GetColumnStyle() const {
	return m_columnStyle;
}


#endif //#if !defined(__XTPREPORTPAINTMANAGER_H__)
