#ifndef	__AGPMITEMCONVERT_H__
#define	__AGPMITEMCONVERT_H__

#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"

#include "AgpdItemConvert.h"

#include "AuRandomNumber.h"
#include "AuPacket.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmItemConvertD" )
#else
#pragma comment ( lib , "AgpmItemConvert" )
#endif
#endif

typedef enum _AgpdItemConvertCBID {
	AGPDITEMCONVERT_CB_PROCESS_PHYSICAL_CONVERT			= 0,
	AGPDITEMCONVERT_CB_PROCESS_SOCKET_CONVERT,
	AGPDITEMCONVERT_CB_PROCESS_SPIRITSTONE_CONVERT,
	AGPDITEMCONVERT_CB_PROCESS_RUNE_CONVERT,

	AGPDITEMCONVERT_CB_RESULT_PHYSICAL_CONVERT,
	AGPDITEMCONVERT_CB_RESULT_SOCKET_CONVERT,
	AGPDITEMCONVERT_CB_RESULT_SPIRITSTONE_CONVERT,
	AGPDITEMCONVERT_CB_RESULT_RUNE_CONVERT,

	AGPDITEMCONVERT_CB_SEND_UPDATE_FACTOR,

	AGPDITEMCONVERT_CB_ASK_REALLY_SPIRITSTONE_CONVERT,
	AGPDITEMCONVERT_CB_SPIRITSTONE_CONVERT_CHECK_RESULT,

	AGPDITEMCONVERT_CB_ASK_REALLY_RUNE_CONVERT,
	AGPDITEMCONVERT_CB_RUNE_CONVERT_CHECK_RESULT,

	AGPDITEMCONVERT_CB_ADD,
	AGPDITEMCONVERT_CB_UPDATE,

	AGPDITEMCONVERT_CB_ADJUST_SUCCESS_PROB,

	AGPDITEMCONVERT_CB_CONVERT_AS_DROP,
	AGPDITEMCONVERT_CB_PROCESS_SOCKET_INITIALIZE,
} AgpdItemConvertCBID;

typedef enum _AgpdItemConvertOperation {
	AGPDITEMCONVERT_OPERATION_ADD						= 0,
	AGPDITEMCONVERT_OPERATION_REQUEST_PHYSICAL_CONVERT,
	AGPDITEMCONVERT_OPERATION_REQUEST_ADD_SOCKET,
	AGPDITEMCONVERT_OPERATION_REQUEST_RUNE_CONVERT,
	AGPDITEMCONVERT_OPERATION_REQUEST_SPIRITSTONE_CONVERT,
	AGPDITEMCONVERT_OPERATION_RESPONSE_PHYSICAL_CONVERT,
	AGPDITEMCONVERT_OPERATION_RESPONSE_ADD_SOCKET,
	AGPDITEMCONVERT_OPERATION_RESPONSE_SPIRITSTONE_CONVERT,
	AGPDITEMCONVERT_OPERATION_RESPONSE_RUNE_CONVERT,
	AGPDITEMCONVERT_OPERATION_RESPONSE_SPIRITSTONE_CHECK_RESULT,
	AGPDITEMCONVERT_OPERATION_RESPONSE_RUNE_CHECK_RESULT,
	AGPDITEMCONVERT_OPERATION_CHECK_CASH_RUNE_CONVERT,
	AGPDITEMCONVERT_OPERATION_SOCKET_INITIALIZE,
} AgpdItemConvertOperation;


class AgpmItemConvert : public ApModule {
private:
	AgpmFactors				*m_pcsAgpmFactors;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmItem				*m_pcsAgpmItem;
	AgpmSkill				*m_pcsAgpmSkill;

