#include "stdafx.h"
#include "ModelTool.h"
#include "AgcmIDBoard.h"

BOOL CModelToolApp::SetEmpty()
{
	if(!ReleaseAllObject())
		return FALSE;

	InitializeMember();
	InitializeTitle();

	return TRUE;
}

RpClump *CModelToolApp::SetStaticModel( const CHAR *szDffName, BOOL bObject)
{
	RemoveTargetCharacter();

	if(!ReleaseAllObject())
		return NULL;

	if(!CModelToolDlg::GetInstance())
		return NULL;

	if(!CModelToolDlg::GetInstance()->GetRenderForm())
		return NULL;

	if(!CModelToolDlg::GetInstance()->GetRenderForm()->GetRenderWare())
		return NULL;

	if(!CModelToolDlg::GetInstance()->GetRenderForm()->GetRenderWare()->GetWorld())
		return NULL;

	if (!m_csAgcEngine.GetAgcmResourceLoaderModule())
		return NULL;

	CHAR szDffPath[AMT_MAX_STR];
	sprintf( szDffPath, "%s%s", bObject ? AMT_OBJECT_DATA_PATH : AMT_CHARACTER_DATA_PATH, szDffName );
	m_pcsStaticModel = m_csAgcEngine.GetAgcmResourceLoaderModule()->LoadClump(szDffPath);
	if(!m_pcsStaticModel)
		return NULL;

	if (!m_csAgcEngine.GetAgcmRenderModule())
		return NULL;

	AcuObject::SetClumpType(m_pcsStaticModel, 0);
	m_csAgcEngine.GetAgcmRenderModule()->AddClumpToWorld(m_pcsStaticModel, AGCMRENDER_ADD_NONE);

	RwFrame *pstFrame = RpClumpGetFrame(m_pcsStaticModel);
	if(pstFrame)
	{
		RwV3d v3dTemp = {0.0f, 0.0f, 0.0f};
		RwFrameTranslate(pstFrame, &v3dTemp, rwCOMBINEREPLACE);
	}

	strcpy(m_szStaticModelDffName, szDffName);

	m_eObjectType = AMT_OBJECT_TYPE_STATIC_MODEL;

	if(CModelToolDlg::GetInstance()->GetTitleForm())
	{
		CHAR szTitle[AMT_MAX_STR];
		sprintf(szTitle, "%s - %s   {%s}", AMT_TITLE_NAME_STATIC_MODEL, m_szStaticModelDffName, g_pNationalCodeStr[m_uNationalCode]);
		CModelToolDlg::GetInstance()->GetTitleForm()->SetCurrentTitle(szTitle);
	}

	ResetCamera();

	return m_pcsStaticModel;
}

BOOL CModelToolApp::LoadTerrain( BOOL bChange )
{
	//return TRUE;

	if( !bChange && m_pcsBottomClump )		return FALSE;

	if( bChange && m_pcsBottomClump )
		ReleaseClump( &m_pcsBottomClump );

	m_pcsBottomClump = m_csAgcEngine.GetAgcmResourceLoaderModule()->LoadClump( (char*)m_csRenderOption.m_strTerrain.GetBuffer() );
	if( m_pcsBottomClump )
	{
		AcuObject::SetClumpType(m_pcsBottomClump, 0);
		m_csAgcEngine.GetAgcmRenderModule()->AddClumpToWorld(m_pcsBottomClump, AGCMRENDER_ADD_NONE);

		RwV3d	v3dTemp = {3.0f, 1.0f, 3.0f};
		RwFrameScale( RpClumpGetFrame(m_pcsBottomClump), &v3dTemp, rwCOMBINEREPLACE );

		v3dTemp.x = 0.0f;
		v3dTemp.y = 0.0f;
		v3dTemp.z = 350.0f;
		RwFrameTranslate(RpClumpGetFrame(m_pcsBottomClump), &v3dTemp, rwCOMBINEPOSTCONCAT);
	}

	return TRUE;
}

