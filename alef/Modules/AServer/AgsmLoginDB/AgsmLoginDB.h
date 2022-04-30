/*============================================================================

	AgsmLoginDB.h

******************************************************************************/


#ifndef _AGSM_LOGIN_DB_H_
	#define _AGSM_LOGIN_DB_H_

#include "ApDefine.h"
#include "AgsEngine.h"
#include "AuDatabase2.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AgpmAdmin.h"
#include "AgsmAccountManager.h"
#include "AgsmServerManager2.h"
#include "AgsmFactors.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmSkill.h"
#include "AgsmQuest.h"
#include "AgsmSkillManager.h"
#include "AgsmLoginClient.h"
#include "AgsmLoginServer.h"
#include "AgsmUIStatus.h"
#include "AgsmEventSkillMaster.h"
#include "AgsmAuction.h"
#include "AgsmGuild.h"
#include "AgsmGKforPCRoom.h"
#include "zzThread.h"
#include <wininet.h>
#include "AgpmPvP.h"
#include "AgpmWorld.h"
#include "AgsmWorld.h"
#include "AgpmConfig.h"
#include "AgsmReturnToLogin.h"

#ifdef _AREA_WESTERN_
#include "AgsmCogpas.h"
#endif

#pragma comment(lib, "wininet")


class AgsmLoginDB;
class AgsmTitle;

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGSMLOGINDB_AUTH_RESULT
	{
	AGSMLOGINDB_AUTH_RESULT_UNKNOWN = 0,
	AGSMLOGINDB_AUTH_RESULT_NOT_MEMBER = 1,		// 회원이 아님
	AGSMLOGINDB_AUTH_RESULT_XXX = 2,
	AGSMLOGINDB_AUTH_RESULT_PWD_FAIL = 3,		// 패스워드 오류
	AGSMLOGINDB_AUTH_RESULT_FOREIGN = 4,		// 대기 상태의 외국인
	AGSMLOGINDB_AUTH_RESULT_NEED_NAME = 5,		// 실명전환이 필요한 회원
	AGSMLOGINDB_AUTH_RESULT_IN_PENALTY = 6,		// 경고
	AGSMLOGINDB_AUTH_RESULT_IN_STOP = 7,		// 이용정지
	AGSMLOGINDB_AUTH_RESULT_ABUSE_MAIL = 8,		// e-mail 도용
	AGSMLOGINDB_AUTH_RESULT_PERM_STOP = 9,		// 영구 이용 정지
	AGSMLOGINDB_AUTH_RESULT_NEED_PA = 10,		// 부모 동의가 필요한 회원
	AGSMLOGINDB_AUTH_RESULT_SUCCESS = 11,		// 성공
	AGSMLOGINDB_AUTH_RESULT_BLOCKED_IP = 12,	// 블록된 IP에서 접근
	AGSMLOGINDB_AUTH_RESULT_NOT_CONNECT = 13,	// 로그인 디비에 접근할수 없음
	AGSMLOGINDB_AUTH_RESULT_WAIT = 14,			// 로그인 디비에 쿼리를 보내고 기다리고 있음
	};


const enum eAGSMLOGINDB_RENAME_RESULT
	{
	AGSMLOGINDB_RENAME_RESULT_SUCCESS = 0,
	AGSMLOGINDB_RENAME_RESULT_FAIL,					// update error
	AGSMLOGINDB_RENAME_RESULT_SAME_OLD_NEW_ID,		// old and new id are equi.
	AGSMLOGINDB_RENAME_RESULT_NOT_EXIST_ID,			// old id isn't exist
	AGSMLOGINDB_RENAME_RESULT_DUP_NEW_ID,				// new id already exist(duplicated)
	};


#define	AGSMLOGINDB_DB_WAIT_TIME		INFINITE


/************************************************/
/*		The Definition of HTTP Helper class		*/
/************************************************/
//
class ApHttp
	{
	private:
		enum {URLBufferSize = 256, ResultBufferSize = 20000};
		HINTERNET m_hOpen;
		HINTERNET m_hHttp;

		TCHAR	m_URLBuffer[URLBufferSize];
		TCHAR	m_Result[ResultBufferSize];
		DWORD	m_dwReceiveBytes;

	public:
		ApHttp();
		~ApHttp();
		BOOL Open();
		BOOL IsOpen();
		INT32 SendRequest(const TCHAR *pszName, const TCHAR *pszPassword);
		void Close();
	};

/********************************************/
/*		The Definition of Worker class		*/
/********************************************/
//
class AgsmLoginWorker : public zzThread
	{
	public :
		AgsmLoginDB				*m_pThis;

#ifdef _AREA_KOREA_
		ApHttp					m_Http;
#endif
		BOOL					m_bStop;
		INT16					m_nIndex;
	
	public:
		AgsmLoginWorker();
		~AgsmLoginWorker();

		void	Set(AgsmLoginDB *pThis, INT16 nIndex);
		void	Stop();
		DWORD	Do();
	};




