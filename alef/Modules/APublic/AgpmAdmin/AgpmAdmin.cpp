// AgpmAdmin.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 04.


#include "AgpmAdmin.h"
#include <time.h>
#include "ApMemoryTracker.h"
#include "AgppAdmin.h"

/////////////////////////////////////////////////////////////////////////////
//
// AgpmAdmin
// Start!!!
AgpmAdmin::AgpmAdmin()
{
	SetModuleName("AgpmAdmin");

	EnableIdle(FALSE);

	SetModuleType(APMODULE_TYPE_PUBLIC);

	SetModuleData(sizeof(AgpdAdmin));

	// 대략 패킷 타입 세팅이랑, 패킷 속성을 만들어주고...
	SetPacketType(AGPMADMIN_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operator
							AUTYPE_INT32,			1,	// Admin ID
							AUTYPE_PACKET,			1,	// Login
							AUTYPE_PACKET,			1,	// Search
							AUTYPE_PACKET,			1,	// Search - Character Data
							AUTYPE_PACKET,			1,	// Edit - Character Data
							AUTYPE_PACKET,			1,	// Search - Character Item
							AUTYPE_PACKET,			1,	// Search - Character Skill
							AUTYPE_PACKET,			1,	// Search - Character Party, Result 값이 날라오기도 한다.
							AUTYPE_PACKET,			1,	// Char Move
							AUTYPE_PACKET,			1,	// Ban
							AUTYPE_PACKET,			1,	// Help
							AUTYPE_PACKET,			1,	// Item
							AUTYPE_PACKET,			1,	// Server Info
							AUTYPE_PACKET,			1,	// Title
							AUTYPE_END,				0
							);

	m_csLoginPacket.SetFlagLength(sizeof(INT8));
	m_csLoginPacket.SetFieldType(
							AUTYPE_INT8,			1,	// 1 : Login, 0 : Logout
							AUTYPE_INT32,			1,	// Admin ID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Game ID
							AUTYPE_INT16,			1,	// Admin Level
							AUTYPE_INT8,			1,	// Server ID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// AccName
							AUTYPE_END,				0
							);

	m_csSearchPacket.SetFlagLength(sizeof(INT8));
	m_csSearchPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Object Type
							AUTYPE_INT8,			1,	// Field
							AUTYPE_INT32,			1,	// Search (ID)
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Game ID
							AUTYPE_PACKET,			1,	// Search Result *결과를 보낼때 따라가는데 NULL 일 수도 있다.
							AUTYPE_END,				0
							);

	m_csSearchResultPacket.SetFlagLength(sizeof(INT16));
	m_csSearchResultPacket.SetFieldType(
							AUTYPE_INT16,			1,	// Total Result Num	- 1 based
							AUTYPE_INT16,			1,	// Now Count - 1 based
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Game ID
							AUTYPE_INT32,			1,		// CID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Acc Name
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Guild ID
							AUTYPE_INT32,			1,		// TID
							AUTYPE_INT32,			1,		// Level
							AUTYPE_CHAR,			AGPMADMIN_MAX_UT_STRING + 1,		// UT
							AUTYPE_CHAR,			AGPMADMIN_MAX_CREDATE_STRING + 1,	// CreDate
							AUTYPE_CHAR,			AGPMADMIN_MAX_IP_STRING + 1,		// IP
							AUTYPE_INT32,			1,		// Status
							AUTYPE_END,				0
							);

	m_csSearchCharDataPacket.SetFlagLength(sizeof(INT16));
	m_csSearchCharDataPacket.SetFieldType(
							AUTYPE_INT32,			1,	// CID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Char Name 2003.12.29.
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Acc Name
							AUTYPE_UINT32,			1,									// Current Conntected TimeStamp
							AUTYPE_CHAR,			AGPMADMIN_MAX_IP_STRING + 1,		// IP
							AUTYPE_CHAR,			AGPMADMIN_MAX_IP_STRING + 1,		// Last IP
							AUTYPE_END,				0
							);

	m_csSearchCharItemPacket.SetFlagLength(sizeof(INT8));
	m_csSearchCharItemPacket.SetFieldType(
							AUTYPE_INT32,			1,	// Char CID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Char Name
							AUTYPE_INT8,			1,	// Position (Body, Bag, Board, Store...)
							AUTYPE_END,				0
							);

	m_csSearchCharSkillPacket.SetFlagLength(sizeof(INT8));
	m_csSearchCharSkillPacket.SetFieldType(
							AUTYPE_INT32,			1,	// Char CID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Char Name
							AUTYPE_INT16,			1,	// Type (All, Mastery1, Mastery2, Mastery2... OR Skill Mastery Sub Type)
							AUTYPE_END,				0
							);

	// 2005.05.06. steeple. Party 를 Custom 으로 변경.
	// 구조체를 통째로 넘기기 위해서 사용.
	m_csSearchCustomPacket.SetFlagLength(sizeof(INT8));
	m_csSearchCustomPacket.SetFieldType(
							AUTYPE_INT32,			1,	// Custom Type
							AUTYPE_MEMORY_BLOCK,	1,	// 각 데이터들이 구조체 통째로 넘어온다.
							AUTYPE_END,				0
							);
	
	m_csCharMovePacket.SetFlagLength(sizeof(INT8));
	m_csCharMovePacket.SetFieldType(
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Char Name
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Target Name (보통 NULL 이다. 윗놈을 이놈에게 보낼때 쓰이게 된다.)
							AUTYPE_FLOAT,			1,	// x
							AUTYPE_FLOAT,			1,	// y
							AUTYPE_FLOAT,			1,	// z
							AUTYPE_END,				0
							);
	
	m_csCharEditPacket.SetFlagLength(sizeof(INT16));
	m_csCharEditPacket.SetFieldType(
							AUTYPE_MEMORY_BLOCK,	1,	// 깔끔하게 stAgpdAdminCharEdit 을 통째로!!!
							AUTYPE_MEMORY_BLOCK,	1,	// Skill 2006.05.04.
							AUTYPE_END,				0
							);

	m_csBanPacket.SetFlagLength(sizeof(INT16));
	m_csBanPacket.SetFieldType(
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Char Name
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// Acc Name
							AUTYPE_INT8,			1,	// Logout
							AUTYPE_INT8,			1,	// Ban Set Flag
							AUTYPE_INT32,			1,	// ChatBan Start Time
							AUTYPE_INT8,			1,	// ChatBan Keep Time
							AUTYPE_INT32,			1,	// CharBan Start Time
							AUTYPE_INT8,			1,	// CharBan Keep Time
							AUTYPE_INT32,			1,	// AccBan Start Time
							AUTYPE_INT8,			1,	// AccBan Keep Time
							AUTYPE_END,				0
							);

	m_csItemPacket.SetFlagLength(sizeof(INT16));
	m_csItemPacket.SetFieldType(
							AUTYPE_MEMORY_BLOCK,	1,	// 깔끔하게 stAgpdAdminItemOperation 을 통째로!!!
							AUTYPE_END,				0
							);

	m_csServerInfoPacket.SetFlagLength(sizeof(INT8));
	m_csServerInfoPacket.SetFieldType(
							AUTYPE_CHAR,			AGPMADMIN_MAX_IP_STRING + 1,	// Server IP
							AUTYPE_INT16,			1,	// Server Port
							AUTYPE_END,				0
							);

	m_csTitlePacket.SetFlagLength(sizeof(INT16));
	m_csTitlePacket.SetFieldType(
							AUTYPE_MEMORY_BLOCK,	1,	// stAgpdAdminTitleOperation
							AUTYPE_END,				0
							);
	
	m_nIndexADCharacter = 0;
	m_lConnectableAccountLevel = 0;
}

