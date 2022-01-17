/******************************************************************************
Module:  AuPacket.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 01. 06
******************************************************************************/

#include "AuPacket.h"
#include "magdebug.h"
#include "ApPacket.h"

#include "AuProfileManager.h"

#include "ApMemoryTracker.h"

#define PACKET_BUFFER_SIZE 2048

AuCircularBuffer g_AuCircularBuffer;
AuCircularBuffer g_AuCircularOutBuffer;

UINT	g_ulMinCompressSize					= 80;

/*
UINT64	g_ullTotalMakePacketSize			= 0;	// Packet으로 만들어진 총 사이즈
UINT64	g_ullCompressedCount				= 0;	// 압축을 시도한 총 횟수
UINT64	g_ullCompressedNotEfficientCount	= 0;	// 압축을 했으나 압축하기 전 보다 크기가 더 커진경우의 횟수
UINT64	g_ullCompressedMakePacketSize		= 0;	// 80byte가 넘는 애들의 압축하기전 총합

UINT64	g_ullOriginalPacketSize[2000]		= { 0 , };
UINT16	g_ullCompressedPacketSize[2000]		= { 0 , };
*/


#ifdef _DEBUG
ApCriticalSection	g_MutexPacket;
#endif

AuPacket::AuPacket()
{
	m_bIsSetField		= FALSE;
	m_nFlagLength		= 0;

	ZeroMemory(m_cFieldType, sizeof(UINT16) * 64);
	ZeroMemory(m_cFieldLength, sizeof(UINT16) * 64);

	m_cNumField			= 0;

	/*
	strcpy( ( char * ) m_cFieldType		, "" );
	strcpy( ( char * ) m_cFieldLength	, "" );
	*/
}

AuPacket::~AuPacket()
{
//	if (m_bAllocated)
//		GlobalFree(m_pvData);
}

//		SetFieldType
//	Functions
//		- Parsing을 위해서 모든 Field의 Type을 정해준다. 
//		  GetField(), MakeField(), MakePacket()을 위해서 필수적으로 필요하다.
//			Ex) SetFieldType(AUTYPE_INT16, 1, AUTYPE_CHAR, 10, AUTYPE_UINT32, 1, AUTYPE_PACKET, 1, AUTYPE_END, 0);
//				    ==> 0x01 : INT16
//						0x02 : CHAR[10]
//						0x04 : UINT32
//						0x08 : 또 다른 플래그 (중첩된 플래그다)
//	Arguments
//		- ... : 가변길이 변수의 끝을 나타내기 위해 젤 마지막은 AUTYPE_END를 인자로 넘겨준다.
//	Return value
//		- INT16 : process result
///////////////////////////////////////////////////////////////////////////////
INT16 AuPacket::SetFieldType(UINT16 cFirst, ...)
{
	va_list	ap;

	UINT16	i = cFirst;
	m_cNumField	= 0;

	va_start(ap, cFirst);
	while (i != AUTYPE_END)
	{
		ASSERT(i < AUTYPE_MAX && "AuPacket: SetFieldType 에러. AUTYPE_END 확인 바람");

		m_cFieldType[m_cNumField] = i;

		i = va_arg(ap, UINT16);
		ASSERT(i > 0 && "AuPacket: SetFieldType 에러. 타입 크기가 0이하로 설정됨... ㅡ.ㅡ");
		m_cFieldLength[m_cNumField] = i;

		m_cNumField++;

		i = va_arg(ap, UINT16);
	}
	va_end(ap);

	m_bIsSetField = TRUE;

	return TRUE;
}