/****************************************************/
/*		The Definition of Game DB Group class		*/
/****************************************************/
//
class AgsmLoginGameDBGroup
	{
	private:
		AuDatabaseManager	*m_pManager;
		AuDatabase2			*m_pDatabases;
		INT16				m_nConnection;
		
		zzWaitableCollection	m_Semaphores;
	
	public:
		AgsmLoginGameDBGroup();
		virtual ~AgsmLoginGameDBGroup();
		
		BOOL			Connect(AuDatabaseManager *pManager, TCHAR *pszUser, TCHAR *pszPwd, TCHAR *pszName, INT16 nConnection);
		AuDatabase2*	GetIdle();
	};




/************************************************/
/*		The Definition of Extra Data class		*/
/************************************************/
//
struct AgsdLoginExpeditionList
	{
	public :
		TCHAR	m_szWorldName[AGPDWORLD_MAX_WORLD_NAME + 1];
		TCHAR	m_szCharName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
		
	public :
		AgsdLoginExpeditionList()
			{
			ZeroMemory(m_szWorldName, sizeof(m_szWorldName));
			ZeroMemory(m_szCharName, sizeof(m_szCharName));
			}
	};


struct AgsdLoginCoupon
	{
	public :
		INT32		m_lSeqMaster;
		TCHAR		m_szCouponNo[25 + 1];
		TCHAR		m_szCompen[50 + 1];
	
	public :
		AgsdLoginCoupon()
			{
			m_lSeqMaster = 0;
			ZeroMemory(m_szCouponNo, sizeof(m_szCouponNo));
			ZeroMemory(m_szCompen, sizeof(m_szCompen));
			}
	};


struct AgsdCouponItem
	{
	public :
		INT32	m_lItemTID;
		INT32	m_lItemQty;
	
	public :
		AgsdCouponItem()
			{
			m_lItemTID = 0;
			m_lItemQty = 0;
			}
	};


#ifdef _AREA_CHINA_

#include "PT/SDKStuff.h"

class AgsmClientInterface : public CClientInterface
{
	public:
		AgsmClientInterface();
		~AgsmClientInterface();

	public:
		bool InitPT();

		void QuestEKey(CSessionPtr& p_session, const char* challegeNum);
		void OperateResult(CSessionPtr& p_session, int operation, int result);
		void RecvDownMessage(CSDMessage& v_msg) {}

	public:
		CAccountServerPtr m_pAS;

		AgsmLoginDB		*m_pAgsmLoginDB;
		AgsmLoginClient	*m_pAgsmLoginClient;
};
#endif

