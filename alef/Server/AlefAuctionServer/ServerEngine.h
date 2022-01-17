#ifndef	__ALEF_AUCTION_SERVER_H__
#define __ALEF_AUCTION_SERVER_H__

#include "AsCommonLib.h"
#include "AuOLEDB.h"
#include "AgpmFactors.h"
#include "AgpmItem.h"
#include "AgsEngine.h"
#include "AgsmServerManager.h"
#include "AgsmAuctionServer.h"
#include "AgsmAuctionClient.h"
#include "AgsmAuctionServerDB.h"

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