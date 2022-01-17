/*====================================================================

	AgpmGuildWarehouse.h
	
====================================================================*/


#ifndef _AGPM_GUILD_WAREHOUSE_H_
	#define _AGPM_GUILD_WAREHOUSE_H_


#include "ApBase.h"
#include "ApModule.h"
#include "AgpdGuildWarehouse.h"
#include "AgpmCharacter.h"
#include "AgpmGuild.h"
#include "AgpmItem.h"
#include "AgpmGrid.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGPMGUILDWAREHOUSE_OPERATION
	{
	AGPMGUILDWAREHOUSE_OPERATION_NONE = -1,
	AGPMGUILDWAREHOUSE_OPERATION_BASE_INFO,
	AGPMGUILDWAREHOUSE_OPERATION_OPEN,
	AGPMGUILDWAREHOUSE_OPERATION_CLOSE,
	AGPMGUILDWAREHOUSE_OPERATION_MONEY_IN,
	AGPMGUILDWAREHOUSE_OPERATION_MONEY_OUT,
	AGPMGUILDWAREHOUSE_OPERATION_ITEM_IN,
	AGPMGUILDWAREHOUSE_OPERATION_ITEM_OUT,
	AGPMGUILDWAREHOUSE_OPERATION_EXPAND,
	};


const enum eAGPMGUILDWAREHOUSE_CB
	{
	AGPMGUILDWAREHOUSE_CB_NONE = -1,
	AGPMGUILDWAREHOUSE_CB_BASE_INFO,
	AGPMGUILDWAREHOUSE_CB_OPEN,
	AGPMGUILDWAREHOUSE_CB_CLOSE,
	AGPMGUILDWAREHOUSE_CB_MONEY_IN,
	AGPMGUILDWAREHOUSE_CB_MONEY_OUT,
	AGPMGUILDWAREHOUSE_CB_ITEM_IN,
	AGPMGUILDWAREHOUSE_CB_ITEM_OUT,
	AGPMGUILDWAREHOUSE_CB_EXPAND,
	AGPMGUILDWAREHOUSE_CB_UPDATE_MONEY,
	AGPMGUILDWAREHOUSE_CB_ADD_ITEM,
	AGPMGUILDWAREHOUSE_CB_REMOVE_ITEM,
	AGPMGUILDWAREHOUSE_CB_MAX
	};


const enum eAGPMGUILDWAREHOUSE_RESULT
	{
	AGPMGUILDWAREHOUSE_RESULT_NONE = -1,
	AGPMGUILDWAREHOUSE_RESULT_SUCCESS,
	AGPMGUILDWAREHOUSE_RESULT_NOT_ENOUGH_PREV,
	AGPMGUILDWAREHOUSE_RESULT_LOAD_ENDED,
	AGPMGUILDWAREHOUSE_RESULT_RETRY,
	AGPMGUILDWAREHOUSE_RESULT_NOT_ENOUGH_MONEY,
	AGPMGUILDWAREHOUSE_RESULT_MAX,
	AGPMGUILDWAREHOUSE_RESULT_INVEN_MONEY_EXCEED,
	};




/********************************************************/
/*		The Definition of AgpmGuildWarehouse class		*/
/********************************************************/
//
class AgpmGuildWarehouse : public ApModule
	{
	private :
		//	Related modules
		AgpmCharacter	*m_pAgpmCharacter;
		AgpmGuild		*m_pAgpmGuild;
		AgpmItem		*m_pAgpmItem;
		AgpmGrid		*m_pAgpmGrid;
		
		//	Attached Data
		INT16			m_nIndexGuildAD;
		
	public:
		//	Packet
		AuPacket		m_csPacket;

	public:
		AgpmGuildWarehouse();
		virtual ~AgpmGuildWarehouse();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

		//	Attached Data
		AgpdGuildWarehouse*		GetGuildWarehouse(AgpdGuild *pAgpdGuild);
		AgpdGuildWarehouse*		GetGuildWarehouse(TCHAR *pszGuild);
		AgpdGuildWarehouse*		GetGuildWarehouse(AgpdCharacter *pAgpdCharacter);
		static BOOL	ConAgpdGuildWarehouse(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	DesAgpdGuildWarehouse(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Operations
		BOOL	AddItem(AgpdGuildWarehouse *pAgpdGuildWarehouse, AgpdItem *pAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bCallback = FALSE);
		BOOL	AddItem(AgpdGuild *pAgpdGuild, AgpdItem *pAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bCallback = FALSE);
		BOOL	AddItem(AgpdGuildWarehouse *pAgpdGuildWarehouse, AgpdItem *pAgpdItem, BOOL bCallback = TRUE);
		BOOL	AddItem(AgpdGuild *pAgpdGuild, AgpdItem *pAgpdItem, BOOL bCallback = FALSE);
		BOOL	RemoveItem(AgpdGuildWarehouse *pAgpdGuildWarehouse, AgpdItem *pAgpdItem);
		BOOL	RemoveItem(AgpdGuild *pAgpdGuild, AgpdItem *pAgpdItem);
		BOOL	AddMoney(AgpdGuildWarehouse *pAgpdGuildWarehouse, INT64 llMoney);
		BOOL	AddMoney(AgpdGuild *pAgpdGuild, INT64 llMoney);
		BOOL	AddMoney(TCHAR *pszGuild, INT64 llMoney);
		BOOL	SubMoney(AgpdGuildWarehouse *pAgpdGuildWarehouse, INT64 llMoney);
		BOOL	SubMoney(AgpdGuild *pAgpdGuild, INT64 llMoney);
		BOOL	SubMoney(TCHAR *pszGuild, INT64 llMoney);
		BOOL	ClearAllItems(AgpdGuildWarehouse *pAgpdGuildWarehouse);
		BOOL	ClearAllItems(AgpdCharacter *pAgpdCharacter);
		
		//	Validation
		BOOL	CheckPreviledge(AgpdCharacter *pAgpdCharacter, AgpdGuild *pAgpdGuild, eAGPDGUILDWAREHOUSE_PREV ePrev);
		BOOL	CheckPreviledge(AgpdCharacter *pAgpdCharacter, eAGPDGUILDWAREHOUSE_PREV ePrev);
				
		//	Module Callbacks

		//	Callback setting
		BOOL	SetCallbackBaseInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackOpen(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackClose(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackMoneyIn(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackMoneyOut(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackItemIn(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackItemOut(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackExpand(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackUpdateMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackAddItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackRemoveItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	};


#endif