/*==========================================================================

	AgsmServerManager2.h

==========================================================================*/

#ifndef _AGSM_SERVER_MANAGER2_H_
	#define _AGSM_SERVER_MANAGER2_H_

#include "ApBase.h"
#include "AsDefine.h"
#include "AgsdServer2.h"
#include "AgsaServer2.h"
#include "AgsmAOIFilter.h"
//#include "AgsmDBStream.h"
#include "AuPacket.h"
#include "AgsEngine.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
	#pragma comment (lib , "AgsmServerManager2D")
#else
	#pragma comment (lib , "AgsmServerManager2")
#endif
#endif


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const INT16 AGSMSERVER_MAX_NUM_SERVER					= 2000;

typedef enum _eAGSMSERVER_PACKET_OPERATION {
	AGSMSERVER_PACKET_OPERATION_CONNECT			= 0,
	AGSMSERVER_PACKET_OPERATION_CONNECT_RESULT,		// #####
	AGSMSERVER_PACKET_OPERATION_DISCONNECT,			// #####
	AGSMSERVER_PACKET_OPERATION_SERVER_DOWN,		// #####
	AGSMSERVER_PACKET_OPERATION_SET_SERVER_FLAG,
	AGSMSERVER_PACKET_OPERATION_NOTIFY_STATUS,
	AGSMSERVER_PACKET_OPERATION_MANUAL_CONNECT,
	AGSMSERVER_PACKET_OPERATION_REPLY_CONNECT,
} eAgsmServerPacketOperation;

typedef enum eAgsmServerConnectType {
	AGSMSERVER_CONNECT_TYPE_REQUEST,
	AGSMSERVER_CONNECT_TYPE_ACCEPT
} eAgsmServerConnectType;

typedef enum _eAgsmServerCB {
	AGSMSERVER_CB_ADD_SERVER				= 0,
	AGSMSERVER_CB_REMOVE_SERVER,
	AGSMSERVER_CB_UPDATE_SERVER_STATUS,
	AGSMSERVER_CB_RECONNECT_SERVER,
} eAgsmServerCB;

// ... INI(DB) Key(Column) String
#define AGSMSERVER_INI_NAME_GROUP_NAME					_T("WORLD")
#define	AGSMSERVER_INI_NAME_SERVER_TYPE					_T("TYPE")
#define AGSMSERVER_INI_NAME_SERVER_ADDRESS				_T("ADDRESS")
#define	AGSMSERVER_INI_NAME_CONTROL_AREA				_T("CONTROLAREA")
#define	AGSMSERVER_INI_NAME_DBUSER						_T("DBUSER")
#define	AGSMSERVER_INI_NAME_DBPWD						_T("DBPWD")
#define	AGSMSERVER_INI_NAME_DBDSN						_T("DBDSN")
#define	AGSMSERVER_INI_NAME_LOGDBUSER					_T("LOGDBUSER")
#define	AGSMSERVER_INI_NAME_LOGDBPWD					_T("LOGDBPWD")
#define	AGSMSERVER_INI_NAME_LOGDBDSN					_T("LOGDBDSN")
#define	AGSMSERVER_INI_NAME_SHOW						_T("SHOW")
#define	AGSMSERVER_INI_NAME_EXTRA						_T("EXTRA")
#define AGSMSERVER_INI_NAME_DBVENDER					_T("DBVENDER")

#define RELAY_SERVER_START_ID							200


/********************************************************/
/*		The Definition of AsgsmServerManager2 class		*/
/********************************************************/
//
class AgsmServerManager2 : public AgsModule
	{
	protected:
		//	Related modules
		AgsmAOIFilter		*m_pAgsmAOIFilter;

		//	Server admin
		AgsaServer2			m_csServerAdmin;
		AgsaServerWorld		m_csWorldAdmin;

		//	This server
		INT32				m_lServerID;
		
		//	Servers in same world
		AgsdServer2*		m_pcsRelayServer;
		AgsdServer2*		m_pcsDealServer;
		AgsdServer2*		m_pcsRecruitServer;
		AgsdServer2*		m_pcsMasterDBServer;
		AgsdServer2*		m_pcsCompenDBServer;
		AgsdServer2*		m_pcsAimEventServer;
		ApSafeArray<AgsdServer2 *, AGSMSERVER_MAX_NUM_SERVER>	m_pcsGameServers;
		INT16				m_nNumGameServers;

		//	Item DB ID event
		HANDLE				m_hGetItemDBIDEvent;

	protected:
		BOOL		_IsInArea(AuAREA csArea, AuPOS csPos);

		//	Module data
		AgsdServer2*	_CreateServer();
		BOOL			_DestroyServer(AgsdServer2 *pcsServer);

	public:
		AgsmServerManager2();
		virtual ~AgsmServerManager2();		

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();

		BOOL		ReadServerInfo();

		//	Admin
		BOOL		SetMaxServer(INT16 nCount);
		BOOL		AddServer(AgsdServer2* pcsServer);
		BOOL		AddServer(INT32 lServerID, CHAR *pszWorld, CHAR *pszIP, CHAR *pszControlArea,
								INT8 cType, BOOL bShow, CHAR *pszDatabase);

		//	Status
		BOOL		UpdateServerStatus(AgsdServer2 *pcsServer, eSERVERSTATUS eNewStatus, INT16 nNumPlayers);
		
		//	Callback setting
		BOOL		SetCallbackAddServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL		SetCallbackUpdateServerStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// pData = AgsdServer2 *, pCustData = (INT16 *) nOldStatus

		//	Get server
		AgsdServer2*	GetServer(INT32 lServerID);
		AgsdServer2*	GetServer(CHAR *pszIP);
		AgsdServer2*	GetLoginServers(INT16 *pnIndex);					// iterate (by pnIndex)
		AgsdServer2*	GetGameServers(INT16 *pnIndex);
		AgsdServer2*	GetGameServerOfWorld(CHAR *pszWorld);
		AgsdServer2*	GetGameServersInGroup(INT16 *pnIndex);
		CHAR*			GetGameDBName(CHAR *pszWorld);
		AgsdServer2*	GetGameServerBySocketIndex(INT32 lIndex);	// 2004.07.14. steeple
		AgsdServer2*	GetDealServer();									// non-iterate
		AgsdServer2*	GetRecruitServer();
		AgsdServer2*	GetRelayServer();
		AgsdServer2*	GetRelayServer(AgsdServer2 *pcsGameServer);
		AgsdServer2*	GetControlServer(AuPOS posDestArea, AgsdControlArea *pstCurrentControlArea = NULL);
		UINT8			GetServerType(AgsdServer2 *pcsServer);
		AgsdServer2*	GetMasterDBServer();
		AgsdServer2*	GetCompenDBServer();
		//JK_½É¾ß¼¦´Ù¿î
		INT32			CheckMidnightShutDownTime();

		
		AgsdServer2*	GetAimEventServer()
		{
			return m_pcsAimEventServer;
		};

		INT32			GetServerID( void ) { return m_lServerID; }

		//	This server
		BOOL			InitThisServer();		// init this server and set related servers(in same world)
		int				CheckThisServer();
		AgsdServer2*	GetThisServer();
		UINT8			GetThisServerType();

		//	Packet
		BOOL			SendPacketGameServers(PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority);
	};

typedef AgsmServerManager2 AgsmServerManager;

#endif