/************************************************/
/*		The Definition of AgsmLoginDB class		*/
/************************************************/
//
class AgsmLoginDB : public AgsModule
	{
	friend class AgsmLoginWorker;

	public:

	private:
		// Worker
		AgsmLoginWorker			*m_pWorkers;
		INT16					m_nWorker;

		// 영문 인증
#ifdef _AREA_WESTERN_
		//CCogpas	*m_pCogpas;
#endif

		//	Database
		ApAdmin					m_Map;			// <Game DB Name, Game DB Collection>
		AuDatabaseManager		m_DatabaseManagerOracle;
		AuDatabaseManager		m_DatabaseManagerMSSQL;
		AuDatabase2				*m_pMasterDatabases;
		AuDatabase2				*m_pCompenDatabases;
		zzWaitableCollection	m_MasterSema;
		zzWaitableCollection	m_CompenSema;
		eAUDB_VENDER			m_eMasterVender;
		
		//	Query Map
		ApAdmin					m_QueryMap[AUDB_VENDER_MAX];

		// Related Modules
		AgpmLogin				*m_pAgpmLogin;
		AgpmGrid				*m_pAgpmGrid;
		AgpmFactors				*m_pAgpmFactors;
		AgpmCharacter			*m_pAgpmCharacter;
		AgpmItem				*m_pAgpmItem;
		AgpmSkill				*m_pAgpmSkill;
		AgpmAdmin				*m_pAgpmAdmin;
		AgpmUIStatus			*m_pAgpmUIStatus;
		AgsmAccountManager		*m_pAgsmAccountManager;
		AgsmServerManager		*m_pAgsmServerManager;
		AgsmFactors				*m_pAgsmFactors;
		AgsmCharacter			*m_pAgsmCharacter;
		AgsmCharManager			*m_pAgsmCharManager;
		AgsmItem				*m_pAgsmItem;
		AgsmItemManager			*m_pAgsmItemManager;
		AgsmItemConvert			*m_pAgsmItemConvert;
		AgsmSkill				*m_pAgsmSkill;
		AgsmQuest				*m_pAgsmQuest;
		AgsmSkillManager		*m_pAgsmSkillManager;
		AgsmLoginServer			*m_pAgsmLoginServer;
		AgsmLoginClient			*m_pAgsmLoginClient;
		AgsmUIStatus			*m_pAgsmUIStatus;
		AgsmEventSkillMaster	*m_pAgsmEventSkillMaster;
		AgsmAuction				*m_pAgsmAuction;
		AgsmGuild				*m_pAgsmGuild;
		AgsmGK					*m_pAgsmGK;
		AgpmPvP					*m_pAgpmPvP;
		AgpmWorld				*m_pAgpmWorld;
		AgsmWorld				*m_pAgsmWorld;
		AgpmConfig				*m_pAgpmConfig;
		AgsmReturnToLogin		*m_pAgsmReturnToLogin;
		AgsmTitle				*m_pAgsmTitle;
		AgsmLogin				*m_pAgsmLogin;

		// Login Key
		AuGenerateID64			m_csGenerateID64;

		// Expedition
		AgsdLoginExpeditionList	*m_pAgsdExpeditionList;
		INT32	m_lNumExpeditionList;

	public:
		AgsmLoginDB();
		virtual ~AgsmLoginDB();

		//	ApModule inherited
		BOOL OnAddModule();
		BOOL OnInit();

		//	Worker(Thread)
		BOOL	StartWorker(INT16 nWorker);
		void	StopWorker();
		
		//	Main
		BOOL	ProcessOperation(AgsmLoginQueueInfo *pQueryInfo, ApHttp *pHttp = NULL);
		
		//	Login step
		BOOL	ProcessCheckAccount1(AgsmLoginQueueInfo *pQueryInfo, ApHttp *pHttp);
		BOOL	ProcessCheckAccount2(AgsmLoginQueueInfo *pQueryInfo);
		INT32	ProcessCheckAccountFromDB(AgsmLoginQueueInfo *pQueryInfo);
		BOOL	ProcessCheckBillInfo(AgsmLoginQueueInfo *pQueryInfo);
		BOOL	ProcessGetUnion(AgsmLoginQueueInfo *pQueryInfo);
		BOOL	ProcessGetEKey(AgsmLoginQueueInfo* pQueryInfo);
		BOOL	ProcessGetCharacters(AgsmLoginQueueInfo *pQueryInfo);
		BOOL	ProcessSelectCharacter(AgsmLoginQueueInfo *pQueryInfo);
	
		//	Create/Remove
		BOOL	ProcessCreateCharacter(AgsmLoginQueueInfo *pQueryInfo);
		BOOL	ProcessRemoveCharacter(AgsmLoginQueueInfo *pQueryInfo);
		BOOL	ProcessRenameCharacter(AgsmLoginQueueInfo *pQueryInfo);
		
		//	Status Update
		BOOL	ProcessResetLoginStatusByDisconnectFromGameServer(AgsmLoginQueueInfo *pQueryInfo);
		BOOL	ProcessResetLoginStatusByDisconnectFromLoginServer(AgsmLoginQueueInfo *pQueryInfo);
		BOOL	ProcessSetLoginStatusInGameServer(AgsmLoginQueueInfo *pQueryInfo);
		BOOL	ProcessReturnToLoginServer(AgsmLoginQueueInfo *pQueryInfo);		

		//	Login Key
		void	SetKeyRange(INT32 ServerIndex);
		
		//	Expedition
		BOOL	LoadExpeditionList();
		BOOL	IsExistExpeditionList(TCHAR *pszWorldName, TCHAR *pszCharName);

		//	Compensation
		BOOL	CheckCompensation(AgsmLoginQueueInfo *pQueryInfo, TCHAR *pszChar);
		BOOL	GetCompenMaster(AgpdLoginCompenMaster **ppCompenMaster, INT32 &lCount, INT32 lCompenID = 0);
		BOOL	IsNotCompensated(AuDatabase2 *pDatabase, eAGPMLOGINDB_COMPEN_TYPE eType, TCHAR *pszDetailTable, AgpdLoginCompenDetail *pDetail);
		BOOL	PayCompensation(TCHAR *pszDBName, AgpdCharacter *pAgpdCharacter, TCHAR *pszWorld, AgpdLoginCompenMaster *pAgpdLoginCompenMaster);
		
		//	Coupon
		BOOL	ProcessCoupon(TCHAR *pszDBName, AgpdCharacter *pAgpdCharacter, TCHAR *pszWorld);
		BOOL	GetEventCouponInfo(AuDatabase2 *pDatabase, AgsdAccount *pcsAccount);
		BOOL	ProcessEventCoupon(AuDatabase2 *pMaster, AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter, TCHAR *pszWorld);
		
		//	Query
		BOOL	LoadQuery(TCHAR *pszFile, eAUDB_VENDER eVender);
		TCHAR*	GetQuery(TCHAR *pszKey, eAUDB_VENDER eVender);
		void	RemoveAllQuery();

		//	Callbacks
		static BOOL	CBCheckBillAuthResponse(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);
		static BOOL CBReceiveBillAuthResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGetCharacterCount(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBReconnectClient(PVOID pData, PVOID pClass, PVOID pCustData);

		//	ETC
		BOOL	SetBillAuthResponse(TCHAR *pszAccountID, UINT32 ulNID, BOOL IsPCRoom, BOOL IsAuthPCRoom, AgpdBillInfo *pcsBillInfo, int isLimited = 0, int isProtected = 0);
		BOOL	SetReconnectClient(UINT32 ulNID, TCHAR *pszAccountName);		// set client returned from game server

	protected:
		//	Insert helper
		BOOL	_InsertCharacter(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter, INT8 cSlot, TCHAR *pszServerName);
		BOOL	_InsertItem(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter);
		BOOL	_InsertSkill(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter);
		BOOL	_InsertAccountWorld(AuDatabase2 *pDatabase, AgsdCharacter *pAgsdCharacter, TCHAR *pszServerName);
		BOOL	_WriteWorldMapping(AuDatabase2 *pDatabase, TCHAR *pszAccountID, TCHAR *pszServerName,
									TCHAR *pszCharID, BOOL bRemove = FALSE);
		
		//	Select helper
		INT32	_SelectCharacterView(AuDatabase2 *pDatabase, TCHAR *pszChar, INT8 *pcSlot = NULL, UINT32 ulNID = 0);
		BOOL	_SelectCharacterViewExceptMaster(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter);
		BOOL	_SelectCharacterExceptView(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter, TCHAR *pszServerName);
		BOOL	_SelectNonEquipItems(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter);
		BOOL	_SelectCashItems(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter);
		BOOL	_SelectBankItems(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter, TCHAR *pszServerName);
		BOOL	_SelectAccountWorld(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter, TCHAR *pszServerName);
		BOOL	_SelectSkill(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter);
		BOOL	_SelectQuest(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter);
		BOOL	_SelectGuild(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter);
		BOOL	_SelectTitle(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter);
		//BOOL	_SelectSales(AuDatabase2 *pDatabase, AgpdCharacter *pAgpdCharacter);
		
		//	Status Helper
		BOOL	_CheckLoginStatus(AuDatabase2 *pDatabase, UINT32 ulClientNID, TCHAR *pszAccountID);
		BOOL	_WriteLoginStatus(AuDatabase2 *pDatabase, BOOL bUpdate, TCHAR *pszAccountID, UINT64 ullKey, BOOL bTran);
		BOOL	_WriteLoginStatus(AuDatabase2 *pDatabase, BOOL bUpdate, TCHAR *pszAccountID, UINT64 ullKey,
								   TCHAR *pszServer, TCHAR *pszChar, INT32 lCID, BOOL bTran = TRUE);
		BOOL	_CheckSocialNumber(TCHAR *pszSocialNo, INT32 lAge);
		BOOL	_IsArchlord(AuDatabase2 *pDatabase, TCHAR *pszChar);
		BOOL	_IsArchlordGuard(AuDatabase2 *pDatabase, TCHAR *pszChar);
		//JK_케릭삭제시패스워드확인
		BOOL	_CheckAccountPassword(AuDatabase2 *pDatabase, UINT32 ulClientNID, TCHAR *pszAccountID, TCHAR *pszPassword);


		//	Login Key
		UINT64	_GetKey();

		//	Database
		BOOL			ConnectDatabase();
		void			DisconnectDatabase();
		AuDatabase2*	GetIdleMasterDatabase();
		AuDatabase2*	GetIdleGameDatabase(TCHAR *pszName);
		AuDatabase2*	GetIdleGameDatabase(INT32 *plIndex);
		AuDatabase2*	GetIdleCompenDatabase();

#ifdef _WEBZEN_AUTH_
		int				WebzenAuthFromDB(AgsmLoginQueueInfo *pQueryInfo, INT32 &nAge);
		//JK_웹젠인증 : 케릭터 선택하기로 돌아 왔을때 나이 정보와 PCGuid를 다시 받아온다.
		int				GetWebzenAuthInfoFromDB(AgsmLoginQueueInfo *pQueryInfo) ;
		bool			ParseWebzenGamestring( char* szGameString ,int &dwAccount, string &szGameAuthKey, int &dwClientCnt);
		bool			ParseGlobalGamestring( char* szGameString ,string &szGameId, string &szGameAuthKey, string &szMacAddress);
#endif
	};

#endif
