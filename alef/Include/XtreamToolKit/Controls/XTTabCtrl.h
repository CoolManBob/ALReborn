// XTTabCtrl.h interface for the CXTTabCtrl class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#if !defined(__XTTABCTRL_H__)
#define __XTTABCTRL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CXTTabCtrlButtons;

#include "XTTabBaseTheme.h"
#include "XTThemeManager.h"
#include "XTTabBase.h"

DECLATE_TABCTRLEX_BASE(CXTTabCtrlBase, CTabCtrl, CXTTabExBase)

//===========================================================================
// Summary:
//     CXTTabCtrl is a multiple inheritance class derived from CTabCtrl and
//     CXTTabCtrlBase. It is used to create a CXTTabCtrl class object. See
//     CXTTabCtrlBase for additional functionality.
//===========================================================================
class _XTP_EXT_CLASS CXTTabCtrl : public CXTTabCtrlBase
{
	DECLARE_DYNAMIC(CXTTabCtrl)
	DECLARE_THEME_REFRESH(CXTTabCtrl)

	friend class CXTTabExBase;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTTabCtrl object
	//-----------------------------------------------------------------------
	CXTTabCtrl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTTabCtrl object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTTabCtrl();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to enable or disable tooltip usage.
	// Parameters:
	//     bEnable - TRUE to enable tooltip usage.
	// Returns:
	//     TRUE if the tooltip control was found and updated, otherwise returns
	//     FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL EnableToolTips(BOOL bEnable);

protected:
//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTTabCtrl)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTTabCtrl)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE

};

//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTTABCTRL_H__)
