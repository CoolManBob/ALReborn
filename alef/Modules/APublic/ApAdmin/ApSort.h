#ifndef __APSORT_H__
#define __APSORT_H__
/*
	정렬을 간편하게 위해 deque 를 랩핑한 클래스
*/
#pragma warning( disable:4786)
#include <deque>

#include "ApMutualEx.h"
#include <algorithm>
template < class _T_ >
class ApSort
{
private:
	typedef std::deque < _T_ > ApSortList;
	typedef typename ApSortList::iterator ApSortListIter;
	ApSortList m_SortList;
	ApSortListIter m_SortListIter;
public:
	ApSort(){}
	~ApSort()
	{
		m_SortList.clear();
	}

	void Clear()
	{
		m_SortList.clear();
	}

	BOOL Insert( _T_ Data )
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return FALSE;
		m_SortList.push_back( Data );
		return TRUE;
	}
	BOOL Remove( _T_ Data )
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return FALSE;
		ApSortListIter Iter_End( m_SortList.end() );
		for (ApSortListIter Iter = m_SortList.begin(); Iter != Iter_End; ++Iter)
		{
			if (Data == *Iter)
			{
				m_SortList.erase(Iter);
				return TRUE;
			}
		}
		return FALSE;
	}
	VOID DeleteAllMemory()
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return;
		ApSortListIter Iter_End( m_SortList.end() );
		for (ApSortListIter Iter = m_SortList.begin(); Iter != Iter_End; ++Iter)
		{
			_T_ Data = (*Iter);
			delete Data;
		}
		m_SortList.clear();

	}
	_T_ GetHead()
	{
		m_SortListIter = m_SortList.begin();
		if (m_SortListIter == m_SortList.end()) return NULL;
		return *m_SortListIter;
	}
	BOOL IsEnd()
	{
		return ( m_SortListIter == m_SortList.end() );
	}
	_T_ GetNext()
	{
		if (m_SortListIter == m_SortList.end()) return NULL;
		++m_SortListIter;
		if (m_SortListIter == m_SortList.end()) return NULL;
		return *m_SortListIter;
	}
	_T_ Erase()
	{
		if (m_SortListIter == m_SortList.end()) return NULL;
		m_SortListIter = m_SortList.erase(m_SortListIter);
		if (m_SortListIter == m_SortList.end()) return NULL;
		return *m_SortListIter;
	}
	size_t SizeSortList()
	{
		return m_SortList.size();
	}
	template < class _SORT_ >
	BOOL Sort( _SORT_  SortObject )
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return FALSE;
		std::sort( m_SortList.begin(), m_SortList.end(), SortObject );
		return TRUE;
	}

	ApMutualEx m_Mutex;
};

#endif