AgpdCharacter *CModelToolApp::SetCharacter(INT32 lTID, HTREEITEM hSelected, BOOL bResetCamera, BOOL bReleaseAll, INT32 lForceCID, RwV3d *pv3dForcePos)
{
	if( !m_csAgcEngine.GetAgpmCharacterModule() || !m_csAgcEngine.GetAgcmCharacterModule() )
		return NULL;

	if( bReleaseAll && !ReleaseAllObject( FALSE ) )
		return NULL;

	INT32 lCID	= lForceCID ? lForceCID : GenerateCID();

	CHAR	szGameID[AMT_MAX_STR];
	sprintf( szGameID, "AMT%d", lCID );
	AgpdCharacter *pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->AddCharacter(lCID, lTID, szGameID);
	if ( !pcsAgpdCharacter )				return NULL;

	if( !lForceCID )
		pcsAgpdCharacter->m_ulCharType |= AGPMCHAR_TYPE_PC;

	AgpdCharacterTemplate *pcsAgpdCharacterTemplate = pcsAgpdCharacter->m_pcsCharacterTemplate;
	if (!pcsAgpdCharacterTemplate)			return NULL;

	if (m_csAgcEngine.GetAgpmItemModule())
	{
		AgpdItemADCharTemplate *pcsAgpdItemADCharTemplate = m_csAgcEngine.GetAgpmItemModule()->GetADCharacterTemplate(pcsAgpdCharacterTemplate);
		if (!pcsAgpdItemADCharTemplate)		return NULL;

		if (!m_csAgcEngine.GetAgpmCharacterModule()->UpdateInit(pcsAgpdCharacter))
			return NULL;

		int nSpecialOffset = AGPMITEM_PART_LANCER - AGPMITEM_PART_BODY + 1;
		for (INT16 nPart = AGPMITEM_PART_BODY; nPart <= AGPMITEM_PART_HAND_RIGHT; ++nPart)
		{
			if( pcsAgpdItemADCharTemplate->m_lDefaultEquipITID[ nPart + nSpecialOffset ] )
			{
				if( !EquipItem(lCID, pcsAgpdItemADCharTemplate->m_lDefaultEquipITID[ nPart + nSpecialOffset ] ) )
					continue;
			}
			else if( pcsAgpdItemADCharTemplate->m_lDefaultEquipITID[ nPart ] )
			{
				if( !EquipItem(lCID, pcsAgpdItemADCharTemplate->m_lDefaultEquipITID[ nPart ] ) )
					continue;
			}
		}
	}

	m_eObjectType	= AMT_OBJECT_TYPE_CHARACTER;
	m_lID			= lCID;
	m_lTID			= lTID;
	
	AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
	if (!pcsAgcdCharacter)				return NULL;
	if (!pcsAgcdCharacter->m_pClump)	return NULL;

	AgcdCharacterTemplate *pstAgcdCharacterTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;
	if (!pstAgcdCharacterTemplate)		return NULL;

	if (CModelToolDlg::GetInstance())
	{
		if(CModelToolDlg::GetInstance()->GetTitleForm())
		{
			CHAR szTitle[AMT_MAX_STR];
			sprintf(szTitle, "%s - [%d]%s   {%s}", AMT_TITLE_NAME_CHARACTER, pcsAgpdCharacterTemplate->m_lID, pcsAgpdCharacterTemplate->m_szTName, g_pNationalCodeStr[m_uNationalCode]);
			CModelToolDlg::GetInstance()->GetTitleForm()->SetCurrentTitle(szTitle);
		}
	}

	if (pv3dForcePos)
	{
		RwFrame	*pstForceFrame = RpClumpGetFrame(pcsAgcdCharacter->m_pClump);
		RwFrameRotate(pstForceFrame, &RWUTIL_YAXIS, 180.0f, rwCOMBINEPOSTCONCAT);
		RwFrameTranslate(pstForceFrame, pv3dForcePos, rwCOMBINEPOSTCONCAT);

		AuPOS cIniPos;
		cIniPos.x = pv3dForcePos->x;
		cIniPos.y = pv3dForcePos->y;
		cIniPos.z = pv3dForcePos->z;
		m_csAgcEngine.GetAgpmCharacterModule()->UpdatePosition(pcsAgpdCharacter, &cIniPos);
	}
	else
	{
		RwV3d	v3dDefPos	= {0.0f, 0.0f, 0.0f};
		RwFrameTranslate(RpClumpGetFrame(pcsAgcdCharacter->m_pClump), &v3dDefPos, rwCOMBINEPOSTCONCAT);
	}

	if (bResetCamera)
		ResetCamera();

	m_hCurrentTreeItem = hSelected;

	if (m_csAgcEngine.GetAgcmEventEffectModule())
		m_csAgcEngine.GetAgcmEventEffectModule()->SetSelfCID(m_lID);

	SetTarget(pcsAgpdCharacter, pcsAgcdCharacter);

	if (m_csAgcEngine.GetAgcmEventEffectModule())
	{
		INT32 lOffset = m_lStartTargetID;
		for (INT32 lCount = 0; lCount < m_lNumTargets; ++lCount, --lOffset)
		{
			AgpdCharacter* pcsTarget = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(lOffset);
			if (pcsTarget)
				m_csAgcEngine.GetAgcmEventEffectModule()->RemoveAllEffect((ApBase *)(pcsTarget));
		}
	}

	if( m_csAgcEngine.GetAgcmCharacterModule() && m_csAgcEngine.GetAgpmCharacterModule()->IsPC( pcsAgpdCharacter ) )
		m_csAgcEngine.GetAgcmCharacterModule()->SetSelfCharacter(pcsAgpdCharacter, FALSE);

	if (	(m_csAgcEngine.GetAgcmSkillDlgModule()) &&
			(m_csAgcEngine.GetAgcmEffectDlgModule()) &&
			(m_csAgcEngine.GetAgcmEventEffectModule())	)
	{
		if (m_csAgcEngine.GetAgcmSkillDlgModule()->IsOpenCharacterSkillDlg())
		{
			AgpdSkillTemplateAttachData	*pcsAgpdAttachSkillTemplateData		= m_csAgcEngine.GetAgpmSkillModule()->GetAttachSkillTemplateData((ApBase *)(pcsAgpdCharacterTemplate));
			AgcdSkillAttachTemplateData	*pstAgcdSkillAttachTemplateData		= m_csAgcEngine.GetAgcmSkillModule()->GetAttachTemplateData((ApBase *)(pcsAgpdCharacterTemplate));

			if ((!pcsAgpdAttachSkillTemplateData) || (!pstAgcdSkillAttachTemplateData))
				return NULL;

			m_csAgcEngine.GetAgcmSkillDlgModule()->InitializeCharacterSkillDlg(
				pstAgcdCharacterTemplate,
				pcsAgpdAttachSkillTemplateData,
				pstAgcdSkillAttachTemplateData									);
		}
		else if (m_csAgcEngine.GetAgcmEffectDlgModule()->IsOpenUseEffectSet())
		{
			AgcdUseEffectSet *pstAgcdUseEffectSet = m_csAgcEngine.GetAgcmEventEffectModule()->GetUseEffectSet(pcsAgpdCharacterTemplate);
			if (!pstAgcdUseEffectSet)
				return FALSE;

			if (!m_csAgcEngine.GetAgcmEffectDlgModule()->InitializeUseEffectSet(pstAgcdUseEffectSet))
				return FALSE;
		}
	}

	LoadTerrain();

	SetMatD3DFxCB();

	AgcmIDBoard*	pIDBoard	=	(*m_csAgcEngine.GetAgcmTextBoardModule()->GetBoardDataList().begin())->pIDBoard;

	if( m_csRenderOption.m_bShowTextBoarder == FALSE ) 
	{
		if ( pIDBoard )		pIDBoard->SetEnabled( FALSE );
		
		m_csAgcEngine.GetAgcmTextBoardModule()->SetDrawHPMP( FALSE );		//. HP
		m_csAgcEngine.GetAgcmTextBoardModule()->SetDrawNameMine( FALSE );	//. "AMT1",
	}
	else
	{
		if ( pIDBoard )		pIDBoard->SetEnabled( TRUE );
			
		m_csAgcEngine.GetAgcmTextBoardModule()->SetDrawHPMP( TRUE );		//. HP
		m_csAgcEngine.GetAgcmTextBoardModule()->SetDrawNameMine( TRUE );	//. "AMT1",
	}

	//lod 체크..
	if( !GetEngine()->GetAgcmCharacterModule()->IsValidLOD( pcsAgpdCharacterTemplate, pstAgcdCharacterTemplate ) )
		MessageBox( NULL, "LOD 데이타가 이상합니다.", "LOD ERROR", MB_OK );
	
	return pcsAgpdCharacter;
}

AgpdItem *CModelToolApp::EquipItem(INT32 lCID, INT32 lTID)
{
	if( !m_csAgcEngine.GetAgpmItemModule() )		return NULL;

	AgpmItem*		pAgpmItem		= m_csAgcEngine.GetAgpmItemModule();
	if( !pAgpmItem )		return NULL;

	AgpmCharacter*	pAgpmCharacter	= m_csAgcEngine.GetAgpmCharacterModule();
	if( !pAgpmCharacter )	return NULL;

	AgpdItem *pcsAgpdItem = pAgpmItem->AddItem( GenerateIID(), lTID, 0 );
	if( !pcsAgpdItem )		return NULL;
	
	AgpdCharacter *pcsAgpdCharacter	= m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(lCID);
	if( !pcsAgpdCharacter )	return NULL;

	pcsAgpdItem->m_ulCID = pcsAgpdCharacter->m_lID;
	pcsAgpdItem->m_pcsCharacter = pcsAgpdCharacter;

	if( !pAgpmItem->InitItem( pcsAgpdItem ) )
		return NULL;

	// 장착할때만 살짝 아바타 아이템도 일반 아이템으로 바꾼다.
	AgpdItemTemplateEquip *pTemplateEquip = (AgpdItemTemplateEquip *) pcsAgpdItem->m_pcsItemTemplate;
	AgpmItemPart	nPartBackup = pTemplateEquip->m_nPart;

	pTemplateEquip->m_nPart = pTemplateEquip->GetPartIndex();
	if( !pAgpmItem->EquipItem( pcsAgpdCharacter, pcsAgpdItem, FALSE ) )
	{
		pcsAgpdItem = NULL;
		return NULL;
	}
	pTemplateEquip->m_nPart = nPartBackup;

	return pcsAgpdItem;
}

//@{ Jaewon 20041004
#include "AcuRpMatD3DFx.h"
static RpMaterial *materialHasMatD3dFxPipeCB(RpMaterial *pMaterial, void *data)
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
static RpAtomic *atomicTurnSkinningOffCB(RpAtomic *pAtomic, void *data)
{
	RpGeometry *pGeometry = RpAtomicGetGeometry(pAtomic);

	BOOL bHasMatD3dFxPipe = FALSE;
	RpGeometryForAllMaterials(pGeometry, materialHasMatD3dFxPipeCB, (void*)&bHasMatD3dFxPipe);
	
	if(bHasMatD3dFxPipe)
		RpSkinGeometrySetSkin(pGeometry, NULL);

	return pAtomic;
}
//@} Jaewon

