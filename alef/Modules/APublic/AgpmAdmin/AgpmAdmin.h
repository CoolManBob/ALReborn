// AgpmAdmin.h
// (C) NHN - ArchLord Development Team
// steeple, 2003. 09. 04.

#ifndef _AGPMADMIN_H_
#define _AGPMADMIN_H_

#include "ApModule.h"
#include "AuPacket.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpdAdmin.h"
#include "AgpmConfig.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgpmAdminD.lib")
#else
	#pragma comment(lib, "AgpmAdmin.lib")
#endif
#endif
//@} Jaewon


class AgpmAdmin : public ApModule
{
private:
	AgpmCharacter* m_pcsAgpmCharacter;
	AgpmItem* m_pcsAgpmItem;
	AgpmConfig* m_pcsAgpmConfig;

	INT16 m_nIndexADCharacter;

	list<stAgpdAdminInfo*> m_listAdmin;
	
	ApMutualEx m_csAdminLock;	// Admin List 관리

public:
	INT32 m_lConnectableAccountLevel;

public:
	AuPacket m_csPacket;
	AuPacket m_csLoginPacket;
	AuPacket m_csSearchPacket;
	AuPacket m_csSearchResultPacket;
	AuPacket m_csSearchCharDataPacket;
	AuPacket m_csSearchCharItemPacket;
	AuPacket m_csSearchCharSkillPacket;
	AuPacket m_csSearchCharSkillSubPacket;
	AuPacket m_csSearchCustomPacket;
	AuPacket m_csCharMovePacket;
	AuPacket m_csCharEditPacket;
	AuPacket m_csBanPacket;
	AuPacket m_csItemPacket;
	AuPacket m_csServerInfoPacket;
	AuPacket m_csTitlePacket;

public:
	AgpmAdmin();
	virtual ~AgpmAdmin();

	// Virtual Function 들
	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle(UINT32 ulClockCount);
	BOOL OnDestroy();

	void AdminLock();
	void AdminUnlock();

	// Admin Packet 을 받는다.
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	// Admin Add / Remove 
	BOOL OnOperationLogin(AgpdCharacter* pcsAgpdCharacter, PVOID pvLogin);
	BOOL OnOperationAddAdmin(AgpdCharacter* pcsAgpdCharacter, stAgpdAdminInfo* pstAdminInfo);
	BOOL OnOperationRemoveAdmin(AgpdCharacter* pcsAgpdCharacter, INT32 lRemoveAdminID);

	// Num of Current User
	BOOL OnOperationNumCurrentUser(AgpdCharacter* pcsAgpdCharacter, PVOID pvLogin);
	
	// Connect Server
	BOOL OnOperationConnect(AgpdCharacter* pcsAgpdCharacter, PVOID pvLogin, UINT32 ulNID);

	// Server Info
	BOOL OnOperationServerInfo(AgpdCharacter* pcsAgpdCharacter, PVOID pvServerInfo);

	// Char Operation
	BOOL OnOperationSearch(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearch);
	BOOL OnOperationSearchResult(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearch);
	BOOL OnOperationSearchCharData(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearchCharData);
	BOOL OnOperationSearchCharItem(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearchCharItem);
	BOOL OnOperationSearchCharSkill(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearchCharSkill);
	BOOL OnOperationSearchCustom(AgpdCharacter* pcsAgpdCharacter, PVOID pvSearchCustom);
	BOOL OnOperationCharMove(AgpdCharacter* pcsAgpdCharacter, PVOID pvCharMove);
	BOOL OnOperationCharEdit(AgpdCharacter* pcsAgpdCharacter, PVOID pvCharEdit);

	// Ban
	BOOL OnOperationBan(AgpdCharacter* pcsAgpdCharacter, PVOID pvBan);

	// Item
	BOOL OnOperationItem(AgpdCharacter* pcsCharacter, PVOID pvItem);

	// Guild - 2005.04.25. steeple
	BOOL OnOperationGuild(AgpdCharacter* pcsAgpdCharacter, PVOID pvGuild);

	// Ping - 2007.07.30. steeple
	BOOL OnOperationPing(UINT32 ulNID, PVOID pvPing);

	// User Position
	BOOL OnOperationUserPosition(AgpdCharacter* pcsCharacter);

	// Process User Position Statistics
	BOOL OnOperationUserPositionStatistics(PVOID pvUserPosition);

	// 어드민 클라이언트 관련 프로세싱
	BOOL OnOperationAdminClient(AgpdCharacter* pcsCharacter, PVOID pvIPInfo);

	// 타이틀 관련 프로세스.
	BOOL OnOperationTitle(AgpdCharacter* pcsCharacter, PVOID pvTitle);

	// Attached Data
	AgpdAdmin* GetADCharacter(AgpdCharacter* pData);
	// Attached Data Constructor / Destructor
	static BOOL ConAgpdAdminADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesAgpdAdminADChar(PVOID pData, PVOID pClass, PVOID pCustData);

	// Admin Data
	stAgpdAdminInfo* GetAdminInfo(INT32 lCID);
	INT32 GetAdminIDList(list<INT32>* pList);
	INT32 GetAdminInfoList(list<stAgpdAdminInfo*>* pList);
	list<stAgpdAdminInfo*>* GetAdminInfoList() { return &m_listAdmin; }

	BOOL SetAdminCharacter(AgpdCharacter* pcsAgpdCharacter, INT16 nLevel);	// 2004.03.09 변경. DB 에서 Level 을 읽는다.
	BOOL IsAdminCharacter(AgpdCharacter* pcsAgpdCharacter);
	BOOL IsAdminCharacter(INT32 lCID);
	INT16 GetAdminLevel(AgpdCharacter* pcsAgpdCharacter);
	INT16 GetAdminLevel(INT32 lCID);
	
