#pragma once

#include "ApDefine.h"
#include "MagDebug.h"

template <typename T, INT32 TCount>
class ApSafeArray
{
private:
	T m_Array[TCount];
	enum {lArrayCount = TCount, lBufferSize = sizeof(T) * TCount};
	void operator&() {}

public:
	typedef T	value_type;
	typedef T*	iterator;
	typedef T&	reference;

	iterator begin()	{return m_Array;}
	iterator end()		{return m_Array + lArrayCount;}

protected:
	INT32 Distance(iterator first, iterator last)
	{
		ASSERT(IsValidIterator(first));
		ASSERT(begin() < last);
		ASSERT(end() >= last);
		ASSERT(last > first);

		return (INT32)(last - first);
	}

public:
	ApSafeArray()	{MemSet(0, TCount);}

	void MemSetAll()	{ZeroMemory(m_Array, sizeof(m_Array));}
	INT32 GetLength()	{return lArrayCount;}

	reference operator[](INT32 lIndex)
	{
		ASSERT(lIndex >= 0);
		ASSERT(lIndex < lArrayCount);

		return m_Array[lIndex];
	}

	void MemSet(INT32 lStart, INT32 lCount)
	{
		ASSERT(lStart >= 0);
		ASSERT(lCount >= 0);
		ASSERT((lStart + lCount) <= lArrayCount);

		ZeroMemory(m_Array + lStart, sizeof(T) * lCount);
	}

	void MemCopy(INT32 lDestIndex, iterator iterSource, INT32 lCount)
	{
		ASSERT(lDestIndex >= 0);
		ASSERT(lCount >= 0);
		ASSERT(lDestIndex < lArrayCount);
		ASSERT((lDestIndex + lCount) <= lArrayCount);

		memcpy(&m_Array[lDestIndex], iterSource, sizeof(T) * lCount);
	}

	iterator erase(iterator iter)
	{
		ASSERT(IsValidIterator(iter));

		memmove(iter, iter + 1, (Distance(iter, end()) - 1) * sizeof(T));
		MemSet(lArrayCount - 1, 1);

		return iter;
	}

	bool IsValidIterator(const iterator iter)
	{
		if (iter < begin())
			return false;

		if (iter >= end())
			return false;

		return true;
	}
};