AgpmAdmin::~AgpmAdmin()
{
	// 남아 있는 데이터를 비운다.
	if(m_listAdmin.size() != 0)
	{
		list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
		while(iterData != m_listAdmin.end())
		{
			if(*iterData)
				delete *iterData;

			iterData++;
		}

		m_listAdmin.clear();
	}
}

BOOL AgpmAdmin::OnAddModule()
{
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgpmItem = (AgpmItem*)GetModule("AgpmItem");
	m_pcsAgpmConfig = (AgpmConfig*)GetModule("AgpmConfig");

	if(!m_pcsAgpmCharacter || !m_pcsAgpmItem)
		return FALSE;

	// AgpdAdmin 을 Attach 시킨다.
	m_nIndexADCharacter = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdAdmin), ConAgpdAdminADChar, DesAgpdAdminADChar);

	// RemoveChar Callback 추가. 2004.03.09.
	if(!m_pcsAgpmCharacter->SetCallbackRemoveChar(CBRemoveAdmin, this))
		return FALSE;
	
	return TRUE;
}

BOOL AgpmAdmin::OnInit()
{
	return TRUE;
}

BOOL AgpmAdmin::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmAdmin::OnDestroy()
{
	return TRUE;
}







void AgpmAdmin::AdminLock()
{
	m_csAdminLock.WLock();
}

void AgpmAdmin::AdminUnlock()
{
	m_csAdminLock.Release();
}

BOOL AgpmAdmin::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgpmAdmin::OnReceive");

	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = AGPMADMIN_PACKET_MAX;
	INT32 lCID = -1;
	PVOID pvLogin = NULL;
	PVOID pvSearch = NULL;
	PVOID pvSearchCharData = NULL;
	PVOID pvCharEdit = NULL;
	PVOID pvSearchCharItem = NULL;
	PVOID pvSearchCharSkill = NULL;
	PVOID pvSearchCustom = NULL;
	PVOID pvCharMove = NULL;
	PVOID pvBan = NULL;
	PVOID pvHelp = NULL;
	PVOID pvItem = NULL;
	PVOID pvServerInfo = NULL;
	PVOID pvTitle = NULL;
		
	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&pvLogin,
						&pvSearch,
						&pvSearchCharData,
						&pvCharEdit,
						&pvSearchCharItem,
						&pvSearchCharSkill,
						&pvSearchCustom,
						&pvCharMove,
						&pvBan,
						&pvHelp,
						&pvItem,
						&pvServerInfo,
						&pvTitle
						);

	AgpdCharacter* pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);
	if(!pcsCharacter && cOperation != AGPMADMIN_PACKET_PING)
		return FALSE;

	if(pcsCharacter)
		pcsCharacter->m_Mutex.WLock();

	BOOL bResult = TRUE;
	switch(cOperation)
	{
		case AGPMADMIN_PACKET_LOGIN:	// Client, Server
			if(pvLogin)
				bResult = OnOperationLogin(pcsCharacter, pvLogin);
			break;

		case AGPMADMIN_PACKET_CONNECT:	// Server
			if(pvLogin)
				bResult = OnOperationConnect(pcsCharacter, pvLogin, ulNID);
			break;

		case AGPMADMIN_PACKET_NUM_CURRENT_USER:		// Client
			if(pvLogin)
				bResult = OnOperationNumCurrentUser(pcsCharacter, pvLogin);
			break;

		case AGPMADMIN_PACKET_SEARCH_CHARACTER:	// Client, Server
			if(pvSearch)
				bResult = OnOperationSearch(pcsCharacter, pvSearch);
			break;

		case AGPMADMIN_PACKET_SEARCH_RESULT:	// Client
			if(pvSearch)
				bResult = OnOperationSearchResult(pcsCharacter, pvSearch);
			break;

		case AGPMADMIN_PACKET_SEARCH_CHARDATA:	// Client
			if(pvSearchCharData)
				bResult = OnOperationSearchCharData(pcsCharacter, pvSearchCharData);
			break;

		case AGPMADMIN_PACKET_SEARCH_CHARITEM:	// Server
			if(pvSearchCharItem)
				bResult = OnOperationSearchCharItem(pcsCharacter, pvSearchCharItem);
			break;

		case AGPMADMIN_PACKET_SEARCH_CHARSKILL:	// Server
			if(pvSearchCharSkill)
				bResult = OnOperationSearchCharSkill(pcsCharacter, pvSearchCharSkill);
			break;

		case AGPMADMIN_PACKET_SEARCH_CUSTOM:	// Client.	2005.05.06. steeple
			if(pvSearchCustom)
				bResult = OnOperationSearchCustom(pcsCharacter, pvSearchCustom);
			break;

		case AGPMADMIN_PACKET_CHARMOVE:	// Server
			if(pvCharMove)
				bResult = OnOperationCharMove(pcsCharacter, pvCharMove);
			break;

		case AGPMADMIN_PACKET_CHAREDIT:	// Client, Server
			if(pvCharEdit)
				bResult = OnOperationCharEdit(pcsCharacter, pvCharEdit);
			break;

		case AGPMADMIN_PACKET_BAN:	// Client, Server
			if(pvBan)
				bResult = OnOperationBan(pcsCharacter, pvBan);
			break;

		case AGPMADMIN_PACKET_ITEM:	// Client, Server
			if(pvItem)
				bResult = OnOperationItem(pcsCharacter, pvItem);
			break;

		case AGPMADMIN_PACKET_SERVERINFO:	// Client
			if(pvServerInfo)
				bResult = OnOperationServerInfo(pcsCharacter, pvServerInfo);
			break;

		case AGPMADMIN_PACKET_GUILD:	// Client, Server
			// 그냥 패킷 추가하기 귀찮아서 CharEdit 패킷을 사용한다. -0-;
			if(pvCharEdit)
				bResult = OnOperationGuild(pcsCharacter, pvCharEdit);
			break;

		case AGPMADMIN_PACKET_PING:			// 2007.07.30. steeple
			if(pvItem)
				bResult = OnOperationPing(ulNID, pvItem);
			break;

		case AGPMADMIN_PACKET_USER_POSITION_REQUEST: // Server
				bResult = OnOperationUserPosition(pcsCharacter);
			break;

		case AGPMADMIN_PACKET_USER_POSITION_ANSWER: // Client
				bResult = OnOperationUserPositionStatistics(pvPacket);
				// 이곳에 클라이언트 처리 
			break;
		case AGPMADMIN_PACKET_ADMIN_CLIENT_LOGIN:
				bResult = OnOperationAdminClient(pcsCharacter, pvServerInfo);
			break;
		case AGPMADMIN_PACKET_TITLE:
			if(pvTitle)
				bResult = OnOperationTitle(pcsCharacter, pvTitle);
			break;
	}

	if(pcsCharacter)
		pcsCharacter->m_Mutex.Release();

	return bResult;
}

