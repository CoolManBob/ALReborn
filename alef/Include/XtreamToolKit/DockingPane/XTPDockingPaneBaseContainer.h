// XTPDockingPaneBaseContainer.h : interface for the CXTPDockingPaneBaseContainer class.
//
// This file is a part of the XTREME DOCKINGPANE MFC class library.
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
#if !defined(__XTPDOCKINGPANEBASECONTAINER_H__)
#define __XTPDOCKINGPANEBASECONTAINER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPDockingPaneDefines.h"
#include "XTPDockingPaneBase.h"

//===========================================================================
// Summary:
//     CXTPDockingPaneBaseContainer is a CXTPDockingPaneBase derived class. It represents the parent
//     class for a all virtual containers.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneBaseContainer : public CXTPDockingPaneBase
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Protected constructor. You cannot create this class.
	// Parameters:
	//     type    - Docking Pane type. Can be any of the values listed in the Remarks section.
	//     pLayout - Points to a CXTPDockingPaneLayout object.
	// Remarks:
	//     Docking Pane type can be one of the following:
	//     * <b>xtpPaneTypeDockingPane</b> Indicates the pane's style is a docking pane.
	//     * <b>xtpPaneTypeTabbedContainer</b> Indicates the pane's style is a tabbed container for pane.
	//     * <b>xtpPaneTypeSplitterContainer</b> Indicates the pane's style is a splitter container.
	//     * <b>xtpPaneTypeMiniWnd</b> Indicates the pane's style is a floating window container.
	//     * <b>xtpPaneTypeClient</b> Indicates the pane's style is a container for client area.
	//     * <b>xtpPaneTypeAutoHidePanel</b> Indicates the pane's style is an auto-hide panel container.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBaseContainer(XTPDockingPaneType type, CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDockingPaneBase object, handles cleanup and
	//     deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDockingPaneBaseContainer();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if one pane is contained in the
	//     collection of another.
	// Parameters:
	//     pPane - Points to a CXTPDockingPaneBase object
	// Returns:
	//     POSITION of the pane in the collection if successful; otherwise
	//     returns NULL.
	//-----------------------------------------------------------------------
	virtual POSITION ContainPane(CXTPDockingPaneBase* pPane) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to find all panes of a specific XTPDockingPaneType
	//     within the container.
	// Parameters:
	//     type  - Type of docking pane to search for within container.
	//     pList - An array of type CXTPDockingPaneBaseList that will
	//             contain a list of all panes of the specified type
	//             that are in the tabbed container.  This points to a
	//             CXTPDockingPaneBaseList object that will contain
	//             found panes.
	// Remarks:
	//     pList is an external list that you must create and pass in as a
	//     parameter.  See example below:
	// Example:
	//     This will get the count of panes inside container:
	// <code>
	// CXTPDockingPaneBaseList lst;
	//
	// m_pTopContainer->FindPane(xtpPaneTypeTabbedContainer, &lst);
	// int nCount = (int)lst.GetCount();
	// </code>
	//-----------------------------------------------------------------------
	virtual void FindPane(XTPDockingPaneType type, CXTPDockingPaneBaseList* pList) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the position of the first pane of this container
	// Returns:
	//     A POSITION value that can be used for iteration or object pointer retrieval;
	//     it is NULL if the list is empty.
	// See Also: GetNext
	//-----------------------------------------------------------------------
	POSITION GetHeadPosition() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the pane element identified by rPosition, then sets rPosition to the POSITION value of the next
	//     entry in the container. You can use GetNext in a forward iteration loop if you establish the initial position with a call to GetHeadPosition
	// Parameters:
	//     pos - Specifies a reference to a POSITION value returned by a previous GetNext, GetHeadPosition, or other method call
	// Returns:
	//     Pane identified by rPosition
	// See Also: GetHeadPosition
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* GetNext(POSITION& pos) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the first pane in the container
	// Returns:
	//     First pane in the container
	// See Also: GetLastPane, GetHeadPosition
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* GetFirstPane() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the last pane in the container
	// Returns:
	//     Last pane in the container
	// See Also: GetFirstPane, GetHeadPosition
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* GetLastPane() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to return collection of the child panes.
	// Returns:
	//     A reference to the child panes collection.
	//-----------------------------------------------------------------------
	CXTPDockingPaneBaseList& GetPanes();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if there are children panes
	// Returns:
	//     TRUE if no children panes; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsEmpty() const;

protected:
	CXTPDockingPaneBaseList m_lstPanes;         // Child panes collection

};

AFX_INLINE CXTPDockingPaneBaseList& CXTPDockingPaneBaseContainer::GetPanes() {
	return m_lstPanes;
}


#endif // #if !defined(__XTPDOCKINGPANEBASECONTAINER_H__)
