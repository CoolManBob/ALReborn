/******************************************************************************
Module:  AgpmItem.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 16
******************************************************************************/

#include <stdio.h>
#include "AgpmItem.h"
#include "ApModuleStream.h"
#include "AuPackingManager.h"
#include "AuExcelBinaryLib.h"

#pragma warning( push )
#pragma warning( disable : 4309 )

CHAR GeldName[4][5] =
{
	{ 0xb0, 0xd6, 0xb5, 0xe5, 0x00 },	// korea
	{ 0xbd, 0xf0, 0xb1, 0xd2, 0x00 },	// china
	{ 'G', 'e', 'l', 'd', 0x00 },		// western
	{ 0x00, 0x00, 0x00, 0x00, 0x00 }	// japan
};

#pragma warning( pop )

BOOL AgpmItem::CharTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpmItem					*pThis = (AgpmItem *) pClass;
	AgpdCharacterTemplate		*pcsCharacterTemplate = (AgpdCharacterTemplate *) pData;
	AgpdItemADCharTemplate		*pcsItem = pThis->GetADCharacterTemplate(pcsCharacterTemplate);
	INT16						nIndex;
	CHAR						szName[128];
	CHAR						szValue[128];

	for (nIndex = 0; nIndex < AGPMITEM_PART_NUM; nIndex++)
	{
		if (pcsItem->m_lDefaultEquipITID[nIndex])
		{
			INT32	lStrlen	= sprintf(szName, "%s%d", AGPMITEM_INI_NAME_DEFAULT_ITEM, nIndex);
			ASSERT(lStrlen < 128);
			lStrlen	= sprintf(szValue, "%d:%d", nIndex, pcsItem->m_lDefaultEquipITID[nIndex]);
			ASSERT(lStrlen < 128);
			if (!pStream->WriteValue(szName, szValue))
			{
				TRACE("AgpmItem::CharTemplateWriteCB() Error (1) !!!\n");
				return FALSE;
			}
		}
	}

	for(nIndex = 0; nIndex < AGPMITEM_DEFAULT_INVEN_NUM; ++nIndex)
	{
		if(pcsItem->m_aalDefaultInvenInfo[nIndex][AGPDITEM_DEFAULT_INVEN_INDEX_TID])
		{
			INT32	lStrlen	= sprintf(szName, "%s%d", AGPMITEM_INI_NAME_DEFAULT_INVEN_INFO, nIndex);
			ASSERT(lStrlen < 128);
			lStrlen	= sprintf(
				szValue,
				"%d:%d:%d",
				nIndex,
				pcsItem->m_aalDefaultInvenInfo[nIndex][AGPDITEM_DEFAULT_INVEN_INDEX_TID],
				pcsItem->m_aalDefaultInvenInfo[nIndex][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT]	);
			ASSERT(lStrlen < 128);

			if(!pStream->WriteValue(szName, szValue))
			{
				TRACE("AgpmItem::CharTemplateWriteCB() Error (2) !!!\n");
				return FALSE;
			}
		}
	}

	if (!pStream->WriteValue(AGPMITEM_INI_NAME_DEFAULT_INVEN_INFO, pcsItem->m_lDefaultMoney))
	{
		TRACE("AgpmItem::CharTemplateWriteCB() Error (3) !!!\n");
		return FALSE;
	}

	return TRUE;
}

BOOL AgpmItem::CharTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	INT32						lTemp1, lTemp2, lTemp3;
	const CHAR					*szValueName;
	CHAR						szValue[128];
	AgpmItem *pThis = (AgpmItem *) pClass;
	AgpdCharacterTemplate		*pcsCharacterTemplate = (AgpdCharacterTemplate *) pData;

	AgpdItemADCharTemplate		*pcsItem = pThis->GetADCharacterTemplate(pcsCharacterTemplate);

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strncmp(szValueName, AGPMITEM_INI_NAME_DEFAULT_ITEM, strlen(AGPMITEM_INI_NAME_DEFAULT_ITEM)))
		{
			pStream->GetValue(szValue, 128);
			if (!pThis->ParseADCharTemplate(pcsItem, szValue))
			{
				TRACE("AgpmItem::CharTemplateReadCB() Error (1) !!!\n");
				return FALSE;
			}
		}
		else
		if(!strncmp(szValueName, AGPMITEM_INI_NAME_DEFAULT_INVEN_INFO, strlen(AGPMITEM_INI_NAME_DEFAULT_INVEN_INFO)))
		{
			pStream->GetValue(szValue, 128);

			sscanf(szValue, "%d:%d:%d", &lTemp1, &lTemp2, &lTemp3);

			if (lTemp2 > 0)
				pcsItem->m_aalDefaultInvenInfo[lTemp1][AGPDITEM_DEFAULT_INVEN_INDEX_TID]			= lTemp2;
			if (lTemp3 > 0)
				pcsItem->m_aalDefaultInvenInfo[lTemp1][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT]	= lTemp3;
		}
		else
		if(!strncmp(szValueName, AGPMITEM_INI_NAME_DEFAULT_INVEN_INFO, strlen(AGPMITEM_INI_NAME_DEFAULT_INVEN_INFO)))
		{
			pStream->GetValue(szValue, 128);

			pcsItem->m_lDefaultMoney	= atoi(szValue);
		}
	}

	return TRUE;
}

BOOL AgpmItem::ParseADCharTemplate(AgpdItemADCharTemplate *pcsItem, CHAR *szValue)
{
	INT16	nIndex = 0;
	INT16	nLength;
	INT16	nPartIndex = -1;

	nLength = (INT16)strlen(szValue);

	for (; nIndex < nLength; nIndex++)
	{
		if (szValue[nIndex] >= '0' && szValue[nIndex] <= '9')
		{
			nPartIndex = atoi(szValue + nIndex);
			break;
		}
	}

	if (nPartIndex < AGPMITEM_PART_BODY || nPartIndex >= AGPMITEM_PART_NUM)
		return FALSE;

	for (; nIndex < nLength; nIndex++)
	{
		if (!(szValue[nIndex] >= '0' && szValue[nIndex] <= '9'))
			break;
	}

	for (; nIndex < nLength; nIndex++)
	{
		if ((szValue[nIndex] >= '0' && szValue[nIndex] <= '9'))
		{
			pcsItem->m_lDefaultEquipITID[nPartIndex] = atoi(szValue + nIndex);
			break;
		}
	}

	return TRUE;
}

BOOL AgpmItem::TemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpdItemTemplate					*pcsTemplate		= (AgpdItemTemplate *) pData;
	AgpdItemTemplateEquip				*pcsTemplateEquip	= (AgpdItemTemplateEquip *) pcsTemplate;
	AgpdItemTemplateUsable				*pcsTemplateUsable	= (AgpdItemTemplateUsable *) pcsTemplate;
	AgpdItemTemplateOther				*pcsTemplateOther	= (AgpdItemTemplateOther *) pcsTemplate;
	AgpdItemTemplateUsableSpiritStone	*pcsTemplateSpirit	= (AgpdItemTemplateUsableSpiritStone *) pcsTemplate;

	AgpmItem				*pThis = (AgpmItem *) pClass;

	// Character Template의 값들을 Write한다.
	if (!pStream->WriteValue(AGPMITEM_INI_NAME_NAME, pcsTemplate->m_szName.c_str()))
	{
		TRACE("AgpmItem::TemplateWriteCB() Error (1) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMITEM_INI_NAME_TYPE, pcsTemplate->m_nType))
	{
		TRACE("AgpmItem::TemplateWriteCB() Error (2) !!!\n");
		return FALSE;
	}

	// Add 092302 Bob Jung.-인벤토리에 써먹을라공...
	if(!pStream->WriteValue(AGPMITEM_INI_NAME_SIZE_WIDTH, pcsTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_WIDTH]))
	{
		TRACE("AgpmItem::TemplateWriteCB() Error (3) !!!\n");
		return FALSE;
	}
	// Add 092302 Bob Jung.-인벤토리에 써먹을라공...
	if(!pStream->WriteValue(AGPMITEM_INI_NAME_SIZE_HEIGHT, pcsTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_HEIGHT]))
	{
		TRACE("AgpmItem::TemplateWriteCB() Error (4) !!!\n");
		return FALSE;
	}

	// Add 060503 Bob Jung.-Stackable관련...
	if(pcsTemplate->m_bStackable)
	{
		if(!pStream->WriteValue(AGPMITEM_INI_NAME_IS_STACKABLE, 1))
		{
			TRACE("AgpmItem::TemplateWriteCB() Error (5) !!!\n");
			return FALSE;
		}

		if(!pStream->WriteValue(AGPMITEM_INI_NAME_MAX_STACKABLE_COUNT, pcsTemplate->m_lMaxStackableCount))
		{
			TRACE("AgpmItem::TemplateWriteCB() Error (6) !!!\n");
			return FALSE;
		}
	}

	// factor 데이타를 쓴다.
	pThis->m_pagpmFactors->StreamWrite(&pcsTemplate->m_csFactor, pStream);

	if (!pStream->WriteValue(AGPMITEM_INI_NAME_RESTRICT_FACTOR, 0))
	{
		TRACE("AgpmItem::TemplateWriteCB() Error (write AGPMITEM_INI_NAME_RESTRICT_FACTOR) !!!\n");
		return FALSE;
	}

	pThis->m_pagpmFactors->StreamWrite(&pcsTemplate->m_csRestrictFactor, pStream);

	switch (pcsTemplate->m_nType)
	{
	case AGPMITEM_TYPE_EQUIP:
		{
			if( !pStream->WriteValue(AGPMITEM_INI_NAME_PART, pcsTemplateEquip->m_nPart))
			{
				TRACE("AgpmItem::TemplateWriteCB() Error (5) !!!\n");
				return FALSE;
			}

			if( !pStream->WriteValue(AGPMITEM_INI_NAME_KIND, pcsTemplateEquip->m_nKind))
			{
				TRACE("AgpmItem::TemplateWriteCB() Error (6) !!!\n");
				return FALSE;
			}

			if( pcsTemplateEquip->m_nKind)
			{
				AgpdItemTemplateEquipWeapon *pcsWeapon = (AgpdItemTemplateEquipWeapon *)(pcsTemplateEquip);
				if (!pStream->WriteValue(AGPMITEM_INI_NAME_WeaponType, pcsWeapon->m_nWeaponType))
				{
					TRACE("AgpmItem::TemplateWriteCB() Error (8) !!!\n");
					return FALSE;
				}
			}

			if( pcsTemplateEquip->m_lightInfo.GetType() )
			{
				// 라이트 정보 기록함...
				char	str[ 256 ];

				pcsTemplateEquip->m_lightInfo.MakeString( str );

				if (!pStream->WriteValue(AGPMITEM_INI_NAME_LightInfo,str))
				{
					TRACE("AgpmItem::TemplateWriteCB() Error AGPMITEM_INI_NAME_LightInfo !!!\n");
					return FALSE;
				}
			}
		}
		break;
	case AGPMITEM_TYPE_USABLE:
		{
			if (!pStream->WriteValue(AGPMITEM_INI_NAME_USABLE_TYPE, pcsTemplateUsable->m_nUsableItemType))
			{
				TRACE("AgpmItem::TemplateWriteCB() Error (20) !!!\n");
				return FALSE;
			}

			if (!pStream->WriteValue(AGPMITEM_INI_NAME_USE_INTERVAL, (INT32) pcsTemplateUsable->m_ulUseInterval))
			{
				TRACE("AgpmItem::TemplateWriteCB() Error (Use Interval) !!!\n");
				return FALSE;
			}

			if(pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
			{
				if (!pStream->WriteValue(AGPMITEM_INI_NAME_SPIRIT_STONE_TYPE, (INT32)(pcsTemplateSpirit->m_eSpiritStoneType)))
				{
					TRACE("AgpmItem::TemplateWriteCB() Error (21-2) !!!\n");
					return FALSE;
				}
			}

			if(pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
			{
				if (!pStream->WriteValue(AGPMITEM_INI_NAME_TRANSFORM_DURATION, (INT32) ((AgpdItemTemplateUsableTransform *) pcsTemplateSpirit)->m_ulDuration))
				{
					TRACE("AgpmItem::TemplateWriteCB() Error (21-2) !!!\n");
					return FALSE;
				}
			}

			if (!pStream->WriteValue(AGPMITEM_INI_NAME_EFFECT_FACTOR, 0))
			{
				TRACE("AgpmItem::TemplateWriteCB() Error (21) !!!\n");
				return FALSE;
			}

			// factor 데이타를 쓴다.
			BOOL		bIsSaveResultData	= FALSE;

			AgpdFactor	*pcsFactorResult	= (AgpdFactor *) pThis->m_pagpmFactors->GetFactor(&pcsTemplateUsable->m_csEffectFactor, AGPD_FACTORS_TYPE_RESULT);

			if (pcsFactorResult)
			{
				for (int nType = AGPD_FACTORS_TYPE_CHAR_STATUS; nType < AGPD_FACTORS_MAX_TYPE; ++nType)
				{
					if (pcsFactorResult->m_pvFactor[nType])
					{
						bIsSaveResultData	= TRUE;
						break;
					}
				}
			}

			if (bIsSaveResultData)
				pThis->m_pagpmFactors->StreamWrite(pcsFactorResult, pStream);
			else
				pThis->m_pagpmFactors->StreamWrite(&pcsTemplateUsable->m_csEffectFactor, pStream);

			if (!pStream->WriteValue(AGPMITEM_INI_NAME_EFFECT_ACTIVITY_TIME, pcsTemplateUsable->m_lEffectActivityTimeMsec))
			{
				TRACE("AgpmItem::TemplateWriteCB() Error (22) !!!\n");
				return FALSE;
			}

			if (!pStream->WriteValue(AGPMITEM_INI_NAME_EFFECT_APPLY_COUNT, pcsTemplateUsable->m_lEffectApplyCount))
			{
				TRACE("AgpmItem::TemplateWriteCB() Error (23) !!!\n");
				return FALSE;
			}

//			if (!pStream->WriteValue(AGPMITEM_INI_NAME_SPELL_NAME, pcsTemplateUsable->m_szSpellName))
//			{
//				TRACE("AgpmItem::TemplateWriteCB() Error (24) !!!\n");
//				return FALSE;
//			}
		}
		break;
	case AGPMITEM_TYPE_OTHER:
		{
			if (!pStream->WriteValue(AGPMITEM_INI_NAME_OTHER_TYPE, (INT32) pcsTemplateOther->m_eOtherItemType))
			{
				TRACE("AgpmItem::TemplateWriteCB() Error (30) !!!\n");
				return FALSE;
			}
		}
		break;
	default:
		{
			TRACE("AgpmItem::TemplateWriteCB() Error (8) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmItem::TemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	const CHAR				*szValueName;
//	CHAR					szValue[128];
//	UINT32					ulTemp1, ulTemp2;
	AgpmItem							*pThis				= (AgpmItem *) pClass;
	AgpdItemTemplate					*pcsTemplate		= (AgpdItemTemplate *) pData;
	AgpdItemTemplateEquip				*pcsTemplateEquip	= (AgpdItemTemplateEquip *) pcsTemplate;
	AgpdItemTemplateEquipWeapon			*pcsTemplateWeapon	= (AgpdItemTemplateEquipWeapon *) pcsTemplate;
	AgpdItemTemplateUsable				*pcsTemplateUsable	= (AgpdItemTemplateUsable *) pcsTemplate;
	AgpdItemTemplateOther				*pcsTemplateOther	= (AgpdItemTemplateOther *) pcsTemplate;
	AgpdItemTemplateUsableSpiritStone	*pcsTemplateSpirit	= (AgpdItemTemplateUsableSpiritStone *) pcsTemplate;
	AgpdItemTemplateUsableTransform		*pcsTemplateTransform	= (AgpdItemTemplateUsableTransform *) pcsTemplate;

	pcsTemplate->m_nType = AGPMITEM_TYPE_NUM;

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPM_FACTORS_INI_START))
		{
			pThis->m_pagpmFactors->StreamRead(&pcsTemplate->m_csFactor, pStream);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_RESTRICT_FACTOR))
		{
			if (!pStream->ReadNextValue())
				return FALSE;

			szValueName = pStream->GetValueName();

			if (!strcmp(szValueName, AGPM_FACTORS_INI_START))
				pThis->m_pagpmFactors->StreamRead(&pcsTemplate->m_csRestrictFactor, pStream);
			else
				return FALSE;
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_NAME))
		{
			char nameTemp[AGPMITEM_MAX_ITEM_NAME];
			pStream->GetValue(nameTemp, AGPMITEM_MAX_ITEM_NAME);

			pcsTemplate->m_szName = nameTemp;

			// 지금 돈의 Other Type이 세팅이 안되어 있다. 고로.. 돈 TID가 세팅 안된다.
			// 임시로 이름이 "겔드"인게 있음 그걸로 Money TID를 세팅한다.

			/*if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
			{
				if ( strcmp(pcsTemplate->m_szName.c_str(), GeldName[AP_SERVICE_AREA_CHINA]) == 0)
					pThis->m_lItemMoneyTID = pcsTemplate->m_lID;
			}*/
			if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
			{
				if ( pcsTemplate->m_szName.compare(GeldName[AP_SERVICE_AREA_WESTERN]) == 0)
					pThis->m_lItemMoneyTID = pcsTemplate->m_lID;
			}
			/*else
			{
				if ( strcmp(pcsTemplate->m_szName.c_str(), GeldName[AP_SERVICE_AREA_KOREA]) == 0)
					pThis->m_lItemMoneyTID = pcsTemplate->m_lID;
			}*/
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_TYPE))
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_nType);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_SIZE_WIDTH)) // Add 092302 Bob Jung.
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_WIDTH]);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_SIZE_HEIGHT)) // Add 092302 Bob Jung.
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_HEIGHT]);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_PART) && pcsTemplate->m_nType == AGPMITEM_TYPE_EQUIP)
		{
			pStream->GetValue((INT32 *) &pcsTemplateEquip->m_nPart);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_KIND) && pcsTemplate->m_nType == AGPMITEM_TYPE_EQUIP)
		{
			pStream->GetValue((INT32 *) &pcsTemplateEquip->m_nKind);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_WeaponType) && pcsTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
		{			
			pStream->GetValue((INT32 *) &pcsTemplateWeapon->m_nWeaponType);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_USABLE_TYPE) && pcsTemplate->m_nType == AGPMITEM_TYPE_USABLE)
		{
			pStream->GetValue((INT32 *) &pcsTemplateUsable->m_nUsableItemType);

			if (pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_ARROW)
				pThis->m_lItemArrowTID	= pcsTemplate->m_lID;
			else if (pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_BOLT)
				pThis->m_lItemBoltTID	= pcsTemplate->m_lID;
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_SPIRIT_STONE_TYPE) && pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
		{
			pStream->GetValue((INT32 *) &pcsTemplateSpirit->m_eSpiritStoneType);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_TRANSFORM_DURATION) && pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM)
		{
			pStream->GetValue((INT32 *) &pcsTemplateTransform->m_ulDuration);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_USE_INTERVAL) && pcsTemplate->m_nType == AGPMITEM_TYPE_USABLE)
		{
			pStream->GetValue((INT32 *) &pcsTemplateUsable->m_ulUseInterval);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_EFFECT_FACTOR))
		{
			if (!pStream->ReadNextValue())
				return FALSE;

			szValueName = pStream->GetValueName();
			if (!strcmp(szValueName, AGPM_FACTORS_INI_START))
			{
				AgpdFactor	*pcsResult = (AgpdFactor *) pThis->m_pagpmFactors->SetFactor(&pcsTemplateUsable->m_csEffectFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
				if (!pcsResult)
					return FALSE;

				pThis->m_pagpmFactors->StreamRead(pcsResult, pStream);
			}
			else
				return FALSE;
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_EFFECT_ACTIVITY_TIME))
		{
			pStream->GetValue((INT32 *) &pcsTemplateUsable->m_lEffectActivityTimeMsec);
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_EFFECT_APPLY_COUNT))
		{
			pStream->GetValue((INT32 *) &pcsTemplateUsable->m_lEffectApplyCount);

			/*
			if (pcsTemplateUsable->m_lEffectApplyCount != 0 &&
				pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_POTION)
				pcsTemplateUsable->m_lEffectActivityTimeMsec = (INT32) (pcsTemplateUsable->m_lEffectActivityTimeMsec / (float) pcsTemplateUsable->m_lEffectApplyCount);
			*/
		}
