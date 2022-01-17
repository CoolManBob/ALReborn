#include "AgcmEventEffectData.h"
#include "AcuRpMatD3DFx.h"

#include "ApMemoryTracker.h"
#include "AgpdCharacter.h"

CHAR* g_aszFXDataTypeName[E_FX_MAX_DATA_TYPE] =
{
	"FX"
	"FLOAT",
	"TEXTURE",
	"VECTOR4",
	NULL,
};

AgcmEventEffectData::AgcmEventEffectData()
{
	memset( m_pacsAttackSound, 0, sizeof(AgcdEventEffectSound *) * AGPMITEM_EQUIP_WEAPON_TYPE_NUM);
	memset( m_pacsHitSound, 0, sizeof(AgcdEventEffectSound *) * AGPMITEM_EQUIP_WEAPON_TYPE_NUM * AGCM_EVENT_EFFECT_DATA_SOUND_HIT_SOUND_NUM);
	m_pcsDefaultAttackSound = NULL;
	memset( m_pacsDefaultHitSound, 0, sizeof(AgcdEventEffectSound *) * AGCM_EVENT_EFFECT_DATA_SOUND_HIT_SOUND_NUM);

	m_vecMovingSoundSet.clear();

	memset(m_alNumHitSound, 0, sizeof(INT32) * AGPMITEM_EQUIP_WEAPON_TYPE_NUM);
	m_lNumDefaultHitSound = 0;

	memset(m_pastConvertedWeaponGradeEffect, 0, sizeof(AgcdUseEffectSetData *) * AGCM_EVENT_EFFECT_DATA_CONVERTED_ITEM_GRADE_NUM);
	memset(m_pacsComvertedArmourGradeEffect, 0, sizeof (AgcdConvertedArmourFX *) * E_SPIRIT_STONE_TYPE_NUM * AGCM_EVENT_EFFECT_DATA_MAX_GRADE_CONVERTED_ARMOUR);

	memset(m_pastSpiritStoneHitEffect, 0, sizeof(AgcdUseEffectSetData *) * E_SPIRIT_STONE_TYPE_NUM * AGCM_EVENT_EFFECT_DATA_MAX_SS_LEVEL * AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA);
	memset(m_pastSpiritStoneAttrEffect, 0, sizeof(AgcdUseEffectSetData *) * E_SPIRIT_STONE_TYPE_NUM * AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA);

	m_pszAnimationDataPath		=	NULL;
	m_nCommonCharEffectCount	=	0;
	m_ppCommonCharEffect		=	NULL;
}

AgcmEventEffectData::~AgcmEventEffectData()
{
	m_csSoundDataAdmin.RemoveAll();
	RemoveAllSocialAnimation();
	m_csAnimation2.RemoveAllRtAnim();

	for( INT32 lSSGrade = 0; lSSGrade < AGCM_EVENT_EFFECT_DATA_CONVERTED_ITEM_GRADE_NUM; ++lSSGrade)
	{
		if (m_pastConvertedWeaponGradeEffect[lSSGrade])
			m_csEffectDataAdmin2.Delete(m_pastConvertedWeaponGradeEffect[lSSGrade], TRUE);
	}

	for( INT32 lSSType = 0; lSSType < E_SPIRIT_STONE_TYPE_NUM; ++ lSSType)
	{
		for (INT32 lEffectIndex = 0; lEffectIndex < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lEffectIndex)
		{
			if (m_pastSpiritStoneAttrEffect[lSSType][lEffectIndex])
				m_csEffectDataAdmin2.Delete(m_pastSpiritStoneAttrEffect[lSSType][lEffectIndex], TRUE);
		}

		for (INT32 lSSLevel = 0; lSSLevel < AGCM_EVENT_EFFECT_DATA_MAX_SS_LEVEL; ++lSSLevel)
		{
			for (INT32 lEffectIndex = 0; lEffectIndex < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lEffectIndex)
			{
				if (m_pastSpiritStoneHitEffect[lSSType][lSSLevel][lEffectIndex])
					m_csEffectDataAdmin2.Delete(m_pastSpiritStoneHitEffect[lSSType][lSSLevel][lEffectIndex], TRUE);
			}
		}
	}

	for (INT32 lSSType = 0; lSSType < E_SPIRIT_STONE_TYPE_NUM; ++lSSType)
	{
		for (INT32 lSSGrade = 0; lSSGrade < AGCM_EVENT_EFFECT_DATA_MAX_GRADE_CONVERTED_ARMOUR; ++lSSGrade)
		{
			if (m_pacsComvertedArmourGradeEffect[lSSType][lSSGrade])
			{
				if (m_pacsComvertedArmourGradeEffect[lSSType][lSSGrade]->m_pcsAnimTexture)
				{
					RwTextureDestroy(m_pacsComvertedArmourGradeEffect[lSSType][lSSGrade]->m_pcsAnimTexture);
					m_pacsComvertedArmourGradeEffect[lSSType][lSSGrade]->m_pcsAnimTexture = NULL;
				}

				for (INT32 lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
				{
					if (m_pacsComvertedArmourGradeEffect[lSSType][lSSGrade]->m_pacsMaterial[lMatIndex])
					{
						RpMaterialDestroy(m_pacsComvertedArmourGradeEffect[lSSType][lSSGrade]->m_pacsMaterial[lMatIndex]);
						m_pacsComvertedArmourGradeEffect[lSSType][lSSGrade]->m_pacsMaterial[lMatIndex] = NULL;
					}
				}

				DEF_SAFEDELETE( m_pacsComvertedArmourGradeEffect[lSSType][lSSGrade] );
			}
		}
	}

	DEF_SAFEDELETE( m_pszAnimationDataPath );

	_DestroyCharEffectMemory();
}

VOID AgcmEventEffectData::SetMaxSoundData(INT32 lMax)
{
	m_csSoundDataAdmin.Initialize(lMax);
}

INT32 AgcmEventEffectData::SearchMaterialIndex(CHAR *szMaterialName)
{
	#define	NAME_COMPARE( str , nType )		\
		if(!strcmp(szMaterialName, str))	\
			return nType;

	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SOIL		, APMMAP_MATERIAL_SOIL		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SWAMP		, APMMAP_MATERIAL_SWAMP		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_GRASS		, APMMAP_MATERIAL_GRASS		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SAND		, APMMAP_MATERIAL_SAND		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_LEAF		, APMMAP_MATERIAL_LEAF		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SNOW		, APMMAP_MATERIAL_SNOW		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_WATER		, APMMAP_MATERIAL_WATER		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_STONE		, APMMAP_MATERIAL_STONE		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_WOOD		, APMMAP_MATERIAL_WOOD		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_METAL		, APMMAP_MATERIAL_METAL		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_BONE		, APMMAP_MATERIAL_BONE		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_MUD			, APMMAP_MATERIAL_MUD		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SOILGRASS	, APMMAP_MATERIAL_SOILGRASS	 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SOLIDSOIL	, APMMAP_MATERIAL_SOLIDSOIL	 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_SPORE		, APMMAP_MATERIAL_SPORE		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_MOSS		, APMMAP_MATERIAL_MOSS		 )
	NAME_COMPARE( AGCM_EVENT_EFFECT_SOUND_NAME_TILE_GRANITE		, APMMAP_MATERIAL_GRANITE	 )

	return -1;
}

INT32 AgcmEventEffectData::SearchCommonCharTypeIndex(CHAR *szCommonCharType)
{
	return -1;
}

INT32 AgcmEventEffectData::SearchSpiritStoneIndex(CHAR *szSpiritStone)
{
	if (!strcmp(szSpiritStone, AGCM_EVENT_EFFECT_SPIRIT_STONE_FIRE_NAME))
		return E_SPIRIT_STONE_TYPE_FIRE;
	else if (!strcmp(szSpiritStone, AGCM_EVENT_EFFECT_SPIRIT_STONE_AIR_NAME))
		return E_SPIRIT_STONE_TYPE_AIR;
	else if (!strcmp(szSpiritStone, AGCM_EVENT_EFFECT_SPIRIT_STONE_WATER_NAME))
		return E_SPIRIT_STONE_TYPE_WATER;
	else if (!strcmp(szSpiritStone, AGCM_EVENT_EFFECT_SPIRIT_STONE_MAGIC_NAME))
		return E_SPIRIT_STONE_TYPE_MAGIC;
	else if (!strcmp(szSpiritStone, AGCM_EVENT_EFFECT_SPIRIT_STONE_EARTH_NAME))
		return E_SPIRIT_STONE_TYPE_EARTH;
	else if (!strcmp(szSpiritStone, AGCM_EVENT_EFFECT_SPIRIT_STONE_ICE_NAME))
		return E_SPIRIT_STONE_TYPE_ICE;
	else if (!strcmp(szSpiritStone, AGCM_EVENT_EFFECT_SPIRIT_STONE_LIGHTENING_NAME))
		return E_SPIRIT_STONE_TYPE_LIGHTENING;
	else if (!strcmp(szSpiritStone, AGCM_EVENT_EFFECT_SPIRIT_STONE_POISON_NAME))
		return E_SPIRIT_STONE_TYPE_POISON;
	else if (!strcmp(szSpiritStone, AGCM_EVENT_EFFECT_SPIRIT_STONE_PHYSICAL_NAME))
		return E_SPIRIT_STONE_TYPE_PHYSICAL;
	else if (!strcmp(szSpiritStone, AGCM_EVENT_EFFECT_SPIRIT_STONE_RUNE_NAME))
		return E_SPIRIT_STONE_TYPE_RUNE;

	return -1;
}

