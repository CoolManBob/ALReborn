// XTPRibbonTab.h: interface for the CXTPRibbonTab class.
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
#if !defined(__XTPRIBBONTAB_H__)
#define __XTPRIBBONTAB_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPRibbonGroups;
class CXTPRibbonBar;
class CXTPRibbonGroup;
class CXTPRibbonTab;
class CXTPCommandBar;

#include "TabManager/XTPTabManager.h"


//-----------------------------------------------------------------------
// Summary:
//     Ribbon Tab context color
// Example:
// <code>
// pTab = pRibbonBar->AddTab(ID_TAB_CHARTFORMAT);
// pTab->SetContextTab(xtpRibbonTabContextColorGreen, ID_TAB_CONTEXTCHART);
// </code>
// See Also:
//     CXTPRibbonTab, CXTPRibbonTab::SetContextColor
//-----------------------------------------------------------------------
enum XTPRibbonTabContextColor
{
	xtpRibbonTabContextColorNone,           // Default color
	xtpRibbonTabContextColorBlue,           // Blue context color
	xtpRibbonTabContextColorYellow,         // Yellow context color
	xtpRibbonTabContextColorGreen,          // Green context color
	xtpRibbonTabContextColorRed,            // Red context color
	xtpRibbonTabContextColorPurple,         // Purple context color
	xtpRibbonTabContextColorCyan,           // Cyan context color
	xtpRibbonTabContextColorOrange          // Orange context color
};


//{{AFX_CODEJOCK_PRIVATE

// Context headers internal classes

class _XTP_EXT_CLASS CXTPRibbonTabContextHeader
{
public:
	CXTPRibbonTabContextHeader(CXTPRibbonTab* pFirstTab);

public:
	CXTPRibbonTab* m_pFirstTab;
	CXTPRibbonTab* m_pLastTab;
	CString m_strCpation;
	XTPRibbonTabContextColor m_color;
	CRect m_rcRect;
};


class _XTP_EXT_CLASS CXTPRibbonTabContextHeaders
{
public:
	CXTPRibbonTabContextHeaders();
	~CXTPRibbonTabContextHeaders();
public:
	void RemoveAll();
	int GetCount() const;
	void Add(CXTPRibbonTabContextHeader* pHeader);
	CXTPRibbonTabContextHeader* GetHeader(int nIndex) const;
	CXTPRibbonTabContextHeader* HitTest(CPoint pt) const;

protected:
	CArray<CXTPRibbonTabContextHeader*, CXTPRibbonTabContextHeader*> m_arrHeaders;
};
//}}AFX_CODEJOCK_PRIVATE