//		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_SPELL_NAME))
//		{
//			pStream->GetValue(pcsTemplateUsable->m_szSpellName, AGPMSKILL_MAX_SKILL_NAME);
//		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_OTHER_TYPE))
		{
			INT32	lBuffer = 0;
			pStream->GetValue(&lBuffer);

			pcsTemplateOther->m_eOtherItemType = (AgpmItemOtherType) lBuffer;

			// money template을 읽었다. TID를 저장해 놓는다.
			if (pcsTemplateOther->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
				pThis->m_lItemMoneyTID = pcsTemplateOther->m_lID;
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_IS_STACKABLE))
		{
			BOOL bIsStackable = FALSE;
			pStream->GetValue(&bIsStackable);

			pcsTemplate->m_bStackable = bIsStackable;
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_MAX_STACKABLE_COUNT))
		{
			INT32 lBuffer = 0;
			pStream->GetValue(&lBuffer);

			pcsTemplate->m_lMaxStackableCount = lBuffer;
		}
		else if (!strcmp(szValueName, AGPMITEM_INI_NAME_LightInfo))
		{
			char	str[ 256 ];
			pStream->GetValue( str , 256 );
			pcsTemplateEquip->m_lightInfo.ReadString( str );
		}
	}
	
	// 2004. 02. 10. 김태희. 한번 불러준다.
	pThis->EnumCallback(ITEM_CB_ID_READ_TEMPLATE, (PVOID)pcsTemplate, NULL);

	return TRUE;
}

BOOL AgpmItem::ItemWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	return TRUE;
}

BOOL AgpmItem::ItemReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	return TRUE;
}

BOOL AgpmItem::StreamWriteTemplate(CHAR *szFile, BOOL bEncryption)
{
	ApModuleStream			csStream;
	INT32					lIndex = 0;
	CHAR					szTID[32];
	AgpdItemTemplate		*pcsAgpdItemTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	if( bEncryption )	csStream.SetType(AUINIMANAGER_TYPE_KEYINDEX);

	// 등록된 모든 Item Template에 대해서...
	for(AgpaItemTemplate::iterator it = csTemplateAdmin.begin(); it != csTemplateAdmin.end(); ++it)
	{
		pcsAgpdItemTemplate = it->second;

		INT32	lStrlen	= sprintf(szTID, "%d", pcsAgpdItemTemplate->m_lID);
		ASSERT(lStrlen < 32);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMITEM_DATA_TYPE_TEMPLATE, pcsAgpdItemTemplate, this))
		{
			TRACE("AgpmItem::StreamWriteTemplate() Error (1) !!!\n");
			return FALSE;
		}
	}

	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL AgpmItem::StreamWriteTemplates( char* pPathName, char* pEntryFileName, BOOL bEncryption )
{
	if( !pPathName || strlen( pPathName ) <= 0 ) return FALSE;
	if( !pEntryFileName || strlen( pEntryFileName ) <= 0 ) return FALSE;

	OutputDebugString( "==== ItemTemplates Saving Start... \n" );

	char strCurrDir[ 256 ] = { 0, };
	::GetCurrentDirectory( 256, strCurrDir );

	char strItemTemplatePath[ 256 ] = { 0, };
	sprintf_s( strItemTemplatePath, 256, "%s\\%s", strCurrDir, pPathName );
	::CreateDirectory( strItemTemplatePath, NULL );

	// 등록된 모든 Item Template에 대해서...
	for( AgpaItemTemplate::iterator iter = csTemplateAdmin.begin() ; iter != csTemplateAdmin.end() ; ++iter )
	{
		AgpdItemTemplate* ppdItemTemplate = iter->second;
		if( ppdItemTemplate )
		{
			char strFileName[ 256 ] = { 0, };
			sprintf_s( strFileName, 256, "%s\\%08d.ini", pPathName, ppdItemTemplate->m_lID );

			if( !StreamWriteOneTemplate( ppdItemTemplate, strFileName, bEncryption ) )
			{
#ifdef _DEBUG
				char strDebug[ 256 ] = { 0, };
				sprintf_s( strDebug, 256, "Failed to write ItemTemplate File, TID = %d, FileName = %s, Name = %s\n", ppdItemTemplate->m_lID, strFileName, ppdItemTemplate->m_szName );
				OutputDebugString( strDebug );
#endif
				TRACE("AgpmItem::StreamWriteTemplate() Error (1) !!!\n");
			}
		}
	}

	// 대상 폴더의 상위 폴더인 INI 폴더에 ItemTemplateEntry.ini 파일을 만든다.
	if( !SaveItemTemplateEntryINI( pEntryFileName, bEncryption ) )
	{
#ifdef _DEBUG
		OutputDebugString( "Failed to write ItemTemplate Entry File, FileName = INI\\ItemTemplateEntry.xml\n" );
#endif
	}

	OutputDebugString( "==== ItemTemplates Saving Complete... \n" );
	return TRUE;
}

BOOL AgpmItem::StreamWriteOneTemplate( AgpdItemTemplate* ppdItemTemplate, char* pFileName, BOOL bEncryption )
{
	if( !ppdItemTemplate ) return FALSE;
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	ApModuleStream csStream;
	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );
	
	if( bEncryption )
	{
		csStream.SetType( AUINIMANAGER_TYPE_KEYINDEX );
	}

	char strSectionName[ 32 ] = { 0, };
	sprintf_s( strSectionName, sizeof(strSectionName), "%d", ppdItemTemplate->m_lID );

	// TID로 Section을 설정하고
	csStream.SetSection( strSectionName );

	// Stream Enumerate 한다.
	if( !csStream.EnumWriteCallback( AGPMITEM_DATA_TYPE_TEMPLATE, ppdItemTemplate, this ) )	return FALSE;
	if( !csStream.Write( pFileName, 0, bEncryption ) ) return FALSE;

	char* pINIPath = strstr( pFileName, "INI" );
	AddItemTemplateEntry( ppdItemTemplate->m_lID, ppdItemTemplate->m_szName, pINIPath );

//#ifdef _DEBUG
//	char strDebug[ 256 ] = { 0, };
//	sprintf_s( strDebug, 256, "Write ItemTemplate File, TID = %d, FileName = %s Name = %s\n", ppdItemTemplate->m_lID, pFileName, ppdItemTemplate->m_szName );
//	OutputDebugString( strDebug );
//#endif

	return TRUE;
}

BOOL AgpmItem::StreamReadTemplate(CHAR *szFile, CHAR *pszErrorMessage, BOOL bDecryption)
{
	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	INT32				lTID;
	AgpdItemTemplate	*pcsAgpdItemTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	if( !csStream.Open(szFile, 0, bDecryption) ) return FALSE;

#ifdef _DEBUG
	OutputDebugString( "--- 아이템 템플릿 통파일 읽기를 시도합니다. ---\n" );
#endif

	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; i++)
	{
		// Section Name은 TID 이다.
		lTID = atoi(csStream.ReadSectionName(i));

		// Template을 추가한다.
		pcsAgpdItemTemplate = AddItemTemplate(lTID);
		if (!pcsAgpdItemTemplate)
		{
			TRACE("AgpmItem::StreamReadTemplate() Error (1) !!!\n");
			return FALSE;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMITEM_DATA_TYPE_TEMPLATE, pcsAgpdItemTemplate, this))
		{
			if(pszErrorMessage)
				sprintf(pszErrorMessage, "ERROR:[TID : %d]%s가 잘못되었습니다!", pcsAgpdItemTemplate->m_lID, pcsAgpdItemTemplate->m_szName);

			TRACE("AgpmItem::StreamReadTemplate() Error (2) !!!\n");
			return FALSE;
		}

		string pTemplateName = GetItemTemplateName( pcsAgpdItemTemplate->m_lID );
		if( pTemplateName.length() > 0)
		{
			// 엔트리파일에서 읽어들인 이름이 있다면 그걸로 이름을 바꿔준다.
			//memset( pcsAgpdItemTemplate->m_szName, 0, sizeof( char ) * ( AGPMITEM_MAX_ITEM_NAME + 1 ) );
			//strcpy_s( pcsAgpdItemTemplate->m_szName, sizeof( char ) * ( AGPMITEM_MAX_ITEM_NAME + 1 ), pTemplateName );
			pcsAgpdItemTemplate->m_szName.resize(sizeof(char) * (AGPMITEM_MAX_ITEM_NAME + 1));
			pcsAgpdItemTemplate->m_szName = pTemplateName;
		}
		else
		{
			// 엔트리파일에 없는 템플릿을 읽었으면 엔트리에 추가시켜 준다.
			// 개별 파일명을 만들어서 넣어준다.
			char strFileName[ 256 ] = { 0, };
			sprintf_s( strFileName, sizeof( char ) * 256, "INI\\ItemTemplate\\%08d.ini", pcsAgpdItemTemplate->m_lID );
			AddItemTemplateEntry( pcsAgpdItemTemplate->m_lID, pcsAgpdItemTemplate->m_szName, strFileName );
		}

#ifdef _DEBUG
		char strDebug[ 256 ] = { 0, };
		sprintf_s( strDebug, sizeof( char ) * 256, "Load Template From 통파일, TID = %d, Name = %s\n", pcsAgpdItemTemplate->m_lID, pcsAgpdItemTemplate->m_szName );
		OutputDebugString( strDebug );
#endif
	}

#ifdef _DEBUG
	OutputDebugString( "--- 아이템 템플릿 통파일 읽기를 완료했습니다. ---\n" );
#endif
	return TRUE;
}

BOOL AgpmItem::StreamReadTemplates( char* pPathName, char* pEntryFileName, char* pErrorString, BOOL bDecryption )
{
	if( !pPathName || strlen( pPathName ) <= 0 ) return FALSE;

	if( !LoadItemTemplateEntryINI( pEntryFileName, bDecryption ) )
	{
#ifdef _DEBUG
		OutputDebugString( "Failed to load ItemTemplate Entry File. ( INI\\ItemTemplateEntry.xml )\n" );
#endif
	}

	OutputDebugString( "==== ItemTemplates Loading Start... \n" );

	//// 이 부분은 지정 폴더내의 모든 ini 를 읽을때..
	//WIN32_FIND_DATA Find;

	//char strSearchKeyword[ 256 ] = { 0, };
	//sprintf_s( strSearchKeyword, sizeof( char ) * 256, "%s\\*.ini", pPathName );

	//HANDLE hHandle = FindFirstFile( strSearchKeyword, &Find );
	//if( hHandle == INVALID_HANDLE_VALUE ) return FALSE;

	//char strLoadFileName[ 256 ] = { 0, };
	//sprintf_s( strLoadFileName, sizeof( char ) * 256, "%s\\%s", pPathName, Find.cFileName );

	//if( !StreamReadOneTemplate( strLoadFileName, bDecryption ) )
	//{
	//	if( pErrorString )
	//	{
	//		sprintf( pErrorString, "ERROR: Failed to load Item Template. ( FileName = %s )", strLoadFileName );
	//	}
	//}

	//while( FindNextFile( hHandle, &Find ) )
	//{
	//	memset( strLoadFileName, 0, sizeof( char ) * 256 );
	//	sprintf_s( strLoadFileName, sizeof( char ) * 256, "%s\\%s", pPathName, Find.cFileName );

	//	if( !StreamReadOneTemplate( strLoadFileName, bDecryption ) )
	//	{
	//		if( pErrorString )
	//		{
	//			sprintf( pErrorString, "ERROR: Failed to load Item Template. ( FileName = %s )", strLoadFileName );
	//		}
	//	}
	//}

	// 이 부분은 엔트리에 있는 거만 읽을때..
	int nItemTemplateCount = m_mapItemTemplates.size();
	std::map< int, stItemTemplateEntry >::iterator Iter;
	std::map< int, stItemTemplateEntry >::iterator EndIter;

	Iter = m_mapItemTemplates.begin();
	EndIter = m_mapItemTemplates.end();

	while( Iter != EndIter )
	{
		stItemTemplateEntry* pTemplateEntry = &Iter->second;
		if( pTemplateEntry )
		{
			if( !StreamReadOneTemplate( (char*)pTemplateEntry->m_strTemplateFileName.c_str(), bDecryption))
			{
				if( pErrorString )
				{
					sprintf( pErrorString, "ERROR: Failed to load Item Template. ( FileName = %s )", pTemplateEntry->m_strTemplateFileName.c_str() );
#ifdef _DEBUG
					OutputDebugString( pErrorString );
#endif
				}
			}
		}

		Iter++;
	}

	OutputDebugString( "==== ItemTemplates Loading Complete.\n" );
	return TRUE;
}

