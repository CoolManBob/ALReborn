/******************************************************************************
Module:  AgsmAccountManager.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 01. 27
******************************************************************************/

#if !defined(__AGSMACCOUNTMANAGER_H__)
#define __AGSMACCOUNTMANAGER_H__

#include "AgpmCharacter.h"
#include "AgpmBillInfo.h"
#include "AgsEngine.h"
#include "AgsaAccountManager.h"
#include "AuPacket.h"
#include "AuGenerateID.h"
#include "AgsmServerManager2.h"//JK_심야샷다운

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmAccountManagerD" )
#else
#pragma comment ( lib , "AgsmAccountManager" )
#endif
#endif

#define TPACK_EXPIRED_CALLBACK_LOG_FILENAME "LOG\\TPackExpiredCallBack.log"

typedef enum _eAGSMACCOUNT_DATA_TYPE {
	AGSMACCOUNT_DATA_TYPE_ACCOUNT	= 0
} eAGSMACCOUNT_DATA_TYPE;

/*
typedef enum _eAgsmAccountPacketOperation {
	AGSMACCOUNT_PACKET_OPERATION_ADD		= 0,
	AGSMACCOUNT_PACKET_OPERATION_REMOVE
} eAgsmAccountPacketOperation;
*/

typedef enum _AgsmAccountCBID {
	AGSMACCOUNT_CB_ID_RESET_NID				= 0,
	AGSMACCOUNT_CB_ID_REMOVE_CHARACTER,
} AgsmAccountCBID;

class AgsmBillInfo;
class AgsmLoginClient;//JK_중복로그인

class AgsmAccountManager : public AgsModule {
private:
	static AgsmAccountManager*	m_pInstance;

	AgpmCharacter		*m_pagpmCharacter;
	AgpmBillInfo		*m_pcsAgpmBillInfo;
	AgsmBillInfo		*m_pcsAgsmBillInfo;
	AgsmLoginClient		*m_pcsAgsmLoginClient;//JK_중복로그인
	AgsmServerManager	*m_pAgsmServerManager;//JK_심야샷다운
	AgpmConfig			*m_pAgpmConfig;//JK_심야샷다운

	AgsaAccount			m_csAccountAdmin;

	AuGenerateID		m_csGenerateID;

	ApAdmin				m_csAdminAccountRemove;

	UINT32				m_ulPrevRemoveClockCount;

	UINT32				m_ulLastCheckClockForMidNightShutDown;//JK_심야샷다운

public:
	AgsmAccountManager();
	~AgsmAccountManager();

	static AgsmAccountManager* GetInstance()
	{
		return AgsmAccountManager::m_pInstance;
	}

	BOOL			OnAddModule();

	BOOL			OnInit();
	BOOL			OnDestroy();
	BOOL			OnIdle(UINT32 ulClockCount);
	BOOL			OnIdle2(UINT32 ulClockCount);

	BOOL			SetMaxAccount(INT32 lCount);

	//BOOL			InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag);

	AgsdAccount*	CreateAccount();
	BOOL			DestroyAccount(AgsdAccount *pcsAccount);

	AgsdAccount*	AddAccount(CHAR *szAccountID, UINT32 ulNID);
	//AgsdAccount*	AddAccountLocal(CHAR *szAccountID, INT32 lAccountID);
	AgsdAccount*	GetAccount(CHAR *szAccountID);
	AgsdAccount*	GetAccount(INT32 lAccountID);
	AgsdAccount*	GetAccountLock(CHAR *szAccountID);
	AgsdAccount*	GetAccountLock(INT32 lAccountID);
	BOOL			RemoveAccount(CHAR *szAccountID, BOOL bRemoveCharacter = FALSE);
	BOOL			RemoveAccount(INT32 lAccountID, BOOL bRemoveCharacter = FALSE);
	//BOOL			RemoveAccountLocal(AgsdAccount *pcsAccount);

	//BOOL			LockAdminAccount();
	//BOOL			ReleaseAdminAccount();

	//BOOL			SetSelectedChar(AgsdAccount *pcsAccount, INT32 lCID);

	//BOOL			OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);

	BOOL			AddCharacterToAccount(CHAR *szAccountID, INT32 lCID, CHAR *szCharName, BOOL bIsSetBillInfo = TRUE);
	BOOL			RemoveCharacterFromAccount(CHAR *szAccountID, CHAR *szCharName);
	INT32			GetCIDFromAccount(CHAR *szAccountID, CHAR *szCharName);
	BOOL			RemoveAllCharacters(CHAR *szAccountID);

	BOOL			RemoveCharacterExceptOne(CHAR *szAccountID, CHAR *szCharName);

	BOOL			SetLoginStep(CHAR *szAccountID, AgpdLoginStep eNewStep);
	AgpdLoginStep	GetLoginStep(CHAR *szAccountID);

	BOOL			SetCallbackResetNID(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL			SetBillInfo(CHAR *pszAccountID, AgpdBillInfo *pcsBillInfo);

	BOOL			SetCouponNum(AgsdAccount *pcsAccount, INT32 lCoupon);
	CouponInfo		*AddCouponInfo(AgsdAccount *pcsAccount, CouponInfo *pcsCouponInfo);

	INT32			GetAccountNum();

	BOOL			SetMaxAccountRemove(INT32 nCount);
	BOOL			ProcessRemove(UINT32 ulClockCount);
	BOOL			AddRemoveAccount(AgsdAccount *pcsAccount);

	BOOL			UpdateTPackType(CHAR* szAccount, BOOL bAdd);
	AgpdCharacter*	GetPlayingCharacter(CHAR *szAccountID);
	//JK_중복로그인
	BOOL			RemoveDuplicateAccountWithMessage(CHAR *szAccountID);

#ifdef _AREA_KOREA_
#ifdef _HANGAME_
	// T-Pack관련 상품만료 이벤트 관련 Callback
	static int		OnBillingEventNotified(PVOID ppInfo);
#endif
#endif
};

#endif //__AGSMACCOUNTMANAGER_H__
