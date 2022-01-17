// AgpmFactors.cpp: implementation of the AgpmFactors class.
//
//////////////////////////////////////////////////////////////////////

#include "AgpmFactors.h"

#include "AuMath.h"
#include <math.h>
#include <stdio.h>
//#include "AuIniFile.h"
#include "ApMemoryTracker.h"
#include "ApModuleStream.h"

// AgpdFactor class member functions
//////////////////////////////////////////////////////////////////////

// AgpmFactors class member functions
//////////////////////////////////////////////////////////////////////

CHAR *g_aszFactorsTypeName[AGPD_FACTORS_MAX_TYPE] =
{
	NULL,
	"CharStatus",
	"CharType",
	"CharPoint",
	"CharPointMax",
	"CharPointRecoveryRate",
	"Attribute",
	"Damage",
	"Defense",
	"Attack",
	"Item",
	"DIRT",
	"Price",
	NULL
};

CHAR *g_aszFactorsININame[AGPD_FACTORS_MAX_TYPE][2] =
{
	{NULL							, NULL							},
	{"CharStatusStart"				, "CharStatusEnd"				},
	{"CharTypeStart"				, "CharTypeEnd"					},
	{"CharPointStart"				, "CharPointEnd"				},
	{"CharPointMaxStart"			, "CharPointMaxEnd"				},
	{"CharPointRecoveryRateStart"	, "CharPointRecoveryRateEnd"	},
	{NULL							, NULL							},
	{"DamageStart"					, "DamageEnd"					},
	{"DefenseStart"					, "DefenseEnd"					},
	{"AttackStart"					, "AttackEnd"					},
	{"ItemStart"					, "ItemEnd"						},
	{"DIRTStart"					, "DIRTEnd"						},
	{"PriceStart"					, "PriceEnd"					},
	{NULL							, NULL							}
};

CHAR *g_aszFactorININame[AGPD_FACTORS_MAX_TYPE][20] =
{
	{NULL},
	{"Con"				, "Str"				, "Int"				, "Dex"				, "Cha"				, "Luk"				, "Wis"				, "Level"			, "Movement"		, "MovementFast"		},
	{"Race"				, "Gender"			, "Class"			},
	{"Hp"				, "Mp"				, "Sp"				, "Exp"				, "ExpHigh"			, "AttackPoint"		, "MagicAttackPoint", "MagicIntensity"	, "Agro"			},
	{"Hp"				, "Mp"				, "Sp"				, "Exp"				, "ExpHigh"			, "AttackPoint"		, "MagicAttackPoint", "MagicIntensity"	, "AR"				, "DR"				, "BaseEXP"				},
	{"Hp"				, "Mp"				, "Sp"				},
	{"Physical"			, "Magic"			, "Water"			, "Fire"			, "Earth"			, "AIR"				, "Poison"			, "Lightening"		, "Ice"				, "PhysicalBlock"		, "Skill"		, "Heroic"	,	"Heroic_Melee"	,	"Heroic_Ranged"	,	"Heroic_Magic"	},						
	{"MinDamage"		, "MaxDamage"		},
	{"DefensePoint"		, "DefenseRate"		},
	{"AttackRange"		, "HitRange"		, "Speed"			, "SkillCast"		, "SkillDelay"		, "HitRate"			, "EvadeRate"		, "DodgeRate"		},
	//{"Durability"		, "Hand"			, "Rank"			, "EgoLevel"		, "EgoMaxLevel"		, "EgoExp"			, "SoulLevel"		, "SoulRace"		, "SoulCubeLevel"	, "PhysicalRank"		},
	{"Durability"		, "Hand"			, "Rank"			, "PhysicalRank"	, "MaxDurability"	},
	{"Duration"			, "Intensity"		, "Range"			, "Target"			},
	{"Price"			, "PCPrice"			}
};

AgpdFactorDetail	g_alFactorDetail[AGPD_FACTORS_MAX_TYPE] =
{
	{AGPD_FACTORS_TYPE_RESULT		, 1												},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_CHARSTATUS_MAX_TYPE				},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_CHARTYPE_MAX_TYPE				},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_CHARPOINT_MAX_TYPE				},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_CHARPOINTMAX_MAX_TYPE			},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_CHARPOINT_RECOVERY_RATE_MAX_TYPE	},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_ATTRIBUTE_MAX_TYPE				},
	{AGPD_FACTORS_TYPE_ATTRIBUTE	, AGPD_FACTORS_DAMAGE_MAX_TYPE					},
	{AGPD_FACTORS_TYPE_ATTRIBUTE	, AGPD_FACTORS_DEFENSE_MAX_TYPE					},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_ATTACK_MAX_TYPE					},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_ITEM_MAX_TYPE					},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_DIRT_MAX_TYPE					},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_PRICE_MAX_TYPE					},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_OWNER_MAX_TYPE					},
	{AGPD_FACTORS_TYPE_NONE			, AGPD_FACTORS_AGRO_MAX_TYPE					},
};

INT16	g_nFactorSize[AGPD_FACTORS_MAX_TYPE] = 
{
	sizeof(AgpdFactor),
	sizeof(AgpdFactorCharStatus),
	sizeof(AgpdFactorCharType),
	sizeof(AgpdFactorCharPoint),
	sizeof(AgpdFactorCharPointMax),
	sizeof(AgpdFactorCharPointRecoveryRate),
	sizeof(AgpdFactorAttribute),
	sizeof(AgpdFactorDamage),
	sizeof(AgpdFactorDefense),
	sizeof(AgpdFactorAttack),
	sizeof(AgpdFactorItem),
	sizeof(AgpdFactorDIRT),
	sizeof(AgpdFactorPrice),
	sizeof(AgpdFactorOwner),
	sizeof(AgpdFactorAgro)
};

BOOL	g_bFactorUseFloat[AGPD_FACTORS_MAX_TYPE][20] = 
{
	{ 0 },
	{ TRUE  , TRUE  , TRUE  , TRUE  , TRUE  , TRUE  , TRUE  , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE },
	{ FALSE , FALSE , FALSE },
	{ FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , TRUE, FALSE },
	{ FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE },
	{ FALSE , FALSE , FALSE },
	{ TRUE  , TRUE  , TRUE  , TRUE  , TRUE  , TRUE  , TRUE  , TRUE  , TRUE	, TRUE	, TRUE, TRUE, TRUE, TRUE, TRUE },
	{ FALSE , FALSE },
	{ FALSE , FALSE },
	{ FALSE , FALSE , FALSE , FALSE	, FALSE	, FALSE	, FALSE	, FALSE	},
	//{ FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE },
	{ FALSE , FALSE , FALSE , FALSE , FALSE },
	{ FALSE , FALSE , FALSE , FALSE , FALSE , FALSE , FALSE, FALSE },
	{ FALSE , FALSE , FALSE , FALSE },
	{ FALSE , FALSE },
	{ FALSE , FALSE , FALSE , FALSE , FALSE }
};

ApString<AGPM_FACTORS_MAX_LENGTH_DATA_TYPE_NAME>	g_strDataTypeName[AGPM_FACTORS_MAX_NUM_DATA_TYPE_NAME];
INT32												g_lDataTypeNameCount[AGPM_FACTORS_MAX_NUM_DATA_TYPE_NAME] = { 0, };

AgpmFactors::AgpmFactors()
{
	SetModuleName("AgpmFactors");

	// flag length = 2bytes
	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
							AUTYPE_PACKET,		1,			// Result
							AUTYPE_PACKET,		1,			// CharStatus
							AUTYPE_PACKET,		1,			// CharType
							AUTYPE_PACKET,		1,			// CharPoint
							AUTYPE_PACKET,		1,			// CharPointMax
							AUTYPE_PACKET,		1,			// CharPointRecoveryRate
							AUTYPE_PACKET,		1,			// Attribute
							AUTYPE_PACKET,		1,			// Damage
							AUTYPE_PACKET,		1,			// Defense
							AUTYPE_PACKET,		1,			// Attack
							AUTYPE_PACKET,		1,			// Item
							AUTYPE_PACKET,		1,			// DIRT
							AUTYPE_PACKET,		1,			// Price
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_RESULT].SetFlagLength(sizeof(INT8));
	m_acsPacket[AGPD_FACTORS_TYPE_RESULT].SetFieldType(
							AUTYPE_PACKET,		1,			// result
							AUTYPE_END,			0
							);

	// flag length = 1byte
	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].SetFlagLength(sizeof(INT16));
	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].SetFieldType(
							AUTYPE_INT32,		1,			// con
							AUTYPE_INT32,		1,			// str
							AUTYPE_INT32,		1,			// int
							AUTYPE_INT32,		1,			// dex
							AUTYPE_INT32,		1,			// cha
							AUTYPE_INT32,		1,			// luk
							AUTYPE_INT32,		1,			// wis
							AUTYPE_INT32,		1,			// level
							AUTYPE_INT32,		1,			// movement
							AUTYPE_INT32,		1,			// movement fast
							AUTYPE_INT32,		1,			// union rank
							AUTYPE_INT32,		1,			// murderer point
							AUTYPE_INT32,		1,			// mukza point
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_TYPE].SetFlagLength(sizeof(INT8));
	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_TYPE].SetFieldType(
							AUTYPE_INT32,		1,			// race
							AUTYPE_INT32,		1,			// gender
							AUTYPE_INT32,		1,			// class
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT].SetFlagLength(sizeof(INT32));
	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT].SetFieldType(
							AUTYPE_INT32,		1,			// HP
							AUTYPE_INT32,		1,			// MP
							AUTYPE_INT32,		1,			// SP
							AUTYPE_INT32,		1,			// EXPLOW
							AUTYPE_INT32,		1,			// EXPHIGH
							AUTYPE_INT32,		1,			// Attack Point
							AUTYPE_INT32,		1,			// Magic Attack Point
							AUTYPE_INT32,		1,			// Magic Intensity
							AUTYPE_INT32,		1,			// Agro Point
							AUTYPE_INT32,		1,			// Dmg Normal
							AUTYPE_INT32,		1,			// Dmg Attr Magic
							AUTYPE_INT32,		1,			// Dmg Attr Water
							AUTYPE_INT32,		1,			// Dmg Attr Fire
							AUTYPE_INT32,		1,			// Dmg Attr Earth
							AUTYPE_INT32,		1,			// Dmg Attr Air
							AUTYPE_INT32,		1,			// Dmg Attr Poison
							AUTYPE_INT32,		1,			// Dmg Attr Lightening
							AUTYPE_INT32,		1,			// Dmg Attr Ice
							AUTYPE_INT32,		1,			// Bonus Exp
							AUTYPE_INT32,		1,			// Dmg Attr Heroic 
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT_MAX].SetFlagLength(sizeof(INT16));
	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT_MAX].SetFieldType(
							AUTYPE_INT32,		1,			// HP
							AUTYPE_INT32,		1,			// MP
							AUTYPE_INT32,		1,			// SP
							AUTYPE_INT32,		1,			// EXPLOW
							AUTYPE_INT32,		1,			// EXPHIGH
							AUTYPE_INT32,		1,			// Attack Point
							AUTYPE_INT32,		1,			// Magic Attack Point
							AUTYPE_INT32,		1,			// Magic Intensity
							AUTYPE_INT32,		1,			// AR
							AUTYPE_INT32,		1,			// DR
							AUTYPE_INT32,		1,			// Base EXP
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT_RECOVERY_RATE].SetFlagLength(sizeof(INT8));
	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT_RECOVERY_RATE].SetFieldType(
							AUTYPE_INT32,		1,			// HP
							AUTYPE_INT32,		1,			// MP
							AUTYPE_INT32,		1,			// SP
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_ATTRIBUTE].SetFlagLength(sizeof(INT16));
	m_acsPacket[AGPD_FACTORS_TYPE_ATTRIBUTE].SetFieldType(
							AUTYPE_INT32,		1,			// Physical
							AUTYPE_INT32,		1,			// Magic
							AUTYPE_INT32,		1,			// Water
							AUTYPE_INT32,		1,			// Fire
							AUTYPE_INT32,		1,			// Earth
							AUTYPE_INT32,		1,			// Air
							AUTYPE_INT32,		1,			// Poison
							AUTYPE_INT32,		1,			// Lightening
							AUTYPE_INT32,		1,			// Ice
							AUTYPE_INT32,		1,			// Physical Block
							AUTYPE_INT32,		1,			// Skill Block
							AUTYPE_INT32,		1,			// Heroic
							AUTYPE_INT32,		1,			// Heroic_Melee
							AUTYPE_INT32,		1,			// Heroic_Ranged
							AUTYPE_INT32,		1,			// Heroic_Magic
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_DAMAGE].SetFlagLength(sizeof(INT8));
	m_acsPacket[AGPD_FACTORS_TYPE_DAMAGE].SetFieldType(
							AUTYPE_PACKET,		1,			// min damage
							AUTYPE_PACKET,		1,			// max damage
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_DEFENSE].SetFlagLength(sizeof(INT8));
	m_acsPacket[AGPD_FACTORS_TYPE_DEFENSE].SetFieldType(
							AUTYPE_PACKET,		1,			// defense point
							AUTYPE_PACKET,		1,			// defense rate
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_ATTACK].SetFlagLength(sizeof(INT8));
	m_acsPacket[AGPD_FACTORS_TYPE_ATTACK].SetFieldType(
							AUTYPE_INT32,		1,			// attack range
							AUTYPE_INT32,		1,			// hit range
							AUTYPE_INT32,		1,			// speed
							AUTYPE_INT32,		1,			// skill cast
							AUTYPE_INT32,		1,			// skill delay
							AUTYPE_INT32,		1,			// hit rate
							AUTYPE_INT32,		1,			// evade rate
							AUTYPE_INT32,		1,			// dodge rate
							AUTYPE_END,			0
							);

	//m_acsPacket[AGPD_FACTORS_TYPE_ITEM].SetFlagLength(sizeof(INT16));
	m_acsPacket[AGPD_FACTORS_TYPE_ITEM].SetFlagLength(sizeof(INT8));
	m_acsPacket[AGPD_FACTORS_TYPE_ITEM].SetFieldType(
							AUTYPE_INT32,		1,			// durability
							AUTYPE_INT32,		1,			// hand
							AUTYPE_INT32,		1,			// rank
							//AUTYPE_INT32,		1,			// ego item level
							//AUTYPE_INT32,		1,			// ego item maximum level
							//AUTYPE_INT32,		1,			// ego item exp
							//AUTYPE_INT32,		1,			// soul level
							//AUTYPE_INT32,		1,			// soul race
							//AUTYPE_INT32,		1,			// soulcube level
							AUTYPE_INT32,		1,			// physical rank
							AUTYPE_INT32,		1,			// max durability
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_DIRT].SetFlagLength(sizeof(INT8));
	m_acsPacket[AGPD_FACTORS_TYPE_DIRT].SetFieldType(
							AUTYPE_INT32,		1,			// duration
							AUTYPE_INT32,		1,			// intensity
							AUTYPE_INT32,		1,			// range
							AUTYPE_INT32,		1,			// target
							AUTYPE_INT32,		1,			// skill level
							AUTYPE_INT32,		1,			// skill point
							AUTYPE_INT32,		1,			// skill exp
							AUTYPE_INT32,		1,			// heroic point
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_PRICE].SetFlagLength(sizeof(INT8));
	m_acsPacket[AGPD_FACTORS_TYPE_PRICE].SetFieldType(
							AUTYPE_INT32,		1,			// npc price
							AUTYPE_INT32,		1,			// pc price
							AUTYPE_INT32,		1,			// money high
							AUTYPE_INT32,		1,			// money low
							AUTYPE_END,			0
							);

	m_acsPacket[AGPD_FACTORS_TYPE_OWNER].SetFlagLength(sizeof(INT8));
	m_acsPacket[AGPD_FACTORS_TYPE_OWNER].SetFieldType(
							AUTYPE_END,			0
							);
}

AgpmFactors::~AgpmFactors()
{
}


