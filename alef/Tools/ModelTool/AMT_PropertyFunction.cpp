#include "stdafx.h"
#include "ModelTool.h"
#include "CreateObjectDlg.h"
#include "DefaultEquipmentsDlg.h"
#include "EditDlg.h"
#include "AcuCreateClump.h"
#include "EditFaceDlg.h"
#include "EditHairDlg.h"
#include "EditRideDlg.h"
#include "AttachFaceDlg.h"
#include "AttachHairDlg.h"
#include "DefaultHeadRenderTypeDlg.h"
#include "AgcmObject.h"

eTemplateKind GetTemplateKind( eAmtObjectType eType )
{
	switch( eType )
	{
	case AMT_OBJECT_TYPE_CHARACTER:	return eTemplateCharacter;
	case AMT_OBJECT_TYPE_ITEM:		return eTemplateItem;
	case AMT_OBJECT_TYPE_OBJECT:	return eTemplateObject;
	case AMT_OBJECT_TYPE_SKILL:		return eTemplateSkill;
	default:						return eTemplateMax;
	}
}

BOOL CModelToolApp::F_CreateObject()
{
	if (m_eObjectType != AMT_OBJECT_TYPE_STATIC_MODEL)
		return FALSE;

	CCreateObjectDlg dlg;
	dlg.DoModal();

	return TRUE;
}

BOOL CModelToolApp::F_SetAnimation()
{
	if( m_eObjectType != AMT_OBJECT_TYPE_CHARACTER )
		return FALSE;

	AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetCCharacter( m_lID );
	if( !pcsAgcdCharacter )									return FALSE;
	if( !pcsAgcdCharacter->m_pstAgcdCharacterTemplate )		return FALSE;

	if( m_csAgcEngine.GetAgcmAnimationDlgModule()->OpenCharAnim( pcsAgcdCharacter, pcsAgcdCharacter->m_pstAgcdCharacterTemplate ) )
	{
		SetSaveDataCharacter();
		return TRUE;
	}
	else return FALSE;
}

BOOL CModelToolApp::F_SetDefaultEquipments()
{
	if( m_eObjectType != AMT_OBJECT_TYPE_CHARACTER )
		return FALSE;

	AgpdCharacter			*pcsAgpdCharacter			= m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
	if((!pcsAgpdCharacter) && (!pcsAgpdCharacter->m_pcsCharacterTemplate))
		return FALSE;

	AgpdItemADCharTemplate	*pcsAgpdItemADCharTemplate	= m_csAgcEngine.GetAgpmItemModule()->GetADCharacterTemplate(pcsAgpdCharacter->m_pcsCharacterTemplate);
	if(!pcsAgpdItemADCharTemplate)
		return FALSE;

	CDefaultEquipmentsDlg dlg( pcsAgpdItemADCharTemplate );
	if( dlg.DoModal() == IDOK )
	{
		SetSaveDataCharacter();
		ApplyObject();
	}

	return TRUE;
}

BOOL CModelToolApp::F_SetFactors()
{
	eTemplateKind	eKind = GetTemplateKind( m_eObjectType );
	AgpdFactor* pcsFactor = m_csAgcEngine.GetFactor( eKind, m_lTID );
	return m_csAgcEngine.GetAgcmFactorsDlgModule()->OpenFactorsDlg( pcsFactor );
}

