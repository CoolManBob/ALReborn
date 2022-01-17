//2003.2.26 -Ashulam-

#ifndef	__AGPM_EVENT_NPCTRADE_H__
#define __AGPM_EVENT_NPCTRADE_H__

#include "ApModule.h"
#include "AuExcelTxtLib.h"
#include "ApmEventManager.h"
#include "AgpdEventNPCTrade.h"
#include "AgpmGrid.h"

#define AGPMEVENT_NPCTRADE_MAX_NPC				1000

//NPCTrade는 4미터 안에서만 거래가 가능하다.
#define	AGPMEVENT_NPCTRADE_MAX_USE_RANGE		800

#define AGPMEVENT_NPCTRADE_INI_NAME_START	"NPCTradeStart"
#define AGPMEVENT_NPCTRADE_INI_NAME_END		"NPCTradeEnd"
#define AGPMEVENT_NPCTRADE_INI_TEMPLATE		"Template"

//NPC Trade Operation
typedef enum
{
	AGPDEVENTNPCTRADE_REQUESTITEMLIST	= 0,
	AGPDEVENTNPCTRADE_SEND_ALL_ITEM_INFO,
	AGPDEVENTNPCTRADE_BUYITEM,
	AGPDEVENTNPCTRADE_SELLITEM
} AgpdNPCTradeOperation;

//NPC Trade Operation
typedef enum
{
	AGPDEVENTNPCTRADE_RESULT_SELL_SUCCEEDED	= 0,
	AGPDEVENTNPCTRADE_RESULT_SELL_EXCEED_MONEY,
	AGPDEVENTNPCTRADE_RESULT_BUY_SUCCEEDED,
	AGPDEVENTNPCTRADE_RESULT_BUY_NOT_ENOUGH_MONEY,
	AGPDEVENTNPCTRADE_RESULT_BUY_FULL_INVENTORY,
	AGPDEVENTNPCTRADE_RESULT_INVALID_ITEMID,
	AGPDEVENTNPCTRADE_RESULT_INVALID_ITEM_COUNT,
	AGPDEVENTNPCTRADE_RESULT_INVALID_USE_CLASS,
	AGPDEVENTNPCTRADE_RESULT_CANNOT_BUY,
	AGPDEVENTNPCTRADE_RESULT_NO_SELL_TO_MURDERER,
} AgpdNPCTradeResult;

//CallBack함수 리스트.
typedef enum AgpmEventNPCTradeCallbackPoint			// Callback ID in Character Module 
{
	AGPMEVENT_NPCTRADE_CB_ID_DELETE_GRID	= 0
} AgpmEventNPCTradeCallbackPoint;

class AgpmEventNPCTrade : public ApModule  
{
	AuExcelTxtLib		m_csExcelTxtLib;

	ApmEventManager		*m_pcsApmEventManager;
	AgpmGrid			*m_pcsAgpmGrid;

	ApAdmin				m_csNPCTradeTemplate;
	ApAdmin				m_csNPCTradeItemGroup;

public:
	AgpmEventNPCTrade();
	virtual ~AgpmEventNPCTrade();

	BOOL			OnAddModule();

	static BOOL			CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL LoadNPCTradeRes( char *pstrFileName , BOOL bDecryption = TRUE );

	ApAdmin *GetTemplate();
	AgpdEventNPCTradeTemplate *GetNPCTradeTemplate( INT32 lTemplateTID );
	ApAdmin *GetGroupItemTemplate();
	BOOL	IsItemInGrid( AgpdEventNPCTradeTemplate *pcsTemplate, INT32 lItemTID );

	BOOL	SetCallbackDeleteGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif //__AGPD_EVENT_NPCTRADE_H__