BOOL AgpmFactors::OnAddModule()
{
	// Streamming 추가~(010803, Bob)
	if(!AddStreamCallback(AGPM_FACTORS_STREAM_TYPE_CHARTYPE_NAME, CharacterTypeStreamReadCB, CharacterTypeStreamWriteCB, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmFactors::OnInit()
{
	return TRUE;
}

BOOL AgpmFactors::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmFactors::OnDestroy()
{
	return TRUE;
}

BOOL AgpmFactors::InitFactor(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return FALSE;

	DestroyFactor(pcsFactor);

	/*
	// NULL로 초기화
	for (int i = 0; i < AGPD_FACTORS_MAX_TYPE; ++i)
	{
		if (pcsFactor->m_pvFactor[i])
			DestroyFactor(pcsFactor, i);

		pcsFactor->m_pvFactor[i] = NULL;
	}
	*/

	return TRUE;
}

BOOL AgpmFactors::DestroyFactor(AgpdFactor *pcsFactor)
{
	if ( !pcsFactor || IsBadReadPtr(pcsFactor, sizeof(AgpdFactor*)) )
		return FALSE;

	// 먼저 result factor를 삭제한다.
	if (pcsFactor->m_pvFactor[AGPD_FACTORS_TYPE_RESULT] != NULL)
	{
		DestroyFactor((AgpdFactor *) pcsFactor->m_pvFactor[AGPD_FACTORS_TYPE_RESULT]);

		//pcsFactor->m_pvFactor[AGPD_FACTORS_TYPE_RESULT]	= NULL;
	}

	for (int i = 0; i < AGPD_FACTORS_MAX_TYPE; ++i)
	{
		if ( pcsFactor->m_pvFactor[i] != NULL && !IsBadReadPtr(pcsFactor->m_pvFactor[i], sizeof(void*)) )
		{
//			free(pcsFactor->m_pvFactor[i]);
			FreeFactorType((eAgpdFactorsType) i, pcsFactor->m_pvFactor[i]);
			/*
			if (-1 != m_csMemoryPool.GetTypeIndex())
				m_csMemoryPool.Free(pcsFactor->m_pvFactor[i]);
			else
				delete [] (BYTE*)pcsFactor->m_pvFactor[i];
			*/

			pcsFactor->m_pvFactor[i] = NULL;
		}
	}

	return TRUE;
}

BOOL AgpmFactors::DestroyFactor(AgpdFactor *pcsFactor, INT16 nType)
{
	if (!pcsFactor || nType < 0 || nType >= AGPD_FACTORS_MAX_TYPE)
		return FALSE;

	if (!pcsFactor->m_pvFactor[nType])
		return TRUE;

//	free(pcsFactor->m_pvFactor[nType]);
	FreeFactorType((eAgpdFactorsType) nType, pcsFactor->m_pvFactor[nType]);
	/*
	if (-1 != m_csMemoryPool.GetTypeIndex())
		m_csMemoryPool.Free(pcsFactor->m_pvFactor[nType]);
	else
		delete [] (BYTE*)pcsFactor->m_pvFactor[nType];
	*/

	pcsFactor->m_pvFactor[nType] = NULL;

	return TRUE;
}

PVOID AgpmFactors::SetFactor(AgpdFactor *pcsDstFactor, AgpdFactor *pcsSrcFactor, INT16 nType)
{
	if (!pcsDstFactor || nType < 0 || nType >= AGPD_FACTORS_MAX_TYPE)
		return NULL;
	
	if (pcsSrcFactor)
		return SetFactor(&pcsDstFactor->m_pvFactor[nType], pcsSrcFactor->m_pvFactor[nType], nType);
	else
		return SetFactor(&pcsDstFactor->m_pvFactor[nType], NULL, nType);

	return NULL;
}

PVOID AgpmFactors::SetFactor(PVOID *ppvDstFactor, PVOID pvSrcFactor, INT16 nType)
{
	if (nType < 0 || nType >= AGPD_FACTORS_MAX_TYPE)
		return NULL;

	// 처음 사용하는 경우라면 메모리를 할당한다.
	if (*ppvDstFactor == NULL)
	{
		//*ppvDstFactor = GlobalAlloc(GMEM_FIXED, g_nFactorSize[nType]);
//		*ppvDstFactor = malloc(g_nFactorSize[nType]);
//		*ppvDstFactor = (PVOID) new BYTE[g_nFactorSize[nType]];
		*ppvDstFactor	= AllocFactorType((eAgpdFactorsType) nType);
		/*
		if (-1 != m_csMemoryPool.GetTypeIndex())
			*ppvDstFactor = m_csMemoryPool.Alloc();
		else
			*ppvDstFactor = (PVOID) new BYTE[g_nFactorSize[nType]];
		*/

		if (*ppvDstFactor == NULL)
			return NULL;

		ZeroMemory(*ppvDstFactor, g_nFactorSize[nType]);
	}

	if (ppvDstFactor && pvSrcFactor)
	{
		CopyMemory(*ppvDstFactor, pvSrcFactor, g_nFactorSize[nType]);
	}

	return *ppvDstFactor;
}

AgpdFactor *AgpmFactors::SetFactors(AgpdFactor *pcsDstFactor, AgpdFactor *pcsSrcFactor, INT16 nFlag)
{
	if (!pcsDstFactor)
		return NULL;

	INT32	nIndex;

	for (nIndex = 0; nIndex < AGPD_FACTORS_MAX_TYPE; ++nIndex)
	{
		if ((1 << nIndex) & nFlag)
		{
			// 처음 사용하는 경우라면 메모리를 할당한다.
			if (pcsDstFactor->m_pvFactor[nIndex] == NULL)
			{
				//pcsDstFactor->m_pvFactor[nIndex] = GlobalAlloc(GMEM_FIXED, g_nFactorSize[nIndex]);
//				pcsDstFactor->m_pvFactor[nIndex] = malloc(g_nFactorSize[nIndex]);
//				pcsDstFactor->m_pvFactor[nIndex] = (PVOID) new BYTE[g_nFactorSize[nIndex]];
				pcsDstFactor->m_pvFactor[nIndex]	= AllocFactorType((eAgpdFactorsType) nIndex);
				/*
				if (-1 != m_csMemoryPool.GetTypeIndex())
					pcsDstFactor->m_pvFactor[nIndex] = m_csMemoryPool.Alloc();
				else
					pcsDstFactor->m_pvFactor[nIndex] = (PVOID) new BYTE[g_nFactorSize[nIndex]];
				*/

				if (pcsDstFactor->m_pvFactor[nIndex] == NULL)
					return NULL;

				ZeroMemory(pcsDstFactor->m_pvFactor[nIndex], g_nFactorSize[nIndex]);
			}

			if (pcsSrcFactor && pcsSrcFactor->m_pvFactor[nIndex])
				CopyMemory(pcsDstFactor->m_pvFactor[nIndex], pcsSrcFactor->m_pvFactor[nIndex], g_nFactorSize[nIndex]);
		}
	}

	return pcsDstFactor;
}

PVOID AgpmFactors::GetFactor(AgpdFactor *pcsFactor, eAgpdFactorsType eType)
{
	if (!pcsFactor || eType < 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return NULL;

	return pcsFactor->m_pvFactor[eType];
}

BOOL AgpmFactors::IsUseFactor(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return FALSE;

	BOOL	bIsUseFactor	= FALSE;

	for (int i = 0; i < AGPD_FACTORS_MAX_TYPE; ++i)
	{
		if (pcsFactor->m_pvFactor[i])
		{
			bIsUseFactor	= TRUE;
			break;
		}
	}

	return bIsUseFactor;
}

BOOL AgpmFactors::ReflectPacket(PVOID pvFactor, PVOID pvPacket, INT16 nPacketLength, eAgpdFactorsType eType)
{
	if (!pvFactor || !pvPacket || eType <= 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return TRUE;

	INT32 *	plFactor = (INT32 *) pvFactor;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		m_acsPacket[eType].GetField(FALSE, pvPacket, nPacketLength,  
						plFactor + 0,
						plFactor + 1,
						plFactor + 2,
						plFactor + 3,
						plFactor + 4,
						plFactor + 5,
						plFactor + 6,
						plFactor + 7,
						plFactor + 8,
						plFactor + 9,
						plFactor + 10,
						plFactor + 11,
						plFactor + 12,
						plFactor + 13,
						plFactor + 14,
						plFactor + 15,
						plFactor + 16,
						plFactor + 17,
						plFactor + 18,
						plFactor + 19);
	}
	else
	{
		INT32	lIndex;
//		PVOID	apvPacket[20];
//
//		memset(apvPacket, 0, sizeof(PVOID) * 20);

		ApSafeArray<PVOID, 20>	apvPacket;
		apvPacket.MemSetAll();

		m_acsPacket[eType].GetField(FALSE, pvPacket, nPacketLength,  
						&apvPacket[0],
						&apvPacket[1],
						&apvPacket[2],
						&apvPacket[3],
						&apvPacket[4],
						&apvPacket[5],
						&apvPacket[6],
						&apvPacket[7],
						&apvPacket[8],
						&apvPacket[9],
						&apvPacket[10],
						&apvPacket[11],
						&apvPacket[12],
						&apvPacket[13],
						&apvPacket[14],
						&apvPacket[15],
						&apvPacket[16],
						&apvPacket[17],
						&apvPacket[18],
						&apvPacket[19]);

//						apvPacket + 0,
//						apvPacket + 1,
//						apvPacket + 2,
//						apvPacket + 3,
//						apvPacket + 4,
//						apvPacket + 5,
//						apvPacket + 6,
//						apvPacket + 7,
//						apvPacket + 8,
//						apvPacket + 9,
//						apvPacket + 10,
//						apvPacket + 11,
//						apvPacket + 12,
//						apvPacket + 13,
//						apvPacket + 14,
//						apvPacket + 15,
//						apvPacket + 16,
//						apvPacket + 17,
//						apvPacket + 18,
//						apvPacket + 19);

		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			ReflectPacket((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex,
						  apvPacket[lIndex],
						  0,
						  g_alFactorDetail[eType].eFactorType);
		}
	}

	return TRUE;
}

//		ReflectPacket
//	Functions
//		- 받은 factor packet을 분석해서 pcsFactor에 반영시킨다.
//	Arguments
//		- pcsFactor : pvPacket에 있는 내용을 분석해서 반영시킬 factor
//		- pcsFactorCalc : pcsFactor 주인이 가지고 있는 AgpdFactorCalc pointer. 별 관심 없음 걍 NULL을 준다.
//				(패킷에 대한 값이 pcsFactor에 반영되고 그 후에 pcsFactorCalc 값이 재 계산된다)
//		- pvPacket : factor packet
//		- nPacketLength : packet length
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmFactors::ReflectPacket(AgpdFactor *pcsFactor, /*AgpdFactor *pcsFactorResult, */
								PVOID pvPacket, INT16 nPacketLength,
								AgpdFactor *pcsFactorBuffer)
{
	if (!pcsFactor || !pvPacket)
		return FALSE;

	//if (pcsFactorResult)
	//	EnumCallback(AGPM_FACTORS_CB_REMOVE_FACTOR, pcsFactor, pcsFactorResult);

	//PVOID	apvPacket[AGPD_FACTORS_MAX_TYPE];
	ApSafeArray<PVOID, AGPD_FACTORS_MAX_TYPE>	apvPacket;
	INT32	lIndex;

	//memset(apvPacket, 0, sizeof(PVOID) * AGPD_FACTORS_MAX_TYPE);
	apvPacket.MemSetAll();

	m_csPacket.GetField(FALSE, pvPacket, nPacketLength,
						&apvPacket[0],
						&apvPacket[1],
						&apvPacket[2],
						&apvPacket[3],
						&apvPacket[4],
						&apvPacket[5],
						&apvPacket[6],
						&apvPacket[7],
						&apvPacket[8],
						&apvPacket[9],
						&apvPacket[10],
						&apvPacket[11],
						&apvPacket[12]);
//						apvPacket + 0,
//						apvPacket + 1,
//						apvPacket + 2,
//						apvPacket + 3,
//						apvPacket + 4,
//						apvPacket + 5,
//						apvPacket + 6,
//						apvPacket + 7,
//						apvPacket + 8,
//						apvPacket + 9,
//						apvPacket + 10,
//						apvPacket + 11,
//						apvPacket + 12);

	// pcsFactor의 이전 값(파티원들간의 싱크 맞춰줄때 쓰이는 값)을 저장해놓는다.
	INT32	lOldLevel		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMovement	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMoveFast	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldUnionRank	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMurderer	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMukza		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldCharisma	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldHP			= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxHP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMP			= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxMP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldSP			= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxSP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldEXPLow		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldEXPHigh		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxEXPLow	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxEXPHigh	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	INT32	lOldSkillPoint	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	INT32	lOldDurability	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldHeroicPoint = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	AgpdFactor				*pcsFactorResult = (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (pcsFactorResult)
	{
		GetValue(pcsFactorResult, &lOldLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		GetValue(pcsFactorResult, &lOldMovement, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		GetValue(pcsFactorResult, &lOldMoveFast, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
		GetValue(pcsFactorResult, &lOldUnionRank, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_UNION_RANK);
		GetValue(pcsFactorResult, &lOldMurderer, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
		GetValue(pcsFactorResult, &lOldMukza, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA);
		GetValue(pcsFactorResult, &lOldCharisma, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
		GetValue(pcsFactorResult, &lOldHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		GetValue(pcsFactorResult, &lOldMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
		GetValue(pcsFactorResult, &lOldMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		GetValue(pcsFactorResult, &lOldMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
		GetValue(pcsFactorResult, &lOldSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
		GetValue(pcsFactorResult, &lOldMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);
		GetValue(pcsFactorResult, &lOldEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW);
		GetValue(pcsFactorResult, &lOldEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_HIGH);
		GetValue(pcsFactorResult, &lOldMaxEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW);
		GetValue(pcsFactorResult, &lOldMaxEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_HIGH);

		GetValue(pcsFactorResult, &lOldSkillPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_POINT);

		GetValue(pcsFactorResult, &lOldDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
		GetValue(pcsFactorResult, &lOldHeroicPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_HEROIC_POINT);
	}
	// pcsFactor의 이전 값(파티원들간의 싱크 맞춰줄때 쓰이는 값)을 저장해놓는다.

	for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (apvPacket[lIndex])
		{
			SetFactor(pcsFactor, NULL, lIndex);

			if (lIndex == AGPD_FACTORS_TYPE_RESULT)
				ReflectPacket((AgpdFactor *) GetFactor(pcsFactor, (eAgpdFactorsType) lIndex), apvPacket[lIndex], 0);
			else
				ReflectPacket(GetFactor(pcsFactor, (eAgpdFactorsType) lIndex), apvPacket[lIndex], 0, (eAgpdFactorsType) lIndex);
		}
	}

	INT32	lNewLevel = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMovement = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMoveFast = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewUnionRank = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMurderer = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMukza = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewCharisma = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewHP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMaxHP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMaxMP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewSP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMaxSP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewEXPLow = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewEXPHigh = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMaxEXPLow = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMaxEXPHigh = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	INT32	lNewSkillPoint = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	INT32	lNewDurability = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewHeroicPoint = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	AgpdFactorCharPoint *	pstCharPoint = NULL;

	pcsFactorResult = (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (pcsFactorResult)
	{
		GetValue(pcsFactorResult, &lNewLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		GetValue(pcsFactorResult, &lNewMovement, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		GetValue(pcsFactorResult, &lNewMoveFast, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
		GetValue(pcsFactorResult, &lNewUnionRank, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_UNION_RANK);
		GetValue(pcsFactorResult, &lNewMurderer, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
		GetValue(pcsFactorResult, &lNewMukza, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA);
		GetValue(pcsFactorResult, &lNewCharisma, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
		GetValue(pcsFactorResult, &lNewHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		GetValue(pcsFactorResult, &lNewMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
		GetValue(pcsFactorResult, &lNewMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		GetValue(pcsFactorResult, &lNewMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
		GetValue(pcsFactorResult, &lNewSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
		GetValue(pcsFactorResult, &lNewMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);
		GetValue(pcsFactorResult, &lNewEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW);
		GetValue(pcsFactorResult, &lNewEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_HIGH);
		GetValue(pcsFactorResult, &lNewMaxEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW);
		GetValue(pcsFactorResult, &lNewMaxEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_HIGH);

		GetValue(pcsFactorResult, &lNewSkillPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_SKILL_POINT);

		GetValue(pcsFactorResult, &lNewDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
		GetValue(pcsFactorResult, &lNewHeroicPoint, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_HEROIC_POINT);

		pstCharPoint = (AgpdFactorCharPoint *) GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	}

	if (lOldHP != lNewHP)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= IntToPtr(lOldHP);
		pvBuffer[1]	= IntToPtr(1);

		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_HP, pcsFactor, pvBuffer);
	}
	
	if (lOldMP != lNewMP)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= IntToPtr(lOldMP);
		pvBuffer[1]	= IntToPtr(1);

		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_MP, pcsFactor, pvBuffer);
	}

	if (lOldHP != lNewHP ||
		lOldLevel != lNewLevel ||
		lOldMaxHP != lNewMaxHP)
	{
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_PARTY, pcsFactor, NULL);
	}

	if (lOldHP != lNewHP ||
		lOldMP != lNewMP ||
		lOldLevel != lNewLevel ||
		lOldMaxHP != lNewMaxHP ||
		lOldEXPLow != lNewEXPLow ||
		lOldEXPHigh != lNewEXPHigh ||
		lOldMaxMP != lNewMaxMP ||
		lOldSP != lNewSP ||
		lOldMaxSP != lNewMaxSP ||
		lOldMaxEXPLow != lNewMaxEXPLow ||
		lOldMaxEXPHigh != lNewMaxEXPHigh)
	{
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_UI, pcsFactor, NULL);
	}

	if (lOldMovement != lNewMovement ||
			 lOldMoveFast != lNewMoveFast)
	{
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_MOVEMENT, pcsFactor, NULL);
	}

	if (pstCharPoint && pstCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL] != 0 ||
		pstCharPoint && pstCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_MAGIC] != 0 ||
		pstCharPoint && pstCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_WATER] != 0 ||
		pstCharPoint && pstCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_FIRE] != 0 ||
		pstCharPoint && pstCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_EARTH] != 0 ||
		pstCharPoint && pstCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_AIR] != 0)
	{
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_DAMAGE, pcsFactor, NULL);
	}

	if (pstCharPoint && pstCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_BONUS_EXP] != 0)
	{
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_BONUS_EXP, pcsFactor, NULL);

		pstCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_BONUS_EXP]	= 0;
	}

	if (lOldSkillPoint != lNewSkillPoint)
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_SKILL_POINT, pcsFactor, NULL);

	if (lOldMurderer != AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE
		&& lNewMurderer != AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE
		&& lOldMurderer != lNewMurderer)
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_MURDERER_POINT, pcsFactor, &lOldMurderer);

	if (lOldCharisma != AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE
		&& lNewCharisma != AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE
		&& lOldCharisma != lNewCharisma)
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_CHARISMA_POINT, pcsFactor, &lOldCharisma);

	if (lOldDurability != lNewDurability)
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_DURABILITY, pcsFactor, NULL);

	if (lOldHeroicPoint != lNewHeroicPoint)
	{
		// 이곳에 클라이언트 처리
		EnumCallback( AGPM_FACTORS_CB_UPDATE_FACTOR_HEROIC_POINT , pcsFactor , NULL );
	}

	if (pcsFactorResult)
		InitDamageFactors(pcsFactorResult, pcsFactorBuffer);

	return TRUE;
}

PVOID AgpmFactors::MakePacketFactor(PVOID pvFactor, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	if (eType < 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return NULL;

	//PVOID	apvPacketTemp[20];
	ApSafeArray<PVOID, 20>	apvPacketTemp;
	apvPacketTemp.MemSetAll();
	INT32	lIndex;
	INT16	nPacketLength;
	INT32 *	plFactor = (INT32 *) pvFactor;
	PVOID	pvValue = pvFactor;
	PVOID	pvPacket = NULL;

	//memset(apvPacketTemp, 0, sizeof(PVOID) * 20);

	if (pvFactor)
	{
		if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_RESULT)
		{
			pvPacket = MakePacketFactors((AgpdFactor *) pvFactor, (eAgpdFactorsType) lSubType1, lSubType2, lSubType3);
		}
		else if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
		{
			if (lSubType1 == (-1))
			{
				for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
				{
					if (plFactor[lIndex] != 0 || IsMustSend(eType, lIndex))
						apvPacketTemp[lIndex] = plFactor + lIndex;
				}

				pvPacket = m_acsPacket[eType].MakePacket(FALSE, &nPacketLength, 0, 
															apvPacketTemp[0],
															apvPacketTemp[1],
															apvPacketTemp[2],
															apvPacketTemp[3],
															apvPacketTemp[4],
															apvPacketTemp[5],
															apvPacketTemp[6],
															apvPacketTemp[7],
															apvPacketTemp[8],
															apvPacketTemp[9],
															apvPacketTemp[10],
															apvPacketTemp[11],
															apvPacketTemp[12],
															apvPacketTemp[13],
															apvPacketTemp[14],
															apvPacketTemp[15],
															apvPacketTemp[16],
															apvPacketTemp[17],
															apvPacketTemp[18],
															apvPacketTemp[19]);
			}
			else
			{
				for (lIndex = 0; lIndex < 20; ++lIndex)
				{
					if (lIndex == lSubType1)
						apvPacketTemp[lIndex] = plFactor + lIndex;
				}

				pvPacket = m_acsPacket[eType].MakePacket(FALSE, &nPacketLength, 0, 
															apvPacketTemp[0],
															apvPacketTemp[1],
															apvPacketTemp[2],
															apvPacketTemp[3],
															apvPacketTemp[4],
															apvPacketTemp[5],
															apvPacketTemp[6],
															apvPacketTemp[7],
															apvPacketTemp[8],
															apvPacketTemp[9],
															apvPacketTemp[10],
															apvPacketTemp[11],
															apvPacketTemp[12],
															apvPacketTemp[13],
															apvPacketTemp[14],
															apvPacketTemp[15],
															apvPacketTemp[16],
															apvPacketTemp[17],
															apvPacketTemp[18],
															apvPacketTemp[19]);
			}
		}
		else
		{
			for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
			{
				pvValue = (PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

				if (lSubType1 == lIndex || lSubType1 == (-1))
					apvPacketTemp[lIndex] = MakePacketFactor(pvValue, g_alFactorDetail[eType].eFactorType, lSubType2);
			}

			pvPacket = m_acsPacket[eType].MakePacket(FALSE, &nPacketLength, 0, 
														apvPacketTemp[0],
														apvPacketTemp[1],
														apvPacketTemp[2],
														apvPacketTemp[3],
														apvPacketTemp[4],
														apvPacketTemp[5],
														apvPacketTemp[6],
														apvPacketTemp[7],
														apvPacketTemp[8],
														apvPacketTemp[9],
														apvPacketTemp[10],
														apvPacketTemp[11],
														apvPacketTemp[12],
														apvPacketTemp[13],
														apvPacketTemp[14],
														apvPacketTemp[15],
														apvPacketTemp[16],
														apvPacketTemp[17],
														apvPacketTemp[18],
														apvPacketTemp[19]);

			for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
			{
				if (apvPacketTemp[lIndex])
					m_acsPacket[g_alFactorDetail[eType].eFactorType].FreePacket(apvPacketTemp[lIndex]);
			}
		}
	}

	return pvPacket;
}

PVOID AgpmFactors::MakePacketFactors(AgpdFactor *pcsFactor, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	if (!pcsFactor)
		return NULL;

	//PVOID	apvPacket[AGPD_FACTORS_MAX_TYPE];
	ApSafeArray<PVOID, AGPD_FACTORS_MAX_TYPE>	apvPacket;
	INT32	lIndex;
	INT16	nPacketLength;

	//memset(apvPacket, 0, sizeof(PVOID) * AGPD_FACTORS_MAX_TYPE);
	apvPacket.MemSetAll();

	for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (eType == AGPD_FACTORS_TYPE_NONE || eType == (eAgpdFactorsType) lIndex)
			apvPacket[lIndex] = MakePacketFactor(GetFactor(pcsFactor, (eAgpdFactorsType) lIndex), (eAgpdFactorsType) lIndex, lSubType1, lSubType2, lSubType3);
	}

	PVOID pvPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0, 
													apvPacket[0],
													apvPacket[1],
													apvPacket[2],
													apvPacket[3],
													apvPacket[4],
													apvPacket[5],
													apvPacket[6],
													apvPacket[7],
													apvPacket[8],
													apvPacket[9],
													apvPacket[10],
													apvPacket[11],
													apvPacket[12]);

	for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (apvPacket[lIndex])
			m_acsPacket[lIndex].FreePacket(apvPacket[lIndex]);
	}

	return pvPacket;
}

PVOID AgpmFactors::MakePacketFactorsOneType(AgpdFactor *pcsFactor, eAgpdFactorsType eType)
{
	if (!pcsFactor)
		return NULL;

	//PVOID	apvPacket[AGPD_FACTORS_MAX_TYPE];
	ApSafeArray<PVOID, AGPD_FACTORS_MAX_TYPE>	apvPacket;
	INT32	lIndex;
	INT16	nPacketLength;

	//memset(apvPacket, 0, sizeof(PVOID) * AGPD_FACTORS_MAX_TYPE);
	apvPacket.MemSetAll();

	for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (lIndex == eType)
		{
			apvPacket[lIndex] = MakePacketFactor(GetFactor(pcsFactor, (eAgpdFactorsType) lIndex), (eAgpdFactorsType) lIndex);
			break;
		}
	}

	if (lIndex == AGPD_FACTORS_MAX_TYPE)
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0, 
													apvPacket[0],
													apvPacket[1],
													apvPacket[2],
													apvPacket[3],
													apvPacket[4],
													apvPacket[5],
													apvPacket[6],
													apvPacket[7],
													apvPacket[8],
													apvPacket[9],
													apvPacket[10],
													apvPacket[11],
													apvPacket[12]);

	for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (apvPacket[lIndex])
			m_acsPacket[lIndex].FreePacket(apvPacket[lIndex]);
	}

	return pvPacket;
}

PVOID AgpmFactors::MakePacketResult(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return NULL;

	PVOID	pvPacketResult = MakePacketFactors(pcsFactorResult);
	if (!pvPacketResult)
		return NULL;

	INT16	nPacketLength = 0;

	PVOID	pvPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0,
													pvPacketResult,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	m_csPacket.FreePacket(pvPacketResult);

	return pvPacket;
}

PVOID AgpmFactors::MakePacketFactorsCharView(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	PVOID	pvPacketCharStatus		= NULL;
	PVOID	pvPacketCharType		= NULL;
	PVOID	pvPacketCharPoint		= NULL;
	PVOID	pvPacketCharPointMax	= NULL;
	PVOID	pvPacketAttack			= NULL;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return NULL;

	INT16	nPacketLength;

	AgpdFactorCharStatus	*pcsFactorResultCharStatus	= (AgpdFactorCharStatus *) GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_STATUS);
	if (pcsFactorResultCharStatus)
	{
		pvPacketCharStatus		= m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].MakePacket(FALSE, NULL, 0,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													&pcsFactorResultCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL],
													&pcsFactorResultCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT],
													&pcsFactorResultCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST],
													(pcsFactorResultCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_UNION_RANK]) ? &pcsFactorResultCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_UNION_RANK] : NULL,
													(pcsFactorResultCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER]) ? &pcsFactorResultCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER] : NULL,
													(pcsFactorResultCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA]) ? &pcsFactorResultCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA] : NULL);
	}

	pvPacketCharType		= MakePacketFactor(GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_TYPE), AGPD_FACTORS_TYPE_CHAR_TYPE);
	pvPacketCharPoint		= MakePacketFactor(GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	pvPacketCharPointMax	= MakePacketFactor(GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT_MAX), AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	pvPacketAttack			= MakePacketFactor(GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_ATTACK), AGPD_FACTORS_TYPE_ATTACK);

	PVOID pvPacketResult = m_csPacket.MakePacket(FALSE, &nPacketLength, 0, 
													NULL,
													pvPacketCharStatus,
													pvPacketCharType,
													pvPacketCharPoint,
													pvPacketCharPointMax,
													NULL,
													NULL,
													NULL,
													NULL,
													pvPacketAttack,
													NULL,
													NULL,
													NULL);

	if (pvPacketCharStatus)
		m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].FreePacket(pvPacketCharStatus);
	if (pvPacketCharType)
		m_acsPacket[AGPD_FACTORS_TYPE_CHAR_TYPE].FreePacket(pvPacketCharType);
	if (pvPacketCharPoint)
		m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT].FreePacket(pvPacketCharPoint);
	if (pvPacketCharPointMax)
		m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT_MAX].FreePacket(pvPacketCharPointMax);
	if (pvPacketAttack)
		m_acsPacket[AGPD_FACTORS_TYPE_ATTACK].FreePacket(pvPacketAttack);

	PVOID	pvPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0,
													pvPacketResult,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	m_csPacket.FreePacket(pvPacketResult);

	return pvPacket;
}

