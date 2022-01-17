// AgcmHealthCheck.h: interface for the AgcmHealthCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMHEALTHCHECK_H__97200C94_B7C6_46CC_B880_771AB5BD0D19__INCLUDED_)
#define AFX_AGCMHEALTHCHECK_H__97200C94_B7C6_46CC_B880_771AB5BD0D19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcModule.h"
#include "ApBase.h"
#include "ApAdmin.h"
#include "AgcmResourceLoader.h"
#include <process.h>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmHealthCheckD" )
#else
#pragma comment ( lib , "AgcmHealthCheck" )
#endif
#endif

class AgcmHealthCheck : public AgcModule  
{
private:
	AgcmResourceLoader *			m_pcsAgcmResourceLoader;

	BOOL							m_bEnd;
	HANDLE							m_hThread;

	UINT32							m_ulLastIdleTime;

	BOOL							m_bCheckEnable;

	UINT32							m_ulCriticality;
	UINT32							m_ulCheckDelay;

public:
	AgcmHealthCheck();
	virtual ~AgcmHealthCheck();

public:
	BOOL		OnAddModule();
	BOOL		OnInit();
	BOOL		OnPreDestroy();
	BOOL		OnDestroy();
	BOOL		OnIdle(UINT32 ulClockCount);

	VOID		SetValues(UINT32 ulCriticality, UINT32 ulCheckDelay);
	VOID		EnableCheck(BOOL bCheck = TRUE);

	static unsigned __stdcall ProcessCheck(LPVOID pvArgs);
};

#endif // !defined(AFX_AGCMHEALTHCHECK_H__97200C94_B7C6_46CC_B880_771AB5BD0D19__INCLUDED_)