AgpdItem *CModelToolApp::SetItem(INT32 lTID, HTREEITEM hSelected, BOOL bResetCamera, BOOL bReleaseAll)
{
	if ( !m_csAgcEngine.GetAgpmItemModule() || !m_csAgcEngine.GetAgcmItemModule() )
		return NULL;

	if (m_bExport && m_eObjectType == AMT_OBJECT_TYPE_CHARACTER)
	{
		if (CModelToolDlg::GetInstance()->MessageBox("캐릭터에 장착하시겠습니까?", "Equip", MB_ICONQUESTION | MB_YESNO) == IDYES)
			return EquipItem(m_lID, lTID);
	}
	else if ( m_eObjectType == AMT_OBJECT_TYPE_CHARACTER && CModelToolDlg::GetInstance()->GetResourceForm()->IsEquipments(hSelected) )
	{
		if (CModelToolDlg::GetInstance()->MessageBox("캐릭터에 장착하시겠습니까?", "Equip", MB_ICONQUESTION | MB_YESNO) == IDYES)
			return EquipItem(m_lID, lTID);
	}

	RemoveTargetCharacter();

	if (bReleaseAll)
	{
		if(!ReleaseAllObject(FALSE))
			return NULL;
	}

	INT32 lIID = GenerateIID();
	AgpdItem *pcsAgpdItem = m_csAgcEngine.GetAgpmItemModule()->AddItem(lIID, lTID, 0);
	if (!pcsAgpdItem)
		return NULL;

	AgpdItemTemplate *pcsAgpdItemTemplate = (AgpdItemTemplate *)(pcsAgpdItem->m_pcsItemTemplate);
	if (!pcsAgpdItemTemplate)
		return NULL;

	if (!m_csAgcEngine.GetAgpmItemModule()->InitItem(pcsAgpdItem))
		return NULL;

	if (!m_csAgcEngine.GetAgpmItemModule()->AddItemToField(pcsAgpdItem))
		return NULL;

	m_eObjectType	= AMT_OBJECT_TYPE_ITEM;
	m_lID			= lIID;
	m_lTID			= lTID;

	AgcdItem *pcsAgcdItem = m_csAgcEngine.GetAgcmItemModule()->GetItemData(pcsAgpdItem);
	if (!pcsAgcdItem)
		return NULL;

	AgcdItemTemplate *pcsAgcdItemTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(pcsAgpdItemTemplate);
	if (!pcsAgcdItemTemplate)
		return NULL;

	if (!m_bExport && !pcsAgcdItemTemplate->m_pcsPreData)
		return NULL;

	if (m_pcsArmourClump1)
		ReleaseClump(&m_pcsArmourClump1);

	if (m_pcsArmourClump2)
		ReleaseClump(&m_pcsArmourClump2);

	if ( pcsAgpdItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP && ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR )
	{
		RwFrame		*pstArmourFrame;

		if (pcsAgcdItemTemplate->m_pstBaseClump)
		{
			m_pcsArmourClump1 = m_csAgcEngine.GetAgcmItemModule()->CreateArmourPartClump(pcsAgcdItemTemplate->m_pstBaseClump, ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->GetPartIndex());
			if (m_pcsArmourClump1)
			{
				if (pcsAgcdItemTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)
					AcuObject::SetClumpPreLitLim( m_pcsArmourClump1, &pcsAgcdItemTemplate->m_pcsPreData->m_stPreLight );

				RpClumpForAllAtomics(m_pcsArmourClump1, atomicTurnSkinningOffCB, NULL);

				AcuObject::SetClumpType(m_pcsArmourClump1,
										pcsAgcdItemTemplate->m_lObjectType | ACUOBJECT_TYPE_ITEM,
										pcsAgpdItem->m_lID,
										pcsAgpdItem,
										pcsAgpdItemTemplate,
										NULL,
										NULL,
										&pcsAgcdItemTemplate->m_stBSphere,
										NULL,
										&pcsAgcdItemTemplate->m_stOcTreeData);

				if (pcsAgcdItemTemplate->m_csClumpRenderType.m_lSetCount)
					m_csAgcEngine.GetAgcmRenderModule()->ClumpSetRenderType(m_pcsArmourClump1, &pcsAgcdItemTemplate->m_csClumpRenderType);

				m_csAgcEngine.GetAgcmRenderModule()->AddClumpToWorld(m_pcsArmourClump1);

				pstArmourFrame = RpClumpGetFrame(m_pcsArmourClump1);
				if (pstArmourFrame)
				{
					RwV3d v3dTemp = {0.0f, 0.0f, -100.0f};
					RwFrameTranslate(pstArmourFrame, &v3dTemp, rwCOMBINEREPLACE);
				}
			}
		}

		if (pcsAgcdItemTemplate->m_pstSecondClump)
		{
			m_pcsArmourClump2 = m_csAgcEngine.GetAgcmItemModule()->CreateArmourPartClump(pcsAgcdItemTemplate->m_pstSecondClump, ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->GetPartIndex());
			if (m_pcsArmourClump2)
			{
				if (pcsAgcdItemTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)
					AcuObject::SetClumpPreLitLim( m_pcsArmourClump2, &pcsAgcdItemTemplate->m_pcsPreData->m_stPreLight );
						
				RpClumpForAllAtomics(m_pcsArmourClump2, atomicTurnSkinningOffCB, NULL);

				AcuObject::SetClumpType(m_pcsArmourClump2,
										pcsAgcdItemTemplate->m_lObjectType | ACUOBJECT_TYPE_ITEM,
										pcsAgpdItem->m_lID,
										pcsAgpdItem,
										pcsAgpdItemTemplate,
										NULL,
										NULL,
										&pcsAgcdItemTemplate->m_stBSphere,
										NULL,
										&pcsAgcdItemTemplate->m_stOcTreeData);

				if (pcsAgcdItemTemplate->m_csClumpRenderType.m_lSetCount)
					m_csAgcEngine.GetAgcmRenderModule()->ClumpSetRenderType(m_pcsArmourClump2, &pcsAgcdItemTemplate->m_csClumpRenderType);

				m_csAgcEngine.GetAgcmRenderModule()->AddClumpToWorld(m_pcsArmourClump2);

				if (pstArmourFrame)
				{
					RwV3d v3dTemp = {-100.0f, 0.0f, -100.0f};
					RwFrameTranslate(pstArmourFrame, &v3dTemp, rwCOMBINEREPLACE);
				}

				pstArmourFrame = RpClumpGetFrame(m_pcsArmourClump2);
				if (pstArmourFrame)
				{
					RwV3d v3dTemp = {100.0f, 0.0f, -100.0f};
					RwFrameTranslate(pstArmourFrame, &v3dTemp, rwCOMBINEREPLACE);
				}
			}
		}
	}

	if (CModelToolDlg::GetInstance())
	{
		if (CModelToolDlg::GetInstance()->GetTitleForm())
		{
			CHAR szTitle[AMT_MAX_STR];
			sprintf(szTitle, "%s - [%d]%s   {%s}", AMT_TITLE_NAME_ITEM, pcsAgpdItemTemplate->m_lID, pcsAgpdItemTemplate->m_szName, g_pNationalCodeStr[m_uNationalCode]);
			CModelToolDlg::GetInstance()->GetTitleForm()->SetCurrentTitle(szTitle);
		}
	}

	if ( m_csAgcEngine.GetAgcmEffectDlgModule() && m_csAgcEngine.GetAgcmEventEffectModule() )
	{
		if (m_csAgcEngine.GetAgcmEffectDlgModule()->IsOpenUseEffectSet())
		{
			AgcdUseEffectSet *pstAgcdUseEffectSet = m_csAgcEngine.GetAgcmEventEffectModule()->GetUseEffectSet(pcsAgpdItemTemplate);
			if (!pstAgcdUseEffectSet)
				return FALSE;

			if (!m_csAgcEngine.GetAgcmEffectDlgModule()->InitializeUseEffectSet(pstAgcdUseEffectSet))
				return FALSE;
		}
	}

	if (bResetCamera)
		ResetCamera();

	m_hCurrentTreeItem = hSelected;
	
	return pcsAgpdItem;
}

