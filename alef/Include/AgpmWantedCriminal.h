/*====================================================================

	AgpmWantedCriminal.h
	
====================================================================*/


#ifndef _AGPM_WANTEDCRIMINAL_H_
	#define _AGPM_WANTEDCRIMINAL_H_


#include "ApBase.h"
#include "ApModule.h"
#include "ApmMap.h"
#include "ApmEventManager.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AuGenerateID.h"
#include "AgpdWantedCriminal.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const INT32	AGPMWANTEDCRIMINAL_MAX_VISIBLE_RANK =			100;
const INT32	AGPMWANTEDCRIMINAL_MAX_USE_RANGE =				1600;
const INT32	AGPMWANTEDCRIMINAL_MAX_CRIMINAL_PER_PAGE =		10;
const INT32 AGPMWANTEDCRIMINAL_MAX_PAGE =					10;
const INT32 AGPMWANTEDCRIMINAL_END_OF_LIST =				(-100);

const enum eAGPMWANTEDCRIMINAL_DATATYPE
	{
	AGPMWANTEDCRIMINAL_DATATYPE_WC = 0,
	};


const enum eAGPMWANTEDCRIMINAL_OPERATION
	{
	AGPMWANTEDCRIMINAL_OPERATION_NONE = -1,
	AGPMWANTEDCRIMINAL_OPERATION_EVENT_REQUEST = 0,
	AGPMWANTEDCRIMINAL_OPERATION_EVENT_GRANT,
	AGPMWANTEDCRIMINAL_OPERATION_ARRANGE,
	AGPMWANTEDCRIMINAL_OPERATION_REQ_LIST,
	AGPMWANTEDCRIMINAL_OPERATION_RES_LIST,
	AGPMWANTEDCRIMINAL_OPERATION_MY_RANK,
	AGPMWANTEDCRIMINAL_OPERATION_UPDATE_KILLER,
	AGPMWANTEDCRIMINAL_OPERATION_UPDATE_FLAG,
	AGPMWANTEDCRIMINAL_OPERATION_NOTIFY,
	AGPMWANTEDCRIMINAL_OPERATION_MAX
	};


const enum eAGPMWANTEDCRIMINAL_CB
	{
	AGPMWANTEDCRIMINAL_CB_NONE = -1,
	AGPMWANTEDCRIMINAL_CB_EVENT_REQUEST,
	AGPMWANTEDCRIMINAL_CB_EVENT_GRANT,
	AGPMWANTEDCRIMINAL_CB_ARRANGE,
	AGPMWANTEDCRIMINAL_CB_REQ_LIST,
	AGPMWANTEDCRIMINAL_CB_RES_LIST,
	AGPMWANTEDCRIMINAL_CB_MY_RANK,
	AGPMWANTEDCRIMINAL_CB_UPDATE_KILLER,
	AGPMWANTEDCRIMINAL_CB_UPDATE_FLAG,
	AGPMWANTEDCRIMINAL_CB_NOTIFY,
	AGPMWANTEDCRIMINAL_CB_MAX
	};


const enum eAGPMWANTEDCRIMINAL_RESULT
	{
	AGPMWANTEDCRIMINAL_RESULT_NONE = -1,
	AGPMWANTEDCRIMINAL_RESULT_SUCCESS,
	AGPMWANTEDCRIMINAL_RESULT_FAIL_NOT_ENOUGH_BASE_MONEY,
	AGPMWANTEDCRIMINAL_RESULT_FAIL_NOT_ENOUGH_MONEY,
	AGPMWANTEDCRIMINAL_RESULT_FAIL_ALREADY_ARRANGED,
	AGPMWANTEDCRIMINAL_RESULT_KILL_WANTED_CRIMINAL,
	AGPMWANTEDCRIMINAL_RESULT_DEAD_WANTED_CRIMINAL,
	AGPMWANTEDCRIMINAL_RESULT_MAX,
	};


