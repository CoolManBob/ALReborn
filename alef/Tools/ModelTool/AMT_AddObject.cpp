#include "stdafx.h"
#include "ModelTool.h"
#include "EditDlg.h"

AgpdCharacterTemplate *CModelToolApp::AddCharacterTemplate(AgpdCharacterTemplate *pcsAgpdCharacterTemplate, 
														   AgcdCharacterTemplate *pcsAgcdCharacterTemplate, DWORD dwID)
{
	//ID처리...
	INT32 lTID = GenerateCTID( dwID );
	if( !lTID )
	{
		MessageBox( NULL, "이미 ID가 존재합니다.", "Error", MB_OK );
		return NULL;
	}
	
	AgpdCharacterTemplate* pcsNewAgpdTemplate = m_csAgcEngine.GetAgpmCharacterModule()->AddCharacterTemplate( lTID );
	if(!pcsNewAgpdTemplate)		return NULL;

	AgcdCharacterTemplate *pcsNewAgcdTemplate = m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(pcsNewAgpdTemplate);
	if(!pcsNewAgcdTemplate)		return NULL;

	TSO::SetDefaultSequenceNumber( pcsNewAgpdTemplate );
	m_csAgcEngine.SetSaveData( pcsNewAgpdTemplate );

	if( !pcsNewAgcdTemplate->m_pcsPreData || !pcsAgcdCharacterTemplate->m_pcsPreData )
		return NULL;

	pcsNewAgcdTemplate->m_lHeight = pcsAgcdCharacterTemplate->m_lHeight;
	strcpy( pcsNewAgpdTemplate->m_szTName, pcsAgpdCharacterTemplate->m_szTName );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pcsPreData->m_pszDFFName, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pcsPreData->m_pszDADFFName, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pcsPreData->m_pszPickDFFName, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pcsPreData->m_pszLabel, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pcsPreData->m_pszDFFName, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName );
	
	AgcdUseEffectSet *pstAgcdUseEffectSet = m_csAgcEngine.GetAgcmEventEffectModule()->GetUseEffectSet(pcsNewAgpdTemplate);
	if(!pstAgcdUseEffectSet)	return NULL;

	HTREEITEM hParent	= NULL;

	if (pcsNewAgcdTemplate->m_pcsPreData->m_pszLabel)
	{
		CHAR	szFind[256];
		sprintf(szFind, "[%s]", pcsNewAgcdTemplate->m_pcsPreData->m_pszLabel);
		hParent = CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter()->_FindItemEx(szFind);
	}

	if (!hParent)
		hParent = CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeHandleCharacter();

	CResourceTree* pResTree = CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter();
	if( pResTree )
	{
		HTREEITEM hInserted = pResTree->_InsertItem( pcsNewAgpdTemplate->m_szTName, hParent, (DWORD)(pcsNewAgpdTemplate->m_lID) );
		pResTree->SetItemColor( hInserted, RGB(64, 64, 64) );

		if ( pcsNewAgpdTemplate )
		{
			AgcdCharacterTemplate* pTempAgcdCharTemplate = m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(pcsNewAgpdTemplate);
			if ( pTempAgcdCharTemplate )
			{
				pTempAgcdCharTemplate->m_nDNF = 0xFFffFFff;
				HTREEITEM hDestItem = pResTree->_FindItemEx(pcsNewAgpdTemplate->m_szTName);
				pResTree->SetTreeDNF_Flag(hDestItem, m_uNationalCode, pTempAgcdCharTemplate->m_nDNF);
			}
		}
	}

	return pcsNewAgpdTemplate;
}

AgpdItemTemplate *CModelToolApp::MakeItemTemplate(CHAR *szName, AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pcsAgcdItemTemplate, INT32 lArmourID, DWORD dwID, BOOL bEnsureVisible)
{
	INT32 lTID = GenerateITID( dwID );
	if( !lTID )
	{
		MessageBox( NULL, "이미 ID가 존재합니다.", "Error", MB_OK );
		return NULL;
	}

	AgpdItemTemplate *pcsNewAgpdTemplate = m_csAgcEngine.GetAgpmItemModule()->AddItemTemplate(lTID);
	if(!pcsNewAgpdTemplate)		return NULL;

	AgcdItemTemplate *pcsNewAgcdTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(pcsNewAgpdTemplate);
	if(!pcsNewAgcdTemplate)		return NULL;

	if ( (!pcsNewAgcdTemplate->m_pcsPreData) || (!pcsAgcdItemTemplate->m_pcsPreData) )
		return NULL;

	strcpy(pcsNewAgpdTemplate->m_szName, szName);

	AgpdItemTemplateEquip* pcsNewAgpdItemTemplateEquip		= (AgpdItemTemplateEquip *)(pcsNewAgpdTemplate);
	AgpdItemTemplateEquipWeapon* pcsNewAgpdItemTemplateEquipWeapon	= (AgpdItemTemplateEquipWeapon *)(pcsNewAgpdItemTemplateEquip);

	pcsNewAgpdTemplate->m_nType	= pcsAgpdItemTemplate->m_nType;
	if(pcsNewAgpdTemplate->m_nType == AGPMITEM_TYPE_EQUIP)
	{
		AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip = (AgpdItemTemplateEquip *)(pcsAgpdItemTemplate);

		pcsNewAgpdItemTemplateEquip->m_nKind = pcsAgpdItemTemplateEquip->m_nKind;
		pcsNewAgpdItemTemplateEquip->m_nPart = pcsAgpdItemTemplateEquip->m_nPart;

		if(pcsNewAgpdItemTemplateEquip->m_nKind	== AGPMITEM_EQUIP_KIND_ARMOUR)
		{
			pcsNewAgpdItemTemplateEquip->m_nPart = (AgpmItemPart)(lArmourID);
		}
		else if(pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
		{
			pcsNewAgpdItemTemplateEquipWeapon->m_nWeaponType = ((AgpdItemTemplateEquipWeapon *)(pcsAgpdItemTemplateEquip))->m_nWeaponType;
		}
	}

	pcsNewAgpdTemplate->m_bStackable			= pcsAgpdItemTemplate->m_bStackable;
	pcsNewAgpdTemplate->m_lMaxStackableCount	= pcsAgpdItemTemplate->m_lMaxStackableCount;
	pcsNewAgcdTemplate->m_bEquipTwoHands		= pcsAgcdItemTemplate->m_bEquipTwoHands;
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pcsPreData->m_pszBaseDFFName, pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pcsPreData->m_pszSecondDFFName, pcsAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pcsPreData->m_pszFieldDFFName, pcsAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pcsPreData->m_pszTextureName, pcsAgcdItemTemplate->m_pcsPreData->m_pszTextureName );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pcsPreData->m_pszBaseDFFName, pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName );

	if(!m_csAgcEngine.GetAgpmFactorsModule()->SetFactor(&pcsNewAgpdTemplate->m_csFactor, NULL, AGPD_FACTORS_TYPE_ITEM))
		return NULL;

	if( pcsNewAgpdTemplate->m_nType == AGPMITEM_TYPE_EQUIP )
	{
		if( pcsNewAgpdItemTemplateEquip->m_nKind != AGPMITEM_EQUIP_KIND_ARMOUR )
		{
			RpClump* pClump = m_csAgcEngine.GetAgcmItemModule()->GetClumpAdminData(pcsNewAgcdTemplate->m_pcsPreData->m_pszBaseDFFName);
//			if( pClump ) {
//				char szTemp[256];
//				sprintf( szTemp, "\"%s\" clump not found.", pcsNewAgcdTemplate->m_pcsPreData->m_pszBaseDFFName );
//				MessageBox( szTemp, "Error", MB_OK );
//			}
			pcsNewAgcdTemplate->m_lAtomicCount = RpClumpGetNumAtomics( pClump );
			m_csAgcEngine.GetAgcmItemModule()->RemoveClumpAdminData(pcsNewAgcdTemplate->m_pcsPreData->m_pszBaseDFFName); // 아이템 클럼프 관리상 지워줘야 한다. (툴종료때 모두 지워서 상관없지만...)

			if( pcsNewAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
			{
				if(pcsNewAgcdTemplate->m_bEquipTwoHands)
					pcsNewAgcdTemplate->m_lAtomicCount += pcsNewAgcdTemplate->m_lAtomicCount;

				m_csAgcEngine.GetAgpmFactorsModule()->SetValue(&pcsNewAgpdTemplate->m_csFactor, pcsNewAgcdTemplate->m_bEquipTwoHands ? 2 : 1, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);
			}
		}
	}

	if( pcsAgcdItemTemplate->m_pItemTransformInfo )
		pcsAgcdItemTemplate->m_pItemTransformInfo->Assign( pcsNewAgcdTemplate->m_pItemTransformInfo );
	pcsNewAgcdTemplate->m_lObjectType = pcsAgcdItemTemplate->m_lObjectType;

	char szNewItemName[256];
	sprintf(szNewItemName, "%s_%05d",pcsNewAgpdTemplate->m_szName, pcsNewAgpdTemplate->m_lID);
	HTREEITEM hInserted = CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeItem()->_InsertItem(
		szNewItemName,
		CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeHandleItemParent( pcsNewAgpdTemplate, pcsNewAgcdTemplate ),
		(DWORD)(pcsNewAgpdTemplate->m_lID),
		bEnsureVisible );

	CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeItem()->SetItemColor(hInserted, RGB(64, 64, 64));

	return pcsNewAgpdTemplate;
}

