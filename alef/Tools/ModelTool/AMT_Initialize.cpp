#include "stdafx.h"
#include "ModelTool.h"
#include "AgcmItem.h"
#include "AgcdEffGlobal.h"
#include "ApModuleStream.h"

extern AuTickCounter	g_csTickCounter;

VOID CModelToolApp::FirstInitMember()
{
	InitializeMember();

	m_pcsStingDlg					= NULL;
	m_v3dTargetPosOffset.x			= 0.0f;
	m_v3dTargetPosOffset.y			= 0.0f;
	m_v3dTargetPosOffset.z			= 0.0f;
	m_lStartTargetID				= AMT_CHARACTER_TARGET_ID1;
	m_lTargetTID					= AMT_CHARACTER_TARGET_TID2;
	m_lNumTargets					= 1;
	m_lResourceMode					= ID_AMT_RESOURCE_TOOLBAR_DATA;
	m_uNationalCode					= 0;
}

VOID CModelToolApp::InitializeMember()
{
#ifdef _EXPORT
	m_bExport						= TRUE;
#else
	m_bExport						= FALSE;
#endif // _EXPORT

	m_bActiveWindows				= TRUE;
	m_pcsBottomClump				= NULL;
	m_pcsArmourClump1				= NULL;
	m_pcsArmourClump2				= NULL;
	m_pcsStaticModel				= NULL;
	m_pcsPickedAtomic				= NULL;
	m_hCurrentTreeItem				= NULL;
	m_lID							= 0;
	m_lTID							= 0;
	m_eObjectType					= AMT_OBJECT_TYPE_NONE;
	m_lVerNums						= 0;

	memset(m_aulLODDefaultDistance, 0, sizeof(UINT32) * 5);
	strcpy(m_szStaticModelDffName, "");

	AuPackingManager::MakeInstance();
	if (m_bExport)
	{
		AuPackingManager::GetSingletonPtr()->LoadReferenceFile(NULL);			// NULL이면 현재 디렉토리를 사용한다..
		AuPackingManager::GetSingletonPtr()->SetFilePointer();
	}
}

BOOL CModelToolApp::LoadToolData()
{
	const CHAR	*szValueName;
	CHAR		szValue[AMT_MAX_STR];

	ApModuleStream csStream;
	if (!csStream.Open(AMT_TOOL_DATA_PATH_NAME))
		return TRUE; // skip

	if (csStream.GetNumSections() != 1)
		return FALSE;

	csStream.ReadSectionName(0);
	csStream.SetValueID(-1);

	while (csStream.ReadNextValue())
	{
		szValueName = csStream.GetValueName();

		if (!strcmp(szValueName, AMT_TOOL_DATA_INI_NAME_CAMERA_OFFSET))
		{
			csStream.GetValue(szValue, AMT_MAX_STR);
			sscanf(szValue, "%f %f %f", &m_csCameraOffset.m_fRotate,
										&m_csCameraOffset.m_fZoom,
										&m_csCameraOffset.m_fMove		);
		}
		else if (!strcmp(szValueName, AMT_TOOL_DATA_INI_NAME_EDIT_OFFSET))
		{
			csStream.GetValue(szValue, AMT_MAX_STR);
			sscanf(szValue, "%f %f %f", &m_csEditOffset.m_fPosit,
										&m_csEditOffset.m_fRotate,
										&m_csEditOffset.m_fScale	);
		}
		else if (!strcmp(szValueName, AMT_TOOL_DATA_INI_NAME_CHARACTER_OFFSET))
		{
			csStream.GetValue(szValue, AMT_MAX_STR);
			sscanf(szValue, "%d", &m_csCharacterOffset.m_bCharacterTarget);
		}
		else if (!strcmp(szValueName, AMT_TOOL_DATA_INI_NAME_WINDOW_SIZE))
		{
			csStream.GetValue(szValue, AMT_MAX_STR);
			sscanf(szValue, "%d %d %d %d",	&m_rtInitWindow.left,
											&m_rtInitWindow.top,
											&m_rtInitWindow.right,
											&m_rtInitWindow.bottom	);
		}
		else if (!strcmp(szValueName, AMT_TOOL_DATA_INI_NAME_RENDER_OPTION))
		{
			/*

			2008. 06. 09. pm 15:49 by 성일

			Resource Tree Width 값이 저장되지 않는 문제 해결을 위해 다음과 같이 수정합니다.

			1. AMT_Initialize.cpp 의 CModelToolApp::LoadToolData() 에서 dwWidth 로 읽어들이던 값을
			   m_csRenderOption.m_dwResourceTreeWidth 로 직접 로드.
			2. AMT_ToolFunction.cpp 의 CModelToolApp::F_SetRenderOption() 함수에서 
			   m_csRenderOption.m_dwResourceTreeWidth 의 값을 계산해서 초기화 시키던 부분을 주석처리
		    3. CModelToolApp::F_SetRenderOption() 에서 Resource Form 의 크기를 조정하던 구문을
			   CModelToolApp::InitInstance() 에서 초기화 과정이 종료된 후 한번 실행해주도록 수정

			*/
			//DWORD dwWidth;
			csStream.GetValue(szValue, AMT_MAX_STR);
			sscanf(szValue, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d",
															&m_csRenderOption.m_bShowBlocking,
															&m_csRenderOption.m_bShowCameraPos,
															&m_csRenderOption.m_bShowCollisionClump,
															&m_csRenderOption.m_bShowPickClump,
															&m_csRenderOption.m_bShowFPS,
															&m_csRenderOption.m_bShowClumpPolygons,
															&m_csRenderOption.m_bShowClumpVertices,
															&m_csRenderOption.m_bShowTemp,
															&m_csRenderOption.m_bShowBoundingSphere,
															&m_csRenderOption.m_bMatD3DFx,
															&m_csRenderOption.m_bShowHitRange,
															&m_csRenderOption.m_bShowSiegeWarCollObj,
															&m_csRenderOption.m_bEnableSound,
															&m_csRenderOption.m_dwResourceTreeWidth );
															//&dwWidth );
		}
		else if (!strcmp(szValueName, AMT_TOOL_DATA_INI_NAME_TARGET_OPTION))
		{
			csStream.GetValue(szValue, AMT_MAX_STR);
			sscanf(szValue, "%d %d %f %f %f",	&m_lTargetTID,
												&m_lNumTargets,
												&m_v3dTargetPosOffset.x,
												&m_v3dTargetPosOffset.y,
												&m_v3dTargetPosOffset.z		);
		}
		else if ( !strcmp(szValueName, AMT_TOOL_DATA_INI_NAME_NATIONAL_CODE) )
		{
			csStream.GetValue(szValue, AMT_MAX_STR);
			sscanf(szValue, "%d", &m_uNationalCode);

			m_bNationalCodeChanged = FALSE;
		}
	}

	return TRUE;
}

