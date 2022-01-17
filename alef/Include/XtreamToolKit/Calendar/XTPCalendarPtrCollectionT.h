// XTPCalendarPtrCollectionT.h: CXTPCalendarPtrCollectionT template.
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
#if !defined(_XTPCALENDARCOLLECTIONT_H__)
#define _XTPCALENDARCOLLECTIONT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4097)

#include "Common/XTPSmartPtrInternalT.h"

//===========================================================================
// Summary:
//     This class represents a simple array collection of objects.
// Remarks:
//     Array indexes always start at position 0.
//
//          As with a C array, the access time for indexed element of this
//          array is constant and is independent of the array size.
// See Also:  overview, CArray overview
//===========================================================================
template<class _TObject>
class CXTPCalendarPtrCollectionT : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection constructor.
	// See Also: ~CXTPCalendarPtrCollectionT()
	//-----------------------------------------------------------------------
	CXTPCalendarPtrCollectionT();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection destructor.
	// Remarks:
	//     Handles member items deallocation. Decreases reference of all
	//     stored  objects.
	// See Also: RemoveAll()
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarPtrCollectionT();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the number of elements
	//     in the collection.
	// Remarks:
	//     Call this method to retrieve the number of elements in the array.
	//     Because indexes are zero-based, the size is 1 greater than
	//     the largest index.
	// Example: See example for CXTPCalendarEvents::Add method.
	// Returns:
	//     The number of items in the collection.
	//-----------------------------------------------------------------------
	virtual int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function adds a new element to the end of the array.
	// Parameters:
	//     pNewElement - A pointer to a _TObjectThe object. Element to add to the array.
	//     bWithAddRef - A BOOL.  If this parameter is TRUE then InternalAddRef()
	//                   is called for pNewElement, otherwise it
	//                   is not called.
	// Remarks:
	//     Use this method to add the specified pointer to the end
	//     of the events collection. Reference to the added object is
	//     increased.
	// Example:
	// <code>
	// CXTPCalendarPtrCollectionT<CXTPCalendarEvent> arList;
	// CXTPCalendarEventPtr ptrEvent1 = new CXTPCalendarEvent()
	//
	// // Add() will call InernalAddRef()
	// arList.Add(ptrEvent1);
	//
	// // Add() will not call InernalAddRef()
	// arList.Add(new CXTPCalendarEvent(), FALSE);
	//
	// // GetAt() will call InernalAddRef()
	// CXTPCalendarEventPtr ptrEvent0 = arList.GetAt(0);
	//
	// // GetAt() will not call InernalAddRef()
	// CXTPCalendarEvent* pEvent1 = arList.GetAt(1, FALSE);
	//
	// ASSERT(2 == arList.GetCount());
	//
	// //RemoveAll() will call InernalRelease() for all objects
	// arList.RemoveAll();
	//
	// ASSERT(0 == arList.GetCount());
	//
	// Version 2
	//
	// CXTPCalendarPtrCollectionT<CXTPCalendarEvent> arList;
	// CXTPCalendarEventPtr ptrEvent1 = new CXTPCalendarEvent()
	//
	// // Add() will call InernalAddRef()
	// arList.Add(ptrEvent1);
	//
	// // Add() will not call InernalAddRef()
	// arList.Add(new CXTPCalendarEvent(), FALSE);
	//
	// // GetAt() will call InernalAddRef()
	// CXTPCalendarEventPtr ptrEvent0 = arList.GetAt(0);
	//
	// // GetAt() will not call InernalAddRef()
	// CXTPCalendarEvent* pEvent1 = arList.GetAt(1, FALSE);
	//
	// ASSERT(2 == arList.GetCount());
	//
	// //RemoveAll() will call InernalRelease() for all objects
	// arList.RemoveAll();
	//
	// ASSERT(0 == arList.GetCount());
	// </code>
	// See Also: CXTPCalendarPtrCollectionT overview, GetAt, RemoveAll,
	//           GetCount
	//-----------------------------------------------------------------------
	virtual void Add(_TObject* pNewElement);
	virtual void Add(_TObject* pNewElement, BOOL bWithAddRef); // <combine CXTPCalendarPtrCollectionT::Add@_TObject*>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set a new element to the position
	//     specified in the nIndex parameter.
	// Parameters:
	//     nIndex      - An integer index that is greater than or equal
	//                   to 0 and less than the value returned by
	//                   GetCount.
	//     pElement    - A pointer to a _TObject object. The element to add to the array.
	// Remarks:
	//     Use this method to set the specified element to the
	//     position specified in nIndex parameter.
	//     Reference to the previous object is decreased.
	//     Reference to the new object is increased.
	// Example:
	// <code>
	// CXTPCalendarPtrCollectionT<CXTPCalendarEvent> arList;
	// CXTPCalendarEventPtr ptrEvent1 = new CXTPCalendarEvent()
	//
	// arList.Add(ptrEvent1);
	// arList.SetAt(0, new CXTPCalendarEvent());
	// </code>
	// See Also: GetAt, Add, InsertAt
	//-----------------------------------------------------------------------
	virtual void SetAt(int nIndex, _TObject* pElement);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function finds element index in the collection using
	//      its pointer.
	// Parameters:
	//      pElement - A pointer to a _TObject object. The element to find in
	//                 the array.
	// Returns:
	//      -1 if element is not found, otherwise an integer index that is
	//      greater than or equal to 0 and less than the value returned
	//      by GetCount. The specified element currently at this index.
	//-----------------------------------------------------------------------
	virtual int Find(const _TObject* pElement);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to insert a new element at the
	//     position specified in the nIndex parameter.
	// Parameters:
	//     nIndex      - An integer index that is greater than or equal
	//                   to 0 and less than the value returned by
	//                   GetCount().
	//     pElement    - A pointer to a _TObject object. The element to add to the array.
	// Remarks:
	//     Use this method to insert the specified element t the
	//     position specified in the nIndex parameter.
	//     Reference to the new object is increased.
	// Example:
	// <code>
	// CXTPCalendarPtrCollectionT<CXTPCalendarEvent> arList;
	// CXTPCalendarEventPtr ptrEvent1 = new CXTPCalendarEvent()
	//
	// arList.Add(ptrEvent1);
	// arList.InsertAt(0, new CXTPCalendarEvent());
	// </code>
	// See Also: GetAt, Add, InsertAt
	//-----------------------------------------------------------------------
	virtual void InsertAt(int nIndex, _TObject* pElement);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to add a new elements to the end
	//     of the array.
	// Parameters:
	//     pElementsArray - A pointer to an array of _TObject objects.
	//                      A pointer to the collection to add to the array.
	// Remarks:
	//     Use this method to add the elements from the specified
	//     array to the end of the collection. Reference to the
	//     new object is increased.
	// Example:
	// <code>
	// CXTPCalendarPtrCollectionT<CXTPCalendarEvent> arList1;
	// CXTPCalendarPtrCollectionT<CXTPCalendarEvent> arList2;
	// ...
	// arList1.Append(&arList2);
	// </code>
	// See Also: GetAt, Add, InsertAt, SetAt
	//-----------------------------------------------------------------------
	virtual void Append(CXTPCalendarPtrCollectionT<_TObject>* pElementsArray);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to change the array size.
	// Parameters:
	//     nNewSize - An int that contains the new array size.
	// Remarks:
	//     Argument nNewSize contains the new array size (number of elements).
	//     Must be greater than or equal to 0.
	//-----------------------------------------------------------------------
	virtual void SetSize(int nNewSize);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns an element at the specified
	//     numeric index. Reference to the returned object is increased.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetCount.
	//     bWithAddRef - A BOOL. If this parameter is TRUE then InternalAddRef()
	//                   is called for the returned object,
	//                   otherwise it is not called.
	// Remarks:
	//     Returns the array element at the specified index.
	// Example: See the example for the Add() method.
	// Returns:
	//     A pointer to a _TObject object. The element currently at this index.
	//-----------------------------------------------------------------------
	virtual _TObject* GetAt(int nIndex) const;
	virtual _TObject* GetAt(int nIndex, BOOL bWithAddRef) const; // <combine CXTPCalendarPtrCollectionT::GetAt@int@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function removes an element at the specified index
	//     from the array.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetCount().
	// Remarks:
	//     Removes the pointer from this array and releases instance
	//     of the stored object.
	//-----------------------------------------------------------------------
	virtual void RemoveAt(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function finds an element using its pointer and removes
	//     it from the array.
	// Parameters:
	//      pElement - A pointer to a _TObject object. The element to find in
	//                 the array.
	// Remarks:
	//     Removes the pointer from this array and releases instance
	//     of the stored object.
	// See Also: RemoveAt(), RemoveAll()
	//-----------------------------------------------------------------------
	virtual void Remove(_TObject* pElement);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to remove all of the elements from
	//     the array.
	// Remarks:
	//     Removes all of the pointers from this array and releases instances
	//     of all stored objects.
	// Example: See example for CXTPCalendarEvents::Add method.
	// See Also: CXTPCalendarEvents overview
	//-----------------------------------------------------------------------
	virtual void RemoveAll();

protected:
	typedef CXTPSmartPtrInternalT<_TObject> TObjectPtr;  // Smart pointer type for stored objects.
	CArray<TObjectPtr, TObjectPtr&> m_arElements;        // Objects storage.
};
////////////////////////////////////////////////////////////////////////////