BOOL CModelToolApp::F_SetLOD()
{
	if( m_bExport )		return FALSE;
		
	AgcmLODManager* pLodMgr = m_csAgcEngine.GetAgcmLODManagerModule();

	INT32				lTemp;
	AgcdDefaultLODInfo	stInfo;
	AgcdPreLOD			*pstAgcdPreLOD;
	AgcdLOD				*pstAgcdLOD;

	switch(m_eObjectType)
	{
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacterTemplate* ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacterTemplate(m_lTID);
			if( !ppTemplate )				return FALSE;

			AgcdCharacterTemplate* pcTemplate = (AgcdCharacterTemplate *)(m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(ppTemplate));
			if( !pcTemplate )				return FALSE;
			if( !pcTemplate->m_pcsPreData )	return FALSE;
				
			pstAgcdPreLOD = m_csAgcEngine.GetAgcmPreLODManagerModule()->GetCharacterPreLOD(ppTemplate);
			if( !pstAgcdPreLOD )			return FALSE;

			pstAgcdLOD = &pcTemplate->m_stLOD;
			if( pstAgcdLOD->m_lNum > 1 )	return FALSE;
			
			stInfo.m_paszDFFInfo[0] = pcTemplate->m_pcsPreData->m_pszDADFFName ? pcTemplate->m_pcsPreData->m_pszDADFFName : pcTemplate->m_pcsPreData->m_pszDFFName;

			if( pstAgcdLOD->m_lNum == 0 )
			{
				m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.AddLODData( pstAgcdLOD );
				if( pstAgcdLOD->m_lNum != 1 || !pstAgcdLOD->m_pstList )
					return FALSE;
			}
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItemTemplate *ppTemplate			= m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(m_lTID);
			if(!ppTemplate)
				return FALSE;

			AgcdItemTemplate *pcTemplate			= m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(ppTemplate);
			if(!pcTemplate)
				return FALSE;

			if (!pcTemplate->m_pcsPreData)
				return FALSE;

			pstAgcdPreLOD							= m_csAgcEngine.GetAgcmPreLODManagerModule()->GetItemPreLOD(ppTemplate);
			if(!pstAgcdPreLOD)
				return FALSE;

			pstAgcdLOD								= &pcTemplate->m_stLOD;
			stInfo.m_paszDFFInfo[0]					= pcTemplate->m_pcsPreData->m_pszBaseDFFName;
			lTemp									= 1;

			if (	(pcTemplate->m_pstSecondClump) &&
					(pcTemplate->m_pcsPreData->m_pszSecondDFFName)	)
			{
				stInfo.m_paszDFFInfo[1]				= pcTemplate->m_pcsPreData->m_pszSecondDFFName;
				lTemp								= 2;
			}

			if(lTemp > pstAgcdLOD->m_lNum)
			{
				for(INT32 lCount = 0; lCount < lTemp - pstAgcdLOD->m_lNum; ++lCount)
				{
					if(!m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.AddLODData(pstAgcdLOD))
						return FALSE;
				}
			}
			else if(lTemp < pstAgcdLOD->m_lNum)
			{
				for(INT32 lCount = 0; lCount < pstAgcdLOD->m_lNum - lTemp; ++lCount)
				{
					if(!m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.RemoveLODData(pstAgcdLOD, pstAgcdLOD->m_lNum - 1))
						return FALSE;
				}
			}
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObjectTemplate *ppTemplate			= m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate(m_lTID);
			if(!ppTemplate)
				return FALSE;

			AgcdObjectTemplate *pcTemplate			= m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(ppTemplate);
			if(!pcTemplate)
				return FALSE;

			pstAgcdPreLOD							= m_csAgcEngine.GetAgcmPreLODManagerModule()->GetObjectPreLOD(ppTemplate);
			if(!pstAgcdPreLOD)
				return FALSE;

			pstAgcdLOD								= &pcTemplate->m_stLOD;

			// 기본 DFF를 설정한다.
			AgcdObjectTemplateGroupList	*pstList	= pcTemplate->m_stGroup.m_pstList;
			while(pstList)
			{
				//stInfo.m_paszDFFInfo[pstList->m_csData.m_lIndex] = pstList->m_csData.m_szDFFName;
				stInfo.m_paszDFFInfo[pstList->m_csData.m_lIndex] = pstList->m_csData.m_pszDFFName;
				pstList = pstList->m_pstNext;
			}

			lTemp = pstAgcdLOD->m_lNum;

			// Group 갯수와 LOD 갯수를 맞춘다.
			if(pcTemplate->m_stGroup.m_lNum > pstAgcdLOD->m_lNum)
			{
				for( INT32 lCount = 0; lCount < pcTemplate->m_stGroup.m_lNum - lTemp; ++lCount)
				{
					if(!m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.AddLODData(pstAgcdLOD))
						return FALSE;
				}
			}
			else if(pcTemplate->m_stGroup.m_lNum < pstAgcdLOD->m_lNum)
			{
				for( INT32 lCount = 0; lCount < pstAgcdLOD->m_lNum - lTemp; ++lCount)
				{
					if(!m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.RemoveLODData(pstAgcdLOD, pstAgcdLOD->m_lNum - 1))
						return FALSE;
				}
			}
		}
		break;

	default:
			return FALSE;
	}	

	char* szPath = ( m_eObjectType == AMT_OBJECT_TYPE_OBJECT ? AMT_OBJECT_DATA_PATH : AMT_CHARACTER_DATA_PATH );
	if( m_csAgcEngine.GetAgcmLODDlgModule()->OpenLODDlg( pstAgcdLOD, pstAgcdPreLOD, szPath, &stInfo ) )
	{
		SetSaveData();
		return TRUE;
	}
	else return FALSE;
}

BOOL CModelToolApp::F_SetGeometry(CHAR *szGeometryKeyName)
{
	if( m_csAgcEngine.GetAgcmBlockingDlgModule()->IsOpenEditBlocking() )
		return TRUE;

	ApBase* pcsBase	= m_csAgcEngine.GetTemplate( GetTemplateKind( m_eObjectType ), m_lTID );
	if( !pcsBase )		return FALSE;

	for( INT32 lCount = 0; lCount < AMT_MAX_GEOMETRY_DATA_ARRAY; ++lCount )
		m_astBlocking[lCount].type = AUBLOCKING_TYPE_NONE;

	m_nNumBlocking						= 0;
	AgcdGeometryDataLink	*pcsLink	= m_csAgcEngine.GetAgcmGeometryDataManagerModule()->GetGeometryDataLink(pcsBase, szGeometryKeyName);
	AgcdGeometryData		*pcsNext	= pcsLink->m_pcsHead;
	while (pcsNext)
	{
		memcpy(m_astBlocking + m_nNumBlocking, &pcsNext->m_stGeom, sizeof(AuBLOCKING));

		++m_nNumBlocking;
		pcsNext							= pcsNext->m_pcsNext;
	}

	{
		if( m_csAgcEngine.GetAgcmBlockingDlgModule()->Open( m_astBlocking, &m_nNumBlocking, AMT_MAX_GEOMETRY_DATA_ARRAY, (PVOID)(pcsLink) ) == IDOK )
		{
			SetSaveData();
			return TRUE;
		}
		else return FALSE;
	}
}

