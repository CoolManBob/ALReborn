#ifndef	__AUSENDDEBUGINFO_H__
#define	__AUSENDDEBUGINFO_H__

#include <winsock2.h>
#include <windows.h>


#pragma comment (lib, "ws2_32")

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AuSendDebugInfoD" )
#else
#pragma comment ( lib , "AuSendDebugInfo" )
#endif
#endif

class AuSendDebugInfo {
private:
	BOOL	SendData(SOCKET socket, PVOID pvData, UINT32 ulLength);

public:
	AuSendDebugInfo();
	~AuSendDebugInfo();

	BOOL	SendDebugInfoFile(CHAR *szServerIPAddr, UINT32 ulPort, CHAR *szDebugFileName);
	//@{ Jaewon 20041026
	// send memory data version
	BOOL SendDebugInfoMemory(CHAR *szServerIPAddr, UINT32 ulPort, UINT8 *pData, UINT32 ulSize);
	//@} Jaewon
};

#endif	//__AUSENDDEBUGINFO_H__