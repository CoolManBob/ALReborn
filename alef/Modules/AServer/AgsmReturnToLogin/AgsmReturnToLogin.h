#pragma once

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmReturnToLoginD" )
#else
#pragma comment ( lib , "AgsmReturnToLogin" )
#endif
#endif

#include "AgpmReturnToLogin.h"
#include "AgsmCharacter.h"
#include "AgsmAccountManager.h"
//#include "AgsmLoginDB.h"

#include "AgsdAuthKey.h"

typedef enum _AgsmReturnToLoginCBID {
	AGSMRETURN_CB_NOTIFY_SAVE_ALL	= 0,
	AGSMRETURN_CB_SET_RECONNECT_CLIENT
} AgsmReturnToLoginCBID;

class AgsmReturnToLogin : public AgsModule {
private:
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmReturnToLogin	*m_pcsAgpmReturnToLogin;
	AgsmAccountManager	*m_pcsAgsmAccountManager;
	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmServerManager2	*m_pcsAgsmServerManager2;
	//AgsmLoginDB			*m_pcsAgsmLoginDB;

	AgsaAuthKeyPool		m_csAuthKeyPoolAdmin;

public:
	AgsmReturnToLogin();
	virtual ~AgsmReturnToLogin();

	BOOL	OnAddModule();
	BOOL	OnIdle(UINT32 ulClockCount);

	static BOOL	CBRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRequestKey(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBResponseKey(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBReconnectLoginServer(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	ReturnToLoginServer(AgpdCharacter *pcsCharacter);
	BOOL	ReceiveNotifySaveAll(CHAR *pszName);

	static BOOL	CBTimeoutNotifySaveAll(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);
	static BOOL	CBFinishSaveAll(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);

	BOOL	SetCallbackNotifySaveAll(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackReconnectClient(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SendPacketRequestFailed(AgpdCharacter *pcsCharacter);
	BOOL	SendPacketRequestKey(AgpdCharacter *pcsCharacter);
	BOOL	SendPacketResponseKey(CHAR *pszAccountName, INT32 lAuthKey, UINT32 ulNID);
	BOOL	SendPacketReceivedAuthKey(AgpdCharacter *pcsCharacter, INT32 lAuthKey);
	BOOL	SendPacketEndProcess(UINT32 ulNID);
};