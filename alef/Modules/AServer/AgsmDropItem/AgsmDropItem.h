#if !defined(__AGSMDROPITEM_H__)
#define __AGSMDROPITEM_H__

#include "AuRandomNumber.h"
#include "ApModule.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmParty.h"
#include "AgpmItem.h"
#include "AgpmDropItem.h"
#include "AgsmItemManager.h"
#include "AgsmItem.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmDropItemD" )
#else
#pragma comment ( lib , "AgsmDropItem" )
#endif
#endif

class AgsmDropItem : public AgsModule
{
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmParty			*m_pcsAgpmParty;
	AgpmItem			*m_pcsAgpmItem;
	AgpmItemConvert		*m_pcsAgpmItemConvert;
	AgpmDropItem		*m_pcsAgpmDropItem;
	AgsmItemManager		*m_pcsAgsmItemManager;
	AgsmItem			*m_pcsAgsmItem;

	INT32				m_nIndexDropTemplateInfo;

	MTRand				m_csRand;

public:
	AgsmDropItem();
	~AgsmDropItem();

	static BOOL CBDropMoneyToField(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdDropItemTemplate* GetDropItemTemplate(AgpdCharacter *pData, INT32 lIndex);
	BOOL SetDropItemTemplate(AgpdDropItemTemplate *pcsTemplate, AgpdCharacter *pData, INT32 lIndex );
	BOOL DropItem( AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter = NULL );
	BOOL DropItem( AgpdCharacter *pcsAgpdCharacter, AgpdDropItemBag *pcsAgpdDropItemBag, ApBase *pcsFirstLooter = NULL );
	BOOL DropItemToField( AgpdCharacter *pcsCharacter, AgpdDropItemBag *pcsAgpdDropItemBag, INT32 lIndex, INT32 lPCLevel, INT32 lMobLevel, ApBase *pcsFirstLooter = NULL, BOOL bCheckDropRate = TRUE );
	INT32 GetRandomRuneOptValue( INT32 lMinValue, INT32 lMaxValue );
	BOOL ProcessRuneOption( AgpdItem *pcsAgpdItem, AgpdDropItemInfo *pcsAgpdDropItemInfo );
	BOOL AddItemToField( AgpdCharacter *pcsAgpdCharacter, AgpdDropItemInfo *pcsAgpdDropItemInfo, INT32 lQuantity, ApBase *pcsFirstLooter = NULL );

	INT32 GetProbableByLvlGap( AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter );
	BOOL DropCommonItem( AgpdCharacter *pcsCharacter, ApBase *pcsFirstLooter, AgpdDropItemTemplate *pcsAgpdDropItemTemplate );
	BOOL DropEquipItem( AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter, AgpdDropItemEquipTemplate *pcsAgpdDropEquipItemTemplate, float fProbable );

//	BOOL AddItemToField( AgpdCharacter *pcsAgpdCharacter, INT32 lItemTID, ApBase *pcsFirstLooter );
	BOOL AddItemToField( AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter, INT32 lItemTID, INT32 lDropCount = 0 );

	BOOL OnAddModule();
};

#endif
