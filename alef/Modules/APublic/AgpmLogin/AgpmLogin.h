/*=================================================================================

	AgpmLogin.h

=================================================================================*/

#ifndef	_AGPMLOGIN_H_
	#define _AGPMLOGIN_H_

#include "ApBase.h"
#include "ApDefine.h"
#include "ApModule.h"
#include "AuPacket.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define AGPMLOGIN_MAX_CONNECTION			1000
#define AGPMLOGIN_MAX_CHAR_PER_ACCOUNT		4
#define AGPMLOGIN_MIN_CHARID_SIZE			4
#define	AGPMLOGINDB_DB_MAX_COMPEN_MASTER	10


//
//	==========		Packet size		==========
//
#define	AGPMLOGIN_SERVERGROUPNAMESIZE	32
#define	AGPMLOGIN_IPADDRSIZE			23
#define AGPMLOGIN_EKEYSIZE				16
#define AGPMLOGIN_EKEY_CHALLENGE		5
#define AGPMLOGIN_J_AUTHSTRING			2048	// Hangame 인증 스트링


//
//	==========		Login Operation		==========
//
const enum eAGPMLOGIN_OPERATION
	{
	AGPMLOGIN_ENCRYPT_CODE = 0,
	AGPMLOGIN_SIGN_ON,						// AGPMLOGIN_LOGIN_SUCCEEDED
	AGPMLOGIN_EKEY,							// EKEY
	AGPMLOGIN_UNION_INFO,					// AGPMLOGIN_GET_UNION
	AGPMLOGIN_CHARACTER_NAME,				// AGPMLOGIN_GET_CHARACTERS(S -> C)
	AGPMLOGIN_CHARACTER_NAME_FINISH,		// AGPMLOGIN_GET_CHARACTER_FINISH(S -> C)
	AGPMLOGIN_CHARACTER_INFO,				// AGPMLOGIN_GET_CHARACTERS(C -> S)
	AGPMLOGIN_CHARACTER_INFO_FINISH,		// AGPMLOGIN_SEND_CHARACTER_FINISH
	AGPMLOGIN_ENTER_GAME,					// AGPMLOGIN_SELECT_CHARACTER, AGPMLOGIN_LOGIN_COMPLETE
	AGPMLOGIN_RETURN_TO_SELECT_WORLD,		// AGPMLOGIN_RETURN_TO_SELECT_SERVER
	
	AGPMLOGIN_RACE_BASE,					// AGPMLOGIN_REQUEST_CREATE_CHARACTER_INFO, AGPMLOGIN_RESPONSE_CREATE_CHARACTER_INFO
	AGPMLOGIN_NEW_CHARACTER_NAME,			// AGPMLOGIN_CREATE_CHARACTER
	AGPMLOGIN_NEW_CHARACTER_INFO_FINISH,	// AGPMLOGIN_SEND_CREATE_CHARACTER_FINISH
	AGPMLOGIN_REMOVE_CHARACTER,
	AGPMLOGIN_RENAME_CHARACTER,				// AGPMLOGIN_UPDATE_CHARACTER

	AGPMLOGIN_LOGIN_RESULT,					// common login result code(error)

	AGPMLOGIN_REMOVE_DUPLICATED_ACCOUNT,	// only game server, AGPMLOGIN_REMOVE_DUPLICATE_ACCOUNT,	AGPMLOGIN_REMOVE_DUPLICATE_CHARACTER
	
	AGPMLOGIN_COMPENSATION_INFO,
	AGPMLOGIN_COMPENSATION_CHARACTER_SELECT,
	AGPMLOGIN_COMPENSATION_CHARACTER_CANCEL,

	AGPMLOGIN_REMOVE_DUPLICATED_ACCOUNT_FOR_LOGINSERVER,	//다른 로그인 서버들의 중복 Account를 제거하기 위함 //JK_중복로그인


	AGPMLOGIN_INVALID_CLIENT_VERSION = 32,	// backward compatibility
	};


//
//	==========	Login Result	==========
//
typedef enum
{
	AGPMLOGIN_RESULT_INVALID_ACCOUNT = 0,
	AGPMLOGIN_RESULT_INVALID_PASSWORD,
	AGPMLOGIN_RESULT_INVALID_PASSWORD_LIMIT_EXCEED,
	AGPMLOGIN_RESULT_NOT_APPLIED,
	AGPMLOGIN_RESULT_NOT_ENOUGH_AGE,
	AGPMLOGIN_RESULT_NOT_BETA_TESTER,
	AGPMLOGIN_RESULT_DISCONNECTED_BY_DUPLICATE_ACCOUNT,
	AGPMLOGIN_RESULT_CHAR_NAME_ALREADY_EXIST,
	AGPMLOGIN_RESULT_FULL_SLOT,
	AGPMLOGIN_RESULT_RENAME_SUCCESS,
	AGPMLOGIN_RESULT_RENAME_FAIL,					// unknown fail(DB, server, ...)
	AGPMLOGIN_RESULT_RENAME_SAME_OLD_NEW_ID,		// old and new id are equi.
	AGPMLOGIN_RESULT_UNMAKABLE_CHAR_NAME,
	AGPMLOGIN_RESULT_NEED_EKEY,						// ekey
	
	AGPMLOGIN_RESULT_CANNOT_DISCONNECT_TRY_LATER,
	
	AGPMLOGIN_RESULT_GAMESERVER_NOT_READY,
	AGPMLOGIN_RESULT_GAMESERVER_FULL,
	AGPMLOGIN_RESULT_ACCOUNT_BLOCKED,
	
	AGPMLOGIN_RESULT_CANT_DELETE_CHAR_B4_1DAY,
	AGPMLOGIN_RESULT_CANT_DELETE_CHAR_GUILD_MASTER,
	AGPMLOGIN_RESULT_MIDNIGHTSHUTDOWN_UNDERAGE,		// 샷다운제 시행으로 만 16세 미만 고객은 접속이 제한됩니다.(12AM~6AM)
} AgpdLoginResult;


