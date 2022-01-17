/******************************************************************************
Module:  AgsmCharManager.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 23
******************************************************************************/

#if !defined(__AGSMCHARMANAGER_H__)
#define __AGSMCHARMANAGER_H__

#include "ApBase.h"

#include <dplay8.h>

#include "AuPacket.h"
#include "AuGenerateID.h"

#include "ApmMap.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgpmItem.h"
#include "AgpmEventBinding.h"
#include "AgpmStartupEncryption.h"
#include "AgpmLog.h"
#include "AgpmAdmin.h"
#include "AgpmBillInfo.h"

//#include "AgsmDBStream.h"
#include "AgsEngine.h"
#include "AgsmAOIFilter.h"
//#include "AgsmServerManager.h"
#include "AgsmServerManager2.h"
#include "AgsmFactors.h"
#include "AgsmCharacter.h"
#include "AgsmSystemInfo.h"
#include "AgsmZoning.h"
#include "AgsmItem.h"
#include "AgpmConfig.h"

#include "AgsmAccountManager.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmCharManagerD" )
#else
#pragma comment ( lib , "AgsmCharManager" )
#endif
#endif

//#define	__ATTACH_LOGINSERVER__

typedef enum _eAGSMCHARMM_CB {
	AGSMCHARMM_CB_CREATECHAR				= 0,
	AGSMCHARMM_CB_DELETECHAR,
	AGSMCHARMM_CB_LOADCHAR,
	AGSMCHARMM_CB_UNLOADCHAR,
	AGSMCHARMM_CB_ENTERGAMEWORLD,
	//AGSMCHARMANAGER_CB_SENDADDCHAR

	AGSMCHARMM_CB_CONNECTED_CHAR,

	AGSMCHARMM_CB_INSERT_CHARACTER_TO_DB,

	AGSMCHARMM_CB_ADDCHAR_RESULT,
	
	AGSMCHARMM_CB_SET_CHAR_LEVEL,

	AGSMCHARMM_CB_DISCONNECT_CHARACTER,

	AGSMCHARMM_CB_DELETE_COMPLETE,

	AGSMCHARMM_CB_SET_CHARACTER_GAME_DATA,
	AGSMCHARMM_CB_CHECK_LOGIN_POSITION,
}eAGSMCHARMM_CB;

/*
typedef enum _eAGSMCHARMM_DATA_TYPE {
	AGSMCHARMM_DATA_TYPE_ACCOUNT	= 0,
	AGSMCHARMM_DATA_TYPE_CHARACTER
} eAGSMCHARMM_DATA_TYPE;
*/

//#define	AGSMCHARMANAGER_PACKET_TYPE		0x03

typedef enum _eAgsmCharManagerPacketOperation {
#ifndef	__ATTACH_LOGINSERVER__
	AGSMCHARMANAGER_PACKET_OPERATION_LOGIN						= 0,
	AGSMCHARMANAGER_PACKET_OPERATION_LOGIN_RESULT,
	AGSMCHARMANAGER_PACKET_OPERATION_CREATECHAR,
	AGSMCHARMANAGER_PACKET_OPERATION_CREATECHAR_RESULT,
	AGSMCHARMANAGER_PACKET_OPERATION_DELETECHAR,
	AGSMCHARMANAGER_PACKET_OPERATION_DELETECHAR_RESULT,
	AGSMCHARMANAGER_PACKET_OPERATION_SELECTCHAR,
	AGSMCHARMANAGER_PACKET_OPERATION_SELECTCHAR_RESULT,
	AGSMCHARMANAGER_PACKET_OPERATION_LOADCHAR,
	AGSMCHARMANAGER_PACKET_OPERATION_LOADCHAR_RESULT
#else
	AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_ADDCHAR			= 0,
	AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_ADDCHAR_RESULT,
	AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_CLIENT_CONNECT,
	AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_CLIENT_CONNECT_RESULT,
	AGSMCHARMANAGER_PACKET_OPERATION_SEND_COMPLETE,
	AGSMCHARMANAGER_PACKET_OPERATION_SEND_INCOMPLETE,
	AGSMCHARMANAGER_PACKET_OPERATION_SEND_CANCEL,
	AGSMCHARMANAGER_PACKET_OPERATION_SETTING_CHARACTER_OK,
	AGSMCHARMANAGER_PACKET_OPERATION_CHARACTER_POSITION,
	AGSMCHARMANAGER_PACKET_OPERATION_LOADING_COMPLETE,
	AGSMCHARMANAGER_PACKET_OPERATION_DELETE_COMPLETE,
	AGSMCHARMANAGER_PACKET_OPERATION_AUTH_KEY,
#endif //__ATTACH_LOGINSERVER__
} eAgsmCharManagerPacketOperation;