PVOID AgpmFactors::MakePacketFactorsCharHPMax(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	PVOID	pvPacketCharStatus		= NULL;
	PVOID	pvPacketCharType		= NULL;
	PVOID	pvPacketCharPoint		= NULL;
	PVOID	pvPacketCharPointMax	= NULL;
	PVOID	pvPacketAttack			= NULL;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return NULL;

	INT16	nPacketLength;

	pvPacketCharPoint		= MakePacketFactor(GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	if (!pvPacketCharPoint)
		return NULL;

	pvPacketCharPointMax	= MakePacketFactor(GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT_MAX), AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	if (!pvPacketCharPointMax)
	{
		m_csPacket.FreePacket(pvPacketCharPoint);
		return NULL;
	}

	PVOID pvPacketResult = m_csPacket.MakePacket(FALSE, &nPacketLength, 0, 
													NULL,
													NULL,
													NULL,
													pvPacketCharPoint,
													pvPacketCharPointMax,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	if (pvPacketCharPoint)
		m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT].FreePacket(pvPacketCharPoint);
	if (pvPacketCharPointMax)
		m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT_MAX].FreePacket(pvPacketCharPointMax);

	PVOID	pvPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0,
													pvPacketResult,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	m_csPacket.FreePacket(pvPacketResult);

	return pvPacket;
}

PVOID AgpmFactors::MakePacketFactorsCharHP(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return NULL;

	PVOID	pvPacketCharPoint		= MakePacketFactor(GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	INT16	nPacketLength;

	PVOID pvPacketResult = m_csPacket.MakePacket(FALSE, &nPacketLength, 0, 
													NULL,
													NULL,
													NULL,
													pvPacketCharPoint,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	if (pvPacketCharPoint)
		m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT].FreePacket(pvPacketCharPoint);

	PVOID	pvPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0,
													pvPacketResult,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	m_csPacket.FreePacket(pvPacketResult);

	return pvPacket;
}

PVOID AgpmFactors::MakePacketFactorsItemView(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	PVOID	pvPacketDamage			= NULL;
	PVOID	pvPacketDefense			= NULL;
	PVOID	pvPacketItem			= NULL;
	PVOID	pvPacketDIRT			= NULL;


	INT16	nPacketLength;

	pvPacketDamage = MakePacketFactor(GetFactor(pcsFactor, AGPD_FACTORS_TYPE_DAMAGE), AGPD_FACTORS_TYPE_DAMAGE);
	pvPacketDefense = MakePacketFactor(GetFactor(pcsFactor, AGPD_FACTORS_TYPE_DEFENSE), AGPD_FACTORS_TYPE_DEFENSE);
	pvPacketItem = MakePacketFactor(GetFactor(pcsFactor, AGPD_FACTORS_TYPE_ITEM), AGPD_FACTORS_TYPE_ITEM);
	pvPacketDIRT = MakePacketFactor(GetFactor(pcsFactor, AGPD_FACTORS_TYPE_DIRT), AGPD_FACTORS_TYPE_DIRT);

	PVOID pvPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0, 
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													pvPacketDamage,
													pvPacketDefense,
													NULL,
													pvPacketItem,
													pvPacketDIRT,
													NULL);

	m_acsPacket[AGPD_FACTORS_TYPE_DAMAGE].FreePacket(pvPacketDamage);
	m_acsPacket[AGPD_FACTORS_TYPE_DEFENSE].FreePacket(pvPacketDefense);
	m_acsPacket[AGPD_FACTORS_TYPE_ITEM].FreePacket(pvPacketItem);
	m_acsPacket[AGPD_FACTORS_TYPE_DIRT].FreePacket(pvPacketDIRT);

	return pvPacket;
}

PVOID AgpmFactors::MakePacketMoney(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	PVOID	pvPacketPrice			= NULL;
	INT16	nPacketLength;

	pvPacketPrice = MakePacketFactor(GetFactor(pcsFactor, AGPD_FACTORS_TYPE_PRICE), AGPD_FACTORS_TYPE_PRICE);

	PVOID pvPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0, 
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													pvPacketPrice);

	if (pvPacketPrice)
		m_acsPacket[AGPD_FACTORS_TYPE_PRICE].FreePacket(pvPacketPrice);

	return pvPacket;
}

PVOID AgpmFactors::MakePacketItem(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	AgpdFactorItem	*pcsFactorItem	= (AgpdFactorItem *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_ITEM);
	if (!pcsFactorItem)
		return NULL;

	PVOID	pvPacketItem	= m_acsPacket[AGPD_FACTORS_TYPE_ITEM].MakePacket(FALSE, NULL, 0,
													&pcsFactorItem->lValue[AGPD_FACTORS_ITEM_TYPE_DURABILITY],
													NULL,
													NULL,
													//NULL,
													//NULL,
													//NULL,
													//NULL,
													//NULL,
													//NULL,
													&pcsFactorItem->lValue[AGPD_FACTORS_ITEM_TYPE_PHYSICAL_RANK],
													&pcsFactorItem->lValue[AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY]);

	PVOID pvPacket = m_csPacket.MakePacket(FALSE, NULL, 0, 
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													pvPacketItem,
													NULL,
													NULL);

	m_acsPacket[AGPD_FACTORS_TYPE_ITEM].FreePacket(pvPacketItem);

	return pvPacket;
}

PVOID AgpmFactors::MakePacketItemRestrict(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	AgpdFactorCharStatus	*pcsFactorCharStatus	= (AgpdFactorCharStatus *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);
	if (!pcsFactorCharStatus)
		return NULL;

	PVOID	pvPacketCharStatus	= m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].MakePacket(FALSE, NULL, 0,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													&pcsFactorCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL],
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	PVOID pvPacket = m_csPacket.MakePacket(FALSE, NULL, 0, 
													NULL,
													pvPacketCharStatus,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].FreePacket(pvPacketCharStatus);

	return pvPacket;
}

/*
	2005.06.15. Modify By SungHoon
	AGPD_FACTORS_TYPE_RESULT 방식으로 수정
*/
PVOID AgpmFactors::MakePacketLevel(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	PVOID	pvPacketCharStatus			= NULL;
	INT16	nPacketLength;

	AgpdFactor				*pcsFactorResult	= (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	AgpdFactorCharStatus	*pstFactorCharStatus= (AgpdFactorCharStatus *) GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_STATUS);
	
	pvPacketCharStatus = MakePacketFactor(pstFactorCharStatus, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	PVOID pvFactorPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0, 
													NULL,
													pvPacketCharStatus,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	PVOID pvResultPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0, 
													pvFactorPacket,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);

	if (pvPacketCharStatus)
		m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].FreePacket(pvPacketCharStatus);

	return pvResultPacket;
}

PVOID AgpmFactors::MakePacketFactorDiffer(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor, eAgpdFactorsType eType)
{
	if (!pcsDestFactor || !pcsSourceFactor)
		return NULL;

	AgpdFactorCharPoint *pcsFactorSourceCharPoint	= (AgpdFactorCharPoint *) pcsSourceFactor;
	AgpdFactorCharPoint *pcsFactorDestCharPoint		= (AgpdFactorCharPoint *) pcsDestFactor;

	if (pcsFactorSourceCharPoint && pcsFactorDestCharPoint)
	{
		ApSafeArray<PVOID, 20>	apvTemp;
		apvTemp.MemSetAll();

		for (INT32 lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			apvTemp[lIndex]	= (pcsFactorSourceCharPoint->lValue[lIndex] != pcsFactorDestCharPoint->lValue[lIndex]) ? &pcsFactorDestCharPoint->lValue[lIndex] : NULL;
		}

		return m_acsPacket[eType].MakePacket(FALSE, NULL, 0,
						apvTemp[0],
						apvTemp[1],
						apvTemp[2],
						apvTemp[3],
						apvTemp[4],
						apvTemp[5],
						apvTemp[6],
						apvTemp[7],
						apvTemp[8],
						apvTemp[9],
						apvTemp[10],
						apvTemp[11],
						apvTemp[12],
						apvTemp[13],
						apvTemp[14],
						apvTemp[15],
						apvTemp[16],
						apvTemp[17],
						apvTemp[18],
						apvTemp[19]);
	}

	return NULL;
}

PVOID AgpmFactors::MakePacketFactorDiffer(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor)
{
	if (!pcsDestFactor || !pcsSourceFactor)
		return NULL;

	ApSafeArray<PVOID, AGPD_FACTORS_MAX_TYPE>	pvPacketFactor;
	pvPacketFactor.MemSetAll();

	for (int i = AGPD_FACTORS_TYPE_CHAR_STATUS; i <= AGPD_FACTORS_TYPE_PRICE; ++i)
	{
		AgpdFactorCharStatus *pcsFactorSourceStatus	= (AgpdFactorCharStatus *) GetFactor(pcsSourceFactor, (eAgpdFactorsType) i);
		AgpdFactorCharStatus *pcsFactorDestStatus	= (AgpdFactorCharStatus *) GetFactor(pcsDestFactor, (eAgpdFactorsType) i);

		if (pcsFactorSourceStatus && pcsFactorDestStatus)
		{
			if (g_alFactorDetail[i].eFactorType == AGPD_FACTORS_TYPE_NONE)
			{
				pvPacketFactor[i]	= MakePacketFactorDiffer((AgpdFactor *) pcsFactorDestStatus, (AgpdFactor *) pcsFactorSourceStatus, (eAgpdFactorsType) i);
			}
			else
			{
				ApSafeArray<PVOID, 20>	apvPacketTemp;
				apvPacketTemp.MemSetAll();

				INT32 lIndex = 0;
				for (lIndex = 0; lIndex < g_alFactorDetail[i].lNumber; ++lIndex)
				{
					PVOID pvSourceValue = (PVOID) ((CHAR *) pcsFactorSourceStatus + g_nFactorSize[g_alFactorDetail[i].eFactorType] * lIndex);
					PVOID pvDestValue = (PVOID) ((CHAR *) pcsFactorDestStatus + g_nFactorSize[g_alFactorDetail[i].eFactorType] * lIndex);

					apvPacketTemp[lIndex] = MakePacketFactorDiffer((AgpdFactor *) pvDestValue, (AgpdFactor *) pvSourceValue, g_alFactorDetail[i].eFactorType);
				}

				pvPacketFactor[i] = m_acsPacket[i].MakePacket(FALSE, NULL, 0, 
															apvPacketTemp[0],
															apvPacketTemp[1],
															apvPacketTemp[2],
															apvPacketTemp[3],
															apvPacketTemp[4],
															apvPacketTemp[5],
															apvPacketTemp[6],
															apvPacketTemp[7],
															apvPacketTemp[8],
															apvPacketTemp[9],
															apvPacketTemp[10],
															apvPacketTemp[11],
															apvPacketTemp[12],
															apvPacketTemp[13],
															apvPacketTemp[14],
															apvPacketTemp[15],
															apvPacketTemp[16],
															apvPacketTemp[17],
															apvPacketTemp[18],
															apvPacketTemp[19]);

				for (lIndex = 0; lIndex < g_alFactorDetail[i].lNumber; ++lIndex)
				{
					if (apvPacketTemp[lIndex])
						m_acsPacket[g_alFactorDetail[i].eFactorType].FreePacket(apvPacketTemp[lIndex]);
				}
			}
		}
	}

	PVOID	pvPacket = m_csPacket.MakePacket(FALSE, NULL, 0,
													NULL,
													pvPacketFactor[1],
													pvPacketFactor[2],
													pvPacketFactor[3],
													pvPacketFactor[4],
													pvPacketFactor[5],
													pvPacketFactor[6],
													pvPacketFactor[7],
													pvPacketFactor[8],
													pvPacketFactor[9],
													pvPacketFactor[10],
													pvPacketFactor[11],
													pvPacketFactor[12]);

	for (int i = AGPD_FACTORS_TYPE_CHAR_STATUS; i <= AGPD_FACTORS_TYPE_PRICE; ++i)
	{
		if (pvPacketFactor[i])
			m_csPacket.FreePacket(pvPacketFactor[i]);
	}

	return pvPacket;
}

