/******************************************************************************
Module:  AgsmServerManager.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 12. 10
******************************************************************************/

#if !defined(__AGSMSERVERMANAGER_H__)
#define __AGSMSERVERMANAGER_H__

#include "ApBase.h"
#include "AsDefine.h"

#include "ApMutualEx.h"

#include "AuPacket.h"

//#include "ApmMap.h"

#include "AgsEngine.h"

#include "AgsdServer.h"
#include "AgsaServer.h"

#include "AgsmAOIFilter.h"
#include "AgsmDBStream.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgsmServerManagerD" )
#else
#pragma comment ( lib , "AgsmServerManager" )
#endif


const int AGSMSERVER_MAX_NUM_SERVER					= 20;

const int AGSMSERVER_MAX_RECONNECT					= 50;

const int AGSMSERVER_MAX_WAIT_CONNECT_RESULT_CLOCK	= 7000;		// milisecond

//const int AGSMSERVER_PACKET_TYPE			= 0x1A;

typedef enum _eAGSMSERVER_DATA_TYPE {
	AGSMSERVER_DATA_TYPE_SERVER		= 0,
	AGSMSERVER_DATA_TYPE_TEMPLATE
} eAGSMSERVER_DATA_TYPE;


typedef enum _eAGSMSERVER_PACKET_OPERATION {
	AGSMSERVER_PACKET_OPERATION_CONNECT			= 0,
	AGSMSERVER_PACKET_OPERATION_CONNECT_RESULT,
	AGSMSERVER_PACKET_OPERATION_DISCONNECT,
	AGSMSERVER_PACKET_OPERATION_SERVER_DOWN,
	AGSMSERVER_PACKET_OPERATION_SET_SERVER_FLAG,
} eAgsmServerPacketOperation;


typedef enum eAgsmServerConnectType {
	AGSMSERVER_CONNECT_TYPE_REQUEST,
	AGSMSERVER_CONNECT_TYPE_ACCEPT
} eAgsmServerConnectType;


typedef enum _eAgsmServerCB {
	AGSMSERVER_CB_ADD_SERVER				= 0,
	AGSMSERVER_CB_RECONNECT_SERVER,
	AGSMSERVER_CB_UPDATE_SERVER_STATUS,
} eAgsmServerCB;


#define AGSMSERVER_INI_NAME_GROUP_NAME					"GroupName"
#define	AGSMSERVER_INI_NAME_SERVER_TYPE					"ServerType"
#define	AGSMSERVER_INI_NAME_SERVER_NAME					"ServerName"
#define AGSMSERVER_INI_NAME_SERVER_ADDRESS				"ServerAddress"
#define	AGSMSERVER_INI_NAME_CONTROL_AREA				"ControlArea"
#define AGSMSERVER_INI_NAME_IS_CONNECT_LOGIN			"IsConnectLoginServer"
#define AGSMSERVER_INI_NAME_ID_SERVER_FLAG				"IDServerFlag"
#define AGSMSERVER_INI_NAME_ID_FLAG_SIZE				"IDFlagSize"
#define AGSMSERVER_INI_NAME_ITEM_DBID_SERVER_FLAG		"ItemDBIDServerFlag"
#define AGSMSERVER_INI_NAME_ITEM_DBID_FLAG_SIZE			"ItemDBIDFlagSize"
#define	AGSMSERVER_INI_NAME_SKILL_DBID_SERVER_FLAG		"SkillDBIDServerFlag"
#define	AGSMSERVER_INI_NAME_SKILL_DBID_FLAG_SIZE		"SkillDBIDFlagSize"


//	AgsmServer class
///////////////////////////////////////////////////////////////////////////////
class AgsmServerManager : public AgsModule {
private:
	//ApmMap*				m_papmMap;
	AgsmDBStream		*m_pagsmDBStream;
	AgsmAOIFilter		*m_pagsmAOIFilter;

	AgsdServer			m_csServer[AGSMSERVER_MAX_NUM_SERVER];

	AgsaServer			m_csServerAdmin;
	AgsaServerTemplate	m_csTemplateAdmin;

	AgsdServer*			CreateServer();
	BOOL				DestroyServer(AgsdServer *pcsServer);

	AgsdServerTemplate*	CreateTemplate();
	BOOL				DestroyTemplate(AgsdServerTemplate *pcsTemplate);

	BOOL				IsInArea(AuAREA csArea, AuPOS csPos);

	INT32				m_lServerTID;							// this server tid
	INT32				m_lServerID;							// this server id
	CHAR				m_szGroupName[AGSM_MAX_SERVER_NAME];	// this server group name
	CHAR				m_szServerName[AGSM_MAX_SERVER_NAME];	// this server name

	AgsdServer*			m_pcsLoginServer;
	AgsdServer*			m_pcsDealServer;
	AgsdServer*			m_pcsRecruitServer;
	AgsdServer*			m_pcsRelayServer;

