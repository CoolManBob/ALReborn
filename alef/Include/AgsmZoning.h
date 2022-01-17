#ifndef	__AGSMZONING_H__
#define	__AGSMZONING_H__

#include "ApBase.h"
#include "AgsEngine.h"

#include "ApmMap.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"

#include "AgsmMap.h"
#include "AgsmCharacter.h"
//#include "AgsmServerManager.h"
#include "AgsmServerManager2.h"
#include "AgsmAOIFilter.h"

#include "AuPacket.h"

#include "AgsdZoning.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmZoningD" )
#else
#pragma comment ( lib , "AgsmZoning" )
#endif
#endif


typedef enum _AgsmZoningCB {
	AGSMZONING_CB_ZONING_START						= 0,
	AGSMZONING_CB_ZONING_PASS_CONTROL,

	AGSMZONING_CB_SYNCADDCHAR,
} AgsmZoningCB;

typedef enum _AgsmZoningPacketOperation {
	AGSMZONING_PACKET_OPERATION_START_ZONING				= 0,
	AGSMZONING_PACKET_OPERATION_START_ZONING_RESULT			= 7,
	AGSMZONING_PACKET_OPERATION_SEND_ZONESERVER_ADDR		= 1,
	AGSMZONING_PACKET_OPERATION_CONNECT_ZONESERVER			= 2,
	AGSMZONING_PACKET_OPERATION_RESULT_CONNECT_ZONESERVER	= 3,
	AGSMZONING_PACKET_OPERATION_PASSCONTROL					= 5, 
	AGSMZONING_PACKET_OPERATION_RECV_PASSCONTROL			= 6,
	AGSMZONING_PACKET_OPERATION_END_ZONING					= 4,
	AGSMZONING_PACKET_OPERATION_ZONING_FAIL					= 8,
} AgsmZoningPacketOperation;

class AgsmZoning : public AgsModule {
private:
	ApmMap				*m_pcsApmMap;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;

	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmServerManager	*m_pAgsmServerManager;
	AgsmMap				*m_pcsAgsmMap;
	AgsmAOIFilter		*m_pcsAgsmAOIFilter;

	INT16				m_nIndexADCharacter;

	AuPacket			m_csPacket;

	// 조닝 관련 함수들
	BOOL				ProcessZoning(AgpdCharacter *pcsCharacter, ApWorldSector *pSector, ApWorldSector **paForwardSector);

	BOOL				CheckZoning(AgsdServer *pcsServer);


	BOOL				SendZoneServerAddr(INT32 lCID, INT32 lServerID);
	BOOL				SendZoneServerAddr(AgpdCharacter *pcsCharacter, INT32 lServerID);

	BOOL				PassControl(AgpdCharacter *pCharacter, AgsdServer *pcsServer);
	BOOL				SendPassControl(INT32 lCID, INT32 lServerID);

	BOOL				EndZoning(AgpdCharacter *pcsCharacter);
	
	BOOL				SendConnectResult(AgpdCharacter *pCharacter, BOOL bResult);

	BOOL				SendStartZoning(AgpdCharacter *pcsCharacter, AgsdServer *pcsServer);
	BOOL				SendStartZoningResult(INT32 lCID, INT32 lServerID);

	BOOL				SendZoningFail(INT32 lCharacterID, INT32 lServerID);
	BOOL				SendZoningFail(AgpdCharacter *pcsCharacter, INT32 lServerID);

	BOOL				AllServerAdd(AgpdCharacter *pcsCharacter);

	/*
	BOOL				SyncAddChar(AgpdCharacter *pCharacter, ApWorldSector *pcsSector);
	BOOL				SyncRemoveChar(AgpdCharacter *pCharacter, ApWorldSector *pcsSector);
	*/

	BOOL				SyncAddChar(AgpdCharacter *pCharacter, AgsmAOICell *pcsCell);
	BOOL				SyncRemoveChar(AgpdCharacter *pCharacter, AgsmAOICell *pcsCell);

	BOOL				SendPacketCharSync(AgpdCharacter *pCharacter, UINT32 ulNID);
	BOOL				SendPacketCharRemoveSync(AgpdCharacter *pCharacter);

public:
	AgsmZoning();
	virtual ~AgsmZoning();

	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	static BOOL			ConAgsdZoning(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgsdZoning(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBEnterGameworld(PVOID pData, PVOID pClass, PVOID pCustData);
	/*
	static BOOL			CBUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBAddSector(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRemoveSector(PVOID pData, PVOID pClass, PVOID pCustData);
	*/

	static BOOL			CBAddCell(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRemoveCell(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				SetCallbackZoningStart(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackZoningPassControl(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackSyncAddChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	AgsdZoning*			GetADCharacter(AgpdCharacter *pcsCharacter);

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
};

#endif	//__AGSMZONING_H__