BOOL AgpmFactors::GetAttackableRange( AgpdFactor *pcsSrcFactor, AgpdFactor *pcsDstFactor )
{
	if (!pcsSrcFactor || !pcsDstFactor)
		return 0;

	// attack range를 가져온다.
	AgpdFactor	*pcsFactorResult = (AgpdFactor *) GetFactor(pcsSrcFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return 0;

	AgpdFactorAttack	*pstAttack = (AgpdFactorAttack *) GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_ATTACK);

	INT32	lAttackRange = pstAttack ? pstAttack->lValue[AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE] : 0;

	if (lAttackRange <= 0)
		lAttackRange = 100;

	// hit range를 가져온다.
	pcsFactorResult = (AgpdFactor *) GetFactor(pcsDstFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return 0;

	pstAttack = (AgpdFactorAttack *) GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_ATTACK);

	INT32	lHitRange = pstAttack ? pstAttack->lValue[AGPD_FACTORS_ATTACK_TYPE_HITRANGE] : 0;

	return lAttackRange + lHitRange;
}

//		IsInRange
//	Functions
//		- Destination Position이 Source Position의 Range내에 있는지를 검사
//	Arguments
//		- pstSrcPos		: Source Position
//		- pstDstPos		: Destination Position
//		- pstSrcFactor	: Source Factor
//		- pstDstFactor	: Destination Position
//		- pstCalcPos	: 만약 Range안에 있지 않다면, 가장 가까운 Position
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmFactors::IsInRange(AuPOS *pstSrcPos, AuPOS *pstDstPos, AgpdFactor *pcsSrcFactor, AgpdFactor *pcsDstFactor, AuPOS *pstCalcPos, INT32 *plRange, INT32 *plTargetRange)
{
	if (!pstSrcPos || !pstDstPos || !pcsSrcFactor || !pcsDstFactor)
		return FALSE;

	// attack range를 가져온다.
	AgpdFactor	*pcsFactorResult = (AgpdFactor *) GetFactor(pcsSrcFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorAttack	*pstAttack = (AgpdFactorAttack *) GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_ATTACK);

	INT32	lAttackRange = pstAttack ? pstAttack->lValue[AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE] : 0;

	if (lAttackRange <= 0)
		lAttackRange = 100;

	// hit range를 가져온다.
	pcsFactorResult = (AgpdFactor *) GetFactor(pcsDstFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	pstAttack = (AgpdFactorAttack *) GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_ATTACK);

	INT32	lHitRange = pstAttack ? pstAttack->lValue[AGPD_FACTORS_ATTACK_TYPE_HITRANGE] : 0;

	if (plRange)
		*plRange = lAttackRange + lHitRange;

	return IsInRange(pstSrcPos, pstDstPos, lAttackRange, lHitRange, pstCalcPos, plTargetRange);
}

BOOL AgpmFactors::IsInRange(AuPOS *pstSrcPos, AuPOS *pstDstPos, INT32 lAttackRange, INT32 lHitRange, AuPOS *pstCalcPos, INT32 *plTargetRange)
{
	if (!pstSrcPos || !pstDstPos)
		return FALSE;

	FLOAT				fDistance;
	FLOAT				fTemp;
	FLOAT				fRange = (FLOAT) (lAttackRange + lHitRange);

	FLOAT				fx = pstSrcPos->x - pstDstPos->x;
	FLOAT				fy = pstSrcPos->y - pstDstPos->y;
	FLOAT				fz = pstSrcPos->z - pstDstPos->z;

	fDistance = (FLOAT) sqrt(fx * fx +
							 //fy * fy +
							 fz * fz);

	if (plTargetRange)
		*plTargetRange	= (INT32)	fDistance;

	if (fDistance <= fRange)
		return TRUE;

	if (pstCalcPos)
	{
		fTemp = fRange / (fDistance + 20);	// 1 더해주는 이유? 딱 해당 위치까지만 가지 않고 약간 더 앞으로 가도록

		pstCalcPos->x = pstDstPos->x + fx * fTemp;
		pstCalcPos->y = pstDstPos->y + fy * fTemp;
		pstCalcPos->z = pstDstPos->z + fz * fTemp;
	}

	return FALSE;
}

/*
//		IsInRange
//	Functions
//		- Destination Position이 Source Position의 Range내에 있는지를 검사
//	Arguments
//		- pstSrcPos		: Source Position
//		- pstDstPos		: Destination Position
//		- pstSrcFactor	: Source Factor
//		- pstDstBlock	: Destination의 Blocking Info
//		- pstBlockMatrix	: Destination의 회전 Matrix
//		- pstCalcPos	: 만약 Range안에 있지 않다면, 가장 가까운 Position
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmFactors::IsInRange(AuPOS *pstSrcPos, AuPOS *pstDstPos, AgpdFactor *pcsSrcFactor, AuBLOCKING *pstDstBlock, AuMATRIX *pstBlockMatrix, AuPOS *pstCalcPos)
{
	FLOAT				fRange = 0;

	if (pcsSrcFactor)
	{
		AgpdFactor *pcsFactorResult = (AgpdFactor *) GetFactor(pcsSrcFactor, AGPD_FACTORS_TYPE_RESULT);
		if (!pcsFactorResult)
			return FALSE;

		AgpdFactorAttack *pstAttack = (AgpdFactorAttack *) GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_ATTACK);
		if (pstAttack)
			fRange = pstAttack->lAttackRange;
	}

	return IsInRange(pstSrcPos, pstDstPos, fRange, pstDstBlock, pstBlockMatrix, pstCalcPos);
}

BOOL AgpmFactors::IsInRange(AuPOS *pstSrcPos, AuPOS *pstDstPos, FLOAT fRange, AuBLOCKING *pstDstBlock, AuMATRIX *pstBlockMatrix, AuPOS *pstCalcPos)
{
	FLOAT				fDistance;
	FLOAT				fTemp;

	if (!pstDstBlock || pstDstBlock->type == AUBLOCKING_TYPE_NONE)
	{

		fDistance = (FLOAT) sqrt((pstSrcPos->x - pstDstPos->x) * (pstSrcPos->x - pstDstPos->x) +
								 (pstSrcPos->y - pstDstPos->y) * (pstSrcPos->y - pstDstPos->y) +
								 (pstSrcPos->z - pstDstPos->z) * (pstSrcPos->z - pstDstPos->z));

		if (fDistance <= fRange)
			return TRUE;

		if (pstCalcPos)
		{
			fTemp = fRange / fDistance;

			pstCalcPos->x = pstDstPos->x + (pstSrcPos->x - pstDstPos->x) * fTemp;
			pstCalcPos->y = pstDstPos->y + (pstSrcPos->y - pstDstPos->y) * fTemp;
			pstCalcPos->z = pstDstPos->z + (pstSrcPos->z - pstDstPos->z) * fTemp;
		}

		return FALSE;
	}

	AuBLOCKING	stBlock = *pstDstBlock;
	AuV3D		stSrcPos;
	FLOAT		fDistX, fDistY, fDistZ;

	AuMath::V3DSubtract(&stSrcPos, pstSrcPos, pstDstPos);

	switch (stBlock.type)
	{
	case AUBLOCKING_TYPE_BOX:
		if (pstBlockMatrix)
		{
			AuMATRIX	stInvMatrix;

			AuMath::MatrixInverse(&stInvMatrix, pstBlockMatrix);

			AuMath::V3DTransform(&stBlock.data.box.inf, &pstDstBlock->data.box.inf, &stInvMatrix);
			AuMath::V3DTransform(&stBlock.data.box.sup, &pstDstBlock->data.box.sup, &stInvMatrix);
			AuMath::V3DTransform(&stSrcPos, pstSrcPos, pstBlockMatrix);
		}

		if (stSrcPos.x < stBlock.data.box.inf.x)
			fDistX = stSrcPos.x - stBlock.data.box.inf.x;
		else if (stSrcPos.x > stBlock.data.box.sup.x)
			fDistX = stSrcPos.x - stBlock.data.box.sup.x;
		else
			fDistX = 0;

		if (stSrcPos.y < stBlock.data.box.inf.y)
			fDistY = stSrcPos.y - stBlock.data.box.inf.y;
		else if (stSrcPos.y > stBlock.data.box.sup.y)
			fDistY = stSrcPos.y - stBlock.data.box.sup.y;
		else
			fDistY = 0;

		if (stSrcPos.z < stBlock.data.box.inf.z)
			fDistZ = stSrcPos.z - stBlock.data.box.inf.z;
		else if (stSrcPos.z > stBlock.data.box.sup.z)
			fDistZ = stSrcPos.z - stBlock.data.box.sup.z;
		else
			fDistZ = 0;

		if (!fDistX && !fDistY && !fDistZ)
			fDistance = 0;
		else
		{
			fDistance = (FLOAT) sqrt(fDistX * fDistX + fDistY * fDistY + fDistZ * fDistZ);

			if (fDistance < fRange && pstCalcPos)
			{
				pstCalcPos->x = stSrcPos.x - fDistX;
				pstCalcPos->y = stSrcPos.y - fDistY;
				pstCalcPos->z = stSrcPos.z - fDistZ;

				if (pstBlockMatrix)
					AuMath::V3DTransform(&stSrcPos, pstCalcPos, pstBlockMatrix);
			}
		}

		break;

	case AUBLOCKING_TYPE_SPHERE:
		fDistance = (FLOAT) sqrt((stSrcPos.x - stBlock.data.sphere.center.x) * (stSrcPos.x - stBlock.data.sphere.center.x) +
								 (stSrcPos.y - stBlock.data.sphere.center.y) * (stSrcPos.y - stBlock.data.sphere.center.y) +
								 (stSrcPos.z - stBlock.data.sphere.center.z) * (stSrcPos.z - stBlock.data.sphere.center.z)) - stBlock.data.sphere.radius;


		if (fDistance < fRange && pstCalcPos)
		{
			fTemp = stBlock.data.sphere.radius / (fDistance + stBlock.data.sphere.radius);

			stSrcPos.x = stBlock.data.sphere.center.x + (stSrcPos.x - stBlock.data.sphere.center.x) * fTemp;
			stSrcPos.y = stBlock.data.sphere.center.y + (stSrcPos.y - stBlock.data.sphere.center.y) * fTemp;
			stSrcPos.z = stBlock.data.sphere.center.z + (stSrcPos.z - stBlock.data.sphere.center.z) * fTemp;
		}

		break;

	case AUBLOCKING_TYPE_CYLINDER:
		if (pstBlockMatrix)
		{
			AuMATRIX	stInvMatrix;

			AuMath::MatrixInverse(&stInvMatrix, pstBlockMatrix);

			AuMath::V3DTransform(&stBlock.data.cylinder.center, &pstDstBlock->data.cylinder.center, &stInvMatrix);
			AuMath::V3DTransform(&stSrcPos, pstSrcPos, &stInvMatrix);
		}

		fDistX = (FLOAT) sqrt((stBlock.data.cylinder.center.x - stSrcPos.x) * (stBlock.data.cylinder.center.x - stSrcPos.x) +
							  (stBlock.data.cylinder.center.z - stSrcPos.z) * (stBlock.data.cylinder.center.z - stSrcPos.z));

		if (fDistX <= stBlock.data.cylinder.radius)
			fDistX = 0;
		else
			fDistX = fDistX - stBlock.data.cylinder.radius;

		if (stSrcPos.y > stBlock.data.cylinder.center.y + stBlock.data.cylinder.height)
			fDistY = stSrcPos.y - stBlock.data.cylinder.center.y - stBlock.data.cylinder.height;
		else if (stSrcPos.y < stBlock.data.cylinder.center.y)
			fDistY = stBlock.data.cylinder.center.y - stSrcPos.y;
		else
			fDistY = 0;

		if (!fDistX && !fDistY)
			fDistance = 0;
		else
		{
			fDistance = (FLOAT) sqrt(fDistX * fDistX + fDistY * fDistY);

			if (fDistance < fRange && pstCalcPos)
			{
				fTemp = stBlock.data.cylinder.radius / (fDistX + stBlock.data.cylinder.radius);

				pstCalcPos->x = stBlock.data.cylinder.center.x + (stSrcPos.x - stBlock.data.cylinder.center.x) / fTemp;
				pstCalcPos->y = fDistY;
				pstCalcPos->z = stBlock.data.cylinder.center.x + (stSrcPos.x - stBlock.data.cylinder.center.x) / fTemp;

				if (pstBlockMatrix)
					AuMath::V3DTransform(&stSrcPos, pstCalcPos, pstBlockMatrix);
			}
		}

		break;
	}

	if (fDistance <= fRange)
		return TRUE;

	if (pstCalcPos)
	{
		AuMath::V3DAdd(&stSrcPos, &stSrcPos, pstDstPos);

		fTemp = fRange / (fDistance - fRange);

		pstCalcPos->x = stSrcPos.x + (pstDstPos->x - stSrcPos.x) / fTemp;
		pstCalcPos->y = stSrcPos.y + (pstDstPos->y - stSrcPos.y) / fTemp;
		pstCalcPos->z = stSrcPos.z + (pstDstPos->z - stSrcPos.z) / fTemp;
	}

	return FALSE;
}
*/

