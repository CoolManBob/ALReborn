//
//	AsDP9Peer.h
////////////////////////////////////////////////////////////////////////////

#ifndef __ASDP9PEER_H__
#define __ASDP9PEER_H__

#include "AsDP9.h"
#include <dplay8.h>

const int ASDP9_PEER_MAX_CONNECTION			= 16;

class AsDP9Peer {
public:
	AsDP9Peer();
	~AsDP9Peer();

	BOOL	Initialize(GUID *pguidApp, AsCMDQueue *pqueueRecv);

	INT16	Open(CHAR *szIPAddress);
	BOOL	Close(INT16 nDPIndex);

	HRESULT	Start();
	BOOL	Stop();

	BOOL	IsConnected(INT16 nServerID);

	INT16	Send(CHAR* pData, INT16 nDataSize, INT16 nServerID);

	BOOL	SetSelfCID(INT32 lServerID);
};

#endif //__ASDP9PEER_H__