ApdObject *CModelToolApp::SetObject(INT32 lTID, HTREEITEM hSelected, BOOL bResetCamera, BOOL bReleaseAll)
{
	if ( !m_csAgcEngine.GetApmObjectModule() || !m_csAgcEngine.GetAgcmObjectModule() )
		return NULL;

	RemoveTargetCharacter();

	if (bReleaseAll)
	{
		if (!ReleaseAllObject(FALSE))
			return NULL;
	}

	INT32 lOID = GenerateOID();
	ApdObject *pcsApdObject = m_csAgcEngine.GetApmObjectModule()->AddObject(lOID, lTID);
	if (!pcsApdObject)
		return NULL;

	ApdObjectTemplate *pcsApdObjectTemplate = (ApdObjectTemplate *)(pcsApdObject->m_pcsTemplate);
	if (!pcsApdObjectTemplate)
		return NULL;

	if (!m_csAgcEngine.GetApmObjectModule()->UpdateInit(pcsApdObject))
		return NULL;

	m_eObjectType	= AMT_OBJECT_TYPE_OBJECT;
	m_lID			= lOID;
	m_lTID			= lTID;

	AgcdObject *pcsAgcdObject = m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pcsApdObject);
	if (!pcsAgcdObject)
		return NULL;

	AgcdObjectTemplate *pcsAgcdObjectTemplate = (AgcdObjectTemplate *)(m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(pcsApdObjectTemplate));
	if (!pcsAgcdObjectTemplate)
		return NULL;

	if (CModelToolDlg::GetInstance())
	{
		if (CModelToolDlg::GetInstance()->GetTitleForm())
		{
			CHAR szTitle[AMT_MAX_STR];
			sprintf(szTitle, "%s - [%d]%s   {%s}", AMT_TITLE_NAME_OBJECT, pcsApdObjectTemplate->m_lID, pcsApdObjectTemplate->m_szName, g_pNationalCodeStr[m_uNationalCode]);
			CModelToolDlg::GetInstance()->GetTitleForm()->SetCurrentTitle(szTitle);
		}
	}

	if ( m_csAgcEngine.GetAgcmEffectDlgModule() && m_csAgcEngine.GetAgcmEventEffectModule() )
	{
		if (m_csAgcEngine.GetAgcmEffectDlgModule()->IsOpenUseEffectSet())
		{
			AgcdUseEffectSet *pstAgcdUseEffectSet = m_csAgcEngine.GetAgcmEventEffectModule()->GetUseEffectSet(pcsApdObjectTemplate);
			if (!pstAgcdUseEffectSet)
				return NULL;

			if (!m_csAgcEngine.GetAgcmEffectDlgModule()->InitializeUseEffectSet(pstAgcdUseEffectSet))
				return FALSE;
		}
	}

	//lod 체크..
	if( GetEngine()->GetAgcmObjectModule()->IsValidLOD( pcsApdObjectTemplate, pcsAgcdObjectTemplate ) == false )
	{
		MessageBox( NULL, "LOD 데이타가 이상합니다.", "LOD ERROR", MB_OK );
	}

	if (bResetCamera)
		ResetCamera();

	m_hCurrentTreeItem = hSelected;
	return pcsApdObject;
}

BOOL CModelToolApp::SetSkill(INT32 lTID, HTREEITEM hSelected)
{
	if (	(!m_csAgcEngine.GetAgpmSkillModule()) ||
			(!m_csAgcEngine.GetAgcmSkillModule())		)

	RemoveTargetCharacter();

	if(!ReleaseAllObject())
		return FALSE;

	AgpdSkillTemplate *pcsAgpdSkillTemplate = m_csAgcEngine.GetAgpmSkillModule()->GetSkillTemplate(lTID);
	if(!pcsAgpdSkillTemplate)
		return FALSE;

	AgcdSkillTemplate *pcsAgcdSkillTemplate = m_csAgcEngine.GetAgcmSkillModule()->GetADSkillTemplate((PVOID)(pcsAgpdSkillTemplate));
	if(!pcsAgcdSkillTemplate)
		return FALSE;

	m_eObjectType	= AMT_OBJECT_TYPE_SKILL;
	m_lID			= 0;
	m_lTID			= lTID;

	if(CModelToolDlg::GetInstance())
	{
		if(CModelToolDlg::GetInstance()->GetTitleForm())
		{
			CHAR szTitle[AMT_MAX_STR];
			sprintf(szTitle, "%s - [%d]%s   {%s}", AMT_TITLE_NAME_SKILL, pcsAgpdSkillTemplate->m_lID, pcsAgpdSkillTemplate->m_szName, g_pNationalCodeStr[m_uNationalCode]);
			CModelToolDlg::GetInstance()->GetTitleForm()->SetCurrentTitle(szTitle);
		}
	}
	
	ResetCamera();

	m_hCurrentTreeItem = hSelected;

	return TRUE;
}

BOOL CModelToolApp::SetCharacterDepthCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pData) || (!pClass))
		return FALSE;

	CModelToolApp			*pThis				= (CModelToolApp *)(pClass);
	float					*pfDepth			= (float *)(pData);

	AgpdCharacter			*pcsAgpdCharacter	= pThis->m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(pThis->m_lID);
	if(!pcsAgpdCharacter)
		return FALSE;
	AgcdCharacter			*pcsAgcdCharacter	= pThis->m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
	if((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump))
		return FALSE;

	*pfDepth = pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_fDepth;

	return TRUE;
}


BOOL CModelToolApp::SetCharacterHeightCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pData) || (!pClass))
		return FALSE;

	CModelToolApp			*pThis				= (CModelToolApp *)(pClass);
	INT32					*plHeight			= (INT32 *)(pData);

	AgpdCharacter			*pcsAgpdCharacter	= pThis->m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(pThis->m_lID);
	if(!pcsAgpdCharacter)
		return FALSE;
	AgcdCharacter			*pcsAgcdCharacter	= pThis->m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
	if((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump))
		return FALSE;

	RpClump* pstClump = pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_pDefaultArmourClump;
	if( !pstClump )
		pstClump = pcsAgcdCharacter->m_pClump;

	INT32	lHeight = 1;
	if( pcsAgcdCharacter->m_pRide )
	{
		RpHAnimHierarchy* pInHierarchy = pcsAgcdCharacter->m_pInHierarchy;
		if( !pInHierarchy )
			return FALSE;

		lHeight = RwUtilClumpGetRideHeight( pstClump, pInHierarchy );
	}
	else
		lHeight = RwUtilClumpGetHeight(pstClump);

	if (pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale != 1.0f)
		lHeight = (INT32)((FLOAT)(lHeight) * pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale);

	*plHeight = lHeight;

	return TRUE;
}

BOOL CModelToolApp::SetCharacterHeight(AgcdCharacter *pstCharacter)
{
	INT32 lHeight = RwUtilClumpGetHeight(pstCharacter->m_pClump);
	if (pstCharacter->m_pstAgcdCharacterTemplate->m_fScale != 1.0f)
		lHeight = (INT32)((FLOAT)(lHeight) * pstCharacter->m_pstAgcdCharacterTemplate->m_fScale);

	pstCharacter->m_pstAgcdCharacterTemplate->m_lHeight = lHeight;

	return TRUE;
}

BOOL CModelToolApp::GenerateAllCharacterTemplateHeight()
{
	AgpdCharacterTemplate	*ppTemplate;

	AgpdCharacter			*ppCharacter;
	AgcdCharacter			*pcCharacter;

	CHAR					szGameID[AMT_MAX_STR];
	INT32					lCID;
	INT32					lIndex = 0;	

	ReleaseAllObject();

	for (	ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex)	)
	{
		lCID	= GenerateCID();
		sprintf(szGameID, "ArchLordModelTool%d", lCID);

		ppCharacter = m_csAgcEngine.GetAgpmCharacterModule()->AddCharacter(lCID, ppTemplate->m_lID, szGameID);
		if(!ppCharacter)
			return FALSE;

		if(!m_csAgcEngine.GetAgpmCharacterModule()->UpdateInit(ppCharacter))
			return FALSE;

		pcCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(ppCharacter);
		if((!pcCharacter) || (!pcCharacter->m_pClump))
			return FALSE;

		SetCharacterHeight(pcCharacter);

		ReleaseAllObject();
	}

	SetEmpty();

	return TRUE;
}

BOOL CModelToolApp::SetCharacterHitRangeCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pData) || (!pClass))
		return FALSE;

	CModelToolApp			*pThis				= (CModelToolApp *)(pClass);
	INT32					*plHitRange			= (INT32 *)(pData);

	AgpdCharacter			*pcsAgpdCharacter	= pThis->m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(pThis->m_lID);
	if(!pcsAgpdCharacter)
		return FALSE;
	AgcdCharacter			*pcsAgcdCharacter	= pThis->m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
	if((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump))
		return FALSE;

//	*(plHitRange) = (INT32)((RwUtilClumpGetWidth(pcsAgcdCharacter->m_pClump) / 2) + AMT_CHARACTER_HIT_RANGE_OFFSET);
	INT32 lHitRange = pThis->GetCharacterHitRange(pcsAgcdCharacter->m_pClump);
	if (pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale != 1.0f)
		lHitRange = (INT32)((FLOAT)(lHitRange) * pcsAgcdCharacter->m_pstAgcdCharacterTemplate->m_fScale);

	 *(plHitRange) = lHitRange;

	return TRUE;
}