	INT32					m_lIndexADChar;
	INT32					m_lIndexADItem;
	INT32					m_lIndexADItemTemplate;

public:
//	AgpdItemConvertPhysical			m_astTablePhysical[AGPDITEMCONVERT_MAX_PHYSICAL_CONVERT + 1];
	ApSafeArray<AgpdItemConvertPhysical, AGPDITEMCONVERT_MAX_ITEM_RANK + 1>		m_astTablePhysical;
//	AgpdItemConvertPhysicalFail		m_astTablePhysicalFail[AGPDITEMCONVERT_MAX_PHYSICAL_CONVERT + 1];
	//ApSafeArray<AgpdItemConvertPhysicalFail, AGPDITEMCONVERT_MAX_PHYSICAL_CONVERT + 1>	m_astTablePhysicalFail;
//	AgpdItemConvertAddSocket		m_astTableSocket[AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1];
	ApSafeArray<AgpdItemConvertAddSocket, AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1>		m_astTableSocket;
//	AgpdItemConvertAddSocketFail	m_astTableSocketFail[AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1];
	ApSafeArray<AgpdItemConvertAddSocketFail, AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1>	m_astTableSocketFail;
//	AgpdItemConvertSpiritStone		m_astTableSpiritStone[AGPDITEMCONVERT_MAX_SPIRIT_STONE + 1];
	ApSafeArray<AgpdItemConvertSpiritStone, AGPDITEMCONVERT_MAX_SPIRIT_STONE + 1>		m_astTableSpiritStone;
	AgpdItemConvertSameAttrBonus	m_stTableSameAttrBonus;
//	AgpdItemConvertRune				m_astTableRune[AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1];
	ApSafeArray<AgpdItemConvertRune, AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1>				m_astTableRune;
//	AgpdItemConvertRuneFail			m_astTableRuneWeaponFail[AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1];
	ApSafeArray<AgpdItemConvertRuneFail, AGPDITEMCONVERT_MAX_WEAPON_SOCKET + 1>			m_astTableRuneWeaponFail;

	AgpdItemConvertPoint	m_stConvertPoint;

private:
	MTRand					m_csRandom;

public:
	AuPacket				m_csPacket;
	AuPacket				m_csPacketTID;

private:
	BOOL					StreamConvertPhysicalTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	BOOL					StreamConvertPhysicalSuccessTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	BOOL					StreamConvertPhysicalFailTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	BOOL					StreamConvertPhysicalInitTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	BOOL					StreamConvertPhysicalDestroyTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	/*
	BOOL					StreamConvertPhysicalFailTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	BOOL					StreamConvertAddSocketTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	BOOL					StreamConvertAddSocketFailTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	*/
	BOOL					StreamConvertSpiritStoneTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	BOOL					StreamConvertAddBonusTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	BOOL					StreamConvertRuneTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);
	BOOL					StreamConvertRuneFailTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow);

public:
	AgpdItemConvertResult	IsPhysicalConvertable(AgpdItem *pcsItem, BOOL bIsCheckStatus = TRUE);
	AgpdItemConvertResult	IsValidCatalyst(AgpdItem *pcsItem);

	BOOL					IsEgoItem(AgpdItem *pcsItem);

	INT32					GetPhysicalConvertLevel(AgpdItem *pcsItem);

	AgpdItemConvertResult	PhysicalConvert(AgpdItem *pcsItem, AgpdItem *pcsCatalystItem);

	BOOL					InitializePhysicalConvert(AgpdItem *pcsItem);
	BOOL					ApplyPhysicalConvert(AgpdItem *pcsItem, BOOL bIsUpdateFactor = TRUE);
	BOOL					CalcPhysicalConvertFactor(AgpdItem *pcsItem);

	BOOL					SetPhysicalConvert(AgpdItem *pcsItem, INT32 lConvertLevel, BOOL bIsCheckStatus = TRUE);

	INT64					GetSocketCost(AgpdItem *pcsItem, INT32 lNumSocket);
	INT64					GetSocketCost(AgpdItem *pcsItem);
	INT64					GetNextSocketCost(AgpdItem *pcsItem);
	BOOL					IsEnoughSocketCost(AgpdCharacter *pcsCharacter, INT32 lNumSocket, AgpdItem *pcsItem);
	BOOL					IsWeapon(AgpdItem *pcsItem);
	BOOL					IsArmour(AgpdItem *pcsItem);
	BOOL					IsEtc(AgpdItem *pcsItem);

	AgpdItemConvertSocketResult	IsSocketConvertable(AgpdItem *pcsItem, BOOL IsCheckCost = TRUE);
	BOOL					IsSocketInitializable(AgpdItem *pcsItem);

	AgpdItemConvertSocketResult	SocketConvert(AgpdItem *pcsItem);

	BOOL					InitializeSocket(AgpdItem *pcsItem, BOOL bIsKeepSocket = FALSE);
	BOOL					AddSocketConvert(AgpdItem *pcsItem);

	BOOL					SetSocketConvert(AgpdItem *pcsItem, INT32 lSocketLevel);

	AgpdItemConvertSpiritStoneResult	IsSpiritStoneConvertable(AgpdItem *pcsItem);
	AgpdItemConvertSpiritStoneResult	IsValidSpiritStone(AgpdItem *pcsItem);

	AgpdItemConvertSpiritStoneResult	SpiritStoneConvert(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, BOOL bAdmin = FALSE);

	BOOL					InitializeSpiritStoneConvert(AgpdItem *pcsItem);
	BOOL					AddSpiritStoneConvert(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone);
	BOOL					AddSpiritStoneConvert(AgpdItem *pcsItem, AgpdItemTemplate *pcsItemTemplate);
	BOOL					CalcSpiritStoneConvertFactor(AgpdItem *pcsItem);

	AgpdItemConvertRuneResult	IsRuneConvertable(AgpdItem *pcsItem);
	AgpdItemConvertRuneResult	IsValidRuneItem(AgpdItem *pcsItem);
	AgpdItemConvertRuneResult	IsProperPart(AgpdItem *pcsItem, AgpdItem *pcsRuneItem);
	AgpdItemConvertRuneResult	IsProperLevel(AgpdItem *pcsItem, AgpdItem *pcsRuneItem);
	AgpdItemConvertRuneResult	IsAntiNumber(AgpdItem *pcsItem, AgpdItem *pcsRuneItem);

	AgpdItemConvertRuneResult	RuneConvert(AgpdItem *pcsItem, AgpdItem *pcsRuneItem, BOOL bAdmin = FALSE);

	BOOL						InitializeRuneConvert(AgpdItem *pcsItem);
	BOOL						RemoveItemOptionRune(AgpdItem *pcsItem, INT32 lItemOptionTID);
	BOOL						AddRuneConvert(AgpdItem *pcsItem, AgpdItem *pcsRuneItem);
	BOOL						AddRuneConvert(AgpdItem *pcsItem, AgpdItemTemplate *pcsItemTemplate, BOOL bIsUpdateFactor = TRUE);
	BOOL						CalcRuneConvertFactor(AgpdItem *pcsItem);

	BOOL					OnOperationAdd(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, PVOID pvPacketTIDList);
	BOOL					OnOperationRequestPhysicalConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lCatalystIID);
	BOOL					OnOperationRequestAddSocket(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem);
	BOOL					OnOperationRequestRuneConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lRuneIID);
	BOOL					OnOperationRequestSpiritStoneConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lSpiritStoneID);
	BOOL					OnOperationResponsePhysicalConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket);
	BOOL					OnOperationResponseAddSocket(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket);
	BOOL					OnOperationResponseSpiritStoneConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT32 lTID);
	BOOL					OnOperationResponseRuneConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT32 lTID);
	BOOL					OnOperationResponseRuneCheckResult(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lSpiritStoneID, INT8 cActionResult);
	BOOL					OnOperationResponseSpiritStoneCheckResult(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lSpiritStoneID, INT8 cActionResult);
	BOOL					OnOperationCheckCashRuneConvert(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, INT32 lRuneIID);

