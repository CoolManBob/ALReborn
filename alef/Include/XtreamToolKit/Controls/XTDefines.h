// XTDefines.h interface for the CXTAuxData struct.
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
#if !defined(__XTDEFINES_H__)
#define __XTDEFINES_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTNotify.h"

// ----------------------------------------------------------------------
// Summary:
//     Enumeration to determine mouse state.
// Remarks:
//     XTMouseState type defines the constants used to determine the
//     state and position the mouse cursor is with relation to a
//     control's window. This is used to determine how the control should
//     be rendered.
// See Also:
//     CXTFlatEdit::DrawEdit, CXTFlatComboBox::DrawCombo,
//     CXTOutBarCtrl::DrawFolder
//
// <KEYWORDS xtMouseNormal, xtMouseHover, xtMouseSelect>
// ----------------------------------------------------------------------
enum XTMouseState
{
	xtMouseNormal = 1, // The mouse cursor is not hovering.
	xtMouseHover  = 2, // The mouse cursor is hovering the control.
	xtMouseSelect = 3  // The mouse cursor is hovering the control and the left button is pressed.
};

/////////////////////////////////////////////////////////////////////////////
// Input:   bHorz - TRUE if horizontally oriented, FALSE otherwise.
//          pRBBI - A pointer to a BARBANDINFO structure, the window can modify its parameters as appropriate.
//
// Returns: If the application is to process this message, the return value should be TRUE.
//
// Remarks: When a window is added to a rebar, the XTWM_ADDREBAR message is sent to the window.
//
// Example: Here is an example of how an application would process the XTWM_ADDREBAR
//          message.
//
//          <pre>BEGIN_MESSAGE_MAP(CWidget, CWnd)
//              //{{AFX_MSG_MAP(CWidget)
//              ON_MESSAGE(XTWM_ADDREBAR, OnAddReBar)
//              //}}AFX_MSG_MAP
//          END_MESSAGE_MAP()</pre>
//
//          <pre>LRESULT CWidget::OnAddReBar(WPARAM wParam, LPARAM lParam)
//          {
//              BOOL bHorz = (BOOL) wParam;
//
//              REBARBANDINFO* pRBBI = (REBARBANDINFO*) lParam;
//                  // TODO: Handle message.
//
//              return TRUE;
//          }</pre>
// Summary: The XTWM_ADDREBAR message is sent to a window whenever it is added
//          to a CXTReBar control.
//
//          <pre>XTWM_ADDREBAR
//          bHorz = (BOOL) wParam;            // TRUE if horizontal.
//          pRBBI = (REBARBANDINFO*) lParam;  // Points to a REBARBANDINFO structure.</pre>
const UINT XTWM_ADDREBAR = (WM_XTP_CONTROLS_BASE + 30);

/////////////////////////////////////////////////////////////////////////////
// Input:   pNMTB - Value of wParam specifies a NMTOOLBAR structure that represents the toolbar
//          Button where the dropdown event occurred.
//
//          pRect - Value of lParam points to a CRect object that represents the size and location
//          of the toolbar button where the dropdown event occurred.
//
// Returns: Dropdown is displaying a modal window, the return value should be 0, otherwise
//          return 1 for menu style displays.
//
// Remarks: When the user clicks on a dropdown arrow for a toolbar button, the CBRN_XT_DROPDOWN
//          message is sent to the toolbar's owner window.
//
// Example: Here is an example of how an application would process the CBRN_XT_DROPDOWN
//          message.
//
//          <pre>BEGIN_MESSAGE_MAP(CMainFrame, CXTMDIFrameWnd)
//              //{{AFX_MSG_MAP(CMainFrame)
//              ON_MESSAGE(CBRN_XT_DROPDOWN, OnDropDown)
//              //}}AFX_MSG_MAP
//          END_MESSAGE_MAP()</pre>
//
//          <pre>LRESULT CMainFrame::OnDropDown(WPARAM wParam, LPARAM lParam)
//          {
//              NMTOOLBAR* pNMTB = (NMTOOLBAR*)wParam;
//              ASSERT(pNMTB != NULL);
//
//              CRect* pRect = (CRect*)lParam;
//              ASSERT(pRect != NULL);
//
//              // TODO: Handle toolbar dropdown notification (click on REDO drop arrow).
//
//              return 0;
//          }</pre>
// Summary: The CBRN_XT_DROPDOWN message is sent to the CXTToolBar window
//          whenever a dropdown event occurs in the CXTToolBar.
//
//          <pre>CBRN_XT_DROPDOWN
//          pNMTB = (NMTOOLBAR*)wParam;  // pointer to a NMTOOLBAR struct
//          pRect = (CRect*)lParam;      // pointer to a CRect object</pre>
const UINT CBRN_XT_DROPDOWN = (WM_XTP_CONTROLS_BASE + 31);
/////////////////////////////////////////////////////////////////////////////
// Input:   bHorz - TRUE if horizontally oriented, FALSE otherwise.
//          pRBSCI - A pointer to an XT_REBARSIZECHILDINFO structure, the window can modify its parameters as appropriate.
//
// Returns: If the application is to process this message, the return value should be TRUE.
//
// Remarks: When a window is added to a rebar, the XTWM_REBAR_SIZECHILD message is sent to the window.
//
// Example: Here is an example of how an application would process the XTWM_REBAR_SIZECHILD
//          message.
//
//          <pre>BEGIN_MESSAGE_MAP(CWidget, CWnd)
//              //{{AFX_MSG_MAP(CWidget)
//              ON_MESSAGE(XTWM_REBAR_SIZECHILD, OnReBarSizeChild)
//              //}}AFX_MSG_MAP
//          END_MESSAGE_MAP()</pre>
//
//          <pre>LRESULT CWidget::OnReBarSizeChild(WPARAM wParam, LPARAM lParam)
//          {
//              BOOL bHorz = (BOOL) wParam;
//
//              XT_REBARSIZECHILDINFO* pRBSCI = (XT_REBARSIZECHILDINFO*) lParam;
//                  // TODO: Handle message.
//
//              return TRUE;
//          }</pre>
// Summary: The XTWM_REBAR_SIZECHILD message is sent when CXTReBar control has resized
//          the band in which the recipient resides.
//
//          <pre>XTWM_REBAR_SIZECHILD
//          bHorz = (BOOL) wParam; // TRUE if horizontal.
//          pRBSCI = (XT_REBARSIZECHILDINFO*) lParam;  // Points to an XT_REBARSIZECHILDINFO structure.</pre>
const UINT XTWM_REBAR_SIZECHILD = (WM_XTP_CONTROLS_BASE + 32);