INT32 AgcmEventEffectData::SearchSpiritStoneIndex(INT32 lSSType)
{
	switch (lSSType)
	{
	case AGPMITEM_USABLE_SS_TYPE_FIRE:		return E_SPIRIT_STONE_TYPE_FIRE;
	case AGPMITEM_USABLE_SS_TYPE_AIR:		return E_SPIRIT_STONE_TYPE_AIR;
	case AGPMITEM_USABLE_SS_TYPE_WATER:		return E_SPIRIT_STONE_TYPE_WATER;
	case AGPMITEM_USABLE_SS_TYPE_EARTH:		return E_SPIRIT_STONE_TYPE_EARTH;
	case AGPMITEM_USABLE_SS_TYPE_MAGIC:		return E_SPIRIT_STONE_TYPE_MAGIC;
	case AGPMITEM_USABLE_SS_TYPE_ICE:		return E_SPIRIT_STONE_TYPE_ICE;
	case AGPMITEM_USABLE_SS_TYPE_LIGHTENING:return E_SPIRIT_STONE_TYPE_LIGHTENING;
	case AGPMITEM_USABLE_SS_TYPE_POISON:	return E_SPIRIT_STONE_TYPE_POISON;
	default:								return -1;
	}
	
	return -1;
}

BOOL AgcmEventEffectData::ReadCommonCharEffectData(CHAR *szPathName, BOOL bDecryption)
{
	AuExcelTxtLib	csAuExcelTxtLib;
	if (!csAuExcelTxtLib.OpenExcelFile(szPathName, TRUE, bDecryption))
	{
		csAuExcelTxtLib.CloseFile();
		return FALSE;
	}

	const INT32				lStartColumn				= 1;
	const INT32				lPixKeyColumn				= 0;
	const INT32				lPixKeyRow					= 0;
	INT32					lCommonCharTypeIndex		= 0;
	CHAR					*pszData					= NULL;
	AgcdUseEffectSetData	*pstEffectData				= NULL;

	_CreateCharEffectMemory( csAuExcelTxtLib.GetRow() );

	for (INT32 lRow = 1; lRow <= csAuExcelTxtLib.GetRow(); ++lRow)
	{
		//pszData = csAuExcelTxtLib.GetData(lPixKeyColumn, lRow);
		//if( !pszData )						continue;
			
		pstEffectData								= SearchEmptyCommonCharEffectData( lRow-1 );
		ASSERT(pstEffectData);
		if( !pstEffectData )				continue;

		pstEffectData->m_lType = lRow-1;
		ReadCommonEffectData(&csAuExcelTxtLib, pstEffectData, lStartColumn, lPixKeyRow, lRow);
	}

	csAuExcelTxtLib.CloseFile();

	return TRUE;
}

BOOL AgcmEventEffectData::_CreateCharEffectMemory( INT32 nCount )
{
	if( m_ppCommonCharEffect )			return FALSE;

	m_nCommonCharEffectCount			=	nCount;
	m_ppCommonCharEffect				=	new AgcdUseEffectSetData*[ m_nCommonCharEffectCount ];
	for( INT i = 0 ; i < m_nCommonCharEffectCount  ; ++i )
	{
		m_ppCommonCharEffect[i]	=	new AgcdUseEffectSetData[ AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA ];
	}

	return TRUE;
}

BOOL AgcmEventEffectData::_DestroyCharEffectMemory( VOID )
{
	if( !m_ppCommonCharEffect )		return FALSE;

	for( INT i = 0 ; i < m_nCommonCharEffectCount ; ++i )
	{
		DEF_SAFEDELETEARRAY( m_ppCommonCharEffect[i] );
	}
	DEF_SAFEDELETEARRAY( m_ppCommonCharEffect );

	return TRUE;
}

INT32	CBCompareArmour(const void *pvData1, const void *pvData2)
{
	AgcdConvertedArmourFX **	ppstData1 = (AgcdConvertedArmourFX **) pvData1;
	AgcdConvertedArmourFX **	ppstData2 = (AgcdConvertedArmourFX **) pvData2;

	return (*ppstData2)->m_lPoint - (*ppstData1)->m_lPoint;
}