BOOL CModelToolApp::F_SetEffect()
{
	eTemplateKind eKind = GetTemplateKind( m_eObjectType );
	AgcdUseEffectSet* pstAgcdUseEffectSet = m_csAgcEngine.GetUseEffect( eKind, m_lTID );
	if( !pstAgcdUseEffectSet )	return FALSE;

	m_csAgcEngine.GetAgcmEffectDlgModule()->OpenUseEffectSet( pstAgcdUseEffectSet );
	SetSaveData();
	return TRUE;
}

BOOL CModelToolApp::F_ReloadEffect()
{
	m_csAgcEngine.GetAgcmEff2()->bForTool_ReloadAllEffect();
	return TRUE;
}

BOOL CModelToolApp::F_SetObjectType()
{
	INT32	*pplObjectType, *pclObjectType;
	RwRGBA	*pstRGBA;

	switch( m_eObjectType )
	{
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacterTemplate *ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacterTemplate(m_lTID);
			if(!ppTemplate)
				return FALSE;

			AgcdCharacterTemplate *pcTemplate = m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(ppTemplate);
			if(!pcTemplate)
				return FALSE;

			if (!pcTemplate->m_pcsPreData)
				return FALSE;

			pclObjectType	= &pcTemplate->m_lObjectType;
			pstRGBA			= &pcTemplate->m_pcsPreData->m_stPreLight;

			if(m_csAgcEngine.GetAgcmObjectTypeDlg()->OpenObjectType(NULL, pclObjectType, pstRGBA))
			{
				SetSaveDataCharacter();
				ApplyObject();
			}
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItemTemplate *ppTemplate = m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(m_lTID);
			if(!ppTemplate)
				return FALSE;

			AgcdItemTemplate *pcTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(ppTemplate);
			if(!pcTemplate)
				return FALSE;

			if (!pcTemplate->m_pcsPreData)
				return FALSE;

			pclObjectType	= &pcTemplate->m_lObjectType;
			pstRGBA			= &pcTemplate->m_pcsPreData->m_stPreLight;

			if(m_csAgcEngine.GetAgcmObjectTypeDlg()->OpenObjectType(NULL, pclObjectType, pstRGBA))
			{
				SetSaveDataItem();
				ApplyObject();
			}
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObjectTemplate *ppTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate(m_lTID);
			if(!ppTemplate)
				return FALSE;

			AgcdObjectTemplate *pcTemplate = m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(ppTemplate);
			if(!pcTemplate)
				return FALSE;

			pplObjectType	= &ppTemplate->m_nObjectType;
			pclObjectType	= &pcTemplate->m_lObjectType;
			pstRGBA			= &pcTemplate->m_stPreLight;

			if(m_csAgcEngine.GetAgcmObjectTypeDlg()->OpenObjectType(pplObjectType, pclObjectType, pstRGBA))
			{
				SetSaveDataObject();
				ApplyObject();
			}
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CModelToolApp::F_SetDefaultProperty()
{
	switch(m_eObjectType)
	{
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacterTemplate *ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacterTemplate(m_lTID);
			if(!ppTemplate)
				return FALSE;

			AgcdCharacterTemplate *pcTemplate = m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(ppTemplate);
			if(!pcTemplate)
				return FALSE;

			if(!m_csAgcEngine.GetAgcmCharacterDlgModule()->OpenCharacterDlg(ppTemplate, pcTemplate))
				return TRUE;

			if( !UpdateCharacterTemplate(ppTemplate, pcTemplate ) )
				return FALSE;

			ApplyObject();
			SetSaveDataCharacter();
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItemTemplate *ppTemplate = m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(m_lTID);
			if(!ppTemplate)
				return FALSE;

			AgcdItemTemplate *pcTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(ppTemplate);
			if(!pcTemplate)
				return FALSE;


			INT32	lAtomicCount = pcTemplate->m_lAtomicCount;

			//@{ 2006/08/01 burumal
			//if(!m_csAgcEngine.GetAgcmItemDlgModule()->OpenItemDlg(ppTemplate, pcTemplate))
			if(!m_csAgcEngine.GetAgcmItemDlgModule()->OpenItemDlg(ppTemplate, pcTemplate, true))
			//if(!m_csAgcEngine.GetAgcmItemDlgModule()->OpenItemDlg(ppTemplate, pcTemplate, false))
			//@}
			{
//				free(ppPreTemplate);
//				free(pcPreTemplate);
				SetSaveDataItem();
				return TRUE; //skip
			}

			if(ppTemplate->m_nType == AGPMITEM_TYPE_EQUIP)
			{
				AgpdItemTemplateEquip		*pcsTemplateEquip		= (AgpdItemTemplateEquip *)(ppTemplate);

				if(pcsTemplateEquip->m_nKind != AGPMITEM_EQUIP_KIND_ARMOUR)
				{
					pcTemplate->m_lAtomicCount = RpClumpGetNumAtomics(m_csAgcEngine.GetAgcmItemModule()->GetClumpAdminData(pcTemplate->m_pcsPreData->m_pszBaseDFFName));
					m_csAgcEngine.GetAgcmItemModule()->RemoveClumpAdminData(pcTemplate->m_pcsPreData->m_pszBaseDFFName); // 아이템 클럼프 관리상 지워줘야 한다. (툴종료때 모두 지워서 상관없지만...)

					if(pcsTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
					{
						if(pcTemplate->m_bEquipTwoHands)
							pcTemplate->m_lAtomicCount += pcTemplate->m_lAtomicCount;

						m_csAgcEngine.GetAgpmFactorsModule()->SetValue(&ppTemplate->m_csFactor, pcTemplate->m_bEquipTwoHands ? 2 : 1, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);
					}

					if (lAtomicCount != pcTemplate->m_lAtomicCount)
					{
						pcTemplate->m_pItemTransformInfo->Clear();
						//m_csAgcEngine.GetAgcmItemModule()->RemoveAllTransformInfo(pcTemplate);
					}
				}
			}

			if(!UpdateItemTemplate(ppTemplate, pcTemplate/*, (AgpdItemTemplate *)(ppPreTemplate), (AgcdItemTemplate *)(pcPreTemplate)*/))
			{
//				free(ppPreTemplate);
//				free(pcPreTemplate);
				return FALSE;
			}

//			free(ppPreTemplate);
//			free(pcPreTemplate);

			ApplyObject();
			SetSaveDataItem();
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObjectTemplate *ppTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate(m_lTID);
			if(!ppTemplate)
				return FALSE;

			AgcdObjectTemplate *pcTemplate = m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(ppTemplate);
			if(!pcTemplate)
				return FALSE;

//			PVOID	ppPreTemplate, pcPreTemplate;
//			ppPreTemplate = malloc(sizeof(ApdObjectTemplate));
//			pcPreTemplate = malloc(sizeof(AgcdObjectTemplate));
//			memcpy(ppPreTemplate, ppTemplate, sizeof(ApdObjectTemplate));
//			memcpy(pcPreTemplate, pcTemplate, sizeof(AgcdObjectTemplate));

//			if(!m_csAgcmObject.GetObjectList()->CopyObjectTemplateGroup(&((AgcdObjectTemplate *)(pcPreTemplate))->m_stGroup, &pcTemplate->m_stGroup))
//			{
//				free(ppPreTemplate);
//				free(pcPreTemplate);
//				return FALSE;
//			}

			if(!m_csAgcEngine.GetAgcmObjectDlgModule()->OpenObjectDlg(ppTemplate, pcTemplate))
			{
//				m_csAgcmObject.GetObjectList()->RemoveAllObjectTemplateGroup(&((AgcdObjectTemplate *)(pcPreTemplate))->m_stGroup);
//				free(ppPreTemplate);
//				free(pcPreTemplate);
				return TRUE; // skip
			}

			if(!UpdateObjectTemplate(ppTemplate, pcTemplate/*, (ApdObjectTemplate *)(ppPreTemplate), (AgcdObjectTemplate *)(pcPreTemplate)*/))
			{
//				m_csAgcmObject.GetObjectList()->RemoveAllObjectTemplateGroup(&((AgcdObjectTemplate *)(pcPreTemplate))->m_stGroup);
//				free(ppPreTemplate);
//				free(pcPreTemplate);
				return FALSE;
			}

//			m_csAgcmObject.GetObjectList()->RemoveAllObjectTemplateGroup(&((AgcdObjectTemplate *)(pcPreTemplate))->m_stGroup);
//			free(ppPreTemplate);
//			free(pcPreTemplate);

			ApplyObject();
			SetSaveDataObject();
		}
		break;

	case AMT_OBJECT_TYPE_SKILL:
		{
			AgpdSkillTemplate	*ppTemplate = m_csAgcEngine.GetAgpmSkillModule()->GetSkillTemplate(m_lTID);
			if(!ppTemplate)
				return FALSE;

			AgcdSkillTemplate	*pcTemplate = m_csAgcEngine.GetAgcmSkillModule()->GetADSkillTemplate((PVOID)(ppTemplate));
			if(!pcTemplate)
				return FALSE;

			CHAR				szTemp[AMT_MAX_STR];
			strcpy(szTemp, ppTemplate->m_szName);

			m_csAgcEngine.GetAgcmSkillDlgModule()->OpenSkillTemplateDlg(ppTemplate, pcTemplate);

			if(strcmp(szTemp, ppTemplate->m_szName))
			{
				CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeSkill()->SetItemText(m_hCurrentTreeItem, ppTemplate->m_szName);
			}

			ApplyObject();
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CModelToolApp::F_EditEquipments()
{
	if( m_eObjectType != AMT_OBJECT_TYPE_CHARACTER )
		return FALSE;

	CModelToolDlg::GetInstance()->OpenEditEquipmentsDlg();
	return TRUE;
}

BOOL CModelToolApp::F_SetSkill()
{
	if( m_eObjectType != AMT_OBJECT_TYPE_CHARACTER )
		return FALSE;

	AgpdCharacter				*pcsAgpdCharacter					= m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
	if((!pcsAgpdCharacter) || (!pcsAgpdCharacter->m_pcsCharacterTemplate))
		return FALSE;

	ApBase						*pcsAgpdCharacterTemplateBase		= (ApBase *)(pcsAgpdCharacter->m_pcsCharacterTemplate);
	AgcdCharacterTemplate		*pcsAgcdCharacterTemplate			= m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(pcsAgpdCharacter->m_pcsCharacterTemplate);

	AgpdSkillTemplateAttachData	*pcsAgpdAttachSkillTemplateData		= m_csAgcEngine.GetAgpmSkillModule()->GetAttachSkillTemplateData(pcsAgpdCharacterTemplateBase);
	AgcdSkillAttachTemplateData	*pstAgcdSkillAttachTemplateData		= m_csAgcEngine.GetAgcmSkillModule()->GetAttachTemplateData(pcsAgpdCharacterTemplateBase);
	if((!pcsAgpdAttachSkillTemplateData) || (!pstAgcdSkillAttachTemplateData))
		return FALSE;

	if(!m_csAgcEngine.GetAgcmSkillDlgModule()->OpenCharacterSkillDlg(
		pcsAgcdCharacterTemplate,
		pcsAgpdAttachSkillTemplateData,
		pstAgcdSkillAttachTemplateData,
		AMT_CHARACTER_ANIMATION_DATA_FIND_PATH1,
		AMT_CHARACTER_ANIMATION_DATA_FIND_PATH2							))
		return FALSE;

	SetSaveDataCharacter();
	return TRUE;
}

BOOL CModelToolApp::F_SetEventInitialize()
{
	switch ( m_eObjectType )
	{
	case AMT_OBJECT_TYPE_CHARACTER:		return SetCharacter( m_lTID, m_hCurrentTreeItem, FALSE ) ? TRUE: FALSE;
	case AMT_OBJECT_TYPE_ITEM:			return SetItem( m_lTID, m_hCurrentTreeItem, FALSE ) ? TRUE: FALSE;
	case AMT_OBJECT_TYPE_OBJECT:		return SetObject( m_lTID, m_hCurrentTreeItem, FALSE ) ? TRUE: FALSE;
	default:							return FALSE;
	}
}

BOOL CModelToolApp::F_SetEventWork()
{
	if( m_eObjectType != AMT_OBJECT_TYPE_OBJECT )	return FALSE;

	ApdObject* pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObject( m_lID );
	if( !pcsApdObject )		return FALSE;

	m_csAgcEngine.GetAgcmObjectModule()->ForceEnumCallback( AGCMOBJECT_CB_ID_WORK_OBJECT, pcsApdObject );

	return TRUE;
}


//@{ Jaewon 20041004
#include "AcuRpMatD3DFx.h"
static RpMaterial *materialTurnSkinningOffCB(RpMaterial *pMaterial, void *data)
{
	DxEffect *pEffect = RpMaterialD3DFxGetEffect(pMaterial);
	if(pEffect)	
	{
		//@{ Jaewon 20041020
		// RpMaterialD3DFxSetEffect invalidates pEffect->pSharedD3dXEffect->name,
		// so at first save the name to a temporary buffer.
		char buf[FX_FILE_NAME_LENGTH];
		strcpy(buf, pEffect->pSharedD3dXEffect->name);
		RwBool bOK = RpMaterialD3DFxSetEffect(pMaterial, buf, NULL);
		//@} Jaewon
		ASSERT(bOK);
		*(BOOL*)data = TRUE;
	}

	return pMaterial;
}
static RpAtomic *atomicTurnSkinningOffCB(RpAtomic *pAtomic, void* data)
{
	RpGeometry *pGeometry = RpAtomicGetGeometry(pAtomic);
	BOOL bUseMatD3DFxPipe = FALSE;
	RpGeometryForAllMaterials(pGeometry, materialTurnSkinningOffCB, (void*)&bUseMatD3DFxPipe);
	if(bUseMatD3DFxPipe)
		RpSkinGeometrySetSkin(pGeometry, NULL);

	return pAtomic;
}
//@} Jaewon

BOOL CModelToolApp::F_SetRenderType()
{
	RpAtomic *pAtomic = NULL;
	switch (m_eObjectType)
	{
	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItemTemplate *ppdItemTemplate = m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(m_lTID);
			if( ppdItemTemplate == NULL )
				return FALSE;

			AgcdItemTemplate *pcdItemTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(ppdItemTemplate);
			if( pcdItemTemplate == NULL )
				return FALSE;

			//@{ Jaewon 20050701
			// If there is a armour clump, use it instead of m_pstBaseClump.
			if( m_pcsArmourClump1 )
			{
				pAtomic = m_pcsArmourClump1->atomicList;
			}
			else if( pcdItemTemplate->m_pstBaseClump ) {
				pAtomic = pcdItemTemplate->m_pstBaseClump->atomicList;
			}
			//@} Jaewon
		}
		break;
	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObject						*pObject	= m_csAgcEngine.GetApmObjectModule()->GetObject(m_lID);
			if (!pObject)
				return NULL;
			AgcdObject						*cObject	= m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pObject);
			if ((!cObject) || (!cObject->m_pstTemplate))
				return NULL;

			AgcdObjectGroupList	*			pstList	= cObject->m_stGroup.m_pstList;

			if( pstList )
				pAtomic = pstList->m_csData.m_pstClump->atomicList;
		}
		break;
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacter	*pcsAgpdCharacter	= GetEngine()->GetAgpmCharacterModule()->GetCharacter(m_lID);
			if (!pcsAgpdCharacter)
				return FALSE;

			AgcdCharacter	*pcsAgcdCharacter	= GetEngine()->GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
			if (!pcsAgcdCharacter)
				return FALSE;

			GetEngine()->GetAgcmCharacterModule()->DropAnimation(pcsAgcdCharacter);
			RpClumpForAllAtomics( pcsAgcdCharacter->m_pClump, atomicTurnSkinningOffCB, NULL );
		}
		break;

	default:
		return FALSE;
	}

	m_csAgcEngine.GetAgcmUDADlgModule()->OpenMainUDADlg(FALSE);

	SetUDAData( pAtomic );

	SetSaveData();

	return TRUE;
}

BOOL CModelToolApp::F_SetFace()
{
	if (m_eObjectType != AMT_OBJECT_TYPE_CHARACTER)
		return FALSE;

	AgpdCharacter		*pObject		= m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
	if (!pObject)
		return FALSE;
	AgcdCharacter		*cObject		= m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pObject);
	if ((!cObject) || (!cObject->m_pstAgcdCharacterTemplate) || (!cObject->m_pClump))
		return FALSE;

	RpClumpForAllAtomics(cObject->m_pClump, atomicTurnSkinningOffCB, NULL);

	m_csAgcEngine.GetAgcmSetFaceDlgModule()->OpenMainFaceDlg();

	RpAtomic *	pstAtomic = cObject->m_pClump->atomicList;
	do
	{
		if (pstAtomic->id == cObject->m_pstAgcdCharacterTemplate->m_lFaceAtomicIndex)
			SetFaceAtomic(pstAtomic);

		pstAtomic	= pstAtomic->next;
	} while (pstAtomic != cObject->m_pClump->atomicList);

	SetSaveDataCharacter();

	return TRUE;
}

