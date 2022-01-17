#include "stdafx.h"
#include "ModelTool.h"

#define fANGLE_OFFSET	2.0f

#define	fMAX_BENDING_OFFSET_UP	20.0f
#define	fMAX_BENDING_OFFSET_DN	50.0f

VOID CModelToolApp::UpdateApp()
{
	if( !CModelToolDlg::GetInstance() )						return;
	if( !CModelToolDlg::GetInstance()->GetRenderForm() )	return;

	m_csAgcEngine.OnIdle();		//FullUIModul을 만들어 줘야한당.. 
		
	CRenderWare* pcsRenderWare = GetRenderWare();
	if ( !pcsRenderWare )									return;

	pcsRenderWare->UpdateFPS();

	if( m_csRenderOption.m_bShowClumpPolygons )		UpdateClumpPolygons();
	if( m_csRenderOption.m_bShowClumpVertices )		UpdateClumpVertices();
	if( m_csRenderOption.m_bShowCameraPos )			UpdateCameraPosBuffer( pcsRenderWare );

	UpdateAnimationDlg();

	//[KTH] 일단 사용하지 않는것 같아서 막아둔다
	//AgpdCharacter* pAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter( m_lID );
	//if( !pAgpdCharacter )		return FALSE;
	//AgpdCharacter* pcsTargetCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter( pAgpdCharacter->m_alTargetCID[0] );
	//if( !pcsTargetCharacter )	return FALSE;
	//AgcdCharacter* pAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData( pAgpdCharacter );
	//if ( !pAgcdCharacter )		return FALSE;
	//AgcdCharacter* pAgcdTargetCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData( pcsTargetCharacter );
	//if ( !pAgcdCharacter )		return FALSE;

	//if ( GetAsyncKeyState(VK_CONTROL) < 0 )
	//{
	//	RwV3d vDir, vTempDir;
	//	vTempDir.x = pAgcdTargetCharacter->m_pPickAtomic->boundingSphere.center.x - pAgcdCharacter->m_pPickAtomic->boundingSphere.center.x;
	//	vTempDir.y = pAgcdTargetCharacter->m_pPickAtomic->boundingSphere.center.y - pAgcdCharacter->m_pPickAtomic->boundingSphere.center.y;
	//	vTempDir.z = pAgcdTargetCharacter->m_pPickAtomic->boundingSphere.center.z - pAgcdCharacter->m_pPickAtomic->boundingSphere.center.z;
	//	
	//	RwV3dNormalize(&vDir, &vTempDir);

	//	if ( GetAsyncKeyState(VK_DOWN) < 0 )
	//		pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree -= fANGLE_OFFSET;

	//	if ( GetAsyncKeyState(VK_UP) < 0 )
	//		pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree += fANGLE_OFFSET;
	//}
	//else
	//{
	//	if ( pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree > 0.0f )
	//	{
	//		pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree -= fANGLE_OFFSET;
	//		
	//		if ( pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree < 0.0f )
	//			pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree = 0.0f;
	//	}
	//	else
	//	if ( pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree < 0.0f )
	//	{
	//		pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree += fANGLE_OFFSET;
	//		if ( pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree > 0.0f )
	//			pAgcdCharacter->m_pstAgcdCharacterTemplate->m_fBendingDegree = 0.0f;
	//	}
	//}
}

VOID CModelToolApp::UpdateAnimationDlg()
{
	if( !CModelToolDlg::GetInstance()->IsOpenAnimationOptionDlg() )		return;
	if( m_eObjectType != AMT_OBJECT_TYPE_CHARACTER )		return;

	AgpdCharacter *pstAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
	if( !pstAgpdCharacter )		return;
	AgcdCharacter *pstAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pstAgpdCharacter);
	if( !pstAgcdCharacter )		return;
		
	if( pstAgcdCharacter->m_bStop )								return;
	if( !pstAgcdCharacter->m_pInHierarchy )						return;
	if( !pstAgcdCharacter->m_pInHierarchy->currentAnim )		return;

	CModelToolDlg::GetInstance()->UpdateAnimTime((INT32)(pstAgcdCharacter->m_pInHierarchy->currentAnim->currentTime * 100.0f));
}

