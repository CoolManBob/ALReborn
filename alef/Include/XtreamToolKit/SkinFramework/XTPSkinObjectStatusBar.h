// XTPSkinObjectStatusBar.h: interface for the CXTPSkinObjectStatusBar class.
//
// This file is a part of the XTREME SKINFRAMEWORK MFC class library.
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
#if !defined(__XTPSKINOBJECTSTATUSBAR_H__)
#define __XTPSKINOBJECTSTATUSBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObject.h"
#include "XTPSkinObjectFrame.h"


//===========================================================================
// Summary:
//     CXTPSkinObjectStatusBar is a CXTPSkinObjectFrame derived class.
//     It implements standard status bar control
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectStatusBar : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectStatusBar)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectStatusBar object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectStatusBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectStatusBar object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectStatusBar();

protected:
//{{AFX_CODEJOCK_PRIVATE
	void RefreshMetrics();

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectStatusBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectStatusBar)
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(__XTPSKINOBJECTSTATUSBAR_H__)