//		UpdateFactor
//	Functions
//		- pcsFactor를 pcsUpdateFactor 값으로 변경한다.
//		- 변경후 변경된 데이타에 대한 패킷을 만들어 리턴한다. (리턴받은 곳에서 FreePacket() 해줘야 한다.
//	Arguments
//		- pcsFactor			: 원본 factor
//		- pcsUpdateFactor	: 변경될 값을 담고 있는 factor
//		- bMakePacket		: 패킷 만들지 여부
//	Return value
//		- PVOID : 변경된 항목에 대한 패킷
///////////////////////////////////////////////////////////////////////////////
PVOID AgpmFactors::UpdateFactor(AgpdFactor *pcsFactor, AgpdFactor *pcsUpdateFactor, BOOL bMakePacket)
{
	if (!pcsFactor || !pcsUpdateFactor)
		return NULL;

	// pcsFactor의 이전 값(파티원들간의 싱크 맞춰줄때 쓰이는 값)을 저장해놓는다.
	INT32	lOldLevel		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMovement	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldUnionRank	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMurderer	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMukza		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldHP			= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxHP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMP			= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxMP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldSP			= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxSP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldEXPLow		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldEXPHigh		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxEXPLow	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxEXPHigh	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	AgpdFactor				*pcsFactorResult = (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (pcsFactorResult)
	{
		GetValue(pcsFactorResult, &lOldLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		GetValue(pcsFactorResult, &lOldMovement, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		GetValue(pcsFactorResult, &lOldUnionRank, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_UNION_RANK);
		GetValue(pcsFactorResult, &lOldMurderer, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
		GetValue(pcsFactorResult, &lOldMukza, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA);
		GetValue(pcsFactorResult, &lOldHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		GetValue(pcsFactorResult, &lOldMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
		GetValue(pcsFactorResult, &lOldMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		GetValue(pcsFactorResult, &lOldMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
		GetValue(pcsFactorResult, &lOldSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
		GetValue(pcsFactorResult, &lOldMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);
		GetValue(pcsFactorResult, &lOldEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW);
		GetValue(pcsFactorResult, &lOldEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_HIGH);
		GetValue(pcsFactorResult, &lOldMaxEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW);
		GetValue(pcsFactorResult, &lOldMaxEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_HIGH);
	}
	// pcsFactor의 이전 값(파티원들간의 싱크 맞춰줄때 쓰이는 값)을 저장해놓는다.

	ApSafeArray<PVOID, AGPD_FACTORS_MAX_TYPE>	apvPacket;
	apvPacket.MemSetAll();
	INT32	lIndex;

	for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (GetFactor(pcsUpdateFactor, (eAgpdFactorsType) lIndex))
		{
			if (g_alFactorDetail[lIndex].eFactorType == AGPD_FACTORS_TYPE_RESULT)
				apvPacket[lIndex] = UpdateFactor((AgpdFactor *) SetFactor(pcsFactor, NULL, (eAgpdFactorsType) lIndex),
										(AgpdFactor *) GetFactor(pcsUpdateFactor, (eAgpdFactorsType) lIndex), bMakePacket);
			else
				apvPacket[lIndex] = UpdateFactor(SetFactor(pcsFactor, NULL, (eAgpdFactorsType) lIndex),
										GetFactor(pcsUpdateFactor, (eAgpdFactorsType) lIndex),
										(eAgpdFactorsType) lIndex, bMakePacket);
		}
	}

	PVOID	pvPacket = NULL;

	if (bMakePacket)
	{
		INT16	nPacketLength;
		pvPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0,
												apvPacket[0],
												apvPacket[1],
												apvPacket[2],
												apvPacket[3],
												apvPacket[4],
												apvPacket[5],
												apvPacket[6],
												apvPacket[7],
												apvPacket[8],
												apvPacket[9],
												apvPacket[10],
												apvPacket[11],
												apvPacket[12]);

		for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
		{
			if (apvPacket[lIndex])
				m_csPacket.FreePacket(apvPacket[lIndex]);
		}
	}

	INT32	lNewLevel = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMovement = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewUnionRank = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMurderer = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMukza = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewHP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMaxHP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMaxMP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewSP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMaxSP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewEXPLow = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewEXPHigh = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMaxEXPLow = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMaxEXPHigh = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	pcsFactorResult = (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (pcsFactorResult)
	{
		GetValue(pcsFactorResult, &lNewLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		GetValue(pcsFactorResult, &lNewMovement, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		GetValue(pcsFactorResult, &lNewUnionRank, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_UNION_RANK);
		GetValue(pcsFactorResult, &lNewMurderer, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
		GetValue(pcsFactorResult, &lNewMukza, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA);
		GetValue(pcsFactorResult, &lNewHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		GetValue(pcsFactorResult, &lNewMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
		GetValue(pcsFactorResult, &lNewMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		GetValue(pcsFactorResult, &lNewMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
		GetValue(pcsFactorResult, &lNewSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
		GetValue(pcsFactorResult, &lNewMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);
		GetValue(pcsFactorResult, &lNewEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW);
		GetValue(pcsFactorResult, &lNewEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_HIGH);
		GetValue(pcsFactorResult, &lNewMaxEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW);
		GetValue(pcsFactorResult, &lNewMaxEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_HIGH);
	}

	if (lOldHP != lNewHP)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= IntToPtr(lOldHP);
		pvBuffer[1]	= IntToPtr(0);

		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_HP, pcsFactor, pvBuffer);
	}

	if (lOldMP != lNewMP)
	{
		PVOID	pvBuffer[2];
		pvBuffer[0]	= IntToPtr(lOldMP);
		pvBuffer[1]	= IntToPtr(0);

		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_MP, pcsFactor, pvBuffer);
	}

	if (lOldLevel != lNewLevel ||
		lOldMovement != lNewMovement ||
		lOldUnionRank != lNewUnionRank ||
		lOldMurderer != lNewMurderer ||
		lOldMukza != lNewMukza)
	{
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_VIEW, pcsFactor, NULL);
	}

	if (lOldLevel != lNewLevel ||
		lOldHP != lNewHP ||
		lOldMaxHP != lNewMaxHP ||
		lOldMP != lNewMP ||
		lOldMaxMP != lNewMaxMP ||
		lOldSP != lNewSP ||
		lOldMaxSP != lNewMaxSP ||
		lOldEXPLow != lNewEXPLow ||
		lOldEXPHigh != lNewEXPHigh ||
		lOldMaxEXPLow != lNewMaxEXPLow ||
		lOldMaxEXPHigh != lNewMaxEXPHigh)
	{
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_UI, pcsFactor, NULL);
	}

	if (lOldMovement != lNewMovement)
	{
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_MOVEMENT, pcsFactor, NULL);
	}

	if (lOldHP != lNewHP || lOldMaxHP != lNewMaxHP)
	{
		EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_PARTY, pcsFactor, NULL);
	}

	return pvPacket;
}

PVOID AgpmFactors::UpdateFactor(PVOID pvFactor, PVOID pvUpdateFactor, eAgpdFactorsType eType, BOOL bMakePacket)
{
	if (!pvFactor || !pvUpdateFactor || eType <= 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return NULL;

	INT32	lFactorNumber = g_alFactorDetail[eType].lNumber;

	ApSafeArray<INT32 *, 20>	aplValue;
	aplValue.MemSetAll();
	ApSafeArray<PVOID, 20>	apvValue;
	apvValue.MemSetAll();

	INT32	lIndex;
	INT32 *	plUpdateFactor = (INT32 *) pvUpdateFactor;
	INT32 *	plFactor = (INT32 *) pvFactor;

	PVOID	pvFactorTemp;
	PVOID	pvUpdateFactorTemp;

	INT16	nPacketLength;
	PVOID	pvPacket = NULL;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		for (lIndex = 0; lIndex < lFactorNumber; ++lIndex)
		{
			if (plUpdateFactor[lIndex] != AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE && plFactor[lIndex] != plUpdateFactor[lIndex])
			{
				plFactor[lIndex] = plUpdateFactor[lIndex];
				aplValue[lIndex] = &plFactor[lIndex];
			}
		}

		if (bMakePacket)
		{
			pvPacket = m_acsPacket[eType].MakePacket(FALSE, &nPacketLength, 0, 
													aplValue[0],
													aplValue[1],
													aplValue[2],
													aplValue[3],
													aplValue[4],
													aplValue[5],
													aplValue[6],
													aplValue[7],
													aplValue[8],
													aplValue[9],
													aplValue[10],
													aplValue[11],
													aplValue[12],
													aplValue[13],
													aplValue[14],
													aplValue[15],
													aplValue[16],
													aplValue[17],
													aplValue[18],
													aplValue[19]);
		}
	}
	else
	{
		for (lIndex = 0; lIndex < lFactorNumber; ++lIndex)
		{
			pvFactorTemp = (PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);
			pvUpdateFactorTemp = (PVOID) ((CHAR *) pvUpdateFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

			apvValue[lIndex] = UpdateFactor(pvFactorTemp, pvUpdateFactorTemp, g_alFactorDetail[eType].eFactorType, bMakePacket);
		}

		if (bMakePacket)
		{
			pvPacket = m_acsPacket[eType].MakePacket(FALSE, &nPacketLength, 0,
													apvValue[0],
													apvValue[1],
													apvValue[2],
													apvValue[3],
													apvValue[4],
													apvValue[5],
													apvValue[6],
													apvValue[7],
													apvValue[8],
													apvValue[9],
													apvValue[10],
													apvValue[11],
													apvValue[12],
													apvValue[13],
													apvValue[14],
													apvValue[15],
													apvValue[16],
													apvValue[17],
													apvValue[18],
													apvValue[19]);
		}
	}

	for (lIndex = 0; lIndex < lFactorNumber; ++lIndex)
	{
		if (apvValue[lIndex])
			m_acsPacket[lIndex].FreePacket(apvValue[lIndex]);
	}

	return pvPacket;
}

PVOID AgpmFactors::CalcFactor(AgpdFactor *pcsFactor, AgpdFactor *pcsUpdateFactor, BOOL bZero, BOOL bMakePacket, BOOL bAdd, BOOL bCheckUpdate)
{
	if (!pcsFactor || !pcsUpdateFactor)
		return NULL;

	// pcsFactor의 이전 값(파티원들간의 싱크 맞춰줄때 쓰이는 값)을 저장해놓는다.
	INT32	lOldLevel		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldHP			= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxHP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMP			= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxMP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldSP			= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxSP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldEXPLow		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldEXPHigh		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxEXPLow	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lOldMaxEXPHigh	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	AgpdFactor	*pcsFactorResult = NULL;

	if (bCheckUpdate)
	{
		if (GetFactor(pcsFactor, AGPD_FACTORS_TYPE_OWNER))
		{
			pcsFactorResult = (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);

			if (pcsFactorResult)
			{
				GetValue(pcsFactorResult, &lOldLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
				GetValue(pcsFactorResult, &lOldHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
				GetValue(pcsFactorResult, &lOldMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
				GetValue(pcsFactorResult, &lOldMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
				GetValue(pcsFactorResult, &lOldMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
				GetValue(pcsFactorResult, &lOldSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
				GetValue(pcsFactorResult, &lOldMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);
				GetValue(pcsFactorResult, &lOldEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW);
				GetValue(pcsFactorResult, &lOldEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_HIGH);
				GetValue(pcsFactorResult, &lOldMaxEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW);
				GetValue(pcsFactorResult, &lOldMaxEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_HIGH);
			}
			// pcsFactor의 이전 값(파티원들간의 싱크 맞춰줄때 쓰이는 값)을 저장해놓는다.
		}
	}

	ApSafeArray<PVOID, AGPD_FACTORS_MAX_TYPE>	apvPacket;
	apvPacket.MemSetAll();

	INT32	lIndex;

	for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (GetFactor(pcsUpdateFactor, (eAgpdFactorsType) lIndex))
		{
			if (!GetFactor(pcsFactor, (eAgpdFactorsType) lIndex))
				SetFactor(pcsFactor, NULL, (eAgpdFactorsType) lIndex);

			if (g_alFactorDetail[lIndex].eFactorType == AGPD_FACTORS_TYPE_RESULT)
			{
				apvPacket[lIndex] = CalcFactor((AgpdFactor *) GetFactor(pcsFactor, (eAgpdFactorsType) lIndex),
												(AgpdFactor *) GetFactor(pcsUpdateFactor, (eAgpdFactorsType) lIndex), bZero, bMakePacket, bAdd);
			}
			else
			{
				apvPacket[lIndex] = CalcFactor(GetFactor(pcsFactor, (eAgpdFactorsType) lIndex),
												GetFactor(pcsUpdateFactor, (eAgpdFactorsType) lIndex), (eAgpdFactorsType) lIndex, bZero, bMakePacket, bAdd);
			}
		}
	}

	PVOID	pvPacket = NULL;

	if (bMakePacket)
	{
		INT16	nPacketLength;
		pvPacket = m_csPacket.MakePacket(FALSE, &nPacketLength, 0, 
												apvPacket[0],
												apvPacket[1],
												apvPacket[2],
												apvPacket[3],
												apvPacket[4],
												apvPacket[5],
												apvPacket[6],
												apvPacket[7],
												apvPacket[8],
												apvPacket[9],
												apvPacket[10],
												apvPacket[11],
												apvPacket[12]);

		for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
		{
			m_acsPacket[lIndex].FreePacket(apvPacket[lIndex]);
		}
	}

	if (bCheckUpdate)
	{
		INT32	lNewLevel = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		INT32	lNewHP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		INT32	lNewMaxHP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		INT32	lNewMP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		INT32	lNewMaxMP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		INT32	lNewSP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		INT32	lNewMaxSP = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		INT32	lNewEXPLow = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		INT32	lNewEXPHigh = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		INT32	lNewMaxEXPLow = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		INT32	lNewMaxEXPHigh = AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

		if (GetFactor(pcsFactor, AGPD_FACTORS_TYPE_OWNER))
		{
			pcsFactorResult = (AgpdFactor *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
			if (pcsFactorResult)
			{
				GetValue(pcsFactorResult, &lNewLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
				GetValue(pcsFactorResult, &lNewHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
				GetValue(pcsFactorResult, &lNewMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
				GetValue(pcsFactorResult, &lNewMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
				GetValue(pcsFactorResult, &lNewMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
				GetValue(pcsFactorResult, &lNewSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
				GetValue(pcsFactorResult, &lNewMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);
				GetValue(pcsFactorResult, &lNewEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW);
				GetValue(pcsFactorResult, &lNewEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_HIGH);
				GetValue(pcsFactorResult, &lNewMaxEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW);
				GetValue(pcsFactorResult, &lNewMaxEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_HIGH);
			}

			if (lOldLevel != lNewLevel || lOldHP != lNewHP || lOldMaxHP != lNewMaxHP)
			{
				EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_PARTY, pcsFactor, NULL);
				EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_UI, pcsFactor, NULL);
			}
			else if ( lOldMP != lNewMP || lOldMaxMP != lNewMaxMP || lOldSP != lNewSP || lOldMaxSP != lNewMaxSP ||
				lOldEXPHigh != lNewEXPHigh || lOldMaxEXPHigh != lNewMaxEXPHigh ||
				lOldEXPLow != lNewEXPLow || lOldMaxEXPLow != lNewMaxEXPLow )
			{
				EnumCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_UI, pcsFactor, NULL);
			}
		}
	}

	return pvPacket;
}

PVOID AgpmFactors::CalcFactor(PVOID pvFactor, PVOID pvUpdateFactor, eAgpdFactorsType eType, BOOL bZero, BOOL bMakePacket, BOOL bAdd)
{
	if (!pvFactor || !pvUpdateFactor || eType <= 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return NULL;

	INT32	lFactorNum = g_alFactorDetail[eType].lNumber;
	ApSafeArray<INT32 *, 20>	aplValue;
	aplValue.MemSetAll();
	INT32 *	plFactor = (INT32 *) pvFactor;
	INT32 *	plUpdateFactor = (INT32 *) pvUpdateFactor;
	PVOID	pvPacket = NULL;

	ApSafeArray<PVOID, 20>	apvValue;
	apvValue.MemSetAll();

	INT32	lIndex;
	PVOID	pvFactorTemp;
	PVOID	pvUpdateFactorTemp;
	INT16	nPacketLength;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		for (lIndex = 0; lIndex < lFactorNum; ++lIndex)
		{
			if ((eType == AGPD_FACTORS_TYPE_CHAR_POINT && lIndex == AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW) ||
				(eType == AGPD_FACTORS_TYPE_CHAR_POINT_MAX && lIndex == AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW))
			{
				INT64	llExp	= 0;
				CopyMemory(&llExp, &plUpdateFactor[lIndex + 1], sizeof(INT32));
				CopyMemory(((CHAR *) &llExp) + sizeof(INT32), &plUpdateFactor[lIndex], sizeof(INT32));

				INT64	llCalcExp	= 0;
				CopyMemory(&llCalcExp, &plFactor[lIndex + 1], sizeof(INT32));
				CopyMemory(((CHAR *) &llCalcExp) + sizeof(INT32), &plFactor[lIndex], sizeof(INT32));

				if (bAdd)
					llCalcExp += llExp;
				else
					llCalcExp -= llExp;

				//if (bZero && llCalcExp < 0)
				//	llCalcExp = 0;

				/*  plFactor를 Update한 후에 aplValue에다가 넣고 Packet을 만들어야 되는데, 걍 aplValue에다가 값을 때려넣으면 어쩐다냐. Parn (광주니 한턱 쏴!!!)
				CopyMemory(&plFactor[lIndex + 1], &llCalcExp, sizeof(INT32));
				CopyMemory(&plFactor[lIndex], ((CHAR *) &llCalcExp) + sizeof(INT32), sizeof(INT32));
				*/

				CopyMemory(&plFactor[lIndex + 1], &llCalcExp, sizeof(INT32));
				CopyMemory(&plFactor[lIndex], ((CHAR *) &llCalcExp) + sizeof(INT32), sizeof(INT32));

				aplValue[lIndex + 1] = &plFactor[lIndex + 1];
				aplValue[lIndex] = &plFactor[lIndex];

				++lIndex;
			}
			else if (plUpdateFactor[lIndex] != 0)
			{
				if (bAdd)
					plFactor[lIndex] += plUpdateFactor[lIndex];
				else
					plFactor[lIndex] -= plUpdateFactor[lIndex];

				if (bZero && plFactor[lIndex] < 0)
					plFactor[lIndex] = 0;

				aplValue[lIndex] = &plFactor[lIndex];
			}
		}

		if (bMakePacket)
		{
			pvPacket = m_acsPacket[eType].MakePacket(FALSE, &nPacketLength, 0, 
																	aplValue[0],
																	aplValue[1],
																	aplValue[2],
																	aplValue[3],
																	aplValue[4],
																	aplValue[5],
																	aplValue[6],
																	aplValue[7],
																	aplValue[8],
																	aplValue[9],
																	aplValue[10],
																	aplValue[11],
																	aplValue[12],
																	aplValue[13],
																	aplValue[14],
																	aplValue[15],
																	aplValue[16],
																	aplValue[17],
																	aplValue[18],
																	aplValue[19]);
		}
	}
	else
	{
		for (lIndex = 0; lIndex < lFactorNum; ++lIndex)
		{
			pvFactorTemp = (PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);
			pvUpdateFactorTemp = (PVOID) ((CHAR *) pvUpdateFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

			apvValue[lIndex] = CalcFactor(pvFactorTemp, pvUpdateFactorTemp, g_alFactorDetail[eType].eFactorType, bZero, bMakePacket, bAdd);
		}

		if (bMakePacket)
		{
			pvPacket = m_acsPacket[eType].MakePacket(FALSE, &nPacketLength, 0,
													apvValue[0],
													apvValue[1],
													apvValue[2],
													apvValue[3],
													apvValue[4],
													apvValue[5],
													apvValue[6],
													apvValue[7],
													apvValue[8],
													apvValue[9],
													apvValue[10],
													apvValue[11],
													apvValue[12],
													apvValue[13],
													apvValue[14],
													apvValue[15],
													apvValue[16],
													apvValue[17],
													apvValue[18],
													apvValue[19]);
													
		}
	}

	for (lIndex = 0; lIndex < lFactorNum; ++lIndex)
	{
		if (apvValue[lIndex])
			m_acsPacket[lIndex].FreePacket(apvValue[lIndex]);
	}

	return pvPacket;
}

PVOID AgpmFactors::InitUpdateFactor(PVOID pvFactor, eAgpdFactorsType eType)
{
	INT32	lIndex;
	INT32 *	plValue = (INT32 *) pvFactor;
	PVOID	pvValue;

	if (!pvFactor)
	{
		return NULL;
	}

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			plValue[lIndex]	= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
		}
	}
	else
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			pvValue = (PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

			InitUpdateFactor(pvValue, g_alFactorDetail[eType].eFactorType);
		}
	}


	return pvFactor;
}

PVOID AgpmFactors::InitUpdateFactor(AgpdFactor *pcsFactor, eAgpdFactorsType eType)
{
	if (!pcsFactor)
		return NULL;

	InitFactor(pcsFactor);

	if (!SetFactor(pcsFactor, NULL, eType))
		return NULL;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_RESULT)
	{
		INT32	lIndex;

		AgpdFactor *pcsFactorTemp = (AgpdFactor *) GetFactor(pcsFactor, eType);

		for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
		{
			if (g_alFactorDetail[eType].eFactorType != AGPD_FACTORS_TYPE_RESULT)
				InitUpdateFactor(pcsFactorTemp, (eAgpdFactorsType) lIndex);
		}

		return GetFactor(pcsFactor, eType);
	}
	else
	{
		return InitUpdateFactor(GetFactor(pcsFactor, eType), eType);
	}

	return NULL;
}

PVOID AgpmFactors::InitCalcFactor(AgpdFactor *pcsFactor, eAgpdFactorsType eType)
{
	if (!pcsFactor)
		return NULL;

	InitFactor(pcsFactor);

	PVOID	pvFactor = SetFactor(pcsFactor, NULL, eType);

	return pvFactor;
}

BOOL AgpmFactors::StreamRead(AgpdFactor *pcsFactor, ApModuleStream *pStream)
{
	if (!pcsFactor || !pStream)
		return FALSE;

	CHAR *	szValueName;
	INT32	lIndex;

	pStream->ReadNextValue();
	do {
		szValueName = (CHAR *) pStream->GetValueName();

		for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
		{
			if (g_aszFactorsININame[lIndex][0] && !strcmp(szValueName, g_aszFactorsININame[lIndex][0]))
			{
				if (!pStream->ReadNextValue())
					return FALSE;

				if (!StreamReadFactor(&pcsFactor->m_pvFactor[lIndex], (eAgpdFactorsType) lIndex, pStream))
					return FALSE;

				break;
			}
		}
	} while (strcmp(szValueName, AGPM_FACTORS_INI_END) != 0 && pStream->ReadNextValue());	// AGPM_FACTORS_INI_END가 나올때까지 계속한다.

	return TRUE;
}

// Luk 추가(010803, Bob)
BOOL AgpmFactors::StreamReadFactor(PVOID *ppvFactor, eAgpdFactorsType eType, ApModuleStream *pStream, BOOL bEmbedded)
{
	if (!pStream || eType <= 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return FALSE;

	INT32	lIndex;
	CHAR	*szValueName;
	CHAR	szValue[128];
	INT32 *	plValue;
	PVOID	pvValue;
	szValueName = (CHAR *) pStream->GetValueName();

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		while (TRUE)
		{
			// 이름과 값을 가져온다.
			szValueName = (CHAR *) pStream->GetValueName();
			pStream->GetValue(szValue, 128);

			for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
			{
				if (!g_aszFactorININame[eType][lIndex])
					continue;

				if (!strcmp(szValueName, g_aszFactorININame[eType][lIndex]))
				{
					SetFactor(ppvFactor, NULL, eType);
					if (!*ppvFactor)
						return FALSE;

					plValue = (INT32 *) *ppvFactor;

					if (g_bFactorUseFloat[eType][lIndex])
						plValue[lIndex] = (INT32) (atof(szValue) * 100);
					else
						plValue[lIndex] = atoi(szValue);

					break;
				}
			}

			if (lIndex == g_alFactorDetail[eType].lNumber)
				break;

			if (!pStream->ReadNextValue())
				break;
		}// while (strcmp(szValueName, aszFactorsININame[eType][1]) != 0 && pStream->ReadNextValue());
	}
	else
	{
		while (TRUE)
		{
			// 이름과 값을 가져온다.
			szValueName = (CHAR *) pStream->GetValueName();
			pStream->GetValue(szValue, 128);

			for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
			{
				if (!g_aszFactorININame[eType][lIndex])
					continue;

				if (!strcmp(szValueName, g_aszFactorININame[eType][lIndex]))
				{
					if (!pStream->ReadNextValue())
						return FALSE;

					SetFactor(ppvFactor, NULL, eType);
					if (!*ppvFactor)
						return FALSE;

					pvValue = (PVOID) ((CHAR *) *ppvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

					if (!StreamReadFactor(&pvValue, g_alFactorDetail[eType].eFactorType, pStream, TRUE))
						return FALSE;

					break;
				}
			}

			if (lIndex == g_alFactorDetail[eType].lNumber)
				break;

			if (!pStream->ReadNextValue())
				break;
		}// while (strcmp(szValueName, aszFactorsININame[eType][1]) != 0 && pStream->ReadNextValue());
	}
		
	return TRUE;
}

BOOL AgpmFactors::StreamWrite(AgpdFactor *pcsFactor, ApModuleStream *pStream)
{
	if (!pcsFactor || !pStream)
		return FALSE;

	// 시작이란걸 표시한다.
	if (!pStream->WriteValue(AGPM_FACTORS_INI_START, 0))
		return FALSE;

	// 하나 하나 스트림에 기록한다.

	INT32	lIndex;

	for (lIndex = 1; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (!StreamWriteFactor(GetFactor(pcsFactor, (eAgpdFactorsType) lIndex), (eAgpdFactorsType) lIndex, pStream))
			return FALSE;
	}

	// 여기까지가 끝이라는걸 표시한다. 오줌으로 영역표시한다.
	if (!pStream->WriteValue(AGPM_FACTORS_INI_END, 0))
		return FALSE;

	return TRUE;
}

BOOL AgpmFactors::StreamWriteFactor(PVOID pvFactor, eAgpdFactorsType eType, ApModuleStream *pStream, BOOL bEmbedded)
{
	if (!pStream || eType <= 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return FALSE;

	if (!pvFactor)
		return TRUE;

	INT32	lIndex;
	INT32 *	plValue = (INT32 *) pvFactor;
	PVOID	pvValue = pvFactor;

	if (!bEmbedded && g_aszFactorsININame[eType][0])
	{
		if (!pStream->WriteValue(g_aszFactorsININame[eType][0], 0))
			return FALSE;
	}

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			if (g_aszFactorININame[eType][lIndex])
			{
				if (g_bFactorUseFloat[eType][lIndex])
				{
					if (!pStream->WriteValue(g_aszFactorININame[eType][lIndex], (FLOAT) (plValue[lIndex] / 100.0f)))
						return FALSE;
				}
				else
				{
					if (!pStream->WriteValue(g_aszFactorININame[eType][lIndex], plValue[lIndex]))
						return FALSE;
				}
			}
		}
	}
	else
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			pvValue = (PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

			if (!pStream->WriteValue(g_aszFactorININame[eType][lIndex], 0))
				return FALSE;

			if (!StreamWriteFactor(pvValue, g_alFactorDetail[eType].eFactorType, pStream, TRUE))
				return FALSE;

			if (!pStream->WriteValue(g_aszFactorININame[eType][lIndex], 0))
				return FALSE;
		}
	}

	if (!bEmbedded && g_aszFactorsININame[eType][1])
	{
		if (!pStream->WriteValue(g_aszFactorsININame[eType][1], 0))
			return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 072403 Bob Jung.
******************************************************************************/
INT32 AgpmFactors::FindCharacterRaceIndex(CHAR *szRace)
{
	if(!m_csAgpmFactorCharTypeNameSet.m_lRaceNum)
		return -1;

	for(INT32 lCount = 0; lCount < m_csAgpmFactorCharTypeNameSet.m_lRaceNum; ++lCount)
	{
		if(!strcmp(m_csAgpmFactorCharTypeNameSet.m_csRace[lCount].m_szName, szRace))
			return m_csAgpmFactorCharTypeNameSet.m_csRace[lCount].m_lIndex;
	}

	return -1;
}

/******************************************************************************
* Purpose :
*
* 072403 Bob Jung.
******************************************************************************/
INT32 AgpmFactors::FindCharacterClassIndex(CHAR *szClass)
{
	if(!m_csAgpmFactorCharTypeNameSet.m_lRaceNum)
		return -1;

	if(!m_csAgpmFactorCharTypeNameSet.m_lClassNum)
		return -1;

	for(INT32 lClassIndex = 0; lClassIndex < m_csAgpmFactorCharTypeNameSet.m_lClassNum; ++lClassIndex)
	{
		for(INT32 lRaceIndex = 0; lRaceIndex < m_csAgpmFactorCharTypeNameSet.m_lRaceNum; ++lRaceIndex)
		{
			if(!strcmp(m_csAgpmFactorCharTypeNameSet.m_csClass[lRaceIndex][lClassIndex].m_szName, szClass))
				return m_csAgpmFactorCharTypeNameSet.m_csClass[lRaceIndex][lClassIndex].m_lIndex;
		}
	}

	return -1;
}

/******************************************************************************
* Purpose :
*
* 072403 Bob Jung.
******************************************************************************/
INT32 AgpmFactors::FindCharacterClassIndex(CHAR *szRace, CHAR *szClass)
{
	INT32 lRace = FindCharacterRaceIndex(szRace);
	if(lRace < 0)
		return -1;

	if(!m_csAgpmFactorCharTypeNameSet.m_lClassNum)
		return -1;

	for(INT32 lCount = 0; lCount < m_csAgpmFactorCharTypeNameSet.m_lClassNum; ++lCount)
	{
		if(!strcmp(m_csAgpmFactorCharTypeNameSet.m_csClass[lRace][lCount].m_szName, szClass))
			return m_csAgpmFactorCharTypeNameSet.m_csClass[lRace][lCount].m_lIndex;
	}

	return -1;
}

CHAR* AgpmFactors::GetCharacterRaceName(INT32 lRace)
{
	if(m_csAgpmFactorCharTypeNameSet.m_lRaceNum < 1 || m_csAgpmFactorCharTypeNameSet.m_lRaceNum <= lRace)
		return NULL;

	for(INT32 lCount = 0; lCount < m_csAgpmFactorCharTypeNameSet.m_lRaceNum; ++lCount)
	{
		if (m_csAgpmFactorCharTypeNameSet.m_csRace[lCount].m_lIndex == lRace)
			return m_csAgpmFactorCharTypeNameSet.m_csRace[lCount].m_szName;
	}

	return NULL;
}

CHAR* AgpmFactors::GetCharacterClassName(INT32 lRace, INT32 lClass)
{
	if (m_csAgpmFactorCharTypeNameSet.m_lRaceNum < 1 || m_csAgpmFactorCharTypeNameSet.m_lRaceNum < lRace ||
		m_csAgpmFactorCharTypeNameSet.m_lClassNum < 1 || m_csAgpmFactorCharTypeNameSet.m_lClassNum < lClass)
		return NULL;

	INT32 i = 0, lCount = 0;
	for(i = 0; i < m_csAgpmFactorCharTypeNameSet.m_lRaceNum; ++i)
	{
		if (m_csAgpmFactorCharTypeNameSet.m_csRace[i].m_lIndex == lRace)
			break;
	}

	if (i == m_csAgpmFactorCharTypeNameSet.m_lRaceNum)
		return NULL;

	for(lCount = 0; lCount < m_csAgpmFactorCharTypeNameSet.m_lClassNum; ++lCount)
	{
		if (m_csAgpmFactorCharTypeNameSet.m_csClass[i][lCount].m_lIndex == lClass)
			return m_csAgpmFactorCharTypeNameSet.m_csClass[i][lCount].m_szName;
	}

	return NULL;
}

CHAR* AgpmFactors::GetCharacterGenderName(INT32 lGender)
{
	if (m_csAgpmFactorCharTypeNameSet.m_lGenderNum < 1)
		return NULL;

	for(INT32 lCount = 0; lCount < m_csAgpmFactorCharTypeNameSet.m_lRaceNum; ++lCount)
	{
		if (m_csAgpmFactorCharTypeNameSet.m_csGender[lCount].m_lIndex == lGender)
			return m_csAgpmFactorCharTypeNameSet.m_csGender[lCount].m_szName;
	}

	return NULL;
}

/******************************************************************************
* Purpose :
*
* 010903 Bob Jung.
******************************************************************************/
BOOL AgpmFactors::ParseCharacterTypeClass(CHAR *szValue, AgpmFactorCharTypeNameSet* pSet, INT16 nRaceNum, INT16 nClassIndex)
{
	CHAR	szTemp[AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_LENGTH];

	INT32	lLen			= (INT32)strlen(szValue);
	INT32	lProcess		= 0;
	INT32	lTempProcess	= 0;
	INT32	lIndex			= atoi(szValue);

	for(;lProcess < lLen; ++lProcess)
	{
		if(szValue[lProcess] == ':')
		{
			++lProcess;
			break;
		}
	}

	for(INT32 lRaceIndex = 0; lRaceIndex < nRaceNum; ++lRaceIndex)
	{
		for(lTempProcess = 0; ; ++lProcess, ++lTempProcess)
		{
			if(lProcess >= lLen)
				break;

			if(szValue[lProcess] == ':')
			{
				szTemp[lTempProcess] = '\0';
				strcpy(pSet->m_csClass[lRaceIndex][nClassIndex].m_szName, szTemp);

				pSet->m_csClass[lRaceIndex][nClassIndex].m_lIndex = lIndex;

				++lProcess;
				break;
			}

			szTemp[lTempProcess] = szValue[lProcess];
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 010803 Bob Jung.
******************************************************************************/
BOOL AgpmFactors::CharacterTypeStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpmFactorCharTypeNameSet	*pSet	= (AgpmFactorCharTypeNameSet *)(pData);
	AgpmFactors					*pThis	= (AgpmFactors *)(pClass);

	const CHAR	*szValueName;
	CHAR		szValue[AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_LENGTH];
	INT32		lRaceNum	= 0;
	INT32		lClassNum	= 0;
	INT32		lGenderNum	= 0;

	while(pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		// 같은게 있으면 마구마구 넣는다~
		if(!strcmp(szValueName, AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_RACE))
		{
			pStream->GetValue(szValue, AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_LENGTH);
			sscanf(szValue, "%d:%s", &pSet->m_csRace[lRaceNum].m_lIndex, pSet->m_csRace[lRaceNum].m_szName);
			++lRaceNum;
		}
		else if(!strcmp(szValueName, AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_CLASS))
		{
			//pStream->GetValue(pSet->m_stClass[nClassNum++].m_szName, AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_LENGTH);
			pStream->GetValue(szValue, AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_LENGTH);

			if(!pThis->ParseCharacterTypeClass(szValue, pSet, lRaceNum, lClassNum))
			{
				OutputDebugString("AgpmFactors::CharacterTypeStreamReadCB() Error (1) !!!\n");
				return FALSE;
			}

			++lClassNum;
		}
		else if(!strcmp(szValueName, AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_SET_GENDER))
		{
			pStream->GetValue(szValue, AGPM_FACTORS_TYPE_CHAR_STATUS_NAME_LENGTH);
			sscanf(szValue, "%d:%s", &pSet->m_csGender[lGenderNum].m_lIndex, pSet->m_csGender[lGenderNum].m_szName);
			++lGenderNum;
		}
	}

	pSet->m_lRaceNum	= lRaceNum;
	pSet->m_lClassNum	= lClassNum;
	pSet->m_lGenderNum	= lGenderNum;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 010803 Bob Jung.
******************************************************************************/
BOOL AgpmFactors::CharacterTypeStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{ // 이 녀석은 하는 일이 없다.. -_-;;;
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 010803 Bob Jung.
******************************************************************************/
BOOL AgpmFactors::CharacterTypeStreamRead(CHAR *szFile, BOOL bDecryption)
{
	ApModuleStream	csStream;
	UINT16			nNumKeys;
	CHAR			szSectionName[32];

	csStream.Open(szFile, 0, bDecryption);
	nNumKeys = csStream.GetNumSections();

	for(INT16 nCount = 0; nCount < nNumKeys; ++nCount)
	{
		strcpy(szSectionName, csStream.ReadSectionName(nCount));

		if(!csStream.EnumReadCallback(AGPM_FACTORS_STREAM_TYPE_CHARTYPE_NAME, &m_csAgpmFactorCharTypeNameSet, this))
			return FALSE;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 010803 Bob Jung.
******************************************************************************/
BOOL AgpmFactors::CharacterTypeStreamWrite(CHAR *szFile, BOOL bEncryption)
{ // 이 녀석은 하는 일이 없다.. -_-;;;
	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 060903 Bob Jung.
******************************************************************************/
CHAR *AgpmFactors::GetFactorTypeName(INT32 lIndex)
{
	if((lIndex < 0) || (lIndex >= AGPD_FACTORS_MAX_TYPE))
		return NULL;

	return g_aszFactorsTypeName[lIndex];
}

/******************************************************************************
* Purpose :
*
* 060903 Bob Jung.
******************************************************************************/
CHAR *AgpmFactors::GetFactorININame(INT32 lIndex, INT32 lSubIndex)
{
	if((lIndex < 0) || (lIndex >= AGPD_FACTORS_MAX_TYPE))
		return NULL;

	return g_aszFactorININame[lIndex][lSubIndex];
}

/******************************************************************************
* Purpose :
*
* 060903 Bob Jung.
******************************************************************************/
AgpdFactorDetail *AgpmFactors::GetFactorDetail(INT32 lIndex)
{
	if((lIndex < 0) || (lIndex >= AGPD_FACTORS_MAX_TYPE))
		return NULL;

	return &g_alFactorDetail[lIndex];
}

BOOL AgpmFactors::CopyFactorNotNull(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor)
{
	if (!pcsDestFactor || !pcsSourceFactor)
		return FALSE;

	for (int i = 0; i < AGPD_FACTORS_MAX_TYPE; ++i)
	{
		if (pcsSourceFactor->m_pvFactor[i])
			CopyFactor(pcsDestFactor, pcsSourceFactor, TRUE, FALSE, (eAgpdFactorsType) i);
	}

	return TRUE;
}

BOOL AgpmFactors::CopyFactorNotZero(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor)
{
	if (!pcsDestFactor || !pcsSourceFactor)
		return FALSE;

	for (INT32 lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (GetFactor(pcsSourceFactor, (eAgpdFactorsType) lIndex))
		{
			if (g_alFactorDetail[lIndex].eFactorType == AGPD_FACTORS_TYPE_RESULT)
				CopyFactorNotZero((AgpdFactor *) SetFactor(pcsDestFactor, NULL, (eAgpdFactorsType) lIndex),
									(AgpdFactor *) GetFactor(pcsSourceFactor, (eAgpdFactorsType) lIndex));
			else
				CopyFactorNotZero(SetFactor(pcsDestFactor, NULL, (eAgpdFactorsType) lIndex),
									GetFactor(pcsSourceFactor, (eAgpdFactorsType) lIndex),
									(eAgpdFactorsType) lIndex);
		}
	}

	return TRUE;
}

BOOL AgpmFactors::CopyFactorNotZero(PVOID pcsDestFactor, PVOID pcsSourceFactor, eAgpdFactorsType eType)
{
	if (!pcsDestFactor || !pcsSourceFactor || eType <= 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return FALSE;

	INT32	lFactorNumber = g_alFactorDetail[eType].lNumber;
	INT32	lIndex;
	INT32 *	plUpdateFactor = (INT32 *) pcsSourceFactor;
	INT32 *	plFactor = (INT32 *) pcsDestFactor;

	PVOID	pvFactorTemp;
	PVOID	pvUpdateFactorTemp;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		for (lIndex = 0; lIndex < lFactorNumber; ++lIndex)
		{
			if (plUpdateFactor[lIndex] != 0)
			{
				plFactor[lIndex] = plUpdateFactor[lIndex];
			}
		}
	}
	else
	{
		for (lIndex = 0; lIndex < lFactorNumber; ++lIndex)
		{
			pvFactorTemp = (PVOID) ((CHAR *) pcsDestFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);
			pvUpdateFactorTemp = (PVOID) ((CHAR *) pcsSourceFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

			CopyFactorNotZero(pvFactorTemp, pvUpdateFactorTemp, g_alFactorDetail[eType].eFactorType);
		}
	}

	return TRUE;
}

BOOL AgpmFactors::CopyFactor(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor, BOOL bCopyCharPoint, BOOL bCopyResult, eAgpdFactorsType eType, BOOL bOverwrite)
{
	if (!pcsDestFactor || !pcsSourceFactor)
		return FALSE;

	if (!bOverwrite)
	{
		if (eType == AGPD_FACTORS_TYPE_NONE)
		{
			if (!InitFactor(pcsDestFactor))
				return FALSE;
		}
		else
		{
			if (!DestroyFactor(pcsDestFactor, eType))
				return FALSE;
		}
	}

	for (int i = 0; i < AGPD_FACTORS_MAX_TYPE; ++i)
	{
		if (eType != AGPD_FACTORS_TYPE_NONE &&
			eType != (eAgpdFactorsType) i)
			continue;

		if (i == AGPD_FACTORS_TYPE_RESULT)
		{
			if (!bCopyResult || !GetFactor(pcsSourceFactor, AGPD_FACTORS_TYPE_RESULT))
				continue;

			CopyFactor((AgpdFactor *) SetFactor(pcsDestFactor, NULL, AGPD_FACTORS_TYPE_RESULT), (AgpdFactor *) GetFactor(pcsSourceFactor, AGPD_FACTORS_TYPE_RESULT), bCopyCharPoint);
		}

		if (i == AGPD_FACTORS_TYPE_CHAR_POINT && !bCopyCharPoint)
			continue;

		if (pcsSourceFactor->m_pvFactor[i] == NULL)
			continue;

		if (!SetFactor(pcsDestFactor, pcsSourceFactor, i))
			return FALSE;
	}

	pcsDestFactor->m_bPoint = pcsSourceFactor->m_bPoint;

	return TRUE;
}

BOOL AgpmFactors::CopyFactorStatus(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor)
{
	if (!pcsDestFactor || !pcsSourceFactor)
		return FALSE;

	// status중 str, dex, con, wis, int, cha 만 카피한다.
	AgpdFactorCharStatus	*pcsDestCharStatus	= (AgpdFactorCharStatus *) SetFactor(pcsDestFactor, NULL, AGPD_FACTORS_TYPE_CHAR_STATUS);
	if (pcsDestCharStatus)
	{
		AgpdFactorCharStatus	*pcsSourceCharStatus	= (AgpdFactorCharStatus *) GetFactor(pcsSourceFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);
		if (pcsSourceCharStatus)
		{
			pcsDestCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_CON]		= pcsSourceCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_CON];
			pcsDestCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_STR]		= pcsSourceCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_STR];
			pcsDestCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_INT]		= pcsSourceCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_INT];
			pcsDestCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_DEX]		= pcsSourceCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_DEX];
			pcsDestCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_CHA]		= pcsSourceCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_CHA];
			pcsDestCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_WIS]		= pcsSourceCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_WIS];
		}
	}

	CopyFactor(pcsDestFactor, pcsSourceFactor, FALSE, FALSE, AGPD_FACTORS_TYPE_DAMAGE);
	CopyFactor(pcsDestFactor, pcsSourceFactor, FALSE, FALSE, AGPD_FACTORS_TYPE_DEFENSE);
	CopyFactor(pcsDestFactor, pcsSourceFactor, FALSE, FALSE, AGPD_FACTORS_TYPE_ATTACK);

	return TRUE;
}

BOOL AgpmFactors::AddResultFactor(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor)
{
	if (!pcsDestFactor || !pcsSourceFactor)
	{
		return FALSE;
	}

	AgpdFactor *pcsResultFactor = (AgpdFactor *) SetFactor(pcsDestFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsResultFactor)
		return FALSE;

	INT32	lIndex;

	for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (g_alFactorDetail[lIndex].eFactorType != AGPD_FACTORS_TYPE_RESULT)
		{
			AddResultFactor(GetFactor(pcsDestFactor, (eAgpdFactorsType) lIndex), GetFactor(pcsSourceFactor, (eAgpdFactorsType) lIndex), (eAgpdFactorsType) lIndex);
		}
	}

	return TRUE;
}

BOOL AgpmFactors::AddResultFactor(PVOID pvDestFactor, PVOID pvSourceFactor, eAgpdFactorsType eType)
{
	if (!pvSourceFactor || eType <= 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return FALSE;

	INT32	lIndex;
	INT32 *	plDestValue = (INT32 *) pvDestFactor;
	INT32 *	plSourceValue = (INT32 *) pvSourceFactor;
	PVOID	pvDestValue = pvDestFactor;
	PVOID	pvSourceValue = pvSourceFactor;

	if (!pvDestFactor)
	{
		pvDestFactor = SetFactor(&pvDestFactor, &pvSourceFactor, eType);
		if (!pvDestFactor)
			return FALSE;

		return TRUE;
	}

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			plDestValue[lIndex] += plSourceValue[lIndex];
		}
	}
	else
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			pvDestValue = (PVOID) ((CHAR *) pvDestFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);
			pvSourceValue = (PVOID) ((CHAR *) pvSourceFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

			if (!AddResultFactor(pvDestValue, pvSourceValue, g_alFactorDetail[eType].eFactorType))
				return FALSE;
		}
	}

	return TRUE;
}

AgpdFactor::AgpdFactor()
{
	for (int i = 0; i < AGPD_FACTORS_MAX_TYPE; ++i)
		m_pvFactor[i] = NULL;

	m_bPoint = TRUE;
}

AgpdFactor::~AgpdFactor()
{
}

BOOL AgpmFactors::SetOwnerFactor(AgpdFactor *pcsFactor, INT32 lID, PVOID pvOwner)
{
	if (!pcsFactor || lID <= 0 || !pvOwner)
		return FALSE;

	AgpdFactorOwner	*pcsFactorOwner = (AgpdFactorOwner *) SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_OWNER);
	if (!pcsFactorOwner)
		return FALSE;

	pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_ID] = (INT_PTR)lID;
	pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER] = (INT_PTR) pvOwner;

	return TRUE;
}

BOOL AgpmFactors::SetCallbackUpdateFactorParty(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_PARTY, pfCallback, pClass);
}

BOOL AgpmFactors::SetCallbackUpdateFactorUI(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_UI, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorSkillPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_SKILL_POINT, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorHeroicPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_HEROIC_POINT, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorDamage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_DAMAGE, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorView(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_VIEW, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorMovement(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_MOVEMENT, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorBonusExp(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_BONUS_EXP, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorHp(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_HP, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorMp(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_MP, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorMurdererPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_MURDERER_POINT, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorCharismaPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_CHARISMA_POINT, pfCallback, pClass );
}

BOOL AgpmFactors::SetCallbackUpdateFactorDurability(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_FACTORS_CB_UPDATE_FACTOR_DURABILITY, pfCallback, pClass );
}

BOOL AgpmFactors::CompareFactor(AgpdFactor *pcsDestFactor, AgpdFactor *pcsSourceFactor, INT32 lValue)
{
	if (!pcsDestFactor || !pcsSourceFactor)
		return FALSE;

	INT32	lIndex;

	for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (g_alFactorDetail[lIndex].eFactorType != AGPD_FACTORS_TYPE_RESULT)
		{
			if(!CompareFactor(GetFactor(pcsDestFactor, (eAgpdFactorsType) lIndex), GetFactor(pcsSourceFactor, (eAgpdFactorsType) lIndex), (eAgpdFactorsType) lIndex, lValue))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmFactors::CompareFactor(PVOID pvDestFactor, PVOID pvSourceFactor, eAgpdFactorsType eType, INT32 lValue)
{
	if (!pvSourceFactor)
		return TRUE;

	if (!pvDestFactor)
		return FALSE;

	INT32	lIndex;
	INT32 *	plDestValue = (INT32 *) pvDestFactor;
	INT32 *	plSourceValue = (INT32 *) pvSourceFactor;
	PVOID	pvDestValue = pvDestFactor;
	PVOID	pvSourceValue = pvSourceFactor;
	INT32	lDiff;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			if (plSourceValue[lIndex] < 0)
				continue;

			lDiff = plDestValue[lIndex] - plSourceValue[lIndex];

			if (!lValue && lDiff)
				return FALSE;
			if (lValue > 0 && lDiff <= 0)
				return FALSE;
			if (lValue < 0 && lDiff >= 0)
				return FALSE;
		}
	}
	else
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			pvDestValue = (PVOID) ((CHAR *) pvDestFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);
			pvSourceValue = (PVOID) ((CHAR *) pvSourceFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

			if (!CompareFactor(pvDestValue, pvSourceValue, g_alFactorDetail[eType].eFactorType, lValue))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmFactors::CalcResultFactor(AgpdFactor *pcsTargetFactor, AgpdFactor *pcsFactorChar, AgpdFactor *pcsPointFactor, AgpdFactor *pcsPercentFactor)
{
	if (!pcsTargetFactor || !pcsFactorChar || !pcsPointFactor || !pcsPercentFactor)
		return FALSE;

	AgpdFactor	*pcsTargetFactorResult		= (AgpdFactor *) SetFactor(pcsTargetFactor, NULL, AGPD_FACTORS_TYPE_RESULT);

	INT32	i = 0, lIndex = 0;

	if (!pcsTargetFactorResult)
		return FALSE;

	CopyFactor(pcsTargetFactorResult, pcsFactorChar, FALSE, FALSE);

	CalcFactor(pcsTargetFactorResult, pcsPointFactor, FALSE, FALSE, TRUE);

	DestroyFactor(pcsTargetFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);

	for (i = 0; i < AGPD_FACTORS_MAX_TYPE; ++i)
	{
		if (pcsPercentFactor->m_pvFactor[i])
			break;
	}

	if (i != AGPD_FACTORS_MAX_TYPE)
	{
		for (lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
		{
			if (g_alFactorDetail[lIndex].eFactorType != AGPD_FACTORS_TYPE_RESULT ||
				lIndex != AGPD_FACTORS_TYPE_CHAR_POINT)
			{
				CalcPercentFactor(GetFactor(pcsTargetFactorResult, (eAgpdFactorsType) lIndex),
								  GetFactor(pcsTargetFactorResult, (eAgpdFactorsType) lIndex),
								  GetFactor(pcsPercentFactor, (eAgpdFactorsType) lIndex),
								  (eAgpdFactorsType) lIndex);
			}
		}
	}

	return TRUE;
}

BOOL AgpmFactors::CalcPercentFactor(PVOID pvTargetFactor, PVOID pvPointFactor, PVOID pvPercentFactor, eAgpdFactorsType eType)
{
	if (!pvTargetFactor || !pvPointFactor || !pvPercentFactor)
		return FALSE;

	INT32	lIndex;
	INT32 *	plTargetValue = (INT32 *) pvTargetFactor;
	INT32 *	plPointValue = (INT32 *) pvPointFactor;
	INT32 *	plPercentValue = (INT32 *) pvPercentFactor;
	PVOID	pvTargetValue = pvTargetFactor;
	PVOID	pvPointValue = pvPointFactor;
	PVOID	pvPercentValue = pvPercentFactor;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			// 팩터 퍼센트 음수값 적용되도록 주석처리 2012-04-17 silvermoo // 잠시 보류(몬스터가 공격 타겟을 잃는 현상 발생)
 			if (plPercentValue[lIndex] <= (-100))
 				plTargetValue[lIndex] = 0;
 			else
			{
				if (g_bFactorUseFloat[eType][lIndex])
				{
					// 2008.03.31. steeple
					// pvTargetFactor 와 pvPointFactor 가 같은 놈이 오면 값이 계산하면서 변하기 때문에 로컬 변수를 이용했다.
					double dPointValue = (double)plPointValue[lIndex];
					double dTargetValue = dPointValue * (1 + (double)plPercentValue[lIndex] / (double)10000);

					if(((INT32)(dPointValue / 100 + FLT_EPSILON)) == ((INT32)(dTargetValue / 100 + FLT_EPSILON)))
					{
						if (plPercentValue[lIndex] < 0)
							dTargetValue -= (double)100;
						else if (plPercentValue[lIndex] > 0)
							dTargetValue += (double)100;
					}

					plTargetValue[lIndex] = (INT32)(dTargetValue + FLT_EPSILON);
				}
				else
				{
					if ((eType == AGPD_FACTORS_TYPE_CHAR_POINT && lIndex == AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW) ||
						(eType == AGPD_FACTORS_TYPE_CHAR_POINT_MAX && lIndex == AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW))
					{
						INT64	llPointValue	= 0;
						INT64	llPercentValue	= 0;
						INT64	llTargetValue	= 0;

						CopyMemory(&llPointValue, &plPointValue[lIndex + 1], sizeof(INT32));
						CopyMemory(((CHAR *) &llPointValue) + sizeof(INT32), &plPointValue[lIndex], sizeof(INT32));

						CopyMemory(&llPercentValue, &plPercentValue[lIndex + 1], sizeof(INT32));
						CopyMemory(((CHAR *) &llPercentValue) + sizeof(INT32), &plPercentValue[lIndex], sizeof(INT32));

						// INT64 캐스팅 전에 FLT_EPSILON 더했다. 2005.09.28. steeple
						llTargetValue	= (INT64) ((double) llPointValue * (1.0f + (double) llPercentValue / 100.0) + FLT_EPSILON);

						if (llPointValue == llTargetValue)
						{
							if (llPercentValue < 0)
								--llTargetValue;
							else if (llPercentValue > 0)
								++llTargetValue;
						}

						CopyMemory(&plTargetValue[lIndex + 1], &llTargetValue, sizeof(INT32));
						CopyMemory(&plTargetValue[lIndex], ((CHAR *) &llTargetValue) + sizeof(INT32), sizeof(INT32));

						++lIndex;
					}
					else
					{
						// 2008.03.31. steeple
						// pvTargetFactor 와 pvPointFactor 가 같은 놈이 오면 값이 계산하면서 변하기 때문에 로컬 변수를 이용했다.
						double dPointValue = (double)plPointValue[lIndex];
						double dTargetValue = dPointValue * (1 + (double)plPercentValue[lIndex] / (double)100);

						if(dPointValue == dTargetValue)
						{
							if (plPercentValue[lIndex] < 0)
								--dTargetValue;
							else if (plPercentValue[lIndex] > 0)
								++dTargetValue;
						}

						plTargetValue[lIndex] = (INT32)(dTargetValue + FLT_EPSILON);
					}
				}
			}
		}
	}
	else
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			pvTargetValue = (PVOID) ((CHAR *) pvTargetFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);
			pvPointValue = (PVOID) ((CHAR *) pvPointFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);
			pvPercentValue = (PVOID) ((CHAR *) pvPercentFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

			CalcPercentFactor(pvTargetValue, pvPointValue, pvPercentValue, g_alFactorDetail[eType].eFactorType);
		}
	}

	return TRUE;
}

INT32 *AgpmFactors::GetValuePointer(PVOID pvFactor, eAgpdFactorsType eType, INT32 lType1, INT32 lType2, INT32 lType3)
{
	if (!pvFactor)
		return NULL;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_RESULT)
		return GetValuePointer((AgpdFactor *) pvFactor, (eAgpdFactorsType) lType1, lType2, lType3);
	else if (g_alFactorDetail[eType].eFactorType == -1)
	{
		if (lType1 >= g_alFactorDetail[eType].lNumber)
			return NULL;

		return &((INT32 *)pvFactor)[lType1];
	}
	else
	{
		return GetValuePointer((PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lType1), g_alFactorDetail[eType].eFactorType, lType2, lType3);
	}

	return NULL;
}

BOOL AgpmFactors::GetValue(PVOID pvFactor, INT32 *plValue, eAgpdFactorsType eType, INT32 lType1, INT32 lType2, INT32 lType3)
{
	if (!pvFactor)
		return FALSE;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_RESULT)
		return GetValue((AgpdFactor *) pvFactor, plValue, (eAgpdFactorsType) lType1, lType2, lType3);
	else if (g_alFactorDetail[eType].eFactorType == -1)
	{
		if (lType1 >= g_alFactorDetail[eType].lNumber)
			return FALSE;

		if (g_bFactorUseFloat[eType][lType1])
			*plValue = (INT32) (((INT32 *) pvFactor)[lType1] / 100);
		else
			*plValue = ((INT32 *) pvFactor)[lType1];
		return TRUE;
	}
	else
	{
		return GetValue((PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lType1), plValue, g_alFactorDetail[eType].eFactorType, lType2, lType3);
	}

	return TRUE;
}

BOOL AgpmFactors::SetValue(PVOID pvFactor, INT32 lValue, eAgpdFactorsType eType, INT32 lType1, INT32 lType2, INT32 lType3)
{
	if (!pvFactor)
		return FALSE;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_RESULT)
		return SetValue((AgpdFactor *) pvFactor, lValue, (eAgpdFactorsType) lType1, lType2, lType3);
	else if (g_alFactorDetail[eType].eFactorType == -1)
	{
		if (lType1 >= g_alFactorDetail[eType].lNumber)
			return FALSE;

		if (g_bFactorUseFloat[eType][lType1])
			((INT32 *) pvFactor)[lType1] = lValue * 100;
		else
			((INT32 *) pvFactor)[lType1] = lValue;
		return TRUE;
	}
	else
	{
		return SetValue((PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lType1), lValue, g_alFactorDetail[eType].eFactorType, lType2, lType3);
	}

	return TRUE;
}

BOOL AgpmFactors::GetValue(PVOID pvFactor, FLOAT *pfValue, eAgpdFactorsType eType, INT32 lType1, INT32 lType2, INT32 lType3)
{
	if (!pvFactor)
		return FALSE;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_RESULT)
		return GetValue((AgpdFactor *) pvFactor, pfValue, (eAgpdFactorsType) lType1, lType2, lType3);
	else if (g_alFactorDetail[eType].eFactorType == -1)
	{
		if (lType1 >= g_alFactorDetail[eType].lNumber)
			return FALSE;

		if (g_bFactorUseFloat[eType][lType1])
			*pfValue = (FLOAT) (((INT32 *) pvFactor)[lType1] / 100.0);
		else
			*pfValue = (FLOAT) ((INT32 *) pvFactor)[lType1];
		return TRUE;
	}
	else
	{
		return GetValue((PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lType1), pfValue, g_alFactorDetail[eType].eFactorType, lType2, lType3);
	}

	return TRUE;
}

BOOL AgpmFactors::SetValue(PVOID pvFactor, FLOAT fValue, eAgpdFactorsType eType, INT32 lType1, INT32 lType2, INT32 lType3)
{
	if (!pvFactor)
		return FALSE;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_RESULT)
		return SetValue((AgpdFactor *) pvFactor, fValue, (eAgpdFactorsType) lType1, lType2, lType3);
	else if (g_alFactorDetail[eType].eFactorType == -1)
	{
		if (lType1 >= g_alFactorDetail[eType].lNumber)
			return FALSE;

		if (g_bFactorUseFloat[eType][lType1])
			((INT32 *) pvFactor)[lType1] = (INT32) (fValue * 100);
		else
			((INT32 *) pvFactor)[lType1] = (INT32) fValue;

		return TRUE;
	}
	else
	{
		return SetValue((PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lType1), fValue, g_alFactorDetail[eType].eFactorType, lType2, lType3);
	}

	return TRUE;
}

INT32 *AgpmFactors::GetValuePointer(AgpdFactor *pcsFactor, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	if (!pcsFactor || lSubType1 < 0 || eType < 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return NULL;

	return GetValuePointer(GetFactor(pcsFactor, eType), eType, lSubType1, lSubType2, lSubType3);
}

BOOL AgpmFactors::GetValue(AgpdFactor *pcsFactor, INT32 *plValue, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	if (!pcsFactor || !plValue || lSubType1 < 0 || eType < 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return FALSE;

	return GetValue(GetFactor(pcsFactor, eType), plValue, eType, lSubType1, lSubType2, lSubType3);
}

BOOL AgpmFactors::SetValue(AgpdFactor *pcsFactor, INT32 lValue, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	if (!pcsFactor || lSubType1 < 0 || eType < 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return FALSE;

	return SetValue(SetFactor(pcsFactor, NULL, eType), lValue, eType, lSubType1, lSubType2, lSubType3);
}

BOOL AgpmFactors::GetValue(AgpdFactor *pcsFactor, FLOAT *pfValue, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	if (!pcsFactor || !pfValue || lSubType1 < 0 || eType < 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return FALSE;

	return GetValue(GetFactor(pcsFactor, eType), pfValue, eType, lSubType1, lSubType2, lSubType3);
}

BOOL AgpmFactors::SetValue(AgpdFactor *pcsFactor, FLOAT fValue, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	if (!pcsFactor || lSubType1 < 0 || eType < 0 || eType >= AGPD_FACTORS_MAX_TYPE)
		return FALSE;

	return SetValue(SetFactor(pcsFactor, NULL, eType), fValue, eType, lSubType1, lSubType2, lSubType3);
}

BOOL AgpmFactors::IsFloatValue(eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_RESULT)
		return IsFloatValue((eAgpdFactorsType) lSubType1, lSubType2, lSubType3);
	else if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		if (lSubType1 >= g_alFactorDetail[eType].lNumber)
			return FALSE;

		return g_bFactorUseFloat[eType][lSubType1];
	}
	else
	{
		return IsFloatValue(g_alFactorDetail[eType].eFactorType, lSubType2, lSubType3);
	}

	return FALSE;
}

INT32 AgpmFactors::GetRace(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharType	*pcsFactorCharType = NULL;

	AgpdFactor	*pcsResultFactor	= (AgpdFactor *)	GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (pcsResultFactor)
		pcsFactorCharType = (AgpdFactorCharType *) GetFactor(pcsResultFactor, AGPD_FACTORS_TYPE_CHAR_TYPE);
	else
		pcsFactorCharType = (AgpdFactorCharType *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_TYPE);

	if (!pcsFactorCharType)
		return (-1);

	return pcsFactorCharType->lValue[AGPD_FACTORS_CHARTYPE_TYPE_RACE];
}

BOOL AgpmFactors::CheckRace(AuRaceType eCheckRace, AgpdFactor *pcsFactor)
{
	INT32 lRace = GetRace(pcsFactor);

	if(lRace == AURACE_TYPE_NONE)
		return TRUE;

	return ((lRace & (1 << (eCheckRace-1))) > 0 );
}

INT32 AgpmFactors::GetGender(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharType	*pcsFactorCharType = NULL;

	AgpdFactor	*pcsResultFactor	= (AgpdFactor *)	GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (pcsResultFactor)
		pcsFactorCharType = (AgpdFactorCharType *) GetFactor(pcsResultFactor, AGPD_FACTORS_TYPE_CHAR_TYPE);
	else
		pcsFactorCharType = (AgpdFactorCharType *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_TYPE);

	if (!pcsFactorCharType)
		return (-1);

	return pcsFactorCharType->lValue[AGPD_FACTORS_CHARTYPE_TYPE_GENDER];
}

INT32 AgpmFactors::GetClass(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharType	*pcsFactorCharType = NULL;

	AgpdFactor	*pcsResultFactor	= (AgpdFactor *)	GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (pcsResultFactor)
		pcsFactorCharType = (AgpdFactorCharType *) GetFactor(pcsResultFactor, AGPD_FACTORS_TYPE_CHAR_TYPE);
	else
		pcsFactorCharType = (AgpdFactorCharType *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_TYPE);

	if (!pcsFactorCharType)
		return (-1);

	return pcsFactorCharType->lValue[AGPD_FACTORS_CHARTYPE_TYPE_CLASS];
}

BOOL AgpmFactors::CheckClass(AuCharClassType eCheckClass, AgpdFactor *pcsFactor)
{
	INT32 lClass = GetClass(pcsFactor);

	if(lClass == AUCHARCLASS_TYPE_NONE)
		return TRUE;

	return ((lClass & (1 << (eCheckClass-1))) > 0 );
}

INT32 AgpmFactors::GetLevel(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharStatus	*pcsFactorCharStatus	= NULL;

	AgpdFactor	*pcsResultFactor	= (AgpdFactor *)	GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (pcsResultFactor)
		pcsFactorCharStatus = (AgpdFactorCharStatus *) GetFactor(pcsResultFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);
	else
		pcsFactorCharStatus = (AgpdFactorCharStatus *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);

	if (!pcsFactorCharStatus)
		return (-1);

	return pcsFactorCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL];
}

INT32 AgpmFactors::GetLevelBefore(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharStatus	*pcsFactorCharStatus	= NULL;

	pcsFactorCharStatus = (AgpdFactorCharStatus *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);

	return pcsFactorCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_BEFORELEVEL];
}

INT32	AgpmFactors::GetLevelLimited	(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharStatus	*pcsFactorCharStatus	= NULL;

	pcsFactorCharStatus = (AgpdFactorCharStatus *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);

	return pcsFactorCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_LIMITEDLEVEL];
}

INT32 AgpmFactors::GetMurderPoint(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharStatus	*pcsFactorCharStatus	= NULL;

	AgpdFactor	*pcsResultFactor	= (AgpdFactor *)	GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (pcsResultFactor)
		pcsFactorCharStatus = (AgpdFactorCharStatus *) GetFactor(pcsResultFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);
	else
		pcsFactorCharStatus = (AgpdFactorCharStatus *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);

	if (!pcsFactorCharStatus)
		return (-1);

	return pcsFactorCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER];
}

INT64 AgpmFactors::GetExp(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
		return (-1);

	INT64	llExp	= 0;

	CopyMemory(&llExp, &pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_EXP_HIGH], sizeof(INT32));
	CopyMemory(((CHAR *) &llExp) + sizeof(INT32), &pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW], sizeof(INT32));

	return llExp;
}

