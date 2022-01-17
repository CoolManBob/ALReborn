/******************************************************************************
Module:  AgsmSkillManager.h
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 26
******************************************************************************/

#if !defined(__AGSMSKILLMANAGER_H__)
#define __AGSMSKILLMANAGER_H__

#include "AgpmFactors.h"
#include "AgpmSkill.h"
#include "AgpmProduct.h"
#include "AgsmSkill.h"
#include "AuGenerateID.h"

//#include "AgsmDBStream.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"

#include "AuDatabase2.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmSkillManagerD" )
#else
#pragma comment ( lib , "AgsmSkillManager" )
#endif
#endif


/*
typedef enum _eAgsmSkillManagerCB {
	AGSMSKILLMANAGER_CB_DB_STREAM_INSERT		= 0,
	AGSMSKILLMANAGER_CB_DB_STREAM_SELECT,
	AGSMSKILLMANAGER_CB_DB_STREAM_UPDATE,
	AGSMSKILLMANAGER_CB_DB_STREAM_DELETE
} eAgsmSkillManagerCB;
*/

#pragma pack (1)
class AgsmSkillManager : public AgsModule {
private:
	AgpmFactors			*m_pagpmFactors;
	AgpmCharacter		*m_pagpmCharacter;
	AgpmSkill			*m_pagpmSkill;
	AgpmProduct			*m_pagpmProduct;

	//AgsmDBStream		*m_pagsmDBStream;
	AgsmCharacter		*m_pagsmCharacter;
	AgsmCharManager		*m_pagsmCharManager;
	AgsmSkill			*m_pagsmSkill;

	AuGenerateID		m_csGenerateID;

public:
	AgsmSkillManager();
	~AgsmSkillManager();

	BOOL				OnAddModule();

	BOOL				OnInit();
	BOOL				OnDestroy();

	// m_csGenerateID, m_csGenerateID64 ¼¼ÆÃ
	BOOL				InitServer(UINT32 ulStartValue, UINT32 ulServerFlag = 0, INT16 nSizeServerFlag = 0, INT32 lRemoveIDQueueSize = 0);

	INT32				GenerateID();

	// manage skill (create/remove/etc...)
	AgpdSkill *			CreateSkill(ApBase *pcsOwnerBase, INT32 lSkillTID);
	BOOL				RemoveSkill(AgpdSkill *pcsSkill);

	//static BOOL			CBDeleteSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBCreateCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBDeleteCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBGetNewSkillID(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBLearnSkill(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBForgetSkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBSetCharLevel(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBRemoveSkillID(PVOID pData, PVOID pClass, PVOID pCustData);

	/*
	BOOL				SetCallbackDBStreamInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackDBStreamDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackDBStreamSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackDBStreamUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	*/

	/*
	// db operation
	BOOL				StreamInsertDB(CHAR *szServerGroup, CHAR *szServerName, UINT64 ulMaxDBID, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamSelectDB(CHAR *szServerGroup, CHAR *szServerName, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamUpdateDB(CHAR *szServerGroup, CHAR *szServerName, UINT64 ulMaxDBID, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamDeleteDB(CHAR *szServerGroup, CHAR *szServerName, ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL			CBStreamDB(PVOID pData, PVOID pClass, PVOID pCustData);

	// Get Query Result;
	BOOL				GetSelectResult2(COLEDB *pcOLEDB, AgpdCharacter *pcsCharacter);
	*/
	BOOL				GetSelectResult5(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter);
};
#pragma pack()

#endif	//__AGSMSKILLMANAGER_H__
