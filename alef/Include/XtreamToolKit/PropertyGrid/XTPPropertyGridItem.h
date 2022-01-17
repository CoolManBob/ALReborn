// XTPPropertyGridItem.h interface for the CXTPPropertyGridItem class.
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
#if !defined(__XTPPROPERTYGRIDITEM_H__)
#define __XTPPROPERTYGRIDITEM_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Common/XTPSystemHelpers.h"

// class forwards.

class CXTPPropertyGridView;
class CXTPPropertyGridItem;
class CXTPPropertyGridItemMetrics;
class CXTPImageManagerIcon;
class CXTPPropertyGridInplaceButtons;
class CXTPPropertyGridInplaceSlider;
class CXTPPropertyGridInplaceSpinButton;
class CXTPPropertyGridInplaceControls;

//-----------------------------------------------------------------------
// Summary:
//     This constant is used to determine the "gutter" width of the property
//     grid.  This is the area on the left of the grid that the
//     expand buttons are drawn in.
// Remarks:
//     This is useful when drawing owner drawn controls such as buttons
//     and drop-down menus.
// Example:
//     The following example illustrates using XTP_PGI_EXPAND_BORDER:
// <code>
// void CDelphiGridPage::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
// {
//     // code snip ...
//
//     CRect rcText(rc);
//     rcText.left = max(1, pItem->GetIndent()) * XTP_PGI_EXPAND_BORDER + 3;
//     rcText.right = rcCaption.right - 1;
//     rcText.bottom -= 1;
//     dc.DrawText(pItem->GetCaption(), rcText, DT_SINGLELINE | DT_VCENTER);
// }
// </code>
//-----------------------------------------------------------------------
const int XTP_PGI_EXPAND_BORDER = 14;

//-----------------------------------------------------------------------
// Summary:
//     Flags that indicate what styles to set for the grid item.
// Example:
//     <code>pItem->SetFlags(xtpGridItemHasEdit | xtpGridItemHasComboButton);</code>
// See Also: CXTPPropertyGridItem::SetFlags, OnInplaceButtonDown
//
// <KEYWORDS xtpGridItemHasEdit, xtpGridItemHasExpandButton, xtpGridItemHasComboButton>
//-----------------------------------------------------------------------
enum XTPPropertyGridItemFlags
{
	xtpGridItemHasEdit         = 1, // Item has an edit control.
	xtpGridItemHasExpandButton = 2, // Item has an expand button.
	xtpGridItemHasComboButton  = 4  // Item has a combo button.
};

//===========================================================================
// Summary:
//     CXTPPropertyGridItems is a CList derived class. It represents the items
//     collection with some internal functionality.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridItems : public CXTPCmdTarget
{

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to find an item.
	// Parameters:
	//     strCaption - Caption for the item to find.
	//     nID - Identifier for the item to find.
	// Returns:
	//     A pointer to a CXTPPropertyGridItem object or NULL if item was not found.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* FindItem(UINT nID) const;
	CXTPPropertyGridItem* FindItem(LPCTSTR strCaption) const; // <COMBINE CXTPPropertyGridItems::FindItem@UINT@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine the index of a CXTPPropertyGridItem.
	// Parameters:
	//     pItem - Pointer to the CXTPPropertyGridItem you want to know
	//             the index of.
	// Returns:
	//     The index of the item within the collection of items, -1 if
	//     the item is not found.
	//-----------------------------------------------------------------------
	int Find(CXTPPropertyGridItem* pItem) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to clear all the items in a list.
	//-----------------------------------------------------------------------
	void Clear();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get count of the items in a list.
	// Returns:
	//     Count of the items in a list.
	//-----------------------------------------------------------------------
	long GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if any items have been added
	//     to the property grid.
	// Returns:
	//     TRUE if there are no items in the grid, FALSE if at least
	//     one item has been added to the grid.
	//-----------------------------------------------------------------------
	BOOL IsEmpty() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get an item by its index.
	// Parameters:
	//     nIndex - Item's index.
	// Returns:
	//     A pointer to a CXTPPropertyGridItem object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to remove an item by its index.
	// Parameters:
	//     nIndex - Item's index.
	//-----------------------------------------------------------------------
	void RemoveAt (int nIndex);

	//-------------------------------------------------------------------------
	// Summary:
	//     This member sorts the collection of items in alphabetical order.
	// Remarks:
	//     This method uses Visual C++ run-time library (MSVCRT)
	//     implementation of the quick-sort function, qsort, for sorting
	//     stored CXTPPropertyGridItem objects.
	//-------------------------------------------------------------------------
	void Sort();


protected:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridItems object.
	//-------------------------------------------------------------------------
	CXTPPropertyGridItems();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member adds a new item to the end (tail) of the
	//     Property Grid's collection of items. The collection can be empty
	//     before the operation.
	// Parameters:
	//     pItem - Pointer to the CXTPPropertyGridItem to add to the collection.
	//     pItems - Pointer to the collection of CXTPPropertyGridItems to
	//              add to the collection.
	// Remarks:
	//     The pItems version adds a collection of items to the end (tail) of the
	//     Property Grid's collection of items. The collection can be empty
	//     before the operation.
	//-----------------------------------------------------------------------
	void AddTail(CXTPPropertyGridItem* pItem);
	void AddTail(CXTPPropertyGridItems* pItems); //<combine CXTPPropertyGridItems::AddTail@CXTPPropertyGridItem*>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to insert a CXTPPropertyGridItem object
	//     into the collections of items at a specific location specified
	//     by nIndex.
	// Parameters:
	//     nIndex - Position in the collection of items to insert pItem.
	//     pItem  - Pointer to the CXTPPropertyGridItem to add to the collection.
	//-----------------------------------------------------------------------
	void InsertAt(int nIndex, CXTPPropertyGridItem* pItem);

private:
	static int _cdecl CompareFunc(const CXTPPropertyGridItem** ppItem1, const CXTPPropertyGridItem** ppItem2);

private:
	CArray<CXTPPropertyGridItem*, CXTPPropertyGridItem*> m_arrItems;
	//CXTPPropertyGridView* m_pGrid;

	friend class CXTPPropertyGridItem;
	friend class CXTPPropertyGridView;
};

