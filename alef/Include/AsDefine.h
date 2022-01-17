/******************************************************************************
Module:  AsDefine.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 17
******************************************************************************/

#if !defined(__ASDEFINE_H__)
#define __ASDEFINE_H__

#include "ApBase.h"
#include "ApMutualEx.h"
#include "ApPacket.h"
#include "ApMemory.h"
#include "ApModule.h"

#include "AsDebugOutput.h"

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define	__DB3_VERSION__

//	stCOMMAND structure - command data(packet) unit
///////////////////////////////////////////////////////////////////////////////

typedef struct _stCOMMAND : public ApMemory<_stCOMMAND, 20> 
{
	//BOOL			bServer;
	UINT16			unType;
	UINT32			dpnid;
	PVOID			pvObject;		// bServer : TRUE -> pvCharacter, FALSE -> pvServer
	UINT16			unDataSize;
	CHAR			szData[APPACKET_MAX_PACKET_SIZE];
	DispatchArg		stCheckArg;

} stCOMMAND, *pstCOMMAND;


/*
//	stCOMMANDSVR structure - command data(packet) unit to server
///////////////////////////////////////////////////////////////////////////////
typedef struct _stCOMMANDSVR {
	UINT16		unType;
	UINT32		dpnid;
	PVOID		pvServer;
	UINT16		unDataSize;
	CHAR		szData[APPACKET_MAX_SVR_PACKET_SIZE];
} stCOMMANDSVR, *pstCOMMANDSVR;
*/
//	stCMDCONTEXT structure
///////////////////////////////////////////////////////////////////////////////
typedef struct _stCMDCONTEXT {
	HANDLE		hEvent;
	INT16		nType;
	stCOMMAND	stCommand;
} stCMDCONTEXT, *pstCMDCONTEXT;

//	stTHREADOBJECT structure
///////////////////////////////////////////////////////////////////////////////
typedef struct _stTHREADOBJECT {
	HANDLE		hThread;
	UINT		ulThreadID;
	PVOID		pThreadClass;
} stTHREADOBJECT, *pstTHREADOBJECT;


/*
//	hit history structure
///////////////////////////////////////////////////////////////////////////////
typedef struct _stHITHISTORY {
	INT16	nCharType;			// ID Type (Party or PC or NPC)
	INT32	ID;					// PartyID or CID
	INT16	nPoint;
	UINT32	lFirstHitTime;
	UINT32	lLastHitTime;
	BOOL	bFirstHit;
} stHITHISTORY, *pstHITHISTORY;

typedef enum _eAS_CHARTYPE {
	AS_CHARTYPE_PC,
	AS_CHARTYPE_NPC,
	AS_CHARTYPE_PARTY
} eAS_CHARTYPE;
*/


//	global definition
///////////////////////////////////////////////////////////////////////////////
typedef enum _eSERVERSTATUS {
	GF_SERVER_MODULE_START				= 0,
	GF_SERVER_ENGINE_START				= 1,
	GF_SERVER_START						= 2,
	GF_SERVER_PAUSE						= 3,
	GF_SERVER_STOP						= 4,
	GF_SERVER_FULL						= 5,
	GF_SERVER_LOST_CONNECTION			= 6,
	GF_SERVER_SETUP_CONNECTION			= 7,
	GF_SERVER_COMPLETE_CONNECTION		= 8
} eSERVERSTATUS;

const int AS_VIEW_HALFSIZE					= 20;			// 시야의 절반 (타일 단위)


//	AsDP8 module
///////////////////////////////////////////////////////////////////////////////
const int ASDP_PORT							= 9998;			// 서버쪽 리스닝 포트

const int ASDP_CK_CREATE_PLAYER				= 1;
const int ASDP_CK_DELETE_PLAYER				= 2;
const int ASDP_CK_RECEIVE_DATA				= 3;

#define ASDP_CONNECT_TIMEOUT		1000	// milisecond
#define ASDP_CONNECT_RETRIES		2
#define	ASDP_TIMEOUT_KEEPALIVE		3000	// milisecond

#define ASDP_INVALID_DPNID			0		// invalid dpnid (사실 invalid dpnid가 0이란 보장은 없다. ㅡ.ㅡ 나쁜 엠에쑤...)


//	AgsGameProcess
///////////////////////////////////////////////////////////////////////////////
const int AGSGP_PROCESS_CYCYE_TIME			= 1000;			// 1초
const int AGSGP_MAX_PROCESS_CLIENT_PER_THR	= 200;			// thread 하나당 처리할 수 있는 클라이언트 최대수

