// XTPSkinObjectComboBox.h: interface for the CXTPSkinObjectComboBox class.
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
#if !defined(__XTPSKINOBJECTCOMBOXBOX__)
#define __XTPSKINOBJECTCOMBOXBOX__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"

//===========================================================================
// Summary:
//     CXTPSkinObjectComboBox is a CXTPSkinObjectFrame derived class.
//     It implements standard comboboxes
//===========================================================================
class CXTPSkinObjectComboBox : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectComboBox)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectComboBox object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectComboBox();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectComboBox object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectComboBox();

	virtual void OnDraw(CDC* pDC);
protected:
//{{AFX_CODEJOCK_PRIVATE

	DECLARE_MESSAGE_MAP()
	//{{AFX_VIRTUAL(CXTPSkinObjectButton)
	virtual int GetClientBrushMessage();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectButton)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnMouseLeave();
	afx_msg void OnStyleChanged();
	afx_msg LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

protected:
	BOOL m_bHot;
	BOOL m_bPushed;
//}}AFX_CODEJOCK_PRIVATE
};

//===========================================================================
// Summary:
//     CXTPSkinObjectDateTime is a CXTPSkinObjectComboBox derived class.
//     It implements date time control
//===========================================================================
class CXTPSkinObjectDateTime : public CXTPSkinObjectComboBox
{
	DECLARE_DYNCREATE(CXTPSkinObjectDateTime)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectDateTime object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectDateTime();

	CRect GetButtonRect();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when visual metrics was changed
	//-----------------------------------------------------------------------
	void RefreshMetrics();

protected:
	void OnSkinChanged(BOOL bPrevState, BOOL bNewState);
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDraw(CDC* pDC);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void DrawFrame(CDC* pDC);
//}}AFX_CODEJOCK_PRIVATE
};

//===========================================================================
// Summary:
//     CXTPSkinObjectMonthCal is a CXTPSkinObjectFrame derived class.
//     It implements month calendar
//===========================================================================
class CXTPSkinObjectMonthCal : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectMonthCal)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectMonthCal object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectMonthCal();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when visual metrics was changed
	//-----------------------------------------------------------------------
	void RefreshMetrics();
};


#endif // !defined(__XTPSKINOBJECTCOMBOXBOX__)
