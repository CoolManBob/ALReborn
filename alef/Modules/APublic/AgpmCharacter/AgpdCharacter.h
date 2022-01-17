#ifndef	_AGPDCHARACTER_H_
#define _AGPDCHARACTER_H_

#include "ApBase.h"
#include "AgpmFactors.h"
#include "AgpdTitle.h"
#include "AgpdTitleQuest.h"

#define	AGPDCHARACTER_MAX_BLOCK_INFO		12
#define AGPDCHARACTER_MAX_TARGET			99
#define	AGPDCHARACTER_MAX_ID_LENGTH			48		// 게임에서 사용되는 캐릭터 이름 길이 -> NPC 이름 길이 때문에 늘어났음
const INT32 AGPDCHARACTER_NAME_LENGTH		= 32;	// DB에서 사용되는 캐릭터 이름 길이

#define	AGPDCHARACTER_MAX_INVEN_MONEY		1000000000
#define	AGPDCHARACTER_MAX_BANK_MONEY		1000000000
#define	AGPDCHARACTER_MAX_CASH				1000000000000000000

typedef ApString<AGPDCHARACTER_NAME_LENGTH> ApStrCharacterName;
/*
typedef enum _AgpdCharType {
	AGPDCHAR_TYPE_NONE						= (-1),
	AGPDCHAR_TYPE_PC,
	AGPDCHAR_TYPE_NPC,
	AGPDCHAR_TYPE_MONSTER,
	AGPDCHAR_TYPE_CREATURE
} AgpdCharType;
*/

const int	AGPMCHAR_TYPE_NONE						= 0x00000000;
const int	AGPMCHAR_TYPE_PC						= 0x00000001;
const int	AGPMCHAR_TYPE_NPC						= 0x00000002;
const int	AGPMCHAR_TYPE_MONSTER					= 0x00000004;
const int	AGPMCHAR_TYPE_CREATURE					= 0x00000008;
const int	AGPMCHAR_TYPE_GUARD						= 0x00000010;
const int	AGPMCHAR_TYPE_GM						= 0x00000020;
const int	AGPMCHAR_TYPE_ATTACKABLE				= 0x00010000;
const int	AGPMCHAR_TYPE_TARGETABLE				= 0x00020000;
const int	AGPMCHAR_TYPE_MOVABLENPC				= 0x00040000;	// 마고자 (2005-06-22 오후 4:25:55) : 움직여 다니는 NPC 만 체크.
const int	AGPMCHAR_TYPE_POLYMORPH					= 0x00080000;	//. 2006. 6. 21. Nonstopdj. 변형가능한 type.
const int	AGPMCHAR_TYPE_TRAP						= 0x00100000;	//. 2006. 12. 81. Magoja , 높이가 지정돼는 고정형 몬스터.
const int	AGPMCHAR_TYPE_SUMMON					= 0x00200000;	//. 2008. 11. 25. iluvs. 나왔다가 바로 사라지는 환영 계열 캐릭터


typedef enum AgpdCharStatus {
	AGPDCHAR_STATUS_LOGOUT					= 0,	// 이넘은 접속도 안한넘이다.
	AGPDCHAR_STATUS_IN_LOGIN_PROCESS,				// 로그인 직후 게임에 들어오기 전에
	AGPDCHAR_STATUS_IN_GAME_WORLD,					// 게임에 들어와서 잼나게 하고있다.
	AGPDCHAR_STATUS_RETURN_TO_LOGIN_SERVER,
} AgpdCharStatus;

typedef enum AgpdCharActionStatus {
	AGPDCHAR_STATUS_NORMAL					= 0,	// 잘 살아서 돌아 댕긴다.
	AGPDCHAR_STATUS_PREDEAD,						// 일단은 죽었는데... 핑타임을 보고 결정되기 전까진 걍 냅둔다.
	AGPDCHAR_STATUS_DEAD,							// 완전히 죽었당

	AGPDCHAR_STATUS_NOT_ACTION,
	AGPDCHAR_STATUS_MOVE,
	AGPDCHAR_STATUS_ATTACK,
	AGPDCHAR_STATUS_READY_SKILL,			// 스킬을 쓰기위해 정해진 시간을 대기중이다.
	AGPDCHAR_STATUS_TRADE,					// 거래 중이다.
	AGPDCHAR_STATUS_PARTY,					// 파티를 맺고 있는 중이다. 아싸~~~!!!!
	//AGPDCHAR_STATUS_STUN					// 기절했다.
} AgpdCharActionStatus;

