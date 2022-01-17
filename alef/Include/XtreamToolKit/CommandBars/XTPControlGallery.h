// XTPControlGallery.h
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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
#if !defined(__XTPCONTROLGALLERY_H__)
#define __XTPCONTROLGALLERY_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "XTPCommandBarsDefines.h"
#include "XTPControlPopup.h"
#include "Common/XTPWinThemeWrapper.h"
#include "XTPControlComboBox.h"
#include "XTPScrollBar.h"

class CXTPControlGalleryItems;
class CXTPControlGallery;
class CXTPControlGalleryPaintManager;

const UINT XTP_GN_PREVIEWSTART      = 0x1010;
const UINT XTP_GN_PREVIEWCANCEL     = 0x1011;
const UINT XTP_GN_PREVIEWAPPLY      = 0x1012;
const UINT XTP_GN_PREVIEWCHANGE     = 0x1013;

//===========================================================================
// Summary:
//     CXTPControlGalleryItem is a CCmdTarget derived class. It represents single item of
//     gallery object.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlGalleryItem : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlGalleryItem object
	//-----------------------------------------------------------------------
	CXTPControlGalleryItem();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get size of the item.
	// Returns:
	//     Size of the item
	// See Also: SetSize, CXTPControlGalleryItems::SetItemSize
	//-----------------------------------------------------------------------
	virtual CSize GetSize();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set size of the individual item. You can call
	//     CXTPControlGalleryItems::SetItemSize to set default size for items
	// Parameters:
	//     szItem - Size of the item to be set
	//-----------------------------------------------------------------------
	void SetSize(CSize szItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the item
	// Parameters:
	//     pDC      - Pointer to valid device context
	//     pGallery - Parent gallery of the item
	//     rcItem   - Bounding rectangle of the item
	//     bEnabled - TRUE to draw item enabled; FALSE - disabled
	//     bSelected - TRUE to draw item selected
	//     bPressed - TRUE to draw item pressed
	//     bChecked - TRUE to draw item checked
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC, CXTPControlGallery* pGallery, CRect rcItem, BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get image for item
	//-----------------------------------------------------------------------
	CXTPImageManagerIcon* GetImage();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if the item is label
	//-----------------------------------------------------------------------
	BOOL IsLabel() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set caption of the item
	// Parameters:
	//     lpszCaption - Caption of the item to be set
	// See Also: GetCaption, SetToolTip, GetToolTip
	//-----------------------------------------------------------------------
	void SetCaption(LPCTSTR lpszCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set tooltip  of the item
	// Parameters:
	//     lpszToolTip - Tooltip of the item to be set
	// See Also: GetToolTip, SetCaption, GetCaption
	//-----------------------------------------------------------------------
	void SetToolTip(LPCTSTR lpszToolTip);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves tooltip of the item
	// See Also: SetToolTip, SetCaption, GetCaption
	//-----------------------------------------------------------------------
	CString GetToolTip() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves caption of the item
	// See Also: SetCaption, SetToolTip, GetToolTip
	//-----------------------------------------------------------------------
	CString GetCaption() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves index of the item
	//-----------------------------------------------------------------------
	int GetIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the identifier of the item.
	// Parameters:
	//     nId - Identifier to be set.
	//-----------------------------------------------------------------------
	void SetID(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the item's identifier.
	// Returns:
	//     The identifier of the item.
	//-----------------------------------------------------------------------
	int GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the 32-bit value associated with the item.
	// Parameters:
	//     dwData - Contains the new value to associate with the item.
	//-----------------------------------------------------------------------
	void SetData(DWORD_PTR dwData);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the application-supplied 32-bit value
	//     associated with the item.
	// Returns:
	//     The 32-bit value associated with the item.
	//-----------------------------------------------------------------------
	DWORD_PTR GetData() const;

protected:
	int m_nId;              // Identifier
	int m_nIndex;           // Index of the item
	int m_nImage;           // Image index
	CString m_strCaption;   // Caption of the item
	CString m_strToolTip;   // Tooltip of the item
	CXTPControlGalleryItems* m_pItems;  // Parent items collection
	DWORD_PTR m_dwData;     // 32-bit data
	CSize m_szItem;         // Size of the item; CSize(0, 0) - to use default value
	BOOL m_bLabel;          // TRUE if item is label


	friend class CXTPControlGalleryItems;
};

//===========================================================================
// Summary:
//     CXTPControlGalleryItems is a CCmdTarget derived class. It represents a collection
//     of the items for gallery control.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlGalleryItems : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPControlGalleryItems)

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlGalleryItems object.
	// Parameters:
	//     pAction - Action for gallery control to link with.
	// Remarks:
	//     Protected constructor, use CreateItems to create items for gallery
	//-----------------------------------------------------------------------
	CXTPControlGalleryItems(CXTPControlAction* pAction);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPControlGalleryItems object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPControlGalleryItems();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Creates new items collection for gallery control
	// Parameters:
	//     pCommandBars - Parent Commandbars.
	//     nId - Identifier of the items
	//-----------------------------------------------------------------------
	static CXTPControlGalleryItems* AFX_CDECL CreateItems(CXTPCommandBars* pCommandBars, int nId);


public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove all items in collection
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to remove item.
	// Parameters:
	//     nIndex - Index of the item to be removed.
	//-----------------------------------------------------------------------
	void Remove(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to add a new item.
	// Parameters:
	//     pItem - Item to be added
	//     nId   - Identifier of the item
	//     nImage - Image of the item
	//     lpszCaption - Caption of the item to be added
	// Returns:
	//     A pointer to the added item.
	//-----------------------------------------------------------------------
	CXTPControlGalleryItem* AddItem(int nId, int nImage);
	CXTPControlGalleryItem* AddItem(CXTPControlGalleryItem* pItem, int nId = -1, int nImage = -1); //<combine CXTPControlGalleryItems::AddItem@int@int>
	CXTPControlGalleryItem* AddItem(LPCTSTR lpszCaption, int nImage = -1); //<combine CXTPControlGalleryItems::AddItem@int@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to insert a new item.
	// Parameters:
	//     nIndex - Index to insert new item
	//     pItem - Item to be added
	//     nId   - Identifier of the item
	//     nImage - Image of the item
	// Returns:
	//     A pointer to the added item.
	//-----------------------------------------------------------------------
	CXTPControlGalleryItem* InsertItem(int nIndex, CXTPControlGalleryItem* pItem, int nId = -1, int nImage = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to add a new label item.
	// Parameters:
	//     nId   - Identifier of the item
	//     lpszCaption - Caption of the item to be added
	// Returns:
	//     A pointer to the added item.
	//-----------------------------------------------------------------------
	CXTPControlGalleryItem* AddLabel(int nId);
	CXTPControlGalleryItem* AddLabel(LPCTSTR lpszCaption); //<combine CXTPControlGalleryItems::AddLabel@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set image manager for items
	// Parameters:
	//     pImageManager - new image manager to be set
	//-----------------------------------------------------------------------
	void SetImageManager(CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves image manager of the items
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines default size of the items
	// See Also: SetItemSize
	//-----------------------------------------------------------------------
	CSize GetItemSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set size of the item
	// Parameters:
	//     szItem - New size to be set
	// See Also: GetItemSize
	//-----------------------------------------------------------------------
	void SetItemSize(CSize szItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the count of the items
	// Returns:
	//     The count of the items in collection.
	//-----------------------------------------------------------------------
	int GetItemCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to return the item at the specified index.
	// Parameters:
	//     nIndex - An integer index.
	// Returns:
	//     The CXTPControlGalleryItem pointer currently at this index.
	//-----------------------------------------------------------------------
	CXTPControlGalleryItem* GetItem(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to enable/disable clipping items if selection was changed
	// Parameters:
	//     bClipItems - TRUE to enable clipping; FALSE to disable
	// Remarks:
	//     By default clipping is enabled
	//-----------------------------------------------------------------------
	void ClipItems(BOOL bClipItems);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called if items was added or removed in collection
	//-----------------------------------------------------------------------
	void OnItemsChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the item
	// Parameters:
	//     pDC      - Pointer to valid device context
	//     pGallery - Parent gallery of the item
	//     pItem    - Item to be drawn
	//     rcItem   - Bounding rectangle of the item
	//     bEnabled - TRUE to draw item enabled; FALSE - disabled
	//     bSelected - TRUE to draw item selected
	//     bPressed - TRUE to draw item pressed
	//     bChecked - TRUE to draw item checked
	//-----------------------------------------------------------------------
	virtual BOOL PreDrawItem(CDC* pDC, CXTPControlGallery* pGallery, CXTPControlGalleryItem* pItem, CRect rcItem,
		BOOL bEnabled, BOOL bSelected, BOOL bPressed, BOOL bChecked);

private:
	void UpdateIndexes(int nStart /*= 0*/);

protected:
	CSize m_szItem;                         // Size of the items
	CXTPImageManager* m_pImageManager;      // Image manager of items
	BOOL m_bClipItems;                      // TRUE to clip items
	CXTPControlAction* m_pAction;           // Action of the items
	CArray<CXTPControlGalleryItem*, CXTPControlGalleryItem*> m_arrItems;    // Array of items

private:
	friend class CXTPControlGalleryItem;
	friend class CXTPControlGallery;
	friend class CXTPControlGalleryAction;


};

//===========================================================================
// Summary:
//     CXTPControlGallery is a CXTPControlPopup derived class. It represents gallery control of toolbar.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlGallery : public CXTPControlPopup, public CXTPScrollBase
{
	DECLARE_XTP_CONTROL(CXTPControlGallery)

public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Structure contains item position description
	//-------------------------------------------------------------------------
	struct GALLERYITEM_POSITION
	{
		RECT rcItem;                    // Bounding rectangle of the item
		CXTPControlGalleryItem* pItem;  // Item pointer
		BOOL bBeginRow;                 // TRUE it item starts new row
	};


public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlGallery object
	//-----------------------------------------------------------------------
	CXTPControlGallery();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPControlGallery object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPControlGallery();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set items for gallery
	// Parameters:
	//     pItems - Pointer to items to be set
	//-----------------------------------------------------------------------
	void SetItems(CXTPControlGalleryItems* pItems);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set margins around items
	// Parameters:
	//     nLeft   - Left margin to be set
	//     nTop    - Top margin to be set
	//     nRight  - nRight margin to be set
	//     nBottom - Bottom margin to be set
	//-----------------------------------------------------------------------
	void SetItemsMargin(int nLeft, int nTop, int nRight, int nBottom);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set size of the control
	// Parameters:
	//     szControl - New control size to be set
	//-----------------------------------------------------------------------
	void SetControlSize(CSize szControl);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get count of items in gallery
	// Returns:
	//     Count of items in the gallery
	// See Also: SetItems, GetItem
	//-----------------------------------------------------------------------
	int GetItemCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve item by its index
	// Parameters:
	//     nIndex - Index of the item to be retrieved
	// Returns:
	//     Item in specified index; NULL if failed
	// See Also: SetItems, GetItemCount
	//-----------------------------------------------------------------------
	CXTPControlGalleryItem* GetItem(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get items associated with the gallery
	// See Also: SetItems
	//-----------------------------------------------------------------------
	CXTPControlGalleryItems* GetItems() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set selected item in the gallery
	// Parameters:
	//     nSelected - Item to select
	// See Also: GetSelectedItem
	//-----------------------------------------------------------------------
	void SetSelectedItem(int nSelected);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get selected item in the gallery
	// See Also: SetSelectedItem
	//-----------------------------------------------------------------------
	int GetSelectedItem() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set checked item identifier for the gallery
	// Parameters:
	//     nId - Item id
	// See Also: GetCheckedItem
	//-----------------------------------------------------------------------
	void SetCheckedItem(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get checked item identifier
	// See Also: SetCheckedItem
	//-----------------------------------------------------------------------
	int GetCheckedItem() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if there is selection in gallery
	// See Also: GetSelectedItem
	//-----------------------------------------------------------------------
	BOOL IsItemSelected() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to draw borders around gallery
	// Parameters:
	//     bShowBorders - TRUE to show borders around gallery
	// See Also: IsShowBorders
	//-----------------------------------------------------------------------
	void ShowBorders(BOOL bShowBorders);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if borders are visible
	// See Also: ShowBorders
	//-----------------------------------------------------------------------
	BOOL IsShowBorders() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show/hide labels of the gallery
	// Parameters:
	//     bShowLabels - TRUE to show labels
	//-----------------------------------------------------------------------
	void ShowLabels(BOOL bShowLabels);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show/hide scrollbar of the gallery
	// Parameters:
	//     bShowScrollBar - TRUE to show scrollbar
	//-----------------------------------------------------------------------
	void ShowScrollBar(BOOL bShowScrollBar);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines total borders width for gallery
	//-----------------------------------------------------------------------
	virtual CRect GetBorders() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines bounding rectangle of the item
	// Parameters:
	//     nIndex - Item index.
	//-----------------------------------------------------------------------
	CRect GetItemDrawRect(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method determines where a point lies in a specified item.
	// Parameters:
	//     point - Specifies the point to be tested.
	//     lpRect - Retrieves rectangle where item is located. Can be NULL
	// Returns:
	//     An index of CXTPControlGalleryItem that occupies the specified
	//     point or NULL if no control occupies the point.
	//-----------------------------------------------------------------------
	int HitTestItem(CPoint point, LPRECT lpRect = NULL) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines bounding rectangle of the items
	//-----------------------------------------------------------------------
	CRect GetItemsRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Redraw items
	// Parameters:
	//     lpRect - Rectangle to redraw
	//     bAnimate - TURE to animate changes
	//-----------------------------------------------------------------------
	void InvalidateItems(LPCRECT lpRect = NULL, BOOL bAnimate = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves CXTPControlGalleryPaintManager pointer to draw gallery
	//-----------------------------------------------------------------------
	CXTPControlGalleryPaintManager* GetGalleryPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to make sure that an item is visible in the gallery.
	//     This will scroll the gallery until the item is visible.
	// Parameters:
	//     nIndex - Item index that should be visible.
	//-----------------------------------------------------------------------
	void EnsureVisible(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set scroll position of gallery
	// Parameters:
	//     nScrollPos - New scrollbar position to be set
	// See Also: GetScrollPos
	//-----------------------------------------------------------------------
	void SetScrollPos(int nScrollPos);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines position of scroll bar of gallery control
	// See Also: SetScrollPos
	//-----------------------------------------------------------------------
	int GetScrollPos() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to scroll gallery for specified top item
	// Parameters:
	//     nIndex - Item to be scrolled to.
	// See Also: SetScrollPos
	//-----------------------------------------------------------------------
	int SetTopIndex(int nIndex);

public:
//{{AFX_CODEJOCK_PRIVATE
	BOOL HasBottomSeparator() const;
	long GetNext(long nIndex, int nDirection) const;
	long GetNextInRow(long nIndex, int nDirection) const;
	long GetNextInColumn(long nIndex, int nDirection) const;
	long GetNextInPage(long nIndex, int nDirection) const;
	int FindItem(int nStartAfter, LPCTSTR lpszItem, BOOL bExact) const;
	CString GetItemCaption(int nIndex);
	BOOL IsScrollButtonEnabled(int ht);
	BOOL IsScrollBarEnabled() const;
	BOOL IsShowAsButton() const;
	void HideSelection();

//}}AFX_CODEJOCK_PRIVATE

protected:

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to set position of all items
	//-------------------------------------------------------------------------
	void Reposition();

	//-----------------------------------------------------------------------
	// Summary:
	//      This method is called to draw all items
	// Parameters:
	//      pDC - Pointer to device context to draw
	//-----------------------------------------------------------------------
	void DrawItems(CDC* pDC);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to copy the control.
	// Parameters:
	//     pControl - Points to a source CXTPControl object
	//     bRecursive - TRUE to copy recursively.
	//-----------------------------------------------------------------------
	void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	void DoPropExchange(CXTPPropExchange* pPX);

protected:
//{{AFX_CODEJOCK_PRIVATE
	virtual CSize GetSize(CDC* pDC);
	virtual void Draw(CDC* pDC);

	virtual void OnMouseMove(CPoint point);

	virtual void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	virtual void OnLButtonUp(CPoint point);
	virtual BOOL OnSetSelected(int bSelected);
	virtual void SetRect(CRect rcControl);
	void DoScroll(int cmd, int pos);
	void GetScrollInfo(SCROLLINFO* pSI);
	void PerformMouseMove(CPoint point);
	void PerformMouseDown(CPoint point);
	void PerformMouseUp(CPoint point);
	void FillControl(CDC* pDC, CRect rcControl);
	virtual void SetAction(CXTPControlAction* pAction);

protected:
	void DrawScrollBar(CDC* pDC);
	CRect GetScrollBarRect();
	void CalcScrollBarInfo(LPRECT lprc, SCROLLBARPOSINFO* pSBInfo, SCROLLINFO* pSI);

	BOOL OnHookMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	BOOL IsFocused() const;
	BOOL OnHookKeyDown(UINT nChar, LPARAM lParam);
	virtual void OnScrollChanged();
	CString GetTooltip(LPPOINT pPoint, LPRECT lpRectTip, INT_PTR* nHit) const;
	void OnExecute();
	BOOL OnSetPopup(BOOL bPopup);
	void OnMouseHover();

protected:
	CWnd* GetParentWindow() const;
	virtual void AdjustExcludeRect(CRect& rc, BOOL /*bVertical*/);
	virtual void RedrawScrollBar();
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE
	virtual HRESULT GetAccessibleChildCount(long* pcountChildren);
	virtual HRESULT GetAccessibleChild(VARIANT varChild, IDispatch** ppdispChild);
	virtual HRESULT GetAccessibleName(VARIANT varChild, BSTR* pszName);
	virtual HRESULT GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole);
	virtual HRESULT AccessibleLocation(long *pxLeft, long *pyTop, long *pcxWidth, long* pcyHeight, VARIANT varChild);
	virtual HRESULT AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarChild);
	virtual HRESULT GetAccessibleState(VARIANT varChild, VARIANT* pvarState);
	virtual HRESULT GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction);
	virtual HRESULT AccessibleDoDefaultAction(VARIANT varChild);
	virtual HRESULT AccessibleSelect(long flagsSelect, VARIANT varChild);
//}}AFX_CODEJOCK_PRIVATE


protected:

	int m_nChecked;             // Checked id
	int m_nSelected;            // Index of Selected item
	BOOL m_bHideSelection;      // TRUE to hide selection
	BOOL m_bPressed;            // TRUE if item is pressed
	int m_nScrollPos;           // Scroll position
	BOOL m_bKeyboardSelected;   // TRUE if item was selected using keyboard

	int m_nTotalHeight;         // Total height of items

	BOOL m_bShowLabels;         // TRUE to show labels

	BOOL m_bShowScrollBar;      // TRUE to show scrollbars
	BOOL m_bShowBorders;        // TRUE to show borders
	CRect m_rcMargin;           // Margins of gallery items
	BOOL m_bPreview;            // TRUE if control in preview mode

	CArray<GALLERYITEM_POSITION, GALLERYITEM_POSITION&> m_arrRects; // bounding rectangles

	friend class CXTPControlGalleryItems;
};


//===========================================================================
// Summary:
//     CXTPControlGalleryPaintManager is standalone class used to draw CXTPControlGallery object
//===========================================================================
class _XTP_EXT_CLASS CXTPControlGalleryPaintManager
{
public:


	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlGalleryPaintManager object
	// Parameters:
	//     pPaintManager - PaintManager of commandbars
	//-----------------------------------------------------------------------
	CXTPControlGalleryPaintManager(CXTPPaintManager* pPaintManager);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw label control
	// Parameters:
	//     pDC - Pointer to device context
	//     rc - Bounding rectangle of the label
	//-----------------------------------------------------------------------
	virtual void DrawLabel(CDC* pDC, CXTPControlGalleryItem* pLabel, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw background of the gallery
	// Parameters:
	//      pDC - Pointer to device context
	//      pGallery - CXTPControlGallery object to draw
	//      rc - Bounding rectangle of gallery object
	//-----------------------------------------------------------------------
	virtual void FillControl(CDC* pDC, CXTPControlGallery* pGallery, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw scrollbar of the gallery
	// Parameters:
	//     pDC - Pointer to device context
	//     pScrollBar - ScrollBar to draw
	//-----------------------------------------------------------------------
	virtual void DrawScrollBar(CDC* pDC, CXTPScrollBase* pScrollBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method called to draw scrollbar of inplace gallery
	// Parameters:
	//     pDC - Pointer to device context
	//     pGallery - CXTPControlGallery object to draw
	// See Also: DrawScrollBar
	//-----------------------------------------------------------------------
	virtual void DrawPopupScrollBar(CDC* pDC, CXTPControlGallery* pGallery);

	//-------------------------------------------------------------------------
	// Summary:
	//     Recalculates gallery metrics
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns parent paintmanager object
	//-----------------------------------------------------------------------
	CXTPPaintManager* GetPaintManager() const;

public:
	int m_cxHScroll;            // Width, in pixels, of the arrow bitmap on a horizontal scroll bar
	int m_cyHScroll;            // Height, in pixels, of a horizontal scroll bar.

	int m_cyVScroll;            // Height, in pixels, of the arrow bitmap on a vertical scroll bar.
	int m_cxVScroll;            // Width, in pixels, of a vertical scroll bar;

	int m_cyPopupUp;            // Height of Up arrow of in place scrollbar
	int m_cyPopupDown;          // Height of Down arrow of in place scrollbar
	int m_cxPopup;              // Width of arrows of in place scrollbar


protected:
	CXTPWinThemeWrapper m_themeScrollBar;

protected:
	CXTPPaintManager* m_pPaintManager;
};

AFX_INLINE CXTPPaintManager* CXTPControlGalleryPaintManager::GetPaintManager() const {
	return m_pPaintManager;
}

class _XTP_EXT_CLASS CXTPControlComboBoxGalleryPopupBar : public CXTPControlComboBoxPopupBar
{
	DECLARE_XTP_COMMANDBAR(CXTPControlComboBoxGalleryPopupBar)

protected:
	CXTPControlComboBoxGalleryPopupBar();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a popup bar object.
	// Parameters:
	//     pCommandBars - Points to a CXTPCommandBars object
	// Returns:
	//     A pointer to a CXTPControlComboBoxGalleryPopupBar object
	//-----------------------------------------------------------------------
	static CXTPControlComboBoxGalleryPopupBar* AFX_CDECL CreateComboBoxGalleryPopupBar(CXTPCommandBars* pCommandBars);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	BOOL SetTrackingMode(int bMode, BOOL bSelectFirst, BOOL bKeyboard);
	BOOL OnHookKeyDown(UINT nChar, LPARAM lParam);

	CXTPControlGallery* GetGalleryItem() const;

	//{{AFX_VIRUAL(CXTPControlComboBoxList)
	virtual int GetCurSel() const {
		return GetGalleryItem()->GetSelectedItem();
	}
	virtual int FindString(int nStartAfter, LPCTSTR lpszItem) const {
		return GetGalleryItem()->FindItem(nStartAfter, lpszItem, FALSE);
	}
	virtual int FindStringExact(int nIndexStart, LPCTSTR lpsz) const {
		return GetGalleryItem()->FindItem(nIndexStart, lpsz, TRUE);
	}
	virtual int SetTopIndex(int nIndex) {
		return GetGalleryItem()->SetTopIndex(nIndex);
	}
	virtual void SetCurSel(int nIndex) {
		GetGalleryItem()->SetSelectedItem(nIndex);
	}
	virtual void GetText(int nIndex, CString& rString) const {
		rString = GetGalleryItem()->GetItemCaption(nIndex);
	}
	//}}AFX_VIRUAL

	//{{AFX_MSG(CXTPControlComboBoxList)
	void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE
};



AFX_INLINE BOOL CXTPControlGalleryItem::IsLabel() const {
	return m_bLabel;
}
AFX_INLINE void CXTPControlGalleryItem::SetCaption(LPCTSTR lpszCaption) {
	m_strCaption = lpszCaption;
}
AFX_INLINE void CXTPControlGalleryItem::SetToolTip(LPCTSTR lpszToolTip) {
	m_strToolTip = lpszToolTip;
}
AFX_INLINE CString CXTPControlGalleryItem::GetToolTip() const {
	return m_strToolTip;
}
AFX_INLINE CString CXTPControlGalleryItem::GetCaption() const {
	return m_strCaption;
}
AFX_INLINE void CXTPControlGalleryItem::SetID(int nId) {
	m_nId = nId;
}
AFX_INLINE int CXTPControlGalleryItem::GetID() const {
	return m_nId;
}
AFX_INLINE CXTPImageManager* CXTPControlGalleryItems::GetImageManager() const {
	return m_pImageManager;
}
AFX_INLINE CSize CXTPControlGalleryItems::GetItemSize() const {
	return m_szItem;
}
AFX_INLINE void CXTPControlGalleryItems::SetItemSize(CSize szItem) {
	m_szItem = szItem;
}
AFX_INLINE void CXTPControlGalleryItems::ClipItems(BOOL bClipItems) {
	m_bClipItems = bClipItems;
}

AFX_INLINE void CXTPControlGalleryItem::SetData(DWORD_PTR dwData) {
	m_dwData = dwData;
}
AFX_INLINE DWORD_PTR CXTPControlGalleryItem::GetData() const {
	return m_dwData;
}

AFX_INLINE void CXTPControlGallery::SetControlSize(CSize szControl) {
	m_nWidth = szControl.cx;
	m_nHeight = szControl.cy;
}
AFX_INLINE BOOL CXTPControlGallery::IsShowBorders() const {
	return m_bShowBorders;
}
AFX_INLINE void CXTPControlGallery::ShowLabels(BOOL bShowLabels) {
	m_bShowLabels = bShowLabels;
}
AFX_INLINE void CXTPControlGallery::ShowScrollBar(BOOL bShowScrollBar) {
	m_bShowScrollBar = bShowScrollBar;
}
AFX_INLINE void CXTPControlGallery::ShowBorders(BOOL bShowBorders) {
	m_bShowBorders = bShowBorders;
}
AFX_INLINE BOOL CXTPControlGallery::IsItemSelected() const {
	return !m_bHideSelection && GetSelectedItem() != -1;
}

#endif // #if !defined(__XTPCONTROLGALLERY_H__)