typedef struct _stAgsmCharManagerLoginInfo {
	CHAR	*szServerGroupName;
	CHAR	*szAccountName;
	CHAR	*szCharName;
} stAgsmCharManagerLoginInfo, *pstAgsmCharManagerLoginInfo;

class AgsmSkill;
class AgsmTitle;
class AgsmBillInfo;
class AgsmBilling;//JK_빌링
class AuHangameTPack;
class AuHanIPCheckForServer;
class AgsmPrivateTrade; //JK_거래중금지


//	AgsmCharManager class
///////////////////////////////////////////////////////////////////////////////
class AgsmCharManager : public AgsModule {
private:
	ApAdmin						m_csCertificatedAccount;

	ApmMap*						m_papmMap;
	ApmEventManager*			m_papmEventManager;
	AgpmFactors*				m_pagpmFactors;
	AgpmCharacter*				m_pagpmCharacter;
	AgpmItem*					m_pagpmItem;
	AgpmEventBinding*			m_pagpmEventBinding;
	AgpmStartupEncryption*		m_pagpmStartupEncryption;
	AgpmLog*					m_pagpmLog;
	AgpmAdmin*					m_pagpmAdmin;
	AgpmBillInfo*				m_pagpmBillInfo;
	AgsmBillInfo*				m_pagsmBillInfo;
	AgsmBilling*				m_pagsmBilling;//JK_빌링

	//AgsmDBStream*				m_pagsmDBStream;
	AgsmAOIFilter*				m_pagsmAOIFilter;
	//AgsmServerManager*			m_pagsmServerManager;
	AgsmServerManager*			m_pAgsmServerManager;
	AgsmCharacter*				m_pagsmCharacter;
	AgsmSystemInfo*				m_pagsmSystemInfo;
	AgsmFactors*				m_pagsmFactors;
	AgsmAccountManager*			m_pagsmAccountManager;
	AgsmZoning*					m_pagsmZoning;
	AgsmTitle*					m_pagsmTitle;

	//AgsaAccount					m_csAccountAdmin;
	//AgsaCharacterManager		m_csCharAdmin;
	
	AgsmItem*					m_pagsmItem;

	AgpmConfig*					m_pAgpmConfig;
	AgsmSkill*					m_pagsmSkill;

	AgsmPrivateTrade*			m_pcsAgsmPrivateTrade;//JK_거래중금지


	AuPacket					m_csPacket;

	//AuGenerateID				m_csGenerateAID;
	AuGenerateID				m_csGenerateCID;
#ifdef _AREA_KOREA_
#ifdef _HANGAME_
	AuHanIPCheckForServer*		m_csHanIPCheck;
	AuHangameTPack*				m_csHangameTPack;
#endif
#endif
	BOOL IsValidLogin(CHAR *szAccountName);
	BOOL SendLoginResult(CHAR *szAccountName, INT32 lAccountID, UINT32 ulNID);

	//AgsdAccount* CreateAccountData();
	//BOOL DestroyAccountData(AgsdAccount *pcsAccount);
	//AgpdCharacter* CreateCharacterData();
	//BOOL DestroyCharacterData(AgpdCharacter *pcsCharacter);

	//AgsdAccount* AddAccount(CHAR *szAccountID);
	//AgsdAccount* GetAccount(CHAR *szAccountID);
	//AgsdAccount* GetAccount(INT32 lAccountID);
	//BOOL RemoveAccount(CHAR *szAccountID);
	//BOOL RemoveAccount(INT32 lAccountID);

	INT16 PreSetting();

	BOOL SendCharacterSettingOK(AgpdCharacter *pcsCharacter, UINT32 ulNID);
	BOOL SendCharacterLoadingPosition(AgpdCharacter *pcsCharacter, UINT32 ulNID);

public:
	AgsmCharManager();
	~AgsmCharManager();

	BOOL OnAddModule();

	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	BOOL OnValid(CHAR* szData, INT16 nSize);

	//BOOL SetMaxAccount(INT16 nCount);
	//BOOL SetMaxCharacter(INT16 nCount);

	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL OnDisconnect(INT32 lCID, UINT32 ulNID);

	//AgpdCharacter* GetCharacter(CHAR *szAccountID, INT32 lCID);
	//AgpdCharacter* GetCharacter(AgsdAccount *pcsAccount, INT32 lCID);

	//INT16 EnterGameWorld(AgpdCharacter *pCharacter, DPNID dpnid);		// pc
	INT16 EnterGameWorld(AgpdCharacter *pCharacter, BOOL bIsCheckValidate = TRUE);					// npc

	BOOL InitPCRoomModule();
	BOOL CheckPCRoom(AgpdCharacter* pcsCharacter);
	BOOL CheckOutPCRoom(AgpdCharacter* pcsCharacter);

//	INT16 PostSetting(DPNID dpnid);

