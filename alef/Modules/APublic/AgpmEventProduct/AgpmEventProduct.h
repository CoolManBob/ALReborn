/*====================================================================

	AgpmEventProduct.h
	
====================================================================*/

#ifndef _AGPM_EVENT_PRODUCT_H_
	#define _AGPM_EVENT_PRODUCT_H_

#include "AgpmCharacter.h"
#include "AgpmProduct.h"
#include "ApmEventManager.h"
#include "AgpmEventSkillMaster.h"
#include "AuPacket.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgpmEventProductD.lib")
#else
	#pragma comment(lib, "AgpmEventProduct.lib")
#endif
#endif

/****************************************/
/*		the Definition of Constants		*/
/****************************************/
//
#define	AGPMEVENT_PRODUCT_MAX_USE_RANGE				1600
#define AGPMEVENT_PRODUCT_STREAM_NAME_CATEGORY		"ProductCategory"
#define AGPMEVENT_PRODUCT_STREAM_NAME_END			"ProductEnd"

enum eAGPMEVENT_PRODUCT_PACKET
	{
	AGPMEVENT_PRODUCT_PACKET_REQUEST = 0,
	AGPMEVENT_PRODUCT_PACKET_GRANT,
	AGPMEVENT_PRODUCT_PACKET_LEARN_SKILL,
	AGPMEVENT_PRODUCT_PACKET_LEARN_SKILL_RESULT,	
	AGPMEVENT_PRODUCT_PACKET_BUY_COMPOSE,
	AGPMEVENT_PRODUCT_PACKET_BUY_COMPOSE_RESULT,	
	};

enum eAGPMEVENT_PRODUCT_CB
	{
	AGPMEVENT_PRODUCT_CB_REQUEST = 0,
	AGPMEVENT_PRODUCT_CB_GRANT,
	AGPMEVENT_PRODUCT_CB_LEARN_SKILL,
	AGPMEVENT_PRODUCT_CB_LEARN_SKILL_RESULT,
	AGPMEVENT_PRODUCT_CB_BUY_COMPOSE,
	AGPMEVENT_PRODUCT_CB_BUY_COMPOSE_RESULT,
	};

enum eAGPMEVENT_PRODUCT_RESULT
	{
	AGPMEVENT_PRODUCT_RESULT_SUCCESS = 0,
	AGPMEVENT_PRODUCT_RESULT_FAIL,
	AGPMEVENT_PRODUCT_RESULT_FAIL_ALREADY_BOUGHT,
	AGPMEVENT_PRODUCT_RESULT_FAIL_NOT_ENOUGH_MONEY,
	AGPMEVENT_PRODUCT_RESULT_FAIL_INSUFFICIENT_LEVEL,
	AGPMEVENT_PRODUCT_RESULT_FAIL_NEED_PREREQUISITES,
	};

/****************************************************/
/*		The Definition of Event Attached Data		*/
/****************************************************/
//
class AgpdEventProduct
	{
	public:
		INT32		m_eCategory;
	};

/********************************************************/
/*		The Definition of AgpdEventProduct class		*/
/********************************************************/
//
class AgpmEventProduct : public ApModule
	{
	private:
		AgpmCharacter			*m_pAgpmCharacter;
		AgpmProduct				*m_pAgpmProduct;
		ApmEventManager			*m_pApmEventManager;
		AgpmEventSkillMaster	*m_pAgpmEventSkillMaster;
		
	public:
		AuPacket	m_csPacket;
		AuPacket	m_csPacketEmb;
		
	public:
		AgpmEventProduct();
		virtual ~AgpmEventProduct();

		//	ApModule inherited
		BOOL OnAddModule();
		BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

		//	Validation
		BOOL	IsValidLearnSkill(AgpdCharacter *pAgpdCharacter, AgpdSkillTemplate *pAgpdSkillTemplate, INT32 *plResult = NULL, INT64 *pllPrice = NULL, INT32* plTax = 0);
		BOOL	IsValidLearnSkill(AgpdCharacter *pAgpdCharacter, INT32 lSkillTID, INT32 *plResult = NULL, INT64 *pllPrice = NULL, INT32 *plTax = 0);
		BOOL	IsValidBuyCompose(AgpdCharacter *pAgpdCharacter, AgpdComposeTemplate *pAgpdComposeTemplate, INT32 *plResult = NULL);
		BOOL	IsSufficientLevel(AgpdSkill *pAgpdSkill, INT32 lComposeLevel);

		//	
		INT32	GetBuyCost(INT32 lSkillTID, AgpdCharacter *pAgpdCharacter, INT32* plTax);
		INT32	GetBuyCost(AgpdSkillTemplate *pAgpdSkillTemplate, AgpdCharacter *pAgpdCharacter, INT32* plTax);
		INT32	GetLearnableLevel(INT32 lSkillTID);
		INT32	GetLearnableLevel(AgpdSkillTemplate *pAgpdSkillTemplate);

		//	Operations
		BOOL OnOperationRequest(ApdEvent* pApdEvent, AgpdCharacter* pAgpdCharacter);
		BOOL OnOperationGrant(ApdEvent* pApdEvent, AgpdCharacter* pAgpdCharacter);
		BOOL OnOperationBuyCompose(ApdEvent* pApdEvent, AgpdCharacter* pAgpdCharacter, INT32 lComposeID);
		BOOL OnOperationBuyComposeResult(AgpdCharacter* pAgpdCharacter, INT32 lComposeID, INT32 lResult);
		BOOL OnOperationLearnSkill(ApdEvent* pApdEvent, AgpdCharacter* pAgpdCharacter, INT32 lSkillTID);
		BOOL OnOperationLearnSkillResult(AgpdCharacter* pAgpdCharacter, INT32 lSkillTID, INT32 lResult);

		//	Packet
		PVOID	MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
		PVOID	MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
		PVOID	MakePacketBuyCompose(ApdEvent *pApdEvent, INT32 lCID, INT32 lComposeID, INT16 *pnPacketLength);
		PVOID	MakePacketBuyComposeResult(INT32 lCID, INT32 lComposeID, INT32 lResult, INT16 *pnPacketLength);
		PVOID	MakePacketLearnSkill(ApdEvent *pApdEvent, INT32 lCID, INT32 lSkillTID, INT16 *pnPacketLength);
		PVOID	MakePacketLearnSkillResult(INT32 lCID, INT32 lSkillTID, INT32 lResult, INT16 *pnPacketLength);

		//	Callback setting
		BOOL	SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackBuyCompose(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackBuyComposeResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackLearnSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackLearnSkillResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		static BOOL CBActionProduct(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Event Data
		PVOID	MakePacketEventData(ApdEvent *pApdEvent);
		BOOL	ParsePacketEventData(ApdEvent *pApdEvent, PVOID pvPacketCustom);
		
		static BOOL	CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);
		
		static BOOL CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);
		
		static BOOL	CBEmbeddedPacketEncode(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEmbeddedPacketDecode(PVOID pData, PVOID pClass, PVOID pCustData);
	};

#endif