BOOL CModelToolApp::F_CreateClump()
{
	if (!m_csAgcEngine.GetAgcmBlockingDlgModule()->IsOpenEditBlocking())
		return FALSE;

	if (CModelToolDlg::GetInstance()->MessageBox("현재 작업하시는 geometry를\nclump로 생성하시겠습니까?", "Create clump", MB_ICONQUESTION | MB_YESNO) == IDNO)
		return TRUE;

	m_nNumBlocking = 0;
	for (INT32 lCount = 0; lCount < AMT_MAX_GEOMETRY_DATA_ARRAY; ++lCount)
	{
		if (m_astBlocking[lCount].type == AUBLOCKING_TYPE_NONE)
			continue;

		++m_nNumBlocking;
	}

	if (!m_nNumBlocking)
	{
		CModelToolDlg::GetInstance()->MessageBox("Geometry가 없습니다!", "ERROR!");
		return TRUE;
	}

	CHAR		szFileName[AMT_MAX_STR];
	memset(szFileName, 0, AMT_MAX_STR);
	CEditDlg	dlg(szFileName, "파일 이름을 입력해 주세요.");
	if (dlg.DoModal() != IDOK)
		return FALSE;

	CHAR		szPath[AMT_MAX_STR];

	switch (m_eObjectType)
	{
	case AMT_OBJECT_TYPE_CHARACTER:
	case AMT_OBJECT_TYPE_ITEM:
		{
			sprintf(szPath, "%s%s", AMT_CHARACTER_DATA_PATH, szFileName);
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			sprintf(szPath, "%s%s", AMT_OBJECT_DATA_PATH, szFileName);
		}
		break;

	default:
		return FALSE;
	}

	return ACC_CREATE_CLUMP(szPath, m_astBlocking, m_nNumBlocking);
}