INT32 CModelToolApp::GetCharacterHitRange(RpClump *pstClump)
{
	return (INT32)((RwUtilClumpGetWidth(pstClump) / 3) + AMT_CHARACTER_HIT_RANGE_OFFSET);
}

BOOL CModelToolApp::GenerateAllCharacterTemplateHitRange(BOOL bGenHeight)
{
	AgpdCharacterTemplate	*ppTemplate;

	AgpdCharacter			*ppCharacter;
	AgcdCharacter			*pcCharacter;

	CHAR					szGameID[AMT_MAX_STR];
	INT32					lCID;
	INT32					lIndex = 0;	
	INT32					lHitRange;

	ReleaseAllObject();

	for (	ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex)	)
	{
		lCID	= GenerateCID();
		sprintf(szGameID, "ArchLordModelTool%d", lCID);

		ppCharacter = m_csAgcEngine.GetAgpmCharacterModule()->AddCharacter(lCID, ppTemplate->m_lID, szGameID);
		if (!ppCharacter)
			return FALSE;

		if (!m_csAgcEngine.GetAgpmCharacterModule()->UpdateInit(ppCharacter))
			return FALSE;

		pcCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(ppCharacter);
		if ((!pcCharacter) || (!pcCharacter->m_pClump))
			return FALSE;

		lHitRange	= GetCharacterHitRange(pcCharacter->m_pClump);
		if (pcCharacter->m_pstAgcdCharacterTemplate->m_fScale != 1.0f)
			lHitRange	= (INT32)((FLOAT)(lHitRange) * (pcCharacter->m_pstAgcdCharacterTemplate->m_fScale));

		if (!m_csAgcEngine.GetAgpmFactorsModule()->SetFactor(&ppTemplate->m_csFactor, NULL, AGPD_FACTORS_TYPE_ATTACK))
			return FALSE;

		if (!m_csAgcEngine.GetAgpmFactorsModule()->SetValue(&ppTemplate->m_csFactor, lHitRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HITRANGE))
			return FALSE;

		if (bGenHeight)
			SetCharacterHeight(pcCharacter);

		ReleaseAllObject();
	}
	
	SetEmpty();

	return TRUE;
}

BOOL CModelToolApp::GenerateAllCharacterTemplateBSphere()
{
	AgpdCharacterTemplate	*ppTemplate;

	AgpdCharacter			*ppCharacter;
	AgcdCharacter			*pcCharacter;

	CHAR					szGameID[AMT_MAX_STR];
	INT32					lCID;
	INT32					lIndex = 0;	

	ReleaseAllObject();

	//@{ 2006/05/15 burumal
	char pMsgBuff[256];
	int nCurCount = 0;
	int nTotalCount = m_csAgcEngine.GetAgpmCharacterModule()->m_csACharacterTemplate.GetObjectCount();
	//@}

	for (	ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex)	)
	{
		nCurCount++;
		sprintf(pMsgBuff,"%s\n[%d / %d]  %s", AMT_START_UP_GENERATE_BSPHERE, nCurCount, nTotalCount, ppTemplate->m_szTName);
		m_pcsStingDlg->SetString(pMsgBuff);
		
		lCID	= GenerateCID();
		sprintf(szGameID, "ArchLordModelTool%d", lCID);

		ppCharacter = m_csAgcEngine.GetAgpmCharacterModule()->AddCharacter(lCID, ppTemplate->m_lID, szGameID);
		if(!ppCharacter)
			return FALSE;

		if(!m_csAgcEngine.GetAgpmCharacterModule()->UpdateInit(ppCharacter))
			return FALSE;

		pcCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(ppCharacter);
		if((!pcCharacter) || (!pcCharacter->m_pClump) || (!pcCharacter->m_pstAgcdCharacterTemplate))
			return FALSE;
		
		//@{ 2006/05/16 burumal
		//RwUtilClumpGetBoundingSphere(pcCharacter->m_pClump, &pcCharacter->m_pstAgcdCharacterTemplate->m_stBSphere);
		if ( pcCharacter->m_pstAgcdCharacterTemplate->m_stBSphere.radius <= 0 )
			RwUtilClumpGetBoundingSphere(pcCharacter->m_pClump, &pcCharacter->m_pstAgcdCharacterTemplate->m_stBSphere);
		//@}

		ReleaseAllObject();
	}
	
	SetEmpty();

	return TRUE;
}

BOOL CModelToolApp::GenerateAllItemTemplateBSphere()
{
	AgpdItemTemplate	*ppTemplate;

	AgpdItem			*ppItem;
	AgcdItem			*pcItem;

	INT32				lIndex = 0;	

	ReleaseAllObject();

	//@{ 2006/05/15 burumal
	char pMsgBuff[256];
	int nCurCount = 0;
	int nTotalCount = m_csAgcEngine.GetAgpmItemModule()->csTemplateAdmin.GetObjectCount();
	//@}

	CString	str;

	for (	ppTemplate = m_csAgcEngine.GetAgpmItemModule()->GetTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetAgpmItemModule()->GetTemplateSequence(&lIndex)	)
	{
		//@{ 2006/05/15 burumal
		nCurCount++;
		sprintf(pMsgBuff,"%s\n[%d / %d]  %s", AMT_START_UP_GENERATE_BSPHERE, nCurCount, nTotalCount, ppTemplate->m_szName);
		m_pcsStingDlg->SetString(pMsgBuff);
		//@}
		str.Format( "%s(%d) 아이템 바운딩 스피어 생성 오류" , ppTemplate->m_szName , ppTemplate->m_lID );

		ppItem = m_csAgcEngine.GetAgpmItemModule()->AddItem(GenerateIID(), ppTemplate->m_lID, 0);
		if(!ppItem)
		{
			::MessageBox( NULL , str , "ModelTool" , MB_OK );
			return FALSE;
		}

		if(!m_csAgcEngine.GetAgpmItemModule()->InitItem(ppItem))
		{
			::MessageBox( NULL , str , "ModelTool" , MB_OK );
			return FALSE;
		}

		if(!m_csAgcEngine.GetAgpmItemModule()->AddItemToField(ppItem))
		{
			::MessageBox( NULL , str , "ModelTool" , MB_OK );
			return FALSE;
		}

		pcItem = m_csAgcEngine.GetAgcmItemModule()->GetItemData(ppItem);
		if((!pcItem) || (!pcItem->m_pstClump) || (!pcItem->m_pstAgcdItemTemplate))
		{
			::MessageBox( NULL , str , "ModelTool" , MB_OK );
			return FALSE;
		}

		//@{ 2006/05/16 burumal
		//RwUtilClumpGetBoundingSphere(pcItem->m_pstClump, &pcItem->m_pstAgcdItemTemplate->m_stBSphere);
		if ( pcItem->m_pstAgcdItemTemplate->m_stBSphere.radius <= 0 )
			RwUtilClumpGetBoundingSphere(pcItem->m_pstClump, &pcItem->m_pstAgcdItemTemplate->m_stBSphere);
		//@}

		ReleaseAllObject();
	}

	SetEmpty();

	return TRUE;
}