BOOL AgpmItem::StreamReadOneTemplate( char* pFileName, BOOL bDecryption )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	ApModuleStream csStream;
	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );

	// 노말하게 .ini 파일을 읽어보고
	if( !csStream.Open( pFileName, 0, bDecryption ) )
	{
		// 없으면 패킹파일에서 읽기를 시도한다.
		AuPackingManager* pPackingManager = AuPackingManager::GetSingletonPtr();
		if( !pPackingManager ) return FALSE;

		ApdFile csFile;
		if( !pPackingManager->OpenFile( pFileName, &csFile ) ) return FALSE;

		if( !csFile.m_bLoadFromPackingFile || !csFile.m_pcsPackingFile || !csFile.m_pcsPackingFile->m_pcsDataNode ) return FALSE;

		FILE* pFile = csFile.m_pcsPackingFolder->m_fpFile;
		if( !pFile ) return FALSE;

		fseek( pFile, csFile.m_pcsPackingFile->m_pcsDataNode->m_lStartPos, 0 );

		int nFileSize = csFile.m_pcsPackingFile->m_pcsDataNode->m_lSize;
		char* pBuffer = new char[ nFileSize ];
		if( !pBuffer ) return FALSE;

		memset( pBuffer, 0, sizeof( char ) * nFileSize );
		fread( pBuffer, sizeof( char ) * nFileSize, 1, pFile );

		// 입려한 버퍼는 내부에서 지워진다.. 따로 delete 하지 않음..
		BOOL bResult = csStream.Parse( pBuffer, nFileSize, bDecryption );
		if( !bResult ) return FALSE;
	}

	int nTID = atoi( csStream.ReadSectionName( 0 ) );
	if( nTID <= 0 ) return FALSE;

	AgpdItemTemplate* ppdItemTemplate = AddItemTemplate( nTID );
	if( !ppdItemTemplate ) return FALSE;
	if( !csStream.EnumReadCallback( AGPMITEM_DATA_TYPE_TEMPLATE, ppdItemTemplate, this ) ) return FALSE;

	string pTemplateName = GetItemTemplateName( ppdItemTemplate->m_lID );
	if( pTemplateName.length() > 0)
	{
		// 엔트리파일에서 읽어들인 이름이 있다면 그걸로 이름을 바꿔준다.
		//memset( ppdItemTemplate->m_szName, 0, sizeof( char ) * ( AGPMITEM_MAX_ITEM_NAME + 1 ) );
		//strcpy_s( ppdItemTemplate->m_szName, sizeof( char ) * ( AGPMITEM_MAX_ITEM_NAME + 1 ), pTemplateName );
		ppdItemTemplate->m_szName.resize(sizeof(char) * (AGPMITEM_MAX_ITEM_NAME + 1));
		ppdItemTemplate->m_szName = pTemplateName;
	}
	else
	{
		// 엔트리파일에 없는 템플릿을 읽었으면 엔트리에 추가시켜 준다.
		AddItemTemplateEntry( ppdItemTemplate->m_lID, ppdItemTemplate->m_szName, pFileName );
	}

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, 256, "Load ItemTemplate, TID = %d, Name = %s\n", ppdItemTemplate->m_lID, ppdItemTemplate->m_szName );
	OutputDebugString( strDebug );
#endif
	return TRUE;
}

BOOL AgpmItem::StreamWrite(CHAR *szFile)
{
	return TRUE;
}

BOOL AgpmItem::StreamRead(CHAR *szFile)
{
	return TRUE;
}

