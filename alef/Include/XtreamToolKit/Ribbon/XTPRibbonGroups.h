// XTPRibbonGroups.h: interface for the CXTPRibbonGroups class.
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
#if !defined(__XTPRIBBONGROUPS_H__)
#define __XTPRIBBONGROUPS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPRibbonGroup;
class CXTPRibbonTab;


//===========================================================================
// Summary:
//     Represents a collection of CXTPRibbonGroup objects.
// Remarks:
//     A CXTPRibbonTab uses the CXTPRibbonGroups object for its Groups Collection
//     to store all of the RibbonGroup objects that have been added to the
//     RibbonTab.  A CXTPRibbonGroup is used to hold items such as buttons,
//     popups, edit controls, etc.
//
//     Groups are added to the Groups Collection using the Add
//     and InsertAt methods.
// See Also: Add, InsertAt
//===========================================================================
class _XTP_EXT_CLASS CXTPRibbonGroups : public CXTPCmdTarget
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPRibbonGroups object
	//-----------------------------------------------------------------------
	CXTPRibbonGroups();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPRibbonGroups object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPRibbonGroups();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Copies all groups
	// Parameters:
	//     pGroups - Groups to copy from
	//-----------------------------------------------------------------------
	void Copy(CXTPRibbonGroups* pGroups);

	//-------------------------------------------------------------------------
	// Summary:
	//     Removes all groups.
	// See Also: InsertAt, Remove, GetCount, GetAt, Add
	//-------------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes the group at the specified index.
	// Parameters:
	//     nIndex - Index of the group to be removed.
	// See Also: InsertAt, Add, GetCount, GetAt, RemoveAll
	//-----------------------------------------------------------------------
	void Remove(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a CXTPRibbonGroup to a CXTPRibbonTab at a specific location.
	// Parameters:
	//     nIndex      - Position within the tab's Groups Collection to
	//                   insert the new CXTPRibbonGroup.
	//     lpszCaption - Text to display in the group title bar.
	//                   This in the group's caption.
	//     nId         - Identifier of new group
	//     pGroup      - New group to insert
	// Remarks:
	//     InsertAt adds a group at a specific location within the tab's
	//     Groups Collection.  Groups will be displayed in the order they
	//     are added.  This means the group with an Index of zero (0) will
	//     be displayed first to the far left, then to the immediate right
	//     will be the group with an Index of one (1), etc...
	// Returns:
	//     Pointer to the CXTPRibbonGroup object added to the CXTPRibbonTab
	//     Groups Collection.
	// See Also: Add, Remove, GetCount, GetAt, RemoveAll
	//-----------------------------------------------------------------------
	CXTPRibbonGroup* InsertAt(int nIndex, LPCTSTR lpszCaption, int nId = -1);
	CXTPRibbonGroup* InsertAt(int nIndex, CXTPRibbonGroup* pGroup); // <combine CXTPRibbonGroups::InsertAt@int@LPCTSTR@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a CXTPRibbonGroup to a CXTPRibbonTab.
	// Parameters:
	//     lpszCaption - Text to display in the group title bar.
	//                   This is the group's caption.
	//     nId         - Identifier of new group
	// Remarks:
	//     Tabs in the Ribbon Bar contain one or more CXTPRibbonGroup objects.
	//     Buttons, Popups, etc can then be added to the groups.
	// Returns:
	//     Pointer to the CXTPRibbonGroup object added to the CXTPRibbonTab Groups Collection.
	// See Also: InsertAt, Remove, GetCount, GetAt, RemoveAll
	//-----------------------------------------------------------------------
	CXTPRibbonGroup* Add(LPCTSTR lpszCaption, int nId = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//     Total number of CXTPRibbonGroup objects that have been added to a CXTPRibbonTab.
	// Returns:
	//     Integer containing the total number of CXTPRibbonGroup objects that
	//     have been added to a CXTPRibbonTab.
	// See Also: InsertAt, Remove, Add, GetAt, RemoveAll
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets a CXTPRibbonGroup object that has been added to the CXTPRibbonTab
	//     at the specified index.
	// Parameters:
	//     nIndex - Index of the group to retrieve within the
	//              collection of groups.
	// Remarks:
	//     GetAt retrieves the CXTPRibbonGroup from the m_arrGroups groups collection
	//     that contains all of the CXTPRibbonGroup objects that have been added
	//     to the CXTPRibbonTab.
	// Returns:
	//     The CXTPRibbonGroup at the specified index.
	// See Also: InsertAt, Remove, GetCount, Add, RemoveAll
	//-----------------------------------------------------------------------
	CXTPRibbonGroup* GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines which ribbon group,
	//     if any, is at a specified position.
	// Parameters:
	//     point - A CPoint that contains the coordinates of the point to test.
	// Returns:
	//     The CXTPRibbonGroup that is at the specified point, if no group is
	//     at the point, then NULL is returned.
	//-----------------------------------------------------------------------
	CXTPRibbonGroup* HitTest(CPoint point) const;

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
	//     This method is called internally to reposition groups and controls.
	// Parameters:
	//     pDC      - Pointer to a valid device context
	//     nLength  - Length of the parent bar.
	//     dwMode   - Mode of the parent bar.
	//     rcBorder - Border to be set.
	// See Also: CXTPRibbonBar::RepositionGroups
	//-----------------------------------------------------------------------
	void CalcDynamicSize(CDC* pDC, int nLength, DWORD dwMode, const CRect& rcBorder);

public:

	//-----------------------------------------------------------------------
	// Summary: Call this method to get group scroll position
	// Returns: Position of scroll offset
	//-----------------------------------------------------------------------
	int GetScrollPos() const;

protected:
//{{AFX_CODEJOCK_PRIVATE
	int _CalcSmartLayoutToolBar(int* pWidth);
	void _ReduceSmartLayoutToolBar(CDC* pDC, int* pWidth, int nWidth);
//}}AFX_CODEJOCK_PRIVATE

private:
	void RefreshIndexes();

protected:

protected:
	CArray<CXTPRibbonGroup*, CXTPRibbonGroup*> m_arrGroups;     // Collection of groups.
	CXTPRibbonTab* m_pParentTab;    // A pointer to the CXTPRibbonTab object the CXTPRibbonGroup belongs to.
	                                // This is the tab that the group is displayed in.
	int m_nGroupsScrollPos;         // Scroll position
	friend class CXTPRibbonTab;
	friend class CXTPRibbonBar;
};

AFX_INLINE int CXTPRibbonGroups::GetCount() const {
	return (int)m_arrGroups.GetSize();
}
AFX_INLINE CXTPRibbonGroup* CXTPRibbonGroups::GetAt(int nIndex) const {
	ASSERT(nIndex < m_arrGroups.GetSize());
	return (nIndex >= 0 && nIndex < m_arrGroups.GetSize()) ? m_arrGroups.GetAt(nIndex) : NULL;
}
AFX_INLINE CXTPRibbonTab* CXTPRibbonGroups::GetParentTab() const {
	return  m_pParentTab;
}
AFX_INLINE int CXTPRibbonGroups::GetScrollPos() const {
	return m_nGroupsScrollPos;
}

#endif // !defined(__XTPRIBBONGROUPS_H__)