BOOL CModelToolApp::GenerateAllObjectTemplateBSphere()
{
	ApdObjectTemplate			*ppTemplate;
	AgcdObjectTemplate			*pcTemplate;

	AgcdObjectTemplateGroupList	*pstList;

	INT32						lIndex = 0;

	CHAR						szMessage[AMT_MAX_STR];

	ReleaseAllObject();

	//@{ 2006/05/15 burumal
	char pMsgBuff[256];
	int nCurCount = 0;
	int nTotalCount = m_csAgcEngine.GetApmObjectModule()->m_clObjectTemplates.GetObjectCount();
	//@}

	for (	ppTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex)	)
	{
		//@{ 2006/05/15 burumal
		nCurCount++;
		sprintf(pMsgBuff,"%s\n[%d / %d]  %s", AMT_START_UP_GENERATE_BSPHERE, nCurCount, nTotalCount, ppTemplate->m_szName);
		m_pcsStingDlg->SetString(pMsgBuff);
		//@}

		pcTemplate = m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(ppTemplate);
		if(!pcTemplate)
			return FALSE;

		pstList = pcTemplate->m_stGroup.m_pstList;
		while(pstList)
		{
			if(!pstList->m_csData.m_pstClump)
			{
				//pstList->m_csData.m_pstClump = m_csAgcEngine.GetAgcmObjectModule()->LoadClump(pstList->m_csData.m_szDFFName);
				pstList->m_csData.m_pstClump =
					m_csAgcEngine.GetAgcmObjectModule()->LoadClump(pstList->m_csData.m_pszDFFName);
				if(!pstList->m_csData.m_pstClump)
				{
					sprintf(szMessage, "%s(%d)\n를 읽어드릴수 없습니다!", pstList->m_csData.m_pszDFFName, ppTemplate->m_lID);
					CModelToolDlg::GetInstance()->MessageBox(szMessage, "BSphere 구하기 실패!", MB_OK);
					return FALSE;
				}
			}

			//@{ 2006/05/16 burumal
			//RwUtilClumpGetBoundingSphere(pstList->m_csData.m_pstClump, &pstList->m_csData.m_stBSphere);
			if ( pstList->m_csData.m_stBSphere.radius <= 0 )
				RwUtilClumpGetBoundingSphere(pstList->m_csData.m_pstClump, &pstList->m_csData.m_stBSphere);
			//@}

			RpClumpDestroy(pstList->m_csData.m_pstClump);
			pstList->m_csData.m_pstClump = NULL;

			pstList = pstList->m_pstNext;
		}
	}

	ReleaseAllObject();
	SetEmpty();

	return TRUE;
}

BOOL CModelToolApp::GenerateBoundingSphereCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if ((!pClass) || (!pData))
		return FALSE;

	CModelToolApp	*pcsThis	= (CModelToolApp *)(pClass);
	RwSphere		*pcsDest	= (RwSphere *)(pData);

	FLOAT			fScale		= 1.0f;
	RpClump			*pstClump	= NULL;
	RpAtomic		*pPickAtom	= NULL;
	
	switch (pcsThis->m_eObjectType)
	{
	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacter* ppObj = pcsThis->GetEngine()->GetAgpmCharacterModule()->GetCharacter(pcsThis->m_lID);
			if (!ppObj)		return FALSE;

			AgcdCharacter* pcObj = pcsThis->GetEngine()->GetAgcmCharacterModule()->GetCharacterData(ppObj);
			if (!pcObj)		return FALSE;

			pPickAtom	= pcObj->m_pPickAtomic;
			pstClump	= pcObj->m_pClump;
			fScale		= pcObj->m_pstAgcdCharacterTemplate->m_fScale;
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItem* ppObj = pcsThis->GetEngine()->GetAgpmItemModule()->GetItem(pcsThis->m_lID);
			if (!ppObj)		return FALSE;

			AgcdItem* pcObj = pcsThis->GetEngine()->GetAgcmItemModule()->GetItemData(ppObj);
			if (!pcObj)		return FALSE;

			pPickAtom	= pcObj->m_pstPickAtomic;
			pstClump	= pcObj->m_pstClump;
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			ApdObject* ppObj = pcsThis->GetEngine()->GetApmObjectModule()->GetObject(pcsThis->m_lID);
			if (!ppObj)		return FALSE;
				
			AgcdObject* pcObj = pcsThis->GetEngine()->GetAgcmObjectModule()->GetObjectData(ppObj);
			if (!pcObj)		return FALSE;

			INT32 lGroupIndex = (INT32)(pCustData);
			AgcdObjectGroupData* pcsData = pcsThis->GetEngine()->GetAgcmObjectModule()->GetObjectList()->GetObjectGroup(&pcObj->m_stGroup, lGroupIndex);
			if (!pcsData)	return FALSE;
			
			//pPickAtom	= pcObj->m_pstPickAtomic;
			pstClump	= pcsData->m_pstClump;
		}
		break;
	}

	if( pPickAtom )
	{
		RpAtomic* pEndPickAtomic = pPickAtom;
		INT32 nBSphereCount = 0;
		RwSphere EachBSphere;

		pcsDest->center.x = pcsDest->center.y = pcsDest->center.z = 0;
		pcsDest->radius = 0;

		do {
			RwUtilCalcSphere( pPickAtom, &EachBSphere );

			pcsDest->center.x += EachBSphere.center.x;
			pcsDest->center.y += EachBSphere.center.y;
			pcsDest->center.z += EachBSphere.center.z;

			nBSphereCount++;
			
			if( !pPickAtom->next )
				break;

			pPickAtom = pPickAtom->next;
		} while ( pPickAtom != pEndPickAtomic );
		
		pcsDest->center.x /= nBSphereCount;
		pcsDest->center.y /= nBSphereCount;
		pcsDest->center.z /= nBSphereCount;

		// calc radius value		
		do {

			// 이렇게 하면 간단하게 구할수 있지만 정확하지 않다
//			RwUtilCalcSphere( pPickAtom, &EachBSphere);
//			float fDist = AUPOS_DISTANCE(pcsDest->center, EachBSphere.center);
//			fDist += EachBSphere.radius;
//			if ( fDist > pcsDest->radius )
//				pcsDest->radius = fDist;
			
			RpGeometry* pGeometry = RpAtomicGetGeometry(pPickAtom);
			if ( pGeometry )
			{
				INT32 nMorphTargetCount = RpGeometryGetNumMorphTargets(pGeometry);
				INT32 nVertCount = RpGeometryGetNumVertices(pGeometry);
				
				RwMatrix* pTransformMat = RwFrameGetLTM(RpAtomicGetFrame(pPickAtom));
				ASSERT( pTransformMat );
				
				RwV3d vGeoMax, vGeoMin;

				RwV3d* pVerts = NULL;
				RpMorphTarget* pMorphTarget = RpGeometryGetMorphTarget(pGeometry, 0);
				if ( pMorphTarget )
					pVerts = RpMorphTargetGetVertices(pMorphTarget);

				if ( pMorphTarget && pVerts )
				{					
					{
						RwV3d vTransformed;
						RwV3dTransformPoint(&vTransformed, &(pVerts[0]), pTransformMat);

						vGeoMax.x = vGeoMin.x = vTransformed.x;
						vGeoMax.y = vGeoMin.y = vTransformed.y;
						vGeoMax.z = vGeoMin.z = vTransformed.z;
					}

					for ( INT32 nIdx = 0; nIdx < nMorphTargetCount; nIdx++ )
					{
						pMorphTarget = RpGeometryGetMorphTarget(pGeometry, nIdx);
						if ( !pMorphTarget )
							break;

						pVerts = RpMorphTargetGetVertices(pMorphTarget);
						if ( !pVerts )
							break;

						RwV3d vMax, vMin;

						{
							RwV3d vTransformed;
							RwV3dTransformPoint(&vTransformed, &(pVerts[0]), pTransformMat);
						
							vMax.x = vMin.x = vTransformed.x;
							vMax.y = vMin.y = vTransformed.y;
							vMax.z = vMin.z = vTransformed.z;
						}

						for ( INT32 nVert = 0; nVert < nVertCount; nVert++ )
						{
							{
								RwV3d vTransformed;
								RwV3dTransformPoint(&vTransformed, &(pVerts[nVert]), pTransformMat);

								// max
								if ( vMax.x < vTransformed.x )
									vMax.x = vTransformed.x;

								if ( vMax.y < vTransformed.y )
									vMax.y = vTransformed.y;

								if ( vMax.z < vTransformed.z )
									vMax.z = vTransformed.z;

								// min
								if ( vMin.x > vTransformed.x )
									vMin.x = vTransformed.x;

								if ( vMin.y > vTransformed.y )
									vMin.y = vTransformed.y;

								if ( vMin.z > vTransformed.z )
									vMin.z = vTransformed.z;
							}
						}

						// max
						if ( vGeoMax.x < vMax.x )
							vGeoMax.x = vMax.x;

						if ( vGeoMax.y < vMax.y )
							vGeoMax.y = vMax.y;

						if ( vGeoMax.z < vMax.z )
							vGeoMax.z = vMax.z;

						// min
						if ( vGeoMin.x > vMin.x )
							vGeoMin.x = vMin.x;

						if ( vGeoMin.y > vMin.y )
							vGeoMin.y = vMin.y;

						if ( vGeoMin.z > vMin.z )
							vGeoMin.z = vMin.z;
					}

					float fMaxDist = AUPOS_DISTANCE(pcsDest->center, vGeoMax);
					float fMinDist = AUPOS_DISTANCE(pcsDest->center, vGeoMin);

					if ( fMaxDist > fMinDist )
					{
						if ( pcsDest->radius < fMaxDist )
							pcsDest->radius = fMaxDist;
					}
					else
					{
						if ( pcsDest->radius < fMinDist )
							pcsDest->radius = fMinDist;
					}
				}
			}
			
			if( !pPickAtom->next )
				break;

			pPickAtom = pPickAtom->next;
		} while ( pPickAtom != pEndPickAtomic );
		
		RwMatrix invsLTM;
		RwMatrixInvert( &invsLTM, RwFrameGetLTM( RpAtomicGetFrame( pPickAtom) ) );
		RwV3dTransformPoint( &pcsDest->center, &pcsDest->center, &invsLTM );
	}
	else
	{
		if ( pstClump )
		{
			RwUtilCalcSphere( pstClump, pcsDest );

			RwMatrix	invsLTM;
			RwMatrixInvert( &invsLTM, RwFrameGetLTM( RpClumpGetFrame( pstClump ) ) );
			RwV3dTransformPoint( &pcsDest->center, &pcsDest->center, &invsLTM );
		}
	}

	return TRUE;
}

