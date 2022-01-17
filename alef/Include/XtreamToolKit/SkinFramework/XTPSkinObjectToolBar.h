// XTPSkinObjectToolBar.h: interface for the CXTPSkinObjectToolBar class.
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
#if !defined(__XTPSKINOBJECTTOOLBAR_H__)
#define __XTPSKINOBJECTTOOLBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObject.h"
#include "XTPSkinObjectFrame.h"


//===========================================================================
// Summary:
//     CXTPSkinObjectToolBar is a CXTPSkinObjectFrame derived class.
//     It implements standard toolbar control
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectToolBar : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectToolBar)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectToolBar object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectToolBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectToolBar object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectToolBar();

protected:
	void DrawToolbarH(CDC* pDC);
	void DrawButton(CDC* pDC, int nIndex);
	HIMAGELIST GetImageList(int iMode, int iIndex);

	BOOL HasDropDownArrow(LPTBBUTTON ptbb);
	BOOL HasSplitDropDown(LPTBBUTTON ptbb);
	void DrawButtonImage(CDC* pDC, int x, int y, int nIndex);
	BOOL HasButtonImage(LPTBBUTTON ptbb);
	BOOL IsAlphaImageList(HIMAGELIST himl);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectToolBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectToolBar)
	void OnPaint();
	//}}AFX_MSG

//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(__XTPSKINOBJECTTOOLBAR_H__)
