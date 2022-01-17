#include "AgcmItem.h"
#include "AgcCharacterUtil.h"
#include "RtPITexD.h"
#include "ApModuleStream.h"

/******************************************************************************
* Purpose :
*
* 082402. Bob Jung
******************************************************************************/
BOOL AgcmItem::ItemTemplateStreamReadCB(PVOID pData,
										ApModule *pClass,
										ApModuleStream *pcsStream)
{
	//	CHAR					name_buffer[AGCMITEM_MAX_PATH_NAME_BUFFER];
	CHAR					value_buffer[AGCMITEM_MAX_PATH_VALUE_BUFFER];
	//	INT32					process;
	UINT32					ulTemp1, ulTemp2, ulTemp3, ulTemp4, ulRt;
	FLOAT					fTemp1, fTemp2, fTemp3, fTemp4;
	AgcmItem				*pThis						= (AgcmItem *)(pClass);
	AgpdItemTemplate		*pcsAgpdItemTemplate		= (AgpdItemTemplate *)(pData);
	AgcdItemTemplate		*pstAgcdItemTemplate		= pThis->GetTemplateData(pcsAgpdItemTemplate);
	AgpdItemTemplateEquip	*pstAgpdItemTemplateEquip	= (AgpdItemTemplateEquip *)(pcsAgpdItemTemplate);

	//@{ 2006/05/04 burumal
	pstAgcdItemTemplate->m_nDNF = 0;
	//@}

	while(pcsStream->ReadNextValue())
	{
		if (pThis->m_pcsAgcmLODManager)
		{
			ulRt = pThis->m_pcsAgcmLODManager->StreamRead(pcsStream, &pstAgcdItemTemplate->m_stLOD);
			if(ulRt == E_AGCD_LOD_STREAM_READ_RESULT_READ)
				continue;
			else if(ulRt == E_AGCD_LOD_STREAM_READ_RESULT_ERROR)
			{
				ASSERT(!"AgcmCharacter::ItemTemplateStreamReadCB() LOD Stream Read !!!");
				return FALSE;
			}
		}

		if (pThis->m_pcsAgcmRender)
		{
			ulRt = pThis->m_pcsAgcmRender->StreamReadClumpRenderType(pcsStream, &pstAgcdItemTemplate->m_csClumpRenderType);
			if (ulRt == E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_READ)
				continue;
			else if (ulRt == E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_ERROR)
			{
				ASSERT(!"AgcmCharacter::ItemTemplateStreamReadCB() RenderType Stream Read !!!");
				return FALSE;
			}
		}

		if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_BASE_CLUMP))
		{
			//pcsStream->GetValue(pstAgcdItemTemplate->m_szBaseDFFName, AGCDITEM_DFF_NAME_LENGTH);

			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_NAME_BUFFER);

			if (strcmp(value_buffer, ""))
			{
				if (pstAgcdItemTemplate->m_pcsPreData)
				{
					if (pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName)
					{
						ASSERT(!"!pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName");
						return FALSE;
					}

					pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName	= new CHAR [strlen(value_buffer) + 1];
					strcpy(pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName, value_buffer);
				}
				else
				{
					ulTemp1	= atoi(value_buffer);
					//pstAgcdItemTemplate->m_ulExportDffID	|= ((UINT64)(ulTemp1) << 48);
					pstAgcdItemTemplate->SetBaseDFFID(ulTemp1);
				}
			}
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_SECOND_CLUMP))
		{
			//pcsStream->GetValue(pstAgcdItemTemplate->m_szSecondDFFName, AGCDITEM_DFF_NAME_LENGTH);

			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_NAME_BUFFER);

			if (strcmp(value_buffer, ""))
			{
				if (pstAgcdItemTemplate->m_pcsPreData)
				{
					if (pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName)
					{
						ASSERT(!"!pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName");
						return FALSE;
					}

					pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName	= new CHAR [strlen(value_buffer) + 1];
					strcpy(pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName, value_buffer);
				}
				else
				{
					ulTemp1	= atoi(value_buffer);
					//pstAgcdItemTemplate->m_ulExportDffID	|= ((UINT64)(ulTemp1) << 32);
					pstAgcdItemTemplate->SetSecondDFFID(ulTemp1);
				}
			}
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_FIELD_CLUMP))
		{
			//pcsStream->GetValue(pstAgcdItemTemplate->m_szFieldDFFName, AGCDITEM_DFF_NAME_LENGTH);

			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_NAME_BUFFER);

			if (strcmp(value_buffer, ""))
			{
				if (pstAgcdItemTemplate->m_pcsPreData)
				{
					if (pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName)
					{
						ASSERT(!"!pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName");
						return FALSE;
					}

					pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName	= new CHAR [strlen(value_buffer) + 1];
					strcpy(pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName, value_buffer);
				}
				else
				{
					ulTemp1	= atoi(value_buffer);
					//pstAgcdItemTemplate->m_ulExportDffID	|= ((UINT64)(ulTemp1) << 16);
					pstAgcdItemTemplate->SetFieldDFFID(ulTemp1);
				}
			}
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_PICK_CLUMP))
		{
			//pcsStream->GetValue(pstAgcdItemTemplate->m_szPickDFFName, AGCDITEM_DFF_NAME_LENGTH);

			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_NAME_BUFFER);

			if (strcmp(value_buffer, ""))
			{
				if (pstAgcdItemTemplate->m_pcsPreData)
				{
					if (pstAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName)
					{
						ASSERT(!"!pstAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName");
						return FALSE;
					}

					pstAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName	= new CHAR [strlen(value_buffer) + 1];
					strcpy(pstAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName, value_buffer);
				}
				else
				{
					ulTemp1	= atoi(value_buffer);
					//pstAgcdItemTemplate->m_ulExportDffID	|= ((UINT64)(ulTemp1));
					pstAgcdItemTemplate->SetPickDFFID(ulTemp1);
				}
			}
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_TEXTURE))
		{
			//pcsStream->GetValue(pstAgcdItemTemplate->m_szTextureName, AGCDITEM_TEXTURE_NAME_LENGTH);

			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_NAME_BUFFER);

			if (strcmp(value_buffer, ""))
			{
				if (pstAgcdItemTemplate->m_pcsPreData)
				{
					if (pstAgcdItemTemplate->m_pcsPreData->m_pszTextureName)
					{
						ASSERT(!"!pstAgcdItemTemplate->m_pcsPreData->m_pszTextureName");
						return FALSE;
					}

					pstAgcdItemTemplate->m_pcsPreData->m_pszTextureName	= new CHAR [strlen(value_buffer) + 1];
					strcpy(pstAgcdItemTemplate->m_pcsPreData->m_pszTextureName, value_buffer);
				}
				else
				{
					ulTemp1	= atoi(value_buffer);
					//pstAgcdItemTemplate->m_ulExportTextureID	|= ((UINT64)(ulTemp1) << 48);
					pstAgcdItemTemplate->SetTextureID(ulTemp1);
				}
			}
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_SMALL_TEXTURE))
		{
			//pcsStream->GetValue(pstAgcdItemTemplate->m_szSmallTextureName, AGCDITEM_TEXTURE_NAME_LENGTH);

			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_NAME_BUFFER);

			if (strcmp(value_buffer, ""))
			{
				if (pstAgcdItemTemplate->m_pcsPreData)
				{
					if (pstAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName)
					{
						ASSERT(!"!pstAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName");
						return FALSE;
					}

					pstAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName	= new CHAR [strlen(value_buffer) + 1];
					strcpy(pstAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName, value_buffer);
				}
				else
				{
					ulTemp1	= atoi(value_buffer);
					//pstAgcdItemTemplate->m_ulExportTextureID	|= ((UINT64)(ulTemp1) << 32);
					pstAgcdItemTemplate->SetSmallTextureID(ulTemp1);
				}
			}
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_DURABILITY_ZERO_TEXTURE))
		{
			//pcsStream->GetValue(pstAgcdItemTemplate->m_szDurabilityZeroTextureName, AGCDITEM_TEXTURE_NAME_LENGTH);

			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_NAME_BUFFER);

			if (strcmp(value_buffer, ""))
			{
				if (pstAgcdItemTemplate->m_pcsPreData)
				{
					if (pstAgcdItemTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName)
					{
						ASSERT(!"!pstAgcdItemTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName");
						return FALSE;
					}

					pstAgcdItemTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName	= new CHAR [strlen(value_buffer) + 1];
					strcpy(pstAgcdItemTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName, value_buffer);
				}
				else
				{
					ulTemp1	= atoi(value_buffer);
					//pstAgcdItemTemplate->m_ulExportTextureID	|= ((UINT64)(ulTemp1) << 16);
					pstAgcdItemTemplate->SetDurZeroTextureID(ulTemp1);
				}
			}
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_DURABILITY_5UNDER_TEXTURE))
		{
			//pcsStream->GetValue(pstAgcdItemTemplate->m_szDurability5UnderTextureName, AGCDITEM_TEXTURE_NAME_LENGTH);

			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_NAME_BUFFER);

			if (strcmp(value_buffer, ""))
			{
				if (pstAgcdItemTemplate->m_pcsPreData)
				{
					if (pstAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName)
					{
						ASSERT(!"!pstAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName");
						return FALSE;
					}

					pstAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName	= new CHAR [strlen(value_buffer) + 1];
					strcpy(pstAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName, value_buffer);
				}
				else
				{
					ulTemp1	= atoi(value_buffer);
					//pstAgcdItemTemplate->m_ulExportTextureID	|= ((UINT64)(ulTemp1));
					pstAgcdItemTemplate->SetDur5UnderTextureID(ulTemp1);
				}
			}
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_ATOMIC_COUNT))
		{
			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_VALUE_BUFFER);
			sscanf(value_buffer, "%d", &pstAgcdItemTemplate->m_lAtomicCount);
		}
		else if(!strncmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_TRANSFORM, strlen(AGCMITEM_INI_NAME_TRANSFORM)))
		{
			if( !pstAgcdItemTemplate->m_pItemTransformInfo->Read( pcsStream, pstAgcdItemTemplate ) ) return FALSE;
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_FIELD_TRANSFORM))
		{
			if(!pThis->ParseValueFieldTransform(pstAgcdItemTemplate, pcsStream)) return FALSE;
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_EQUIP_TWOHANDS))
		{
			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_VALUE_BUFFER);

			sscanf(value_buffer, "%d", &pstAgcdItemTemplate->m_bEquipTwoHands);
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_WITH_HAIR))
		{
			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_VALUE_BUFFER);

			sscanf(value_buffer, "%d", &pstAgcdItemTemplate->m_bWithHair);
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_WITH_FACE))
		{
			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_VALUE_BUFFER);

			sscanf(value_buffer, "%d", &pstAgcdItemTemplate->m_bWithFace);
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_OBJECT_TYPE))
		{
			pcsStream->GetValue(&pstAgcdItemTemplate->m_lObjectType);
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_PRE_LIGHT))
		{
			if (pstAgcdItemTemplate->m_pcsPreData)
			{
				pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_VALUE_BUFFER);

				sscanf(value_buffer, "%d %d %d %d", &ulTemp1, &ulTemp2, &ulTemp3, &ulTemp4);

				pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.red		= ulTemp1;
				pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.green	= ulTemp2;
				pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.blue	= ulTemp3;
				pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.alpha	= ulTemp4;
			}
		}
		else if(!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_BOUNDING_SPHERE))
		{
			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_VALUE_BUFFER);

			sscanf(value_buffer, "%f:%f:%f:%f", &fTemp1, &fTemp2, &fTemp3, &fTemp4);

			pstAgcdItemTemplate->m_stBSphere.center.x	= fTemp1;
			pstAgcdItemTemplate->m_stBSphere.center.y	= fTemp2;
			pstAgcdItemTemplate->m_stBSphere.center.z	= fTemp3;
			pstAgcdItemTemplate->m_stBSphere.radius		= fTemp4;
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_OCTREE_DATA))		// add by gemani
		{
			pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_VALUE_BUFFER);

			FLOAT	afTemp[12];
			sscanf(value_buffer,
				"%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
				afTemp + 0,
				afTemp + 1,
				afTemp + 2,
				afTemp + 3,
				afTemp + 4,
				afTemp + 5,
				afTemp + 6,
				afTemp + 7,
				afTemp + 8,
				afTemp + 9,
				afTemp + 10,
				afTemp + 11);

			// 위에 임시로 set. 아직 데이터가 set되있지 않다..
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].x	= afTemp[0];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].y	= afTemp[1];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].z	= afTemp[2];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].x	= afTemp[3];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].y	= afTemp[4];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].z	= afTemp[5];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].x	= afTemp[6];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].y	= afTemp[7];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].z	= afTemp[8];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].x	= afTemp[9];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].y	= afTemp[10];
			//pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].z	= afTemp[11];
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_LABEL))
		{
			if (pstAgcdItemTemplate->m_pcsPreData)
			{
				pcsStream->GetValue(value_buffer, AGCMITEM_MAX_PATH_VALUE_BUFFER);

				if (strcmp(value_buffer, ""))
				{
					if (pstAgcdItemTemplate->m_pcsPreData->m_pszLabel)
						return FALSE;;

					pstAgcdItemTemplate->m_pcsPreData->m_pszLabel	= new CHAR[strlen(value_buffer) + 1];

					strcpy(pstAgcdItemTemplate->m_pcsPreData->m_pszLabel, value_buffer);
				}
			}
		}
		//@{ 2006/05/03 burumal
		else if ( !strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_DNF_1) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 1;

			if ( nDNF > 0 )
				pstAgcdItemTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_DNF_2) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 2;

			if ( nDNF > 0 )
				pstAgcdItemTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_DNF_3) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 3;

			if ( nDNF > 0 )
				pstAgcdItemTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_DNF_4) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 4;

			if ( nDNF > 0 )
				pstAgcdItemTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		//@}
	}

	pThis->EnumCallback(AGCMITEM_CB_ID_READ_TEMPLATE, (PVOID)pcsAgpdItemTemplate, (PVOID)pstAgcdItemTemplate);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 082402. Bob Jung
