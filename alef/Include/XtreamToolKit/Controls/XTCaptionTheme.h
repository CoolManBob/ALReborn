// XTCaptionTheme.h: interface for the CXTCaptionTheme class.
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
#if !defined(__XTCAPTIONTHEME_H__)
#define __XTCAPTIONTHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTCaption;
class CXTButtonTheme;

#include "Controls/XTButtonTheme.h"

DECLARE_THEME_FACTORY(CXTCaptionTheme)

//===========================================================================
// Summary:
//     CXTCaptionTheme is a CXTThemeManagerStyle derived class. It
//     is used to control the caption theme. It is used to control the
//     theme of captions. Derive custom themes from CXTCaptionTheme.
//===========================================================================
class _XTP_EXT_CLASS CXTCaptionTheme : public CXTThemeManagerStyle
{
public:

	// -----------------------------------------------
	// Summary:
	//     Constructs a CXTCaptionTheme object.
	// -----------------------------------------------
	CXTCaptionTheme();

	// ----------------------------------------------------------------
	// Summary:
	//     This function is called to set the edges of a caption to the
	//     system default colors for edge shadow and edge light.
	// ----------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to draw the background of the caption at the
	//     location specified in the rcItem rect.
	// Parameters:
	//     pDC      - A CDC pointer that represents the current device
	//                context.
	//     pCaption - A pointer to a CXTCaption object.
	//     rcItem   - Location where the caption is drawn.
	//-----------------------------------------------------------------------
	virtual void DrawCaptionBack(CDC* pDC, CXTCaption* pCaption, CRect& rcItem);

	// ----------------------------------------------------------------
	// Summary:
	//     Called by CXTCaption to draw the caption text.
	// Parameters:
	//     pDC      -  A CDC pointer that represents the current device
	//                 context.
	//     pCaption -  A pointer to a CXTCaption object.
	// ----------------------------------------------------------------
	virtual void DrawCaptionText(CDC* pDC, CXTCaption* pCaption);

	// ----------------------------------------------------------------------
	// Summary:
	//     Called by CXTCaption to draw the caption icon.
	// Parameters:
	//     pDC      -  A CDC pointer that represents the current device
	//                 context.
	//     pCaption -  A pointer to a CXTCaption object.
	//     rcItem   -  Location where the caption is drawn.
	// Remarks:
	//     Call this function to draw the icon on the caption. Only draws the
	//     icon if there is enough space between the caption borders.
	// ----------------------------------------------------------------------
	virtual void DrawCaptionIcon(CDC* pDC, CXTCaption* pCaption, CRect& rcItem);

public:
	CXTPPaintManagerColor m_clrEdgeShadow; // The color of the edge shadow for the current theme.
	CXTPPaintManagerColor m_clrEdgeLight;  // The color of the edge light for the current theme.

};

//===========================================================================
// Summary:
//     CXTCaptionThemeOfficeXP is a CXTThemeManagerStyle derived class. It
//     is used to control the caption theme. It is used to control the
//     theme of captions. Derive custom themes from CXTCaptionTheme.
//===========================================================================
class _XTP_EXT_CLASS CXTCaptionThemeOfficeXP : public CXTCaptionTheme
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to draw the background of the caption.
	// Parameters:
	//     pDC      - A CDC pointer that represents the current device
	//                context.
	//     pCaption - A pointer to a CXTCaption object.
	//     rcItem   - CRect object with the size and location where the caption is drawn.
	//-----------------------------------------------------------------------
	virtual void DrawCaptionBack(CDC* pDC, CXTCaption* pCaption, CRect& rcItem);
};

//===========================================================================
// Summary:
//     CXTCaptionThemeOffice2003 is derived from CXTCaptionTheme.
//     It is used to set the caption to the Office 2003 theme.
//===========================================================================
class _XTP_EXT_CLASS CXTCaptionThemeOffice2003 : public CXTCaptionTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary: Default class constructor.  Call this member function to
	//          construct a CXTCaptionThemeOffice2003 object.
	//-----------------------------------------------------------------------
	CXTCaptionThemeOffice2003();
//{{AFX_CODEJOCK_PRIVATE
	CXTCaptionThemeOffice2003(BOOL bPrimaryCaption); // deprecated.
//}}AFX_CODEJOCK_PRIVATE
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to draw the background of the caption.
	// Parameters:
	//     pDC      - A CDC pointer that represents the current device
	//                context.
	//     pCaption - A pointer to a CXTCaption object.
	//     rcItem   - CRect object with the size and location where the caption is drawn.
	//-----------------------------------------------------------------------
	virtual void DrawCaptionBack(CDC* pDC, CXTCaption* pCaption, CRect& rcItem);
};

DECLARE_THEME_FACTORY(CXTCaptionButtonTheme)