RpAtomic *CModelToolApp::ArmourCheckCB(RpAtomic *atomic, void *data)
{
	CAmtArmourCheck	*pcsArmourCheck	= (CAmtArmourCheck *)(data);

	RwInt32 lPartID = atomic->iPartID;
	if(lPartID >= AGPMITEM_PART_BODY)
	{
		switch(lPartID)
		{
		case AGPMITEM_PART_BODY:	pcsArmourCheck->m_bHasBody = TRUE;	break;
		case AGPMITEM_PART_HEAD:	pcsArmourCheck->m_bHasHead = TRUE;	break;
		case AGPMITEM_PART_ARMS:	pcsArmourCheck->m_bHasArms = TRUE;	break;
		case AGPMITEM_PART_HANDS:	pcsArmourCheck->m_bHasHands = TRUE;	break;
		case AGPMITEM_PART_LEGS:	pcsArmourCheck->m_bHasLegs = TRUE;	break;
		case AGPMITEM_PART_FOOT:	pcsArmourCheck->m_bHasFoot = TRUE;	break;
		case AGPMITEM_PART_ARMS2:	pcsArmourCheck->m_bHasArms2 = TRUE;	break;
		default:
			break;
		}
	}

	return atomic;
}

AgpdItemTemplate *CModelToolApp::AddItemTemplate(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pcsAgcdItemTemplate, DWORD dwID, BOOL bEnsureVisible)
{
	if( !pcsAgcdItemTemplate->m_pcsPreData )		return NULL;

	AgpdItemTemplate* pstRt = NULL;
	switch(pcsAgpdItemTemplate->m_nType)
	{
	case AGPMITEM_TYPE_EQUIP:
		{
			AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip = (AgpdItemTemplateEquip *)(pcsAgpdItemTemplate);

			switch(pcsAgpdItemTemplateEquip->m_nKind)
			{
			case AGPMITEM_EQUIP_KIND_ARMOUR:
				{
					if (!m_pcsStaticModel)
					{
						m_pcsStaticModel = m_csAgcEngine.GetAgcmItemModule()->GetClumpAdminData( pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName );
						if (!m_pcsStaticModel)
							return NULL;
					}

					m_csAgcEngine.GetAgcmRenderModule()->RemoveClumpFromWorld( m_pcsStaticModel );

					CAmtArmourCheck	csArmourCheck;
					RpClumpForAllAtomics( m_pcsStaticModel, ArmourCheckCB, &csArmourCheck );
					m_csAgcEngine.GetAgcmItemModule()->RemoveClumpAdminData( pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName );
					m_pcsStaticModel		= NULL;

					DWORD dwNewID = dwID;
					CHAR szTempName[AMT_MAX_STR];
					if(csArmourCheck.m_bHasBody)
					{
						sprintf(szTempName, "%s%s", pcsAgpdItemTemplate->m_szName, AMT_ARMOUR_PART_BODY_NAME);
						pstRt = MakeItemTemplate(szTempName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, AGPMITEM_PART_BODY, dwNewID, bEnsureVisible);
						if( pstRt )
							dwNewID = 0;
					}

					if(csArmourCheck.m_bHasHead)
					{
						sprintf(szTempName, "%s%s", pcsAgpdItemTemplate->m_szName, AMT_ARMOUR_PART_HEAD_NAME);
						pstRt = MakeItemTemplate(szTempName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, AGPMITEM_PART_HEAD, dwNewID, bEnsureVisible);
						if( pstRt )
							dwNewID = 0;
					}

					if(csArmourCheck.m_bHasArms)
					{
						sprintf(szTempName, "%s%s", pcsAgpdItemTemplate->m_szName, AMT_ARMOUR_PART_ARMS_NAME);
						pstRt = MakeItemTemplate(szTempName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, AGPMITEM_PART_ARMS, dwNewID, bEnsureVisible);
						if( pstRt )
							dwNewID = 0;
					}

					if(csArmourCheck.m_bHasHands)
					{
						sprintf(szTempName, "%s%s", pcsAgpdItemTemplate->m_szName, AMT_ARMOUR_PART_HANDS_NAME);
						pstRt = MakeItemTemplate(szTempName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, AGPMITEM_PART_HANDS, dwNewID, bEnsureVisible);
						if( pstRt )
							dwNewID = 0;
					}

					if(csArmourCheck.m_bHasLegs)
					{
						sprintf(szTempName, "%s%s", pcsAgpdItemTemplate->m_szName, AMT_ARMOUR_PART_LEGS_NAME);
						pstRt = MakeItemTemplate(szTempName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, AGPMITEM_PART_LEGS, dwNewID, bEnsureVisible);
						if( pstRt )
							dwNewID = 0;
					}

					if(csArmourCheck.m_bHasFoot)
					{
						sprintf(szTempName, "%s%s", pcsAgpdItemTemplate->m_szName, AMT_ARMOUR_PART_FOOT_NAME);
						pstRt = MakeItemTemplate(szTempName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, AGPMITEM_PART_FOOT, dwNewID, bEnsureVisible);
						if( pstRt )
							dwNewID = 0;
					}

					if(csArmourCheck.m_bHasArms2)
					{
						sprintf(szTempName, "%s%s", pcsAgpdItemTemplate->m_szName, AMT_ARMOUR_PART_ARMS2_NAME);
						pstRt = MakeItemTemplate(szTempName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, AGPMITEM_PART_ARMS2, dwNewID, bEnsureVisible);
						if( pstRt )
							dwNewID = 0;
					}
				}
				break;

			case AGPMITEM_EQUIP_KIND_WEAPON:
				{
					pstRt = MakeItemTemplate(pcsAgpdItemTemplate->m_szName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, 0, dwID, bEnsureVisible);
				}
				break;

			case AGPMITEM_EQUIP_KIND_SHIELD:
				{
					pstRt = MakeItemTemplate(pcsAgpdItemTemplate->m_szName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, 0, dwID, bEnsureVisible);
				}
				break;

			case AGPMITEM_EQUIP_KIND_RING:
				{
					pstRt = MakeItemTemplate(pcsAgpdItemTemplate->m_szName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, 0, dwID, bEnsureVisible);
				}
				break;

			case AGPMITEM_EQUIP_KIND_NECKLACE:
				{
					pstRt = MakeItemTemplate(pcsAgpdItemTemplate->m_szName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, 0, dwID, bEnsureVisible);
				}
				break;

			case AGPMITEM_EQUIP_KIND_RIDE:
				{
					pstRt = MakeItemTemplate(pcsAgpdItemTemplate->m_szName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, 0, dwID, bEnsureVisible);
				}
				break;
			}
		}
		break;

	case AGPMITEM_TYPE_USABLE:
		{
			pstRt = MakeItemTemplate(pcsAgpdItemTemplate->m_szName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, 0, dwID, bEnsureVisible);
		}
		break;

	case AGPMITEM_TYPE_OTHER:
		{
			pstRt = MakeItemTemplate(pcsAgpdItemTemplate->m_szName, pcsAgpdItemTemplate, pcsAgcdItemTemplate, 0, dwID, bEnsureVisible);
		}
		break;
	}

	if ( pstRt )
	{
		CResourceTree* pResTree = CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeItem();
		AgcdItemTemplate* pTempAgcdItemTemplate =  m_csAgcEngine.GetAgcmItemModule()->GetTemplateData( pstRt );
		if ( pTempAgcdItemTemplate )
		{
			pTempAgcdItemTemplate->m_nDNF = 0xFFffFFff;

			char szNewItemName[256];
			sprintf(szNewItemName, "%s_%05d", pstRt->m_szName, pstRt->m_lID);
			HTREEITEM hDestItem = pResTree->_FindItemEx(szNewItemName);
			pResTree->SetTreeDNF_Flag(hDestItem, m_uNationalCode, pTempAgcdItemTemplate->m_nDNF);
		}
	}

	return pstRt;
}

