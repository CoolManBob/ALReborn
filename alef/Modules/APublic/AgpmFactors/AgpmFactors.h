// AgpmFactors.h: interface for the AgpmFactors class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPMFACTORS_H__BF339BA7_252E_4C47_9413_0D2F8D3AA7DF__INCLUDED_)
#define AFX_AGPMFACTORS_H__BF339BA7_252E_4C47_9413_0D2F8D3AA7DF__INCLUDED_

#include "ApBase.h"
#include "ApModuleStream.h"
#include "AuPacket.h"
#include "AgpdFactors.h"

#define	AGPM_FACTORS_INI_START						"FactorStart"
#define AGPM_FACTORS_INI_END						"FactorEnd"

// 추가(010803, Bob)
#define AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_LENGTH		64
#define AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_MAX		20
#define AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_RACE		"Race"
#define AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_CLASS	"Class"
#define AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_GENDER	"Gender"

#define AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE			0x80000000

#define	AGPM_FACTORS_MAX_NUM_DATA_TYPE_NAME				20
#define	AGPM_FACTORS_MAX_LENGTH_DATA_TYPE_NAME			32

typedef enum _eAgpmFactorsFlag {
	AGPM_FACTORS_FLAG_CHAR_STATUS		= 1 << AGPD_FACTORS_TYPE_CHAR_STATUS,
	AGPM_FACTORS_FLAG_CHAR_TYPE			= 1 << AGPD_FACTORS_TYPE_CHAR_TYPE,
	AGPM_FACTORS_FLAG_CHAR_POINT		= 1 << AGPD_FACTORS_TYPE_CHAR_POINT,
	AGPM_FACTORS_FLAG_CHAR_POINT_MAX	= 1 << AGPD_FACTORS_TYPE_CHAR_POINT_MAX,
	AGPM_FACTORS_FLAG_ATTRIBUTE			= 1 << AGPD_FACTORS_TYPE_ATTRIBUTE,
	AGPM_FACTORS_FLAG_DAMAGE			= 1 << AGPD_FACTORS_TYPE_DAMAGE,
	AGPM_FACTORS_FLAG_DEFENSE			= 1 << AGPD_FACTORS_TYPE_DEFENSE,
	AGPM_FACTORS_FLAG_ATTACK			= 1 << AGPD_FACTORS_TYPE_ATTACK,
	AGPM_FACTORS_FLAG_ITEM				= 1 << AGPD_FACTORS_TYPE_ITEM,
	AGPM_FACTORS_FLAG_DIRT				= 1 << AGPD_FACTORS_TYPE_DIRT,
	AGPM_FACTORS_FLAG_PRICE				= 1 << AGPD_FACTORS_TYPE_PRICE,
	AGPM_FACTORS_FLAG_RESULT			= 1 << AGPD_FACTORS_TYPE_RESULT,
	AGPM_FACTORS_FLAG_OWNER				= 1 << AGPD_FACTORS_TYPE_OWNER
} _eAgpmFactorsFlag;

// Stream에서 사용(010308, Bob)
typedef enum _eAgpmFactorStreamType {
	AGPM_FACTORS_STREAM_TYPE_CHARTYPE_NAME = 0,
	AGPM_FACTORS_STREAM_TYPE_NUM
} eAgpmFactorStreamType;

typedef enum _eAgpmFactorsCBID {
	AGPM_FACTORS_CB_UPDATE_FACTOR_PARTY		= 0,
	AGPM_FACTORS_CB_UPDATE_FACTOR_UI,
	AGPM_FACTORS_CB_UPDATE_FACTOR_SKILL_POINT,
	AGPM_FACTORS_CB_UPDATE_FACTOR_DAMAGE,
	AGPM_FACTORS_CB_UPDATE_FACTOR_VIEW,
	AGPM_FACTORS_CB_UPDATE_FACTOR_MOVEMENT,
	AGPM_FACTORS_CB_UPDATE_FACTOR_BONUS_EXP,
	AGPM_FACTORS_CB_UPDATE_FACTOR_HP,
	AGPM_FACTORS_CB_UPDATE_FACTOR_MP,
	AGPM_FACTORS_CB_UPDATE_FACTOR_MURDERER_POINT,
	AGPM_FACTORS_CB_UPDATE_FACTOR_DURABILITY,
	AGPM_FACTORS_CB_UPDATE_FACTOR_CHARISMA_POINT,
	AGPM_FACTORS_CB_UPDATE_FACTOR_HEROIC_POINT,
} eAgpmFactorsCBID;


class AgpmFactorCharTypeNameSetData
{
public:
	INT32	m_lIndex;
	CHAR	m_szName[AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_LENGTH];

	AgpmFactorCharTypeNameSetData()
	{
		m_lIndex	= 0;
		ZeroMemory(m_szName, sizeof(CHAR) * AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_LENGTH);
	}
};

