// XTPCalendarEvents.h: CXTPCalendarEvents template.
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
#if !defined(_XTPCALENDAREVENTS_H__)
#define _XTPCALENDAREVENTS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//}}AFX_CODEJOCK_PRIVATE



//{{AFX_CODEJOCK_PRIVATE
class CXTPCalendarEvent;
//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//      This class represents a simple array collection of CXTPCalendarEvent
//      objects.
// Remarks:
//      Array indexes always start at position 0.
//
//      As with a C array, the access time for indexed element of this
//      array is constant and is independent of the array size.
//
//      Methods Add(), SetAt(), and Append() call InternalAddRef() for the
//      added event.
//      Methods Get(), and Find() do not call InternalAddRef() for the returned
//      event.
//      Methods RemoveAt(), RemoveBy(), RemoveAll(), and the destructor call
//      InternalRelease() for the removed events.
//
// See Also: CArray overview, CXTPCalendarEvent overview.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarEvents : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarEvents)
	//}}AFX_CODEJOCK_PRIVATE
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection constructor.
	// See Also: ~CXTPCalendarEvents()
	//-----------------------------------------------------------------------
	CXTPCalendarEvents();

	//-----------------------------------------------------------------------
	// Summary:
	//     Default collection destructor.
	// Remarks:
	//     Handles member item deallocation. Decreases reference of all
	//     stored CXTPCalendarEvent objects.
	// See Also: RemoveAll()
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarEvents();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to add a new event element to the
	//     end of an array.
	// Parameters:
	//     pNewEvent   - The event element to add to this array.
	//     bWithAddRef - If this parameter is TRUE, then InternalAddRef()
	//                   is called for the pNewEvent, otherwise InternalAddRef()
	//                   is not called. The default value is TRUE.
	// Remarks:
	//     Use this method to add the specified event pointer to the end
	//     of the events collection. Reference to the new object is
	//     increased or decreased depending on the value of the bWithAddRef parameter.
	// Example:
	// <code>
	// // CXTPCalendarEvents - derived class
	// CXTPCalendarEvents* pList = new CXTPCalendarEvents();
	// CXTPCalendarEventPtr ptrEvent1 = new CXTPCalendarEvent()
	//
	// // Add() will call InernalAddRef()
	// pList->Add(ptrEvent1);
	//
	// // Add() will not call InernalAddRef()
	// pList->Add(new CXTPCalendarEvent(), FALSE);
	//
	// // GetAt() will call InernalAddRef()
	// CXTPCalendarEventPtr ptrEvent0 = pList->GetAt(0, TRUE);
	//
	// // GetAt() will not call InernalAddRef()
	// CXTPCalendarEvent* pEvent1 = pList->GetAt(1);
	//
	// ASSERT(2 == pList->GetCount());
	//
	// //RemoveAll() will call InernalRelease() for all objects
	// pList->RemoveAll();
	//
	// ASSERT(0 == pList->GetCount());
	// </code>
	// See Also: GetAt, RemoveAll, GetCount, CXTPCalendarEvents overview,
	//           CXTPCalendarEvent overview.
	//-----------------------------------------------------------------------
	void Add(CXTPCalendarEvent* pNewEvent, BOOL bWithAddRef = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Adds a new events element to the end of an array.
	// Parameters:
	//     pEventsArray - The pointer to the collection of event elements
	//     to add to this array.
	// Remarks:
	//     Use this method to add event elements from the specified
	//     array to the end of the events collection. Reference to the
	//     new object is increased by default. This depends on the array
	//     constructor parameter. See array constructor
	//     CXTPCalendarEvents(BOOL bWithAddRefRelease = TRUE);
	// Example:
	// <code>
	// // CXTPCalendarEvents - derived class
	// CXTPCalendarEvents arEvents1;
	// CXTPCalendarEventsPtr ptrEvents2 = new CXTPCalendarEvents();
	// ...
	// ptrEvents2->Append(&arEvents1);
	// </code>
	// See Also: CXTPCalendarEvents overview, GetAt, RemoveAll, GetCount,
	//           constructor CXTPCalendarEvents, CXTPCalendarEvents overview,
	//           CXTPCalendarEvents overview, CXTPCalendarEvent overview,
	//           CXTPCalendarEvent overview
	//-----------------------------------------------------------------------
	void Append(CXTPCalendarEvents* pEventsArray);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get an event at the specified numeric index.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetCount.
	//     bWithAddRef - If this parameter is TRUE the InternalAdRef()
	//                   will be called for the returned object,
	//                   otherwise it will not be called.
	//                   The default value is FALSE.
	// Remarks:
	//     Reference to the returned object is increased or not depending on
	//     bWithAddRef parameter.
	//     Returns the array element at the specified index.
	// Example: See example for CXTPCalendarEvents::Add method.
	// Returns:
	//     The pointer to the CXTPCalendarEvent element currently at this
	//     index.
	//-----------------------------------------------------------------------
	CXTPCalendarEvent* GetAt(int nIndex, BOOL bWithAddRef = FALSE) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set a new event element to the
	//     position specified in the nIndex parameter.
	// Parameters:
	//     nIndex      - An integer index that is greater than or equal
	//                   to 0 and less than the value returned by
	//                   GetCount.
	//     pEvent      - The event element to add to this array.
	//     bWithAddRef - If this parameter is TRUE,  then InternalAddRef()
	//                   is called for the pEvent object,
	//                   otherwise InternalAddRef() is not called.
	// Remarks:
	//     Use this method to set the specified event pointer to the
	//     position specified in the nIndex parameter.
	//     Reference to the previous object is decreased by default. This
	//     depends on the array constructor parameter. See the array constructor
	//     CXTPCalendarEvents(BOOL bWithAddRefRelease = TRUE);
	//     Reference to the new object is increased or decreased depending
	//     on the bWithAddRef parameter.
	// Example:
	// <code>
	// // CXTPCalendarEvents - derived class
	// CXTPCalendarEvents* pList = new CXTPCalendarEvents();
	// CXTPCalendarEventPtr ptrEvent1 = new CXTPCalendarEvent()
	//
	// pList->Add(ptrEvent1);
	// pList->SetAt(0, new CXTPCalendarEvent(), FALSE);
	// </code>
	// See Also: CXTPCalendarEvents overview,
	//           constructor CXTPCalendarEvents, CXTPCalendarEvents overview,
	//           CXTPCalendarEvents overview, CXTPCalendarEvent overview,
	//           CXTPCalendarEvent overview
	//-----------------------------------------------------------------------
	void SetAt(int nIndex, CXTPCalendarEvent* pEvent, BOOL bWithAddRef = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Inserts an element at a specified index.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and may be greater than the value returned by GetCount.
	//     pEvent      - Pointer to the event to insert.
	//     bWithAddRef - Set this value to TRUE to increment the reference count of the inserted object,
	//                   FALSE to insert object without incrementing reference count.
	//                   Default value is TRUE.
	// Remarks:
	//     Inserts one element at a specified index in an array. In the process,
	//     it shifts up (by incrementing the index) the existing element at
	//     this index, and it shifts up all the elements above it.
	// See Also: CXTPCalendarEvents overview
	//-----------------------------------------------------------------------
	void InsertAt(int nIndex, CXTPCalendarEvent* pEvent, BOOL bWithAddRef = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes an element of the specified index from this array.
	// Parameters:
	//     nIndex - An integer index that is greater than or equal to 0
	//              and less than the value returned by GetCount.
	// Remarks:
	//     Removes the pointer from this array. Reference to the removed
	//     object is decreased by default.
	// See Also: CXTPCalendarEvents overview
	//-----------------------------------------------------------------------
	void RemoveAt(int nIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the number of
	//     CXTPCalendarEvent elements in this collection.
	// Remarks:
	//     Call this method to retrieve the number of elements in the array.
	//     Because indexes are zero-based, the size is 1 greater than
	//     the largest index.
	// Example: See example for CXTPCalendarEvents::Add method.
	// Returns:
	//     An int that contains the number of items in the collection.
	// See Also: CXTPCalendarEvents overview
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Set size of the array.
	// Parameters:
	//     nNewSize - A new array size (elements count).
	//     nGrowBy  - Amount of elements to grow array when Add and InsertAt
	//                methods used. -1 (by default) means using default
	//                parameter value.
	// See Also:
	//     GetCount, RemoveAll
	//-----------------------------------------------------------------------
	void SetSize(int nNewSize, int nGrowBy = -1);

	//-----------------------------------------------------------------------
	// Summary:
	//     This function is used to remove all of the elements from this array.
	// Remarks:
	//     Removes all of the pointers from this array. Reference to
	//     removed objects are decreased by default.
	// Example: See example for CXTPCalendarEvents::Add method.
	// See Also: CXTPCalendarEvents overview
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is used to find an event using the EventID in this array.
	// Parameters:
	//     dwEventID - A DWORD that contains the unique event ID.
	//     pFEvent - A pointer to a CXTPCalendarEvent object that
	//                 contains the event object.
	// Remarks:
	// When using dwEventID -
	//     This method searches this collection for the first match of
	//     an event object with event ID equal to dwEventID.
	// When using pFEvent -
	//     This method searches this collection for the first match of
	//     a event object with identifiers equal to identifiers of
	//     the pFEvent. If there are no recurrence events, then these
	//     are Event IDs. For recurrence events, then these are
	//     RecurrencePatternID, Start/End times, or
	//     RException_StartTimeOrig/RException_EndTimeOrig.
	//     For details see implementation CXTPCalendarEvent::IsEqualIDs()
	// Example:
	// <code>
	// // CXTPCalendarEvents - derived class
	// CXTPCalendarEvents arEvents;
	// CXTPCalendarEvent* pEvent;
	// ...
	// if (arEvents.Find(pEvent->GetEventID()) < 0) {
	//     arEvents.Add(pEvent);
	// }
	// </code>
	// Returns:
	//     The zero-based index of the first event in the collection
	//     that matches the requested dwEventID.
	//     -1 if the event is not found.
	// See Also: CXTPCalendarEvents overview,
	//           constructor CXTPCalendarEvents, CXTPCalendarEvents overview,
	//           CXTPCalendarEvents overview, CXTPCalendarEvent overview,
	//           CXTPCalendarEvent overview
	//-----------------------------------------------------------------------
	int Find(CXTPCalendarEvent* pFEvent) const;
	int Find(DWORD dwEventID) const; // <combine CXTPCalendarEvents::Find@CXTPCalendarEvent*@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to find an event using the EventID
	//     in this array.
	// Parameters:
	//     dwEventID - A DWORD that contains the unique event ID.
	//     pFEvent - A CXTPCalendarEvent pointer that contains the
	//                 event object to compare.
	// Remarks:
	// When using dwEventID -
	//     This method searches this collection for the first match of
	//     a event object with event ID equal to dwEventID.
	//     Reference to the returned object is not increased.
	// When using pFEvent -
	//     This method searches this collection for the first match of
	//     a event object with identifiers equal to identifiers of
	//     the pFEvent. In the case of no recurrence events these
	//     are EventIDs. In the case of recurrence events these are
	//     RecurrencePatternIDs, Start/End times or
	//     RException_StartTimeOrig/RException_EndTimeOrig.
	//     For details see implementation CXTPCalendarEvent::IsEqualIDs()
	//     Reference to the returned object is not increased by default.
	// Example:
	// <code>
	// // CXTPCalendarEvents - derived class
	// CXTPCalendarEvents arEvents;
	// CXTPCalendarEvent* pEvent;
	// ...
	// CXTPCalendarEvent* pEvent2;
	// pEvent2 = arEvents.Find(pEvent->GetEventID());
	// if (pEvent2) {
	//     pEvent2.Update(pEvent);
	// }
	// </code>
	// Returns:
	//     The pointer to the first event in the collection
	//     that matches the requested dwEventID;
	//     NULL if the event is not found.
	// See Also: CXTPCalendarEvents overview, CXTPCalendarEvent overview,
	//-----------------------------------------------------------------------
	CXTPCalendarEvent* FindEvent(DWORD dwEventID) const;
	CXTPCalendarEvent* FindEvent(CXTPCalendarEvent* pFEvent) const; // <combine CXTPCalendarEvents::FindEvent@DWORD@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to find and remove an event using
	//     EventID in this array.
	// Parameters:
	//     dwEventID - A DWORD containing a unique event ID.
	// Remarks:
	//     This method searches this collection for the first match of
	//     an event object with the event ID equal to dwEventID and removes
	//     the object from the collection..
	// Example:
	// <code>
	// // CXTPCalendarEvents - derived class
	// CXTPCalendarEvents arEvents;
	// DWORD dwEventIDToRemove;
	// ...
	// if (arEvents.RemoveByID(dwEventIDToRemove) < 0) {
	//  ...
	// }
	// </code>
	// Returns:
	//     TRUE - if the event is found and removed.
	//     FALSE - if the event is not found.
	// See Also: CXTPCalendarEvents overview,
	//           constructor CXTPCalendarEvents, CXTPCalendarEvents overview,
	//           CXTPCalendarEvents overview, CXTPCalendarEvent overview,
	//           CXTPCalendarEvent overview
	//-----------------------------------------------------------------------
	BOOL RemoveByID(DWORD dwEventID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Clone events in the collection.
	// Remarks:
	//     This function pointer is used to replace events objects to their clones.
	//     If there is not enough memory to clone some events  they are
	//     removed from the collection.
	// See Also: CXTPCalendarEvent::CloneEvent()
	//-----------------------------------------------------------------------
	void CloneEvents();

	//-----------------------------------------------------------------------
	// Summary:
	//     Define a function pointer for comparing events.
	// Remarks:
	//     This function pointer is used in the Sort method.
	// See Also: Sort method, CompareEvents_ForView function,
	//           CompareEvents_ByID function
	//-----------------------------------------------------------------------
	typedef int (_cdecl* T_CompareFunc)(const CXTPCalendarEvent** ppEv1, const CXTPCalendarEvent** ppEv2);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to sort events in this array.
	// Parameters:
	//     pCompareFunc - A T_CompareFunc function pointer that is used
	//                    to compare events.
	// Remarks:
	//     This method sorts events in this collection using specified
	//     compare events function. The QuickSort algorithm is used.
	// Example:
	// <code>
	// CXTPCalendarEvents - derived class
	// CXTPCalendarEvents arEvents;
	// ...
	// arEvents.Sort(CompareEvents_ForView);
	// ...
	// </code>
	// See Also: CXTPCalendarEvents overview,
	//           CXTPCalendarEvents overview, CXTPCalendarEvents overview,
	//           CXTPCalendarEvent overview, CXTPCalendarEvent overview,
	//           CompareEvents_ForView function, CompareEvents_ByID function,
	//           qsort() function in the stdlib.h
	//-----------------------------------------------------------------------
	void Sort(T_CompareFunc pCompareFunc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to compare specified events so that
	//     the events are displayed properly in the event's views.
	// Parameters:
	//     ppEv1 - Pointer to the first event.
	//     ppEv2 - Pointer to the second event.
	// Remarks:
	//     This function is used as a parameter for the Sort method to sort
	//     events in the order needed to display the events in the event's views.
	// Example: See example for CXTPCalendarEvents::Sort method.
	// See Also: method CXTPCalendarEvents::Sort overview,
	//           CompareEvents_ByID function
	//-----------------------------------------------------------------------
	static int _cdecl CompareEvents_ForView(const CXTPCalendarEvent** ppEv1, const CXTPCalendarEvent** ppEv2);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to compare specified events by
	//     the event's IDs.
	// Parameters:
	//     ppEv1 - Pointer to the first event pointer.
	//     ppEv2 - Pointer to the second event pointer.
	// Remarks:
	//     This function is used as a parameter for the Sort method to sort
	//     events using EventIDs in ascending order.  This is useful for
	//     performing a fast search for an event ID.
	// Example: See example for CXTPCalendarEvents::Sort method.
	// See Also: method CXTPCalendarEvents::Sort overview,
	//           CompareEvents_ForView function
	//-----------------------------------------------------------------------
	static int _cdecl CompareEvents_ByID(const CXTPCalendarEvent** ppEv1, const CXTPCalendarEvent** ppEv2);

protected:
	CArray<CXTPCalendarEvent*, CXTPCalendarEvent*> m_arEvents; // An internal storage for CXTPCalendarEvent pointers.

};

/////////////////////////////////////////////////////////////////////////////


AFX_INLINE int CXTPCalendarEvents::GetCount() const {
	return (int)m_arEvents.GetSize();
}

AFX_INLINE void CXTPCalendarEvents::SetSize(int nNewSize, int nGrowBy)
{
	m_arEvents.SetSize(nNewSize, nGrowBy);
}

AFX_INLINE BOOL CXTPCalendarEvents::RemoveByID(DWORD dwEventID) {
	int nFIndex = Find(dwEventID);
	if (nFIndex >= 0) {
		RemoveAt(nFIndex);
	}
	return (nFIndex >= 0);
}

#endif // !defined(_XTPCALENDAREVENTS_H__)