ApdObjectTemplate *CModelToolApp::AddObjectTemplate(ApdObjectTemplate *pcsApdObjectTemplate, AgcdObjectTemplate *pcsAgcdObjectTemplate, DWORD dwID)
{
	INT32 lTID = GenerateOTID( dwID );
	if( !lTID )
	{
		MessageBox( NULL, "이미 ID가 존재합니다.", "Error", MB_OK );
		return NULL;
	}

	ApdObjectTemplate *pcsNewApdTemplate = m_csAgcEngine.GetApmObjectModule()->AddObjectTemplate(lTID);
	if( !pcsNewApdTemplate )		return NULL;

	AgcdObjectTemplate *pcsNewAgcdTemplate = m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(pcsNewApdTemplate);
	if( !pcsNewAgcdTemplate )			return NULL;

	TSO::SetDefaultSequenceNumber( pcsNewApdTemplate );
	m_csAgcEngine.SetSaveData( pcsNewApdTemplate );
		
	strcpy(pcsNewApdTemplate->m_szName,					pcsApdObjectTemplate->m_szName);		
	strcpy(pcsNewAgcdTemplate->m_szCategory,			pcsAgcdObjectTemplate->m_szCategory);
	strcpy(pcsNewAgcdTemplate->m_szCollisionDFFName,	pcsAgcdObjectTemplate->m_szCollisionDFFName);

	// group
	AgcmObject					*pcsAgcmObject		= m_csAgcEngine.GetAgcmObjectModule();
	AgcmObjectList				*pcsAgcmObjectList	= pcsAgcmObject->GetObjectList();
	AgcdObjectTemplateGroupData *pcsGroupData, *pcsNewGroupData;
	for( INT32 lIndex = 0; ; ++lIndex )
	{
		pcsGroupData = pcsAgcmObjectList->GetObjectTemplateGroup(&pcsAgcdObjectTemplate->m_stGroup, lIndex);
		if ( !pcsGroupData )
		{
			if ( !lIndex )
				return NULL;

			break;
		}

		pcsNewGroupData = pcsAgcmObjectList->GetObjectTemplateGroup(&pcsNewAgcdTemplate->m_stGroup, lIndex);
		if (!pcsNewGroupData)
			pcsNewGroupData = pcsAgcmObjectList->AddObjectTemplateGroup(&pcsNewAgcdTemplate->m_stGroup);
		if (!pcsNewGroupData)
			return NULL;

		pcsAgcmObject->CopyObjectTemplateGroupData(pcsNewApdTemplate->m_lID, pcsNewGroupData, pcsGroupData);
		m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.AddLODData(&pcsNewAgcdTemplate->m_stLOD);	// LOD를 추가한다~
	}

	CResourceTree* pResourceTree = CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject();

	CHAR szTemp[AMT_MAX_STR];
	sprintf(szTemp, "[%s]", pcsNewAgcdTemplate->m_szCategory);

	HTREEITEM hParent = pResourceTree->_FindItemEx(szTemp);
	if( !hParent )	return NULL;

	HTREEITEM hInserted = pResourceTree->_InsertItem( pcsNewApdTemplate->m_szName, hParent, (DWORD)(pcsNewApdTemplate->m_lID));
	pResourceTree->SetItemColor( hInserted, RGB(64, 64, 64) );
	AgcdObjectTemplate* pTempAgcdObjectTemplate = m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData( pcsNewApdTemplate );
	if ( pTempAgcdObjectTemplate )
	{
		pTempAgcdObjectTemplate->m_nDNF = 0xFFffFFff;
		HTREEITEM hDestItem = pResourceTree->_FindItemEx(pcsNewApdTemplate->m_szName);
		pResourceTree->SetTreeDNF_Flag(hDestItem, m_uNationalCode, pTempAgcdObjectTemplate->m_nDNF);
	}

	return pcsNewApdTemplate;
}

AgpdSkillTemplate *CModelToolApp::AddSkillTemplate(AgpdSkillTemplate *pcsAgpdSkillTemplate, AgcdSkillTemplate *pcsAgcdSkillTemplate, DWORD dwID, BOOL bEnsureVisible)
{
	INT32 lTID = GenerateSTID( dwID );
	if( !lTID )
	{
		MessageBox( NULL, "이미 ID가 존재합니다.", "Error", MB_OK );
		return NULL;
	}

	AgpdSkillTemplate *pcsNewAgpdTemplate = m_csAgcEngine.GetAgpmSkillModule()->AddSkillTemplate(lTID);
	if(!pcsNewAgpdTemplate)		return NULL;

	AgcdSkillTemplate *pcsNewAgcdTemplate = m_csAgcEngine.GetAgcmSkillModule()->GetADSkillTemplate((PVOID)(pcsNewAgpdTemplate));
	if(!pcsNewAgcdTemplate)		return NULL;

	strcpy( pcsNewAgpdTemplate->m_szName, pcsAgpdSkillTemplate->m_szName );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pszTextureName, pcsAgcdSkillTemplate->m_pszTextureName );
	UTIL().ReallocCopyString( &pcsNewAgcdTemplate->m_pszSmallTextureName, pcsAgcdSkillTemplate->m_pszSmallTextureName );

	CResourceTree* pResourceTree = CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeSkill();

	HTREEITEM hInserted = pResourceTree->_InsertItem( pcsNewAgpdTemplate->m_szName,
													CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeHandleSkill(), 
													(DWORD)(pcsNewAgpdTemplate->m_lID), bEnsureVisible );
	pResourceTree->SetItemColor(hInserted, RGB(64, 64, 64));

	AgcdSkillTemplate* pTempAgcdSkillTemplate = m_csAgcEngine.GetAgcmSkillModule()->GetADSkillTemplate(pcsNewAgpdTemplate);
	if ( pTempAgcdSkillTemplate )
	{
		pTempAgcdSkillTemplate->m_nDNF = 0xFFffFFff;
		HTREEITEM hDestItem = pResourceTree->_FindItemEx(pcsNewAgpdTemplate->m_szName);
		pResourceTree->SetTreeDNF_Flag(hDestItem, m_uNationalCode, pTempAgcdSkillTemplate->m_nDNF);
	}

	return pcsNewAgpdTemplate;
}