// 2004.08.25. steeple
// Special Status 는 겹칠 수 있으므로 Bit 연산이 가능하게 한다.
// 2007.05.09. steeple
// 64비트로 변경.
const UINT64 AGPDCHAR_SPECIAL_STATUS_NORMAL						= 0x0000000000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_STUN						= 0x0000000000000001;
const UINT64 AGPDCHAR_SPECIAL_STATUS_FREEZE						= 0x0000000000000002;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SLOW						= 0x0000000000000004;
const UINT64 AGPDCHAR_SPECIAL_STATUS_INVINCIBLE					= 0x0000000000000008;
const UINT64 AGPDCHAR_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE		= 0x0000000000000010;
const UINT64 AGPDCHAR_SPECIAL_STATUS_NOT_ADD_AGRO				= 0x0000000000000020;	// 선공만 안한다.
const UINT64 AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO					= 0x0000000000000040;	// 타켓으로 인식하지 못함.
const UINT64 AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT				= 0x0000000000000080;	// 스턴방어상태.
const UINT64 AGPDCHAR_SPECIAL_STATUS_PVP_INVINCIBLE				= 0x0000000000000100;
const UINT64 AGPDCHAR_SPECIAL_STATUS_TRANSPARENT				= 0x0000000000000200;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DONT_MOVE					= 0x0000000000000400;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SUMMONER					= 0x0000000000000800;
const UINT64 AGPDCHAR_SPECIAL_STATUS_TAME						= 0x0000000000001000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_HALT						= 0x0000000000002000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_WAS_TAMED					= 0x0000000000004000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_COUNSEL					= 0x0000000000008000;	// 상담중 2005.10.30. steeple
const UINT64 AGPDCHAR_SPECIAL_STATUS_EVENT_GIFTBOX				= 0x0000000000010000;	// 선물상자 이벤트
const UINT64 AGPDCHAR_SPECIAL_STATUS_ADMIN_SPAWNED_CHAR			= 0x0000000000020000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISABLE					= 0x0000000000040000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_USE						= 0x0000000000080000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_REPAIR						= 0x0000000000100000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT			= 0x0000000000200000;	// 2006.09.21. steeple
const UINT64 AGPDCHAR_SPECIAL_STATUS_HUMAN_CASTLE_OWNER			= 0x0000000000400000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_ORC_CASTLE_OWNER			= 0x0000000001000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_MOONELF_CASTLE_OWNER		= 0x0000000002000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_ARCHLORD_CASTLE_OWNER		= 0x0000000004000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DROP_RATE_100				= 0x0000000008000000;	// 아이템 100% drop
const UINT64 AGPDCHAR_SPECIAL_STATUS_RIDE_ARCADIA				= 0x0000000010000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE		= 0x0000000020000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE		= 0x0000000040000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISABLE_SKILL				= 0x0000000080000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISABLE_NORMAL_ATK			= 0x0000000100000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SLEEP						= 0x0000000200000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SLOW_INVINCIBLE			= 0x0000000400000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_GO							= 0x0000000800000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_LEVELLIMIT					= 0x0000001000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISARMAMENT				= 0x0000002000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DRAGONSCION_CASTLE_OWNER	= 0x0000004000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISABLE_CHATTING			= 0x0000008000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_HOLD						= 0x0000010000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_CONFUSION					= 0x0000020000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_FEAR						= 0x0000040000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_BERSERK					= 0x0000080000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SHRINK						= 0x0000100000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISEASE					= 0x0000200000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_NONE						= 0x8000000000000000;

// 2007.06.29. steeple
// 각 항목별 무적 확률.
struct InvincibleInfo
{
	INT32 lNormalATKProbability;
	INT32 lSkillATKProbability;
	INT32 lStunProbability;
	INT32 lSlowProbability;
	INT32 lAttrProbability;

	InvincibleInfo()
	{
		clear();
	}

	void clear()
	{
		lNormalATKProbability	= 0;
		lSkillATKProbability	= 0;
		lStunProbability		= 0;
		lSlowProbability		= 0;
		lAttrProbability		= 0;
	}
};

typedef enum _AgpdCharacterCriminalStatus {
	AGPDCHAR_CRIMINAL_STATUS_INNOCENT		= 0,
	AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED,
} AgpdCharacterCriminalStatus;

