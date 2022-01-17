// AgcmQuest.h: interface for the AgcmQuest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMQUEST_H__4DCE4603_C36E_402F_A583_43F482A93232__INCLUDED_)
#define AFX_AGCMQUEST_H__4DCE4603_C36E_402F_A583_43F482A93232__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgpmQuest.h"
#include "AgcModule.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmQuestD" )
#else
#pragma comment ( lib , "AgcmQuest" )
#endif
#endif

class AgcmQuest : public AgcModule
{
public:
	AgpmQuest*	m_pcsAgpmQuest;

public:
	AgcmQuest();
	virtual ~AgcmQuest();

	BOOL OnInit();
	BOOL OnAddModule();
	BOOL OnIdle(UINT32 ulClockCount);	
	BOOL OnDestroy();

	// Check Point Validation Check
//	BOOL IsCheckPointQuest(ApdEve

	// Callback function
	BOOL SetCallbackNotifyNewQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackNotifyIncompleteQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackNotifyCompleteQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// Send Packet
	BOOL SendPacketRequireQuest(INT32 lQuestTID, ApdEvent* pEvent, INT32 lCID);
	BOOL SendPacketCompleteQuest(INT32 lQuestTID, ApdEvent* pEvent, INT32 lCID);
	BOOL SendPacketQuestCancel(INT32 lQuestTID, INT32 lCID);
	BOOL SendPacketCheckPoint(INT32 lQuestTID, ApdEvent* pEvent, INT32 lCID, INT32 lCheckPointIndex);
};

#endif // !defined(AFX_AGCMQUEST_H__4DCE4603_C36E_402F_A583_43F482A93232__INCLUDED_)