BOOL CModelToolApp::CopyCharacterTemplate(INT32 lTID)
{
	AgpmCharacter* pcsAgpmCharacter	= m_csAgcEngine.GetAgpmCharacterModule();
	AgcmCharacter* pcsAgcmCharacter	= m_csAgcEngine.GetAgcmCharacterModule();
	if ( !pcsAgpmCharacter || !pcsAgcmCharacter )		return FALSE;

	AgpdCharacterTemplate *pstAgpdCharacterTemplate = pcsAgpmCharacter->GetCharacterTemplate(lTID);
	if ( !pstAgpdCharacterTemplate )	return FALSE;
		

	AgcdCharacterTemplate *pstAgcdCharacterTemplate = pcsAgcmCharacter->GetTemplateData(pstAgpdCharacterTemplate);
	if ( !pstAgcdCharacterTemplate )	return FALSE;
		
	CHAR	szTName[AMT_MAX_STR];
	strcpy(szTName, pstAgpdCharacterTemplate->m_szTName);
	CEditDlg dlg(szTName, "캐릭터 템플릿의 이름을 입력하세요!");
	if (dlg.DoModal() == IDOK)
	{
		if ( stricmp(szTName, pstAgpdCharacterTemplate->m_szTName) == 0 )
		{
			CModelToolDlg::GetInstance()->MessageBox("템플릿 이름이 동일합니다!", "Error", MB_OK);
			return FALSE;
		}

		if ( CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter()->_FindItemEx(szTName) )
		{
			CModelToolDlg::GetInstance()->MessageBox("이미 존재하는 템플릿입니다!", "Error", MB_OK);
			return FALSE;
		}

		CHAR szTemp[AMT_MAX_STR];
		strcpy(szTemp, pstAgpdCharacterTemplate->m_szTName);
		strcpy(pstAgpdCharacterTemplate->m_szTName, szTName);

		AgpdCharacterTemplate *pstAgpdCharacterTemplate2 = AddCharacterTemplate(pstAgpdCharacterTemplate, pstAgcdCharacterTemplate);
		if (!pstAgpdCharacterTemplate2)
		{
			strcpy(pstAgpdCharacterTemplate->m_szTName, szTemp);
			return FALSE;
		}
		
		strcpy(pstAgpdCharacterTemplate->m_szTName, szTemp);
		
		AgcdCharacterTemplate *pstAgcdCharacterTemplate2 = pcsAgcmCharacter->GetTemplateData(pstAgpdCharacterTemplate2);
		if ( !pstAgcdCharacterTemplate2 )		return FALSE;

		pstAgcdCharacterTemplate2->m_lAnimType2	= pstAgcdCharacterTemplate->m_lAnimType2;
		pcsAgcmCharacter->AllocateAnimationData( pstAgcdCharacterTemplate2 );

		INT32	lNumType2	= pcsAgcmCharacter->GetAnimType2Num(pstAgcdCharacterTemplate);
		for (INT32 lType = AGCMCHAR_ANIM_TYPE_WAIT; lType < AGCMCHAR_MAX_ANIM_TYPE; ++lType)
		{
			for (INT32 lType2 = 0; lType2 < lNumType2; ++lType2)
			{
				if (!pstAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2])
					continue;

				if (!pstAgcdCharacterTemplate2->m_pacsAnimationData[lType][lType2])
					pstAgcdCharacterTemplate2->m_pacsAnimationData[lType][lType2]	= new AgcdCharacterAnimation();

				if (!pcsAgcmCharacter->GetAgcaAnimation2()->CopyAnimation(
					pstAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]->m_pcsAnimation,
					&pstAgcdCharacterTemplate2->m_pacsAnimationData[lType][lType2]->m_pcsAnimation	))
					return FALSE;

				if (!pcsAgcmCharacter->CopyAnimAttachedData(
					pstAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]->m_pcsAnimation,
					pstAgcdCharacterTemplate2->m_pacsAnimationData[lType][lType2]->m_pcsAnimation	))
					return FALSE;

				if (!m_csAgcEngine.GetAgcmEventEffectModule()->CopyCharAnimAttachedData(
					pstAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]->m_pcsAnimation,
					pstAgcdCharacterTemplate2->m_pacsAnimationData[lType][lType2]->m_pcsAnimation	))
					return FALSE;

				if ( pstAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]->m_pstAnimFlags )
				{
					if ( pcsAgcmCharacter->GetAgcaAnimation2()->AddFlags(
						&pstAgcdCharacterTemplate2->m_pacsAnimationData[lType][lType2]->m_pstAnimFlags) )
					{
						pstAgcdCharacterTemplate2->m_pacsAnimationData[lType][lType2]->m_pstAnimFlags->m_unAnimFlag =
							pstAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]->m_pstAnimFlags->m_unAnimFlag;

						pstAgcdCharacterTemplate2->m_pacsAnimationData[lType][lType2]->m_pstAnimFlags->m_unPreference = 
							pstAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]->m_pstAnimFlags->m_unPreference;
					}
				}
			}
		}

		if ( !CopyEffectData((ApBase *)(pstAgpdCharacterTemplate), (ApBase *)(pstAgpdCharacterTemplate2)) )
			return FALSE;

		// LOD
		AgcdLOD* pcsSrcLOD = &pstAgcdCharacterTemplate->m_stLOD;
		AgcdLOD* pcsDestLOD = &pstAgcdCharacterTemplate2->m_stLOD;
		AgcdPreLOD* pcsSrcPreLOD = m_csAgcEngine.GetAgcmPreLODManagerModule()->GetPreLOD(pstAgpdCharacterTemplate);
		AgcdPreLOD* pcsDestPreLOD = m_csAgcEngine.GetAgcmPreLODManagerModule()->GetPreLOD(pstAgpdCharacterTemplate2);
		CopyLODData( pcsSrcLOD, pcsDestLOD, pcsSrcPreLOD, pcsDestPreLOD );

		// Default equipments
		AgpdItemADCharTemplate	*pcsSrcADCharTemplate	= m_csAgcEngine.GetAgpmItemModule()->GetADCharacterTemplate(pstAgpdCharacterTemplate);
		if (!pcsSrcADCharTemplate)		return FALSE;
			
		AgpdItemADCharTemplate	*pcsDestADCharTemplate	= m_csAgcEngine.GetAgpmItemModule()->GetADCharacterTemplate(pstAgpdCharacterTemplate2);
		if (!pcsDestADCharTemplate)		return FALSE;
			
		pcsDestADCharTemplate->m_lDefaultEquipITID.MemCopy(0, &pcsSrcADCharTemplate->m_lDefaultEquipITID[0], AGPMITEM_PART_NUM);

		///////////////////////////////////////////////////////////////////////
		//. 2005. 09. 12 Nonstopdj
		for( int nIndex = 0; nIndex < AGPMITEM_DEFAULT_INVEN_NUM; ++nIndex )
		{
			if(pcsSrcADCharTemplate->m_aalDefaultInvenInfo[nIndex][AGPDITEM_DEFAULT_INVEN_INDEX_TID])
			{
				pcsDestADCharTemplate->m_aalDefaultInvenInfo[nIndex][AGPDITEM_DEFAULT_INVEN_INDEX_TID] = 
					pcsSrcADCharTemplate->m_aalDefaultInvenInfo[nIndex][AGPDITEM_DEFAULT_INVEN_INDEX_TID];

				pcsDestADCharTemplate->m_aalDefaultInvenInfo[nIndex][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT] = 
					pcsSrcADCharTemplate->m_aalDefaultInvenInfo[nIndex][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT];
		
			}
		}

		for (INT32	lPartID = 0; lPartID < AGPMITEM_PART_NUM; ++lPartID)
		{
			if ( !pcsDestADCharTemplate->m_lDefaultEquipITID[lPartID] )		continue;

			AgpdItemTemplate* ppDefItem	= m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(pcsDestADCharTemplate->m_lDefaultEquipITID[lPartID]);
			if (!ppDefItem)		return FALSE;

			AgcdItemTemplate* pcDefItem	= m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(ppDefItem);
			if (!pcDefItem)		return FALSE;

			ItemTransformInfo* pcsCurInfo = pcDefItem->m_pItemTransformInfo->GetInfo( pstAgpdCharacterTemplate->m_lID );
			if( !pcsCurInfo )	continue;

			ItemTransformInfo* pNewInfo = pcDefItem->m_pItemTransformInfo->Insert( pstAgpdCharacterTemplate2->m_lID, pcDefItem->m_lAtomicCount );
			if( pNewInfo )
			{
				memcpy( pNewInfo->m_astTransform, pcsCurInfo->m_astTransform, sizeof(RwMatrix) * pcDefItem->m_lAtomicCount );
				memcpy( pNewInfo->m_alPartID, pcsCurInfo->m_alPartID, sizeof(INT32) * pcDefItem->m_lAtomicCount );
			}
		}

		pstAgcdCharacterTemplate2->m_stBSphere = pstAgcdCharacterTemplate->m_stBSphere;
		AgpdSkillTemplateAttachData* pcsAgpdSkiilTemplateADSrc = m_csAgcEngine.GetAgpmSkillModule()->GetAttachSkillTemplateData((ApBase *)(pstAgpdCharacterTemplate));
		AgpdSkillTemplateAttachData* pcsAgpdSkiilTemplateADDest = m_csAgcEngine.GetAgpmSkillModule()->GetAttachSkillTemplateData((ApBase *)(pstAgpdCharacterTemplate2));

		memcpy( pcsAgpdSkiilTemplateADDest->m_aszDefaultTName, pcsAgpdSkiilTemplateADSrc->m_aszDefaultTName, sizeof(pcsAgpdSkiilTemplateADSrc->m_aszDefaultTName) );
		(pcsAgpdSkiilTemplateADDest->m_aszUsableSkillTName).MemCopy( 0, &pcsAgpdSkiilTemplateADSrc->m_aszUsableSkillTName[0], AGPMSKILL_MAX_SKILL_OWN - 1);

		//실제 VisualInfo사용하기
		ApBase						*pcsAgpdCharacterTemplateSrcBase		= (ApBase *)(pstAgpdCharacterTemplate);
		ApBase						*pcsAgpdCharacterTemplateDestBase		= (ApBase *)(pstAgpdCharacterTemplate2);

		AgcdSkillAttachTemplateData	*pstAgcdSkillAttachTemplateDataSrc		= m_csAgcEngine.GetAgcmSkillModule()->GetAttachTemplateData(pcsAgpdCharacterTemplateSrcBase);
		AgcdSkillAttachTemplateData	*pstAgcdSkillAttachTemplateDataDest		= m_csAgcEngine.GetAgcmSkillModule()->GetAttachTemplateData(pcsAgpdCharacterTemplateDestBase);

		
		AgcdCharacterTemplate		*pcsAgcdCharacterTemplate		= pcsAgcmCharacter->GetTemplateData(pstAgpdCharacterTemplate);
		if (!pcsAgcdCharacterTemplate)
			return FALSE;
		
		//. 원본의 VisualInfo 총갯수
		int					nTotalVisualInfoCount	=  0;//m_csAgcEngine.GetAgcmSkillModule()->GetNumVisualInfo(pcsAgcdCharacterTemplate);
		ACA_AttachedData	*pcsAttachedDataDest	= NULL;
		ACA_AttachedData	*pcsAttachedDataSrc		= NULL;
		if(pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo)
		{
			while(pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount])
			{
				//. VisualInfo 할당
				m_csAgcEngine.GetAgcmSkillModule()->CheckVisualInfo(
					pstAgcdCharacterTemplate, 
					pstAgcdSkillAttachTemplateDataDest, 
					nTotalVisualInfoCount //pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_iVisualIndex
					);
				
				INT32	lNumAnimType2	= pcsAgcmCharacter->GetAnimType2Num(pcsAgcdCharacterTemplate);
				int		nCount			= 0;
				if (lNumAnimType2 < 1)
					return FALSE;

				//. m_paszInfo char* 할당
				CHAR** ppasSrcInfo	= pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_paszInfo;
				CHAR** ppasDestInfo = pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_paszInfo;

				if(*ppasSrcInfo != NULL)
				{
					ppasDestInfo = new CHAR * [lNumAnimType2];
					memset(ppasDestInfo, 0, sizeof (CHAR *) * lNumAnimType2);
				}

				///////////////////////////////////////////////////////////////////////
				//. 애니메이션 할당 및 복사
				for(nCount = 0; nCount < lNumAnimType2; nCount++)
				//int nCount = 0;
				//while(pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount])
				{
					//. m_paszInfo char** 할당 및 복사
					if(ppasSrcInfo[nCount] != NULL)
					{
						ppasDestInfo[nCount] = new CHAR [strlen(ppasSrcInfo[nCount]) + 1];
						memset(ppasDestInfo[nCount], 0, sizeof (CHAR ) * strlen(ppasSrcInfo[nCount]) + 1);
						memcpy(ppasDestInfo[nCount], ppasSrcInfo[nCount], sizeof (CHAR ) * strlen(ppasSrcInfo[nCount]));
					}

					if (pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount] != NULL)
					{
						pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount] = new AgcdCharacterAnimation();

						pcsAgcmCharacter->GetAgcaAnimation2()->CopyAnimation(
							pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pcsAnimation,
							&pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pcsAnimation
							);

						//. 애니메이션 flag복사
						if (pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pstAnimFlags != NULL)
						{
							pcsAgcmCharacter->GetAgcaAnimation2()->AddFlags(
								&pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pstAnimFlags);

							pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pstAnimFlags->m_unAnimFlag	
								= pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pstAnimFlags->m_unAnimFlag;
							pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pstAnimFlags->m_unPreference
								= pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pstAnimFlags->m_unPreference;
						}

						//. 애니메이션 point복사
						if (pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pcsAnimation->m_pcsHead != NULL)
						{
							pcsAttachedDataSrc	=
								(ACA_AttachedData *)(pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData(
								AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
								pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pcsAnimation->m_pcsHead));

							pcsAttachedDataDest	=
								(ACA_AttachedData *)(pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData(
								AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
								pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pacsAnimation[nCount]->m_pcsAnimation->m_pcsHead));

							if(pcsAttachedDataSrc->m_pszPoint != NULL)
							{
								pcsAttachedDataDest->m_pszPoint	= new CHAR [strlen(pcsAttachedDataSrc->m_pszPoint) + 1];
								strcpy(pcsAttachedDataDest->m_pszPoint, pcsAttachedDataSrc->m_pszPoint);
							}

						}
					}
				}

				
				///////////////////////////////////////////////////////////////////////
				//. 스킬이펙트관련 할당 및 복사
				for(nCount = 0; nCount < lNumAnimType2; nCount++)
				{
					if (pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pastEffect[nCount] != NULL && 
						pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pastEffect[nCount]->m_pcsHead)
					{
						pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pastEffect[nCount] = new AgcdUseEffectSet();
						pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pastEffect[nCount]->m_pcsHead = new AgcdUseEffectSetList();

						AgcdUseEffectSetList*	pcsEDataSrcList		=	pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pastEffect[nCount]->m_pcsHead;
						AgcdUseEffectSetList*	pcsEDataDestList	=	pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pastEffect[nCount]->m_pcsHead;
						
						AgcdUseEffectSetData*	pcsEDataDest = NULL;
						AgcdUseEffectSetData*	pcsEDataSrc	 = NULL;

						//. AgcdUseEffectSet 두개의 flag 복사.
						pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pastEffect[nCount]->m_ulConditionFlags = 
						pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pastEffect[nCount]->m_ulConditionFlags;

						pstAgcdSkillAttachTemplateDataDest->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pastEffect[nCount]->m_ulCustomFlags = 
						pstAgcdSkillAttachTemplateDataSrc->m_pacsSkillVisualInfo[nTotalVisualInfoCount]->m_pastEffect[nCount]->m_ulCustomFlags;

						while( pcsEDataSrcList )
						{
							AgcdUseEffectSetData*	pcsEDataSrc		= &pcsEDataSrcList->m_csData;
							AgcdUseEffectSetData*	pcsEDataDest	= &pcsEDataDestList->m_csData;

							if( !pcsEDataDest )
								pcsEDataDest = new AgcdUseEffectSetData();

							//. skillmodule의 member copydata()를 이용하여 값을 복사.
							m_csAgcEngine.GetAgcmSkillModule()->GetEffectAdmin2().CopyData(pcsEDataDest, pcsEDataSrc);

							if( pcsEDataSrcList->m_pcsNext )
							{
								pcsEDataSrcList = pcsEDataSrcList->m_pcsNext;
								if( pcsEDataDestList )
								{
									pcsEDataDestList->m_pcsNext	= new AgcdUseEffectSetList;
									pcsEDataDestList = pcsEDataDestList->m_pcsNext;
								}
							}
							else
								break;
						}
					}
				}

				++nTotalVisualInfoCount;
			}
		}

		///////////////////////////////////////////////////////////////////////
		//. 2005. 09. 12 Nonstopdj
		//. character type copy
		pstAgpdCharacterTemplate2->m_ulCharType	= pstAgpdCharacterTemplate->m_ulCharType;

		///////////////////////////////////////////////////////////////////////
		//. 2005. 09. 12 Nonstopdj
		//. character custormizing data copy (face, hair) & attach data
		//. DEFAULT_FACE ~ storage data member allocate
		if(pstAgcdCharacterTemplate->m_pcsDefaultHeadData != NULL)
		{
			//. allocate default Head Data
			if(pstAgcdCharacterTemplate2->m_pcsDefaultHeadData == NULL)
				pstAgcdCharacterTemplate2->m_pcsDefaultHeadData = new AgcdCharacterDefaultHeadTemplate;

			int nFaceNum = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->GetFaceNum();

			//pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->nFaceNum = nFaceNum;
			//pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->pFaceInfo = new std::string[nFaceNum];
			pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->vecFaceInfo.resize( nFaceNum );

			pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_FaceRenderType.Alloc( nFaceNum );
			pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_FaceRenderType.MemsetRenderType( 0 );
			pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_FaceRenderType.MemsetCustData( 0 );

			for(int nFaceIndex = 0; nFaceIndex < nFaceNum; nFaceIndex++)
			{
				//. 실제 Face 데이터 복사
				pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->vecFaceInfo[nFaceIndex] = 
					pstAgcdCharacterTemplate->m_pcsDefaultHeadData->vecFaceInfo[nFaceIndex];

				pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_FaceRenderType.m_vecRenderType[nFaceIndex] = 
					pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecRenderType[nFaceIndex];

				pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_FaceRenderType.m_vecCustData[nFaceIndex] = 
					pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecCustData[nFaceIndex];
			}

			//. DEFAULT_HAIR ~ storage data member allocate
			int nHairNum = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->GetHairNum();

			pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->vecHairInfo.resize( nHairNum );

			pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_HairRenderType.Alloc( nHairNum );
			pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_HairRenderType.MemsetRenderType( 0 );
			pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_HairRenderType.MemsetCustData( 0 );

			for(int nHairIndex = 0; nHairIndex < nHairNum; nHairIndex++)
			{
				//. 실제 Hair 데이터 복사
				pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->vecHairInfo[nHairIndex] = 
					pstAgcdCharacterTemplate->m_pcsDefaultHeadData->vecHairInfo[nHairIndex];

				pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_HairRenderType.m_vecRenderType[nHairIndex] = 
					pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecRenderType[nHairIndex];

				pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_HairRenderType.m_vecCustData[nHairIndex] = 
					pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecCustData[nHairIndex];
			}

			//. CUSTOMIZE_REVIEW data 복사
			pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_vtxNearCamera = 
				pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxNearCamera;

			pstAgcdCharacterTemplate2->m_pcsDefaultHeadData->m_vtxFarCamera = 
				pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxFarCamera;
		}

		///////////////////////////////////////////////////////////////////////
		// ObjectType
		pstAgcdCharacterTemplate2->m_lObjectType = pstAgcdCharacterTemplate->m_lObjectType;
		///////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		// Height
		pstAgcdCharacterTemplate2->m_lHeight = pstAgcdCharacterTemplate->m_lHeight;
		///////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		// Scale
		pstAgcdCharacterTemplate2->m_fScale	= pstAgcdCharacterTemplate->m_fScale;
		///////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////
		// Height
		INT32 lHitRange = 0;
		if (m_csAgcEngine.GetAgpmFactorsModule()->GetValue(&pstAgpdCharacterTemplate->m_csFactor, &lHitRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HITRANGE))
		{
			if (!m_csAgcEngine.GetAgpmFactorsModule()->SetFactor(&pstAgpdCharacterTemplate2->m_csFactor, NULL, AGPD_FACTORS_TYPE_ATTACK))
				return FALSE;
			if (!m_csAgcEngine.GetAgpmFactorsModule()->SetValue(&pstAgpdCharacterTemplate2->m_csFactor, lHitRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HITRANGE))
				return FALSE;
		}
		///////////////////////////////////////////////////////////////////////

		//@{ 2007/01/15 burumal
		pstAgpdCharacterTemplate2->m_fSiegeWarCollBoxHeight		= pstAgpdCharacterTemplate->m_fSiegeWarCollBoxHeight;
		pstAgpdCharacterTemplate2->m_fSiegeWarCollBoxWidth		= pstAgpdCharacterTemplate->m_fSiegeWarCollBoxWidth;
		pstAgpdCharacterTemplate2->m_fSiegeWarCollSphereRadius	= pstAgpdCharacterTemplate->m_fSiegeWarCollSphereRadius;
		pstAgpdCharacterTemplate2->m_fSiegeWarCollObjOffsetX	= pstAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetX;
		pstAgpdCharacterTemplate2->m_fSiegeWarCollObjOffsetZ	= pstAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetZ;

		pstAgpdCharacterTemplate2->m_bSelfDestructionAttackType	= pstAgpdCharacterTemplate->m_bSelfDestructionAttackType;

		for ( int nPickIdx = 0; nPickIdx < D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP; ++nPickIdx )
			pstAgcdCharacterTemplate2->m_lPickingNodeIndex[nPickIdx] = pstAgcdCharacterTemplate->m_lPickingNodeIndex[nPickIdx];

		pstAgcdCharacterTemplate2->m_bTagging = pstAgcdCharacterTemplate->m_bTagging;

		pstAgcdCharacterTemplate2->m_lLookAtNode = pstAgcdCharacterTemplate->m_lLookAtNode;

		pstAgcdCharacterTemplate2->m_bUseBending = pstAgcdCharacterTemplate->m_bUseBending;
		pstAgcdCharacterTemplate2->m_fBendingFactor = pstAgcdCharacterTemplate->m_fBendingFactor;
		pstAgcdCharacterTemplate2->m_fBendingDegree = pstAgcdCharacterTemplate->m_fBendingDegree;
		//@}

		return TRUE;
	}

	return FALSE;
}

