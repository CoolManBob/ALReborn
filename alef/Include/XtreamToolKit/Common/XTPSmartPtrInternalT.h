// XTPSmartPtrInternalT.h: CXTPSmartPtrInternalT template definition.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#if !defined(_XtpSmartPtrInternal_H__)
#define _XtpSmartPtrInternal_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE

/////////////////////////////////////////////////////////////////////////////
// template: CXTPSmartPtrInternalT
// macro:    XTP_DEFINE_SMART_PTR_INTERNAL(_TClassName)
//           XTP_DEFINE_SMART_PTR_ARRAY_INTERNAL(_TClassName)
/////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------
// Parameters:
//     ClassName :  Name of the class used to define the smart pointer.
// Remarks:
//     Define smart pointer for the specified class as ClassNamePtr.
//     CXTPSmartPtrInternalT is used for this purpose.
// For example: XTP_DEFINE_SMART_PTR_INTERNAL(CCmdTarget) will be replaced
//     to: typedef CXTPSmartPtrInternalT<CCmdTarget> CCmdTargetPtr;
// -----------------------------------------------------------------------
#define XTP_DEFINE_SMART_PTR_INTERNAL(ClassName) \
	typedef CXTPSmartPtrInternalT<ClassName> ClassName##Ptr;

// -----------------------------------------------------------------------
// Parameters:
//     ClassName :  Name of the class used to define the array of smart pointers.
// Remarks:
//     Define smart pointers array for the specified class as ClassNamePtrArray.
//     CXTPInternalCollectionT is used for this purpose.
//     Also define smart pointer for array as ClassNamePtrArrayPtr.
// -----------------------------------------------------------------------

#define XTP_DEFINE_SMART_PTR_ARRAY_INTERNAL(ClassName) \
	typedef CXTPInternalCollectionT<ClassName> ClassName##PtrArray; \
	typedef CXTPSmartPtrInternalT<ClassName> ClassName##PtrArrayPtr;

//===========================================================================
// Remarks:
//     This internal class is used for semi-automatic incrementing
//          and decrementing reference to any object derived from CCmdTarget
//          or other class, which has InternalAddRef(), InternalRelease()
//          methods.
// See Also: macro XTP_DEFINE_SMART_PTR_INTERNAL(_TClassName)
//===========================================================================
template<class _TObject>
class CXTPSmartPtrInternalT
{
protected:
	//------------------------------------------------------------------------
	// Remarks:
	//     This class type definition
	//------------------------------------------------------------------------
	typedef CXTPSmartPtrInternalT<_TObject> Tthis;

	_TObject* m_pObject; // A pointer to a handled object
public:

