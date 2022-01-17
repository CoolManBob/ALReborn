#ifndef __AGCM_EVENT_EFFECT_DATA_H__
#define __AGCM_EVENT_EFFECT_DATA_H__

#include "AuExcelTxtLib.h"
#include "AgpmItem.h"
#include "AgcaEventEffectSound.h"
#include "AuRandomNumber.h"
#include "AgcdEffectData.h"
#include "AgcaEffectData.h"
#include "AgcdEventEffect.h"

#include <vector>

#define AGCM_EVENT_EFFECT_SOUND_NAME_HIT1					"Hit_1"
#define AGCM_EVENT_EFFECT_SOUND_NAME_HIT2					"Hit_2"
#define AGCM_EVENT_EFFECT_SOUND_NAME_HIT3					"Hit_3"
#define AGCM_EVENT_EFFECT_SOUND_NAME_HIT4					"Hit_4"
#define AGCM_EVENT_EFFECT_SOUND_NAME_FLOURISH				"Flourish"

#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SOIL					"SOIL"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SWAMP					"SWAMP"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_GRASS					"GRASS"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SAND					"SAND"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_LEAF					"LEAF"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SNOW					"SNOW"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_WATER					"WATER"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_STONE					"STONE"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_WOOD					"WOOD"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_METAL					"METAL"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_BONE					"BONE"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_MUD					"MUD"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SOILGRASS				"SOILGRASS"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SOLIDSOIL				"SOILSOLID"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SPORE					"SPORE"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_MOSS					"MOSS"
#define AGCM_EVENT_EFFECT_SOUND_NAME_TILE_GRANITE				"GRANITE"

#define AGCM_EVENT_EFFECT_SOUND_NAME_FOOT_LEFT				"LEFT"
#define AGCM_EVENT_EFFECT_SOUND_NAME_FOOT_RIGHT				"RIGHT"

#define AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_W1					"W1"
#define AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_W2					"W2"
#define AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_W3					"W3"
#define AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_W4					"W4"
#define AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_W5					"W5"
#define AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_A1					"A1"
#define AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_A2					"A2"
#define AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_A3					"A3"
#define AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_A4					"A4"
#define AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_A5					"A5"

#define AGCM_EVENT_EFFECT_SPIRIT_STONE_FIRE_NAME					"FIRE"
#define AGCM_EVENT_EFFECT_SPIRIT_STONE_AIR_NAME						"AIR"
#define AGCM_EVENT_EFFECT_SPIRIT_STONE_WATER_NAME					"WATER"
#define AGCM_EVENT_EFFECT_SPIRIT_STONE_MAGIC_NAME					"MAGIC"
#define AGCM_EVENT_EFFECT_SPIRIT_STONE_EARTH_NAME					"EARTH"
#define AGCM_EVENT_EFFECT_SPIRIT_STONE_ICE_NAME						"ICE"
#define AGCM_EVENT_EFFECT_SPIRIT_STONE_LIGHTENING_NAME				"LIGHTENING"
#define AGCM_EVENT_EFFECT_SPIRIT_STONE_POISON_NAME					"POISON"
#define AGCM_EVENT_EFFECT_SPIRIT_STONE_PHYSICAL_NAME				"PHYSICAL"
#define AGCM_EVENT_EFFECT_SPIRIT_STONE_RUNE_NAME					"RUNE"

#define AGCM_EVENT_EFFECT_DATA_NAME_ID								"ID"
#define AGCM_EVENT_EFFECT_DATA_NAME_EID								"EID"
#define AGCM_EVENT_EFFECT_DATA_NAME_X								"OFFSET_X"
#define AGCM_EVENT_EFFECT_DATA_NAME_Y								"OFFSET_Y"
#define AGCM_EVENT_EFFECT_DATA_NAME_Z								"OFFSET_Z"
#define AGCM_EVENT_EFFECT_DATA_NAME_SCALE							"SCALE"
#define AGCM_EVENT_EFFECT_DATA_NAME_NODEID							"NODE_ID"
#define AGCM_EVENT_EFFECT_DATA_NAME_GAP								"GAP"
#define AGCM_EVENT_EFFECT_DATA_NAME_RGBA_SCALE						"RGBA"
#define AGCM_EVENT_EFFECT_DATA_NAME_PARTICLE_SCALE					"PARTICLE"
#define AGCM_EVENT_EFFECT_DATA_NAME_EMITTER_ATOMIC					"ATOMIC_EMITTER"
#define AGCM_EVENT_EFFECT_DATA_NAME_EMITTER_CLUMP					"CLUMP_EMITTER"
#define AGCM_EVENT_EFFECT_DATA_NAME_EFFECT_SUB						"EFFECT"

#define AGCM_EVENT_EFFECT_DATA_MAX_SUB_EFFECT						5

