#if !defined(__AGSMDROPITEM2_H__)
#define __AGSMDROPITEM2_H__

#include "AuRandomNumber.h"
#include "ApModule.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmParty.h"
#include "AgpmItem.h"
#include "AgpmDropItem2.h"
#include "AgpmConfig.h"

#include "AgsmCharacter.h"
#include "AgsmItemManager.h"
#include "AgsmItem.h"
#include "AgsmParty.h"
#include "AgpmAI2.h"
#include "AgsmTitle.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmDropItem2D" )
#else
#pragma comment ( lib , "AgsmDropItem2" )
#endif
#endif

class AgsmTitle;

class AgsmDropItem2 : public AgsModule
{
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmParty			*m_pcsAgpmParty;
	AgpmItem			*m_pcsAgpmItem;
	AgpmItemConvert		*m_pcsAgpmItemConvert;
	AgpmDropItem2		*m_pcsAgpmDropItem2;

	AgpmConfig			*m_pcsAgpmConfig;

	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmItemManager		*m_pcsAgsmItemManager;
	AgsmItem			*m_pcsAgsmItem;
	AgsmParty			*m_pcsAgsmParty;

	AgpmAI2				*m_pcsAgpmAI2;

	AgsmTitle			*m_pcsAgsmTitle;

	INT32				m_nIndexDropTemplateInfo;

	MTRand				m_csRand;

public:
	AgsmDropItem2();
	~AgsmDropItem2();

	BOOL OnAddModule();
	BOOL OnInit();

	BOOL DropItem(AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter = NULL);
	BOOL DropItemToField(AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter, AgpdItem *pcsItem);

	BOOL DropLootItem(AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter);

	static BOOL CBDropMoneyToField(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDropItem(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif
