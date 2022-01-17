// AcQueue.cpp: implementation of the AcQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "ApBase.h"
#include "AcQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcQueue g_AcReceiveQueue;

AcQueue::AcQueue()
{
}

AcQueue::~AcQueue()
{
	AuAutoLock Lock(m_csMutex);
	if (Lock.Result())
	{
		// Queue에 남아있는 Data를 모두 삭제한다.
		while (!m_stlQueue.empty())
		{
			AcPacketData* pQueueData = m_stlQueue.front();
			m_stlQueue.pop();

	//		delete pQueueData;
		}
	}
}

VOID AcQueue::Push(AcPacketData* pQueueData)
{
	AuAutoLock Lock(m_csMutex);
	if (!Lock.Result()) return;

	m_stlQueue.push(pQueueData);
}

AcPacketData* AcQueue::Pop()
{
	AuAutoLock Lock(m_csMutex);
	if (!Lock.Result()) return NULL;
	
	// 2004.05.14. steeple 추가
	// Queue Size 가 1 보다 작으면 return NULL
	if(GetCount() < 1)
		return NULL;

	AcPacketData *pQueueData = m_stlQueue.front();

	m_stlQueue.pop();

	return pQueueData;
}

BOOL AcQueue::IsEmpty()
{
	// Queue가 비어있는지 검사
	return m_stlQueue.empty();
}

INT32 AcQueue::GetCount()
{
	return (INT32)m_stlQueue.size();
}