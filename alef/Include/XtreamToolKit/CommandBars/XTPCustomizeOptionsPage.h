// XTPCustomizeOptionsPage.h : interface for the CXTPCustomizeOptionsPage class.
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
#if !defined(__XTPCUSTOMIZEOPTIONSPAGE_H__)
#define __XTPCUSTOMIZEOPTIONSPAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPCustomizeSheet;
class CXTPCommandBars;

#include "Common/XTPWinThemeWrapper.h"

//===========================================================================
// Summary:
//     CXTPGroupLine is a CStatic derived class.
//     It used in CXTPCustomizeOptionsPage page to draw group line static control.
//===========================================================================
class _XTP_EXT_CLASS CXTPGroupLine : public CStatic
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPGroupLine object
	//-----------------------------------------------------------------------
	CXTPGroupLine();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPGroupLine)
	void OnPaint();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

private:
	CXTPWinThemeWrapper m_themeButton;
};


//===========================================================================
// Summary:
//     CXTPCustomizeOptionsPage is a CPropertyPage derived class.
//     It represents the Options page of the Customize dialog.
//===========================================================================
class _XTP_EXT_CLASS CXTPCustomizeOptionsPage : public CPropertyPage
{

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCustomizeOptionsPage object
	// Parameters:
	//     pSheet - Points to a CXTPCustomizeSheet object that this page
	//     belongs to.
	//-----------------------------------------------------------------------
	CXTPCustomizeOptionsPage(CXTPCustomizeSheet* pSheet);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCustomizeOptionsPage object, handles cleanup
	//     and deallocation.
	//-----------------------------------------------------------------------
	~CXTPCustomizeOptionsPage();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves parent command bars.
	// Returns:
	//     A pointer to a CXTPCommandBars object
	//-----------------------------------------------------------------------
	CXTPCommandBars* GetCommandBars() const;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Adds animation string from resource
	// Parameters:
	//     nIDResource - Specifies the string resource ID to add.
	//-----------------------------------------------------------------------
	void AddComboString(UINT nIDResource);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPCustomizeOptionsPage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPCustomizeOptionsPage)
	afx_msg void OnCheckFullMenus();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckAfterdelay();
	afx_msg void OnCheckLargeicons();
	afx_msg void OnCheckScreenttips();
	afx_msg void OnCheckShortcuts();
	afx_msg void OnResetData();
	afx_msg void OnAnimationChanged();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

public:

	enum
	{
		IDD = XTP_IDD_PAGE_OPTIONS      // Property page identifier
	};

	BOOL m_bAlwaysShowFullMenus;        // TRUE to show full menus always
	BOOL m_bShowFullAfterDelay;         // TRUE to show menus after delay
	BOOL m_bLargeIcons;                 // TRUE to show large icons
	BOOL m_bToolBarScreenTips;          // TRUE to show tool tips
	BOOL m_bToolBarAccelTips;           // TRUE to show accelerators with tool tips
	CComboBox m_comboAnimationType;     // Animation combo box
	int m_nAnimationType;               // Selected animation type
	CXTPGroupLine m_wndPersonalizedGroup;   // Personalized group line
	CXTPGroupLine m_wndOtherGroup;          // Other group line

protected:
	CXTPCustomizeSheet* m_pSheet;       // Parent sheet.
};

#endif // !defined(__XTPCUSTOMIZEOPTIONSPAGE_H__)