AgcdConvertedArmourFX *AgcmEventEffectData::GetConvertedArmourFX(INT32 lSSIndex, INT32 lPoint, BOOL bAdd)
{
	if (lSSIndex <= E_SPIRIT_STONE_TYPE_NONE || lSSIndex >= E_SPIRIT_STONE_TYPE_NUM)
		return NULL;

	for (INT32 lIndex = 0; lIndex < AGCM_EVENT_EFFECT_DATA_MAX_GRADE_CONVERTED_ARMOUR; ++lIndex)
	{
		if (!m_pacsComvertedArmourGradeEffect[lSSIndex][lIndex])
		{
			if (bAdd)
			{
				AgcdConvertedArmourFX *	pcsFX	= new AgcdConvertedArmourFX();

				m_pacsComvertedArmourGradeEffect[lSSIndex][lIndex]					= pcsFX;
				m_pacsComvertedArmourGradeEffect[lSSIndex][lIndex]->m_lPoint		= lPoint;

				for (INT32 lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					m_pacsComvertedArmourGradeEffect[lSSIndex][lIndex]->m_pacsMaterial[lMatIndex]	= RpMaterialCreate();

				qsort(&m_pacsComvertedArmourGradeEffect[lSSIndex][0], lIndex + 1, sizeof(AgcdConvertedArmourFX *), CBCompareArmour);

				return pcsFX;
			}
		}
		else
		{
			if (m_pacsComvertedArmourGradeEffect[lSSIndex][lIndex]->m_lPoint == lPoint)
			{
				return m_pacsComvertedArmourGradeEffect[lSSIndex][lIndex];
			}
			if (!bAdd && m_pacsComvertedArmourGradeEffect[lSSIndex][lIndex]->m_lPoint < lPoint)
			{
				return m_pacsComvertedArmourGradeEffect[lSSIndex][lIndex];
			}
		}
	}

	return NULL;
}

PVOID __atofCB__(PVOID pvData1, PVOID pvData2)
{
	CHAR	*pszSrc	= (CHAR *)(pvData1);
	FLOAT	*pfData	= (FLOAT *)(pvData2);

	(*pfData)		= (FLOAT)(atof(pszSrc));

	return NULL;
}

PVOID __atoiCB__(PVOID pvData1, PVOID pvData2)
{
	CHAR	*pszSrc		= (CHAR *)(pvData1);
	UINT32	*pulData	= (UINT32 *)(pvData2);

	(*pulData)			= atoi(pszSrc);

	return NULL;
}

VOID __ParseChar__(CHAR *pszSrc, CHAR szDiv, CHAR szSkip, UINT32 ulRef, UINT8 unDataSize, UINT32 ulNumData, AcCallbackData2 pfCallback)
{
	UINT32	ulPointer	= 0;
	UINT32	ulOffset	= 0;
	UINT32	ulNumParse	= 0;
	UINT32	ulLen		= strlen(pszSrc);
	CHAR	szTemp[256];
	memset(szTemp, 0, sizeof (CHAR) * 256);

	for (UINT32 ulCount = 0; ; ++ulCount)
	{
		if (	(ulCount < ulLen) &&
				(pszSrc[ulCount] == szSkip)		)
			continue;

		if (	(ulCount >= ulLen) ||
				(pszSrc[ulCount] == szDiv)	)
		{
			++ulNumParse;
			if (ulNumParse > ulNumData)
				break;	// fin.

			ulPointer	= ulRef + (unDataSize * (ulNumParse - 1));
			pfCallback(szTemp, (PVOID)(ulPointer));
			//pfCallback(szTemp, pData + (ulNumParse - 1));

			if (ulCount >= ulLen)
				break;	// fin.

			memset(szTemp, 0, sizeof (CHAR) * 256);
			ulOffset	= 0;
			continue;
		}

		szTemp[ulOffset++]	= pszSrc[ulCount];
	}

	return;
}

BOOL AgcmEventEffectData::ReadConvertedArmourEffectData(CHAR *szPathName, BOOL bDecryption)
{
	AuExcelTxtLib	csAuExcelTxtLib;
	if (!csAuExcelTxtLib.OpenExcelFile(szPathName, TRUE, bDecryption))
	{
		ASSERT(!"Failed - csAuExcelTxtLib.OpenExcelFile()");
		csAuExcelTxtLib.CloseFile();
		return FALSE;
	}

	//@{ Jaewon 20050215
	bool noShader = ((((const D3DCAPS9*)RwD3D9GetCaps())->VertexShaderVersion & 0xffff) < 0x0101);
	//@} Jaewon

	const INT32	lStartRow		= 1;
	const INT32	lStartColumn	= 2;
	const INT32	lMaxRow			= csAuExcelTxtLib.GetRow();
	const INT32	lMaxColumn		= csAuExcelTxtLib.GetColumn();
	const INT32	lKeyType		= 0;
	const INT32	lKeyGrade		= 1;
	const INT32 lKeyRow			= 0;
	CHAR		*pszData		= NULL;
	INT32		lSSIndex		= 0;
	INT32		lPoint			= 0;
	INT32		lMatIndex		= 0;

	D3DXVECTOR4	stVector4(0, 0, 0, 0);
	UINT32	aulTemp[4]	= {0, 0, 0, 0};
	FLOAT	afTemp[4]	= {0, 0, 0, 0};
	

	AgcdConvertedArmourFX	*pcsFX	= NULL;

	for (INT32 lRow = lStartRow; lRow < lMaxRow; ++lRow)
	{
		pszData		= csAuExcelTxtLib.GetData(lKeyType, lRow);
		if (!pszData)
			continue;

		lSSIndex	= SearchSpiritStoneIndex(pszData);
		if (lSSIndex == -1)
		{
			ASSERT(!"Failed - SearchSpiritStoneIndex()");
			return FALSE;
		}

		pszData		= csAuExcelTxtLib.GetData(lKeyGrade, lRow);
		if (!pszData)
			continue;

		lPoint		= atoi(pszData);

		pcsFX		= GetConvertedArmourFX(lSSIndex, lPoint, TRUE);
		if (!pcsFX)
		{
			ASSERT(!"Failed - GetConvertedArmourFX()");
			return FALSE;
		}

		for (INT32 lCol = lStartColumn; lCol < lMaxColumn; ++lCol)
		{
			pszData		= csAuExcelTxtLib.GetData(lCol, lKeyRow);
			if (!pszData)
				continue;

			if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_FX))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;
				//@{ Jaewon 20050215
				if(!noShader)
				//@} Jaewon
				{
					RpMaterialD3DFxSetEffectWeight1(pcsFX->m_pacsMaterial[0], pszData);
					RpMaterialD3DFxSetEffectWeight2(pcsFX->m_pacsMaterial[1], pszData);
					RpMaterialD3DFxSetEffectWeight3(pcsFX->m_pacsMaterial[2], pszData);
					RpMaterialD3DFxSetEffectWeight4(pcsFX->m_pacsMaterial[3], pszData);
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_TEXTURE))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)		continue;
				pcsFX->m_pcsAnimTexture	= RwTextureRead(pszData, NULL);
				
				if(!noShader)
				{
					for (lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					{
						ASSERT(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]) && pcsFX->m_pcsAnimTexture);
						if(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]) && pcsFX->m_pcsAnimTexture)
							RpMaterialD3DFxSetTexture(pcsFX->m_pacsMaterial[lMatIndex], "animTexture", pcsFX->m_pcsAnimTexture);
					}
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_AMBIENT))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				__ParseChar__(pszData, ',', '"', (UINT32)(aulTemp), sizeof (UINT32), 3, __atoiCB__);
				stVector4.x	= (FLOAT)(aulTemp[0]) / 255.0f;
				stVector4.y	= (FLOAT)(aulTemp[1]) / 255.0f;
				stVector4.z	= (FLOAT)(aulTemp[2]) / 255.0f;
				stVector4.w	= 1.0f;
				//@{ Jaewon 20050215
				if(!noShader)
				//@} Jaewon
				{
					for (lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					{
						//@{ Jaewon 20050317
						// Check, check, and check...
						ASSERT(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]));
						if(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]))
						//@} Jaewon
							RpMaterialD3DFxSetVector(pcsFX->m_pacsMaterial[lMatIndex], "materialAmbient", &stVector4);
					}
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_DIFFUSE))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				__ParseChar__(pszData, ',', '"', (UINT32)(aulTemp), sizeof (UINT32), 3, __atoiCB__);
				stVector4.x	= (FLOAT)(aulTemp[0]) / 255.0f;
				stVector4.y	= (FLOAT)(aulTemp[1]) / 255.0f;
				stVector4.z	= (FLOAT)(aulTemp[2]) / 255.0f;
				stVector4.w	= 1.0f;
				
				if(!noShader)
				{
					for (lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					{
						ASSERT(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]));
						if(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]))
							RpMaterialD3DFxSetVector(pcsFX->m_pacsMaterial[lMatIndex], "materialDiffuse", &stVector4);
					}
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_SPECULAR))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				__ParseChar__(pszData, ',', '"', (UINT32)(aulTemp), sizeof (UINT32), 3, __atoiCB__);
				stVector4.x	= (FLOAT)(aulTemp[0]) / 255.0f;
				stVector4.y	= (FLOAT)(aulTemp[1]) / 255.0f;
				stVector4.z	= (FLOAT)(aulTemp[2]) / 255.0f;
				stVector4.w	= 1.0f;
				
				if(!noShader)
				{
					for (lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					{
						ASSERT(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]));
						if(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]))
							RpMaterialD3DFxSetVector(pcsFX->m_pacsMaterial[lMatIndex], "materialSpecular", &stVector4);
					}
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_UVTRANSLATION))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)	continue;

				__ParseChar__(pszData, ',', '"', (UINT32)(afTemp), sizeof (FLOAT), 4, __atofCB__);
				stVector4.x	= afTemp[0];
				stVector4.y	= afTemp[1];
				stVector4.z	= afTemp[2];
				stVector4.w	= afTemp[3];
				if(!noShader)
				{
					for (lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					{
						ASSERT(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]));
						if(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]))
							RpMaterialD3DFxSetVector(pcsFX->m_pacsMaterial[lMatIndex], "velocity", &stVector4);
					}
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_UVSCALE))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				__ParseChar__(pszData, ',', '"', (UINT32)(afTemp), sizeof (FLOAT), 4, __atofCB__);
				stVector4.x	= afTemp[0];
				stVector4.y	= afTemp[1];
				stVector4.z	= afTemp[2];
				stVector4.w	= afTemp[3];
				
				if(!noShader)
				{
					for (lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					{
						ASSERT(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]));
						if(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]))
							RpMaterialD3DFxSetVector(pcsFX->m_pacsMaterial[lMatIndex], "scale", &stVector4);
					}
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_UVROTATION))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				afTemp[0]	= (FLOAT)(atof(pszData));

				if(!noShader)
				{
					for (lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					{
						ASSERT(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]));
						if(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]))
							RpMaterialD3DFxSetFloat(pcsFX->m_pacsMaterial[lMatIndex], "rotation", afTemp[0]);
					}
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_UVANIMWEIGHT))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				afTemp[0]	= (FLOAT)(atof(pszData));

				if(!noShader)
				{
					for (lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					{
						ASSERT(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]));
						if(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]))
							RpMaterialD3DFxSetFloat(pcsFX->m_pacsMaterial[lMatIndex], "weight", afTemp[0]);
					}
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_USELECTIONVECTOR))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				__ParseChar__(pszData, ',', '"', (UINT32)(afTemp), sizeof (FLOAT), 4, __atofCB__);
				stVector4.x	= afTemp[0];
				stVector4.y	= afTemp[1];
				stVector4.z	= afTemp[2];
				stVector4.w	= afTemp[3];
				
				if(!noShader)
				{
					for (lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					{
						ASSERT(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]));
						if(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]))
							RpMaterialD3DFxSetVector(pcsFX->m_pacsMaterial[lMatIndex], "uSelect", &stVector4);
					}
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_FX_NAME_VSELECTIONVECTOR))
			{
				pszData		= csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)		continue;

				__ParseChar__(pszData, ',', '"', (UINT32)(afTemp), sizeof (FLOAT), 4, __atofCB__);
				stVector4.x	= afTemp[0];
				stVector4.y	= afTemp[1];
				stVector4.z	= afTemp[2];
				stVector4.w	= afTemp[3];

				if(!noShader)
				{
					for (lMatIndex = 0; lMatIndex < AGCM_EVNET_EFFECT_DATA_MAX_MATERIAL; ++lMatIndex)
					{
						ASSERT(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]));
						if(RpMaterialD3DFxGetEffect(pcsFX->m_pacsMaterial[lMatIndex]))
							RpMaterialD3DFxSetVector(pcsFX->m_pacsMaterial[lMatIndex], "vSelect", &stVector4);
					}
				}
			}
		}
	}

	csAuExcelTxtLib.CloseFile();

	return TRUE;
}