BOOL CModelToolApp::F_EditFace()
{
	if( m_eObjectType != AMT_OBJECT_TYPE_CHARACTER )		return FALSE;

	AgpmCharacter* pAgpmCharacter = m_csAgcEngine.GetAgpmCharacterModule();
	AgcmCharacter* pAgcmCharacter = m_csAgcEngine.GetAgcmCharacterModule();

	AgpdCharacter *pcsAgpdCharacter	= pAgpmCharacter->GetCharacter(m_lID);
	if( !pcsAgpdCharacter )		return FALSE;
		
	AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter )		return FALSE;
	if( !pcsAgcdCharacter->m_pstAgcdCharacterTemplate )		return FALSE;

	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;
	if( !pCharTemplate->m_pcsDefaultHeadData )
        pCharTemplate->m_pcsDefaultHeadData = new AgcdCharacterDefaultHeadTemplate;

	AgcdCharacterDefaultHeadTemplate* pDefaultTemplate = pCharTemplate->m_pcsDefaultHeadData;

	CEditFaceDlg dlgEditFace;

	vector< string >& vecString	= pDefaultTemplate->vecFaceInfo;
	for( vector< string >::iterator Itr = vecString.begin(); Itr != vecString.end(); ++Itr )
		dlgEditFace.AddDefaultFace( (*Itr).c_str() );

	if( dlgEditFace.DoModal() != IDOK )		return TRUE;

	pAgcmCharacter->DetachFace( pcsAgcdCharacter );
	pcsAgcdCharacter->m_pFace = NULL;

	int nFaceNum = dlgEditFace.GetDefaultFaceNum();
	pcsAgpdCharacter->m_pcsCharacterTemplate->m_lFaceNum = nFaceNum;

	vecString.clear();
	for( int i = 0; i < nFaceNum; i++ )
		vecString.push_back( dlgEditFace.GetDefaultFaceNth( i ) );

	pDefaultTemplate->m_FaceRenderType.Clear();
	pDefaultTemplate->m_FaceRenderType.Alloc( nFaceNum );
	pDefaultTemplate->m_FaceRenderType.MemsetRenderType( R_ALPHAFUNC );
	pDefaultTemplate->m_FaceRenderType.MemsetCustData( 0 );

	m_csAgcEngine.GetAgcmCharacterModule()->LoadTemplateDefaultFace( pCharTemplate );

	// attach face dialog
	if( CModelToolDlg::GetInstance()->IsOpenAttachFaceDlg() )
	{
		CAttachFaceDlg* pAttachFaceDlg = CModelToolDlg::GetInstance()->GetAttachFaceDlg();
		pAttachFaceDlg->SetDefaultFace();
	}

	SetSaveDataCharacter();

	return TRUE;
}

