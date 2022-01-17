#ifndef	__AGSMCASPER_H__
#define	__AGSMCASPER_H__


#include "AgpmCasper.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmItemManager.h"

#include "AuRandomNumber.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmCasperD" )
#else
#pragma comment ( lib , "AgsmCasper" )
#endif
#endif

class AgsmCasper : public AgsModule {
private:
	AgpmCasper			*m_pcsAgpmCasper;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmItem			*m_pcsAgpmItem;
	AgpmItemConvert		*m_pcsAgpmItemConvert;
	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmCharManager		*m_pcsAgsmCharManager;
	AgsmItemManager		*m_pcsAgsmItemManager;

	BOOL		SendPacketFail(UINT32 ulNID);
	BOOL		SendPacketSuccess(INT32 lCreatedCID, UINT32 ulNID);

	MTRand		m_csRandom;

public:
	AgsmCasper();
	virtual	~AgsmCasper();

	BOOL		OnAddModule();

	static BOOL CBRequestConnection(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBMemoryOverflow(PVOID pData, PVOID pClass, PVOID pCustData);
};


#endif	//__AGPMCASPER_H__
