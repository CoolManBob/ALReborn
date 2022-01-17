#include "AgcmCharacter.h"
#include "AgcmTextBoard.h"
#include "AgcmCamera2.h"
#include "AgpmItem.h"
#include "AgcmItem.h"

const int AGCMCHAR_RIDENODE_ID	= 20;

namespace
{
	void SetAt( RwMatrix* pmtxResult, const RwV3d* pInAt )
	{
		RwV3d* pRight = RwMatrixGetRight( pmtxResult );
		RwV3d* pUp	  = RwMatrixGetUp( pmtxResult );
		RwV3d* pAt    =	RwMatrixGetAt( pmtxResult );

		RwV3d x, y, z;
		y.x = 0; y.y = 1.f; y.z = 0;
        RwV3dNormalize( &z, pInAt );
		RwV3dCrossProduct( &x, &y, &z );
		RwV3dNormalize( &x, &x );
		RwV3dCrossProduct( &y, &z, &x );

		pRight->x = x.x; pRight->y = x.y; pRight->z = x.z;
		pUp->x = y.x;	 pUp->y = y.y;	  pUp->z = y.z;
		pAt->x = z.x;	 pAt->y = z.y;	  pAt->z = z.z;
	}
}

INT32 AgcmCharacter::GenerateRideCID()
{
	AgpdCharacter	*pcsAgpdCharacter;
	INT32			nCount = AGCMCHAR_RIDE_CID_START;
	for( pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacter(nCount);
		 pcsAgpdCharacter;
		 pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacter(++nCount) );

	return nCount;
}

AgcdCharacter* AgcmCharacter::CreateRideCharacter( int nCID, int nTID )
{
//	PROFILE("AgpmCharacter::CreateRideCharacter");


//	return pcsAgcdCharacter;
	return NULL;
}

void AgcmCharacter::DestroyRideCharacter( AgcdCharacter* pcsAgcdCharacter )
{
	PROFILE("AgpmCharacter::DestroyRideCharacter");

	if( pcsAgcdCharacter == NULL )
		return;

	AgpdCharacter* pcsAgpdRideCharacter = GetCharacter( pcsAgcdCharacter->m_pRide );
    m_pcsAgpmCharacter->RemoveCharacter( pcsAgcdCharacter->m_nRideCID );
}

BOOL AgcmCharacter::AttachRide( AgcdCharacter* pstCharacter, int nTID )
{
	PROFILE("AgcmCharacter::AttachRide");

	if( pstCharacter == NULL )
		return TRUE;

	if( pstCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED )
		return TRUE;

	if( pstCharacter->m_pstAgcdCharacterTemplate == NULL )
		return TRUE;

	DetachRide( pstCharacter );

	int nCID				= GenerateRideCID();
	pstCharacter->m_nRideCID= nCID;

	AgpdCharacter *pcsAgpdCharacter = GetCharacter( pstCharacter );

	char szGameID[256];
	sprintf( szGameID, "RIDE%04d", nCID );
	AgpdCharacter *pcsAgpdCharacterRide = m_pcsAgpmCharacter->AddCharacter( nCID, nTID, szGameID );
	if( pcsAgpdCharacterRide == NULL )
		return FALSE;

	pcsAgpdCharacter->m_nRideCID = pcsAgpdCharacterRide->m_lID;
	AgcdCharacter *pcsAgcdCharacterRide = GetCharacterData( pcsAgpdCharacterRide );
	if( pcsAgcdCharacterRide == NULL )
		return NULL;

	pstCharacter->m_pRide	= pcsAgcdCharacterRide;
	if( pcsAgcdCharacterRide == NULL ) {
		pstCharacter->m_nRideCID = 0;
		return FALSE;
	}
	pcsAgcdCharacterRide->m_pOwner = pstCharacter;

	if( m_pcsAgpmCharacter->UpdateInit( pcsAgpdCharacterRide ) == NULL )
		return FALSE;

	return TRUE;
}

