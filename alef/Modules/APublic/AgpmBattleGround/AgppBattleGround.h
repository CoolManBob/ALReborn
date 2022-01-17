#ifndef AGPPBATTLEGROUND_H
#define AGPPBATTLEGROUND_H

//////////////////////////////////////////////////////////////////////////
//
#define MAX_NOTICE_LEN 1024

enum eAgpmBattleGroundNoticeType
{
	AGPM_BATTLEGROUND_NOTICE_BEFORE = 1,
	AGPM_BATTLEGROUND_NOTICE_ING,
	AGPM_BATTLEGROUND_NOTICE_AFTER,
	AGPM_BATTLEGROUND_NOTICE_END,
	AGPM_BATTLEGROUND_NOTICE_EPICZONE,
};

//////////////////////////////////////////////////////////////////////////
// BattleGround Packet Define
enum _eAgpmBattleGroundPacketType
{
	AGPM_BATTLEGROUND_PACKET_NOTICE = 1,
};

struct PACKET_BATTLEGROUND : public PACKET_HEADER
{
	INT8	pcOperation;
	
	PACKET_BATTLEGROUND()
		: pcOperation(0)
	{
		cType			= AGPM_BATTLEGROUND_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_BATTLEGROUND);
	}
};

struct PACKET_BATTLEGROUND_NOTICE : public PACKET_BATTLEGROUND
{
	CHAR strNotice[MAX_NOTICE_LEN];
	DWORD dwColor;
	INT32 lNoticeType;
	BOOL bEvent;

	PACKET_BATTLEGROUND_NOTICE(CHAR* Notice, DWORD Color, INT32 NoticeType, BOOL Event)
	{
		pcOperation		= AGPM_BATTLEGROUND_PACKET_NOTICE;
		unPacketLength	= (UINT16)sizeof(PACKET_BATTLEGROUND_NOTICE);

		memset(strNotice, 0, sizeof(strNotice));
		strcpy(strNotice, Notice);
		dwColor = Color;
		lNoticeType = NoticeType;
		bEvent = Event;
	}
};

#endif // AGPPBATTLEGROUND_H