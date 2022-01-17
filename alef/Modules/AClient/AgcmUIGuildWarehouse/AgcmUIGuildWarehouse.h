/*====================================================================

	AgcmUIGuildWarehouse.h
	
====================================================================*/


#ifndef _AGCM_UI_GUILD_WAREHOUSE_H_
	#define _AGCM_UI_GUILD_WAREHOUSE_H_


#include "AgcModule.h"
#include "AgcmUIManager2.h"
#include "AgpmGrid.h"
#include "AgcmCharacter.h"
#include "AgpmItem.h"
#include "AgpmFactors.h"
#include "AgpmSiegeWar.h"
#include "AgcmItem.h"
#include "AgcmUIControl.h"
#include "AgcmUIConsole.h"
#include "AgpmGuildWarehouse.h"
#include "AgcmChatting2.h"
#include "AgcmUIMain.h"
#include "AgcmUIItem.h"
#include "AgcmEventGuild.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGCMUIGUILDWAREHOUSE_EVENT
	{
	AGCMUIGUILDWAREHOUSE_EVENT_OPEN = 0,
	AGCMUIGUILDWAREHOUSE_EVENT_CLOSE,
	AGCMUIGUILDWAREHOUSE_EVENT_MAX
	};


const enum eAGCMUIGUILDWAREHOUSE_MESSAGE
	{
	AGCMUIGUILDWAREHOUSE_MESSAGE_CAPTION = 0,
	AGCMUIGUILDWAREHOUSE_MESSAGE_NOT_ENOUGH_MONEY,
	AGCMUIGUILDWAREHOUSE_MESSAGE_EXPAND_CONFIRM,
	AGCMUIGUILDWAREHOUSE_MESSAGE_MAX
	};




/****************************************************/
/*		The Definition of AgcmUIGuildWarehouse class		*/
/****************************************************/
//
class AgcmUIGuildWarehouse : public AgcModule
	{
	public:
		static CHAR s_szMessage[AGCMUIGUILDWAREHOUSE_MESSAGE_MAX][30];
		static CHAR s_szEvent[AGCMUIGUILDWAREHOUSE_EVENT_MAX][30];

	public:
		//	Related modules
		AgpmFactors				*m_pAgpmFactors;
		AgpmGrid				*m_pAgpmGrid;
		AgpmCharacter			*m_pAgpmCharacter;
		AgpmItem				*m_pAgpmItem;
		AgpmGuild				*m_pAgpmGuild;
		AgpmGuildWarehouse		*m_pAgpmGuildWarehouse;
		AgpmSiegeWar			*m_pAgpmSiegeWar;
		AgcmCharacter			*m_pAgcmCharacter;
		AgcmEventGuild			*m_pAgcmEventGuild;
		AgcmItem				*m_pAgcmItem;
		AgcmUIManager2			*m_pAgcmUIManager2;
		AgcmUIControl			*m_pAgcmUIControl;
		AgcmUIMain				*m_pAgcmUIMain;
		AgcmUIItem				*m_pAgcmUIItem;

		//	UI events
		INT32					m_lEvent[AGCMUIGUILDWAREHOUSE_EVENT_MAX];

		//	User data
		AgpdGrid				*m_pWarehouseGrid;
		AgcdUIUserData			*m_pstWarehouseGrid;
		INT32					m_lWarehouseLayerIndex;
		AgcdUIUserData			*m_pstWarehouseLayerIndex;
		INT64					m_llWarehouseMoney;
		AgcdUIUserData			*m_pstWarehouseMoney;
		
		AgcdUIUserData			*m_pstActiveButton;
		AgcdUIUserData			*m_pstDummy;
		INT32					m_lActiveButton;
		INT32					m_lDummy;				// dummy for tab

		//	ETC
		BOOL					m_bSetGrid;
		BOOL					m_bMoneyInput;
		BOOL					m_bIsOpen;
		AuPOS					m_stOpenPos;
		
	protected:
		//	General helper
		CHAR*	GetMessageTxt(eAGCMUIGUILDWAREHOUSE_MESSAGE eMessage);

		//	OnAddModule helper
		BOOL	AddEvent();
		BOOL	AddFunction();
		BOOL	AddDisplay();
		BOOL	AddUserData();
		BOOL	AddBoolean();
		
	public:
		AgcmUIGuildWarehouse();
		virtual ~AgcmUIGuildWarehouse();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();

		//	Operations
	  #ifdef _DEBUG
		void	TestOpen();
	  #endif
		BOOL	Open();
		BOOL	SetGrid(AgpdCharacter *pAgpdCharacter);
		BOOL	OnItemMovedIn(AgpdItem *pAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);
		BOOL	OnItemMovedOut(AgpdItem *pAgpdItem, INT16 nStatus, INT16 nLayer, INT16 nRow, INT16 nColumn);
		
		//	Module Callbacks
		static BOOL CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBBaseInfo(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBItemMovedToInventory(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBAddItemToGuildWarehouse(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBRemoveItemFromGuildWarehouse(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOpenResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBExpandResult(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Event Callback
		static BOOL	CBGrant(PVOID pData, PVOID pClass, PVOID pCustData);

		//	System driven UI callback
		static BOOL CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Function Callbacks
		static BOOL CBMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickLayer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickMoneyIn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickMoneyOut(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickMoneyConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL CBClickExpansion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		//	Display Callbacks
		static BOOL CBDisplayCaption(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplayMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
		static BOOL CBDisplayMoneyConfirm(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

		//	Boolean Callbacks
		static BOOL CBIsActiveMoneyInButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
		static BOOL CBIsActiveMoneyOutButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
		static BOOL CBIsActiveExpandSlot(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
		static BOOL CBIsActiveSlot1(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
		static BOOL CBIsActiveSlot2(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
		static BOOL CBIsActiveSlot3(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
		
		//	Packet
		BOOL	SendOpen(AgpdCharacter *pAgpdCharacter);
		BOOL	SendClose(AgpdCharacter *pAgpdCharacter);
		BOOL	SendItemIn(AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn);
		BOOL	SendItemOut(AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem, INT32 lStauts, INT32 lLayer, INT32 lRow, INT32 lColumn);
		BOOL	SendMoneyChange(AgpdCharacter *pAgpdCharacter, INT64 llMoney, BOOL bIn);
		BOOL	SendExpandSlot(AgpdCharacter *pAgpdCharacter);
	};


#endif