#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_GREETING					"GREETING"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_CELEBRATION				"CELEBRATION"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_GRATITUDE				"GRATITUDE"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_ENCOURAGEMENT			"ENCOURAGEMENT"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_DISREGARD				"DISREGARD"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_DANCING					"DANCING"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_DOZINESS					"DOZINESS"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_STRETCH					"STRETCH"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_LAUGH					"GIGGLER"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_WEEPING					"WEEPING"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_RAGE						"RAGE"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_POUT						"POUT"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_APOLOGY					"APOLOGY"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_TOAST					"TOAST"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_CHEER					"CHEER"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_RUSH						"RUSH"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SIT						"SIT"
#define AGCM_EVNET_EFFECT_SOCIAL_TYPE_NAME_OKMARIO					"OKMARIO"
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT1					"SELECT1"	// 맨손
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT2					"SELECT2"	// 한손검, 활, 스탭
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT3					"SELECT3"	// 한손둔기, 석궁, !스탭
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT4					"SELECT4"	// 두손검
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT5					"SELECT5"	// 두손둔기
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT6					"SELECT6"	// 폴암
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT7					"SELECT7"	// 싸이쓰, 카타리야, 챠크람
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT8					"SELECT8"	// 한손제논
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT9					"SELECT9"	// 두손제논
#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT10					"SELECT10"	// 두손카론

#define AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK				"_BACK"

#define AGCM_EVENT_EFFECT_RACE_NAME_HUMAN							"HUMAN"
#define AGCM_EVENT_EFFECT_RACE_NAME_ORC								"ORC"
#define AGCM_EVENT_EFFECT_RACE_NAME_MOONELF							"MOONELF"
#define AGCM_EVENT_EFFECT_RACE_NAME_DRAGONSCION						"DRAGONSCION"

#define AGCM_EVENT_EFFECT_CLASS_NAME_KNIGHT							"KNIGHT"
#define AGCM_EVENT_EFFECT_CLASS_NAME_RANGER							"RANGER"
#define AGCM_EVENT_EFFECT_CLASS_NAME_MONK							"MONK"
#define AGCM_EVENT_EFFECT_CLASS_NAME_MAGE							"MAGE"

#define AGCM_EVENT_EFFECT_GENDER_NAME_MALE							"MALE"
#define AGCM_EVENT_EFFECT_GENDER_NAME_FEMALE						"FEMALE"

#define AGCM_EVENT_EFFECT_FX_NAME_TYPE								"TYPE"
#define AGCM_EVENT_EFFECT_FX_NAME_GRADE								"GRADE"
#define AGCM_EVENT_EFFECT_FX_NAME_FX								"FX"
#define AGCM_EVENT_EFFECT_FX_NAME_TEXTURE							"TEXTURE"
#define AGCM_EVENT_EFFECT_FX_NAME_AMBIENT							"AMBIENT"
#define AGCM_EVENT_EFFECT_FX_NAME_DIFFUSE							"DIFFUSE"
#define AGCM_EVENT_EFFECT_FX_NAME_SPECULAR							"SPECULAR"
#define AGCM_EVENT_EFFECT_FX_NAME_UVTRANSLATION						"Uv Translation"
#define AGCM_EVENT_EFFECT_FX_NAME_UVSCALE							"UV Scale"
#define AGCM_EVENT_EFFECT_FX_NAME_UVROTATION						"UV Rotation"
#define AGCM_EVENT_EFFECT_FX_NAME_UVANIMWEIGHT						"UV Anim weight"
#define AGCM_EVENT_EFFECT_FX_NAME_USELECTIONVECTOR					"U Selection vector"
#define AGCM_EVENT_EFFECT_FX_NAME_VSELECTIONVECTOR					"V Selection vector"

#define AGCM_EVENT_EFFECT_DATA_SOUND_HIT_SOUND_NUM					4
#define AGCM_EVENT_EFFECT_DATA_SOUND_FOOT_INDEX_NUM					2
#define AGCM_EVENT_EFFECT_DATA_CONVERTED_ITEM_GRADE_NUM				5
#define AGCM_EVENT_EFFECT_DATA_CONVERTED_ITEM_ATTR_NUM				8
#define AGCM_EVENT_EFFECT_DATA_MAX_SS_LEVEL							5
#define AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA						10
#define AGCM_EVENT_EFFECT_DATA_MAX_EFFECT							10000
#define AGCM_EVENT_EFFECT_DATA_MAX_RT_ANIMATION						300
#define AGCM_EVENT_EFFECT_DATA_MAX_GRADE_CONVERTED_ARMOUR			5
#define AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL							4

typedef PVOID (*AgcmEventEffectDataParamFunction)(PVOID pvClass, PVOID pvData1, PVOID pvData2);

