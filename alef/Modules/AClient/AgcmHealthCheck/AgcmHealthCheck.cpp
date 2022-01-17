// AgcmHealthCheck.cpp: implementation of the AgcmHealthCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcEngine.h"
#include "AgcmHealthCheck.h"
#include "ApMemoryTracker.h"

#define	AGCMHEALTHCHECK_DEFAULT_CRITICAL	90000
#define AGCMHEALTHCHECK_DEFAULT_DELAY		1000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmHealthCheck::AgcmHealthCheck()
{
	SetModuleName("AgcmHealthCheck");

	EnableIdle(TRUE);

	m_bEnd			= FALSE;
	m_bCheckEnable	= FALSE;
	m_ulCriticality	= AGCMHEALTHCHECK_DEFAULT_CRITICAL;
	m_ulCheckDelay	= AGCMHEALTHCHECK_DEFAULT_DELAY;
	m_hThread		= NULL;

	m_ulLastIdleTime	= 0;
}

AgcmHealthCheck::~AgcmHealthCheck()
{
}

BOOL		AgcmHealthCheck::OnAddModule()
{
	m_pcsAgcmResourceLoader	= (AgcmResourceLoader *) GetModule("AgcmResourceLoader");

	return TRUE;
}

BOOL		AgcmHealthCheck::OnInit()
{
#ifndef _DEBUG
	unsigned int uiThreadID;
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, ProcessCheck, this, 0, &uiThreadID);
	if (m_hThread == (HANDLE) -1)
		return FALSE;
#endif

	return TRUE;
}

BOOL		AgcmHealthCheck::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmHealthCheck::OnIdle");

	if (m_ulLastIdleTime < ulClockCount)
		m_ulLastIdleTime	= ulClockCount;

	return TRUE;
}

BOOL		AgcmHealthCheck::OnPreDestroy()
{
	m_bEnd	= TRUE	;

#ifdef _DEBUG
	WaitForSingleObject(m_hThread, 2000);

	// 2004.12.07. steeple Thread Handle ´ÝÀ½
	if(m_hThread) CloseHandle(m_hThread);
	m_hThread = NULL;
#endif

	return TRUE;
}

BOOL		AgcmHealthCheck::OnDestroy()
{
	return TRUE;
}

VOID		AgcmHealthCheck::SetValues(UINT32 ulCriticality, UINT32 ulCheckDelay)
{
	m_ulCriticality	= ulCriticality;
	m_ulCheckDelay	= ulCheckDelay;
}

VOID		AgcmHealthCheck::EnableCheck(BOOL bCheck)
{
	m_bCheckEnable	= bCheck;

	if (m_bCheckEnable)
		m_ulLastIdleTime	= GetClockCount();
}

unsigned __stdcall AgcmHealthCheck::ProcessCheck(LPVOID pvArgs)
{
	AgcmHealthCheck *			pThis = (AgcmHealthCheck *) pvArgs;
	UINT32						ulDelay;

	pThis->m_ulLastIdleTime	= pThis->GetClockCount();

	while (!pThis->m_bEnd)
	{
		if (pThis->m_bCheckEnable)
		{
			if (pThis->m_pcsAgcmResourceLoader && pThis->m_pcsAgcmResourceLoader->m_bForceImmediate)
			{
				pThis->m_ulLastIdleTime	= pThis->GetClockCount();
			}
			else
			{
				ulDelay	= pThis->GetClockCount() - pThis->m_ulLastIdleTime;
				CHAR szTemp[128];
				sprintf(szTemp, "HealthCheck Delay : %d\n", ulDelay);
				OutputDebugString(szTemp);
				if (ulDelay >= pThis->m_ulCriticality)
				{
					ASSERT(!"DeadLock Detected !!!");
					RaiseException(EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE, 0, NULL);
				}
			}
		}

		MsgWaitForMultipleObjects(0, NULL, 0, pThis->m_ulCheckDelay, 0);
	}

	_endthreadex(0);
	return 0;
}

