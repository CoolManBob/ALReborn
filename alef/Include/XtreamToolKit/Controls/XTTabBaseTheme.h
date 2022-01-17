// XTTabBaseTheme.h: interface for the CXTTabBaseTheme class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#if !defined(__XTTABCTRLTHEME_H__)
#define __XTTABCTRLTHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPDrawHelpers.h"
#include "XTThemeManager.h"

class CXTTabCtrl;
class CXTTabBase;
class CXTTabCtrlButton;
class CXTTabCtrlButtons;
class CXTTcbItem;

DECLARE_THEME_FACTORY(CXTTabBaseTheme)

//===========================================================================
// Summary:
//     CXTTabBaseTheme is used to draw the CXTTabBase object.  All themes
//     used for CXTTabBase should inherit from this base class.
//===========================================================================
class _XTP_EXT_CLASS CXTTabBaseTheme : public CXTThemeManagerStyle
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Construct a CXTTabBaseTheme object.
	//-------------------------------------------------------------------------
	CXTTabBaseTheme();
	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTTabBaseTheme object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	virtual ~CXTTabBaseTheme();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this function to refresh the colors of the Tab control.
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to render the tab control using
	//     the specified theme.
	// Parameters:
	//     pDC      - A CDC pointer that represents the current device
	//                context.
	//     pTabCtrl - A pointer to a CXTTabBase object.  Contains information
	//                about the tab control.
	//-----------------------------------------------------------------------
	virtual void DrawTabCtrl(CDC* pDC, CXTTabBase* pTabCtrl);
	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to get the current background color for the theme.
	// Returns:
	//      A CXTPPaintManagerColor object that contains the value for the current background color.
	// See Also:
	//      CXTTabBaseThemeOffice2003::GetBackColor
	//-----------------------------------------------------------------------
	virtual CXTPPaintManagerColor GetBackColor() const;
	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to redraw the borders for a particular tab.
	// Parameters:
	//      pMember - A pointer to a CXTTcbItem struct that contains information about the tab.
	// See Also:
	//      CXTTabBaseThemeOfficeXP::AdjustBorders
	//-----------------------------------------------------------------------
	virtual void AdjustBorders(CXTTcbItem* pMember);
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw a single button of a tab control.
	// Parameters:
	//     pDC       - A CDC pointer that represents the current device
	//                 context.
	//     pButton   - A pointer to a CXTTabCtrlButton object.
	//     clrButton - The color of the button.
	// Note:
	//     This function ASSERTS if called. Must be overridden.
	//-----------------------------------------------------------------------
	virtual void DrawButton(CDC* pDC, CXTTabCtrlButton* pButton, COLORREF clrButton);

	virtual COLORREF FillButtons(CDC* pDC, CXTTabCtrlButtons* pButtons);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw the borders around the tab control.
	// Parameters:
	//      pDC      - A pointer to a valid device context.
	//      rcClient - A CRect object that contains the location and the dimensions of the Tab Control.
	// See Also:
	//      CXTTabBaseThemeOfficeXP::DrawBorders
	//-----------------------------------------------------------------------
	virtual void DrawBorders(CDC* pDC, const CRect& rcClient);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw the header for the Tab Control.
	// Parameters:
	//      pDC          - A pointer to a valid device context.
	//      pTabCtrlBase - A pointer to a CXTTabBase object.  Contains information
	//                     about the tab control.
	//      rcHeader     - A CRect object that contains the location and the dimensions of the header.
	// See Also:
	//      CXTTabBaseThemeOfficeXP::FillHeader, FillTabFaceNativeWinXP
	//-----------------------------------------------------------------------
	virtual void FillHeader(CDC* pDC, CXTTabBase* pTabCtrlBase, CRect rcHeader);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw the tab face so that the
	//      tab appears with the Native Windows XP theme.
	// Parameters:
	//      pDC       - A pointer to a valid device context.
	//      pTabCtrl  - A pointer to a CTabCtrl object. This is the associated TabControl object.
	//      rcItem    - A CRect object that contains the location and the dimensions of the tab.
	//      bSelected - TRUE if the tab is selected. FALSE otherwise.
	// See Also:
	//      CXTTabBaseThemeOfficeXP::FillTabFace, CXTTabBaseThemeOffice2003::FillTabFace,
	//      FillTabFace, DrawTab, DrawTabIcon, DrawTabText
	//-----------------------------------------------------------------------
	virtual void FillTabFaceNativeWinXP(CDC* pDC, CTabCtrl* pTabCtrl, CRect rcItem, BOOL bSelected);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called draw the tab face.
	// Parameters:
	//      pDC       - A pointer to a valid device context.
	//      pTabCtrl  - A pointer to a CTabCtrl object.
	//      rcItem    - A CRect object that contains the location and the dimensions of the tab.
	//      bSelected - TRUE if the tab is selected.  FALSE otherwise.
	// See Also:
	//      CXTTabBaseThemeOfficeXP::FillTabFace, CXTTabBaseThemeOffice2003::FillTabFace,
	//      DrawTab, DrawTabIcon, DrawTabText, FillTabFaceNativeWinXP
	//-----------------------------------------------------------------------
	virtual void FillTabFace(CDC* pDC, CTabCtrl* pTabCtrl, CRect rcItem, BOOL bSelected);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw a tab on the TabControl.
	// Parameters:
	//      pDC          - A pointer to a valid device context.
	//      pTabCtrlBase - A pointer to a CXTTabBase object.  Contains information
	//                     about the tab control.
	//      iItem        - An int that specifies the tab item to draw.
	// See Also:
	//      CXTTabBaseThemeOfficeXP::FillTabFace, CXTTabBaseThemeOffice2003::FillTabFace,
	//      DrawTabIcon, DrawTabText, FillTabFaceNativeWinXP, CXTExcelTabCtrlTheme::DrawTab,
	//      FillTabFace
	//-----------------------------------------------------------------------
	virtual void DrawTab(CDC* pDC, CXTTabBase* pTabCtrlBase, int iItem);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw the icon on the tab.
	// Parameters:
	//      pDC      - A pointer to a valid device context.
	//      pTabCtrl - A pointer to a CTabCtrl object.
	//      rcItem   - A CRect object that contains the location and the dimensions of the tab.
	//      iItem    - An int that represents the tab item to draw the icon on.
	// See Also:
	//      CXTTabBaseThemeOfficeXP::FillTabFace, CXTTabBaseThemeOffice2003::FillTabFace,
	//      DrawTab, DrawTabText, FillTabFace,
	//      FillTabFaceNativeWinXP, CXTExcelTabCtrlTheme::DrawTab
	//-----------------------------------------------------------------------
	virtual void DrawTabIcon(CDC* pDC, CTabCtrl* pTabCtrl, CRect& rcItem, int iItem);
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw text on the tab.
	// Parameters:
	//      pDC       - Pointer to a valid device context.
	//      pTabCtrl  - A pointer to a CTabCtrl object.
	//      rcItem    - A CRect object that contains the location and the dimensions of the tab.
	//      iItem     - Index of the item to draw
	//      bSelected - TRUE if the tab is selected.  FALSE otherwise.
	//      bBoldFont - TRUE if the text is to be bold. FALSE otherwise.
	// See Also:
	//      CXTTabBaseThemeOfficeXP::FillTabFace, CXTTabBaseThemeOffice2003::FillTabFace,
	//      DrawTabIcon, FillTabFace, FillTabFaceNativeWinXP,
	//      CXTExcelTabCtrlTheme::DrawTab
	//-----------------------------------------------------------------------
	virtual void DrawTabText(CDC* pDC, CTabCtrl* pTabCtrl, CRect& rcItem, int iItem, BOOL bSelected, BOOL bBoldFont);
	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to get the bounding rectangle of the tab
	//      header area.
	// Parameters:
	//      pTabCtrlBase - A pointer to a CXTTabBase object. Contains information
	//                     about the tab control.
	// Returns:
	//      A CRect object that contains the bounding rectangle of the header area.
	// See Also:
	//      FillHeader
	//-----------------------------------------------------------------------
	virtual CRect GetHeaderRect(CXTTabBase* pTabCtrlBase);
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to paint the Navigation buttons on the tab control.
	// Parameters:
	//     pTabCtrlBase - A pointer to a CXTTabBase object. Contains
	//                    information about the tab control.
	// See Also:
	//      DrawButton
	//-----------------------------------------------------------------------
	virtual void ShowButtons(CXTTabBase* pTabCtrlBase);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw a border around a Tab child window.
	// Parameters:
	//      pDC - A pointer to a valid device context.
	//      r   - A CRect object that contains the location and the dimensions of the child window.
	// See Also:
	//      CXTTabBaseThemeOfficeXP::DrawBorder, CXTTabBaseThemeOffice2003::DrawBorder
	//-----------------------------------------------------------------------
	virtual void DrawBorder(CDC* pDC, const CRect& r);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to determine the tab style.
	// Parameters:
	//      pTabCtrl - A pointer to a CTabCtrl object.
	// Returns:
	//      A DWORD representing the tab style.
	// See Also:
	//      IsLeft, IsRight, IsTop, IsBottom, IsVert, IsHorz
	//-----------------------------------------------------------------------
	DWORD GetTabStyle(CTabCtrl* pTabCtrl) const {
		ASSERT(::IsWindow(pTabCtrl->GetSafeHwnd())); return (pTabCtrl->GetStyle() & (TCS_VERTICAL|TCS_RIGHT|TCS_BOTTOM));
	}
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to determine if the tabs are drawn
	//      on the left side of the TabControl.
	// Parameters:
	//      dwStyle - A DWORD value that represents the current tab style.
	// Returns:
	//      A BOOL.  TRUE if the tabs are drawn on the left side of the TabControl.
	//      FALSE otherwise.
	// See Also:
	//      GetTabStyle, IsRight, IsTop, IsBottom, IsVert, IsHorz
	//-----------------------------------------------------------------------
	BOOL IsLeft(DWORD dwStyle) const {
		return ((dwStyle & (TCS_VERTICAL|TCS_RIGHT|TCS_BOTTOM)) == TCS_VERTICAL);
	}
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to determine if the tabs are drawn
	//      on the right side of the TabControl.
	// Parameters:
	//      dwStyle - A DWORD value that represents the current tab style.
	// Returns:
	//      A BOOL.  TRUE if the tabs are drawn on the right side of the TabControl.
	//      FALSE otherwise.
	// See Also:
	//      GetTabStyle, IsLeft, IsTop, IsBottom, IsVert, IsHorz
	//-----------------------------------------------------------------------
	BOOL IsRight(DWORD dwStyle) const {
		return ((dwStyle & (TCS_VERTICAL|TCS_RIGHT|TCS_BOTTOM)) == (TCS_VERTICAL|TCS_RIGHT));
	}
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to determine if the tabs are drawn
	//      on the top of the TabControl.
	// Parameters:
	//      dwStyle - A DWORD value that represents the current tab style.
	// Returns:
	//      A BOOL.  TRUE if the tabs are drawn on the top of the TabControl.
	//      FALSE otherwise.
	// See Also:
	//      GetTabStyle, IsLeft, IsRight, IsBottom, IsVert, IsHorz
	//-----------------------------------------------------------------------
	BOOL IsTop(DWORD dwStyle) const {
		return ((dwStyle & (TCS_VERTICAL|TCS_RIGHT|TCS_BOTTOM)) == 0);
	}
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to determine if the tabs are drawn
	//      on the bottom of the TabControl.
	// Parameters:
	//      dwStyle - A DWORD value that represents the current tab style.
	// Returns:
	//      A BOOL.  TRUE if the tabs are drawn on the bottom of the TabControl.
	//      FALSE otherwise.
	// See Also:
	//      GetTabStyle, IsLeft, IsRight, IsTop, IsVert, IsHorz
	//-----------------------------------------------------------------------
	BOOL IsBottom(DWORD dwStyle) const {
		return ((dwStyle & (TCS_VERTICAL|TCS_RIGHT|TCS_BOTTOM)) == TCS_BOTTOM);
	}
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to determine if the tabs are drawn
	//      vertically on the TabControl.
	// Parameters:
	//      dwStyle - A DWORD value that represents the current tab style.
	// Returns:
	//      A BOOL.  TRUE if the tabs are drawn vertically on the TabControl.
	//      FALSE otherwise.
	// See Also:
	//      GetTabStyle, IsLeft, IsRight, IsTop, IsBottom, IsHorz
	//-----------------------------------------------------------------------
	BOOL IsVert(DWORD dwStyle) const {
		return (IsRight(dwStyle) || IsLeft(dwStyle));
	}
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to determine if the tabs are drawn
	//      horizontally on the TabControl.
	// Parameters:
	//      dwStyle - A DWORD value that represents the current tab style.
	// Returns:
	//      A BOOL.  TRUE if the tabs are drawn horizontally on the TabControl.
	//      FALSE otherwise.
	// See Also:
	//      GetTabStyle, IsLeft, IsRight, IsTop, IsVert, IsBottom
	//-----------------------------------------------------------------------
	BOOL IsHorz(DWORD dwStyle) const {
		return (IsTop(dwStyle) || IsBottom(dwStyle));
	}
	//-----------------------------------------------------------------------
	// Summary:
	//       This member function is called to swap the dark and the light colors.
	// Parameters:
	//       clrLight - An RGB value that represents the light color.
	//       clrDark  - An RGB value that represents the dark color.
	// See Also:
	//      VerticalLine, HorizontalLine, Pixel
	//-----------------------------------------------------------------------
	void SwapColors(COLORREF& clrLight, COLORREF& clrDark);
	//-----------------------------------------------------------------------
	// Summary:
	//       This member function is called to draw a vertical line.
	// Parameters:
	//      pDC     - A pointer to a valid device context.
	//      xPos    - An int that specifies the logical x-coordinate of the line.
	//      yPos    - An int that specifies the logical y-coordinate of the line.
	//      nLength - Specifies the length of the line.
	//      clr     - An RGB value that specifies the color of the line.
	// See Also:
	//      HorizontalLine, Pixel, SwapColors
	//-----------------------------------------------------------------------
	void VerticalLine(CDC* pDC, int xPos, int yPos, int nLength, COLORREF clr);
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw a horizontal line.
	// Parameters:
	//      pDC     - A pointer to a valid device context.
	//      xPos    - An int that specifies the logical x-coordinate of the line.
	//      yPos    - An int that specifies the logical y-coordinate of the line.
	//      nLength - Specifies the length of the line.
	//      clr     - An RGB value that specifies the color of the line.
	// See Also:
	//      VerticalLine, Pixel, SwapColors
	//-----------------------------------------------------------------------
	void HorizontalLine(CDC* pDC, int xPos, int yPos, int nLength, COLORREF clr);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw a single pixel.
	// Parameters:
	//      pDC  - A pointer to a valid device context.
	//      xPos - An int that specifies the logical x-coordinate of the pixel.
	//      yPos - An int that specifies the logical y-coordinate of the pixel.
	//      clr  - An RGB value that specifies the color of the pixel.
	// See Also:
	//       VerticalLine, HorizontalLine, SwapColors
	//-----------------------------------------------------------------------
	void Pixel(CDC* pDC, int xPos, int yPos, COLORREF clr);

