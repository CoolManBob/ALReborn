// AgpmEventTeleport.h: interface for the AgpmEventTeleport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPMEVENTTELEPORT_H__9A67D2B3_5469_4077_8C59_1843C2477176__INCLUDED_)
#define AFX_AGPMEVENTTELEPORT_H__9A67D2B3_5469_4077_8C59_1843C2477176__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApAdmin.h"
#include "ApModule.h"
#include "AuPacket.h"
#include "AuGenerateID.h"

#include "ApmEventManager.h"

#include "AgpdEventTeleport.h"

#include "AgpmSiegeWar.h"
#include "AgpmFactors.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmEventTeleportD" )
#else
#pragma comment ( lib , "AgpmEventTeleport" )
#endif
#endif


//#define	AGPMEVENT_TELEPORT_PACKET_TYPE		(APMEVENT_FLAG_PACKET_TYPE | APDEVENT_FUNCTION_TELEPORT)

/*
#define AGPMTELEPORT_STREAM_NAME_LIST		"TeleportList"

#define AGPMTELEPORT_STREAM_NAME_START		"TeleportStart"
#define AGPMTELEPORT_STREAM_NAME_TARGET		"TeleportTarget"
#define AGPMTELEPORT_STREAM_NAME_END		"TeleportEnd"
*/

// teleport group stream
//////////////////////////////////////////////////////////////
#define	AGPMTELEPORT_STREAM_NAME_DESCRIPTION	"Description"

// teleport point stream
//////////////////////////////////////////////////////////////
#define	AGPMTELEPORT_STREAM_NAME_RADIUS_MIN		"RadiusMin"
#define	AGPMTELEPORT_STREAM_NAME_RADIUS_MAX		"RadiusMax"
#define	AGPMTELEPORT_STREAM_NAME_POINT_TYPE		"PointTargetType"
#define	AGPMTELEPORT_STREAM_NAME_POINT_POS		"PointPos"
#define	AGPMTELEPORT_STREAM_NAME_POINT_BASE		"PointBase"
#define AGPMTELEPORT_STREAM_NAME_POINT_EVENT	"PointEvent"
#define	AGPMTELEPORT_STREAM_NAME_USE_TYPE		"UseType"	//종족별.. 누가 쓸지..

#define	AGPMTELEPORT_STREAM_NAME_POINT_GROUP_NAME	"Group"
#define	AGPMTELEPORT_STREAM_NAME_POINT_TARGET_NAME	"TargetGroup"

#define	AGPMTELEPORT_STREAM_NAME_POINT_REGION_TYPE	"RegionType"
#define	AGPMTELEPORT_STREAM_NAME_POINT_SPECIAL_TYPE	"SpecialType"

// teleport event stream
//////////////////////////////////////////////////////////////
#define	AGPMTELEPORT_STREAM_NAME_POINT_NAME		"PointName"
#define AGPMTELEPORT_STREAM_NAME_EVENT_END		"TeleportEnd"

#define	AGPMTELEPORT_MAX_USE_RANGE				1600

typedef enum AgpmEventTeleportCallbackPoint			// Callback ID in Character Module 
{
	AGPMEVENT_TELEPORT_CB_ID_TELEPORT					= 0,
	AGPMEVENT_TELEPORT_CB_ID_CALC_POS,
	AGPMEVENT_TELEPORT_CB_ID_RECEIVED_REQUEST_RESULT,
	AGPMEVENT_TELEPORT_CB_ID_RECEIVED_TELEPORT_START,
	AGPMEVENT_TELEPORT_CB_ID_RECEIVED_TELEPORT_CANCELED,
	AGPMEVENT_TELEPORT_CB_ID_CHECK_USABLE_TELEPORT,
	AGPMEVENT_TELEPORT_CB_ID_RETURN_TOWN,
	AGPMEVENT_TELEPORT_CB_ID_RECEIVE_TELEPORT_LOADING,
	AGPMEVENT_TELEPORT_CB_ID_SYNC_TELEPORT_LOADING,
	AGPMEVENT_TELEPORT_CB_ID_TELEPORT_LOG,
	AGPMEVENT_TELEPORT_CB_ID_ARCHLORD_MESSAGE_ID,
	AGPMEVENT_TELEPORT_CB_ID_GET_BATTLEGROUND_STATE,
	AGPMEVENT_TELEPORT_CB_ID_BATTLEGROUND_TELEPORT_LOG,
} AgpmEventTeleportCallbackPoint;