enum eAgcmEventEffectDataCommonCharType
{
	E_COMMON_CHAR_TYPE_SPAWN = 0,
	E_COMMON_CHAR_TYPE_LV_UP,
	E_COMMON_CHAR_TYPE_LV_UP2,
	E_COMMON_CHAR_TYPE_LEARN_SKILL,
	E_COMMON_CHAR_TYPE_TRANSFORM,
	E_COMMON_CHAR_TYPE_QUEST_NEW,
	E_COMMON_CHAR_TYPE_QUEST_COMPLETE,
	E_COMMON_CHAR_TYPE_QUEST_INCOMPLETE,
	E_COMMON_CHAR_TYPE_QUEST_DONE,
	E_COMMON_CHAR_TYPE_EVENT,
	E_COMMON_CHAR_TYPE_STUN,
	E_COMMON_CHAR_TYPE_SLOW,
	E_COMMON_CHAR_TYPE_POISON,
	E_COMMON_CHAR_TYPE_TIMEATTACK,
	E_COMMON_CHAR_TYPE_TELEPORT,
	E_COMMON_CHAR_TYPE_FLAMEFORCE,
	E_COMMON_CHAR_TYPE_FIRESUMMON,
	E_COMMON_CHAR_TYPE_ABSORBMANA,
	E_COMMON_CHAR_TYPE_ABSORBLIFE,
	E_COMMON_CHAR_TYPE_FROSTAMOR,
	E_COMMON_CHAR_TYPE_EARTHMOB,
	E_COMMON_CHAR_TYPE_FIREMOB,
	E_COMMON_CHAR_TYPE_WATERMOB,
	E_COMMON_CHAR_TYPE_AIRMOB,
	E_COMMON_CHAR_TYPE_MAGICMOB,
	E_COMMON_CHAR_TYPE_LIGHTENINGMOB,
	E_COMMON_CHAR_TYPE_POISONMOB,
	E_COMMON_CHAR_TYPE_ICEMOB,
	E_COMMON_CHAR_TYPE_MENTALCIRCUIT,
	E_COMMON_CHAR_TYPE_VITALCHARGE,
	E_COMMON_CHAR_TYPE_PULLINGMANNA,
	E_COMMON_CHAR_TYPE_CONSULT,
	E_COMMON_CHAR_TYPE_LENS_STONE,
	E_COMMON_CHAR_TYPE_RESURRECTION_ORB,
	E_COMMON_CHAR_TYPE_GIFT_BOX,
	E_COMMON_CHAR_TYPE_WINGS_OF_KING,
	E_COMMON_CHAR_TYPE_WINGS_OF_KING_O_S,
	E_COMMON_CHAR_TYPE_WINGS_OF_KING_O_W,
	E_COMMON_CHAR_TYPE_FIRECRACKER,
	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL,
	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_O_S,
	E_COMMON_CHAR_TYPE_WINGS_OF_ANGEL_O_W,
	E_COMMON_CHAR_TYPE_DETECT_SCROLL,
	E_COMMON_CHAR_TYPE_GIFT_FIRE,
	E_COMMON_CHAR_TYPE_WINGS_OF_BAT,
	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_O_S,
	E_COMMON_CHAR_TYPE_WINGS_OF_BAT_O_W,
	E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL,
	E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL_O_S,
	E_COMMON_CHAR_TYPE_WINGS_OF_DEVIL_O_W,
	E_COMMON_CHAR_TYPE_FIRECRACKER_S_L,
	E_COMMON_CHAR_TYPE_FIRECRACKER_S_M,
	E_COMMON_CHAR_TYPE_FIRECRACKER_S_H,
	E_COMMON_CHAR_TYPE_HEART_EMOTICON,
	E_COMMON_CHAR_TYPE_SHERBET,
	E_COMMON_CHAR_TYPE_VICTIM_IN,
	E_COMMON_CHAR_TYPE_VICTIM_DI,
	E_COMMON_CHAR_TYPE_SONGPYEN,
	E_COMMON_CHAR_TYPE_FIRECRACKER_S_L_01,
	E_COMMON_CHAR_TYPE_FIRECRACKER_S_M_01,
	E_COMMON_CHAR_TYPE_FIRECRACKER_S_H_01,
	E_COMMON_CHAR_TYPE_FIRECRACKER_S_H_02,
	E_COMMON_CHAR_TYPE_FIRECRACKER_S_H_03,
	E_COMMON_CHAR_TYPE_FIRECRACKER_S_H_04,
	E_COMMON_CHAR_TYPE_FIRECRACKER_S_H_05,
	E_COMMON_CHAR_TYPE_WINGS_OF_15,
	E_COMMON_CHAR_TYPE_WINGS_OF_15_O_S,
	E_COMMON_CHAR_TYPE_WINGS_OF_15_O_W,
	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE,
	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_O_S,
	E_COMMON_CHAR_TYPE_WINGS_OF_JUDGE_O_W,
	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH,
	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_O_S,
	E_COMMON_CHAR_TYPE_WINGS_OF_MESSIAH_O_W,
	E_COMMON_CHAR_TYPE_BUK_PORKET,
	E_COMMON_CHAR_TYPE_ARCHLORD_3RD,
	E_COMMON_CHAR_TYPE_GOMARK,
	E_COMMON_CHAR_TYPE_CURE,
	E_COMMON_CHAR_TYPE_MOON,
	E_COMMON_CHAR_TYPE_AIR_CLOAK_HK,	
	E_COMMON_CHAR_TYPE_AIR_CLOAK_HA,
	E_COMMON_CHAR_TYPE_AIR_CLOAK_HM,
	E_COMMON_CHAR_TYPE_AIR_CLOAK_OB,
	E_COMMON_CHAR_TYPE_AIR_CLOAK_OH,
	E_COMMON_CHAR_TYPE_AIR_CLOAK_OS,
	E_COMMON_CHAR_TYPE_AIR_CLOAK_MS,
	E_COMMON_CHAR_TYPE_AIR_CLOAK_MR,
	E_COMMON_CHAR_TYPE_AIR_CLOAK_ME,
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_HK,	
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_HA,	
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_HM,	
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_OB,	
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_OH,	
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_OS,	
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_MS,	
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_MR,	
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_ME,	
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_HK,	
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_HA,	
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_HM,	
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_OB,	
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_OH,	
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_OS,	
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_MS,	
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_MR,	
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_ME,	
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_HK,	
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_HA,	
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_HM,	
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_OB,	
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_OH,	
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_OS,	
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_MS,	
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_MR,	
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_ME,	
	E_COMMON_CHAR_TYPE_WATER_CLOAK_HK,	
	E_COMMON_CHAR_TYPE_WATER_CLOAK_HA,	
	E_COMMON_CHAR_TYPE_WATER_CLOAK_HM,	
	E_COMMON_CHAR_TYPE_WATER_CLOAK_OB,	
	E_COMMON_CHAR_TYPE_WATER_CLOAK_OH,	
	E_COMMON_CHAR_TYPE_WATER_CLOAK_OS,	
	E_COMMON_CHAR_TYPE_WATER_CLOAK_MS,	
	E_COMMON_CHAR_TYPE_WATER_CLOAK_MR,	
	E_COMMON_CHAR_TYPE_WATER_CLOAK_ME,	
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_HK,	
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_HA,	
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_HM,	
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_OB,	
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_OH,	
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_OS,	
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_MS,	
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_MR,	
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_ME,	
	E_COMMON_CHAR_TYPE_POISON_CLOAK_HK,	
	E_COMMON_CHAR_TYPE_POISON_CLOAK_HA,	
	E_COMMON_CHAR_TYPE_POISON_CLOAK_HM,	
	E_COMMON_CHAR_TYPE_POISON_CLOAK_OB,	
	E_COMMON_CHAR_TYPE_POISON_CLOAK_OH,	
	E_COMMON_CHAR_TYPE_POISON_CLOAK_OS,	
	E_COMMON_CHAR_TYPE_POISON_CLOAK_MS,	
	E_COMMON_CHAR_TYPE_POISON_CLOAK_MR,	
	E_COMMON_CHAR_TYPE_POISON_CLOAK_ME,	
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_HK,	
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_HA,	
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_HM,	
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_OB,	
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_OH,	
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_OS,	
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_MS,	
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_MR,	
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_ME,
	E_COMMON_CHAR_TYPE_HALLOWEEN,
	E_COMMON_CHAR_TYPE_AIR_CLOAK_SLA,	
	E_COMMON_CHAR_TYPE_AIR_CLOAK_SCI,
	E_COMMON_CHAR_TYPE_AIR_CLOAK_OBI,
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_SLA,	
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_SCI,
	E_COMMON_CHAR_TYPE_FIRE_CLOAK_OBI,
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_SLA,	
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_SCI,
	E_COMMON_CHAR_TYPE_MAGIC_CLOAK_OBI,
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_SLA,	
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_SCI,
	E_COMMON_CHAR_TYPE_GROUND_CLOAK_OBI,
	E_COMMON_CHAR_TYPE_WATER_CLOAK_SLA,	
	E_COMMON_CHAR_TYPE_WATER_CLOAK_SCI,
	E_COMMON_CHAR_TYPE_WATER_CLOAK_OBI,
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_SLA,	
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_SCI,
	E_COMMON_CHAR_TYPE_SOUL_CLOAK_OBI,
	E_COMMON_CHAR_TYPE_POISON_CLOAK_SLA,	
	E_COMMON_CHAR_TYPE_POISON_CLOAK_SCI,
	E_COMMON_CHAR_TYPE_POISON_CLOAK_OBI,
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_SLA,	
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_SCI,
	E_COMMON_CHAR_TYPE_SOUL2_CLOAK_OBI,
	E_COMMON_CHAR_TYPE_WING_KING_SLA,	
	E_COMMON_CHAR_TYPE_WING_KING_SCI,
	E_COMMON_CHAR_TYPE_WING_KING_OBI,
	E_COMMON_CHAR_TYPE_WING_ANGEL_SLA,	
	E_COMMON_CHAR_TYPE_WING_ANGEL_SCI,
	E_COMMON_CHAR_TYPE_WING_ANGEL_OBI,
	E_COMMON_CHAR_TYPE_WING_BAT_SLA,	
	E_COMMON_CHAR_TYPE_WING_BAT_SCI,
	E_COMMON_CHAR_TYPE_WING_BAT_OBI,
	E_COMMON_CHAR_TYPE_WING_DEVIL_SLA,	
	E_COMMON_CHAR_TYPE_WING_DEVIL_SCI,
	E_COMMON_CHAR_TYPE_WING_DEVIL_OBI,
	E_COMMON_CHAR_TYPE_WING_15_SLA,	
	E_COMMON_CHAR_TYPE_WING_15_SCI,
	E_COMMON_CHAR_TYPE_WING_15_OBI,
	E_COMMON_CHAR_TYPE_WING_JUDGE_SLA,	
	E_COMMON_CHAR_TYPE_WING_JUDGE_SCI,
	E_COMMON_CHAR_TYPE_WING_JUDGE_OBI,
	E_COMMON_CHAR_TYPE_WING_MESSIAH_SLA,	
	E_COMMON_CHAR_TYPE_WING_MESSIAH_SCI,
	E_COMMON_CHAR_TYPE_WING_MESSIAH_OBI,

