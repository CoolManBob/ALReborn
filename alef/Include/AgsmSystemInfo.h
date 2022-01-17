#ifndef __AGSMSYSTEMINFO_H_
#define __AGSMSYSTEMINFO_H_

#include "AgsEngine.h"
#include "AgpmCharacter.h"
#include "AgsmCharacter.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmSystemInfoD" )
#else
#pragma comment ( lib , "AgsmSystemInfo" )
#endif
#endif

class AgsmSystemInfo : public AgsModule
{
	AgpmCharacter		*m_pagpmCharacter;
	AgsmCharacter		*m_pagsmCharacter;

	AuPacket			m_csPacket;   //SystemInfo 패킷.

public:

	AgsmSystemInfo();
	~AgsmSystemInfo();

	BOOL SendServerInfo( INT16 nServerType, INT32 lCID, AgsdServer *pcsAgsdServer );

	//어디에나 있는 OnAddModule이다. 특별할거 없다. 특별하면 안되나? 아햏햏~
	BOOL OnAddModule();
	//패킷을 받고 파싱하자~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
};

#endif