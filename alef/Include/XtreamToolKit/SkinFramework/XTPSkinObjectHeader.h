// XTPSkinObjectHeader.h: interface for the CXTPSkinObjectHeader class.
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
#if !defined(__XTPSKINOBJECTHEADER_H__)
#define __XTPSKINOBJECTHEADER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"


//===========================================================================
// Summary:
//     CXTPSkinObjectHeader is a CXTPSkinObjectFrame derived class.
//     It implements standard list header control
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectHeader : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectHeader)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectHeader object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectHeader();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectHeader object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectHeader();

	void OnDraw(CDC* pDC);

protected:
//{{AFX_CODEJOCK_PRIVATE

	void DrawItemEntry(CDC* pDC, int nIndex, CRect rcItem, int nState);
	int HitTest(CPoint pt, UINT* pFlags = NULL) const;

protected:
	DECLARE_MESSAGE_MAP()
	//{{AFX_VIRTUAL(CXTPSkinObjectHeader)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectHeader)
	afx_msg void OnPaint();

	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

protected:
	BOOL m_bLBtnDown;
	int m_nHotItem;

//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(__XTPSKINOBJECTHEADER_H__)