	//-----------------------------------------------------------------------
	// Parameters:
	//     pObject             - Pointer to the handled object.
	//     bCallInternalAddRef - If this parameter is TRUE
	//                                pObject->InternalAddRef() will be
	//                                called in constructor.
	//                                By default this parameter is FALSE.
	// Summary:
	//     Default class constructor.
	// See Also: ~CXTPSmartPtrInternalT()
	//-----------------------------------------------------------------------
	CXTPSmartPtrInternalT(_TObject* pObject = NULL, BOOL bCallInternalAddRef = FALSE)
	{
		m_pObject = pObject;

		if (bCallInternalAddRef && m_pObject) {
			((CCmdTarget*)m_pObject)->InternalAddRef();
		}
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Copy class constructor.
	// See Also:
	//      CXTPSmartPtrInternalT(_TObject* pObject, BOOL bCallInternalAddRef)
	// Parameters:
	//     rSrc :  the source object reference.
	//-----------------------------------------------------------------------
	CXTPSmartPtrInternalT(const Tthis& rSrc)
	{
		m_pObject = (_TObject*)rSrc;
		if (m_pObject) {
			((CCmdTarget*)m_pObject)->InternalAddRef();
		}
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Default class destructor.
	// Remarks:
	//     Call InternalRelease() for the not NULL handled object.
	// See Also: CXTPSmartPtrInternalT constructors
	//-----------------------------------------------------------------------
	virtual ~CXTPSmartPtrInternalT() {
		if (m_pObject) {
			((CCmdTarget*)m_pObject)->InternalRelease();
		}
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Set new handled object.
	// Parameters:
	//     rSrc                 - Pointer to the new handled object.
	//     bCallInternalAddRef - If this parameter is TRUE
	//                           pObject->InternalAddRef() will be
	//                           called in constructor.
	//                           By default this parameter is FALSE.
	// Remarks:
	//     InternalRelease() for the previous not NULL handled object
	//     will be called. InternalAddRef() for the new not NULL handled
	//     object will be called.
	// See Also:
	//     operator=
	//-----------------------------------------------------------------------
	void SetPtr(_TObject* pObject, BOOL bCallInternalAddRef = FALSE)
	{
		_TObject* pObjOld = m_pObject;

		if (bCallInternalAddRef && pObject) {
			((CCmdTarget*)pObject)->InternalAddRef();
		}
		m_pObject = pObject;

		if (pObjOld) {
			((CCmdTarget*)pObjOld)->InternalRelease();
		}
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Get a handled object and set internal object member to NULL
	//          without call InternalRelease().
	// Returns:
	//     Pointer to the handled object.
	//-----------------------------------------------------------------------
	_TObject* Detach() {
		_TObject* pObj = m_pObject;
		m_pObject = NULL;
		return pObj;
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Get a handled object and set internal object member to NULL
	//          without call InternalRelease().
	// Returns:
	//     Pointer to the handled object.
	//-----------------------------------------------------------------------
	_TObject* GetInterface(BOOL bWithAddRef = TRUE) {
		if (bWithAddRef && m_pObject) {
			((CCmdTarget*)m_pObject)->InternalAddRef();
		}
		return m_pObject;
	};

	//-----------------------------------------------------------------------
	// Parameters:
	//     pNewObj  - Pointer to the new handled object.
	// Summary:
	//     Set new handled object.
	// Remarks:
	//     InternalRelease() for the previous not NULL handled object
	//          will be called.
	// Returns:
	//     Pointer to the new handled object.
	//-----------------------------------------------------------------------
	_TObject* operator=(_TObject* pNewObj)
	{
		_TObject* pObjOld = m_pObject;
		m_pObject = pNewObj;

		if (pObjOld) {
			((CCmdTarget*)pObjOld)->InternalRelease();
		}

		return pNewObj;
	};

	//-----------------------------------------------------------------------
	// Parameters:
	//     rSrc - Reference to the new handled object.
	// Summary:
	//     Set new handled object.
	// Remarks:
	//     InternalRelease() for the previous not NULL handled object
	//          will be called. InternalAddRef() for the new not NULL handled
	//          object will be called.
	// Returns:
	//     Reference to this class instance.
	//-----------------------------------------------------------------------
	const Tthis& operator=(const Tthis& rSrc) {
		_TObject* pObjOld = m_pObject;

		m_pObject = rSrc;

		if (m_pObject) {
			((CCmdTarget*)m_pObject)->InternalAddRef();
		}

		if (pObjOld) {
			((CCmdTarget*)pObjOld)->InternalRelease();
		}

		return rSrc;
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Get a handled object.
	// Returns:
	//     Pointer to the handled object.
	//-----------------------------------------------------------------------
	_TObject* operator->() const{
		ASSERT(m_pObject);
		return m_pObject;
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     Get a handled object.
	// Returns:
	//     Pointer to the handled object.
	//-----------------------------------------------------------------------
	operator _TObject*() const {
		return m_pObject;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Check is handled object equal NULL.
	// Returns:
	//     TRUE if handled object equal NULL, else FALSE.
	//-----------------------------------------------------------------------
	BOOL operator!() const {
		return !m_pObject;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Equal-to operator.
	// Parameters:
	//      pObj :  [in] Pointer to the other object.
	//      ptr2 :  [in] Smart pointer to the other object.
	// Remarks:
	//     This operator compare internal stored pointer and specified
	//          pointer.
	// Returns:
	//     TRUE if pointer are equal, else FALSE.
	//-----------------------------------------------------------------------
	BOOL operator==(const _TObject* pObj) const {
		return pObj == m_pObject;
	}
	// <COMBINE operator==>
	BOOL operator==(const Tthis& ptr2) const {
		return m_pObject == (_TObject*)ptr2;
	}
};

//===========================================================================
// Remarks:
//      This internal class is used as collection for pointers of objects,
//      derived from CCmdTarget or other class, which has InternalAddRef(),
//      InternalRelease() methods. InternalAddRef() is called for stored
//      object. InternalRelease() is called when object is removed.
//      Also this collection object derived from IXTPInternalUnknown.
// See Also:
//      CXTPSmartPtrInternalT, CXTPInternalUnknown
//===========================================================================
template<class _TObject>
class CXTPInternalCollectionT : public CArray<CXTPSmartPtrInternalT<_TObject>,
											  const CXTPSmartPtrInternalT<_TObject>& >
{
public:
	//------------------------------------------------------------------------
	// Remarks:
	//      Stored objects type definition.
	//------------------------------------------------------------------------
	typedef _TObject                        TObject;

	//------------------------------------------------------------------------
	// Remarks:
	//      Stored objects smart-pointer type definition.
	//------------------------------------------------------------------------
	typedef CXTPSmartPtrInternalT<_TObject> TObjectPtr;

	//--------------------------------------------------------------------
	// Summary:
	//      Default object constructor.
	//--------------------------------------------------------------------
	CXTPInternalCollectionT(){};

	//--------------------------------------------------------------------
	// Summary:
	//      Default object destructor.
	//--------------------------------------------------------------------
	virtual ~CXTPInternalCollectionT(){};
/*
	//--------------------------------------------------------------------
	// Summary:
	//      Get pointer to this object.
	// Parameters:
	//      bWithAddRef : [in]  If TRUE - InternalAddRef() is called before
	//                          returning pointer. It is TRUE by default.
	// Returns:
	//      Pointer to this object.
	// Remarks:
	//      This function gets pointer to this object and calls
	//      InternalAddRef() if specified.
	// See also:
	//      IXTPInternalUnknown.
	//--------------------------------------------------------------------
	CXTPInternalCollectionT<_TObject>* GetInterface(BOOL bWithAddRef = TRUE) {
		if(bWithAddRef) {
			InternalAddRef();
		}
		return this;
	};*/

	//--------------------------------------------------------------------
	// Summary:
	//      Get one of stored objects by index.
	// Parameters:
	//      nIndex      : [in] Zero based index of the stored object.
	//      bWithAddRef : [in] If TRUE - InternalAddRef() is called before
	//                           returning pointer. It is TRUE by default.
	// Returns:
	//      Pointer to one of stored objects by index.
	// Remarks:
	//      This function gets pointer to one of stored objects by index
	//      and calls InternalAddRef() if specified.
	// See also:
	//      CArray::GetAt, IXTPInternalUnknown.
	//--------------------------------------------------------------------
	virtual _TObject* GetAt(int nIndex, BOOL bWithAddRef = TRUE)
	{
		TObjectPtr& rPtr = ElementAt(nIndex);
		_TObject* pObj = rPtr.GetInterface(bWithAddRef);
		return pObj;
	}

	//--------------------------------------------------------------------
	// Summary:
	//      Add object pointer to the array.
	// Parameters:
	//      pObj        : [in] Pointer to the storing object.
	//      bCallAddRef : [in] If TRUE - InternalAddRef() is called before
	//                           adding pointer. It is TRUE by default.
	// Returns:
	//      Added object index.
	// Remarks:
	//      This method add a new element to the collection
	//      and calls InternalAddRef() if specified.
	// See also:
	//      CArray::Add, IXTPInternalUnknown.
	//--------------------------------------------------------------------
	virtual int AddPtr(_TObject* pObj, BOOL bCallAddRef = FALSE)
	{
		TObjectPtr ptrObj(pObj, bCallAddRef);
		return (int)Add(ptrObj);
	}
};

//===========================================================================
// Summary:
//     This class represents a simple array of typed elements.
//     It repeats CArray methods names and parameters as far as CArray behavior.
// Remarks:
//     This class derived from CCmdTarget. This allow to use it as base class
//     for collection which has features for ActiveX support.
// See Also: CArray, CCmdTarget.
//===========================================================================
template<class TYPE, class ARG_TYPE, class OLE_TYPE = long>
class CXTPArrayT : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-----------------------------------------------------------------------
	CXTPArrayT();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPArrayT();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the number of elements
	//     in the collection.
	// Remarks:
	//     Call this method to retrieve the number of elements in the array.
	//     Because indexes are zero-based, the size is 1 greater than
	//     the largest index.
	// Returns:
	//     The number of items in the collection.
	//-----------------------------------------------------------------------
	virtual int GetSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to change the array size.
	// Parameters:
	//     nNewSize - An int that contains the new array size.
	//     nGrowBy  - An int that contains a number of elements to grow
	//                internal data storage if need.
	// Remarks:
	//     Argument nNewSize contains the new array size (number of elements).
	//     Must be greater than or equal to 0.
	//-----------------------------------------------------------------------
	virtual void SetSize(int nNewSize, int nGrowBy = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to remove all of the elements from
	//     the array.
	// See Also: RemoveAt
	//-----------------------------------------------------------------------
	virtual void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns an element at the specified
	//     numeric index.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetSIze.
	// Remarks:
	//     Returns the array element at the specified index.
	// Returns:
	//     The element currently at this index.
	//-----------------------------------------------------------------------
	virtual TYPE GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set a new element to the position
	//     specified in the nIndex parameter.
	// Parameters:
	//     nIndex      - An integer index that is greater than or equal
	//                   to 0 and less than the value returned by
	//                   GetCount.
	//     newElement  - A new value for the specified index.
	// Remarks:
	//     Use this method to set the specified element to the
	//     position specified in nIndex parameter.
	// See Also: GetAt, Add, InsertAt
	//-----------------------------------------------------------------------
	virtual void SetAt(int nIndex, ARG_TYPE newElement);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns a reference to element at the
	//     specified numeric index.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetSIze.
	// Returns:
	//     The array element reference at the specified index.
	//-----------------------------------------------------------------------
	virtual TYPE& ElementAt(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to Direct Access to the element data.
	//     May return NULL.
	// Returns:
	//     The pointer to allocated array data or NULL.
	//-----------------------------------------------------------------------
	virtual const TYPE* GetData() const;
	virtual TYPE* GetData(); //<COMBINE GetData>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set a new element to the position
	//     specified in the nIndex parameter.
	//     Potentially growing the array.
	// Parameters:
	//     nIndex      - An integer index that is greater than or equal
	//                   to 0 and less than the value returned by
	//                   GetCount.
	//     newElement  - A new value for the specified index.
	// Remarks:
	//     Use this method to set the specified element to the
	//     position specified in nIndex parameter.
	// See Also: SetAt, Add, InsertAt
	//-----------------------------------------------------------------------
	virtual void SetAtGrow(int nIndex, ARG_TYPE newElement);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function adds a new element to the end of the array.
	// Parameters:
	//     newElement  - A new value for the specified index.
	//-----------------------------------------------------------------------
	virtual int Add(ARG_TYPE newElement);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to add a new elements to the end
	//     of the array.
	// Parameters:
	//     src - A reference to an array of TYPE objects to add to this array.
	// Remarks:
	//     Use this method to add the elements from the specified
	//     array to the end of the collection.
	// See Also: GetAt, Add, InsertAt, SetAt
	//-----------------------------------------------------------------------
	virtual int Append(const CXTPArrayT& src);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to copy elements from other array.
	// Parameters:
	//     src - A reference to an array of TYPE objects to add to this array.
	// Remarks:
	//     Use this method to copy elements from the specified
	//     array to this collection.
	// See Also: GetAt, Add, InsertAt, SetAt
	//-----------------------------------------------------------------------
	virtual void Copy(const CXTPArrayT& src);

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator returns an element at the specified
	//     numeric index.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetSIze.
	// Remarks:
	//     Returns the array element at the specified index.
	// Returns:
	//     The element currently at this index.
	//-----------------------------------------------------------------------
	TYPE operator[](int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator returns a reference to element at the
	//     specified numeric index.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetSIze.
	// Returns:
	//     The array element reference at the specified index.
	//-----------------------------------------------------------------------
	TYPE& operator[](int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to insert a new element at the
	//     position specified in the nIndex parameter.
	// Parameters:
	//     nIndex      - An integer index that is greater than or equal
	//                   to 0 and less than the value returned by
	//                   GetCount().
	//     newElement  - A new value for the specified index.
	//     nCount      - A number of elements to be inserted. By default is 1.
	//-----------------------------------------------------------------------
	virtual void InsertAt(int nIndex, ARG_TYPE newElement, int nCount = 1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function removes an element at the specified index
	//     from the array.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetCount().
	//     nCount - The number of elements to remove.
	//-----------------------------------------------------------------------
	virtual void RemoveAt(int nIndex, int nCount = 1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to insert a new elements at the
	//     position specified in the nIndex parameter.
	// Parameters:
	//     nStartIndex - An integer index that is greater than or equal
	//                   to 0 and less than the value returned by
	//                   GetCount().
	//     pNewArray   - A pointer to the array which contains values to insert.
	//-----------------------------------------------------------------------
	virtual void InsertAt(int nStartIndex, CXTPArrayT* pNewArray);

protected:
	CArray<TYPE, ARG_TYPE> m_arElements; // Elements storage.

protected:
	// OLE collection implementation helpers for macros DECLARE_ENUM_VARIANT_EX(derived-class, long);

	//{{AFX_CODEJOCK_PRIVATE
	virtual long OleGetItemCount();

	virtual OLE_TYPE OleGetItem(long nIndex);
	virtual void OleSetItem(long nIndex, OLE_TYPE oleValue);

	virtual void OleAdd(OLE_TYPE otElement);
	virtual void OleRemove(long nIndex);
	virtual void OleRemoveAll();
	//}}AFX_CODEJOCK_PRIVATE
};

/////////////////////////////////////////////////////////////////////////////
template<class TYPE, class ARG_TYPE, class OLE_TYPE>
CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::CXTPArrayT()
{
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::~CXTPArrayT()
{
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE int CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::GetSize() const
{
	return (int)m_arElements.GetSize();
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::SetSize(int nNewSize, int nGrowBy)
{
	m_arElements.SetSize(nNewSize, nGrowBy);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::RemoveAll()
{
	m_arElements.RemoveAll();
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE TYPE CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::GetAt(int nIndex) const
{
	return m_arElements.GetAt(nIndex);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::SetAt(int nIndex, ARG_TYPE newElement)
{
	m_arElements.SetAt(nIndex, newElement);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE TYPE& CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::ElementAt(int nIndex)
{
	return m_arElements.ElementAt(nIndex);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE const TYPE* CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::GetData() const
{
	return m_arElements.GetData();
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE TYPE* CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::GetData()
{
	return m_arElements.GetData();
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::SetAtGrow(int nIndex, ARG_TYPE newElement)
{
	m_arElements.SetAtGrow(nIndex, newElement);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE int CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::Add(ARG_TYPE newElement)
{
	return (int)m_arElements.Add(newElement);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE int CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::Append(const CXTPArrayT& src)
{
	return (int)m_arElements.Append(src.m_arElements);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::Copy(const CXTPArrayT& src)
{
	m_arElements.Copy(src.m_arElements);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE TYPE CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::operator[](int nIndex) const
{
	return m_arElements[nIndex];
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE TYPE& CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::operator[](int nIndex)
{
	return m_arElements[nIndex];
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::InsertAt(int nIndex, ARG_TYPE newElement, int nCount)
{
	m_arElements.InsertAt(nIndex, newElement, nCount);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::RemoveAt(int nIndex, int nCount)
{
	m_arElements.RemoveAt(nIndex, nCount);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
AFX_INLINE void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::InsertAt(int nStartIndex, CXTPArrayT* pNewArray)
{
	ASSERT(pNewArray);
	if (pNewArray)
		m_arElements.InsertAt(nStartIndex, &(pNewArray->m_arElements));
}

//===========================================================================
template<class TYPE, class ARG_TYPE, class OLE_TYPE>
OLE_TYPE CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::OleGetItem(long nIndex)
{
	if (nIndex < 0 || nIndex >= (long)GetSize())
		AfxThrowOleException(DISP_E_BADINDEX);

	return (OLE_TYPE)GetAt(nIndex);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::OleSetItem(long nIndex, OLE_TYPE oleValue)
{
	if (nIndex < 0 || nIndex >= (long)GetSize())
		AfxThrowOleException(DISP_E_BADINDEX);

	SetAt(nIndex, (ARG_TYPE)oleValue);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
long CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::OleGetItemCount()
{
	return (long)m_arElements.GetSize();
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::OleAdd(OLE_TYPE otElement)
{
	Add((TYPE)otElement);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::OleRemove(long nIndex)
{
	if (nIndex < 0 || nIndex >= (long)GetSize())
		AfxThrowOleException(DISP_E_BADINDEX);

	RemoveAt(nIndex);
}

template<class TYPE, class ARG_TYPE, class OLE_TYPE>
void CXTPArrayT<TYPE, ARG_TYPE, OLE_TYPE>::OleRemoveAll()
{
	RemoveAll();
}

#endif // !defined(_XtpSmartPtrInternal_H__)