	E_COMMON_CHAR_MAX_TYPE
};

enum eAgcmEventEffectDataSpiritStoneType
{
	E_SPIRIT_STONE_TYPE_NONE = -1,
	E_SPIRIT_STONE_TYPE_FIRE = 0,
	E_SPIRIT_STONE_TYPE_AIR,
	E_SPIRIT_STONE_TYPE_WATER,
	E_SPIRIT_STONE_TYPE_MAGIC,
	E_SPIRIT_STONE_TYPE_EARTH,
	E_SPIRIT_STONE_TYPE_ICE,
	E_SPIRIT_STONE_TYPE_LIGHTENING,
	E_SPIRIT_STONE_TYPE_POISON,
	E_SPIRIT_STONE_TYPE_PHYSICAL,
	E_SPIRIT_STONE_TYPE_RUNE,
	E_SPIRIT_STONE_TYPE_NUM
};

enum eAgcmEventEffectFXDataType
{
	E_FX_DATA_TYPE_FX	= 0,
	E_FX_DATA_TYPE_FLOAT,
	E_FX_DATA_TYPE_VECTOR,
	E_FX_DATA_TYPE_TEXTURE,
	E_FX_MAX_DATA_TYPE
};

class AgcdConvertedArmourFX
{
public:
	AgcdConvertedArmourFX()
	{
		memset( this, 0, sizeof( AgcdConvertedArmourFX ) );
	}

