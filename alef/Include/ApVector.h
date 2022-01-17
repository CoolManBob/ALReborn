#pragma once

#include "ApSafeArray.h"
#include "MagDebug.h"

template <typename T, INT32 TCount>
class ApVector : public ApSafeArray<T, TCount>
{
private:
	INT32 size_;

	void incSize()		{++size_;}
	void decSize()		{--size_;}
	void clearSize()	{size_ = 0;}

public:
	ApVector()			{clearSize();}

	INT32 size()		{return size_;}
	bool empty()		{return (0 == size());}
	bool full()			{return (TCount <= size());}

	iterator end()		{return begin() + size();}
	reference front()	{return (*begin());}
	reference back()	{return (*(end() - 1));}

	void clear()
	{
		clearSize();
		MemSetAll();
	}

	bool push_back(const T& value)
	{
		ASSERT(false == full());

		if (full())
			return false;

		(*this)[size()] = value;
		incSize();
		
		return true;
	}

	bool pop_back()
	{
		ASSERT(false == empty());

		if (empty())
			return false;

		decSize();
		erase(end() - 1);
		
		return true;
	}

	reference at(INT32 index)
	{
		ASSERT(0 <= index);
		ASSERT(GetLength() > index);

		return (*this)[index];
	}

	iterator insert(iterator where, const T& value)
	{
		ASSERT(IsValidIterator(where));
		ASSERT(size() < GetLength() - 1);

		memmove(where + 1, where, (Distance(where, end())) * sizeof(T));
		*where = value;
		incSize();

		return where;
	}

	iterator erase(iterator where)
	{
		ASSERT(IsValidIterator(where));
		ASSERT(0 != size());

		if (0 == size())
			return end();

		ApSafeArray<T, TCount>::erase(where);
		decSize();

		return where;
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