/////////////////////////////////////////////////////////////////////////////
// Returns: TRUE if the control bar shall be fully visualized, FALSE if just a wire frame must be rendered.
//
// Remarks: When a control bar is dragged the XTWM_QUERYVISUALIZE message is sent to the control bar.
//
// Example: Here is an example of how an application would process the XTWM_QUERYVISUALIZE
//          message.
//
//          <pre>BEGIN_MESSAGE_MAP(CWidget, CXTControlBar)
//              //{{AFX_MSG_MAP(CWidget)
//              ON_MESSAGE(XTWM_QUERYVISUALIZE, OnQueryVisualize)
//              //}}AFX_MSG_MAP
//          END_MESSAGE_MAP()</pre>
//
//          <pre>LRESULT CWidget::OnQueryVisualize(WPARAM wParam, LPARAM lParam)
//          {
//              UNREFERENCED_PARAMETER(wParam);
//              UNREFERENCED_PARAMETER(lParam);
//              // Allow full visualizations.
//              return TRUE;
//          }</pre>
// Summary: CXTDockContext sends the XTWM_QUERYVISUALIZE message to a control bar to
//          check if the control bar shall be visualized while dragging or just a wire
//          frame shall be rendered.
//
//          <pre>XTWM_QUERYVISUALIZE
//          wParam - not used, must be zero
//          lParam - not used, must be zero</pre>
const UINT XTWM_QUERYVISUALIZE = (WM_XTP_CONTROLS_BASE + 33);

/////////////////////////////////////////////////////////////////////////////
// Returns: TRUE if control bar has a hot spot, FALSE if it does not.
//
// Remarks: CXTDockContext sends the XTWM_QUERYHOTSPOT message the control bar to check
//          for a hot spot.
//
// Example: Here is an example of how an application would process the XTWM_QUERYHOTSPOT
//          message.
//
//          <pre>BEGIN_MESSAGE_MAP(CWidget, CXTControlBar)
//              //{{AFX_MSG_MAP(CWidget)
//              ON_MESSAGE(XTWM_QUERYHOTSPOT, OnQueryHotSpot)
//              //}}AFX_MSG_MAP
//          END_MESSAGE_MAP()</pre>
//
//          <pre>LRESULT CWidget::OnQueryHotSpot(WPARAM wParam, LPARAM lParam)
//          {
//              UNREFERENCED_PARAMETER(wParam);
//              LPSIZE lpSize = (LPSIZE) lParam;  // Points to a LPSIZE structure.
//              // TODO: Handle message.
//              return TRUE;
//          }</pre>
// Summary: CXTDockContext sends the XTWM_QUERYHOTSPOT message to a control bar to
//          check if the control bar has a hot spot, a point that must match the position
//          of the cursor.
//
//          <pre>XTWM_QUERYHOTSPOT
//          wParam - not used, must be zero
//          lpSize = (LPSIZE) lParam;  // IN  - Extent of the rectangle in which
//                                     //       hot spot is to be defined
//                                     // OUT - Offset of the hot spot location
//                                     //       within the rectangle.</pre>
const UINT XTWM_QUERYHOTSPOT = (WM_XTP_CONTROLS_BASE + 34);

// used internally - notifies rebar control that the ideal size of the
// embedded control has changed.
// wParam - control's handle, HWND
// lParam - new ideal size, UINT
// Return value is ignored.

const UINT XTWM_IDEALSIZECHANGED = (WM_XTP_CONTROLS_BASE + 36);

#ifndef BTNS_WHOLEDROPDOWN
#define BTNS_WHOLEDROPDOWN              0x80  /* draw dropdown arrow, but without split arrow section */
#endif
// Summary: The I_IMAGENONE constant is used when defining text-only toolbar buttons with no
//          space allocated to the button icon
//          The constant is normally defined in version 5.81 and greater SDK headers
#ifndef I_IMAGENONE
#define I_IMAGENONE (-2)
#endif//I_IMAGENONE

#ifndef IDC_HAND
#define IDC_HAND            MAKEINTRESOURCE(32649)
#endif

//////////////////////////////////////////////////////////////////////

#endif // #if !defined(__XTDEFINES_H__)