******************************************************************************/
BOOL AgcmItem::ItemTemplateStreamWriteCB(PVOID pData,
										 ApModule *pClass,
										 ApModuleStream *pcsStream)
{
	INT32					lIndex					= 0;
	char					value_buffer[AGCMITEM_MAX_PATH_VALUE_BUFFER];
	AgcmItem				*pThis					= (AgcmItem *)(pClass);
	AgpdItemTemplate		*pcsAgpdItemTemplate	= (AgpdItemTemplate *)(pData);
	AgcdItemTemplate		*pstAgcdItemTemplate	= pThis->GetTemplateData(pcsAgpdItemTemplate);

	if (pstAgcdItemTemplate->m_pcsPreData)
	{
		if (pstAgcdItemTemplate->m_pcsPreData->m_pszLabel)
			pcsStream->WriteValue(AGCMITEM_INI_NAME_LABEL, pstAgcdItemTemplate->m_pcsPreData->m_pszLabel);

		if (pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName)
			pcsStream->WriteValue(AGCMITEM_INI_NAME_BASE_CLUMP, pstAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName);
		if (pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName)
			pcsStream->WriteValue(AGCMITEM_INI_NAME_SECOND_CLUMP, pstAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName);
		if (pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName)
			pcsStream->WriteValue(AGCMITEM_INI_NAME_FIELD_CLUMP, pstAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName);
		if (pstAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName)
			pcsStream->WriteValue(AGCMITEM_INI_NAME_PICK_CLUMP, pstAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName);

		if (pstAgcdItemTemplate->m_pcsPreData->m_pszTextureName)
			pcsStream->WriteValue(AGCMITEM_INI_NAME_TEXTURE, pstAgcdItemTemplate->m_pcsPreData->m_pszTextureName);
		if (pstAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName)
			pcsStream->WriteValue(AGCMITEM_INI_NAME_SMALL_TEXTURE, pstAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName);
		if (pstAgcdItemTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName)
			pcsStream->WriteValue(AGCMITEM_INI_NAME_DURABILITY_ZERO_TEXTURE, pstAgcdItemTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName);
		if (pstAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName)
			pcsStream->WriteValue(AGCMITEM_INI_NAME_DURABILITY_5UNDER_TEXTURE, pstAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName);

		if (pstAgcdItemTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)
		{
			sprintf( value_buffer, "%d %d %d %d",
					pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.red,
					pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.green,
					pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.blue,
					pstAgcdItemTemplate->m_pcsPreData->m_stPreLight.alpha );

			pcsStream->WriteValue(AGCMITEM_INI_NAME_PRE_LIGHT, value_buffer);
		}
	}
	else
	{
		lIndex	= (INT32)(pstAgcdItemTemplate->GetBaseDFFID());
		if (lIndex > 0)		pcsStream->WriteValue(AGCMITEM_INI_NAME_BASE_CLUMP, lIndex);
		lIndex	= (INT32)(pstAgcdItemTemplate->GetSecondDFFID());
		if (lIndex > 0)		pcsStream->WriteValue(AGCMITEM_INI_NAME_SECOND_CLUMP, lIndex);
		lIndex	= (INT32)(pstAgcdItemTemplate->GetFieldDFFID());
		if (lIndex > 0)		pcsStream->WriteValue(AGCMITEM_INI_NAME_FIELD_CLUMP, lIndex);
		lIndex	= (INT32)(pstAgcdItemTemplate->GetPickDFFID());
		if (lIndex > 0)		pcsStream->WriteValue(AGCMITEM_INI_NAME_PICK_CLUMP, lIndex);

		lIndex	= (INT32)(pstAgcdItemTemplate->GetTextureID());
		if (lIndex > 0)		pcsStream->WriteValue(AGCMITEM_INI_NAME_TEXTURE, lIndex);
		lIndex	= (INT32)(pstAgcdItemTemplate->GetSmallTextureID());
		if (lIndex > 0)		pcsStream->WriteValue(AGCMITEM_INI_NAME_SMALL_TEXTURE, lIndex);
		lIndex	= (INT32)(pstAgcdItemTemplate->GetDurZeroTextureID());
		if (lIndex > 0)		pcsStream->WriteValue(AGCMITEM_INI_NAME_DURABILITY_ZERO_TEXTURE, lIndex);
		lIndex	= (INT32)(pstAgcdItemTemplate->GetDur5UnderTextureID());
		if (lIndex > 0)		pcsStream->WriteValue(AGCMITEM_INI_NAME_DURABILITY_5UNDER_TEXTURE, lIndex);
			
	}

	// Atomic 갯수
	sprintf(value_buffer, "%d", pstAgcdItemTemplate->m_lAtomicCount);
	pcsStream->WriteValue(AGCMITEM_INI_NAME_ATOMIC_COUNT, value_buffer);

	//////////////////////////////////
	// Transform 정보
	pstAgcdItemTemplate->m_pItemTransformInfo->Write( pcsStream );
	//////////////////////////////////

	//////////////////////////////////
	// Field transform 정보
	if(pstAgcdItemTemplate->m_pstFieldClumpTransform)
	{
		print_compact_format(value_buffer, "%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",	pstAgcdItemTemplate->m_pstFieldClumpTransform->right.x,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->right.y,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->right.z,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->up.x,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->up.y,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->up.z,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->at.x,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->at.y,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->at.z,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->pos.x,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->pos.y,
			pstAgcdItemTemplate->m_pstFieldClumpTransform->pos.z		);

		pcsStream->WriteValue(AGCMITEM_INI_NAME_FIELD_TRANSFORM, value_buffer);
	}
	//////////////////////////////////

	//////////////////////////////////
	// LOD	
	if (pThis->m_pcsAgcmLODManager)
		pThis->m_pcsAgcmLODManager->StreamWrite(pcsStream, &pstAgcdItemTemplate->m_stLOD);
	//////////////////////////////////

	// render type
	if (pThis->m_pcsAgcmRender)
		pThis->m_pcsAgcmRender->StreamWriteClumpRenderType(pcsStream, &pstAgcdItemTemplate->m_csClumpRenderType);

	// object type
	if(!pcsStream->WriteValue(AGCMITEM_INI_NAME_OBJECT_TYPE, pstAgcdItemTemplate->m_lObjectType))
	{
		return FALSE;
	}

	// 두 손 장착인가?
	sprintf(value_buffer, "%d", pstAgcdItemTemplate->m_bEquipTwoHands);
	pcsStream->WriteValue(AGCMITEM_INI_NAME_EQUIP_TWOHANDS, value_buffer);

	// 투구장착시 머리를 날리는가?
	if( pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP ) {
		AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip = (AgpdItemTemplateEquip *)(pcsAgpdItemTemplate);
		if( pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_HEAD ) {
			sprintf(value_buffer, "%d", pstAgcdItemTemplate->m_bWithHair);
			pcsStream->WriteValue(AGCMITEM_INI_NAME_WITH_HAIR, value_buffer);
		}
	}

	// 투구장착시 얼굴을 날리는가?
	if( pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP ) {
		AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip = (AgpdItemTemplateEquip *)(pcsAgpdItemTemplate);
		if( pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_HEAD ) {
			sprintf(value_buffer, "%d", pstAgcdItemTemplate->m_bWithFace);
			pcsStream->WriteValue(AGCMITEM_INI_NAME_WITH_FACE, value_buffer);
		}
	}

	// Bounding sphere
	print_compact_format(
		value_buffer,
		"%f:%f:%f:%f",
		pstAgcdItemTemplate->m_stBSphere.center.x,
		pstAgcdItemTemplate->m_stBSphere.center.y,
		pstAgcdItemTemplate->m_stBSphere.center.z,
		pstAgcdItemTemplate->m_stBSphere.radius		);
	pcsStream->WriteValue(AGCMITEM_INI_NAME_BOUNDING_SPHERE, value_buffer);	

	// octree data
	{
		print_compact_format(value_buffer,
			"%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].x,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].y,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[0].z,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].x,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].y,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[1].z,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].x,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].y,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[2].z,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].x,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].y,
			pstAgcdItemTemplate->m_stOcTreeData.topVerts_MAX[3].z	);

		if (!pcsStream->WriteValue(AGCMITEM_INI_NAME_OCTREE_DATA, value_buffer))
			return FALSE;
	}

	//@{ 2006/05/03 burumal		
	pcsStream->WriteValue(AGCMITEM_INI_NAME_DNF_1, (pstAgcdItemTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_KOREA		)) ? 1 : 0);
	pcsStream->WriteValue(AGCMITEM_INI_NAME_DNF_2, (pstAgcdItemTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_CHINA		)) ? 1 : 0);
	pcsStream->WriteValue(AGCMITEM_INI_NAME_DNF_3, (pstAgcdItemTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_WESTERN	)) ? 1 : 0);
	pcsStream->WriteValue(AGCMITEM_INI_NAME_DNF_4, (pstAgcdItemTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_JAPAN		)) ? 1 : 0);
	//@}

	return TRUE;
}