	DPNID				m_dpnidGameServers;

	// 서버간 연결 관련 멤버들
	INT16				m_nTotalConnect;
	INT16				m_nNumAccept;

	BOOL				m_bIsConnect;

	BOOL				Connect(AgsdServer *pcsServer);
	BOOL				Disconnect(AgsdServer *pcsServer, INT16 nConnectType);

	BOOL				ConnectLoginServer();

	BOOL				IsConnectComplete();

	HANDLE				m_hGetItemDBIDEvent;
	HANDLE				m_hGetSkillDBIDEvent;

public:
	AuPacket			m_csPacket;

	AgsmServerManager();
	~AgsmServerManager();

	BOOL OnValid(CHAR* pszData, INT16 nSize);

	BOOL OnAddModule();

	BOOL OnIdle50Times();

	BOOL OnInit();
	BOOL OnDestroy();
	//BOOL OnIdle();

	BOOL SetMaxServer(INT16 nCount);
	BOOL SetMaxTemplate(INT16 nCount);

	BOOL				StreamWriteTemplate(CHAR *szFile);
	BOOL				StreamReadTemplate(CHAR *szFile);

	static BOOL			TemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			TemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	AgsdServer*			AddServer(INT32 lTID, INT16 nServerType);
	AgsdServer*			GetServer(INT32 lServerID);
	AgsdServer*			GetServerName(CHAR *szServerName);
	AgsdServer*			GetServer(CHAR *szAddress);
	BOOL				RemoveServer(INT32 lServerID);

	AgsdServerTemplate*	AddServerTemplate(INT32 lTID);
	AgsdServerTemplate*	GetServerTemplate(INT32 lTID);
	BOOL				RemoveServerTemplate(INT32 lTID);

	AgsdServer*			GetControlServer(AuPOS posDestArea, AgsdControlArea *pstCurrentControlArea = NULL);

	//AgsdServer*			IsNeedZoning(AuPOS posDest);
	//AgsdServer*			IsNeedZoning(ApWorldSector *pSector);

	AgsdServer*			GetLoginServers(INT16 *pnIndex);
	AgsdServer*			GetDealServer();
	AgsdServer*			GetRecruitServer();
	AgsdServer*			GetRelayServer();
	AgsdServer*			GetGameServers(INT16 *pnIndex);
	AgsdServer*			GetGameServers(CHAR *szGroupName, INT16 *pnIndex);

	//게임서버 그룹이름들을 얻어내는 함수이다. 현재는 그냥 존재한다. -_-; 이광준 대리님께서 바꿔주신다고
	//하셨으니 믿고 그냥 쓰자~ 2003/4/28
	char				*GetGameServerGroupName();

	DPNID				GetGameServersDPNID();

	AgsdServer*			GetThisServer();

	UINT8				GetServerType(AgsdServer *pcsServer);
	UINT8				GetThisServerType();

	BOOL				InitServerData();

	// 서버간 연결 관련 함수들
	BOOL				SetupConnection();

	BOOL				DisconnectAllServers();

	BOOL				OnDisconnectSvr(INT32 lServerID);

	// 서버간 데이타 전송 관련 함수들

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);

	//AgsdControlServer*	GetADMap(PVOID pData);

	//static BOOL			ConAgsdControlServer(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CheckConnect(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);

	BOOL				SendConnectPacket(AgsdServer *pcsServer);
	BOOL				SendConnectResultPacket(AgsdServer *pcsServer);

	BOOL				TryConnection();

	BOOL				SetCallbackAddServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReconnectServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackUpdateServerStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SendPacketGameServers(PVOID pvPacket, INT16 nSize);

	BOOL				GetServerFlag(INT16 *pnServerFlag, INT16 *pnFlagSize);
	BOOL				GetItemDBIDServerFlag(INT16 *pnServerFlag, INT16 *pnFlagSize, UINT64 *pullDBID);
	BOOL				GetSkillDBIDServerFlag(INT16 *pnServerFlag, INT16 *pnFlagSize, UINT64 *pullDBID);
	BOOL				GetServerDBIDRange( INT16 nServerFlag, INT16 nServerFlagSize, UINT64 &ullMinRange, UINT64 &ullMaxRange );

	BOOL				GetItemDBID(UINT64 *pllDBID);
	BOOL				GetSkillDBID(UINT64 *pllDBID);

	BOOL				StreamSelectItemDBID();
	BOOL				StreamSelectSkillDBID();

	static BOOL			CBStreamDB(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				ConnectionLostToLoginServer(AgsdServer *pcsServer);
	BOOL				ConnectionLostToGameServer(AgsdServer *pcsServer);
	BOOL				ConnectionLostToRelayServer(AgsdServer *pcsServer);

	BOOL				UpdateServerStatus(AgsdServer *pcsServer, eSERVERSTATUS eNewStatus);
};

#endif //__AGSMSERVERMANAGER_H__
