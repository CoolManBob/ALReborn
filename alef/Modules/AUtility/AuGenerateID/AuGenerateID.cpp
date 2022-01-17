// AuGenerateID.cpp: implementation of the AuGenerateID class.
//
//////////////////////////////////////////////////////////////////////

#include "AuGenerateID.h"
#include "ApMemoryTracker.h"
// class AuGenerateID member functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AuGenerateID::AuGenerateID()
{
	Initialize();
}

AuGenerateID::~AuGenerateID()
{
}

BOOL AuGenerateID::Initialize(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag, INT32 lRemoveIDQueueSize, BOOL bUsePool)
{
	// nSizeServerBit 가 4이고 ulServerBit 가 00000000 00000000 00000000 00000101 이라면
	//	00101000 00000000 00000000 00000000 으로 만든다. (젤 앞에 sign bit는 건들지 않는다)
	m_MutexID.Init();

	m_ulServerFlag	= ulServerFlag << (31 - nSizeServerFlag);

	AuAutoLock	lock(m_MutexID);
	if (!lock.Result()) return FALSE;

	m_lID			= m_ulServerFlag | ulStartValue;

	m_lMinID		= m_lID;
	m_lMaxID		= ((ulServerFlag + 1) << (31 - nSizeServerFlag)) - 1;

	m_pCurrentPool	= NULL;
	m_bUsePool		= FALSE;

	if (lRemoveIDQueueSize > 0)
		m_csRemoveIDQueue.Initialize(lRemoveIDQueueSize, sizeof(INT32));

	if (bUsePool)
	{
		m_bUsePool	= TRUE;

		m_csRemovePool[0].Initialize(10000, sizeof(INT32));
		m_csRemovePool[1].Initialize(10000, sizeof(INT32));

		m_csRemovePool[0].SetNextPool(&m_csRemovePool[1]);
		m_csRemovePool[1].SetNextPool(&m_csRemovePool[0]);

		m_pCurrentPool	= m_csRemovePool;
	}

	return TRUE;
}

INT32 AuGenerateID::GetID()
{
	INT32 lNewID = (-1);

	AuAutoLock	lock(m_MutexID);
	if (!lock.Result()) return (-1);

	m_csRemoveIDQueue.GetHeadData(&lNewID);

	if (lNewID <= 0)
		lNewID = ++m_lID;

	return lNewID;
}

INT32 AuGenerateID::GetCurrentID()
{
	AuAutoLock	lock(m_MutexID);
	if (!lock.Result()) return (-1);

	INT32 lCurrentID = m_lID;

	return lCurrentID;
}

BOOL AuGenerateID::AddRemoveID(INT32 lRemoveID)
{
	if (lRemoveID < m_lMinID ||
		lRemoveID > m_lMaxID)
		return FALSE;

	AuAutoLock	lock(m_MutexID);
	if (!lock.Result()) return FALSE;

	if (m_bUsePool)
	{
		if (m_pCurrentPool->IsFull())
		{
			m_pCurrentPool	= m_pCurrentPool->GetNextPool();

			INT32 *plPoolData	= (INT32 *) m_pCurrentPool->GetData(0);
			if (plPoolData && *plPoolData > 0)
			{
				for (int i = 0; i < 10000; ++i)
				{
					plPoolData	= (INT32 *) m_pCurrentPool->GetData(i);
					if (plPoolData && *plPoolData > 0)
						m_csRemoveIDQueue.AddData(plPoolData);
					else
						break;
				}
			}

			m_pCurrentPool->Reset();
		}

		return m_pCurrentPool->AddData(&lRemoveID);
	}
	else
		return m_csRemoveIDQueue.AddData(&lRemoveID);

	return FALSE;
}

// class AuGenerateID64 member functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AuGenerateID64::AuGenerateID64()
{
	m_MutexID.Init();

	m_lServerIndex	= 0;
	m_llID			= 0;
}

AuGenerateID64::~AuGenerateID64()
{
	m_MutexID.Destroy();
}

BOOL AuGenerateID64::Initialize(INT32 ServerIndex)
{
	AuAutoLock	lock(m_MutexID);
	if (!lock.Result()) return FALSE;

	m_lServerIndex = ServerIndex;

	return TRUE;
}

INT64 AuGenerateID64::GetID()
{
	AuAutoLock	lock(m_MutexID);
	if (!lock.Result())
		return (-1);

	struct tm *newtime;
	__time64_t long_time;

	_time64( &long_time );
	newtime = _localtime64( &long_time );

	INT64 ulDBID = 0;

	ulDBID = m_lServerIndex*100 + (newtime->tm_year % 100);
	ulDBID = ulDBID*100 + newtime->tm_mon + 1;
	ulDBID = ulDBID*100 + newtime->tm_mday;
	ulDBID = ulDBID*100 + newtime->tm_hour;
	ulDBID = ulDBID*100 + newtime->tm_min;
	ulDBID = ulDBID*100 + newtime->tm_sec;
	ulDBID = ulDBID*10000 + m_llID++;

	if( m_llID >= 10000 )
		m_llID = 0;

	return ulDBID;
}

