/*=============================================================================

	AgsmLogin.h

=============================================================================*/

#ifndef _AGSM_LOGIN_SERVER_H_
	#define _AGSM_LOGIN_SERVER_H_


#include "ApBase.h"
#include "AgsEngine.h"
#include "AgpmLogin.h"
#include "AgpmCharacter.h"
#include "AgsmCharacter.h"
#include "Queue.h"




/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGSMLOGIN_FLOW
	{
	AGSMLOGIN_FLOW_CHECK_ACCOUNT = 0,
	AGSMLOGIN_FLOW_SELECT_CHARACTER,
	};


const enum eAGSMLOGIN_OPERATION
	{
	AGSMLOGIN_OPERATION_CREATE_CHARACTER	 = 0,
	AGSMLOGIN_OPERATION_REMOVE_CHARACTER,
	AGSMLOGIN_OPERATION_RENAME_CHARACTER,
	AGSMLOGIN_OPERATION_SELECT_CHARACTER,
	AGSMLOGIN_OPERATION_GET_UNION,
	AGSMLOGIN_OPERATION_GET_CHARACTERS,
	AGSMLOGIN_OPERATION_GET_PREV_ITEM_DBID,
	//AGSMLOGIN_OPERATION_GET_PREV_SKILL_DBID,
	AGSMLOGIN_OPERATION_CHECK_ACCOUNT,
	AGSMLOGIN_OPERATION_GET_EKEY,
	AGSMLOGIN_OPERATION_RESET_LOGINSATUS_BY_DISCONNECT_FROM_GAMESERVER,
	AGSMLOGIN_OPERATION_SET_LOGINSATUS_IN_GAMESERVER,
	AGSMLOGIN_OPERATION_RESET_LOGINSATUS_BY_DISCONNECT_FROM_LOGINSERVER,
	};


const enum eAGSMLOGINSERVER_LOGIN_STATUS
	{
	AGSMLOGINSERVER_LOGIN_STATUS_NONE = 0,
	AGSMLOGINSERVER_LOGIN_STATUS_IN_LOGINSERVER,
	AGSMLOGINSERVER_LOGIN_STATUS_IN_GAMESERVER,
	AGSMLOGINSERVER_LOGIN_STATUS_CONNECT_TO_GAMESERVER,
	AGSMLOGINSERVER_LOGIN_STATUS_DISCONNECT_FROM_SERVER,
	};




/************************************************/
/*		The Definition of Character Info		*/
/************************************************/
//
class AgsmLoginCreatedCharInfo
	{
	public:
		TCHAR			m_szCharName[AGPACHARACTER_MAX_ID_STRING + 1];
		INT32			m_lIndex;

	public:
		AgsmLoginCreatedCharInfo()
			{
			ZeroMemory(m_szCharName, sizeof(m_szCharName));
			m_lIndex = 0;
			}

		~AgsmLoginCreatedCharInfo()
			{
			}
	};




/************************************************/
/*		The Definition of Login Queue Data		*/
/************************************************/
//
class AgsmLoginQueueInfo : public ApMemory<AgsmLoginQueueInfo, 10000>
	{
	public:
		INT8			m_nOperation;
		INT32			m_lCID;
		INT32			m_lTID;
		INT32			m_isLimited;
		INT32			m_isProtected;
		UINT32			m_ulCharNID;
		UINT32			m_ulGameServerNID;

		TCHAR			m_szAccountID[AGPACHARACTER_MAX_ID_STRING + 1];
		TCHAR			m_szPassword[AGPACHARACTER_MAX_PW_STRING + 1];
		TCHAR			m_szServerName[AGPMLOGIN_SERVERGROUPNAMESIZE + 1];
		TCHAR			m_szDBName[AGPMLOGIN_SERVERGROUPNAMESIZE + 1];
		TCHAR			m_szCharacterID[AGPACHARACTER_MAX_ID_STRING + 1];
		TCHAR			m_szIPAddress[AGPMLOGIN_IPADDRSIZE + 1];
		TCHAR			m_szNewCharacterID[AGPACHARACTER_MAX_ID_STRING + 1];
		TCHAR			m_szHanAuthString[2048+1];
#ifdef _WEBZEN_AUTH_
		TCHAR			m_szGBAuthString[2048+1]; //Webzen service authstring
		DWORD			m_dwAccountGUID;
		DWORD			m_dwAccountType;
		DWORD			m_dwPCRoomGuid;
		DWORD			m_dwAge;
		DWORD			m_dwClientCnt; //클라이언트 구동 갯수 PC방의 경우 하나만 빌링에 로그인 시킨다.
#endif

	public:
		AgsmLoginQueueInfo()
			{
			Init();
			}

		~AgsmLoginQueueInfo()
			{
			}
		
		void Init()
			{
			m_lCID = 0;
			m_lTID = 0;
			m_isLimited = 0;
			m_isProtected = 0;
			m_ulCharNID = 0;
			m_ulGameServerNID = 0;

			ZeroMemory(m_szAccountID, sizeof(m_szAccountID));
			ZeroMemory(m_szPassword, sizeof(m_szPassword));
			ZeroMemory(m_szServerName, sizeof(m_szServerName));
			ZeroMemory(m_szDBName, sizeof(m_szDBName));
			ZeroMemory(m_szCharacterID, sizeof(m_szCharacterID));
			ZeroMemory(m_szIPAddress, sizeof(m_szIPAddress));
			ZeroMemory(m_szNewCharacterID, sizeof(m_szNewCharacterID));
			ZeroMemory(m_szHanAuthString, sizeof(m_szHanAuthString));
#ifdef _WEBZEN_AUTH_
			ZeroMemory(m_szGBAuthString, sizeof(m_szGBAuthString));
			m_dwAccountGUID = 0;
			m_dwAccountType = 0;
			m_dwPCRoomGuid	= 0;
			m_dwAge			= 0;
			m_dwClientCnt   = 0;
#endif
			}
	};