BOOL CModelToolApp::LoadResourceData()
{
	if (!LoadStaticData())
	{
		CModelToolDlg::GetInstance()->MessageBox("Tool을 종료합니다!\nFAILED LoadStaticData()", "ERROR", MB_ICONERROR | MB_OK);
		return FALSE;
	}

	if (!LoadTemplateData())
	{
		CModelToolDlg::GetInstance()->MessageBox("Tool을 종료합니다!\nFAILED LoadTemplateData()", "ERROR", MB_ICONERROR | MB_OK);
		return FALSE;
	}

	if (!LoadLODList())
	{
		CModelToolDlg::GetInstance()->MessageBox("Tool을 종료합니다!\nFAILED LoadLODList()", "ERROR", MB_ICONERROR | MB_OK);
		return FALSE;
	}

	if (!LoadGeometryDataFile())
	{
		CModelToolDlg::GetInstance()->MessageBox("Tool을 종료합니다!\nFAILED LoadGeometryDataFile()", "ERROR", MB_ICONERROR | MB_OK);
		return FALSE;
	}

	if (!LoadEtcFile())
	{
		CModelToolDlg::GetInstance()->MessageBox("Tool을 종료합니다!\nFAILED LoadEtcFile()", "ERROR", MB_ICONERROR | MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL CModelToolApp::LoadEtcFile()
{
	CFileFind	csFind;
	if( csFind.FindFile() )
	{
		CHAR	szPathName[AMT_MAX_STR];
		sprintf(szPathName, "%s%s", AMT_INI_PATH, AMT_LOD_DISTANCE_TYPE_FILE_NAME);
		m_csAgcEngine.GetAgcmLODManagerModule()->ReadLODDistanceData( szPathName, m_bExport );
	}

	csFind.Close();

	return TRUE;
}

BOOL CModelToolApp::LoadStaticData()
{
	if( !InitStringDlg() )		return FALSE;

	m_pcsStingDlg->SetString( AMT_START_UP_DATA_STR );

	CResourceForm* pcsResourceForm = CModelToolDlg::GetInstance()->GetResourceForm();
	if( !pcsResourceForm )		return FALSE;

	CResourceTree* pcsTreeData = pcsResourceForm->GetTreeData();
	if( !pcsTreeData )			return FALSE;

	char pSavedPath[1024];
	GetCurrentDirectory( 1024, pSavedPath );
	ASSERT( pSavedPath[0] );

	char	drive[ 256 ], dir[ 256 ], fname[ 256 ], ext[ 256 ];
	pcsResourceForm->SetTreeHandleData1( pcsTreeData->_InsertItem( AMT_RESOURCE_TREE_NAME_DATA1, TVI_ROOT, E_AMT_ID_RESOURCE_TREE_DATA_STATIC_CHARACTER ) );
	_splitpath( AMT_CHARACTER_DATA_FIND_PATH2, drive, dir, fname, ext );
	SetCurrentDirectory( dir );
	pcsResourceForm->InsertItemEx( pcsTreeData, "*.rws", pcsResourceForm->GetTreeHandleData1(), 0, -1, -1 );
	pcsTreeData->Expand(pcsResourceForm->GetTreeHandleData1(), TVE_COLLAPSE);

	SetCurrentDirectory(pSavedPath);

	pcsResourceForm->SetTreeHandleData2( pcsTreeData->_InsertItem( AMT_RESOURCE_TREE_NAME_DATA2, TVI_ROOT, E_AMT_ID_RESOURCE_TREE_DATA_STATIC_OBJECT ) );
	_splitpath( AMT_OBJECT_DATA_FIND_PATH2, drive, dir, fname, ext );
	SetCurrentDirectory(dir);
	pcsResourceForm->InsertItemEx( pcsTreeData, "*.rws", pcsResourceForm->GetTreeHandleData2(), 0, -1, -1 );
	pcsTreeData->Expand(pcsResourceForm->GetTreeHandleData2(), TVE_COLLAPSE);

	SetCurrentDirectory(pSavedPath);

	ReleaseStringDlg();

	return TRUE;
}

BOOL CModelToolApp::LoadTemplateData()
{
	if(!LoadSkillTemplate())
		return FALSE;

	if(!LoadCharacterTemplate())
		return FALSE;

	if(!LoadItemTemplate())
		return FALSE;

	if(!LoadObjectTemplate())
		return FALSE;
	
	return TRUE;
}

BOOL CModelToolApp::LoadCharacterTemplate()
{
	InitStringDlg();
	m_pcsStingDlg->SetString(AMT_START_UP_CHARACTER_TEMPLATE_STR);

	if (!CModelToolDlg::GetInstance())
	{
		ReleaseStringDlg();
		return FALSE;
	}

	CResourceForm		*pcsResourceForm	= CModelToolDlg::GetInstance()->GetResourceForm();
	if (!pcsResourceForm)
	{
		CModelToolDlg::GetInstance()->MessageBox("GetResourceForm Error", "ERROR", MB_ICONERROR | MB_OK);
		ReleaseStringDlg();
		return FALSE;
	}

	CResourceTree		*pcsTreeCharacter	= pcsResourceForm->GetTreeCharacter();
	if (!pcsTreeCharacter)
	{
		CModelToolDlg::GetInstance()->MessageBox("GetTreeCharacter Error", "ERROR", MB_ICONERROR | MB_OK);
		ReleaseStringDlg();
		return FALSE;
	}

	//@{ 2006/05/05 burumal	
	//pcsTreeCharacter->DeleteAllItems();
	pcsTreeCharacter->_DeleteAllItems();
	//@}

	BOOL bSetLabel	= pcsResourceForm->ReadCharacterLabel( AMT_CHARACTER_LABEL_TXT_PATH_NAME );
	if ( !bSetLabel )
		pcsResourceForm->SetTreeHandleCharacter(pcsTreeCharacter->_InsertItem(AMT_RESOURCE_TREE_NAME_CHARACTER, TVI_ROOT, E_AMT_ID_RESOURCE_TREE_DATA_CHARACTER));

	/*
	if( bError ) {
		DebugString( "%s", szOutput );
		return FALSE;
	}
	*/

	switch( _LoadCharacterTemplate() )
	{
	case TSO::LE_NOERROR:
		break;
	default:
	case TSO::LE_READFAILD:
		{
			::MessageBox( NULL , "읽다가 중간에 에러났어요", "모델툴" , MB_OK );
			return FALSE;
		}
		break;
	case TSO::LE_NO_SEPARATED_TEMPLATE:
		{
			::MessageBox( NULL , "이전 케릭터 템플릿를 읽어 들입니다. 저장할때는 새 포멧으로 저장하게 됩니다", "모델툴" , MB_OK );
			if( ! _LoadCharacterTemplateOld() ) return FALSE;
		}
		break;
	}

	INT32 lIndex = 0;
	for (	AgpdCharacterTemplate *pcsTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex);
			pcsTemplate;
			pcsTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex))
	{
		AgcdCharacterTemplate *pcsAgcdCharacterTemplate = m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(pcsTemplate);
		HTREEITEM hParent;
		if (bSetLabel)
		{
			if (!pcsAgcdCharacterTemplate)
			{
				CHAR	szError[128];
				sprintf(szError, "GetTemplateData(%d) Error", pcsTemplate->m_lID);
				CModelToolDlg::GetInstance()->MessageBox(szError, "ERROR", MB_ICONERROR | MB_OK);
				ReleaseStringDlg();
				return FALSE;
			}

			if (pcsAgcdCharacterTemplate->m_pcsPreData)
			{
				if (!pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel)
				{
					CHAR	szError[128];
					sprintf(szError, "m_pcsPreData->m_pszLabel(%d) Error", pcsTemplate->m_lID);
					CModelToolDlg::GetInstance()->MessageBox(szError, "ERROR", MB_ICONERROR | MB_OK);
					ReleaseStringDlg();
					return FALSE;
				}

				CString		strText = pcsAgcdCharacterTemplate->m_pcsPreData->m_pszLabel;
				strText.Insert(0, "[");
				strText += "]";

				hParent = pcsTreeCharacter->_FindItemEx(strText);
				if (!hParent)
				{
					CHAR	szError[128];
					sprintf(szError, "Char_Label.txt와 일치하지 않습니다._FindItem(%d,%s) Error", pcsTemplate->m_lID, strText);
					CModelToolDlg::GetInstance()->MessageBox(szError, "ERROR", MB_ICONERROR | MB_OK);
					ReleaseStringDlg();
					return FALSE;
				}

				//sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_CHARACTER_TEMPLATE_STR, strText);

				// 쓸데없이 느려짐.
				// m_pcsStingDlg->SetString(pMsgBuff);
			}
		}
		else
			hParent = pcsResourceForm->GetTreeHandleCharacter();

		HTREEITEM hInsertedItem = pcsTreeCharacter->_InsertItem(pcsTemplate->m_szTName, hParent, (DWORD)(pcsTemplate->m_lID));
		if ( hInsertedItem )
			pcsTreeCharacter->SetTreeDNF_Flag( hInsertedItem, m_uNationalCode, pcsAgcdCharacterTemplate->m_nDNF );
	}

	ReleaseStringDlg();

	return TRUE;
}

BOOL CModelToolApp::LoadItemTemplate()
{
	InitStringDlg();
	m_pcsStingDlg->SetString(AMT_START_UP_ITEM_TEMPLATE_STR);
	
	AgpmItem* pAgpmItem = GetEngine()->GetAgpmItemModule();

	CResourceForm* pcsResourceForm = CModelToolDlg::GetInstance()->GetResourceForm();
	if(!pcsResourceForm)
	{
		ReleaseStringDlg();
		return FALSE;
	}

	CResourceTree* pcsTreeItem = pcsResourceForm->GetTreeItem();
	if(!pcsTreeItem)
	{
		ReleaseStringDlg();
		return FALSE;
	}
	pcsTreeItem->_DeleteAllItems();

	pcsResourceForm->SetTreeHandleItem( pcsTreeItem->_InsertItem( AMT_RESOURCE_TREE_NAME_ITEM, TVI_ROOT, E_AMT_ID_RESOURCE_TREE_DATA_ITEM ) );

	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_COMMON,		 pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_COMMON,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_MOONELF_SHA, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_MOONELF_SHA,pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_MOONELF_HUN, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_MOONELF_HUN,pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_MOONELF_SB,	 pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_MOONELF_SB ,pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_ORC_SHA,	 pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_ORC_SHA,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_ORC_WAR,	 pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_ORC_WAR,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_ORC_HUN,	 pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_ORC_HUN,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_HUMAN_ARC,   pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_HUMAN_ARC,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_HUMAN_KNI,   pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_HUMAN_KNI,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_HUMAN_WIZ,   pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_HUMAN_WIZ,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_DRAGONSCION_SCION,   pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_DRAGONSCION_SCION,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_DRAGONSCION_SLA,   pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_DRAGONSCION_SLA,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_DRAGONSCION_OBI,   pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_DRAGONSCION_OBI,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	pcsResourceForm->SetTreeHandleItemChar( CResourceForm::CHAR_DRAGONSCION_SUM,   pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_DRAGONSCION_SUM,	 pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_CHAR) );
	
	pcsResourceForm->SetTreeHandleEquip( CResourceForm::CHAR_COMMON, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_EQUIP,  pcsResourceForm->GetTreeHandleItemChar( CResourceForm::CHAR_COMMON ), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP));
	pcsResourceForm->SetTreeHandleArmour( CResourceForm::CHAR_COMMON, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_ARMOUR, pcsResourceForm->GetTreeHandleEquip( CResourceForm::CHAR_COMMON ),	E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_ARMOUR));

	pcsResourceForm->SetTreeHandleWeaponParent( pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_WEAPON,			pcsResourceForm->GetTreeHandleEquip( CResourceForm::CHAR_COMMON ), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
	pcsResourceForm->SetTreeHandlePcWeaponParent( pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_PC_WEAPON,		pcsResourceForm->GetTreeHandleWeaponParent(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
	pcsResourceForm->SetTreeHandleMonsterWeaponParent( pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_MONSTER_WEAPON,	pcsResourceForm->GetTreeHandleWeaponParent(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
	for( int i = 0; i < AGPMITEM_EQUIP_WEAPON_TYPE_NUM; i++ )
	{
		pcsResourceForm->SetTreeHandlePcWeapon( i, pcsTreeItem->_InsertItem( g_aszWeaponTypeName[i], pcsResourceForm->GetTreeHandlePcWeaponParent(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER ));
		pcsResourceForm->SetTreeHandleMonsterWeapon( i, pcsTreeItem->_InsertItem( g_aszWeaponTypeName[i], pcsResourceForm->GetTreeHandleMonsterWeaponParent(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER ));
	}

	pcsResourceForm->SetTreeHandleShield( pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_SHIELD, pcsResourceForm->GetTreeHandleEquip( CResourceForm::CHAR_COMMON ), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
	pcsResourceForm->SetTreeHandleRing( pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_RING, pcsResourceForm->GetTreeHandleEquip( CResourceForm::CHAR_COMMON ), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
	pcsResourceForm->SetTreeHandleNecklace( pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_NECKLACE, pcsResourceForm->GetTreeHandleEquip( CResourceForm::CHAR_COMMON ), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
	pcsResourceForm->SetTreeHandleRide( pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_RIDE, pcsResourceForm->GetTreeHandleEquip( CResourceForm::CHAR_COMMON ), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));

	for( int i = CResourceForm::CHAR_MOONELF_SHA; i < CResourceForm::CHAR_NUM; i++ )
	{
		pcsResourceForm->SetTreeHandleEquip( i, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_EQUIP,  pcsResourceForm->GetTreeHandleItemChar(i), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP));
		pcsResourceForm->SetTreeHandleArmour( i, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_ARMOUR, pcsResourceForm->GetTreeHandleEquip(i),  E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_ARMOUR));

		pcsResourceForm->SetTreeHandleBody( i, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_BODY,   pcsResourceForm->GetTreeHandleArmour(i), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
		pcsResourceForm->SetTreeHandleHead( i, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_HEAD,   pcsResourceForm->GetTreeHandleArmour(i), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
		pcsResourceForm->SetTreeHandleArms( i, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_ARMS,   pcsResourceForm->GetTreeHandleArmour(i), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
		pcsResourceForm->SetTreeHandleHands( i, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_HANDS,  pcsResourceForm->GetTreeHandleArmour(i), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
		pcsResourceForm->SetTreeHandleLegs( i, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_LEGS,   pcsResourceForm->GetTreeHandleArmour(i), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
		pcsResourceForm->SetTreeHandleFoot( i, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_FOOT,   pcsResourceForm->GetTreeHandleArmour(i), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
		pcsResourceForm->SetTreeHandleArms2( i, pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_ARMS2,  pcsResourceForm->GetTreeHandleArmour(i), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_EQUIP_OTHER));
	}

	pcsResourceForm->SetTreeHandleUsable(pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_USABLE, pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_USABLE));
	pcsResourceForm->SetTreeHandleOther(pcsTreeItem->_InsertItem(AMT_RESOURCE_TREE_NAME_OTHER, pcsResourceForm->GetTreeHandleItem(), E_AMT_ID_RESOURCE_TREE_DATA_ITEM_OTHER));

	// ItemTemplate 새 형식으로 저장하지 않음..마고자 2008/04/24
	switch( TSO::LE_NO_SEPARATED_TEMPLATE ) //TSO::LoadItemTemplateSeperated( m_csAgcEngine , "ini\\ItemTemplate" , MessageBoxLogger()  )  )
	{
	case TSO::LE_NO_SEPARATED_TEMPLATE:
		{
			// ::MessageBox( NULL , "이전 아이템 템플릿를 읽어 들입니다. 저장할때는 새 포멧으로 저장하게 됩니다", "모델툴" , MB_OK );

			CHAR szIniPathName[AMT_MAX_STR];
			sprintf(szIniPathName, "%s%s", AMT_INI_PATH, AMT_ITEM_TEMPLATE_INI_NAME);

			CHAR		szErrorCode[AMT_MAX_STR];
			CFileFind	csFind;
			if (csFind.FindFile(szIniPathName))
			{
				if ( !m_csAgcEngine.GetAgpmItemModule()->StreamReadTemplate( szIniPathName, szErrorCode, m_bExport ) )
				{
					ReleaseStringDlg();
					CModelToolDlg::GetInstance()->MessageBox(szErrorCode, "ERROR", MB_ICONERROR | MB_OK);
					CModelToolDlg::GetInstance()->MessageBox("아이템 템플릿 읽기 실패!", "ERROR", MB_ICONERROR | MB_OK);
					csFind.Close();
					return FALSE;
				}
			}
			else
				DebugString( "%s가 없습니다!!!\n", szIniPathName );

			csFind.Close();

			m_csAgcEngine.GetAgcmPreLODManagerModule()->ItemPreLODStreamRead(AMT_ITEM_PRE_LOD_INI_PATH_NAME);
			m_csAgcEngine.GetAgcmGeometryDataManagerModule()->ReadItemGeomDataFile(AMT_ITEM_GEOMETRY_DATA_INI_PATH_NAME);
		}
		break;
	case TSO::LE_NOERROR:
		break;
	case TSO::LE_READFAILD:
	default:
		{
			::MessageBox( NULL , "읽다가 중간에 에러났어요", "모델툴" , MB_OK );
			return FALSE;
		}
		break;
	}

	CHAR	szTemp[AMT_MAX_STR];
	m_csAgcEngine.GetAgpmItemModule()->StreamReadImportData(AMT_ITEM_DATA_TABLE_TXT_PATH_NAME, szTemp, m_bExport ? TRUE : FALSE);	

	char pMsgBuff[256];
	char szNewItemName[256];

	INT32 lIndex = 0;
	for(AgpdItemTemplate *pcsTemplate = m_csAgcEngine.GetAgpmItemModule()->GetTemplateSequence(&lIndex);
		pcsTemplate;
		pcsTemplate = m_csAgcEngine.GetAgpmItemModule()->GetTemplateSequence(&lIndex))
	{
		AgcdItemTemplate* pcsAgcdItemTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData( pcsTemplate );
		HTREEITEM hParent = CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeHandleItemParent( pcsTemplate, pcsAgcdItemTemplate );
		if( hParent )
		{
			sprintf(szNewItemName, "%s_%05d", pcsTemplate->m_szName, pcsTemplate->m_lID);
			HTREEITEM hInsertedItem = pcsTreeItem->_InsertItem(szNewItemName, hParent, (DWORD)(pcsTemplate->m_lID));
			if ( hInsertedItem )
				pcsTreeItem->SetTreeDNF_Flag(hInsertedItem, m_uNationalCode, pcsAgcdItemTemplate->m_nDNF);
			
			sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_ITEM_TEMPLATE_STR, pcsTemplate->m_szName);

			// 쓸데없이 느려짐..
			// m_pcsStingDlg->SetString(pMsgBuff);
		}
	}

	ReleaseStringDlg();

	return TRUE;
}

BOOL CModelToolApp::LoadObjectTemplate()
{
	InitStringDlg();
	m_pcsStingDlg->SetString(AMT_START_UP_OBJECT_TEMPLATE_STR);

	CResourceForm		*pcsResourceForm	= CModelToolDlg::GetInstance()->GetResourceForm();
	if(!pcsResourceForm)
	{
		ReleaseStringDlg();
		return FALSE;
	}

	CResourceTree		*pcsTreeObject		= pcsResourceForm->GetTreeObject();
	if(!pcsTreeObject)
	{
		ReleaseStringDlg();
		return FALSE;
	}

	BOOL		bSetLabel = TRUE;
	CHAR szErrorMessage[AMT_MAX_STR];
	{
		pcsTreeObject->_DeleteAllItems();

		if (!pcsResourceForm->ReadObjectLabel(AMT_OBJECT_CATEGORY_PATH_NAME))
		{
			bSetLabel = FALSE;
			pcsResourceForm->SetTreeHandleCharacter(pcsTreeObject->_InsertItem(AMT_RESOURCE_TREE_NAME_DATA2, TVI_ROOT, E_AMT_ID_RESOURCE_TREE_DATA_OBJECT));
		}
	}

	switch( TSO::LoadObjectTemplateSeperated( m_csAgcEngine , "ini\\ObjectTemplate" , MessageBoxLogger()  )  )
	{
	case TSO::LE_NOERROR:
		break;
	default:
	case TSO::LE_READFAILD:
		{
			::MessageBox( NULL , "읽다가 중간에 에러났어요", "모델툴" , MB_OK );
			return FALSE;
		}
		break;
	case TSO::LE_NO_SEPARATED_TEMPLATE:
		{
			::MessageBox( NULL , "이전 오브젝트 템플릿를 읽어 들입니다. 저장할때는 새 포멧으로 저장하게 됩니다", "모델툴" , MB_OK );

			CHAR szIniPathName[AMT_MAX_STR];
			sprintf(szIniPathName, "%s%s", AMT_INI_PATH, AMT_OBJECT_TEMPLATE_INI_NAME);

			CFileFind	csFind;
			if( csFind.FindFile(szIniPathName) )
			{
				if ( !m_csAgcEngine.GetApmObjectModule()->StreamReadTemplate( szIniPathName, NULL, NULL, szErrorMessage, m_bExport ) )
				{
					ReleaseStringDlg();
					CModelToolDlg::GetInstance()->MessageBox( szErrorMessage, "ERROR", MB_ICONERROR | MB_OK );
					CModelToolDlg::GetInstance()->MessageBox( "오브젝트 템플릿 읽기 실패!", "ERROR", MB_ICONERROR | MB_OK );
					csFind.Close();
					return FALSE;
				}
			}
			else
				DebugString( "%s가 없습니다!!!\n", szIniPathName );
			csFind.Close();
			m_csAgcEngine.GetAgcmPreLODManagerModule()->ObjectPreLODStreamRead(AMT_OBJECT_PRE_LOD_INI_PATH_NAME);
			m_csAgcEngine.GetAgcmGeometryDataManagerModule()->ReadObjGeomDataFile(AMT_OBJT_GEOMETRY_DATA_INI_PATH_NAME);
		}
	}

	{
		char	pMsgBuff[256];
		CHAR	szCategory[AMT_MAX_STR];
		INT32	lIndex = 0;

		for(ApdObjectTemplate *pcsTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex);
			pcsTemplate;
			pcsTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex))
		{
			AgcdObjectTemplate	*pcsAgcdObjectTemplate = m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(pcsTemplate);
			if(!pcsAgcdObjectTemplate)
			{
				ReleaseStringDlg();
				CModelToolDlg::GetInstance()->MessageBox("오브젝트 템플릿이 이상합니다!", "ERROR", MB_ICONERROR | MB_OK);
				return FALSE;
			}

			sprintf( szCategory, "[%s]", pcsAgcdObjectTemplate->m_szCategory );
			HTREEITEM hParent = pcsTreeObject->_FindItemEx( bSetLabel ? szCategory : AMT_RESOURCE_TREE_NAME_DATA2 );
			if(!hParent)
			{
				ReleaseStringDlg();

				sprintf(szErrorMessage, "%s 카테고리가 없습니다.", szCategory);
				CModelToolDlg::GetInstance()->MessageBox(szErrorMessage, "ERROR", MB_ICONERROR | MB_OK);
				return FALSE;
			}

			HTREEITEM hInsertedItem = pcsTreeObject->_InsertItem(pcsTemplate->m_szName, hParent, (DWORD)(pcsTemplate->m_lID));
			if ( hInsertedItem )
				pcsTreeObject->SetTreeDNF_Flag(hInsertedItem, m_uNationalCode, pcsAgcdObjectTemplate->m_nDNF);

			sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_OBJECT_TEMPLATE_STR, pcsTemplate->m_szName);

			// 쓸데없이 느려짐.
			//m_pcsStingDlg->SetString(pMsgBuff);
		}
	}

	//csFind.Close();

	ReleaseStringDlg();

	return TRUE;
}

BOOL CModelToolApp::LoadSkillTemplate()
{
	InitStringDlg();
	m_pcsStingDlg->SetString(AMT_START_UP_SKILL_TEMPLATE_STR);

	CResourceForm* pcsResourceForm = CModelToolDlg::GetInstance()->GetResourceForm();
	if( !pcsResourceForm )		return FALSE;

	CResourceTree* pcsTreeSkill = pcsResourceForm->GetTreeSkill();
	if( !pcsTreeSkill )			return FALSE;

	pcsTreeSkill->_DeleteAllItems();

	pcsResourceForm->SetTreeHandleSkill(pcsTreeSkill->_InsertItem(AMT_RESOURCE_TREE_NAME_SKILL, TVI_ROOT, E_AMT_ID_RESOURCE_TREE_DATA_SKILL));

	CHAR szIniPathName[AMT_MAX_STR];
	sprintf(szIniPathName, "%s%s", AMT_INI_PATH, AMT_SKILL_TEMPLATE_INI_NAME);
	
	char pMsgBuff[256];
	CFileFind	csFind;
	if( csFind.FindFile(szIniPathName) )
	{
		CHAR	szErrorCode[AMT_MAX_STR];
		if (!m_csAgcEngine.GetAgpmSkillModule()->StreamReadTemplate( szIniPathName,	szErrorCode, m_bExport ) )
		{
			CModelToolDlg::GetInstance()->MessageBox(szErrorCode, "ERROR", MB_ICONERROR | MB_OK);
			CModelToolDlg::GetInstance()->MessageBox("스킬 템플릿 읽기 실패!", "ERROR", MB_ICONERROR | MB_OK);
			csFind.Close();
			return FALSE;
		}

		HTREEITEM	hParent	= pcsResourceForm->GetTreeHandleSkill();
		INT32		lIndex	= 0;
		for(AgpdSkillTemplate *pcsTemplate = m_csAgcEngine.GetAgpmSkillModule()->GetSkillTemplateSequence(&lIndex);
			pcsTemplate;
			pcsTemplate = m_csAgcEngine.GetAgpmSkillModule()->GetSkillTemplateSequence(&lIndex)	)
		{
			HTREEITEM hInsertedItem = pcsTreeSkill->_InsertItem(pcsTemplate->m_szName, hParent, (DWORD)(pcsTemplate->m_lID));
			if ( hInsertedItem )
			{
				AgcdSkillTemplate* pcsAgcdSkillTemplate = m_csAgcEngine.GetAgcmSkillModule()->GetADSkillTemplate((PVOID)(pcsTemplate));
				pcsTreeSkill->SetTreeDNF_Flag(hInsertedItem, m_uNationalCode, pcsAgcdSkillTemplate->m_nDNF);
			}

			sprintf(pMsgBuff, "%s\n%s", AMT_START_UP_SKILL_TEMPLATE_STR, pcsTemplate->m_szName);

			// 쓸데없이 느려짐.
			//m_pcsStingDlg->SetString(pMsgBuff);
		}
	}
	else
		DebugString( "%s가 없습니다!!!\n", szIniPathName );

	csFind.Close();

	ReleaseStringDlg();

	return TRUE;
}

BOOL CModelToolApp::InitializeTitle()
{
	if(!CModelToolDlg::GetInstance())
		return FALSE;

	if(!CModelToolDlg::GetInstance()->GetTitleForm())
		return FALSE;
	
	char szTitle[256];
	sprintf(szTitle, "%s   {%s}", AMT_TITLE_NAME_INITIALIZE, g_pNationalCodeStr[m_uNationalCode]);
	CModelToolDlg::GetInstance()->GetTitleForm()->SetCurrentTitle(szTitle);

	return TRUE;
}

BOOL CModelToolApp::InitializeModule(CHAR *szDebug)
{
	g_csTickCounter.Initialize();

	if (!InitStringDlg())
	{
		strcpy(szDebug, "FAILED InitStringDlg()");
		return FALSE;
	}

	if (!GetRenderWare())
	{
		strcpy(szDebug, "FAILED GetRenderWare()");
		return FALSE;
	}

	char pMsgText[128];
	sprintf(pMsgText, "%s\nRegister modules...", AMT_START_UP_ADD_MODULE);
	m_pcsStingDlg->SetString(pMsgText);

	m_csAgcEngine.SetExport(m_bExport);

	if( !m_csAgcEngine.OnRegisterModule() )
	{
		::MessageBox( NULL , "Register Module Failed! Check Module List" , "ModelTool" , MB_OK );
		return FALSE;
	}

	sprintf(pMsgText, "%s\nSet module default properties...", AMT_START_UP_ADD_MODULE);
	m_pcsStingDlg->SetString(pMsgText);

	if (m_csAgcEngine.GetApmOcTreeModule())
		m_csAgcEngine.GetApmOcTreeModule()->m_bOcTreeEnable				= FALSE;

	if (m_csAgcEngine.GetAgcmResourceLoaderModule())
		m_csAgcEngine.GetAgcmResourceLoaderModule()->m_bForceImmediate	= TRUE;

	if (m_csAgcEngine.GetAgcmRenderModule())
	{
		m_csAgcEngine.GetAgcmRenderModule()->m_bUseCullMode				= FALSE;
		m_csAgcEngine.GetAgcmRenderModule()->SetMainFrame(RwCameraGetFrame(GetRenderWare()->GetCamera()));
		m_csAgcEngine.GetAgcmRenderModule()->SetWorldCamera(GetRenderWare()->GetWorld(), GetRenderWare()->GetCamera());
		m_csAgcEngine.GetAgcmRenderModule()->SetLight(GetRenderWare()->GetAmbient(), GetRenderWare()->GetDirection());
	}

	if( m_csAgcEngine.GetAgcmCustomizeRenderModule() )
	{
		m_csAgcEngine.GetAgcmCustomizeRenderModule()->SetWorld( GetRenderWare()->GetWorld() );
	}

	// AddModule이외의 초기화들
	if (m_csAgcEngine.GetAgpmCharacterModule())
	{
		m_csAgcEngine.GetAgpmCharacterModule()->SetMaxCharacter(AMT_CHAR_NUM);
		m_csAgcEngine.GetAgpmCharacterModule()->SetMaxCharacterTemplate(AMT_CHAR_TEMPLATE_NUM);
	}

	if (m_csAgcEngine.GetAgpmItemModule())
	{
		m_csAgcEngine.GetAgpmItemModule()->SetMaxItem(AMT_ITEM_NUM);
		m_csAgcEngine.GetAgpmItemModule()->SetMaxItemTemplate(20000);
	}

	if (m_csAgcEngine.GetApmObjectModule())
	{
		m_csAgcEngine.GetApmObjectModule()->SetMaxObject(AMT_OBJECT_NUM);
		m_csAgcEngine.GetApmObjectModule()->SetMaxObjectTemplate(AMT_OBJECT_TEMPLATE_NUM);
	}

	if (m_csAgcEngine.GetAgpmSkillModule())
	{
		m_csAgcEngine.GetAgpmSkillModule()->SetMaxSkill(AMT_SKILL_NUM);
		m_csAgcEngine.GetAgpmSkillModule()->SetMaxSkillTemplate(AMT_SKILL_TEMPLATE_NUM);
	}

	if (m_csAgcEngine.GetAgcmObjectModule())
	{
		if (m_bExport)
			m_csAgcEngine.GetAgcmObjectModule()->AddFlags(E_AGCM_OBJECT_FLAGS_EXPORT);

		m_csAgcEngine.GetAgcmObjectModule()->SetMaxAnimation(AMT_OBJ_ANIMATION_NUM);
		m_csAgcEngine.GetAgcmObjectModule()->SetClumpPath(AMT_OBJECT_DATA_PATH);
		m_csAgcEngine.GetAgcmObjectModule()->SetRpWorld(GetRenderWare()->GetWorld());
		m_csAgcEngine.GetAgcmObjectModule()->SetAnimationPath(AMT_OBJECT_ANIMATION_DATA_PATH);
		m_csAgcEngine.GetAgcmObjectModule()->SetupSystemObject(TRUE);
		m_csAgcEngine.GetAgcmObjectModule()->SetTexturePath(AMT_OBJECT_TEXTURE_PATH);
	}

	if (m_csAgcEngine.GetAgcmCharacterModule())
	{
		if (m_bExport)
			m_csAgcEngine.GetAgcmCharacterModule()->AddFlags(E_AGCM_CHAR_FLAGS_EXPORT);

		m_csAgcEngine.GetAgcmCharacterModule()->SetMaxAnimations(AMT_CHAR_ANIMATION_NUM);
		m_csAgcEngine.GetAgcmCharacterModule()->SetTexturePath(AMT_CHARACTER_TEXTURE_PATH);
		m_csAgcEngine.GetAgcmCharacterModule()->SetWorld(GetRenderWare()->GetWorld(), GetRenderWare()->GetCamera());
		m_csAgcEngine.GetAgcmCharacterModule()->SetClumpPath(AMT_CHARACTER_DATA_PATH);
		m_csAgcEngine.GetAgcmCharacterModule()->SetAnimationPath(AMT_CHARACTER_ANIMATION_DATA_PATH);
		m_csAgcEngine.GetAgcmCharacterModule()->SetCallbackChangeAnim(CharacterChangeAnimCB, this);
		m_csAgcEngine.GetAgcmCharacterModule()->SkipSearchSituation();
	}

	if (m_csAgcEngine.GetAgcmItemModule())
	{
		if (m_bExport)
			m_csAgcEngine.GetAgcmItemModule()->AddFlags(E_AGCM_ITEM_FLAGS_EXPORT);

		m_csAgcEngine.GetAgcmItemModule()->SetWorld(GetRenderWare()->GetWorld());
		m_csAgcEngine.GetAgcmItemModule()->SetClumpPath(AMT_CHARACTER_DATA_PATH);
		m_csAgcEngine.GetAgcmItemModule()->SetMaxItemClump(AMT_ITEM_CLUMP_NUM);
		m_csAgcEngine.GetAgcmItemModule()->SetTexturePath(AMT_CHARACTER_TEXTURE_PATH);
		m_csAgcEngine.GetAgcmItemModule()->SetIconTexturePath(AMT_ITEM_TEXTURE_PATH);
	}

	if (m_csAgcEngine.GetAgcmEventEffectModule())
	{
		m_csAgcEngine.GetAgcmEventEffectModule()->SetSoundDataNum(AMT_EFFECT_SOUND_NUM);
		m_csAgcEngine.GetAgcmEventEffectModule()->SetForceStereoSound();
	}

	if (m_csAgcEngine.GetAgcmLODDlgModule())
	{
		if (!m_bExport)
		{
			m_csAgcEngine.GetAgcmLODDlgModule()->SetCallbackApplyLOD(ReloadObjectCB, this);
			m_csAgcEngine.GetAgcmLODDlgModule()->SetDefaultLODRange(m_aulLODDefaultDistance);
		}
	}

	if (m_csAgcEngine.GetAgcmBlockingDlgModule())
	{
		m_csAgcEngine.GetAgcmBlockingDlgModule()->SetCallbackInitBlocking(InitBlockingCB, this);
		m_csAgcEngine.GetAgcmBlockingDlgModule()->SetCallbackFinishUpEditingBlocking(FinishUpEditingBlockingCB, this);
	}
 
	if (m_csAgcEngine.GetAgcmAnimationDlgModule())
	{
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetFindCharAnimSoundPath(AMT_CHARACTER_ANIMATION_SOUND_FILE_FIND_PATH);
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetFindCharAnimPath(AMT_CHARACTER_ANIMATION_DATA_FIND_PATH1, AMT_CHARACTER_ANIMATION_DATA_FIND_PATH2);
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetCharAnimPath(AMT_CHARACTER_ANIMATION_DATA_PATH);
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetCallbackCharStartAnimation(StartAnimationCB, this);
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetCallbackCharGetAnimation(GetAnimationCB, this);

		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetCallbackReadRtAnim(ReadRtAnimCB, this);
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetCallbackAddAnimation(AddAnimationCB, this);
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetCallbackAddAnimData(AddAnimDataCB, this);
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetCallbackRemoveAnimData(RemoveAnimDataCB, this);
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetCallbackRemoveAnimation(RemoveAnimationCB, this);
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetCallbackRemoveAllAnimation(RemoveAllAnimationCB, this);
		m_csAgcEngine.GetAgcmAnimationDlgModule()->SetCallbackStartAnimation(StartPureAnimCB, this);
	}

	if (m_csAgcEngine.GetAgcmCharacterDlgModule())
	{
		m_csAgcEngine.GetAgcmCharacterDlgModule()->SetClumpFindPathName(AMT_CHARACTER_DATA_FIND_PATH1, AMT_CHARACTER_DATA_FIND_PATH2);
		m_csAgcEngine.GetAgcmCharacterDlgModule()->SetCallbackSetCharacterHeight(SetCharacterHeightCB, this);
		m_csAgcEngine.GetAgcmCharacterDlgModule()->SetCallbackSetCharacterDepth(SetCharacterDepthCB, this);	//. 2005. 10. 07. Nonstopdj depth값 반환 cb등록
		m_csAgcEngine.GetAgcmCharacterDlgModule()->SetCallbackSetCharacterHitRange(SetCharacterHitRangeCB, this);
		m_csAgcEngine.GetAgcmCharacterDlgModule()->SetCallbackSetCharacterBSphere(GenerateBoundingSphereCB, this);
	}

	if (m_csAgcEngine.GetAgcmItemDlgModule())
	{
		m_csAgcEngine.GetAgcmItemDlgModule()->SetClumpFindPathName(AMT_CHARACTER_DATA_FIND_PATH1, AMT_CHARACTER_DATA_FIND_PATH2);
		m_csAgcEngine.GetAgcmItemDlgModule()->SetTextureFindPathName(AMT_ITEM_TEXTURE_FIND_PATH1, AMT_ITEM_TEXTURE_FIND_PATH2, AMT_ITEM_TEXTURE_FIND_PATH3);
		m_csAgcEngine.GetAgcmItemDlgModule()->SetCallbackGenerateBSphere(GenerateBoundingSphereCB, this);
	}

	if (m_csAgcEngine.GetAgcmObjectDlgModule())
	{
		m_csAgcEngine.GetAgcmObjectDlgModule()->SetClumpFindPathName(AMT_OBJECT_DATA_FIND_PATH1, AMT_OBJECT_DATA_FIND_PATH2);
		m_csAgcEngine.GetAgcmObjectDlgModule()->SetObjectCategoryPathName(AMT_OBJECT_CATEGORY_PATH_NAME);
		m_csAgcEngine.GetAgcmObjectDlgModule()->SetAnimationFindPathName(AMT_OBJECT_ANIMATION_DATA_FIND_PATH1, AMT_OBJECT_ANIMATINO_DATA_FIND_PATH2);
		m_csAgcEngine.GetAgcmObjectDlgModule()->SetCallbackGetAnimData(ObjectGetAnimDataCB, this);
		m_csAgcEngine.GetAgcmObjectDlgModule()->SetCallbackGenerateBSphere(GenerateBoundingSphereCB, this);

		m_csAgcEngine.GetAgcmObjectDlgModule()->SetCallbackAddAnimation(AddAnimationCB, this);
		m_csAgcEngine.GetAgcmObjectDlgModule()->SetCallbackReadRtAnim(ReadRtAnimCB, this);
		m_csAgcEngine.GetAgcmObjectDlgModule()->SetCallbackRemoveAllAnimation(RemoveAllAnimationCB, this);
	}

	if (m_csAgcEngine.GetAgcmSkillDlgModule())
	{
		m_csAgcEngine.GetAgcmSkillDlgModule()->SetCallbackGetAnimation(GetSkillAnimationCB, this);
		m_csAgcEngine.GetAgcmSkillDlgModule()->SetCallbackStartSkillAnimation(StartSkillAnimationCB, this);
		m_csAgcEngine.GetAgcmSkillDlgModule()->SetCallbackStartSkillEffect(StartSkillEffectCB, this);
		m_csAgcEngine.GetAgcmSkillDlgModule()->SetTextureFindPathName(AMT_SKILL_TEXTURE_FIND_PATH1, AMT_SKILL_TEXTURE_FIND_PATH2, AMT_SKILL_TEXTURE_FIND_PATH3);

		m_csAgcEngine.GetAgcmSkillDlgModule()->SetCallbackAddAnimation(AddAnimationCB, this);
		m_csAgcEngine.GetAgcmSkillDlgModule()->SetCallbackStartAnim(StartAnimCB, this);
		m_csAgcEngine.GetAgcmSkillDlgModule()->SetCallbackRemoveAllAnimation(RemoveAllAnimationCB, this);
		m_csAgcEngine.GetAgcmSkillDlgModule()->SetCallbackReadRtAnim(ReadRtAnimCB, this);
	}

	if (m_csAgcEngine.GetAgcmLODManagerModule())
	{
		m_aulLODDefaultDistance[0] = 3000;
		m_aulLODDefaultDistance[1] = 6000;
		m_aulLODDefaultDistance[2] = 9000;
		m_aulLODDefaultDistance[3] = 120000;
		m_aulLODDefaultDistance[4] = 150000;
		m_csAgcEngine.GetAgcmLODManagerModule()->SetDefaultDistance(m_aulLODDefaultDistance);
	}

	if (m_csAgcEngine.GetAgcmEff2Module())
	{
		m_csAgcEngine.GetAgcmEff2Module()->bSetGlobalVariable( NULL, NULL, GetRenderWare()->GetWorld(), GetRenderWare()->GetCamera());
		if (!m_bExport)
			AgcdEffGlobal::bGetInst().bFlagOff(E_GFLAG_USEPACK);
	}

	if (m_csAgcEngine.GetAgcmSound())
	{
		m_csAgcEngine.GetAgcmSound()->m_b3DSampleEnable	= TRUE;
		m_csAgcEngine.GetAgcmSound()->m_bSampleEnable	= TRUE;
		m_csAgcEngine.GetAgcmSound()->m_bReadFromNotPacked = FALSE;
	}

	if (m_csAgcEngine.GetApmEventManagerModule())
		m_csAgcEngine.GetApmEventManagerModule()->SetMaxEvent(AMT_EVENT_NUM);

	if (m_csAgcEngine.GetAgcmGeometryDataManagerModule())
	{
		m_csAgcEngine.GetAgcmGeometryDataManagerModule()->AttachData(AMT_GEOM_DATA_PICKING);
		m_csAgcEngine.GetAgcmGeometryDataManagerModule()->AttachData(AMT_GEOM_DATA_COLLISION);
		m_csAgcEngine.GetAgcmGeometryDataManagerModule()->AttachData(AMT_GEOM_DATA_OCTREE);
	}	

	sprintf(pMsgText, "%s\nInitialize modules ...", AMT_START_UP_ADD_MODULE);
	m_pcsStingDlg->SetString(pMsgText);

	// 모듈을 초기화한당...
	m_csAgcEngine.Initialize();

	AcuFrameMemory::Create();
	AcuFrameMemory::InitManager(10000000);

	RwReal fTemp = 0.0f;
	RwRenderStateSet(rwRENDERSTATEFOGTYPE,		(void *)(rwFOGTYPEEXPONENTIAL));
	RwRenderStateSet(rwRENDERSTATEFOGCOLOR,		(void *)(RWRGBALONG(255, 255, 255, 255)));
	RwRenderStateSet(rwRENDERSTATEFOGDENSITY,	(void *)(&fTemp));

	RwRenderStateSet(rwRENDERSTATECULLMODE,		(void *)(rwCULLMODECULLNONE));

	CModelToolDlg::GetInstance()->GetResourceForm()->GetResourceSaver().Initialize( &m_csAgcEngine );
	return TRUE;
}

BOOL CModelToolApp::InitStringDlg()
{
	if(m_pcsStingDlg)
		return TRUE;

	if(!CModelToolDlg::GetInstance())
		return FALSE;

	m_pcsStingDlg = new CStringDlg();
	m_pcsStingDlg->Create();
	m_pcsStingDlg->ShowWindow(SW_SHOW);

	return TRUE;
}

VOID CModelToolApp::ReleaseStringDlg()
{
	if(m_pcsStingDlg)
	{
		m_pcsStingDlg->ShowWindow(SW_HIDE);
		m_pcsStingDlg->DestroyWindow();

		delete m_pcsStingDlg;
		m_pcsStingDlg = NULL;
	}
}

BOOL CModelToolApp::LoadLODList()
{
	if (!m_bExport)
	{
		/*
		m_csAgcEngine.GetAgcmPreLODManagerModule()->CharPreLODStreamRead(AMT_CHARACTER_PRE_LOD_INI_PATH_NAME);
		m_csAgcEngine.GetAgcmPreLODManagerModule()->ItemPreLODStreamRead(AMT_ITEM_PRE_LOD_INI_PATH_NAME);
		m_csAgcEngine.GetAgcmPreLODManagerModule()->ObjectPreLODStreamRead(AMT_OBJECT_PRE_LOD_INI_PATH_NAME);
		*/
	}

	return TRUE;
}

BOOL CModelToolApp::LoadGeometryDataFile()
{
	/*
	m_csAgcEngine.GetAgcmGeometryDataManagerModule()->ReadCharGeomDataFile(AMT_CHAR_GEOMETRY_DATA_INI_PATH_NAME);
	m_csAgcEngine.GetAgcmGeometryDataManagerModule()->ReadItemGeomDataFile(AMT_ITEM_GEOMETRY_DATA_INI_PATH_NAME);
	m_csAgcEngine.GetAgcmGeometryDataManagerModule()->ReadObjGeomDataFile(AMT_OBJT_GEOMETRY_DATA_INI_PATH_NAME);
	*/

	return TRUE;
}

BOOL CModelToolApp::ReloadAllTemplate(BOOL bCurSave)
{
	if(!ReloadCharacterTemplate(bCurSave))
		return FALSE;

	if(!ReloadItemTemplate(bCurSave))
		return FALSE;

	if(!ReloadObjectTemplate(bCurSave))
		return FALSE;

	return TRUE;
}

BOOL CModelToolApp::ReloadStaticData()
{
	CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeData()->_DeleteAllItems();	
	return LoadStaticData();
}

BOOL CModelToolApp::ReloadCharacterTemplate(BOOL bCurSave)
{
	if(bCurSave)
	{
		// 새 쎄이브.
		if( !_SaveCharacterTemplate() )
		{
			ReleaseStringDlg();
			return FALSE;
		}

		if (!CModelToolDlg::GetInstance()->GetResourceForm()->SaveCharacterLabel(AMT_CHARACTER_LABEL_TXT_PATH_NAME))
			return FALSE;

		/*
		m_csAgcEngine.GetAgcmPreLODManagerModule()->CharPreLODStreamWrite(AMT_CHARACTER_PRE_LOD_INI_PATH_NAME);
		m_csAgcEngine.GetAgcmGeometryDataManagerModule()->WriteItemGeomDataFile(AMT_CHAR_GEOMETRY_DATA_INI_PATH_NAME);
		*/
	}

	if (!ReleaseAllObject())
		return FALSE;	

	if (!m_csAgcEngine.GetAgpmCharacterModule()->RemoveAllCharacterTemplate())
		return FALSE;

	SetEmpty();

	if (!LoadCharacterTemplate())
		return FALSE;

	/*
	m_csAgcEngine.GetAgcmPreLODManagerModule()->CharPreLODStreamRead(AMT_CHARACTER_PRE_LOD_INI_PATH_NAME);
	m_csAgcEngine.GetAgcmGeometryDataManagerModule()->ReadCharGeomDataFile(AMT_CHAR_GEOMETRY_DATA_INI_PATH_NAME);

	if (!m_bExport)
		m_csAgcEngine.GetAgcmPreLODManagerModule()->CharPreLODStreamRead(AMT_CHARACTER_PRE_LOD_INI_PATH_NAME);
	*/

	return TRUE;
}

BOOL CModelToolApp::ReloadItemTemplate(BOOL bCurSave)
{
	if(bCurSave)
	{
		if(!m_csAgcEngine.GetAgpmItemModule()->StreamWriteTemplate(AMT_ITEM_TEMPLATE_INI_PATH_NAME, m_bExport))
			return FALSE;

		m_csAgcEngine.GetAgcmPreLODManagerModule()->ItemPreLODStreamWrite(AMT_ITEM_PRE_LOD_INI_PATH_NAME);
		m_csAgcEngine.GetAgcmGeometryDataManagerModule()->WriteItemGeomDataFile(AMT_ITEM_GEOMETRY_DATA_INI_PATH_NAME);

		//if( ! TSO::SaveItemTemplateSeperated( m_csAgcEngine , "ini\\ItemTemplate" , MessageBoxLogger()  ) ) return FALSE;
	}

	if(!ReleaseAllObject())
		return FALSE;	

	if(!m_csAgcEngine.GetAgpmItemModule()->DestroyAllTemplate())
		return FALSE;

	SetEmpty();

	if(!LoadItemTemplate())
		return FALSE;

	/*
	m_csAgcEngine.GetAgcmPreLODManagerModule()->ItemPreLODStreamRead(AMT_ITEM_PRE_LOD_INI_PATH_NAME);
	m_csAgcEngine.GetAgcmGeometryDataManagerModule()->ReadItemGeomDataFile(AMT_ITEM_GEOMETRY_DATA_INI_PATH_NAME);

	if (!m_bExport)
		m_csAgcEngine.GetAgcmPreLODManagerModule()->ItemPreLODStreamRead(AMT_ITEM_PRE_LOD_INI_PATH_NAME);
	*/

	return TRUE;
}

BOOL CModelToolApp::ReloadObjectTemplate(BOOL bCurSave)
{
	if(bCurSave)
	{
		if ((!CModelToolDlg::GetInstance()) || (!CModelToolDlg::GetInstance()->GetResourceForm()))
			return FALSE;

		if (!CModelToolDlg::GetInstance()->GetResourceForm()->SaveObjectLabel(AMT_OBJECT_CATEGORY_PATH_NAME))
			return FALSE;

		/*
		if(!m_csAgcEngine.GetApmObjectModule()->StreamWriteTemplate( AMT_OBJECT_TEMPLATE_INI_PATH_NAME, m_bExport ))
			return FALSE;

		m_csAgcEngine.GetAgcmPreLODManagerModule()->ObjectPreLODStreamWrite(AMT_OBJECT_PRE_LOD_INI_PATH_NAME);
		m_csAgcEngine.GetAgcmGeometryDataManagerModule()->WriteObjGeomDataFile(AMT_OBJT_GEOMETRY_DATA_INI_PATH_NAME);
		*/

		// 2008. 06. 10. pm 12:35 by 성일
		// 이 시점에서 템플릿을 로드하면 기존에 들고 있던 템플릿이 지워지지 않은 상태이므로
		// 로딩에 실패하게 됨.. 어차피 아래에서 비우고 로드하는 작업이 있으므로 이 코드를 주석처리함
		//if( !TSO::LoadObjectTemplateSeperated( m_csAgcEngine , "ini\\ObjectTemplate" , MessageBoxLogger()  ) )
		//	return FALSE;
	}

	if (!ReleaseAllObject())
		return FALSE;

	if (!m_csAgcEngine.GetApmObjectModule()->DestroyAllTemplate())
		return FALSE;

	SetEmpty();

	if (!LoadObjectTemplate())
		return FALSE;

	/*
	m_csAgcEngine.GetAgcmPreLODManagerModule()->ObjectPreLODStreamRead(AMT_OBJECT_PRE_LOD_INI_PATH_NAME);
	m_csAgcEngine.GetAgcmGeometryDataManagerModule()->ReadObjGeomDataFile(AMT_OBJT_GEOMETRY_DATA_INI_PATH_NAME);

	if (!m_bExport)
		m_csAgcEngine.GetAgcmPreLODManagerModule()->ObjectPreLODStreamRead(AMT_OBJECT_PRE_LOD_INI_PATH_NAME);
		*/

	return TRUE;
}

BOOL CModelToolApp::ReloadSkillTemplate(BOOL bCurSave)
{
	if(bCurSave)
		if(!m_csAgcEngine.GetAgpmSkillModule()->StreamWriteTemplate(AMT_SKILL_TEMPLATE_INI_PATH_NAME, m_bExport))
			return FALSE;

	if(!ReleaseAllObject())
		return FALSE;

	if(!m_csAgcEngine.GetAgpmSkillModule()->RemoveAllSkillTemplate())
		return FALSE;

	SetEmpty();

	return LoadSkillTemplate();
}

//@{ 2006/05/04 burumal
void CModelToolApp::ClearAllTreeState(CResourceTree* pResTree)
{
	HTREEITEM	hTemp;

	HTREEITEM hItem = pResTree->GetRootItem();
	while (hItem)
	{
		while (hItem)
		{			
			pResTree->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK);

			hTemp = pResTree->GetNextItem(hItem, TVGN_CHILD);
			if (!hTemp)		break;

			hItem = hTemp;
		}

		while (hItem)
		{
			hTemp = pResTree->GetNextItem(hItem, TVGN_NEXT);
			if (hTemp)
			{
				hItem = hTemp;
				break;
			}

			hItem = pResTree->GetParentItem(hItem);
			if (!hItem)		return;
		}
	}

}

void CModelToolApp::ResetAllResourceTreeDNF(UINT32 uCurNationalCode)
{
	ResetCharacterResourceTreeDNF(uCurNationalCode);
	ResetItemResourceTreeDNF(uCurNationalCode);
	ResetObjectResourceTreeDNF(uCurNationalCode);
	ResetSkillResourceTreeDNF(uCurNationalCode);
}

void CModelToolApp::ResetCharacterResourceTreeDNF(UINT32 uCurNationalCode)
{
	CResourceForm* pcsResourceForm	= CModelToolDlg::GetInstance()->GetResourceForm();
	if( !pcsResourceForm )		return;

	CResourceTree* pcsTreeCharacter	= pcsResourceForm->GetTreeCharacter();
	if( !pcsTreeCharacter )		return;

	ClearAllTreeState(pcsTreeCharacter);

	INT32 lIndex = 0;
	for (AgpdCharacterTemplate* pcsTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex);
		pcsTemplate;
		pcsTemplate = m_csAgcEngine.GetAgpmCharacterModule()->GetTemplateSequence(&lIndex))
	{	
		HTREEITEM hSelectedItem = pcsTreeCharacter->_FindItemEx(pcsTemplate->m_szTName);
		if ( !hSelectedItem )	continue;

		AgcdCharacterTemplate*	pcsAgcdCharacterTemplate = m_csAgcEngine.GetAgcmCharacterModule()->GetTemplateData(pcsTemplate);
		pcsTreeCharacter->SetTreeDNF_Flag(hSelectedItem, m_uNationalCode, pcsAgcdCharacterTemplate->m_nDNF);
	}	
}

void CModelToolApp::ResetItemResourceTreeDNF(UINT32 uCurNationalCode)
{
	CResourceForm* pcsResourceForm	= CModelToolDlg::GetInstance()->GetResourceForm();
	if( !pcsResourceForm )		return;

	CResourceTree* pcsTreeItem = pcsResourceForm->GetTreeItem();
	if( !pcsTreeItem )			return;

	ClearAllTreeState(pcsTreeItem);

	INT32 lIndex = 0;
	for(AgpdItemTemplate* pcsTemplate = m_csAgcEngine.GetAgpmItemModule()->GetTemplateSequence(&lIndex);
		pcsTemplate;
		pcsTemplate = m_csAgcEngine.GetAgpmItemModule()->GetTemplateSequence(&lIndex))
	{
		char szNewItemName[256];
		sprintf(szNewItemName, "%s_%05d", pcsTemplate->m_szName, pcsTemplate->m_lID);
		HTREEITEM hSelectedItem = pcsTreeItem->_FindItemEx(szNewItemName);
		if ( !hSelectedItem )	continue;

		AgcdItemTemplate* pcsAgcdItemTemplate = m_csAgcEngine.GetAgcmItemModule()->GetTemplateData( pcsTemplate );
		pcsTreeItem->SetTreeDNF_Flag(hSelectedItem, m_uNationalCode, pcsAgcdItemTemplate->m_nDNF);
	}
}

void CModelToolApp::ResetObjectResourceTreeDNF(UINT32 uCurNationalCode)
{
	CResourceForm* pcsResourceForm	= CModelToolDlg::GetInstance()->GetResourceForm();
	if( !pcsResourceForm )		return;

	CResourceTree* pcsTreeObject = pcsResourceForm->GetTreeObject();
	if( !pcsTreeObject )		return;

	ClearAllTreeState(pcsTreeObject);

	INT32 lIndex = 0;
	for ( ApdObjectTemplate* pcsTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex);
		pcsTemplate;
		pcsTemplate = m_csAgcEngine.GetApmObjectModule()->GetObjectTemplateSequence(&lIndex))
	{
		HTREEITEM hSelectedItem = pcsTreeObject->_FindItemEx(pcsTemplate->m_szName);			
		if ( !hSelectedItem )	continue;

		AgcdObjectTemplate* pcsAgcdObjectTemplate = m_csAgcEngine.GetAgcmObjectModule()->GetTemplateData(pcsTemplate);
		pcsTreeObject->SetTreeDNF_Flag(hSelectedItem, m_uNationalCode, pcsAgcdObjectTemplate->m_nDNF);
	}
}

void CModelToolApp::ResetSkillResourceTreeDNF(UINT32 uCurNationalCode)
{
	CResourceForm* pcsResourceForm	= CModelToolDlg::GetInstance()->GetResourceForm();
	if( !pcsResourceForm )		return;

	CResourceTree* pcsTreeSkill		= pcsResourceForm->GetTreeSkill();
	if( !pcsTreeSkill )			return;

	ClearAllTreeState(pcsTreeSkill);

	INT32 lIndex = 0;
	for(AgpdSkillTemplate *pcsTemplate = m_csAgcEngine.GetAgpmSkillModule()->GetSkillTemplateSequence(&lIndex);
		pcsTemplate;
		pcsTemplate = m_csAgcEngine.GetAgpmSkillModule()->GetSkillTemplateSequence(&lIndex)	)
	{	
		HTREEITEM hSelectedItem = pcsTreeSkill->_FindItemEx(pcsTemplate->m_szName);
		if ( !hSelectedItem )	continue;

		AgcdSkillTemplate* pcsAgcdSkillTemplate = m_csAgcEngine.GetAgcmSkillModule()->GetADSkillTemplate((PVOID)(pcsTemplate));
		pcsTreeSkill->SetTreeDNF_Flag(hSelectedItem, m_uNationalCode, pcsAgcdSkillTemplate->m_nDNF);
	}	
}