const enum eAGPMWANTEDCRIMINAL_BASE_BOUNTY
	{
	AGPMWANTEDCRIMINAL_BASE_BOUNTY_1 = 0,
	AGPMWANTEDCRIMINAL_BASE_BOUNTY_2,
	AGPMWANTEDCRIMINAL_BASE_BOUNTY_3,
	AGPMWANTEDCRIMINAL_BASE_BOUNTY_4,
	AGPMWANTEDCRIMINAL_BASE_BOUNTY_5,
	AGPMWANTEDCRIMINAL_BASE_BOUNTY_MAX,
	};




/********************************************************/
/*		The Definition of AgpmWantedCriminal class		*/
/********************************************************/
//
class AgpmWantedCriminal : public ApModule
	{
	public:
		static AgpdWantedCriminalBaseBounty	m_BaseBounty[AGPMWANTEDCRIMINAL_BASE_BOUNTY_MAX];
	private:
		//	Related modules
		ApmEventManager	*m_pApmEventManager;
		AgpmCharacter	*m_pAgpmCharacter;
		
		//	CAD
		INT16			m_nIndexCharacterAD;
		
		//	Map
		ApAdmin			m_csAdmin;

	public:
		//	Packet
		AuPacket		m_csPacket;
		AuPacket		m_csPacketWC;		// for AgpdWantedCriminal

	public:
		AgpmWantedCriminal();
		virtual ~AgpmWantedCriminal();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

		//	Module data
		AgpdWantedCriminal*	CreateWantedCriminal();
		void				DestroyWantedCriminal(AgpdWantedCriminal *pAgpdWantedCriminal);
		
		//	Map
		BOOL				SetMaxWantedCriminal(INT32 lCount);		
		BOOL				AddWantedCriminal(AgpdWantedCriminal *pAgpdWantedCriminal);
		BOOL				RemoveWantedCriminal(AgpdWantedCriminal *pAgpdWantedCriminal);
		AgpdWantedCriminal*	GetWantedCriminal(TCHAR *pszCharID);
		AgpdWantedCriminal* GetWantedCriminal(AgpdCharacter *pAgpdCharacter);
		
		//	CAD
		static BOOL	ConAgpdWantedCriminalCAD(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	DesAgpdWantedCriminalCAD(PVOID pData, PVOID pClass, PVOID pCustData);
		AgpdWantedCriminalCAD*	GetCAD(ApBase *pApBase);

		//	Operations
		BOOL	OnOperationEventRequest(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter);
		BOOL	OnOperationEventGrant(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter);

		//	Helper
		BOOL	IsEnoughBaseBounty(AgpdCharacter *pAgpdCharacter);
		BOOL	IsEnoughBounty(AgpdCharacter *pAgpdCharacter, INT64 llMoney);
		TCHAR*	GetFinalAttacker(AgpdCharacter *pAgpdCharacter);
		void	SetFinalAttacker(AgpdCharacter *pAgpdCharacter, TCHAR *pszKiller);
		BOOL	IsFinalAttacker(AgpdCharacter *pAgpdCharacter, TCHAR *pszKiller);
		INT64	GetBaseBounty(AgpdCharacter *pAgpdCharacter);
		INT64	GetBaseBounty(INT32 lLevel);

		//	Packet
		PVOID	MakePacketWC(AgpdWantedCriminal *pAgpdWantedCriminal, INT16 *pnPacketLength);
		BOOL	ParsePacketWC(PVOID pvPacketEmb, AgpdWantedCriminal *pAgpdWantedCriminal);

		//	Event Packet
		PVOID	MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
		PVOID	MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);

		//	Event Callbacks
		static BOOL	CBEventAction(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Callback setting
		BOOL	SetCallbackEventRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);		
		BOOL	SetCallbackArrange(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackRequestList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackResponseList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackMyRank(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackUpdateKiller(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackUpdateFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackNotify(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	};


#endif