INT16 AuPacket::GetField(BOOL bIsPacket, PVOID pvPacketRaw, INT16 nPacketLength, PVOID pvFirst, ...)
{
	if (!m_nFlagLength)
	{
		return FALSE;
	}

	PVOID	pvPacket	= pvPacketRaw;
/*	
	if (bIsPacket && ((PACKET_HEADER *) pvPacket)->Flag.Compressed == true)
	{
		PVOID	pvOutBuffer	= g_AuCircularOutBuffer.Alloc(APPACKET_MAX_PACKET_SIZE);
		if (!pvOutBuffer)
			return FALSE;

		ZeroMemory(pvOutBuffer, APPACKET_MAX_PACKET_SIZE);

		UINT16	unOutBufferSize	= APPACKET_MAX_PACKET_SIZE - sizeof(PACKET_HEADER);

		if (!m_MiniLZO.DeCompress((BYTE *) pvPacket + sizeof(PACKET_HEADER),
								  (UINT16) ((PACKET_HEADER *) pvPacket)->unPacketLength - sizeof(PACKET_HEADER) - 1,
								  (BYTE *) pvOutBuffer + sizeof(PACKET_HEADER),
								  &unOutBufferSize))
			return FALSE;

		CopyMemory(pvOutBuffer, pvPacket, sizeof(PACKET_HEADER));

		pvPacket	= pvOutBuffer;
	}
*/
	va_list ap;

	int	nIndex = 0;
	PVOID	pvBuffer = pvFirst;
	DWORD	dwFlagMask = 0x01;		// bitmask를 세팅한다.
	DWORD	dwFlag;
	PVOID	pvIndex;

	if (bIsPacket)
	{
		switch (m_nFlagLength) {
		case 1:
			dwFlag = ((UINT8 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)))[0];
			break;
		case 2:
			dwFlag = ((UINT16 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)))[0];
			break;
		case 4:
			dwFlag = ((UINT32 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)))[0];
			break;
		}

		pvIndex = (CHAR *) pvPacket + sizeof(PACKET_HEADER) + m_nFlagLength;
	}
	else
	{
		switch (m_nFlagLength) {
		case 1:
			dwFlag = ((UINT8 *) ((CHAR *) pvPacket + sizeof(UINT16)))[0];
			break;
		case 2:
			dwFlag = ((UINT16 *) ((CHAR *) pvPacket + sizeof(UINT16)))[0];
			break;
		case 4:
			dwFlag = ((UINT32 *) ((CHAR *) pvPacket + sizeof(UINT16)))[0];
			break;
		}

		pvIndex = (CHAR *) pvPacket + m_nFlagLength + sizeof(UINT16) /*packet length*/;	// 첫번째 데이타를 가르킨다.
	}
	
	va_start(ap, pvFirst);
	while (nIndex < m_cNumField)
	{
		if ((dwFlagMask & dwFlag))	// Flag의 이 필드가 1로 세팅되어 있으면 데이타를 가져온다.
		{
			if (bIsPacket && pvIndex > ((CHAR *) pvPacketRaw) + nPacketLength)
			{
				// 잘못된 Packet이 들어왔다.
				return FALSE;
			}

			if (m_cFieldType[nIndex] == AUTYPE_PACKET)		// 데이타가 또다른 Flag를 갖는 데이타다.
			{
				if (m_cFieldLength[nIndex] > 1)
				{
					INT8	cArraySize = *(INT8 *) pvIndex;

					PVOID *pvPacketArray = (PVOID *) pvBuffer;

					pvIndex = (CHAR *) pvIndex + sizeof(INT8);
					for (int j = 0; j < cArraySize; ++j)
					{
						UINT16	cLength = *(UINT16 *) pvIndex;

						if (pvPacketArray)
						{
							pvPacketArray[j] = pvIndex;
						}

						pvIndex = (CHAR *) pvIndex + sizeof(UINT16) + cLength;
					}
				}
				else
				{
					// 데이타가 플래그를 포함하고 있는경우 Flag 앞에 있는 길이를 보고 이 데이타의 크기를 구한다.
					UINT16	cLength = *(UINT16 *) pvIndex;

					if (pvBuffer)
						*(PVOID *)pvBuffer = (CHAR *) pvIndex;

					pvIndex = (CHAR *) pvIndex + sizeof(UINT16) + cLength;
				}
			}
			else if (m_cFieldType[nIndex] == AUTYPE_MEMORY_BLOCK)
			{
				UINT16	cLength = *(UINT16 *) pvIndex;

				if (pvBuffer)
					*(PVOID *)pvBuffer = (CHAR *) pvIndex + sizeof(UINT16);

				pvIndex = (CHAR *) pvIndex + sizeof(UINT16) + cLength;

				if (bIsPacket && pvIndex > ((CHAR *) pvPacketRaw) + nPacketLength)
				{
					// 잘못된 Packet이 들어왔다.
					return FALSE;
				}

				pvBuffer = va_arg(ap, PVOID);
				if (pvBuffer)
					*((UINT16 *) pvBuffer) = cLength;
			}
			else if (m_cFieldType[nIndex] == AUTYPE_CHAR)
			{
				// 인자로 넘어온 값에 데이타 포인터를 넘겨준다.
				if (pvBuffer)
					*(CHAR **)pvBuffer = (CHAR *) pvIndex;

				pvIndex = (CHAR *) pvIndex + AuTypeSize[m_cFieldType[nIndex]] * m_cFieldLength[nIndex];

				if (bIsPacket && pvIndex > ((CHAR *) pvPacketRaw) + nPacketLength)
				{
					// 잘못된 Packet이 들어왔다.
					return FALSE;
				}
			}
			else						// 걍 보통의 데이타다.
			{
				if (bIsPacket && (CHAR *) pvIndex + sizeof(CHAR) * AuTypeSize[m_cFieldType[nIndex]] * m_cFieldLength[nIndex] > ((CHAR *) pvPacketRaw) + nPacketLength)
				{
					// 잘못된 Packet이 들어왔다.
					return FALSE;
				}

				// 인자로 넘어온 값에 데이타 포인터를 넘겨준다.
				if (pvBuffer)
					CopyMemory(pvBuffer, pvIndex, sizeof(CHAR) * AuTypeSize[m_cFieldType[nIndex]] * m_cFieldLength[nIndex]);

				pvIndex = (CHAR *) pvIndex + AuTypeSize[m_cFieldType[nIndex]] * m_cFieldLength[nIndex];
			}
		}
		else if (pvBuffer && m_cFieldType[nIndex] == AUTYPE_PACKET)
		{
			*(PVOID *)pvBuffer = NULL;
		}
		else if (m_cFieldType[nIndex] == AUTYPE_MEMORY_BLOCK)
		{
			pvBuffer = va_arg(ap, PVOID);
		}

		pvBuffer = NULL;

		nIndex++;
		dwFlagMask <<= 1;

		pvBuffer = va_arg(ap, PVOID);
	}
	va_end(ap);

	return TRUE;
}