class AgpmFactorCharTypeNameSet
{
public:
	INT32 m_lRaceNum;
	INT32 m_lGenderNum;
	INT32 m_lClassNum;

//	AgpmFactorCharTypeNameSetData m_csRace[AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_MAX];
//	AgpmFactorCharTypeNameSetData m_csGender[AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_MAX];
	ApSafeArray<AgpmFactorCharTypeNameSetData, AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_MAX>	m_csRace;
	ApSafeArray<AgpmFactorCharTypeNameSetData, AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_MAX>	m_csGender;
	AgpmFactorCharTypeNameSetData m_csClass[AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_MAX][AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_MAX];

	AgpmFactorCharTypeNameSet()
	{
		m_lRaceNum		= 0;
		m_lGenderNum	= 0;
		m_lClassNum		= 0;
	}
};

class AgpmFactors : public ApModule
{
private:
	PVOID	UpdateFactor(PVOID pvFactor, PVOID pvUpdateFactor, eAgpdFactorsType eType, BOOL bMakePacket);
	BOOL	CopyFactorNotZero(PVOID pcsDestFactor, PVOID pcsSourceFactor, eAgpdFactorsType eType);
	PVOID	CalcFactor(PVOID pvFactor, PVOID pvUpdateFactor, eAgpdFactorsType eType, BOOL bZero, BOOL bMakePacket, BOOL bAdd);
	BOOL	CalcPercentFactor(PVOID pvTargetFactor, PVOID pvPointFactor, PVOID pvPercentFactor, eAgpdFactorsType eType);
	BOOL	CompareFactor(PVOID pvDestFactor, PVOID pvSourceFactor, eAgpdFactorsType eType, INT32 lValue);
	PVOID	InitUpdateFactor(PVOID pvFactor, eAgpdFactorsType eType);
	BOOL	StreamReadFactor(PVOID *ppvFactor, eAgpdFactorsType eType, ApModuleStream *pStream, BOOL bEmbedded = FALSE);
	BOOL	StreamWriteFactor(PVOID pvFactor, eAgpdFactorsType eType, ApModuleStream *pStream, BOOL bEmbedded = FALSE);
	BOOL	AddResultFactor(PVOID pvDestFactor, PVOID pvSourceFactor, eAgpdFactorsType eType);
	PVOID	MakePacketFactor(PVOID pvFactor, eAgpdFactorsType eType, INT32 lSubType1 = -1, INT32 lSubType2 = -1, INT32 lSubType3 = -1);

	BOOL	DivideFactor(PVOID pvFactor, eAgpdFactorsType eType, INT32 lDivideNum);

	PVOID	SetFactor(PVOID *ppvDstFactor, PVOID pvSrcFactor, INT16 nType);

	INT32	*GetValuePointer(PVOID pvFactor, eAgpdFactorsType eType, INT32 lType1, INT32 lType2 = -1, INT32 lType3 = -1); // Add(290903, BOB)
	BOOL	GetValue(PVOID pvFactor, INT32 *plValue, eAgpdFactorsType eType, INT32 lType1, INT32 lType2 = -1, INT32 lType3 = -1);
	BOOL	SetValue(PVOID pvFactor, INT32 lValue, eAgpdFactorsType eType, INT32 lType1, INT32 lType2 = -1, INT32 lType3 = -1);
	BOOL	GetValue(PVOID pvFactor, FLOAT *pfValue, eAgpdFactorsType eType, INT32 lType1, INT32 lType2 = -1, INT32 lType3 = -1);
	BOOL	SetValue(PVOID pvFactor, FLOAT fValue, eAgpdFactorsType eType, INT32 lType1, INT32 lType2 = -1, INT32 lType3 = -1);

	// 010803, Bob
	static BOOL CharacterTypeStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL CharacterTypeStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	// 010903, Bob
	BOOL	ParseCharacterTypeClass(CHAR *szValue, AgpmFactorCharTypeNameSet* pSet, INT16 nRaceNum, INT16 nClassIndex);

public:
	// 072403 Bob (실패시 -1 리턴한당...)
	INT32						FindCharacterRaceIndex(CHAR *szRace);
	INT32						FindCharacterClassIndex(CHAR *szClass);
	INT32						FindCharacterClassIndex(CHAR *szRace, CHAR *szClass);

	CHAR*						GetCharacterRaceName(INT32 lRace);
	CHAR*						GetCharacterClassName(INT32 lRace, INT32 lClass);
	CHAR*						GetCharacterGenderName(INT32 lGender);

	// 010803, Bob
	BOOL						CharacterTypeStreamRead(CHAR *szFile, BOOL bDecryption);
	BOOL						CharacterTypeStreamWrite(CHAR *szFile, BOOL bEncryption);
	AgpmFactorCharTypeNameSet	*GetCharTypeNameSet() {return &m_csAgpmFactorCharTypeNameSet;}

