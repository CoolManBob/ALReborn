// XTPSkinObjectTreeView.h: interface for the CXTPSkinObjectTreeView class.
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
#if !defined(__XTPSKINOBJECTTREEVIEW_H__)
#define __XTPSKINOBJECTTREEVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"


//===========================================================================
// Summary:
//     CXTPSkinObjectTreeView is a CXTPSkinObjectComCtl32Control derived class.
//     It implements standard tree view control
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectTreeView : public CXTPSkinObjectComCtl32Control
{
	DECLARE_DYNCREATE(CXTPSkinObjectTreeView)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectTreeView object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectTreeView();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectTreeView object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectTreeView();

protected:
//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTPSkinObjectTreeView)
	virtual void OnSkinChanged(BOOL bPrevState, BOOL bNewState);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectTreeView)
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(__XTPSKINOBJECTTREEVIEW_H__)
