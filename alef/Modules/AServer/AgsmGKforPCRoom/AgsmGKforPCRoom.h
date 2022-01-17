#ifndef	__AGSMGKFORPCROOM_H__
#define	__AGSMGKFORPCROOM_H__

#include "AgsEngine.h"
#include "AgsmServerManager2.h"

#include "AgsmGKPacket.h"
#include "AgsmCharacter.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmGKforPCRoomD" )
#else
#pragma comment ( lib , "AgsmGKforPCRoom" )
#endif
#endif

typedef enum _AgsmGKCBID {
	AGSMGK_RECEIVE_AUTH_RESULT				= 0,
	AGSMGK_RECEIVE_BILL_AUTH_RESULT
} AgsmGKCBID;

typedef enum	_AgsmGKReturn {
	AGSMGK_RETURN_NOT_EXIST_LKSERVER		= 0,
	AGSMGK_RETURN_CONNECT_FAIL,
	AGSMGK_RETURN_CONNECT_SUCCESS,
	AGSMGK_RETURN_NEED_WAIT,
} AgsmGKReturn;

class AgsmGK : public AgsModule {
private:
	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmServerManager2	*m_pcsAgsmServerManager2;

	ApMemoryPool		m_csPacketMemoryPool;

	AgsdServer			*m_pcsAgsdLKServer;

public:
	AgsmGK();
	virtual ~AgsmGK();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();

	static BOOL	DispatchGK(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);
	static BOOL	DisconnectGK(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);

	BOOL	SendPacketAuth(CHAR *pszAccountID, UINT32 ulNID);
	BOOL	SendPacketLogout(CHAR *pszAccountID, UINT32 ulNID);
	BOOL	SendPacketBillAuth(CHAR *pszAccountID, UINT32 ulNID);

	static BOOL	CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBExitGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAddServerInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	AgsmGKReturn	ConnectLKServer();
	BOOL	WaitForConnectServer();

	BOOL	SetCallbackReceiveAuthResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReceiveBillAuthResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL CheckConnectLKServer(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);

	AgsdServer*	GetLKServer() {return m_pcsAgsdLKServer;};

private:
	BOOL	OnReceive(PVOID pvPacket, UINT32 ulNID);
};

#endif	//__AGSMGKFORPCROOM_H__