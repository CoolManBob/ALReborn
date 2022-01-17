// XTFlatControlsTheme.h: interface for the CXTFlatComboBoxTheme class.
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
#if !defined(__XTFLATCONTROLSTHEME_H__)
#define __XTFLATCONTROLSTHEME_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPDrawHelpers.h"
#include "XTThemeManager.h"

class CXTFlatComboBox;
class CXTFlatEdit;

DECLARE_THEME_FACTORY(CXTFlatComboBoxTheme)

//===========================================================================
// Summary:
//     CXTFlatControlsTheme is used to draw the CXTTabCtrl object.  All themes
//     used for CXTTabCtrl should inherit from this base class.
//===========================================================================
class _XTP_EXT_CLASS CXTFlatComboBoxTheme : public CXTThemeManagerStyle
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFlatComboBoxTheme object.
	//-----------------------------------------------------------------------
	CXTFlatComboBoxTheme();

	//-------------------------------------------------------------------------
	// Summary:
	//     This function refreshes the colors on the flat control.
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to draw a Flat ComboBox.
	// Parameters:
	//     pDC    - A CDC pointer that represents the current device
	//              context.
	//     pCombo - A pointer to a CXTFlatComboBox object.  Represents a flat combo box.
	//     eState - An int that is used to specify the state of the flat combo box.
	//              Possible values are: NORMAL, RAISED, and PRESSED.
	//-----------------------------------------------------------------------
	virtual void DrawFlatComboBox(CDC* pDC, CXTFlatComboBox* pCombo, int eState);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function retrieve the dimensions, in pixels, of a
	//      3-D drop down arrow box.
	// Returns:
	//      A reference to a CSize object that contains 3-D border dimensions.
	//-----------------------------------------------------------------------
	CSize& GetThumbSize() {
		return m_sizeThumb;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function retrieve the dimensions, in pixels, of a
	//      3-D window border.
	// Returns:
	//      A reference to a CSize object that contains 3-D border dimensions.
	//-----------------------------------------------------------------------
	CSize& GetBorderSize() {
		return m_sizeBorder;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function retrieve the dimensions, in pixels, of a
	//      3-D window edge.
	// Returns:
	//      A reference to a CSize object that contains 3-D edge dimensions.
	//-----------------------------------------------------------------------
	CSize& GetEdgeSize() {
		return m_sizeEdge;
	}

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called by the Combo Box for drawing the
	//      down arrow.
	// Parameters:
	//      pDC     - Device context for drawing.
	//      rcArrow - A rectangle in which to draw.
	//      eState  - State of the Combo box
	//      pCombo  - Combo box to draw down arrow for
	//-----------------------------------------------------------------------
	virtual void DrawDownArrow(CDC* pDC, const CRect& rcArrow, int eState, CXTFlatComboBox* pCombo);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to get the width of the drop down arrow.
	// Returns:
	//      An int that contains the width of the drop down arrow.
	// See Also:
	//      DrawDownArrow, DrawArrowBack
	//-----------------------------------------------------------------------
	int GetDropArrowWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this function to draw the background for the arrow button.
	// Parameters:
	//      pDC     - A pointer to a valid device context.
	//      rcArrow - A CRect object that contains the location and the dimensions of the background.
	//      eState  - An int that contains the current state for the arrow button.
	//      pCombo  - A pointer to a valid CXTFlatComboBox object.
	// See Also:
	//      CXTFlatComboBoxThemeOfficeXP::DrawArrowBack, CXTFlatComboBoxThemeOffice2003::DrawArrowBack
	//-----------------------------------------------------------------------
	virtual void DrawArrowBack(CDC* pDC, CRect& rcArrow, int eState, CXTFlatComboBox* pCombo);

protected:
	int m_cxArrow;      // This member variable contains the width of the drop down arrow.
	CSize m_sizeThumb;  // Stores dimensions, in pixels, of a 3-D drop down arrow box.
	CSize m_sizeBorder; // Stores dimensions, in pixels, of a 3-D window border.
	CSize m_sizeEdge;   // Stores dimensions, in pixels, of a 3-D window edge.
};

//===========================================================================
// Summary:
//     CXTFlatComboBoxThemeOfficeXP class is derived from CXTFlatComboBoxTheme.
//     This class is used to create the Office 2000 Theme for flat controls.
//===========================================================================
class _XTP_EXT_CLASS CXTFlatComboBoxThemeOfficeXP : public CXTFlatComboBoxTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFlatComboBoxThemeOfficeXP object.
	//-----------------------------------------------------------------------
	CXTFlatComboBoxThemeOfficeXP();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to draw a rectangle around a ComboBox.
	// Parameters:
	//     pDC    - A CDC pointer that represents the current device
	//              context.
	//     pCombo - A pointer to a CXTFlatComboBox object.  Represents a flat combo box.
	//     eState - An int that is used to specify the state of the flat combo box.
	//              Possible values are: NORMAL, RAISED, and PRESSED.
	//-----------------------------------------------------------------------
	virtual void DrawFlatComboBox(CDC* pDC, CXTFlatComboBox* pCombo, int eState);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Call this function to draw the background for the arrow button.
	// Parameters:
	//      pDC     - A pointer to a valid device context.
	//      rcArrow - A CRect object that contains the location and the dimensions of the background.
	//      eState  - An int that contains the current state for the arrow button.
	//      pCombo  - A pointer to a valid CXTFlatComboBox object.
	// See Also:
	//      CXTFlatComboBoxTheme::DrawArrowBack, CXTFlatComboBoxThemeOffice2003::DrawArrowBack
	//-----------------------------------------------------------------------
	virtual void DrawArrowBack(CDC* pDC, CRect& rcArrow, int eState, CXTFlatComboBox* pCombo);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is called by the Combo Box for drawing the
	//      down arrow.
	// Parameters:
	//      pDC     - Device context for drawing.
	//      rcArrow - A rectangle in which to draw.
	//      eState  - State of the Combo box
	//      pCombo  - Combo box to draw down arrow for
	//-----------------------------------------------------------------------
	virtual void DrawDownArrow(CDC* pDC, const CRect& rcArrow, int eState, CXTFlatComboBox* pCombo);
};