// Client, Server
BOOL AgpmAdmin::OnOperationLogin(AgpdCharacter* pcsAgpdCharacter, PVOID pvLogin)
{
	INT8 cOperation = 0;
	stAgpdAdminInfo stAdminInfo;
	ZeroMemory(&stAdminInfo, sizeof(stAdminInfo));
	CHAR* szCharName = NULL, *szAccName = NULL;
	m_csLoginPacket.GetField(FALSE, pvLogin, 0,
								&cOperation,
								&stAdminInfo.m_lCID,
								&szCharName,
								&stAdminInfo.m_lAdminLevel,
								&stAdminInfo.m_lServerID,
								&szAccName);
	if(szCharName && strlen(szCharName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stAdminInfo.m_szAdminName, szCharName);
	if(szAccName && strlen(szAccName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stAdminInfo.m_szAccName, szAccName);

	// 2004.03.24. Operation 설정. NumCurrentUser 추가.
	switch(cOperation)
	{
		case AGPMADMIN_LOGIN:
			OnOperationAddAdmin(pcsAgpdCharacter, &stAdminInfo);
			break;

		case AGPMADMIN_LOGOUT:
			OnOperationRemoveAdmin(pcsAgpdCharacter, stAdminInfo.m_lCID);
			break;
	}

	return TRUE;
}

// Client, Server
// 이 함수는 Client 가 받을 때는 OnReceive 에서 호출하지만,
// 서버에서는 Admin 이 접속할때, 호출하게 된다.
//
//
// 변경. 2004.03.05.
// 이 함수는 수도 없이 불리게 된다. 모든 캐릭터가 접속할때마다 불리게 되는데,
// 그때는 pstAdminInfo 의 Level 을 잘 보고 세팅을 해야 한다.
//
// 변경. 2004.03.25.
// 현재 접속자 수를 관리할 때도 사용하게 된다. Attached Data 를 잘 보면 된다.
//
// 변경. 2004.03.26.
// 현재 접속자 수 관리 AddCharacter 는 AgsmCharacter 에서 ServerID 를 받았을 때 불리게 된다.
BOOL AgpmAdmin::OnOperationAddAdmin(AgpdCharacter* pcsAgpdCharacter, stAgpdAdminInfo* pstAdminInfo)
{
	if(!pstAdminInfo)
		return FALSE;

	AgpdAdmin* pcsAttachedAdmin = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAttachedAdmin)
		return FALSE;

	// Level 체크가 이리로 들어옴. 2004.03.05. steeple
	if(pstAdminInfo->m_lAdminLevel < AGPMADMIN_LEVEL_ADMIN)
		return FALSE;
	
	// Admin List Lock
	AdminLock();

	stAgpdAdminInfo* pstAdminInfoInList = GetAdminInfo(pstAdminInfo->m_lCID);
	if(pstAdminInfoInList)	// 이미 Add 되어있다.
	{
		// 변경해준다.
		memcpy(pstAdminInfoInList, pstAdminInfo, sizeof(stAgpdAdminInfo));
	}
	else
	{
		stAgpdAdminInfo* pstNewAdminInfo = new stAgpdAdminInfo;
		memset(pstNewAdminInfo, 0, sizeof(stAgpdAdminInfo));
		memcpy(pstNewAdminInfo, pstAdminInfo, sizeof(stAgpdAdminInfo));

		m_listAdmin.push_back(pstNewAdminInfo);
	}

	// Admin List Unlock
	AdminUnlock();

	// Attach Data 에 값을 넣어준다.
	pcsAttachedAdmin->m_lAdminCID = pstAdminInfo->m_lCID;
	pcsAttachedAdmin->m_lAdminLevel = pstAdminInfo->m_lAdminLevel;

	EnumCallback(AGPMADMIN_CB_ADD_ADMIN, pcsAgpdCharacter, pstAdminInfo);

#ifdef _DEBUG
	PrintAdminList();
#endif
	
	return TRUE;
}

// Client, Server
// 이 함수는 Client 가 받을 때는 OnReceive 에서 호출하지만,
// 서버에서는 Admin 이 나갈때, 호출하게 된다.
BOOL AgpmAdmin::OnOperationRemoveAdmin(AgpdCharacter* pcsAgpdCharacter, INT32 lRemoveAdminID)
{
	AdminLock();

	if(m_listAdmin.size() == 0)
	{
		AdminUnlock();
		return FALSE;
	}

	BOOL bFound = FALSE;
	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if(*iterData && (*iterData)->m_lCID == lRemoveAdminID)
		{
			bFound = TRUE;

			delete *iterData;
			m_listAdmin.erase(iterData);

			break;
		}

		iterData++;
	}

	if(bFound)
		EnumCallback(AGPMADMIN_CB_REMOVE_ADMIN, &lRemoveAdminID, NULL);

	AdminUnlock();

#ifdef _DEBUG
	PrintAdminList();
#endif

	return TRUE;
}

