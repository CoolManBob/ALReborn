// XTPSkinObjectButton.h: interface for the CXTPSkinObjectButton class.
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
#if !defined(_XTPSKINOBJECTBUTTON_H__)
#define _XTPSKINOBJECTBUTTON_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"

//===========================================================================
// Summary:
//     CXTPSkinObjectButton is a CXTPSkinObjectFrame derived class.
//     It implements standard buttons
//===========================================================================
class CXTPSkinObjectButton: public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectButton)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectButton object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectButton();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectButton object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectButton();

protected:
//{{AFX_CODEJOCK_PRIVATE
	BOOL OnHookMessage(UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);
	void OnSkinChanged(BOOL bPrevState, BOOL bNewState);
	BOOL OnHookDefWindowProc(UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);
	BOOL IsDefWindowProcAvail(int nMessage) const;

protected:
	virtual void OnDraw(CDC* pDC, CRect rc);
	BOOL IsPushButton();
	DWORD GetAlignment();
	void CalcRect(CDC* pDC, LPRECT lprc, int code);
	void ButtonDrawCheck(CDC* pDC);
	CSize GetGlyphSize(BOOL bCheckBox);
	void DrawButtonText(CDC* pDC);

	BYTE GetButtonStyle();
	void OnDraw(CDC* pDC);


protected:
	DECLARE_MESSAGE_MAP()
	//{{AFX_VIRTUAL(CXTPSkinObjectButton)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	afx_msg LRESULT OnSetState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseLeave();
	afx_msg void OnInvalidate();
	afx_msg void OnDefaultAndInvalidate();
	afx_msg LRESULT OnUpdateUIState(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

public:
	BYTE m_bStyle;          // Custom style of button

protected:

	BOOL m_bHot;            // TRUE if mouse under button
	BOOL m_bPushed;         // TRUE if button was pressed
};


#endif // !defined(_XTPSKINOBJECTBUTTON_H__)
