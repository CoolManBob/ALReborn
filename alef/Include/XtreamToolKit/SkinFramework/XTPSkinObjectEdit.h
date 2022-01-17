// XTPSkinObjectEdit.h: interface for the CXTPSkinObjectEdit class.
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
#if !defined(__XTPSKINOBJECTEDIT_H__)
#define __XTPSKINOBJECTEDIT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"

//===========================================================================
// Summary:
//     CXTPSkinObjectEdit is a CXTPSkinObjectUser32Control derived class.
//     It implements standard edit control
//===========================================================================
class CXTPSkinObjectEdit : public CXTPSkinObjectUser32Control
{
	DECLARE_DYNCREATE(CXTPSkinObjectEdit)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectEdit object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectEdit();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectEdit object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectEdit();

protected:
//{{AFX_CODEJOCK_PRIVATE
	void DrawFrame(CDC* pDC);
	virtual int GetClientBrushMessage();
protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectEdit)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectEdit)
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(__XTPSKINOBJECTEDIT_H__)