typedef enum _AgpmEventTeleportDataType {
	AGPMEVENT_TELEPORT_DATA_TYPE_POINT					= 0,
	AGPMEVENT_TELEPORT_DATA_TYPE_GROUP,
} AgpmEventTeleportDataType;


typedef enum	_AgpmEventTeleportOperation {
	AGPMEVENT_TELEPORT_OPERATION_TELEPORT_POINT			= 0,
	AGPMEVENT_TELEPORT_OPERATION_TELEPORT_START,
	AGPMEVENT_TELEPORT_OPERATION_TELEPORT_CANCELED,
	AGPMEVENT_TELEPORT_OPERATION_TELEPORT_CUSTOM_POS,
	AGPMEVENT_TELEPORT_OPERATION_REQUEST_TELEPORT,
	AGPMEVENT_TELEPORT_OPERATION_REQUEST_TELEPORT_GRANTED,
	AGPMEVENT_TELEPORT_OPERATION_REQUEST_TELEPORT_IGNOIRED,
	AGPMEVENT_TELEPORT_OPERATION_TELEPORT_RETURN,
	AGPMEVENT_TELEPORT_OPERATION_TELEPORT_LOADING,
} AgpmEventTeleportOperation;

class AgpmArchlord;
class AgpmItem;
class AgpmGrid;
class AgpmBattleGround;
class ApmMap;

class AgpmEventTeleport : public ApModule  
{
private:
	ApmEventManager *	m_pcsApmEventManager;
	AgpmCharacter *		m_pcsAgpmCharacter;
	AgpmSiegeWar *		m_pcsAgpmSiegeWar;
	AgpmArchlord *		m_pcsAgpmArchlord;
	AgpmItem *			m_pcsAgpmItem;
	AgpmGrid *			m_pcsAgpmGrid;
	AgpmGuild *			m_pcsAgpmGuild;
	AgpmFactors*		m_pcsAgpmFactors;
	AgpmBattleGround*	m_pcsAgpmBattleGround;
	ApmMap*				m_pApmMap;
	

	INT32				m_lMaxPointID;
	INT32				m_lMaxGroupID;

	BOOL				m_bProtectPoint;		// Teleport Point가 Attach된 Base가 날라가도 Point는 보호 (Parn)

	INT32				m_nIndexADCharacter;
	INT32				m_nIndexADCharacterTemplate;

	CHAR **				m_pszFeeName;

public:
	ApAdmin				m_csAdminPoint;
	ApAdmin				m_csAdminGroup;
	ApAdmin				m_csAdminFee;
	
public:
	AuPacket			m_csPacket;

	AgpmEventTeleport();
	virtual ~AgpmEventTeleport();

	// Virtual Function 들
	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL				SetMaxTeleportPoint(INT16 nCount);
	BOOL				SetMaxTeleportGroup(INT16 nCount);

	AgpdTeleportPoint*	CreateTeleportPoint();
	BOOL				DestroyTeleportPoint(AgpdTeleportPoint *pcsTeleportPoint);

	AgpdTeleportGroup*	CreateTeleportGroup();
	BOOL				DestroyTeleportGroup(AgpdTeleportGroup *pcsTeleportGroup);

	AgpdTeleportPoint*	AddTeleportPoint(CHAR *szPointName, CHAR *szPointDescription = NULL);
	BOOL				RemoveTeleportPoint(AgpdTeleportPoint *pcsPoint, BOOL bForce = FALSE);
	BOOL				RemoveTeleportPoint(CHAR *szPointName, BOOL bForce = FALSE)	{ return RemoveTeleportPoint(GetTeleportPoint(szPointName), bForce);	}
	BOOL				RemoveTeleportPoint(INT32 lID, BOOL bForce = FALSE)			{ return RemoveTeleportPoint(GetTeleportPoint(lID), bForce);			}
	AgpdTeleportPoint*	GetTeleportPoint(CHAR *szPointName);
	AgpdTeleportPoint*	GetTeleportPoint(INT32 lID);
	BOOL				ChangeTeleportPointName(AgpdTeleportPoint *pcsPoint, CHAR *szName);