	// Etc...
	UINT32 GetCurrentTimeStamp();
	INT32 GetCurrentTimeDate(CHAR* szTimeBuf);

	// DB Data Convert
	BOOL DecodingItemPos(stAgpdAdminItem* pstItem, CHAR* szData, INT32 lDataLength);
	BOOL DecodingItemConvertHistory(stAgpdAdminItem* pstItem, CHAR* szData, INT32 lDataLength);
	BOOL DecodingItemOption(stAgpdAdminItem* pstItem, CHAR* szData, INT32 lDataLength);
	BOOL DecodingItemSkillPlus(stAgpdAdminItem* pstItem, CHAR* szData, INT32 lDataLength);





	// Packet Make
	PVOID MakeAdminPacket(BOOL bPacket, INT16* pnPacketLength, UINT8 cType, PVOID pnOperation, INT32* plCID,
											PVOID pvLogin, PVOID pvSearch, PVOID pvCharData, PVOID pvCharEdit, PVOID pvCharItem,
											PVOID pvCharSkill, PVOID pvCustom, PVOID pvCharMove, PVOID pvBan, PVOID pvHelp,
											PVOID pvItem, PVOID pvServerInfo, PVOID pvTitle);
	
	PVOID MakeLoginPacket(INT16* pnPacketLength, INT8* pnFlag, INT32* plAdminCID, CHAR* szCharName, INT16* pnAdminLevel, INT16* pnServerID,
											CHAR* szAccName);
	PVOID MakeLoginPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvLogin);
	PVOID MakeConnectPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvLogin);	// 2004.03.10.
	PVOID MakeNumCurrentUserPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvLogin);	// 2004.03.24.

	PVOID MakeSearchPacket(INT16* pnPacketLength, INT8* pnType, INT8* pnField, INT32* plID, CHAR* szCharName, PVOID pvResult);
	PVOID MakeSearchPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvSearch);

	PVOID MakeSearchResultPacket(INT16* pnPacketLength, INT16* pnNum, INT16* pnCount, CHAR* szCharName, INT32* plCID,
											CHAR* szAccName, CHAR* szGuildID, INT32* plTID, INT32* plLevel, CHAR* szUT, CHAR* szCreDate,
											CHAR* szIP, INT32* plStatus);
	PVOID MakeSearchResultPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvSearch);	// SearchResult 는 Search 안에 들어가 있다.

	PVOID MakeCharDataSubPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szCharName, CHAR* szAccName, UINT32 ulConnectedTimeStamp,
											CHAR* szIP, CHAR* szLastIP);
	PVOID MakeCharDataSubPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCharDataSub);

	PVOID MakeCharItemPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szCharName, INT8* pnPosition);
	PVOID MakeCharItemPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCharItem);

	PVOID MakeCharSkillPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szCharName, INT16* pnType);
	PVOID MakeCharSkillPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCharSkill);

	PVOID MakeCustomPacket(INT16* pnPacketLength, INT32* plCustomType, PVOID pvCustomData, INT16 nDataLength);
	PVOID MakeCustomPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCustom);
	
	PVOID MakeCharMovePacket(INT16* pnPacketLength, CHAR* szCharName, CHAR* szTargetName, FLOAT* pfX, FLOAT* pfY, FLOAT* pfZ);
	PVOID MakeCharMovePacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCharMove);

	PVOID MakeCharEditPacket(INT16* pnPacketLength, stAgpdAdminCharEdit* pstCharEdit, INT16 nSize, PVOID pvSkillData = NULL, INT16 nSkillDataSize = 0);
	PVOID MakeCharEditPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvCharEdit);

	PVOID MakeBanPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szCharName, CHAR* szAccName, INT8* pnLogout, INT8* pnBanFlag,
											INT32* plChatBanStartTime, INT8* pnChatBanKeepTime, INT32* plCharBanStartTime, INT8* pnCharBanKeepTime,
											INT32* plAccBanStartTime, INT8* pnAccBanKeepTime);
	PVOID MakeBanPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvBan);

	PVOID MakeItemPacket(INT16* pnPacketLength, stAgpdAdminItemOperation* pstItemOperation, INT16 nSize);
	PVOID MakeItemPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvItem);

	PVOID MakeServerInfoPacket(INT16* pnPacketLength, CHAR* szServerIP, INT16* pnPort);
	PVOID MakeServerInfoPacket(INT16* pnPacketLength, INT32* plCID, PVOID pvServerInfo);

	PVOID MakeTitlePacket(INT16* pnPacketLength, stAgpdAdminTitleOperation* pstTitleOperation, INT16 nSize);
	PVOID MakeTitlePacket(INT16* pnPacketLength, INT32* plCID, PVOID pvTitle);
	
	PVOID MakePingPacket(INT16* pnPacketLength, PVOID pvPing, INT16 nSize);
		


	// Callback 을 등록해 주는 함수.
	BOOL SetCallbackAddAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackServerInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackConnect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	BOOL SetCallbackAddCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackNumCurrentUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackSearchCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSearchResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCharData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCharItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCharSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCustom(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCharMove(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCharEdit(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBan(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackItemCreate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackItemResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackPing(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUserPosition(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUserPositionInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackAdminClientLogin(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackTitleEdit(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	// Callback static 함수
	static BOOL CBRemoveAdmin(PVOID pData, PVOID pClass, PVOID pCustData);	// 2004.03.09. 재추가


	
	// Debug
	void PrintAdminList();
};

#endif //_AGPMADMIN_H_