BOOL AuPacket::SetCID(PVOID pvPacket, INT16 nPacketLength, INT32 lCID)
{
	if (!pvPacket ||
		nPacketLength < 7 /* sizeof(UINT16) + sizeof(UINT8) + sizeof(INT32) */ ||
		lCID == 0)
		return FALSE;

	((PPACKET_HEADER) pvPacket)->lOwnerID	= lCID;

	return TRUE;
}

PVOID AuPacket::MakePacket(BOOL bIsPacket, INT16 *pnPacketLength, UINT8 cType, ...)
{
	PROFILE("AuPacket::MakePacket");
	
	if (!m_nFlagLength)
		return NULL;

	//m_Flag.m_ulFlag = 0;
	UINT32	ulFlag = 0;

	// 먼저 크기를 구한다.
	va_list ap;

	UINT16	nLength;
	if (bIsPacket)
		//nLength = sizeof(UINT16)/*packet length*/ + sizeof(UINT8)/*packet type*/ + sizeof(INT32)/*CID*/ + m_nFlagLength;
		nLength = sizeof(PACKET_HEADER) + m_nFlagLength;
	else
		nLength = sizeof(UINT16)/*packet length*/ + m_nFlagLength;

	va_start(ap, cType);

	PVOID	pvBuffer = va_arg(ap, PVOID);
	DWORD	dwMask = 0x01;

	int i = 0, j = 0;
	for (i = 0; i < m_cNumField; i++)
	{
		if (pvBuffer)
		{
			BOOL	bSetFlag = TRUE;

			if (m_cFieldType[i] == AUTYPE_PACKET)
			{
				if (m_cFieldLength[i] > 1)
				{
					PVOID	*pvPacketArray = (PVOID *) pvBuffer;
					for (j = 0; j < m_cFieldLength[i]; ++j)
					{
						if (!pvPacketArray[j])
							break;

						nLength += sizeof(UINT16) + *(UINT16 *) pvPacketArray[j];
					}

					if (j > 0)
					{
						// if packet type is array packet..
						nLength += sizeof(INT8);	// array의 크기를 앞에 저장한다.
					}
					else
					{
						bSetFlag = FALSE;
					}
				}
				else
				{
					// field type이 flag인 경우 flag 앞에 있는 UINT8 크기의 길이를 보고 총 길이를 계산한다.
					nLength += sizeof(UINT16) + *(UINT16 *) pvBuffer;
				}
			}
			else if (m_cFieldType[i] == AUTYPE_MEMORY_BLOCK)
			{
				pvBuffer = va_arg(ap, PVOID);
				if (pvBuffer)
				{
					nLength += sizeof(UINT16) + *(INT16 *) pvBuffer;
				}
			}
			else
			{
				nLength += AuTypeSize[m_cFieldType[i]] * m_cFieldLength[i];
			}

			if (bSetFlag)
				ulFlag |= dwMask;
		}

		pvBuffer = va_arg(ap, PVOID);

		dwMask <<= 1;
	}
	va_end(ap);

	if (!nLength)
		return FALSE;

#ifdef	_DEBUG
	ASSERT(nLength > 0);
	ASSERT(nLength < PACKET_BUFFER_SIZE * 40);
#else
	if (nLength < 0 ||
		nLength >= PACKET_BUFFER_SIZE * 40)
		return FALSE;
#endif	//_DEBUG

	//ASSERT(nLength < PACKET_BUFFER_SIZE);

//	InterlockedIncrement(&g_lMakePacketCount);

	if (bIsPacket)
		//nLength += 2;
		nLength += 1;	// end guard byte

	PVOID pvPacketRaw = g_AuCircularBuffer.Alloc(nLength);		// 패킷 앞뒤로 가드바이트를 붙인다.

	if (!pvPacketRaw)
		return FALSE;

//	PVOID	pvPacket	= NULL;

	if (bIsPacket)
	{
		// set guard byte
		*((BYTE *) pvPacketRaw)					= APPACKET_FRONT_GUARD_BYTE;
		*((BYTE *) pvPacketRaw + nLength - 1)	= APPACKET_REAR_GUARD_BYTE;

//		pvPacket	= (PVOID) ((BYTE *) pvPacketRaw + 1);
	}
//	else
//		pvPacket	= pvPacketRaw;

	PVOID	pvPacket	= pvPacketRaw;

	if (pnPacketLength)
		*pnPacketLength = nLength;

	//*((UINT16 *) pvPacket) = nLength;

	((PACKET_HEADER *) pvPacket)->unPacketLength	= nLength;

	if (bIsPacket)
	{
		//*((INT32 *) ((CHAR *) pvPacket + sizeof(UINT16) + sizeof(UINT8)))	= 0;
		((PACKET_HEADER *) pvPacket)->lOwnerID	=	 0;
		((PACKET_HEADER *) pvPacket)->Flag.Compressed	= 0;
	}

	// 크기도 알아냈으니 이제 실제 패킷을 구성한다.

	CHAR* pIndex;
	if (bIsPacket)
	{
		// 패킷 타입을 세팅한다.
		//*(UINT8 *) ((CHAR *) pvPacket + sizeof(UINT16)) = cType;
		((PACKET_HEADER *) pvPacket)->cType	= cType;

		// Flag 크기만큼 m_pvPacket에 복사한다.
		switch (m_nFlagLength) {
		case 1:
			*(UINT8 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		case 2:
			*(UINT16 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		case 4:
			*(UINT32 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		}

		// 이제 하나씩 데이타를 붙인다.
		pIndex = (CHAR *) pvPacket + sizeof(PACKET_HEADER) + m_nFlagLength;
	}
	else
	{
		// Flag 부터 끝까지의 길이를 세팅한다.
		*(UINT16 *) pvPacket = nLength - sizeof(UINT16);

		// Flag 크기만큼 m_pvPacket에 복사한다.
		switch (m_nFlagLength) {
		case 1:
			*(UINT8 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		case 2:
			*(UINT16 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		case 4:
			*(UINT32 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		}

		// 이제 하나씩 데이타를 붙인다.
		pIndex = (CHAR *) pvPacket + sizeof(UINT16) + m_nFlagLength;
	}

	va_start(ap, cType);

	pvBuffer = va_arg(ap, PVOID);

	for (i = 0; i < m_cNumField; i++)
	{
		if (pvBuffer)
		{
			if (m_cFieldType[i] == AUTYPE_PACKET)
			{
				if (m_cFieldLength[i] > 1)
				{
					INT8	*pcArraySize = (INT8 *) pIndex;

					pIndex += sizeof(INT8);

					PVOID	*pvPacketArray = (PVOID *) pvBuffer;
					for (j = 0; j < m_cFieldLength[i]; ++j)
					{
						if (!pvPacketArray[j])
							break;

						nLength = sizeof(UINT16) + *(UINT16 *) pvPacketArray[j];

						if (nLength > 0)
							CopyMemory(pIndex, pvPacketArray[j], nLength);

						pIndex += nLength;
					}

					if (j > 0)
						*pcArraySize = j;

					nLength = 0;
				}
				else
				{
					// field type이 flag인 경우 flag 앞에 있는 UINT16 크기의 길이를 보고 총 길이를 계산한다.
					nLength = sizeof(UINT16) + *(UINT16 *) pvBuffer;
				}
			}
			else if (m_cFieldType[i] == AUTYPE_MEMORY_BLOCK)
			{
				PVOID pvLength = va_arg(ap, PVOID);
				if (pvLength)
				{
					// 데이타 길이를 먼저 복사해 넣는다.
					CopyMemory(pIndex, pvLength, sizeof(UINT16));
					pIndex += sizeof(UINT16);

					nLength = *(INT16 *) pvLength;
				}
			}
			else
			{
				nLength = AuTypeSize[m_cFieldType[i]] * m_cFieldLength[i];
			}
			
			if (nLength > 0)
				CopyMemory(pIndex, pvBuffer, nLength);

			pIndex += nLength;

			nLength = 0;
		}

		pvBuffer = va_arg(ap, PVOID);
	}
	va_end(ap);


	if (bIsPacket && ((PACKET_HEADER *) pvPacketRaw)->unPacketLength > g_ulMinCompressSize)
	{
		UINT16	unDataLength	= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength - sizeof(PACKET_HEADER) - 1 /* End Guard Byte */;

/*		
		PVOID	pvTempBuffer	= (PVOID *) g_AuCircularOutBuffer.Alloc(APPACKET_MAX_PACKET_SIZE);

		CopyMemory(pvTempBuffer, pvPacketRaw, ((PACKET_HEADER *) pvPacketRaw)->unPacketLength);

		if (!m_MiniLZO.Compress((BYTE *) pvPacketRaw + sizeof(PACKET_HEADER), &unDataLength))
			return pvPacketRaw;

		PVOID	pvTempBuffer2	= (PVOID *) g_AuCircularOutBuffer.Alloc(APPACKET_MAX_PACKET_SIZE);
		UINT16	unDeCompressSize	= APPACKET_MAX_PACKET_SIZE - 10;

		m_MiniLZO.DeCompress((BYTE *) pvPacketRaw + sizeof(PACKET_HEADER), unDataLength, (BYTE *) pvTempBuffer2 + sizeof(PACKET_HEADER), &unDeCompressSize);

		if (memcmp((CHAR *) pvTempBuffer + sizeof(PACKET_HEADER), (CHAR *) pvTempBuffer2 + sizeof(PACKET_HEADER), unDataLength) != 0)
			ASSERT(0);
		
		((PACKET_HEADER *) pvPacketRaw)->unPacketLength	= unDataLength + sizeof(PACKET_HEADER) + 1;

		*((BYTE *) pvPacketRaw + ((PACKET_HEADER *) pvPacketRaw)->unPacketLength - 1)	= APPACKET_REAR_GUARD_BYTE;
		
		*pnPacketLength	= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength;

		((PACKET_HEADER *) pvPacketRaw)->Flag.Compressed	= true;
*/

#ifdef _DEBUG
		g_MutexPacket.Lock();
		
//		g_ullCompressedPacketSize[unOriginalLength / 10]	+= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength;

		g_MutexPacket.Unlock();
#endif
	}

	return pvPacketRaw;
}

PVOID AuPacket::MakeStaticPacket(BOOL bIsPacket, INT16 *pnPacketLength, UINT8 cType, ...)
{
	PROFILE("AuPacket::MakePacket");
	
	if (!m_nFlagLength)
		return NULL;

	//m_Flag.m_ulFlag = 0;
	UINT32	ulFlag = 0;

	// 먼저 크기를 구한다.
	va_list ap;

	UINT16	nLength;
	if (bIsPacket)
		//nLength = sizeof(UINT16)/*packet length*/ + sizeof(UINT8)/*packet type*/ + sizeof(INT32)/*CID*/ + m_nFlagLength;
		nLength = sizeof(PACKET_HEADER) + m_nFlagLength;
	else
		nLength = sizeof(UINT16)/*packet length*/ + m_nFlagLength;

	va_start(ap, cType);

	PVOID	pvBuffer = va_arg(ap, PVOID);
	DWORD	dwMask = 0x01;

	int i = 0, j = 0;
	for (i = 0; i < m_cNumField; i++)
	{
		if (pvBuffer)
		{
			BOOL	bSetFlag = TRUE;

			if (m_cFieldType[i] == AUTYPE_PACKET)
			{
				if (m_cFieldLength[i] > 1)
				{
					PVOID	*pvPacketArray = (PVOID *) pvBuffer;
					for (j = 0; j < m_cFieldLength[i]; ++j)
					{
						if (!pvPacketArray[j])
							break;

						nLength += sizeof(UINT16) + *(UINT16 *) pvPacketArray[j];
					}

					if (j > 0)
					{
						// if packet type is array packet..
						nLength += sizeof(INT8);	// array의 크기를 앞에 저장한다.
					}
					else
					{
						bSetFlag = FALSE;
					}
				}
				else
				{
					// field type이 flag인 경우 flag 앞에 있는 UINT8 크기의 길이를 보고 총 길이를 계산한다.
					nLength += sizeof(UINT16) + *(UINT16 *) pvBuffer;
				}
			}
			else if (m_cFieldType[i] == AUTYPE_MEMORY_BLOCK)
			{
				pvBuffer = va_arg(ap, PVOID);
				if (pvBuffer)
				{
					nLength += sizeof(UINT16) + *(INT16 *) pvBuffer;
				}
			}
			else
			{
				nLength += AuTypeSize[m_cFieldType[i]] * m_cFieldLength[i];
			}

			if (bSetFlag)
				ulFlag |= dwMask;
		}

		pvBuffer = va_arg(ap, PVOID);

		dwMask <<= 1;
	}
	va_end(ap);

	if (!nLength)
		return FALSE;

#ifdef	_DEBUG
	ASSERT(nLength > 0);
	ASSERT(nLength < PACKET_BUFFER_SIZE * 40);
#else
	if (nLength < 0 ||
		nLength >= PACKET_BUFFER_SIZE * 40)
		return FALSE;
#endif	//_DEBUG

	//ASSERT(nLength < PACKET_BUFFER_SIZE);

//	InterlockedIncrement(&g_lMakePacketCount);

	if (bIsPacket)
		//nLength += 2;
		nLength += 1;	// end guard byte

//	PVOID pvPacketRaw = g_AuCircularBuffer.Alloc(nLength);		// 패킷 앞뒤로 가드바이트를 붙인다.
	PVOID pvPacketRaw = (PVOID)new CHAR[nLength];//g_AuCircularBuffer.Alloc(nLength);

	if (!pvPacketRaw)
		return FALSE;

//	PVOID	pvPacket	= NULL;

	if (bIsPacket)
	{
		// set guard byte
		*((BYTE *) pvPacketRaw)					= APPACKET_FRONT_GUARD_BYTE;
		*((BYTE *) pvPacketRaw + nLength - 1)	= APPACKET_REAR_GUARD_BYTE;

//		pvPacket	= (PVOID) ((BYTE *) pvPacketRaw + 1);
	}
//	else
//		pvPacket	= pvPacketRaw;

	PVOID	pvPacket	= pvPacketRaw;

	if (pnPacketLength)
		*pnPacketLength = nLength;

	//*((UINT16 *) pvPacket) = nLength;

	((PACKET_HEADER *) pvPacket)->unPacketLength	= nLength;

	if (bIsPacket)
	{
		//*((INT32 *) ((CHAR *) pvPacket + sizeof(UINT16) + sizeof(UINT8)))	= 0;
		((PACKET_HEADER *) pvPacket)->lOwnerID	=	 0;
		((PACKET_HEADER *) pvPacket)->Flag.Compressed	= 0;
	}

	// 크기도 알아냈으니 이제 실제 패킷을 구성한다.

	CHAR* pIndex;
	if (bIsPacket)
	{
		// 패킷 타입을 세팅한다.
		//*(UINT8 *) ((CHAR *) pvPacket + sizeof(UINT16)) = cType;
		((PACKET_HEADER *) pvPacket)->cType	= cType;

		// Flag 크기만큼 m_pvPacket에 복사한다.
		switch (m_nFlagLength) {
		case 1:
			*(UINT8 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		case 2:
			*(UINT16 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		case 4:
			*(UINT32 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		}

		// 이제 하나씩 데이타를 붙인다.
		pIndex = (CHAR *) pvPacket + sizeof(PACKET_HEADER) + m_nFlagLength;
	}
	else
	{
		// Flag 부터 끝까지의 길이를 세팅한다.
		*(UINT16 *) pvPacket = nLength - sizeof(UINT16);

		// Flag 크기만큼 m_pvPacket에 복사한다.
		switch (m_nFlagLength) {
		case 1:
			*(UINT8 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		case 2:
			*(UINT16 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		case 4:
			*(UINT32 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		}

		// 이제 하나씩 데이타를 붙인다.
		pIndex = (CHAR *) pvPacket + sizeof(UINT16) + m_nFlagLength;
	}

	va_start(ap, cType);

	pvBuffer = va_arg(ap, PVOID);

	for (i = 0; i < m_cNumField; i++)
	{
		if (pvBuffer)
		{
			if (m_cFieldType[i] == AUTYPE_PACKET)
			{
				if (m_cFieldLength[i] > 1)
				{
					INT8	*pcArraySize = (INT8 *) pIndex;

					pIndex += sizeof(INT8);

					PVOID	*pvPacketArray = (PVOID *) pvBuffer;
					for (j = 0; j < m_cFieldLength[i]; ++j)
					{
						if (!pvPacketArray[j])
							break;

						nLength = sizeof(UINT16) + *(UINT16 *) pvPacketArray[j];

						if (nLength > 0)
							CopyMemory(pIndex, pvPacketArray[j], nLength);

						pIndex += nLength;
					}

					if (j > 0)
						*pcArraySize = j;

					nLength = 0;
				}
				else
				{
					// field type이 flag인 경우 flag 앞에 있는 UINT16 크기의 길이를 보고 총 길이를 계산한다.
					nLength = sizeof(UINT16) + *(UINT16 *) pvBuffer;
				}
			}
			else if (m_cFieldType[i] == AUTYPE_MEMORY_BLOCK)
			{
				PVOID pvLength = va_arg(ap, PVOID);
				if (pvLength)
				{
					// 데이타 길이를 먼저 복사해 넣는다.
					CopyMemory(pIndex, pvLength, sizeof(UINT16));
					pIndex += sizeof(UINT16);

					nLength = *(INT16 *) pvLength;
				}
			}
			else
			{
				nLength = AuTypeSize[m_cFieldType[i]] * m_cFieldLength[i];
			}
			
			if (nLength > 0)
				CopyMemory(pIndex, pvBuffer, nLength);

			pIndex += nLength;

			nLength = 0;
		}

		pvBuffer = va_arg(ap, PVOID);
	}
	va_end(ap);

	if (bIsPacket && ((PACKET_HEADER *) pvPacketRaw)->unPacketLength > g_ulMinCompressSize)
	{
		UINT16	unDataLength	= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength - sizeof(PACKET_HEADER) - 1 /* End Guard Byte */;

/*		
		PVOID	pvTempBuffer	= (PVOID *) g_AuCircularOutBuffer.Alloc(APPACKET_MAX_PACKET_SIZE);

		CopyMemory(pvTempBuffer, pvPacketRaw, ((PACKET_HEADER *) pvPacketRaw)->unPacketLength);

		if (!m_MiniLZO.Compress((BYTE *) pvPacketRaw + sizeof(PACKET_HEADER), &unDataLength))
			return pvPacketRaw;

		PVOID	pvTempBuffer2	= (PVOID *) g_AuCircularOutBuffer.Alloc(APPACKET_MAX_PACKET_SIZE);
		UINT16	unDeCompressSize	= APPACKET_MAX_PACKET_SIZE - 10;

		m_MiniLZO.DeCompress((BYTE *) pvPacketRaw + sizeof(PACKET_HEADER), unDataLength, (BYTE *) pvTempBuffer2 + sizeof(PACKET_HEADER), &unDeCompressSize);

		if (memcmp((CHAR *) pvTempBuffer + sizeof(PACKET_HEADER), (CHAR *) pvTempBuffer2 + sizeof(PACKET_HEADER), unDataLength) != 0)
			ASSERT(0);
		
		((PACKET_HEADER *) pvPacketRaw)->unPacketLength	= unDataLength + sizeof(PACKET_HEADER) + 1;

		*((BYTE *) pvPacketRaw + ((PACKET_HEADER *) pvPacketRaw)->unPacketLength - 1)	= APPACKET_REAR_GUARD_BYTE;
		
		*pnPacketLength	= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength;

		((PACKET_HEADER *) pvPacketRaw)->Flag.Compressed	= true;
*/

#ifdef _DEBUG
		g_MutexPacket.Lock();
		
//		g_ullCompressedPacketSize[unOriginalLength / 10]	+= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength;

		g_MutexPacket.Unlock();
#endif
	}

	return pvPacketRaw;
}

BOOL AuPacket::FreePacket(PVOID pvPacket, BOOL bEmbeddedPacket)
{
//	if (!pvPacket)
//		return FALSE;
//
//	InterlockedIncrement(&g_lFreePacketCount);

	return TRUE;
}

BOOL AuPacket::FreeStaticPacket(PVOID pvPacket, BOOL bEmbeddedPacket)
{
	if (!pvPacket)
		return FALSE;

	delete [] (CHAR*)pvPacket;

	return TRUE;
}

BOOL AuPacket::SetFlagLength(INT16 nLength)
{
	m_nFlagLength = nLength;

	return TRUE;
}

INT16 AuPacket::GetFlagLength()
{
	return m_nFlagLength;
}

AuCryptManager& AuPacket::GetCryptManager()
{
	static AuCryptManager csCryptManager;
	return csCryptManager;
}

PVOID AuPacket::EncryptPacket(PVOID pInput, INT16 nSize, INT16* pnOutputSize, AuCryptActor& csCryptActor, eAuCryptType eCryptType)
{
	if(!pInput || nSize < 1 || !pnOutputSize)
		return NULL;

	// 암호화 하지 않는 상황이라면 대입해주고 나간다.
	if(AuPacket::GetCryptManager().IsUseCrypt() == FALSE)
	{
		*pnOutputSize = nSize;
		return pInput;
	}

	if(eCryptType == AUCRYPT_TYPE_NONE)
	{
		*pnOutputSize = nSize;
		return pInput;
	}
	else if(eCryptType == AUCRYPT_TYPE_BOTH)
	{
		// 이렇게 하면 장난친 것임.
		ASSERT(!"AuPacket::EncryptPacket(...) eCryptType is BOTH");
		return NULL;
	}

	// Packet Header 를 얻는다.
	ENC_PACKET_HEADER pHeader = *(ENC_PACKET_HEADER*)pInput;

	// 이미 암호화 되어 있다면 리턴값 대입해주고 나간다.
	//if ((pHeader.bGuardByte == APENCPACKET_FRONT_PUBLIC_BYTE	&& *((BYTE*)pInput + nSize - 1) == APENCPACKET_REAR_PUBLIC_BYTE) ||
	//	(pHeader.bGuardByte == APENCPACKET_FRONT_PRIVATE_BYTE	&& *((BYTE*)pInput + nSize - 1) == APENCPACKET_REAR_PRIVATE_BYTE))
	if(IS_ENC_PACKET(pInput, nSize))
	{
		*pnOutputSize = nSize;
		return pInput;
	}

    // 암호화 후의 크기를 계산한다.
	INT16 nOutputSize = (INT16)AuPacket::GetCryptManager().GetOutputSize(eCryptType, nSize);
	nOutputSize += sizeof(ENC_PACKET_HEADER) + 1;	// Encrypted Packet Header + Encrypted Rear Byte

#ifdef	_DEBUG
	ASSERT(nOutputSize > 0);
	ASSERT(nOutputSize < APPACKET_MAX_PACKET_SIZE);
#else
	if(nOutputSize < 0 || nOutputSize >= APPACKET_MAX_PACKET_SIZE)
		return NULL;
#endif//_DEBUG

	*pnOutputSize = nOutputSize;

	// 메모리를 할당한다.
	PVOID pOutput = g_AuCircularBuffer.Alloc(nOutputSize);

	// Dummy 세팅 2008.04.01. steeple
	SetDummy(pInput, nSize);

	// 암호화한다.
	if(AuPacket::GetCryptManager().Encrypt(csCryptActor, eCryptType, (BYTE*)pInput, (BYTE*)pOutput + sizeof(ENC_PACKET_HEADER), nSize) < 1)
	{
		FreeEncryptBuffer(pOutput, nOutputSize);
		ASSERT(!"AuPacket::EncryptPacket(...) m_csCryptManager.Encrypt returned failiure");
		return NULL;
	}

	// 암호화한 패킷 헤더를 세팅한다.
	((ENC_PACKET_HEADER*)pOutput)->unPacketLength = nOutputSize;
	//((ENC_PACKET_HEADER*)pOutput)->ulSeqID = ++ulSendSeqID;
	if(eCryptType == AUCRYPT_TYPE_PUBLIC)
	{
		((ENC_PACKET_HEADER*)pOutput)->bGuardByte = APENCPACKET_FRONT_PUBLIC_BYTE;
		*((BYTE*)pOutput + nOutputSize - 1) = APENCPACKET_REAR_PUBLIC_BYTE;
	}
	else if(eCryptType == AUCRYPT_TYPE_PRIVATE)
	{
		((ENC_PACKET_HEADER*)pOutput)->bGuardByte = APENCPACKET_FRONT_PRIVATE_BYTE;
		*((BYTE*)pOutput + nOutputSize - 1) = APENCPACKET_REAR_PRIVATE_BYTE;
	}

	return pOutput;
}

// 2006.04.12. steeple
PVOID AuPacket::DecryptPacket(PVOID pInput, INT16 nSize, INT16* pnOutputSize, AuCryptActor& csCryptActor, eAuCryptType eCryptType)
{
	if(!pInput || nSize < 1 || !pnOutputSize)
		return NULL;

	// 암호화 하지 않는 상황이라면 대입해주고 나간다.
	if(AuPacket::GetCryptManager().IsUseCrypt() == FALSE)
	{
		*pnOutputSize = nSize;
		return pInput;
	}

	if(eCryptType == AUCRYPT_TYPE_NONE)
	{
		*pnOutputSize = nSize;
		return pInput;
	}
	else if(eCryptType == AUCRYPT_TYPE_BOTH)
	{
		// 이렇게 하면 장난친 것임.
		ASSERT(!"AuPacket::DecryptPacket(...) eCryptType is BOTH");
		return NULL;
	}

	// Packet Header 를 얻는다.
	ENC_PACKET_HEADER pHeader = *(ENC_PACKET_HEADER*)pInput;

	// 암호화 되어 있는 상태가 아니라면 리턴값 대입해주고 나간다.
	if(!IS_ENC_PACKET(pInput, nSize))
	{
		*pnOutputSize = nSize;
		return pInput;
	}

	// 일반적으로 "암호화된 크기 >= 원래 크기" 이다.
	// 그래서 복호화 할 때는 따로 버퍼를 잡지 않고 그냥 같은 메모리를 output 으로 넣는다.
	//
	//
	// 그렇지만, 알고리즘에 따라 Output Buffer 가 필요하다면 할당한다. AuSeed 가 추가되면서 변경됨.
	BYTE* pInputStart = NULL, *pOutput = NULL;
	if(AuPacket::GetCryptManager().UseDecryptBuffer(eCryptType) == FALSE)
		pOutput = (BYTE*)pInput + sizeof(ENC_PACKET_HEADER);
	else
	{
		PVOID pPacket = g_AuCircularBuffer.Alloc(nSize - sizeof(ENC_PACKET_HEADER) - 1);
		if(!pPacket)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - Alloc fail", __FUNCTION__, __LINE__);
			AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);

			return NULL;
		}

		pOutput = (BYTE*)pPacket;
	}

	pInputStart = (BYTE*)pInput + sizeof(ENC_PACKET_HEADER);	// 암호화 된 부분까지 넘긴다.
	AuPacket::GetCryptManager().Decrypt(csCryptActor, eCryptType, pInputStart, pOutput, nSize - sizeof(ENC_PACKET_HEADER) - 1);	// -(Encrypted Header) -(Encrypted Rear Byte)

	*pnOutputSize = ((PACKET_HEADER*)pOutput)->unPacketLength;

	return (PVOID)pOutput;
}

// 2006.04.12. steeple
void AuPacket::FreeEncryptBuffer(PVOID pBuffer, INT16 nSize)
{
	// 현재는 아무 일도 안한다. 2006.04.12
	// 왜냐하면 Circular Buffer 를 쓰기 때문에 굳이 삭제할 필요는 없다.
}

// 2008.04.01. steeple
BOOL AuPacket::SetDummy(PVOID pvPacket, INT16 nSize)
{
	// 암호화 하기 전에 Header 에 dummy 를 세팅한다.
	// 같은 암호화 패킷을 암호화 한 후에 내용이 다르게 하기 위한 거임.
	clock_t clock_ = clock();
	((PACKET_HEADER*)pvPacket)->lDummy = (UINT32)clock_;

	return TRUE;
}