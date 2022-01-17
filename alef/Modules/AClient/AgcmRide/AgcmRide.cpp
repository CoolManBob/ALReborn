#include "AgcmRide.h"
#include "AgpmItem.h"
#include "AgcmItem.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"


AgcmRide* AgcmRide::m_pThis = NULL;

AgcmRide::AgcmRide()
{
	SetModuleName( "AgcmRide" );
	EnableIdle( FALSE );
	m_pThis = this;
}

AgcmRide::~AgcmRide()
{
}

BOOL AgcmRide::OnAddModule()
{
	m_pcsAgpmItem		= (AgpmItem*)GetModule("AgpmItem");
	m_pcsAgcmItem		= (AgcmItem*)GetModule("AgcmItem");
	m_pcsAgpmCharacter	= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgcmCharacter	= (AgcmCharacter*)GetModule("AgcmCharacter");
	m_pcsAgpmRide 		= (AgpmRide*)GetModule("AgpmRide");

	if( !m_pcsAgpmItem || !m_pcsAgcmItem || !m_pcsAgpmCharacter || !m_pcsAgcmCharacter || !m_pcsAgpmRide )
		return FALSE;

	if (!m_pcsAgpmRide->SetCallbackRideAck(CBRideAck, this))
		return FALSE;

	if (!m_pcsAgpmRide->SetCallbackDismountAck(CBRideDismountAck, this))
		return FALSE;

	if (!m_pcsAgpmRide->SetCallbackTimeout(CBRideTimeOut, this))
		return FALSE;

	if (!m_pcsAgpmRide->SetCallbackRideTID(CBRideTID, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackInitChar(CBInitCharacter, this))
		return FALSE;

	m_pcsAgcmItem->SetAgcdRide( this );

	return TRUE;
}

BOOL AgcmRide::OnInit()
{
	return TRUE;
}

BOOL AgcmRide::OnDestroy()
{
	return TRUE;
}

BOOL AgcmRide::CBRideAck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmRide *pThis = (AgcmRide*)pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter*)pData;
	INT32* pItemID = (INT32*)pCustData;

	AgcdCharacter* pcsAgcdCharacter = m_pThis->m_pcsAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return FALSE;

	pcsAgpdCharacter->m_bRidable = TRUE;

	AgpdItem* pcsAgpdItem = m_pThis->m_pcsAgpmItem->GetItem( *pItemID );
	if( pcsAgpdItem == NULL )
		return FALSE;

	if( pcsAgpdItem->m_pcsItemTemplate == NULL )
		return FALSE;

	m_pThis->m_pcsAgcmCharacter->AttachRide( pcsAgcdCharacter, pcsAgpdItem->m_pcsItemTemplate->m_nSubType );

	return TRUE;
}

BOOL AgcmRide::CBRideDismountAck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmRide *pThis = (AgcmRide*)pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter*)pData;
	INT32* pItemID = (INT32*)pCustData;

	AgcdCharacter* pcsAgcdCharacter = m_pThis->m_pcsAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return FALSE;

	pcsAgpdCharacter->m_bRidable = FALSE;
	pcsAgpdCharacter->m_nRideCID = 0;

	m_pThis->m_pcsAgcmCharacter->DetachRide( pcsAgcdCharacter );

	AgpdGridItem* pLancerGridItem = m_pThis->m_pcsAgpmItem->GetEquipItem( pcsAgpdCharacter, AGPMITEM_PART_LANCER );
	if( pLancerGridItem ) {
		AgpdItem* pcsAgpdItemLancer = m_pThis->m_pcsAgpmItem->GetItem( pLancerGridItem->m_lItemID );
		m_pThis->m_pcsAgcmItem->UnEquipItem( pcsAgpdCharacter, pcsAgpdItemLancer, FALSE );
	}

	m_pThis->m_pcsAgcmItem->SetEquipAnimType( pcsAgpdCharacter, pcsAgcdCharacter );

	AgpdItem* pcsAgpdItemWeapon = NULL;
	AgpdGridItem* pWeaponGridItem = m_pThis->m_pcsAgpmItem->GetEquipItem( pcsAgpdCharacter, AGPMITEM_PART_HAND_RIGHT );
	if( pWeaponGridItem ) {
		pcsAgpdItemWeapon = m_pThis->m_pcsAgpmItem->GetItem( pWeaponGridItem->m_lItemID );
		m_pThis->m_pcsAgcmItem->EquipItem( pcsAgpdCharacter, pcsAgpdItemWeapon, FALSE );
	}
	pWeaponGridItem = m_pThis->m_pcsAgpmItem->GetEquipItem( pcsAgpdCharacter, AGPMITEM_PART_HAND_LEFT );
	if( pWeaponGridItem ) {
		pcsAgpdItemWeapon = m_pThis->m_pcsAgpmItem->GetItem( pWeaponGridItem->m_lItemID );
		m_pThis->m_pcsAgcmItem->EquipItem( pcsAgpdCharacter, pcsAgpdItemWeapon, FALSE );
	}

	return TRUE;
}

BOOL AgcmRide::CBRideTimeOut(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

// 2006.10.10. steeple
BOOL AgcmRide::CBRideTID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmRide* pThis = static_cast<AgcmRide*>(pClass);
	AgpdCharacter* pcsAgpdCharacter = static_cast<AgpdCharacter*>(pData);
	INT32 lRideTID = *static_cast<INT32*>(pCustData);

	AgcdCharacter* pcsAgcdCharacter = m_pThis->m_pcsAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return FALSE;

	pcsAgpdCharacter->m_bRidable = TRUE;

	pThis->m_pcsAgcmCharacter->AttachRide( pcsAgcdCharacter, lRideTID );
	return TRUE;
}

BOOL AgcmRide::CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)	pData;
	AgcmRide		*pThis				= (AgcmRide *)		pClass;

	if (pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_RIDE_ARCADIA &&
		pcsAgpdCharacter != pThis->m_pcsAgcmCharacter->GetSelfCharacter())
	{
		AgcdCharacter* pcsAgcdCharacter = pThis->m_pcsAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
		if( pcsAgcdCharacter == NULL )
			return FALSE;

		pcsAgpdCharacter->m_bRidable = TRUE;

		// 알카디아 하드코딩 
		int lRideTID = 1295;
		pThis->m_pcsAgcmCharacter->AttachRide( pcsAgcdCharacter, lRideTID );
	}

	return TRUE;
}