// XTPReportRow.h: interface for the CXTPReportRow class.
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
#if !defined(__XTPREPORTROW_H__)
#define __XTPREPORTROW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPReportDefines.h"

class CXTPReportRecord;
class CXTPReportControl;
class CXTPReportTip;
class CXTPReportRecordItem;
class CXTPReportRows;
class CXTPReportColumn;

struct XTP_REPORTRECORDITEM_DRAWARGS;
struct XTP_REPORTRECORDITEM_METRICS;
struct XTP_REPORTRECORDITEM_CLICKARGS;

//===========================================================================
// Summary:
//     CXTPReportRow class represents an row of the report control
//     window.
// Remarks:
//     Each item has corresponding a CXTPReportRecord, and could have a list
//     of sub-items associated with it. By clicking a Row item, the user
//     can expand and collapse the associated list of sub-items.
//
//     CXTPReportRow handles a behavior of report row. It processes
//     user mouse and keyboard events, it also implements external control
//     member functions. It is also handling drawing logic for the row.
//
//     A collection of report row items is combined into CXTPReportRows
//     and could be accessed via CXTPReportControl::GetRows() method.
// See Also:
//     CXTPReportControl overview, CXTPReportRows, CXTPReportRecord,
//     CXTPReportRecordItem, CXTPReportGroupRow
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRow : public CXTPHeapObjectT<CCmdTarget, CXTPReportRowAllocator>
{
	DECLARE_DYNAMIC(CXTPReportRow)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     A constructor of the CXTPReportRow object.
	//-----------------------------------------------------------------------
	CXTPReportRow();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys CXTPReportRow object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportRow();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called for initial initialization of the row.
	// Parameters:
	//     pRow - Row which settings will be copied for the row.
	//     pControl - Pointer to the parent report control.
	//     pRecord - Pointer to the record associated with the row.
	//-----------------------------------------------------------------------
	virtual void InitRow(CXTPReportRow* pRow);
	virtual void InitRow(CXTPReportControl* pControl, CXTPReportRecord* pRecord);// <COMBINE CXTPReportRow::InitRow@CXTPReportRow*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Get associated record.
	// Returns:
	//     Associated list record with data.
	// Example:
	//     <code>CXTPReportRecord* pRecord = pRow->GetRecord();</code>
	// See Also: CXTPReportRecord overview
	//-----------------------------------------------------------------------
	virtual CXTPReportRecord* GetRecord() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the row on the provided DC.
	// Parameters:
	//     pDC    - Pointer to control drawing context.
	//     rcRow  - Coordinates of the rectangle for drawing this row.
	//     nLeftOffset - Start drawing left offset in pixels (Horizontal scroll position).
	// Remarks:
	//     Call this member function to draw an item. This function
	//     only prepares data for drawing and calls needed drawing functions
	//     from PaintManager.
	//
	//     Thus if you wish to change the look of your report item, you must just
	//     provide your own implementation of PaintManager
	// See Also: CXTPPaintManager
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC, CRect rcRow, int nLeftOffset);

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates height of the row depending on the provided DC.
	// Parameters:
	//     pDC - Provided DC.
	//  nWidth - Row width.
	// Remarks:
	//     Row height depends of many factors: provided device context,
	//     font to print text, visibility of rows items. GetHeight()
	//     takes into account all of them to get real height of Row.
	// Returns:
	//     Returns height of the row in logical units.
	//-----------------------------------------------------------------------
	virtual int GetHeight(CDC* pDC, int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns current row index in the view.
	// Remarks:
	//     ReportControl draws row in some order. It depends of Index value
	//     You use GetIndex() to get current index value for particular row
	// Returns:
	//     Current row index in the view.
	//-----------------------------------------------------------------------
	virtual int GetIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns row focused state.
	// Remarks:
	//     Call this member function if you want to determine whether
	//     this row is focused in the report control or not.
	// Returns:
	//     TRUE if row is focused, FALSE otherwise.
	// See Also: IsSelected
	//-----------------------------------------------------------------------
	virtual BOOL IsFocused() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns row selected state.
	// Remarks:
	//     Call this member function if you want to determine whether
	//     this row is selected in the report control or not.
	// Returns:
	//     TRUE if row is selected, FALSE otherwise.
	// See Also: IsFocused
	//-----------------------------------------------------------------------
	virtual BOOL IsSelected() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns row expanded state.
	// Remarks:
	//     Call this member function if you want to determine whether
	//     this row is expanded in the report control or not.
	// Returns:
	//     TRUE if row is expanded, FALSE otherwise.
	// See Also: SetExpanded
	//-----------------------------------------------------------------------
	virtual BOOL IsExpanded() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets row expanded state.
	// Parameters:
	//     bExpanded - New row expanded state.
	// Remarks:
	//     Call this member function if you want to expand or collapse
	//     this row.
	// See Also: IsExpanded
	//-----------------------------------------------------------------------
	virtual void SetExpanded(BOOL bExpanded);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to select/set focus to the row.  If multiple
	//     selection is enabled, then this row will be added to the selection.
	// Parameters:
	//     bSelected - TRUE to select the row, FALSE to de-select the row.
	//-----------------------------------------------------------------------
	virtual void SetSelected(BOOL bSelected);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to select all child items of specified row.
	//-----------------------------------------------------------------------
	virtual void SelectChilds();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if this row is a group row.
	// Remarks:
	//     Call this member function if you want to determine whether
	//     this row is a group row or not.
	// Returns:
	//     TRUE if it is a group row, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsGroupRow() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines which report item, if any, is at a specified position.
	// Parameters:
	//     ptPoint     - Point to test.
	//     pRectItem   - Returns item rect.
	//     ppColumn    - Returns a pointer to the column at tested position.
	// Remarks:
	//     Call this member function to test a row contents at the
	//     specified position.
	// Returns:
	//     The record item at the specified position, if any,
	//     or NULL otherwise.
	//-----------------------------------------------------------------------
	virtual CXTPReportRecordItem* HitTest(CPoint ptPoint, CRect* pRectItem = NULL, CXTPReportColumn** ppColumn = NULL) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Process mouse clicks. Called by the framework when the user
	//     has clicked the mouse button in the row area.
	// Parameters:
	//     ptClicked - Position of the cursor, in screen coordinates,
	//                 at the time of the mouse click.
	// Remarks:
	//     Call this member function if you want a row to process
	//     a mouse button click message.
	//     You should provide the method with the coordinates of the
	//     mouse message to proceed.
	// See Also: OnDblClick, OnMouseMove, OnContextMenu
	//-----------------------------------------------------------------------
	virtual void OnClick(CPoint ptClicked);

	//-----------------------------------------------------------------------
	// Summary:
	//     Processes mouse left button down clicks.
	// Parameters:
	//     pClickArgs - structure which contains mouse click arguments
	// See Also: OnClick
	//-----------------------------------------------------------------------
	virtual BOOL OnLButtonDown(XTP_REPORTRECORDITEM_CLICKARGS* pClickArgs);

	//-----------------------------------------------------------------------
	// Summary:
	//     Process mouse double clicks. Called by the framework when
	//     the user has double-clicked the mouse button in the row area.
	// Parameters:
	//     ptClicked - Position of the cursor, in screen coordinates,
	//                 at the time of the mouse click.
	// Remarks:
	//     Call this member function if you want a row to process
	//     a mouse button double click message.
	//     You should provide the method with the coordinates of the
	//     mouse message to proceed.
	// See Also: OnClick, OnMouseMove, OnContextMenu
	//-----------------------------------------------------------------------
	virtual void OnDblClick(CPoint ptClicked);

	//-----------------------------------------------------------------------
	// Summary:
	//     Process mouse moving.
	// Parameters:
	//     nFlags - Indicates whether various virtual keys are down.
	//     point  - Point coordinates where the message was fired from.
	// Remarks:
	//     Call this member function if you want a row to process
	//     a mouse move message. You should provide the method with the
	//     coordinates of the mouse message to proceed and a flag
	//     showing which of virtual keys were down.
	// See Also: OnClick, OnDblClick, OnContextMenu
	//-----------------------------------------------------------------------
	virtual void OnMouseMove(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     Process context menu. Called by the framework when the user
	//     has clicked the right mouse button (right clicked) in the window.
	// Parameters:
	//     ptClick - Position of the cursor, in screen coordinates,
	//               at the time of the mouse click.
	// Remarks:
	//     Call this member function if you want a row to process
	//     a mouse right button down (context menu call) message.
	//     You should provide the method with the coordinates of the
	//     mouse message to proceed.
	// See Also: OnClick, OnDblClick, OnMouseMove
	//-----------------------------------------------------------------------
	virtual void OnContextMenu(CPoint ptClick);

	//-----------------------------------------------------------------------
	// Summary:
	//     Shows a specified tooltip window at the specified position.
	// Parameters:
	//     ptTip   - A screen position to show tooltip at.
	//     pTipWnd - Pointer to the specified tooltip window.
	// Remarks:
	//     Call this member function if you want to show a report
	//     tip window for this row.
	// See Also: CXTPReportTip overview
	//-----------------------------------------------------------------------
	virtual void ShowToolTip(CPoint ptTip, CXTPReportTip* pTipWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a pointer to the parent report control.
	// Remarks:
	//     Call this member function to retrieve a pointer to the
	//     CXTPReportControl which this item belongs to.
	// Returns:
	//     A pointer to an associated report control object.
	// See Also: CXTPReportControl overview
	//-----------------------------------------------------------------------
	CXTPReportControl* GetControl() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the parent row of the current row.
	// Remarks:
	//     When report rows are arranged as a tree view, this member
	//     function returns a pointer to the parent CXTPReportRow item.
	// Returns:
	//     Pointer to the parent row if any, or NULL otherwise.
	// See Also: HasParent, GetTreeDepth, GetChilds, AddChild, HasChildren
	//-----------------------------------------------------------------------
	virtual CXTPReportRow* GetParentRow() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the parent rows of the current row.
	// Remarks:
	//     When report rows are arranged as a tree view, this member
	//     function returns a pointer to the parent CXTPReportRows item.
	// Returns:
	//     Pointer to the parent rows if any, or NULL otherwise.
	// See Also: HasParent, GetTreeDepth, GetChilds, AddChild, HasChildren
	//-----------------------------------------------------------------------
	virtual CXTPReportRows* GetParentRows() const;


	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a depth of this row in tree view.
	// Remarks:
	//     When report rows are arranged as a tree view, this member
	//     function returns a depth level of the row, which means
	//     the number of parent elements for the row..
	// Returns:
	//     A depth of this row in tree view in levels.
	// See Also: GetParentRow, HasParent, GetChilds, AddChild, HasChildren
	//-----------------------------------------------------------------------
	virtual int GetTreeDepth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if a row has a parent in tree view.
	// Parameters:
	//     pRow - A row to check.
	// Remarks:
	//     When report rows are arranged as a tree view, this member
	//     function calculates whether this row has a parent row or not.
	// Returns:
	//     TRUE if specified row has a parent, FALSE otherwise.
	// See Also:
	//     HasParent, GetParentRow, GetTreeDepth, GetChilds, AddChild, HasChildren
	//-----------------------------------------------------------------------
	virtual BOOL HasParent(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a collection of this row children.
	// Remarks:
	//     Call this member function to retrieve access to the collection
	//     of row children.
	// Returns:
	//     CXTPReportRows collection of row children.
	// See Also:
	//     CXTPReportRows overview, GetParentRow, HasParent, GetTreeDepth, AddChild, HasChildren
	//-----------------------------------------------------------------------
	virtual CXTPReportRows* GetChilds();

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a child row to the current row.
	// Parameters:
	//     pRow - Pointer to a child CXTPReportRow object.
	// Remarks:
	//     Call this member function if you want to add a CXTPReportRow
	//     object as a child of current row in tree view mode.
	// Returns:
	//     A pointer to the added CXTPReportRow object.
	// See Also: GetParentRow, HasParent, GetTreeDepth, GetChilds
	//-----------------------------------------------------------------------
	virtual CXTPReportRow* AddChild(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if a row has children in tree view.
	// Parameters:
	//     pRow - A row to check.
	// Remarks:
	//     When report rows are arranged as a tree view, this member
	//     function calculates whether this row has children rows or not.
	// Returns:
	//     TRUE if the specified row has children rows, FALSE otherwise.
	// See Also:
	//     HasParent, GetParentRow, GetTreeDepth, GetChilds, AddChild, HasChildren
	//-----------------------------------------------------------------------
	virtual BOOL HasChildren() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the next sibling row of the current row.
	// Remarks:
	//     When report rows are arranged as a tree view, this member
	//     function returns a pointer to the next CXTPReportRow item.
	// Returns:
	//     Pointer to the next sibling row if any, or NULL otherwise.
	// See Also: HasParent, GetTreeDepth, GetChilds, AddChild, HasChildren
	//-----------------------------------------------------------------------
	virtual CXTPReportRow* GetNextSiblingRow() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets coordinates of the collapse/expand button for this row.
	// Parameters:
	//     rcCollapse - The rectangle in report window client coordinates.
	// Remarks:
	//     Call this member function to setup the coordinates of the
	//     collapse/expand button for the row. Note that it could be
	//     different depending on the row tree depth.
	// See Also: GetTreeDepth, GetRect
	//-----------------------------------------------------------------------
	virtual void SetCollapseRect(CRect rcCollapse);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if preview item is visible.
	// Remarks:
	//     Call this member function to determine whether preview item
	//     for this row is visible or not.
	// Returns:
	//     TRUE if preview item is visible, FALSE otherwise.
	// See Also: CXTPReportRecordItemPreview overview, CXTPReportRecord::GetItemPreview
	//-----------------------------------------------------------------------
	virtual BOOL IsPreviewVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if items are visible
	// Returns:
	//     TRUE if items are visible
	// See Also IsPreviewVisible
	//-----------------------------------------------------------------------
	virtual BOOL IsItemsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns coordinates of the row rectangle.
	// Returns:
	//     Coordinates of the row rectangle in report window client coordinates.
	// Remarks:
	//     Call this member function to determine the coordinates of
	//     the region where this row is drawn on the parent window.
	// See Also: SetCollapseRect
	//-----------------------------------------------------------------------
	virtual CRect GetRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns drawing metrics for the row.
	// Parameters:
	//     pDrawArgs    - Pointer to the provided draw arguments
	//                    structure for calculating metrics.
	//     pItemMetrics - Pointer to the metrics structure to fill.
	// Remarks:
	//     Call this member function when you want to retrieve drawing metrics.
	// See Also: XTP_REPORTRECORDITEM_DRAWARGS, XTP_REPORTRECORDITEM_METRICS
	//-----------------------------------------------------------------------
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine the row depth level when row is a group row.
	// Returns:
	//     Row depth level when row is a group row.
	//-----------------------------------------------------------------------
	virtual int GetGroupLevel() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines rectangle of specified item
	// Parameters:
	//     pItem - Record Item
	// Returns:
	//     Rectangle of the item.
	//-----------------------------------------------------------------------
	virtual CRect GetItemRect(CXTPReportRecordItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the row is the last tree row.
	//     Or is not part of a tree row.
	// Returns:
	//     TRUE if the row is the last tree row.  FALSE if the row is not
	//     part of a tree row or is not the last tree row.
	//-----------------------------------------------------------------------
	virtual BOOL IsLastTreeRow() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to force this row to be visible.  This will cause
	//     the report control to scroll to this row.  This is also called by
	//     CXTPReportControl::SetFocusedRow to make visible the newly focused row.
	//-----------------------------------------------------------------------
	virtual void EnsureVisible();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is call to update rectangle of item
	// Parameters:
	//     rcItem - Bounding Rectangle of item cell
	//     pColumn - Column to test for tree column
	// See Also: CXTPReportColumn::IsTreeColumn
	//-----------------------------------------------------------------------
	virtual void ShiftTreeIndent(CRect& rcItem, CXTPReportColumn* pColumn) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Fill XTP_REPORTRECORDITEM_METRICS structure with drawing metrics
	// Parameters:
	//     pColumn - Column of the item
	//     pItem - Item to be draen
	//     pMetrics - Output drawing metrics for the item
	// See Also: GetItemMetrics
	//-----------------------------------------------------------------------
	virtual void FillMetrics(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem, XTP_REPORTRECORDITEM_METRICS* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     The framework calls this member function to determine whether a
	//     point is in the bounding rectangle of the specified tool.
	// Parameters:
	//     point - Specifies the x- and y-coordinate of the cursor. These
	//             coordinates are always relative to the upper-left corner of the window
	//     pTI   - A pointer to a TOOLINFO structure.
	// Returns:
	//     If the tooltip control was found, the window control ID. If
	//     the tooltip control was not found, -1.
	//-----------------------------------------------------------------------
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Sets new row index in the view.
	// Parameters:
	//     nNewIndex - New row numeric index.
	// Remarks:
	//     Call this member function when you want to change the row
	//     position in the corresponding row collection.
	//-----------------------------------------------------------------------
	virtual void SetIndex(int nNewIndex);

//private:
	//-----------------------------------------------------------------------
	// Summary:
	//     Get the latest row index from the group specified.
	// Parameters:
	//     pChilds - Children collection.
	// Returns:
	//     Last row index in group.
	//-----------------------------------------------------------------------
	int GetLastChildRow(CXTPReportRows* pChilds) const;

protected:
	CXTPReportRecord* m_pRecord;    // Pointer to the associated CXTPReportRecord object.
	CXTPReportControl* m_pControl;  // Pointer to the parent report control.
	int m_nIndex;                   // Item index.
	CRect m_rcRow;                  // A rectangle where a row is to be drawn.
	CRect m_rcCollapse;             // A rectangle with coordinates of collapse/expand bitmap.

	CXTPReportRow* m_pParentRow;    // Pointer to the parent row in the tree.
	CXTPReportRows* m_pParentRows;  // Pointer to the parent rows in the tree.
	int m_nRowLevel;                // Row depth level in a tree.
	int m_nGroupLevel;              // Contains row depth level when row is a group row.
	BOOL m_bVisible;                // TRUE if this row should be visible, FALSE when row is hidden.

	CXTPReportRows* m_pChilds;      // A collection of row children.
	BOOL m_bExpanded;               // TRUE when row is expanded in a tree view, FALSE when row is collapsed.
	int m_nPreviewHeight;           // Height of the preview item of this row.
	int m_nChildIndex;              // Index of row inside parent rows.

private:

	friend class CXTPReportControl;
	friend class CXTPReportSelectedRows;
	friend class CXTPReportRows;
};

AFX_INLINE void CXTPReportRow::SetIndex(int nNewIndex) {
	m_nIndex = nNewIndex;
}

AFX_INLINE CXTPReportRecord* CXTPReportRow::GetRecord() const{
	return m_pRecord;
}

AFX_INLINE CXTPReportRow* CXTPReportRow::GetParentRow() const {
	return m_pParentRow;
}

AFX_INLINE CXTPReportControl* CXTPReportRow::GetControl() const {
	return m_pControl;
}

AFX_INLINE void CXTPReportRow::SetCollapseRect(CRect rcCollapse) {
	m_rcCollapse = rcCollapse;
}
AFX_INLINE CRect CXTPReportRow::GetRect() const {
	return m_rcRow;
}
AFX_INLINE int CXTPReportRow::GetGroupLevel() const {
	return m_nGroupLevel;
}
AFX_INLINE CXTPReportRows* CXTPReportRow::GetParentRows() const {
	return m_pParentRows;
}


#endif //#if !defined(__XTPREPORTROW_H__)