BOOL AgcmEventEffectData::ReadConvertedItemGradeEffectData(CHAR *szPathName, BOOL bDecryption)
{
	AuExcelTxtLib	csAuExcelTxtLib;
	if (!csAuExcelTxtLib.OpenExcelFile(szPathName, TRUE, bDecryption))
	{
		csAuExcelTxtLib.CloseFile();
		return FALSE;
	}

	const INT32				lStartRow		= 1;
	const INT32				lStartColumn	= 1;
	const INT32				lPixKeyColumn	= 0;
	const INT32				lPixKeyRow		= 0;
	CHAR					*pszData		= NULL;
	AgcdUseEffectSetData	*pstEffectData	= NULL;

	for (INT32 lRow = lStartRow; lRow < csAuExcelTxtLib.GetRow(); ++lRow)
	{
		pszData								= csAuExcelTxtLib.GetData(lPixKeyColumn, lRow);
		if (!pszData)
			continue;

		if (!strcmp(pszData, AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_W1))
		{
			if (!m_pastConvertedWeaponGradeEffect[0])
			{
				m_pastConvertedWeaponGradeEffect[0]		= m_csEffectDataAdmin2.Create(0);
				if (!m_pastConvertedWeaponGradeEffect[0])
				{
					csAuExcelTxtLib.CloseFile();
					return FALSE;
				}
			}

			pstEffectData								= m_pastConvertedWeaponGradeEffect[0];
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_W2))
		{
			if (!m_pastConvertedWeaponGradeEffect[1])
			{
				m_pastConvertedWeaponGradeEffect[1]		= m_csEffectDataAdmin2.Create(1);
				if (!m_pastConvertedWeaponGradeEffect[1])
				{
					csAuExcelTxtLib.CloseFile();
					return FALSE;
				}
			}

			pstEffectData								= m_pastConvertedWeaponGradeEffect[1];
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_W3))
		{
			if (!m_pastConvertedWeaponGradeEffect[2])
			{
				m_pastConvertedWeaponGradeEffect[2]		= m_csEffectDataAdmin2.Create(2);
				if (!m_pastConvertedWeaponGradeEffect[2])
				{
					csAuExcelTxtLib.CloseFile();
					return FALSE;
				}
			}

			pstEffectData								= m_pastConvertedWeaponGradeEffect[2];
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_W4))
		{
			if (!m_pastConvertedWeaponGradeEffect[3])
			{
				m_pastConvertedWeaponGradeEffect[3]		= m_csEffectDataAdmin2.Create(3);
				if (!m_pastConvertedWeaponGradeEffect[3])
				{
					csAuExcelTxtLib.CloseFile();
					return FALSE;
				}
			}

			pstEffectData								= m_pastConvertedWeaponGradeEffect[3];
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_CONVERTED_ITEM_GRADE_NAME_W5))
		{
			if (!m_pastConvertedWeaponGradeEffect[4])
			{
				m_pastConvertedWeaponGradeEffect[4]		= m_csEffectDataAdmin2.Create(4);
				if (!m_pastConvertedWeaponGradeEffect[4])
				{
					csAuExcelTxtLib.CloseFile();
					return FALSE;
				}
			}

			pstEffectData								= m_pastConvertedWeaponGradeEffect[4];
		}
		else
			continue;

		ReadCommonEffectData(&csAuExcelTxtLib, pstEffectData, lStartColumn, lPixKeyRow, lRow);
	}

	csAuExcelTxtLib.CloseFile();

	return TRUE;
}
BOOL AgcmEventEffectData::ReadMovingSoundData(CHAR *szPathName, BOOL bDecryption)
{
	AuExcelTxtLib	csAuExcelTxtLib;
	if (!csAuExcelTxtLib.OpenExcelFile(szPathName, TRUE, bDecryption))
	{
		csAuExcelTxtLib.CloseFile();
		return FALSE;
	}

	// ID 를 결정함..
	CHAR	*pszData	= NULL;
	INT32	nTID		= 0;

	AgcmEventEffectData::stMovingSoundSet * pSoundSet = GetMovingSoundSet( nTID );

	AgcdEventEffectSound	*pstSound			= NULL;
	INT32					lMaterialIndex		= 0;
	const INT32				lTNameColumn		= 0;
	const INT32				lKeyRow				= 0;
	for (INT32 lRow = 1; lRow < csAuExcelTxtLib.GetRow(); ++lRow)
	{
		pszData				= csAuExcelTxtLib.GetData(lTNameColumn, lRow);
		if (!pszData)
			continue;

		lMaterialIndex		= SearchMaterialIndex(pszData);
		if (lMaterialIndex == -1)
		{
			// 첫칸에 없는 타입이 있으면..
			// 템플릿 ID 일것이다.

			nTID		= atoi( pszData );
            if( nTID != 0 )
			{
				pSoundSet	= GetMovingSoundSet( nTID );
			}
			continue;
		}

		for (INT32 lCol = 1; lCol < csAuExcelTxtLib.GetColumn(); ++lCol)
		{
			pszData = csAuExcelTxtLib.GetData(lCol, lKeyRow);
			if (!pszData)		continue;

			if (!strcmp(pszData, AGCM_EVENT_EFFECT_SOUND_NAME_FOOT_LEFT))
			{
				pszData = csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				pstSound = m_csSoundDataAdmin.GetSound(pszData, TRUE);
				if (!pstSound)
				{
					csAuExcelTxtLib.CloseFile();
					return FALSE;
				}

				if (!SetMovingSound( pSoundSet , lMaterialIndex, 0, pstSound))
				{
					csAuExcelTxtLib.CloseFile();
					return FALSE;
				}
			}
			else if (!strcmp(pszData, AGCM_EVENT_EFFECT_SOUND_NAME_FOOT_RIGHT))
			{
				pszData = csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				pstSound = m_csSoundDataAdmin.GetSound(pszData, TRUE);
				if (!pstSound)
				{
					csAuExcelTxtLib.CloseFile();
					return FALSE;
				}

				if (!SetMovingSound( pSoundSet , lMaterialIndex, 1, pstSound))
				{
					csAuExcelTxtLib.CloseFile();
					return FALSE;
				}
			}
		}
	}

	csAuExcelTxtLib.CloseFile();
	return TRUE;
}

AgcdUseEffectSetData *AgcmEventEffectData::SearchEmptyCommonCharEffectData(INT32 lCommonCharType)
{
	for (INT32 lCount = 0; lCount < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lCount)
	{
		if (!m_ppCommonCharEffect[lCommonCharType][lCount].m_ulEID)
			return &(m_ppCommonCharEffect[lCommonCharType][lCount]);
	}

	return NULL;
}

// 2007.09.14. steeple
// Map 으로 할까 하다가 그냥 루프 돈다.
AgcdUseEffectSetData *AgcmEventEffectData::SearchCommonCharEffectDataByID(INT32 lID)
{
	if(lID < 0)		return NULL;

	for (INT32 i = 0; i < m_nCommonCharEffectCount; ++i)
	{
		for (INT32 lCount = 0; lCount < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lCount)
		{
			if (m_ppCommonCharEffect[i][lCount].m_lID == 0)
				break;

			if (m_ppCommonCharEffect[i][lCount].m_lID == lID)
				return &(m_ppCommonCharEffect[i][lCount]);
		}
	}

	return NULL;
}

// 2007.09.17. steeple
// Map 으로 할까 하다가 그냥 루프 돈다.
AgcdUseEffectSetData *AgcmEventEffectData::SearchCommonCharEffectDataByEID(INT32 lEID)
{
	if(lEID < 0)
		return NULL;

	for (INT32 i = 0; i < m_nCommonCharEffectCount; ++i)
	{
		for (INT32 lCount = 0; lCount < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lCount)
		{
			if (m_ppCommonCharEffect[i][lCount].m_ulEID == 0)
				break;

			if (m_ppCommonCharEffect[i][lCount].m_ulEID == lEID)
				return &(m_ppCommonCharEffect[i][lCount]);
		}
	}

	return NULL;
}

AgcdUseEffectSetData *AgcmEventEffectData::SearchEmptySpiritStoneAttrEffectData(INT32 lSpiritStoneType, INT32 *plIndex)
{
	for (INT32 lCount = 0; lCount < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lCount)
	{
		if (!m_pastSpiritStoneAttrEffect[lSpiritStoneType][lCount])
		{
			m_pastSpiritStoneAttrEffect[lSpiritStoneType][lCount] = m_csEffectDataAdmin2.Create(lCount);

			*plIndex = lCount;

			return m_pastSpiritStoneAttrEffect[lSpiritStoneType][lCount];
		}
	}

	return NULL;
}

AgcdUseEffectSetData *AgcmEventEffectData::SearchEmptySpiritStoneHitEffectData(INT32 lSpiritStoneType, INT32 lSpiritStoneLevel)
{
	for (INT32 lCount = 0; lCount < AGCM_EVENT_EFFECT_DATA_MAX_EFFECT_DATA; ++lCount)
	{
		if (!m_pastSpiritStoneHitEffect[lSpiritStoneType][lSpiritStoneLevel][lCount])
		{
			m_pastSpiritStoneHitEffect[lSpiritStoneType][lSpiritStoneLevel][lCount] = m_csEffectDataAdmin2.Create(lCount);

			return m_pastSpiritStoneHitEffect[lSpiritStoneType][lSpiritStoneLevel][lCount];
		}
	}

	return NULL;
}