BOOL CModelToolApp::F_AttachFace()
{
	if (m_eObjectType != AMT_OBJECT_TYPE_CHARACTER)
		return FALSE;

	if( CModelToolDlg::GetInstance()->IsOpenAttachFaceDlg() )
		return TRUE;

	CModelToolDlg::GetInstance()->OpenAttachFaceDlg();
	return TRUE;
}

BOOL CModelToolApp::F_EditHair()
{
	if( m_eObjectType != AMT_OBJECT_TYPE_CHARACTER )		return FALSE;

	AgpmCharacter* pAgpmCharacter = m_csAgcEngine.GetAgpmCharacterModule();
	AgcmCharacter* pAgcmCharacter = m_csAgcEngine.GetAgcmCharacterModule();

	AgpdCharacter *pcsAgpdCharacter	= pAgpmCharacter->GetCharacter(m_lID);
	if( !pcsAgpdCharacter )		return FALSE;

	AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter )		return FALSE;
	if( !pcsAgcdCharacter->m_pstAgcdCharacterTemplate )	return FALSE;
		
	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;
	if( !pCharTemplate->m_pcsDefaultHeadData )
		pCharTemplate->m_pcsDefaultHeadData = new AgcdCharacterDefaultHeadTemplate;

	AgcdCharacterDefaultHeadTemplate* pDefaultTemplate = pCharTemplate->m_pcsDefaultHeadData;

	CEditHairDlg dlgEditHair;

	vector< string >& vecString	= pDefaultTemplate->vecHairInfo;
	for( vector< string >::iterator Itr = vecString.begin(); Itr != vecString.end(); ++Itr )
		dlgEditHair.AddDefaultHair( (*Itr).c_str() );

	if( dlgEditHair.DoModal() != IDOK )
		return TRUE;

	pAgcmCharacter->DetachHair( pcsAgcdCharacter );
	pcsAgcdCharacter->m_pHair = NULL;

	int nHairNum = dlgEditHair.GetDefaultHairNum();
	pcsAgpdCharacter->m_pcsCharacterTemplate->m_lHairNum = nHairNum;

	vecString.clear();
	for( int i = 0; i < nHairNum; i++ )
		vecString.push_back( dlgEditHair.GetDefaultHairNth( i ) );

	pDefaultTemplate->m_HairRenderType.Clear();
	pDefaultTemplate->m_HairRenderType.Alloc( nHairNum );
	pDefaultTemplate->m_HairRenderType.MemsetRenderType( R_ALPHAFUNC );
	pDefaultTemplate->m_HairRenderType.MemsetCustData( 0 );

	m_csAgcEngine.GetAgcmCharacterModule()->LoadTemplateDefaultHair( pCharTemplate );

	// attach face dialog
	if( CModelToolDlg::GetInstance()->IsOpenAttachHairDlg() )
	{
		CAttachHairDlg* pAttachHairDlg = CModelToolDlg::GetInstance()->GetAttachHairDlg();
		pAttachHairDlg->SetDefaultHair();
	}

	SetSaveDataCharacter();

	return TRUE;
}