typedef enum AgpdCharacterActionType {
	AGPDCHAR_ACTION_TYPE_NONE				= 0,
	AGPDCHAR_ACTION_TYPE_ATTACK,
	AGPDCHAR_ACTION_TYPE_ATTACK_MISS,
	AGPDCHAR_ACTION_TYPE_SKILL,
	AGPDCHAR_ACTION_TYPE_PICKUP_ITEM,
	AGPDCHAR_ACTION_TYPE_EVENT_TELEPORT,
	AGPDCHAR_ACTION_TYPE_EVENT_NPC_TRADE,
	AGPDCHAR_ACTION_TYPE_EVENT_MASTERY_SPECIALIZE,
	AGPDCHAR_ACTION_TYPE_EVENT_BANK,
	AGPDCHAR_ACTION_TYPE_EVENT_ITEMCONVERT,
	AGPDCHAR_ACTION_TYPE_EVENT_GUILD,
	AGPDCHAR_ACTION_TYPE_EVENT_PRODUCT,
	AGPDCHAR_ACTION_TYPE_EVENT_NPC_DIALOG,
	AGPDCHAR_ACTION_TYPE_EVENT_SKILL_MASTER,
	AGPDCHAR_ACTION_TYPE_EVENT_REFINERY,
	AGPDCHAR_ACTION_TYPE_EVENT_QUEST,
	AGPDCHAR_ACTION_TYPE_MOVE,
	AGPDCHAR_ACTION_TYPE_PRODUCT_SKILL,
	AGPDCHAR_ACTION_TYPE_EVENT_CHARCUSTOMIZE,
	AGPDCHAR_ACTION_TYPE_EVENT_ITEMREPAIR,
	AGPDCHAR_ACTION_TYPE_EVENT_REMISSION,
	AGPDCHAR_ACTION_TYPE_EVENT_WANTEDCRIMINAL,
	AGPDCHAR_ACTION_TYPE_EVENT_SIEGE_WAR,
	AGPDCHAR_ACTION_TYPE_EVENT_TAX,
	AGPDCHAR_ACTION_TYPE_USE_SIEGEWAR_ATTACK_OBJECT,
	AGPDCHAR_ACTION_TYPE_CARVE_A_SEAL,
	AGPDCHAR_ACTION_TYPE_EVENT_GUILD_WAREHOUSE,
	AGPDCHAR_ACTION_TYPE_USE_EFFECT,
	AGPDCHAR_ACTION_TYPE_EVENT_ARCHLORD,
	AGPDCHAR_ACTION_TYPE_EVENT_GAMBLE,
	AGPDCHAR_ACTION_TYPE_EVENT_GACHA,
	AGPDCHAR_MAX_ACTION_TYPE
} AgpdCharacterActionType;

typedef enum AgpdCharacterActionResultType {
	AGPDCHAR_ACTION_RESULT_TYPE_NONE		= 0,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS,
	AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_ARROW,
	AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_BOLT,
	AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_MP,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_CRITICAL,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_EVADE,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_DODGE,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_BLOCK,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_REFLECT,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_ABSORB,
	AGPDCHAR_ACTION_RESULT_TYPE_COUNTER_ATTACK,
	AGPDCHAR_ACTION_RESULT_TYPE_DEATH_STRIKE,
	AGPDCHAR_ACTION_RESULT_TYPE_CAST_SKILL,
	AGPDCHAR_ACTION_RESULT_TYPE_REFLECT_DAMAGE,
	AGPDCHAR_ACTION_RESULT_TYPE_REDUCE_DAMAGE,
	AGPDCHAR_ACTION_RESULT_TYPE_CONVERT_DAMAGE,
	AGPDCHAR_ACTION_RESULT_TYPE_CHARGE,
} AgpdCharacterActionResultType;

typedef enum AgpdCharacterActionBlockType {
	AGPDCHAR_ACTION_BLOCK_TYPE_ALL			= 0x00000001,
	AGPDCHAR_ACTION_BLOCK_TYPE_ATTACK		= 0x00000002,
	AGPDCHAR_ACTION_BLOCK_TYPE_SKILL		= 0x00000004,
} AgpdCharacterActinoBlockType;

typedef enum AgpdCharacterAgroType
{
	AGPDCHAR_AGRO_TYPE_NORMAL = 0x00000000,
	AGPDCHAR_AGRO_TYPE_NOT_ADD = 0x00000001,
	AGPDCHAR_AGRO_TYPE_HIDE = 0x00000002,
} AgpdCharacterAgroType;