BOOL AgcmEventEffectData::ReadCommonEffectData(AuExcelLib *pcsAuExcelTxtLib, AgcdUseEffectSetData *pstEffectData, INT32 lStartColumn, INT32 lPixKeyRow, INT32 lRow)
{
	CHAR	*pszData	= NULL;
	FLOAT	fTemp		= 0.0f;
	for (INT32 lCol = lStartColumn; lCol < pcsAuExcelTxtLib->GetColumn(); ++lCol)
	{
		pszData	= pcsAuExcelTxtLib->GetData(lCol, lPixKeyRow);
		if (!pszData)
			continue;

		if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_ID))
		{
			pszData = pcsAuExcelTxtLib->GetData(lCol, lRow);
			if (!pszData)
				continue;

			pstEffectData->m_lID				= atoi(pszData);
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_EID))
		{
			pszData = pcsAuExcelTxtLib->GetData(lCol, lRow);
			if (!pszData)
				continue;

			pstEffectData->m_ulEID				= atoi(pszData);
			pstEffectData->m_ulConditionFlags	= AGCDEVENTEFFECT_CONDITION_FLAG_TEMP;
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_X))
		{
			pszData = pcsAuExcelTxtLib->GetData(lCol, lRow);
			if (!pszData)
				continue;

			fTemp	= (FLOAT)(atof(pszData));
			if (fTemp != 0.0f)
			{
				if (!pstEffectData->m_pv3dOffset)
				{
					pstEffectData->m_pv3dOffset		= new RwV3d();
					pstEffectData->m_pv3dOffset->y	= 0.0f;
					pstEffectData->m_pv3dOffset->z	= 0.0f;
				}

				pstEffectData->m_pv3dOffset->x = fTemp;
			}
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_Y))
		{
			pszData = pcsAuExcelTxtLib->GetData(lCol, lRow);
			if (!pszData)
				continue;

			fTemp	= (FLOAT)(atof(pszData));
			if (fTemp != 0.0f)
			{
				if (!pstEffectData->m_pv3dOffset)
				{
					pstEffectData->m_pv3dOffset	= new RwV3d();
					pstEffectData->m_pv3dOffset->x	= 0.0f;
					pstEffectData->m_pv3dOffset->z	= 0.0f;
				}

				pstEffectData->m_pv3dOffset->y = fTemp;
			}
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_Z))
		{
			pszData = pcsAuExcelTxtLib->GetData(lCol, lRow);
			if (!pszData)
				continue;

			fTemp	= (FLOAT)(atof(pszData));
			if (fTemp != 0.0f)
			{
				if (!pstEffectData->m_pv3dOffset)
				{
					pstEffectData->m_pv3dOffset	= new RwV3d();
					pstEffectData->m_pv3dOffset->x	= 0.0f;
					pstEffectData->m_pv3dOffset->y	= 0.0f;
				}

				pstEffectData->m_pv3dOffset->z = fTemp;
			}
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_SCALE))
		{
			pszData = pcsAuExcelTxtLib->GetData(lCol, lRow);
			if (!pszData)
				continue;

			pstEffectData->m_fScale = (FLOAT)(atof(pszData));
			if (pstEffectData->m_fScale == 0.0f)
				pstEffectData->m_fScale		= 1.0f;
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_NODEID))
		{
			pszData = pcsAuExcelTxtLib->GetData(lCol, lRow);
			if (!pszData)
				continue;

			pstEffectData->m_lParentNodeID = atoi(pszData);
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_GAP))
		{
			pszData = pcsAuExcelTxtLib->GetData(lCol, lRow);
			if (!pszData)
				continue;

			pstEffectData->m_ulStartGap = atoi(pszData);
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_RGBA_SCALE))
		{
			INT32	lRed, lGreen, lBlue, lAlpha;
			pszData = pcsAuExcelTxtLib->GetData(lCol, lRow);
			if (!pszData)
				continue;

			sscanf(pszData, "\"%d,%d,%d,%d\"", &lAlpha, &lRed, &lGreen, &lBlue);
			pstEffectData->m_rgbScale.red = lRed;
			pstEffectData->m_rgbScale.green = lGreen;
			pstEffectData->m_rgbScale.blue = lBlue;
			pstEffectData->m_rgbScale.alpha = lAlpha;
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_PARTICLE_SCALE))
		{
			pszData = pcsAuExcelTxtLib->GetData(lCol, lRow);
			if (!pszData)
				continue;

			sscanf(pszData, "%f", &pstEffectData->m_fParticleNumScale);
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_EMITTER_ATOMIC))
		{
			pstEffectData->m_bAtomicEmitter = pcsAuExcelTxtLib->GetDataToInt(lCol, lRow);
		}
		else if (!strcmp(pszData, AGCM_EVENT_EFFECT_DATA_NAME_EMITTER_CLUMP))
		{
			pstEffectData->m_bClumpEmitter = pcsAuExcelTxtLib->GetDataToInt(lCol, lRow);
		}

		// 2007.09.13. steeple
		// 추가저인 Effect ID
		for(INT32 i = 0; i < AGCM_EVENT_EFFECT_DATA_MAX_SUB_EFFECT; ++i)
		{
			TCHAR szColumn[128];
			_stprintf(szColumn, "%s%02d", AGCM_EVENT_EFFECT_DATA_NAME_EFFECT_SUB, i + 1);

			if(!strcmp(pszData, szColumn))
			{
				pstEffectData->m_alSubEffect[i] = pcsAuExcelTxtLib->GetDataToInt(lCol, lRow);
			}
		}
	}

	return TRUE;
}

BOOL AgcmEventEffectData::ReadCommonFXData(AuExcelLib *pcsAuExcelTxtLib, RpMaterial *pstMaterial, INT32 lNumMaterial, INT32 lStartColumn, INT32 lPixKeyRow, INT32 lPixTypeRow, INT32 lRow)
{
	CHAR *						szDataName	= NULL;
	CHAR *						szDataType	= NULL;
	CHAR *						szDataValue = NULL;
	D3DXVECTOR4					stVector4(0, 0, 0, 0);
	UINT32						aulTemp[4]	= {0, 0, 0, 0};
	FLOAT						afTemp[4]	= {0, 0, 0, 0};
	INT32						lFXDataType;
	INT32						lIndex;
	RwTexture *					pstTexture;

	for (INT32 lCol = lStartColumn; lCol < pcsAuExcelTxtLib->GetColumn(); ++lCol)
	{
		szDataName	= pcsAuExcelTxtLib->GetData(lCol, lPixKeyRow);
		if (!szDataName)
			continue;

		szDataType	= pcsAuExcelTxtLib->GetData(lCol, lPixTypeRow);
		if (!szDataType)
			continue;

		szDataValue	= pcsAuExcelTxtLib->GetData(lCol, lRow);
		if (!szDataValue)
			continue;

		for (lFXDataType = E_FX_DATA_TYPE_FX; lFXDataType < E_FX_MAX_DATA_TYPE; ++lFXDataType)
		{
			if (g_aszFXDataTypeName[lFXDataType] && !strcmp(szDataName, g_aszFXDataTypeName[lFXDataType]))
				break;
		}

		ASSERT(lFXDataType < E_FX_MAX_DATA_TYPE);

		switch (lFXDataType)
		{
		case E_FX_DATA_TYPE_FX:
			{
				if (1 <= lNumMaterial)
					RpMaterialD3DFxSetEffectWeight1(pstMaterial + 0, szDataValue);
				if (2 <= lNumMaterial)
					RpMaterialD3DFxSetEffectWeight2(pstMaterial + 1, szDataValue);
				if (3 <= lNumMaterial)
					RpMaterialD3DFxSetEffectWeight3(pstMaterial + 2, szDataValue);
				if (4 <= lNumMaterial)
					RpMaterialD3DFxSetEffectWeight4(pstMaterial + 3, szDataValue);
			}
			break;

		case E_FX_DATA_TYPE_FLOAT:
			{
				afTemp[0]	= (FLOAT) atof(szDataValue);

				for (lIndex = 0; lIndex < lNumMaterial; ++lIndex)
				{
					RpMaterialD3DFxSetFloat(pstMaterial + lIndex, szDataName, afTemp[0]);
				}
			}
			break;

		case E_FX_DATA_TYPE_VECTOR:
			{
				__ParseChar__(szDataValue, ',', '"', (UINT32)(afTemp), sizeof (FLOAT), 4, __atofCB__);
				stVector4.x	= afTemp[0];
				stVector4.y	= afTemp[1];
				stVector4.z	= afTemp[2];
				stVector4.w	= afTemp[3];

				for (lIndex = 0; lIndex < lNumMaterial; ++lIndex)
				{
					RpMaterialD3DFxSetVector(pstMaterial + lIndex, szDataName, &stVector4);
				}
			}
			break;

		case E_FX_DATA_TYPE_TEXTURE:
			{
				pstTexture = RwTextureRead(szDataValue, NULL);

				for (lIndex = 0; lIndex < lNumMaterial; ++lIndex)
				{
					RpMaterialD3DFxSetTexture(pstMaterial + lIndex, szDataName, pstTexture);
				}
			}
			break;
		}
	}

	return TRUE;
}

BOOL AgcmEventEffectData::ReadSpiritStoneEffectData(CHAR *szPathName, BOOL bDecryption, AgcmEventEffectDataParamFunction pfGetEffectSetData)
{
	AuExcelTxtLib	csAuExcelTxtLib;
	if (!csAuExcelTxtLib.OpenExcelFile(szPathName, TRUE, bDecryption))
	{
		csAuExcelTxtLib.CloseFile();
		return FALSE;
	}

	const INT32				lPixLevelColumn				= 0;
	const INT32				lPixSpiritStoneTypeColumn	= 1;
	const INT32				lStartColumn				= 2;
	const INT32				lPixKeyRow					= 0;

	AgcdUseEffectSetData	*pEffectSet					= NULL;
	CHAR					*pszData					= NULL;
	INT32					lLevel						= 0;
	INT32					lSpiritStoneIndex			= 0;
	for (INT32 lRow = 1; lRow < csAuExcelTxtLib.GetRow(); ++lRow)
	{
		pszData		= csAuExcelTxtLib.GetData(lPixLevelColumn, lRow);
		if (!pszData)
			continue;

		lLevel		= atoi(pszData);

		pszData		= csAuExcelTxtLib.GetData(lPixSpiritStoneTypeColumn, lRow);
		if (!pszData)
			continue;

		lSpiritStoneIndex	= SearchSpiritStoneIndex(pszData);
		if (lSpiritStoneIndex == -1)
		{
			csAuExcelTxtLib.CloseFile();
			return FALSE;
		}

		pEffectSet	= (AgcdUseEffectSetData *)(pfGetEffectSetData(this, (PVOID)(&lSpiritStoneIndex), (PVOID)(&lLevel)));
		if (!pEffectSet)
		{
			csAuExcelTxtLib.CloseFile();
			return FALSE;
		}

		ReadCommonEffectData(&csAuExcelTxtLib, pEffectSet, lStartColumn, lPixKeyRow, lRow);
	}

	csAuExcelTxtLib.CloseFile();
	return TRUE;
}