private:
	typedef void (AFX_CDECL* LPFNDRAWROTATEDBITS)(int cx, int cy, UINT* pSrcBits, UINT* pDestBits);
	void DrawRotatedButton(CDC* pDC, CRect rcItem, BOOL bSelected, BOOL bSwap, LPFNDRAWROTATEDBITS pfnRotatedProc);
	static void AFX_CDECL DrawRotatedBitsLeft(int cx, int cy, UINT* pSrcBits, UINT* pDestBits);
	static void AFX_CDECL DrawRotatedBitsRight(int cx, int cy, UINT* pSrcBits, UINT* pDestBits);
	static void AFX_CDECL DrawRotatedBitsBottom(int cx, int cy, UINT* pSrcBits, UINT* pDestBits);


public:
	BOOL m_bSystemDrawing;  // TRUE to perform system drawing. FALSE otherwise.
	BOOL m_bUseWinThemes;   // TRUE to use Windows XP themes when available. FALSE otherwise.

protected:
	int  m_iEdge;    // The Y dimension of a 3D border.
	CXTPWinThemeWrapper m_wrapTheme; // Windows Theme wrapper.
	CXTPPaintManagerColor m_clrNormalText; // Color of text in a normal tab.  Normal tabs are tabs that are not selected or disabled.
	CXTPPaintManagerColor m_clrSelectedText; // Color of text in a "selected" tab button.  The "selected" tab button is the tab with focus.  I.e. The tab becomes active when it is clicked.
};

