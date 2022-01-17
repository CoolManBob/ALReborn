/******************************************************************************
Module:  AgcmLoginMultiple.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 3. 18
******************************************************************************/

#ifndef __AGCM_LOGIN_H__
#define __AGCM_LOGIN_H__

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
#include "AgcmCamera.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgcmLoginMultipleD" )
#else
#pragma comment ( lib , "AgcmLoginMultiple" )
#endif

#define AGCMLOGIN_ID_MAX_LENGTH				64
#define	AGCMLOGIN_PASSWORD_MAX_LENGTH		64
/*
enum	// Login 흐름 상황
{
	AGCMLOGIN_LOGIN_MODE = 0,
	AGCMLOGIN_ADDCHARACTER_CHECK,
	AGCMLOGIN_CHARACTER_SELECT,
	AGCMLOGIN_CHARACTER_CREATE,
	AGCMLOGIN_CHARACTER_CREATING,		// Character를 생성한다고 보냈다
	AGCMLOGIN_LOGIN_END_MAIN_GAME
};
*/

// Login Process Mode
enum
{
	AGCMLOGIN_PRE_LOGIN_MODE			=		0	,	// Pre Login Mode
	AGCMLOGIN_ID_PASSWORD_INPUT_MODE				,	
	AGCMLOGIN_UNION_SELECT_MODE						, 
	AGCMLOGIN_CHARACTER_CREATE_MODE					,
	AGCMLOGIN_WAIT_ADD_MY_CHARACTER_MODE			,	// DB에 있는 나의 Character가 모두 Add Character되기를 기다린다 
	AGCMLOGIN_WAIT_ADD_MY_CREATE_CHARACTER_MODE		, 
	AGCMLOGIN_CHARACTER_SELECT_MODE					,
	AGCMLOGIN_POST_LOGIN_MODE						,	// Game Mode
	AGCMLOGIN_SEND_RETURN_TO_LOGIN_SERVER			,	// Exit메뉴( 케릭터 선택 ) - Game Server로 SendReturnToLoginServer 를 보낸 상태 
	AGCMLOGIN_WAIT_DISCONNECT						,	// Exit메뉴( 케릭터 선택 ) - Game Server가 끊어주기를 기다린다 
	AGCMLOGIN_AUTO_LOGIN_PROCESS						// Exit메뉴( 케릭터 선택 ) - Game Server가 강제로 끊어주고 자동으로 Login중이다 
};

enum	// Camera Pos At Status
{
	AGCMLOGIN_CAMERA_HUMAN_SELECT		=		0	,
	AGCMLOGIN_CAMERA_HUMAN_CREATE					,
	AGCMLOGIN_CAMERA_ORC_SELECT						, 
	AGCMLOGIN_CAMERA_ORC_CREATE						
};

