// XTPOffice2007Theme.h : interface for the CXTPOffice2007Theme class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPOFFICE2007THEME_H__)
#define __XTPOFFICE2007THEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CXTPOffice2007Image;
class CXTPOffice2007Images;
class CXTPOffice2007FrameHook;

#include "XTPPaintManager.h"

//===========================================================================
// Summary:
//     The CXTPOffice2007Theme class is used to enable an Office 2007 style theme for Command Bars
// See Also: CXTPPaintManager::SetTheme
//===========================================================================
class _XTP_EXT_CLASS CXTPOffice2007Theme : public CXTPOffice2003Theme
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPOffice2007Theme object.
	//-----------------------------------------------------------------------
	CXTPOffice2007Theme();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPOffice2007Theme object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPOffice2007Theme();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Loads a bitmap image from the specified path.
	// Parameters:
	//     lpszFileName - The name of the file that contains the bitmaps
	//                    to load.
	// Remarks:
	//     The images are the bitmaps that represent all the visual components
	//     of the Ribbon Bar.  For example tab buttons, group buttons, menu buttons,
	//     toolbar buttons, option button, toolbar dropdown, etc.
	//     The images are loaded using LoadImage and are stored in the m_pImages
	//     image collection.
	//     Images for the Office 2007 theme can be found in the \Source\Ribbon\Res
	//     folder.
	// Returns: New CXTPOffice2007Image containing the loaded bitmap.
	// See Also: SetImageHandle, m_pImages
	//-----------------------------------------------------------------------
	CXTPOffice2007Image* LoadImage(LPCTSTR lpszFileName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Specifies the resource that contains the bitmaps to be loaded for the
	//     theme.  This allows images to be loaded from something other than bitmap files.
	// Parameters:
	//     hResource - Handle to a visual style dll file handle. Or
	//                 some other resource that contains the images
	//                 to load for the theme.
	// Remarks:
	//     The images are the bitmaps that represent all the visual components
	//     of the Ribbon Bar.  For example tab buttons, group buttons, menu buttons,
	//     toolbar buttons, option button, toolbar dropdown, etc.
	//     The images are loaded using LoadImage and are stored in the m_pImages
	//     image collection.
	//     Images for the Office 2007 theme can be found in the \Source\Ribbon\Res
	//     folder.
	// Example:
	//     <code>((CXTPOffice2007Theme*)XTPPaintManager())->SetImageHandle(XTPSkinManager()->GetResourceFile()->GetModuleHandle());</code>
	// See Also: LoadImage, m_pImages
	//-----------------------------------------------------------------------
	void SetImageHandle(HMODULE hResource, LPCTSTR lpszIniFileName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves height of caption
	//-----------------------------------------------------------------------
	int GetFrameCaptionHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves ribbon caption height
	//-----------------------------------------------------------------------
	int GetRibbonCaptionHeight() const;


protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to refresh the visual metrics of the manager.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the base theme of the manager.
	// Returns:
	//     Returns the base theme.
	//-----------------------------------------------------------------------
	XTPPaintTheme BaseTheme() { return xtpThemeOffice2007; }

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to fill a status bar.
	// Parameters:
	//     pDC  - Pointer to a valid device context
	//     pBar - Points to a CXTPStatusBar object
	//-----------------------------------------------------------------------
	virtual void FillStatusBar(CDC* pDC, CXTPStatusBar* pBar);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the status bar's gripper.
	// Parameters:
	//     pDC      - Pointer to a valid device context
	//     rcClient - Client rectangle of the status bar.
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarGripper(CDC* pDC, CRect rcClient);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw frame of single cell of status bar.
	// Parameters:
	//     pDC - Points to a valid device context.
	//     rc - CRect object specifying size of area.
	//     bGripperPane - TRUE if pane is last cell of status bar
	//-----------------------------------------------------------------------
	virtual void DrawStatusBarPaneBorder(CDC* pDC, CRect rc, BOOL bGripperPane);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw frame if Ribbon was found
	// Parameters:
	//      pDC - Pointer to device context
	//      pFrameHook - CXTPOffice2007FrameHook hook object
	//-----------------------------------------------------------------------
	virtual void DrawRibbonFrame(CDC* pDC, CXTPOffice2007FrameHook* pFrameHook);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to create window region
	// Parameters:
	//      pFrameHook - CXTPOffice2007FrameHook hook object
	//      sz - Window size
	// Returns: Region of window to be set
	//-----------------------------------------------------------------------
	virtual HRGN CalcRibbonFrameRegion(CXTPOffice2007FrameHook* pFrameHook, CSize sz);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw caption button
	// Parameters:
	//      pDC - Pointer to device context
	//      rc - Bounding rectangle of the button
	//      nId - Identifier of the button
	//      bSelected - TURE if button is highlighted
	//      bPressed -  TURE if button is pressed
	//      bActive - TURE if frame is active
	//-----------------------------------------------------------------------
	virtual void DrawRibbonFrameCaptionButton(CDC* pDC, CRect rc, int nId, BOOL bSelected, BOOL bPressed, BOOL bActive);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to get CXTPOffice2007Images pointer
	//-----------------------------------------------------------------------
	CXTPOffice2007Images* GetImages() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set images for theme
	// Parameters:
	//     pImages - new CXTPOffice2007Images to bet set
	// See Also: GetImages
	//-----------------------------------------------------------------------
	void SetImages(CXTPOffice2007Images* pImages);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves buttons text color
	// Parameters:
	//     bSelected   - TRUE if the control is selected.
	//     bPressed    - TRUE if the control is pushed.
	//     bEnabled    - TRUE if the control is enabled.
	//     bChecked    - TRUE if the control is checked.
	//     bPopuped    - TRUE if the control is popuped.
	//     barType     - Parent's bar type
	//     barPosition - Parent's bar position.
	//-----------------------------------------------------------------------
	virtual COLORREF GetRectangleTextColor(BOOL bSelected, BOOL bPressed, BOOL bEnabled, BOOL bChecked, BOOL bPopuped, XTPBarType barType, XTPBarPosition barPosition);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws workspace space
	// Parameters:
	//     pDC - Pointer to device context
	//     rc - Bounding rectangle
	//     rcExclude - Excluded rectangle
	//-----------------------------------------------------------------------
	void FillWorkspace(CDC* pDC, CRect rc, CRect rcExclude);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw frame caption text
	// Parameters:
	//     pDC - Pointer to device context
	//     rcCaptionText - Bounding rectangle of window text
	//     pSite - Frame to draw
	//     bActive - TRUE if frame is active
	//-----------------------------------------------------------------------
	virtual void DrawCaptionText(CDC* pDC, CRect rcCaptionText, CWnd* pSite, BOOL bActive);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if images is available
	// Returns:
	//     TRUE if images exist in application
	//-----------------------------------------------------------------------
	BOOL IsImagesAvailable();


	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves frame small icon
	// Parameters:
	//     pFrame - Window to retrieve icon
	// See Also: GetFrameLargeIcon
	//-----------------------------------------------------------------------
	HICON GetFrameSmallIcon(CWnd* pFrame);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves frame large icon
	// Parameters:
	//     pFrame - Window to retrieve icon
	// See Also: GetFrameLargeIcon
	//-----------------------------------------------------------------------
	HICON GetFrameLargeIcon(CWnd* pFrame);

public:
	BOOL m_bFrameStatusBar; // TRUE if status bar used to draw.

protected:
	CXTPPaintManagerFont m_fontFrameCaption;    // Font used to draw text in the frame caption

	int m_nRibbonCaptionHeight;         // Ribbon caption height
	int m_nFrameCaptionHeight;          // Frame caption


protected:
//{{AFX_CODEJOCK_PRIVATE
	CXTPOffice2007Images* m_pImages;
	COLORREF m_clrFrameBorderActive0;
	COLORREF m_clrFrameBorderActive1;
	COLORREF m_clrFrameBorderActive2;
	COLORREF m_clrFrameBorderActive3;
	COLORREF m_clrFrameBorderInactive0;
	COLORREF m_clrFrameBorderInactive1;
	COLORREF m_clrFrameBorderInactive2;
	COLORREF m_clrFrameBorderInactive3;
	COLORREF m_clrFrameCaptionTextActive;
	COLORREF m_clrFrameCaptionTextInActive;
	COLORREF m_clrFrameCaptionTextActiveTitle;
	COLORREF m_clrMenuBarText;
	COLORREF m_clrStatusBarShadow;
	CXTPPaintManagerColorGradient m_clrStatusBarTop;
	CXTPPaintManagerColorGradient m_clrStatusBarBottom;
	COLORREF m_clrWorkspaceClientTop;
	COLORREF m_clrWorkspaceClientMiddle;
	COLORREF m_clrWorkspaceClientBottom;
//}}AFX_CODEJOCK_PRIVATE
};

AFX_INLINE int CXTPOffice2007Theme::GetFrameCaptionHeight() const {
	return m_nFrameCaptionHeight;
}
AFX_INLINE int CXTPOffice2007Theme::GetRibbonCaptionHeight() const {
	return m_nRibbonCaptionHeight;
}



#endif // #if !defined(__XTPOFFICE2007THEME_H__)
