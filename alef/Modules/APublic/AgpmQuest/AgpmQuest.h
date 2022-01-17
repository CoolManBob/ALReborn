// AgpmQuest.h: interface for the AgpmQuest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPMQUEST_H__9E4CA7EE_8FB3_4ED7_BF47_2BA30E616675__INCLUDED_)
#define AFX_AGPMQUEST_H__9E4CA7EE_8FB3_4ED7_BF47_2BA30E616675__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApModule.h"
#include "ApmMap.h"
#include "AgpdQuest.h"
#include "AgpdQuestTemplate.h"
#include "AgpaQuestTemplate.h"
#include "AgpaQuestGroup.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AuPacket.h"
#include "ApmEventManager.h"
#include "AgpmGrid.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmQuestD" )
#else
#pragma comment ( lib , "AgpmQuest" )
#endif
#endif

//@{ 2006/12/18 burumal
//#define AGPDQUEST_MAX_QUEST				800			//AGSDQUEST_EXPAND_BLOCK
#define AGPDQUEST_MAX_QUEST				1000			//AGSDQUEST_EXPAND_BLOCK
//@}
#define AGPDQUEST_MAX_QUEST_TEMPLATE	4000		//AGSDQUEST_EXPAND_BLOCK

#define LIST_BUFFER_SIZE 1024

enum EnumGroupFieldName
{
	AGPDQUEST_GROUP_FIELD_TID		= 0,
	AGPDQUEST_GROUP_FIELD_NPC_NAME,
	AGPDQUEST_GROUP_FIELD_GIVE,
	AGPDQUEST_GROUP_FIELD_CONFIRM,
	AGPDQUEST_GROUP_FIELD_CHECKPOINT
};

enum EnumAgpmQuestOperation
{
	AGPMQUEST_PACKET_OPERATION_REQUIRE_QUEST			= 0,
	AGPMQUEST_PACKET_OPERATION_REQUIRE_QUEST_RESULT,
	AGPMQUEST_PACKET_OPERATION_QUEST_COMPLETE,
	AGPMQUEST_PACKET_OPERATION_QUEST_COMPLETE_RESULT,
	AGPMQUEST_PACKET_OPERATION_INIT_QUEST,
	AGPMQUEST_PACKET_OPERATION_INIT_FLAGS,
	AGPMQUEST_PACKET_OPERATION_CANCEL,
	AGPMQUEST_PACKET_OPERATION_CANCEL_RESULT,
	AGPMQUEST_PACKET_OPERATION_INVENTORY_FULL,
	AGPMQUEST_PACKET_OPERATION_QUEST_INVENTORY_FULL,
	AGPMQUEST_PACKET_OPERATION_UPDATE_QUEST,
	AGPMQUEST_PACKET_OPERATION_CHECKPOINT,
	AGPMQUEST_PACKET_OPERATION_CHECKPOINT_RESULT,
	AGPMQUEST_PACKET_OPERATION_MAX
};

struct CBQuestParam
{
	AgpdCharacter	*pcsAgpdCharacter;
	ApdEvent		*pcsEvent;
	INT32			lQuestTID;
	INT32			lCheckPointIndex;
	BOOL			bResult;
	PVOID			pvPacket;
	BYTE*			pBuffer1;
	UINT16			nBufferLength1;
	BYTE*			pBuffer2;
	UINT16			nBufferLength2;
	BYTE*			pBuffer3;
	UINT16			nBufferLength3;
	
	CBQuestParam() : pcsAgpdCharacter(NULL), pcsEvent(NULL), lQuestTID(0), lCheckPointIndex(0), 
		bResult(FALSE), pvPacket(NULL),	pBuffer1(NULL), nBufferLength1(0),
		pBuffer2(NULL), nBufferLength2(0), pBuffer3(NULL), nBufferLength3(0)
	{}
};


class AgpmQuest : public ApModule
{
public:
	AgpaQuestTemplate m_csQuestTemplate;
	AgpaQuestGroup m_csQuestGroup;

	AgpmCharacter	*m_pcsAgpmCharacter;
	AgpmFactors		*m_pcsAgpmFactors;
	AgpmItem		*m_pcsAgpmItem;
	AgpmSkill		*m_pcsAgpmSkill;
	ApmEventManager	*m_pcsApmEventManager;
	AgpmGrid		*m_pcsAgpmGrid;

	INT32			m_lIndexAttachData;

	AuPacket		m_csPacket;
	AuPacket		m_csPacketQuest;

public:
	AgpmQuest();
	virtual ~AgpmQuest();

	// Virtual Functions
	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle(UINT32 ulClockCount);
	BOOL OnDestroy();

	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	
	static BOOL AgpdQuestConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AgpdQuestDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	// Stream Functions
	BOOL StreamReadTemplate(CHAR *szFile, BOOL bDecryption);
	BOOL StreamReadGroup(CHAR *szFile, BOOL bDecryption);

	BOOL PrevNextQuestDataSetting();

