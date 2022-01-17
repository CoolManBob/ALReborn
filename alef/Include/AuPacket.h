/******************************************************************************
Module:  AuPacket.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 22
******************************************************************************/

#if !defined(__AUPACKET_H__)
#define __AUPACKET_H__

#include "ApBase.h"
#include "AuType.h"
#include "AuCircularBuffer.h"
#include "AuMiniLZO.h"

#include "AuCryptManager.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AuPacketD" )
#else
#pragma comment ( lib , "AuPacket" )
#endif
#endif

extern AuCircularBuffer g_AuCircularBuffer;
extern AuCircularBuffer g_AuCircularOutBuffer;

class AuPacket {
private:
	AuMiniLZO	m_MiniLZO;
	UINT16		m_cFieldType[64];		// 패킷안에 있는 필드들의 타입
	UINT16		m_cFieldLength[64];		// 패킷안에 있는 필드들의 길이
	UINT16		m_cNumField;
	BOOL		m_bIsSetField;			// SetFieldType()을 실행했는지 여부
	INT16		m_nFlagLength;			// m_dwFlag 길이

public:
	AuPacket();
	~AuPacket();

	//Packet을 세팅한다.
	INT16 SetFieldType(UINT16 cFirst, ...);
	BOOL SetFlagLength(INT16 nLength);

	INT16 GetField(BOOL bIsPacket, PVOID pvPacket, INT16 nPacketLength, PVOID pvFirst, ...);

	//현재 Packet의 Flag를 return 한다. (최대 길이는 unsigned long long 인 8byte이다.)
	//DWORD GetFlag();

	//Packet을 생성한다.
	//INT16 MakeField(PVOID pvFirst, ...);
	PVOID MakePacket(BOOL bIsPacket, INT16 *pnPacketLength, UINT8 cType, ...);
	PVOID MakeStaticPacket(BOOL bIsPacket, INT16 *pnPacketLength, UINT8 cType, ...);

	//패킷을 가져온당
	//PVOID GetPacket();
	//INT16 GetPacketLength();
	INT16 GetFlagLength();

	BOOL FreePacket(PVOID pvPacket, BOOL bEmbeddedPacket = TRUE);
	BOOL FreeStaticPacket(PVOID pvPacket, BOOL bEmbeddedPacket = TRUE);

	BOOL SetCID(PVOID pvPacket, INT16 nPacketLength, INT32 lCID);

	static AuCryptManager& GetCryptManager();
	static PVOID EncryptPacket(PVOID pInput, INT16 nSize, INT16* pnOutputSize, AuCryptActor& csCryptActor, eAuCryptType eCryptType = AUCRYPT_TYPE_PUBLIC);
	static PVOID DecryptPacket(PVOID pInput, INT16 nSize, INT16* pnOutputSize, AuCryptActor& csCryptActor, eAuCryptType eCryptType = AUCRYPT_TYPE_PRIVATE);
	static void FreeEncryptBuffer(PVOID pBuffer, INT16 nSize);

	static BOOL SetDummy(PVOID pvPacket, INT16 nSize);
};

#endif //__AUPACKET_H__
