#include "stdafx.h"
#include "ModelTool.h"

BOOL CModelToolApp::CheckArmour(RpAtomic *pstAtomic)
{
	return pstAtomic->iPartID >= AGPMITEM_PART_BODY && pstAtomic->iPartID <= AGPMITEM_PART_ARMS2 ? TRUE : FALSE;
}

BOOL CModelToolApp::CheckItemArmour()
{
	if ( AMT_OBJECT_TYPE_ITEM != m_eObjectType )				return FALSE;
		
	AgpdItemTemplate* pcsAgpdItemTemplate = (AgpdItemTemplate *)(m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(m_lTID));
	if (!pcsAgpdItemTemplate)									return FALSE;
	if (AGPMITEM_TYPE_EQUIP != pcsAgpdItemTemplate->m_nType )	return FALSE;

	return AGPMITEM_EQUIP_KIND_ARMOUR == ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind ? TRUE : FALSE;
}

BOOL CModelToolApp::CheckItemWeapon()
{
	if ( AMT_OBJECT_TYPE_ITEM != m_eObjectType )				return FALSE;

	AgpdItemTemplate	*pcsAgpdItemTemplate = (AgpdItemTemplate *)(m_csAgcEngine.GetAgpmItemModule()->GetItemTemplate(m_lTID));
	if (!pcsAgpdItemTemplate)									return FALSE;
	if (AGPMITEM_TYPE_EQUIP != pcsAgpdItemTemplate->m_nType )	return FALSE;

	return AGPMITEM_EQUIP_KIND_WEAPON == ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind ? TRUE : FALSE;
}

VOID CModelToolApp::SetInitTransform()
{
	if( m_pcsPickedAtomic )
		RwMatrixCopy( &m_pcsInitTransform, RwFrameGetMatrix( RpAtomicGetFrame( m_pcsPickedAtomic ) ) );
}

VOID CModelToolApp::InitTransformPickedAtomic()
{
	if( m_pcsPickedAtomic && RpAtomicGetFrame( m_pcsPickedAtomic ) )
		RwFrameTransform(RpAtomicGetFrame(m_pcsPickedAtomic), &m_pcsInitTransform, rwCOMBINEREPLACE);
}

BOOL CModelToolApp::ApplyTransform( RwV3d *pos, INT32 axis, RwReal degree, RwV3d *scale )
{
	if( !m_pcsPickedAtomic )	return FALSE;

	m_cRwUtil.ScaleAtomic( m_pcsPickedAtomic, *scale );
	m_cRwUtil.MouseRotAtomic( m_pcsPickedAtomic, axis, degree );
	m_cRwUtil.TransAtomic( m_pcsPickedAtomic, *pos );
	
	//RwFrame* pFrame = RpAtomicGetFrame(m_pcsPickedAtomic);
	//if(!pFrame)				return FALSE;

	//RwMatrix	*pMat	= RwFrameGetMatrix(pFrame);
	//RwV3d		BasePos = *RwMatrixGetPos(pMat);

	//RwV3dScale(&BasePos, &BasePos, -1.0f);
	//RwFrameTranslate(pFrame, &BasePos, rwCOMBINEPOSTCONCAT);

	//if(degree != 0.0f)
	//{
	//	RwV3d v3dAxis;

	//	if(axis == 0)
	//	{
	//		RwV3dAssign(&v3dAxis, &pMat->right);
	//	}
	//	else if(axis == 1)
	//	{
	//		RwV3dAssign(&v3dAxis, &pMat->up);
	//	}
	//	else if(axis == 2)
	//	{
	//		RwV3dAssign(&v3dAxis, &pMat->at);
	//	}
	//	else
	//	{
	//		RwV3dScale(&BasePos, &BasePos, -1.0f);
	//		RwFrameTranslate(pFrame, &BasePos, rwCOMBINEPOSTCONCAT);
	//		return FALSE;
	//	}

	//	RwFrameRotate(pFrame, &v3dAxis, degree, rwCOMBINEPOSTCONCAT);
	//}

	//RwV3dScale(&BasePos, &BasePos, -1.0f);
	//RwFrameTranslate(pFrame, &BasePos, rwCOMBINEPOSTCONCAT);

	//// translate
	//RwV3d	v3dTemp1 = {0, 0, 0};
	//RwV3d	v3dTemp2 = {0, 0, 0};
	//RwV3d	v3dTemp3 = {0, 0, 0};
	//RwV3d	v3dRst;

	//if(pos->x != 0)
	//	RwV3dScale(&v3dTemp1, &pMat->right, pos->x);
	//if(pos->y != 0)
	//	RwV3dScale(&v3dTemp2, &pMat->up, pos->y);
	//if(pos->z != 0)
	//	RwV3dScale(&v3dTemp3, &pMat->at, pos->z);

	//RwV3dAdd(&v3dRst, &v3dTemp1, &v3dTemp2);
	//RwV3dAdd(&v3dRst, &v3dRst, &v3dTemp3);

	//if(!RwUtilEqualV3d(&v3dRst, 0.0f, 0.0f, 0.0f))
	//	RwFrameTranslate(pFrame, &v3dRst, rwCOMBINEPOSTCONCAT);

	return TRUE;
}