	INT32		m_lPoint;
	RpMaterial*	m_pacsMaterial[AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL];
	RwTexture*	m_pcsAnimTexture;
};

namespace NS_SOCIAL
{
	namespace NS_PRIVATE
	{
		class CSocial
		{
			enum {	eSoundNameLen = 32,	};

		public:
			CSocial() : pAnim_(NULL)	{						}
			~CSocial()					{	ASSERT( !pAnim_);	}	// AgcaAnimation2 요넘이 지워주는군..

			AgcdAnimation2*		bGetAnim(void)							{	return pAnim_;	}
			AgcdAnimation2**	bGetPPtrAnim(void)						{	return &pAnim_;	}
			RwChar*				bGetSoundName(void)						{	return szSound_.begin();	}
			void				bSetSoundName(const RwChar* szSound)	{	strncpy( szSound_.begin(), szSound, eSoundNameLen-1 );	}
			VOID bRemoveAllAnim(AgcaAnimation2& agcaAnimation2)	{	agcaAnimation2.RemoveAllAnimation( &pAnim_ );	}

		private:
			CSocial(const CSocial& cpy)					{}
			CSocial& operator = (const CSocial& cpy)	{ return *this; }

		private:
			AgcdAnimation2*						pAnim_;
			ApSafeArray<RwChar, eSoundNameLen>	szSound_;
		};
	};

	template<RwInt32 num = 0>
	class CSocialAnimAndSound
	{
	public:
		CSocialAnimAndSound(RwInt32 tid, AgpdCharacterSocialType baseSocialType) : TID_(tid), baseSocialType_(baseSocialType){};

		RwInt32			bGetTID(void)const									{	return TID_;	}
		AgcdAnimation2*	bGetAnim(AgpdCharacterSocialType socialType)		{	return arrSocial_[socialType-baseSocialType_].bGetAnim();	}
		const RwChar*	bGetSoundName(AgpdCharacterSocialType socialType)	{	return arrSocial_[socialType-baseSocialType_].bGetSoundName();	}
		void			bSetSoundName(AgpdCharacterSocialType socialType, const RwChar* szSound)	{
			arrSocial_[socialType-baseSocialType_].bSetSoundName(szSound);
		}

		RwInt32 bSetData( AgpdCharacterSocialType socialType, RwChar* szAnim, const RwChar* szSound, AgcaAnimation2& agcaAnimation2 )			
		{
			ASSERT( !bGetAnim(socialType) && !(*bGetSoundName(socialType)) );

			RwInt32 ir = 0;
			if( !bGetAnim(socialType) && NULL == *bGetSoundName(socialType) )	
			{
				if( szAnim )
					if (!agcaAnimation2.AddAnimation( arrSocial_[socialType-baseSocialType_].bGetPPtrAnim(), szAnim))
					{
						ir = -1;
						ASSERT( !"m_csAnimation2.AddAnimation() failed!" );
					}

				if( szSound )
				{
					bSetSoundName( socialType, szSound );
					ir = -1;
				}
			}
			else 
				ir = -1;

			return ir;
		}