INT64 AgpmFactors::GetMaxExp(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharPointMax *pcsFactorCharPointMax = (AgpdFactorCharPointMax *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
	if (!pcsFactorCharPointMax)
		return (-1);

	INT64	llMaxExp	= 0;

	CopyMemory(&llMaxExp, &pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_HIGH], sizeof(INT32));
	CopyMemory(((CHAR *) &llMaxExp) + sizeof(INT32), &pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW], sizeof(INT32));

	return llMaxExp;
}

BOOL AgpmFactors::SetExp(AgpdFactor *pcsFactor, INT64 llExp)
{
	if (!pcsFactor)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
		return FALSE;

	CopyMemory(&pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_EXP_HIGH], &llExp, sizeof(INT32));
	CopyMemory(&pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW], ((CHAR *) &llExp) + sizeof(INT32), sizeof(INT32));

	return TRUE;
}

BOOL AgpmFactors::SetMaxExp(AgpdFactor *pcsFactor, INT64 llMaxExp)
{
	if (!pcsFactor)
		return FALSE;

	AgpdFactorCharPointMax *pcsFactorCharPointMax = (AgpdFactorCharPointMax *) SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
	if (!pcsFactorCharPointMax)
		return FALSE;

	CopyMemory(&pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_HIGH], &llMaxExp, sizeof(INT32));
	CopyMemory(&pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW], ((CHAR *) &llMaxExp) + sizeof(INT32), sizeof(INT32));

	return TRUE;
}

