// XTPOffice2007FrameHook.h: interface for the CXTPOffice2007FrameHook class.
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
#if !defined(__XTPOFFICE2007FRAMEHOOK_H__)
#define __XTPOFFICE2007FRAMEHOOK_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "XTPHookManager.h"

class CXTPRibbonPaintManager;
class CXTPRibbonQuickAccessControls;
class CXTPRibbonTab;
class CXTPRibbonControlTab;
class CXTPTabPaintManager;
class CXTPRibbonBar;
class CXTPRibbonGroup;
class CXTPOffice2007Theme;
class CXTPControls;


//===========================================================================
// Summary:
//     CXTPOffice2007FrameHook is CXTPHookManagerHookAble derived class is helper
//     for Office 2007 window skinning
//===========================================================================
class _XTP_EXT_CLASS CXTPOffice2007FrameHook : public CXTPHookManagerHookAble
{
	class CControlCaptionButton;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPOffice2007FrameHook object
	//-----------------------------------------------------------------------
	CXTPOffice2007FrameHook();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPOffice2007FrameHook object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPOffice2007FrameHook();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable Office 2007 frame skinning
	// Parameters:
	//     pCommandBars - Parent commandbars object
	//-----------------------------------------------------------------------
	void EnableOffice2007Frame(CXTPCommandBars* pCommandBars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable Office 2007 frame skinning
	// Parameters:
	//     pSite - Window to skin
	//     pPaintManager - Paint manager used to skin window
	//-----------------------------------------------------------------------
	void EnableOffice2007Frame(CWnd* pSite, CXTPPaintManager* pPaintManager);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns Skinned window
	//-----------------------------------------------------------------------
	CWnd* GetSite() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns Paint Manager used to skin window
	//-----------------------------------------------------------------------
	CXTPOffice2007Theme* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if caption of skinned window is visible
	//-----------------------------------------------------------------------
	BOOL IsCaptionVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if Skinned window is active
	//-----------------------------------------------------------------------
	BOOL IsFrameActive() const;

	//-------------------------------------------------------------------------
	// Summary:
	//      Recalculates borders and region of frame
	//-------------------------------------------------------------------------
	void RecalcFrameLayout();

	//-------------------------------------------------------------------------
	// Summary:
	//      Recalculates borders and region of frame with small delay
	//-------------------------------------------------------------------------
	void DelayRecalcFrameLayout();

	//-------------------------------------------------------------------------
	// Summary:
	//      Redraw ribbon bar if found
	// See Also: RedrawFrame
	//-------------------------------------------------------------------------
	void RedrawRibbonBar();

	//-------------------------------------------------------------------------
	// Summary:
	//     Redraws frame of skinned window
	// See Also: RedrawRibbonBar
	//-------------------------------------------------------------------------
	void RedrawFrame();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if frame has skinned scrollbar to draw rounded rects
	// Parameters:
	//      pnStatusHeight - Height of status bar to return
	// Returns: TRUE if frame has skinned status bar
	// See Also: IsMDIMaximized
	//-----------------------------------------------------------------------
	BOOL IsFrameHasStatusBar(int* pnStatusHeight = NULL) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if frame is maximized child window
	//-----------------------------------------------------------------------
	BOOL IsMDIMaximized() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns collection of caption buttons
	//-----------------------------------------------------------------------
	CXTPControls* GetCaptionButtons() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates height of caption
	// Returns:
	//     Height of caption in pixels
	// See Also: GetFrameBorder
	//-----------------------------------------------------------------------
	int GetCaptionHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates size of frame borders
	// Returns: Size of frame borders  in pixels
	// See Also: GetCaptionHeight
	//-----------------------------------------------------------------------
	int GetFrameBorder() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if Vista AERO theme is enabled
	// Returns:
	//     TRUE if Vista AERO theme is enabled
	//-----------------------------------------------------------------------
	BOOL IsDwmEnabled() const;

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by WindowProc, or is called during message reflection.
	// Parameters:
	//     hWnd - Window handle that the message belongs to.
	//     nMessage - Specifies the message to be sent.
	//     wParam - Specifies additional message-dependent information.
	//     lParam - Specifies additional message-dependent information.
	//     lResult - The return value of WindowProc. Depends on the message; may be NULL.
	// Returns:
	//     TRUE if message was processed.
	//-----------------------------------------------------------------------
	virtual int OnHookMessage(HWND hWnd, UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);

protected:

//{{AFX_CODEJOCK_PRIVATE
	void TrackCaptionButton(CXTPControl* pSelected);
	void UpdateFrameRegion();
	void UpdateFrameRegion(CSize szFrameRegion, BOOL bUpdate = FALSE);
	void RepositionCaptionButtons();
	void AddCaptionButton(int nID, int nHit);
	void ScreenToFrame(LPPOINT lpPoint);
	DWORD GetSiteStyle(BOOL bExStyle = FALSE) const;
	CXTPControl* HitTestCaptionButton(CPoint point);
//}}AFX_CODEJOCK_PRIVATE

protected:
	HWND m_hwndSite;                    // Skinned window handle
	CXTPRibbonBar* m_pRibbonBar;        // Child Ribbonbar object
	CXTPCommandBars* m_pCommandBars;    // Attached commandbars object
	CXTPPaintManager* m_pPaintManager;  // PaintManager used to draw frame
	BOOL m_bActive;                     // TRUE if frame is active
	CSize m_szFrameRegion;              // Previous size of window
	BOOL m_bDelayReclalcLayout;         // TRUE to recalculate layout after small delay
	CXTPControls* m_pCaptionButtons;    // Caption buttons
	CXTPControl* m_pSelected;           // Selected caption button
	BOOL m_bButtonPressed;              // Pressed caption button
	BOOL m_bOffice2007FrameEnabled;     // TRUE if skinning is enabled
	BOOL m_nSkipNCPaint;                // TRUE to skip frame drawing
	BOOL m_bInUpdateFrame;              // TRUE if UpdateFrame method is executed
	int m_nFrameBorder;                 // Size of frame borders
	BOOL m_bDwmEnabled;                 // TRUE if Vista AERO theme is enabled
	BOOL m_bLockNCPaint;                // TRUE to disable repaint of non client area

private:
	UINT m_nMsgUpdateSkinState;
	UINT m_nMsgQuerySkinState;


private:
	friend class CXTPRibbonBar;
	friend class CControlCaptionButton;
};

AFX_INLINE CWnd* CXTPOffice2007FrameHook::GetSite() const {
	return CWnd::FromHandle(m_hwndSite);
}
AFX_INLINE BOOL CXTPOffice2007FrameHook::IsFrameActive() const {
	return m_bActive;
}
AFX_INLINE CXTPControls* CXTPOffice2007FrameHook::GetCaptionButtons() const {
	return m_pCaptionButtons;
}
AFX_INLINE int CXTPOffice2007FrameHook::GetFrameBorder() const {
	return m_nFrameBorder;
}
AFX_INLINE BOOL CXTPOffice2007FrameHook::IsDwmEnabled() const {
	return m_bDwmEnabled;
}

#endif // !defined(__XTPOFFICE2007FRAMEHOOK_H__)
