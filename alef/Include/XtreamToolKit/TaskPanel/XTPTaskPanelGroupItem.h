// XTPTaskPanelGroupItem.h interface for the CXTPTaskPanelGroupItem class.
//
// This file is a part of the XTREME TASKPANEL MFC class library.
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
#if !defined(__XTPTASKPANELGROUPITEM_H__)
#define __XTPTASKPANELGROUPITEM_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPTaskPanelItem.h"

//===========================================================================
// Summary:
//     CXTPTaskPanelGroupItem is a CXTPTaskPanelItem derived class. It is used in TaskPanel control
//     as single item of the group.
//===========================================================================
class _XTP_EXT_CLASS CXTPTaskPanelGroupItem : public CXTPTaskPanelItem
{
	DECLARE_SERIAL(CXTPTaskPanelGroupItem)

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPTaskPanelGroupItem object
	//-------------------------------------------------------------------------
	CXTPTaskPanelGroupItem();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPTaskPanelGroupItem object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPTaskPanelGroupItem();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when item must be repositioned.
	// Parameters:
	//     rc - New rectangle of the item.
	//-----------------------------------------------------------------------
	virtual CRect OnReposition(CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw item in specified rectangle.
	// Parameters:
	//     pDC - Pointer to a valid device context
	//     rc  - Rectangle of item to draw.
	//-----------------------------------------------------------------------
	virtual void OnDrawItem(CDC* pDC, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the margins of the item.
	// Returns:
	//     The margins of the item.
	//-----------------------------------------------------------------------
	CRect& GetMargins();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves rectangle of the item.
	// Returns:
	//     Rectangle of the item.
	//-----------------------------------------------------------------------
	CRect GetItemRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set the bounding rectangle for the
	//     CXTPTaskPanelGroupItem.
	// Parameters:
	//     lpRect  - Rectangle of the item.
	//-----------------------------------------------------------------------
	void SetItemRect(LPCRECT lpRect);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the parent group of the item.
	// Returns:
	//     Parent group of the item.
	//-----------------------------------------------------------------------
	CXTPTaskPanelGroup* GetItemGroup() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set bold font for the item to be used.
	// Parameters:
	//     bBold - TRUE to use bold font.
	//-----------------------------------------------------------------------
	void SetBold(BOOL bBold = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve if the font is bold.
	// Returns:
	//     TRUE is the font is bold, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsBold() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method attaches control handle to item.
	// Parameters:
	//     hWnd - Window handle to attach.
	//-----------------------------------------------------------------------
	void SetControlHandle(HWND hWnd);

	HWND GetControlHandle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when an animation step is executed.
	// Parameters:
	//     nStep - Step of animation.
	//-----------------------------------------------------------------------
	void OnAnimate(int nStep);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the size of the item.
	// Parameters:
	//     szItem - Item size.
	//     bAutoHeight - TRUE to stretch control in group.
	// See Also: GetSize
	//-----------------------------------------------------------------------
	void SetSize(CSize szItem, BOOL bAutoHeight = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the backcolor of this item.
	// Returns:
	//     Backcolor of this item.
	//-----------------------------------------------------------------------
	COLORREF GetBackColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get individual color for task panel item
	//-----------------------------------------------------------------------
	COLORREF GetTextColor() const;

	// ----------------------------------------------------------------
	// Summary:
	//     Call this member to set individual color for task panel item
	// Parameters:
	//     clrText - New text color to set
	// ----------------------------------------------------------------
	void SetTextColor(COLORREF clrText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the rectangle of the "tooltip zone"
	//     while the mouse is positioned over the group item.
	// Returns:
	//     The rectangle of the "tooltip zone" while the mouse is positioned
	//     over the group item.
	// Remarks:
	//     The "tooltip zone" is the area that a tooltip will be displayed
	//     while the mouse is positioned over the group item.
	// See Also:
	//     CXTPTaskPanelGroup::GetHitTestRect
	//-----------------------------------------------------------------------
	CRect GetHitTestRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the item is currently selected.
	//     Note that multiple items can be selected at one time, but only one
	//     can have focus.
	// Returns:
	//     TRUE if the item is currently selected.
	//-----------------------------------------------------------------------
	BOOL IsItemSelected() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to select this item.  Multiple items can have
	//     focus if CXTPTaskPanel::m_bSelectItemOnFocus is FALSE.
	// Parameters:
	//     bSelected - TRUE if the item will be selected, FALSE to no longer
	//                 select the item.
	//-----------------------------------------------------------------------
	void SetItemSelected(BOOL bSelected);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set text representing item dragged out of control.
	// Parameters:
	//     lpszDragText - Dragged text of the item.
	//----------------------------------------------------------------------
	void SetDragText(LPCTSTR lpszDragText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get text representing item dragged out of control.
	// Returns:
	//     Text will be dragged out.
	// Remarks:
	//     You can override it to create dynamic text.
	//----------------------------------------------------------------------
	virtual CString GetDragText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to cache a COleDataSource object that
	//     contains the group item being dragged.
	// Parameters:
	//     srcItem - COleDataSource object to cache.
	//     bCacheTextData - TRUE to cache text presentation of object.
	// Returns:
	//     TRUE if successful, FALSE if CacheGlobalData returns NULL.
	// Remarks:
	//     This places the item being dragged onto the clipboard.
	// See Also:
	//     CreateFromOleData, CacheGlobalData, CopyToClipboard,
	//     PasteFromClipboard, PrepareDrag
	//-----------------------------------------------------------------------
	BOOL PrepareDrag (COleDataSource& srcItem, BOOL bCacheTextData = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves size of the item
	// Returns:
	//     Size of the item.
	// See Also: SetSize
	//-----------------------------------------------------------------------
	CSize GetSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the item is stretched inside its group.
	// Returns:
	//     TRUE if item fill whole group height; FALSE if it has defined height.
	//-----------------------------------------------------------------------
	BOOL IsAutoHeight() const;

protected:
//{{AFX_CODEJOCK_PRIVATE
	// System accessibility Support

	virtual HRESULT GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction);
	virtual HRESULT AccessibleDoDefaultAction(VARIANT varChild);
	virtual HRESULT GetAccessibleState(VARIANT varChild, VARIANT* pvarState);
	virtual HRESULT AccessibleSelect(long flagsSelect, VARIANT varChild);
//}}AFX_CODEJOCK_PRIVATE

private:
	void GetPreviewBitmap(CWnd* pWnd, CBitmap& bmp);

protected:

	CRect m_rcMargins;      // Item's margins.
	CRect m_rcItem;         // Rectangle of the item.

	BOOL m_bBold;           // TRUE to use bold font.
	BOOL m_bSelected;       // TRUE if the item is currently selected.

	HWND m_hWnd;            // Only used for items of type xtpTaskItemTypeControl, this is the control's child window
	CSize m_szItem;         // Size of item.  If size is set to 0, the size will be calculated by the task panel.
	CBitmap m_bmpPreview;   // Internally used.  This holds a screen shot of the attached Windows control when the item type is xtpTaskItemTypeControl.
	                        // This screen shot is used during group animation.  During animation, the Windows control is hidden and the screen shot of
	                        // the control is used in place of the actual control during the animation process.
	CSize m_szPreview;      // Last preview bitmap size
	CString m_strDragText;  // Drag text of the item.
	BOOL m_bAutoHeight;     // TRUE if item stretched inside its group.
	COLORREF m_clrText;     // Text color


	friend class CXTPTaskPanelGroup;
};

AFX_INLINE CRect& CXTPTaskPanelGroupItem::GetMargins() {
	return m_rcMargins;
}
AFX_INLINE CRect CXTPTaskPanelGroupItem::GetItemRect() const {
	return m_rcItem;
}
AFX_INLINE void CXTPTaskPanelGroupItem::SetItemRect(LPCRECT lpRect) {
	::CopyRect(&m_rcItem, lpRect);
}
AFX_INLINE CSize CXTPTaskPanelGroupItem::GetSize() const {
	return m_szItem;
}
AFX_INLINE BOOL CXTPTaskPanelGroupItem::IsAutoHeight() const {
	return m_bAutoHeight;
}
AFX_INLINE COLORREF CXTPTaskPanelGroupItem::GetTextColor() const {
	return m_clrText;
}
AFX_INLINE void CXTPTaskPanelGroupItem::SetTextColor(COLORREF clr) {
	m_clrText = clr;
	RedrawPanel();
}


#endif // !defined(__XTPTASKPANELGROUPITEM_H__)