typedef enum _AgpdCharacterSocialType
{
	AGPDCHAR_SOCIAL_TYPE_GREETING	= 0,	// 인사
	AGPDCHAR_SOCIAL_TYPE_CELEBRATION,		// 축하
	AGPDCHAR_SOCIAL_TYPE_GRATITUDE,			// 감사
	AGPDCHAR_SOCIAL_TYPE_ENCOURAGEMENT,		// 격려
	AGPDCHAR_SOCIAL_TYPE_DISREGARD,			// 무시
	AGPDCHAR_SOCIAL_TYPE_DANCING,			// 춤추기
	AGPDCHAR_SOCIAL_TYPE_DOZINESS,			// 졸기
	AGPDCHAR_SOCIAL_TYPE_STRETCH,			// 기지개
	AGPDCHAR_SOCIAL_TYPE_LAUGH,				// 웃기
	AGPDCHAR_SOCIAL_TYPE_WEEPING,			// 울기
	AGPDCHAR_SOCIAL_TYPE_RAGE,				// 분노
	AGPDCHAR_SOCIAL_TYPE_POUT,				// 토라짐
	AGPDCHAR_SOCIAL_TYPE_APOLOGY,			// 사과
	AGPDCHAR_SOCIAL_TYPE_TOAST,				// 건배
	AGPDCHAR_SOCIAL_TYPE_CHEER,				// 환호
	AGPDCHAR_SOCIAL_TYPE_RUSH,				// 돌격
	AGPDCHAR_SOCIAL_TYPE_SIT,				// 앉기
	AGPDCHAR_SOCIAL_TYPE_SPECIAL1,			// 빤따스띡~

	AGPDCHAR_SOCIAL_TYPE_SELECT1,			// 선택(맨손)
	AGPDCHAR_SOCIAL_TYPE_SELECT2,			// 선택(한손검,활,STAFF)
	AGPDCHAR_SOCIAL_TYPE_SELECT3,			// 선택(한손둔기,석궁,!STAFF)
	AGPDCHAR_SOCIAL_TYPE_SELECT4,			// 선택(두손검)
	AGPDCHAR_SOCIAL_TYPE_SELECT5,			// 선택(두손둔기)
	AGPDCHAR_SOCIAL_TYPE_SELECT6,			// 선택(폴암류)
	AGPDCHAR_SOCIAL_TYPE_SELECT7,			// 선택(싸이쓰류, 카타리야, 챠크람 추가.)
	AGPDCHAR_SOCIAL_TYPE_SELECT8,			// 선택(한손제논)
	AGPDCHAR_SOCIAL_TYPE_SELECT9,			// 선택(두손제논)
	AGPDCHAR_SOCIAL_TYPE_SELECT10,			// 선택(두손카론)
	
	// 로긴 선택창에서... 다른 캐릭이 선택되면 제자리로 돌아가는 소셜에니 발동
	AGPDCHAR_SOCIAL_TYPE_SELECT1_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT2_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT3_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT4_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT5_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT6_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT7_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT8_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT9_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT10_BACK,

	AGPDCHAR_SOCIAL_TYPE_GM_GREETING	,	// 인사
	AGPDCHAR_SOCIAL_TYPE_GM_CELEBRATION	,	// 축하
	AGPDCHAR_SOCIAL_TYPE_GM_DANCING		,	// 춤
	AGPDCHAR_SOCIAL_TYPE_GM_WEEPING		,	// 울기
	AGPDCHAR_SOCIAL_TYPE_GM_TOAST		,	// 건배
	AGPDCHAR_SOCIAL_TYPE_GM_CHEER		,	// 환호
	AGPDCHAR_SOCIAL_TYPE_GM_DEEPBOW		,	// 절
	AGPDCHAR_SOCIAL_TYPE_GM_HI			,	// 안녕하세요
	AGPDCHAR_SOCIAL_TYPE_GM_WAIT		,	// 잠시만 기다려주세요
	AGPDCHAR_SOCIAL_TYPE_GM_HAPPY		,	// 아크로드와 함께 즐거운 하루 되세요

	AGPDCHAR_SOCIAL_TYPE_MAX
} AgpdCharacterSocialType;


typedef enum _AgpdCharacterOptionFlag			//	2005.05.31. By SungHoon	거부 기능 열거List
{
	AGPDCHAR_OPTION_REFUSE_TRADE			=	0x00000001,		// 거래 거부
	AGPDCHAR_OPTION_REFUSE_PARTY_IN			=	0x00000002,		// 파티 초대 거부
	AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE		=	0x00000004,		// 길드전 거부
	AGPDCHAR_OPTION_REFUSE_BATTLE			=	0x00000008,		// 결투 거부
	AGPDCHAR_OPTION_REFUSE_GUILD_IN			=	0x00000010,		// 길드초대 거부
	AGPDCHAR_OPTION_REFUSE_GUILD_RELATION	=	0x00000040,		// 연대-적대 거부
	AGPDCHAR_OPTION_REFUSE_BUDDY			=	0x00000080,		// 친구 초대 거부
	AGPDCHAR_OPTION_AUTO_PICKUP_ONOFF		=	0x00000100,		// 펜인벤토리를 통한 AutoPickUP On/OFF
} AgpdCharacterOptionFlag;

typedef enum _eAgpmCharacterTamableType			// 2005.10.04. steeple
{
	AGPDCHAR_TAMABLE_TYPE_NONE				= 0,
	AGPDCHAR_TAMABLE_TYPE_BY_FORMULA,
} AgpdCharacterTamableType;

typedef enum _eAgpmCharacterRangeType
{
	AGPDCHAR_RANGE_TYPE_MELEE = 0,
	AGPDCHAR_RANGE_TYPE_RANGE,
} AgpdCharacterRangeType;

