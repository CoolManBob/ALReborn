#pragma once

//#include "PatchInfo.h"
#include "AuPacket.h"

#include "AuPatchCheckCode.h"
#include "AuPackingManager.h"

#include <stdio.h>
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>

#include <vector>
#include <string>

#include "PatchPacketTypeCS.h"
#include "PatchPacketTypeSC.h"

#include "AgsEngine.h"
#include "ApAdmin.h"

#define PATCH_SERVER_MAX_IP		5
#define MAX_DEBUG_MESSAGE		256

class AgsmPatchServer2 : public AgsModule
{
public:
	typedef vector<CJZPFileInfo *>		VectorPatchInfo;
	typedef VectorPatchInfo::iterator	IterPatchInfo;

public:
	//ÆÐÄ¡¿ë
	ApAdmin				m_csClientPatchInfo;

	AuPackingManager	m_cAuPackingManager;
	AuPatchCheckCode	m_cAuPatchCheckCode;

	CHAR				m_strServerIP[PATCH_DOWNLOAD_SERVER_LENGTH];
	UINT32				m_iServerPort;

	INT32				m_iVersion;
	INT32				m_iCurrentUsers;

	CCriticalSection	m_cs;

public:
	AgsmPatchServer2();
	~AgsmPatchServer2();

	BOOL OnAddModule();
	BOOL OnInit();

	BOOL OnDisconnect(AsServerSocket *pSocket);
	BOOL OnReceive(PVOID pvPacket, UINT32 ulNID);

	//PatchCode
	INT32 GetPatchFiles( INT32 iVersion, UINT32 iPatchCheckCode, VectorPatchInfo *pcList );

	//Download Server Set
	BOOL LoadDownlodINI(CHAR *szFile);
	BOOL SetDownloadServer( CHAR *pstrServerIP, UINT32 iServerPort );
	CHAR *GetDownloadServerIP();
	UINT32 GetDownloadServerPort();

	//PatchCheckCode Load;
	BOOL LoadPatchCode( CHAR *pstrFileName );

	VectorPatchInfo *GetPatchInfo(UINT32 ulNID);
	BOOL RemovePatchInfo(UINT32 ulNID);

	BOOL SendPatchFileInfo(UINT32 ulNID);
	BOOL SendRemoveFileInfo( UINT32 ulNID );

	static BOOL DispatchPatch(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);
	static BOOL DisconnectPatch(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);
};