//===========================================================================
// Summary:
//     CXTFlatComboBoxThemeOffice2003 class is derived from CXTFlatComboBoxThemeOfficeXP.
//     This class is used to create the Office 2003 Theme for flat controls.
//===========================================================================
class _XTP_EXT_CLASS CXTFlatComboBoxThemeOffice2003 : public CXTFlatComboBoxThemeOfficeXP
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFlatComboBoxThemeOffice2003 object.
	//-----------------------------------------------------------------------
	CXTFlatComboBoxThemeOffice2003();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to draw a rectangle around a ComboBox.
	// Parameters:
	//     pDC    - A CDC pointer that represents the current device
	//              context.
	//     pCombo - A pointer to a CXTFlatComboBox object.  Represents a flat combo box.
	//     eState - An int that is used to specify the state of the flat combo box.
	//              Possible values are: NORMAL, RAISED, and PRESSED.
	//-----------------------------------------------------------------------
	virtual void DrawFlatComboBox(CDC* pDC, CXTFlatComboBox* pCombo, int eState);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//      Call this function to draw the background for the arrow button.
	// Parameters:
	//      pDC     - A pointer to a valid device context.
	//      rcArrow - A CRect object that contains the location and the dimensions of the background.
	//      eState  - An int that contains the current state for the arrow button.
	//      pCombo  - A pointer to a valid CXTFlatComboBox object.
	// See Also:
	//      CXTFlatComboBoxTheme::DrawArrowBack, CXTFlatComboBoxThemeOfficeXP::DrawArrowBack
	//-----------------------------------------------------------------------
	virtual void DrawArrowBack(CDC* pDC, CRect& rcArrow, int eState, CXTFlatComboBox* pCombo);
};

DECLARE_THEME_FACTORY(CXTFlatEditTheme)

