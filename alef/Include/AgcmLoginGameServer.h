#ifndef __AGCM_LOGIN_GAMESERVER_H__
#define __AGCM_LOGIN_GAMESERVER_H__

#include "ApBase.h"

#include "AuMD5Encrypt.h"
#include "AgcModule.h"
#include "ApBase.h"
#include "AgpmLogin.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgcmConnectManager.h"
#include "resource.h"
#include "AgcEngine.h"
#include "AgcmRender.h"
#include "AgcmMap.h"
//@{ kday 20050113
//#include "AgcmCamera.h"
//@} kday
#include "AgcmLogin.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmLoginGameServerD" )
#else
#pragma comment ( lib , "AgcmLoginGameServer" )
#endif
#endif

#define AGCMLOGIN_ID_MAX_LENGTH				64
#define	AGCMLOGIN_PASSWORD_MAX_LENGTH		64

// Login 관련 Callback
typedef enum AgcmLoginGameServerCallbackPoint
{
	AGCMLOGINGAMESERVER_CB_ID_RESPONSE_RETURN_TO_LOGINSERVER	= 0 
} AgcmLoginGameServerCallbackPoint;

class AgcmLoginGameServer : public AgcModule
{
	AgpmLogin			*m_pcsAgpmLogin;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgcmCharacter		*m_pcsAgcmCharacter;
	AgcmConnectManager	*m_pcsAgcmConnectManager;
	AgcmLogin			*m_pcsAgcmLogin;

	INT16					m_ulNID;

public:

	BOOL	SendReturnToLoginServer( char *pstrServerGroupName, char *pstrAccountID, char *pstrCharName, INT32 lCID );
	BOOL	SendDisconnectFromGameServer( char *pstrServerGroupName, char *pstrAccountID, char *pstrCharName, INT32 lCID );

	BOOL	SetCallbackResponseReturnToLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)				;
	
	AgcmLoginGameServer();
	~AgcmLoginGameServer();

	virtual BOOL OnAddModule();

	//패킷을 받고 파싱하자~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);
};

#endif