// AgpmEventQuest.h: interface for the AgpmEventQuest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPMEVENTQUEST_H__D378F41D_15F8_465B_8205_68C80AC549DC__INCLUDED_)
#define AFX_AGPMEVENTQUEST_H__D378F41D_15F8_465B_8205_68C80AC549DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApmEventManager.h"
#include "AgpmCharacter.h"
#include "AuPacket.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgpmEventQuestD.lib")
#else
	#pragma comment(lib, "AgpmEventQuest.lib")
#endif
#endif

#define	AGPMEVENT_QUEST_MAX_USE_RANGE				1600
#define AGPMEVENT_QUEST_STREAM_NAME_GROUP_ID		"QuestGroupID"
#define AGPMEVENT_QUEST_STREAM_NAME_EVENT_END		"QuestEnd"

class AgpdQuestEventAttachData {
public:
	INT32 lQuestGroupID;
};

typedef enum _eAgpmEventQuestPacketType
{
	AGPMEVENT_QUEST_PACKET_REQUEST = 0,
	AGPMEVENT_QUEST_PACKET_GRANT,
	AGPMEVENT_QUEST_PACKET_MAX,
} eAgpmEventQuestPacketType;

typedef enum _eAgpmEventQuestCallback
{
	AGPMEVENT_QUEST_CB_REQUEST = 0,
	AGPMEVENT_QUEST_CB_GRANT,
	AGPMEVENT_QUEST_CB_MAX,
} eAgpmEventQuestCallback;

class AgpmEventQuest : public ApModule
{
public:
	AgpmCharacter* m_pcsAgpmCharacter;
	ApmEventManager* m_pcsApmEventManager;

	AuPacket	m_csPacket;
	AuPacket	m_csPacketEventData;

public:
	AgpmEventQuest();
	virtual ~AgpmEventQuest();

	BOOL OnAddModule();

	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	// Event Data Stream
	static BOOL CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	// Event Data
	PVOID MakePacketEventData(ApdEvent *pcsEvent);
	BOOL ParseEventDataPacket(ApdEvent *pcsEvent, PVOID pvPacketCustomData);

	static BOOL			CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL OnOperationRequest(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter, INT32 lRange);
	BOOL OnOperationGrant(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);

	PVOID MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength, INT32 lRange);
	PVOID MakePacketEventGrant(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);

	BOOL SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// ApmEventManager Embedded Packet
	static BOOL CBEmbeddedMakeEventPacket(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEmbeddedReceiveEventPacket(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBActionQuest(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif // !defined(AFX_AGPMEVENTQUEST_H__D378F41D_15F8_465B_8205_68C80AC549DC__INCLUDED_)
