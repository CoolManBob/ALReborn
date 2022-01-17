/*=============================================================

	AgsmInterServerLink.h

=============================================================*/

#ifndef _AGSM_INTER_SERVER_LINK_H_
	#define _AGSM_INTER_SERVER_LINK_H_

#include "ApBase.h"
#include "AuPacket.h"
#include "AsDefine.h"
#include "AgsEngine.h"

#include "AgsmServerManager2.h"
#include <deque>
using namespace std;

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment (lib, "AgsmInterServerLinkD")
#else
	#pragma comment (lib, "AgsmInterServerLink")
#endif
#endif

#define AGSMINTERSERVERLINK_MAX_WAIT_TIME		2000
#define AGSMINTERSERVERLINK_TERM_RECONNECT		3000
#define AGSMINTERSERVERLINK_TERM_NOTIFY			5000
#define	AGSMINTERSERVERLINK_MAX_RETRY			10

typedef enum _eAGSMINTERSERVERLINK_CB
	{
	AGSMINTERSERVERLINK_CB_CONNECT				= 0,
	AGSMINTERSERVERLINK_CB_NOTIFY,
	AGSMINTERSERVERLINK_CB_DISCONNECT,
	AGSMINTERSERVERLINK_CB_REPLY_FAIL,
	AGSMINTERSERVERLINK_CB_GET_CURRENT_USER,
	} eAGSMINTERSERVELINK;

/********************************************************/
/*		The Definition of AgsmInterServerLink class		*/
/********************************************************/
//
class AgsmInterServerLink : public AgsModule
	{
	protected:
		AgsmServerManager		*m_pAgsmServerManager;
		AgsmAOIFilter			*m_pAgsmAOIFilter;
		AuPacket				m_csPacket;

		// ... For idle
		UINT32					m_ulPreviousClockReconnect;
		UINT32					m_ulPreviousClockNotify;

		deque<AgsdServer *>		m_Notify;
		deque<AgsdServer *>		m_DisconnectedServers;

		INT32					m_lReplyCount;

		ApMutualEx				m_NotifyCS;
		ApMutualEx				m_DisconnectedServersCS;
		ApMutualEx				m_ReplyCS;

		BOOL					m_bDisconnectLoginServer;
		BOOL					m_bDestroyThread;

		stTHREADOBJECT			m_stSendThread;

	public:
		AgsmInterServerLink();
		virtual ~AgsmInterServerLink();

		// ... ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
		BOOL	OnDisconnectSvr(INT32 lServerID, UINT32 ulNID);
		BOOL	OnIdle2(UINT32 ulClockCount);

		// ... Connect/Disconnect method
		BOOL	ConnectAll(BOOL bExceptRelay = FALSE);
		BOOL	DisconnectAll();
		BOOL	DisconnectLoginServers(BOOL bPushQueue = TRUE);
		BOOL	SetDisconnectLoginServer(BOOL bSet = TRUE);

		// ... Connect servers per type
		BOOL	ConnectGameServers();
		BOOL	ConnectLoginServers(BOOL bForceConnect = FALSE);
		BOOL	ConnectRelayServer(BOOL bForceConnect = FALSE);

		static BOOL	ConnectCheckCallback(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);

	protected:
		// ... Internal
		BOOL	_Connect(AgsdServer *pcsServer);
		BOOL	_ConnectEx(AgsdServer *pcsServer);
		void	_Disconnect(AgsdServer *pcsServer);
		BOOL	_Notify(AgsdServer *pcsServer);
		void	_RequestReply(AgsdServer *pcsServer);
		void	_ResponseReply(AgsdServer *pcsServer);
		void	_OnConnect(AgsdServer *pcsServer, UINT32 ulNID);
		void	_OnDisconnect(AgsdServer *pcsServer, UINT32 ulNID);
		void	_OnNotify(AgsdServer *pcsServer, INT8 nStatus, INT16 nNumPlayers);

		// ... OnIdle operation
		void	_IdleReconnect(UINT32 ulClockCount);
		void	_IdleNotify(UINT32 ulClockCount);	

		// ... Queue
		void		_PushNotify(AgsdServer *pcsServer);
		AgsdServer*	_PopNotify();
		void		_PushDisconnected(AgsdServer *pcsServer);
		AgsdServer* _PopDisconnected();

		// ... Packet send
		BOOL	_SendConnectInfo(AgsdServer *pcsServer);
		BOOL	_SendStatusInfo(AgsdServer *pcsServer);
		BOOL	_SendReplyConnect(AgsdServer *pcsServer);

	public:
		// ... Callback setting
		BOOL	SetCallbackConnect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackNotify(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackDisconnect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackReplyFail(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		BOOL	SetCallbackGetCurrentUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		static UINT WINAPI ConnectServerThread(PVOID pvParam);
	};

#endif // _AGSM_INTER_SERVER_LINK_H_