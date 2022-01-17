/******************************************************************************

For MT-SAFE and/or Easy Use of STL Classes, kelovon

******************************************************************************/

#ifndef _AU_EZ_STL_H_
#define _AU_EZ_STL_H_

#include <windows.h>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <functional>
#include "ApMutualEx.h"

#pragma warning(disable:4786)

using namespace std;

typedef ApCriticalSection	mt_lock;

//class mt_lock
//{
//public:
//	mt_lock()
//	{
//		InitializeCriticalSection(&m_csCriticalSection);
//	}
//
//	~mt_lock()
//	{
//		DeleteCriticalSection(&m_csCriticalSection);
//	}
//
//	inline void Lock()
//	{
//		EnterCriticalSection(&m_csCriticalSection);
//	}
//
//#if (_WIN32_WINNT >= 0x0400)
//	inline BOOL TryLock()
//	{
//		/*
//		BOOL TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
//
//		Return Values
//		- If the critical section is successfully entered or the current
//		thread already owns the critical section, the return value is nonzero.
//		- If another thread already owns the critical section,
//		the return value is zero.
//		*/
//		return TryEnterCriticalSection(&m_csCriticalSection);
//	}
//#endif	// (_WIN32_WINNT >= 0x0400)_endif
//
//	inline void Unlock()
//	{
//		LeaveCriticalSection(&m_csCriticalSection);
//	}
//
//public:
//	CRITICAL_SECTION		m_csCriticalSection;
//};

// mt_auto_lock 변수 생성 후, scope를 빠져나오기 전에
// mt_lock이 invalidate되는 경우 cancel_unlock_on_exit()을 반드시 호출해야함.
//class mt_auto_lock
//{
//public:
//	mt_auto_lock(mt_lock *i_pMTLock)
//	{
//		m_pMTLock = i_pMTLock;
//		m_pMTLock->Lock();
//	}
//
//	~mt_auto_lock()
//	{
//		if (m_pMTLock != NULL)
//		{
//			m_pMTLock->Unlock();
//		}
//	}
//
//	void unlock_auto_lock(void)
//	{
//		if(m_pMTLock)
//		{
//			m_pMTLock->Unlock();
//		}
//		m_pMTLock = NULL;
//	}
//
//public:
//	mt_lock	*m_pMTLock;
//};

template<class _K, class _Ty, class _Pr = less<_K>, class _A = allocator<_Ty> >
class AuMtMap : public map< _K, _Ty, _Pr, _A >, public mt_lock
{
public:
	// Functions with Lock
	inline void ClearLock()
	{
		Lock();
		clear();
		Unlock();
	}

	inline BOOL InsertLock(const _K& key, const _Ty& value)
	{
		pair<map< _K, _Ty, _Pr, _A >::iterator, bool> res;

		Lock();
		res = insert(pair<_K, _Ty>(key, value));
		Unlock();

		return res.second;
	}

	inline BOOL DeleteLock(const _K& key)
	{
		int nDeleted;

		Lock();
		nDeleted = erase(key);
		Unlock();

		return (nDeleted!=0)?TRUE:FALSE;
	}

	inline BOOL DeleteLock(const _K& key, _Ty& value)
	{
		map< _K, _Ty, _Pr, _A >::iterator itr;

		Lock();
		itr = find(key);
		if (itr == end())
		{
			Unlock();
			return FALSE;
		}

		value = itr->second;
		erase(itr);
		Unlock();

		return TRUE;
	}

	inline BOOL DeleteLock(iterator it)
	{
		iterator itRet;
		BOOL bRet = FALSE;

		Lock();
		itRet = erase(it);
		if(itRet != end())
		{
			bRet = TRUE;
		}
		Unlock();

		return bRet;
	}

	inline _Ty FindLock(const _K& key)
	{
		map< _K, _Ty, _Pr, _A >::iterator itr;
		_Ty retTy = NULL;

		Lock();
		itr = find(key);
		if (itr != end())
		{
			retTy = itr->second;
		}
		Unlock();

		return retTy;
	}


	///////////////////////////////////////////////////////////////////////////////
	/// \fn			inline _Ty PopLock(const _K& key)
	/// \brief		
	/// \author		cmkwon
	/// \date		2004-10-21 ~ 2004-10-21
	/// \warning	객체가 value로 사용되면 안된다
	///				value는 Pointer 사용만 가능함
	///
	/// \param		
	/// \return		
	///////////////////////////////////////////////////////////////////////////////
	inline _Ty PopLock(const _K& key)
	{
		map< _K, _Ty, _Pr, _A >::iterator itr;
		_Ty	ty = NULL;

		Lock();
		itr = find(key);
		if (itr != end())
		{
			ty = itr->second;
			erase(itr);
		}
		Unlock();
		return ty;
	}

