// XTPSyntaxEditColorSampleText.h : header file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSYNTAXEDITPROPERTIESSAMPLETEXT_H__)
#define __XTPSYNTAXEDITPROPERTIESSAMPLETEXT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPColorManager.h"

//===========================================================================
// Summary:
//     This class, derived from CStatic, used to display sample of colored text.
//===========================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditColorSampleText : public CStatic
{
public:
	// -------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	// -------------------------------------------------------------------
	CXTPSyntaxEditColorSampleText();

	// -------------------------------------------------------------------
	// Summary:
	//      Default object destructor.
	// -------------------------------------------------------------------
	virtual ~CXTPSyntaxEditColorSampleText();

public:
	// -------------------------------------------------------------------
	// Summary:
	//     Set text color.
	// Parameters:
	//     crText - A color value.
	// -------------------------------------------------------------------
	void SetTextColor(COLORREF crText);

	// -------------------------------------------------------------------
	// Summary:
	//     Get text color.
	// Returns:
	//     A color value.
	// -------------------------------------------------------------------
	COLORREF GetTextColor() const;

	// -------------------------------------------------------------------
	// Summary:
	//     Set background color
	// Parameters:
	//     crBack - A color value.
	// -------------------------------------------------------------------
	void SetBackColor(COLORREF crBack);

	// -------------------------------------------------------------------
	// Summary:
	//     Get background color.
	// Returns:
	//     A color value.
	// -------------------------------------------------------------------
	COLORREF GetBackColor() const;

	// -------------------------------------------------------------------
	// Summary:
	//     Set border color.
	// Parameters:
	//     crBorder - A color value.
	// -------------------------------------------------------------------
	void SetBorderColor(COLORREF crBorder);

	// -------------------------------------------------------------------
	// Summary:
	//     Get border color.
	// Returns:
	//     A color value.
	// -------------------------------------------------------------------
	COLORREF GetBorderColor() const;

	// -------------------------------------------------------------------
	// Summary:
	//     Redraw control.
	// -------------------------------------------------------------------
	void Refresh();

	//{{AFX_VIRTUAL(CXTPSyntaxEditColorSampleText)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPSyntaxEditColorSampleText)
	//{{AFX_CODEJOCK_PRIVATE
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_CODEJOCK_PRIVATE
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CXTPPaintManagerColor m_crBack;
	CXTPPaintManagerColor m_crText;
	CXTPPaintManagerColor m_crBorder;
};

#endif // !defined(__XTPSYNTAXEDITPROPERTIESSAMPLETEXT_H__)