	BOOL SendCompletePacket(CHAR *szServerGroupName, CHAR *szAccountName, CHAR *szCharName, UINT32 ulNID);
	BOOL SendCancelPacket(CHAR *szServerGroupName, CHAR *szAccountName, CHAR *szCharName, UINT32 ulNID);
	BOOL SendDeleteCompletePacket(CHAR* szCharName, UINT32 ulNID);

	BOOL SendAuthKeyPacket(CHAR* szCharName, INT32 lAuthKey, UINT32 ulNID);

	BOOL SetCallbackCreateChar		( ApModuleDefaultCallBack pfCallback, PVOID pClass	);
	BOOL SetCallbackDeleteChar		( ApModuleDefaultCallBack pfCallback, PVOID pClass	);
	BOOL SetCallbackLoadChar		( ApModuleDefaultCallBack pfCallback, PVOID pClass	);
	BOOL SetCallbackUnLoadChar		( ApModuleDefaultCallBack pfCallback, PVOID pClass	);
	BOOL SetCallbackEnterGameWorld	( ApModuleDefaultCallBack pfCallback, PVOID pClass	);

	BOOL SetCallbackConnectedChar	( ApModuleDefaultCallBack pfCallback, PVOID pClass	);

	BOOL SetCallbackInsertCharacterToDB	( ApModuleDefaultCallBack pfCallback, PVOID pClass	);

	BOOL SetCallbackAddCharResult	( ApModuleDefaultCallBack pfCallback, PVOID pClass	);

	BOOL SetCallbackSetCharLevel	( ApModuleDefaultCallBack pfCallback, PVOID pClass	);

	BOOL SetCallbackDisconnectCharacter	( ApModuleDefaultCallBack pfCallback, PVOID pClass	);

	BOOL SetCallbackDeleteComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSetCharacterGameData( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL SetCallbackCheckLoginPosition( ApModuleDefaultCallBack pfCallback, PVOID pClass );

	BOOL InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag, INT32 lRemoveIDQueueSize = 0);

	INT16 AttachCharacterData(PVOID pClass, INT16 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	static BOOL CBRemoveCharFromMap(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveCharacterID(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBGetNewCID(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBDeleteCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBCompleteRecvCharFromLoginServer(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBSendAuthKey(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBCheckPCRoomType(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	UpdateInit(AgpdCharacter *pcsCharacter);

	//AgpdCharacter* AddCharacter(CHAR *szAccountID, CHAR *szName, INT32 lTID, UINT32 ulNID);
	AgpdCharacter* AddCharacter(/*AgsdAccount *pcsAccount, */CHAR *szName, INT32 lTID, UINT32 ulNID, BOOL bLoadChar = FALSE);
	//BOOL RemoveCharacter(/*CHAR *szAccountID, */INT32 lCID);
	//BOOL RemoveCharacter(AgsdAccount *pcsAccount, INT32 lCID);

	/*
	INT16	LoadCharacter(AgsdAccount *pcsAccount);
	BOOL	LoadCharacterComplete(AgpdCharacter *pcsCharacter);
	BOOL	LoadCharacterFailed(AgsdAccount *pcsAccount);

	INT16	SelectCharacter(CHAR *szAccountName, INT32 lCID);
	*/

	AgpdCharacter* CreateCharacter(CHAR *szAccountName, CHAR *szCharName, INT32 lTID, UINT32 ulNID, BOOL bIsPC = TRUE, CHAR *pszServerName = NULL);
	//BOOL	CreateCharacterComplete(AgpdCharacter *pcsCharacter);

	/*
	INT16 DeleteCharacter(CHAR *szAccountName, INT32 lCID);
	INT16 DeleteCharacter(CHAR *szAccountName, AgpdCharacter *pcsCharacter);
	*/

	//인증된 Account관리용
	BOOL AddCertificatedAccount( char *pstrAccountID, INT32 lCID );
	BOOL RemoveCertificatedAccount( char *pstrAccountID );
	INT32 GetCertificatedAccountCID( char *pstrAccountID );

	/*
	static BOOL CBDBOperationResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDBFactor(PVOID pData, PVOID pClass, PVOID pCustData);
	*/
	static BOOL CBDisconnectDB(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	AdjustCharMaxPoint(AgpdCharacter *pcsCharacter);
	

	//////////////////////////////////////////////////////////////////////////
	// Log 관련 - 2004.05.02. steeple
	BOOL	WriteLoginLog(AgpdCharacter* pcsAgpdCharacter);
	BOOL	WriteLogoutLog(AgpdCharacter* pcsAgpdCharacter);
	BOOL	WriteLogoutBankLog(AgpdCharacter* pcsAgpdCharacter);

	BOOL	EnumCallbackCreateCharacter(AgpdCharacter * pcsAgpdCharacter);

//	BOOL	SendDummyPacket(INT32 lSize, INT32 lCount);
};

#endif //__AGSMCHARMANAGER_H__
