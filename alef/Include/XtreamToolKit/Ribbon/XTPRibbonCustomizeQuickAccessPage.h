// XTPRibbonCustomizeQuickAccessPage.h: interface for the CXTPRibbonCustomizeQuickAccessPage class.
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
#if !defined(__XTPRIBBONCUSTOMIZEQUICKACCESSPAGE_H__)
#define __XTPRIBBONCUSTOMIZEQUICKACCESSPAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPCustomizeSheet;
class CXTPCommandBars;
class CXTPRibbonBar;

#include "CommandBars/XTPCustomizeCommandsPage.h"

//===========================================================================
// Summary:
//     CXTPRibbonCustomizeQuickAccessPage is a CPropertyPage derived class.
//     It represents the Quick Access page of the Customize dialog.
//===========================================================================
class _XTP_EXT_CLASS CXTPRibbonCustomizeQuickAccessPage : public CPropertyPage
{
// Construction
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPRibbonCustomizeQuickAccessPage object
	// Parameters:
	//     pSheet - Points to a CXTPCustomizeSheet object
	//-----------------------------------------------------------------------
	CXTPRibbonCustomizeQuickAccessPage(CXTPCustomizeSheet* pSheet);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCustomizeCommandsPage object, handles cleanup
	//     and deallocation.
	//-----------------------------------------------------------------------
	~CXTPRibbonCustomizeQuickAccessPage();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves Ribbon bar to customize
	// Returns:
	//     Pointer to commandbars' ribbon bar
	//-----------------------------------------------------------------------
	CXTPRibbonBar* GetRibbonBar();


public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Adds categories branch from menu resource.
	// Parameters:
	//     nIDResource   - Menu resource from where categories will be built.
	//     bListSubItems - TRUE to add sub menus to categories.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL AddCategories(UINT nIDResource,  BOOL bListSubItems = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds categories branch from Controls.
	// Parameters:
	//     pControls - Points to a CXTPControls object
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL AddCategories(CXTPControls* pControls);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a new category from a CMenu object.
	// Parameters:
	//     strCategory   - Category to be added.
	//     pMenu         - Points to a CMenu object
	//     bListSubItems - TRUE to add sub menus to the category.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL AddCategory(LPCTSTR strCategory, CMenu* pMenu, BOOL bListSubItems = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method adds a new empty category in the given index.
	// Parameters:
	//     strCategory - Category to be added.
	//     nIndex      - Index to insert.
	// Returns:
	//     A pointer to a CXTPControls object
	//-----------------------------------------------------------------------
	CXTPControls* InsertCategory(LPCTSTR strCategory, int nIndex = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the control list of the given category.
	// Parameters:
	//     strCategory - Category to retrieve.
	// Returns:
	//     A pointer to a CXTPControls object.
	//-----------------------------------------------------------------------
	CXTPControls* GetControls(LPCTSTR strCategory);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Refills list of quick access controls
	//-----------------------------------------------------------------------
	void RefreshQuickAccessList();

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPRibbonCustomizeQuickAccessPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPRibbonCustomizeQuickAccessPage)
	afx_msg void OnCheckShowQuickAccessBelow();
	virtual BOOL OnInitDialog();
	afx_msg void OnCategoriesSelectionChanged();
	afx_msg void OnDblclkListQuickAccess();
	afx_msg void OnDblclkListCommands();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonReset();
	afx_msg void OnCommandsSelectionChanged();
	afx_msg void OnQuickAccessSelectionChanged();
	//}}AFX_MSG

private:
	XTP_COMMANDBARS_CATEGORYINFO* FindCategory(LPCTSTR strCategory) const;
	XTP_COMMANDBARS_CATEGORYINFO* GetCategoryInfo(int nIndex);

public:
	CComboBox   m_lstCategories;            // Categories list
	CXTPCustomizeCommandsListBox    m_lstQuickAccess;   // Quick access list
	CXTPCustomizeCommandsListBox    m_lstCommands;      // Commands list
	BOOL    m_bShowQuickAccessBelow;                    // TRUE to show quick access below ribbon

protected:
	CXTPCustomizeSheet* m_pSheet;           // Parent Sheet window
	CXTPCommandBars* m_pCommandBars;        // Parent CommandBars object
	CXTPCommandBarsCategoryArray m_arrCategories;   // Array of categories.

};

#endif // !defined(__XTPRIBBONCUSTOMIZEQUICKACCESSPAGE_H__)