	CHAR						*GetFactorTypeName(INT32 lIndex);
	CHAR						*GetFactorININame(INT32 lIndex, INT32 lSubIndex);
	AgpdFactorDetail			*GetFactorDetail(INT32 lIndex);

protected:
	// 추가(010803, Bob)
	AgpmFactorCharTypeNameSet m_csAgpmFactorCharTypeNameSet;

public:
	AuPacket	m_csPacket;
	//AuPacket	m_acsPacket[AGPD_FACTORS_MAX_TYPE];
	ApSafeArray<AuPacket, AGPD_FACTORS_MAX_TYPE>	m_acsPacket;

	ApMemoryPool				m_csMemoryPoolFactorType[AGPD_FACTORS_MAX_TYPE];

	ApMemoryPool				m_csMemoryPoolAgpdFactor;

	AgpmFactors();
	virtual ~AgpmFactors();

	// Virtual Function 들
	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnIdle(UINT32 ulClockCount);
	BOOL	OnDestroy();

	BOOL	InitFactor(AgpdFactor *pcsFactor);
	BOOL	DestroyFactor(AgpdFactor *pcsFactor);

	BOOL	IsUseFactor(AgpdFactor *pcsFactor);

	BOOL	DestroyFactor(AgpdFactor *pcsFactor, INT16 nType);

	PVOID					SetFactor(AgpdFactor *pcsDstFactor, AgpdFactor *pcsSrcFactor, INT16 nType);
	AgpdFactor *			SetFactors(AgpdFactor *pcsDstFactor, AgpdFactor *pcsSrcFactor, INT16 nFlag);

	PVOID					GetFactor(AgpdFactor *pcsFactor, eAgpdFactorsType eType);

	BOOL	ReflectPacket(PVOID pvFactor, PVOID pvPacket, INT16 nPacketLength, eAgpdFactorsType eType);
	BOOL	ReflectPacket(AgpdFactor *pcsFactor, /*AgpdFactor *pcsFactorResult,*/ PVOID pvPacket, INT16 nPacketLength, AgpdFactor *pcsFactorBuffer = NULL);

	PVOID	MakePacketFactors(AgpdFactor *pcsFactor, eAgpdFactorsType eType = AGPD_FACTORS_TYPE_NONE, INT32 lSubType1 = -1, INT32 lSubType2 = -1, INT32 lSubType3 = -1);
	PVOID	MakePacketFactorsOneType(AgpdFactor *pcsFactor, eAgpdFactorsType eType);
	PVOID	MakePacketResult(AgpdFactor *pcsFactor);
	PVOID	MakePacketFactorsCharView(AgpdFactor *pcsFactor);
	PVOID	MakePacketFactorsCharHPMax(AgpdFactor *pcsFactor);
	PVOID	MakePacketFactorsCharHP(AgpdFactor *pcsFactor);
	PVOID	MakePacketFactorsItemView(AgpdFactor *pcsFactor);
	PVOID	MakePacketMoney(AgpdFactor *pcsFactor);
	PVOID	MakePacketItem(AgpdFactor *pcsFactor);
	PVOID	MakePacketItemRestrict(AgpdFactor *pcsFactor);
	PVOID	MakePacketLevel(AgpdFactor *pcsFactor);

	PVOID	MakePacketFactorDiffer(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor);
	PVOID	MakePacketFactorDiffer(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor, eAgpdFactorsType eType);

	INT32	GetAttackableRange( AgpdFactor *pcsSrcFactor, AgpdFactor *pcsDstFactor );
	BOOL	IsInRange(AuPOS *pstSrcPos, AuPOS *pstDstPos, AgpdFactor *pcsSrcFactor, AgpdFactor *pcsDstFactor, AuPOS *pstCalcPos = NULL, INT32 *plRange = NULL, INT32 *plTargetRange = NULL);
	BOOL	IsInRange(AuPOS *pstSrcPos, AuPOS *pstDstPos, INT32 lAttackRange, INT32 lHitRange, AuPOS *pstCalcPos = NULL, INT32 *plTargetRange = NULL);

	PVOID	UpdateFactor(AgpdFactor *pcsFactor, AgpdFactor *pcsUpdateFactor, BOOL bMakePacket = TRUE);
	PVOID	CalcFactor(AgpdFactor *pcsFactor, AgpdFactor *pcsUpdateFactor, BOOL bZero = TRUE, BOOL bMakePacket = TRUE, BOOL bAdd = TRUE, BOOL bCheckUpdate = TRUE);

	PVOID	InitUpdateFactor(AgpdFactor *pcsFactor, eAgpdFactorsType eType);
	PVOID	InitCalcFactor(AgpdFactor *pcsFactor, eAgpdFactorsType eType);

