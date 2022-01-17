#if !defined(__AGPMDROPITEM2_H__)
#define __AGPMDROPITEM2_H__

#include "AgpdDropItem2.h"

#include "ApModule.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AuExcelTxtLib.h"
#include "AgpmBillInfo.h"
#include "AgpmConfig.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmDropItem2D" )
#else
#pragma comment ( lib , "AgpmDropItem2" )
#endif
#endif
//@} Jaewon

#define AGPMDROP_RATE_LIMIT		100000
#define AGPMDROP_RATE_RATIO		1000

typedef std::vector<INT32>		EventItemVector;

struct EventItemEffect
{
	INT32 m_lEventID;
	EventItemVector m_vcItemTIDs;
	INT32 m_lEffectID;
	INT32 m_lRate;
	INT32 m_lEnable;

	EventItemEffect() : m_lEventID(0),
						m_lEffectID(0),
						m_lRate(0),
						m_lEnable(0)
	{
	}
};

typedef map<INT32, EventItemEffect>				EventMap;
typedef map<INT32, EventItemEffect>::iterator	EventMapIter;

typedef enum
{
	AGPM_DROPITEM_CB_DROP_ITEM	= 0,
	AGPM_DROPITEM_CB_GET_BONUS_OF_PARTY_MEMBER,
} AgpmDropItem2CBPoint;

class AgpmDropItem2 : public ApModule
{
public:
	AgpmFactors *			m_pcsAgpmFactors;
	AgpmCharacter *			m_pcsAgpmCharacter;
	AgpmItem *				m_pcsAgpmItem;
	AgpmItemConvert *		m_pcsAgpmItemConvert;
	AgpmBillInfo *			m_pcsAgpmBillInfo;
	AgpmConfig				*m_pcsAgpmConfig;		// kelovon, 20051007

	INT32					m_lGroupCount;
	ApAdmin					m_csDropGroups;

	AgpdDropItem2Map		m_csDropGroups2;

	INT32					m_lSocketCount;
	AgpdDropItemSocket *	m_pcsDropSockets;

	INT32					m_lOptionCount;
	AgpdDropItemOptionNum *	m_pcsDropOptionNums;

	BOOL					m_bReadRankRate;

	AgpdDropItemOption		m_acsDropOptions		[AGPDITEM_OPTION_MAX_PART][AGPMCHAR_MAX_LEVEL + 1][AGPMDROP_MAX_ITEMRANK];
	AgpdDropItemOption		m_acsRefineryOptions	[AGPDITEM_OPTION_MAX_PART][AGPMCHAR_MAX_LEVEL + 1][AGPMDROP_MAX_ITEMRANK];
	AgpdDropItemOption		m_acsGachaOptions		[AGPDITEM_OPTION_MAX_PART][AGPMCHAR_MAX_LEVEL + 1][AGPMDROP_MAX_ITEMRANK];

	INT32					m_lADIndexCharacterTemplate;
	INT32					m_lADIndexItemTemplate;

	MTRand					m_csRand;

	ApCriticalSection		m_csCSectionOptionProbability;
	INT32					m_alRankRate[AGPMDROP_MAX_RANK+1][AGPMDROP_MAX_RANK+1];

	EventMap				m_mapEventItem;
	INT32					m_lCurrentEventID;
	
public:
	AgpmDropItem2();
	~AgpmDropItem2();

	BOOL			OnAddModule();
	BOOL			OnDestroy();

	BOOL			ReadDropGroup(CHAR *szFile, BOOL bDecryption, BOOL bReload = FALSE);
	BOOL			ReadDropGroup2(CHAR *szFile, BOOL bDecryption, BOOL bReload = FALSE);
	BOOL			ReadDropSocket(CHAR *szFile, BOOL bDecryption, BOOL bReload = FALSE);
	BOOL			ReadDropOption(CHAR *szFile, BOOL bDecryption, BOOL bReload = FALSE);
	BOOL			ReadDropRankRate(CHAR *szFile, BOOL bDecryption, BOOL bReload = FALSE);

