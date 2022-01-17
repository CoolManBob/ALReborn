#ifndef	__AGSMITEMCONVERT_H__
#define	__AGSMITEMCONVERT_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmItemConvertD" )
#else
#pragma comment ( lib , "AgsmItemConvert" )
#endif
#endif

#include "AgpmItemConvert.h"

#include "AgsmCharacter.h"
#include "AgsmItem.h"
#include "AgsmCombat.h"
#include "AgsmSkill.h"
#include "AgpmTitle.h"

#include "AgpmLog.h"

#include "AuRandomNumber.h"

typedef enum	_AgsmItemConvertCB {
	AGSMITEMCONVERT_CB_UPDATE_CONVERT_HISTORY_TO_DB,
} AgsmItemConvertCB;

class AgpmConfig;

class AgsmItemConvert : public AgsModule {
private:
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmItem			*m_pcsAgpmItem;
	AgpmItemConvert		*m_pcsAgpmItemConvert;
	AgpmCharacter		*m_pcsAgpmCharacter;

	AgsmAOIFilter		*m_pcsAgsmAOIFilter;
	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmItem			*m_pcsAgsmItem;
	AgsmCombat			*m_pcsAgsmCombat;
	AgsmSkill			*m_pcsAgsmSkill;
	AgpmTitle			*m_pcsAgpmTitle;

	AgpmConfig			*m_pcsAgpmConfig;
	
	AgpmLog				*m_pcsAgpmLog;

	MTRand				m_csRandom;

public:
	AgsmItemConvert();
	virtual ~AgsmItemConvert();

	BOOL				OnAddModule();
	BOOL				OnInit();

	static BOOL			CBProcessPhysicalConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBProcessSocketConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBProcessSpiritStoneConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBProcessRuneConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBProcessSocketInitialize(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBSendUpdateFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBSendItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBSendItemView(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBRuneCheckResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBSpiritStoneCheckResult(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBItemConvertHistoryInsert(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				EncodeConvertHistory(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferSize);
	BOOL				DecodeConvertHistory(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferSize);

	BOOL				SendPacketAdd(AgpdItem *pcsItem, UINT32 ulNID, BOOL bGroupNID);

	BOOL				SetCallbackUpdateConvertHistoryToDB(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL			CBCheckDefense(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBCheckSpecialStatusFactor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBCheckPoisonStatusFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBAdjustSuccessProb(PVOID pData, PVOID pClass, PVOID pCustData);

	// 2005.03.15. steeple
	BOOL				WriteConvertLog(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem, INT8 cFlag, INT8 cResult, INT32 lCost);
};

#endif	//__AGSMITEMCONVERT_H__
