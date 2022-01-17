// AgcmEventQuest.h: interface for the AgcmEventQuest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMEVENTQUEST_H__E0176399_92A7_4D96_9620_E1130248356D__INCLUDED_)
#define AFX_AGCMEVENTQUEST_H__E0176399_92A7_4D96_9620_E1130248356D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgpmEventQuest.h"

#include "AgcmCharacter.h"
#include "AgcmEventManager.h"
#include "AgcmObject.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgcmEventQuestD.lib")
#else
	#pragma comment(lib, "AgcmEventQuest.lib")
#endif
#endif

typedef enum _eAgcmEventQuestCallback
{
	AGCMEVENTQUEST_CB_GRANT = 0,
} eAgcmEventQuestCallBack;

class AgcmEventQuest : public AgcModule
{
private:
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmEventQuest		*m_pcsAgpmEventQuest;

	AgcmCharacter		*m_pcsAgcmCharacter;
	AgcmEventManager	*m_pcsAgcmEventManager;
	AgcmObject			*m_pcsAgcmObject;

public:
	ApdEvent			*m_pcsLastGrantEvent;
	
public:
	AgcmEventQuest();
	virtual ~AgcmEventQuest();

	BOOL OnAddModule();
	
	BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);	
};

#endif // !defined(AFX_AGCMEVENTQUEST_H__E0176399_92A7_4D96_9620_E1130248356D__INCLUDED_)
