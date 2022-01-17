#if !defined(__AGSMAREACHATTING_H__)
#define	__AGSMAREACHATTING_H__

#include "ApArray.h"

#include "AgpmLog.h"
#include "AgpmCharacter.h"
#include "AgpmAreaChatting.h"
#include "AgpmAdmin.h"
#include "AgsEngine.h"

#include "AgsmCharacter.h"
#include "AgsmItem.h"
#include "AgsmChatting.h"
#include "AgsmGlobalChatting.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmAreaChattingD" )
#else
#pragma comment ( lib , "AgsmAreaChatting" )
#endif
#endif

const INT32 AGSMAREACHATTING_MAX_REGIONTEMPLATE = AGPMADMIN_MAX_REGION_INDEX;

#define	AGSMAREACHATTING_MAX_CHAT_SIZE				128

typedef enum	_AgsmAreaChatType	{
	AGSMAREACHAT_TYPE_REGION				= 1,
	AGSMAREACHAT_TYPE_RACE,
	AGSMAREACHAT_TYPE_ALL,
	AGSMAREACHAT_TYPE_GLOBAL,
	AGSMAREACHAT_TYPE_ARCHLORD,
} AgsmAreaChatType;

typedef struct _AgsmChatReceiveBuffer {
	AgsmAreaChatType	eChatType;
	INT32	lSendCID;
	UINT32	lSize;
	CHAR	szPacketBuffer[1024 + 1];
	CHAR	szSenderName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	CHAR	szServerName[AGSM_MAX_SERVER_NAME + 1];
	BOOL	bIsNotify;
} AgsmChatReceiveBuffer;

class AgsdAreaChatting {
public:
	UINT32		m_ulLastReceivedTimeMSec;
};

class AgpmBattleGround;
class AgsmSystemMessage;

class AgsmAreaChatting : public AgsModule {
private:
	AgpmLog				*m_pcsAgpmLog;
	ApmMap				*m_pcsApmMap;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmAreaChatting	*m_pcsAgpmAreaChatting;
	AgpmAdmin			*m_pcsAgpmAdmin;

	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmItem			*m_pcsAgsmItem;
	AgsmChatting		*m_pcsAgsmChatting;
	AgsmServerManager2	*m_pcsAgsmServerManager2;
	AgsmGlobalChatting	*m_pcsAgsmGlobalChatting;
	AgpmBattleGround*	m_pagpmBattleGround;
	AgsmSystemMessage*	m_pagsmSystemMessage;

	stTHREADOBJECT		m_stSendThread;

	BOOL				m_bDestroyThread;

	ApAdmin				m_acsRegionAdmin[AGSMAREACHATTING_MAX_REGIONTEMPLATE];
	//ApAdmin				m_acsRaceAdmin[AURACE_TYPE_MAX];
	ApAdmin				m_acsRaceAdmin[12];
	ApAdmin				m_csAllPlayerAdmin;

	INT32				m_lMaxUser;
	INT32				m_lMaxClient;

	ApMutualEx			m_Mutex;
	//ApArray<USE_AUTOLOCK>	m_csReceiveBuffer;

	ApMemoryPool		m_csReceivePool;
	AuCircularBuffer	m_csSendCircularBuffer;

	std::queue<AgsmChatReceiveBuffer*>	m_stlQueue;

	INT16				m_nIndexAttachCharacterData;

public:
	AgsmAreaChatting();
	~AgsmAreaChatting();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();

	static BOOL	CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAddCharacterToMap(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRemoveCharacterFromMap(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBRecvChatMessage(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRecvRaceChatMessage(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRecvAllChatMessage(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRecvArchlordMessage(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBRecvBroadCastMessage(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	SetMaxRegionChar(INT32 lMaxUser);
	BOOL	SetMaxClient(INT32 lMaxClilent);

	//BOOL	OnDisconnect(CHAR *szAccountName, UINT32 ulNID);

	static UINT WINAPI SendChatThread(PVOID pvParam);

	PVOID	MakeAreaChatPacket(AgsmChatReceiveBuffer *pcsBuffer, AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, CHAR *szHeader = NULL, UINT32 ulTextColorRGB = 0);
	PVOID	MakeAreaChatPacket(BOOL bIsNotify, CHAR *pszSenderName, CHAR *pszServerName, CHAR *pszMessage, UINT16 unMessageLength, INT16 *pnPacketLength, CHAR *szHeader = NULL, UINT32 ulTextColorRGB = 0);

	BOOL	SendPacketToRegion(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength);
	BOOL	SendPacketToRace(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength);
	BOOL	SendPacketToAll(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength, BOOL bIsArchlordMessage = FALSE);

	BOOL	BroadCastMessage(AgsmChatReceiveBuffer *pcsBuffer);

	AgsdAreaChatting*	GetAttachCharacterData(AgpdCharacter *pcsCharacter);

	BOOL	CheckColorText(CHAR *szMessage, UINT16 unMessageLength);

	ApAdmin* GetRegionAdmin() { return m_acsRegionAdmin; }
};

#endif	//__AGSMAREACHATTING_H__