BOOL AgpmItem::StreamReadImportData(CHAR *szFile, CHAR *szDebugString, BOOL bDecryption)
{
	if (!szFile || !szDebugString)
		return FALSE;

	using namespace AuExcel;

	AuExcelLib * pExcel = LoadExcelFile( szFile , bDecryption );
	AuAutoPtr< AuExcelLib >	ptrExcel = pExcel;

	if( pExcel == NULL)
	{
		OutputDebugString("AgpmCharacter::StreamReadImportData() Error (1) !!!\n");
		return FALSE;
	}

	const INT16				nKeyRow						= 0;
	const INT16				nTNameColumn				= 0;	
	char					*pszData					= NULL;
	AgpdItemTemplate		*pcsAgpdItemTemplate		= NULL;

	enum	IMPORT_VALUE
	{
		IMVINDEX_ITEM_REQUIRE_LEVEL			=1,
		IMVINDEX_ITEM_REQUIRE_CLASS			,
		IMVINDEX_ITEM_REQUIRE_RACE			,
		IMVINDEX_ITEM_REQUIRE_GENDER		,
		IMVINDEX_ITEM_NPC_PRICE				,
		IMVINDEX_ITEM_PC_PRICE				,
		IMVINDEX_ITEM_HAND					,
		IMVINDEX_ITEM_RANK					,
		IMVINDEX_ITEM_GACHA_RANK			,
		IMVINDEX_ITEM_PHYSICAL_RANK			,
		IMVINDEX_ITEM_DEFENSE_RATE			,
		IMVINDEX_ITEM_DURABILITY			,
		IMVINDEX_ITEM_AC					,
		IMVINDEX_ITEM_PHYSICAL_MIN_DAMAGE	,
		IMVINDEX_ITEM_MAGIC_MIN_DAMAGE		,
		IMVINDEX_ITEM_WATER_MIN_DAMAGE		,
		IMVINDEX_ITEM_FIRE_MIN_DAMAGE		,
		IMVINDEX_ITEM_EARTH_MIN_DAMAGE		,
		IMVINDEX_ITEM_AIR_MIN_DAMAGE		,
		IMVINDEX_ITEM_POISON_MIN_DAMAGE		,
		IMVINDEX_ITEM_LIGHTNING_MIN_DAMAGE	,
		IMVINDEX_ITEM_ICE_MIN_DAMAGE		,
		IMVINDEX_ITEM_PHYSICAL_MAX_DAMAGE	,
		IMVINDEX_ITEM_MAGIC_MAX_DAMAGE		,
		IMVINDEX_ITEM_WATER_MAX_DAMAGE		,
		IMVINDEX_ITEM_FIRE_MAX_DAMAGE		,
		IMVINDEX_ITEM_EARTH_MAX_DAMAGE		,
		IMVINDEX_ITEM_AIR_MAX_DAMAGE		,
		IMVINDEX_ITEM_POISON_MAX_DAMAGE		,
		IMVINDEX_ITEM_ICE_MAX_DAMAGE		,
		IMVINDEX_ITEM_LIGHTNING_MAX_DAMAGE	,
		IMVINDEX_ITEM_PHYSICAL_DEFENSE_POINT,
		IMVINDEX_ITEM_MAGIC_DEFENSE_POINT	,
		IMVINDEX_ITEM_WATER_DEFENSE_POINT	,
		IMVINDEX_ITEM_FIRE_DEFENSE_POINT	,
		IMVINDEX_ITEM_EARTH_DEFENSE_POINT	,
		IMVINDEX_ITEM_AIR_DEFENSE_POINT		,
		IMVINDEX_ITEM_POISON_DEFENSE_POINT	,
		IMVINDEX_ITEM_ICE_DEFENSE_POINT		,
		IMVINDEX_ITEM_LIGHTNING_DEFENSE_POINT,
		IMVINDEX_ITEM_BLOCK_RATE			,
		IMVINDEX_ITEM_MAGIC_DEFENSE_RATE	,
		IMVINDEX_ITEM_WATER_DEFENSE_RATE	,
		IMVINDEX_ITEM_FIRE_DEFENSE_RATE		,
		IMVINDEX_ITEM_EARTH_DEFENSE_RATE	,
		IMVINDEX_ITEM_AIR_DEFENSE_RATE		,
		IMVINDEX_ITEM_POISON_DEFENSE_RATE	,
		IMVINDEX_ITEM_ICE_DEFENSE_RATE		,
		IMVINDEX_ITEM_LIGHTNING_DEFENSE_RATE,
		IMVINDEX_ITEM_PHYSICAL_AR			,
		IMVINDEX_ITEM_MAGICAL_AR			,
		IMVINDEX_HP_BUFF					,
		IMVINDEX_MP_BUFF					,
		IMVINDEX_ATTACK_BUFF				,
		IMVINDEX_DEFENSE_BUFF				,
		IMVINDEX_RUN_BUFF					,
		IMVINDEX_BUYER_TYPE					,
		IMVINDEX_USING_TYPE					,
		IMVINDEX_DESTINATION				,
		IMVINDEX_CASH						,
		IMVINDEX_REMAIN_TIME				,
		IMVINDEX_EXPIRE_TIME				,
		IMVINDEX_CLASSIFY_ID				,
		IMVINDEX_CAN_STOP_USING_ITEM		,
		IMVINDEX_CASH_ITEM_USE_TYPE			,
		IMVINDEX_ENABLE_ON_RIDE				,
		IMVINDEX_ENABLE_GAMBLE				,
		IMVINDEX_QUESTGROUP					,
		IMVINDEX_OPTION_TID					,
		IMVINDEX_LINK_ID					,
		IMVINDEX_SKILL_PLUS					,
		IMVINDEX_ITEM_PHYSICAL_DR			,
		IMVINDEX_ITEM_ATTACK_RANGE			,
		IMVINDEX_ITEM_ATTACK_SPEED			,
		IMVINDEX_ITEM_MAGIC_INTENSITY		,
		IMVINDEX_ITEM_BONUS_POINT_CON		,
		IMVINDEX_ITEM_BONUS_POINT_STR		,
		IMVINDEX_ITEM_BONUS_POINT_DEX		,
		IMVINDEX_ITEM_BONUS_POINT_INT		,
		IMVINDEX_ITEM_BONUS_POINT_WIS		,
		IMVINDEX_ITEM_BONUS_POINT_HP		,
		IMVINDEX_ITEM_BONUS_POINT_MP		,
		IMVINDEX_ITEM_BONUS_POINT_SP		,
		IMVINDEX_ITEM_POTION_TYPE2			,
		IMVINDEX_ITEM_APPLY_EFFECT_COUNT	,
		IMVINDEX_ITEM_APPLY_EFFECT_TIME		,
		IMVINDEX_ITEM_USE_INTERVAL			,
		IMVINDEX_ITEM_MAX_STACK_COUNT		,
		IMVINDEX_ITEM_TYPE					,
		IMVINDEX_ITEM_SUB_TYPE				,
		IMVINDEX_ITEM_EXTRA_TYPE			,
		IMVINDEX_ITEM_SPIRIT_STONE_TYPE		,
		IMVINDEX_ITEM_BOUND_TYPE			,
		IMVINDEX_ITEM_RUNE_ATTRIBUTE_VALUE	,
		IMVINDEX_ITEM_TARGET_TID			,
		IMVINDEX_ITEM_DURATION				,
		IMVINDEX_FIRST_CATEGORY				,
		IMVINDEX_SECOND_CATEGORY			,
		IMVINDEX_FIRST_CATEGORY_NAME		,
		IMVINDEX_SECOND_CATEGORY_NAME		,
		IMVINDEX_USE_SKILL_ID				,
		IMVINDEX_USE_SKILL_LEVEL			,
		IMVINDEX_TITLE_FONT_COLOR			,
		IMVINDEX_MIN_SOCKET					,
		IMVINDEX_MAX_SOCKET					,
		IMVINDEX_MIN_OPTION					,
		IMVINDEX_MAX_OPTION					,
		IMVINDEX_EVENT_ITEM					,
		IMVINDEX_PCBANG_ONLY				,
		IMVINDEX_VILLAIN_ONLY				,
		IMVINDEX_QUESTITEM					,
		IMVINDEX_GACHATYPE					,
		IMVINDEX_STAMINACURE				,
		IMVINDEX_REMAIN_STAMINA_TIME		,
		IMVINDEX_ITEM_SECTION_NUMBER		,
		IMVINDEX_ITEM_HEROIC_MIN_DAMAGE		,
		IMVINDEX_ITEM_HEROIC_MAX_DAMAGE		,
		IMVINDEX_ITEM_HEROIC_DEFENSE_POINT	,
		IMVINDEX_MAX					= 255
	};

	int	aColumn[ IMVINDEX_MAX ] = { 0 , };

	for(INT16 nCol = 1; nCol < pExcel->GetColumn(); ++nCol)
	{
		pszData = pExcel->GetData(nCol, nKeyRow);
		if(!pszData)
			continue;
		if(!strcmp(pszData,			AGPMITEM_IMPORT_ITEM_REQUIRE_LEVEL			)) aColumn[ nCol] = IMVINDEX_ITEM_REQUIRE_LEVEL			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_REQUIRE_CLASS			)) aColumn[ nCol] = IMVINDEX_ITEM_REQUIRE_CLASS			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_REQUIRE_RACE			)) aColumn[ nCol] = IMVINDEX_ITEM_REQUIRE_RACE			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_REQUIRE_GENDER			)) aColumn[ nCol] = IMVINDEX_ITEM_REQUIRE_GENDER		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_NPC_PRICE				)) aColumn[ nCol] = IMVINDEX_ITEM_NPC_PRICE				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_PC_PRICE				)) aColumn[ nCol] = IMVINDEX_ITEM_PC_PRICE				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_HAND					)) aColumn[ nCol] = IMVINDEX_ITEM_HAND					;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_RANK					)) aColumn[ nCol] = IMVINDEX_ITEM_RANK					;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_GACHA_RANK				)) aColumn[ nCol] = IMVINDEX_ITEM_GACHA_RANK			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_PHYSICAL_RANK			)) aColumn[ nCol] = IMVINDEX_ITEM_PHYSICAL_RANK			;
		/*else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_DEFENSE_RATE			)) aColumn[ nCol] = IMVINDEX_ITEM_DEFENSE_RATE			;*/
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_DURABILITY				)) aColumn[ nCol] = IMVINDEX_ITEM_DURABILITY			;
		/*else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_AC						)) aColumn[ nCol] = IMVINDEX_ITEM_AC					;*/
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_PHYSICAL_MIN_DAMAGE	)) aColumn[ nCol] = IMVINDEX_ITEM_PHYSICAL_MIN_DAMAGE	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_MAGIC_MIN_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_MAGIC_MIN_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_WATER_MIN_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_WATER_MIN_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_FIRE_MIN_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_FIRE_MIN_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_EARTH_MIN_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_EARTH_MIN_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_AIR_MIN_DAMAGE			)) aColumn[ nCol] = IMVINDEX_ITEM_AIR_MIN_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_POISON_MIN_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_POISON_MIN_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_LIGHTNING_MIN_DAMAGE	)) aColumn[ nCol] = IMVINDEX_ITEM_LIGHTNING_MIN_DAMAGE	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_ICE_MIN_DAMAGE			)) aColumn[ nCol] = IMVINDEX_ITEM_ICE_MIN_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_PHYSICAL_MAX_DAMAGE	)) aColumn[ nCol] = IMVINDEX_ITEM_PHYSICAL_MAX_DAMAGE	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_MAGIC_MAX_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_MAGIC_MAX_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_WATER_MAX_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_WATER_MAX_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_FIRE_MAX_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_FIRE_MAX_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_EARTH_MAX_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_EARTH_MAX_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_AIR_MAX_DAMAGE			)) aColumn[ nCol] = IMVINDEX_ITEM_AIR_MAX_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_POISON_MAX_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_POISON_MAX_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_ICE_MAX_DAMAGE			)) aColumn[ nCol] = IMVINDEX_ITEM_ICE_MAX_DAMAGE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_LIGHTNING_MAX_DAMAGE	)) aColumn[ nCol] = IMVINDEX_ITEM_LIGHTNING_MAX_DAMAGE	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_PHYSICAL_DEFENSE_POINT	)) aColumn[ nCol] = IMVINDEX_ITEM_PHYSICAL_DEFENSE_POINT;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_MAGIC_DEFENSE_POINT	)) aColumn[ nCol] = IMVINDEX_ITEM_MAGIC_DEFENSE_POINT	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_WATER_DEFENSE_POINT	)) aColumn[ nCol] = IMVINDEX_ITEM_WATER_DEFENSE_POINT	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_FIRE_DEFENSE_POINT		)) aColumn[ nCol] = IMVINDEX_ITEM_FIRE_DEFENSE_POINT	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_EARTH_DEFENSE_POINT	)) aColumn[ nCol] = IMVINDEX_ITEM_EARTH_DEFENSE_POINT	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_AIR_DEFENSE_POINT		)) aColumn[ nCol] = IMVINDEX_ITEM_AIR_DEFENSE_POINT		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_POISON_DEFENSE_POINT	)) aColumn[ nCol] = IMVINDEX_ITEM_POISON_DEFENSE_POINT	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_ICE_DEFENSE_POINT		)) aColumn[ nCol] = IMVINDEX_ITEM_ICE_DEFENSE_POINT		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_LIGHTNING_DEFENSE_POINT)) aColumn[ nCol] = IMVINDEX_ITEM_LIGHTNING_DEFENSE_POINT;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_BLOCK_RATE				)) aColumn[ nCol] = IMVINDEX_ITEM_BLOCK_RATE			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_MAGIC_DEFENSE_RATE		)) aColumn[ nCol] = IMVINDEX_ITEM_MAGIC_DEFENSE_RATE	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_WATER_DEFENSE_RATE		)) aColumn[ nCol] = IMVINDEX_ITEM_WATER_DEFENSE_RATE	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_FIRE_DEFENSE_RATE		)) aColumn[ nCol] = IMVINDEX_ITEM_FIRE_DEFENSE_RATE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_EARTH_DEFENSE_RATE		)) aColumn[ nCol] = IMVINDEX_ITEM_EARTH_DEFENSE_RATE	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_AIR_DEFENSE_RATE		)) aColumn[ nCol] = IMVINDEX_ITEM_AIR_DEFENSE_RATE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_POISON_DEFENSE_RATE	)) aColumn[ nCol] = IMVINDEX_ITEM_POISON_DEFENSE_RATE	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_ICE_DEFENSE_RATE		)) aColumn[ nCol] = IMVINDEX_ITEM_ICE_DEFENSE_RATE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_LIGHTNING_DEFENSE_RATE	)) aColumn[ nCol] = IMVINDEX_ITEM_LIGHTNING_DEFENSE_RATE;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_PHYSICAL_AR			)) aColumn[ nCol] = IMVINDEX_ITEM_PHYSICAL_AR			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_MAGICAL_AR				)) aColumn[ nCol] = IMVINDEX_ITEM_MAGICAL_AR			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_HP_BUFF						)) aColumn[ nCol] = IMVINDEX_HP_BUFF					;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_MP_BUFF						)) aColumn[ nCol] = IMVINDEX_MP_BUFF					;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ATTACK_BUFF					)) aColumn[ nCol] = IMVINDEX_ATTACK_BUFF				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_DEFENSE_BUFF				)) aColumn[ nCol] = IMVINDEX_DEFENSE_BUFF				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_RUN_BUFF					)) aColumn[ nCol] = IMVINDEX_RUN_BUFF					;
		else if(!stricmp(pszData,	AGPMITEM_IMPORT_BUYER_TYPE					)) aColumn[ nCol] = IMVINDEX_BUYER_TYPE					;
		else if(!stricmp(pszData,	AGPMITEM_IMPORT_USING_TYPE					)) aColumn[ nCol] = IMVINDEX_USING_TYPE					;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_DESTINATION					)) aColumn[ nCol] = IMVINDEX_DESTINATION				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_CASH						)) aColumn[ nCol] = IMVINDEX_CASH						;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_REMAIN_TIME					)) aColumn[ nCol] = IMVINDEX_REMAIN_TIME				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_EXPIRE_TIME					)) aColumn[ nCol] = IMVINDEX_EXPIRE_TIME				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_CLASSIFY_ID					)) aColumn[ nCol] = IMVINDEX_CLASSIFY_ID				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_CAN_STOP_USING_ITEM			)) aColumn[ nCol] = IMVINDEX_CAN_STOP_USING_ITEM		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_CASH_ITEM_USE_TYPE			)) aColumn[ nCol] = IMVINDEX_CASH_ITEM_USE_TYPE			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ENABLE_ON_RIDE				)) aColumn[ nCol] = IMVINDEX_ENABLE_ON_RIDE				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ENABLE_GAMBLE				)) aColumn[ nCol] = IMVINDEX_ENABLE_GAMBLE				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_QUESTGROUP					)) aColumn[ nCol] = IMVINDEX_QUESTGROUP					;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_OPTION_TID					)) aColumn[ nCol] = IMVINDEX_OPTION_TID					;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_LINK_ID						)) aColumn[ nCol] = IMVINDEX_LINK_ID					;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_SKILL_PLUS					)) aColumn[ nCol] = IMVINDEX_SKILL_PLUS					;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_PHYSICAL_DR			)) aColumn[ nCol] = IMVINDEX_ITEM_PHYSICAL_DR			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_ATTACK_RANGE			)) aColumn[ nCol] = IMVINDEX_ITEM_ATTACK_RANGE			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_ATTACK_SPEED			)) aColumn[ nCol] = IMVINDEX_ITEM_ATTACK_SPEED			;
		/*else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_MAGIC_INTENSITY		)) aColumn[ nCol] = IMVINDEX_ITEM_MAGIC_INTENSITY		;*/
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_BONUS_POINT_CON		)) aColumn[ nCol] = IMVINDEX_ITEM_BONUS_POINT_CON		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_BONUS_POINT_STR		)) aColumn[ nCol] = IMVINDEX_ITEM_BONUS_POINT_STR		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_BONUS_POINT_DEX		)) aColumn[ nCol] = IMVINDEX_ITEM_BONUS_POINT_DEX		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_BONUS_POINT_INT		)) aColumn[ nCol] = IMVINDEX_ITEM_BONUS_POINT_INT		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_BONUS_POINT_WIS		)) aColumn[ nCol] = IMVINDEX_ITEM_BONUS_POINT_WIS		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_BONUS_POINT_HP			)) aColumn[ nCol] = IMVINDEX_ITEM_BONUS_POINT_HP		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_BONUS_POINT_MP			)) aColumn[ nCol] = IMVINDEX_ITEM_BONUS_POINT_MP		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_BONUS_POINT_SP			)) aColumn[ nCol] = IMVINDEX_ITEM_BONUS_POINT_SP		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_POTION_TYPE2			)) aColumn[ nCol] = IMVINDEX_ITEM_POTION_TYPE2			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_APPLY_EFFECT_COUNT		)) aColumn[ nCol] = IMVINDEX_ITEM_APPLY_EFFECT_COUNT	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_APPLY_EFFECT_TIME		)) aColumn[ nCol] = IMVINDEX_ITEM_APPLY_EFFECT_TIME		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_USE_INTERVAL			)) aColumn[ nCol] = IMVINDEX_ITEM_USE_INTERVAL			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_MAX_STACK_COUNT		)) aColumn[ nCol] = IMVINDEX_ITEM_MAX_STACK_COUNT		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_TYPE					)) aColumn[ nCol] = IMVINDEX_ITEM_TYPE					;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_SUB_TYPE				)) aColumn[ nCol] = IMVINDEX_ITEM_SUB_TYPE				;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_EXTRA_TYPE				)) aColumn[ nCol] = IMVINDEX_ITEM_EXTRA_TYPE			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_SPIRIT_STONE_TYPE		)) aColumn[ nCol] = IMVINDEX_ITEM_SPIRIT_STONE_TYPE		;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_BOUND_TYPE				)) aColumn[ nCol] = IMVINDEX_ITEM_BOUND_TYPE			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_RUNE_ATTRIBUTE_VALUE	)) aColumn[ nCol] = IMVINDEX_ITEM_RUNE_ATTRIBUTE_VALUE	;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_TARGET_TID				)) aColumn[ nCol] = IMVINDEX_ITEM_TARGET_TID			;
		else if(!strcmp(pszData,	AGPMITEM_IMPORT_ITEM_DURATION				)) aColumn[ nCol] = IMVINDEX_ITEM_DURATION				;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_FIRST_CATEGORY				)) aColumn[ nCol] = IMVINDEX_FIRST_CATEGORY				;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_SECOND_CATEGORY				)) aColumn[ nCol] = IMVINDEX_SECOND_CATEGORY			;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_FIRST_CATEGORY_NAME			)) aColumn[ nCol] = IMVINDEX_FIRST_CATEGORY_NAME		;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_SECOND_CATEGORY_NAME		)) aColumn[ nCol] = IMVINDEX_SECOND_CATEGORY_NAME		;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_USE_SKILL_ID				)) aColumn[ nCol] = IMVINDEX_USE_SKILL_ID				;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_USE_SKILL_LEVEL				)) aColumn[ nCol] = IMVINDEX_USE_SKILL_LEVEL			;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_TITLE_FONT_COLOR			)) aColumn[ nCol] = IMVINDEX_TITLE_FONT_COLOR			;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_MIN_SOCKET					)) aColumn[ nCol] = IMVINDEX_MIN_SOCKET					;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_MAX_SOCKET					)) aColumn[ nCol] = IMVINDEX_MAX_SOCKET					;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_MIN_OPTION					)) aColumn[ nCol] = IMVINDEX_MIN_OPTION					;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_MAX_OPTION					)) aColumn[ nCol] = IMVINDEX_MAX_OPTION					;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_EVENT_ITEM					)) aColumn[ nCol] = IMVINDEX_EVENT_ITEM					;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_PCBANG_ONLY					)) aColumn[ nCol] = IMVINDEX_PCBANG_ONLY				;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_VILLAIN_ONLY				)) aColumn[ nCol] = IMVINDEX_VILLAIN_ONLY				;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_QUESTITEM					)) aColumn[ nCol] = IMVINDEX_QUESTITEM					;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_GACHATYPE					)) aColumn[ nCol] = IMVINDEX_GACHATYPE					;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_STAMINACURE					)) aColumn[ nCol] = IMVINDEX_STAMINACURE				;
		else if(!_stricmp(pszData,	AGPMITEM_IMPORT_REMAIN_STAMINA_TIME			)) aColumn[ nCol] = IMVINDEX_REMAIN_STAMINA_TIME		;
		else if(!_stricmp(pszData,  AGPMITEM_IMPORT_ITEM_SECTION_NUM			)) aColumn[ nCol] = IMVINDEX_ITEM_SECTION_NUMBER		;
		else if(!_stricmp(pszData,  AGPMITEM_IMPORT_ITEM_HEROIC_MIN_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_HEROIC_MIN_DAMAGE		;
		else if(!_stricmp(pszData,  AGPMITEM_IMPORT_ITEM_HEROIC_MAX_DAMAGE		)) aColumn[ nCol] = IMVINDEX_ITEM_HEROIC_MAX_DAMAGE		;
		else if(!_stricmp(pszData,  AGPMITEM_IMPORT_ITEM_HEROIC_DEFENSE_POINT	)) aColumn[ nCol] = IMVINDEX_ITEM_HEROIC_DEFENSE_POINT	;
	
	}


	for(INT16 nRow = 1; nRow < pExcel->GetRow(); ++nRow)
	{
		pszData = pExcel->GetData(nTNameColumn, nRow);
		if(!pszData)
		{
			continue;
		}

		pcsAgpdItemTemplate = GetItemTemplate(atoi(pszData));
		if(!pcsAgpdItemTemplate)
		{
			sprintf(szDebugString, "AgpmItem::StreamReadImportData() %s이(가) 업습니다!!!\n", pszData);
			TRACE(szDebugString);

			sprintf(szDebugString, "WARNNING > %s이(가) 업습니다!!!", pszData);

			EnumCallback(ITEM_CB_ID_STREAM_READ_IMPORT_DATA_ERROR_REPORT, szDebugString, NULL);

			continue;
		}

		sprintf(szDebugString, "FAILED > %s !!!", pszData);

		for(INT16 nCol = 1; nCol < pExcel->GetColumn(); ++nCol)
		{
			switch( aColumn[ nCol] )
			{
			case IMVINDEX_ITEM_REQUIRE_LEVEL			:
				{
					CHAR* pszValue = pExcel->GetData(nCol, nRow);
					if(pszValue && pszValue[0])
					{
						INT32 lOffset = 0;
						INT32 lLevel1st, lLevel2nd;
						lLevel1st = lLevel2nd = 0;

						do
						{
							sscanf(pszValue + lOffset , "%d", &lLevel1st);

							int i = 0;
							for (i = lOffset; i < (int)strlen(pszValue); ++i)
							{
								if(pszValue[i] == ';')
								{
									lOffset	= i + 1;
									break;
								}
							}

							if (i == (int)strlen(pszValue))
								break;

							sscanf(pszValue + lOffset, "%d", &lLevel2nd);
							break;
						}
						while (true);


						m_pagpmFactors->SetValue(&pcsAgpdItemTemplate->m_csRestrictFactor,
												 lLevel1st,
												 AGPD_FACTORS_TYPE_CHAR_STATUS,
												 AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

						if(lLevel2nd)
							pcsAgpdItemTemplate->m_lLimitedLevel = lLevel2nd;
					}
				}
				break;
			case IMVINDEX_ITEM_REQUIRE_CLASS			:
				{
					INT32 lClassMask	= AUCHARCLASS_TYPE_NONE;

					CHAR* szBuffer = NULL;
					CHAR* szToken = NULL;
					CHAR szSeps[] = ";";

					szBuffer = pExcel->GetData(nCol, nRow);
					if(szBuffer && strlen(szBuffer))
					{
						szToken = strtok(szBuffer, szSeps);
						while(szToken)
						{
							INT32 lClass = (INT32)atoi(szToken);

							if(lClass > AUCHARCLASS_TYPE_NONE && lClass < AUCHARCLASS_TYPE_MAX)
							{
								lClassMask |= (1 << (lClass - 1));
							}
							
							szToken = strtok(NULL, szSeps);
						}

						m_pagpmFactors->SetValue(&pcsAgpdItemTemplate->m_csRestrictFactor, lClassMask, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);
					}
				}
				break;
			case IMVINDEX_ITEM_REQUIRE_RACE			:
				{
					INT32 lRaceMask	= AURACE_TYPE_NONE;

					CHAR* szBuffer = NULL;
					CHAR* szToken = NULL;
					CHAR szSeps[] = ";";

					szBuffer = pExcel->GetData(nCol, nRow);
					if(szBuffer && strlen(szBuffer))
					{
						szToken = strtok(szBuffer, szSeps);
						while(szToken)
						{
							INT32 lRace = (INT32)atoi(szToken);

							if(lRace > AURACE_TYPE_NONE && lRace < AURACE_TYPE_MAX)
							{
								lRaceMask |= (1 << (lRace - 1));
							}
							
							szToken = strtok(NULL, szSeps);
						}

						m_pagpmFactors->SetValue(&pcsAgpdItemTemplate->m_csRestrictFactor, lRaceMask, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
					}
				} break;
			case IMVINDEX_ITEM_REQUIRE_GENDER			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csRestrictFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_GENDER))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (7) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_NPC_PRICE				:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_PRICE, AGPD_FACTORS_PRICE_TYPE_NPC_PRICE))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (8-1) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_PC_PRICE				:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_PRICE, AGPD_FACTORS_PRICE_TYPE_PC_PRICE))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (8-1) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_HAND					:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (8-2) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_RANK					:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (8-3) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_GACHA_RANK					:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_GACHA))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (8-3) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_PHYSICAL_RANK			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_PHYSICAL_RANK))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (8-93) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			/*
			case IMVINDEX_ITEM_DEFENSE_RATE			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (9) !!!\n");
						return FALSE;
					}
				}
				break;
			*/
			case IMVINDEX_ITEM_DURABILITY				:
				{
					CHAR* pszValue = pExcel->GetData(nCol, nRow);
					if (NULL != pszValue && '@' == *pszValue)
					{
						// 무제한 사용을 원한다.
						pcsAgpdItemTemplate->m_bFreeDuration = TRUE;

						// dummy로 50000을 입력한다. 0이면 예외 처리에 걸릴수 있기 때문
						m_pagpmFactors->SetValue(&pcsAgpdItemTemplate->m_csFactor, 50000, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);
						m_pagpmFactors->SetValue(&pcsAgpdItemTemplate->m_csFactor, 50000, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
					}
					else if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY) ||
							!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (10) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			/*
			case IMVINDEX_ITEM_AC						:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16) !!!\n");
						return FALSE;
					}
				}
				break;
			*/
			case IMVINDEX_ITEM_PHYSICAL_MIN_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-1) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_MAGIC_MIN_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-2) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_WATER_MIN_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-3) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_FIRE_MIN_DAMAGE			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-4) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_EARTH_MIN_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-5) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_AIR_MIN_DAMAGE			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-6) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_POISON_MIN_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-7) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_LIGHTNING_MIN_DAMAGE	:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-8) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_ICE_MIN_DAMAGE			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-9) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_PHYSICAL_MAX_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-10) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_MAGIC_MAX_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-11) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_WATER_MAX_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-12) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_FIRE_MAX_DAMAGE			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-13) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_EARTH_MAX_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-14) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_AIR_MAX_DAMAGE			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-15) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_POISON_MAX_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-16) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_ICE_MAX_DAMAGE			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-17) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_LIGHTNING_MAX_DAMAGE	:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-18) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_PHYSICAL_DEFENSE_POINT	:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-19) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_MAGIC_DEFENSE_POINT		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-20) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_WATER_DEFENSE_POINT		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-21) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_FIRE_DEFENSE_POINT		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-22) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_EARTH_DEFENSE_POINT		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-23) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_AIR_DEFENSE_POINT		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-24) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_POISON_DEFENSE_POINT	:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-25) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_ICE_DEFENSE_POINT		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-26) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_LIGHTNING_DEFENSE_POINT	:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-27) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_BLOCK_RATE				:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL_BLOCK))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-28) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_MAGIC_DEFENSE_RATE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-29) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_WATER_DEFENSE_RATE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-30) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_FIRE_DEFENSE_RATE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-31) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_EARTH_DEFENSE_RATE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-32) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_AIR_DEFENSE_RATE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-33) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_POISON_DEFENSE_RATE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-34) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_ICE_DEFENSE_RATE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-35) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_LIGHTNING_DEFENSE_RATE	:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-36) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_PHYSICAL_AR				:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-36) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_MAGICAL_AR				:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAR))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-36) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_HEROIC_MIN_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-36) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_HEROIC_MAX_DAMAGE		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-36) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_HEROIC_DEFENSE_POINT		:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-36) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_SECTION_NUMBER			:
				{
					pcsAgpdItemTemplate->m_eItemSectionType = (eAgpmItemSectionType)pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_HP_BUFF						:
				{
					pcsAgpdItemTemplate->m_lHpBuff = pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_MP_BUFF						:
				{
					pcsAgpdItemTemplate->m_lMpBuff = pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_ATTACK_BUFF					:
				{
					pcsAgpdItemTemplate->m_lAttackBuff = pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_DEFENSE_BUFF					:
				{
					pcsAgpdItemTemplate->m_lDefenseBuff = pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_RUN_BUFF						:
				{
					pcsAgpdItemTemplate->m_lRunBuff = pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_BUYER_TYPE					:
				{
					pcsAgpdItemTemplate->m_eBuyerType = (AgpmItemBuyerType)pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_USING_TYPE					:
				{
					pcsAgpdItemTemplate->m_lUsingType = pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_DESTINATION					:
				{
					if(pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
					{
						if(((AgpdItemTemplateUsable*)pcsAgpdItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL)
						{
							CHAR* szTmp = pExcel->GetData(nCol, nRow);
							if(szTmp)
							{
								AuPOS stPOS;
								memset(&stPOS, 0, sizeof(AuPOS));
								int iResult = sscanf(szTmp, "%f;%f;%f", &stPOS.x, &stPOS.y, &stPOS.z);

								((AgpdItemTemplateUsableTeleportScroll*)(pcsAgpdItemTemplate))->m_stDestination = stPOS;
							}
						}
					}
				}
				break;
			case IMVINDEX_CASH							:
				{
					INT32 lValue = pExcel->GetDataToInt(nCol, nRow);
					if(lValue >= 100)
					{
						pcsAgpdItemTemplate->m_bContinuousOff = TRUE;
						lValue -= 100;
					}

					pcsAgpdItemTemplate->m_eCashItemType = (AgpmItemCashItemType)lValue;
				}
				break;
			case IMVINDEX_REMAIN_TIME					:
				{
					pcsAgpdItemTemplate->m_lRemainTime = INT64(60) * 1000 * pExcel->GetDataToInt(nCol, nRow);	// 분단위 입력
				}
				break;
			case IMVINDEX_EXPIRE_TIME					:
				{
					pcsAgpdItemTemplate->m_lExpireTime = 60*pExcel->GetDataToInt(nCol, nRow);	// 분단위 입력
				}
				break;
			case IMVINDEX_CLASSIFY_ID					:
				{
					pcsAgpdItemTemplate->m_lClassifyID = pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_CAN_STOP_USING_ITEM			:
				{
					pcsAgpdItemTemplate->m_bCanStopUsingItem = (BOOL)pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_CASH_ITEM_USE_TYPE			:
				{
					pcsAgpdItemTemplate->m_eCashItemUseType = (AgpmItemCashItemUseType)pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_ENABLE_ON_RIDE				:
				{
					pcsAgpdItemTemplate->m_bEnableOnRide = (BOOL)pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_ENABLE_GAMBLE				:
				{
					pcsAgpdItemTemplate->m_bEnableGamble = (BOOL)pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_QUESTGROUP					:
				{
					pcsAgpdItemTemplate->m_nQuestGroup = (INT32)pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_GACHATYPE					:
				{
					pcsAgpdItemTemplate->m_nGachaType = (INT32)pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_OPTION_TID					:
				{
					StreamReadOptionTID(pcsAgpdItemTemplate, *pExcel, nCol, nRow);
				}
				break;
			case IMVINDEX_LINK_ID						:
				{
					StreamReadLinkID(pcsAgpdItemTemplate, *pExcel, nCol, nRow);
				}
				break;
			case IMVINDEX_SKILL_PLUS					:
				{
					StreamReadSkillPlusTID(pcsAgpdItemTemplate, *pExcel, nCol, nRow);
				}
				break;
			case IMVINDEX_ITEM_PHYSICAL_DR				:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_DR))
					{
						TRACE("AgcmImportClientData::ImportCharacterData() Error (16-36) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_ATTACK_RANGE			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (11) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_ATTACK_SPEED			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (12) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			/*
			case IMVINDEX_ITEM_MAGIC_INTENSITY			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_MI))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (17) !!!\n");
						return FALSE;
					}
				}
				break;
			*/
			case IMVINDEX_ITEM_BONUS_POINT_CON			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (17-1) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_BONUS_POINT_STR			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (17-2) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_BONUS_POINT_DEX			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (17-3) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_BONUS_POINT_INT			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (17-4) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_BONUS_POINT_WIS			:
				{
					if(!m_pagpmCharacter->SetStreamFactor(pExcel, &pcsAgpdItemTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS))
					{
						TRACE("AgpmItem::StreamReadImportData() Error (17-5) !!!\n");
						pExcel->CloseFile();
						return FALSE;
					}
				}
				break;
			case IMVINDEX_ITEM_BONUS_POINT_HP			:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if(pszData)
					{
						if(pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
						{
							TRACE("AgpmItem::StreamReadImportData() Error (17-6) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						if(!m_pagpmCharacter->SetStreamFactor(pExcel, &((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_csEffectFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP))
						{
							TRACE("AgpmItem::StreamReadImportData() Error (17-6) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						((AgpdItemTemplateUsablePotion *) pcsAgpdItemTemplate)->m_ePotionType	= AGPMITEM_USABLE_POTION_TYPE_HP;
					}
				}
				break;
			case IMVINDEX_ITEM_BONUS_POINT_MP			:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if(pszData)
					{
						if(pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
						{
							TRACE("AgpmItem::StreamReadImportData() Error (17-7) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						if(!m_pagpmCharacter->SetStreamFactor(pExcel, &((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_csEffectFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP))
						{
							TRACE("AgpmItem::StreamReadImportData() Error (17-7) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}
						
						if(((AgpdItemTemplateUsablePotion *) pcsAgpdItemTemplate)->m_ePotionType == AGPMITEM_USABLE_POTION_TYPE_HP)
						{
							((AgpdItemTemplateUsablePotion *) pcsAgpdItemTemplate)->m_ePotionType = AGPMITEM_USABLE_POTION_TYPE_BOTH;
						}
						else
						{
							((AgpdItemTemplateUsablePotion *) pcsAgpdItemTemplate)->m_ePotionType	= AGPMITEM_USABLE_POTION_TYPE_MP;
						}
					}
				}
				break;
			case IMVINDEX_ITEM_BONUS_POINT_SP			:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if(pszData)
					{
						if(pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
						{
							TRACE("AgpmItem::StreamReadImportData() Error (17-8) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						if(!m_pagpmCharacter->SetStreamFactor(pExcel, &((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_csEffectFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP))
						{
							TRACE("AgpmItem::StreamReadImportData() Error (17-8) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						((AgpdItemTemplateUsablePotion *) pcsAgpdItemTemplate)->m_ePotionType	= AGPMITEM_USABLE_POTION_TYPE_SP;
					}
				}
				break;
			case IMVINDEX_ITEM_POTION_TYPE2			:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if(pszData)
					{
						if(pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
						{
							TRACE("AgpmItem::StreamReadImportData() Error (17-8) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						if (atoi(pszData) != 0)
							((AgpdItemTemplateUsablePotion *) pcsAgpdItemTemplate)->m_ePotionType2	= (AgpmItemUsablePotionType2) atoi(pszData);
					}
				}
				break;
			case IMVINDEX_ITEM_APPLY_EFFECT_COUNT		:
				{
					if(pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
						continue;

					pszData = pExcel->GetData(nCol, nRow);
					if(pszData)
						((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_lEffectApplyCount = atoi(pszData);
					else
						((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_lEffectApplyCount = 0;
				}
				break;
			case IMVINDEX_ITEM_APPLY_EFFECT_TIME		:
				{
					if(pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
						continue;

					pszData = pExcel->GetData(nCol, nRow);
					if(pszData)
						((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_lEffectActivityTimeMsec = atoi(pszData);
					else
						((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_lEffectActivityTimeMsec = 0;
				}
				break;
			case IMVINDEX_ITEM_USE_INTERVAL			:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if(pszData)
					{
						if(pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
						{
							TRACE("AgpmItem::StreamReadImportData() Error (33-3) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_ulUseInterval = atoi(pszData);
					}
				}
				break;
			case IMVINDEX_ITEM_MAX_STACK_COUNT			:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if(pszData)
					{
						pcsAgpdItemTemplate->m_lMaxStackableCount = atoi(pszData);

						if (pcsAgpdItemTemplate->m_lMaxStackableCount > 0)
							pcsAgpdItemTemplate->m_bStackable	= TRUE;
					}
				}
				break;
			case IMVINDEX_ITEM_TYPE					:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
	/*					if (pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP)
						{
							TRACE("AgpmItem::StreamReadImportData() Error (33-4) !!!\n");
							return FALSE;
						}*/

						if (	(pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP) &&
								(((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)	)
						{
							((AgpdItemTemplateEquipWeapon *)(pcsAgpdItemTemplate))->m_nWeaponType = (AgpmItemEquipWeaponType)(atoi(pszData));
						}
						else if (pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
						{
							((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_nUsableItemType = (AgpmItemUsableType)(atoi(pszData));

							if (((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_nUsableItemType == AGPMITEM_USABLE_TYPE_CONVERT_CATALYST)
								SetCatalystTID(pcsAgpdItemTemplate->m_lID);
							else if (((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_nUsableItemType == AGPMITEM_USABLE_TYPE_LUCKY_SCROLL)
								SetLuckyScrollTID(pcsAgpdItemTemplate->m_lID);
							else if (((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_BOOK)
							{
								INT32	lRace	= AURACE_TYPE_NONE;
								INT32	lClass	= AUCHARCLASS_TYPE_NONE;

								m_pagpmFactors->GetValue(&pcsAgpdItemTemplate->m_csRestrictFactor, &lRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
								m_pagpmFactors->GetValue(&pcsAgpdItemTemplate->m_csRestrictFactor, &lClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

								SetSkillBookTID((AuRaceType) lRace, (AuCharClassType) lClass, pcsAgpdItemTemplate->m_lID);
							}
							else if (((AgpdItemTemplateUsable *)(pcsAgpdItemTemplate))->m_nUsableItemType == AGPMITEM_USABLE_TYPE_REVERSE_ORB)
							{
								//SetReverseOrbTID(pcsAgpdItemTemplate->m_lID);
							}
						}
						else if (pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_OTHER)
						{
							// 2005.11.02. steeple 추가 작업
							AgpmItemOtherType eOtherType = (AgpmItemOtherType)atoi(pszData);
							switch(eOtherType)
							{
								case AGPMITEM_OTHER_TYPE_SKULL:
									((AgpdItemTemplateOther *) pcsAgpdItemTemplate)->m_eOtherItemType = AGPMITEM_OTHER_TYPE_SKULL;
									break;

								case AGPMITEM_OTHER_TYPE_MONEY:
									((AgpdItemTemplateOther *) pcsAgpdItemTemplate)->m_eOtherItemType = AGPMITEM_OTHER_TYPE_MONEY;
									break;

								case AGPMITEM_OTHER_TYPE_ETC_ITEM:
									((AgpdItemTemplateOther *) pcsAgpdItemTemplate)->m_eOtherItemType = AGPMITEM_OTHER_TYPE_ETC_ITEM;
									break;

								case AGPMITEM_OTHER_TYPE_FIRST_LOOTER_ONLY:
									((AgpdItemTemplateOther *) pcsAgpdItemTemplate)->m_eOtherItemType = AGPMITEM_OTHER_TYPE_FIRST_LOOTER_ONLY;
									break;

								case AGPMITEM_OTHER_TYPE_REMISSION:
									((AgpdItemTemplateOther *) pcsAgpdItemTemplate)->m_eOtherItemType = AGPMITEM_OTHER_TYPE_REMISSION;
									break;

								case AGPMITEM_OTHER_TYPE_ARCHON_SCROLL:
									((AgpdItemTemplateOther *) pcsAgpdItemTemplate)->m_eOtherItemType = AGPMITEM_OTHER_TYPE_ARCHON_SCROLL;
									break;
							}
						}
						else
						{
							TRACE("AgpmItem::StreamReadImportData() Error (33-4) !!!\n");
							continue;
						}
					}
				}
				break;
			case IMVINDEX_ITEM_SUB_TYPE				:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						if (pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
						{
							if (((AgpdItemTemplateUsable *) pcsAgpdItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL)
							{
								((AgpdItemTemplateUsableTeleportScroll *)(pcsAgpdItemTemplate))->m_eTeleportScrollType = (AgpmItemUsableTeleportScrollType) atoi(pszData);
							}
							else if (((AgpdItemTemplateUsable *) pcsAgpdItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_RUNE)
							{
								((AgpdItemTemplateUsableRune *) pcsAgpdItemTemplate)->m_eRuneAttributeType	= (AgpmItemRuneAttribute) atoi(pszData);
							}
							else if (((AgpdItemTemplateUsable *) pcsAgpdItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_POTION)
							{
								((AgpdItemTemplateUsablePotion *) pcsAgpdItemTemplate)->m_bIsPercentPotion	= TRUE;
							}
							else if (((AgpdItemTemplateUsable *) pcsAgpdItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL)
							{
								((AgpdItemTemplateUsableSkillScroll *) pcsAgpdItemTemplate)->m_eScrollSubType	= (AgpmItemUsableScrollSubType) atoi(pszData);
							}
							else if (((AgpdItemTemplateUsable *) pcsAgpdItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_AREA_CHATTING)
							{
								((AgpdItemTemplateUsableAreaChatting *) pcsAgpdItemTemplate)->m_eAreaChattingType	= (AgpmItemUsableAreaChattingType) atoi(pszData);
							}
							else
							{
								// 그냥 서브 타입에 세팅해 놓는다. 2005.11.22. steeple
								pcsAgpdItemTemplate->m_nSubType = atoi(pszData);

								if (((AgpdItemTemplateUsable *) pcsAgpdItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_CHATTING)
								{
									if (pcsAgpdItemTemplate->m_nSubType == (INT32) AGPMITEM_USABLE_CHATTING_TYPE_EMPHASIS)
										SetChattingEmphasisTID(pcsAgpdItemTemplate->m_lID);
								}
							}
						}
						else if (pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_OTHER &&
							((AgpdItemTemplateOther *) pcsAgpdItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_SKULL)
						{
							((AgpdItemTemplateOtherSkull *) pcsAgpdItemTemplate)->m_eOtherSkullType = (AgpmItemOtherSkullType) atoi(pszData);

							if (((AgpdItemTemplateOtherSkull *) pcsAgpdItemTemplate)->m_eOtherSkullType == AGPMITEM_OTHER_SKULL_TYPE_HUMAN)
								SetHumanSkullTID(pcsAgpdItemTemplate->m_lID);
							else if (((AgpdItemTemplateOtherSkull *) pcsAgpdItemTemplate)->m_eOtherSkullType == AGPMITEM_OTHER_SKULL_TYPE_ORC)
								SetOrcSkullTID(pcsAgpdItemTemplate->m_lID);
							else if (((AgpdItemTemplateOtherSkull *) pcsAgpdItemTemplate)->m_eOtherSkullType == AGPMITEM_OTHER_SKULL_TYPE_1)
								SetSkullTID(pcsAgpdItemTemplate->m_lID, 1);
							else if (((AgpdItemTemplateOtherSkull *) pcsAgpdItemTemplate)->m_eOtherSkullType == AGPMITEM_OTHER_SKULL_TYPE_2)
								SetSkullTID(pcsAgpdItemTemplate->m_lID, 2);
							else if (((AgpdItemTemplateOtherSkull *) pcsAgpdItemTemplate)->m_eOtherSkullType == AGPMITEM_OTHER_SKULL_TYPE_3)
								SetSkullTID(pcsAgpdItemTemplate->m_lID, 3);
							else if (((AgpdItemTemplateOtherSkull *) pcsAgpdItemTemplate)->m_eOtherSkullType == AGPMITEM_OTHER_SKULL_TYPE_4)
								SetSkullTID(pcsAgpdItemTemplate->m_lID, 4);
							else if (((AgpdItemTemplateOtherSkull *) pcsAgpdItemTemplate)->m_eOtherSkullType == AGPMITEM_OTHER_SKULL_TYPE_5)
								SetSkullTID(pcsAgpdItemTemplate->m_lID, 5);
						}
						else if (pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP)
						{

							pcsAgpdItemTemplate->m_nSubType = pExcel->GetDataToInt(nCol, nRow);
						}
					}
				}
				break;
			case IMVINDEX_ITEM_EXTRA_TYPE				:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						pcsAgpdItemTemplate->m_lExtraType = atoi(pszData);
					}
				}
				break;
			case IMVINDEX_ITEM_SPIRIT_STONE_TYPE		:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						if (pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE ||
							((AgpdItemTemplateUsable *) pcsAgpdItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
						{
							TRACE("AgpmItem::StreamReadImportData() Error (34) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						((AgpdItemTemplateUsableSpiritStone *)(pcsAgpdItemTemplate))->m_eSpiritStoneType = (AgpmItemUsableSpiritStoneType) atoi(pszData);
					}
				}
				break;
			case IMVINDEX_ITEM_BOUND_TYPE				:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						SetBoundType(pcsAgpdItemTemplate, (AgpmItemBoundTypes) atoi(pszData));
					}
				}
				break;
			/*case IMVINDEX_ITEM_RUNE_ATTRIBUTE_VALUE:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						if (pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE ||
							((AgpdItemTemplateUsable *) pcsAgpdItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_RUNE)
						{
							TRACE("AgpmItem::StreamReadImportData() Error (134) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						((AgpdItemTemplateUsableRune *)(pcsAgpdItemTemplate))->m_lValue = atoi(pszData);
					}
				}
				break;*/
			case IMVINDEX_ITEM_TARGET_TID				:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						if (pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE ||
							((AgpdItemTemplateUsable *) pcsAgpdItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_TRANSFORM)
						{
							TRACE("AgpmItem::StreamReadImportData() Error (134) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						((AgpdItemTemplateUsableTransform *)(pcsAgpdItemTemplate))->m_lTargetTID = atoi(pszData);
					}
				}
				break;
			case IMVINDEX_ITEM_DURATION				:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						if (pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_USABLE ||
							((AgpdItemTemplateUsable *) pcsAgpdItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_TRANSFORM)
						{
							TRACE("AgpmItem::StreamReadImportData() Error (135) !!!\n");
							pExcel->CloseFile();
							return FALSE;
						}

						((AgpdItemTemplateUsableTransform *)(pcsAgpdItemTemplate))->m_ulDuration = atoi(pszData);
					}
				}
				break;
			case IMVINDEX_FIRST_CATEGORY				:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						pcsAgpdItemTemplate->m_lFirstCategory = atoi(pszData);
					}
				}
				break;
			case IMVINDEX_SECOND_CATEGORY				:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						pcsAgpdItemTemplate->m_lSecondCategory = atoi(pszData);
					}
				}
				break;
			case IMVINDEX_FIRST_CATEGORY_NAME			:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
					}
				}
				break;
			case IMVINDEX_SECOND_CATEGORY_NAME			:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
					}
				}
				break;
			case IMVINDEX_USE_SKILL_ID					:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						((AgpdItemTemplateUsableSkillScroll *) pcsAgpdItemTemplate)->m_lSkillTID	= atoi(pszData);
					}
				}
				break;
			case IMVINDEX_USE_SKILL_LEVEL				:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						((AgpdItemTemplateUsableSkillScroll *) pcsAgpdItemTemplate)->m_lSkillLevel	= atoi(pszData);
					}
				}
				break;
			case IMVINDEX_TITLE_FONT_COLOR				:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						((AgpdItemTemplateUsableSkillScroll *) pcsAgpdItemTemplate)->m_ulTitleFontColor	= atoi(pszData);
					}
				}
				break;
			case IMVINDEX_MIN_SOCKET					:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						pcsAgpdItemTemplate->m_lMinSocketNum	= atoi(pszData);
					}
				}
				break;
			case IMVINDEX_MAX_SOCKET					:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						pcsAgpdItemTemplate->m_lMaxSocketNum	= atoi(pszData);
					}
				}
				break;
			case IMVINDEX_MIN_OPTION					:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						pcsAgpdItemTemplate->m_lMinOptionNum	= atoi(pszData);
					}
				}
				break;
			case IMVINDEX_MAX_OPTION					:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						pcsAgpdItemTemplate->m_lMaxOptionNum	= atoi(pszData);
					}
				}
				break;
			case IMVINDEX_EVENT_ITEM					:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						pcsAgpdItemTemplate->m_bIsEventItem	= (BOOL) atoi(pszData);
					}
				}
				break;
			case IMVINDEX_PCBANG_ONLY					:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						pcsAgpdItemTemplate->m_lIsUseOnlyPCBang	= atoi(pszData);
					}
				}
				break;
			case IMVINDEX_VILLAIN_ONLY					:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						pcsAgpdItemTemplate->m_bIsVillainOnly	= (BOOL) atoi(pszData);
					}
				}
				break;
			case IMVINDEX_QUESTITEM					:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
					{
						// 퀘스트 그룹아이디를 여기서 읽음..
						pcsAgpdItemTemplate->m_nQuestGroup		= atoi(pszData);
					}
				}
				break;
			case IMVINDEX_STAMINACURE:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
						pcsAgpdItemTemplate->m_llStaminaCure = 1000 * pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IMVINDEX_REMAIN_STAMINA_TIME:
				{
					pszData = pExcel->GetData(nCol, nRow);
					if (pszData)
						pcsAgpdItemTemplate->m_llStaminaRemainTime = 1000 * pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			default:
				PVOID	pvBuffer[3];
				pvBuffer[0]				= (PVOID)	pExcel;
				pvBuffer[1]				= (PVOID)	nRow;
				pvBuffer[2]				= (PVOID)	nCol;

				if (!EnumCallback(ITEM_CB_ID_STREAM_READ_IMPORT_DATA, pcsAgpdItemTemplate, pvBuffer))
				{
					pExcel->CloseFile();
					TRACE("AgpmItem::StreamReadImportData() Error (300) !!!\n");
					return FALSE;
				}
				break;
			}
		}
	}

	pExcel->CloseFile();
	return TRUE;
}

BOOL AgpmItem::CBInitTemplateDefaultValue(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmItem				*pThis						= (AgpmItem *)				pClass;
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate	= (AgpdCharacterTemplate *)	pData;

	//if (pcsAgpdCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_PC)
	{
		AgpdItemADCharTemplate	*pcsItemADCharTemplate		= pThis->GetADCharacterTemplate(pcsAgpdCharacterTemplate);
		if (pcsItemADCharTemplate)
		{
			pcsItemADCharTemplate->m_lDefaultEquipITID.MemSetAll();
			ZeroMemory(pcsItemADCharTemplate->m_aalDefaultInvenInfo, sizeof(pcsItemADCharTemplate->m_aalDefaultInvenInfo));
		}
	}

	return TRUE;
}

BOOL AgpmItem::CharacterImportDataReadCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmItem				*pThis						= (AgpmItem *)				pClass;
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate	= (AgpdCharacterTemplate *)	pData;
	PVOID					*ppvBuffer					= (PVOID *)					pCustData;

	AuExcelLib				*pcsExcelTxtLib				= (AuExcelLib *)			ppvBuffer[0];
	INT32					nRow						= PtrToInt(ppvBuffer[1]);
	INT32					nCol						= PtrToInt(ppvBuffer[2]);

	if (!pcsExcelTxtLib)
		return FALSE;

	// key row is zero
	//////////////////////////////////////////////////////////

	CHAR					*pszData				= pcsExcelTxtLib->GetData(nCol, 0);
	if(!pszData)
		return FALSE;

	INT32					lTemp, lTemp2;

	if ((!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_EQUIP_RHAND)) || (!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_EQUIP_LHAND)))
	{
		lTemp	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
			lTemp = atoi(pszData);

		if(!pThis->SetDefaultEquipments(pcsAgpdCharacterTemplate, lTemp))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (38) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_EQUIP_HEAD))
	{
		lTemp	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
			lTemp = atoi(pszData);

		if(!pThis->SetDefaultEquipments(pcsAgpdCharacterTemplate, lTemp, AGPMITEM_PART_HEAD))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (39) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_EQUIP_BODY))
	{
		lTemp	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
			lTemp = atoi(pszData);

		if(!pThis->SetDefaultEquipments(pcsAgpdCharacterTemplate, lTemp, AGPMITEM_PART_BODY))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (40) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_EQUIP_ARM))
	{
		lTemp	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			lTemp = atoi(pszData);
		}

		if(!pThis->SetDefaultEquipments(pcsAgpdCharacterTemplate, lTemp, AGPMITEM_PART_ARMS))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (41) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_EQUIP_HAND))
	{
		lTemp	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			lTemp = atoi(pszData);
		}

		if(!pThis->SetDefaultEquipments(pcsAgpdCharacterTemplate, lTemp, AGPMITEM_PART_HANDS))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (42) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_EQUIP_LEG))
	{
		lTemp	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			lTemp = atoi(pszData);
		}

		if(!pThis->SetDefaultEquipments(pcsAgpdCharacterTemplate, lTemp, AGPMITEM_PART_LEGS))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (43) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_EQUIP_FOOT))
	{
		lTemp	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			lTemp = atoi(pszData);
		}

		if(!pThis->SetDefaultEquipments(pcsAgpdCharacterTemplate, lTemp, AGPMITEM_PART_FOOT))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (44) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_EQUIP_ARM2))
	{
		lTemp	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			lTemp = atoi(pszData);
		}

		if(!pThis->SetDefaultEquipments(pcsAgpdCharacterTemplate, lTemp, AGPMITEM_PART_ARMS2))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (45) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_WEAPON))
	{
		lTemp	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			lTemp = atoi(pszData);
		}

		AgpdItemTemplate	*pcsItemTemplate	= pThis->GetItemTemplate(lTemp);
		if (pcsItemTemplate && pcsItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP)
		{
			if(!pThis->SetDefaultEquipments(pcsAgpdCharacterTemplate, lTemp, ((AgpdItemTemplateEquip *) pcsItemTemplate)->m_nPart))
			{
				TRACE("AgpmItem::CharacterImportDataReadCB() Error (45) !!!\n");
				return FALSE;
			}
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_INVENTORY_ITEM1))
	{
		lTemp	= 0;
		lTemp2	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			sscanf(pszData, "%d;%d", &lTemp, &lTemp2);
//					lTemp = atoi(pszData);
		}

		if(!pThis->SetDefaultInventoryItems(pcsAgpdCharacterTemplate, lTemp, lTemp2, 0))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (46) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_INVENTORY_ITEM2))
	{
		lTemp	= lTemp2	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			sscanf(pszData, "%d;%d", &lTemp, &lTemp2);
		}

		if(!pThis->SetDefaultInventoryItems(pcsAgpdCharacterTemplate, lTemp, lTemp2, 1))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (47) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_INVENTORY_ITEM3))
	{
		lTemp	= lTemp2	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			sscanf(pszData, "%d;%d", &lTemp, &lTemp2);
		}

		if(!pThis->SetDefaultInventoryItems(pcsAgpdCharacterTemplate, lTemp, lTemp2, 2))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (48) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_INVENTORY_ITEM4))
	{
		lTemp	= lTemp2	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			sscanf(pszData, "%d;%d", &lTemp, &lTemp2);
		}

		if(!pThis->SetDefaultInventoryItems(pcsAgpdCharacterTemplate, lTemp, lTemp2, 3))
		{
			TRACE("AgpmItem::CharacterImportDataReadCB() Error (49) !!!\n");
			return FALSE;
		}
	}
	else if(!strcmp(pszData, AGPMITEM_IMPORT_CHAR_DEFAULT_MONEY))
	{
		lTemp	= 0;

		pszData = pcsExcelTxtLib->GetData(nCol, nRow);
		if(pszData)
		{
			sscanf(pszData, "%d", &lTemp);
		}

		AgpdItemADCharTemplate *pstAgpdItemADCharTemplate = pThis->GetADCharacterTemplate(pcsAgpdCharacterTemplate);
		if(!pstAgpdItemADCharTemplate)
			return FALSE;

		pstAgpdItemADCharTemplate->m_lDefaultMoney	= lTemp;
	}

	return TRUE;
}