BOOL CModelToolApp::ReloadObjectCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CModelToolApp	*pThis			= (CModelToolApp *)(pClass);
	return pThis->ApplyObject();
}

BOOL CModelToolApp::ApplyObject()
{
	m_pcsPickedAtomic	= NULL;

	switch(m_eObjectType)
	{
	case AMT_OBJECT_TYPE_CHARACTER:
		{			
			if (!ReleaseCharacter())
				return FALSE;

			if (!SetCharacter(m_lTID, m_hCurrentTreeItem, FALSE, FALSE))
				return FALSE;
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			if (!ReleaseItem())
				return FALSE;

			if (!SetItem(m_lTID, m_hCurrentTreeItem, FALSE, FALSE))
				return FALSE;
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			if (!ReleaseObject())
				return FALSE;

			if (!SetObject(m_lTID, m_hCurrentTreeItem, FALSE, FALSE))
				return FALSE;
		}
		break;

	case AMT_OBJECT_TYPE_SKILL:
		{
			if (!ReleaseObject())
				return FALSE;

			if (!SetSkill(m_lTID, m_hCurrentTreeItem))
				return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL CModelToolApp::CalcObjtTemplateOctreeData()
{
	ApdObjectTemplate			*ppTemplate;
	AgcdObjectTemplate			*pcTemplate;

	AgcdGeometryDataLink		*pcLink;
	AgcdGeometryData			*pcList;

	INT32						lIndex = 0;
	INT32						lCount;

	CHAR						szMessage[AMT_MAX_STR];

	FILE						*pOutFile = fopen("OCTREE_ERROR.TXT", "wt");
	if (!pOutFile)
		return FALSE;

	ReleaseAllObject();

	for (	ppTemplate	= m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex);
			ppTemplate;
			ppTemplate	= m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex)	)
	{
		pcTemplate		= m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(ppTemplate);
		if (!pcTemplate)
		{
			sprintf(szMessage, "%s의 template을 가져올 수 없습니다.", ppTemplate->m_szName);
			CModelToolDlg::GetInstance()->MessageBox(szMessage, "ERROR!");

			fclose(pOutFile);
			return FALSE;
		}


		// 최대 BBOX
		{
			pcTemplate->m_lRefCount = 1;
			m_csAgcEngine.GetAgcmObjectModule()->LoadTemplateData(pcTemplate, FALSE);

			RwBBox						stBox;
			INT32						lClumpNum = 0;
			RpClump						*pastClump[10];
			memset(pastClump, 0, sizeof(RpClump *) * 10);
			AgcdObjectTemplateGroupList	*pstList	= pcTemplate->m_stGroup.m_pstList;
			while (pstList)
			{
				if (pstList->m_csData.m_pstClump)
				{
					pastClump[lClumpNum++]			= pstList->m_csData.m_pstClump;
				}

				pstList								= pstList->m_pstNext;
			}

			if (lClumpNum > 0)
			{
				RwUtilClumpGetBoundingBox(pastClump, lClumpNum, &stBox);

				pcTemplate->m_stOcTreeData.topVerts_MAX[0].x	= stBox.inf.x;
				pcTemplate->m_stOcTreeData.topVerts_MAX[0].y	= stBox.sup.y;
				pcTemplate->m_stOcTreeData.topVerts_MAX[0].z	= stBox.inf.z;
				pcTemplate->m_stOcTreeData.topVerts_MAX[1].x	= stBox.inf.x;
				pcTemplate->m_stOcTreeData.topVerts_MAX[1].y	= stBox.sup.y;
				pcTemplate->m_stOcTreeData.topVerts_MAX[1].z	= stBox.sup.z;
				pcTemplate->m_stOcTreeData.topVerts_MAX[2].x	= stBox.sup.x;
				pcTemplate->m_stOcTreeData.topVerts_MAX[2].y	= stBox.sup.y;
				pcTemplate->m_stOcTreeData.topVerts_MAX[2].z	= stBox.inf.z;
				pcTemplate->m_stOcTreeData.topVerts_MAX[3].x	= stBox.sup.x;
				pcTemplate->m_stOcTreeData.topVerts_MAX[3].y	= stBox.sup.y;
				pcTemplate->m_stOcTreeData.topVerts_MAX[3].z	= stBox.sup.z;
			}

			m_csAgcEngine.GetAgcmObjectModule()->ReleaseObjectTemplateData(pcTemplate);
		}

		if (pcTemplate->m_lObjectType & ACUOBJECT_TYPE_OCCLUDER)
		{
			pcLink		= m_csAgcEngine.GetAgcmGeometryDataManagerModule()->GetGeometryDataLink(ppTemplate, AMT_GEOM_DATA_OCTREE);
			if (!pcLink)
			{
				sprintf(szMessage, "%s의 Link가 없습니다.", ppTemplate->m_szName);
				CModelToolDlg::GetInstance()->MessageBox(szMessage, "ERROR!");

				fclose(pOutFile);
				return FALSE;
			}

			pcList		= pcLink->m_pcsHead;
			lCount		= 0;
			while (pcList)
			{
				if (pcList->m_stGeom.type == AUBLOCKING_TYPE_BOX)
					++lCount;

				pcList	= pcList->m_pcsNext;
			}

			if (lCount > APDEFINE_MAX_OCCLUDER_BOX)
			{
				fprintf(pOutFile, "%s의 옥트리 데이터의 최대갯수를 초과했습니다.\n", ppTemplate->m_szName);
				pcTemplate->m_stOcTreeData.nOccluderBox = APDEFINE_MAX_OCCLUDER_BOX;
				continue;
			}

			pcTemplate->m_stOcTreeData.nOccluderBox = lCount;

			if (lCount < 1)
			{
				fprintf(pOutFile, "%s의 옥트리 데이터가 없습니다.\n", ppTemplate->m_szName);
				continue;
			}

			if ( pcTemplate->m_stOcTreeData.pTopVerts )
				delete [] pcTemplate->m_stOcTreeData.pTopVerts;

			pcTemplate->m_stOcTreeData.pTopVerts = new RwV3d[ pcTemplate->m_stOcTreeData.nOccluderBox * 4 ];

			pcList		= pcLink->m_pcsHead;
			lCount		= 0;
			while (pcList)
			{
				if (pcList->m_stGeom.type == AUBLOCKING_TYPE_BOX)
				{
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4].x	= pcList->m_stGeom.data.box.inf.x;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4].y	= pcList->m_stGeom.data.box.sup.y;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4].z	= pcList->m_stGeom.data.box.inf.z;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4+1].x	= pcList->m_stGeom.data.box.inf.x;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4+1].y	= pcList->m_stGeom.data.box.sup.y;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4+1].z	= pcList->m_stGeom.data.box.sup.z;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4+2].x	= pcList->m_stGeom.data.box.sup.x;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4+2].y	= pcList->m_stGeom.data.box.sup.y;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4+2].z	= pcList->m_stGeom.data.box.inf.z;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4+3].x	= pcList->m_stGeom.data.box.sup.x;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4+3].y	= pcList->m_stGeom.data.box.sup.y;
					pcTemplate->m_stOcTreeData.pTopVerts[lCount*4+3].z	= pcList->m_stGeom.data.box.sup.z;

					++lCount;
				}

				pcList	= pcList->m_pcsNext;
			}
		}
	}

	ReleaseAllObject();
	SetEmpty();

	fclose(pOutFile);

	return TRUE;
}

