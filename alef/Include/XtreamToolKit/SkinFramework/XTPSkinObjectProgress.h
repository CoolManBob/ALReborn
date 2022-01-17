// XTPSkinObjectProgress.h: interface for the CXTPSkinObjectProgress class.
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
#if !defined(__XTPSKINOBJECTPROGRESS_H__)
#define __XTPSKINOBJECTPROGRESS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"


//===========================================================================
// Summary:
//     CXTPSkinObjectProgress is a CXTPSkinObjectFrame derived class.
//     It implements standard progress control
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectProgress : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectProgress)

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectProgress object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectProgress();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectProgress object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectProgress();

protected:
//{{AFX_CODEJOCK_PRIVATE
	void OnSkinChanged(BOOL bPrevState, BOOL bNewState);
	void OnDraw(CDC* pDC);

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectProgress)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectProgress)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

protected:
	BOOL m_bStaticEdgeRemoved;
//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(__XTPSKINOBJECTPROGRESS_H__)
