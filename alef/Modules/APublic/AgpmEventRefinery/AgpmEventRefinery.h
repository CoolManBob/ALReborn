/*=========================================================

	AgpmEventRefinery.h

=========================================================*/

#ifndef	_AGPM_EVENT_REFINERY_H_
	#define	_AGPM_EVENT_REFINERY_H_

#include "ApmEventManager.h"
#include "AgpmCharacter.h"

#include "AuPacket.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
	#pragma comment(lib , "AgpmEventRefineryD" )
#else
	#pragma comment(lib , "AgpmEventRefinery" )
#endif
#endif


/****************************************/
/*		The Definition of Constatns		*/
/****************************************/
//
#define AGPMEVENTREFINERY_MAX_USE_RANGE		1600

const enum eAGPMEVENTREFINERY_OPERATION
	{
	AGPMEVENTREFINERY_OPERATION_REQUEST = 0,
	AGPMEVENTREFINERY_OPERATION_GRANT,
	};

const enum eAGPMEVENTREFINERY_CB
	{
	AGPMEVENTREFINERY_CB_REQUEST = 0,
	AGPMEVENTREFINERY_CB_GRANT,
	};


/************************************************/
/*		The Definition of AgpmEventRefinery		*/
/************************************************/
//
class AgpmEventRefinery : public ApModule
	{
	private:
		ApmEventManager		*m_pApmEventManager;
		AgpmCharacter		*m_pAgpmCharacter;

	public:
		AuPacket			m_csPacket;
		AuPacket			m_csPacketCustom;

	public:
		AgpmEventRefinery();
		virtual ~AgpmEventRefinery();

		// ... ApModule Inherited		
		BOOL	OnAddModule();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

		// ... Pakcet
		PVOID	MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
		PVOID	MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);

		// ... Setting Callback
		BOOL	SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		// ... Callback
		static BOOL	CBAction(PVOID pData, PVOID pClass, PVOID pCustData);

		// ... Event Callbacks
		static BOOL	CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventStreamWrite(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventStreamRead(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventPacketMake(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventPacketReceive(PVOID pData, PVOID pClass, PVOID pCustData);

	private:
		// ... Operation
		BOOL	OnOperationRequest(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);
		BOOL	OnOperationGrant(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);

		// ... Custom Event Packet(Embedded)
		PVOID	MakePacketCustomEvent(ApdEvent *pApdEvent);
		BOOL	ParsePacketCustomEvent(ApdEvent *pApdEvent, PVOID pvPacketCustom);
	};

#endif