BOOL AgpmItem::SetDefaultEquipments(AgpdCharacterTemplate *pstAgpdCharacterTemplate, INT32 lITID, INT32 lPart)
{
	if (lITID == 0)
		return TRUE;

	AgpdItemADCharTemplate *pstAgpdItemADCharTemplate = GetADCharacterTemplate(pstAgpdCharacterTemplate);
	if(!pstAgpdItemADCharTemplate)
		return FALSE;

	if(lITID)
	{
		AgpdItemTemplate *pstAgpdItemTemplate = GetItemTemplate(lITID);
		if((!pstAgpdItemTemplate) || (pstAgpdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP))
			return FALSE;

		AgpdItemTemplateEquip *pstAgpdItemTemplateEquip = (AgpdItemTemplateEquip *)(pstAgpdItemTemplate);
//		if((pstAgpdItemTemplateEquip->m_nKind != lKind) || ((lPart) && (pstAgpdItemTemplateEquip->m_nPart != lPart)))
//			return FALSE;

//		pstAgpdItemADCharTemplate = GetADCharacterTemplate(pstAgpdCharacterTemplate);
//		if(!pstAgpdItemADCharTemplate)
//			return FALSE;

		pstAgpdItemADCharTemplate->m_lDefaultEquipITID[pstAgpdItemTemplateEquip->m_nPart] = lITID;
	}
	else if(lPart)
	{
		pstAgpdItemADCharTemplate->m_lDefaultEquipITID[lPart] = 0;
	}

	return TRUE;
}

