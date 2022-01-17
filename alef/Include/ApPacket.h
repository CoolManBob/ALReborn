/******************************************************************************
Module:  ApPacket.h
Notices: Copyright (c) 2002 netong
Purpose: 
Last Update: 2002. 04. 02
******************************************************************************/

#if !defined(__APPACKET_H__)
#define __APPACKET_H__

#include "windows.h"
#include "ApDefine.h"

const int APPACKET_MAX_PACKET_TYPE		= 120;
const int APPACKET_MAX_ACTION_TYPE		= 16;

const int APPACKET_MAX_PACKET_SIZE		= 10240 * 4;

const int APPACKET_MAX_DATA_TYPE		= 0x0A;
const int APPACKET_ACTION_DATA			= 0;

#pragma pack(1)

///////////////////////////////////////////////////////////////////////////////
// common header
///////////////////////////////////////////////////////////////////////////////

struct Flags
{
	bool Compressed : 1;		// 압축 여부 확인
	bool Unused1 : 1;
	bool Unused2 : 1;
	bool Unused3 : 1;
	bool Unused4 : 1;
	bool Unused5 : 1;
	bool Unused6 : 1;
	bool Unused7 : 1;
};

typedef struct _stPACKET_HEADER {
	BYTE		bGuardByte;
	UINT16		unPacketLength;
	UINT8		cType;
	Flags		Flag;
	INT32		lOwnerID;
	UINT32		lDummy;
} PACKET_HEADER, *PPACKET_HEADER;

typedef struct _stLK_PACKET_HEADER {
	UINT16		unLen;
	UINT16		unHeaderMsg;
	UINT32		ulSeqID;
} LK_PACKET_HEADER, *PLK_PACKET_HEADER;

typedef struct _stBILLING_PACKET_HEADER {
	char		cReturnCode[2];
	char		cLength[5];
} BILLING_PACKET_HEADER, *PBILLING_PACKET_HEADER;

typedef struct _stENC_PACKET_HEADER		// 헤더 앞 3Byte (GuardByte, Length) 를 PACKET_HEADER 와 같이 만들어서 캐스팅시 편하게 한다.
{
	BYTE		bGuardByte;
	UINT16		unPacketLength;
	UINT32		ulSeqID;
} ENC_PACKET_HEADER, *PENC_PACKET_HEADER;

typedef struct _stPATCH_PACKET_HEADER {
	UINT32		m_iSize;
	BYTE		m_iCommand;
} PATCH_PACKET_HEADER, *PPATCH_PACKET_HEADER;

#pragma pack()

const BYTE APPACKET_FRONT_GUARD_BYTE	= 0xD6;
const BYTE APPACKET_REAR_GUARD_BYTE		= 0x6B;

const BYTE APENCPACKET_FRONT_PUBLIC_BYTE	= 0xA1;
const BYTE APENCPACKET_FRONT_PRIVATE_BYTE	= 0xB1;
const BYTE APENCPACKET_REAR_PUBLIC_BYTE		= 0xAF;
const BYTE APENCPACKET_REAR_PRIVATE_BYTE	= 0xBF;

const int APPACKET_MIN_PACKET_SIZE		= 8;	// byte (	front guardbyte = 1
												//			packet length	= 2
												//			packet type		= 1
												//			owner id		= 4

#define IS_ENC_PACKET(p, size)		\
	((*((BYTE*)p) == APENCPACKET_FRONT_PUBLIC_BYTE	&& *((BYTE*)p + size - 1) == APENCPACKET_REAR_PUBLIC_BYTE) ||		\
	(*((BYTE*)p) == APENCPACKET_FRONT_PRIVATE_BYTE	&& *((BYTE*)p + size - 1) == APENCPACKET_REAR_PRIVATE_BYTE))		\
	? TRUE : FALSE

#define IS_ENC_FRONT_BYTE(p)		\
	(p == APENCPACKET_FRONT_PUBLIC_BYTE || p == APENCPACKET_FRONT_PRIVATE_BYTE)	\
	? TRUE : FALSE

#endif //__APPACKET_H__