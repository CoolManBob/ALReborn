// XTPReportGroupItem.h: interface for the CXTPReportGroupItem class.
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
#if !defined(__XTPREPORTGROUPROW_H__)
#define __XTPREPORTGROUPROW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPReportRow.h"

//===========================================================================
// Summary:
//     This class represents a 'group row' of the report control view -
//     that means a row which has no associated Record item, but
//     has a special text caption instead.
// Remarks:
//     CXTPReportGroupRow has a slightly different reaction on mouse events and another
//     draw logic.
//
//     All other behavior of the group row is similar to its parent.
//
// See Also: CXTPReportRow overview, CXTPReportControl overview
//===========================================================================
class _XTP_EXT_CLASS CXTPReportGroupRow : public CXTPReportRow
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     A constructor of the CXTPReportGroupRow object.
	//-----------------------------------------------------------------------
	CXTPReportGroupRow();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the group text caption.
	// Remarks:
	//     Call this member function to retrieve a text caption of this
	//     group row item.
	// Returns:
	//     Group text caption.
	// See Also: CXTPReportGroupRow::CXTPReportGroupRow
	//-----------------------------------------------------------------------
	virtual CString GetCaption();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns tool tip text displayed for this group row.
	// Remarks:
	//     Call this member function to retrieve the tool tip text displayed the
	//     this group row.
	// Returns:
	//     Tool tip Text for this row.
	//-----------------------------------------------------------------------
	virtual CString GetTooltip();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to specify the text that is displayed as the group
	//     caption when the column that the ReportRecordItem is in has been grouped.
	// Parameters:
	//     lpszCaption - Caption to be set
	// Remarks:
	//     When a column that the ReportRecordItem belong to has been grouped,
	//     the caption of the column is displayed as
	//     the group caption.  If you specify a caption with CXTPReportGroupRow::SetCaption
	//     for the ReportRecordItem, the m_strGroupText text
	//     will be displayed instead of the caption of the column.
	//     You can use CXTPReportGroupRow::SetCaption to "sub group" your ReportRecordItems when
	//     the column they belong to has been grouped.  The ReportRecordItems
	//     will be "sub grouped" by the value in m_strGroupText when the
	//     column they belong to is grouped.
	//-----------------------------------------------------------------------
	void SetCaption(LPCTSTR lpszCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the row using the specified device context.
	// Parameters:
	//     pDC     - A pointer to the device context in which the drawing occurs.
	//     rcRow   - Position of the row in client window coordinates.
	//     nLeftOffset - Start drawing left offset in pixels (Horizontal scroll position).
	// See Also: CXTPReportRow::Draw
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC, CRect rcRow, int nLeftOffset);

	//-----------------------------------------------------------------------
	// Summary:
	//     Process mouse single clicks.
	// Parameters:
	//     ptClicked - Point coordinates where the message was fired from.
	// See Also: CXTPReportRow::OnClick
	//-----------------------------------------------------------------------
	void OnClick(CPoint ptClicked);

	//-----------------------------------------------------------------------
	// Summary:
	//     Process mouse double clicks.
	// Parameters:
	//     ptClicked - Point coordinates where the message was fired from.
	// Remarks:
	//     Group row adds a collapse/expand functionality if double-
	//     clicked on a row.
	// See Also: CXTPReportRow::OnDblClick
	//-----------------------------------------------------------------------
	void OnDblClick(CPoint ptClicked);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if this is a group row or not.
	// Returns:
	//     TRUE because this is a group row.
	// See Also: CXTPReportRow::IsGroupRow
	//-----------------------------------------------------------------------
	BOOL IsGroupRow() const;

protected:

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
	CString m_strGroupText;     // group text label

	friend class CXTPReportPaintManager;

};

AFX_INLINE BOOL CXTPReportGroupRow::IsGroupRow() const {
	return TRUE;
}
AFX_INLINE CString CXTPReportGroupRow::GetTooltip() {
	return GetCaption();
}

#endif //#if !defined(__XTPREPORTGROUPROW_H__)