// 2005.12.14. steeple
// Skill 쪽에서 처리하던 것을 이리로 옮겼다. 렌즈 스톤 때메 ㅜㅜ
typedef enum
{
	AGPDCHAR_ADDITIONAL_EFFECT_NONE						= 0x00000000,
	AGPDCHAR_ADDITIONAL_EFFECT_END_BUFF_EXPLOSION		= 0x00000001,
	AGPDCHAR_ADDITIONAL_EFFECT_TELEPORT					= 0x00000002,
	AGPDCHAR_ADDITIONAL_EFFECT_ABSORB_MP				= 0x00000004,	// 공격했을 때 데미지 흡수
	AGPDCHAR_ADDITIONAL_EFFECT_ABSORB_HP				= 0x00000008,	// 공격했을 때 데미지 흡수
	AGPDCHAR_ADDITIONAL_EFFECT_CONVERT_MP				= 0x00000010,	// 방어했을 때 데미지 흡수
	AGPDCHAR_ADDITIONAL_EFFECT_CONVERT_HP				= 0x00000020,	// 방어했을 때 데미지 흡수
	AGPDCHAR_ADDITIONAL_EFFECT_RELEASE_TARGET			= 0x00000040,	// 해당 캐릭터를 타겟한 PC 들에게서 타겟 선택을 릴리즈 시킨다.
	AGPDCHAR_ADDITIONAL_EFFECT_LENS_STONE				= 0x00000080,	// 렌즈 스톤 이펙트
	AGPDCHAR_ADDITIONAL_EFFECT_FIRECRACKER				= 0x00000100,	// 폭죽
	AGPDCHAR_ADDITIONAL_EFFECT_CHARISMA_POINT			= 0x00000200,	// 카리스마 포인트 획득
} AgpdCharacterAdditionalEffect;

enum eDisturbAction
{
	AGPDCHAR_DISTURB_ACTION_NONE						= 0x00,
	AGPDCHAR_DISTURB_ACTION_ATTACK						= 0x01,
	AGPDCHAR_DISTURB_ACTION_MOVE						= 0x02,
	AGPDCHAR_DISTURB_ACTION_USE_ITEM					= 0x04,
	AGPDCHAR_DISTURB_ACTION_SKILL						= 0x08
};

// bit flag options
//////////////////////////////////////////////////////////
union EventStatusFlag
{
	UINT16	BitFlag;
	struct  
	{
		UINT8 uBitFlag;
		UINT8 NationFlag;
	};
};

enum eEventFlagNationFlag
{
	AGPDCHAR_EVENTFLAG_NONE = 1,
	AGPDCHAR_EVENTFLAG_BLACKRIBBON,
	AGPDCHAR_EVENTFLAG_KOREA, 
};

const int	AGPM_CHAR_BIT_FLAG_IS_PC_ROOM			= 0x01;
const int	AGPM_CHAR_BIT_FLAG_EXPEDITION			= 0x02;

const int	AGPMCHAR_MAX_UNION				= 20;
const int	AGPMCHAR_MURDERER_LEVEL0_POINT	= 0;
const int	AGPMCHAR_MURDERER_LEVEL1_POINT	= 40;
const int	AGPMCHAR_MURDERER_LEVEL2_POINT	= 60;
const int	AGPMCHAR_MURDERER_LEVEL3_POINT	= 100;

const int	AGPMCHAR_MAX_LEVEL				= 121;

const int	AGPMCHAR_LEVELUP_SKILL_POINT	= 3;

const int	AGPMCHAR_DEFAULT_MOVE_SPEED		= 4000;

const int	AGPMCHAR_MAX_ATTACKER_LIST		= 20;

const int	AGPMCHAR_MAX_TOWN_NAME			= 64;

const int	AGPMCHAR_MAX_COMBAT_MODE_TIME	= 10000;					// 마지막으로 쌈박한 이후에 10초동안은 쌈박질 모드이다.

class   AgpdCharacterAgroDecreaseData
{
public:
	INT32			m_lSlowPercent;
	INT32			m_lSlowTime;
	INT32			m_lFastPercent;
	INT32			m_lFastTime;
};

class	AgpdCharacterTemplate : public ApBase
{
public:
	//CHAR					m_szTName[AGPACHARACTERT_MAX_TEMPLATE_NAME + 1];
	CHAR					m_szTName[AGPACHARACTER_MAX_ID_STRING + 1];

	INT16					m_nBlockInfo;
//	ApSafeArray<AuBLOCKING, AGPDCHARACTER_MAX_BLOCK_INFO>			m_astBlockInfo; 확정될때까지 잠시 주석(BOB, 181004)
	AuBLOCKING				m_astBlockInfo[AGPDCHARACTER_MAX_BLOCK_INFO];

	AgpdFactor				m_csFactor;				// 각종 Template Factor

	UINT32					m_ulCharType;

	//BOOL					m_bIsMonster;			// 이넘이 몬스터인지 여부 (TRUE : 몬스터, FALSE : 플레이어)

	ApSafeArray<AgpdFactor, AGPMCHAR_MAX_LEVEL + 1>					m_csLevelFactor;
	//AgpdFactor				m_csLevelFactor[AGPMCHAR_MAX_LEVEL + 1];	// 레벨 별 클래스 status들... 레벨이 올라가면 이넘으로 캐릭터 스탯을 덮어쓴다.

