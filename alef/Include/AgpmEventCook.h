/*=========================================================

	AgpmEventCook.h

==========================================================*/

#ifndef _AGPM_EVENT_COOK_H_
	#define _AGPM_EVENT_COOK_H_

#include "AgpmCharacter.h"
#include "ApmEventManager.h"
#include "AuPacket.h"

#ifdef _DEBUG
	#pragma comment(lib, "AgpmEventCookD.lib")
#else
	#pragma comment(lib, "AgpmEventCook.lib")
#endif

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define	AGPMEVENT_COOK_MAX_USE_RANGE				1600

typedef enum _eAgpmEventCookPacketType
	{
	AGPMEVENT_COOK_PACKET_REQUEST = 0,
	AGPMEVENT_COOK_PACKET_GRANT,
	AGPMEVENT_COOK_PACKET_MAX,
	} eAgpmEventCookPacketType;

typedef enum _eAgpmEventCookCallback
	{
	AGPMEVENT_COOK_CB_REQUEST = 0,
	AGPMEVENT_COOK_CB_GRANT,
	AGPMEVENT_COOK_CB_MAX,
	} eAgpmEventCookCallback;

/****************************************************/
/*		The Definition of AgpmEventCook class		*/
/****************************************************/
//
class AgpmEventCook : public ApModule
	{
	private:
		AgpmCharacter*		m_pcsAgpmCharacter;
		ApmEventManager*	m_pcsApmEventManager;
		
	public:
		AuPacket m_csPacket;
		
	public:
		AgpmEventCook();
		virtual ~AgpmEventCook();

		// ... ApModule inherited
		BOOL OnAddModule();
		BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);

		// ... Operation
		BOOL OnOperationRequest(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);
		BOOL OnOperationGrant(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);

		// ... Packet
		PVOID MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
		PVOID MakePacketEventGrant(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);

		// ... Set callback
		BOOL SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		static BOOL CBActionCook(PVOID pData, PVOID pClass, PVOID pCustData);
	};

#endif
