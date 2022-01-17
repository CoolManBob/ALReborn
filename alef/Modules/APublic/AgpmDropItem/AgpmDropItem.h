#if !defined(__AGPMDROPITEM_H__)
#define __AGPMDROPITEM_H__

#include "AgpdDropItem.h"
#include "AgpaDropItem.h"

#include "ApModule.h"
#include "AgpmItem.h"
#include "AuExcelTxtLib.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmDropItemD" )
#else
#pragma comment ( lib , "AgpmDropItem" )
#endif
#endif
//@} Jaewon

#define AGPMDROPITEM_STREAM_TEMPLATENAME						"TemplateName"
#define AGPMDROPITEM_STREAM_BAG									"Bag"
#define AGPMDROPITEM_STREAM_DROPITEMCOUNT						"DropItemCount"

#define AGPMDROPITEM_STREAM_DROPITEMTID							"DropItemTID"
#define AGPMDROPITEM_STREAM_DROPITEMNAME						"DropItemName"
#define AGPMDROPITEM_STREAM_DROPRATE							"DropRate"
#define AGPMDROPITEM_STREAM_EQUIPRATE							"EquipRate"
#define AGPMDROPITEM_STREAM_MINQUANTITY							"MinQuantity"
#define AGPMDROPITEM_STREAM_MAXQUANTITY							"MaxQuantity"
#define AGPMDROPITEM_STREAM_JACKPOTRATE							"JackpotRate"
#define AGPMDROPITEM_STREAM_JACKPOTPIECE						"JackpotPiece"

#define AGPMDROPITEM_STREAM_RUNEOPT_DAMAGE						"RuneoptDamage"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINDAMAGE					"RuneoptMinDamage"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXDAMAGE					"RuneoptMaxDamage"
#define AGPMDROPITEM_STREAM_RUNEOPT_ATTACKPOINT					"RuneoptAttackPoint"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINATTACKPOINT				"RuneoptMinAttackPoint"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXATTACKPOINT				"RuneoptMaxAttackPoint"
#define AGPMDROPITEM_STREAM_RUNEOPT_DEFENSERATE					"RuneoptDefenseRate"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINDEFENSERATE				"RuneoptMinDefenseRate"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXDEFENSERATE				"RuneoptMaxDefenseRate"
#define AGPMDROPITEM_STREAM_RUNEOPT_DEFENSEPOINT				"RuneoptDefensePoint"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINDEFENSEPOINT				"RuneoptMinDefensePoint"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXDEFENSEPOINT				"RuneoptMaxDefensePoint"
#define AGPMDROPITEM_STREAM_RUNEOPT_HP							"RuneoptHP"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINHP						"RuneoptMinHP"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXHP						"RuneoptMaxHP"
#define AGPMDROPITEM_STREAM_RUNEOPT_MP							"RuneoptMP"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINMP						"RuneoptMinMP"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXMP						"RuneoptMaxMP"
#define AGPMDROPITEM_STREAM_RUNEOPT_SP							"RuneoptSP"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINSP						"RuneoptMinSP"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXSP						"RuneoptMaxSP"
#define AGPMDROPITEM_STREAM_RUNEOPT_HPREGEN						"RuneoptHPRegen"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINHPREGEN					"RuneoptMinHPRegen"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXHPREGEN					"RuneoptMaxHPRegen"
#define AGPMDROPITEM_STREAM_RUNEOPT_MPREGEN						"RuneoptMPRegen"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINMPREGEN					"RuneoptMinMPRegen"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXMPREGEN					"RuneoptMaxMPRegen"
#define AGPMDROPITEM_STREAM_RUNEOPT_SPREGEN						"RuneoptSPRegen"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINSPREGEN					"RuneoptMinSPRegen"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXSPREGEN					"RuneoptMaxSPRegen"
#define AGPMDROPITEM_STREAM_RUNEOPT_ATTACKSPEED					"RuneoptAttackSpeed"
#define AGPMDROPITEM_STREAM_RUNEOPT_MINATTACKSPEED				"RuneoptMinAttackSpeed"
#define AGPMDROPITEM_STREAM_RUNEOPT_MAXATTACKSPEED				"RuneoptMaxAttackSpeed"

#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT0						"ItemDropCount0"
#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT1						"ItemDropCount1"
#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT2						"ItemDropCount2"
#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT3						"ItemDropCount3"
#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT4						"ItemDropCount4"
#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT5						"ItemDropCount5"
#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT6						"ItemDropCount6"
#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT7						"ItemDropCount7"
#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT8						"ItemDropCount8"
#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT9						"ItemDropCount9"
#define AGPMDROPITEM_STREAM_ITEMDROP_COUNT10					"ItemDropCount10"

#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT0				"EquipItemDropCount0"
#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT1				"EquipItemDropCount1"
#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT2				"EquipItemDropCount2"
#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT3				"EquipItemDropCount3"
#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT4				"EquipItemDropCount4"
#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT5				"EquipItemDropCount5"
#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT6				"EquipItemDropCount6"
#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT7				"EquipItemDropCount7"
#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT8				"EquipItemDropCount8"
#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT9				"EquipItemDropCount9"
#define AGPMDROPITEM_STREAM_EQUIPITEMDROP_COUNT10				"EquipItemDropCount10"

#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT0						"RuneDropCount0"
#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT1						"RuneDropCount1"
#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT2						"RuneDropCount2"
#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT3						"RuneDropCount3"
#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT4						"RuneDropCount4"
#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT5						"RuneDropCount5"
#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT6						"RuneDropCount6"
#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT7						"RuneDropCount7"
#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT8						"RuneDropCount8"
#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT9						"RuneDropCount9"
#define AGPMDROPITEM_STREAM_RUNEDROP_COUNT10					"RuneDropCount10"

typedef enum
{
	AGPMDROPITEM_DATA_TEMPLATE	= 0,
	AGPMDROPITEM_DATA_EQUIP_TEMPLATE,
} AgpmDropItemData;

class AgpmDropItem : public ApModule
{
	AgpmItem				*m_pcsAgpmItem;
	AuExcelTxtLib			m_csExcelLib;

public:
	AgpaDropItemTemplate		m_aDropItemTemplate;
	AgpaDropItemEquipTemplate	m_aEquipItemTemplate;

	AgpmDropItem();
	~AgpmDropItem();
	
	BOOL			OnAddModule();

//	StreamWriteTemplate(CHAR *szFile);
//	StreamReadTemplate(CHAR *szFile);

//	static BOOL		CBDropItemTemplateWrite(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
//	static BOOL		CBDropItemTemplateRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	//ReadDropTemplate
	BOOL			ReadTemplate( char *pstrFileName );
	BOOL			ReadEquipItemDropTemplate( char *pstrFileName );

	INT32			GetEmptySlot( AgpdDropItemData *pcsDropItemData );
	AgpdDropEquipData *GetEmptyEquipRaceSlot( AgpdDropItemEquipTemplate *pcsDropItemEquipTemplate );
	INT32			GetEmptyEquipItemSlot( AgpdDropEquipData *pcsDropEquipData );
};

#endif
