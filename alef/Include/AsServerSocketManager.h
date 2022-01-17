/*
	Notices: Copyright (c) NHN Studio 2003
	Created by: Bryan Jeong (2003/12/24)
 */

// AsServerSocketManager.h: interface for the AsServerSocketManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASSERVERSOCKETMANAGER_H__53E06B8B_A33A_4AB0_A2E7_FF81042AF8D1__INCLUDED_)
#define AFX_ASSERVERSOCKETMANAGER_H__53E06B8B_A33A_4AB0_A2E7_FF81042AF8D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AsServerSocket.h"

const INT32 MAX_CONNECTION_COUNT = 5000;

class AsServerSocketManager  
{
private:
	AsServerSocket	*m_pSocketPool;
	INT32			m_lMaxConnectionCount;

	EnumSocketType			m_eDefaultSocketType;

public:
	AsServerSocketManager();
	virtual ~AsServerSocketManager();

	BOOL Initialize(INT32 lMaxConnectionCount = MAX_CONNECTION_COUNT, AsTimer *pcsTimer = NULL);
	BOOL IsValidPointer(AsServerSocket* pSocket);
	AsServerSocket* operator[](INT32 lIndex);
	VOID DisconnectAll();
	BOOL IsValidArrayIndex(INT32 lArrayIndex);

	BOOL SetMaxSendBuffer(INT32 lBufferSize);

	void SetDefaultSocketType(EnumSocketType eSocketType);

	void SendPacketToAll(PVOID pvPacket, INT32 lPacketLength, PACKET_PRIORITY ePriority = PACKET_PRIORITY_1);

	INT32 GetMaxConnectionCount() { return m_lMaxConnectionCount; };
};

#endif // !defined(AFX_ASSERVERSOCKETMANAGER_H__53E06B8B_A33A_4AB0_A2E7_FF81042AF8D1__INCLUDED_)