	AgpdTeleportPoint*	GetSequencePoint(INT32 *plIndex);
	AgpdTeleportPoint*	GetSequencePointInGroup(AgpdTeleportGroup *pcsGroup, INT_PTR *plIndex);

	VOID				DeleteAllGroupInPoint(AgpdTeleportPoint *pcsPoint);
	VOID				DeleteAllPointInGroup(AgpdTeleportGroup *pcsGroup);

	AgpdTeleportGroup*	AddTeleportGroup(CHAR *szGroupName, CHAR *szGroupDescription = NULL);
	BOOL				RemoveTeleportGroup(AgpdTeleportGroup *pcsGroup);
	BOOL				RemoveTeleportGroup(CHAR *szGroupName)	{ return RemoveTeleportGroup(GetTeleportGroup(szGroupName));	}
	BOOL				RemoveTeleportGroup(INT32 lID)			{ return RemoveTeleportGroup(GetTeleportGroup(lID));			}
	AgpdTeleportGroup*	GetTeleportGroup(CHAR *szGroupName);
	AgpdTeleportGroup*	GetTeleportGroup(INT32 lID);
	BOOL				ChangeTeleportGroupName(AgpdTeleportGroup *pcsGroup, CHAR *szName);

	AgpdTeleportGroup*	GetSequenceGroup(INT32 *plIndex);
	AgpdTeleportGroup*	GetSequenceGroupInPoint(AgpdTeleportPoint *pcsPoint, INT_PTR *plIndex);
	AgpdTeleportGroup*	GetSequenceTargetGroup(AgpdTeleportPoint *pcsPoint, INT_PTR *plIndex);

	INT32				GetTargetPointList(AgpdTeleportPoint *pcsPoint, AgpdTeleportPoint *apcsTargetPoint[], INT32 lBufferSize);

	// Target Point의 좌표를 알아낸다.
	BOOL				GetTargetPoint(AgpdTeleportPoint *pcsPoint, AuPOS *pstPos, FLOAT *pfRadiusMin, FLOAT *pfRadiusMax);

	BOOL				CheckValidRange(ApdEvent *pcsEvent, AuPOS *pcsGenerator, AuPOS *pstDestPos = NULL);
	BOOL				CheckValidTeleport(AgpdTeleportPoint *pcsSourcePoint, AgpdTeleportPoint *pcsTargetPoint);
	BOOL				CheckBaseStatus(ApBase *pcsBase);

	BOOL				Teleport(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos, BOOL bCheckRequirement, AgpdTeleportPoint *pcsSource = NULL, AgpdTeleportPoint *pcsTarget = NULL);

	BOOL				AddPointToGroup(INT32 lGroupID, INT32 lPointID)			{ return AddPointToGroup(GetTeleportGroup(lGroupID), GetTeleportPoint(lPointID));	}
	BOOL				AddPointToGroup(CHAR *szGroupName, CHAR *szPointName)	{ return AddPointToGroup(GetTeleportGroup(szGroupName), GetTeleportPoint(szPointName));	}
	BOOL				AddPointToGroup(AgpdTeleportGroup *pcsGroup, AgpdTeleportPoint *pcsPoint);
	BOOL				AddTargetGroupToPoint(AgpdTeleportGroup *pcsGroup, AgpdTeleportPoint *pcsPoint);

	BOOL				RemovePointNGroup(AgpdTeleportGroup *pcsGroup, AgpdTeleportPoint *pcsPoint);
	BOOL				RemoveTargetGroupFromPoint(AgpdTeleportGroup *pcsGroup, AgpdTeleportPoint *pcsPoint);

