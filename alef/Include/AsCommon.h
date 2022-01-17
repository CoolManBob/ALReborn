/******************************************************************************
Module:  AsCommon.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 18
******************************************************************************/

#if !defined(__ASCOMMON_H__)
#define __ASCOMMON_H__

#include "windows.h"
#include "ApMutualEx.h"

typedef struct _stIOCPOBJECT {
	HANDLE		hCompPort;
	UINT32		ulThreadID;
} stIOCPOBJECT, *pstIOCPOBJECT;

class AsQueueLIFO {
private:
	INT16		m_nQueueSize;
	INT16		m_nDataSize;
	PVOID		m_pQueueData;

	INT16		m_nNumData;

	ApMutualEx	m_Mutex;

public:
	AsQueueLIFO();
	~AsQueueLIFO();

	BOOL Initialize(INT16 nQueueSize, INT16 nDataSize);

	BOOL PopQueue(PVOID pBuffer);
	BOOL PushQueue(PVOID pBuffer);
	BOOL ResetQueue();
};

inline AsQueueLIFO::~AsQueueLIFO()
{
	if (m_pQueueData != NULL)
	{
		GlobalFree(m_pQueueData);
	}
}

inline BOOL AsQueueLIFO::Initialize(INT16 nQueueSize, INT16 nDataSize)
{
	m_Mutex.WLock();

	m_nQueueSize = nQueueSize;
	m_nDataSize = nDataSize;

	m_pQueueData = (PVOID) GlobalAlloc(GMEM_FIXED, nDataSize);
	if (m_pQueueData == NULL)
	{
		// memory allocation error. log error
		return FALSE;
	}

	m_nNumData = 0;

	m_Mutex.Release();

	return TRUE;
}

inline BOOL AsQueueLIFO::PopQueue(PVOID pBuffer)
{
	m_Mutex.WLock();

	if (m_nNumData == 0)
	{
		// queue is empty
		m_Mutex.Release();
		return FALSE;
	}

	CopyMemory(pBuffer, ((CHAR *) m_pQueueData) + (m_nDataSize * m_nNumData), m_nDataSize);

	m_nNumData--;

	m_Mutex.Release();

	return TRUE;
}

inline BOOL AsQueueLIFO::PushQueue(PVOID pBuffer)
{
	m_Mutex.WLock();

	if (m_nNumData == m_nQueueSize)
	{
		// queue is full
		m_Mutex.Release();
		return FALSE;
	}

	m_nNumData++;

	CopyMemory(((CHAR *) m_pQueueData) + (m_nDataSize * m_nNumData), pBuffer, m_nDataSize);

	m_Mutex.Release();

	return TRUE;
}

inline BOOL AsQueueLIFO::ResetQueue()
{
	m_Mutex.WLock();

	m_nQueueSize = 0;
	m_nDataSize = 0;

	if (m_pQueueData != NULL)
	{
		GlobalFree(m_pQueueData);
	}

	m_nNumData = 0;

	m_Mutex.Release();
}

#endif //__ASCOMMON_H__