		VOID bRemoveAllAnim(AgcaAnimation2& agcaAnimation2)
		{
			for( ApSafeArray<NS_PRIVATE::CSocial, num>::iterator Itr = arrSocial_.begin(); Itr != arrSocial_.end(); ++Itr )
				(*Itr).bRemoveAllAnim(agcaAnimation2);
		}

	private:
		const RwInt32							TID_;
		const AgpdCharacterSocialType			baseSocialType_;
		ApSafeArray<NS_PRIVATE::CSocial, num>	arrSocial_;
	};

	class AgcdSocialGeneral
	{
	public:
		enum { num = AGPDCHAR_SOCIAL_TYPE_SPECIAL1 - AGPDCHAR_SOCIAL_TYPE_GREETING + 1, };

		AgcdSocialGeneral(RwInt32 tid) : social_(tid, AGPDCHAR_SOCIAL_TYPE_GREETING)	{		}
		CSocialAnimAndSound<num>&	bGetSocial()										{	return social_;								}
		AgcdAnimation2*				bGetAnim(AgpdCharacterSocialType socialType)		{	return social_.bGetAnim(socialType);		}
		const RwChar*				bGetSoundName(AgpdCharacterSocialType socialType)	{	return social_.bGetSoundName(socialType);	}

	private:
		CSocialAnimAndSound<num>		social_;

	};

	class AgcdSocialLoginMode
	{
	public:
		enum { num = AGPDCHAR_SOCIAL_TYPE_SELECT10_BACK - AGPDCHAR_SOCIAL_TYPE_SELECT1 + 1, };

		AgcdSocialLoginMode(RwInt32 tid) : social_(tid,AGPDCHAR_SOCIAL_TYPE_SELECT1){};
		CSocialAnimAndSound<num>&	bGetSocial()										{	return social_;								}
		AgcdAnimation2*				bGetAnim(AgpdCharacterSocialType socialType)		{	return social_.bGetAnim(socialType);		}
		const RwChar*				bGetSoundName(AgpdCharacterSocialType socialType)	{	return social_.bGetSoundName(socialType);	}

	private:
		CSocialAnimAndSound<num>		social_;
	};

	class AgcdSocialForGM
	{
	public:
		enum { num = AGPDCHAR_SOCIAL_TYPE_GM_HAPPY - AGPDCHAR_SOCIAL_TYPE_GM_GREETING + 1, };

		AgcdSocialForGM(RwInt32 tid) : social_(tid,AGPDCHAR_SOCIAL_TYPE_GM_GREETING)	{		};
		CSocialAnimAndSound<num>&	bGetSocial()										{	return social_;								}
		AgcdAnimation2*				bGetAnim(AgpdCharacterSocialType socialType)		{	return social_.bGetAnim(socialType);		};
		const RwChar*				bGetSoundName(AgpdCharacterSocialType socialType)	{	return social_.bGetSoundName(socialType);	};

	private:
		CSocialAnimAndSound<num>		social_;
	};

	typedef std::vector<AgcdSocialGeneral*>		SocialGenVec;
	typedef SocialGenVec::iterator				SocialGenVecItr;
	typedef std::vector<AgcdSocialLoginMode*>	SocialLoginVec;
	typedef SocialLoginVec::iterator			SocialLoginVecItr;
	typedef std::vector<AgcdSocialForGM*>		SocialGMVec;
	typedef SocialGMVec::iterator				SocialGMVecItr;

	template< typename T >
	VOID RemoveAllSocial( T& container, AgcaAnimation2& agcaAnimation2 )
	{
		for( T::iterator Itr = container.begin(); Itr != container.end(); ++Itr )
			(*Itr)->bGetSocial().bRemoveAllAnim( agcaAnimation2 );
		container.clear();
	}

	template< typename T >
	class functrCmpTID
	{
	public:
		functrCmpTID(RwInt32 tid) : tid_(tid)	{		};

		bool operator() ( T* psocial )
		{
			return (tid_ == psocial->bGetSocial().bGetTID());
		}

	private:
		const RwInt32 tid_;
	};

	template< typename SOCIAL_CONTAINER, typename SOCIAL_TYPE >
	AgcdAnimData2* FindAndGetAnim(SOCIAL_CONTAINER& container, INT32 tid, AgpdCharacterSocialType socialtype, AgcaAnimation2& agcaAnimation2)
	{
		SOCIAL_CONTAINER::iterator find = std::find_if( container.begin(), container.end(), functrCmpTID<SOCIAL_TYPE>(tid) );
		if( find == container.end() || !(*find) || !((*find)->bGetAnim(socialtype)) )
			return NULL;

		agcaAnimation2.ReadRtAnim( (*find)->bGetAnim( socialtype ) );
		return (*find)->bGetAnim(socialtype)->m_pcsHead;
	};

