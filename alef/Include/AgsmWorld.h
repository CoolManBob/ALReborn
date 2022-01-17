/*=============================================================================

	AgsmWorld.h

=============================================================================*/

#ifndef _AGSM_WORLD_H_
	#define _AGSM_WORLD_H_


#include "ApBase.h"
#include "AsDefine.h"
#include "AgsEngine.h"
#include "AgpmWorld.h"
#include "AgsmInterServerLink.h"
#include "AgsmServerManager2.h"
#include "AgsmLoginClient.h"


/************************************************/
/*		The Definition of AgsmWorld class		*/
/************************************************/
//
class AgsmWorld : public AgsModule
	{
	protected:
		//	Related modules
		AgpmWorld			*m_pAgpmWorld;
		AgsmServerManager	*m_pAgsmServerManager;
		AgsmInterServerLink	*m_pAgsmInterServerLink;
		AgsmLoginClient		*m_pAgsmLoginClient;
	
	protected:
		BOOL	_UpdateStatus(AgsdServer* pcsServer);
		BOOL	_UpdateStatus(AgsdServer *pcsServer, AgpdWorld *pcsWorld);
		INT16	_CalculateStatus(AgsdServer* pcsServer);

		//	Packet send(internal)
		BOOL	_SendPacketGetWorldResult(TCHAR* pszWorld, UINT32 ulNID);
		BOOL	_SendPacketGetWorldResult(AgpdWorld* pcsAgpdWorld, UINT32 ulNID);
		BOOL	_SendPacketGetWorldResultAll(TCHAR* pszEncoded, UINT32 ulNID);

	public:
		AgsmWorld();
		virtual ~AgsmWorld();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();
		
		//	Packet send(external)
		BOOL	SendPacketCharCount(TCHAR *pszAccount, TCHAR *pszEncoded, UINT32 ulNID);

		//	Callback methods
		static BOOL	CBGetWorld(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBUpdateServerStatus(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBAddServer(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBRemoveServer(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBDisconnectServer(PVOID pData, PVOID pClass, PVOID pCustData);
	};

#endif