BOOL AgcmCharacter::AttachRideClump( AgcdCharacter* pcsAgcdCharacterRide )
{
	if( pcsAgcdCharacterRide->m_pOwner == NULL )
		return TRUE;

	RpHAnimHierarchy* pInHierarchy = pcsAgcdCharacterRide->m_pInHierarchy;
	if( pInHierarchy == NULL )
		return FALSE;

	RwInt32 lNodeIndex = RpHAnimIDGetIndex( pInHierarchy, AGCMCHAR_RIDENODE_ID);
	if( lNodeIndex == -1 ) 
		return FALSE;

	RwFrame* pRevisionFrame = NULL;
	pRevisionFrame = RwFrameCreate();

	AgpdCharacter* pAgpdCharacterOwner  = GetCharacter( pcsAgcdCharacterRide->m_pOwner );
	AgcdCharacter* pAgcdCharacterOwner  = pcsAgcdCharacterRide->m_pOwner;
	AgpdCharacter* pcsAgpdCharacterRide = GetCharacter( pcsAgcdCharacterRide );

	// 좌표 각도 세팅
	pcsAgpdCharacterRide->m_stPos			= pAgpdCharacterOwner->m_stPos;
	pcsAgpdCharacterRide->m_stDestinationPos= pAgpdCharacterOwner->m_stDestinationPos;
	pcsAgpdCharacterRide->m_stDirection		= pAgpdCharacterOwner->m_stDirection;
	pcsAgpdCharacterRide->m_fTurnX			= pAgpdCharacterOwner->m_fTurnX;
	pcsAgpdCharacterRide->m_fTurnY			= pAgpdCharacterOwner->m_fTurnY;

	RpClump* pOwnerClump = pcsAgcdCharacterRide->m_pOwner->m_pClump;
	RpClump* pClump		 = pcsAgcdCharacterRide->m_pClump;

	if( pOwnerClump == NULL )
		return FALSE;

	if( pClump == NULL )
		return FALSE;

	// 탈것의 위치를 기수의 위치로 세팅
	RwFrame* pOwnerClumpFrame	= RpClumpGetFrame( pOwnerClump );
	RwFrame* pClumpFrame		= RpClumpGetFrame( pClump );
	RwMatrix* pOwnerClumpMatrix = RwFrameGetMatrix( pOwnerClumpFrame );
	RwMatrix* pClumpMatrix		= RwFrameGetMatrix( pClumpFrame );

	// 기수의 위치를 0으로 세팅
	RwFrameSetIdentity ( pOwnerClumpFrame );

	// 각도보정.
	RwV3d vtxAxis;
	vtxAxis.x = 0.f, vtxAxis.y = 1.f, vtxAxis.z = 0;
	RwFrameRotate( pRevisionFrame, &vtxAxis, 90.f, rwCOMBINEREPLACE );
	vtxAxis.x = 0.f, vtxAxis.y = 0, vtxAxis.z = 1.f;
	RwFrameRotate( pRevisionFrame, &vtxAxis, 90.f, rwCOMBINEPRECONCAT );

	RwV3d* pvtxNodePos = RwMatrixGetPos( RwFrameGetLTM( pInHierarchy->pNodeInfo[lNodeIndex].pFrame ) );
	RwFrameAddChild( pInHierarchy->pNodeInfo[lNodeIndex].pFrame, pRevisionFrame );
	RwFrameAddChild( pRevisionFrame, pOwnerClumpFrame );

//	RwFrameTranslate( pClumpFrame, &vtxOwnerPos, rwCOMBINEPRECONCAT );
//	SetAt( RwFrameGetMatrix(pClumpFrame), &vtxOwnerAt );
//	pcsAgcdCharacterRide->m_pRideRevisionFrame = pRevisionFrame;
	RwMatrixScale( pClumpMatrix, &pcsAgcdCharacterRide->m_pstAgcdCharacterTemplate->m_stScale, rwCOMBINEREPLACE);
	RwMatrixRotate( pClumpMatrix, &m_stXAxis, pcsAgpdCharacterRide->m_fTurnX, rwCOMBINEPOSTCONCAT);
	RwMatrixRotate( pClumpMatrix, &m_stYAxis, pcsAgpdCharacterRide->m_fTurnY, rwCOMBINEPOSTCONCAT);
	RwMatrixTranslate( pClumpMatrix, (RwV3d *)&pcsAgpdCharacterRide->m_stPos, rwCOMBINEPOSTCONCAT);
	RwFrameUpdateObjects( pClumpFrame);

	AgcmTextBoardMng *pAgcmTextBoardMng = (AgcmTextBoardMng *)GetModule("AgcmTextBoardMng");
	if( pAgcmTextBoardMng ) {
		pAgcmTextBoardMng->SetHeight( pAgpdCharacterOwner );
	}
	pAgcmTextBoardMng->DestroyBoard( pcsAgpdCharacterRide );

	// self character이면
	if( pAgpdCharacterOwner->m_lID == m_lSelfCID ) {
		SetSelfRideClump( pcsAgcdCharacterRide->m_pClump );
		AgcmCamera2* pAgcmCamera2 = (AgcmCamera2 *)GetModule("AgcmCamera2");

		// 아래 첫번째 인자 Owner 로 수정. 2006.11.03. steeple
		AgcmCamera2::CBOnTargetSet( pAgpdCharacterOwner, pAgcmCamera2, NULL );
	}

	AgpmItem* pAgpmItem = (AgpmItem *)GetModule("AgpmItem");
	AgcmItem* pAgcmItem = (AgcmItem *)GetModule("AgcmItem");
	AgpdItem* pcsAgpdItemWeapon;
	AgpdGridItem* pWeaponGridItem = pAgpmItem->GetEquipItem( pAgpdCharacterOwner, AGPMITEM_PART_HAND_RIGHT );
	if( pWeaponGridItem ) {
		pcsAgpdItemWeapon = pAgpmItem->GetItem( pWeaponGridItem->m_lItemID );
		pAgcmItem->UnEquipItem( pAgpdCharacterOwner, pcsAgpdItemWeapon, FALSE );
	}
	pWeaponGridItem = pAgpmItem->GetEquipItem( pAgpdCharacterOwner, AGPMITEM_PART_HAND_LEFT );
	if( pWeaponGridItem ) {
		pcsAgpdItemWeapon = pAgpmItem->GetItem( pWeaponGridItem->m_lItemID );
		pAgcmItem->UnEquipItem( pAgpdCharacterOwner, pcsAgpdItemWeapon, FALSE );
	}

	//. 2006. 5. 26. nonstopdj.
	//. 반드시 pAgcdCharacterOwner->m_lCurAnimType2은 AGCMCHAR_AT2_WARRIR_DEFAULT_RIDE, 
	//. AGCMCHAR_AT2_ARCHER_DEFAULT_RIDE, AGCMCHAR_AT2_WIZARD_DEFAULT_RIDE 중 하나가 되어야 한다.
	INT32	lAnimType2	= pAgcdCharacterOwner->m_pstAgcdCharacterTemplate->m_lAnimType2;
	pAgcdCharacterOwner->m_lCurAnimType2 = pAgcmItem->GetAnimDefaultRide( lAnimType2 );

	AgpdGridItem* pLancerGridItem = pAgpmItem->GetEquipItem( pAgpdCharacterOwner, AGPMITEM_PART_LANCER );
	if( pLancerGridItem ) {
		AgpdItem* pcsAgpdItemLancer = pAgpmItem->GetItem( pLancerGridItem->m_lItemID );
		pAgcmItem->EquipItem( pAgpdCharacterOwner, pcsAgpdItemLancer, FALSE );
	}
	/*else {
		INT32	lAnimType2	= pAgcdCharacterOwner->m_pstAgcdCharacterTemplate->m_lAnimType2;
		pAgcdCharacterOwner->m_lCurAnimType2 = pAgcmItem->GetAnimDefaultRide( lAnimType2 );
	}*/

	if( (pAgcdCharacterOwner->m_eCurAnimType >= AGCMCHAR_ANIM_TYPE_WAIT) && (pAgcdCharacterOwner->m_eCurAnimType < AGCMCHAR_MAX_ANIM_TYPE) ) {
		if( pAgpdCharacterOwner->m_unActionStatus != AGPDCHAR_STATUS_NOT_ACTION ) {
			if( !StartAnimation( pAgpdCharacterOwner, pAgcdCharacterOwner, pAgcdCharacterOwner->m_eCurAnimType ) )
				return FALSE;
		}
		else {
			if( !StartAnimation( pAgpdCharacterOwner, pAgcdCharacterOwner, AGCMCHAR_ANIM_TYPE_WAIT ) )
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::DetachRide( AgcdCharacter* pstCharacter, BOOL bRemove )
{
	PROFILE("AgcmCharacter::DetachRide");

	if( pstCharacter == NULL )
		return TRUE;

	if( pstCharacter->m_pstAgcdCharacterTemplate == NULL )
		return TRUE;

	//ride character가 삭제될 경우..
	if( pstCharacter->m_pRideRevisionFrame ) {
		RwFrameRemoveChild( pstCharacter->m_pRideRevisionFrame );
	}

	if( pstCharacter->m_pRide == NULL )
		return TRUE;

	//로딩되기전에 시아에서 사라진 경우.
	if( pstCharacter->m_pRide->m_pClump == NULL ) {
		DestroyRideCharacter( pstCharacter );
		pstCharacter->m_nRideCID= 0;
		pstCharacter->m_pRide	= NULL;
		return TRUE;
	}

	RwFrame* pClumpFrame		= NULL;
	RwFrame* pRideClumpFrame	= NULL;

	//. 2006. 1. 25. Nonstopdj
	//. access violation when character clump was removed.
	if(pstCharacter->m_pClump)
		pClumpFrame = RpClumpGetFrame( pstCharacter->m_pClump );

	pRideClumpFrame = RpClumpGetFrame( pstCharacter->m_pRide->m_pClump );

	if( pClumpFrame == NULL )
		return TRUE;

	if( pRideClumpFrame == NULL )
		return TRUE;

	RwV3d vtxRideAt, vtxRidePos;
	RwV3dAssign( &vtxRideAt, RwMatrixGetAt( RwFrameGetLTM(pRideClumpFrame) ) );
	RwV3dAssign( &vtxRidePos, RwMatrixGetPos( RwFrameGetLTM(pRideClumpFrame) ) );

	RwFrame* pRevisionFrame = RwFrameGetParent( pClumpFrame );
	if( pRevisionFrame ) {
		RwFrameRemoveChild( pClumpFrame );
		RwFrameDestroy( pRevisionFrame );
	}
	pstCharacter->m_pRide->m_pRideRevisionFrame = NULL;

	DestroyRideCharacter( pstCharacter );

	RwFrameTranslate( pClumpFrame, &vtxRidePos, rwCOMBINEPRECONCAT );
	SetAt( RwFrameGetMatrix(pClumpFrame), &vtxRideAt );
	pstCharacter->m_nRideCID= 0;
	pstCharacter->m_pRide	= NULL;

	AgcmTextBoardMng *pAgcmTextBoard = (AgcmTextBoardMng *)GetModule("AgcmTextBoardMng");
	AgpdCharacter* pAgpdCharacter = GetCharacter( pstCharacter );
	if( pAgcmTextBoard ) {
		pAgcmTextBoard->SetHeight( pAgpdCharacter );
	}

	// self character이면
	if( pAgpdCharacter->m_lID == m_lSelfCID ) {
		SetSelfRideClump( NULL );
		if( bRemove == FALSE ) {
			AgcmCamera2* pAgcmCamera2 = (AgcmCamera2 *)GetModule("AgcmCamera2");
			AgcmCamera2::CBOnTargetSet( pAgpdCharacter, pAgcmCamera2, NULL );
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::CBRemoveRide(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmCharacter *pThis = (AgcmCharacter*)pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter*)pData;
	AgcdCharacter *pcsAgcdCharacter = pThis->GetCharacterData( pcsAgpdCharacter );

	pThis->DetachRide( pcsAgcdCharacter );

	return TRUE;
}

VOID AgcmCharacter::SetSelfRideClump( RpClump* pSelfRideClump )
{
	m_pSelfRideClump = pSelfRideClump;
}