BOOL AgpmItem::SetDefaultInventoryItems(AgpdCharacterTemplate *pstAgpdCharacterTemplate, INT32 lITID, INT32 lNum, INT32 lIndex)
{
	if (lITID == 0)
		return TRUE;

	AgpdItemTemplate *pstAgpdItemTemplate = GetItemTemplate(lITID);
	if(!pstAgpdItemTemplate)
		return FALSE;

	AgpdItemADCharTemplate *pstAgpdItemADCharTemplate = GetADCharacterTemplate(pstAgpdCharacterTemplate);
	if(!pstAgpdItemADCharTemplate)
		return FALSE;

	pstAgpdItemADCharTemplate->m_aalDefaultInvenInfo[lIndex][AGPDITEM_DEFAULT_INVEN_INDEX_TID]			= lITID;
	pstAgpdItemADCharTemplate->m_aalDefaultInvenInfo[lIndex][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT]	= lNum;

	return TRUE;
}

BOOL AgpmItem::StreamReadTransformData(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		TRACE("AgpmItem::StreamReadTransformData() Error (OpenExcelFile) !!!\n");
		csExcelTxtLib.CloseFile();
		return FALSE;
	}

	INT32	lNumRow			= csExcelTxtLib.GetRow();
	INT32	lNumColumn		= csExcelTxtLib.GetColumn();

	CHAR	*szColumnName	= NULL;
	CHAR	*szValue		= NULL;

	for (int i = 1; i < lNumRow; ++i)
	{
		szValue	= csExcelTxtLib.GetData(0, i);
		if (!szValue)
			continue;

		AgpdItemTemplateUsableTransform	*pcsTemplate	= (AgpdItemTemplateUsableTransform *) GetItemTemplate(atoi(szValue));
		if (!pcsTemplate)
			continue;

		for (int j = 1; j < lNumColumn; ++j)
		{
			szColumnName	= csExcelTxtLib.GetData(j, 0);
			szValue			= csExcelTxtLib.GetData(j, i);

			if (!szColumnName || !szValue)
				continue;

			if (strncmp(szColumnName, AGPMITEM_TRANSFORM_ITEM_MAX_HP, strlen(AGPMITEM_TRANSFORM_ITEM_MAX_HP)) == 0)
			{
				INT32	lMaxHP	= atoi(szValue);

				pcsTemplate->m_lAddMaxHP	= lMaxHP;

				m_pagpmFactors->SetValue(&pcsTemplate->m_csEffectFactor, lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
			}
			else if (strncmp(szColumnName, AGPMITEM_TRANSFORM_ITEM_MAX_MP, strlen(AGPMITEM_TRANSFORM_ITEM_MAX_MP)) == 0)
			{
				INT32	lMaxMP	= atoi(szValue);

				pcsTemplate->m_lAddMaxMP	= lMaxMP;

				m_pagpmFactors->SetValue(&pcsTemplate->m_csEffectFactor, lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
			}
			else if (strncmp(szColumnName, AGPMITEM_TRANSFORM_ITEM_MIN_DAMAGE, strlen(AGPMITEM_TRANSFORM_ITEM_MIN_DAMAGE)) == 0)
			{
				INT32	lMinDamage	= atoi(szValue);

				m_pagpmFactors->SetValue(&pcsTemplate->m_csEffectFactor, lMinDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			}
			else if (strncmp(szColumnName, AGPMITEM_TRANSFORM_ITEM_MAX_DAMAGE, strlen(AGPMITEM_TRANSFORM_ITEM_MAX_DAMAGE)) == 0)
			{
				INT32	lMaxDamage	= atoi(szValue);

				m_pagpmFactors->SetValue(&pcsTemplate->m_csEffectFactor, lMaxDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			}
			else if (strncmp(szColumnName, AGPMITEM_TRANSFORM_ITEM_DEFENSE, strlen(AGPMITEM_TRANSFORM_ITEM_DEFENSE)) == 0)
			{
				INT32	lDefense	= atoi(szValue);

				m_pagpmFactors->SetValue(&pcsTemplate->m_csEffectFactor, lDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			}
			else if (strncmp(szColumnName, AGPMITEM_TRANSFORM_ITEM_MOVEMENT_FAST, strlen(AGPMITEM_TRANSFORM_ITEM_MOVEMENT_FAST)) == 0)
			{
				INT32	lMovementFast	= atoi(szValue);

				m_pagpmFactors->SetValue(&pcsTemplate->m_csEffectFactor, lMovementFast, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
			}
			else if (strncmp(szColumnName, AGPMITEM_TRANSFORM_ITEM_AR, strlen(AGPMITEM_TRANSFORM_ITEM_AR)) == 0)
			{
				INT32	lAR		= atoi(szValue);

				m_pagpmFactors->SetValue(&pcsTemplate->m_csEffectFactor, lAR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR);
			}
			else if (strncmp(szColumnName, AGPMITEM_TRANSFORM_ITEM_DR, strlen(AGPMITEM_TRANSFORM_ITEM_AR)) == 0)
			{
				INT32	lDR		= atoi(szValue);

				m_pagpmFactors->SetValue(&pcsTemplate->m_csEffectFactor, lDR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_DR);
			}
			else if (strncmp(szColumnName, AGPMITEM_TRANSFORM_ITEM_ATK_SPEED, strlen(AGPMITEM_TRANSFORM_ITEM_ATK_SPEED)) == 0)
			{
				INT32	lAtkSpeed		= atoi(szValue);

				m_pagpmFactors->SetValue(&pcsTemplate->m_csEffectFactor, lAtkSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
			}
		}
	}

	csExcelTxtLib.CloseFile();

	return TRUE;
}

BOOL AgpmItem::StreamReadOptionData(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile)
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		TRACE("AgpmItem::StreamReadOptionData() Error (1) !!!\n");
		csExcelTxtLib.CloseFile();
		return FALSE;
	}

	INT32	lNumRow			= csExcelTxtLib.GetRow();
	INT32	lNumColumn		= csExcelTxtLib.GetColumn();

	CHAR	*szColumnName	= NULL;
	CHAR	*szValue		= NULL;

	for (int i = 1; i < lNumRow; ++i)
	{
		szValue	= csExcelTxtLib.GetData(0, i);
		if (!szValue)
			continue;

		AgpdItemOptionTemplate	*pcsOptionTemplate	= (AgpdItemOptionTemplate *) AddItemOptionTemplate(atoi(szValue));
		if (!pcsOptionTemplate)
		{
			pcsOptionTemplate	= GetItemOptionTemplate(atoi(szValue));
			if (!pcsOptionTemplate)
				continue;
		}

		for (int j = 1; j < lNumColumn; ++j)
		{
			szColumnName	= csExcelTxtLib.GetData(j, 0);
			szValue			= csExcelTxtLib.GetData(j, i);

			if (!szColumnName || !szValue)
				continue;

			if (strcmp(szColumnName, AGPMITEM_OPTION_SKILL_LEVEL) == 0)
			{
				INT32	lSkillLevel	= atoi(szValue);

				pcsOptionTemplate->m_lSkillLevel	= lSkillLevel;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SKILL_ID) == 0)
			{
				INT32	lSkillTID	= atoi(szValue);

				pcsOptionTemplate->m_lSkillTID		= lSkillTID;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_BODY) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_BODY;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_LEGS) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_LEGS;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_WEAPON) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_WEAPON;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_SHIELD) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_SHIELD;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_HEAD) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_HEAD;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_RING) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_RING;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_NECKLACE) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_NECKLACE;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_FOOTS) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_FOOTS;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_HANDS) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_HANDS;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_REFINERY) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_REFINERY;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE_GACHA) == 0)
			{
				pcsOptionTemplate->m_ulSetPart	|= AGPDITEM_OPTION_SET_TYPE_GACHA;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_ATK_RANGE_BONUS) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_ATK_SPEED_BONUS) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_MOVEMENT_BONUS) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
				}
				else
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_PHYSICAL_DAMAGE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
				}
				else
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_PHYSICAL_ATTACK_RESIST) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SKILL_BLOCK_RATE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_SKILL_BLOCK);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_SKILL_BLOCK);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_MAGIC_DAMAGE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
				}
				else
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_WATER_DAMAGE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
				}
				else
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_FIRE_DAMAGE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
				}
				else
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_EARTH_DAMAGE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
				}
				else
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_AIR_DAMAGE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
				}
				else
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_POISON_DAMAGE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON);
				}
				else
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON);
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_ICE_DAMAGE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE);
				}
				else
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE);
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_LIGHTENING_DAMAGE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING);
				}
				else
				{
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING);
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING);
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_FIRE_RESIST) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_WATER_RESIST) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_AIR_RESIST) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_EARTH_RESIST) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_MAGIC_RESIST) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_POISON_RESIST) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_ICE_RESIST) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_LIGHTENING_RESIST) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_PHYSICAL_DEFENSE_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_PHYSICAL_ATTACK_RATE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_MAGIC_ATTACK_RATE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAR);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAR);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_BLOCK_RATE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL_BLOCK);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL_BLOCK);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_MAGIC_DEFENSE_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_WATER_DEFENSE_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_FIRE_DEFENSE_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_EARTH_DEFENSE_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_AIR_DEFENSE_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_POISON_DEFENSE_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_ICE_DEFENSE_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_LIGHTENING_DEFENSE_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_HP_BONUS_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_MP_BONUS_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_HP_BONUS_PERCENT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_MP_BONUS_PERCENT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_STR_BONUS_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_DEX_BONUS_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_INT_BONUS_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_CON_BONUS_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_WIS_BONUS_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_RANK_POINT) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_PHYSICAL_RANK);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactorPercent, lValue, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_PHYSICAL_RANK);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_DESCRIPTION) == 0)
			{
				strncpy(pcsOptionTemplate->m_szDescription, szValue, AGPMITEM_OPTION_MAX_DESCRIPTION);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_VISIBLE) == 0)
			{
				pcsOptionTemplate->m_bIsVisible	= TRUE;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SET_TYPE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (lValue >= AGPDITEM_OPTION_SET_DROP &&
					lValue <= AGPDITEM_OPTION_SET_ALL)
				{
					pcsOptionTemplate->m_eOptionSetType	= (AgpdItemOptionSetType)	lValue;
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_TYPE) == 0)
			{
				pcsOptionTemplate->m_lType	= atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_POINT_TYPE) == 0)
			{
				pcsOptionTemplate->m_lPointType	= atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_LEVEL_LIMIT) == 0)
			{
				pcsOptionTemplate->m_lLevelLimit	= atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_RANK_LIMIT) == 0)
			{
				pcsOptionTemplate->m_lRankLimit	= atoi(szValue);

				if( pcsOptionTemplate->m_lRankLimit == 0 )
					pcsOptionTemplate->m_lRankLimit = 1;
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_PROBABILITY) == 0)
			{
				pcsOptionTemplate->m_lProbability	= atoi(szValue);
			}

			// 2006.12.12. steeple
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SKILL_TYPE) == 0)
			{
				pcsOptionTemplate->m_eSkillType = (AgpdItemOptionSkillType)atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_DURATION) == 0)
			{
				switch(pcsOptionTemplate->m_eSkillType)
				{
					case AGPDITEM_OPTION_SKILL_TYPE_STUN:
						pcsOptionTemplate->m_stSkillData.m_ulStunTime = (UINT32)atol(szValue);
						break;

					case AGPDITEM_OPTION_SKILL_TYPE_DOT:
						pcsOptionTemplate->m_stSkillData.m_ulDotDamageTime = (UINT32)atol(szValue);
						break;

					default:
						break;
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SKILL_RATE) == 0)
			{
				switch(pcsOptionTemplate->m_eSkillType)
				{
					case AGPDITEM_OPTION_SKILL_TYPE_CRITICAL:
						pcsOptionTemplate->m_stSkillData.m_lCriticalRate = atoi(szValue);
						break;

					case AGPDITEM_OPTION_SKILL_TYPE_STUN:
						pcsOptionTemplate->m_stSkillData.m_lStunRate = atoi(szValue);
						break;

					case AGPDITEM_OPTION_SKILL_TYPE_DMG_CONVERT_HP:
						pcsOptionTemplate->m_stSkillData.m_lDamageConvertHPRate = atoi(szValue);
						break;

					case AGPDITEM_OPTION_SKILL_TYPE_DOT:
						pcsOptionTemplate->m_stSkillData.m_lDotDamageRate = atoi(szValue);
						break;

					default:
						break;
				}
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_CRITICAL) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lCritical = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_STUN_TIME) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_ulStunTime = (UINT32)atol(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_DAMAGE_CONVERT_HP) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lDamageConvertHP = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_REGEN_HP) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lRegenHP = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_REGEN_MP) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lRegenMP = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_DOT_DAMAGE_TIME) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_ulDotDamageTime = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SKILL_CAST) == 0)
			{
				//pcsOptionTemplate->m_stSkillData.m_lSkillCast = atoi(szValue);

				//INT32	lValue	= atoi(szValue);
				//m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SKILL_CAST);

				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactor, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SKILL_CAST);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactorPercent, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SKILL_CAST);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SKILL_DELAY) == 0)
			{
				//pcsOptionTemplate->m_stSkillData.m_lSkillDelay = atoi(szValue);

				//INT32	lValue	= atoi(szValue);
				//m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csFactor, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SKILL_DELAY);

				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactor, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SKILL_DELAY);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactorPercent, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SKILL_DELAY);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_SKILL_LEVELUP) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lSkillLevelUp = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_BONUS_EXP) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lBonusExp = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_BONUS_MONEY) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lBonusMoney = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_BONUS_DROP_RATE) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lBonusDropRate = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_BONUS_CHARISMA_DROP_RATE) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lBonusCharismaRate = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_EVADE_RATE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactor, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_EVADE_RATE);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactorPercent, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_EVADE_RATE);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_DODGE_RATE) == 0)
			{
				INT32	lValue	= atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactor, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_DODGE_RATE);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactorPercent, lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_DODGE_RATE);
			}
			else if (strcmp(szColumnName, AGPMITEM_IMPORT_ITEM_HEROIC_MIN_DAMAGE) == 0)
			{
				INT32	lValue = atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC);
			}
			else if (strcmp(szColumnName, AGPMITEM_IMPORT_ITEM_HEROIC_MAX_DAMAGE) == 0)
			{
				INT32	lValue = atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactor, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactorPercent, lValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC);
			}
			else if (strcmp(szColumnName, AGPMITEM_IMPORT_ITEM_HEROIC_DEFENSE_POINT) == 0)
			{
				INT32	lValue = atoi(szValue);

				if (pcsOptionTemplate->m_lPointType == 0)
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactor, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC);
				else
					m_pagpmFactors->SetValue(&pcsOptionTemplate->m_csSkillFactorPercent, lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_LEVEL_MIN) == 0)
			{
				pcsOptionTemplate->m_lLevelMin = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_LEVEL_MAX) == 0)
			{
				pcsOptionTemplate->m_lLevelMax= atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_LINK_ID) == 0)
			{
				pcsOptionTemplate->m_lLinkID = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_IGNORE_PHYSICAL_DEFENSE) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lIgnorePhysicalDefenseRate = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_IGNORE_ATTRIBUTE_DEFENSE) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lIgnoreAttributeDefenseRate = atoi(szValue);
			}
			else if (strcmp(szColumnName, AGPMITEM_OPTION_CRITICAL_DEFENSE) == 0)
			{
				pcsOptionTemplate->m_stSkillData.m_lCriticalDefenseRate = atoi(szValue);
			}

		}
	}

	csExcelTxtLib.CloseFile();

	return TRUE;
}