//===========================================================================
// Summary:
//     This class represents a simple array collection of objects pointers.
// Remarks:
//          <b>Parameters</b>
//              BASE_CLASS
//                  Base class of the typed pointer array class;
//                  must be an array class (CObArray or CPtrArray).
//              PTR_TYPE
//                  Type of the elements stored in the base-class array.
//
//          Array indexes always start at position 0.
//
//          As with a C array, the access time for indexed element of this
//          array is constant and is independent of the array size.
//
//          Overridden method RemoveAll() call delete operator for all not NULL
//          elements before call RemoveAll() method from the base class.
//
//          RemoveAll() method is called from array destructor too.
//
// See Also: CTypedPtrArray, CPtrArray, CObArray, CArray
//===========================================================================
template<class BASE_CLASS, class PTR_TYPE>
class CXTPCalendarTypedPtrAutoDeleteArray : public CTypedPtrArray<BASE_CLASS, PTR_TYPE>
{
public:
	//------------------------------------------------------------------------
	// Summary:
	//     Base class type definition
	//------------------------------------------------------------------------
	typedef CTypedPtrArray<BASE_CLASS, PTR_TYPE> TBase;

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarTypedPtrAutoDeleteArray() {};

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarTypedPtrAutoDeleteArray() {
		RemoveAll();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all the elements from the array.
	// Remarks:
	//     Removes all the pointers from the array and deletes
	//     all stored objects.
	//-----------------------------------------------------------------------
	virtual void RemoveAll()
	{
		for(int i = 0; i < GetSize(); i++)
		{
			PTR_TYPE pObj = GetAt(i);
			if (pObj) {
				delete pObj;
				SetAt(i, NULL);
			}
		}
		TBase::RemoveAll();
	}
};

//===========================================================================
// Summary:
//     This class represents a simple map of typed keys to typed objects pointers.
//     Overridden methods SetAt, operator[], RemoveKey, RemoveAll, call delete
//     for previously stored elements.
// Remarks:
//          <b>Parameters</b>
//              KEY
//                  class to key values.
//              TPtr
//                  A pointer type of stored objects.
//
//          Overridden method RemoveAll() call delete operator for all not NULL
//          spored elements before call RemoveAll() method from the base class.
//
//          RemoveAll() method is called from map destructor too.
//
// See Also: CMap
//===========================================================================
template<class KEY, class TPtr>
class CXTPCalendarTypedPtrAutoDeleteMap : public CMap<KEY, KEY, TPtr, TPtr>
{
//{{AFX_CODEJOCK_PRIVATE
	typedef CMap<KEY, KEY, TPtr, TPtr> TBase;
//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Handles member items deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarTypedPtrAutoDeleteMap()
	{
		RemoveAll();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all the elements from the map.
	// Parameters:
	//     bDeletePtr - If TRUE, delete operator will be called for not null
	//                  elements. Default value is TRUE.
	// Remarks:
	//     Removes all data from the map and deletes all stored objects if
	//     this specified by bDeletePtr parameter.
	//-----------------------------------------------------------------------
	virtual void RemoveAll(BOOL bDeletePtr = TRUE)
	{
		if (bDeletePtr)
		{
			POSITION pos = GetStartPosition();
			while (pos)
			{
				KEY tmpKey;
				TPtr pValue = NULL;
				GetNextAssoc(pos, tmpKey, pValue);
				if (pValue)
					delete pValue;
			}
		}
		TBase::RemoveAll();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Add a new (key, value) pair.
	// Parameters:
	//     bDeletePrevPtr - If TRUE, delete operator will be called for previous
	//                      not null element.
	//-----------------------------------------------------------------------
	virtual void SetAt(KEY key, TPtr pValue, BOOL bDeletePrevPtr = TRUE)
	{
		if (bDeletePrevPtr)
			_DeletePtr(key);
		TBase::SetAt(key, pValue);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Lookup and add a new element if not there.
	//-----------------------------------------------------------------------
	TPtr& operator[](KEY key)
	{
		_DeletePtr(key);
		return TBase::operator[](key);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes element by it's key.
	// Parameters:
	//     bDeletePtr - If TRUE, delete operator will be called for not null
	//                  element. Default value is TRUE.
	//-----------------------------------------------------------------------
	virtual BOOL RemoveKey(KEY key, BOOL bDeletePtr = TRUE)
	{
		if (bDeletePtr)
			_DeletePtr(key);
		return TBase::RemoveKey(key);
	}

private:
	void _DeletePtr(KEY key)
	{
		TPtr pValPrev = NULL;
		if (Lookup(key, pValPrev) && pValPrev)
		{
			delete pValPrev;
		}
	}
};



/////////////////////////////////////////////////////////////////////////////
template<class _TObject>
AFX_INLINE CXTPCalendarPtrCollectionT<_TObject>::CXTPCalendarPtrCollectionT()
{
}

template<class _TObject>
AFX_INLINE CXTPCalendarPtrCollectionT<_TObject>::~CXTPCalendarPtrCollectionT()
{
}

template<class _TObject>
AFX_INLINE int CXTPCalendarPtrCollectionT<_TObject>::GetCount() const
{
	return (int)m_arElements.GetSize();
}

template<class _TObject>
AFX_INLINE _TObject* CXTPCalendarPtrCollectionT<_TObject>::GetAt(int nIndex) const
{
	_TObject* pElement = m_arElements[nIndex];
	return pElement;
}

template<class _TObject>
AFX_INLINE _TObject* CXTPCalendarPtrCollectionT<_TObject>::GetAt(int nIndex, BOOL bWithAddRef) const
{
	_TObject* pElement = m_arElements[nIndex];
	if (bWithAddRef && pElement) {
		pElement->InternalAddRef();
	}
	return pElement;
}

template<class _TObject>
AFX_INLINE void CXTPCalendarPtrCollectionT<_TObject>::Add(_TObject* pNewElement, BOOL bWithAddRef)
{
	if (bWithAddRef && pNewElement) {
		pNewElement->InternalAddRef();
	}
	TObjectPtr ptrNewElement(pNewElement);
	m_arElements.Add(ptrNewElement);
}

template<class _TObject>
AFX_INLINE void CXTPCalendarPtrCollectionT<_TObject>::Add(_TObject* pNewElement)
{
	TObjectPtr ptrNewElement(pNewElement);
	m_arElements.Add(ptrNewElement);
}

template<class _TObject>
AFX_INLINE void CXTPCalendarPtrCollectionT<_TObject>::Append(CXTPCalendarPtrCollectionT<_TObject>* pElementsArray)
{
	if (!pElementsArray) {
		ASSERT(FALSE);
		return;
	}

	int nCount = pElementsArray->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		TObjectPtr ptrElement = pElementsArray->GetAt(i, TRUE);
		m_arElements.Add(ptrElement);
	}
}

template<class _TObject>
AFX_INLINE int CXTPCalendarPtrCollectionT<_TObject>::Find(const _TObject* pElement)
{
	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		if (GetAt(i) == pElement)
			return i;
	}
	return -1;
}

template<class _TObject>
AFX_INLINE void CXTPCalendarPtrCollectionT<_TObject>::Remove(_TObject* pElement)
{
	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		if (GetAt(i) == pElement)
		{
			RemoveAt(i);
			break;
		}
	}
}

template<class _TObject>
AFX_INLINE void CXTPCalendarPtrCollectionT<_TObject>::SetSize(int nNewSize)
{
	m_arElements.SetSize(nNewSize);
}

template<class _TObject>
AFX_INLINE void CXTPCalendarPtrCollectionT<_TObject>::SetAt(int nIndex, _TObject* pElement)
{
	TObjectPtr ptrNewElement(pElement);
	m_arElements.SetAt(nIndex, ptrNewElement);
}

template<class _TObject>
AFX_INLINE void CXTPCalendarPtrCollectionT<_TObject>::InsertAt(int nIndex, _TObject* pElement)
{
	TObjectPtr ptrNewElement(pElement);
	m_arElements.InsertAt(nIndex, ptrNewElement);
}

template<class _TObject>
AFX_INLINE void CXTPCalendarPtrCollectionT<_TObject>::RemoveAt(int nIndex/*, BOOL bWithRelease*/)
{
	m_arElements.RemoveAt(nIndex);
}

template<class _TObject>
AFX_INLINE void CXTPCalendarPtrCollectionT<_TObject>::RemoveAll(/*BOOL bWithRelease*/)
{
	m_arElements.RemoveAll();
}

#endif // !defined(_XTPCALENDARCOLLECTIONT_H__)