//===========================================================================
// Summary:
//     Class CXTTabBaseThemeOfficeXP is derived from CXTTabBaseTheme.
//     This class is used to implement the office XP theme for the tab controls.
//===========================================================================
class _XTP_EXT_CLASS CXTTabBaseThemeOfficeXP : public CXTTabBaseTheme
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Construct a CXTTabBaseThemeOfficeXP object.
	//-------------------------------------------------------------------------
	CXTTabBaseThemeOfficeXP();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     This function calls the base class RefreshMetrics and then
	//     calls DeleteObject() for the CPen member variables m_penBlack,
	//     m_penWhite, m_penFace, and m_penText.  Then the CPen member variables
	//     are created with the system default colors for COLOR_BTNTEXT,
	//     COLOR_3DHILIGHT, COLOR_3DFACE, and COLOR_3DSHADOW.  Finally, the
	//     member variable m_iEdge is set to the SM_CYEDGE system value.
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();
	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to redraw the borders for a particular tab.
	// Parameters:
	//      pMember - A pointer to a CXTTcbItem struct that contains information about the tab.
	// See Also:
	//      CXTTabBaseTheme::AdjustBorders
	//-----------------------------------------------------------------------
	virtual void AdjustBorders(CXTTcbItem* pMember);
	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to draw single button of tab control.
	// Parameters:
	//     pDC       - A CDC pointer that represents the current device
	//                 context.
	//     pButton   - A pointer to a CXTTabCtrlButton object.
	//     clrButton - The color of the button.
	// Note:
	//     This function ASSERTS if called. Must be overridden.
	//-----------------------------------------------------------------------
	virtual void DrawButton(CDC* pDC, CXTTabCtrlButton* pButton, COLORREF clrButton);

	virtual COLORREF FillButtons(CDC* pDC, CXTTabCtrlButtons* pButtons);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw the icon on the tab.
	// Parameters:
	//      pDC      - A pointer to a valid device context.
	//      pTabCtrl - A pointer to a CTabCtrl object.
	//      rcItem   - A CRect object that contains the location and the dimensions of the tab.
	//      iItem    - An int that represents the tab item to draw the icon on.
	// See Also:
	//      CXTTabBaseTheme::FillTabFace, CXTTabBaseThemeOffice2003::FillTabFace,
	//      CXTTabBaseTheme::DrawTabIcon, FillTabFace,
	//      CXTTabBaseTheme::FillTabFaceNativeWinXP, CXTExcelTabCtrlTheme::DrawTab
	//-----------------------------------------------------------------------
	virtual void DrawTabIcon(CDC* pDC, CTabCtrl* pTabCtrl, CRect& rcItem, int iItem);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw the borders around the tab control.
	// Parameters:
	//      pDC      - A pointer to a valid device context.
	//      rcClient - A CRect object that contains the location and the dimensions of the Tab Control.
	// See Also:
	//      CXTTabBaseTheme::DrawBorders
	//-----------------------------------------------------------------------
	virtual void DrawBorders(CDC* pDC, const CRect& rcClient);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw the header for the Tab Control.
	// Parameters:
	//      pDC          - A pointer to a valid device context.
	//      pTabCtrlBase - A pointer to a CXTTabBase object.  Contains information
	//                     about the tab control.
	//      rcHeader     - A CRect object that contains the location and the dimensions of the header.
	// See Also:
	//      CXTTabBaseTheme::FillHeader, CXTTabBaseThemeOffice2003::FillHeader
	//-----------------------------------------------------------------------
	virtual void FillHeader(CDC* pDC, CXTTabBase* pTabCtrlBase, CRect rcHeader);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called draw the tab face.
	// Parameters:
	//      pDC       - A pointer to a valid device context.
	//      pTabCtrl  - A pointer to a CTabCtrl object.
	//      rcItem    - A CRect object that contains the location and the dimensions of the tab.
	//      bSelected - TRUE if the tab is selected.  FALSE otherwise.
	// See Also:
	//      CXTTabBaseTheme::FillTabFace, CXTTabBaseThemeOffice2003::FillTabFace,
	//      CXTTabBaseThemeOffice2003::FillTabFace, DrawTabIcon,
	//      CXTExcelTabCtrlTheme::DrawTab, CXTTabBaseTheme::FillTabFaceNativeWinXP
	//-----------------------------------------------------------------------
	virtual void FillTabFace(CDC* pDC, CTabCtrl* pTabCtrl, CRect rcItem, BOOL bSelected);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw a border around a Tab child window.
	// Parameters:
	//      pDC - A pointer to a valid device context.
	//      r   - A CRect object that contains the location and the dimensions of the child window.
	// See Also:
	//      CXTTabBaseTheme::DrawBorder, CXTTabBaseThemeOffice2003::DrawBorder
	//-----------------------------------------------------------------------
	virtual void DrawBorder(CDC* pDC, const CRect& r);
};