	AgpdDropItemGroup *	AddDropGroup(AgpdDropItemGroup *pcsDropGroup);
	AgpdDropItemGroup *	GetDropGroup(INT32 lID);
	AgpdDropItemGroup2*	GetDropGroup2(INT32 lID);

	AgpdDropItemADCharTemplate *	GetCharacterTemplateData(AgpdCharacterTemplate *pcsTemplate)
	{
		return (AgpdDropItemADCharTemplate *) m_pcsAgpmCharacter->GetAttachedModuleData(m_lADIndexCharacterTemplate, pcsTemplate);
	}

	AgpdDropItemADItemTemplate *	GetItemTemplateData(AgpdItemTemplate *pcsTemplate)
	{
		return (AgpdDropItemADItemTemplate *) m_pcsAgpmItem->GetAttachedModuleData(m_lADIndexItemTemplate, pcsTemplate);
	}

	INT32			AdjustRateByLevelGap(INT32	lDropRate, INT32 lTargetLevel, INT32 lAttackerLevel);
	FLOAT			CalcLevelGap(INT32 lTargetLevel, INT32 lAttackerLevel);
	BOOL			ProcessDropItem(AgpdCharacter *pcsDropCharacter, INT32 lAttackerLevel, ApBase *pcsFirstLooter = NULL);
	BOOL			ProcessDropItem2(AgpdCharacter *pcsDropCharacter, INT32 lAttackerLevel, ApBase *pcsFirstLooter = NULL);
	INT32			GetDropRank(AgpdDropItemGroup *pcsDropGroup, INT32 lMobRank);
	INT32			GetDropRate(AgpdItemTemplate *pcsItemTemplate, INT32 lMobLevel, INT32 lMobRank, AgpmItemUsableSpiritStoneType eMobSpiritType, AgpdCharacter *pcsAttacker = NULL);
	BOOL			GetDropItemList(AgpdDropItems *pvtDropList, AgpdDropItems *pvtResultList, INT32 lTotalRate, INT32 lItemNum = 1);

	INT32			GetItemOptionNum(AgpdItem *pcsItem);
	AgpdItemOptionPart	GetItemOptionPart(AgpdItemTemplate *pcsItemTemplate);

	BOOL			ProcessConvertItem(AgpdItem *pcsItem, AgpdCharacter *pcsDropCharacter);
	BOOL			ProcessConvertItemSocket(AgpdItem *pcsItem, INT32 MinSocket, INT32 MaxSocket);
	BOOL			ProcessConvertItemPhysical(AgpdItem *pcsItem, AgpdCharacter *pcsDropCharacter);
	BOOL			ProcessConvertItemOption(AgpdItem *pcsItem, AgpdCharacter *pcsDropCharacter);
	BOOL			ProcessConvertItemOption(AgpdItem *pcsItem, INT32 lTargetLevel);
    BOOL            ProcessConvertItemOptionRefinery(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter, INT32 OptionMin, INT32 OptionMax);
    BOOL            ProcessConvertItemOptionGacha(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter, INT32 OptionMin, INT32 OptionMax );

	BOOL			SetCallbackDropItem(ApModuleDefaultCallBack fnCallback, PVOID pClass);
	BOOL			SetCallbackGetBonusOfPartyMember(ApModuleDefaultCallBack fnCallback, PVOID pClass);

	// Callbacks
	static BOOL		CBStreamImportItemData(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL		CBStreamImportCharacterData(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL		CBStreamEndItemOption(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	static BOOL		CBConvertAsDrop(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	void			SetCurrentEventID(INT32 lEventID) { m_lCurrentEventID = lEventID; }
	INT32			GetCurrentEventID() { return m_lCurrentEventID; }

	BOOL			IsEventItem(INT32 lTID);
};

#endif