RpAtomic *CModelToolApp::SaveTransformCB(RpAtomic *atomic, void *data)
{
	if( !atomic )		return NULL;

	RwFrame* pstFrame = RpAtomicGetFrame(atomic);
	if( !pstFrame )		return NULL;

	RwMatrix* pstMatrix = RwFrameGetMatrix(pstFrame);

	if( !data )			return NULL;
	CSaveTransformSet* pcsSet = (CSaveTransformSet*)(data);
	if( !pcsSet )		return NULL;

	RwInt32 lPartID = atomic->iPartID;
	if( lPartID == AGPMITEM_PART_HAND_RIGHT  || lPartID == AGPMITEM_PART_HAND_LEFT || lPartID == AGPMITEM_PART_LANCER )
	{
		AgpdGridItem* pcsAgpdGridItem = pcsSet->m_pcsAgpmItem->GetEquipItem( pcsSet->m_pcsAgpdCharacter, lPartID );
		if( !pcsAgpdGridItem )					return NULL;
			
		AgpdItem* pcsAgpdItem = pcsSet->m_pcsAgpmItem->GetItem( pcsAgpdGridItem->m_lItemID );
		if( !pcsAgpdItem->m_pcsItemTemplate )	return NULL;
			
		AgcdItemTemplate* pcsAgcdItemTemplate = pcsSet->m_pcsAgcmItem->GetTemplateData( (AgpdItemTemplate*)(pcsAgpdItem->m_pcsItemTemplate) );
		if( !pcsAgcdItemTemplate )				return NULL;

		ItemTransformInfo* pInfo = pcsAgcdItemTemplate->m_pItemTransformInfo->GetInfo( pcsSet->m_lCTID );
		if( !pInfo )
			pInfo = pcsAgcdItemTemplate->m_pItemTransformInfo->Insert( pcsSet->m_lCTID, pcsAgcdItemTemplate->m_lAtomicCount );
		
		INT16 nProcess = min( pcsSet->m_nCBCount2, pcsAgcdItemTemplate->m_lAtomicCount - 1 );

		pcsSet->m_nCBCount2++;
		RwMatrixCopy( pInfo->m_astTransform + nProcess, pstMatrix );
	}

	return atomic;
}

BOOL CModelToolApp::SaveTransform()
{
	AgpdCharacter *pcsAgpdCharacter = m_csAgcEngine.GetAgpmCharacterModule()->GetCharacter(m_lID);
	if((!pcsAgpdCharacter) || (!pcsAgpdCharacter->m_pcsCharacterTemplate))
		return FALSE;

	AgcdCharacter *pcsAgcdCharacter = m_csAgcEngine.GetAgcmCharacterModule()->GetCharacterData(pcsAgpdCharacter);
	if((!pcsAgcdCharacter) || (!pcsAgcdCharacter->m_pClump))
		return FALSE;

	CSaveTransformSet csSet;
	csSet.m_pcsAgpmItem			= m_csAgcEngine.GetAgpmItemModule();
	csSet.m_pcsAgcmItem			= m_csAgcEngine.GetAgcmItemModule();
	csSet.m_pcsAgpdCharacter	= pcsAgpdCharacter;
	csSet.m_pcsAgcdCharacter	= pcsAgcdCharacter;
	csSet.m_lCTID				= pcsAgpdCharacter->m_pcsCharacterTemplate->m_lID;

	RpClumpForAllAtomics( pcsAgcdCharacter->m_pClump, SaveTransformCB, (void *)(&csSet) );

	csSet.m_nCBCount1 = 0;
	csSet.m_nCBCount2 = 0;
	SetSaveDataCharacter();
	return TRUE;
}

void CModelToolApp::TransformPickedAtomic( const RwV2d& vec )
{
	if( !m_pcsPickedAtomic )	return;
		
	switch( CModelToolDlg::GetInstance()->EditEquipmentsDlgGetMouseMode() )
	{
	case EED_POSIT:
		m_cRwUtil.MouseTransAtomic( m_pcsPickedAtomic, vec );
		break;
	case EED_ROTATE:
		m_cRwUtil.MouseRotAtomic( m_pcsPickedAtomic, CModelToolDlg::GetInstance()->EditEquipmentsDlgGetMouseModeAxis(), vec.y );
		break;
	case EED_SCALE:
		m_cRwUtil.MouseScaleAtomic( m_pcsPickedAtomic, vec.y );
		break;
	}	
}

RpAtomic *CModelToolApp::CopyTransformCB(RpAtomic *atomic, void *data)
{
	CCopyTransformSet *pstSet = (CCopyTransformSet *)(data);
	
	if(pstSet->m_nPartID != atomic->iPartID)
		return atomic;

	if(!RpAtomicGetFrame(atomic))
		return FALSE;

	RwMatrix *pstSrcTransform = RwFrameGetMatrix(RpAtomicGetFrame(atomic));
	if(!pstSrcTransform)
		return FALSE;

	RwMatrixCopy( pstSrcTransform, pstSet->m_pstTransform + (pstSet->m_nAtomicCount - ++pstSet->m_nCBCount )	);
					

	return atomic;
}

