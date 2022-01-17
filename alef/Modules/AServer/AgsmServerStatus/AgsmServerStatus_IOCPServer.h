// AgsmServerStatus_IOCPServer.h
// (C) NHN - ArchLord Development Team
// steeple, 2005. 03. 31.
//

#ifndef __AGSMSERVERSTATUS_IOCPSERVER_H__
#define __AGSMSERVERSTATUS_IOCPSERVER_H__

#include "AsIOCPServer.h"

class AgsmServerStatus_IOCPServer : public AsIOCPServer
{
public:
	AgsmServerStatus_IOCPServer();
	virtual ~AgsmServerStatus_IOCPServer();

	virtual BOOL Initialize(INT32 lThreadCount, INT32 lPort, INT32 lMaxConnectionCount = 3000, INT32 lMaxSendBufferMB = 100);
};

#endif	//__AGSMSERVERSTATUS_IOCPSERVER_H__