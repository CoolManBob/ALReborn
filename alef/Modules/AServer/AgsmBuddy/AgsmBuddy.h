#pragma once

#include "AgsEngine.h"
#include "AgpmBuddy.h"

class AgsmCharacter;
class AgsmCharManager;
class AgpdCharacter;
class AgpmCharacter;
class stBuddyRowset;
class ApmMap;
class AgsmPrivateTrade;
class AgsmParty;

enum EnumAgsmBuddyDBOperation
{
	AGSMBUDDY_DB_NONE = 0,
	AGSMBUDDY_DB_INSERT,	// 버디 추가
	AGSMBUDDY_DB_REMOVE,	// 버디 삭제
	AGSMBUDDY_DB_REMOVE2,	// 버디 삭제 offline
	AGSMBUDDY_DB_OPTIONS,	// 옵션 업데이트
	AGSMBUDDY_DB_SELECT,	// 버디 리스트 요청
	AGSMBUDDY_DB_OPTIONS2,	// 옵션 업데이트 (상대가 오프라인)
};

class AgsmBuddy : public AgsModule
{
private:
	AgpmBuddy*			m_pcsAgpmBuddy;
	AgpmCharacter*		m_pcsAgpmCharacter;
	AgsmCharacter*		m_pcsAgsmCharacter;
	AgsmCharManager*	m_pcsAgsmCharManager;
	ApmMap*				m_pcsApmMap;
	AgsmPrivateTrade*	m_pcsAgsmPrivateTrade;
	AgsmParty*			m_pcsAgsmParty;

public:
	AgsmBuddy();
	virtual ~AgsmBuddy();

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnIdle2(UINT32 ulClockCount);
	virtual BOOL OnDestroy();

	// database operation
	BOOL SetCallbackDBInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBRemove(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBRemove2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBOptions(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBOptions2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDBSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// callback functions
	static BOOL CBAddBuddy(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAddRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAddReject(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveBuddy(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBOptionsBuddy(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRegion(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBMentorRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBMentorDelete(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBMentorRequestAccept(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBMentorRequestReject(PVOID pData, PVOID pClass, PVOID pCustData);

	// 사용자 접속 관련
	static BOOL CBCharConnected(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCharDisconnected(PVOID pData, PVOID pClass, PVOID pCustData);

	// ban list check option 관련
	static BOOL CBPartyCheck(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBPrivateTradeCheck(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL PartyCheck(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetChar);
	BOOL PrivateTradeCheck(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetChar);

	// packet operation
	BOOL OperationAddBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL OperationAddRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL OperationAddReject(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL OperationRemoveBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL OperationOptionsBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL OperationRegion(AgpdCharacter* pcsCharacter, AgpdBuddyRegion* pcsBuddyRegion);

	BOOL OperationMentorRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL OperationMentorDelete(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL OperationMentorRequestAccept(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL OperationMentorRequestReject(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);

	BOOL SendAddBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL SendAddRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL SendAddReject(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL SendRemoveBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL SendOptionsBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL SendMsgCodeBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement, EnumAgpmBuddyMsgCode eMsgCode);
	BOOL SendMyBuddyList(AgpdCharacter* pcsCharacter);
	BOOL SendOnlineAllUser(AgpdCharacter* pcsCharacter, BOOL bOnline);
	BOOL SendOnlineAllBuddy(AgpdCharacter* pcsCharacter, BOOL bOnline);
	BOOL SendBuddyOnlineToMe(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	BOOL SendRegion(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTargetChar, INT32 lRegionIndex);
	BOOL SendInitEnd(AgpdCharacter* pcsCharacter);

	//Mentor 패킷send
	BOOL SendMentorRequest(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL SendMentorDelete(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL SendMentorRequestAccept(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL SendMentorRequestReject(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL SendMentorUIOpen(AgpdCharacter *pcsCharacter);

	BOOL ReceiveBuddyDataFromRelay(AgpdCharacter* pcsCharacter, stBuddyRowset *pRowset);

	EnumAgpmBuddyMsgCode IsValidBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	EnumAgpmBuddyMsgCode IsValidMentorRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	EnumAgpmBuddyMsgCode IsValidMentorAccept(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
	EnumAgpmBuddyMsgCode IsValidMentorDelete(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement);
};