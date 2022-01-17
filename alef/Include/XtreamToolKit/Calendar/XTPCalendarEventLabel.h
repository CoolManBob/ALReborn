// XTPCalendarEventLabel.h: interface for the CXTPCalendarEventLabel and
// CXTPCalendarEventLabels classes.
//
// This file is a part of the XTREME CALENDAR MFC class library.
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
#if !defined(_XTPCALENDAREVENTLABEL_H__)
#define _XTPCALENDAREVENTLABEL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPCalendarPtrCollectionT.h"
/////////////////////////////////////////////////////////////////////////////
class CXTPCalendarEventLabel;
class CXTPCalendarEventLabels;


//===========================================================================
// Summary:
//     This class define Calendar Event label properties.
// See Also: CXTPCalendarEvent, CXTPCalendarEvent
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarEventLabel : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarEventLabel)
	//}}AFX_CODEJOCK_PRIVATE
public:
	int         m_nLabelID; // Label ID.
	COLORREF    m_clrColor; // Label color used to fill the event background.
	CString     m_strName;  // Label name.

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class constructor.
	// Parameters:
	//     nID      - Label ID.
	//     clrColor - Label color.
	//     pcszName - Label name.
	//-----------------------------------------------------------------------
	CXTPCalendarEventLabel(int nID = 0, COLORREF clrColor = 0, LPCTSTR pcszName = _T(""));

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles class member deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarEventLabel() {};

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set new object properties.
	// Parameters:
	//     rSrc - Reference to the source object.
	// Remarks:
	//     This is the overloaded assignment operator.
	// Returns:
	//     Reference to this class instance.
	//-----------------------------------------------------------------------
	const CXTPCalendarEventLabel& operator=(const CXTPCalendarEventLabel& rSrc)
	{
		m_nLabelID = rSrc.m_nLabelID;
		m_clrColor = rSrc.m_clrColor;
		m_strName = rSrc.m_strName;

		return *this;
	}
protected:

};
//===========================================================================

/////////////////////////////////////////////////////////////////////////////

//===========================================================================
// Summary:
//     This class is used as a collection of CXTPCalendarEventLabel
//     objects.
// See Also: CXTPCalendarEventLabel, CXTPCalendarEvent, CXTPCalendarEvent
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarEventLabels :
						public CXTPCalendarPtrCollectionT<CXTPCalendarEventLabel>
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarEventLabels)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default class constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarEventLabels();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles class members deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarEventLabels();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to fill the collection using
	//     a default label list.
	//-----------------------------------------------------------------------
	virtual void InitDefaultValues();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to search for the label with a
	//     specified ID.
	// Parameters:
	//     nLabelID - An int that contains the label ID.
	// Returns:
	//     A pointer to a CXTPCalendarEventLabel object or NULL.
	//-----------------------------------------------------------------------
	CXTPCalendarEventLabel* Find(int nLabelID) const;
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Finds label index in the internal array by its ID.
	// Parameters:
	//     nLabelID - Label ID
	// Returns:
	//     Label index in the internal label array.
	// See Also: Find
	//-----------------------------------------------------------------------
	int FindIndex(int nLabelID) const;

};

/////////////////////////////////////////////////////////////////////////////


#endif // !defined(_XTPCALENDAREVENTLABEL_H__)
