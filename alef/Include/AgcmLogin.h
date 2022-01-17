#pragma once

#include "ApBase.h"
#include "AuMD5Encrypt.h"
#include "AgcModule.h"
#include "ApBase.h"
#include "AgpmLogin.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgcmConnectManager.h"
#include "AgcEngine.h"
#include "AgcmRender.h"
#include "AgcmMap.h"
#include "AgpmWorld.h"
#include "AgpmResourceInfo.h"

#define AGCMLOGIN_ID_MAX_LENGTH				64
#define	AGCMLOGIN_PASSWORD_MAX_LENGTH		64

const enum eAGCMLOGIN_MODE
{
	AGCMLOGIN_MODE_PRE_LOGIN = 0,				// Pre Login Mode
	AGCMLOGIN_MODE_ID_PASSWORD_INPUT,	
	AGCMLOGIN_MODE_UNION_SELECT,
	AGCMLOGIN_MODE_SERVER_SELECT,
	AGCMLOGIN_MODE_CHARACTER_CREATE,
	AGCMLOGIN_MODE_WAIT_MY_CHARACTER,			// waiting all of my characters to be add
	AGCMLOGIN_MODE_WAIT_MY_NEW_CHARACTER,		// waiting my characters that newly created to be add
	AGCMLOGIN_MODE_CHARACTER_SELECT,
	AGCMLOGIN_MODE_POST_LOGIN_LOADING,
	AGCMLOGIN_MODE_POST_LOGIN,					// Game Mode
	AGCMLOGIN_MODE_WAIT_DISCONNECT,				// from game server
};

const enum eAGCMLOGIN_CB
{
	AGCMLOGIN_CB_CONNECT = 0,				// AGCMLOGIN_CB_ID_CONNECT_LOGIN_SERVER_SUCCESS
	AGCMLOGIN_CB_DISCONNECT,				// AGCMLOGIN_CB_ID_DISCONNECT_LOGIN_SERVER

	AGCMLOGIN_CB_ENCRYPT_CODE,				// AGCMLOGIN_CB_ID_ENCRYPT_CODE_SUCCESS
	AGCMLOGIN_CB_INVALID_CLIENT_VERSION,	// AGCMLOGIN_CB_ID_RESULT_INVALID_CLIENT_VERSION
	AGCMLOGIN_CB_SIGN_ON,
	AGCMLOGIN_CB_UNION_INFO,				// AGCMLOGIN_CB_ID_RECEIVE_GET_UNION, AGCMLOGIN_CB_ID_RECEIVE_DONT_GET_UNION
	AGCMLOGIN_CB_CHARACTER_NAME,			// AGCMLOGIN_CB_ID_RECEIVE_GET_MYCHARACTER_INFO
	AGCMLOGIN_CB_CHARACTER_NAME_FINISH,		// AGCMLOGIN_CB_ID_GET_CHARACTERS_FINISH
	AGCMLOGIN_CB_CHARACTER_INFO_FINISH,		// AGCMLOGIN_CB_ID_SEND_CHARACTER_FINISH
	AGCMLOGIN_CB_ENTER_GAME,				// AGCMLOGIN_CB_ID_LOGIN_PROCESS_END

	AGCMLOGIN_CB_RACE_BASE,					// AGCMLOGIN_CB_ID_RESPONSE_CREATE_CHARACTER_INFO
	AGCMLOGIN_CB_NEW_CHARACTER_NAME,		// AGCMLOGIN_CB_ID_RECEIVE_CREATE_CHARACTER_INFO
	AGCMLOGIN_CB_NEW_CHARACTER_INFO_FINISH,	// AGCMLOGIN_CB_ID_SEND_CREATE_CHARACTER_FINISH
	AGCMLOGIN_CB_REMOVE_CHARACTER,			// AGCMLOGIN_CB_ID_RECEIVE_REMOVE_MYCHARACTER

	AGCMLOGIN_CB_LOGIN_RESULT,
	AGCMLOGIN_CB_EKEY_ACTIVE,

	AGCMLOGIN_CB_COMPENSATION_INFO,
};

class CRequestConnect
{
public:
	CRequestConnect() : m_ulRequestClock(0), m_bRequestConnect(FALSE)	{	}

	void	Start( UINT32 ulClock )	{
		m_ulRequestClock	= ulClock;
		m_bRequestConnect	= TRUE;
	}
	void	Stop()					{
		m_ulRequestClock	= 0;
		m_bRequestConnect	= FALSE;
	}
	BOOL	Update( UINT32 ulClock )	{
		if( !m_bRequestConnect )	return FALSE;

		if( ulClock - m_ulRequestClock > s_ulRequestWaitClock )
			return TRUE;

		m_ulRequestClock = ulClock;

		return FALSE;
	}

private:
	static const UINT32 s_ulRequestWaitClock = 1000 * 60;

	UINT32	m_ulRequestClock;
	BOOL	m_bRequestConnect;
};

struct AgcdLoginCompenMaster : public AgpdLoginCompenMaster
{
	AgcdLoginCompenMaster()
	{
		ClearCharID();
		ZeroMemory(m_szSelectedCharID, sizeof(m_szSelectedCharID));
	}

	void ClearCharID()
	{
		ZeroMemory(m_szCharID, sizeof(m_szCharID));
		ZeroMemory(m_szCharID, sizeof(m_szCharID1));
		ZeroMemory(m_szCharID, sizeof(m_szCharID2));
	}

	TCHAR	m_szCharID[AGPACHARACTER_MAX_ID_STRING + 1];
	TCHAR	m_szCharID1[AGPACHARACTER_MAX_ID_STRING + 1];
	TCHAR	m_szCharID2[AGPACHARACTER_MAX_ID_STRING + 1];
	TCHAR	m_szSelectedCharID[AGPACHARACTER_MAX_ID_STRING + 1];
};