//===========================================================================
// Summary:
//     CXTFlatControlsTheme is used to draw the CXTTabCtrl object.  All themes
//     used for CXTTabCtrl should inherit from this base class.
//===========================================================================
class _XTP_EXT_CLASS CXTFlatEditTheme : public CXTThemeManagerStyle
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFlatEditTheme object.
	//-----------------------------------------------------------------------
	CXTFlatEditTheme();

	//-------------------------------------------------------------------------
	// Summary:
	//     This function refreshes the colors on the flat control.
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to draw a rectangle around an EditBox.
	// Parameters:
	//     pDC    - A CDC pointer that represents the current device
	//              context.
	//     pEdit  - A pointer to an edit box control.
	//     rWindow - Area to render.
	//     eState - An int that is used to specify the state of the flat edit box.
	//-----------------------------------------------------------------------
	virtual void DrawBorders(CDC* pDC, CXTFlatEdit* pEdit, const CRect& rWindow, int eState);

protected:

	friend class CXTThemeManager;  // Make the CXTThemeManager a friend of the CXTFlatEditTheme class.
};

//===========================================================================
// Summary:
//     CXTFlatEditThemeOfficeXP class is derived from CXTFlatEditTheme.
//     This class is used to create the Office 2000 Theme for flat controls.
//===========================================================================
class CXTFlatEditThemeOfficeXP : public CXTFlatEditTheme
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFlatEditThemeOfficeXP object.
	//-----------------------------------------------------------------------
	CXTFlatEditThemeOfficeXP();

	//-----------------------------------------------------------------------
	// Summary:
	//     This function is called to refresh the colors of the
	//     flat controls Office XP theme.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to render the flat edit control using
	//     the specified theme.
	// Parameters:
	//     pDC    - A CDC pointer that represents the current device
	//              context.
	//     pEdit  - A pointer to an edit box control.
	//     rWindow - Area to render.
	//     eState - An int that is used to specify the state of the flat edit box.
	//-----------------------------------------------------------------------
	virtual void DrawBorders(CDC* pDC, CXTFlatEdit* pEdit, const CRect& rWindow, int eState);

public:
	CXTPPaintManagerColor m_clrHighlightText;   // Used to specify the color of highlighted text.
	CXTPPaintManagerColor m_clrPushedText;      // Used to specify the color of pushed text.
	CXTPPaintManagerColor m_clrHighlight;       // Used to specify the color a highlighted object.
	CXTPPaintManagerColor m_clrPushed;          // Used to specify the color a pushed object.
	CXTPPaintManagerColor m_clrFrameNormal;     // Used to specify the color a normal frame.
	CXTPPaintManagerColor m_clrFrameHighlight;  // Used to specify the color a highlighted frame.
	CXTPPaintManagerColor m_clrFrameEdit;       // Used to specify the color an edit frame.
	CXTPPaintManagerColor m_clrNormal;          // Used to specify the color a normal object.
};

//===========================================================================
// Summary:
//     CXTFlatEditThemeOffice2003 class is derived from CXTFlatEditThemeOfficeXP.
//     This class is used to create the Office 2003 Theme for flat controls.
//===========================================================================
class CXTFlatEditThemeOffice2003 : public CXTFlatEditThemeOfficeXP
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTFlatEditThemeOffice2003 object.
	//-----------------------------------------------------------------------
	CXTFlatEditThemeOffice2003();

	//--------------------------------------------------------------------
	// Summary:
	//     This function is called to refresh the colors of
	//     the flat controls Office 2003 theme.
	//--------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to render the flat edit control using
	//     the specified theme.
	// Parameters:
	//     pDC    - A CDC pointer that represents the current device
	//              context.
	//     pEdit  - A pointer to an edit box control.
	//     rWindow - Area to render.
	//     eState - An int that is used to specify the state of the flat edit box.
	//-----------------------------------------------------------------------
	virtual void DrawBorders(CDC* pDC, CXTFlatEdit* pEdit, const CRect& rWindow, int eState);
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTFLATCONTROLSTHEME_H__)
