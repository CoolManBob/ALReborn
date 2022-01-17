// XTPSkinObjectMenu.h: interface for the CXTPSkinObjectMenu class.
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
#if !defined(__XTPSKINOBJECTMENU_H__)
#define __XTPSKINOBJECTMENU_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"

class CXTPSkinObjectMenu : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectMenu)

public:
	CXTPSkinObjectMenu();
	virtual ~CXTPSkinObjectMenu();

protected:
	void OnDraw(CDC* pDC);

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectMenu)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectMenu)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg LRESULT OnPrint(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDrawNonCLient(CDC* pDC);
	afx_msg LRESULT OnSelectItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChar(WPARAM wParam, LPARAM /*lParam*/);
	void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
};

#endif // !defined(__XTPSKINOBJECTMENU_H__)