VOID CModelToolApp::UpdateCameraPosBuffer(CRenderWare *pcsRenderWare)
{
	RwCamera* pCamera = pcsRenderWare->GetCamera();
	if( !pCamera )		return;

	CRwCamera	camera( pCamera );
	RwMatrix* matLTM = camera.GetLTM();
	if( !matLTM )		return;

	sprintf( m_szCameraPosBuffer, "%f %f %f", matLTM->pos.x, matLTM->pos.y, matLTM->pos.z );
}

VOID CModelToolApp::UpdateClumpPolygons()
{
	m_vPolyNums.clear();

	int nPolyNums;
	switch( m_eObjectType )
	{
	case AMT_OBJECT_TYPE_STATIC_MODEL:
		{
			//@{ 2006/07/11 burumal
			if ( !m_pcsStaticModel )
				return;
			//@}

			nPolyNums	= RwUtilClumpGetTriangles(m_pcsStaticModel);

			m_vPolyNums.push_back( nPolyNums );
		}
		break;

	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacter *pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
			if(!pcsAgpdCharacter)
			{
				nPolyNums = 0;
				return;
			}

			AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
			if((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump))
			{
				nPolyNums = 0;
				return;
			}

			nPolyNums	= RwUtilClumpGetTriangles(pcsAgcdCharacter->m_pClump);
			m_vPolyNums.push_back( nPolyNums );

			m_csAgcEngine.GetAgcmCharacterModule()->GetLodClumpPolyNum( m_vPolyNums, pcsAgpdCharacter );
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItem *pcsAgpdItem = m_csAgcEngine.GetAgpmItemModule()->GetItem(m_lID);
			if(!pcsAgpdItem)
			{
				nPolyNums = 0;
				return;
			}

			AgcdItem *pcsAgcdItem = m_csAgcEngine.GetAgcmItemModule()->GetItemData(pcsAgpdItem);
			if((!pcsAgcdItem) || (!pcsAgcdItem->m_pstClump))
			{
				nPolyNums = 0;
				return;
			}

			nPolyNums	= RwUtilClumpGetTriangles(pcsAgcdItem->m_pstClump);
			m_vPolyNums.push_back( nPolyNums );
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			nPolyNums = 0;

			ApdObject *pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObject(m_lID);
			if(!pcsApdObject)
			{
				return;
			}

			AgcdObject *pcsAgcdObject = m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pcsApdObject);
			if(!pcsAgcdObject)
			{
				return;
			}

			AgcdObjectGroupList	*pstList = pcsAgcdObject->m_stGroup.m_pstList;
			while(pstList)
			{
				if(!pstList->m_csData.m_pstClump)
					break;

				nPolyNums	+= RwUtilClumpGetTriangles(pstList->m_csData.m_pstClump);

				pstList = pstList->m_pstNext;
			}
			m_vPolyNums.push_back( nPolyNums );
		}
		break;

	default:
		{
			m_vPolyNums.clear();
		}
		break;
	}
}

VOID CModelToolApp::UpdateClumpVertices()
{
	switch(m_eObjectType)
	{
	case AMT_OBJECT_TYPE_STATIC_MODEL:
		{
			// @{ 2006/07/11
			if ( !m_pcsStaticModel )
				return;
			//@}

			m_lVerNums	= RwUtilClumpGetVertices(m_pcsStaticModel);
		}
		break;

	case AMT_OBJECT_TYPE_CHARACTER:
		{
			AgpdCharacter *pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
			if(!pcsAgpdCharacter)
			{
				m_lVerNums = 0;
				return;
			}

			AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
			if((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump))
			{
				m_lVerNums = 0;
				return;
			}

			m_lVerNums	= RwUtilClumpGetVertices(pcsAgcdCharacter->m_pClump);
		}
		break;

	case AMT_OBJECT_TYPE_ITEM:
		{
			AgpdItem *pcsAgpdItem = m_csAgcEngine.GetAgpmItemModule()->GetItem(m_lID);
			if(!pcsAgpdItem)
			{
				m_lVerNums = 0;
				return;
			}

			AgcdItem *pcsAgcdItem = m_csAgcEngine.GetAgcmItemModule()->GetItemData(pcsAgpdItem);
			if((!pcsAgcdItem) || (!pcsAgcdItem->m_pstClump))
			{
				m_lVerNums = 0;
				return;
			}

			m_lVerNums	= RwUtilClumpGetVertices(pcsAgcdItem->m_pstClump);
		}
		break;

	case AMT_OBJECT_TYPE_OBJECT:
		{
			m_lVerNums = 0;

			ApdObject *pcsApdObject = m_csAgcEngine.GetApmObjectModule()->GetObject(m_lID);
			if(!pcsApdObject)
			{
				return;
			}
			
			AgcdObject *pcsAgcdObject = m_csAgcEngine.GetAgcmObjectModule()->GetObjectData(pcsApdObject);
			if(!pcsAgcdObject)
			{
				return;
			}

			AgcdObjectGroupList	*pstList = pcsAgcdObject->m_stGroup.m_pstList;
			while(pstList)
			{
				if(!pstList->m_csData.m_pstClump)
					break;

				m_lVerNums	+= RwUtilClumpGetVertices(pstList->m_csData.m_pstClump);

				pstList = pstList->m_pstNext;
			}
		}
		break;

	default:
		{
			m_lVerNums = 0;
		}
		break;
	}
}