BOOL AgcmEventEffectData::ReadSpiritStoneAttrEffectData(CHAR *szPathName, BOOL bDecryption)
{
	AuExcelTxtLib	csAuExcelTxtLib;
	if (!csAuExcelTxtLib.OpenExcelFile(szPathName, TRUE, bDecryption))
	{
		csAuExcelTxtLib.CloseFile();
		return FALSE;
	}

	const INT32				lPixSpiritStoneTypeColumn	= 0;
	const INT32				lPixSpiritStonePointColumn	= 1;
	const INT32				lStartColumn				= 2;
	const INT32				lPixKeyRow					= 0;

	AgcdUseEffectSetData	*pEffectSet					= NULL;
	CHAR					*pszData					= NULL;
	INT32					lSpiritStoneIndex			= 0;
	INT32					lEmptyIndex					= 0;
	for (INT32 lRow = 1; lRow < csAuExcelTxtLib.GetRow(); ++lRow)
	{
		pszData		= csAuExcelTxtLib.GetData(lPixSpiritStoneTypeColumn, lRow);
		if (!pszData)
			continue;

		lSpiritStoneIndex	= SearchSpiritStoneIndex(pszData);
		if (lSpiritStoneIndex == -1)
		{
			csAuExcelTxtLib.CloseFile();
			return FALSE;
		}

		VERIFY(pEffectSet	= SearchEmptySpiritStoneAttrEffectData(lSpiritStoneIndex, &lEmptyIndex));
		if (!pEffectSet)
		{
			csAuExcelTxtLib.CloseFile();
			return FALSE;
		}

		m_alSpiritStoneAttrPoint[lSpiritStoneIndex][lEmptyIndex] = csAuExcelTxtLib.GetDataToInt(lPixSpiritStonePointColumn, lRow);

		ReadCommonEffectData(&csAuExcelTxtLib, pEffectSet, lStartColumn, lPixKeyRow, lRow);
	}

	csAuExcelTxtLib.CloseFile();
	return TRUE;
}

BOOL AgcmEventEffectData::InitializeAnimation()
{
	if (!m_pszAnimationDataPath)
		return FALSE;

	m_csAnimation2.SetMaxRtAnim(AGCM_EVENT_EFFECT_DATA_MAX_RT_ANIMATION);
	m_csAnimation2.SetAnimationPathAndExtension(m_pszAnimationDataPath, NULL);

	return TRUE;
}

VOID AgcmEventEffectData::ForXport_GetAllSocialAnim( std::vector<AgcdAnimData2*>& container )
{
	container.clear();

	container.reserve( m_vecSocialGen.size() * NS_SOCIAL::AgcdSocialGeneral::num
		+ m_vecSocialLogin.size() * NS_SOCIAL::AgcdSocialLoginMode::num );
	//general 
	{
		typedef NS_SOCIAL::SocialGenVec::iterator ITR;
		for( ITR it_curr = m_vecSocialGen.begin(); it_curr != m_vecSocialGen.end(); ++it_curr )
			for( int i = AGPDCHAR_SOCIAL_TYPE_GREETING; i <= AGPDCHAR_SOCIAL_TYPE_SPECIAL1; ++i )
				if( (*it_curr)->bGetAnim(static_cast<AgpdCharacterSocialType>(i)) )
					container.push_back( (*it_curr)->bGetAnim(static_cast<AgpdCharacterSocialType>(i))->m_pcsHead );
	}

	//login
	{
		typedef NS_SOCIAL::SocialLoginVec::iterator ITR;			
		for( ITR it_curr = m_vecSocialLogin.begin(); it_curr != m_vecSocialLogin.end(); ++it_curr )
			for( int i=AGPDCHAR_SOCIAL_TYPE_SELECT1; i <= AGPDCHAR_SOCIAL_TYPE_SELECT10_BACK; ++i )
				if( (*it_curr)->bGetAnim( static_cast<AgpdCharacterSocialType>(i) ) )
					container.push_back( (*it_curr)->bGetAnim( static_cast<AgpdCharacterSocialType>(i))->m_pcsHead );
	}

	//GM
	{
		typedef NS_SOCIAL::SocialGMVec::iterator ITR;
		for( ITR it_curr = m_vecSocialGM.begin(); it_curr != m_vecSocialGM.end(); ++it_curr )
			for( int i=AGPDCHAR_SOCIAL_TYPE_GM_GREETING; i <= AGPDCHAR_SOCIAL_TYPE_GM_HAPPY; ++i )
				if( (*it_curr)->bGetAnim( static_cast<AgpdCharacterSocialType>(i) ) )
					container.push_back( (*it_curr)->bGetAnim( static_cast<AgpdCharacterSocialType>(i))->m_pcsHead );
	}
}

BOOL AgcmEventEffectData::ReadSocialAnimationData(CHAR *szSocialPC, CHAR *szSocialGM, CHAR *szDataPath, BOOL bDecryption)
{
	if (m_pszAnimationDataPath) return FALSE;

	m_pszAnimationDataPath	= new CHAR[strlen(szDataPath) + 1];
	if (!m_pszAnimationDataPath) return FALSE;

	strcpy(m_pszAnimationDataPath, szDataPath);
	InitializeAnimation();

	using namespace NS_SOCIAL;
	const INT32 startCol	= 2;
	const INT32 TIDRow		= 0;
	INT32		startRow	= 2;

	CHAR		*buffAnim	= NULL;
	CHAR		*buffSound	= NULL;
	CHAR		*buffTID	= NULL;
	INT32		TID			= 0;
	AgpdCharacterSocialType socialType	= AGPDCHAR_SOCIAL_TYPE_MAX;

	AuExcelTxtLib	csAuExcelTxtLib;
	csAuExcelTxtLib.OpenExcelFile( szSocialPC, TRUE, bDecryption );

	for( INT32 col = startCol; col < csAuExcelTxtLib.GetColumn(); col += 2 )
	{
		buffTID = csAuExcelTxtLib.GetData(col, TIDRow);
		//ASSERT( buffTID );
		if( !buffTID ) continue;

		TID = atoi(buffTID);

		startRow = 2;//2위에 2줄
		AgcdSocialGeneral* pSocialGeneral = new AgcdSocialGeneral(TID);
		ASSERT( pSocialGeneral );
		if( pSocialGeneral )
		{
			for( INT32 row = startRow; row < AgcdSocialGeneral::num + startRow; ++row )
			{
				buffAnim	= csAuExcelTxtLib.GetData(col, row);
				buffSound	= csAuExcelTxtLib.GetData(col+1, row);

				socialType	= static_cast<AgpdCharacterSocialType>(row - startRow);
				ASSERT( socialType <= AGPDCHAR_SOCIAL_TYPE_SPECIAL1 && socialType >= AGPDCHAR_SOCIAL_TYPE_GREETING );

				pSocialGeneral->bGetSocial().bSetData( socialType, buffAnim, buffSound, m_csAnimation2 );
			}

			m_vecSocialGen.push_back(pSocialGeneral);
		}

		startRow = startRow + AgcdSocialGeneral::num + 1/*구분선*/;
		AgcdSocialLoginMode* pSocialLogin = new AgcdSocialLoginMode(TID);
		ASSERT( pSocialLogin );
		if( pSocialLogin )
		{
			for( INT32 row = startRow; row < AgcdSocialLoginMode::num + startRow; ++row )
			{
				buffAnim	= csAuExcelTxtLib.GetData(col, row);
				buffSound	= csAuExcelTxtLib.GetData(col+1, row);

				socialType	= static_cast<AgpdCharacterSocialType>(row - 3/*2(처음 2줄) + 1(빈칸)*/);
				ASSERT( socialType <= AGPDCHAR_SOCIAL_TYPE_SELECT10_BACK && socialType >= AGPDCHAR_SOCIAL_TYPE_SELECT1 );

				pSocialLogin->bGetSocial().bSetData( socialType, buffAnim, buffSound, m_csAnimation2 );
			}
			m_vecSocialLogin.push_back( pSocialLogin );
		}
	}

	csAuExcelTxtLib.CloseFile();

	csAuExcelTxtLib.OpenExcelFile( szSocialGM, TRUE, bDecryption );

	for( INT32 col = startCol; col < csAuExcelTxtLib.GetColumn(); col += 2 )
	{
		buffTID = csAuExcelTxtLib.GetData(col, TIDRow);
		if( !buffTID ) continue;

		TID = atoi(buffTID);
		//find tid for checking clean data

		startRow = 2;//2위에 2줄
		AgcdSocialForGM* pSocialGM = new AgcdSocialForGM(TID);
		ASSERT( pSocialGM );
		if( pSocialGM )
		{
			for( INT32 row = startRow; row < AgcdSocialForGM::num + startRow; ++row )
			{
				buffAnim	= csAuExcelTxtLib.GetData(col, row);
				buffSound	= csAuExcelTxtLib.GetData(col+1, row);

				socialType	= static_cast<AgpdCharacterSocialType>(row - startRow + AGPDCHAR_SOCIAL_TYPE_GM_GREETING);
				ASSERT( socialType <= AGPDCHAR_SOCIAL_TYPE_GM_HAPPY && socialType >= AGPDCHAR_SOCIAL_TYPE_GM_GREETING );
				pSocialGM->bGetSocial().bSetData( socialType, buffAnim, buffSound, m_csAnimation2 );
			}

			m_vecSocialGM.push_back( pSocialGM );
		}
	}

	csAuExcelTxtLib.CloseFile();

	return TRUE;
}