//===========================================================================
// Summary: CXTCaptionButtonTheme is derived from CXTButtonTheme.  This class
//          is used to set the caption for buttons.
//===========================================================================
class _XTP_EXT_CLASS CXTCaptionButtonTheme : public CXTButtonTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary: Default class constructor.
	//-----------------------------------------------------------------------
	CXTCaptionButtonTheme();

protected:
	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the button caption text.
	// Parameters:
	//       pDC     - A pointer to a valid device context.
	//       nState  - A UINT value that represents the current state of the button.
	//       rcItem  - a CRect object containing the dimensions of the button.
	//       pButton - A pointer to a valid CXTButton object.
	//-----------------------------------------------------------------------
	virtual void DrawButtonText(CDC* pDC, UINT nState, CRect& rcItem, CXTButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the text color of the button caption.
	// Parameters:
	//      nState  - A UINT that contains the current button state.
	//      pButton - A pointer to a valid CXTButton object.
	// Returns:
	//      An RGB value specifying the text color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetTextColor(UINT nState, CXTButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw the background for the
	//     button.
	// Parameters:
	//     lpDIS   - A long pointer to a DRAWITEMSTRUCT structure. The
	//               structure contains information about the item to
	//               be drawn and the type of drawing required.
	//     pButton - Points to a CXTButton object.
	// Returns:
	//     TRUE if the background was drawn successfully, otherwise returns
	//     FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL DrawButtonThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton);
};

//===========================================================================
// Summary: Class CXTCaptionButtonThemeOfficeXP is a CXTButtonThemeOfficeXP
//          derived class.  This class is used to create a button caption that
//          resembles an Office XP button caption.
//===========================================================================
class _XTP_EXT_CLASS CXTCaptionButtonThemeOfficeXP : public CXTButtonThemeOfficeXP
{
public:
	//-----------------------------------------------------------------------
	// Summary: Default class constructor.  Call this member function to
	//          construct a CXTCaptionButtonThemeOfficeXP object.
	//-----------------------------------------------------------------------
	CXTCaptionButtonThemeOfficeXP();

protected:
	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the button caption text.
	// Parameters:
	//       pDC     - A pointer to a valid device context.
	//       nState  - A UINT value that represents the current state of the button.
	//       rcItem  - a CRect object containing the dimensions of the button.
	//       pButton - A pointer to a valid CXTButton object.
	//-----------------------------------------------------------------------
	virtual void DrawButtonText(CDC* pDC, UINT nState, CRect& rcItem, CXTButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the background of the caption.
	// Parameters:
	//     lpDIS   - An LPDRAWITEMSTRUCT struct that contains the information the
	//               owner window must have to determine how to paint an owner-drawn
	//               control.
	//     pButton - A pointer to valid CXTButton object.
	// Returns:
	//     A TRUE if the background was successfully drawn.  FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL DrawButtonThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the text color of the button caption.
	// Parameters:
	//      nState  - A UINT that contains the current button state.
	//      pButton - A pointer to a valid CXTButton object.
	// Returns:
	//      An RGB value specifying the text color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetTextColor(UINT nState, CXTButton* pButton);
};

//===========================================================================
// Summary: Class CXTCaptionButtonThemeOffice2003 is a CXTButtonThemeOffice2003
// derived class.  This class is used to create a button caption that resembles
// an Office 2003 button caption.
//===========================================================================
class _XTP_EXT_CLASS CXTCaptionButtonThemeOffice2003 : public CXTButtonThemeOffice2003
{
protected:
	//-----------------------------------------------------------------------
	// Summary: Call this member function to draw the background of the caption.
	// Parameters:
	//     lpDIS   - An LPDRAWITEMSTRUCT struct that contains the information the
	//               owner window must have to determine how to paint an owner-drawn
	//               control.
	//     pButton - A pointer to valid CXTButton object.
	// Returns:
	//     A TRUE if the background was successfully drawn.  FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL DrawButtonThemeBackground(LPDRAWITEMSTRUCT lpDIS, CXTButton* pButton);

	//-----------------------------------------------------------------------
	// Summary: Call this member function to get the text color of the button caption.
	// Parameters:
	//      nState  - A UINT that contains the current button state.
	//      pButton - A pointer to a valid CXTButton object.
	// Returns:
	//      An RGB value specifying the text color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetTextColor(UINT nState, CXTButton* pButton);

	//-----------------------------------------------------------------------
	// Summary      : This member function is called to determine the correct
	//                gradient values for rendering the button's background.
	// Parameters   : rButton - Reference to the buttons window size.
	//                pCaption - Pointer to the buttons caption parent.
	//                clr - Reference to receive gradient color values.
	// Returns      : TRUE if the gradient colors were successfully determined,
	//                otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL GetGradientValues(CRect& rButton, CXTCaption* pCaption, CXTPPaintManagerColorGradient& clr);
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTCAPTIONTHEME_H__)
