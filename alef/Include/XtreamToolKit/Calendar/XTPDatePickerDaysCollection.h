// XTPDatePickerDaysCollection.h: interface for the CXTPDatePickerDaysCollection class.
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
#if !defined(_XTPDATEPICKERDAYSCOLLECTION_H__)
#define _XTPDATEPICKERDAYSCOLLECTION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE
// XTPDatePickerDaysCollection.h : header file
///

class CXTPDatePickerControl;
class CXTPDatePickerItemDay;

// ----------------------------------------------------------------------
// Remarks:
//     Class CXTPDatePickerDaysCollection provides facilities to build various
//     collection of days, for using in groups. For example, the selected days
//     collection. To create an object of this class, simply call the constructor
//     and provide a pointer to the DatePicker controller.
// ----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPDatePickerDaysCollection : public CXTPCmdTarget
{
	friend class CXTPDatePickerControl;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Selected block structure
	//-----------------------------------------------------------------------
	struct SELECTED_BLOCK
	{
		long nDateBegin;   // Begin date
		long nDateEnd;     // End date
	};

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection constructor.
	// Parameters:
	//     pControl - Pointer to the DatePicker control object.
	// Remarks:
	//     Handles initial initialization.
	//-----------------------------------------------------------------------
	CXTPDatePickerDaysCollection(CXTPDatePickerControl* pControl);

	// -------------------------------------
	// Summary:
	//     Default collection destructor.
	// Remarks:
	//     Handles member item deallocation.
	// See Also:
	//     Clear()
	// -------------------------------------
	virtual ~CXTPDatePickerDaysCollection();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to clear the day collection.
	//-----------------------------------------------------------------------
	void Clear();

	// ---------------------------------------------------------------------
	// Summary:
	//     Call this member function to add a day to the collection.
	// Parameters:
	//     dtDay :  Date of a day, the key of the new element.
	// Remarks:
	//     The primary means to insert an element into a map. First, search
	//     for the key. If the key is found, then the corresponding value is
	//     changed. Otherwise, a new key-value pair is created an added to
	//     the map.
	// See Also:
	//     Remove
	// ---------------------------------------------------------------------
	void Add(const COleDateTime& dtDay);

	// ---------------------------------------------------------------------
	// Summary:
	//     Call this member function to select a day
	// Parameters:
	//     dtDay :  Date of a day, the key of the new element.
	// See Also:
	//     Add, SelectRange
	// ---------------------------------------------------------------------
	void Select(const COleDateTime& dtDay);

	// ---------------------------------------------------------------------
	// Summary:
	//     Call this member function to select days
	// Parameters:
	//     dtDayBegin :  First Date of a day
	//     dtDayEnd :  Last Date of a day
	// See Also:
	//     Select
	// ---------------------------------------------------------------------
	void SelectRange(const COleDateTime& dtDayBegin, const COleDateTime& dtDayEnd);


	// --------------------------------------------------------------------------
	// Summary:
	//     Call this member function to remove a day from the collection.
	// Parameters:
	//     dtDay :  A COleDateTime reference that contains the date of a day. The
	//              key for the element to be removed.
	// Remarks:
	//     Searches the map for the entry corresponding to the supplied key.
	//     If the key is found, then the entry is removed.
	// See Also:
	//     Add(COleDateTime&)
	// --------------------------------------------------------------------------
	void Remove(const COleDateTime& dtDay);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if a day is contained in
	//     the collection.
	// Parameters:
	//     dtDay - Date of a day. The key that identifies the element to
	//             search for.
	// Remarks:
	//     This function uses a hash algorithm to quickly find the map
	//     element using a key that exactly matches the given key.
	// Returns:
	//     Nonzero if the element was found. Otherwise 0.
	//-----------------------------------------------------------------------
	BOOL Contains(const COleDateTime& dtDay) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to count the number of days in the
	//     collection.
	// Returns:
	//     An integer value representing the number of day items in
	//     the collection.
	//-----------------------------------------------------------------------
	int GetSelectedDaysCount();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to count the number of selected blocks.
	// Returns:
	//     An integer value representing the number of selected blocks.
	//-----------------------------------------------------------------------
	int GetSelectedBlocksCount();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the selected block by its index.
	// Parameters:
	//     nIndex - Selected block to retrieve.
	//-----------------------------------------------------------------------
	SELECTED_BLOCK GetSelectedBlock(int nIndex);

	// -----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine the minimum and the maximum
	//     day in the collection.
	// Parameters:
	//     refMinRange :  A COleDateTime reference to the minimum date storage
	//                    variable.
	//     refMaxRange :  A COleDateTime reference to maximum date storage
	//                    variable.
	// Returns:
	//     A boolean value. TRUE if successful. Otherwise FALSE.
	// -----------------------------------------------------------------------
	BOOL GetMinMaxRange(COleDateTime& refMinRange, COleDateTime& refMaxRange) const;

private:

	void _InsertBlock(int nIndexInsert, long nDateBegin, long nDateEnd);
	void AddBlock(long nBegin, long nEnd);

// Attributes
protected:
	CXTPDatePickerControl* m_pControl;        // --------------------------------------------------------
	                                          // This member variable is a pointer to the parent control.
	                                          // --------------------------------------------------------
	CArray<SELECTED_BLOCK, SELECTED_BLOCK&> m_arrSelectedBlocks; // Contains an array of the selected dates blocks.

};

AFX_INLINE int CXTPDatePickerDaysCollection::GetSelectedBlocksCount() {
	return (int)m_arrSelectedBlocks.GetSize();
}
AFX_INLINE CXTPDatePickerDaysCollection::SELECTED_BLOCK CXTPDatePickerDaysCollection::GetSelectedBlock(int nIndex) {
	ASSERT(nIndex >= 0 && nIndex < (int)m_arrSelectedBlocks.GetSize());
	return m_arrSelectedBlocks[nIndex];
}

#endif // !defined(_XTPDATEPICKERDAYSCOLLECTION_H__)