//===========================================================================
// Summary:
//     Class CXTTabBaseThemeOffice2003 is derived from CXTTabBaseThemeOfficeXP.
//===========================================================================
class _XTP_EXT_CLASS CXTTabBaseThemeOffice2003 : public CXTTabBaseThemeOfficeXP
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Construct a CXTTabBaseThemeOffice2003 object.
	//-------------------------------------------------------------------------
	CXTTabBaseThemeOffice2003();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     This function calls the base class RefreshMetrics and then
	//     calls DeleteObject() for the CPen member variables m_penBlack,
	//     m_penWhite, m_penFace, and m_penText.  Then the CPen member variables
	//     are created with the system default colors for COLOR_BTNTEXT,
	//     COLOR_3DHILIGHT, COLOR_3DFACE, and COLOR_3DSHADOW.  Finally, the
	//     member variable m_iEdge is set to the SM_CYEDGE system value.
	//-------------------------------------------------------------------------
	void RefreshMetrics();
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to get the background for the theme.
	// Returns:
	//      A CXTPPaintManagerColor object that contains the value for the background color.
	// See Also:
	//     CXTTabBaseTheme::GetBackColor
	//-----------------------------------------------------------------------
	virtual CXTPPaintManagerColor GetBackColor() const;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw the header for the Tab Control.
	// Parameters:
	//      pDC          - A pointer to a valid device context.
	//      pTabCtrlBase - A pointer to a CXTTabBase object.  Contains information
	//                     about the tab control.
	//      rcHeader     - A CRect object that contains the location and the dimensions of the header.
	// See Also:
	//      CXTTabBaseTheme::FillHeader, CXTTabBaseThemeOfficeXP::FillHeader
	//-----------------------------------------------------------------------
	virtual void FillHeader(CDC* pDC, CXTTabBase* pTabCtrlBase, CRect rcHeader);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw the tab face with a gradient fill.
	// Parameters:
	//      pDC       - A pointer to a valid device context.
	//      pTabCtrl  - A pointer to a CTabCtrl object.
	//      rcItem    - A CRect object that contains the location and the dimensions of the tab.
	//      bSelected - TRUE if the tab is selected. FALSE otherwise.
	// Returns:
	//      An RBG color that represents the dark color of the gradient fill for the tab.
	// See Also:
	//      CXTTabBaseThemeOfficeXP::FillTabFace, CXTTabBaseThemeOffice2003::FillTabFace,
	//      FillTabFace, DrawTab, DrawTabIcon, DrawTabText
	//-----------------------------------------------------------------------
	virtual COLORREF GradientFillTabFace(CDC* pDC, CTabCtrl* pTabCtrl, CRect rcItem, BOOL bSelected);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called draw the tab face.
	// Parameters:
	//      pDC       - A pointer to a valid device context.
	//      pTabCtrl  - A pointer to a CTabCtrl object.
	//      rcItem    - A CRect object that contains the location and the dimensions of the tab.
	//      bSelected - TRUE if the tab is selected.  FALSE otherwise.
	// See Also:
	//      CXTTabBaseTheme::FillTabFace, CXTTabBaseThemeOfficeXP::FillTabFace,
	//      GradientFillTabFace
	//-----------------------------------------------------------------------
	virtual void FillTabFace(CDC* pDC, CTabCtrl* pTabCtrl, CRect rcItem, BOOL bSelected);
	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called to draw a border around a Tab child window.
	// Parameters:
	//      pDC - A pointer to a valid device context.
	//      r   - A CRect object that contains the location and the dimensions of the child window.
	// See Also:
	//      CXTTabBaseTheme::DrawBorder, CXTTabBaseThemeOfficeXP::DrawBorder
	//-----------------------------------------------------------------------
	virtual void DrawBorder(CDC* pDC, const CRect& r);

	virtual COLORREF FillButtons(CDC* pDC, CXTTabCtrlButtons* pButtons);

protected:
	CXTPPaintManagerColorGradient m_clrHeaderFace; // Represents the gradient color of the header face.
	CXTPPaintManagerColor m_clrShadow;             // Represents the color used for shadows.
	CXTPPaintManagerColor m_clrHighlight;          // Represents the highlight color.
	CXTPPaintManagerColor m_clrDarkShadow;         // Represents the color used for dark shadows.
	CXTPPaintManagerColor m_clrButtonSelected;     // Represents the color used for selected buttons.
	CXTPPaintManagerColor m_clrButtonNormal;       // Represents the color used for normal buttons.
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTTABCTRLTHEME_H__)