	inline _Ty* FindLock_Ptr(const _K& key)
	{
		map< _K, _Ty, _Pr, _A >::iterator itr;
		_Ty *pRetTy = NULL;

		Lock();
		itr = find(key);
		if (itr != end())
		{
			pRetTy = &itr->second;
		}
		Unlock();

		return pRetTy;
	}

	// Functions without Lock
	inline BOOL InsertNoLock(const _K& key, const _Ty& value)
	{
		pair<map< _K, _Ty, _Pr, _A >::iterator, bool> res;

		res = insert(pair<_K, _Ty>(key, value));

		return res.second;
	}

	inline BOOL DeleteNoLock(const _K& key)
	{
		int nDeleted;

		nDeleted = erase(key);

		return (nDeleted!=0)?TRUE:FALSE;
	}

	inline BOOL DeleteNoLock(const _K& key, _Ty& value)
	{
		map< _K, _Ty, _Pr, _A >::iterator itr;

		itr = find(key);
		if (itr == end())
		{
			return FALSE;
		}

		value = itr->second;
		erase(itr);

		return TRUE;
	}

	inline _Ty FindNoLock(const _K& key)
	{
		map< _K, _Ty, _Pr, _A >::iterator itr;

		itr = find(key);

		if (itr == end())
		{
			return NULL;
		}
		return itr->second;
	}
};

template<class _K, class _Ty, class _Pr = less<_K>,	class _A = allocator<_Ty> >
class mt_multimap : public multimap< _K, _Ty, _Pr, _A >, public mt_lock
{
public:
	// Functions with Lock
	inline void ClearLock()
	{
		Lock();
		clear();
		Unlock();
	}

	inline void InsertLock(const _K& key, const _Ty& value)
	{
		Lock();
		insert(value_type(key, value));
		Unlock();
	}

	inline BOOL DeleteLock(const _K& key)
	{
		int nDeleted;

		Lock();
		nDeleted = erase(key);
		Unlock();

		return (nDeleted!=0)?TRUE:FALSE;
	}

	// Functions without Lock
	inline void InsertNoLock(const _K& key, const _Ty& value)
	{
		insert(value_type(key, value));
	}

	inline BOOL DeleteNoLock(const _K& key)
	{
		int nDeleted;

		nDeleted = erase(key);

		return (nDeleted!=0)?TRUE:FALSE;
	}
};

template<class _K, class _Pr = less<_K>, class _A = allocator<_K> >
class mt_set : public set<_K, _Pr, _A>, public mt_lock
{
public:
	// Functions with Lock
	inline void ClearLock()
	{
		Lock();
		clear();
		Unlock();
	}

	inline BOOL InsertLock(const _K& key)
	{
		pair<set< _K, _Pr, _A >::iterator, bool> res;

		Lock();
		res = insert(key);
		Unlock();

		return res.second;
	}

	inline BOOL DeleteLock(const _K& key)
	{
		int nDeleted;

		Lock();
		nDeleted = erase(key);
		Unlock();

		return (nDeleted!=0)?TRUE:FALSE;
	}

	inline BOOL DeleteLock(iterator it)
	{
		iterator itRet;
		BOOL bRet = FALSE;
		Lock();
		itRet = erase(it);
		if(itRet != end())
		{
			bRet = TRUE;
		}
		Unlock();

		return bRet;
	}

	inline _K FindLock(const _K& key)
	{
		set< _K, _Pr, _A >::iterator itr;
		_K retTy = NULL;

		Lock();
		itr = find(key);
		if (itr != end())
		{
			retTy = *itr;
		}		
		Unlock();

		return retTy;
	}

	inline _K* FindLock_Ptr(const _K& key)
	{
		set< _K, _Pr, _A >::iterator itr;
		_K *pRetTy = NULL;

		Lock();
		itr = find(key);
		if (itr != end())
		{
			pRetTy = &(*itr);
		}
		Unlock();

		return pRetTy;
	}

	// Functions without Lock
	inline BOOL InsertNoLock(const _K& key)
	{
		pair<set< _K, _Pr, _A >::iterator, bool> res;

		res = insert(key);

		return res.second;
	}

	inline BOOL DeleteNoLock(const _K& key)
	{
		int nDeleted;

		nDeleted = erase(key);

		return (nDeleted!=0)?TRUE:FALSE;
	}

	inline _K FindNoLock(const _K& key)
	{
		set< _K, _Pr, _A >::iterator itr;

		itr = find(key);

		if (itr == end())
		{
			return NULL;
		}
		return *itr;
	}
};

template<class _Ty, class _A = allocator<_Ty> >
class mt_list: public list<_Ty, _A>, public mt_lock
{
public:
	inline _Ty FindLock(const _Ty& key)
	{
		Lock();
		iterator itr = begin();
		while(itr != end())
		{
			if (key == *itr)
			{
				Unlock();
				return *itr;
			}
			itr++;
		}
		Unlock();

		return NULL;
	}

