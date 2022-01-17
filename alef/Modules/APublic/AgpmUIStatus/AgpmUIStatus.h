#ifndef	__AGPMUISTATUS_H__
#define	__AGPMUISTATUS_H__

#include "ApBase.h"

#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmSkill.h"

#include "ApmEventManager.h"

#include "AuPacket.h"

#include "AgpdUIStatus.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmUIStatusD" )
#else
#pragma comment ( lib , "AgpmUIStatus" )
#endif
#endif

typedef enum AgpmUIStatusPacketOperation
{
	AGPMUISTATUS_PACKET_OPERATION_ADD					= 0,
	AGPMUISTATUS_PACKET_OPERATION_UPDATE_ITEM,
	AGPMUISTATUS_PACKET_OPERATION_ENCODED_STRING,

	AGPMUISTATUS_PACKET_OPERATION_UPDATE_VIEW_HELMET_OPTION,

	AGPMUISTATUS_PACKET_OPERATION_NUM

} AgpmUIStatusPacketOperation;

typedef enum AgpmUIStatusCB
{
	AGPMUISTATUS_CB_ADD_QUICKBELT_INFORMATION			= 0,
	AGPMUISTATUS_CB_UPDATE_QUICKBELT_ITEM,
	AGPMUISTATUS_CB_RECEIVED_ENCODED_QUICKBELT_STRING,

	AGPMUISTATUS_CB_UPDATE_VIEW_HELMET_OPTION,

} AgpmUIStatusCB;

class AgpmUIStatus : public ApModule {
private:
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmItem				*m_pcsAgpmItem;
	AgpmSkill				*m_pcsAgpmSkill;

	ApmEventManager			*m_pcsApmEventManager;

	INT16					m_nIndexADCharacter;

public:
	BOOL					OnOperationAdd(AgpdCharacter *pcsCharacter, PVOID *pvPacketWholeItem, INT32 lHPPotionTID, INT32 lMPPotionTID, INT8 cAutoUseHPGage, INT8 cAutoUseMPGage);
	BOOL					OnOperationUpdate(AgpdCharacter *pcsCharacter, INT32 lIndex, PVOID pvPacketItem, INT32 lHPPotionTID, INT32 lMPPotionTID, INT8 cAutoUseHPGage, INT8 cAutoUseMPGage);
	BOOL					OnOperationEncodedString(AgpdCharacter *pcsCharacter, CHAR *szEncodedString, INT32 lStringLength, CHAR* szCooldownString, INT32 lCooldownLength);
	BOOL					OnOperationUpdateViewHelmetOption(AgpdCharacter *pcsCharacter, INT8 cOptionViewHelmet);

public:
	AuPacket				m_csPacket;

public:
	AgpmUIStatus();
	~AgpmUIStatus();

	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();

	static BOOL				ConAgpdUIStatusADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				DesAgpdUIStatusADChar(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdUIStatusADChar*		GetADCharacter(AgpdCharacter *pcsCharacter);

	/*
	BOOL					EncodingQuickBeltBase(CHAR *szStringBuffer, INT32 lBufferLength, ApBase **ppcsQBeltBase);
	BOOL					DecodingQuickBeltBase(CHAR *szStringBuffer, ApBase **ppcsQBeltBase);
	*/

	PVOID					MakePacketWholeQuickBeltInfo(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);
	PVOID					MakePacketUpdateQuickBeltItem(AgpdCharacter *pcsCharacter, INT32 lIndex, ApBase *pcsBaseItem, INT16 *pnPacketLength, INT32 lHPPotionTID = AP_INVALID_IID, INT32 lMPPotionTID = AP_INVALID_IID, INT8 cAutoUseHPGage = (-1), INT8 cAutoUseMPGage = (-1));
	PVOID					MakePacketQBeltString(AgpdCharacter *pcsCharacter, CHAR *szString, INT16 nStringLength, CHAR* szCooldown, INT16 nCooldownLength, INT16 *pnPacketLength);
	PVOID					MakePacketUpdateViewHelmetOption(INT32 lCID, INT16 *pnPacketLength, INT8 cOptionViewHelmet);

	BOOL				    OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL					SetCallbackAddQuickBeltInformation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUpdateQuickBeltItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackReceivedEncodedQBeltString(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUpdateViewHelmetOption(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetQBeltEncodedString(AgpdCharacter *pcsCharacter, CHAR *szString, INT32 lStringLength);
	BOOL					SetCooldownEncodedString(AgpdCharacter *pcsCharacter, CHAR *szString, INT32 lStringLength);
};

#endif	//__AGPMUISTATUS_H__