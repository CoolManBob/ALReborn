#ifndef	__ALEF_RECRUIT_SERVER_H__
#define __ALEF_RECRUIT_SERVER_H__

#include "AsCommonLib.h"
#include "AuOLEDB.h"
#include "AgsEngine.h"
#include "AgsmServerManager.h"
#include "AgsmConnectionSetupGame.h"
#include "AgsmRecruitServer.h"
#include "AgsmRecruitClient.h"
#include "AgsmRecruitServerDB.h"

class ServerEngine : public AgsEngine {
public:
	ServerEngine(GUID guidApp);
	~ServerEngine();

	BOOL	OnRegisterModule();
	BOOL	OnTerminate();

	BOOL	ConnectServers();
	BOOL	DisconnectServers();

	BOOL	SetDBThreadStatus( int iStatus );
	BOOL	WaitDBThreads();
};

#endif