BOOL AgpmAdmin::OnOperationConnect(AgpdCharacter* pcsAgpdCharacter, PVOID pvLogin, UINT32 ulNID)
{
	INT8 cOperation = 0;
	stAgpdAdminInfo stAdminInfo;
	ZeroMemory(&stAdminInfo, sizeof(stAdminInfo));
	CHAR* szCharName = NULL, *szAccName = NULL;
	m_csLoginPacket.GetField(FALSE, pvLogin, 0,
								&cOperation,
								&stAdminInfo.m_lCID,
								&szCharName,
								&stAdminInfo.m_lAdminLevel,
								&stAdminInfo.m_lServerID,
								&szAccName);
	if(szCharName && strlen(szCharName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stAdminInfo.m_szAdminName, szCharName);
	if(szAccName && strlen(szAccName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stAdminInfo.m_szAccName, szAccName);

	EnumCallback(AGPMADMIN_CB_CONNECT, pcsAgpdCharacter, &ulNID);
	return TRUE;
}

// 2004.03.24.
// Client, Server
BOOL AgpmAdmin::OnOperationNumCurrentUser(AgpdCharacter* pcsAgpdCharacter, PVOID pvLogin)
{
	// Admin Login Packet 을 그대로 사용하고,
	// 얍삽하게도 CID 가 오는 곳에 lCurrentUser 가 온다. -0-
	INT8 cOperation = 0;
	INT32 lCurrentUser = 0;
	m_csLoginPacket.GetField(FALSE, pvLogin, 0,
								&cOperation,
								&lCurrentUser,
								NULL,
								NULL,
								NULL,
								NULL);

	EnumCallback(AGPMADMIN_CB_NUM_CURRENT_USER, pcsAgpdCharacter, &lCurrentUser);
	return TRUE;
}

BOOL AgpmAdmin::OnOperationServerInfo(AgpdCharacter* pcsAgpdCharacter, PVOID pvServerInfo)
{
	stAgpdAdminServerInfo stServerInfo;
	ZeroMemory(&stServerInfo, sizeof(stServerInfo));
	CHAR* szServerIP = NULL;

	m_csServerInfoPacket.GetField(FALSE, pvServerInfo, 0,
								&szServerIP,
								&stServerInfo.m_lPort);
	if(szServerIP && strlen(szServerIP) <= AGPMADMIN_MAX_IP_STRING)
		strcpy(stServerInfo.m_szServerIP, szServerIP);

	EnumCallback(AGPMADMIN_CB_SERVERINFO, &stServerInfo, pcsAgpdCharacter);
	return TRUE;
}

// Server
BOOL AgpmAdmin::OnOperationSearch(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearch)
{
	// Result Packet 용 - 여기서는 쓰이지 않는다.
	PVOID pvResult = NULL;

	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));
	CHAR* szSearchName = NULL;
	
	m_csSearchPacket.GetField(FALSE, pvSearch, 0,
								&stSearch.m_iType,
								&stSearch.m_iField,
								&stSearch.m_lObjectCID,
								&szSearchName,
								&pvResult);
	if(szSearchName && strlen(szSearchName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stSearch.m_szSearchName, szSearchName);

	EnumCallback(AGPMADMIN_CB_SEARCH_CHARACTER, &stSearch, pcsAgpdCharacter);
	return TRUE;
}

// Client
BOOL AgpmAdmin::OnOperationSearchResult(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearch)
{
	// Result Packet 용
	PVOID pvResult = NULL;

	CHAR* szSearchName = NULL;
	stAgpdAdminSearch stSearch;
	ZeroMemory(&stSearch, sizeof(stSearch));
	
	m_csSearchPacket.GetField(FALSE, pvSearch, 0,
								&stSearch.m_iType,
								&stSearch.m_iField,
								&stSearch.m_lObjectCID,
								&szSearchName,
								&pvResult);
	if(szSearchName && strlen(szSearchName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stSearch.m_szSearchName, szSearchName);

	if(!pvResult)
		return FALSE;

	CHAR* szCharName = NULL, *szAccName = NULL, *szGuildID = NULL, *szUT = NULL, *szCreDate = NULL, *szIP = NULL;
	INT32 lCID = -1;
	INT16 lTotalNum = -1, lCount = -1;
	INT32 lTID = -1, lLevel = -1, lStatus = -1;

	// 데이터를 받아서 세팅한다.
	stAgpdAdminSearchResult stSearchResult;
	ZeroMemory(&stSearchResult, sizeof(stSearchResult));
	m_csSearchResultPacket.GetField(FALSE, pvResult, 0,
										&lTotalNum,
										&lCount,
										&szCharName,
										&lCID,
										&szAccName,
										&szGuildID,
										&lTID,
										&lLevel,
										&szUT,
										&szCreDate,
										&szIP,
										&lStatus);

	//stSearchResult.m_lTotalNum = lTotalNum;
	//stSearchResult.m_lCount = lCount;

	if(szCharName && strlen(szCharName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stSearchResult.m_szCharName, szCharName);

	//stSearchResult.m_lCID = lCID;

	if(szAccName && strlen(szAccName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stSearchResult.m_szAccName, szAccName);

	if(szGuildID && strlen(szGuildID) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stSearchResult.m_szGuildID, szGuildID);

	stSearchResult.m_lTID = lTID;
	stSearchResult.m_lLevel = lLevel;
	
	//if(szUT && strlen(szUT) <= AGPMADMIN_MAX_UT_STRING)
	//	strcpy(stSearchResult.m_szUT, szUT);

	if(szCreDate && strlen(szCreDate) <= AGPMADMIN_MAX_CREDATE_STRING)
		strcpy(stSearchResult.m_szCreDate, szCreDate);

	if(szIP && strlen(szIP) <= AGPMADMIN_MAX_IP_STRING)
		strcpy(stSearchResult.m_szIP, szIP);

	stSearchResult.m_lStatus = lStatus;

	// Enum 해주고..
	EnumCallback(AGPMADMIN_CB_SEARCH_RESULT, &stSearch, &stSearchResult);

	return TRUE;
}

// Client
BOOL AgpmAdmin::OnOperationSearchCharData(AgpdCharacter* pcsAgpdCharactrer, PVOID pvSearchCharData)
{
	// 여기서는 Sub 데이터를 받는다.
	stAgpdAdminCharDataSub stCharDataSub;
	ZeroMemory(&stCharDataSub, sizeof(stCharDataSub));
	CHAR* szCharName = NULL;
	CHAR* szAccName = NULL;
	CHAR* szIP = NULL;
	CHAR* szLastIP = NULL;

	m_csSearchCharDataPacket.GetField(FALSE, pvSearchCharData, 0,
								&stCharDataSub.m_lCID,
								&szCharName,
								&szAccName, 
								&stCharDataSub.m_ulConnectedTimeStamp,
								&szIP,
								&szLastIP);
	if(szCharName && strlen(szCharName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stCharDataSub.m_szCharName, szCharName);
	if(szAccName && strlen(szAccName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stCharDataSub.m_szAccName, szAccName);
	if(szIP && strlen(szIP) <= AGPMADMIN_MAX_IP_STRING)
		strcpy(stCharDataSub.m_szIP, szIP);
	if(szLastIP && strlen(szLastIP) <= AGPMADMIN_MAX_IP_STRING)
		strcpy(stCharDataSub.m_szLastIP, szLastIP);

	EnumCallback(AGPMADMIN_CB_SEARCH_CHARDATA, &stCharDataSub, pcsAgpdCharactrer);

	return TRUE;
}

// Server
BOOL AgpmAdmin::OnOperationSearchCharItem(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearchCharItem)
{
	stAgpdAdminCharItem stItem;
	ZeroMemory(&stItem, sizeof(stItem));
	CHAR* szCharName;

	m_csSearchCharItemPacket.GetField(FALSE, pvSearchCharItem, 0,
								&stItem.m_lCID,
								&szCharName,
								&stItem.m_lPos);
	if(szCharName && strlen(szCharName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stItem.m_szCharName, szCharName);

	EnumCallback(AGPMADMIN_CB_SEARCH_CHARITEM, &stItem, pcsAgpdCharacter);

	return TRUE;
}

// Server
BOOL AgpmAdmin::OnOperationSearchCharSkill(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearchCharSkill)
{
	stAgpdAdminCharSkill stSkill;
	ZeroMemory(&stSkill, sizeof(stSkill));
	CHAR* szCharName;

	m_csSearchCharSkillPacket.GetField(FALSE, pvSearchCharSkill, 0,
								&stSkill.m_lCID,
								&szCharName,
								&stSkill.m_lType);
	if(szCharName && strlen(szCharName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stSkill.m_szCharName, szCharName);

	EnumCallback(AGPMADMIN_CB_SEARCH_CHARSKILL, &stSkill, pcsAgpdCharacter);

	return TRUE;
}

// Server, Client
BOOL AgpmAdmin::OnOperationSearchCustom(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearchCustom)
{
	INT32 lCustomType = -1;
	PVOID pvCustomData = NULL;
	INT16 nDataLength = -1;

	m_csSearchCustomPacket.GetField(FALSE, pvSearchCustom, 0,
								&lCustomType,
								&pvCustomData, &nDataLength
								);
	if(lCustomType < 0 || !pvCustomData || nDataLength < 1)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = pvCustomData;
	pvBuffer[1] = &nDataLength;

	EnumCallback(AGPMADMIN_CB_SEARCH_CUSTOM, &lCustomType, pvBuffer);

	return TRUE;
}

// Server
BOOL AgpmAdmin::OnOperationCharMove(AgpdCharacter* pcsAgpdCharacter, PVOID pvCharMove)
{
	// 2003.11.25. 김태희
	stAgpdAdminCharDataPosition stPosition;
	ZeroMemory(&stPosition, sizeof(stPosition));
	CHAR* szName = NULL;
	CHAR* szTargetName = NULL;

	m_csCharMovePacket.GetField(FALSE, pvCharMove, 0,
								&szName,
								&szTargetName,
								&stPosition.m_fX,
								&stPosition.m_fY,
								&stPosition.m_fZ);
	if(szName && strlen(szName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stPosition.m_szName, szName);
	if(szTargetName && strlen(szTargetName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stPosition.m_szTargetName, szTargetName);

	EnumCallback(AGPMADMIN_CB_CHARMOVE, &stPosition, pcsAgpdCharacter);

	return TRUE;
}

// Client, Server
BOOL AgpmAdmin::OnOperationCharEdit(AgpdCharacter* pcsAgpdCharacter, PVOID pvCharEdit)
{
	PVOID pvCharEditData = NULL;
	INT16 nSize = 0;
	
	PVOID pvSkillData = NULL;
	INT16 nSkillDataSize = 0;

	m_csCharEditPacket.GetField(FALSE, pvCharEdit, 0,
								&pvCharEditData, &nSize,
								&pvSkillData, &nSkillDataSize);

	if(!pvCharEditData || nSize == 0)
		return FALSE;

	// 2005.03.07. steeple
	// 2003.12.29. 김태희
	stAgpdAdminCharEdit* pstCharEdit = static_cast<stAgpdAdminCharEdit*>(pvCharEditData);
	if(!pstCharEdit)
		return FALSE;

	PVOID pvBuffer[3];
	pvBuffer[0] = pstCharEdit;
	pvBuffer[1] = pvSkillData;
	pvBuffer[2] = &nSkillDataSize;

	EnumCallback(AGPMADMIN_CB_CHAREDIT, pvBuffer, pcsAgpdCharacter);

	return TRUE;
}

// Client, Server
// Guild 관련 Operation. 패킷은 CharEdit 것을 그대루 사용한다. 크헷~
BOOL AgpmAdmin::OnOperationGuild(AgpdCharacter* pcsAgpdCharacter, PVOID pvGuildPacket)
{
	PVOID pvGuild = NULL;
	INT16 nSize = 0;

	PVOID pvDummy = NULL;
	INT16 nDummySize = 0;

	m_csCharEditPacket.GetField(FALSE, pvGuildPacket, 0,
								&pvGuild, &nSize,
								&pvDummy, &nDummySize);

	if(!pvGuild || nSize == 0)
		return FALSE;

	// 2005.03.07. steeple
	// 2003.12.29. 김태희
	stAgpdAdminCharEdit* pstGuild = static_cast<stAgpdAdminCharEdit*>(pvGuild);
	if(!pstGuild)
		return FALSE;

	EnumCallback(AGPMADMIN_CB_GUILD, pstGuild, pcsAgpdCharacter);

	return TRUE;
}

// Client, Server
BOOL AgpmAdmin::OnOperationBan(AgpdCharacter* pcsAgpdCharacter, PVOID pvBan)
{
	stAgpdAdminBan stBan;
	ZeroMemory(&stBan, sizeof(stBan));
	CHAR* szCharName = NULL;
	CHAR* szAccName = NULL;

	m_csBanPacket.GetField(FALSE, pvBan, 0,
								&szCharName,
								&szAccName,
								&stBan.m_lLogout,
								&stBan.m_lBanFlag,
								&stBan.m_lChatBanStartTime,
								&stBan.m_lChatBanKeepTime,
								&stBan.m_lCharBanStartTime,
								&stBan.m_lCharBanKeepTime,
								&stBan.m_lAccBanStartTime,
								&stBan.m_lAccBanKeepTime);
	if(szCharName && strlen(szCharName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stBan.m_szCharName, szCharName);
	if(szAccName && strlen(szAccName) <= AGPACHARACTER_MAX_ID_STRING)
		strcpy(stBan.m_szAccName, szAccName);

	EnumCallback(AGPMADMIN_CB_BAN, &stBan, pcsAgpdCharacter);

	return TRUE;
}

BOOL AgpmAdmin::OnOperationPing(UINT32 ulNID, PVOID pvPing)
{
	if(!pvPing)
		return FALSE;

	PVOID pvPingData = NULL;
	INT16 nSize = 0;

	m_csItemPacket.GetField(FALSE, pvPing, 0,
								&pvPingData, &nSize
								);

	if(!pvPingData || nSize == 0)
		return FALSE;

	stAgpdAdminPing* pstPing = static_cast<stAgpdAdminPing*>(pvPingData);
	EnumCallback(AGPMADMIN_CB_PING, pstPing, &ulNID);

	return TRUE;
}

// Client, Server
// 2004.01.30.
BOOL AgpmAdmin::OnOperationItem(AgpdCharacter* pcsCharacter, PVOID pvItem)
{
	PVOID pvItemOperation = NULL;
	INT16 nLength = 0;

	m_csItemPacket.GetField(FALSE, pvItem, 0,
								&pvItemOperation, &nLength
								);

	if(!pvItemOperation || nLength == 0)
		return FALSE;

	stAgpdAdminItemOperation* pstItemOperation;
	pstItemOperation = static_cast<stAgpdAdminItemOperation*>(pvItemOperation);
	if(!pstItemOperation)
		return FALSE;

	switch(pstItemOperation->m_cOperation)
	{
		case AGPMADMIN_ITEM_CREATE:
		case AGPMADMIN_ITEM_CONVERT:
		case AGPMADMIN_ITEM_DELETE:
		case AGPMADMIN_ITEM_OPTION_ADD:
		case AGPMADMIN_ITEM_OPTION_REMOVE:
		case AGPMADMIN_ITEM_UPDATE:
			EnumCallback(AGPMADMIN_CB_ITEM_CREATE, pstItemOperation, pcsCharacter);
			break;

		case AGPMADMIN_ITEM_RESULT:
			EnumCallback(AGPMADMIN_CB_ITEM_RESULT, pstItemOperation, pcsCharacter);
			break;
	}

	return TRUE;
}

BOOL AgpmAdmin::OnOperationUserPosition(AgpdCharacter* pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	return EnumCallback(AGPMADMIN_CB_USER_POSITION, pcsCharacter, NULL);
}

BOOL AgpmAdmin::OnOperationUserPositionStatistics(PVOID pvUserPosition)
{
	if(NULL == pvUserPosition)
		return FALSE;

	PACKET_AGPMADMIN_USER_POSITON *pPacket = (PACKET_AGPMADMIN_USER_POSITON*)pvUserPosition;
	if(NULL == pPacket)
		return FALSE;

	return EnumCallback(AGPMADMIN_CB_USER_POSITION_INFO , pPacket , NULL );
}

BOOL AgpmAdmin::OnOperationAdminClient(AgpdCharacter* pcsCharacter, PVOID pvIPInfo)
{
	stAgpdAdminServerInfo stServerInfo;
	ZeroMemory(&stServerInfo, sizeof(stServerInfo));
	CHAR* szServerIP = NULL;

	m_csServerInfoPacket.GetField(FALSE, pvIPInfo, 0,
								&szServerIP,
								&stServerInfo.m_lPort);
	if(!szServerIP)
		return FALSE;

	strncpy(stServerInfo.m_szServerIP, szServerIP, AGPMADMIN_MAX_IP_STRING);

	INT16 nAdminLevel = GetAdminLevel(pcsCharacter);

	if(nAdminLevel < AGPMADMIN_LEVEL_ADMIN)
		return FALSE;

	return EnumCallback(AGPMADMIN_CB_ADMIN_CLIENT_LOGIN , &stServerInfo , pcsCharacter );
}

BOOL AgpmAdmin::OnOperationTitle(AgpdCharacter* pcsCharacter, PVOID pvTitle)
{
	PVOID pvTitleOperation = NULL;
	INT16 nLength = 0;

	m_csTitlePacket.GetField(FALSE, pvTitle, 0,
								&pvTitleOperation, &nLength
								);

	if(!pvTitleOperation || nLength == 0)
		return FALSE;

	stAgpdAdminTitleOperation* pstTitleOperation;
	pstTitleOperation = static_cast<stAgpdAdminTitleOperation*>(pvTitleOperation);
	if(!pstTitleOperation)
		return FALSE;

	INT16 nAdminLevel = GetAdminLevel(pcsCharacter);

	if(nAdminLevel < AGPMADMIN_LEVEL_ADMIN)
		return FALSE;

	return EnumCallback(AGPMADMIN_CB_TITLE_EDIT , pstTitleOperation , pcsCharacter );
}


///////////////////////////////////////////////////////////////////////////////////
// Basic Process

AgpdAdmin* AgpmAdmin::GetADCharacter(AgpdCharacter* pData)
{
	if(m_pcsAgpmCharacter)
		return (AgpdAdmin*)m_pcsAgpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, (PVOID)pData);

	return NULL;
}

BOOL AgpmAdmin::ConAgpdAdminADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpmAdmin* pThis = (AgpmAdmin*)pClass;
	AgpdAdmin* pAttachedData = pThis->GetADCharacter((AgpdCharacter*)pData);

	if(!pAttachedData)
		return FALSE;

	// 초기화
	pAttachedData->m_lAdminCID = 0;
	pAttachedData->m_lAdminLevel = 0;
	
	return TRUE;
}

BOOL AgpmAdmin::DesAgpdAdminADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpmAdmin* pThis = (AgpmAdmin*)pClass;
	AgpdAdmin* pAttachedData = pThis->GetADCharacter((AgpdCharacter*)pData);

	if(!pAttachedData)
		return FALSE;

	return TRUE;
}

stAgpdAdminInfo* AgpmAdmin::GetAdminInfo(INT32 lCID)
{
	if(lCID < 0)
		return NULL;

	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if((*iterData)->m_lCID == lCID)
			return *iterData;

		iterData++;
	}

	return NULL;
}

INT32 AgpmAdmin::GetAdminIDList(list<INT32>* pList)
{
	if(m_listAdmin.size() == 0)
		return 0;

	INT32 lCount = 0;
	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if(*iterData)
		{
			pList->push_back((*iterData)->m_lCID);
			lCount++;
		}

		iterData++;
	}

	return lCount;
}

INT32 AgpmAdmin::GetAdminInfoList(list<stAgpdAdminInfo*>* pList)
{
	if(m_listAdmin.size() == 0)
		return 0;

	INT32 lCount = 0;
	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if(*iterData)
		{
			stAgpdAdminInfo* pstAdminInfo = new stAgpdAdminInfo;
			memcpy(pstAdminInfo, (*iterData), sizeof(stAgpdAdminInfo));

			pList->push_back(pstAdminInfo);
			lCount++;
		}

		iterData++;
	}

	return lCount;
}

// 2004.03.03. 인자로 넘어오는 값은 Account Name 이 되어야 한다.
// 약간 임시로 만든거다.
//
// 2004.03.09. 변경.
// DB 에서 읽은 Level 을 세팅한다. (Login Server 에서 한다.)
BOOL AgpmAdmin::SetAdminCharacter(AgpdCharacter* pcsAgpdCharacter, INT16 nLevel)
{
	// NULL 일때만 FALSE 로 리턴하고, 나머진 다 TRUE 로 리턴한다.
	if(!pcsAgpdCharacter)
		return FALSE;

	AgpdAdmin* pcsAttachedADChar = GetADCharacter(pcsAgpdCharacter);
	if(pcsAttachedADChar)
	{
		pcsAttachedADChar->m_lAdminCID = pcsAgpdCharacter->m_lID;
		pcsAttachedADChar->m_lAdminLevel = nLevel;
	}

	return TRUE;
}

BOOL AgpmAdmin::IsAdminCharacter(AgpdCharacter* pcsAgpdCharacter)
{
	if(!pcsAgpdCharacter)
		return FALSE;

	// 개발 및 테스트 편의를 위해서 모든 사용자가 admin이 됨, kelovon, 20051011
	if (m_pcsAgpmConfig && m_pcsAgpmConfig->IsAllAdmin())
		return TRUE;

	AgpdAdmin* pcsAttachedADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAttachedADChar)
		return FALSE;

	if(pcsAttachedADChar->m_lAdminLevel >= AGPMADMIN_LEVEL_ADMIN)
		return TRUE;
	else
		return FALSE;
}

BOOL AgpmAdmin::IsAdminCharacter(INT32 lCID)
{
	if(lCID == 0)
		return 0;

	// 개발 및 테스트 편의를 위해서 모든 사용자가 admin이 됨, kelovon, 20051011
	if (m_pcsAgpmConfig && m_pcsAgpmConfig->IsAllAdmin())
		return TRUE;

	INT16 nLevel = GetAdminLevel(lCID);
	return nLevel >= AGPMADMIN_LEVEL_ADMIN ? TRUE : FALSE;
}

INT16 AgpmAdmin::GetAdminLevel(AgpdCharacter* pcsAgpdCharacter)
{
	if(!pcsAgpdCharacter)
		return 0;

	// 개발 및 테스트 편의를 위해서 모든 사용자가 admin이 됨, kelovon, 20051011
	if (m_pcsAgpmConfig && m_pcsAgpmConfig->IsAllAdmin())
		return AGPMADMIN_LEVEL_5;		// 100 이면 충분하리라 본다. 2006.01.11. steeple

	AgpdAdmin* pcsAttachedADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAttachedADChar)
		return 0;

	return pcsAttachedADChar->m_lAdminLevel;
}

INT16 AgpmAdmin::GetAdminLevel(INT32 lCID)
{
	if(lCID == 0)
		return 0;

	// 개발 및 테스트 편의를 위해서 모든 사용자가 admin이 됨, kelovon, 20051011
	if (m_pcsAgpmConfig && m_pcsAgpmConfig->IsAllAdmin() )
		return 100;		// 100 이면 충분하리라 본다. 2006.01.11. steeple

	INT16 nLevel = 0;

	AdminLock();
	stAgpdAdminInfo* pstAdminInfo = GetAdminInfo(lCID);
	if(pstAdminInfo)
		nLevel = pstAdminInfo->m_lAdminLevel;

	AdminUnlock();

	return nLevel;
}






// Etc
UINT32 AgpmAdmin::GetCurrentTimeStamp()
{
	time_t timeval;
	(void)time(&timeval);
	
	return (UINT32)timeval;
}

INT32 AgpmAdmin::GetCurrentTimeDate(CHAR* szTimeBuf)
{
	if(!szTimeBuf)
		return 0;

	UINT32 lCurrentTimeStamp = GetCurrentTimeStamp();

	time_t timeval;
	struct tm* tm_ptr;

	timeval = (time_t)lCurrentTimeStamp;
	tm_ptr = localtime(&timeval);

	strftime(szTimeBuf, 32, "%Y%m%d%H%M", tm_ptr);
	return (INT32)strlen(szTimeBuf);
}

BOOL AgpmAdmin::DecodingItemPos(stAgpdAdminItem* pstItem, CHAR* szData, INT32 lDataLength)
{
	if(!pstItem || !szData || lDataLength < 1)
		return FALSE;

	sscanf(szData, "%ld,%ld,%ld", &pstItem->m_lLayer, &pstItem->m_lRow, &pstItem->m_lCol);
	return TRUE;
}

BOOL AgpmAdmin::DecodingItemConvertHistory(stAgpdAdminItem* pstItem, CHAR* szData, INT32 lDataLength)
{
	if(!pstItem || !szData || lDataLength < 1)
		return FALSE;

	sscanf(szData, "%d:%d:", &pstItem->m_lPhysicalConvertLevel, &pstItem->m_lNumSocket);

	INT32 i = 0, lNumSeparator = 0;
	for(i; i < lDataLength; ++i)
	{
		if(szData[i] == ':')
			lNumSeparator++;

		if(lNumSeparator == 2)
			break;
	}

	if(i == lDataLength)
		return FALSE;
	else if(i == lDataLength - 1)
		pstItem->m_lNumConvertedSocket = 0;
	else
	{
		i++;

		INT32 lTID[AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1];
		INT32 lNumConvert = 0;

		ZeroMemory(lTID, sizeof(INT32) * (AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1));

		while(i < lDataLength)
		{
			ASSERT(lNumConvert <= AGPDITEMCONVERT_MAX_WEAPON_SOCKET);
			if(lNumConvert > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
				break;

			sscanf(szData + i, "%d", &lTID[lNumConvert++]);

			for(; i < lDataLength; i++)
			{
				if(szData[i] == ',' || szData[i] == ':')
					break;
			}

			if(i == lDataLength)
				break;
			else
				i++;
		}

		pstItem->m_lNumConvertedSocket = lNumConvert;

		for(i = 0; i < lNumConvert; i++)
		{
			pstItem->m_stSocketAttr[i].m_lTID = lTID[i];
			//pstItem->m_stSocketAttr[i].m_pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(lTID[i]);

			//if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
			//	((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
			//	pcsAttachData->m_stSocketAttr[i].bIsSpiritStone	= TRUE;
			//else
			//	pcsAttachData->m_stSocketAttr[i].bIsSpiritStone	= FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmAdmin::DecodingItemOption(stAgpdAdminItem* pstItem, CHAR* szData, INT32 lDataLength)
{
	if(!pstItem || !szData || lDataLength < 1)
		return FALSE;

	INT32 ucOptionTID[AGPDITEM_OPTION_MAX_NUM];
	ZeroMemory(ucOptionTID, sizeof(ucOptionTID));

	int	retval = sscanf(szData, "%d,%d,%d,%d,%d", &ucOptionTID[0],
													 &ucOptionTID[1],
													 &ucOptionTID[2],
													 &ucOptionTID[3],
													 &ucOptionTID[4]);

	if(retval <= 0 || retval == EOF)
		return FALSE;

	for(INT32 i = 0; i < AGPDITEM_OPTION_MAX_NUM; i++)
		pstItem->m_alOptionTID[i] = ucOptionTID[i];

	return TRUE;
}

BOOL AgpmAdmin::DecodingItemSkillPlus(stAgpdAdminItem* pstItem, CHAR* szData, INT32 lDataLength)
{
	if(!pstItem || !szData || lDataLength < 1)
		return FALSE;

	INT32 ulSkillPlusTID[AGPMITEM_MAX_SKILL_PLUS_EFFECT];
	ZeroMemory(ulSkillPlusTID, sizeof(ulSkillPlusTID));

	int	retval = sscanf(szData, "%d,%d,%d", &ulSkillPlusTID[0],
											 &ulSkillPlusTID[1],
											 &ulSkillPlusTID[2]);

	if(retval <= 0 || retval == EOF)
		return FALSE;

	for(INT32 i = 0; i < AGPMITEM_MAX_SKILL_PLUS_EFFECT; i++)
		pstItem->m_alSkillPlusTID[i] = ulSkillPlusTID[i];

	return TRUE;
}







//////////////////////////////////////////////////////////////////////////////////
// Packet Make
PVOID AgpmAdmin::MakeAdminPacket(BOOL bPacket, INT16* pnPacketLength, UINT8 cType, PVOID pnOperation, INT32* plCID,
										PVOID pvLogin, PVOID pvSearch, PVOID pvCharData, PVOID pvCharEdit, PVOID pvCharItem,
										PVOID pvCharSkill, PVOID pvCustom, PVOID pvCharMove, PVOID pvBan, PVOID pvHelp,
										PVOID pvItem, PVOID pvServerInfo, PVOID pvTitle)
{
	return m_csPacket.MakePacket(bPacket, pnPacketLength, cType, pnOperation, plCID,
										pvLogin, pvSearch, pvCharData, pvCharEdit, pvCharItem,
										pvCharSkill, pvCustom, pvCharMove, pvBan, pvHelp,
										pvItem, pvServerInfo, pvTitle);
}

PVOID AgpmAdmin::MakeLoginPacket(INT16* pnPacketLength, INT8* pnFlag, INT32* plAdminCID, CHAR* szCharName, INT16* pnAdminLevel,
										INT16* pnServerID, CHAR* szAccName)
{
	return m_csLoginPacket.MakePacket(FALSE, pnPacketLength, 0, pnFlag, plAdminCID, szCharName, pnAdminLevel, pnServerID, szAccName);
}

PVOID AgpmAdmin::MakeLoginPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvLogin)
{
	INT8 cOperation = AGPMADMIN_PACKET_LOGIN;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, pvLogin, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

// 2004.03.10.
// Admin 이 다른 서버에 접속할 때 사용한다.
// Embedded Packet 은 LoginPacket 을 그대로 사용하고, 전체 패킷의 Operation 이 다른 방식이다.
PVOID AgpmAdmin::MakeConnectPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvLogin)
{
	INT8 cOperation = AGPMADMIN_PACKET_CONNECT;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, pvLogin, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

// 2004.03.24.
// Current User 를 보낸다. 얍삽하게도 Login Packet 을 그냥 사용한다.
PVOID AgpmAdmin::MakeNumCurrentUserPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvLogin)
{
	INT8 cOperation = AGPMADMIN_PACKET_NUM_CURRENT_USER;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, pvLogin, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmAdmin::MakeSearchPacket(INT16* pnPacketLength, INT8* pnType, INT8* pnField, INT32* plID, CHAR* szCharName, PVOID pvResult)
{
	return m_csSearchPacket.MakePacket(FALSE, pnPacketLength, 0, pnType, pnField, plID, szCharName, pvResult);
}

PVOID AgpmAdmin::MakeSearchPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvSearch)
{
	INT8 cOperation = AGPMADMIN_PACKET_SEARCH_CHARACTER;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, pvSearch, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

// Search Result 로 받은 패킷은 Search 패킷 안에 포함된다. 즉, Search Packet 도 만들어 줘야 된다.
PVOID AgpmAdmin::MakeSearchResultPacket(INT16* pnPacketLength, INT16* pnNum, INT16* pnCount, CHAR* szCharName, INT32* plCID,
										CHAR* szAccName, CHAR* szGuildID, INT32* plTID, INT32* plLevel, CHAR* szUT, CHAR* szCreDate,
										CHAR* szIP, INT32* plStatus)
{
	return m_csSearchResultPacket.MakePacket(FALSE, pnPacketLength, 0, pnNum, pnCount, szCharName, plCID, szAccName, szGuildID, plTID,
										plLevel, szUT, szCreDate, szIP, plStatus);
}

PVOID AgpmAdmin::MakeSearchResultPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvSearch)
{
	INT8 cOperation = AGPMADMIN_PACKET_SEARCH_RESULT;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, pvSearch, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmAdmin::MakeCharDataSubPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szCharName, CHAR* szAccName, UINT32 ulConnectedTimeStamp,
										CHAR* szIP, CHAR* szLastIP)
{
	return m_csSearchCharDataPacket.MakePacket(FALSE, pnPacketLength, 0, plCID, szCharName, szAccName, &ulConnectedTimeStamp, 
										szIP, szLastIP);
}

PVOID AgpmAdmin::MakeCharDataSubPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCharDataSub)
{
	INT8 cOperation = AGPMADMIN_PACKET_SEARCH_CHARDATA;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, NULL, pvCharDataSub, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmAdmin::MakeCharItemPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szCharName, INT8* pnPosition)
{
	return m_csSearchCharItemPacket.MakePacket(FALSE, pnPacketLength, 0, plCID, szCharName, pnPosition);
}

PVOID AgpmAdmin::MakeCharItemPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCharItem)
{
	INT8 cOperation = AGPMADMIN_PACKET_SEARCH_CHARITEM;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, NULL, NULL, NULL, pvCharItem, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmAdmin::MakeCharSkillPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szCharName, INT16* pnType)
{
	return m_csSearchCharSkillPacket.MakePacket(FALSE, pnPacketLength, 0, plCID, szCharName, pnType);
}

PVOID AgpmAdmin::MakeCharSkillPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCharSkill)
{
	INT8 cOperation = AGPMADMIN_PACKET_SEARCH_CHARSKILL;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, NULL, NULL, NULL, NULL, pvCharSkill, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmAdmin::MakeCustomPacket(INT16* pnPacketLength, INT32* plCustomType, PVOID pvCustomData, INT16 nDataLength)
{
	if(!plCustomType || !pvCustomData || nDataLength < 1)
		return NULL;

	return m_csSearchCustomPacket.MakePacket(FALSE, pnPacketLength, 0, plCustomType, pvCustomData, &nDataLength);
}

PVOID AgpmAdmin::MakeCustomPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCustom)
{
	INT8 cOperation = AGPMADMIN_PACKET_SEARCH_CUSTOM;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, NULL, NULL, NULL, NULL, NULL, pvCustom, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmAdmin::MakeCharMovePacket(INT16* pnPacketLength, CHAR* szCharName, CHAR* szTargetName, FLOAT* pfX, FLOAT* pfY, FLOAT* pfZ)
{
	return m_csCharMovePacket.MakePacket(FALSE, pnPacketLength, 0, szCharName, szTargetName, pfX, pfY, pfZ);
}

PVOID AgpmAdmin::MakeCharMovePacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCharMove)
{
	INT8 cOperation = AGPMADMIN_PACKET_CHARMOVE;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvCharMove, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmAdmin::MakeCharEditPacket(INT16* pnPacketLength, stAgpdAdminCharEdit* pstCharEdit, INT16 nSize, PVOID pvSkillData, INT16 nSkillDataSize)
{
	if(pvSkillData && nSkillDataSize > 0)
		return m_csCharEditPacket.MakePacket(FALSE, pnPacketLength, 0, (PVOID)pstCharEdit, &nSize, pvSkillData, &nSkillDataSize);
	else
		return m_csCharEditPacket.MakePacket(FALSE, pnPacketLength, 0, (PVOID)pstCharEdit, &nSize, NULL);
}

PVOID AgpmAdmin::MakeCharEditPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCharEdit)
{
	INT8 cOperation = AGPMADMIN_PACKET_CHAREDIT;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, NULL, NULL, pvCharEdit, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
}

PVOID AgpmAdmin::MakeBanPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szCharName, CHAR* szAccName, INT8* pnLogout, INT8* pnBanFlag,
										INT32* plChatBanStartTime, INT8* pnChatBanKeepTime, INT32* plCharBanStartTime, INT8* pnCharBanKeepTime,
										INT32* plAccBanStartTime, INT8* pnAccBanKeepTime)
{
	// 인자로 넘어온 plCID 는 쓰이지 않는다.
	return m_csBanPacket.MakePacket(FALSE, pnPacketLength, 0, szCharName, szAccName, pnLogout, pnBanFlag,
										plChatBanStartTime, pnChatBanKeepTime, plCharBanStartTime, pnCharBanKeepTime, plAccBanStartTime, pnAccBanKeepTime);
}

PVOID AgpmAdmin::MakeBanPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvBan)
{
	INT8 cOperation = AGPMADMIN_PACKET_BAN;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvBan, NULL, NULL, NULL, NULL);
}