BOOL CModelToolApp::UpdateCharacterTemplate(AgpdCharacterTemplate *ppTemplate, AgcdCharacterTemplate *pcTemplate)
{
	CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeCharacter()->SetItemText( m_hCurrentTreeItem, ppTemplate->m_szTName );
	return TRUE;
}

BOOL CModelToolApp::UpdateItemTemplate(AgpdItemTemplate *ppTemplate, AgcdItemTemplate *pcTemplate)
{
	CResourceForm*	pcsResourceForm	= CModelToolDlg::GetInstance()->GetResourceForm();
	CResourceTree*	pcsTreeItem		= pcsResourceForm->GetTreeItem();

	pcsTreeItem->_DeleteItemEx(m_hCurrentTreeItem);
	HTREEITEM hParent = pcsResourceForm->GetTreeHandleItemParent( ppTemplate, pcTemplate );
	if( !hParent )		return FALSE;

	char szText[256];
	sprintf(szText, "%s_%05d", ppTemplate->m_szName, ppTemplate->m_lID);
	m_hCurrentTreeItem = pcsTreeItem->_InsertItem( szText, hParent, (DWORD)(ppTemplate->m_lID) );
	pcsTreeItem->SelectItem(m_hCurrentTreeItem);

	return TRUE;
}

BOOL CModelToolApp::UpdateObjectTemplate(ApdObjectTemplate *ppTemplate, AgcdObjectTemplate *pcTemplate)
{
	AgcdLOD* pstLOD	= &pcTemplate->m_stLOD;

	if(pstLOD->m_lNum != pcTemplate->m_stGroup.m_lNum)
	{
		if( pstLOD->m_lNum < pcTemplate->m_stGroup.m_lNum )
		{
			// 차이만큼 추가한다.
			for( INT32 lIndex = 0; lIndex < pcTemplate->m_stGroup.m_lNum - pstLOD->m_lNum; ++lIndex )
			{
				m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.AddLODData(pstLOD);
			}
		}
		else
		{
			// 차이만큼 삭제한다.
			for( INT32 lIndex = 0; lIndex < pstLOD->m_lNum - pcTemplate->m_stGroup.m_lNum; ++lIndex)
			{
				m_csAgcEngine.GetAgcmLODManagerModule()->m_csLODList.RemoveLODData(pstLOD, pstLOD->m_lNum - 1);
			}
		}
	}

	CHAR szTemp[AMT_MAX_STR];
	sprintf( szTemp, "[%s]", pcTemplate->m_szCategory );

	HTREEITEM hParent = CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject()->_FindItemEx(szTemp);
	if( !hParent )		return FALSE;

	CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject()->_DeleteItemEx( m_hCurrentTreeItem );
	m_hCurrentTreeItem = CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject()->_InsertItem( ppTemplate->m_szName, hParent, (DWORD)(ppTemplate->m_lID) );
	CModelToolDlg::GetInstance()->GetResourceForm()->GetTreeObject()->SelectItem(m_hCurrentTreeItem);

	return TRUE;
}
