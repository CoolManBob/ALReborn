/******************************************************************************
Module:  AgsmAOIFilter.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 23
******************************************************************************/

#if !defined(__AGSMAOIFILTER_H__)
#define __AGSMAOIFILTER_H__

#include "AgsEngine.h"
#include "AgsmAOIMap.h"

class ApmMap;
class AgpmFactors;
class AgpmCharacter;
class AgpmSiegeWar;
class AgpmItem;

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmAOIFilterD" )
#else
#pragma comment ( lib , "AgsmAOIFilter" )
#endif
#endif


typedef enum _eAgsmAOICB {
	AGSMAOI_CB_ID_SEND_CHAR		= 0,
	AGSMAOI_CB_ID_SEND_CHAR_REMOVE,
	AGSMAOI_CB_ID_SEND_CHAR_EQUIPITEM,
	AGSMAOI_CB_ID_SEND_ITEM,
	AGSMAOI_CB_ID_SEND_ITEM_REMOVE,

	AGSMAOI_CB_ID_ADD_CHAR,
	AGSMAOI_CB_ID_REMOVE_CHAR,
	AGSMAOI_CB_ID_MOVE_CHAR,

	AGSMAOI_CB_ID_ADD_ITEM,
	AGSMAOI_CB_ID_REMOVE_ITEM,

	AGSMAOI_CB_ID_CHECK_CONTROLSERVER,

	AGSMAOI_CB_ID_GET_DPNID
} eAgsmAOICB;


typedef enum _eAgsmAOIFilter {
	AGSMAOI_MOVE_UP			= 0,
	AGSMAOI_MOVE_UPRIGHT,
	AGSMAOI_MOVE_RIGHT,
	AGSMAOI_MOVE_DOWNRIGHT,
	AGSMAOI_MOVE_DOWN,
	AGSMAOI_MOVE_DOWNLEFT,
	AGSMAOI_MOVE_LEFT,
	AGSMAOI_MOVE_UPLEFT,
} eAgsmAOIFilter;


const int	AGSMAOI_MAX_RACE							= 16;
const int	AGSMAOI_DEFAULT_NEAR_CELL_COUNT				= 2;
const float AGSMAOI_DEFAULT_VISIBLE_DISTANCE_PER_CELL	= 400.0f;

class AgsmAOIFilter : public AgsModule {
private:
	ApmMap			*m_papmMap;
	AgpmFactors		*m_pagpmFactors;
	AgpmCharacter	*m_pagpmCharacter;
	AgpmSiegeWar	*m_pagpmSiegeWar;
	AgpmItem		*m_pagpmItem;

	INT16			m_nIndexADCharacter;
	INT16			m_nIndexADItem;

	ApAdmin			m_csAdminAllGroup;
	AuCryptActor	m_csCryptActor;		// 멀티캐스트 암호화용 CryptActor

public:
	AgsmAOIMap		m_csAOIMap;

public:
	AgsmAOIFilter();
	~AgsmAOIFilter();

	BOOL OnAddModule();

	BOOL OnInit();
	BOOL OnDestroy();

	static BOOL ConCharacterCellUnit(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesCharacterCellUnit(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL ConItemCellUnit(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesItemCellUnit(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackAddCharacterToMap(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackRemoveCharacterFromMap(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackUpdateCharacterPosition(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackAddItemToMap(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackRemoveItemFromMap(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackResetMonster(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL SendPacketNear(PVOID pvPacket, INT16 nLength, AuPOS posCurrent, PACKET_PRIORITY ePriority, INT32 lRegionIndex = -1);
	BOOL SendPacketNear(PVOID pvPacket, INT16 nLength, AgsmAOICell *pcsCell, INT32 lRegionIndex, PACKET_PRIORITY ePriority);
	BOOL SendPacketNearRace(PVOID pvPacket, INT16 nLength, AuPOS posCurrent, UINT32 lRace);

	BOOL SendPacketNearExceptSelf(PVOID pvPacket, INT16 nLength, AuPOS posCurrent, INT32 lRegionIndex, UINT32 ulSelfNID, PACKET_PRIORITY ePriority);
	BOOL SendPacketNearExceptSelf(PVOID pvPacket, INT16 nLength, AgsmAOICell *pcsCell, INT32 lRegionIndex, UINT32 ulSelfNID, PACKET_PRIORITY ePriority);

	BOOL SendPacketNearExceptNIDs(PVOID pvPacket, INT16 nLength, AuPOS posCurrent, INT32 lRegionIndex, UINT32 *pulExceptNID, INT32 nNumNIDs, PACKET_PRIORITY ePriority);
	BOOL SendPacketNearExceptNIDs(PVOID pvPacket, INT16 nLength, AgsmAOICell *pcsCell, INT32 lRegionIndex, UINT32 *pulExceptNID, INT32 nNumNIDs, PACKET_PRIORITY ePriority);

	BOOL SendPacketGroup(PVOID pvPacket, INT16 nLength, UINT_PTR ulGroupNID, PACKET_PRIORITY ePriority);
	BOOL SendPacketGroupExceptSelf(PVOID pvPacket, INT16 nLength, UINT_PTR ulGroupNID, UINT32 ulSelfNID, PACKET_PRIORITY ePriority);
	BOOL SendPacketGroupExceptNIDs(PVOID pvPacket, INT16 nLength, UINT_PTR ulGroupNID, UINT32 *pulExceptNID, INT32 nNumNIDs, PACKET_PRIORITY ePriority);
	BOOL SendPacketAllGroups(PVOID pvPacket, INT16 nLength, PACKET_PRIORITY ePriority);

	AgsmCellUnit *GetADCharacter(PVOID pData);
	AgsmCellUnit *GetADItem(PVOID pData);

	BOOL SetCallbackAddChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackAddItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackCheckControlServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGetDPNID(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	inline PVOID EncryptGroupPacket(PVOID pvPacket, INT16* pnSize);
	AuCryptActor& GetCryptActorRef() { return m_csCryptActor; }
	AuCryptActor* GetCryptActorPtr() { return &m_csCryptActor; }

	INT32 GetPlayerCountFromAllPlayerGroup();

	INT32	GetNumAroundCharacter(AgsmAOICell *pcsCell, INT32 lRegionIndex = -1);

	BOOL InitAttachData(AgpdCharacter *pcsCharacter);

	VOID	CheckValidLock(ApMutualEx *pMutex);

	// 2006.07.04. steeple
	inline INT32 GetNearCellCountByRegion(INT32 lRegionIndex);
};

#endif //__AGSMAOIFILTER_H__