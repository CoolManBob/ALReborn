// XTPReportHyperlink.h: interface for the CXTPReportHyperlink class.
//
// This file is a part of the XTREME REPORTCONTROL MFC class library.
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
#if !defined(__XTPREPORTRECORDHYPERLINK_H__)
#define __XTPREPORTRECORDHYPERLINK_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPSmartPtrInternalT.h"
#include "XTPReportDefines.h"

class CXTPPropExchange;

#pragma warning(disable: 4097)

//===========================================================================
// Summary:
//     This class represents hyperlinks. It stores first position and length
//     in the text for hyperlink and rectangle for drawing and
//     identify clicks on hyperlink. To create an instance of its class, you
//     simply call constructor with first position and length of the
//     hyperlink's text
//===========================================================================
class _XTP_EXT_CLASS CXTPReportHyperlink : public CXTPHeapObjectT<CCmdTarget, CXTPReportDataAllocator>
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPReportHyperlink)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default CXTPReportHyperlink constructor.
	// Parameters:
	//     nHyperTextBegin - Begin position of the hyperlink item text in the parent item caption.
	//     nHyperTextLen   - Hyperlink item text length.
	//-----------------------------------------------------------------------
	CXTPReportHyperlink(int nHyperTextBegin = 0, int nHyperTextLen = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys CXTPReportHyperlink object. Performs cleanup operations.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportHyperlink();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load a report Hyperlink
	//     using the specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

public:
	CRect       m_rcHyperSpot;      // Hyperlink draw place.
	int         m_nHyperTextBegin;  // Start position of the hyperlink in the item text.
	int         m_nHyperTextLen;    // The length of hyperlink text.

protected:
};


//===========================================================================
// Summary:
//      This typedef used to declare a base class for CXTPReportHyperlinks.
//===========================================================================
typedef CXTPArrayT<CXTPReportHyperlink*, CXTPReportHyperlink*, LPDISPATCH> CXTPReportHyperlinks_base;

//===========================================================================
// Summary:
//      This class represents array of pointers to CXTPReportHyperlink objects.
//      InternalRelease is called for each item in the array destructor.
//      Also serialization for array items is provided (see DoPropExchange method).
//===========================================================================
class _XTP_EXT_CLASS CXTPReportHyperlinks : public CXTPHeapObjectT<CXTPReportHyperlinks_base, CXTPReportDataAllocator>

{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPReportHyperlinks)
	typedef CXTPArrayT<CXTPReportHyperlink*, CXTPReportHyperlink*, LPDISPATCH> TBase;
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default CXTPReportHyperlink constructor.
	//-----------------------------------------------------------------------
	CXTPReportHyperlinks();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default CXTPReportHyperlink destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportHyperlinks();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load a report Hyperlink items
	//     using the specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//      Remove hyperlink at the specified index.
	// Parameters:
	//      nIndex - A zero based index to remove item.
	// See Also:
	//      CXTPReportHyperlink, CXTPArrayT
	//-----------------------------------------------------------------------
	virtual void RemoveAt(int nIndex);

	//-------------------------------------------------------------------------
	// Summary:
	//      Remove all items from the collection.
	// See Also:
	//      RemoveAt, CXTPReportHyperlink, CXTPArrayT
	//-------------------------------------------------------------------------
	virtual void RemoveAll();

	//-------------------------------------------------------------------------
	// Summary:
	//      This method copy all members from specified source object.
	// Parameters:
	//      pSrc - pointer to a source object.
	//-------------------------------------------------------------------------
	virtual void CopyFrom(CXTPReportHyperlinks* pSrc);

protected:

};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPREPORTRECORDHYPERLINK_H__)
