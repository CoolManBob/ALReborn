// XTPSkinObjectMDI.h: interface for the CXTPSkinObjectMDI class.
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
#if !defined(__XTPSKINOBJECTMDI_H__)
#define __XTPSKINOBJECTMDI_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"
//{{AFX_CODEJOCK_PRIVATE

class CXTPSkinObjectMDIClient : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectMDIClient)

public:
	CXTPSkinObjectMDIClient();
	virtual ~CXTPSkinObjectMDIClient();

protected:
	BOOL m_bArrange;

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectMDIClient)
	BOOL IsDefWindowProcAvail(int /*nMessage*/) const;
	void OnHookAttached(LPCREATESTRUCT lpcs, BOOL bAuto);
	void OnHookDetached(BOOL bAuto);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectMDIClient)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnCalcScroll(WPARAM, LPARAM);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMDIArrange();
	//}}AFX_MSG

	CXTPSkinObjectFrame* m_pMDIParent;
};
//}}AFX_CODEJOCK_PRIVATE

#endif // !defined(__XTPSKINOBJECTMDI_H__)
