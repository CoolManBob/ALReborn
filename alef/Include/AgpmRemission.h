/*===========================================================================

	AgpmRemission.h
	
===========================================================================*/

#ifndef _AGPM_REMISSION_H_
	#define _AGPM_REMISSION_H_

#include "ApModule.h"
#include "AuPacket.h"

#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "ApmEventManager.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
//	==========	Operations	==========
//
const enum eAGPMREMISSION_OPERATION
	{
	AGPMREMISSION_OPERATION_EVENT_REQUEST = 0,
	AGPMREMISSION_OPERATION_EVENT_GRANT,
	AGPMREMISSION_OPERATION_REMIT,
	};


//
//	==========	Callback IDs	==========
//
const enum eAGPMREMISSION_CB
	{
	AGPMREMISSION_CB_EVENT_REQUEST = 0,
	AGPMREMISSION_CB_EVENT_GRANT,
	AGPMREMISSION_CB_REMIT
	};


//
//	==========	Remission Type	==========
//
const enum eAGPMREMISSION_TYPE
	{
	AGPMREMISSION_TYPE_NONE = 0, 
	AGPMREMISSION_TYPE_TEST,
	AGPMREMISSION_TYPE_GHELD,
	AGPMREMISSION_TYPE_SKELETON,
	AGPMREMISSION_TYPE_INDULGENCE,
	AGPMREMISSION_TYPE_MAX,
	};


//
//	==========	Result Code	==========
//
const enum eAGPMREMISSION_RESULT
	{
	AGPMREMISSION_RESULT_UNKNOWN = 0,
	AGPMREMISSION_RESULT_SUCCESS,
	AGPMREMISSION_RESULT_REMITABLE,
	AGPMREMISSION_RESULT_FAIL_NOT_MURDERER,
	AGPMREMISSION_RESULT_FAIL_NOT_YET_TIME,					// 소멸대기시간이 안지남
	AGPMREMISSION_RESULT_FAIL_INSUFFICIENT_MONEY,
	AGPMREMISSION_RESULT_FAIL_INSUFFICIENT_SKELETON,
	AGPMREMISSION_RESULT_FAIL_INSUFFICIENT_INDULGENCE,
	};


//
//	==========	Payment	==========
//
#define AGPMREMISSION_PAY_GHELD					5000000
#define AGPMREMISSION_PAY_SKELETON_TID			198
#define AGPMREMISSION_PAY_INDULGENCE_TID		3311
#define AGPMREMISSION_PAY_SKELETON_QTY			10
#define AGPMREMISSION_PAY_INDULGENCE_QTY		10

#define AGPMREMISSION_REMIT_POINT_PER_TIME		2
#define AGPMREMISSION_MAX_USE_RANGE				1600




/****************************************************/
/*		The Definition of Argument structure		*/
/****************************************************/
//
struct AgpdRemitArg
	{
	public :
		INT8	m_cType;
		INT32	m_lResult;
	
	AgpdRemitArg()
		{
		m_cType = AGPMREMISSION_TYPE_NONE;
		m_lResult = AGPMREMISSION_RESULT_UNKNOWN;
		}
	};




/****************************************************/
/*		The Definition of AgpmRemission class		*/
/****************************************************/
//
class AgpmRemission : public ApModule
	{
	private :
		AgpmCharacter	*m_pAgpmCharacter;
		AgpmItem		*m_pAgpmItem;
		ApmEventManager	*m_pApmEventManager;
		AgpmGrid		*m_pAgpmGrid;

	public:
		AuPacket		m_csPacket;
		AuPacket		m_csPacketRemit;
			
	public :
		AgpmRemission();
		virtual ~AgpmRemission();
		
		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
		
		//	Operations
		BOOL	OnOperationEventRequest(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter);
		BOOL	OnOperationEventGrant(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter);
		
		//	Validation
		BOOL	IsValidStatus(AgpdCharacter *pAgpdCharacter, INT8 cType, INT32 *plResult = NULL);
		BOOL	IsValidItem(AgpdCharacter *pAgpdCharacter, INT32 lItemTID, INT32 lItemType, INT32 lItemOtherType, INT32 lCount, INT32 *plCount = NULL);
		INT32	GetRemissionItemCountFromInventory(AgpdCharacter* pcsCharacter, INT32 lItemTID, INT32 lItemType, INT32 lItemOtherType);
		INT32	GetRemissionItemCountFromCashInventory(AgpdCharacter* pcsCharacter, INT32 lItemTID, INT32 lItemType, INT32 lItemOtherType);

		//	Callback setting
		BOOL	SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackRemit(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		//	Event Packet
		PVOID	MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
		PVOID	MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
		
		//	Event Callbacks
		static BOOL	CBEventAction(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventStreamWrite(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventStreamRead(PVOID pData, PVOID pClass, PVOID pCustData);
	};


#endif