// Login 관련 Callback
typedef enum AgcmLoginCallbackPoint
{
	AGCMLOGIN_CB_ID_CONNECT_LOGIN_SERVER_SUCCESS	= 0 ,
	AGCMLOGIN_CB_ID_RECEIVE_DONT_CERTIFY_ID				,		// ID, Password 정보가 잘못 되었다 
	AGCMLOGIN_CB_ID_RECEIVE_GET_UNION					,		// Union을 얻어왔다 
	AGCMLOGIN_CB_ID_RECEIVE_DONT_GET_UNION				,		// Union을 얻지 못했다 -> Union을 선택 해야 한다 
	AGCMLOGIN_CB_ID_RECEIVE_GET_MYCHARACTER_INFO		,		// 나의 계정에 있는 Character에 대한 Index와 Character Name정보를 받았다 
	AGCMLOGIN_CB_ID_LOGIN_PROCESS_END					, 
	AGCMLOGIN_CB_ID_ENCRYPT_CODE_SUCCESS				,		// Encryp Code 를 받는데 성공하다 
	AGCMLOGIN_CB_ID_RECEIVE_ID_FALID					,		// 잘못된 ID이다 
	AGCMLOGIN_CB_ID_RECEIVE_PASSWORD_FALID				,		// Password가 틀렸다 
	AGCMLOGIN_CB_ID_RESPONSE_CREATE_CHARACTER_INFO		,		// Character Create 를 위한 Base Character를 모두 받았다 
	AGCMLOGIN_CB_ID_RECEIVE_REMOVE_MYCHARACTER			,		// My Character 중 Character를 삭제 했다 
	AGCMLOGIN_CB_ID_RECEIVE_CREATE_CHARACTER_INFO		,		// 새로 만든 케릭터의 Info가 날아온다 
	AGCMLOGIN_CB_ID_RESULT_NOT_CREATE_SAME_NAME_CHAR	,		// 같은 이름의 Character가 있어서 생성할 수 없다
	AGCMLOGIN_CB_ID_GET_CHARACTERS_FINISH				,		// 
	AGCMLOGIN_CB_ID_SEND_CHARACTER_FINISH				,
	AGCMLOGIN_CB_ID_SEND_CREATE_CHARACTER_FINISH		,
	AGCMLOGIN_CB_ID_GAME_SERVER_NOT_READY				,
	AGCMLOGIN_CB_ID_RESPONSE_RETURN_TO_LOGINSERVER		,
	AGCMLOGIN_CB_ID_RESULT_CAN_NOT_MAKE_CHAR_NAME		,		// 공백 포함등을 이유로 Character를 생성하지 못했다는 Result Recieve시 
	AGCMLOGIN_CB_ID_RESULT_DISCONNECT_BY_INVALID_PW		,		// 3회이상 패스워드를 틀렸을 경우 등 Disconnect되는 상황
	AGCMLOGIN_CB_ID_RESULT_EXIST_DUPLICATE_ACCOUNT		,		// 이미 로그인 되어 있는 계정이다
	AGCMLOGIN_CB_ID_RESULT_REMOVE_DUPLICATE_ACCOUNT		,		// 이미 로그인 되어 있던 계정을 짤랐다
	AGCMLOGIN_CB_ID_RESULT_CANNOT_REMOVE_DUPLICATE_ACCOUNT,		// 이미 로그인 되어 있는 계정이지만 짜를 수 없다
	AGCMLOGIN_CB_LOGIN_SUCCEEDED
} AgcmLoginCallbackPoint;

class AgcmLoginMultiple : public AgcModule
{
	AuPacket			m_csPacket				;	//Login 패킷.
	AuPacket			m_csServerAddr			;	//Server의 주소가 들어간 정보.
	AuPacket			m_csDetailCharInfo		;	//캐릭생성시 그 캐릭의 상세정보.
	AuPacket			m_csDetailServerInfo	;	//서버군정보

	AgpmCharacter		*m_pcsAgpmCharacter		;
	AgcmCharacter		*m_pcsAgcmCharacter		;
	AgcmConnectManager	*m_pcsAgcmConnectManager;

	char				*m_pstrEncryptCode		;
	AuMD5Encrypt		m_csMD5Encrypt			;
	char				m_pstrIPAddress[23]		;

	AgcWindow *			m_pcsMainWindow			;
	INT32				m_lLoginMode			;

public:
		
	static	BOOL	CBMapLoad( PVOID pData, PVOID pClass, PVOID pCustData					)	;	

	BOOL	RemoveCharacter( char* szName );
	void	SelectCharacter( INT32 lCID, char* pszAccountID, INT16 ulNID);	// Character를 선택했다.
	AgpdCharacter*	GetCharacter( INT32 lCID );

	INT16	LoginToServer();	// Server로 Login한다.
	BOOL	DisconnectLoginServer(INT16 ulNID);
	VOID	SetMainWindow(AgcWindow *pcsWindow);
	char*	GetEncryptCode()	{return m_pstrEncryptCode;	}
	void			SetLoginMode( INT32 lLoginMode )	{ m_lLoginMode = lLoginMode; }
	INT32			GetLoginMode() { return m_lLoginMode;	}

public:
	//아래 데이터는 다 템포러리데이터다.
	INT32				m_lTID;
	HWND				m_hLoginDlg;

	AgcmLoginMultiple();
	~AgcmLoginMultiple();

	INT16 GetLoginServerDPNID();
	BOOL InitRegistry();

	BOOL SendAddToLoginServer(INT16 ulNID);
	BOOL SendSelectServerGroup( char *pstrServerGroupName, INT16 ulNID);
	BOOL SendSelectCharacter( char *pstrServerGroupName, char *pstrAccountID, INT32 lTID, char *pstrCharName, INT16 ulNID);
	BOOL SendCreateCharacter( char *pstrAccountID, char *pstrServerGroupName, INT32 lTID, char *pstrCharName, INT32 lIndex, INT32 lUnion, INT16 ulNID);
	BOOL SendRemoveCharacter( char *pstrAccountID, char *pstrServerGroupName, char *pstrCharName, INT16 ulNID);
	BOOL SendRemoveDuplicateCharacter( char *pstrAccountID, char *pstrServerGroupName, char *pstrCharName, INT16 ulNID);
	BOOL SendGetUnion( char *pstrServerGroupName, char *pstrAccountID, INT16 ulNID);
	BOOL SendGetChars( char *pstrServerGroupName, char *pstrAccountID, INT16 ulNID);
	BOOL SendGetCreateCharacter( char *pstrServerGroupName, char *pstrAccount, char *pstrCharacterName, INT16 ulNID);
	BOOL SendRemoveDuplicateAccount( char *pstrAccountID, INT16 ulNID);
	BOOL SendRequestRaceCharacterInfo( char *pstrAccountID, INT32 lRace, INT16 ulNID);
	