BOOL CModelToolApp::F_AttachHair()
{
	if (m_eObjectType != AMT_OBJECT_TYPE_CHARACTER)
		return FALSE;

	if( CModelToolDlg::GetInstance()->IsOpenAttachHairDlg() )
		return TRUE;

	CModelToolDlg::GetInstance()->OpenAttachHairDlg();

	return TRUE;
}

BOOL CModelToolApp::F_FaceRenderType()
{
	if (m_eObjectType != AMT_OBJECT_TYPE_CHARACTER)
		return FALSE;

	AgpmCharacter* pAgpmCharacter = m_csAgcEngine.GetAgpmCharacterModule();
	AgcmCharacter* pAgcmCharacter = m_csAgcEngine.GetAgcmCharacterModule();

	AgpdCharacter *pcsAgpdCharacter	= pAgpmCharacter->GetCharacter(m_lID);
	if( pcsAgpdCharacter == NULL )
		return FALSE;

	AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return FALSE;

	if( pcsAgcdCharacter->m_pstAgcdCharacterTemplate == NULL )
		return FALSE;

	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;
	if( pCharTemplate->m_pcsDefaultHeadData == NULL ) {
		return FALSE;
	}

	if( pcsAgcdCharacter->m_pFace == NULL )
		return FALSE;

	CDefaultHeadRenderTypeDlg RenderTypeDlg;
	RenderTypeDlg.SetState( CDefaultHeadRenderTypeDlg::STATE_FACE );
	if( RenderTypeDlg.DoModal() == IDOK )
	{
		SetSaveDataCharacter();
	}

	return TRUE;
}

