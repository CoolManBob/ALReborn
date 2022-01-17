/*===========================================================================

	AgsmLoginClient.h

===========================================================================*/

#ifndef _AGSM_LOGIN_CLIENT_H_
	#define _AGSM_LOGIN_CLIENT_H_

#include "ApBase.h"
#include "ApDefine.h"
//#include "AuOLEDB.h"
#include "AuExcelTxtLib.h"
#include "AuMD5Encrypt.h"
#include "AgsEngine.h"
#include "AgpmLogin.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgsmAccountManager.h"
//#include "AgsmDBStream.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
//#include "AgsmServerManager.h"
#include "AgsmServerManager2.h"
#include "AgsmLoginServer.h"
#include "AgpmResourceInfo.h"
#include "AgsmFactors.h"
#include "AgpmLogin.h"
#include "AgsmGKforPCRoom.h"
#include "AgpmEventCharCustomize.h"
#include "AgpmStartupEncryption.h"
//#include "AgsmLoginBst.h"

#include "ApSet.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
typedef enum AgsmLoginDataType
	{
	AGSMLOGIN_DATA_TYPE_BASE_CHAR_POS			= 0,
	} AgsmLoginDataType;

#define	AGSMLOGIN_INI_RACE_NAME					"Race"
#define	AGSMLOGIN_INI_POSITION					"Position"
#define	AGSMLOGIN_INI_DEGREEX					"DegreeX"
#define AGSMLOGIN_INI_DEGREEY					"DegreeY"

///////////////////////////////////////////////////////////////////
const INT32 g_lMaxProcessUserCount				= 2500;
const INT32	g_lMaxCreateProcessUserCount		= 3200;


typedef struct _AgsmBaseCharacterPos
	{
	AuPOS		stPos;
	FLOAT		fDegreeX;
	FLOAT		fDegreeY;
	} AgsmBaseCharacterPos;




