/******************************************************************************
Module:  AgsdServer.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 12. 10
******************************************************************************/

#if !defined(__AGSDSERVER_H__)
#define __AGSDSERVER_H__

#include <dplay8.h>
#include "ApBase.h"

#include "ApBase.h"
#include "AsDefine.h"

#define AGSMSERVER_MAX_CONTROL_AREA			20

typedef enum _eAGSMSERVER_TYPE {
	AGSMSERVER_TYPE_NONE					= 0,
	AGSMSERVER_TYPE_GAME_SERVER,
	AGSMSERVER_TYPE_MASTER_SERVER,
	AGSMSERVER_TYPE_BACKUP_SERVER,
	AGSMSERVER_TYPE_LOGIN_SERVER,
	AGSMSERVER_TYPE_CHARACTER_SERVER,
	AGSMSERVER_TYPE_ITEM_SERVER,
	AGSMSERVER_TYPE_DEAL_SERVER,
	AGSMSERVER_TYPE_RECRUIT_SERVER,
	AGSMSERVER_TYPE_RELAY_SERVER,
	AGSMSERVER_TYPE_MAX
} eAGSMSERVER_TYPE;

class AgsdControlArea {
public:
	AuAREA		m_stControlArea;

	INT16		m_nNumAroundUnit;
	PVOID*		m_pcsAroundUnit[7];		// 만약 이 지역과 인접한 지역이 다른 서버가 관리하는 지역이라면 여기에 그 서버 데이타 포인터를 넣는다.
};

class AgsdServerTemplate : public ApBase {
public:
	INT32		m_lTID;
	CHAR		m_szName[AGSM_MAX_SERVER_NAME];
	CHAR		m_szGroupName[AGSM_MAX_SERVER_NAME];

	// server type
	UINT8		m_cType;	// AGSM_TYPE_GAME_SERVER
							// AGSM_TYPE_MASTER_SERVER
							// AGSM_TYPE_BACKUP_SERVER
							// AGSM_TYPE_LOGIN_SERVER
							// AGSM_TYPE_CHARACTER_SERVER
							// AGSM_TYPE_ITEM_SERVER
							// AGSM_TYPE_DEAL_SERVER
							// AGSM_TYPE_RECRUIT_SERVER

	// server address
	//CHAR		m_szIPv4[16];		// ip address (version 4)
	//UINT32	m_ulPort;			// port number
	CHAR		m_szIPv4Addr[23];	// expected form of "xxx.xxx.xxx.xxx:port"
	//CHAR		m_szIPv6[];			// reserved

	INT16		m_nControlArea;										// 관리하는 지역 갯수
	AgsdControlArea	m_csControlArea[AGSMSERVER_MAX_CONTROL_AREA];	// 게임 서버인경우 관리하는 지역

	BOOL		m_bIsConnectLoginServer;	// 로긴 서버와 연결할 필요가 있는 서버인지를 나타냄

	INT16		m_nServerFlag;				// 각종 아뒤 앞에 붙을 서버 플래그
	INT16		m_nFlagSize;				// 서버 플래그 길이

	UINT64		m_ullItemDBIDStart;
	INT16		m_nItemDBIDServerFlag;		// Item DBID 생성 서버 플래그
	INT16		m_nItemDBIDFlagSize;		// flag size

	UINT64		m_ullSkillDBIDStart;
	INT16		m_nSkillDBIDServerFlag;		// Skill DBID 생성 서버 플래그
	INT16		m_nSkillDBIDFlagSize;		// flag size
};

class AgsdServer : public ApBase {
public:
	// basic information
	INT32		m_lServerID;

	INT32		m_lTID;

	DPNID		m_dpnidServer;	// DPNID of server
	INT16		m_nidServer;	// network_id of server

	INT16		m_nNumReconnect;

	INT16		m_nNumPlayer;	// 현재 처리중인 player 수...

	INT16		m_nStatus;		// 이 서버의 현재 상태. (active인지 아닌지 등등의 값을 갖는다)

	BOOL		m_bIsConnected;
	BOOL		m_bIsAccepted;

	BOOL		m_bIsAuth;		// 연결에 대한 인증을 받았는지 여부 (connect 이후 연패킷을 보내고 그거에 대한 응답을 정상적으로 받으면 TRUE)

	time_t		m_lTime;		// 서버가 시작된 시간.

	ApBase*		m_pcsTemplate;
};

#endif //__AGSDSERVER_H__