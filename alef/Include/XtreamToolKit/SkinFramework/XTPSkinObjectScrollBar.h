// XTPSkinObjectScrollBar.h: interface for the CXTPSkinObjectScrollBar class.
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
#if !defined(__XTPSKINOBJECTSCROLLBAR_H__)
#define __XTPSKINOBJECTSCROLLBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"


//===========================================================================
// Summary:
//     CXTPSkinObjectScrollBar is a CXTPSkinObjectFrame derived class.
//     It implements standard scroll bar control
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectScrollBar: public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectScrollBar)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectScrollBar object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectScrollBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectScrollBar object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectScrollBar();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	void OnDraw(CDC* pDC);

	//{{AFX_VIRTUAL(CXTPSkinObjectScrollBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectScrollBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSetScrollInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetScrollInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetScrollPos(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE


protected:
	XTP_SKINSCROLLBARPOSINFO m_spiCtl;
//}}AFX_CODEJOCK_PRIVATE
};


#endif // !defined(__XTPSKINOBJECTSCROLLBAR_H__)