PVOID AgpmAdmin::MakeItemPacket(INT16* pnPacketLength, stAgpdAdminItemOperation* pstItemOperation, INT16 nSize)
{
	return m_csItemPacket.MakePacket(FALSE, pnPacketLength, 0, (PVOID)pstItemOperation, &nSize);
}

PVOID AgpmAdmin::MakeItemPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvItem)
{
	INT8 cOperation = AGPMADMIN_PACKET_ITEM;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvItem, NULL, NULL);
}

PVOID AgpmAdmin::MakeServerInfoPacket(INT16* pnPacketLength, CHAR* szServerIP, INT16* pnPort)
{
	return m_csServerInfoPacket.MakePacket(FALSE, pnPacketLength, 0, szServerIP, pnPort);
}

PVOID AgpmAdmin::MakeServerInfoPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvServerInfo)
{
	INT8 cOperation = AGPMADMIN_PACKET_SERVERINFO;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvServerInfo, NULL);
}

PVOID AgpmAdmin::MakeTitlePacket(INT16* pnPacketLength, stAgpdAdminTitleOperation* pstTitleOperation, INT16 nSize)
{
	return m_csTitlePacket.MakePacket(FALSE, pnPacketLength, 0, (PVOID)pstTitleOperation, &nSize);
}

PVOID AgpmAdmin::MakeTitlePacket(INT16* pnPacketLength, INT32* plCID, PVOID pvTitle)
{
	INT8 cOperation = AGPMADMIN_PACKET_TITLE;
	return MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, plCID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvTitle);
}