BOOL CModelToolApp::CopyLODData(AgcdLOD *pcsSrcLOD, AgcdLOD *pcsDestLOD, AgcdPreLOD *pcsSrcPreLOD, AgcdPreLOD *pcsDestPreLOD)
{
	AgcdPreLODData	*pcsSrcPreLODData, *pcsDestPreLODData;
	AgcdLODData		*pcsSrcLODData, *pcsDestLODData;

	for (INT32 lCount = 0; lCount < pcsSrcPreLOD->m_lNum; ++lCount)
	{
		pcsSrcPreLODData						= m_csAgcEngine.GetAgcmPreLODManagerModule()->m_csPreLODAdmin.GetPreLODData(pcsSrcPreLOD, lCount);
		pcsSrcLODData							= m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.GetLODData(pcsSrcLOD, lCount);
		if ((!pcsSrcPreLODData) || (!pcsSrcLODData))
			return FALSE;

		pcsDestPreLODData						= m_csAgcEngine.GetAgcmPreLODManagerModule()->m_csPreLODAdmin.GetPreLODData(pcsDestPreLOD, lCount);
		if (!pcsDestPreLODData)
			pcsDestPreLODData					= m_csAgcEngine.GetAgcmPreLODManagerModule()->m_csPreLODAdmin.AddPreLODData(pcsDestPreLOD);
		if (!pcsDestPreLODData)
			return FALSE;

		memcpy(pcsDestPreLODData->m_abTransformGeom, pcsSrcPreLODData->m_abTransformGeom, sizeof(BOOL) * AGPDLOD_MAX_NUM);
		memcpy(&pcsDestPreLODData->m_stBillboard, &pcsSrcPreLODData->m_stBillboard, sizeof(IsBillData));
		memcpy(pcsDestPreLODData->m_aszData, pcsSrcPreLODData->m_aszData, AGPDLOD_MAX_NUM * AGCD_LODDATA_BUFFER_SIZE);

		pcsDestLODData							= m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.GetLODData(pcsDestLOD, lCount);
		if (!pcsDestLODData)
			pcsDestLODData						= m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.AddLODData(pcsDestLOD);
		if (!pcsDestLODData)
			return FALSE;

		pcsDestLODData->m_lHasBillNum			= pcsSrcLODData->m_lHasBillNum;
		memcpy(pcsDestLODData->m_alBillInfo, pcsSrcLODData->m_alBillInfo, sizeof(INT32) * AGPDLOD_MAX_NUM);
		pcsDestLODData->m_ulMaxLODLevel			= pcsSrcLODData->m_ulMaxLODLevel;
		memcpy(pcsDestLODData->m_aulLODDistance, pcsSrcLODData->m_aulLODDistance, sizeof(UINT32) * AGPDLOD_MAX_NUM);
		pcsDestLODData->m_ulBoundary			= pcsSrcLODData->m_ulBoundary;
		pcsDestLODData->m_ulMaxDistanceRatio	= pcsSrcLODData->m_ulMaxDistanceRatio;
		pcsDestLODData->m_pstAgcdLOD			= pcsDestLOD;
	}

	return TRUE;
}

