/******************************************************************************
Module:  AsCMDQueue.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 18
******************************************************************************/

#if !defined(__ASCMDQUEUE_H__)
#define __ASCMDQUEUE_H__

#include "AsDefine.h"
#include "ApMutualEx.h"

// AsCMDQueue class
///////////////////////////////////////////////////////////////////////////////
class AsCMDQueue {
private:
	INT16		m_nHead;
	INT16		m_nTail;
	INT16		m_nQueueSize;
	INT16		m_nCommandSize;
	PVOID		m_pvCommand;
	INT16		m_nDataSize;		// Queue에 있는 데이타 갯수

	INT16		m_nPeakData;		// queue에 데이타가 가장 많을 때 갯수

	ApMutualEx	m_Mutex;

public:
	AsCMDQueue();
	~AsCMDQueue();
	BOOL Initialize(INT16 nQueueSize, INT16 nCommandSize);
	BOOL PopCommand(PVOID pvCommand);
	BOOL PushCommand(PVOID pvCommand);

	INT16 GetDataCount();

	INT16 GetPeakDataCount();

	BOOL Reset();
};

///////////////////////////////////////////////////////////////////////////////
// AsCMDQueue class member functions
///////////////////////////////////////////////////////////////////////////////

inline AsCMDQueue::AsCMDQueue()
{
	m_nHead			= 0;
	m_nTail			= 0;
	m_nDataSize		= 0;
	m_nPeakData		= 0;
}

inline AsCMDQueue::~AsCMDQueue()
{
	GlobalFree(m_pvCommand);
}

inline BOOL AsCMDQueue::Initialize(INT16 nQueueSize, INT16 nCommandSize)
{
	m_nQueueSize	= nQueueSize;
	m_nCommandSize		= nCommandSize;

	m_pvCommand = (pstCOMMAND) GlobalAlloc(GMEM_FIXED, nCommandSize * nQueueSize);
	if (m_pvCommand == NULL)
	{
		// memory allocation error
		return FALSE;
	}

	ZeroMemory(m_pvCommand, m_nCommandSize * nQueueSize);

	return TRUE;
}

inline BOOL AsCMDQueue::PopCommand(PVOID pvCommand)
{
	BOOL	bRetVal = FALSE;

	if (!m_Mutex.WLock())
	{
		return bRetVal;
	}

	if (m_nDataSize >= 1)
	{
		CopyMemory(pvCommand, ((CHAR *) m_pvCommand + (m_nCommandSize * m_nTail++)), m_nCommandSize);

		if (m_nTail == (m_nQueueSize - 1))
			m_nTail = 0;
		m_nDataSize--;

		bRetVal = TRUE;
	}

	// Release ownership of the mutex object.
	if (!m_Mutex.Release())
	{
		// Deal with error.
		bRetVal = FALSE;
	}

	return bRetVal;
}

inline BOOL AsCMDQueue::PushCommand(PVOID pvCommand)
{
	BOOL	bRetVal = FALSE;

	if (!m_Mutex.WLock())
	{
		return bRetVal;
	}

	// Write to the data.
	if (m_nDataSize < m_nQueueSize)
	{
		CopyMemory(((CHAR *) m_pvCommand + (m_nCommandSize * m_nHead++)), pvCommand, m_nCommandSize);

		if (m_nHead == (m_nQueueSize - 1))
			m_nHead = 0;
		m_nDataSize++;

		if (m_nDataSize > m_nPeakData)
			m_nPeakData = m_nDataSize;

		bRetVal = TRUE;
	}
	else	// queue is fulled
	{
		// log

		bRetVal = FALSE;
	}

	// Release ownership of the mutex object.
	if (!m_Mutex.Release())
	{
		// Deal with error.
		bRetVal = FALSE;
	}

	return bRetVal;
}

inline BOOL AsCMDQueue::Reset()
{
	if (!m_Mutex.WLock())
	{
		return FALSE;
	}

	// initialize data.
	ZeroMemory(m_pvCommand, m_nCommandSize * m_nQueueSize);
	m_nHead = 0;
	m_nTail = 0;
	m_nDataSize = 0;

	m_nPeakData = 0;

	// Release ownership of the mutex object.
	if (!m_Mutex.Release())
	{
		// Deal with error.
		return FALSE;
	}
}

inline INT16 AsCMDQueue::GetDataCount()
{
	return m_nDataSize;
}

inline INT16 AsCMDQueue::GetPeakDataCount()
{
	if (!m_Mutex.WLock())
	{
		return FALSE;
	}

	INT16	nPeakDataCount = m_nPeakData;

	m_nPeakData = 0;

	if (!m_Mutex.Release())
	{
		return FALSE;
	}

	return nPeakDataCount;
}

#endif //__ASCMDQUEUE_H__