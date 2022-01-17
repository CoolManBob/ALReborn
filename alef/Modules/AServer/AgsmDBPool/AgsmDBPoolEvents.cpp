// AgsmDBPoolEvents.cpp: implementation of the AgsmDBPoolEvents class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmDBPoolEvents.h"
#include "ApMemoryTracker.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmDBPoolEvents::AgsmDBPoolEvents()
{
	m_pHandles	= NULL;
	m_pOwner	= NULL;
	m_lEventCount = 0;	

}

AgsmDBPoolEvents::~AgsmDBPoolEvents()
{
	if (m_pHandles) delete [] m_pHandles;
	if (m_pOwner)	delete [] m_pOwner;
}

// DB에 연결되었는지 확인을 위한 Event를 생성한다.
BOOL AgsmDBPoolEvents::Create(INT32 lEventCount)
{
	AuAutoLock Lock(m_Muex);

	// 핸들이 할당되어 있으면 생성시키지 않는다.
	if (m_pHandles) return FALSE;

	// Event 생성
	m_lEventCount = lEventCount;
	m_pHandles	= new HANDLE[m_lEventCount];
	m_pOwner	= new BOOL[m_lEventCount];

	for (INT32 i = 0; i < m_lEventCount; ++i)
	{
		m_pHandles[i]	= ::CreateEvent(NULL, FALSE, FALSE, NULL);
		m_pOwner[i]		= NULL;

		if (NULL == m_pHandles[i]) return FALSE;
	}
	
	return TRUE;
}

INT32 AgsmDBPoolEvents::GetIndex()
{
	AuAutoLock Lock(m_Muex);

	// 소유자가 없는 Event index를 알려준다.
	for (INT32 i = 0; i < m_lEventCount; ++i)
	{
		if (FALSE == m_pOwner[i])
		{
			m_pOwner[i] = TRUE;
			return i;
		}
	}

	return INVALID_INDEX;
}

BOOL AgsmDBPoolEvents::Wait(DWORD dwTime)
{
	// JNY TODO : 리턴값을 분석해서 적절한 리턴값을 주어야 한다.
	DWORD dwResult = WaitForMultipleObjects(m_lEventCount, m_pHandles, TRUE, dwTime);
	return TRUE;
}

BOOL AgsmDBPoolEvents::SetDBEvent(INT32 lIndex)
{
	AuAutoLock Lock(m_Muex);

	ASSERT((lIndex < m_lEventCount) && "인덱스가 잘못 지정되었습니다.");
	ASSERT((lIndex >= 0) && "인덱스가 잘못 지정되었습니다.");
	
	return SetEvent(m_pHandles[lIndex]);
}

BOOL AgsmDBPoolEvents::ResetDBEvent(INT32 lIndex)
{
	AuAutoLock Lock(m_Muex);

	ASSERT((lIndex < m_lEventCount) && "인덱스가 잘못 지정되었습니다.");
	ASSERT((lIndex >= 0) && "인덱스가 잘못 지정되었습니다.");

	return ResetEvent(m_pHandles[lIndex]);
}