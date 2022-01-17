// XTPCustomizeMenusPage.h : interface for the CXTPCustomizeMenusPage class.
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
#if !defined(__XTPCUSTOMIZEMENUSPAGE_H__)
#define __XTPCUSTOMIZEMENUSPAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCustomizeOptionsPage.h"

class CXTPToolBar;
class CXTPCustomizeSheet;

//===========================================================================
// Summary:
//     CXTPCustomizeMenusPage is a CPropertyPage derived class.
//     It represents the Menus page of the Customize dialog.
//===========================================================================
class _XTP_EXT_CLASS CXTPCustomizeMenusPage : public CPropertyPage
{
// Construction
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCustomizeMenusPage object
	// Parameters:
	//     pSheet - Points to a CXTPCustomizeSheet object that this page
	//     belongs to.
	//-----------------------------------------------------------------------
	CXTPCustomizeMenusPage(CXTPCustomizeSheet* pSheet);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCustomizeMenusPage object, handles cleanup
	//     and deallocation.
	//-----------------------------------------------------------------------
	~CXTPCustomizeMenusPage();

protected:

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to save user changes
	//-------------------------------------------------------------------------
	void CommitContextMenu();

// Implementation
protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPCustomizeMenusPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPCustomizeMenusPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectionChangedAppmenus();
	afx_msg void OnSelectionChangedContextmenus();
	afx_msg void OnDestroy();
	afx_msg void OnButtonResetContextMenu();
	afx_msg void OnButtonResetAppMenu();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

// Dialog Data
public:
	//{{AFX_DATA(CXTPCustomizeMenusPage)
	enum { IDD = XTP_IDD_PAGE_MENUS };
	CStatic m_wndIcon;
	CComboBox   m_cmbContextMenus;
	CComboBox   m_cmbAppMenus;
	CXTPGroupLine   m_wndApplicationMenus;
	CXTPGroupLine   m_wndContextMenus;
	//}}AFX_DATA

protected:
	CXTPToolBar* m_pContextMenu;    // Active context menu
	CXTPCustomizeSheet* m_pSheet;   // Parent sheet
	CPoint m_ptContextMenu;         // Last position of context menu
	CString m_strDefaultMenu;       // Default menu caption
	CString m_strDefaultMenuDesc;   // Default menu description
};

#endif // !defined(__XTPCUSTOMIZEMENUSPAGE_H__)