BOOL AgcmEventEffectData::ReadSpiritStoneHitEffectData(CHAR *szPathName, BOOL bDecryption)
{
	AuExcelTxtLib	csAuExcelTxtLib;

	if (!csAuExcelTxtLib.OpenExcelFile(szPathName, TRUE, bDecryption))
	{
		csAuExcelTxtLib.CloseFile();
		return FALSE;
	}

	const INT32				lPixLevelColumn				= 0;
	const INT32				lPixSpiritStoneTypeColumn	= 1;
	const INT32				lStartColumn				= 2;
	const INT32				lPixKeyRow					= 0;

	AgcdUseEffectSetData	*pEffectSet					= NULL;
	CHAR					*pszData					= NULL;
	INT32					lLevel						= 0;
	INT32					lSpiritStoneIndex			= 0;
	for (INT32 lRow = 1; lRow < csAuExcelTxtLib.GetRow(); ++lRow)
	{
		pszData		= csAuExcelTxtLib.GetData(lPixLevelColumn, lRow);
		if (!pszData)
			continue;

		lLevel		= atoi(pszData) - 1;		// One base -> Zero base

		pszData		= csAuExcelTxtLib.GetData(lPixSpiritStoneTypeColumn, lRow);
		if (!pszData)
			continue;

		lSpiritStoneIndex	= SearchSpiritStoneIndex(pszData);
		if (lSpiritStoneIndex == -1)
		{
			csAuExcelTxtLib.CloseFile();
			return FALSE;
		}

		pEffectSet	= SearchEmptySpiritStoneHitEffectData(lSpiritStoneIndex, lLevel);
		if (!pEffectSet)
		{
			csAuExcelTxtLib.CloseFile();
			return FALSE;
		}

		ReadCommonEffectData(&csAuExcelTxtLib, pEffectSet, lStartColumn, lPixKeyRow, lRow);
	}

	csAuExcelTxtLib.CloseFile();
	return TRUE;
}

AgcdEventEffectSound *AgcmEventEffectData::GetAttackSound(INT32 lWeaponType)
{
	if(lWeaponType > -1)
		return m_pacsAttackSound[lWeaponType];
	else if(lWeaponType == -1)
		return m_pcsDefaultAttackSound;

	return NULL;
}

AgcdEventEffectSound *AgcmEventEffectData::GetHitSound(INT32 lWeaponType, INT32 lIndex)
{
	if(lWeaponType > -1)
	{
		if (lIndex == -1)
			lIndex = m_csRandom.randInt(m_alNumHitSound[lWeaponType] - 1);

		return m_pacsHitSound[lWeaponType][lIndex];
	}
	else if(lWeaponType == -1)
	{
		if (lIndex == -1)
			lIndex = m_csRandom.randInt(m_lNumDefaultHitSound - 1);

		return m_pacsDefaultHitSound[lIndex];
	}

	return NULL;
}

AgcdEventEffectSound *AgcmEventEffectData::GetMovingSound( stMovingSoundSet * pSoundSet , INT32 lMaterialIndex, INT32 lFootIndex)
{
	return pSoundSet->pacsMovingSound[lMaterialIndex][lFootIndex];
}

BOOL AgcmEventEffectData::SetAttackSound(INT32 lWeaponType, AgcdEventEffectSound *pstSound)
{
	if(lWeaponType > -1)
		m_pacsAttackSound[lWeaponType]	= pstSound;
	else if(lWeaponType == -1)
		m_pcsDefaultAttackSound			= pstSound;
	else
		return FALSE;

	return TRUE;
}

BOOL AgcmEventEffectData::SetHitSound(INT32 lWeaponType, INT32 lIndex, AgcdEventEffectSound *pstSound)
{
	if(lWeaponType > -1)
		m_pacsHitSound[lWeaponType][lIndex]	= pstSound;
	else if(lWeaponType == -1)
		m_pacsDefaultHitSound[lIndex]		= pstSound;
	else
		return FALSE;

	return TRUE;
}

BOOL AgcmEventEffectData::SetMovingSound( stMovingSoundSet * pSoundSet , INT32 lMaterialIndex, INT32 lFootIndex, AgcdEventEffectSound *pstSound)
{
	pSoundSet->pacsMovingSound[lMaterialIndex][lFootIndex] = pstSound;

	return TRUE;
}

VOID AgcmEventEffectData::SetNumHitSound()
{
	INT32 lWeaponType	= 0;
	INT32 lNumHitSound	= 0;

	for (lWeaponType = 0; lWeaponType < AGPMITEM_EQUIP_WEAPON_TYPE_NUM; ++lWeaponType)
	{
		for (lNumHitSound = 0; lNumHitSound < AGCM_EVENT_EFFECT_DATA_SOUND_HIT_SOUND_NUM; ++lNumHitSound)
		{
			if (!m_pacsHitSound[lWeaponType][lNumHitSound])
				break;
		}

		m_alNumHitSound[lWeaponType] = lNumHitSound;
	}

	for (lNumHitSound = 0; lNumHitSound < AGCM_EVENT_EFFECT_DATA_SOUND_HIT_SOUND_NUM; ++lNumHitSound)
	{
		if (!m_pacsDefaultHitSound[lNumHitSound])
			break;
	}

	m_lNumDefaultHitSound = lNumHitSound;
}

AgcdUseEffectSetData *AgcmEventEffectData::GetConvertedWeaponGradeEffectData(INT32 lNumConverted)
{
	if (lNumConverted < 1) // ~ 0
	{ // error
		return NULL;
	}
	else if (lNumConverted < 3) // 1 ~ 2
	{
		return m_pastConvertedWeaponGradeEffect[0];
	}
	else if (lNumConverted < 5) // 3 ~ 4
	{
		return m_pastConvertedWeaponGradeEffect[1];
	}
	else if (lNumConverted < 7) // 5 ~ 6
	{
		return m_pastConvertedWeaponGradeEffect[2];
	}
	else if (lNumConverted < 9) // 7 ~ 8
	{
		return m_pastConvertedWeaponGradeEffect[3];
	}
	else if (lNumConverted < 11) // 9 ~ 10
	{
		return m_pastConvertedWeaponGradeEffect[4];
	}

	return NULL;
}

AgcdUseEffectSetData *AgcmEventEffectData::GetCommonCharEffectData(INT32 lCommonCharType, INT32 lIndex)
{
	return &(m_ppCommonCharEffect[lCommonCharType][lIndex]);
}

AgcdUseEffectSetData *AgcmEventEffectData::GetSpiritStoneAttrEffectData(INT32 lSpiritStoneType, INT32 lIndex)
{
	return m_pastSpiritStoneAttrEffect[lSpiritStoneType][lIndex];
}

INT32 AgcmEventEffectData::GetSpiritStoneAttrPointData(INT32 lSpiritStoneType, INT32 lIndex)
{
	return m_alSpiritStoneAttrPoint[lSpiritStoneType][lIndex];
}

AgcdUseEffectSetData *AgcmEventEffectData::GetSpiritStoneHitEffectData(INT32 lSpiritStoneType, INT32 lSpiritStoneLevel, INT32 lIndex)
{
	return m_pastSpiritStoneHitEffect[lSpiritStoneType][lSpiritStoneLevel][lIndex];
}

AgcdUseEffectSetData *AgcmEventEffectData::GetSpiritStoneHitEffectData(INT32 lNumSpiritStone,
																	   INT32 lRank,
																	   INT32 lLevel,
																	   INT32 lNumConvertedWaterAttr,
																	   INT32 lNumConvertedFireAttr,
																	   INT32 lNumConvertedEarthAttr,
																	   INT32 lNumConvertedAirAttr,
																	   INT32 lNumConvertedMagicAttr,
																	   INT32 lIndex					)
{
	INT32	lActiveRate	= 30 + 29 * (lNumSpiritStone - 1) /  9 + 11 * (lRank - 1) / 4;
	if (lActiveRate < (m_csRandom.randInt(100) + 1))
		return NULL; // skip

	INT32	lWaterRate	= (INT32)((FLOAT)(lNumConvertedWaterAttr) / (FLOAT)(lNumSpiritStone) * 100);
	INT32	lFireRate	= (INT32)((FLOAT)(lNumConvertedFireAttr) / (FLOAT)(lNumSpiritStone) * 100) + lWaterRate;
	INT32	lEarthRate	= (INT32)((FLOAT)(lNumConvertedEarthAttr) / (FLOAT)(lNumSpiritStone) * 100) + lFireRate;
	INT32	lAirRate	= (INT32)((FLOAT)(lNumConvertedAirAttr) / (FLOAT)(lNumSpiritStone) * 100) + lEarthRate;
	INT32	lMagicRate	= (INT32)((FLOAT)(lNumConvertedMagicAttr) / (FLOAT)(lNumSpiritStone) * 100) + lAirRate;

	INT32	lRand		= m_csRandom.randInt(100);

	if (lRand < lWaterRate)
	{
		return m_pastSpiritStoneHitEffect[E_SPIRIT_STONE_TYPE_WATER][lLevel - 1][lIndex];
	}
	else if (lRand < lFireRate)
	{
		return m_pastSpiritStoneHitEffect[E_SPIRIT_STONE_TYPE_FIRE][lLevel - 1][lIndex];
	}
	else if (lRand < lEarthRate)
	{
		return m_pastSpiritStoneHitEffect[E_SPIRIT_STONE_TYPE_EARTH][lLevel - 1][lIndex];
	}
	else if (lRand < lAirRate)
	{
		return m_pastSpiritStoneHitEffect[E_SPIRIT_STONE_TYPE_AIR][lLevel - 1][lIndex];
	}
	else if (lRand < lMagicRate)
	{
		return m_pastSpiritStoneHitEffect[E_SPIRIT_STONE_TYPE_MAGIC][lLevel - 1][lIndex];
	}

	return NULL;
}