	template< typename SOCIAL_CONTAINER, typename SOCIAL_TYPE >
	LPCSTR FindAndGetSound(SOCIAL_CONTAINER& container, INT32 tid, AgpdCharacterSocialType socialtype)
	{
		SOCIAL_CONTAINER::iterator find = std::find_if( container.begin(), container.end(), functrCmpTID<SOCIAL_TYPE>(tid) );
		if( find == container.end() || !(*find) || !((*find)->bGetAnim(socialtype)) )
		{
			ASSERT( !"the TID is not found" );
			return NULL;
		}

		return (*find)->bGetSoundName( socialtype );
	};
};

class AgcmEventEffectData
{
public:
	struct	stMovingSoundSet
	{
		stMovingSoundSet() : nTID(0)
		{
			memset(pacsMovingSound, 0, sizeof(AgcdEventEffectSound *) * APMMAP_MATERIAL_NUM * AGCM_EVENT_EFFECT_DATA_SOUND_FOOT_INDEX_NUM);
		}

		INT32					nTID;
		AgcdEventEffectSound*	pacsMovingSound[APMMAP_MATERIAL_NUM][AGCM_EVENT_EFFECT_DATA_SOUND_FOOT_INDEX_NUM];		// 탈것의 Templte ID , 맨몸이면 0이다.
	};
	typedef vector< stMovingSoundSet >		MovingSoundSetVec;
	typedef MovingSoundSetVec::iterator		MovingSoundSetVecItr;

public:
	AgcmEventEffectData();
	virtual ~AgcmEventEffectData();

public:
	BOOL					ReadSocialAnimationData(CHAR *szSocialPC, CHAR *szSocialGM, CHAR *szDataPath, BOOL bDecryption);

	BOOL					ReadSpiritStoneHitEffectData(CHAR *szPathName, BOOL bDecryption);
	BOOL					ReadSpiritStoneAttrEffectData(CHAR *szPathName, BOOL bDecryption);

	BOOL					ReadCommonCharEffectData(CHAR *szPathName, BOOL bDecryption);
	BOOL					ReadConvertedItemGradeEffectData(CHAR *szPathName, BOOL bDecryption);
	BOOL					ReadConvertedArmourEffectData(CHAR *szPathName, BOOL bDecryption);

	BOOL					ReadMovingSoundData(CHAR *szPathName, BOOL bDecryption);
	VOID					SetMaxSoundData(INT32 lMax);

	AgcdEventEffectSound*	GetAttackSound(INT32 lWeaponType);
	AgcdEventEffectSound*	GetHitSound(INT32 lWeaponType, INT32 lIndex = -1);
	AgcdEventEffectSound*	GetMovingSound( stMovingSoundSet * pSoundSet , INT32 lMaterialIndex, INT32 lFootIndex);

	MTRand*					GetRandomModule()	{return &m_csRandom;}

	AgcdUseEffectSetData*	GetCommonCharEffectData(INT32 lCommonCharType, INT32 lIndex);
	AgcdUseEffectSetData*	GetSpiritStoneAttrEffectData(INT32 lSpiritStoneType, INT32 lIndex);
	INT32					GetSpiritStoneAttrPointData(INT32 lSpiritStoneType, INT32 lIndex);
	AgcdUseEffectSetData*	GetSpiritStoneHitEffectData(INT32 lSpiritStoneType, INT32 lSpiritStoneLevel, INT32 lIndex);
	AgcdUseEffectSetData*	GetSpiritStoneHitEffectData(INT32 lNumSpiritStone, INT32 lRank, INT32 lLevel, 
														INT32 lNumConvertedWaterAttr, INT32 lNumConvertedFireAttr, INT32 lNumConvertedEarthAttr, 
														INT32 lNumConvertedAirAttr, INT32 lNumConvertedMagicAttr, INT32 lIndex);

	AgcdUseEffectSetData*	GetConvertedWeaponGradeEffectData(INT32 lNumConverted);

	INT32					SearchCommonCharTypeIndex(CHAR *szCommonCharType);
	INT32					SearchSpiritStoneIndex(CHAR *szSpiritStone);
	INT32					SearchSpiritStoneIndex(INT32 lSSType);

	AgcdAnimData2*			GetSocialAnim(INT32 tid, AgpdCharacterSocialType socialtype);
	LPCSTR					GetSocialSound(INT32 tid, AgpdCharacterSocialType socialtype);
	//for export
	VOID					ForXport_GetAllSocialAnim( std::vector<AgcdAnimData2*>& container );
	VOID					RemoveLoginSocial(void);

	VOID					RemoveAllSocialAnimation();

	AgcdConvertedArmourFX*	GetConvertedArmourFX(INT32 lSSIndex, INT32 lPoint, BOOL bAdd = FALSE);

	AgcdUseEffectSetData*	SearchCommonCharEffectDataByID(INT32 lID);
	AgcdUseEffectSetData*	SearchCommonCharEffectDataByEID(INT32 lEID);

