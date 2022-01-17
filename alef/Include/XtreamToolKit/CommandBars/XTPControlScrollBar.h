// XTPControlScrollBar.h : interface for the CXTPControlScrollBar class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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

#if !defined(__XTPCONTROLSCROLLBAR_H__)
#define __XTPCONTROLSCROLLBAR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPControl.h"
#include "XTPScrollBar.h"

class CXTPControlScrollBar;

//===========================================================================
// Summary:
//     Inplace scrollbar control of CXTPControlScrollBar.
//===========================================================================
class CXTPControlScrollBarCtrl : public CXTPScrollBar
{
public:
	CXTPControlScrollBarCtrl();
	~CXTPControlScrollBarCtrl();

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CXTPControlScrollBarCtrl)
	afx_msg HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor );
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void HScroll(UINT nSBCode, UINT nPos);
	afx_msg void VScroll(UINT nSBCode, UINT nPos);
	afx_msg void OnScroll(UINT nSBCode, UINT nPos);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPControlScrollBar* m_pControl;   // Parent CXTPControlScrollBar control

private:
	friend class CXTPControlScrollBar;
};


//===========================================================================
// Summary:
//     CXTPControlScrollBar is a CXTPControl derived class.
//     It represents an scrollbar control.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlScrollBar : public CXTPControl
{
	DECLARE_XTP_CONTROL(CXTPControlScrollBar)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlScrollBar object
	//-----------------------------------------------------------------------
	CXTPControlScrollBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPControlScrollBar object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPControlScrollBar();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the scrollbar current position
	// Parameters:
	//     nPos - New position of the scrollbar control
	// See Also: GetPos, SetRange
	//-----------------------------------------------------------------------
	void SetPos(int nPos);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the current position of the scrollbar
	// Returns:
	//     The position of the scrollbar control
	// See Also: SetPos
	//-----------------------------------------------------------------------
	int GetPos();

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the upper and lower limits of the scrollbar control’s range
	// Parameters:
	//     nMin - Specifies the lower limit of the range
	//     nMax - Specifies the upper limit of the range
	// See Also: SetPos
	//-----------------------------------------------------------------------
	void SetRange(int nMin, int nMax);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to enable or disable the control.
	//     If the control does not have flags xtpFlagManualUpdate, you must call
	//     the Enable member of CCmdUI in the ON_UPDATE_COMMAND_UI handler.
	// Parameters:
	//     bEnabled - TRUE if the control is enabled.
	// See Also: GetEnabled, SetChecked
	//-----------------------------------------------------------------------
	void SetEnabled(BOOL bEnabled);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when position of scrollbar was changed
	// Parameters:
	//     nSBCode - Specifies a scroll-bar code that indicates the user’s scrolling request
	//     nPos - Contains the current scroll-box position
	//-----------------------------------------------------------------------
	virtual void OnScroll(UINT nSBCode, UINT nPos);

protected:
//{{AFX_CODEJOCK_PRIVATE
	CSize GetSize(CDC* pDC);
	BOOL HasFocus() const;
	void Draw(CDC* pDC);
	void SetRect(CRect rcControl);
	void SetParent(CXTPCommandBar* pParent);
	void SetHideFlags(DWORD dwFlags);
	void OnCalcDynamicSize(DWORD dwMode);
	void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	virtual int GetCustomizeMinWidth() const;
	virtual BOOL IsCustomizeResizeAllow() const;
	void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);
	void DoPropExchange(CXTPPropExchange* pPX);
//}}AFX_CODEJOCK_PRIVATE


protected:
	CXTPControlScrollBarCtrl* m_pScrollBarCtrl;         // Pointer to scrollbar control
	BOOL m_bHoriz;          // TRUE if control is aligned horizontally
	int m_nMin;             // Lower range
	int m_nMax;             // Upper range
	int m_nPos;             // Current position of scrollbar
	XTPScrollBarStyle m_barStyle;   // Style of scrollbar

	friend class CXTPControlScrollBarCtrl;
};

AFX_INLINE BOOL CXTPControlScrollBar::IsCustomizeResizeAllow() const {
	return TRUE;
}
AFX_INLINE int CXTPControlScrollBar::GetCustomizeMinWidth() const {
	return 20;
}


#endif // !defined(__XTPCONTROLSCROLLBAR_H__)
