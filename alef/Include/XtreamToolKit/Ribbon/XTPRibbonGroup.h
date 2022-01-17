// XTPRibbonGroup.h: interface for the CXTPRibbonGroup class.
//
// This file is a part of the XTREME RIBBON MFC class library.
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
#if !defined(__XTPRIBBONGROUP_H__)
#define __XTPRIBBONGROUP_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPRibbonGroups;
class CXTPRibbonBar;
class CXTPRibbonTab;

class CXTPControl;
class CXTPControlPopup;


#include "CommandBars/XTPControl.h"
#include "CommandBars/XTPCommandBarsDefines.h"


//===========================================================================
// Summary:
//     Represents a single group that is added to a CXTPRibbonTab.
// Remarks:
//     Each CXTPRibbonTab can contain one or more CXTPRibbonGroup objects
//     that are used to hold CXTPControl objects.  The Caption of
//     a CXTPRibbonGroup is a title bar that visually "groups" the CXTPControl
//     objects that have been added to the RibbonGroup Items Collection (CXTPRibbonGroup::GetAt).
//
//     Groups are added to the CXTPRibbonTab Groups Collection using the
//     CXTPRibbonGroups::AddGroup and CXTPRibbonGroups::InsertAt methods.  Items are added to a group
//     using the CXTPRibbonGroup::Add method.
// See Also: CXTPRibbonGroup::GetAt
//===========================================================================
class _XTP_EXT_CLASS CXTPRibbonGroup : public CXTPCmdTarget
{
	DECLARE_DYNCREATE(CXTPRibbonGroup)

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPRibbonGroup object
	//-----------------------------------------------------------------------
	CXTPRibbonGroup();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPRibbonGroup object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPRibbonGroup();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the text to display in the group title bar.
	// Parameters:
	//     lpszCaption - Text to display in the group title bar.
	// Remarks:
	//     The Caption is the text string that is displayed in the group's
	//     title bar.  The Caption of the group is initially set when it is
	//     added or inserted.
	// See Also: GetCaption, CXTPRibbonGroups::AddGroup, CXTPRibbonGroups::InsertAt
	//-----------------------------------------------------------------------
	void SetCaption(LPCTSTR lpszCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the text displayed in the group title bar.
	// Remarks:
	//     The Caption is the text string that is displayed in the group's
	//     title bar.  The Caption of the group is initially set when it is
	//     added or inserted.
	// Returns:
	//     Text displayed in the group title bar.
	// See Also: SetCaption, CXTPRibbonGroups::AddGroup, CXTPRibbonGroups::InsertAt
	//-----------------------------------------------------------------------
	CString GetCaption() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the bounding rectangle for the group.
	// Parameters:
	//     rc - A CRect containing the bounding rectangle for the group.
	// Remarks:
	//     The rectangle is the size of the entire group, including the
	//     group caption and group popup if present.
	// See Also: GetRect
	//-----------------------------------------------------------------------
	virtual void SetRect(CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the bounding rectangle for the group.
	// Remarks:
	//     The rectangle is the size of the entire group, including the
	//     group caption and group popup if present.
	// Returns: A CRect containing the bounding rectangle for the group.
	// See Also: SetRect
	//-----------------------------------------------------------------------
	CRect GetRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     The CXTPRibbonBar object the CXTPRibbonGroup belongs to.
	// Returns:
	//     A pointer to the CXTPRibbonBar object the CXTPRibbonGroup belongs to.
	//     This is the RibbonBar that the group is displayed in.
	// See Also: CXTPRibbonBar
	//-----------------------------------------------------------------------
	CXTPRibbonBar* GetRibbonBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     The CXTPRibbonTab object the CXTPRibbonGroup belongs to.
	// Returns:
	//     A pointer to the CXTPRibbonTab object the CXTPRibbonGroup belongs to.
	//     This is the tab that the group is displayed in.
	// See Also: CXTPRibbonTab
	//-----------------------------------------------------------------------
	CXTPRibbonTab* GetParentTab() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets whether the group is currently highlighted.
	// Remarks:
	//     A group becomes highlighted when the mouse cursor is positioned
	//     over the group or any items in the group.
	// Returns:
	//     TRUE if the group is currently highlighted, FALSE otherwise.
	// See Also: CXTPRibbonBar::GetHighlightedGroup
	//-----------------------------------------------------------------------
	BOOL IsHighlighted() const;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Total number of CXTPControl objects that have been
	//     added to the CXTPRibbonGroup.
	// Returns:
	//     Integer containing the total number of CXTPControl objects that have been
	//     added to the CXTPRibbonGroup.
	// Remarks:
	//     A CXTPRibbonGroup is used to hold one ore more CXTPControl
	//     objects.  The total number of items can be found using GetCount.
	// See Also: CXTPRibbonGroup
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a CXTPControl to the ribbon group.
	// Parameters:
	//     controlType   - Type of control to add to the ribbon group.
	//     nId           - Identifier of the CXTPControl to be added.
	//     lpszParameter - The caption of the CXTPControl to be added.
	//     nBefore       - The index of an existing control that the new control
	//                     should will be inserted before.
	//     bTemporary    - A boolean value indicating whether the control is
	//                     temporary.  Temporary controls will not be saved
	//                     to the system registry when the application is closed,
	//                     and they will not be restored when the application
	//                     is opened.
	//     pControl      - Pointer to a CXTPControl to add to the ribbon group.
	// Remarks:
	//     The Add method allows CXTPControl objects to be added to the ribbon group.
	// Returns:
	//     Pointer to the CXTPControl object that was added to the ribbon group.
	// See Also: CXTPControl, GetAt, GetCount, RemoveAt, RemoveAll
	//-----------------------------------------------------------------------
	CXTPControl* Add(XTPControlType controlType, int nId, LPCTSTR lpszParameter = NULL, int nBefore = -1, BOOL bTemporary = FALSE);
	CXTPControl* Add(CXTPControl* pControl, int nId, LPCTSTR lpszParameter = NULL, int nBefore = -1, BOOL bTemporary = FALSE); // <combine CXTPRibbonGroup::Add@XTPControlType@int@LPCTSTR@int@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a CXTPControl to the ribbon group.
	// Parameters:
	//     pControl - Control to add
	//     nBefore  - The index of an existing control that the new control
	//                should will be inserted before.
	// Returns:
	//     Pointer to the CXTPControl object that was added to the ribbon group.
	// See Also: Add
	//-----------------------------------------------------------------------
	CXTPControl* InsertAt(CXTPControl* pControl, int nBefore = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets a CXTPControl object that has been added to the CXTPRibbonGroup
	//     at the specified index.
	// Parameters:
	//     nIndex - Index of the items to retrieve within the group's
	//              collection of controls.
	// Remarks:
	//     GetAt retrieves the CXTPControl from the m_arrControls items collection
	//     that contains all of the CXTPControl objects that have been added
	//     to the CXTPRibbonGroup.
	// Returns:
	//     The CXTPControl at the specified index.
	// See Also: m_arrControls, Add, RemoveAt, RemoveAll
	//-----------------------------------------------------------------------
	CXTPControl* GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes and item at the specified position in the group's
	//     collection of items.
	// Parameters:
	//     nIndex - Index of the item to remove from the group's
	//              collection of items.
	// Remarks:
	//     Removes an item by it's index from the group's collection of items.
	// See Also: m_arrControls, Add, GetAt, RemoveAll
	//-----------------------------------------------------------------------
	void RemoveAt(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes and item at the from collection of items.
	// Parameters:
	//     pControl - The item to remove from the group's
	//              collection of items.
	// See Also: RemoveAt, Add, GetAt, RemoveAll
	//-----------------------------------------------------------------------
	void Remove(CXTPControl* pControl);

	//-------------------------------------------------------------------------
	// Summary:
	//     Removes all of the items from the group.
	// See Also: m_arrControls, Add, GetAt, RemoveAt
	//-------------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines id the CXTPRibbonGroup is currently displayed as a
	//     group button.
	// Remarks:
	//     A single group button will be displayed if there is not enough
	//     room to display the items in the group.  A group popup is
	//     displayed when the button is clicked.  The popup contains all
	//     the items in the group.  When a group button is clicked, a
	//     CXTPControlPopup is displayed that contains the items of the group.
	//
	//     The m_nIconId specifies the Id of the image to use for the group
	//     button that is displayed when there is not enough room to display
	//     the items in the group.  No image will be displayed for the group
	//     button if m_nIconId is not set with SetIconId.
	//
	// Returns:
	//     TRUE if the group is displayed as a single group button, FALSE
	//     if the group is displayed normally.
	// See Also: SetIconId, GetIconId, GetControlGroupPopup
	//-----------------------------------------------------------------------
	BOOL IsReduced() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the group icon's identifier.  This is the Id of
	//     the image to use for the group button that is displayed when there
	//     is not enough room to display the items in the group.
	// Parameters:
	//     nId - Icon's identifier to be set.
	// Remarks:
	//      The m_nIconId specifies the Id of the image to use for the group button
	//      that is displayed when there is not enough room to display the items
	//      in the group.  When a group button is clicked, a CXTPControlPopup
	//      is displayed that contains the items of the group.  No image will
	//      be displayed for the group button if IconId is not set.
	// See Also: IsReduced, GetIconId, GetControlGroupPopup
	//-----------------------------------------------------------------------
	void SetIconId(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icon's identifier.  This is the Id of
	//     the image to use for the group button that is displayed when there
	//     is not enough room to display the items in the group.
	// Remarks:
	//      The m_nIconId specifies the Id of the image to use for the group button
	//      that is displayed when there is not enough room to display the items
	//      in the group.  When a group button is clicked, a CXTPControlPopup
	//      is displayed that contains the items of the group.  No image will
	//      be displayed for the group button if IconId is not set.
	// Returns:
	//     An icon's identifier of the group.
	// See Also: IsReduced, SetIconId, GetControlGroupPopup
	//-----------------------------------------------------------------------
	int GetIconId() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Specifies whether to display an option button in the group caption.
	// Parameters:
	//     bShowOptionButton - TRUE to show an option button in the
	//                         group's caption, FALSE to hide the option
	//                         button if one was visible.
	// Remarks:
	//     The option button is typically used to launch a dialog associated
	//     with the group.  The Group Id(GetID) is used to identify which
	//     group's option button was clicked.
	// See Also: m_bShowOptionButton, GetID, CXTPRibbonPaintManager::DrawControlGroupOption
	//-----------------------------------------------------------------------
	void ShowOptionButton(BOOL bShowOptionButton = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if option button is visible
	// See Also: ShowOptionButton
	//-----------------------------------------------------------------------
	BOOL IsOptionButtonVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if group currently visible
	// See Also: SetVisible
	//-----------------------------------------------------------------------
	BOOL IsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to show/hide the group
	// Parameters:
	//     bVisible - TRUE to show group; FALSE to hide
	// See Also: IsVisible
	//-----------------------------------------------------------------------
	void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns own index in parent group collection
	//-----------------------------------------------------------------------
	int GetIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to copy parameter from one group to another
	// Parameters:
	//     pGroup - Group which parameter will be copied
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPRibbonGroup* pGroup);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the group's Id.
	// Remarks:
	//     The Id is used to identify the group.
	// Returns:
	//     Integer containing the group's identifier.
	// See Also: GetControlGroupPopup, GetControlGroupOption
	//-----------------------------------------------------------------------
	int GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set Identifier for group
	// Parameters:
	//     nId - The Id is used to identify the group.
	// See Also: GetID
	//-----------------------------------------------------------------------
	void SetID(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if control grouping is used.
	// See Also: SetControlsGrouping
	//-----------------------------------------------------------------------
	BOOL IsControlsGrouping() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to group buttons in the group.
	// Parameters:
	//     bControlsGrouping - TRUE to use grouping for buttons
	// See Also: IsControlsGrouping
	//-----------------------------------------------------------------------
	void SetControlsGrouping(BOOL bControlsGrouping = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to automatically center controls in group
	// Parameters:
	//     bControlsCentering - TRUE to center controls in group
	//-----------------------------------------------------------------------
	void SetControlsCentering(BOOL bControlsCentering = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the group and all of the controls in the group.
	// Parameters:
	//     pDC - Pointer to the device context in which to draw the group.
	//     rcClipBox - The rectangular area of the window that is invalid.
	//-----------------------------------------------------------------------
	virtual void Draw(CDC* pDC, CRect rcClipBox);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     CXTPControl that represents the group's OptionButton (used to
	//     set tool tip and status bar text).
	// Remarks:
	//     GetControlGroupOption is only used to set the TooltipText and
	//     DescriptionText for the OptionButton.  No other properties\methods
	//     of the CXTPControl are used.
	// Returns:
	//     Pointer to the CXTPControl that represents the group's OptionButton.
	// See Also: CXTPRibbonPaintManager::DrawControlGroupOption
	//-----------------------------------------------------------------------
	CXTPControl* GetControlGroupOption() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets a pointer to the CXTPControlPopup that is used to display
	//     controls that are not currently visible in the group.
	// Remarks:
	//     A single group button will be displayed if there is not enough
	//     room to display the items in the group.  A group popup is displayed
	//     when the button is clicked.  The popup contains all the items in
	//     the group.  When a group button is clicked, a CXTPControlPopup
	//     is displayed that contains the items of the group.
	// Returns:
	//     CXTPControlPopup that is used to display
	//     controls that are not currently visible in the group.
	// See Also: IsReduced, CXTPRibbonPaintManager::DrawControlGroupPopup
	//-----------------------------------------------------------------------
	CXTPControlPopup* GetControlGroupPopup() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to load the toolbar specified by nIDResource.
	// Parameters:
	//     nIDResource - Resource ID of the toolbar to be loaded.
	//     bLoadIcons - TRUE to load icons from application resources.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL LoadToolBar(UINT nIDResource, BOOL bLoadIcons = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the controls from the button array.
	// Parameters:
	//     pButtons - Pointer to an array of command Ids. It cannot be NULL.
	//     nCount   - Number of elements in the array pointed to by pButtons.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	// See Also: LoadToolBar
	//-----------------------------------------------------------------------
	BOOL SetButtons(UINT* pButtons, int nCount);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the width and height of the group.
	// Parameters:
	//     pDC - Pointer to the device context the group is drawn on.
	// Remarks:
	//     The total width and height is determined based
	//     on how many controls are added to the group.  Regardless of how
	//     few controls are added, the minimum width of a group is 47 pixels
	//     and the minimum height is 85 pixels.
	// Returns:
	//     A CSize object containing the width and height of the group.
	// See Also: GetCount
	//-----------------------------------------------------------------------
	//virtual CSize GetSize(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     Called automatically to reposition all controls in the group.
	//     For example, when the position/size of Ribbon Bar changes.
	// Parameters:
	//     pDC - Pointer to the device context in which to draw the group.
	//-----------------------------------------------------------------------
	virtual void RepositionControls(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the group was added
	// See Also: OnGroupRemoved
	//-----------------------------------------------------------------------
	virtual void OnGroupAdded();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when the group was removed
	// See Also: OnGroupAdded
	//-----------------------------------------------------------------------
	virtual void OnGroupRemoved();

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates minimum width for the ribbon group
	// Parameters:
	//     pDC - Pointer to device context used to draw group
	// Returns:
	//     Minimum width of ribbon group
	// See Also: Draw
	//-----------------------------------------------------------------------
	virtual int GetMinimumWidth(CDC* pDC);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before calculate size of ribbon group
	// Parameters:
	//     pDC - Pointer to device context used to draw group
	// See Also: OnAfterCalcSize
	//-----------------------------------------------------------------------
	virtual void OnBeforeCalcSize(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called after size of ribbon group was calculated
	// See Also: OnBeforeCalcSize
	//-----------------------------------------------------------------------
	virtual void OnAfterCalcSize();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to find width of the ribbon group
	// Parameters:
	//     pDC - Pointer to device context used to draw group
	// Returns:
	//     Width of ribbon group
	// See Also: OnBeforeCalcSize, OnAfterCalcSize, GetMinimumWidth
	//-----------------------------------------------------------------------
	virtual int OnCalcDynamicSize(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	///    This method is called to decrease size of ribbon group
	// Parameters:
	//     nLevel - Reduce level
	// Returns: TRUE if group was reduced
	//-----------------------------------------------------------------------
	virtual BOOL OnReduceSize(int nLevel);

//{{AFX_CODEJOCK_PRIVATE
protected:
	void OnAdjustBorders(int nWidth, CRect rcBorders);
	void CenterColumn(int nFirstItem, int nLastItem, int nGroupHeight);

private:
	int _CalcSpecialDynamicSize(CDC* pDC);
	int _WrapSpecialDynamicSize(int nWidth, BOOL bIncludeTail);
	int _GetSizeSpecialDynamicSize();
	BOOL _FindBestWrapSpecialDynamicSize();
//}}AFX_CODEJOCK_PRIVATE

protected:

protected:
	BOOL m_bControlsCentering;                  // TRUE to center controls inside groups
	BOOL m_bControlsGrouping;                   // TRUE to group controls
	CString m_strCaption;                       // Caption of the group
	int m_nIndex;                               // Index of the group
	CXTPRibbonGroups* m_pGroups;                // Parent groups collection
	CRect m_rcGroup;                            // Bounding rectangle of the group
	CArray<CXTPControl*, CXTPControl*> m_arrControls;   // Collection of group's controls
	CXTPCommandBar* m_pParent;                  // Parent ribbon bar
	CXTPRibbonBar* m_pRibbonBar;                  // Parent ribbon bar
	int m_nId;                                  // Identifier of the group
	int m_nIconId;                              // Icon index of the group

	CXTPControl* m_pControlGroupOption;         // Group option control
	CXTPControlPopup* m_pControlGroupPopup;     // Group popup control
	BOOL m_bReduced;                            // TRUE if group was reduced
	BOOL m_bShowOptionButton;                   // TRUE to show option button
	BOOL m_bVisible;                            // TRUE if group is visible

	int m_nRowCount;                            // Total rows number of items

private:
	struct LAYOUTINFO;
	struct CONTROLINFO;
	LAYOUTINFO* m_pLayoutInfo;

private:
	friend class CXTPRibbonGroups;
	friend class CXTPRibbonBar;
	friend class CControlGroupPopup;
	friend class CXTPRibbonGroupPopupToolBar;
	friend class CXTPRibbonGroupControlPopup;
};

AFX_INLINE void CXTPRibbonGroup::SetCaption(LPCTSTR lpszCaption) {
	m_strCaption = lpszCaption;
}

AFX_INLINE CString CXTPRibbonGroup::GetCaption() const {
	return m_strCaption;
}

AFX_INLINE CRect CXTPRibbonGroup::GetRect() const {
	return m_rcGroup;
}
AFX_INLINE CXTPRibbonBar* CXTPRibbonGroup::GetRibbonBar() const {
	return (CXTPRibbonBar*)m_pRibbonBar;
}
AFX_INLINE int CXTPRibbonGroup::GetCount() const {
	return (int)m_arrControls.GetSize();
}
AFX_INLINE CXTPControl* CXTPRibbonGroup::GetAt(int nIndex) const {
	return nIndex >= 0 && nIndex < GetCount() ? m_arrControls.GetAt(nIndex) : NULL;
}
AFX_INLINE int CXTPRibbonGroup::GetID() const {
	return m_nId;
}
AFX_INLINE CXTPControl* CXTPRibbonGroup::GetControlGroupOption() const {
	return m_pControlGroupOption;
}
AFX_INLINE CXTPControlPopup* CXTPRibbonGroup::GetControlGroupPopup() const {
	return m_pControlGroupPopup;
}
AFX_INLINE BOOL CXTPRibbonGroup::IsReduced() const {
	return m_bReduced;
}
AFX_INLINE void CXTPRibbonGroup::SetIconId(int nId) {
	m_nIconId = nId;
}
AFX_INLINE int CXTPRibbonGroup::GetIconId() const {
	return m_nIconId <= 0 ? m_nId : m_nIconId;
}
AFX_INLINE void CXTPRibbonGroup::ShowOptionButton(BOOL bShowOptionButton) {
	m_bShowOptionButton = bShowOptionButton;
}
AFX_INLINE int CXTPRibbonGroup::GetIndex() const {
	return m_nIndex;
}
AFX_INLINE BOOL CXTPRibbonGroup::IsControlsGrouping() const {
	return m_bControlsGrouping;
}
AFX_INLINE void CXTPRibbonGroup::SetControlsGrouping(BOOL bControlsGrouping) {
	m_bControlsGrouping = bControlsGrouping;
}
AFX_INLINE void CXTPRibbonGroup::SetControlsCentering(BOOL bControlsCentering) {
	m_bControlsCentering = bControlsCentering;
}

#endif // !defined(__XTPRIBBONGROUP_H__)
