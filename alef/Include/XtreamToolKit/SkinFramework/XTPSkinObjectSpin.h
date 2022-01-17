// XTPSkinObjectSpin.h: interface for the CXTPSkinObjectSpin class.
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
#if !defined(__XTPSKINOBJECTSPIN_H__)
#define __XTPSKINOBJECTSPIN_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"


//===========================================================================
// Summary:
//     CXTPSkinObjectSpin is a CXTPSkinObjectFrame derived class.
//     It implements standard spin control
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectSpin : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectSpin)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectSpin object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectSpin();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectSpin object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectSpin();

	void OnDraw(CDC* pDC);

protected:
//{{AFX_CODEJOCK_PRIVATE
	UINT HitTest(CPoint pt);

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectSpin)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectSpin)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
protected:
	UINT m_nHotButton;
	UINT m_nPressedButton;
//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(__XTPSKINOBJECTSPIN_H__)