//===========================================================================
// Summary:
//     Represents a tab in the Ribbon Bar.
// Remarks:
//     Tabs in the Ribbon Bar are used to display different groups of items.
//     The CXTPRibbonBar.InsertTab method adds a new CXTPRibbonTab object to
//     the tabs collection of the Ribbon Bar.
//
//     Tabs in the Ribbon Bar contain one or more CXTPRibbonGroup objects.
//     Buttons, Popups, etc can then be added to the groups.
//
//     The CXTPRibbonBar::GetTabCount property can be used to determine the total number of
//     tabs that have been added.
// See Also: CXTPRibbonBar.InsertTab, CXTPRibbonBar::GetTabCount, CXTPRibbonBar::GetTab
//===========================================================================
class _XTP_EXT_CLASS CXTPRibbonTab : public CXTPTabManagerItem
{
	DECLARE_DYNCREATE(CXTPRibbonTab)
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPRibbonTab object
	//-----------------------------------------------------------------------
	CXTPRibbonTab();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPRibbonTab object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPRibbonTab();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Collection of CXTPRibbonGroup objects that have been added to
	//     the CXTPRibbonTab.
	// Remarks:
	//     A CXTPRibbonGroup is used to hold buttons, popups, edit controls,
	//     combo box controls, etc.  The group organizes the different
	//     types of controls in a CXTPRibbonTab so it is easy to see what functionality
	//     the controls should perform.  Groups are added to the
	//     Group collection using the AddGroup and InsertGroup methods.
	//     Groups are displayed in the order they are added.  This means
	//     the group with an Index of zero (0) will be displayed first to
	//     the far left, then to the immediate right will be the group
	//     with an Index of one (1), etc...
	//
	// Returns:
	//     Collection of CXTPRibbonGroup objects that have been added to
	//     the CXTPRibbonTab
	// See Also: CXTPRibbonGroups::GetAt, CXTPRibbonGroups::InsertAt, CXTPRibbonGroups::Add, CXTPRibbonGroups.GetCount
	//-----------------------------------------------------------------------
	CXTPRibbonGroups* GetGroups() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     The CXTPRibbonBar object the CXTPRibbonTab belongs to.
	// Returns:
	//     A pointer to the CXTPRibbonBar object the CXTPRibbonTab belongs to.
	//     This is the RibbonBar that the tab is displayed in.
	// See Also: CXTPRibbonBar
	//-----------------------------------------------------------------------
	CXTPRibbonBar* GetRibbonBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the parent command bar.
	// Returns:
	//     The parent command bar object.
	//-----------------------------------------------------------------------
	CXTPCommandBar* GetParent() const;

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to copy the tab.
	// Parameters:
	//     pRibbonTab   - Points to a source CXTPRibbonTab object
	//----------------------------------------------------------------------
	virtual void Copy(CXTPRibbonTab* pRibbonTab);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a CXTPRibbonGroup to a CXTPRibbonTab.
	// Parameters:
	//     lpszCaption - Text to display in the group's title bar.
	//                   This in the group's caption.
	//     nID         - Identifier of group
	// Returns:
	//     Reference to the CXTPRibbonGroup object added to the CXTPRibbonTab's
	//     Groups Collection (CXTPRibbonTab::GetGroups).
	// See Also: CXTPRibbonGroup::SetCaption, CXTPRibbonGroup::SetCaption, CXTPRibbonTab, CXTPRibbonTab::GetGroups
	//-----------------------------------------------------------------------
	CXTPRibbonGroup* AddGroup(LPCTSTR lpszCaption);
	CXTPRibbonGroup* AddGroup(int nID); // <combine CXTPRibbonTab::AddGroup@LPCTSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to find group by its identifier
	// Parameters:
	//     nId - Identifier of group to be found
	// Returns:
	//     Pointer to CXTPRibbonGroup object with specified identifier
	// See Also: FindTab
	//-----------------------------------------------------------------------
	CXTPRibbonGroup* FindGroup(int nId) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get tab color
	// Returns:
	//     XTPRibbonTabContextColor enumerator.
	// See Also: XTPRibbonTabContextColor
	//-----------------------------------------------------------------------
	XTPRibbonTabContextColor GetContextColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set tab color
	// Parameters:
	//     color - XTPRibbonTabContextColor enumerator.
	// See Also: XTPRibbonTabContextColor
	//-----------------------------------------------------------------------
	void SetContextColor(XTPRibbonTabContextColor color);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get context tab header text
	// Returns:
	//     Context header text
	// See Also: SetContextCaption, SetContextColor
	//-----------------------------------------------------------------------
	CString GetContextCaption() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set context tab header text
	// Parameters:
	//     lpszContextCaption - new text to be set
	// See Also: GetContextCaption
	//-----------------------------------------------------------------------
	void SetContextCaption(LPCTSTR lpszContextCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set context tab color and text
	// Parameters:
	//     color - Color to be set
	//     nContextID - Caption string identifier
	//     lpszContextCaption - Caption string
	// See Also: GetContextCaption, GetContextColor
	//-----------------------------------------------------------------------
	void SetContextTab(XTPRibbonTabContextColor color, int nContextID);
	void SetContextTab(XTPRibbonTabContextColor color, LPCTSTR lpszContextCaption = NULL); // <combine CXTPRibbonTab::SetContextTab@XTPRibbonTabContextColor@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the tab's Id.
	// Remarks:
	//     The Id is used to identify the tab.
	// Returns:
	//     Integer containing the tab's identifier.
	// See Also: SetID
	//-----------------------------------------------------------------------
	int GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set Identifier for the tab
	// Parameters:
	//     nId - The Id is used to identify the tab.
	// See Also: GetID
	//-----------------------------------------------------------------------
	void SetID(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieve CXTPRibbonTabContextHeader pointer associated with the tab
	// Parameters:
	//     nId - The Id is used to identify the tab.
	// See Also: GetID
	//-----------------------------------------------------------------------
	CXTPRibbonTabContextHeader* GetContextHeader() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to hide or show the tab.
	// Parameters: bVisible - TRUE to show the tab, FALSE to hide the tab.
	// See Also: IsVisible
	//-----------------------------------------------------------------------
	virtual void SetVisible(BOOL bVisible);


protected:
	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called before removing tab from Ribbon Tabs collection
	//-------------------------------------------------------------------------
	void OnRemoved();


protected:
	CXTPRibbonGroups* m_pGroups;            // Collection of CXTPRibbonGroup objects that have been added to
	                                        // the CXTPRibbonTab.
	CXTPRibbonBar* m_pRibbonBar;            // The CXTPRibbonBar object the CXTPRibbonTab belongs to.
	                                        // This is the RibbonBar that the tab is displayed in.
	CXTPCommandBar* m_pParent;
	int m_nId;                              // Identifier of the tab.
	XTPRibbonTabContextColor m_nContextColor;       // Context color
	CString m_strContextCaption;                    // Context caption
	CXTPRibbonTabContextHeader* m_pContextHeader;   // Context header

	friend class CXTPRibbonBar;
	friend class CXTPRibbonTabContextHeader;
	friend class CXTPRibbonTabPopupToolBar;
};

AFX_INLINE CXTPRibbonGroups* CXTPRibbonTab::GetGroups() const {
	return m_pGroups;
}
AFX_INLINE void CXTPRibbonTab::SetContextTab(XTPRibbonTabContextColor color, LPCTSTR lpszContextCaption) {
	m_nContextColor = color;
	m_strContextCaption = lpszContextCaption;
}
AFX_INLINE void CXTPRibbonTab::SetContextTab(XTPRibbonTabContextColor color, int nID) {
	m_nContextColor = color;
	m_strContextCaption.LoadString(nID);
}
AFX_INLINE int CXTPRibbonTab::GetID() const {
	return m_nId;
}
AFX_INLINE void CXTPRibbonTab::SetID(int nID) {
	m_nId = nID;
}
AFX_INLINE CString CXTPRibbonTab::GetContextCaption() const {
	return m_strContextCaption;
}
AFX_INLINE XTPRibbonTabContextColor CXTPRibbonTab::GetContextColor() const {
	return m_nContextColor;
}
AFX_INLINE void CXTPRibbonTab::SetContextColor(XTPRibbonTabContextColor color) {
	m_nContextColor = color;
}
AFX_INLINE void CXTPRibbonTab::SetContextCaption(LPCTSTR lpszContextCaption) {
	m_strContextCaption = lpszContextCaption;
}
AFX_INLINE CXTPRibbonTabContextHeader* CXTPRibbonTab::GetContextHeader() const {
	return m_pContextHeader;
}
AFX_INLINE CXTPCommandBar* CXTPRibbonTab::GetParent() const {
	return m_pParent;
}

#endif // !defined(__XTPRIBBONTAB_H__)