BOOL CModelToolApp::F_HairRenderType()
{
	if( m_eObjectType != AMT_OBJECT_TYPE_CHARACTER )
		return FALSE;

	AgpmCharacter* pAgpmCharacter = m_csAgcEngine.GetAgpmCharacterModule();
	AgcmCharacter* pAgcmCharacter = m_csAgcEngine.GetAgcmCharacterModule();

	AgpdCharacter *pcsAgpdCharacter	= pAgpmCharacter->GetCharacter(m_lID);
	if( pcsAgpdCharacter == NULL )
		return FALSE;

	AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return FALSE;

	if( pcsAgcdCharacter->m_pstAgcdCharacterTemplate == NULL )
		return FALSE;

	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;
	if( pCharTemplate->m_pcsDefaultHeadData == NULL ) {
		return FALSE;
	}

	if( pcsAgcdCharacter->m_pHair == NULL )
		return FALSE;

	CDefaultHeadRenderTypeDlg RenderTypeDlg;
	RenderTypeDlg.SetState( CDefaultHeadRenderTypeDlg::STATE_HAIR );
	if( RenderTypeDlg.DoModal() == IDOK )
	{
		SetSaveDataCharacter();
	}

	return TRUE;
}

BOOL CModelToolApp::F_CustomizePreviewType()
{
	if( m_eObjectType != AMT_OBJECT_TYPE_CHARACTER )
		return FALSE;

	if( CModelToolDlg::GetInstance()->IsOpenCustomizePreviewDlg() )
		return TRUE;

	CModelToolDlg::GetInstance()->OpenCustomizePreviewDlg();

	return TRUE;
}

BOOL CModelToolApp::F_RideDialog()
{
	if( m_eObjectType != AMT_OBJECT_TYPE_CHARACTER )
		return FALSE;

	if( CModelToolDlg::GetInstance()->IsOpenRideDlg() )
		return TRUE;

	CModelToolDlg::GetInstance()->OpenRideDlg();

	return TRUE;
}

BOOL CModelToolApp::RefreshRenderType()
{	
	RpAtomic* pAtomic = NULL;

	switch (m_eObjectType)
	{
	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItemTemplate *ppdItemTemplate = m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(m_lTID);
			if( ppdItemTemplate == NULL )
				return FALSE;

			AgcdItemTemplate *pcdItemTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(ppdItemTemplate);
			if( pcdItemTemplate == NULL )
				return FALSE;

			//@{ Jaewon 20050701
			// If there is a armour clump, use it instead of m_pstBaseClump.
			if( m_pcsArmourClump1 )
			{
				pAtomic = m_pcsArmourClump1->atomicList;
			}
			else if( pcdItemTemplate->m_pstBaseClump ) {
				pAtomic = pcdItemTemplate->m_pstBaseClump->atomicList;
			}
			//@} Jaewon
		}
		break;
	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObject						*pObject	= m_csAgcEngine.GetApmObjectModule()->GetObject(m_lID);
			if (!pObject)
				return NULL;
			AgcdObject						*cObject	= m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pObject);
			if ((!cObject) || (!cObject->m_pstTemplate))
				return NULL;

			AgcdObjectGroupList	*			pstList	= cObject->m_stGroup.m_pstList;

			if( pstList )
				pAtomic = pstList->m_csData.m_pstClump->atomicList;
		}
		break;
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacter	*pcsAgpdCharacter	= GetEngine()->GetAgpmCharacterModule()->GetCharacter(m_lID);
			if (!pcsAgpdCharacter)
				return FALSE;

			AgcdCharacter	*pcsAgcdCharacter	= GetEngine()->GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
			if (!pcsAgcdCharacter)
				return FALSE;
		}
		break;

	default:
		return FALSE;
	}

	RpClump* pClump	= NULL;

	if ( pAtomic )
	{
		pClump = RpAtomicGetClump(pAtomic);
	}
	else
	{
		if (m_eObjectType == AMT_OBJECT_TYPE_CHARACTER )
		{
			AgpdCharacter* pcsAgpdCharacter	= GetEngine()->GetAgpmCharacterModule()->GetCharacter(m_lID);
			if ( pcsAgpdCharacter )
			{
				AgcdCharacter* pcsAgcdCharacter	= GetEngine()->GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
				if ( pcsAgcdCharacter )
					pClump = pcsAgcdCharacter->m_pClump;
			}
		}
	}
	
	if ( pClump )
	{
		INT32 eType = pClump->stType.eType;
		if ( AcuObject::GetProperty(eType) & ACUOBJECT_TYPE_RENDER_UDA && pClump->atomicList != NULL )
		{
			AgcdClumpRenderType* pRenderType = GetCurrentRenderType(pClump->atomicList);
			if ( pRenderType )
			{
				m_csAgcEngine.GetAgcmRenderModule()->RemoveClumpFromWorld(pClump);
				m_csAgcEngine.GetAgcmRenderModule()->ClumpSetRenderType(pClump, pRenderType);
				m_csAgcEngine.GetAgcmRenderModule()->AddClumpToWorld(pClump);
			}
		}
	}

	return TRUE;
}
