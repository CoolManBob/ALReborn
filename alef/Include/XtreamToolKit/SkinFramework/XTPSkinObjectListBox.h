// XTPSkinObjectListBox.h: interface for the CXTPSkinObjectListBox class.
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
#if !defined(__XTPSKINOBJECTLISTBOX_H__)
#define __XTPSKINOBJECTLISTBOX_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"


//===========================================================================
// Summary:
//     CXTPSkinObjectListBox is a CXTPSkinObjectUser32Control derived class.
//     It implements standard list box control
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectListBox : public CXTPSkinObjectUser32Control
{
	DECLARE_DYNCREATE(CXTPSkinObjectListBox)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectListBox object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectListBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectListBox object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectListBox();

protected:
//{{AFX_CODEJOCK_PRIVATE
	//DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectListBox)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectListBox)
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

};

#endif // !defined(__XTPSKINOBJECTLISTBOX_H__)
