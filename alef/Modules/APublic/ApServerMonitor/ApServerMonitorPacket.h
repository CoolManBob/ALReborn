#ifndef _AP_SERVER_MONITOR_PACKET_H_
#define _AP_SERVER_MONITOR_PACKET_H_

#include "AuPacket.h"
#include "ApPacket.h"
#include "ApServerMonitor.h"

const INT32 MAX_SERVER_NAME_LENGTH		  = 32;
const INT32 MAX_CHARACTER_NAME_LENGTH	  = 32;
const INT32 MAX_RACE_NAME_LENGTH		  = 32;

enum eAssmServerControlType
{
	ASSM_SERVER_CONTROL_TYPE_START					= 1,
	ASSM_SERVER_CONTROL_TYPE_STOP					= 2,
	ASSM_SERVER_CONTROL_TYPE_GET_STATUS				= 3,
	ASSM_SERVER_CONTROL_TYPE_PATCH					= 4,
	ASSM_SERVER_CONTROL_TYPE_SET_CONN_TYPE			= 5,
	ASSM_SERVER_CONTROL_TYPE_RESET_DATA				= 6,
	ASSM_SERVER_CONTROL_TYPE_DRAW_DATA				= 7,
	ASSM_SERVER_CONTROL_TYPE_ERR_DISCONNECT			= 8,
	ASSM_SERVER_CONTROL_TYPE_KILL					= 9,
	ASSM_SERVER_CONTROL_TYPE_SET_ACCOUNT_TYPE		= 10,
	ASSM_SERVER_CONTROL_TYPE_RELOAD_SERVER_CONFIG	= 11,
	ASSM_SERVER_CONTROL_TYPE_CRASH					= 12,
	ASSM_SERVER_CONTROL_TYPE_RECONNECT_LOGIN		= 13,
	ASSM_SERVER_CONTROL_TYPE_DISPLAY_NOTICE			= 14,
	ASSM_SERVER_CONTROL_TYPE_ADMIN_COMMAND			= 15,
	ASSM_SERVER_CONTROL_TYPE_NOTICE_LIST			= 16,
	ASSM_SERVER_CONTROL_TYPE_SIEGE_INFO				= 17,
	ASSM_SERVER_CONTROL_TYPE_GAME_INFO				= 18,
};

enum eAssmPacketType
{
	ASSM_PACKET_SERVER_INFO		= 1,
	ASSM_PACKET_SERVER_CONTROL	= 2,
	ASSM_PACKET_GAME_INFO		= 3,
};

class ApServerMonitorPacket
{
public:
	ApServerMonitorPacket();

	PVOID MakeServerInfoPacket(AssmServerData *pServerData, INT16 *pnPacketLenth);
	BOOL GetServerInfoPacket(PVOID pvPacket, INT16 nPacketLength, AssmServerData *pServerData);

	PVOID MakeServerControlPacket(AssmControlData *pControlData, INT16 *pnPacketLenth);
	BOOL GetServerContolPacket(PVOID pvPacket, INT16 nPacketLength, AssmControlData *pControlData);

public:
	AuPacket m_csApsmServerInfoPacket;
	AuPacket m_csApsmServerControlPacket;
};

//////////////////////////////////////////////////////////////////////////
// Server's Game Info Packet Define
enum _eAgsmServerStatusPacketType
{
	AGSM_GAME_INFO_BATTLEGROUND_PACKET = 1,
};

struct PACKET_ASSM_GAME_INFO : public PACKET_HEADER
{
	INT8	pcOperation;

	PACKET_ASSM_GAME_INFO()
		: pcOperation(0)
	{
		cType			= ASSM_PACKET_GAME_INFO;
		unPacketLength	= (UINT16)sizeof(PACKET_ASSM_GAME_INFO);
	}
};

struct PACKET_ASSM_BATTLEGROUND_INFO : public PACKET_ASSM_GAME_INFO
{
	CHAR ServerName[MAX_SERVER_NAME_LENGTH];
	CHAR KillCharacterName[MAX_CHARACTER_NAME_LENGTH];
	CHAR KillRace[MAX_RACE_NAME_LENGTH];
	CHAR KilledCharacterName[MAX_CHARACTER_NAME_LENGTH];
	CHAR KilledRace[MAX_RACE_NAME_LENGTH];


	PACKET_ASSM_BATTLEGROUND_INFO(CHAR* _ServerName, CHAR* _KillCharacterName, CHAR* _KillRace, CHAR* _KilledCharacter, CHAR* _KilledRace)
	{
		pcOperation		= AGSM_GAME_INFO_BATTLEGROUND_PACKET;
		unPacketLength	= (UINT16)sizeof(PACKET_ASSM_BATTLEGROUND_INFO);

		ZeroMemory(ServerName, sizeof(ServerName));
		ZeroMemory(KillCharacterName, sizeof(KillCharacterName));
		ZeroMemory(KillRace, sizeof(KillRace));
		ZeroMemory(KilledCharacterName, sizeof(KilledCharacterName));
		ZeroMemory(KilledRace, sizeof(KilledRace));
		strcpy(ServerName, _ServerName);
		strcpy(KillCharacterName, _KillCharacterName);
		strcpy(KillRace, _KillRace);
		strcpy(KilledCharacterName, _KilledCharacter);
		strcpy(KilledRace, _KilledRace);	
	}
};
///////////////////////////////////////////////////////////////////////////////

#endif // _AP_SERVER_MONITOR_PACKET_H_