	BOOL	CompareFactor(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor, INT32 lValue = 0);

	BOOL	DivideFactor(AgpdFactor *pcsSrcFactor, INT32 lDivideNum);

	// streaming functions
	BOOL	StreamRead(AgpdFactor *pcsFactor, ApModuleStream *pStream);
	BOOL	StreamWrite(AgpdFactor *pcsFactor, ApModuleStream *pStream);

	BOOL	CopyFactor(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor, BOOL bCopyCharPoint, BOOL bCopyResult = TRUE, eAgpdFactorsType eType = AGPD_FACTORS_TYPE_NONE, BOOL bOverwrite = FALSE);
	BOOL	CopyFactorNotNull(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor);
	BOOL	CopyFactorNotZero(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor);
	BOOL	CopyFactorStatus(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor);

	BOOL	AddResultFactor(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor);

	BOOL	CalcResultFactor(AgpdFactor *pcsTargetFactor, AgpdFactor *pcsFactorChar, AgpdFactor *pcsPointFactor, AgpdFactor *pcsPercentFactor);

	BOOL	SetOwnerFactor(AgpdFactor *pcsFactor, INT32 lID, PVOID pvOwner);

	BOOL	SetCallbackUpdateFactorParty(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorUI(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorSkillPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorDamage(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorView(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorMovement(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorBonusExp(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorHp(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorMp(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorMurdererPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorCharismaPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorDurability(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateFactorHeroicPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	INT32	*GetValuePointer(AgpdFactor *pcsFactor, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2 = -1, INT32 lSubType3 = -1); // Add(290903, BOB)
	BOOL	GetValue(AgpdFactor *pcsFactor, INT32 *plValue, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2 = -1, INT32 lSubType3 = -1);
	BOOL	SetValue(AgpdFactor *pcsFactor, INT32 lValue, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2 = -1, INT32 lSubType3 = -1);
	BOOL	GetValue(AgpdFactor *pcsFactor, FLOAT *pfValue, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2 = -1, INT32 lSubType3 = -1);
	BOOL	SetValue(AgpdFactor *pcsFactor, FLOAT fValue, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2 = -1, INT32 lSubType3 = -1);

	BOOL	IsFloatValue(eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2 = -1, INT32 lSubType3 = -1);

	// 각종 get 함수...
	INT32	GetRace(AgpdFactor *pcsFactor);
	BOOL	CheckRace(AuRaceType eCheckRace, AgpdFactor *pcsFactor);
	INT32	GetGender(AgpdFactor *pcsFactor);
	INT32	GetClass(AgpdFactor *pcsFactor);
	BOOL	CheckClass(AuCharClassType eCheckClass, AgpdFactor *pcsFactor);

	INT32	GetMurderPoint(AgpdFactor *pcsFactor);

	INT32	GetLevel		(AgpdFactor *pcsFactor);
	INT32	GetLevelBefore	(AgpdFactor *pcsFactor);
	INT32	GetLevelLimited	(AgpdFactor *pcsFactor);

	INT32	GetHP(AgpdFactor *pcsFactor);
	INT32	GetMaxHP(AgpdFactor *pcsFactor);
	INT32	GetMP(AgpdFactor *pcsFactor);
	INT32	GetMaxMP(AgpdFactor *pcsFactor);
	INT32	GetSP(AgpdFactor *pcsFactor);
	INT32	GetMaxSP(AgpdFactor *pcsFactor);

	INT64	GetExp(AgpdFactor *pcsFactor);
	INT64	GetMaxExp(AgpdFactor *pcsFactor);

	BOOL	SetExp(AgpdFactor *pcsFactor, INT64 llExp);
	BOOL	SetMaxExp(AgpdFactor *pcsFactor, INT64 llMaxExp);

	INT32	GetSkillPoint(AgpdFactor *pcsFactor);
	INT32	GetSkillLevel(AgpdFactor *pcsFactor);
	INT32	GetHeroicPoint(AgpdFactor *pcsFacter);

	BOOL	InitDamageFactors(AgpdFactor *pcsFactor, AgpdFactor *pcsFactorBuffer = NULL);

	AgpdFactor	*AllocAgpdFactor(INT32 lDataIndex = 0);
	BOOL	FreeAgpdFactor(AgpdFactor *pcsAgpdFactor, INT32 lDataIndex = 0);

	PVOID	AllocFactorType(eAgpdFactorsType eType);
	BOOL	FreeFactorType(eAgpdFactorsType eType, PVOID pvFactor);

	INT32	SetFactorDataName(CHAR *szDataName);

	BOOL	PrintDataTypeLog();

	BOOL	IsMustSend(eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2 = -1, INT32 lSubType3 = -1);
};

#endif // !defined(AFX_AGPMFACTORS_H__BF339BA7_252E_4C47_9413_0D2F8D3AA7DF__INCLUDED_)