//
//	==========	Login Step	==========
//
typedef enum
{
	AGPMLOGIN_STEP_NONE				= 0,
	AGPMLOGIN_STEP_CONNECT,
	AGPMLOGIN_STEP_PASSWORD_CHECK_OK,
	AGPMLOGIN_STEP_GET_EXIST_CHARACTER,
	AGPMLOGIN_STEP_CREATE_CHARACTER,
	AGPMLOGIN_STEP_REMOVE_CHARACTER,
	AGPMLOGIN_STEP_REQUEST_SELECT_CHARACTER,
	AGPMLOGIN_STEP_SELECT_CHARACTER,
	AGPMLOGIN_STEP_ENTER_GAME_WORLD,
	AGPMLOGIN_STEP_UNKNOWN,
} AgpdLoginStep;


//
//	==========	Compensation Type	==========
//
const enum eAGPMLOGINDB_COMPEN_TYPE
	{
	AGPMLOGINDB_COMPEN_TYPE_NONE = 0,
	AGPMLOGINDB_COMPEN_TYPE_ALL,
	AGPMLOGINDB_COMPEN_TYPE_ACCOUNT,
	AGPMLOGINDB_COMPEN_TYPE_CHAR,
	AGPMLOGINDB_COMPEN_TYPE_ALLC,
	};



/************************************************/
/*		The Definition of Extra Data class		*/
/************************************************/
//
struct AgpdLoginCompenMaster
	{
	public :
		INT32		m_lCompenID;
		TCHAR		m_szItemDetails[100 + 1];
		TCHAR		m_szDetailTable[50 + 1];
		TCHAR		m_szDescription[200 + 1];
		eAGPMLOGINDB_COMPEN_TYPE	m_eType;
	
	public :
		AgpdLoginCompenMaster()
			{
			Reset();
			}
			
		void Reset()
			{
			m_lCompenID = 0;
			ZeroMemory(m_szItemDetails, sizeof(m_szItemDetails));
			ZeroMemory(m_szDetailTable, sizeof(m_szDetailTable));
			ZeroMemory(m_szDescription, sizeof(m_szDescription));
			m_eType = AGPMLOGINDB_COMPEN_TYPE_NONE;
			}
	};


struct AgpdLoginCompenDetail
	{
	public :
		INT32	m_lCompenID;
		INT64	m_llCompenSeq;
		TCHAR	m_szAccount[AGPACHARACTER_MAX_ID_STRING + 1];
		TCHAR	m_szWorld[AGPMLOGIN_SERVERGROUPNAMESIZE + 1];
		TCHAR	m_szChar[AGPACHARACTER_MAX_ID_STRING + 1];
		TCHAR	m_szItemDetails[100 + 1];
	
	public :
		AgpdLoginCompenDetail()
			{
			Reset();
			}
		
		void Reset()
			{
			m_lCompenID = 0;
			m_llCompenSeq = 0;
			ZeroMemory(m_szAccount, sizeof(m_szAccount));
			ZeroMemory(m_szWorld, sizeof(m_szWorld));
			ZeroMemory(m_szChar, sizeof(m_szChar));
			
			
			ZeroMemory(m_szItemDetails, sizeof(m_szItemDetails));
			}
	};


struct AgpdLoginCompenItem
	{
	public :
		INT32	m_lItemTID;
		INT32	m_lItemQty;
	
	public :
		AgpdLoginCompenItem()
			{
			m_lItemTID = 0;
			m_lItemQty = 0;
			}
	};


struct AgpdLoginCompenItemList
	{
	public :
		INT32	m_lCount;
		ApSafeArray<AgpdLoginCompenItem, AGPMLOGINDB_DB_MAX_COMPEN_MASTER> m_Items;
		
	public :
		AgpdLoginCompenItemList()
			{
			m_lCount = 0;
			m_Items.MemSetAll();
			}
		
		BOOL Parse(CHAR *pszItemDetails);
	};




/************************************************/
/*		The Definition of AgpmLogin class		*/
/************************************************/
//
class AgpmLogin : public ApModule
	{
	public :
		AuPacket	m_csPacket;
		AuPacket	m_csPacketServerAddr;
		AuPacket	m_csPacketCharInfo;
		AuPacket	m_csPacketServerInfo;
		AuPacket	m_csPacketVersionInfo;
		AuPacket	m_csPacketCompenInfo;
		
	public :
		AgpmLogin();
		virtual ~AgpmLogin();
	
		//	ApModule inherited
		BOOL	OnAddModule();
		
		//	Check
		char*	GetNextChar( char* str ) { return (this->*GetNextCharPtr)((unsigned char*)str); }
		BOOL	IsDuplicatedCharacterOfMigration(TCHAR *pszChar);

	private:
		void	InitFuncPtr( void );

		char*	(AgpmLogin::*GetNextCharPtr)(unsigned char* str);
		char*	GetNextCharKr(unsigned char* str);
		char*	GetNextCharEn(unsigned char* str);
		char*	GetNextCharJp(unsigned char* str);
		char*	GetNextCharCn(unsigned char* str);
		
	};



#endif