	stMovingSoundSet*		GetMovingSoundSet( INT32 nTID );

protected:
	BOOL					ReadCommonEffectData(AuExcelLib *pcsAuExcelTxtLib, AgcdUseEffectSetData *pstEffectData, INT32 lStartColumn, INT32 lPixKeyRow, INT32 lRow);
	BOOL					ReadCommonFXData(AuExcelLib *pcsAuExcelTxtLib, RpMaterial *pstMaterial, INT32 lNumMaterial, INT32 lStartColumn, INT32 lPixKeyRow, INT32 lPixTypeRow, INT32 lRow);

	BOOL					ReadCommonCharEffectData(AgcdUseEffectSetData *pstEffectData, INT32 lStartColumn, INT32 lPixKeyRow, INT32 lRow);
	BOOL					ReadSpiritStoneEffectData(CHAR *szPathName, BOOL bDecryption, AgcmEventEffectDataParamFunction pfGetEffectSetData);

	INT32					SearchMaterialIndex(CHAR *szMaterialName);

	BOOL					SetAttackSound(INT32 lWeaponType, AgcdEventEffectSound *pstSound);
	BOOL					SetHitSound(INT32 lWeaponType, INT32 lIndex, AgcdEventEffectSound *pstSound);
	BOOL					SetMovingSound( stMovingSoundSet * pSoundSet , INT32 lMaterialIndex, INT32 lFootIndex, AgcdEventEffectSound *pstSound);
	
	VOID					SetNumHitSound();

	AgcdUseEffectSetData*	SearchEmptyCommonCharEffectData(INT32 lCommonCharType);
	AgcdUseEffectSetData*	SearchEmptySpiritStoneAttrEffectData(INT32 lSpiritStoneType, INT32 *plIndex);
	AgcdUseEffectSetData*	SearchEmptySpiritStoneHitEffectData(INT32 lSpiritStoneType, INT32 lSpiritStoneLevel);

	INT32					GetSocialTypeIndex(CHAR *szSocialType);
	INT32					GetRaceIndex(CHAR *szRace);
	INT32					GetClassIndex(CHAR *szClass);
	INT32					GetGenderIndex(CHAR *szGender);

	BOOL					InitializeAnimation();

	BOOL					_CreateCharEffectMemory	( INT32 nCount );
	BOOL					_DestroyCharEffectMemory( VOID );

protected:
	AgcaEffectData2				m_csEffectDataAdmin2;
	AgcaEventEffectSound		m_csSoundDataAdmin;
	AgcaAnimation2				m_csAnimation2;

	AgcdEventEffectSound*		m_pacsAttackSound[AGPMITEM_EQUIP_WEAPON_TYPE_NUM];
	AgcdEventEffectSound*		m_pacsHitSound[AGPMITEM_EQUIP_WEAPON_TYPE_NUM][AGCM_EVENT_EFFECT_DATA_SOUND_HIT_SOUND_NUM];
	AgcdEventEffectSound*		m_pcsDefaultAttackSound;
	AgcdEventEffectSound*		m_pacsDefaultHitSound[AGCM_EVENT_EFFECT_DATA_SOUND_HIT_SOUND_NUM];	

	MovingSoundSetVec			m_vecMovingSoundSet;

	INT32						m_alNumHitSound[AGPMITEM_EQUIP_WEAPON_TYPE_NUM];
	INT32						m_lNumDefaultHitSound;

	AgcdUseEffectSetData*		m_pastConvertedWeaponGradeEffect[ AGCM_EVENT_EFFECT_DATA_CONVERTED_ITEM_GRADE_NUM ];
	//AgcdUseEffectSetData		m_astCommonCharEffect[ E_COMMON_CHAR_MAX_TYPE ][ AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA ];
	AgcdUseEffectSetData**		m_ppCommonCharEffect;

	INT32						m_nCommonCharEffectCount;

	AgcdConvertedArmourFX*		m_pacsComvertedArmourGradeEffect[ E_SPIRIT_STONE_TYPE_NUM ][ AGCM_EVENT_EFFECT_DATA_MAX_GRADE_CONVERTED_ARMOUR ];

	AgcdUseEffectSetData*		m_pastSpiritStoneHitEffect[ E_SPIRIT_STONE_TYPE_NUM ][AGCM_EVENT_EFFECT_DATA_MAX_SS_LEVEL ][ AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA ];
	AgcdUseEffectSetData*		m_pastSpiritStoneAttrEffect[ E_SPIRIT_STONE_TYPE_NUM ][AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA ];
	INT32						m_alSpiritStoneAttrPoint[ E_SPIRIT_STONE_TYPE_NUM ][ AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA ];

	NS_SOCIAL::SocialGenVec		m_vecSocialGen;
	NS_SOCIAL::SocialLoginVec	m_vecSocialLogin;
	NS_SOCIAL::SocialGMVec		m_vecSocialGM;
	CHAR*						m_pszAnimationDataPath;

	MTRand						m_csRandom;
};

#endif // __AGCM_EVENT_EFFECT_SOUND_H__