const int AGSGP_RECV_CMD_QUEUE_SIZE			= 2000;
const int AGSGP_SERVER_RECV_CMD_QUEUE_SIZE	= 10000;


//	AgsManageProcess
///////////////////////////////////////////////////////////////////////////////
const int AGSMP_POOL_GAMEPROC				= 30;
const int AGSMP_POOL_NOTIPROC				= 5;


//	AgsNotiProcess
///////////////////////////////////////////////////////////////////////////////
const int AGSNP_CMDTYPE_SEND_CLIENT			= 1;
const int AGSNP_CMDTYPE_SEND_CLIENT_VIEW	= 2;
const int AGSNP_CMDTYPE_SEND_CLIENT_PARTY	= 3;
const int AGSNP_CMDTYPE_SEND_CLIENT_WHOLE	= 4;
const int AGSNP_CMDTYPE_SEND_SERVER			= 5;
const int AGSNP_CMDTYPE_SEND_SERVERS		= 6;

const int AGSNP_SEND_CMD_QUEUE_SIZE			= 20;
const int AGSNP_SENDSVR_CMD_QUEUE_SIZE		= 30;


//	AgsmCharacter
///////////////////////////////////////////////////////////////////////////////
const int AGSMCHAR_DEAD_DURATION_TIME		= 3;

enum eAgsmCharacter_ID			// Callback ID in AgsmCharacter Module 
{
	AGSMCHARACTER_CB_ID_HP			= 0,
	AGSMCHARACTER_CB_ID_POS			= 1,
	AGSMCHARACTER_CB_ID_MP			= 2,
	AGSMCHARACTER_CB_ID_SP			= 3,
	AGSMCHARACTER_CB_ID_EFFECT		= 4
};


//	AgsmAOIFilter
///////////////////////////////////////////////////////////////////////////////
const int AGSMAOI_MAX_CHAR_IN_CELL			= 100;
const int AGSMAOI_MAX_NPC_IN_CELL			= 40;
const int AGSMAOI_MAX_ITEM_IN_CELL			= 100;

const int AGSMAOI_FLAG_INBOUND				= 1;
const int AGSMAOI_FLAG_BOUNDARY				= 2;
const int AGSMAOI_FLAG_OUTBOUND				= 3;

const int AGSMAOI_STEP_PRESTART_ZONING			= 11;
const int AGSMAOI_STEP_START_ZONING				= 12;
const int AGSMAOI_STEP_RECV_CHARINFO			= 13;
const int AGSMAOI_STEP_SEND_CHARINFO			= 14;
const int AGSMAOI_STEP_PREPASS_CONTROL			= 15;
const int AGSMAOI_STEP_PASS_CONTROL				= 16;
const int AGSMAOI_STEP_NOT_ZONING				= 0;

enum eAgsmAOIFilter_ID			// Callback ID in AgsmAOIFilter Module 
{
	AGSMAOI_CB_ID_START_ZONING		= 0,
	AGSMAOI_CB_ID_END_ZONING		= 1
};


//	AgsmCombat
///////////////////////////////////////////////////////////////////////////////


//	AgsmTrade
///////////////////////////////////////////////////////////////////////////////
const int AGSMTRADE_MAX_INVENTORY_ROW		= 20;
const int AGSMTRADE_MAX_INVENTORY_COLUMN	= 8;
const int AGSMTRADE_MAX_ITEM				= 20;


//	AgsmChat
///////////////////////////////////////////////////////////////////////////////
const int AGSMCHAT_MAX_MSG_SIZE				= 128;

//	AgsmServer
///////////////////////////////////////////////////////////////////////////////
const int AGSM_MAX_SERVER_NAME				= 64;

/*
const int AGSM_TYPE_GAME_SERVER				= 0;
const int AGSM_TYPE_MASTER_SERVER			= 1;
const int AGSM_TYPE_BACKUP_SERVER			= 2;
const int AGSM_TYPE_LOGIN_SERVER			= 3;
const int AGSM_TYPE_CHARACTER_SERVER		= 4;
const int AGSM_TYPE_ITEM_SERVER				= 5;
const int AGSM_TYPE_DEAL_SERVER				= 6;
const int AGSM_TYPE_RECRUIT_SERVER			= 7;
*/


//	AgsmCharacterManagement
///////////////////////////////////////////////////////////////////////////////
const int AGSMCHARMM_MAX_USER_DATA_COUNT	= 500;
// 한 계정당 만들수 있는 최대 캐릭터
const int AGSMCHARMM_MAX_CREATE_CHAR		= 5;

#endif //__ASDEFINE_H__