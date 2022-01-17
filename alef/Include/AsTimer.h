/*
	Notices: Copyright (c) NHN Studio 2003
	Created by: Bryan Jeong (2003/12/24)
 */
// AsTimer.h: interface for the AsTimer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASTIMER_H__64CDC9EA_65D3_4570_8544_C0AFE863C339__INCLUDED_)
#define AFX_ASTIMER_H__64CDC9EA_65D3_4570_8544_C0AFE863C339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include <queue>
#include <mmsystem.h>
#include "AsObject.h"
#include "ApMemory.h"
#include "ApMutualEx.h"
#include "ApIOCP.h"

#pragma comment(lib, "winmm")

// 타이머 콜백 함수 정의
typedef BOOL (*AsTimerCallBack) (INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);

class AsTimerEventData : public AsObject, public ApMemory<AsTimerEventData, 20000>
{
private:
	BOOL m_bCancel;									// Cancel 여부

public:
	UINT32				m_uDelayTime;				// 이 이벤트를 처리해야할 시간
	INT32				m_lCID;						// 처리해야할 캐릭터
	PVOID				m_pClass;					// 이벤트를 처리할 클래스
	PVOID				m_pvData;					// 이벤트 처리 함수
	AsTimerCallBack		m_fpTimerCallBack;

	AsTimerEventData(UINT32 uDelayTime = UINT_MAX) : AsObject(ASOBJECT_TIMER), m_uDelayTime(uDelayTime), m_bCancel(FALSE) {}
	virtual ~AsTimerEventData() {}

	BOOL GetCancel() {return m_bCancel;}
	VOID SetCancel(BOOL bCancel) {m_bCancel = bCancel;}
};

class AsTimerEvent 
{
public:
	AsTimerEventData *m_pTimerData;
	
	AsTimerEvent(AsTimerEventData *pTimerData = NULL) : m_pTimerData(pTimerData) {}

	bool operator<(const AsTimerEvent& rPtr) const 
	{
		return m_pTimerData->m_uDelayTime > rPtr.m_pTimerData->m_uDelayTime;
	}
};

class AsTimer  
{
private:
	UINT32			m_uDelay;
	MMRESULT		m_IDMMTimer;

public:
	std::priority_queue<AsTimerEvent> m_stlPriorityQueue;
	ApIOCP				*m_pIOCP;
	ApCriticalSection	m_Mutex;	
	
public:
	AsTimer();
	virtual ~AsTimer();

	VOID Initialize(ApIOCP *pIOCP, UINT32 uDelay = 100);
	VOID Stop();
	AsTimerEventData* AddTimer(UINT32 uDelay, INT32 lCID, PVOID pClass, AsTimerCallBack fpTimerCallBack, PVOID pvData);
};

#endif // !defined(AFX_ASTIMER_H__64CDC9EA_65D3_4570_8544_C0AFE863C339__INCLUDED_)