AuFIFOQueue::AuFIFOQueue()
{
	m_lQueueSize		= 0;
	m_lDataSize			= 0;

	m_lHead				= 0;
	m_lTail				= 0;

	m_pvQueue			= NULL;

	m_lCurrentDataCount	= 0;
}

AuFIFOQueue::~AuFIFOQueue()
{
	if (m_pvQueue)
	{
		delete [] m_pvQueue;

		m_pvQueue			= NULL;
	}
}

BOOL AuFIFOQueue::Initialize(INT32 lQueueSize, INT32 lDataSize)
{
	m_lQueueSize		= lQueueSize;
	m_lDataSize			= lDataSize;

	if (m_pvQueue)
	{
//		free(m_pvQueue);
		delete [] m_pvQueue;

		m_pvQueue			= NULL;
	}

//	m_pvQueue			= (PVOID) malloc(sizeof(CHAR) * lQueueSize * lDataSize);
	m_pvQueue			= (PVOID) new BYTE[sizeof(CHAR) * lQueueSize * lDataSize];

	if (!m_pvQueue)
		return FALSE;

	m_lHead				= 0;
	m_lTail				= 0;

	m_lCurrentDataCount	= 0;

	return TRUE;
}

BOOL AuFIFOQueue::AddData(PVOID pvData)
{
	if (!pvData)
		return FALSE;

	if (m_lCurrentDataCount >= m_lQueueSize)
		return FALSE;

	CopyMemory((CHAR *) m_pvQueue + m_lTail * m_lDataSize, pvData, m_lDataSize);

	++m_lTail;
	++m_lCurrentDataCount;

	if (m_lTail == m_lQueueSize)
		m_lTail = 0;

	return TRUE;
}

BOOL AuFIFOQueue::GetHeadData(PVOID pvData)
{
	if (!pvData)
		return FALSE;

	if (m_lCurrentDataCount <= 0)
		return FALSE;

	CopyMemory(pvData, (CHAR *) m_pvQueue + m_lHead * m_lDataSize, m_lDataSize);

	++m_lHead;
	--m_lCurrentDataCount;

	if (m_lHead == m_lQueueSize)
		m_lHead = 0;

	return TRUE;
}

AuRemovePool::AuRemovePool()
{
	m_pvPool			= NULL;

	m_lPoolSize			= 0;
	m_lDataSize			= 0;
	m_lCurrentDataCount	= 0;

	m_pNext				= NULL;
}

AuRemovePool::~AuRemovePool()
{
	if (m_pvPool)
	{
		delete [] m_pvPool;

		m_pvPool			= NULL;
	}
}

BOOL AuRemovePool::Initialize(INT32 lPoolSize, INT32 lDataSize)
{
	m_lPoolSize			= lPoolSize;
	m_lDataSize			= lDataSize;

	if (m_pvPool)
	{
		delete [] m_pvPool;

		m_pvPool			= NULL;
	}

	m_pvPool			= (PVOID) new BYTE[sizeof(CHAR) * lPoolSize * lDataSize];

	if (!m_pvPool)
		return FALSE;

	ZeroMemory(m_pvPool, sizeof(CHAR) * lPoolSize * lDataSize);

	m_lCurrentDataCount	= 0;

	return TRUE;
}

BOOL AuRemovePool::AddData(PVOID pvData)
{
	if (!m_pvPool || IsFull())
		return FALSE;

	CopyMemory((CHAR *) m_pvPool + m_lCurrentDataCount * m_lDataSize, pvData, m_lDataSize);

	m_lCurrentDataCount++;

	return TRUE;
}

BOOL AuRemovePool::IsFull()
{
	if (m_lPoolSize <= m_lCurrentDataCount)
		return TRUE;

	return FALSE;
}

BOOL AuRemovePool::SetNextPool(AuRemovePool *pNext)
{
	m_pNext	= pNext;

	return TRUE;
}

AuRemovePool* AuRemovePool::GetNextPool()
{
	return m_pNext;
}

PVOID AuRemovePool::GetData(INT32 lIndex)
{
	if (!m_pvPool || m_lPoolSize <= 0 || lIndex < 0 || lIndex >= m_lPoolSize)
		return NULL;

	return (PVOID) ((CHAR *) m_pvPool + lIndex * m_lDataSize);
}

BOOL AuRemovePool::Reset()
{
	m_lCurrentDataCount	= 0;

	return TRUE;
}