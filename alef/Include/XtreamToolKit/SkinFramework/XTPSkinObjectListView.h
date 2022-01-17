// XTPSkinObjectListView.h: interface for the CXTPSkinObjectListView class.
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
#if !defined(__XTPSKINOBJECTLISTVIEW_H__)
#define __XTPSKINOBJECTLISTVIEW_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"

//===========================================================================
// Summary:
//     CXTPSkinObjectListView is a CXTPSkinObjectComCtl32Control derived class.
//     It implements standard listview control
//===========================================================================
class CXTPSkinObjectListView : public CXTPSkinObjectComCtl32Control
{
	DECLARE_DYNCREATE(CXTPSkinObjectListView)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectListView object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectListView();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectListView object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectListView();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectListView)
	void OnSkinChanged(BOOL bPrevState, BOOL bNewState);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectListView)
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(__XTPSKINOBJECTLISTVIEW_H__)