	BOOL SetIDPassword( char *pstrID, char *pstrPassword);
	BOOL SendAccount( char *pstrAccountID, char *pstrAccountPassword, INT16 ulNID );

	BOOL ProcessUnionInfo( INT32 *plUnion, void *pvCharDetailInfo, UINT16 ulNID);
	BOOL ProcessLoginComplete( INT32 lCRequesterID, void *pvCharDetailInfo, void *pvDetailServerInfo, UINT16 ulNID);
	BOOL ProcessRegisterChar( char **ppstrCharName, INT32 *plRegisterCharCount, INT32 *plSlotIndex, void *pvCharDetailInfo, UINT16 ulNID);
	BOOL ProcessRemoveChar( void *pvCharDetailInfo, UINT16 ulNID);
	BOOL ProcessLoginResult( void *pvCharDetailInfo, INT32 lResult, INT16 ulNID );

	BOOL SetCallbackConnectLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass )				;
	BOOL SetCallbackLoginEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass)							;
	BOOL SetCallbackReceiveDontCertifyID(ApModuleDefaultCallBack pfCallback, PVOID pClass )				;
	BOOL SetCallbackReceiveGetUnion(ApModuleDefaultCallBack pfCallback, PVOID pClass)					;
	BOOL SetCallbackReceiveDontGetUnion(ApModuleDefaultCallBack pfCallback, PVOID pClass)				;
	BOOL SetCallbackReceiveGetMyCharacterInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)			;
	BOOL SetCallbackEncryptCodeSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass)				;
	BOOL SetCallbackReceiveIdFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass)					;
	BOOL SetCallbackReceivePasswordFailed(ApModuleDefaultCallBack pfCalllback, PVOID pClass)			;
	BOOL SetCallbackResponseCreateCharacterInfo(ApModuleDefaultCallBack pfCallback,PVOID pClass)		;
	BOOL SetCallbackReceiveRemoveMyCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)			;
	BOOL SetCallbackReceiveGetCreateCharacterInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)		;
	BOOL SetCallbackResultNotCreateSameNameCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)	;
	BOOL SetCallbackGetCharactersFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass)				;
	BOOL SetCallbackSendCharacterFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass)				;
	BOOL SetCallbackSendCreateCharacterFinish(ApModuleDefaultCallBack pfCallback, PVOID pClass)			;
	BOOL SetCallbackGameServerNotReady(ApModuleDefaultCallBack pfCallback, PVOID pClass)				;
	BOOL SetCallbackResponseReturnToLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass )		;
	BOOL SetCallbackResultCanNotMakeCharName(ApModuleDefaultCallBack pfCallback, PVOID pClass )			;
	BOOL SetCallbackResultDisconnectByInvaludPassword(ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL SetCallbackResultExistDuplicateAccount(ApModuleDefaultCallBack pfCallback, PVOID pClass )		;
	BOOL SetCallbackResultRemoveDuplicateAccount(ApModuleDefaultCallBack pfCallback, PVOID pClass )	;
	BOOL SetCallbackResultCannotRemoveDuplicateAccount(ApModuleDefaultCallBack pfCallback, PVOID pClass );

	BOOL SetCallbackReceiveLoginSucceeded(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	//어디에나 있는 OnAddModule이다. 특별할거 없다. 특별하면 안되나? 아햏햏~
	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	//패킷을 받고 파싱하자~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);
	
	// Socket관련 Callback
	static	BOOL	CBSocketOnConnect( PVOID pData, PVOID pClass, PVOID pCustData				)	;
	static	BOOL	CBSocketOnDisConnect( PVOID pData, PVOID pClass, PVOID pCustData			)	;
	static	BOOL	CBSocketOnError( PVOID pData, PVOID pClass, PVOID pCustData					)	;
};

#endif