/****************************************************/
/*		The Definition of Admin(key=nid) Data		*/
/****************************************************/
//
class AgsmLoginEncryptInfo : public ApMemory<AgsmLoginEncryptInfo, 10000>
	{
	public:
		TCHAR			*m_pszEncryptData;
		INT32			m_lInvalidPasswordCount;
		BOOL			m_bLogin;

		TCHAR			m_szAccountID[AGPACHARACTER_MAX_ID_STRING + 1];
		TCHAR			m_szServerName[AGPMLOGIN_SERVERGROUPNAMESIZE + 1];
		INT32			m_lCID;
		INT32			m_ulNID;
		INT32			m_lFlow;

		AgpdLoginStep	m_eLoginStep;
		
		UINT64			m_ullKey;
		
		BOOL			m_bIsPCBangIP;
		
		INT32			m_lCompenID;
		TCHAR			m_szChar4Compen[AGPACHARACTER_MAX_ID_STRING + 1];

		INT32			m_lAge;
		BOOL			m_bReturnToLogin;

	public:
		AgsmLoginEncryptInfo()
			{
			m_pszEncryptData = NULL;
			m_lInvalidPasswordCount = 0;
			m_bLogin = FALSE;

			ZeroMemory(m_szAccountID, sizeof(m_szAccountID));
			ZeroMemory(m_szServerName, sizeof(m_szServerName));			
			m_lCID = 0;
			m_ulNID = 0;
			m_lFlow = 0;

			m_eLoginStep = AGPMLOGIN_STEP_NONE;
			
			m_ullKey = 0;
			
			m_bIsPCBangIP = FALSE;
			
			m_lCompenID = 0;
			ZeroMemory(m_szChar4Compen, sizeof(m_szChar4Compen));

			m_lAge = 0;
			m_bReturnToLogin = FALSE;
			}

		~AgsmLoginEncryptInfo()
			{
			Cleanup();
			}

		void Cleanup()
			{
			if (NULL != m_pszEncryptData)
				{
				delete [] m_pszEncryptData;
				m_pszEncryptData = NULL;
				}

			m_lAge = 0;
			m_bReturnToLogin = FALSE;
			}
	};




/********************************************************/
/*		The Definition of Login Queue Manager class		*/
/********************************************************/
//
class AgsmLoginServer : public AgsModule
	{
	private:
		AgpmLogin			*m_pAgpmLogin;
		AgpmCharacter		*m_pAgpmCharacter;
		AgsmCharacter		*m_pAgsmCharacter;

	public:
		AgsmLoginServer();
		virtual ~AgsmLoginServer();

		//	ApModule inherited
		BOOL	OnAddModule();

		//	Packet send
		//	request remove ghost/duplicated account
		BOOL	SendRemoveDuplicateAccount(TCHAR *pszAccountID, INT32 lCID, UINT32 ulServerNID);
		//	notify character id is renamed
		BOOL	SendRenameCharacter(TCHAR *pszNewChar, TCHAR *pszOldChar, UINT32 ulServerNID);

	private:
		CQueue<PVOID>	*m_pcQueue;
		UINT32			m_ulMaxQueueCount;

	public:
		//	Queue
		virtual BOOL	CheckQueueCount(); 
		virtual BOOL	CheckAndPushToQueue(PVOID pvQuery, TCHAR *pszFile, INT32 lLine);
		virtual BOOL	PushToQueue(PVOID pvQuery, TCHAR *pszFile, INT32 lLine);
		virtual PVOID	PopFromQueue();
	};


#endif
