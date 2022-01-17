// XTPRibbonControlTab.h: interface for the CXTPRibbonControlTab class.
//
// This file is a part of the XTREME RIBBON MFC class library.
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
#if !defined(__XTPRIBBONCONTROLTAB_H__)
#define __XTPRIBBONCONTROLTAB_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPRibbonGroups;
class CXTPRibbonBar;
class CXTPRibbonGroup;
class CXTPRibbonTab;

#include "CommandBars/XTPControlPopup.h"
#include "TabManager/XTPTabManager.h"


//-----------------------------------------------------------------------
// Summary:
//     Structure used as parameter for TCN_SELCHANGING and TCN_SELCHANGE messages when Ribbon tab is changed
//-----------------------------------------------------------------------
struct NMXTPTABCHANGE : public NMXTPCONTROL
{
	CXTPRibbonTab* pTab;    // Ribbon Tab to be selected
};

//===========================================================================
// Summary:
//     CXTPRibbonControlTab is a CXTPTabManager derived class, It represents tabs of the ribbon bar
//===========================================================================
class _XTP_EXT_CLASS CXTPRibbonControlTab : public CXTPControlPopup, public CXTPTabManager
{
	DECLARE_XTP_CONTROL(CXTPRibbonControlTab)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPRibbonControlTab object
	//-----------------------------------------------------------------------
	CXTPRibbonControlTab();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPRibbonControlTab object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPRibbonControlTab();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get a pointer to the tab paint manager.
	//     The tab paint manager is used to customize the appearance of
	//     CXTPTabManagerItem objects and the tab manager.  I.e. Tab colors,
	//     styles, etc...  This member must be overridden in
	//     derived classes.
	// Returns:
	//     Pointer to CXTPTabPaintManager that contains the visual elements
	//     of the tabs.
	//-----------------------------------------------------------------------
	virtual CXTPTabPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves parent CXTPRibbonBar object
	//-----------------------------------------------------------------------
	CXTPRibbonBar* GetRibbonBar() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Cal this method to find tab by its identifier
	// Parameters:
	//     nId - Identifier of tab to be found
	// Returns:
	//     Pointer to CXTPRibbonTab object with specified identifier
	// See Also: GetTab
	//-----------------------------------------------------------------------
	CXTPRibbonTab* FindTab(int nId) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Cal this method to get tab in specified position
	// Parameters:
	//     nIndex - Index of tab to retrieve
	// Returns:
	//     Pointer to CXTPRibbonTab object in specified position
	// See Also: FindTab
	//-----------------------------------------------------------------------
	CXTPRibbonTab* GetTab(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to select a tab in the ribbon bar.
	// Parameters:
	//     pItem - Points to a CXTPTabManagerItem object to be selected.
	//-----------------------------------------------------------------------
	void SetSelectedItem(CXTPTabManagerItem* pItem);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to update position of TabManager.
	//-----------------------------------------------------------------------
	void Reposition();

	//-------------------------------------------------------------------------
	// Summary:
	//     This virtual member is called to determine if control has focus and need
	//     to draw focused rectangle around focused item
	// Returns:
	//     TRUE if header has has focus
	//-------------------------------------------------------------------------
	virtual BOOL HeaderHasFocus() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set focus to the control.
	// Parameters:
	//     bFocused - TRUE to set focus
	//-----------------------------------------------------------------------
	virtual void SetFocused(BOOL bFocused);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the focused state of the control.
	// Returns:
	//     TRUE if the control has focus; otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsFocused() const;

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when the user activate control using its underline.
	//----------------------------------------------------------------------
	void OnUnderlineActivate();

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to check if control accept focus
	// See Also: SetFocused
	//----------------------------------------------------------------------
	virtual BOOL IsFocusable() const;

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when the control becomes selected.
	// Parameters:
	//     bSelected - TRUE if the control becomes selected.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//----------------------------------------------------------------------
	BOOL  OnSetSelected(int bSelected);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when a non-system key is pressed.
	// Parameters:
	//     nChar - Specifies the virtual key code of the given key.
	//     lParam   - Specifies additional message-dependent information.
	// Returns:
	//     TRUE if key handled, otherwise returns FALSE
	//----------------------------------------------------------------------
	BOOL OnHookKeyDown (UINT nChar, LPARAM lParam);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when the icon of the ribbon tab needs to be
	//     drawn.
	// Parameters:
	//     pDC    - Pointer to the destination device context.
	//     pt     - Specifies the location of the image.
	//     pItem  - CXTPTabManagerItem object to draw icon on.
	//     bDraw  - TRUE if the icon needs to be drawn, I.e. the icon size
	//              changed.  FALSE if the icon does not need to be
	//              drawn or redrawn.
	//     szIcon - Size of the tab icon.
	// Remarks:
	//     For example, on mouseover.  This member is overridden by its
	//     descendants.  This member must be overridden in
	//     derived classes.
	// Returns:
	//     TRUE if the icon was successfully drawn, FALSE if the icon
	//     was not drawn.
	//-----------------------------------------------------------------------
	BOOL DrawIcon(CDC* pDC, CPoint pt, CXTPTabManagerItem* pItem, BOOL bDraw, CSize& szIcon) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Initiates redrawing of the ribbon bar control.
	// Remarks:
	//     Call this member function if you want to initialize redrawing
	//     of the control. The control will be redrawn taking into account
	//     its latest state.
	// Parameters:
	//     lpRect - The rectangular area of the window that is invalid.
	//     bAnimate - TRUE to animate changes in bounding rectangle.
	//-----------------------------------------------------------------------
	void RedrawControl(LPCRECT lpRect, BOOL bAnimate);

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks to see if the mouse is locked.
	// Remarks:
	//     The mouse is locked when a CXTPCommandBarsPopup is currently visible.
	// Returns:
	//     TRUE if locked; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsMouseLocked() const;


	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//----------------------------------------------------------------------
	void Draw(CDC* pDC);
protected:
//{{AFX_CODEJOCK_PRIVATE
	void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));
	void ShowPopupBar(BOOL bKeyboard);
	BOOL OnSetPopup(BOOL bPopup);
	void SetEnabled(BOOL bEnabled);
	CString GetItemTooltip(const CXTPTabManagerItem* pItem) const;
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE
	virtual HRESULT GetAccessibleChildCount(long* pcountChildren);
	virtual HRESULT GetAccessibleChild(VARIANT varChild, IDispatch** ppdispChild);
	virtual HRESULT GetAccessibleName(VARIANT varChild, BSTR* pszName);
	virtual HRESULT GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole);
	virtual HRESULT AccessibleLocation(long *pxLeft, long *pyTop, long *pcxWidth, long* pcyHeight, VARIANT varChild);
	virtual HRESULT AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarChild);
	virtual HRESULT GetAccessibleState(VARIANT varChild, VARIANT* pvarState);
	virtual HRESULT GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction);
	virtual HRESULT AccessibleDoDefaultAction(VARIANT varChild);
	virtual HRESULT AccessibleSelect(long flagsSelect, VARIANT varChild);
//}}AFX_CODEJOCK_PRIVATE

protected:
	BOOL m_bFocused; // TRUE if groups focused

	friend class CXTPRibbonBar;
};

AFX_INLINE CXTPRibbonBar* CXTPRibbonControlTab::GetRibbonBar() const {
	return (CXTPRibbonBar*)m_pParent;
}


#endif // !defined(__XTPRIBBONCONTROLTAB_H__)
