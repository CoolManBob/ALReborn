// AgcmCasper.h: interface for the AgcmCasper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMCASPER_H__0D97DD43_9A5D_4500_B004_F503087F1D1C__INCLUDED_)
#define AFX_AGCMCASPER_H__0D97DD43_9A5D_4500_B004_F503087F1D1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcEngine.h"
#include "AgpmCasper.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgcmConnectManager.h"
#include "AgpmOptimizedPacket2.h"

#define AGCM_MAX_USER	800

typedef enum
{
	AGCMCASPER_ACTION_TYPE_NONE	=	0,
	AGCMCASPER_ACTION_TYPE_MOVE,
	AGCMCASPER_ACTION_TYPE_EQUIP,
	AGCMCASPER_ACTION_TYPE_ATTACK,
	AGCMCASPER_ACTION_TYPE_LOGOUT,
	AGCMCASPER_MAX_ACTION_TYPE,
} AgcdCasperActionType;

typedef enum
{
	AGCDCASPER_STATUS_INIT	=	0,
	AGCDCASPER_STATUS_CONNECTING,
	AGCDCASPER_STATUS_CONNECTED,
	AGCDCASPER_STATUS_LOGIN,
} AgcdCasperUserStatus;

typedef struct
{
	INT32					m_lCID				;
	AgpdCharacter *			m_pcsCharacter		;
	AcClientSocket *		m_pcsSocket			;
	AgcdCasperActionType	m_eCurrentAction	;
	INT32					m_lLastActionTick	;
	AuPOS					m_stSpawnPosition	;
	INT32					m_lTargetCID		;
	AgcdCasperUserStatus	m_eStatus			;
} AgcdCasperUser;

class AgcmCasper : public AgcModule  
{
public:
	ApmMap *				m_pcsApmMap;
	AgpmCharacter *			m_pcsAgpmCharacter;
	AgpmItem *				m_pcsAgpmItem;
	AgpmCasper *			m_pcsAgpmCasper;
	//AgpmStartupEncryption*	m_pcsAgpmStartupEncryption;
	AgpmOptimizedPacket2*	m_pcsAgpmOptimizedPacket2;

	AgcmConnectManager *	m_pcsAgcmConnectManager;
	//AgcmStartupEncryption*	m_pcsAgcmStartupEncryption;

	AgcdCasperUser			m_astUsers[AGCM_MAX_USER];

	INT32					m_lMaxUsers;

	CHAR					m_szServerIP[64];
	UINT32					m_ulServerPort;

public:
	AgcmCasper();
	virtual ~AgcmCasper();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32	ulClockCount);
	BOOL	OnPreReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	// Mothods

	BOOL	SetMaxUsers(INT32 lMaxUsers);
	BOOL	ConnectDirectToGameServer();

	INT32	GetEmptyIndex();

	BOOL	SetServerInfo(const CHAR *szIP, UINT32 ulPort);

	VOID	ActionAllUsers();

	BOOL	SendRequestInit(AgcdCasperUser *	pstUser);

	// Actions
	BOOL	MoveCharacter(AgcdCasperUser *	pstUser);
	BOOL	EquipItem(AgcdCasperUser *	pstUser);
	BOOL	AttackCharacter(AgcdCasperUser *	pstUser);
	BOOL	LogoutCharacter(AgcdCasperUser *	pstUser);

	static BOOL	CBSocketOnConnect(PVOID pvData, PVOID pvThis, PVOID pvCustData);
	static BOOL	CBSocketOnDisConnect(PVOID pvData, PVOID pvThis, PVOID pvCustData);
	static BOOL	CBSocketOnError(PVOID pvData, PVOID pvThis, PVOID pvCustData);

	static BOOL	CBInitSuccess(PVOID pvData, PVOID pvThis, PVOID pvCustData);
	static BOOL	CBInitFailure(PVOID pvData, PVOID pvThis, PVOID pvCustData);
};

#endif // !defined(AFX_AGCMCASPER_H__0D97DD43_9A5D_4500_B004_F503087F1D1C__INCLUDED_)
