#ifndef	__AGSMUISTATUS_H__
#define	__AGSMUISTATUS_H__

#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AgpmUIStatus.h"
#include "AgpmEventSkillMaster.h"

#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmItem.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmUIStatusD" )
#else
#pragma comment ( lib , "AgsmUIStatus" )
#endif
#endif

typedef enum _AgsmUIStatusCB {
	AGSMUISTATUS_CB_UPDATE_DATA_TO_DB		= 0,
} AgsmUIStatusCB;

class AgsmUIStatus : public AgsModule {
private:
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmItem				*m_pcsAgpmItem;
	AgpmSkill				*m_pcsAgpmSkill;
	AgpmUIStatus			*m_pcsAgpmUIStatus;
	AgpmEventSkillMaster	*m_pcsAgpmEventSkillMaster;

	AgsmAOIFilter			*m_pcsAgsmAOIFilter;
	AgsmCharacter			*m_pcsAgsmCharacter;
	AgsmCharManager			*m_pcsAgsmCharManager;
	AgsmItem				*m_pcsAgsmItem;

public:
	AgsmUIStatus();
	~AgsmUIStatus();

	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();

	static BOOL				CBReceiveEncodingString(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBUpdateViewHelmetOption(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBSendCharacterAllServerInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBUpdateAllToDB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBSetCharacterGameData(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL					DecodingQBeltString(AgpdCharacter *pcsCharacter, CHAR *szString, INT32 lStringLength);
	BOOL					EncodingQBeltString(AgpdCharacter *pcsCharacter, CHAR *szStringBuffer, INT32 lStringBufferLength);
	BOOL					EncodingDefaultQBeltString(AgpdCharacter *pcsCharacter, CHAR *szStringBuffer, INT32 lStringBufferLength);

	BOOL					SendQBeltString(AgpdCharacter *pcsCharacter, CHAR *szString, INT32 lStringLength, CHAR* szCooldown, INT32 lCooldownLength, UINT32 ulNID);
	BOOL					SendQBeltString(AgpdCharacter *pcsCharacter, UINT32 ulNID);
	BOOL					SendWholeQBeltInfo(AgpdCharacter *pcsCharacter, UINT32 ulNID);

	BOOL					SetCallbackUpdateDataToDB(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGSMUISTATUS_H__