	/*
	ApdEvent*			AddTeleportEvent(ApBase *pcsBase);
	BOOL				AddGroupToEvent(CHAR *szGroupName, ApBase *pcsBase);
	BOOL				AddGroupToEvent(CHAR *szGroupName, ApdEvent *pcsEvent);
	BOOL				RemoveGroupFromEvent(CHAR *szGroupName, ApBase *pcsBase);
	BOOL				RemoveGroupFromEvent(CHAR *szGroupName, ApdEvent *pcsEvent);
	*/

	// Teleport 후에 Callback
	BOOL				SetCallbackTeleport(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackCalcPos(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceivedRequestResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceivedTeleportStart(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReceivedTeleportCanceled(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackCheckUsableTeleport(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackReturnTown(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackTeleportLoading(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackSyncTeleportLoading(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackTeleportLog(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackBattleGroundTeleportLog(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackArchlordMessageId(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackGetBattleGroundState(ApModuleDefaultCallBack pfCallback, PVOID pClass);


	// streaming functions
	BOOL				StreamReadGroup(CHAR *szFile, BOOL bDecryption);
	BOOL				StreamWriteGroup(CHAR *szFile, BOOL bEncryption);

	BOOL				StreamReadPoint(CHAR *szFile, BOOL bDecryption);
	BOOL				StreamWritePoint(CHAR *szFile, BOOL bEncryption);

	BOOL				StreamReadFee(CHAR *szFile, BOOL bDecryption);

	static BOOL			GroupWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			GroupReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	static BOOL			PointWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL			PointReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	/*
	// Module Data를 Streaming (Object/Character/Item 등에 붙은 attach data는 streaming하지 않는다.
	BOOL				StreamWrite(CHAR *szFile);
	BOOL				StreamRead(CHAR *szFile);

	// Teleport Point array를 Streaming한다.
	BOOL				StreamWritePoint(ApModuleStream *pcsStream, AgpdTeleportPoint *pstTarget, UINT16 unTargetNum);
	BOOL				StreamReadPoint(ApModuleStream *pcsStream, AgpdTeleportPoint *pstTarget, UINT16 *punTargetNum, UINT16 unMaxTargetNum);
	*/

	BOOL				SendTeleportInfo(ApdEvent *pstEvent, INT32 *plTargetCID, CHAR *szPointName, AuPOS *pstPos);
	BOOL				SendTeleportReturn(ApdEvent *pstEvent, INT32 *plTargetCID);
	BOOL				SendTeleportStart(INT32 *plTargetCID, AuPOS *pstPos, UINT32 ulNID);
	BOOL				SendTeleportCanceled(INT32 *plTargetCID, UINT32 ulNID);
	BOOL				SendRequestTeleport(ApdEvent *pstEvent, INT32 *plTargetCID);
	BOOL				SendRequestTeleportGranted(ApdEvent *pstEvent, AgpdCharacter *pcsCharacter, UINT32 ulNID);
	BOOL				SendTeleportLoading(INT32 lCID, UINT32 ulNID = 0);

	static BOOL			CBActionEventTeleport(PVOID pData, PVOID pClass, PVOID pCustData);

	// Object/Character/Item 등에 붙은 attach data에 대한 stream callback
	static BOOL			CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			ConAgpdTeleportAttachToChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgpdTeleportAttachToChar(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdTeleportAttachToChar* GetADCharacter(AgpdCharacter *pcsCharacter);

	// 가격 알아내기
	INT32				GetFee(CHAR *szSource, CHAR *szTarget, AgpdCharacter *pcsCharacter, INT32 *plTax = NULL);

	BOOL				RemoveFee();

	BOOL				IsArchlordDungeonPoint(AgpdTeleportPoint *pcsTeleportPoint, AgpdCharacter *pcsCharacter);
	BOOL				IsDisablePoint(AgpdTeleportPoint *pcsTeleportPoint, AgpdCharacter *pcsCharacter);
	void				DungeonTeleportDisable();
	void				ActiveTeleportCastleToDungeon();
};

#endif // !defined(AFX_AGPMEVENTTELEPORT_H__9A67D2B3_5469_4077_8C59_1843C2477176__INCLUDED_)