PVOID AgpmAdmin::MakePingPacket(INT16* pnPacketLength, PVOID pvPing, INT16 nSize)
{
	INT8 cOperation = AGPMADMIN_PACKET_PING;
	PVOID pvPacket = m_csItemPacket.MakePacket(FALSE, pnPacketLength, 0, pvPing, &nSize);
	if(!pvPacket)
		return NULL;

	INT32 lCID = 1;
	PVOID pvPacket2 = MakeAdminPacket(TRUE, pnPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, &lCID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pvPacket, NULL, NULL);
	
	m_csItemPacket.FreePacket(pvPacket);
	return pvPacket2;
}


//////////////////////////////////////////////////////////////////////////

// Callback Reigster Function
BOOL AgpmAdmin::SetCallbackAddAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_ADD_ADMIN, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackRemoveAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_REMOVE_ADMIN, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackAddCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_ADD_CHARACTER, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_REMOVE_CHARACTER, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackNumCurrentUser(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_NUM_CURRENT_USER, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackUserPosition(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_USER_POSITION, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackUserPositionInfo(ApModuleDefaultCallBack pfCallback , PVOID pClass )
{
	return SetCallback(AGPMADMIN_CB_USER_POSITION_INFO,pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackServerInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_SERVERINFO, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackConnect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_CONNECT, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackSearchCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_SEARCH_CHARACTER, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackSearchResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_SEARCH_RESULT, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackCharData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_SEARCH_CHARDATA, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackCharItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_SEARCH_CHARITEM, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackCharSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_SEARCH_CHARSKILL, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackCustom(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_SEARCH_CUSTOM, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackCharMove(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_CHARMOVE, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackCharEdit(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_CHAREDIT, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackBan(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_BAN, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackItemCreate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_ITEM_CREATE, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackItemResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_ITEM_RESULT, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_GUILD, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackPing(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_PING, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackAdminClientLogin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_ADMIN_CLIENT_LOGIN, pfCallback, pClass);
}

BOOL AgpmAdmin::SetCallbackTitleEdit(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMADMIN_CB_TITLE_EDIT, pfCallback, pClass);
}


///////////////////////////////////////////////////////////////////////////////////////////
// Callback Execute Function
// CBRemoveAdmin 은 AgpmCharacter 로부터 불리게 된다.
BOOL AgpmAdmin::CBRemoveAdmin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgpmAdmin* pThis = (AgpmAdmin*)pClass;

	if(!pcsAgpdCharacter || !pThis)
		return FALSE;

	// PC 일 때만 하면 된다.
	if(!pThis->m_pcsAgpmCharacter->IsPC(pcsAgpdCharacter))
		return TRUE;

	AgpdAdmin* pcsAttachedAdmin = (AgpdAdmin*)pThis->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAttachedAdmin)
		return TRUE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveAdmin"));

	// 2004.03.25. 현재 접속자 수 계산을 위해서....
	if(pcsAttachedAdmin->m_lLoginCheck != 0)
		pThis->EnumCallback(AGPMADMIN_CB_REMOVE_CHARACTER, pcsAgpdCharacter, NULL);

	pcsAttachedAdmin->m_lLoginCheck = 0;

	if(pcsAttachedAdmin->m_lAdminCID < AGPMADMIN_LEVEL_ADMIN)
		return TRUE;

	pThis->OnOperationRemoveAdmin(pcsAgpdCharacter, pcsAgpdCharacter->m_lID);

	return TRUE;
}





///////////////////////////////////////////////////////////////////////////////////////////
// Debug
void AgpmAdmin::PrintAdminList()
{
	AdminLock();

	char szBuf[255];
	INT32 lCount = 0;

	sprintf(szBuf, "[Admin] Admin List Count : %d\n", m_listAdmin.size());
	OutputDebugString(szBuf);

	list<stAgpdAdminInfo*>::iterator iterData = m_listAdmin.begin();
	while(iterData != m_listAdmin.end())
	{
		if(!*iterData)
			break;

		sprintf(szBuf, "[Admin] Admin List : Count : %d, CID : %d, Name :%s, Level : %d\n",
								++lCount, (*iterData)->m_lCID, (*iterData)->m_szAdminName, (*iterData)->m_lAdminLevel);
		OutputDebugString(szBuf);
		iterData++;
	}

	AdminUnlock();
}