INT32 AgpmFactors::GetHP(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
		return (-1);

	return pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP];
}

INT32 AgpmFactors::GetMaxHP(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharPoint *pcsFactorCharPointMax = (AgpdFactorCharPoint *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
	if (!pcsFactorCharPointMax)
		return (-1);

	return pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP];
}

INT32 AgpmFactors::GetMP(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
		return (-1);

	return pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_MP];
}

INT32 AgpmFactors::GetMaxMP(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharPoint *pcsFactorCharPointMax = (AgpdFactorCharPoint *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
	if (!pcsFactorCharPointMax)
		return (-1);

	return pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_MP];
}

INT32 AgpmFactors::GetSP(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
		return (-1);

	return pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_SP];
}

INT32 AgpmFactors::GetMaxSP(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return (-1);

	AgpdFactorCharPoint *pcsFactorCharPointMax = (AgpdFactorCharPoint *) GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
	if (!pcsFactorCharPointMax)
		return (-1);

	return pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_SP];
}

INT32 AgpmFactors::GetSkillPoint(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return 0;

	AgpdFactorDIRT	*pcsFactorDIRT	=	NULL;

	AgpdFactor	*pcsResultFactor	= (AgpdFactor *)	GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (pcsResultFactor)
	{
		pcsFactorDIRT	= (AgpdFactorDIRT *) GetFactor(pcsResultFactor, AGPD_FACTORS_TYPE_DIRT);
	}

	if (!pcsFactorDIRT)
		pcsFactorDIRT	= (AgpdFactorDIRT *) GetFactor(pcsFactor,	AGPD_FACTORS_TYPE_DIRT);

	if (!pcsFactorDIRT) return 0;

	return pcsFactorDIRT->lValue[AGPD_FACTORS_DIRT_TYPE_SKILL_POINT];
}