	//몬스터의 시간당 어그로 감소포인트. 몬스터마다 동일하므로 AI보다는 캐릭터쪽으로 붙였다. -Ashulam-
	AgpdCharacterAgroDecreaseData	m_csDecreaseData;

	//드랍관련 세팅.
	INT32					m_lDropTID;
	INT32					m_lGhelldMin;
	INT32					m_lGhelldMax;

	UINT32					m_ulIDColor;

	AgpdFactorAttributeType	m_eAttributeType;

	//customize
	INT32					m_lFaceNum;
	INT32					m_lHairNum;

	AgpdCharacterTamableType	m_eTamableType;		// 2005.10.04. steeple
	INT32						m_lLandAttachType;

	AgpdCharacterRangeType	m_eRangeType;		// 2007.12.05. steeple
	BOOL IsRangeCharacter() { return m_eRangeType == AGPDCHAR_RANGE_TYPE_RANGE; }

	//@{ 2006/08/21 burumal
	FLOAT					m_fSiegeWarCollBoxWidth		;
	FLOAT					m_fSiegeWarCollBoxHeight	;
	FLOAT					m_fSiegeWarCollSphereRadius	;
	FLOAT					m_fSiegeWarCollObjOffsetX	;	//중점 옵셋.
	FLOAT					m_fSiegeWarCollObjOffsetZ	;

	//@}

	//@{ 2006/09/08 burumal
	BOOL					m_bSelfDestructionAttackType;
	//@}

	INT32					m_lStaminaPoint;
	INT32					m_lPetType;
	INT32					m_lStartStaminaPoint;

	BOOL IsBlocking()		{ return ( m_fSiegeWarCollBoxWidth > 0.0f || m_fSiegeWarCollBoxHeight > 0.0f || m_fSiegeWarCollSphereRadius > 0.0f ) ? TRUE : FALSE; }
	BOOL IsBoxCollision()	{ return ( m_fSiegeWarCollBoxWidth > 0.0f || m_fSiegeWarCollBoxHeight > 0.0f ) ? TRUE : FALSE; }
	CHAR * GetName()		{ return m_szTName; }
};

class ApdCharacter : public ApBase
{
public:
	INT32					m_lTID1;
	AgpdCharacterTemplate *	m_pcsCharacterTemplate;
};

typedef struct
{
	AgpdCharacterActionType	m_eActionType;
	BOOL					m_bForceAction;			// 거리 체크를 할지 안할지 여부
	ApBaseExLock			m_csTargetBase;
	AuPOS					m_stTargetPos;

	ApSafeArray<INT_PTR, 5>	m_lUserData;
	//INT32					m_lUserData[5];

	UINT32					m_ulAdditionalEffect;
} AgpdCharacterAction;

typedef struct	// 2005.12.02. steeple
{
	INT32					m_lBonusExpRate;
	INT32					m_lBonusMoneyRate;
	INT32					m_lBonusDropRate;
	INT32					m_lBonusDropRate2;
	INT32					m_lBonusCharismaRate;

	void init()
	{
		m_lBonusExpRate = 0;
		m_lBonusMoneyRate = 0;
		m_lBonusDropRate = 0;
		m_lBonusDropRate2 = 0;
		m_lBonusCharismaRate = 0;
	}
} AgpdCharacterGameBonusByCash;

enum MOVE_DIRECTION
{
	MD_NODIRECTION			,	// 방향 지정이 없는 것을 말함.	
	MD_FORWARD				,
	MD_FORWARDRIGHT			,
	MD_RIGHT				,
	MD_BACKWARDRIGHT		,
	MD_BACKWARD				,
	MD_BACKWARDLEFT			,
	MD_LEFT					,
	MD_FORWARDLEFT			
};

// 중국 중독 방지 시스템용
enum ADDICT_STATUS
{
	AS_GREEN = 0,	// 0~3시간 동안 플레이한 상태
	AS_YELLOW,		// 3~5시간 동안
	AS_RED,			// 5시간 이상
};

class CCSAuth2;

