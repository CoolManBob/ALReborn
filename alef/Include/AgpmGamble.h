/*====================================================================

	AgpmGamble.h
	
====================================================================*/


#ifndef _AGPM_GAMBLE_H_
	#define _AGPM_GAMBLE_H_


#include "ApBase.h"
#include "ApModule.h"
#include "ApmMap.h"
#include "ApmEventManager.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmGrid.h"
#include "AgpmFactors.h"
#include "AgpmDropItem2.h"
#include "AgpdGamble.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define	AGPMGAMBLE_MAX_USE_RANGE			1600


const enum eAGPMGAMBLE_OPERATION
	{
	AGPMGAMBLE_OPERATION_NONE = -1,
	AGPMGAMBLE_OPERATION_EVENT_REQUEST = 0,
	AGPMGAMBLE_OPERATION_EVENT_GRANT,
	AGPMGAMBLE_OPERATION_GAMBLE,
	AGPMGAMBLE_OPERATION_MAX
	};


const enum eAGPMGAMBLE_CB
	{
	AGPMGAMBLE_CB_NONE = -1,
	AGPMGAMBLE_CB_EVENT_REQUEST,
	AGPMGAMBLE_CB_EVENT_GRANT,
	AGPMGAMBLE_CB_GAMBLE,
	AGPMGAMBLE_CB_MAX
	};


const enum eAGPMGAMBLE_RESULT
	{
	AGPMGAMBLE_RESULT_NONE = -1,
	AGPMGAMBLE_RESULT_SUCCESS,
	AGPMGAMBLE_RESULT_NA_RACE,
	AGPMGAMBLE_RESULT_NA_CLASS,
	AGPMGAMBLE_RESULT_NOT_ENOUGH_LEVEL,
	AGPMGAMBLE_RESULT_NOT_ENOUGH_POINT,
	AGPMGAMBLE_RESULT_FULL_INVENTORY,
	AGPMGAMBLE_RESULT_MAX,
	};


const enum AGPMGAMBLE_EXCEL_COLUMN_INDEX
	{
	AGPMGAMBLE_EXCEL_COLUMN_IMAGEID = 0,
	AGPMGAMBLE_EXCEL_COLUMN_NAME,
	AGPMGAMBLE_EXCEL_COLUMN_CLASS,
	AGPMGAMBLE_EXCEL_COLUMN_RACE,
	AGPMGAMBLE_EXCEL_COLUMN_TYPE,
	AGPMGAMBLE_EXCEL_COLUMN_COST,
	AGPMGAMBLE_EXCEL_COLUMN_RANK1,
	AGPMGAMBLE_EXCEL_COLUMN_RANK2,
	AGPMGAMBLE_EXCEL_COLUMN_RANK3,
	};



/************************************/
/*		The Definition of Map		*/
/************************************/
//
typedef map<INT32, AgpdGamble>					GambleMap;
typedef pair<INT32, AgpdGamble>					GambleMapPair;
typedef GambleMap::iterator						GambleMapIter;

typedef map<INT32, AgpdGambleItemList *>		GambleItemMap;
typedef pair<INT32, AgpdGambleItemList *>		GambleItemMapPair;
typedef GambleItemMap::iterator					GambleItemMapIter;



/************************************************/
/*		The Definition of AgpmGamble class		*/
/************************************************/
//
class AgpmGamble : public ApModule
	{
	private :
		//	Related modules
		ApmEventManager	*m_pApmEventManager;
		AgpmCharacter	*m_pAgpmCharacter;
		AgpmItem		*m_pAgpmItem;
		AgpmGrid		*m_pAgpmGrid;
		AgpmFactors		*m_pAgpmFactors;
		AgpmDropItem2	*m_pAgpmDropItem2;
		
	public:
		//	Map
		GambleMap		m_TemplateMap;
		GambleItemMap	m_GambleItemMap;

		//	Packet
		AuPacket		m_csPacket;

	public:
		AgpmGamble();
		virtual ~AgpmGamble();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
		BOOL	OnDestroy();

		//	Operations
		BOOL	OnOperationEventRequest(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter);
		BOOL	OnOperationEventGrant(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter);

		//	Map
		AgpdGamble*			GetTemplate(INT32 lTID);
		AgpdGambleItemList*	GetGambleItemList(INT32 lTID);
		BOOL				MakeGambleItemMap();

		//	Character level specefic item list
		INT32	GetGambleItemList(AgpdGamble *pAgpdGamble, AgpdCharacter *pAgpdCharacter, AgpdGambleItemList *pList);
		INT32	GetGambleItemList(INT32 lTID, AgpdCharacter *pAgpdCharacter, AgpdGambleItemList *pList);

		//	Validation
		BOOL	IsValid(AgpdCharacter *pAgpdCharacter, AgpdGamble *pAgpdGamble, INT32 *plResult = NULL);
		BOOL	IsValid(AgpdCharacter *pAgpdCharacter, INT32 lGambleTID, INT32 *plResult = NULL);
		INT32	Cost(AgpdCharacter *pAgpdCharacter, AgpdGamble *pAgpdGamble);
		
		//	Strem
		BOOL	StreamReadGamble(CHAR *pszFile, BOOL bDecryption);
		
		//	Event Callback
		static BOOL	CBEventAction(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Event Packet
		PVOID	MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
		PVOID	MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);

		//	Callback setting
		BOOL	SetCallbackEventRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);		
		BOOL	SetCallbackGamble(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	private :	
		//	Item Type Check
		BOOL	_CheckItemAllType(AgpdItemTemplate *pAgpdItemTemplate1, AgpdItemTemplate *pAgpdItemTemplate2);
	};


#endif