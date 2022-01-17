// XTPReportRows.h: interface for the CXTPReportRows class.
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
#if !defined(__XTPREPORTROWS_H__)
#define __XTPREPORTROWS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPReportRow;
class CXTPReportControl;
class CXTPReportColumns;

//===========================================================================
// Summary:
//     This class represents a rows collection class.
//     It supports an array of CXTPReportRow pointers.
// Example:
//     See CXTPReportRows::Add for an example of how to work with this class.
// See Also: CXTPReportRow, CXTPReportSelectedRows
//===========================================================================
class _XTP_EXT_CLASS CXTPReportRows : public CXTPHeapObjectT<CCmdTarget, CXTPReportDataAllocator>
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Define a function pointer for comparing events.
	// Remarks:
	//     This function pointer is used in the SortEx method.
	// See Also:
	//     Sort, SortEx, CXTPReportControl::SetRowsCompareFunc
	//-----------------------------------------------------------------------
	typedef int (_cdecl* T_CompareFunc)(const CXTPReportRow** pRow1, const CXTPReportRow** pRow2);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs an empty CXTPReportRow pointer array.
	// Example:
	// <code>
	// // Declare a local CXTPReportRows object.
	// CXTPReportRows myList;
	//
	// // Declare a dynamic CXTPReportRows object.
	// CXTPReportRows* pTree = new CXTPReportRows();
	// </code>
	//-----------------------------------------------------------------------
	CXTPReportRows();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys CXTPReportRows object. Performs cleanup operations.
	//-----------------------------------------------------------------------
	virtual ~CXTPReportRows();

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all the elements from this array.
	// Parameters:
	//     bResetRow - TRUE to remove visible flag of child rows.
	// Remarks:
	//     Removes all the pointers from this array and releases instances
	//     of all stored CXTPReportRow objects.
	// Example:
	//     See example for CXTPReportRows::Add method.
	// See Also: CXTPReportRows overview
	//-----------------------------------------------------------------------
	virtual void Clear(BOOL bResetRow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the number of CXTPReportRow elements in this collection.
	// Remarks:
	//     Call this method to retrieve the number of elements in the array.
	//     Because indexes are zero-based, the size is 1 greater than
	//     the largest index.
	// Example:
	//     See example for CXTPReportRows::Add method.
	// Returns:
	//     The number of items in the collection.
	// See Also: CXTPReportRows overview
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a row at the specified numeric index.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetCount.
	// Remarks:
	//     Returns the array element at the specified index.
	// Example:
	//     See example for CXTPReportRows::Add method.
	// Returns:
	//     The row element currently at this index.
	//-----------------------------------------------------------------------
	virtual CXTPReportRow* GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a new Row element to the end of an array.
	// Parameters:
	//     pRow - The row element to be added to this array.
	// Remarks:
	//     Use this method to add the specified CXTPReportRow pointer
	//     to the end of the CXTPReportRows collection.
	// Returns:
	//     The index of the added row.
	// Example:
	// <code>
	// CXTPReportRows* pTree = new CXTPReportRows();
	// pTree->Add(new CXTPReportRow(pControl, pRecord1));
	// pTree->Add(new CXTPReportRow(pControl, pRecord2));
	// CXTPReportRow* pRow0 = pTree->GetAt(0);
	// CXTPReportRow* pRow1 = pTree->GetAt(1);
	// ASSERT(pRow0 == pTree->GetPrev(pRow1));
	// ASSERT(pRow1 == pTree->GetNext(pRow0));
	// pTree->RemoveAt(0);
	// ASSERT(1 == pTree->GetCount());
	// pTree->InsertAt(0, pRow0);
	// ASSERT(2 == pTree->GetCount());
	// pTree->Clear();
	// ASSERT(0 == pTree->GetCount());
	// </code>
	// See Also:
	//     CXTPReportRows overview, GetAt, InsertAt, Clear, GetNext, GetPrev, GetCount
	//-----------------------------------------------------------------------
	virtual int Add(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes an item from the collection on specified position.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetCount.
	// Remarks:
	//     In the process, it shifts down all the elements above the
	//     removed element. It decrements the upper bound of the array
	//     but does not free memory.
	//     It also releases an instance of the removed element.
	// Example:
	//     See example for CXTPReportRows::Add method.
	//-----------------------------------------------------------------------
	virtual void RemoveAt(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes specified row from the collection.
	// Parameters:
	//     pRow   - The row element to be removed.
	//-----------------------------------------------------------------------
	virtual int RemoveRow(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds one more Row object to the collection at the specified position.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetCount.
	//     pRow   - The row element to be placed in this array.
	// Remarks:
	//     InsertAt inserts one element at a specified index in an array.
	//     In the process, it shifts up (by incrementing the index) the
	//     existing element at this index, and it shifts up all the elements
	//     above it.
	// Example:
	//     See example for CXTPReportRows::Add method.
	//-----------------------------------------------------------------------
	virtual void InsertAt(int nIndex, CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns next row in the list.
	// Parameters:
	//     pRow            - A reference to the current element of the list.
	//     bSkipGroupFocus - TRUE to skip all groups.
	// Remarks:
	//     This function finds a pointer to the provided row element
	//     in the list, and returns a pointer to the elements following
	//     the found one.
	// Returns:
	//     A pointer to a next element of the list.
	// Example:
	//     See example for CXTPReportRows::Add method.
	// See Also: GetPrev, CXTPReportControl::SkipGroupsFocus
	//-----------------------------------------------------------------------
	virtual CXTPReportRow* GetNext(CXTPReportRow* pRow, BOOL bSkipGroupFocus);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns previous row in the list.
	// Parameters:
	//     pRow            - A reference to the current element of the list.
	//     bSkipGroupFocus - TRUE to skip all groups.
	// Remarks:
	//     This function finds a pointer to the provided row element
	//     in the list, and returns a pointer to the elements before
	//     the found one.
	// Returns:
	//     A pointer to a previous element of the list.
	// Example:
	//     See example for CXTPReportRows::Add method.
	// See Also: GetNext, CXTPReportControl::SkipGroupsFocus
	//-----------------------------------------------------------------------
	virtual CXTPReportRow* GetPrev(CXTPReportRow* pRow, BOOL bSkipGroupFocus);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set virtual mode with nCount rows.
	// Parameters:
	//     pRow   - Virtual row
	//     nCount - Count of virtual rows
	//-----------------------------------------------------------------------
	virtual void SetVirtualMode(CXTPReportRow* pRow, int nCount);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to find row corresponded with specified record
	// Parameters:
	//     pRecord  - Record need to find.
	// Returns:
	//     A pointer to associated row object or NULL.
	//-----------------------------------------------------------------------
	virtual CXTPReportRow* Find(CXTPReportRecord* pRecord);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to find row corresponded with specified record.
	//     This function performs search in children too.
	// Parameters:
	//     pRecord  - Record need to find.
	// Returns:
	//     A pointer to associated row object or NULL.
	//-----------------------------------------------------------------------
	virtual CXTPReportRow* FindInTree(CXTPReportRecord* pRecord);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sorts collection elements.
	// Parameters:
	//     pCompareFunc - A T_CompareFunc function pointer that is used
	//                    to compare rows.
	// Remarks:
	//     This method uses Visual C++ run-time library (MSVCRT)
	//     implementation of the quick-sort function, qsort, for sorting
	//     stored CXTPReportRow objects.
	//
	//     Sort() internally uses CompareRows method for comparing 2 rows.
	// See Also: CompareRows
	//-----------------------------------------------------------------------
	virtual void SortEx(T_CompareFunc pCompareFunc);
	virtual void Sort(); // <COMBINE SortEx>



	//-----------------------------------------------------------------------
	// Summary:
	//     Compares 2 rows using groups and sort orders of the report control.
	// Parameters:
	//     pRow1 - First row for comparison.
	//     pRow2 - Second row for comparison.
	// Remarks:
	//     This function is called directly by Sort method.
	//
	//     The default implementation returns the result of the comparison
	//     of *pRow1 and *pRow2.
	//
	//     This implementation uses CXTPReportRecordItem::Compare for comparing
	//     two corresponding items from the row records. First it compares
	//     record items in the group order, then by sort order. It returns
	//     the first comparison result differ from equality. If no differences was
	//     found, it returns 0.
	//
	// Returns:
	//     Zero if pRow1 is equal to pRow2;
	//     Less than zero if pRow1 is less than pRow2;
	//     Greater than zero if pRow1 is greater than pRow2.
	// See Also: Sort, CXTPReportRecordItem::Compare
	//-----------------------------------------------------------------------
	static int _cdecl CompareRows(const CXTPReportRow** pRow1, const CXTPReportRow** pRow2);

protected:
	CArray<CXTPReportRow*, CXTPReportRow*> m_arrRows;   // Internal storage for CXTPReportRow objects.

	CXTPReportRow* m_pVirtualRow;                       // Virtual row.
	int m_nVirtualRowsCount;                            // Count of virtual rows.

};

//===========================================================================
// Summary:
//     Encapsulates a collection of CXTPReportRow pointers that represent
//     the selected rows in a Report Control.
// Remarks:
//     Use this class to programmatically manage a collection of
//     CXTPReportRow pointers that represent the selected rows in a
//     Report Control. This class is commonly used to add or remove rows
//     from the collection.
//
//     Typical work flow is using Add and Remove methods for changing
//     contents of the selection and using Contains method for checking
//     a specific row for its presence in the selection.
// Example:
//     The following example demonstrates how to programmatically use
//     the CXTPReportSelectedRows class to select rows in the Report control.
// <code>
// CXTPReportSelectedRows* pSelRows = pReportControl->GetSelectedRows();
// pSelRows->Add(pRow1);
// pSelRows->Add(pRow2);
// ASSERT(TRUE == pSelRows->Contains(pRow1));
// ASSERT(TRUE == pSelRows->Contains(pRow2));
// pSelRows->Remove(pRow1);
// ASSERT(FALSE == pSelRows->Contains(pRow1));
// pSelRows->Select(pRow1);
// ASSERT(TRUE == pSelRows->Contains(pRow1));
// ASSERT(FALSE == pSelRows->Contains(pRow2));
// </code>
//
// See Also: CXTPReportRow, CXTPReportSelectedRows, CXTPReportControl::GetSelectedRows
//===========================================================================
class _XTP_EXT_CLASS CXTPReportSelectedRows : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs an empty CXTPReportSelectedRows collection.
	// Parameters:
	//     pControl - Pointer to the parent report control.
	// Remarks:
	//     This collection could be created only in association with
	//     the CXTPReportControl object.
	// Example:
	// <code>
	// // from CXTPReportControl member function
	// CXTPReportSelectedRows* pSelectedRows = new CXTPReportSelectedRows(this);
	// </code>
	// See Also: CXTPReportSelectedRows overview
	//-----------------------------------------------------------------------
	CXTPReportSelectedRows(CXTPReportControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Selects a block of rows.
	// Parameters:
	//     nBlockBegin - First row index from the block.
	//     nBlockEnd   - Final row index from the block.
	// Remarks:
	//     This function uses pRowBegin and pRowEnd as the bound for the
	//     required selection. It enumerates parent report control rows
	//     collection and adds all rows from pRowBegin to pRowEnd inclusively
	//     to the selection.
	//-----------------------------------------------------------------------
	void SelectBlock(int nBlockBegin, int nBlockEnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Clears itself, removing selection.
	// Remarks:
	//     Removes all the elements from the selection.
	//-----------------------------------------------------------------------
	void Clear();

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a row to the selection.
	// Parameters:
	//     pRow - Pointer to the row to be added to the selection.
	// Remarks:
	//     This method adds a pointer to the provided row to the selection.
	//     After adding, Contains method will return TRUE for all
	//     checking attempts of this row pointer.
	// Example:
	//     See example at CXTPReportSelectedRows overview
	// See Also: CXTPReportSelectedRows overview, Remove, Select, Clear, Contains
	//-----------------------------------------------------------------------
	void Add(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a rows to the selection.
	// Parameters:
	//     nIndexBegin - First row index of block to be selected.
	//     nIndexEnd - Last row index of block to be selected.
	//-----------------------------------------------------------------------
	void AddBlock(int nIndexBegin, int nIndexEnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes a row from the selection.
	// Parameters:
	//     pRow - Pointer to the row to be removed from the selection.
	// Remarks:
	//     This method removes a provided row pointer from the selection.
	//     After removing, Contains method will return FALSE for all
	//     checking attempts of this row pointer.
	// Example:
	//     See example at CXTPReportSelectedRows overview
	// See Also: CXTPReportSelectedRows overview, Add, Select, Clear, Contains
	//-----------------------------------------------------------------------
	void Remove(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns a value indicating whether the CXTPReportSelectedRows
	//     contains the specified CXTPReportRow pointer.
	// Parameters:
	//     pRow - The CXTPReportRow pointer to search for in the CXTPReportSelectedRows.
	// Remarks:
	//     Use this method to determine whether the CXTPReportSelectedRows
	//     contains the specified CXTPReportRow pointer.
	// Returns:
	//     TRUE if row is contained in the selection, FALSE otherwise.
	// Example:
	//     See example at CXTPReportSelectedRows overview
	// See Also: CXTPReportSelectedRows overview, Add, Remove, Select, Clear
	//-----------------------------------------------------------------------
	BOOL Contains(const CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Inverts selection for the specified row.
	// Parameters:
	//     pRow - Pointer to the specified row.
	// Remarks:
	//     This methods checks the specified method for its presence in
	//     the collection and adds or removes it in the reverse order
	//     depending on the result.
	// Example:
	// <code>
	// CXTPReportSelectedRows* pSelRows = pReportControl->GetSelectedRows();
	// ASSERT(TRUE == pSelRows->Contains(pRow1));
	// pSelRows->Invert(pRow1);
	// ASSERT(FALSE == pSelRows->Contains(pRow1));
	// pSelRows->Invert(pRow1);
	// ASSERT(TRUE == pSelRows->Contains(pRow1));
	// </code>
	// See Also: Add, Remove, Contains
	//-----------------------------------------------------------------------
	void Invert(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Selects only the specified row.
	// Parameters:
	//     pRow - Pointer to the specified row.
	// Remarks:
	//     This method clears the initial selection and
	//     selects only the specified row.
	// Example:
	//     See example at CXTPReportSelectedRows overview
	// See Also: CXTPReportSelectedRows overview, Add, Remove, Contains, Clear
	//-----------------------------------------------------------------------
	void Select(CXTPReportRow* pRow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the number of selected rows in the collection.
	// Remarks:
	//     Call this method to retrieve the number of selected rows
	//     in the array.
	// Returns:
	//     The number of items in the collection.
	// See Also: CXTPReportRows overview
	//-----------------------------------------------------------------------
	int GetCount();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves selected row by index.
	// Parameters:
	//     nIndex - Index of selected row to retrieve.
	// Remarks:
	//     Recommended to use GetFirstSelectedRowPosition / GetNextSelectedRow methods.
	// See Also: GetFirstSelectedRowPosition, GetNextSelectedRow
	//-----------------------------------------------------------------------
	CXTPReportRow* GetAt (int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the position of the first selected row in the list report control.
	// Returns:
	//     value that can be used for iteration or object pointer retrieval;
	//     NULL if no items are selected.
	// See Also: GetNextSelectedItem
	//-----------------------------------------------------------------------
	POSITION GetFirstSelectedRowPosition();

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets next selected row in the list report control.
	// Parameters:
	//     pos - A reference to a POSITION value returned by a previous call to
	//           GetNextSelectedRow or GetFirstSelectedRowPosition.
	//           The value is updated to the next position by this call.
	// Returns:
	//     The pointer of the next selected row in the list report control.
	//-----------------------------------------------------------------------
	CXTPReportRow* GetNextSelectedRow(POSITION& pos);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to check if selection was changed.
	//-----------------------------------------------------------------------
	BOOL IsChanged() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to reset changed flag.
	// Parameters:
	//     bChanged - TRUE to reset.
	//-----------------------------------------------------------------------
	void SetChanged(BOOL bChanged = TRUE);

//private:
	//{{AFX_CODEJOCK_PRIVATE
	void _InsertBlock(int nIndexInsert, int nIndexBegin, int nIndexEnd);
	void _OnExpanded(int nIndex, int nCount);
	void _OnCollapsed(int nIndex, int nCount);

//private:
	struct SELECTED_BLOCK
	{
		int nIndexBegin;
		int nIndexEnd;
	};

//private:
	CXTPReportControl* m_pControl;      // Pointer to the parent report control.
	int m_nRowBlockBegin;               // Pointer to the row where rows block begin from.
	int m_nPosSelected;
	CArray<SELECTED_BLOCK, SELECTED_BLOCK&> m_arrSelectedBlocks;
	BOOL m_bChanged;
	//}}AFX_CODEJOCK_PRIVATE

	friend class CXTPReportControl;
};


AFX_INLINE BOOL CXTPReportSelectedRows::IsChanged() const {
	return m_bChanged;

}
AFX_INLINE void CXTPReportSelectedRows::SetChanged(BOOL bChanged /*= TRUE*/) {
	m_bChanged = bChanged;
}


#endif //#if !defined(__XTPREPORTROWS_H__)