/****************************************************/
/*		The Definition of AgsmLoginClient Class		*/
/****************************************************/
//
class AgsmLoginClient : public AgsModule
	{
	private:
		//	Related modules
		AgpmLogin			*m_pAgpmLogin;
		AgpmFactors			*m_pAgpmFactors;
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmItem			*m_pAgpmItem;
		AgpmAdmin			*m_pAgpmAdmin;
		AgsmAccountManager	*m_pAgsmAccountManager;
		//AgsmDBStream		*m_pAgsmDBStream;
		AgsmCharacter		*m_pAgsmCharacter;
		AgsmCharManager		*m_pAgsmCharManager;
		AgsmItem			*m_pAgsmItem;
		AgsmItemManager		*m_pAgsmItemManager;
		AgsmServerManager	*m_pAgsmServerManager;
		AgsmLoginServer		*m_pAgsmLoginServer;
		AgpmResourceInfo	*m_pAgpmResourceInfo;
		AgsmFactors			*m_pAgsmFactors;
		AgsmGK				*m_pAgsmGK;
		AgpmEventCharCustomize	*m_pAgpmEventCharCustomize;
		AgpmStartupEncryption	*m_pAgpmStartupEncryption;
		AgpmConfig			*m_pAgpmConfig;//JK_심야샷다운

		//	Admin
		ApAdmin				m_csCertificatedNID;

		//	enc/dec
		AuMD5Encrypt		m_csMD5Encrypt;

		vector<string>		m_vstrBanWord;			// 금지 단어
		//ApSet<string>			m_csBanWord;			// 금지 단어
		//ApSet<string>			m_csContainBanWord;		// 금지 단어를 포함

		INT32					m_lRaceBaseCharacter[AURACE_TYPE_MAX][AUCHARCLASS_TYPE_MAX * 3];
		AgsmBaseCharacterPos	m_RaceBaseCharacterPos[AURACE_TYPE_MAX][AUCHARCLASS_TYPE_MAX * 3];

	public:
		INT32				m_lMaxProcessUserCount;
		INT32				m_lMaxCreateProcessUserCount;

	public:
		AgsmLoginClient();
		~AgsmLoginClient();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

		//	EncryptInfo Admin(NID to AccountID)
		AgsmLoginEncryptInfo*		GetCertificatedNID(UINT32 ulNID);
		BOOL						IsCertificatedNID(UINT32 ulNID);
		BOOL						SetCertificatedNID(UINT32 ulNID, CHAR *pszAccountID, UINT64 ullKey);
		BOOL						SetLoginStep(UINT32 ulNID, AgpdLoginStep eLoginStep);
		AgpdLoginStep				GetLoginStep(UINT32 ulNID);

		//	Operations
		//	login
		BOOL	ProcessEncryptCode(PVOID pvPacketVersionInfo, UINT32 ulNID, BOOL bCheckVersion = TRUE);
		BOOL	ProcessSignOn(CHAR *pszAccount, INT8 cAccountLen, CHAR *pszPassword, INT8 cPasswordLen, UINT32 ulNID, CHAR* pszExtraForForeign = NULL);
		BOOL	ProcessEKey(CHAR* pszEKey, UINT32 ulNID);
		BOOL	ProcessGetUnion(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, UINT32 ulNID);
		BOOL	ProcessGetCharacters(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, UINT32 ulNID);
		BOOL	ProcessSelectCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID, INT lMemberBillingNum);
		BOOL	ProcessReturnToSelectWorld(CHAR *pszAccountID, UINT32 ulNID);
		//	create
		BOOL	ProcessBaseCharacterOfRace(PVOID pvCharDetailInfo, UINT32 ulNID);
		BOOL	ProcessCreateCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID);
		//	rename
		BOOL	ProcessRenameCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID);
		//	remove
		//BOOL	ProcessRemoveCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID);
		BOOL	ProcessRemoveCharacter(CHAR *pszAccountID, CHAR *pszPassword,  CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID);
		//	compensation
		BOOL	ProcessCharacter4Compensation(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID, BOOL bSelect);

		//	Callbacks
		static BOOL CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData);
		BOOL		OnDisconnect(INT32 lCID, UINT32 ulNID);
		static BOOL	CBReceiveNewCID(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCompleteSendCharacterInfo(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Stream
		BOOL	LoadCharNameFromExcel(CHAR *pszFile);
		BOOL	StreamReadBaseCharPos(CHAR *pszFile);
		static	BOOL BaseCharacterPosReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

		//	ETC
		BOOL	AddInvalidPasswordLog(UINT32 ulNID);
		BOOL	CheckCharacterName(CHAR *pszCharName);
		BOOL	SearchContainBanWord(CHAR* pszCharName);
		BOOL	CreateBaseCharacterOfRace(INT8 cRace);
		BOOL	IsValidLoginStep(CHAR *pszAccountID, AgpdLoginStep eLoginStep);
		BOOL	IsValidLoginStep(UINT32 ulNID, AgpdLoginStep eLoginStep);

		BOOL	SetAccountAge(UINT32 ulNID, CHAR* szAccountID, INT32 lAge);
		INT32	GetAccountAge(UINT32 ulNID, CHAR* szAccountID);

		//	Packet
		//	login
		BOOL	SendEncryptCode(CHAR *pszEncryptCode, UINT32 ulNID);
		BOOL	SendInvalidClientVersion(UINT32 ulNID);
		BOOL	SendSignOnSuccess(UINT32 ulNID, CHAR* pszAccountID, int isLimited, int isProtected);
		BOOL	SendUnionInfo(INT32 lUnion, UINT32 ulNID);
		BOOL	SendCharacterName(CHAR *pszCharName, INT32 lMaxChars, INT32 lIndex, UINT32 ulNID);
		BOOL	SendCharacterNameFinish(CHAR *pszAccountID, UINT32 ulNID);
		BOOL	SendCharacterInfoFinish(UINT32 lCID, CHAR *pszAccountID, UINT32 ulNID);
		BOOL	SendEnterGameEnd(CHAR *pszAccountID, INT32 lCID, INT32 lTID, CHAR *pszCharName, CHAR *pszIP, UINT32 ulNID);
		//	create
		BOOL	SendBaseCharacterOfRace(INT8 cRace, INT32 lNumChar, UINT32 ulNID);
		BOOL	SendNewCharacterName(CHAR *pszCharName, INT32 lCharCount, INT32 lSlotIndex, UINT32 ulNID); //SendAddCreatedChar
		BOOL	SendNewCharacterInfoFinish(INT32 lCID, UINT32 ulNID);
		//	remove
		BOOL	SendCharacterRemoved(CHAR *pszAccountID, CHAR *pszCharName, UINT32 ulNID);
		//	common result
		BOOL	SendLoginResult(INT32 lResultCode, CHAR *pszCharName, UINT32 ulNID);
		// for ekey
		BOOL	SendEKeyChallengeNum(const char* pszChallengeNum, UINT32 ulNID);
		//	compensation
		BOOL	SendCompensationExist(CHAR *pszAccount, INT32 lCompenID, INT32 lCompenType, CHAR *pszCharID, CHAR *pszItemDetails, CHAR *pszDescription, UINT32 ulNID);

		//	Max User
		INT32	GetMaxProcessUserCount();
		BOOL	SetMaxProcessUserCount(INT32 lMaxProcessUserCount);
		INT32	GetMaxCreateProcessUserCount();
		BOOL	SetMaxCreateProcessUserCount(INT32 lMaxCreateProcessUserCount);

		BOOL	IsReturnToLogin(UINT32 ulNID);
	};


#endif
