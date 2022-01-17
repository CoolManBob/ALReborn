// AsTimer.cpp: implementation of the AsTimer class.
//
//////////////////////////////////////////////////////////////////////

#include "AsTimer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CALLBACK TimerProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	AsTimer* pTimer = (AsTimer*)dwUser;
	AsTimerEvent TimerEvent;

	AuAutoLock Lock(pTimer->m_Mutex);
	if (!Lock.Result()) return;

	UINT uCurrentTime = ::GetTickCount();

	while (TRUE)
	{
		TimerEvent = pTimer->m_stlPriorityQueue.top();
		if (TimerEvent.m_pTimerData->m_uDelayTime < uCurrentTime)
		{
			pTimer->m_pIOCP->PostStatus((ULONG_PTR)(AsObject*)TimerEvent.m_pTimerData);
			pTimer->m_stlPriorityQueue.pop();
		}
		else break;
	}
}

AsTimer::AsTimer()
{
	m_IDMMTimer = 0;
	m_uDelay	= 0;
}

AsTimer::~AsTimer()
{
	Stop();
}

VOID AsTimer::Initialize(ApIOCP *pIOCP, UINT32 uDelay)
{
	m_pIOCP = pIOCP;
	m_uDelay = uDelay;

	m_stlPriorityQueue.push(new AsTimerEventData);

	MMRESULT hResult = ::timeSetEvent(uDelay, 0, TimerProc, (ULONG_PTR)this, TIME_PERIODIC);
	if (NULL != hResult)
		m_IDMMTimer = (unsigned int)hResult;
}

VOID AsTimer::Stop()
{
	if (m_IDMMTimer)
	{
		MMRESULT hResult = ::timeKillEvent(m_IDMMTimer);
		if (TIMERR_NOERROR == hResult)
		{
			// JNY TODO : 오류 처리
		}
	}

	// 타이머에 남아있는 모든 이벤트 삭제
	while (!m_stlPriorityQueue.empty())
	{
		AsTimerEvent TimerEvent = m_stlPriorityQueue.top();
		delete TimerEvent.m_pTimerData;
		m_stlPriorityQueue.pop();
	}
}

AsTimerEventData* AsTimer::AddTimer(UINT32 uDelay, INT32 lCID, PVOID pClass, AsTimerCallBack fpTimerCallBack, PVOID pvData)
{
	AsTimerEventData *pTimerEventData = new AsTimerEventData;
	pTimerEventData->m_uDelayTime = ::GetTickCount() + uDelay;
	pTimerEventData->m_lCID = lCID;
	pTimerEventData->m_pClass = pClass;
	pTimerEventData->m_pvData = pvData;
	pTimerEventData->m_fpTimerCallBack = fpTimerCallBack;

	AuAutoLock Lock(m_Mutex);
	if (!Lock.Result())
	{
		delete pTimerEventData;
		return NULL;
	}

	m_stlPriorityQueue.push(pTimerEventData);

	return pTimerEventData;
}