public:
	AgpmItemConvert();
	virtual ~AgpmItemConvert();

	BOOL					OnAddModule();

	AgpdItemConvertADChar	*GetADChar(AgpdCharacter *pcsCharacter);
	AgpdItemConvertADItem	*GetADItem(AgpdItem *pcsItem);
	AgpdItemConvertADItemTemplate	*GetADItemTemplate(AgpdItemTemplate *pcsItemTemplate);

	static BOOL				ConAgpdItemConvertADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				DesAgpdItemConvertADChar(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				ConAgpdItemConvertADItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				DesAgpdItemConvertADItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				ConAgpdItemConvertADItemTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				DesAgpdItemConvertADItemTemplate(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL					StreamReadConvertTable(CHAR *szFile, BOOL bDecryption);
	BOOL					StreamReadRuneAttribute(CHAR *szFile, BOOL bDecryption);
	BOOL					StreamReadConvertPointTable(CHAR *szFile, BOOL bDecryption);

	AgpdItemConvertResult				ProcessPhysicalConvert(AgpdItem *pcsItem, AgpdItem *pcsCatalystItem);
	AgpdItemConvertSocketResult			ProcessSocketConvert(AgpdItem *pcsItem);
	AgpdItemConvertSpiritStoneResult	ProcessSpiritStoneConvert(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone);
	AgpdItemConvertRuneResult			ProcessRuneConvert(AgpdItem *pcsItem, AgpdItem *pcsRuneItem);

	INT32					GetNumPhysicalConvert(AgpdItem *pcsItem);
	INT32					GetNumSocket(AgpdItem *pcsItem);
	INT32					GetNumConvertedSocket(AgpdItem *pcsItem);
	INT32					GetNumSpiritStone(AgpdItem *pcsItem, BOOL bGrouping = FALSE);	// Grouping : 같은 SpiritStone에 대해서 Group별 Counting 여부
	INT32					GetNumRune(AgpdItem *pcsItem);

	INT32					GetNumConvertAttr(AgpdItem *pcsItem, INT32 lAttributeType);


	BOOL				    OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL					DispatchAddPacket(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, PVOID pvPacket, INT16 nSize);

	BOOL					CalcConvertFactor(AgpdItem *pcsItem);
	BOOL					AddItemOptionRune(AgpdItem *pcsItem, INT32 lItemOptionTID, BOOL bIsValidCheck  = TRUE);

	PVOID					MakePacketAdd(AgpdItem *pcsItem, INT16 *pnPacketLength);

	PVOID					MakePacketRequestPhysicalConvert(INT32 lCID, INT32 lIID, INT32 lCatalystIID, INT16 *pnPacketLength);
	PVOID					MakePacketRequestSocketAdd(INT32 lCID, INT32 lIID, INT16 *pnPacketLength);
	PVOID					MakePacketRequestRuneConvert(INT32 lCID, INT32 lIID, INT32 lRuneIID, INT16 *pnPacketLength);
	PVOID					MakePacketRequestSpiritStoneConvert(INT32 lCID, INT32 lIID, INT32 lSpiritStoneIID, INT16 *pnPacketLength);

	PVOID					MakePacketResponsePhysicalConvert(INT32 lCID, INT32 lIID, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT16 *pnPacketLength);
	PVOID					MakePacketResponseSocketAdd(INT32 lCID, INT32 lIID, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT16 *pnPacketLength);
	PVOID					MakePacketResponseSpiritStoneConvert(INT32 lCID, INT32 lIID, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT32 lTID, INT16 *pnPacketLength);
	PVOID					MakePacketResponseRuneConvert(INT32 lCID, INT32 lIID, INT8 cActionResult, INT8 cNumPhysicalConvert, INT8 cNumSocket, INT8 cNumConvertedSocket, INT32 lTID, INT16 *pnPacketLength);

	PVOID					MakePacketResponseRuneCheckResult(INT32 lCID, INT32 lIID, INT32 lRuneID, INT8 cActionResult, INT16 *pnPacketLength);
	PVOID					MakePacketResponseSpiritStoneCheckResult(INT32 lCID, INT32 lIID, INT32 lSpiritStoneID, INT8 cActionResult, INT16 *pnPacketLength);

	PVOID					MakePacketCheckCashRuneConvert(INT32 lCID, INT32 lIID, INT32 lRuneIID, INT16 *pnPacketLength);
	PVOID					MakePacketSocketInitialize(INT32 lCID, INT32 lIID, INT32 lCashIID, INT16 *pnPacketLength);

	static BOOL				CBEquipItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBUnEquipItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBRequestConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBRequestSpiritStoneConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBRequestRuneConvert(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBInitItem(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL					SetCallbackProcessPhysicalConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackProcessSocketConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackProcessSpiritStoneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackProcessRuneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackProcessSocketInitialize(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackResultPhysicalConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackResultSocketConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackResultSpiritStoneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackResultRuneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackSendUpdateFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackAskReallySpiritStoneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackSpiritStoneCheckResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackAskReallyRuneConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackRuneCheckResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackAdjustSuccessProb(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					SetCallbackConvertAsDrop(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL					EncodeConvertHistory(AgpdItemConvertADItem *pcsAttachData, CHAR *szBuffer, INT32 lBufferSize);
	BOOL					DecodeConvertHistory(AgpdItemConvertADItem *pcsAttachData, CHAR *szBuffer, INT32 lBufferSize, BOOL bIsAddAttribute = FALSE, AgpdItem *pcsItem = NULL);

	INT32					GetTotalConvertPoint(AgpdItem *pcsItem);

	INT32					GetTotalPhysicalConvertPoint(AgpdItem *pcsItem);
	INT32					GetTotalSocketConvertPoint(AgpdItem *pcsItem);
	INT32					GetTotalSpiritStoneConvertPoint(AgpdItem *pcsItem);
	INT32					GetTotalRuneConvertPoint(AgpdItem *pcsItem);

	INT32					GetSpiritStoneAttrConvertPoint(AgpdItem *pcsItem, INT32 lAttributeType);

	INT32					GetPhysicalItemRank(AgpdItem *pcsItem);
	INT32					GetPhysicalItemRank(AgpdItemTemplate *pcsItemTemplate);
	CHAR*					GetPhysicalItemRankName(AgpdItem *pcsItem);

	BOOL					IsConvertedAttribute(AgpdItem *pcsItem, AgpmItemRuneAttribute eRuneAttribute);
	INT32					GetConvertedExtraType(AgpdItem* pcsItem, AgpmItemRuneAttribute eRuneAttribute);
};

#endif	//__AGPMITEMCONVERT_H__