//===========================================================================
// Summary:
//     CXTPPropertyGridItemConstraint is a CCmdTarget derived class. It
//     represents a single item constraints.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridItemConstraint : public CXTPCmdTarget
{
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridItemConstraint object.
	//-------------------------------------------------------------------------
	CXTPPropertyGridItemConstraint();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the index of this constraint
	//     within the collection of constraints.
	// Returns:
	//     Index of this constraint.
	//-----------------------------------------------------------------------
	virtual int GetIndex() const;

	//-----------------------------------------------------------------------
	// Summary: Retrieves the image of the constraint so it can be used outside of Codejock components.
	// Returns: CXTPImageManagerIcon object containing the image of the constraint.
	//-----------------------------------------------------------------------
	virtual CXTPImageManagerIcon* GetImage() const;

public:
	CString m_strConstraint;    // Caption text of constraint.  This is the
	                            // text displayed for this constraint.
	DWORD_PTR   m_dwData;       // The 32-bit value associated with the item.
	int     m_nImage;           // Image index
protected:
	int m_nIndex;               // Index of constraint.
	CXTPPropertyGridItem*  m_pItem; // Parent item

	friend class CXTPPropertyGridItemConstraints;

};

//===========================================================================
// Summary:
//     CXTPPropertyGridItemConstraints is a CCmdTarget derived class. It represents the item
//     constraints collection.
// Remarks:
//     Each PropertyGridItem has it's own collection of constraints.
//     CXTPPropertyGridItem::GetConstraints allows to access the constraints
//     of a given item.
//     If a combo button is added to the property grid item, all
//     the constraints in the item's constraint collection will be displayed
//     in the drop-down list.  The AddConstraint method is used to add
//     constraints to the collection.
// See Also: CXTPPropertyGridItem::GetConstraints, CXTPPropertyGridItem::SetConstraintEdit, AddConstraint
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridItemConstraints : public CXTPCmdTarget
{
protected:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridItemConstraints object.
	// Parameters:
	//     pItem - Pointer to the CXTPPropertyGridItem that the constraints are
	//             added to.
	//-------------------------------------------------------------------------
	CXTPPropertyGridItemConstraints(CXTPPropertyGridItem* pItem);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropertyGridItemConstraints object, handles
	//     cleanup and deallocation
	//-------------------------------------------------------------------------
	~CXTPPropertyGridItemConstraints();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to add a new item constraint.
	// Parameters:
	//     str    - New constraint.
	//     dwData - Data associated with the constraint.
	//     nImage - Image of constraint
	// Returns:
	//     A pointer to the newly added constraint.
	// See Also: CXTPPropertyGridItem::OnConstraintsChanged
	//-----------------------------------------------------------------------
	virtual CXTPPropertyGridItemConstraint* AddConstraint(LPCTSTR str, DWORD_PTR dwData = 0, int nImage = -1);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the total number of constraints
	//     in the collection.
	//-------------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the list of constraints is empty.
	// Returns:
	//     TRUE if there are no constraints in the collection, FALSE is
	//     at least 1 constraint is in the collection.
	//-----------------------------------------------------------------------
	BOOL IsEmpty() const;

	//-------------------------------------------------------------------------
	// Summary:
	//     Removes all the constraints from the collection.
	// See Also: RemoveAt
	//-------------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to remove a constraint by its index.
	// Parameters:
	//     nIndex - Index of a constraint.
	// See Also: RemoveAll
	//-----------------------------------------------------------------------
	void RemoveAt (int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the current constraint.
	// Parameters:
	//     nIndex - Index of the needed constraint.
	//-----------------------------------------------------------------------
	void SetCurrent(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the current constraint.
	// Returns:
	//     The index of the current constraint.
	//-----------------------------------------------------------------------
	int GetCurrent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to find a constraint in the list.
	// Parameters:
	//     str   - A constraint to search for.
	//     dwData - A constraint to search for.
	// Returns:
	//     The zero-based index of the located constraint or -1 if the constraint
	//     is not found.
	//-----------------------------------------------------------------------
	int FindConstraint(LPCTSTR str) const;
	int FindConstraint(DWORD_PTR dwData) const; // <COMBINE CXTPPropertyGridItemConstraints::FindConstraint@LPCTSTR@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get a constraint by its index.
	// Parameters:
	//     nIndex - Index of a constraint.
	// Returns:
	//     Caption of constraint if found, otherwise it will return "".
	// Remarks:
	//     This will only return the caption text of a constraint, to
	//     return the entire constraint, use GetConstraintAt.
	// See Also: GetConstraintAt
	//-----------------------------------------------------------------------
	CString GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get a constraint by its index.
	// Parameters:
	//     nIndex - Index of a constraint.
	// Returns:
	//     Pointer to CXTPPropertyGridItemConstraint object.
	// Remarks:
	//     This will only the entire constraint object, to return only
	//     the caption text of a constraint, use GetAt.
	// See Also: GetAt
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemConstraint* GetConstraintAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to sort constraints alphabetically.
	//-----------------------------------------------------------------------
	virtual void Sort();

private:
	static int _cdecl CompareFunc(const CXTPPropertyGridItemConstraint** ppConstraint1, const CXTPPropertyGridItemConstraint** ppConstraint2);

private:
	CArray<CXTPPropertyGridItemConstraint*, CXTPPropertyGridItemConstraint*>  m_arrConstraints;
	int m_nCurrent;
	CXTPPropertyGridItem* m_pItem;


	friend class CXTPPropertyGridItem;

};

//===========================================================================
// Summary:
//     CXTPPropertyGridItem is the base class representing an item of the
//     Property Grid Control.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridItem : public CXTPCmdTarget, public CXTPAccessible
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridItem object
	// Parameters:  lpszCaption   - Caption of the item.
	//              nID           - Identifier of the item.
	//              strValue      - Initial value.
	//              pBindString   - If not NULL, then the value of this item
	//                              is bound the value of this variable.
	// Remarks:
	//     Class CXTPPropertyGridItem has no default constructor.
	//
	//          When using the second constructor, the Identifier (nID) of the
	//          second constructor can be linked with a STRINGTABLE resource
	//           with the same id in such form "Caption\\nDescription".
	//
	//          BINDING:
	//            Variables can be bound to an item in two ways, the first is
	//            to pass in a variable at the time of creation, the second allows
	//            variables to be bound to an item after creation with the
	//            BindToString member.
	//
	//            Bound variables store the values of the property grid items
	//            and can be accessed without using the property grid methods
	//            and properties.  Bound variables allow the property grid to
	//            store data in variables.  When the value of a PropertyGridItem
	//            is changed, the value of the bound variable will be changed to
	//            the PropertyGridItem value.  The advantage of binding is that
	//            the variable can be used and manipulated without using
	//            PropertyGridItem methods and properties.
	//
	//            NOTE:  If the value of the variable is changed without using
	//            the PropertyGrid, the PropertyGridItem value will not be
	//            updated until you call CXTPPropertyGrid::Refresh.
	// See Also: BindToString
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem(LPCTSTR lpszCaption, LPCTSTR strValue = NULL, CString* pBindString = NULL);
	CXTPPropertyGridItem(UINT nID, LPCTSTR strValue = NULL, CString* pBindString = NULL);  // <COMBINE CXTPPropertyGridItem::CXTPPropertyGridItem@LPCTSTR@LPCTSTR@CString*>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropertyGridItem object, handles cleanup and
	//     deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridItem();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the caption of the item.
	// Parameters:
	//     lpszCaption - The new caption of the item.
	// See Also: SetPrompt
	//-----------------------------------------------------------------------
	void SetCaption(LPCTSTR lpszCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to load a string from a STRINGTABLE resource
	//     to set the caption and description.
	// Parameters:
	//     lpszPrompt - String from resource file that contains the
	//                  caption and description of the item.  Caption
	//                  and description must be separated by "\n" if both
	//                  are included.  You can omit the description and
	//                  only the caption will be set.
	// Remarks:
	//     Resource strings are in the format "Caption\\nDescription".
	// See Also: SetCaption, SetDescription
	//-----------------------------------------------------------------------
	void SetPrompt(LPCTSTR lpszPrompt);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the description of the item.  This is
	//     the text displayed in the help panel.
	// Parameters:
	//     lpszDescription - The new description of the item.
	// See Also: SetPrompt
	//-----------------------------------------------------------------------
	void SetDescription(LPCTSTR lpszDescription);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the read-only state of the item.
	// Parameters:
	//     bReadOnly - Specifies whether to set or remove the read-only state of the
	//     item. A value of TRUE sets the state to read-only. A value of FALSE sets
	//     the state to read/write.
	//-----------------------------------------------------------------------
	void SetReadOnly(BOOL bReadOnly = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the identifier of the item.
	// Parameters:
	//     nID - The new identifier of the item.
	//-----------------------------------------------------------------------
	void SetID(UINT nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to set a specific flag for the item.
	// Parameters:
	//     nFlags - Flag to set. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     nFlags value can be one or more of the following:
	//      * <b>xtpGridItemHasEdit</b> Item has edit control.
	//      * <b>xtpGridItemHasExpandButton</b> Item has expand button.
	//      * <b>xtpGridItemHasComboButton</b> Item has combo button.
	// See Also: XTPPropertyGridItemFlags, OnInplaceButtonDown
	//-----------------------------------------------------------------------
	void SetFlags(UINT nFlags);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the caption of the item.
	// Returns:
	//     A CString object containing the caption of the item.
	//-----------------------------------------------------------------------
	CString GetCaption() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the description of the item.
	// Returns:
	//     A CString object containing the description of the item.
	//-----------------------------------------------------------------------
	CString GetDescription() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the read-only state of the item.
	// Returns:
	//     TRUE if the item is read only, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL GetReadOnly() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the identifier of the item.
	// Returns:
	//     An integer value that represents the identifier of the item.
	//-----------------------------------------------------------------------
	UINT GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the index of the item in property grid list
	// Returns:
	//     An integer value that represents the index of the item.
	//-----------------------------------------------------------------------
	int GetIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the current value of the item.
	// Returns:
	//     A CString object containing the value of the item.
	//-----------------------------------------------------------------------
	CString GetValue() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the XTPPropertyGridItemFlags of the item.
	// Returns:
	//     A integer value that represents item's flags.  Value will
	//     be a member of XTPPropertyGridItemFlags.
	// See Also: XTPPropertyGridItemFlags
	//-----------------------------------------------------------------------
	UINT GetFlags() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     The IsCategory function determines whether or not the item is
	//     a category.
	// Returns:
	//     TRUE if the item is a category, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsCategory() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add a child item.
	// Parameters:
	//     pItem - The item to be added as a child.
	// Returns:
	//     A pointer to a CXTPPropertyGridItem object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* AddChildItem(CXTPPropertyGridItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to insert a child item.
	// Parameters:
	//     pItem - The item to be added as a child.
	//     nIndex - Index of item to be added as a child.
	// Returns:
	//     A pointer to a CXTPPropertyGridItem object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* InsertChildItem(int nIndex, CXTPPropertyGridItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     The HasChilds function determines whether or not the item has children.
	// Returns:
	//     TRUE if the item has children, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL HasChilds() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     The HasChilds function determines whether or not the item has visible children.
	// Returns:
	//     TRUE if the item has visible children, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL HasVisibleChilds() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a list of the children
	// Returns:
	//     A pointer to a CXTPPropertyGridItems object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItems* GetChilds() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     The HasParent function determines whether or not the item has a pParent item
	//     as a parent.
	// Parameters:
	//     pParent - Item to be tested.
	// Returns:
	//     TRUE if pParent is a parent of the item.
	//-----------------------------------------------------------------------
	BOOL HasParent(CXTPPropertyGridItem* pParent);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get the constraints list.
	// Returns:
	//     A pointer to a CXTPPropertyGridItemConstraints object.
	// Example: <code> CXTPPropertyGridItemConstraints* pList = pItem->GetConstraints(); </code>
	// See Also: CXTPPropertyGridItemConstraints::AddConstraint, OnConstraintsChanged
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemConstraints* GetConstraints() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to edit only values from constraint list.
	// Parameters:
	//     bConstraintEdit - TRUE to constraint edit.
	//-----------------------------------------------------------------------
	void SetConstraintEdit(BOOL bConstraintEdit);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves constraint edit state.
	// Returns:
	//     TRUE if item is constraint edit.
	//-----------------------------------------------------------------------
	BOOL GetConstraintEdit() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to expand the item.
	//-----------------------------------------------------------------------
	void Expand();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to collapse the item.
	//-----------------------------------------------------------------------
	void Collapse();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to select the item.
	//-----------------------------------------------------------------------
	void Select();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the bounding rectangle of the item
	// Returns:
	//     A CRect object that represents the bounding rectangle of the item.
	//-----------------------------------------------------------------------
	CRect GetItemRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the bounding rectangle of the item value's text.
	//     Override this member function to change it.
	// Returns:
	//     A CRect object that represents the bounding rectangle of the
	//     item value's text.
	//-----------------------------------------------------------------------
	virtual CRect GetValueRect();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to change an item's value.
	//     Override this method to add new functionality.
	//     You should call the base class version of this function from your
	//     override.
	// Parameters:
	//     strValue - New value of the item.
	//-----------------------------------------------------------------------
	virtual void SetValue(CString strValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to hide/unhide item
	// Parameters:
	//     bHidden - TRUE to hide item
	//-----------------------------------------------------------------------
	void SetHidden(BOOL bHidden);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the item is hidden.
	// Returns:
	//     TRUE if the item is hidden, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsHidden() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when an item's value is changed. Override this
	//     function if needed.
	// Parameters:
	//     strValue - New value of the item.
	//-----------------------------------------------------------------------
	virtual void OnValueChanged(CString strValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to return the parent window
	// Returns:
	//     The parent window.
	//-----------------------------------------------------------------------
	CXTPPropertyGridView* GetGrid() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the item is visible.
	// Returns:
	//     TRUE if the item is visible, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the item is expanded.
	// Returns:
	//     TRUE if the item is expanded, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsExpanded() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the parent property grid item.
	// Returns:
	//     A CXTPPropertyGridItem pointer that represents the parent item, can be
	//     NULL.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* GetParentItem() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the item's indent.
	// Returns:
	//     Item's indent.
	//-----------------------------------------------------------------------
	int GetIndent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when an item is drawn. Override this function if
	//     needed.
	// Parameters:
	//     dc - Reference to the device context to be used for rendering an image
	//     of the item.
	//     rcValue - Bounding rectangle of the item.
	// Returns:
	//     TRUE if item is self-drawn.
	//-----------------------------------------------------------------------
	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when an constraint in inplace list is drawn. Override this function if
	//     needed.
	// Parameters:
	//     pDC - Reference to the device context to be used for rendering an image
	//     of the item.
	//     pConstraint - Constraint to be drawn
	//     rc - Bounding rectangle of the constraint.
	//     bSelected - TRUE if item is selected.
	//-----------------------------------------------------------------------
	virtual void OnDrawItemConstraint(CDC* pDC, CXTPPropertyGridItemConstraint* pConstraint, CRect rc, BOOL bSelected);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get size of constraint in inplace list. Override this function if
	//     needed.
	// Parameters:
	//     pDC - Reference to the device context to be used for rendering an image
	//     of the item.
	//     pConstraint - Constraint to be drawn
	//-----------------------------------------------------------------------
	virtual CSize OnMergeItemConstraint(CDC* pDC, CXTPPropertyGridItemConstraint* pConstraint);

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
	//     Retrieves if the item is selected.
	// Returns:
	//     TRUE if the item is selected, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsSelected() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to bind an item to a CString object.
	// Parameters:
	//     pBindString - CString object to bind to item.
	// Remarks:
	//     Variables can be bound to an item in two ways, the first is
	//     to pass in a variable at the time of creation, the second allows
	//     variables to be bound to an item after creation with the
	//     BindToString member.
	//
	//     Bound variables store the values of the property grid items
	//     and can be accessed without using the property grid methods
	//     and properties.  Bound variables allow the property grid to
	//     store data in variables.  When the value of a PropertyGridItem
	//     is changed, the value of the bound variable will be changed to
	//     the PropertyGridItem value.  The advantage of binding is that
	//     the variable can be used and manipulated without using
	//     PropertyGridItem methods and properties.
	//
	//     NOTE:  If the value of the variable is changed without using
	//     the PropertyGrid, the PropertyGridItem value will not be
	//     updated until you call CXTPPropertyGrid::Refresh.
	//-----------------------------------------------------------------------
	void BindToString(CString* pBindString);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes this item.
	//-----------------------------------------------------------------------
	void Remove();

	//-----------------------------------------------------------------------
	// Summary:
	//     Applies the specified Mask to a CXTPPropertyGridItem data field
	//     to to create a restrictive field for text entry.
	// Parameters:
	//     strMask    - Mask of the item
	//     strLiteral - Defines the prompt area of the mask where text entry
	//                  is allowed.  This should match the areas that you
	//                  have already defined as mask fields.  By default
	//                  the prompt character is '_'.
	//
	//                  Phone No: (___) ___-____
	//
	//
	//     chPrompt   - This value is optional, this is the prompt character
	//                  used in strLiteral to indicate a place holder.  By
	//                  default this is the underscore '_' character.
	// Remarks:
	//     You can use any of these characters for defining your edit mask:
	//     <TABLE>
	//          <b>Mask Character</b>  <b>Description</b>
	//          ---------------------  ------------------------
	//          0                      Numeric (0-9)
	//          9                      Numeric (0-9) or space (' ')
	//          #                      Numeric (0-9) or space (' ') or ('+') or ('-')
	//          L                      Alpha (a-Z)
	//          ?                      Alpha (a-Z) or space (' ')
	//          A                      Alpha numeric (0-9 and a-Z)
	//          a                      Alpha numeric (0-9 and a-Z) or space (' ')
	//          &                      All print character only
	//          H                      Hex digit (0-9 and A-F)
	//          X                      Hex digit (0-9 and A-F) and space (' ')
	//          >                      Forces characters to upper case (A-Z)
	//          <                      Forces characters to lower case (a-z)
	//     </TABLE>
	//
	//      Any of these characters can be combined with additional
	//      characters to create a restrictive field for text entry.
	//      For example, if you wanted to display a prompt for a
	//      telephone number and wanted to restrict only numeric
	//      values, but wanted to display the area code inside of
	//      parentheses '(' and ')' then you could define your
	//      mask as:
	//
	//      Phone No: (000) 000-0000
	//
	// See Also: GetMaskedText, SetMaskedText
	//-----------------------------------------------------------------------
	void SetMask(LPCTSTR strMask, LPCTSTR strLiteral, TCHAR chPrompt = _T('_'));

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set a password mask for this item.  By
	//     default, all characters will be replaced by an asterisk (*).
	// Parameters:
	//     chMask - Character to use as the password mask.
	// Remarks:
	//     The Value will not contain any asterisk (*) or mask, you can
	//     return the actual text with the GetMaskedText member.
	//     You can specify which character will be used as the password
	//     mask.
	//-----------------------------------------------------------------------
	void SetPasswordMask(TCHAR chMask = _T('*'));

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the actual text value for the property
	//     grid item without the "edit mask" characters added by the
	//     SetMask member.
	// Returns:
	//     Actual text value of the item without mask characters.
	// See Also: SetMaskedText, SetMask
	//-----------------------------------------------------------------------
	CString GetMaskedText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member called to determine if value was changed by user.
	// Remarks:
	//     Call CXTPPropertyGrid::HighlightChangedItems to highlight all items with changed values
	// Returns:
	//     TRUE if value was changed
	// See Also: CXTPPropertyGrid::HighlightChangedItems, ResetValue, SetDefaultValue
	//-----------------------------------------------------------------------
	virtual BOOL IsValueChanged() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to rest value to its default
	// See Also: SetDefaultValue, IsValueChanged
	//-----------------------------------------------------------------------
	virtual void ResetValue();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set default value of the item
	// Parameters:
	//     lpszDefaultValue - Value to be set as default.
	// See Also: ResetValue, IsValueChanged
	//-----------------------------------------------------------------------
	void SetDefaultValue(LPCTSTR lpszDefaultValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the actual text value for the property
	//     grid item without the "edit mask" characters added by the
	//     SetMask member.
	// Parameters:
	//     lpszMaskedText - Text string without mask.
	// Remarks:
	//     If a mask is used, then the mask will be applied to the text
	//     in lpszMaskedText.
	// See Also: GetMaskedText, SetMask
	//-----------------------------------------------------------------------
	void SetMaskedText(LPCTSTR lpszMaskedText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the value of the item that the
	//     user is viewing.
	// Returns:
	//     CString value of the data the user is currently seeing.
	//     I.e. If a password mask it used, then the string returned
	//     will be filled with asterisks (*).  If no password mask is used,
	//     it will return the same thing as GetValue.
	//-----------------------------------------------------------------------
	virtual CString GetViewValue();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the 32-bit value associated with the item.
	// Parameters:
	//     dwData - Contains the new value to associate with the item.
	//-----------------------------------------------------------------------
	void SetItemData(DWORD_PTR dwData);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the application-supplied 32-bit value
	//     associated with the item
	// Returns:
	//     The 32-bit value associated with the item
	//-----------------------------------------------------------------------
	DWORD_PTR GetItemData() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return in-place edit control of
	//     the item
	// Returns:
	//     A reference to an in-place edit control.
	//-----------------------------------------------------------------------
	virtual CXTPPropertyGridInplaceEdit& GetInplaceEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set focus to item in-place control.
	//-----------------------------------------------------------------------
	virtual void SetFocusToInplaceControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if in-place buttons are visible for item
	// See Also: CXTPPropertyGrid::SetShowInplaceButtonsAlways
	//-----------------------------------------------------------------------
	virtual BOOL IsInplaceButtonsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves collection of in-place buttons for the item
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceButtons* GetInplaceButtons() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves collection of in-place controls for the item
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceControls* GetInplaceControls() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add standard combo button
	//-----------------------------------------------------------------------
	void AddComboButton();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add standard expand button
	//-----------------------------------------------------------------------
	void AddExpandButton();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add slider control
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceSlider* AddSliderControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add spin control
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceSpinButton* AddSpinButton();


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get metrics of value part
	// Returns:
	//     Pointer to CXTPPropertyGridItemMetrics contained metrics (color, style) of value part
	// See Also: CXTPPropertyGridItemMetrics, GetCaptionMetrics
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemMetrics* GetValueMetrics() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get metrics of caption part
	// Returns:
	//     Pointer to CXTPPropertyGridItemMetrics contained metrics (color, style) of caption part
	// See Also: CXTPPropertyGridItemMetrics, GetValueMetrics
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemMetrics* GetCaptionMetrics() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get metrics of caption part or value part
	// Parameters:
	//     bValueMetrics - TRUE to retrieve the value metrics
	//     bCreate - TRUE to call GetCaptionMetrics or GetValueMetrics to get metrics
	// Returns:
	//     Pointer to CXTPPropertyGridItemMetrics contained metrics (color, style) of caption part
	// See Also: CXTPPropertyGridItemMetrics, GetValueMetrics
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemMetrics* GetMetrics(BOOL bValueMetrics, BOOL bCreate = TRUE) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get variable height of the item.
	// Returns:
	//     Height of the item or -1 if default values used.
	//-----------------------------------------------------------------------
	int GetHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set variable height of the item.
	// Parameters:
	//     nHeight - Height of item or -1 to use default value
	// Remarks:
	//     Call CXTPPropertyGrid::SetVariableItemsHeight to use variable height items
	// See Also: CXTPPropertyGrid::SetVariableItemsHeight, SetMultiLinesCount
	//-----------------------------------------------------------------------
	void SetHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if item is multi line.
	// Returns:
	//     TRUE if item has more than one view line.
	// See Also: SetMultiLinesCount
	//-----------------------------------------------------------------------
	BOOL IsMultiLine() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get total item lines
	// Returns:
	//     Total lines count of multi line item
	//-----------------------------------------------------------------------
	int GetMultiLinesCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set total lines of multi line item
	// Parameters:
	//     nLinesCount - Total lines of multi line item
	// Remarks:
	//     Call CXTPPropertyGrid::SetVariableItemsHeight to use variable height items
	//-----------------------------------------------------------------------
	void SetMultiLinesCount(int nLinesCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to ensure that the item is visible.
	//-----------------------------------------------------------------------
	void EnsureVisible();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before user start edit the item
	// Returns:
	//     TRUE to allow edit item; FALSE to forbid
	//-----------------------------------------------------------------------
	virtual BOOL OnRequestEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before user apply new value of the item
	// Parameters:
	//     strValue - Value to be set
	// Returns:
	//     TRUE to allow new item value; FALSE to forbid
	//-----------------------------------------------------------------------
	virtual BOOL OnAfterEdit(CString& strValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called if user cancel edit item
	//-----------------------------------------------------------------------
	virtual void OnCancelEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set edit styles of the item.
	// Parameters:
	//     dwEditStyle - New style to be set
	// Example:
	//     <code>pItem->SetEditStyle(ES_MULTILINE | ES_AUTOVSCROLL);</code>
	//-----------------------------------------------------------------------
	void SetEditStyle(DWORD dwEditStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the count of items in the dropdown list.
	// Parameters:
	//     nDropDownItemCount - The count of items in the dropdown list.
	//-----------------------------------------------------------------------
	void SetDropDownItemCount(int nDropDownItemCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves count of items in the dropdown list.
	// Returns:
	//     Count of items in the dropdown list
	// See Also: SetDropDownItemCount
	//-----------------------------------------------------------------------
	int GetDropDownItemCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to check value from in-place edit control.
	//-----------------------------------------------------------------------
	virtual void OnValidateEdit();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get edit styles of the item, override it if you need custom style
	// Remarks:
	//     By default it returns WS_CHILD | ES_AUTOHSCROLL
	// Returns:
	//     Styles of the edit control.
	//-----------------------------------------------------------------------
	virtual DWORD GetEditStyle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when visibility state of the item was changed.
	// Parameters:
	//     bVisible - TRUE to set the control visible.
	//-----------------------------------------------------------------------
	virtual void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when index of the item was changed
	//-----------------------------------------------------------------------
	virtual void OnIndexChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when an item is selected.
	//-----------------------------------------------------------------------
	virtual void OnSelect();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when an item is de-selected.
	//-----------------------------------------------------------------------
	virtual void OnDeselect();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when a key is pressed.
	// Parameters:
	//     nChar - Contains the character code value of the key.
	// Returns:
	//     TRUE if a key is pressed, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL OnChar(UINT nChar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the user double-clicks the left
	//     mouse button.
	// Parameters:
	//     nFlags - Indicates whether various virtual keys are down
	//              (see Visual C++ documentation).
	//     point  - Specifies the x- and y-coordinate of the cursor.
	//-----------------------------------------------------------------------
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the user clicks the right mouse
	//     button down.
	// Parameters:
	//     nFlags - Indicates whether various virtual keys are down
	//              (see Visual C++ documentation).
	//     point  - Specifies the x- and y-coordinate of the cursor.
	//-----------------------------------------------------------------------
	virtual void OnRButtonDown(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the user presses the left mouse button.
	// Parameters:
	//     nFlags - Indicates whether various virtual keys are down
	//              (see Visual C++ documentation).
	//     point  - Specifies the x- and y-coordinate of the cursor.
	// Returns:
	//     TRUE if the message processed, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the user release the left mouse button.
	// Parameters:
	//     nFlags - Indicates whether various virtual keys are down
	//     (see Visual C++ documentation).
	//     point - Specifies the x- and y-coordinate of the cursor.
	//-----------------------------------------------------------------------
	virtual void OnLButtonUp(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when a non-system key is pressed.
	// Parameters:
	//     nChar - Specifies the virtual-key code of the given key.
	//-----------------------------------------------------------------------
	virtual BOOL OnKeyDown (UINT nChar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when an item is added to the parent.
	//-----------------------------------------------------------------------
	virtual void OnAddChildItem();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the user presses the in-place button.
	//     Override the method to show an item-specific dialog.
	//-----------------------------------------------------------------------
	virtual void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return in-place list control of
	//     the item
	// Returns:
	//     A reference to an in-place list control.
	//-----------------------------------------------------------------------
	virtual CXTPPropertyGridInplaceList& GetInplaceList();


	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set text of thein-placee edit control.
	// Parameters:
	//     str - String value
	//-----------------------------------------------------------------------
	virtual void SetEditText(const CString& str);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called before the item becomes visible in the
	//     property grid.
	// Remarks:
	//     Before the item is inserted, it is first check to see if it
	//     is bound to a variable, if it is, then the value of the item
	//     is updated with the value stored in the bound variable.
	//
	//          OnBeforeInsert is called when an item is inserted,
	//          when a category is inserted, when a category or item is
	//          expanded, and when the sort property has changed.
	//-----------------------------------------------------------------------
	virtual void OnBeforeInsert();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when constraints are added to this item.
	// Remarks:
	//     OnConstraintsChanged is called when
	//     CXTPPropertyGridItemConstraints::AddConstraint is called.
	// See Also: CXTPPropertyGridItemConstraints::AddConstraint, GetConstraints
	//-----------------------------------------------------------------------
	virtual void OnConstraintsChanged();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to switch constraints.
	//-----------------------------------------------------------------------
	BOOL SelectNextConstraint();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if point belongs value rectangle
	// Parameters:
	//     point - Point need to check.
	//-----------------------------------------------------------------------
	BOOL PtInValueRect(CPoint point);

protected:
//{{AFX_CODEJOCK_PRIVATE
	// System accessibility Support
	virtual HRESULT GetAccessibleParent(IDispatch** ppdispParent);
	virtual HRESULT GetAccessibleDescription(VARIANT varChild, BSTR* pszDescription);
	virtual HRESULT GetAccessibleName(VARIANT varChild, BSTR* pszName);
	virtual HRESULT GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole);
	virtual HRESULT AccessibleLocation(long *pxLeft, long *pyTop, long *pcxWidth, long* pcyHeight, VARIANT varChild);
	virtual HRESULT AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarChild);
	virtual HRESULT GetAccessibleState(VARIANT varChild, VARIANT* pvarState);
	virtual CCmdTarget* GetAccessible();
	virtual HRESULT AccessibleSelect(long flagsSelect, VARIANT varChild);
	virtual HRESULT GetAccessibleValue(VARIANT varChild, BSTR* pszValue);
	DECLARE_INTERFACE_MAP()
//}}AFX_CODEJOCK_PRIVATE

private:
	void Init();
	void Refresh(BOOL bInvalidate = TRUE);
	void Clear();
	CRect GetInplaceWindowsRect();


protected:
	BOOL                    m_bConstraintEdit;  // TRUE to constraint edit.

	int                     m_nIndex;           // Index of the item.
	int                     m_nIndent;          // Indent of the item.
	UINT                    m_nID;              // Identifier of the item.
	UINT                    m_nFlags;           // Item's flags.
	BOOL                    m_bReadOnly;        // TRUE to disable item's edition.
	BOOL                    m_bVisible;         // Visibility of the item.
	BOOL                    m_bCategory;        // TRUE if the item is category.
	BOOL                    m_bExpanded;        // TRUE if item is expanded.
	CString                 m_strValue;         // Value of the item.
	CString                 m_strDefaultValue;  // Default value of the item.
	CString                 m_strCaption;       // Caption of the item.
	CString                 m_strDescription;   // Description of the item.
	CXTPPropertyGridItem*   m_pParent;          // Parent item.
	CXTPPropertyGridView*   m_pGrid;            // Parent grid class.
	CXTPPropertyGridItems*  m_pChilds;          // Child items.
	CXTPPropertyGridItemConstraints*    m_pConstraints;     // Item's constraints.


	mutable CXTPPropertyGridItemMetrics* m_pValueMetrics;   // Value part metrics
	mutable CXTPPropertyGridItemMetrics* m_pCaptionMetrics; // Caption part metrics

	CXTPPropertyGridInplaceButtons* m_pInplaceButtons;          // Inplace buttons array
	CXTPPropertyGridInplaceControls* m_pInplaceControls;

	int m_nDropDownItemCount;   // Maximum drop down items

	CString*                m_pBindString;      // Binded string.
	CString m_strFormat;                        // Format of the double value.

	CString m_strMask;                          // String to use as edit mask.
	CString m_strLiteral;                       // String to use as literal.  This is the same as the edit mask, but all mask characters are replaced with m_chPrompt.
	TCHAR m_chPrompt;                           // Character used as a space holder for a character.  This is used in m_strLiteral.
	bool m_bUseMask;                            // TRUE to use and edit mask to display item data.

	BOOL m_bPassword;                           // TRUE to use a password mask.  If TRUE, then each character will be replaced with an asterisk (*).
	DWORD_PTR m_dwData;                         // The 32-bit value associated with the item
	BOOL m_bHidden;                             // TRUE if item is hidden

	int m_nHeight;                              // Item's height. -1 to use default
	int m_nLinesCount;                          // Count of lines of Multi line item.
	DWORD m_dwEditStyle;                        // Edit style.

	friend class CXTPPropertyGridItems;
	friend class CXTPPropertyGridView;
	friend class CXTPPropertyGridInplaceEdit;
	friend class CXTPPropertyGridInplaceButton;

	DECLARE_DYNAMIC(CXTPPropertyGridItem)


	friend class CXTPPropertyGridItemConstraints;
	friend class CXTPPropertyGrid;
};

//===========================================================================
// Summary:
//     CXTPPropertyGridItemCategory is the CXTPPropertyGridItem derived class. Use it to add subcategory to
//     property grid control.
// Note:    Parent item for category must be also the category.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridItemCategory : public CXTPPropertyGridItem
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridItemCategory object
	// Parameters:  strCaption    - Caption of the item.
	//              nID           - Identifier of the item.
	// Remarks:
	//     Class CXTPPropertyGridItemCategory has no default constructor.
	//
	//          When using the second constructor, the Identifier (nID) of the
	//          second constructor can be linked with a STRINGTABLE resource
	//          with the same id in such form "Caption\\nDescription".
	//-----------------------------------------------------------------------
	CXTPPropertyGridItemCategory(LPCTSTR strCaption);
	CXTPPropertyGridItemCategory(UINT nID); // <COMBINE CXTPPropertyGridItemCategory::CXTPPropertyGridItemCategory@LPCTSTR>
};

//////////////////////////////////////////////////////////////////////
AFX_INLINE long CXTPPropertyGridItems::GetCount() const {
	return (long)m_arrItems.GetSize();
}
AFX_INLINE BOOL CXTPPropertyGridItems::IsEmpty() const {
	return GetCount() == 0;
}
AFX_INLINE void CXTPPropertyGridItems::AddTail(CXTPPropertyGridItem* pItem) {
	m_arrItems.Add(pItem);
}
AFX_INLINE void CXTPPropertyGridItems::InsertAt(int nIndex, CXTPPropertyGridItem* pItem) {
	m_arrItems.InsertAt(nIndex, pItem);
}
AFX_INLINE BOOL CXTPPropertyGridItem::HasChilds() const {
	return !m_pChilds->IsEmpty();
}
AFX_INLINE void CXTPPropertyGridItem::SetCaption(LPCTSTR strCaption) {
	m_strCaption = strCaption;
}
AFX_INLINE CString CXTPPropertyGridItem::GetCaption() const {
	return m_strCaption;
}
AFX_INLINE CString CXTPPropertyGridItem::GetDescription() const {
	return m_strDescription;
}
AFX_INLINE BOOL CXTPPropertyGridItem::GetReadOnly() const{
	return m_bReadOnly;
}
AFX_INLINE void CXTPPropertyGridItem::SetID(UINT nID) {
	m_nID = nID;
}
AFX_INLINE UINT CXTPPropertyGridItem::GetID() const {
	return m_nID;
}
AFX_INLINE CString CXTPPropertyGridItem::GetValue() const {
	return m_strValue;
}
AFX_INLINE BOOL CXTPPropertyGridItem::IsCategory() const {
	return m_bCategory;
}
AFX_INLINE CXTPPropertyGridView* CXTPPropertyGridItem::GetGrid() const {
	return m_pGrid;
}
AFX_INLINE BOOL CXTPPropertyGridItem::OnDrawItemValue(CDC&, CRect) {
	return FALSE;
}
AFX_INLINE BOOL CXTPPropertyGridItem::IsVisible() const {
	return m_bVisible;
}
AFX_INLINE BOOL CXTPPropertyGridItem::IsExpanded() const {
	return m_bExpanded;
}
AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGridItem::GetParentItem() const {
	return m_pParent;
}
AFX_INLINE CXTPPropertyGridItems* CXTPPropertyGridItem::GetChilds() const {
	return m_pChilds;
}
AFX_INLINE int CXTPPropertyGridItem::GetIndent() const {
	return m_nIndent;
}
AFX_INLINE void CXTPPropertyGridItem::SetConstraintEdit(BOOL bConstraintEdit) {
	m_bConstraintEdit = bConstraintEdit;
}
AFX_INLINE BOOL CXTPPropertyGridItem::GetConstraintEdit() const {
	return m_bConstraintEdit;
}
AFX_INLINE void CXTPPropertyGridItem::SetPasswordMask(TCHAR chMask) {
	m_bPassword = TRUE;
	m_chPrompt = chMask;
}
AFX_INLINE void CXTPPropertyGridItem::OnRButtonDown(UINT, CPoint) {

}
AFX_INLINE void CXTPPropertyGridItem::OnConstraintsChanged() {

}
AFX_INLINE void CXTPPropertyGridItem::SetItemData(DWORD_PTR dwData) {
	m_dwData = dwData;
}
AFX_INLINE DWORD_PTR CXTPPropertyGridItem::GetItemData() const {
	return m_dwData;
}
AFX_INLINE void CXTPPropertyGridItem::SetVisible(BOOL bVisible) {
	m_bVisible = bVisible;
}
AFX_INLINE void CXTPPropertyGridItem::OnIndexChanged() {

}
AFX_INLINE CXTPPropertyGridItemConstraints* CXTPPropertyGridItem::GetConstraints() const {
	return m_pConstraints;
}
AFX_INLINE int CXTPPropertyGridItemConstraints::GetCount() const {
	return (int)m_arrConstraints.GetSize();
}
AFX_INLINE BOOL CXTPPropertyGridItemConstraints::IsEmpty() const {
	return GetCount() == 0;
}
AFX_INLINE void CXTPPropertyGridItemConstraints::SetCurrent(int nIndex){
	m_nCurrent = nIndex;
}
AFX_INLINE int CXTPPropertyGridItemConstraints::GetCurrent() const {
	return m_nCurrent;
}
AFX_INLINE int CXTPPropertyGridItemConstraint::GetIndex() const {
	return m_nIndex;
}

AFX_INLINE int CXTPPropertyGridItem::GetHeight() const {
	return m_nHeight;
}
AFX_INLINE void CXTPPropertyGridItem::SetHeight(int nHeight) {
	m_nHeight = nHeight;
}
AFX_INLINE BOOL CXTPPropertyGridItem::IsMultiLine() const {
	return m_nLinesCount > 1;
}
AFX_INLINE int CXTPPropertyGridItem::GetMultiLinesCount() const {
	return m_nLinesCount;
}
AFX_INLINE void CXTPPropertyGridItem::SetMultiLinesCount(int nLinesCount) {
	m_nLinesCount = nLinesCount;
}
AFX_INLINE void CXTPPropertyGridItem::SetEditStyle(DWORD dwEditStyle) {
	m_dwEditStyle = dwEditStyle;
}
AFX_INLINE BOOL CXTPPropertyGridItem::OnKeyDown (UINT /*nChar*/) {
	return FALSE;
}
AFX_INLINE void CXTPPropertyGridItem::SetDropDownItemCount(int nDropDownItemCount) {
	m_nDropDownItemCount = nDropDownItemCount;
}
AFX_INLINE int CXTPPropertyGridItem::GetDropDownItemCount() const {
	return m_nDropDownItemCount;
}



#endif // #if !defined(__XTPPROPERTYGRIDITEM_H__)