class AgpdCharacter : public ApdCharacter
{
public:
	INT32						m_nDimension;			// 계 인덱스.
	//INT32						m_lTID2;
//	CHAR						m_szID[AGPACHARACTER_MAX_ID_STRING + 1];
	CHAR						m_szID[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	//CHAR						m_szAccountID[AGPACHARACTER_MAX_ID_STRING];
	BOOL						m_bMove;				// 움직이고 있는지
	BOOL						m_bMoveFollow;			// 타겟을 따라간다.
	BOOL						m_bMoveFast;			// 빨리 움직인다. 임시로 만드는데... 보통속도의 3배로 뛴다.
	MOVE_DIRECTION				m_eMoveDirection;		// Directional Move인지
	BOOL						m_bPathFinding;			// Path Finding Mode인지
	BOOL						m_bSync;				// Sync를 맞춰야 할때
	BOOL						m_bHorizontal;			// 수평 이동인지

	BOOL						m_bWasMoveFollow;		// 멈추기 전에 Follow 하고 있었는지 여부

	UINT32						m_ulCharType;

	INT32						m_lAdjustMoveSpeed;		// 이동 속도 조절값

	INT32						m_lFollowTargetID;		// 이 타겟을 따라 움직인다.
	INT32						m_lFollowDistance;		// 따라가는 거리 (타겟과의 거리가 이 거리 이하로 줄면 멈춘다.)

	UINT32						m_ulSyncMoveTime;		// 마지막으로 패킷을 보낸 시간

	//CHAR						m_szTitle[AGPACHARACTER_MAX_CHARACTER_TITLE + 1];			// 시부럴 탱탱구리 여따가 스킬초기화값을 넣어서 사람 졸리 짱나게 하네.
	CHAR						m_szSkillInit[AGPACHARACTER_MAX_CHARACTER_SKILLINIT + 1];	// DB컬럼이름이야 어쩔 수 없어도 이거라도 바꾸자.
	CHAR						m_szNickName[AGPACHARACTER_MAX_CHARACTER_NICKNAME + 1];	// 별칭.
	
	AuPOS						m_stPos;
	AuPOS						m_stDestinationPos;		// 이동할때 목적지 좌표
	AuPOS						m_stDirection;			// 이동할때 방향

	UINT32						m_ulStartMoveTime;		// Destination으로 이동하기 시작한 시간
	UINT32						m_ulEndMoveTime;		// 목적지 좌표까지 이동하는 시간

	FLOAT						m_fTurnX;				// 케릭터의 방향 (X축 각도)
	FLOAT						m_fTurnY;				// 케릭터의 방향 (Y축 각도)

	AuMATRIX					m_stMatrix;				// 케릭터의 방향성 Matrix (다른 Factor들은 적용 안됨)

	INT16						m_nBlockInfo;			// Blocking Info의 개수

	ApSafeArray<AuBLOCKING, AGPDCHARACTER_MAX_BLOCK_INFO>			m_astBlockInfo;
//	AuBLOCKING					m_astBlockInfo[AGPDCHARACTER_MAX_BLOCK_INFO];
														// Blocking Info
	ApSafeArray<AuBLOCKING, AGPDCHARACTER_MAX_BLOCK_INFO>			m_astWorldBlockInfo;
//	AuBLOCKING					m_astWorldBlockInfo[AGPDCHARACTER_MAX_BLOCK_INFO];
														// Calculated Blocking Info (world coordinates)

	AgpdFactor					m_csFactor;				// 각종 Factor를 여기서 관리

	// 내부적인 계산에만 사용된다. packet 으로 주고 받고 하지는 않는 넘들이다.
	AgpdFactor					m_csFactorPoint;		// 이 캐릭터의 factor들 중 (아템, 스킬 다 합쳐서) Point 들의 총합
	AgpdFactor					m_csFactorPercent;		// 이 캐릭터의 factor들 중 (아템, 스킬 다 합쳐서) percent 들의 총합

	AgpdTitle*					m_csTitle;
	AgpdTitleQuest*				m_csTitleQuest;

	BOOL						m_bIsTrasform;			// 현재 transform이 진행중인지 여부
	BOOL						m_bIsEvolution;			// 진화 여부
	INT32						m_lOriginalTID;			// for transform tid recovery
	AgpdFactor					m_csFactorOriginal;		// for transform status recovery
	INT32						m_lTransformedByCID;	// 누구한테 Transform 됬는 지. 자기 자신일 수 있다. 2005.10.24. steeple
	INT32						m_eTransformType;		// 어떻게 변신했는지 기록. 2006.08.09. steeple
	INT32						m_lFixTID;				// 처음설정된 TID를 보존.

	INT32						m_lTransformItemTID;	// Item 을 사용해서 transform 한 거라면 여기다가 TID 저장. 2008.03.11. steeple
	BOOL						m_bCopyTransformFactor;	// Copy 를 한건지 Add 를 한건지 저장. 2008.03.11. steeple

	UINT16						m_unCurrentStatus;		// 이 캐릭터의 현재 상태

	INT8						m_unActionStatus;
									// AGDMCHAR_STATUS_NORMAL : 살아있다 (일반 상태)
									// AGDMCHAR_STATUS_PREDEAD : 죽고 처리를 기다린다. (완전히 죽은건 아니다)
									// AGDMCHAR_STATUS_DEAD : 죽었다
									// AGPDCHAR_STATUS_READY_SKILL : 스킬을 쓰기위해 정해진 딜레이 시간동안 기다리고 있다.
									// AGPDCHAR_STATUS_DURATION_SKILL : 스킬을 써서 사용중이다.

									// AGPDCHAR_STATUS_TRADE : 거래를 시작해서 진행 상태다.

	INT8						m_unCriminalStatus;		// 이 캐릭터의 범죄상태 여부를 나타낸다.

	UINT32						m_ulPrevProcessTime;	// 이전에 처리한 시간 (OnIdle에서)

	BOOL						m_bIsAddMap;			// 맵에 추가가 되었는지 여부 (TRUE : 추가)

	INT64						m_llMoney;				//돈~
	INT64						m_llBankMoney;			// 뱅크에 있는 던~
	INT64						m_llCash;				// 현금

	INT8						m_cBankSize;

	INT32						m_alTargetCID[AGPDCHARACTER_MAX_TARGET];

	AgpdCharacterAction			m_stNextAction;			// 다음 Action (움직임이 끝난 후에 이루어질 Action)

	//ApSafeArray<INT32, AGPMCHAR_MAX_ATTACKER_LIST>	m_lAttackerList;	// 날 때린 PC 들 아뒤 리스트

	UINT32						m_ulRemoveTimeMSec;		// 이 캐릭터가 삭제된 클럭카운트

	//CHAR						m_szBindingAreaName[AGPMCHAR_MAX_TOWN_NAME + 1];

	// 마고자 (2004-09-17 오후 5:17:28) : 바인딩 리전 인덱스
	// 현재 소속된 리전 인덱스 를 저장함.
	INT16						m_nBindingRegionIndex;
	INT16						m_nLastExistBindingIndex;
	INT16						m_nBeforeRegionIndex;
	UINT32						m_ulNextRegionRefreshTime;

	BOOL						m_bIsCombatMode;		// 전투모드 == TRUE
	UINT32						m_ulSetCombatModeTime;	// 전투모드 세팅 시간

	UINT32						m_ulCheckBlocking;	// 0x01 : 지형블러킹 0x02 : 오브젝트 블러킹
													// 체크할것만 넣기 .
													// 자세한건 Apmmap ApTileInfo에 있음..

	BOOL						m_bIsActionBlock;
	UINT32						m_ulActionBlockTime;
	AgpdCharacterActionBlockType	m_eActionBlockType;

	UINT64						m_ulSpecialStatus;		// 특수 상태를 Bit Mask 로 저장한다.

	BOOL						m_bIsPostedIdleProcess;

	BOOL						m_bIsActionMoveLock;
	AgpdCharacterAction			m_stLockAction;

	INT32						m_lFaceIndex;
	INT32						m_lHairIndex;

	INT32						m_lOptionFlag;		//	2005.05.31. By SungHoon 거부 Flag
	BOOL						m_bRidable;			// 탈것 아이템을 장착하고 장착 가능한 지역에 있을때만 TRUE
	INT32						m_nRideCID;			// 탈것의 아이디.

	UINT16						m_unEventStatusFlag;

	// 	
	INT32						m_lLastUpdateCriminalFlagTime;		// criminal flag가 세팅된 마지막 시간
	INT32						m_lLastUpdateMurdererPointTime;	// 살인자 포인트가 오른 마지막 시간
	UINT32						m_ulBattleSquareKilledTime;	// 배틀 스퀘어에서 마지막으로 죽은 시간.

	INT32						m_lCountMobMoli;	// 몰이하는 몬스터 수 2005.11.29. steeple
	
	BOOL						m_bIsWantedCriminal;

	AgpdCharacterGameBonusByCash m_stGameBonusByCash;

	BOOL						m_bIsActiveData;
	BOOL						m_bIsReadyRemove;

	BOOL						m_bIsProtectedNPC;
	INT32						m_nNPCID;

	BOOL						m_bNPCDisplayForMap;
	BOOL						m_bNPCDisplayForNameBoard;

	BOOL IsDead() const { return ( m_unActionStatus == AGPDCHAR_STATUS_DEAD ) ? TRUE : FALSE; }

	// 중독 방지 - 2007.07.06. steeple
	UINT32						m_ulOnlineTime;
	INT32						m_lAddictStatus;

	UINT8						m_unDisturbAction;

	INT32						m_ulSpecialStatusReserved;		// SpecialStatus에서 상태이상등으로 필요한 자료 보관용 (ex, 무장해제에서 해제된 파츠)

	BOOL	GetSpecialState( UINT64 ulStatus )	{	return m_ulSpecialStatus & ulStatus ? TRUE : FALSE; 	}
};


class AgpdCharisma
	{
	public :
		INT32					m_lPoint;
		CHAR					m_szTitle[AGPACHARACTER_MAX_CHARACTER_NICKNAME + 1];
	
	public :
		AgpdCharisma()
			{
			m_lPoint = 0;
			ZeroMemory(m_szTitle, sizeof(m_szTitle));
			}
	};

#endif //_AGPDCHARACTER_H_