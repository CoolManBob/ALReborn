/******************************************************************************
Module:  AgsmItemManager.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 10. 25
******************************************************************************/

#if !defined(__AGSMITEMMANAGER_H__)
#define __AGSMITEMMANAGER_H__

#include "ApBase.h"

#include "AuGenerateID.h"

#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgpmDropItem2.h"
#include "AgpmSystemMessage.h"

#include "AgsEngine.h"
#include "AgsmFactors.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmItem.h"
#include "AgsmItemConvert.h"

#include "AgpmGrid.h"


typedef enum _AgsmItemManagerCB {
	AGSMITEMMANAGER_CB_INSERT_ITEM_TO_DB		= 0,
	AGSMITEMMANAGER_CB_REMOVE_ITEM_FROM_DB,
} AgsmItemManagerCB;

typedef enum _eAgsmItemManager {
	AGSMITEMMANAGER_DATA_TYPE_ITEM	= 0
} eAgsmItemManager;

typedef map<UINT64, AgpdSealData> AgsSealData;

class AgsmInterServerLink;
//class AgsmServerManager;
class AgsmEventSystem;

class AgsmItemManager : public AgsModule {
private:
	AgpmGrid			*m_pagpmGrid;

	AgpmCharacter		*m_pagpmCharacter;
	AgpmItem			*m_pagpmItem;
	AgpmItemConvert		*m_pagpmItemConvert;
	AgpmFactors			*m_pagpmFactors;
	AgpmDropItem2		*m_pagpmDropItem2;
	AgpmSystemMessage	*m_pagpmSystemMessage;

	AgsmFactors			*m_pagsmFactors;
	AgsmCharacter		*m_pagsmCharacter;
	AgsmCharManager		*m_pagsmCharManager;
	AgsmItem			*m_pagsmItem;
	AgsmItemConvert		*m_pagsmItemConvert;
	AgsmItemManager		*m_pagsmItemManager;
	AgsmInterServerLink*	m_pagsmInterServerLink;
	AgsmServerManager2*		m_pagsmServerManager;
	AgsmEventSystem*		m_pagsmEventSystem;

	AuGenerateID		m_csGenerateID;
	AuGenerateID64		m_csGenerateID64;
	AuGenerateID64		m_csGenerateID64CashItemBuy;

	AgsSealData			m_SealingData;
	ApMutualEx			m_SealDataMutex;

private:
	CHAR				m_Dummy2[8];		// 8Byte의 더미 바이트 생성... ㅡㅡ;	
	INT32				m_lTemplateIDSkull;
	CHAR				m_Dummy[8];		// 8Byte의 더미 바이트 생성... ㅡㅡ;

public:
	BOOL		OnAddModule();
	BOOL		OnInit();

	AgsmItemManager();
	~AgsmItemManager();

	BOOL		InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag, INT32 lRemoveIDQueueSize = 0);
	BOOL		InitItemDBIDServer(INT32 ServerIndex);

	static BOOL	CBCreateCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUnLoadCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBCreateNewItem(PVOID pData, PVOID pClass, PVOID pCustData);	// 아이템 생성 kelovon 20051123

	static BOOL CBRemoveItemID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBDeleteItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBGetNewIID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGetNewDBID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBGetNewCashItemBuyID(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBSplitItem(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdItem*	CreateItem(INT32 lTID, INT32 lCID, INT32 lItemCount, BOOL bGenerateDBID = TRUE, INT32 lSkillTID = 0, BOOL bOptionAdd = TRUE);
	AgpdItem*	CreateItem(INT32 lTID, AgpdCharacter *pcsCharacter = NULL, INT32 lItemCount = 0, BOOL bGenerateDBID = TRUE, INT32 lSkillTID = 0, BOOL bOptionAdd = TRUE);
	AgpdItem*	CreateItem(INT32 lTID, INT32 lItemCount, CHAR *pszPosition, INT16 nStatus, CHAR *pszConvert,
						   INT32 lDurability, INT32 lMaxDurability, INT32 lStatusFlag, CHAR *pszOption, CHAR *pszSkillPlus,
						   INT16 nInUse, INT32 lUseCount, INT64 lRemainTime, UINT32 lExpireTime, INT64 llStaminaRemainTime,
						   UINT64 ullDBID, AgpdCharacter *pcsCharacter);
	AgpdItem*	CreateItemSkull();
	AgpdItem*	CreateItemWithOutOption(INT32 lTID, AgpdCharacter *pcsCharacter = NULL, INT32 lItemCount = 0, BOOL bGenerateDBID = TRUE, INT32 lSkillTID = 0);

	UINT64		GetDBID();
	UINT64		GetCashItemBuyID();

	AgpdItem*	GetSelectResult5(AgpdCharacter *pAgpdCharacter, AuDatabase2 *pDatabase);
	AgpdItem*	GetSelectBankResult5(AgpdCharacter *pAgpdCharacterCharacter, AuDatabase2 *pDatabase);

	INT32		Remove1857Rune(CHAR *pszConvertHistory, INT32 lLength);

	static BOOL CBRelayConnect(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGetItemExtraData(PVOID pData, PVOID pClass, PVOID pCustData);
	BOOL AddSealingData(AgsdSealData* pcsSealData);
	BOOL RemoveSealingData(UINT64 DBID);
	AgpdSealData* GetSealingData(UINT64 DBID);
};

#endif //__AGSMITEMMANAGER_H__

