/*====================================================================

	AgpmTax.h
	
====================================================================*/


#ifndef _AGPM_TAX_H_
	#define _AGPM_TAX_H_


#include "ApBase.h"
#include "ApModule.h"
#include "ApmMap.h"
#include "ApmEventManager.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AuGenerateID.h"
#include "AgpdTax.h"
#include "AgpmSiegeWar.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define	AGPMTAX_MAX_USE_RANGE			1600


const enum eAGPMTAX_OPERATION
	{
	AGPMTAX_OPERATION_NONE = -1,
	AGPMTAX_OPERATION_EVENT_REQUEST = 0,
	AGPMTAX_OPERATION_EVENT_GRANT,
	AGPMTAX_OPERATION_MODIFY_RATIO,
	AGPMTAX_OPERATION_INQUIRE_TAX,
	AGPMTAX_OPERATION_NOTIFY_RATIO,
	AGPMTAX_OPERATION_MAX
	};


const enum eAGPMTAX_CB
	{
	AGPMTAX_CB_NONE = -1,
	AGPMTAX_CB_EVENT_REQUEST,
	AGPMTAX_CB_EVENT_GRANT,
	AGPMTAX_CB_MODIFY_RATIO,
	AGPMTAX_CB_INQUIRE_TAX,
	AGPMTAX_CB_MAX
	};


const enum eAGPMTAX_RESULT
	{
	AGPMTAX_RESULT_NONE = -1,
	AGPMTAX_RESULT_SUCCESS,
	AGPMTAX_RESULT_FAIL_NOT_OWNER,
	AGPMTAX_RESULT_FAIL_NOT_TIME,
	AGPMTAX_RESULT_FAIL_EXCEED_RANGE,
	AGPMTAX_RESULT_MAX,
	};


const enum eAGPMTAX_RATIO
	{
	AGPMTAX_RATIO_0 = 0,
	AGPMTAX_RATIO_1,
	AGPMTAX_RATIO_2,
	AGPMTAX_RATIO_3,
	AGPMTAX_RATIO_4,
	AGPMTAX_RATIO_5,
	AGPMTAX_RATIO_MAX,
	};




/************************************/
/*		The Definition of Map		*/
/************************************/
//
typedef map<string, AgpdRegionTax *>	RegionTaxMap;
typedef pair<string, AgpdRegionTax *>	RegionTaxPair;
typedef RegionTaxMap::iterator			RegionTaxIter;
typedef map<string, AgpdTax *>			CastleTaxMap;
typedef pair<string, AgpdTax *>			CastleTaxPair;
typedef CastleTaxMap::iterator			CastleTaxIter;



/********************************************/
/*		The Definition of AgpmTax class		*/
/********************************************/
//
class AgpmTax : public ApModule
	{
	public :
		static const INT32	s_lBaseRatio[AGPMTAX_RATIO_MAX];
	
	private :
		//	Related modules
		ApmMap			*m_pApmMap;
		ApmEventManager	*m_pApmEventManager;
		AgpmCharacter	*m_pAgpmCharacter;
		AgpmSiegeWar	*m_pAgpmSiegeWar;
		
		//	Map
		RegionTaxMap	m_RegionTaxMap;
		CastleTaxMap	m_CastleTaxMap;
		
	public:
		//	Packet
		AuPacket		m_csPacket;
		AuPacket		m_csPacketCastleTax;
		AuPacket		m_csPacketRegionTax;

	public:
		AgpmTax();
		virtual ~AgpmTax();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

		//	Operations
		BOOL	OnOperationEventRequest(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter);
		BOOL	OnOperationEventGrant(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter);

		//	Tax
		AgpdTax*		GetTax(TCHAR *pszCastle);
		AgpdTax*		GetTax(INT32 *plIndex);
		AgpdTax*		GetTax(AgpdCharacter *pAgpdCharacter);
		AgpdRegionTax*	GetRegionTax(TCHAR *pszRegion);
		BOOL			UpdateCastleTax(TCHAR *pszCastle, INT64 llTotalIncome, UINT32 ulLatestTransferDate, UINT32 ulLatestModifyDate);
		BOOL			UpdateRegionTax(TCHAR *pszRegion, INT64 llIncome, INT32 lRatio, TCHAR *pszCastle = NULL);
		INT32			GetTaxRatio(TCHAR *pszRegion);
		INT32			GetTaxRatio(AgpdCharacter *pAgpdCharacter);
		INT32			GetTaxRatioArchlordCastle();
		TCHAR*			GetRegion(AgpdCharacter *pAgpdCharacter);
		INT16			GetRegionIndex(TCHAR *pszRegion);
		TCHAR*			GetCastle(AgpdCharacter *pAgpdCharacter);

		INT64			GetTaxSum(AgpdTax* pAgpdTax);
		
		//	Validation
		BOOL	IsValidRatio(AgpdTax *pAgpdTax);
		BOOL	IsValidDate(UINT32 ulLatestModifyDate);
		BOOL	IsCastleOwner(AgpdCharacter *pAgpdCharacter, TCHAR *pszCastle = NULL);
		
		//	Packet Parsing
		BOOL	ParseCastleTax(PVOID pvPacket, AgpdTax *pAgpdTax);
		BOOL	ParseRegionTax(PVOID pvPacket, AgpdRegionTax *pAgpdRegionTax);

		//	Event Packet
		PVOID	MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
		PVOID	MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);

		//	Event Callbacks
		static BOOL	CBEventAction(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Module Callbacks
		static BOOL	CBGetTaxRatio(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Callback setting
		BOOL	SetCallbackEventRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);		
		BOOL	SetCallbackModifyRatio(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackInquireTax(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	};


#endif