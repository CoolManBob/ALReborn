#ifndef	__ALEF_MIGRATE_DB_H__
#define __ALEF_MIGRATE_DB_H__

#include "windows.h"
#include "AsCommonLib.h"
#include "ApmObject.h"
#include "AuOLEDB.h"
#include "AuExcelTxtLib.h"
#include "AgpmGrid.h"
#include "AgpmItem.h"
#include "AgpmCombat.h"
#include "AgpmAdmin.h"
#include "AgpmItemLog.h"
#include "AgpmParty.h"
#include "AgpmEventBinding.h"
#include "AgsmDBStream.h"
#include "AgsmMap.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmItemLog.h"
#include "AgsmCharManager.h"
#include "AgsmSkill.h"
#include "AgsmSkillManager.h"

class ServerEngine : public AgsEngine {
public:
	ServerEngine(GUID guidApp);
	~ServerEngine();

	BOOL	OnRegisterModule();
	BOOL	OnTerminate();

	BOOL	ConnectServers();
	BOOL	ConnectToDB();
	BOOL	GetStartDBID();
	BOOL	DisconnectServers();

	BOOL	SetDBThreadStatus( int iStatus );
	BOOL	WaitDBThreads();

	BOOL	MigrateDB();

	INT32	SelectCharacter(COLEDB *pcOLEDB, CHAR *pstrCharName, INT32 *plSlotIndex);
	BOOL	InsertCharacter(COLEDB *pcOLEDB, INT32 lCID, INT32 lSlotIndex);
};

#endif	//__ALEF_MIGRATE_DB_H__