#include "AgcuEventEffect.h"
#include "ApMemoryTracker.h"

RpAtomic *AgcuEventEffect_SearchAtomicPartIDCB(RpAtomic *atomic, void *data)
{
	stAgcuEventEffect_SearchPartIDSet* pstSet = (stAgcuEventEffect_SearchPartIDSet *)(data);

	if(AgcuEventEffect_GetAtomicPartID(atomic) == pstSet->m_lPartID)
		pstSet->m_pastAtomic[pstSet->m_lCount++] = atomic;

	return atomic;
}

RwInt32 AgcuEventEffect_SearchAtomicPartID(RpClump *pstBase, RpAtomic **pastDestAtomics, RwInt32 lPartID)
{
	stAgcuEventEffect_SearchPartIDSet stSet;
	memset(&stSet, 0, sizeof(stAgcuEventEffect_SearchPartIDSet));

	stSet.m_lPartID		= lPartID;

	RpClumpForAllAtomics(pstBase, AgcuEventEffect_SearchAtomicPartIDCB, (void *)(&stSet));

	for(INT32 lCount = 0; lCount < stSet.m_lCount; ++lCount)
		pastDestAtomics[lCount] = stSet.m_pastAtomic[lCount];

	return stSet.m_lCount;
}

RwInt32 AgcuEventEffect_GetAtomicPartID(RpAtomic *atomic)
{
	RpGeometry* pGeom = RpAtomicGetGeometry(atomic);
	if( !pGeom )		return 0;

	RwInt32 nUsrDatNum = RpGeometryGetUserDataArrayCount(pGeom);
	if( !nUsrDatNum )	return 0;

	for( RwInt32 nCount = 0; nCount < nUsrDatNum; ++nCount )
	{
		RpUserDataArray* pstUserDataArray = RpGeometryGetUserDataArray(pGeom, nCount);
		if( !pstUserDataArray )		continue;

		RwChar* pszUserDataArrayName = RpUserDataArrayGetName(pstUserDataArray);
		if( !strcmp( pszUserDataArrayName, "PartID" ) )
			return RpUserDataArrayGetInt( pstUserDataArray, 0 );
	}

	return 0;
}

AgpdItem *AgcuEventEffect_GetEquipItem(AgpmItem *pModule, AgpdCharacter *pcsAgpdCharacter, AgpmItemPart ePart)
{
	AgpdGridItem* pcsAgpdGridItem = pModule->GetEquipItem( pcsAgpdCharacter, ePart );
	return pcsAgpdGridItem ? pModule->GetItem( pcsAgpdGridItem->m_lItemID ) : NULL;

}

FLOAT AgcuEventEffect_GetCharacterCurrentAnimDuration(AgcmCharacter *pModule, AgpdCharacter *pcsAgpdCharacter)
{
	AgcdCharacter *pcsAgcdCharacter = pModule->GetCharacterData(pcsAgpdCharacter);
	return pcsAgcdCharacter ? AgcuEventEffect_GetCharacterCurrentAnimDuration(pcsAgcdCharacter) : -1.f;
}

FLOAT AgcuEventEffect_GetCharacterCurrentAnimDuration(AgcdCharacter *pcsAgcdCharacter)
{
	return pcsAgcdCharacter->m_csAnimation.GetDuration();
}