BOOL CModelToolApp::CopyEffectData(ApBase *pSrc, ApBase *pDest)
{
	AgcdUseEffectSet	*pSrcEffectSet		= m_csAgcEngine.GetAgcmEventEffectModule()->GetUseEffectSet(pSrc);
	AgcdUseEffectSet	*pDestEffectSet		= m_csAgcEngine.GetAgcmEventEffectModule()->GetUseEffectSet(pDest);

	m_csAgcEngine.GetAgcmEventEffectModule()->GetAgcaEffectData2()->CopyDataSet(pDestEffectSet, pSrcEffectSet);

	return TRUE;
}

BOOL CModelToolApp::CopyItemTemplate(INT32 lTID)
{
	AgpdItemTemplate *pstAgpdItemTemplate = m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(lTID);
	if (!pstAgpdItemTemplate)
		return FALSE;

	AgcdItemTemplate *pstAgcdItemTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData(pstAgpdItemTemplate);
	if (!pstAgcdItemTemplate)
		return FALSE;

	CHAR	szTName[AMT_MAX_STR];
//	memset(szTName, 0, AMT_MAX_STR);
	strcpy(szTName, pstAgpdItemTemplate->m_szName);
	CEditDlg dlg(szTName, "아이템 템플릿의 이름을 입력하세요!");
	if (dlg.DoModal() == IDOK)
	{		
		//@{ 2006/05/08 burumal

		//@{ 2006/09/11 burumal		
		//if ( stricmp(szTName, pstAgpdItemTemplate->m_szName) == 0 )
		char szNewItemName[256];
		sprintf(szNewItemName, "%s_%05d", pstAgpdItemTemplate->m_szName, pstAgpdItemTemplate->m_lID);
		if ( stricmp(szTName, szNewItemName) == 0 )
		//@}
		{
			CModelToolDlg::GetInstance()->MessageBox("템플릿 이름이 동일합니다!", "Error", MB_OK);
			return FALSE;
		}

		if ( CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeItem()->_FindItemEx(szTName) )
		{
			CModelToolDlg::GetInstance()->MessageBox("이미 존재하는 템플릿입니다!", "Error", MB_OK);
			return FALSE;
		}
		//@}

		CHAR szTemp[AMT_MAX_STR];
		strcpy(szTemp, pstAgpdItemTemplate->m_szName);
		//strcpy(pstAgpdItemTemplate->m_szName, szTName);

		AgpdItemTemplate *pstAgpdItemTemplate2 = AddItemTemplate(pstAgpdItemTemplate, pstAgcdItemTemplate, FALSE, TRUE);
		strcpy(pstAgpdItemTemplate->m_szName, szTemp);

		if (!pstAgpdItemTemplate2)
			return FALSE;
		
		if (!CopyEffectData((ApBase *)(pstAgpdItemTemplate), (ApBase *)(pstAgpdItemTemplate2)))
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

BOOL CModelToolApp::CopyObjectTemplate(INT32 lTID)
{
	ApdObjectTemplate	*ppTemplate	= m_csAgcEngine.GetApmObjectModule()->GetObjectTemplate(lTID);
	if ( !ppTemplate )		return FALSE;

	AgcdObjectTemplate	*pcTemplate	= m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(ppTemplate);
	if ( !pcTemplate )		return FALSE;

	CHAR	szTName[AMT_MAX_STR];
	strcpy( szTName, ppTemplate->m_szName );
	CEditDlg dlg( szTName, "오브젝트 템플릿의 이름을 입력하세요!");
	if (dlg.DoModal() == IDOK)
	{
		//@{ 2006/05/08 burumal
		if ( stricmp(szTName, ppTemplate->m_szName) == 0 )
		{
			CModelToolDlg::GetInstance()->MessageBox("템플릿 이름이 동일합니다!", "Error", MB_OK);
			return FALSE;
		}

		if ( CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject()->_FindItemEx(szTName) )
		{
			CModelToolDlg::GetInstance()->MessageBox("이미 존재하는 템플릿입니다!", "Error", MB_OK);
			return FALSE;
		}
		//@}

		CHAR	szTemp[AMT_MAX_STR];
		strcpy(szTemp, ppTemplate->m_szName);
		strcpy(ppTemplate->m_szName, szTName);

		ApdObjectTemplate	*ppTemplate2	= AddObjectTemplate(ppTemplate, pcTemplate, FALSE);
		strcpy(ppTemplate->m_szName, szTemp);
		if (!ppTemplate2)		return FALSE;

		AgcdObjectTemplate	*pcTemplate2	= m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(ppTemplate2);
		if (!pcTemplate2)		return FALSE;

		CModelToolDlg::GetInstance()->GetResourceForm()->UpdateObjectResourceTree();
		return TRUE;
	}

	return FALSE;
}

BOOL CModelToolApp::CopySkillTemplate(INT32 lTID)
{
	AgpdSkillTemplate *pstAgpdSkillTemplate = m_csAgcEngine.GetAgpmSkillModule()->GetSkillTemplate(lTID);
	if (!pstAgpdSkillTemplate)		return FALSE;

	AgcdSkillTemplate *pstAgcdSkillTemplate = m_csAgcEngine.GetAgcmSkillModule()->GetADSkillTemplate(pstAgpdSkillTemplate);
	if (!pstAgcdSkillTemplate)		return FALSE;

	CHAR	szTName[AMT_MAX_STR];
	strcpy(szTName, pstAgpdSkillTemplate->m_szName);
	CEditDlg dlg(szTName, "스킬 템플릿의 이름을 입력하세요!");
	if (dlg.DoModal() == IDOK)
	{
		if ( stricmp(szTName, pstAgpdSkillTemplate->m_szName) == 0 )
		{
			CModelToolDlg::GetInstance()->MessageBox("템플릿 이름이 동일합니다!", "Error", MB_OK);
			return FALSE;
		}

		if ( CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeSkill()->_FindItemEx(szTName) )
		{
			CModelToolDlg::GetInstance()->MessageBox("이미 존재하는 템플릿입니다!", "Error", MB_OK);
			return FALSE;
		}

		CHAR szTemp[AMT_MAX_STR];
		strcpy(szTemp, pstAgpdSkillTemplate->m_szName);
		strcpy(pstAgpdSkillTemplate->m_szName, szTName);

		AgpdSkillTemplate *pstAgpdSkillTemplate2 = AddSkillTemplate(pstAgpdSkillTemplate, pstAgcdSkillTemplate, FALSE, TRUE);
		strcpy(pstAgpdSkillTemplate->m_szName, szTemp);

		if (!pstAgpdSkillTemplate2)		return FALSE;

		AgcdSkillTemplate *pstAgcdSkillTemplate2 = m_csAgcEngine.GetAgcmSkillModule()->GetADSkillTemplate(pstAgpdSkillTemplate2);
		if (!pstAgcdSkillTemplate2)		return FALSE;

		UTIL().ReallocCopyString( &pstAgcdSkillTemplate2->m_pszTextureName, pstAgcdSkillTemplate->m_pszTextureName );
		UTIL().ReallocCopyString( &pstAgcdSkillTemplate2->m_pszSmallTextureName, pstAgcdSkillTemplate->m_pszSmallTextureName );
		UTIL().ReallocCopyString( &pstAgcdSkillTemplate2->m_pszUnableTextureName, pstAgcdSkillTemplate->m_pszUnableTextureName );

		return TRUE;
	}

	return FALSE;
}

BOOL CModelToolApp::SetTarget(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter)
{
	if (!pcsAgpdCharacter)
	{
		pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
		if (!pcsAgpdCharacter)		return FALSE;
	}

	if (!pcsAgcdCharacter)
	{
		pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
		if (!pcsAgcdCharacter)		return FALSE;
	}

	pcsAgpdCharacter->m_alTargetCID[0] = m_lStartTargetID;
	memset(pcsAgpdCharacter->m_alTargetCID, 0, sizeof(INT32) * AGPDCHARACTER_MAX_TARGET);

	if (m_csCharacterOffset.m_bCharacterTarget)
	{
		pcsAgpdCharacter->m_alTargetCID[0]	= pcsAgpdCharacter->m_lID;
	}
	else
	{
		INT32	lOffset = m_lStartTargetID;

		for (INT32 lCount = 0; lCount < m_lNumTargets; ++lCount, --lOffset)
		{
			pcsAgpdCharacter->m_alTargetCID[lCount] = lOffset;
		}
	}

	return TRUE;
}

BOOL CModelToolApp::RemoveTargetCharacter()
{
	INT32	lOffset	= m_lStartTargetID;

	for (INT32 lCount = 0; lCount < m_lNumTargets; ++lCount, --lOffset)
	{
		if ( m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(lOffset) )
		{
			m_csAgcEngine.GetAgpmCharacterModule()->RemoveCharacter(lOffset);
		}
	}

	return TRUE;
}

BOOL CModelToolApp::MakeTargetCharacter()
{	
	CModelToolApp::GetInstance()->RemoveTargetCharacter();

	RwV3d	v3dPos	= {0.0f, 0.0f, 700.0f};
	RwV3dAdd(&v3dPos, &v3dPos, &m_v3dTargetPosOffset);

	RwV3d	v3dTemp;
	INT32	lTempX, lTempZ, lTempOffsetX, lTempOffsetZ;
	INT32	lOffset = m_lStartTargetID;

	for( INT32 lCount = 0; lCount < m_lNumTargets; ++lCount, --lOffset )
	{
		v3dTemp = v3dPos;

		lTempX = m_csRandom.randInt(1);
		lTempZ = m_csRandom.randInt(1);
		lTempOffsetX = m_csRandom.randInt(49);
		lTempOffsetZ = m_csRandom.randInt(49);

		if( lTempX )	v3dTemp.x	+= (lCount * lTempOffsetX);
		else			v3dTemp.x	-= (lCount * lTempOffsetX);

		if( lTempZ )	v3dTemp.z	+= (lCount * lTempOffsetZ);
		else			v3dTemp.z	-= (lCount * lTempOffsetZ);
			
		AgpdCharacter* pAgpdCharacter = SetCharacter( m_lTargetTID, NULL, FALSE, FALSE, lOffset, &v3dTemp );
		if( !pAgpdCharacter )		return FALSE;

		m_csAgcEngine.GetAgcmTextBoardModule()->DisableIDFlag( pAgpdCharacter, TRUE );
	}

	//if( !SetEmpty() )
	//	return FALSE;

	return TRUE;
}
