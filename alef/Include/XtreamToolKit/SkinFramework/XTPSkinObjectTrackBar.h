// XTPSkinObjectTrackBar.h: interface for the CXTPSkinObjectTrackBar class.
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
#if !defined(__XTPSKINOBJECTTRACKBAR_H__)
#define __XTPSKINOBJECTTRACKBAR_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPSkinObjectFrame.h"


//===========================================================================
// Summary:
//     CXTPSkinObjectTrackBar is a CXTPSkinObjectFrame derived class.
//     It implements standard track bar control
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectTrackBar : public CXTPSkinObjectFrame
{
	DECLARE_DYNCREATE(CXTPSkinObjectTrackBar)

private:
	struct DRAWRECT
	{
		RECT rc;
		int iThumbWidth;
		int iThumbHeight;
		int iSizePhys;
		DWORD dwStyle;
	};

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectTrackBar object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectTrackBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectTrackBar object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinObjectTrackBar();

protected:

//{{AFX_CODEJOCK_PRIVATE
	void DrawTics(CDC* pDC);
	void DrawTicsOneLine(CDC* pDC, int dir, int yTic);
	void DrawTic(CDC* pDC, int x, int y, int dir, COLORREF clr);
	int LogToPhys(DWORD dwPos);
	void FillDrawRect();
	void DrawChannel(CDC* pDC);
	void DrawThumb(CDC* pDC);
	void OnDraw(CDC* pDC);

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPSkinObjectTrackBar)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSkinObjectTrackBar)
	BOOL OnEraseBkgnd(CDC* pDC);
	void OnPaint();
	LRESULT OnSetTicFreq(WPARAM, LPARAM);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	LRESULT OnPrintClient(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

protected:
	int m_nTickFreq;
	DRAWRECT m_drawRect;
	BOOL m_bHotThumb;
//}}AFX_CODEJOCK_PRIVATE
};

#endif // !defined(__XTPSKINOBJECTTRACKBAR_H__)