	BOOL SetMaxTemplateCount(INT32 lCount);
	BOOL SetMaxGroupCount(INT32 lCount);

	BOOL SetCallbackRequireQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRequireQuestResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCompleteQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCompleteQuestResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCancelQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCancelQuestResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackInventoryFull(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackQuestInventoryFull(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCheckPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCheckPointResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// Quest Group Read Functions
	BOOL StreamReadQuestList(list< INT32 >* pList, CHAR* szBuffer);

	BOOL StreamReadCheckPoint(list< AgpdQuestCheckPoint >* pList, CHAR* szBuffer);

	// Convert To Index
	INT32 ConvertToEvaluationIndex(LPCTSTR lpStr);
	INT32 ConvertToRaceIndex(LPCTSTR lpStr);
	INT32 ConvertToClassIndex(LPCTSTR lpStr);
	INT32 ConvertToGenderIndex(LPCTSTR lpStr);
	INT32 ConvertToQuestTypeIndex(LPCTSTR lpStr);

	// Quest Template
	AgpdQuestTemplate*	AddTemplate(INT32 lTID);
	BOOL				DeleteTemlate(INT32 lTID);
	AgpdQuestTemplate*	GetTemplateSequence(INT32 *plIndex);

	AgpdQuestTemplate* CreateTemplate();
	BOOL DestroyTemplate(AgpdQuestTemplate *pcsTemplate);

	// Quest Group
	AgpdQuestGroup* AddGroup(INT32 lTID);
	BOOL			DeleteGroup(INT32 lTID);
	AgpdQuestGroup* GetGroupSequence(INT32 *plIndex);
	AgpdQuestGroup* CreateGroup();
	BOOL			DestroyGroup(AgpdQuestGroup* pcsGroup);

	// Attach data
	AgpdQuest* GetAttachAgpdQuestData(AgpdCharacter *pcsCharacter);

	BOOL IsHaveQuest(AgpdCharacter *pcsCharacter, INT32 lQuestTID);

	// Make Packet
	PVOID MakePacketRequireQuest(INT16 *pnPacketLength, ApdEvent *pEvent, INT32 lCID, INT32 lQuestTID);
	PVOID MakePacketRequireQuestResult(INT16 *pnPacketLength, INT32 lCID, BOOL bResult, INT32 lQuestTID, PVOID pvEmbeddedPacket);

	PVOID MakePacketQuestComplete(INT16 *pnPacketLength, ApdEvent *pEvent, INT32 lCID, INT32 lQuestTID);
	PVOID MakePacketQuestCompleteResult(INT16 *pnPacketLength, INT32 lCID, BOOL bResult, INT32 lQuestTID);
	
	PVOID MakePacketQuestCancel(INT16 *pnPacketLength, INT32 lCID, INT32 lQuestTID);
	PVOID MakePacketQuestCancelResult(INT16 *pnPacketLength, INT32 lCID, BOOL bResult, INT32 lQuestTID);

	PVOID MakePacketInitFlag(INT16 *pnPacketLength, INT32 lCID, AgpdQuest* pcsAgpdQuest);
	PVOID MakePacketInitQuest(INT16 *pnPacketLength, INT32 lCID, AgpdCurrentQuest* pCurrentQuest);

	PVOID MakePacketInventoryFull(INT16 *pnPacketLength, INT32 lCID, INT32 lQuestTID);
	PVOID MakePacketQuestInventoryFull(INT16 *pnPacketLength, INT32 lCID, INT32 lQuestTID);

	PVOID MakePacketUpdateQuest(INT16 *pnPacketLength, INT32 lCID, INT32 lQuestTID, INT32 lItemTID, AgpdCurrentQuest* pCurrentQuest);

	PVOID MakePacketEmbeddedQuest(INT32 lQuestID, INT32 lParam1, INT32 lParam2, INT32 lItemTID, CHAR* szNPCName);
	BOOL GetFieldEmbeddedPacket(PVOID pvPacket, INT32 &lQuestID, INT32 &lParam1, INT32 &lParam2, INT32 &lItemTID, CHAR*& szNPCName);

	PVOID MakePacketRequireCheckPoint(INT16 *pnPacketLength, INT32 lCID, ApdEvent *pEvent, INT32 lQuestTID, INT32 lCheckPointIndex);
//	PVOID MakePacketCheckPointResult(INT16 *pnPacketLength, INT32 lCID, INT32 lQuestTID, INT32 lCheckPointIndex);

	// 조건 검사
	BOOL EvaluationStartCondition(AgpdCharacter *pcsAgpdCharacter, INT32 lQuestTID);
	BOOL EvaluationCompleteCondition(AgpdCharacter *pcsAgpdCharacter, INT32 lQuestTID);

	void CharReplace(CHAR* szBuffer, CHAR cOld, CHAR cNew);
};

#endif // !defined(AFX_AGPMQUEST_H__9E4CA7EE_8FB3_4ED7_BF47_2BA30E616675__INCLUDED_)
