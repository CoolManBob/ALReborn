// XTPReportRecordItemText.h: interface for the CXTPReportRecordItemText class.
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
#if !defined(__XTPREPORTRECORDITEMTEXT_H__)
#define __XTPREPORTRECORDITEMTEXT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPReportRecordItem.h"

//===========================================================================
// Summary:
//     This Class represents a text-only cell and inherits basic functionality
//     from CXTPReportRecordItem class.
//     You create a text record item simply by calling a constructor with one
//     parameter - text string
// See Also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemText : public CXTPReportRecordItem
{
	DECLARE_SERIAL(CXTPReportRecordItemText)
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a record text item.
	// Parameters:
	//     szText - A text value for the item.
	//-----------------------------------------------------------------------
	CXTPReportRecordItemText(LPCTSTR szText = _T(""));

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a text caption of this item.
	// Parameters:
	//     pColumn - Corresponded column of the item.
	// Remarks:
	//     Overwrites parents' member function
	// Returns:
	//     Text caption of the item.
	//-----------------------------------------------------------------------
	CString GetCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets the text caption for the item.
	// Parameters:
	//     szText - Text caption for the item.
	//-----------------------------------------------------------------------
	void SetValue(LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when value of in-place edit control changed
	// Parameters:
	//     pItemArgs - Pointer to structure with items arguments.
	//     szText    - New in-place edit text.
	//-----------------------------------------------------------------------
	virtual void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the item value in its original format.
	// Returns:
	//     Initial item text string.
	//-----------------------------------------------------------------------
	CString GetValue();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load a report record item
	//     using the specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);


protected:
	CString m_strText;  // Item text value.
};

AFX_INLINE CString CXTPReportRecordItemText::GetValue() {
	return m_strText;
}

//===========================================================================
// Summary:
//     This class represents a numeric cell and inherits basic functionality
//     from CXTPReportRecordItem class.
// Remarks:
//     You create a number record item simply by calling constructor with
//     the only one parameter - its numeric value. There is an additional
//     constructor with second parameter - format string.
//     Format string allows you to determine exactly in which format
//     the numeric value will be represented on the screen.
//     It could be useful for displaying money values, etc.
// See Also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemNumber : public CXTPReportRecordItem
{
	DECLARE_SERIAL(CXTPReportRecordItemNumber)
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a record numeric item with the specific formatting.
	// Parameters:
	//     dValue - Value of this numeric item.
	//     strFormat - Format string for creating caption value of the item.
	// Remarks:
	//     If the format string is set, the value of the item will be formatted
	//     according to this format string before drawing it convenient
	//     in many cases (drawing date for example). Format string is C - like
	//     style, see sprintf() C function or CString.Format() member function
	//-----------------------------------------------------------------------
	CXTPReportRecordItemNumber(double dValue = 0);
	CXTPReportRecordItemNumber(double dValue, LPCTSTR strFormat); // <combine CXTPReportRecordItemNumber::CXTPReportRecordItemNumber@double>

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a text caption of this item.
	// Parameters:
	//     pColumn - Corresponded column of the item.
	// Remarks:
	//     Overwrites parents' member function
	// Returns:
	//     Text caption of the item.
	//-----------------------------------------------------------------------
	CString GetCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns item value in its original format.
	// Returns:
	//     Item original numeric value.
	//-----------------------------------------------------------------------
	double GetValue();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set value of the item
	// Parameters:
	//     dValue - New item value
	//-----------------------------------------------------------------------
	void SetValue(double dValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when value of in-place edit control changed
	// Parameters:
	//     pItemArgs - Pointer to structure with items arguments.
	//     szText    - New in-place edit text.
	//-----------------------------------------------------------------------
	virtual void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Compares this item with the provided one.
	// Parameters:
	//  pColumn - Pointer to a CXTPReportColumn object.
	//  pItem - An item to compare with the current.
	// Remarks:
	//     Overwrites parents' member function
	// Returns:
	//     Zero if pItem is equal with current;
	//     Less than zero if less than pItem;
	//     Greater than zero if greater than pItem.
	//-----------------------------------------------------------------------
	int Compare(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load a report record item
	//     using the specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);
protected:
	double m_dValue;    // Cell value.
};

//===========================================================================
// Summary:
//     Represents a time cell and inherits basic functionality
//     from CXTPReportRecordItem class.
// Remarks:
//     You create a CXTPReportRecordItemDateTime record item simply by calling a constructor with one
//     COleDateTime parameter
// See Also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemDateTime : public CXTPReportRecordItem
{
	DECLARE_SERIAL(CXTPReportRecordItemDateTime)
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates record date/time item.
	// Parameters:
	//     odtValue - Initial date/time item value.
	//-----------------------------------------------------------------------
	CXTPReportRecordItemDateTime(COleDateTime odtValue = (DATE)0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a text caption of this item.
	// Parameters:
	//     pColumn - Corresponded column of the item.
	// Remarks:
	//     Overwrites parents' member function.
	// Returns:
	//     Text caption of the item.
	//-----------------------------------------------------------------------
	CString GetCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Compares this item with the provided one.
	// Parameters:
	//  pColumn - Pointer to a CXTPReportColumn object.
	//     pItem - The item provided for comparing with current.
	// Remarks:
	//     Overwrites parents' member function
	// Returns:
	//     Zero if pItem is equal with current;
	//     Less than zero if less than pItem;
	//     Greater than zero if greater than pItem.
	//-----------------------------------------------------------------------
	int Compare(CXTPReportColumn* pColumn, CXTPReportRecordItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when value of in-place edit control changed
	// Parameters:
	//     pItemArgs - Pointer to structure with items arguments.
	//     szText    - New in-place edit text.
	//-----------------------------------------------------------------------
	virtual void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the item value in its original format.
	// Returns:
	//     Item date-time value.
	//-----------------------------------------------------------------------
	COleDateTime GetValue();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set value of the item
	// Parameters:
	//     odtValue - New item value
	//-----------------------------------------------------------------------
	void SetValue(COleDateTime odtValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load a report record item
	//     using the specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

protected:
	COleDateTime m_odtValue;    // Cell value.
};


#if _MFC_VER >= 0x0600

//===========================================================================
// Summary:
//     Represents a variant item and inherits basic functionality
//     from CXTPReportRecordItem class.
//     You can create a bitmap record item simply by calling a constructor with
//     the only one parameter - value of VARIANT type.
// See Also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemVariant : public CXTPReportRecordItem
{
	DECLARE_SERIAL(CXTPReportRecordItemVariant)
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new variant record item.
	// Parameters:
	//     lpValue - Pointer to the initial item value.
	//-----------------------------------------------------------------------
	CXTPReportRecordItemVariant(const VARIANT& lpValue = COleVariant((long)0));

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a text caption of this item.
	// Parameters:
	//     pColumn - Corresponded column of the item.
	// Remarks:
	//     Overwrites parents' member function
	// Returns:
	//     Text caption of the item.
	//-----------------------------------------------------------------------
	CString GetCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets new value for the item.
	// Parameters:
	//     var - New value for the item.
	//-----------------------------------------------------------------------
	void SetValue(COleVariant& var);

	//-----------------------------------------------------------------------
	// Summary:
	//     Compares this item with the provided one.
	// Parameters:
	//     pItem - The item provided for comparing with current.
	// Remarks:
	//     Overwrites parents' member function
	// Returns:
	//     Zero if pItem is equal with current;
	//     Less than zero if less than pItem;
	//     Greater than zero if greater than pItem.
	//-----------------------------------------------------------------------
	int Compare(CXTPReportColumn*, CXTPReportRecordItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when value of in-place edit control changed
	// Parameters:
	//     pItemArgs - Pointer to structure with items arguments.
	//     szText    - New in-place edit text.
	//-----------------------------------------------------------------------
	virtual void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when user select specified constraint in in-place list
	// Parameters:
	//     pItemArgs   - Pointer to structure with items arguments
	//     pConstraint - Selected constraint
	//-----------------------------------------------------------------------
	void OnConstraintChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, CXTPReportRecordItemConstraint* pConstraint);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves associated value with selected constraint.
	// Parameters:
	//     pItemArgs - Pointer to structure with items arguments
	// Returns:
	//     DWORD value associated with selected constraint.
	//-----------------------------------------------------------------------
	DWORD GetSelectedConstraintData(XTP_REPORTRECORDITEM_ARGS* pItemArgs);

	virtual void DoPropExchange(CXTPPropExchange* pPX);//<COMBINE CXTPReportRecordItem::DoPropExchange>

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before value changed bu user
	// Parameters:
	//      pItemArgs - item arguments
	//     lpNewValue - new value to be set
	// Returns:
	//     TRUE to cancel edit operation.
	//-----------------------------------------------------------------------
	BOOL OnValueChanging(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPVARIANT lpNewValue);

protected:


public:
	COleVariant m_oleValue;    // Item value.
	static int m_nSortLocale;  // Sort locale
};

#endif


//===========================================================================
// Summary:
//     This represents a preview cell and inherits basic functionality
//     from CXTPReportRecordItem class.
// Remarks:
//     You create a CXTPReportRecordItemPreview record item simply by calling constructor with one
//     parameter - text string.
// See Also: CXTPReportRecordItem
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRecordItemPreview : public CXTPReportRecordItem
{
	DECLARE_SERIAL(CXTPReportRecordItemPreview)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Creates a new preview record item.
	// Parameters:
	//     szPreviewText - Pointer to the preview text of item.
	//-----------------------------------------------------------------------
	CXTPReportRecordItemPreview(LPCTSTR szPreviewText = _T(""));

	//-----------------------------------------------------------------------
	// Summary:
	//     Set preview text
	// Parameters:
	//     strPreviewText - new preview text
	//-----------------------------------------------------------------------
	void SetPreviewText(LPCTSTR strPreviewText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns preview text
	// Returns:
	//     Text of preview item
	//-----------------------------------------------------------------------
	virtual CString GetPreviewText();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to determine preview area height.
	// Parameters:
	//     pDC    - Pointer to control drawing context.
	//     pRow   - Pointer to corresponded row.
	//     nWidth - Width of the row
	// Returns:
	//     Height of the preview area.
	//-----------------------------------------------------------------------
	virtual int GetPreviewHeight(CDC* pDC, CXTPReportRow* pRow, int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns preview text
	// Parameters:
	//     pColumn - Corresponded column of the item.
	// Returns:
	//     Text of preview item
	//-----------------------------------------------------------------------
	CString GetCaption(CXTPReportColumn* pColumn);

	//-----------------------------------------------------------------------
	// Summary:
	//     Should be overridden by descendants for drawing itself.
	// Parameters:
	//     pDrawArgs - structure which contain drawing arguments.
	//     pMetrics - structure which contain metrics of the item.
	//-----------------------------------------------------------------------
	virtual void OnDrawCaption(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Fills XTP_REPORTRECORDITEM_METRICS structure
	// Parameters:
	//     pDrawArgs    - Draw arguments for calculating item metrics.
	//     pItemMetrics - Pointer to the metrics item to fill with values.
	// Remarks:
	//     Calculates preview item metrics based on provided draw arguments.
	//     Overwrites parents' member function
	// See Also: XTP_REPORTRECORDITEM_DRAWARGS, XTP_REPORTRECORDITEM_METRICS
	//-----------------------------------------------------------------------
	virtual void GetItemMetrics(XTP_REPORTRECORDITEM_DRAWARGS* pDrawArgs, XTP_REPORTRECORDITEM_METRICS* pItemMetrics);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines of this item is a preview item.
	// Returns:
	//     TRUE for the preview item, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsPreviewItem() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when value of in-place edit control changed
	// Parameters:
	//     pItemArgs - Pointer to structure with items arguments.
	//     szText    - New in-place edit text.
	//-----------------------------------------------------------------------
	virtual void OnEditChanged(XTP_REPORTRECORDITEM_ARGS* pItemArgs, LPCTSTR szText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves caption text bounding rectangle
	// Parameters:
	//     pDrawArgs - structure which contain drawing arguments.
	//     rcItem    - Bounding rectangle of the item
	//-----------------------------------------------------------------------
	virtual void GetCaptionRect(XTP_REPORTRECORDITEM_ARGS* pDrawArgs, CRect& rcItem);

	//<COMBINE CXTPReportRecordItem::DoPropExchange>
	virtual void DoPropExchange(CXTPPropExchange* pPX);

protected:
	CString m_strPreviewText;   // Preview content.
};

//**************************************************************************************
AFX_INLINE void CXTPReportRecordItemText::SetValue(LPCTSTR szText){
	m_strText = szText;
}
AFX_INLINE void CXTPReportRecordItemNumber::SetValue(double dValue) {
	m_dValue = dValue;
}
AFX_INLINE double CXTPReportRecordItemNumber::GetValue() {
	return m_dValue;
}
AFX_INLINE COleDateTime CXTPReportRecordItemDateTime::GetValue() {
	return m_odtValue;
}
AFX_INLINE void CXTPReportRecordItemDateTime::SetValue(COleDateTime odtValue) {
	m_odtValue = odtValue;
}
AFX_INLINE void CXTPReportRecordItemPreview::SetPreviewText(LPCTSTR strPreviewText) {
	m_strPreviewText = strPreviewText;
}
AFX_INLINE CString CXTPReportRecordItemPreview::GetPreviewText() {
	return m_strPreviewText;
}
AFX_INLINE BOOL CXTPReportRecordItemPreview::IsPreviewItem() const {
	return TRUE;
}

#endif //#if !defined(__XTPREPORTRECORDITEMTEXT_H__)