INT32 AgpmFactors::GetSkillLevel(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return 0;

	AgpdFactorDIRT	*pcsFactorDIRT	=
		(AgpdFactorDIRT *) GetFactor(pcsFactor,	AGPD_FACTORS_TYPE_DIRT);

	if (!pcsFactorDIRT) return 0;

	return pcsFactorDIRT->lValue[AGPD_FACTORS_DIRT_TYPE_SKILL_LEVEL];
}

INT32 AgpmFactors::GetHeroicPoint(AgpdFactor *pcsFacter)
{
	if(NULL == pcsFacter)
		return 0;

	AgpdFactorDIRT *pcsFactorDIRT = 
		(AgpdFactorDIRT*) GetFactor(pcsFacter, AGPD_FACTORS_TYPE_DIRT);

	if(NULL == pcsFactorDIRT) return 0;

	return pcsFactorDIRT->lValue[AGPD_FACTORS_DIRT_TYPE_HEROIC_POINT];
}

BOOL AgpmFactors::DivideFactor(AgpdFactor *pcsFactor, INT32 lDivideNum)
{
	if (!pcsFactor || lDivideNum < 1)
		return FALSE;

	// pcsFactor 중 0이 아닌값들을 lDivideNum을 나눈다. 걍 나누기만 한다. 끝이다.

	for (INT32 lIndex = 0; lIndex < AGPD_FACTORS_MAX_TYPE; ++lIndex)
	{
		if (g_alFactorDetail[lIndex].eFactorType != AGPD_FACTORS_TYPE_RESULT)
		{
			if(!DivideFactor(GetFactor(pcsFactor, (eAgpdFactorsType) lIndex), (eAgpdFactorsType) lIndex, lDivideNum))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmFactors::DivideFactor(PVOID pvFactor, eAgpdFactorsType eType, INT32 lDivideNum)
{
	if (!pvFactor)
		return TRUE;

	INT32	lIndex;
	INT32 *	plDestValue = (INT32 *) pvFactor;
	PVOID	pvDestValue = pvFactor;

	if (g_alFactorDetail[eType].eFactorType == AGPD_FACTORS_TYPE_NONE)
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			if ((eType == AGPD_FACTORS_TYPE_CHAR_POINT && lIndex == AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW) ||
				(eType == AGPD_FACTORS_TYPE_CHAR_POINT_MAX && lIndex == AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP_LOW))
			{
				INT64	llDestValue	= 0;
				CopyMemory(&llDestValue, &plDestValue[lIndex + 1], sizeof(INT32));
				CopyMemory(((CHAR *) &llDestValue) + sizeof(INT32), &plDestValue[lIndex], sizeof(INT32));

				llDestValue	= (INT64) (llDestValue / (lDivideNum + 0.0f));

				CopyMemory(&plDestValue[lIndex + 1], &llDestValue, sizeof(INT32));
				CopyMemory(&plDestValue[lIndex], ((CHAR *) &llDestValue) + sizeof(INT32), sizeof(INT32));

				++lIndex;
			}
			else
			{
				plDestValue[lIndex] = (INT32) (plDestValue[lIndex] / (lDivideNum + 0.0));
			}
		}
	}
	else
	{
		for (lIndex = 0; lIndex < g_alFactorDetail[eType].lNumber; ++lIndex)
		{
			pvDestValue = (PVOID) ((CHAR *) pvFactor + g_nFactorSize[g_alFactorDetail[eType].eFactorType] * lIndex);

			if (!DivideFactor(pvDestValue, g_alFactorDetail[eType].eFactorType, lDivideNum))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmFactors::InitDamageFactors(AgpdFactor *pcsFactor, AgpdFactor *pcsFactorBuffer)
{
	if (!pcsFactor)
		return FALSE;

	INT32	lZero		= 0;

	if (pcsFactorBuffer)
	{
		SetFactor(pcsFactorBuffer, pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	}

	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);
	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_MAGIC);
	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_WATER);
	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_FIRE);
	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_EARTH);
	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_AIR);
	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_POISON);
	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_LIGHTENING);
	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_ICE);
	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_BONUS_EXP);
	SetValue(pcsFactor, lZero, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC);

	return TRUE;
}

AgpdFactor* AgpmFactors::AllocAgpdFactor(INT32 lDataIndex)
{
	AgpdFactor	*pcsFactor	= NULL;

	if (-1 != m_csMemoryPoolAgpdFactor.GetTypeIndex())
		pcsFactor = (AgpdFactor *) m_csMemoryPoolAgpdFactor.Alloc();
	else
		pcsFactor = (AgpdFactor *) new BYTE[sizeof(AgpdFactor)];

	if (pcsFactor)
		ZeroMemory(pcsFactor, sizeof(AgpdFactor));

	if (lDataIndex >= 0 &&
		lDataIndex < AGPM_FACTORS_MAX_NUM_DATA_TYPE_NAME)
	{
		++g_lDataTypeNameCount[lDataIndex];
	}

	return pcsFactor;
}

BOOL AgpmFactors::FreeAgpdFactor(AgpdFactor *pcsAgpdFactor, INT32 lDataIndex)
{
	if (!pcsAgpdFactor)
		return FALSE;

	if (-1 != m_csMemoryPoolAgpdFactor.GetTypeIndex())
		m_csMemoryPoolAgpdFactor.Free(pcsAgpdFactor);
	else
		delete [] (BYTE *) pcsAgpdFactor;

	if (lDataIndex >= 0 &&
		lDataIndex < AGPM_FACTORS_MAX_NUM_DATA_TYPE_NAME)
	{
		--g_lDataTypeNameCount[lDataIndex];
	}

	return TRUE;
}

INT32 AgpmFactors::SetFactorDataName(CHAR *szDataName)
{
	for (int i = 1; i < AGPM_FACTORS_MAX_NUM_DATA_TYPE_NAME; ++i)
	{
		if (g_strDataTypeName[i].GetLength() <= 0)
		{
			g_strDataTypeName[i].SetText(szDataName);
			return i;
		}
	}

	return (-1);
}

PVOID AgpmFactors::AllocFactorType(eAgpdFactorsType eType)
{
	if (eType <= AGPD_FACTORS_TYPE_NONE ||
		eType >= AGPD_FACTORS_MAX_TYPE)
		return NULL;

	PVOID	pvFactor	= NULL;

	if (-1 != m_csMemoryPoolFactorType[eType].GetTypeIndex())
		pvFactor = m_csMemoryPoolFactorType[eType].Alloc();
	else
		pvFactor = (PVOID) new BYTE[g_nFactorSize[eType]];

	if (pvFactor)
		ZeroMemory(pvFactor, g_nFactorSize[eType]);

	return pvFactor;
}

BOOL AgpmFactors::FreeFactorType(eAgpdFactorsType eType, PVOID pvFactor)
{
	if (eType <= AGPD_FACTORS_TYPE_NONE ||
		eType >= AGPD_FACTORS_MAX_TYPE ||
		!pvFactor)
		return FALSE;

	if (-1 != m_csMemoryPoolFactorType[eType].GetTypeIndex())
		m_csMemoryPoolFactorType[eType].Free(pvFactor);
	else
		delete [] (BYTE*)pvFactor;

	return TRUE;
}

// 2007.10.01. steeple
// 팩터 패킷을 보낼 때, 값이 0 이어도 보내야 하는 것들.
BOOL AgpmFactors::IsMustSend(eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	if(eType == AGPD_FACTORS_TYPE_CHAR_STATUS)
	{
		if(lSubType1 == (INT32)AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER
			|| lSubType1 == (INT32)AGPD_FACTORS_CHARSTATUS_TYPE_CHA)
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmFactors::PrintDataTypeLog()
{
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "\n\nStartLog..........\n");
	AuLogFile_s("LOG\\FactorDataTypeLog.txt", strCharBuff);


	char strCharBuff2[256] = { 0, };
	sprintf_s(strCharBuff2, sizeof(strCharBuff2), "DataType%d(%s) : %d\n", 0, "DefaultType", g_lDataTypeNameCount[0]);
	AuLogFile_s("LOG\\FactorDataTypeLog.txt", strCharBuff2);

	for (int i = 1; i < AGPM_FACTORS_MAX_NUM_DATA_TYPE_NAME; ++i)
	{
		if (g_strDataTypeName[i].GetLength())
		{
			char strCharBuff3[256] = { 0, };
			sprintf_s(strCharBuff3, sizeof(strCharBuff3), "DataType%d(%s) : %d\n", i, g_strDataTypeName[i].GetBuffer(), g_lDataTypeNameCount[i]);
			AuLogFile_s("LOG\\FactorDataTypeLog.txt", strCharBuff3);
		}
	}

	return TRUE;
}
