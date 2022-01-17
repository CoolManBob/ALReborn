/******************************************************************************
Module:  AgcmEventShrine.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 25
******************************************************************************/

#if !defined(__AGCMEVENTSHRINE_H__)
#define __AGCMEVENTSHRINE_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventShrineD" )
#else
#pragma comment ( lib , "AgcmEventShrine" )
#endif
#endif

#include "ApBase.h"
#include "AgcModule.h"

#include "AgpmShrine.h"

#include "AgcmEventManager.h"

class AgcmEventShrine : public AgcModule {
private:
	AgpmShrine*				m_pcsAgpmShrine;
	AgcmEventManager*		m_pcsAgcmEventManager;

public:
	AgcmEventShrine();
	~AgcmEventShrine();

	BOOL			OnAddModule		(						);
	BOOL			OnInit			(						);
	BOOL			OnDestroy		(						);

	static BOOL		CBActiveEvent	( PVOID pData, PVOID pClass, PVOID pCustData	);
};

#endif	//__AGCMEVENTSHRINE_H__

