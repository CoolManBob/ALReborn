#pragma once
#include <deque>
#include "lock\lock.h"
using namespace CleverLock;

template< typename T>
class CQueueWithLock :public CWithLock
{
public:
	CQueueWithLock(void);
	~CQueueWithLock(void);
	void Push(T &m);
	bool Front(T &m);
	bool Pop(T &m);
	bool Pop();
	inline bool IsEmpty();

	int Size();

	void Clear();
protected:
	deque<T> m_Queue;
};

template< typename T>
CQueueWithLock<T>::CQueueWithLock()
{
}

template< typename T>
CQueueWithLock<T>::~CQueueWithLock()
{
	Clear();
}

template< typename T>
void CQueueWithLock<T>::Push(T &m)
{
	LOCK(*this);
	m_Queue.push_back(m);
}

template< typename T>
bool CQueueWithLock<T>::Front(T &m)
{
	LOCK(*this);
	if (m_Queue.empty())
		return false;

	m = m_Queue.front();
	return true;
}

template< typename T>
bool CQueueWithLock<T>::Pop(T &m)
{
	LOCK(*this);
	if (!Front(m))
		return false;

	m_Queue.pop_front();
	return true;
}

template< typename T>
bool CQueueWithLock<T>::Pop()
{
	LOCK(*this);
	if (m_Queue.empty())
		return false;

	m_Queue.pop_front();
	return true;
}

template< typename T>
bool CQueueWithLock<T>::IsEmpty()
{
	LOCK(*this);
	return m_Queue.empty();
}

template< typename T>
int CQueueWithLock<T>::Size()
{
	LOCK(*this);
	return (int)m_Queue.size();
}

template< typename T>
void CQueueWithLock<T>::Clear()
{
	LOCK(*this);
	m_Queue.clear();
}