INT32 AgcmEventEffectData::GetSocialTypeIndex(CHAR *szSocialType)
{
	if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_GREETING))
		return AGPDCHAR_SOCIAL_TYPE_GREETING;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_CELEBRATION))
		return AGPDCHAR_SOCIAL_TYPE_CELEBRATION;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_GRATITUDE))
		return AGPDCHAR_SOCIAL_TYPE_GRATITUDE;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_ENCOURAGEMENT))
		return AGPDCHAR_SOCIAL_TYPE_ENCOURAGEMENT;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_DISREGARD))
		return AGPDCHAR_SOCIAL_TYPE_DISREGARD;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_DANCING))
		return AGPDCHAR_SOCIAL_TYPE_DANCING;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_DOZINESS))
		return AGPDCHAR_SOCIAL_TYPE_DOZINESS;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_STRETCH))
		return AGPDCHAR_SOCIAL_TYPE_STRETCH;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_LAUGH))
		return AGPDCHAR_SOCIAL_TYPE_LAUGH;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_WEEPING))
		return AGPDCHAR_SOCIAL_TYPE_WEEPING;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_RAGE))
		return AGPDCHAR_SOCIAL_TYPE_RAGE;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_POUT))
		return AGPDCHAR_SOCIAL_TYPE_POUT;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_APOLOGY))
		return AGPDCHAR_SOCIAL_TYPE_APOLOGY;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_TOAST))
		return AGPDCHAR_SOCIAL_TYPE_TOAST;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_CHEER))
		return AGPDCHAR_SOCIAL_TYPE_CHEER;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_RUSH))
		return AGPDCHAR_SOCIAL_TYPE_RUSH;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SIT))
		return AGPDCHAR_SOCIAL_TYPE_SIT;
	else if (!strcmp(szSocialType, AGCM_EVNET_EFFECT_SOCIAL_TYPE_NAME_OKMARIO))
		return AGPDCHAR_SOCIAL_TYPE_SPECIAL1;

	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT1))
		return AGPDCHAR_SOCIAL_TYPE_SELECT1;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT2))
		return AGPDCHAR_SOCIAL_TYPE_SELECT2;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT3))
		return AGPDCHAR_SOCIAL_TYPE_SELECT3;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT4))
		return AGPDCHAR_SOCIAL_TYPE_SELECT4;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT5))
		return AGPDCHAR_SOCIAL_TYPE_SELECT5;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT6))
		return AGPDCHAR_SOCIAL_TYPE_SELECT6;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT7))
		return AGPDCHAR_SOCIAL_TYPE_SELECT7;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT8))
		return AGPDCHAR_SOCIAL_TYPE_SELECT8;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT9))
		return AGPDCHAR_SOCIAL_TYPE_SELECT9;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT10))
		return AGPDCHAR_SOCIAL_TYPE_SELECT10;
	
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT1 AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK))
		return AGPDCHAR_SOCIAL_TYPE_SELECT1_BACK;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT2 AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK))
		return AGPDCHAR_SOCIAL_TYPE_SELECT2_BACK;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT3 AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK))
		return AGPDCHAR_SOCIAL_TYPE_SELECT3_BACK;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT4 AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK))
		return AGPDCHAR_SOCIAL_TYPE_SELECT4_BACK;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT5 AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK))
		return AGPDCHAR_SOCIAL_TYPE_SELECT5_BACK;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT6 AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK))
		return AGPDCHAR_SOCIAL_TYPE_SELECT6_BACK;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT7 AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK))
		return AGPDCHAR_SOCIAL_TYPE_SELECT7_BACK;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT8 AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK))
		return AGPDCHAR_SOCIAL_TYPE_SELECT8_BACK;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT9 AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK))
		return AGPDCHAR_SOCIAL_TYPE_SELECT9_BACK;
	else if (!strcmp(szSocialType, AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT10 AGCM_EVENT_EFFECT_SOCIAL_TYPE_NAME_SELECT_BACK))
		return AGPDCHAR_SOCIAL_TYPE_SELECT10_BACK;

	ASSERT( !"unknow social type!" );
	return -1;
}

INT32 AgcmEventEffectData::GetRaceIndex(CHAR *szRace)
{
	if (!strcmp(szRace, AGCM_EVENT_EFFECT_RACE_NAME_HUMAN))
		return AURACE_TYPE_HUMAN - 1;
	else if (!strcmp(szRace, AGCM_EVENT_EFFECT_RACE_NAME_ORC))
		return AURACE_TYPE_ORC - 1;
	else if (!strcmp(szRace, AGCM_EVENT_EFFECT_RACE_NAME_MOONELF))
		return AURACE_TYPE_MOONELF - 1;
	else if (!strcmp(szRace, AGCM_EVENT_EFFECT_RACE_NAME_DRAGONSCION))
		return AURACE_TYPE_DRAGONSCION - 1;

	return -1;
}

INT32 AgcmEventEffectData::GetClassIndex(CHAR *szClass)
{
	if (!strcmp(szClass, AGCM_EVENT_EFFECT_CLASS_NAME_KNIGHT))
		return AUCHARCLASS_TYPE_KNIGHT - 1;
	else if (!strcmp(szClass, AGCM_EVENT_EFFECT_CLASS_NAME_RANGER))
		return AUCHARCLASS_TYPE_RANGER - 1;
	else if (!strcmp(szClass, AGCM_EVENT_EFFECT_CLASS_NAME_MONK))
		return AUCHARCLASS_TYPE_SCION - 1;
	else if (!strcmp(szClass, AGCM_EVENT_EFFECT_CLASS_NAME_MAGE))
		return AUCHARCLASS_TYPE_MAGE - 1;

	return -1;
}

INT32 AgcmEventEffectData::GetGenderIndex(CHAR *szGender)
{
	if (!strcmp(szGender, AGCM_EVENT_EFFECT_GENDER_NAME_MALE))
		return AUCHARGENDER_TYPE_MALE - 1;
	else if (!strcmp(szGender, AGCM_EVENT_EFFECT_GENDER_NAME_FEMALE))
		return AUCHARGENDER_TYPE_FEMALE - 1;

	return -1;
}

AgcdAnimData2* AgcmEventEffectData::GetSocialAnim(INT32 tid, AgpdCharacterSocialType socialtype)
{
	ASSERT( socialtype >= AGPDCHAR_SOCIAL_TYPE_GREETING );
	ASSERT( socialtype <  AGPDCHAR_SOCIAL_TYPE_MAX );

	if( socialtype < AGPDCHAR_SOCIAL_TYPE_SELECT1 )//일반
		return NS_SOCIAL::FindAndGetAnim<NS_SOCIAL::SocialGenVec, NS_SOCIAL::AgcdSocialGeneral>( m_vecSocialGen, tid, socialtype, m_csAnimation2);
	else if( socialtype <= AGPDCHAR_SOCIAL_TYPE_SELECT10_BACK )//로긴시에만
		return NS_SOCIAL::FindAndGetAnim< NS_SOCIAL::SocialLoginVec, NS_SOCIAL::AgcdSocialLoginMode >( m_vecSocialLogin, tid, socialtype, m_csAnimation2 );
	else if( socialtype <= AGPDCHAR_SOCIAL_TYPE_GM_HAPPY ) // for GM
		return NS_SOCIAL::FindAndGetAnim< NS_SOCIAL::SocialGMVec, NS_SOCIAL::AgcdSocialForGM >( m_vecSocialGM, tid, socialtype, m_csAnimation2 );

	return NULL;
}

LPCSTR AgcmEventEffectData::GetSocialSound(INT32 tid, AgpdCharacterSocialType socialtype)
{	
	ASSERT( socialtype >= AGPDCHAR_SOCIAL_TYPE_GREETING );
	ASSERT( socialtype <  AGPDCHAR_SOCIAL_TYPE_MAX );

	if( socialtype < AGPDCHAR_SOCIAL_TYPE_SELECT1 )//일반
		return NS_SOCIAL::FindAndGetSound<NS_SOCIAL::SocialGenVec, NS_SOCIAL::AgcdSocialGeneral>( m_vecSocialGen, tid, socialtype );
	else if( socialtype <= AGPDCHAR_SOCIAL_TYPE_SELECT10_BACK )//로긴시에만
		return NS_SOCIAL::FindAndGetSound< NS_SOCIAL::SocialLoginVec, NS_SOCIAL::AgcdSocialLoginMode >( m_vecSocialLogin, tid, socialtype );
	else if( socialtype <= AGPDCHAR_SOCIAL_TYPE_GM_HAPPY ) // for GM
		return NS_SOCIAL::FindAndGetSound< NS_SOCIAL::SocialGMVec, NS_SOCIAL::AgcdSocialForGM >( m_vecSocialGM, tid, socialtype );

	return NULL;
}

VOID AgcmEventEffectData::RemoveLoginSocial(void)
{
	NS_SOCIAL::RemoveAllSocial(m_vecSocialLogin, m_csAnimation2);
};

VOID AgcmEventEffectData::RemoveAllSocialAnimation()
{
	NS_SOCIAL::RemoveAllSocial(m_vecSocialGen, m_csAnimation2);
	NS_SOCIAL::RemoveAllSocial(m_vecSocialLogin, m_csAnimation2);
	NS_SOCIAL::RemoveAllSocial(m_vecSocialGM, m_csAnimation2);
}

AgcmEventEffectData::stMovingSoundSet		* AgcmEventEffectData::GetMovingSoundSet( INT32 nTID )
{
	// 해당 템플릿 ID와 일치하는 사운드 셋을 골라낸다.
	for( int i = 0 ; i < ( int ) m_vecMovingSoundSet.size() ; i++ )
	{
		if( m_vecMovingSoundSet[ i ].nTID == nTID )
			return &m_vecMovingSoundSet[ i ];
	}

	// 없는 경우는!..
	// 새로하나 만들어서 넣어 리턴.
	stMovingSoundSet	newset;
	newset.nTID = nTID;
	m_vecMovingSoundSet.push_back( newset );

	return  &m_vecMovingSoundSet[ m_vecMovingSoundSet.size() - 1 ];
}