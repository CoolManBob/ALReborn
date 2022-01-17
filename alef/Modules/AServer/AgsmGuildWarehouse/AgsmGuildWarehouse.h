/*====================================================================

	AgsmGuildWarehouse.h
	
====================================================================*/


#ifndef _AGSM_GUILD_WAREHOUSE_H_
	#define _AGSM_GUILD_WAREHOUSE_H_


#include "AgpmGuildWarehouse.h"
#include "AgsmCharacter.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmGuild.h"
#include "AgpmLog.h"
#include "AgsdBuddy.h"					// for rowset... but informal


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGSMGUILDWAREHOUSE_CB
	{
	AGSMGUILDWAREHOUSE_CB_DB_MONEY_UPDATE = 0,
	AGSMGUILDWAREHOUSE_CB_DB_ITEM_LOAD,
	AGSMGUILDWAREHOUSE_CB_DB_ITEM_IN,
	AGSMGUILDWAREHOUSE_CB_DB_ITEM_OUT,
	};




/********************************************************/
/*		The Definition of AgsmGuildWarehouse class		*/
/********************************************************/
//
class AgsmGuildWarehouse : public AgsModule
	{
	private:
		//	Related modules
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmItem			*m_pAgpmItem;
		AgpmGuild			*m_pAgpmGuild;
		AgpmGrid			*m_pAgpmGrid;
		AgpmGuildWarehouse	*m_pAgpmGuildWarehouse;

		AgsmCharacter		*m_pAgsmCharacter;
		AgsmItem			*m_pAgsmItem;
		AgsmItemManager		*m_pAgsmItemManager;
		AgsmGuild			*m_pAgsmGuild;

		AgpmLog				*m_pAgpmLog;
		
	public:
		AgsmGuildWarehouse();
		virtual ~AgsmGuildWarehouse();

		//	ApModule inherited
		BOOL	OnAddModule();

		//	Operations
		BOOL	OnLoadResult(AgpdCharacter *pAgpdCharacter, stBuddyRowset *pRowset, BOOL bEnd);
		BOOL	OnOpen(AgpdCharacter *pAgpdCharacter);
		BOOL	OnMoneyIn(AgpdCharacter *pAgpdCharacter, INT64 llMoney);
		BOOL	OnMoneyOut(AgpdCharacter *pAgpdCharacter, INT64 llMoney);
		BOOL	OnItemIn(AgpdCharacter *pAgpdCharacter, INT32 lItemID, INT32 lLayer, INT32 lRow, INT32 lColumn);
		BOOL	OnItemOut(AgpdCharacter *pAgpdCharacter, INT32 lItemID, INT32 lStatus, INT32 lLayer, INT32 lRow, INT32 lColumn);
		BOOL	OnUpdateMoney(AgpdGuildWarehouse *pAgpdGuildWarehouse, INT64 llDiff);
		BOOL	OnExpand(AgpdCharacter *pAgpdCharacter);
		BOOL	OnItemRemove(AgpdItem *pAgpdItem);
		
		//	Callbacks
		static BOOL	CBGuildLoaded(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBAfterSendGuildInfo(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOpen(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBClose(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBMoneyIn(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBMoneyOut(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBItemIn(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBItemOut(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBUpdateMoney(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBExpand(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL	CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBCheckGuildDestroy(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Notify
		BOOL	NotifyItemAdd(AgpdGuildWarehouse *pAgpdGuildWarehouse, AgpdItem *pAgpdItem);
		BOOL	NotifyItemRemove(AgpdGuildWarehouse *pAgpdGuildWarehouse, AgpdItem *pAgpdItem, AgpdCharacter *pAgpdCharacterMe);
		BOOL	NotifyMoney(AgpdGuildWarehouse *pAgpdGuildWarehouse, INT64 llMoney);

		//	Packet Send
		BOOL	SendGuildWarehouseBase(AgpdCharacter *pAgpdCharacter, AgpdGuildWarehouse *pAgpdGuildWarehouse);
		BOOL	SendAllItems(AgpdCharacter *pAgpdCharacter, AgpdGuildWarehouse *pAgpdGuildWarehouse);
		BOOL	SendItemAdd(AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem);
		BOOL	SendItemRemove(AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem);
		BOOL	SendOpenResult(AgpdCharacter *pAgpdCharacter, INT32 lResult);
		BOOL	SendMoneyResult(AgpdCharacter *pAgpdCharacter, INT32 lResult);
		BOOL	SendUpdateMoney(AgpdCharacter *pAgpdCharacter, INT64 llMoney);
		BOOL	SendExpandResult(AgpdCharacter *pAgpdCharacter, INT32 lResult);
		
		//	Callback Setting
		BOOL	SetCallbackDBMoneyUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackDBItemLoad(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackDBItemIn(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackDBItemOut(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		
	protected:
		//	Helper
		UINT32	_GetCharacterNID(INT32 lCID);
		UINT32	_GetCharacterNID(AgpdCharacter *pAgpdCharacter);
		
		//	Log
		BOOL	WriteMoneyLog(AgpdCharacter *pAgpdCharacter, INT64 llMoney, BOOL bIn);
	};


#endif