BOOL CModelToolApp::SetCharacterLabel(INT32 lTID, CHAR *szLabel)
{
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate	= GetEngine()->GetAgpmCharacterModule()->GetCharacterTemplate(lTID);
	if (!pcsAgpdCharacterTemplate)
		return FALSE;

	AgcdCharacterTemplate	*pcsAgcdCharacterTemplate	= GetEngine()->GetAgcmCharacterModule()->GetTemplateData(pcsAgpdCharacterTemplate);
	if (!pcsAgcdCharacterTemplate)
		return FALSE;

	if (!pcsAgcdCharacterTemplate->m_pcsPreData)
		return FALSE;

	if (!pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel)
		pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel = new CHAR [strlen(szLabel) + 1];
			
	if (!pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel)
		return FALSE;

	strcpy( pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel, szLabel	);

	return TRUE;
}

BOOL CModelToolApp::SetObjectLabel(INT32 lTID, CHAR *szLabel)
{
	ApdObjectTemplate	*pcsApdObjectTemplate	= GetEngine()->GetApmObjectModule()->GetObjectTemplate(lTID);
	if (!pcsApdObjectTemplate)		return FALSE;

	AgcdObjectTemplate	*pcsAgcdObjectTemplate	= GetEngine()->GetAgcmObjectModule()->GetTemplateData(pcsApdObjectTemplate);
	if (!pcsAgcdObjectTemplate)		return FALSE;

	strcpy( pcsAgcdObjectTemplate->m_szCategory, szLabel );

	return TRUE;
}

void CModelToolApp::SetCharacterTemplateDNF(INT32 nTID, BOOL bDNF)
{
	AgpdCharacterTemplate* pcsAgpdCharacterTemplate	= GetEngine()->GetAgpmCharacterModule()->GetCharacterTemplate(nTID);
	if (!pcsAgpdCharacterTemplate)	return;

	AgcdCharacterTemplate* pcsAgcdCharacterTemplate	= GetEngine()->GetAgcmCharacterModule()->GetTemplateData(pcsAgpdCharacterTemplate);
	if (!pcsAgcdCharacterTemplate)	return;

	if ( bDNF )
		pcsAgcdCharacterTemplate->m_nDNF |= GETSERVICEAREAFLAG(m_uNationalCode);
	else
		pcsAgcdCharacterTemplate->m_nDNF &= (~GETSERVICEAREAFLAG( m_uNationalCode) );
}

void CModelToolApp::SetObjectTemplateDNF(INT32 nTID, BOOL bDNF)
{
	ApdObjectTemplate* pcsApdObjectTemplate = GetEngine()->GetApmObjectModule()->GetObjectTemplate(nTID);
	if ( !pcsApdObjectTemplate )	return;

	AgcdObjectTemplate* pcsAgcdObjectTemplate = (AgcdObjectTemplate*)(GetEngine()->GetAgcmObjectModule()->GetTemplateData(pcsApdObjectTemplate));
	if ( !pcsAgcdObjectTemplate )	return;
		
	if ( bDNF )
		pcsAgcdObjectTemplate->m_nDNF |= GETSERVICEAREAFLAG(m_uNationalCode);
	else
		pcsAgcdObjectTemplate->m_nDNF &= (~GETSERVICEAREAFLAG( m_uNationalCode) );
}

void CModelToolApp::SetItemTemplateDNF(INT32 nTID, BOOL bDNF)
{
	AgpdItemTemplate* pcsAgpdItemTemplate = GetEngine()->GetAgpmItemModule()->GetItemTemplate(nTID);
	if ( !pcsAgpdItemTemplate )		return;

	AgcdItemTemplate* pcsAgcdItemTemplate = GetEngine()->GetAgcmItemModule()->GetTemplateData(pcsAgpdItemTemplate);
	if (!pcsAgcdItemTemplate)		return;

	if ( bDNF )
		pcsAgcdItemTemplate->m_nDNF |= GETSERVICEAREAFLAG(m_uNationalCode);
	else
		pcsAgcdItemTemplate->m_nDNF &= (~GETSERVICEAREAFLAG( m_uNationalCode) );
}

void CModelToolApp::SetSkillTemplateDNF(INT32 nTID, BOOL bDNF)
{
	AgpdSkillTemplate* pcsAgpdSkillTemplate = GetEngine()->GetAgpmSkillModule()->GetSkillTemplate(nTID);
	if ( !pcsAgpdSkillTemplate )	return;

	AgcdSkillTemplate* pcsAgcdSkillTemplate = GetEngine()->GetAgcmSkillModule()->GetADSkillTemplate(pcsAgpdSkillTemplate);
	if(!pcsAgcdSkillTemplate)		return;

	if ( bDNF )
		pcsAgcdSkillTemplate->m_nDNF |= GETSERVICEAREAFLAG(m_uNationalCode);
	else
		pcsAgcdSkillTemplate->m_nDNF &= (~GETSERVICEAREAFLAG( m_uNationalCode) );
}

BOOL CModelToolApp::GetCharacterTemplateDNF(INT32 nTID, INT32* pnDNF)
{
	AgpdCharacterTemplate* pcsAgpdCharacterTemplate	= GetEngine()->GetAgpmCharacterModule()->GetCharacterTemplate(nTID);
	if (!pcsAgpdCharacterTemplate)		return FALSE;

	AgcdCharacterTemplate* pcsAgcdCharacterTemplate	= GetEngine()->GetAgcmCharacterModule()->GetTemplateData(pcsAgpdCharacterTemplate);
	if (!pcsAgcdCharacterTemplate)		return FALSE;
	
	*pnDNF = pcsAgcdCharacterTemplate->m_nDNF;
	return TRUE;
}

BOOL CModelToolApp::GetObjectTemplateDNF(INT32 nTID, INT32* pnDNF)
{
	ApdObjectTemplate* pcsApdObjectTemplate = GetEngine()->GetApmObjectModule()->GetObjectTemplate(nTID);
	if ( !pcsApdObjectTemplate )
		return FALSE;

	AgcdObjectTemplate* pcsAgcdObjectTemplate = (AgcdObjectTemplate*)(GetEngine()->GetAgcmObjectModule()->GetTemplateData(pcsApdObjectTemplate));
	if ( !pcsAgcdObjectTemplate )
		return FALSE;

	*pnDNF = pcsAgcdObjectTemplate->m_nDNF;
	return TRUE;
}

BOOL CModelToolApp::GetItemTemplateDNF(INT32 nTID, INT32* pnDNF)
{
	AgpdItemTemplate* pcsAgpdItemTemplate = GetEngine()->GetAgpmItemModule()->GetItemTemplate(nTID);
	if ( !pcsAgpdItemTemplate )
		return FALSE;

	AgcdItemTemplate* pcsAgcdItemTemplate = GetEngine()->GetAgcmItemModule()->GetTemplateData(pcsAgpdItemTemplate);
	if (!pcsAgcdItemTemplate)
		return NULL;

	*pnDNF = pcsAgcdItemTemplate->m_nDNF;
	return TRUE;
}

BOOL CModelToolApp::GetSkillTemplateDNF(INT32 nTID, INT32* pnDNF)
{
	AgpdSkillTemplate* pcsAgpdSkillTemplate = GetEngine()->GetAgpmSkillModule()->GetSkillTemplate(nTID);
	if ( !pcsAgpdSkillTemplate )
		return FALSE;

	AgcdSkillTemplate* pcsAgcdSkillTemplate = GetEngine()->GetAgcmSkillModule()->GetADSkillTemplate(pcsAgpdSkillTemplate);
	if(!pcsAgcdSkillTemplate)
		return FALSE;

	*pnDNF = pcsAgcdSkillTemplate->m_nDNF;
	return TRUE;
}