class AgcmLogin : public AgcModule
{
public:
	AgcmLogin();
	virtual ~AgcmLogin();

	//	ApModule inherited
	BOOL	OnAddModule();
	BOOL	OnIdle(UINT32 ulClockCount);
	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);	

	//	Login Operations
	//	request
	INT16	ConnectLoginServer();
	INT16	ConnectLoginServer( CONST CHAR*	szIpAddress);

	BOOL	DisconnectLoginServer();
	void	SelectCharacter(INT32 lCID , BOOL &bRename);
	BOOL	RenameCharacter(INT32 lCID, CHAR *pszNewCharID);
	//	result
	BOOL	OnUnionInfo(PVOID pvCharDetailInfo);
	BOOL	OnCharacterName(PVOID pvCharDetailInfo, BOOL bNew = FALSE);
	BOOL	OnEnterGame(INT32 lCRequesterID, PVOID pvCharDetailInfo, PVOID pvDetailServerInfo);
	BOOL	OnRemoveCharacter(PVOID pvCharDetailInfo);
	BOOL	OnLoginResult(PVOID pvCharDetailInfo, INT32 lResult, INT32 ulNID);
	BOOL	OnCompensationInfo(TCHAR *pszAccount, PVOID pvCharDetailInfo);

	//	Helper
	BOOL	InitRegistry();
	VOID	SetMainWindow(AgcWindow *pcsWindow);
	BOOL	SetIDPassword(TCHAR *pszID, TCHAR *pszPassword);
	TCHAR*	GetEncryptCode()					{ return m_szEncryptCode; }
	INT16	GetNID()							{ return m_nNID; }
	void	SetLoginMode(INT32 lLoginMode)		{ m_lLoginMode = lLoginMode; }
	INT32	GetLoginMode()						{ return m_lLoginMode; }

	//	Packet
	BOOL	SendGetEncryptCode();
	BOOL	SendAccount(TCHAR *pszAccount, TCHAR *pszPassword, INT16 nNID);
	BOOL	SendEKey(char* pszEKey, INT16 nNID);
	BOOL	SendGetUnion(TCHAR *pszWorld, TCHAR *pszAccount);
	BOOL	SendGetCharacters(TCHAR *pszWorld, TCHAR *pszAccount);
	BOOL	SendEnterGame(TCHAR *pszWorld, TCHAR *pszAccount, INT32 lTID, TCHAR *pszCharID);

	BOOL	SendGetBaseCharacterOfRace(TCHAR *pszAccount, INT32 lRace);
	BOOL	SendCreateCharacter(TCHAR *pszAccount, TCHAR *pszWorld, INT32 lTID, TCHAR *pszCharID, INT32 lIndex, INT32 lUnion, INT32 lHairIndex, INT32 lFaceIndex);
	BOOL	SendRemoveCharacter(TCHAR *pszAccount, TCHAR *pszWorld, TCHAR *pszCharID, TCHAR *pszPassword = NULL);
	BOOL	SendRenameCharacter(TCHAR *pszWorld, TCHAR *pszAccount, INT32 lTID, TCHAR *pszCharID, TCHAR *pszNewCharID);

	BOOL	SendReturnToSelectServer(TCHAR *pszAccount);
	BOOL	SendCharacter4Compensation(TCHAR *pszAccount, TCHAR *pszWorld, TCHAR *pszCharID, BOOL bSelect);

	//	Callback
	static BOOL	CBSocketConnect(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSocketDisConnect(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSocketError(PVOID pData, PVOID pClass, PVOID pCustData);

	//	Callback Setting
	BOOL	SetCallbackConnectLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDisconnectLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackEncryptCodeSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackInvalidClientVersion(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackSignOnSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUnionInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackCharacterName(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackCharacterNameFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackCharacterInfoFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackEnterGameEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackRaceBase(ApModuleDefaultCallBack pfCallback,PVOID pClass);
	BOOL	SetCallbackNewCharacterName(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackNewCharacterInfoFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackLoginResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackEKeyActive(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackCompensationInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	VOID	SetLoginServer(TCHAR *szAddress);

private :
	BOOL	_IsExistLoginSelectFile( void );

public:
	AgpmLogin*			m_pAgpmLogin;
	AgpmCharacter*		m_pAgpmCharacter;
	AgcmCharacter*		m_pAgcmCharacter;
	AgcmConnectManager*	m_pAgcmConnectManager;
	AgpmResourceInfo*	m_pAgpmResourceInfo;

	AgcWindow*			m_pMainWindow;

	AuMD5Encrypt		m_csMD5Encrypt;

	//	My
	TCHAR				m_szEncryptCode[ENCRYPT_STRING_SIZE];
	TCHAR				m_szIPAddress[64];
	INT32				m_lLoginMode;
	BOOL				m_bIsConnectedLoginServer;
	TCHAR				m_szWorldName[AGPDWORLD_MAX_WORLD_NAME];

	INT16				m_nNID;
	INT32				m_lTotalCharacter;

	INT32				m_lIsLimited;
	INT32				m_lIsProtected;

	//	Temp
	TCHAR				m_szAccount[AGCMLOGIN_ID_MAX_LENGTH];
	TCHAR				m_szPassword[AGCMLOGIN_PASSWORD_MAX_LENGTH];
	TCHAR				m_szConfirmPassword[AGCMLOGIN_PASSWORD_MAX_LENGTH];
	INT32				m_lTID;
	HWND				m_hLoginDlg;

	BOOL				m_bCustomLoginInfo;
	long				m_lCharacterSelected;

	CRequestConnect		m_cRequestConnect;
};