BOOL AgpmItem::StreamReadBankSlotPrice(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		TRACE("AgpmItem::StreamReadBankSlotPrice() Error (OpenExcelFile) !!!\n");
		csExcelTxtLib.CloseFile();
		return FALSE;
	}

	INT32	lNumRow			= csExcelTxtLib.GetRow();
	INT32	lNumColumn		= csExcelTxtLib.GetColumn();

	for (int i = 1; i < lNumRow; ++i)
	{
		CHAR	*pszSlotNumber	= csExcelTxtLib.GetData(0, i);
		if (!pszSlotNumber || atoi(pszSlotNumber) < 0 || atoi(pszSlotNumber) >= AGPMITEM_BANK_MAX_LAYER)
			continue;

		CHAR	*pszPrice		= csExcelTxtLib.GetData(1, i);
		if (!pszPrice)
			continue;

		m_llBankSlotPrice[atoi(pszSlotNumber)]	= _atoi64(pszPrice);
	}

	csExcelTxtLib.CloseFile();

	return TRUE;
}

BOOL AgpmItem::StreamReadLotteryBox(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		TRACE("AgpmItem::StreamReadLotteryBox() Error (OpenExcelFile) !!!\n");
		csExcelTxtLib.CloseFile();
		return FALSE;
	}

	INT32	lNumRow			= csExcelTxtLib.GetRow();
	INT32	lNumColumn		= csExcelTxtLib.GetColumn();

	for (int i = 1; i < lNumRow; ++i)
	{
		CHAR *szItemName = csExcelTxtLib.GetData(0, i);
		if (!szItemName)
		{
			ASSERT(0);
			return FALSE;
		}

		AgpdItemTemplate *pAgpdItemTemplate = GetItemTemplate(atoi(szItemName));
		if (!pAgpdItemTemplate)
		{
			ASSERT(0);
			return FALSE;
		}

		CHAR *szPotItemName = csExcelTxtLib.GetData(1, i);
		if (!szPotItemName)
		{
			ASSERT(0);
			return FALSE;
		}

		AgpdItemTemplate *pAgpdPotItemTemplate = GetItemTemplate(atoi(szPotItemName));
		if (!pAgpdPotItemTemplate
			&& strcmp(LOTTERY_BOX_POT_ITEM_NOTHING_ITEM_STRING, szPotItemName) != 0)
		{
			ASSERT(0);
			return FALSE;
		}

		AgpdLotteryItemInfo stLotteryBoxItemInfo;
		stLotteryBoxItemInfo.nPotItemTID
			= ((strcmp(LOTTERY_BOX_POT_ITEM_NOTHING_ITEM_STRING, szPotItemName) == 0)?LOTTERY_BOX_POT_ITEM_NOTHING_ITEM_TID:pAgpdPotItemTemplate->m_lID);
		stLotteryBoxItemInfo.nMinStackCount = atoi(csExcelTxtLib.GetData(2, i));
		stLotteryBoxItemInfo.nMaxStackCount = atoi(csExcelTxtLib.GetData(3, i));
		stLotteryBoxItemInfo.nPercent = atoi(csExcelTxtLib.GetData(4, i));

		MapLotteryBoxToItems::iterator itr = m_mapLotteryBoxToItems.find(pAgpdItemTemplate->m_lID);

		if (m_mapLotteryBoxToItems.end() == itr)
		{
			vector<AgpdLotteryItemInfo> tmpVector; tmpVector.push_back(stLotteryBoxItemInfo);
			m_mapLotteryBoxToItems.insert(pair<INT32, vector<AgpdLotteryItemInfo> >(pAgpdItemTemplate->m_lID, tmpVector));
		}
		else
		{
			vector<AgpdLotteryItemInfo> &pTmpVector = itr->second;
			stLotteryBoxItemInfo.nPercent += pTmpVector.back().nPercent;
			pTmpVector.push_back(stLotteryBoxItemInfo);
		}
	}

	csExcelTxtLib.CloseFile();

	return TRUE;
}