	inline int DeleteLock(const _Ty& key)
	{
		int nDeleted = 0;
		Lock();
		iterator itr = begin();
		while(itr != end())
		{
			if (key == *itr)
			{
				itr = erase(itr);
				nDeleted++;
				continue;
			}
			itr++;
		}
		Unlock();
		return nDeleted;
	}

	inline void PushBackLock(const _Ty& x)
	{
		Lock();
		push_back(x);
		Unlock();
	}

	inline void ClearLock()
	{
		Lock();
		clear();
		Unlock();
	}

	inline _Ty GetByIndexLock(int idx)
	{
		int tmp = 0;
		Lock();
		iterator itr = begin();
		while(itr != end())
		{
			if (tmp == idx)
			{
				Unlock();
				return *itr;
			}
			itr++;
			tmp++;
		}
		Unlock();
		return NULL;
	}

	inline _Ty GetByIndexNoLock(int idx)
	{
		int tmp = 0;
		iterator itr = begin();
		while(itr != end())
		{
			if (tmp == idx)
			{
				return *itr;
			}
			itr++;
			tmp++;
		}
		return NULL;
	}
};

template<class _Ty, class _A = allocator<_Ty> >
class mt_vector: public vector<_Ty, _A>, public mt_lock
{
public:
	inline void PushBackLock(const _Ty& x)
	{
		Lock();
		push_back(x);
		Unlock();
	}

	inline void ClearLock()
	{
		Lock();
		clear();
		Unlock();
	}
};

template<class _Ty, class _C = deque<_Ty> >
class mt_queue: public queue<_Ty, _C>, public mt_lock
{
public:
	inline void PushLock(const _Ty& x)
	{
		Lock();
		push(x);
		Unlock();
	}

	inline void PopLock(const _Ty& x)
	{
		Lock();
		pop(x);
		Unlock();
	}

	inline void ClearLock()
	{
		Lock();
		clear();
		Unlock();
	}
};

template<class _K, class _Ty, class _Pr = less<_K>, class _A = allocator<_Ty> >
class ez_map : public map< _K, _Ty, _Pr, _A >
{
public:
	inline BOOL InsertEZ(const _K& key, const _Ty& value)
	{
		pair<map< _K, _Ty, _Pr, _A >::iterator, bool> res;

		res = insert(pair<_K, _Ty>(key, value));

		return res.second;
	}

	inline BOOL DeleteEZ(const _K& key)
	{
		int nDeleted;

		nDeleted = erase(key);

		return (nDeleted!=0)?TRUE:FALSE;
	}

	inline BOOL DeleteEZ(const _K& key, _Ty& value)
	{
		map< _K, _Ty, _Pr, _A >::iterator itr;

		itr = find(key);
		if (itr == end())
		{
			return FALSE;
		}

		value = itr->second;
		erase(itr);

		return TRUE;
	}

	inline _Ty FindEZ(const _K& key)
	{
		map< _K, _Ty, _Pr, _A >::iterator itr;

		itr = find(key);

		if (itr == end())
		{
			_Ty		Temp;
			memset(&Temp, 0x00, sizeof(_Ty));
			return Temp;
		}
		return itr->second;
	}

	inline _Ty* FindEZ_ptr(const _K& key)
	{
		map< _K, _Ty, _Pr, _A >::iterator itr;

		itr = find(key);

		if (itr == end())
		{
			return NULL;
		}
		return &(itr->second);
	}
};

template<class _K, class _Pr = less<_K>, class _A = allocator<_K> >
class ez_set : public set<_K, _Pr, _A>
{
public:
	inline BOOL InsertEZ(const _K& key)
	{
		pair<set< _K, _Pr, _A >::iterator, bool> res;

		res = insert(key);

		return res.second;
	}

	inline BOOL DeleteEZ(const _K& key)
	{
		int nDeleted;

		nDeleted = erase(key);

		return (nDeleted!=0)?TRUE:FALSE;
	}

	inline _K FindEZ(const _K& key)
	{
		set< _K, _Pr, _A >::iterator itr;

		itr = find(key);

		if (itr == end())
		{
			return NULL;
		}
		return *itr;
	}
};

template<class _Ty, class _A = allocator<_Ty> >
class ez_list: public list<_Ty, _A>
{
public:
	inline _Ty FindEZ(const _Ty& key)
	{
		iterator itr = begin();
		while(itr != end())
		{
			if (key == *itr)
			{
				return *itr;
			}
			itr++;
		}
		return NULL;
	}

	inline int DeleteEZ(const _Ty& key)
	{
		int nDeleted = 0;
		iterator itr = begin();
		while(itr != end())
		{
			if (key == *itr)
			{
				itr = erase(itr);
				nDeleted++;
				continue;
			}
			itr++;
		}
		return nDeleted;
	}

	inline _Ty GetByIndexEZ(int idx)
	{
		int tmp = 0;
		iterator itr = begin();
		while(itr != end())
		{
			if (tmp == idx)
			{
				return *itr;
			}
			itr++;
			tmp++;
		}
		return NULL;
	}
};

#endif
