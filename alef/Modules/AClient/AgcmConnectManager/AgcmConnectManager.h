#ifndef			__AGCMCONNECTMANAGER_H__
#define			__AGCMCONNECTMANAGER_H__

#include "ApBase.h"
#include "AuPacket.h"

//#include "AcDP8.h"
#include "AcSocketManager.h"
#include "AgcModule.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmConnectManagerD" )
#else
#pragma comment ( lib , "AgcmConnectManager" )
#endif
#endif

typedef enum AgcmConnectManagerPacketOperation {
	AGCMCONNECTMANAGER_PACKET_OPERATION_SEND_ZONESERVER_ADDR		= 1,
	AGCMCONNECTMANAGER_PACKET_OPERATION_CONNECT_ZONESERVER			= 2,
	AGCMCONNECTMANAGER_PACKET_OPERATION_CONNECT_ZONESERVER_RESULT	= 3,
	AGCMCONNECTMANAGER_PACKET_OPERATION_PASS_CONTROL				= 5
} AgcmConnectManagerPacketOperation;

const int AGCM_TYPE_GAME_SERVER				= 1;
const int AGCM_TYPE_BACKUP_SERVER			= 2;
const int AGCM_TYPE_LOGIN_SERVER			= 3;
const int AGCM_TYPE_CHARACTER_SERVER		= 4;
const int AGCM_TYPE_ITEM_SERVER				= 5;
const int AGCM_TYPE_DEAL_SERVER				= 6;
const int AGCM_TYPE_RECRUIT_SERVER			= 7;

typedef struct _stConnectionArray {
	CHAR	szAddress[23];
	INT32	lServerID;
	INT16	nServerType;
} stConnectionArray, *pstConnectionArray;

const int AGCMCONNECTMANAGER_MAX_CONNECTLIST						= 800;

class AgcmConnectManager : public AgcModule
{
private:
	AuPacket				m_csPacket;

	stConnectionArray		m_stConnectionArray[AGCMCONNECTMANAGER_MAX_CONNECTLIST];

	BOOL					SendZoningMessage(INT32 lCID, INT16 nNID);
	BOOL					SendZoningResult(INT32 lCID, INT8 cStatus, INT16 nNID, INT32 lServerID);

	INT16					nControlServerNID;

public:
	AgcmConnectManager();
	~AgcmConnectManager();

	BOOL					OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	INT16					Connect(CHAR *szAddress, INT16 nServerType = ACDP_SERVER_TYPE_GAMESERVER, PVOID pClass = NULL,
									ApModuleDefaultCallBack fpConnect = NULL, ApModuleDefaultCallBack fpDisconnect = NULL, 
									ApModuleDefaultCallBack fpError = NULL);
	BOOL					Disconnect(INT16 nNID);

	BOOL					DisconnectAll();

	INT32					GetControlServerID();
	INT16					GetControlServerNID();

	INT16					GetDealServerNID();
	INT16					GetRecruitServerNID();
	INT16					GetLoginServerNID();
};

#endif //__AGCMCONNECTMANAGER_H__