// 2006.08.28. steeple
// 무조건 붙이는 OptionTID 읽기.
BOOL AgpmItem::StreamReadOptionTID(AgpdItemTemplate* pcsItemTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow)
{
	if(!pcsItemTemplate)
		return FALSE;

	CHAR* szBuffer = NULL;
	CHAR* szToken = NULL;
	CHAR szSeps[] = ";";

	// strtok 는 multi-thread 에서 문제가 있긴 하지만,
	// 이 함수는 처음 뜰 때 한번만 사용하게 되므로 문제가 발생하지 않는다.

	szBuffer = csExcelTxtLib.GetData(lCurCol, lCurRow);
	if(!szBuffer || !strlen(szBuffer))
		return FALSE;

	INT32 lIndex = 0;
	// 문자열 형식은 xxx;xxx;xxx 이런식임
	szToken = strtok(szBuffer, szSeps);
	while(szToken)
	{
		pcsItemTemplate->m_alOptionTID[lIndex++] = atoi(szToken);
		if(lIndex >= AGPDITEM_OPTION_MAX_NUM)
			break;

		szToken = strtok(NULL, szSeps);
	}

	return TRUE;
}

// 2006.12.26. steeple
// 세트 아이템에 붙는 Link_id
BOOL AgpmItem::StreamReadLinkID(AgpdItemTemplate* pcsItemTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow)
{
	if(!pcsItemTemplate)
		return FALSE;

	CHAR* szBuffer = NULL;
	CHAR* szToken = NULL;
	CHAR szSeps[] = ";";

	szBuffer = csExcelTxtLib.GetData(lCurCol, lCurRow);
	if(!szBuffer || !strlen(szBuffer))
		return FALSE;

	INT32 lIndex = 0;
	// 문자열 형식은 xxx;xxx;xxx 이런식임
	szToken = strtok(szBuffer, szSeps);
	while(szToken)
	{
		pcsItemTemplate->m_alLinkID[lIndex++] = atoi(szToken);
		if(lIndex >= AGPDITEM_LINK_MAX_NUM)
			break;

		szToken = strtok(NULL, szSeps);
	}

	return TRUE;
}

// 2007.02.05. steeple
// Skill Plus TID
BOOL AgpmItem::StreamReadSkillPlusTID(AgpdItemTemplate* pcsItemTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow)
{
	if(!pcsItemTemplate)
		return FALSE;

	CHAR* szBuffer = NULL;
	CHAR* szToken = NULL;
	CHAR szSeps[] = ";";

	szBuffer = csExcelTxtLib.GetData(lCurCol, lCurRow);
	if(!szBuffer || !strlen(szBuffer))
		return FALSE;

	INT32 lIndex = 0;
	// 문자열 형식은 xxx;xxx;xxx 이런식임
	szToken = strtok(szBuffer, szSeps);
	while(szToken)
	{
		pcsItemTemplate->m_aunSkillPlusTID[lIndex++] = (UINT16)atoi(szToken);
		if(lIndex >= AGPMITEM_MAX_SKILL_PLUS)
			break;

		szToken = strtok(NULL, szSeps);
	}

	return TRUE;
}

void AgpmItem::ClearItemTemplateEntry( void )
{
	m_mapItemTemplates.clear();
}

BOOL AgpmItem::AddItemTemplateEntry( int nTID, string pName, string pFileName )
{
	if( nTID <= 0 ) return FALSE;
	if( pName.empty() || pName.size() <= 0) return FALSE;
	if( pFileName.empty() || pFileName.size() <= 0) return FALSE;

	std::map< int, stItemTemplateEntry >::iterator Iter;
	Iter = m_mapItemTemplates.find( nTID );

	if( Iter != m_mapItemTemplates.end() )
	{
		stItemTemplateEntry* pEntry = &Iter->second;
		if( pEntry )
		{
			pEntry->m_nTID = nTID;
			pEntry->m_strTemplateName = pName;
			pEntry->m_strTemplateFileName = pFileName;
			//strcpy_s( pEntry->m_strTemplateName, sizeof( char ) * ( AGPMITEM_MAX_ITEM_NAME + 1 ), pName.c_str() );
			//strcpy_s( pEntry->m_strTemplateFileName, sizeof( char ) * 256, pFileName.c_str() );
		}
	}
	else
	{
		stItemTemplateEntry NewEntry;

		NewEntry.m_nTID = nTID;
		NewEntry.m_strTemplateName = pName;
		NewEntry.m_strTemplateFileName = pFileName;
		//strcpy_s( NewEntry.m_strTemplateName, sizeof( char ) * ( AGPMITEM_MAX_ITEM_NAME + 1 ), pName.c_str() );
		//strcpy_s( NewEntry.m_strTemplateFileName, sizeof( char ) * 256, pFileName.c_str() );

		m_mapItemTemplates.insert( std::map< int, stItemTemplateEntry >::value_type( nTID, NewEntry ) );
	}

	return TRUE;
}

string AgpmItem::GetItemTemplateName( int nTID )
{
	std::map< int, stItemTemplateEntry >::iterator Iter;
	Iter = m_mapItemTemplates.find( nTID );

	if( Iter == m_mapItemTemplates.end() ) return "";
	
	stItemTemplateEntry* pEntry = &Iter->second;
	if( !pEntry ) return "";

	return pEntry->m_strTemplateName;
}

BOOL AgpmItem::LoadItemTemplateEntryINI( char* pFileName, BOOL bEncryption )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	ApModuleStream csStream;
	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );
	if( !csStream.Open( pFileName, 0, bEncryption ) ) return FALSE;

	ClearItemTemplateEntry();

	int nSectionCount = csStream.GetNumSections();
	for( int nCount = 0 ; nCount < nSectionCount ; nCount++ )
	{
		int nTID = atoi( csStream.ReadSectionName( nCount ) );
		if( nTID > 0 )
		{
			csStream.ReadNextValue();

			const char* pValueName = csStream.GetValueName();
			if( !pValueName || strlen( pValueName ) <= 0 ) continue;

			string pTemplateName;
			if( strcmp( pValueName, "TemplateName" ) == 0 )
			{
				pTemplateName = ( char* )csStream.GetValue();
				if( pTemplateName.empty() || pTemplateName.length() <= 0) continue;
			}

			csStream.ReadNextValue();
			
			pValueName = csStream.GetValueName();
			if( !pValueName || strlen( pValueName ) <= 0 ) continue;

			string pTemplateFileName;
			if( strcmp( pValueName, "TemplateFileName" ) == 0 )
			{
				pTemplateFileName = ( char* )csStream.GetValue();
				if( pTemplateFileName.empty() || pTemplateFileName.length() <= 0) continue;
			}

			AddItemTemplateEntry( nTID, pTemplateName, pTemplateFileName );
		}
	}
	
#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, 256, "Load ItemTemplate Entries, FileName = %s\n", pFileName );
	OutputDebugString( strDebug );
#endif

	return TRUE;
}

BOOL AgpmItem::LoadItemTemplateEntryXML( char* pFileName, BOOL bEncryption )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	m_DocItemTemplateEntry.Clear();
	if( !_LoadXmlFile( &m_DocItemTemplateEntry, pFileName, bEncryption ) ) return FALSE;

	TiXmlNode* pHeadNode = m_DocItemTemplateEntry.FirstChild( "ItemTemplateEntry" );
	if( !pHeadNode ) return FALSE;

	int nReadTemplateCount = 0;
	TiXmlNode* pNodeTemplate = pHeadNode->FirstChild( "ItemTemplate" );
	while( pNodeTemplate )
	{
		const char* pTemplateTID = pNodeTemplate->ToElement()->Attribute( "TID" );
		if( pTemplateTID && strlen( pTemplateTID ) > 0 )
		{
			int nTID = atoi( pTemplateTID );

			const char* pTemplateName = pNodeTemplate->ToElement()->Attribute( "Name" );
			if( pTemplateName && strlen( pTemplateName ) > 0 )
			{
				const char* pTemplateFileName = pNodeTemplate->ToElement()->Attribute( "FileName" );
				if( pTemplateFileName && strlen( pTemplateFileName ) > 0 )
				{
					AddItemTemplateEntry( nTID, ( char* )pTemplateName, ( char* )pTemplateFileName );
				}
#ifdef _DEBUG
				else
				{
					char strDebug[ 256 ] = { 0, };
					sprintf_s( strDebug, 256, "ItemTemplate FileName is invalid, TID = %d, Name = %s\n", nTID, pTemplateName );
					OutputDebugString( strDebug );
				}
#endif
			}
#ifdef _DEBUG
			else
			{
				char strDebug[ 256 ] = { 0, };
				sprintf_s( strDebug, 256, "ItemTemplate Name is invalid, TID = %d\n", nTID );
				OutputDebugString( strDebug );
			}
#endif
		}
#ifdef _DEBUG
		else
		{
			char strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, 256, "ItemTemplate TID is invalid, Index = %d\n", nReadTemplateCount );
			OutputDebugString( strDebug );
		}
#endif

		pNodeTemplate = pNodeTemplate->NextSibling();
		nReadTemplateCount++;
	}	

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, 256, "Load ItemTemplate Entries, FileName = %s\n", pFileName );
	OutputDebugString( strDebug );
#endif

	return TRUE;
}

BOOL AgpmItem::SaveItemTemplateEntryINI( char* pFileName, BOOL bEncryption )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	ApModuleStream csStream;
	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );
	
	if( bEncryption )
	{
		csStream.SetType( AUINIMANAGER_TYPE_KEYINDEX );
	}

	std::map< int, stItemTemplateEntry >::iterator Iter;
	Iter = m_mapItemTemplates.begin();

	while( Iter != m_mapItemTemplates.end() )
	{
		stItemTemplateEntry* pEntry = &Iter->second;
		if( !pEntry ) continue;

		// TID로 Section을 설정하고
		char strSectionName[ 32 ] = { 0, };
		sprintf_s( strSectionName, sizeof(strSectionName), "%d", pEntry->m_nTID );
		csStream.SetSection( strSectionName );
		csStream.WriteValue( "TemplateName", pEntry->m_strTemplateName.c_str() );
		csStream.WriteValue( "TemplateFileName", pEntry->m_strTemplateFileName.c_str() );

		Iter++;
	}

	csStream.Write( pFileName, 0, bEncryption );

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, 256, "Write ItemTemplate Entry File, FileName = %s\n", pFileName );
	OutputDebugString( strDebug );
#endif

	return TRUE;
}

BOOL AgpmItem::SaveItemTemplateEntryXML( char* pFileName, BOOL bEncryption )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;
	if( !_SaveXmlFile( &m_DocItemTemplateEntry, pFileName, bEncryption ) ) return FALSE;

#ifdef _DEBUG
	char strDebug[ 256 ] = { 0, };
	sprintf_s( strDebug, 256, "Write ItemTemplate Entry File, FileName = %s\n", pFileName );
	OutputDebugString( strDebug );
#endif

	return TRUE;
}

BOOL AgpmItem::_LoadXmlFile( TiXmlDocument* pDoc, char* pFileName, BOOL bIsEncrypt )
{
	if( !pDoc || !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	BOOL bIsResult = FALSE;
	if( bIsEncrypt )
	{
		HANDLE hFile = ::CreateFile( pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile == INVALID_HANDLE_VALUE ) return FALSE;

		// 마지막에 NULL 문자열을 추가해야 하니까 파일사이즈 + 1 해서 초기화한다.
		DWORD dwBufferSize = ::GetFileSize( hFile, NULL ) + 1;
		char* pBuffer = new char[ dwBufferSize ];
		memset( pBuffer, 0, sizeof( char ) * dwBufferSize );

		DWORD dwReadByte = 0;
		if( ::ReadFile( hFile, pBuffer, dwBufferSize, &dwReadByte, NULL ) )
		{
			AuMD5Encrypt Cryptor;
#ifdef _AREA_CHINA_
			if( Cryptor.DecryptString( MD5_HASH_KEY_STRING, pBuffer, dwReadByte ) )
#else
			if( Cryptor.DecryptString( "1111", pBuffer, dwReadByte ) )
#endif
			{
				pDoc->Parse( pBuffer );
				if( !pDoc->Error() )
				{
					bIsResult = TRUE;
				}
			}
		}

		delete[] pBuffer;
		pBuffer = NULL;

		::CloseHandle( hFile );
		hFile = NULL;
	}
	else
	{
		if( pDoc->LoadFile( pFileName ) )
		{
			bIsResult = TRUE;
		}
	}

	return bIsResult;
}

BOOL AgpmItem::_SaveXmlFile( TiXmlDocument* pDoc, char* pFileName, BOOL bIsEncrypt )
{
	if( !pDoc || !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	pDoc->Clear();
	

	TiXmlElement NodeRoot( "ItemTemplateEntry" );
	TiXmlNode* pNodeRoot = pDoc->InsertEndChild( NodeRoot );
	
	std::map< int, stItemTemplateEntry >::iterator Iter;
	Iter = m_mapItemTemplates.begin();

	while( Iter != m_mapItemTemplates.end() )
	{
		stItemTemplateEntry* pEntry = &Iter->second;
		if( !pEntry ) continue;

		TiXmlElement NodeTemplate( "ItemTemplate" );
		TiXmlNode* pNodeTemplate = pNodeRoot->InsertEndChild( NodeTemplate );
		if( pNodeTemplate )
		{
			pNodeTemplate->ToElement()->SetAttribute( "TID", pEntry->m_nTID );
			pNodeTemplate->ToElement()->SetAttribute( "FileName", pEntry->m_strTemplateFileName );
			pNodeTemplate->ToElement()->SetAttribute( "Name", pEntry->m_strTemplateName );
		}

		Iter++;
	}

	if( !pDoc->SaveFile( pFileName ) ) return FALSE;
	return TRUE;
}



