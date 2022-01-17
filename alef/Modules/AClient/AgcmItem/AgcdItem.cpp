#include "AgcdItem.h"
#include "AgcmItem.h"
#include "ApModuleStream.h"

//------------------------------------ CItemTransformInfo ------------------------------------
void	CItemTransformInfo::Assign( CItemTransformInfo* pDst )
{
	pDst = new CItemTransformInfo;
	for( ItemTransformInfoMapItr Itr = m_mapItemTransformInfo.begin(); Itr != m_mapItemTransformInfo.end(); ++Itr )
		pDst->m_mapItemTransformInfo.insert( make_pair( (*Itr).first, new ItemTransformInfo( (*Itr).second ) ) );
}

ItemTransformInfo*	CItemTransformInfo::Insert( INT32 lID, INT32 nCount )
{
	if( !lID )		return NULL;

	ItemTransformInfoMapItr Itr = m_mapItemTransformInfo.find( lID );
	if( Itr != m_mapItemTransformInfo.end() )	return (*Itr).second;

	ItemTransformInfo* pInfo = new ItemTransformInfo( nCount );
	m_mapItemTransformInfo.insert( make_pair( lID, pInfo ) );
	return pInfo;
}

ItemTransformInfo*	CItemTransformInfo::GetInfo( INT32 lID )
{	
	ItemTransformInfoMapItr Itr = m_mapItemTransformInfo.find( lID );
	return Itr != m_mapItemTransformInfo.end() ? (*Itr).second : NULL;
}

BOOL	CItemTransformInfo::Read( ApModuleStream *pcsStream, AgcdItemTemplate* pTempalte )
{
	if( pTempalte->m_lAtomicCount < 1 )		return FALSE;

	INT32 lTemp = 0;
	pcsStream->GetValue(&lTemp);
	if ( lTemp < 1 )						return FALSE;

	ItemTransformInfoMapItr find = m_mapItemTransformInfo.find( lTemp );
	if( find != m_mapItemTransformInfo.end() )
	{
		for( UINT32 i=0; i<pTempalte->m_lAtomicCount; ++i )	//ini를 이동시켜야 한다
			pcsStream->ReadNextValue();
		return TRUE;
	}

	ItemTransformInfo* pInfo = Insert( lTemp, pTempalte->m_lAtomicCount );
	for( UINT32 lCount = 0; lCount < pTempalte->m_lAtomicCount; ++lCount )
	{
		if (!pcsStream->ReadNextValue())
			return FALSE;
		if (strncmp(pcsStream->GetValueName(), AGCMITEM_INI_NAME_TRANSFORMDATA, strlen(AGCMITEM_INI_NAME_TRANSFORMDATA)))
			return FALSE;

		CHAR	szValue[1024];
		memset( szValue, 0, sizeof (CHAR) * 1024 );
		pcsStream->GetValue( szValue, 1024 );
		sscanf( szValue, "%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%d",
				&pInfo->m_astTransform[lCount].right.x,
				&pInfo->m_astTransform[lCount].right.y,
				&pInfo->m_astTransform[lCount].right.z,
				&pInfo->m_astTransform[lCount].up.x,
				&pInfo->m_astTransform[lCount].up.y,
				&pInfo->m_astTransform[lCount].up.z,
				&pInfo->m_astTransform[lCount].at.x,
				&pInfo->m_astTransform[lCount].at.y,
				&pInfo->m_astTransform[lCount].at.z,
				&pInfo->m_astTransform[lCount].pos.x,
				&pInfo->m_astTransform[lCount].pos.y,
				&pInfo->m_astTransform[lCount].pos.z,
				&pInfo->m_alPartID[lCount] );
	}

	return TRUE;
}

BOOL	CItemTransformInfo::Write( ApModuleStream* pcsStream )
{
	if( !pcsStream )	return FALSE;

	for( ItemTransformInfoMapItr Itr = m_mapItemTransformInfo.begin(); Itr != m_mapItemTransformInfo.end(); ++Itr )
	{
		ItemTransformInfo* pInfo = (*Itr).second;
		if( !pInfo )					continue;
		if( !pInfo->m_astTransform )	continue;

		pcsStream->WriteValue( AGCMITEM_INI_NAME_TRANSFORM, (*Itr).first );
		for ( INT32 lIndex = 0; lIndex < ( INT32 ) pInfo->m_nCount; ++lIndex )
		{
			char szBuffer[1024];
			print_compact_format( szBuffer, "%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%d",
								pInfo->m_astTransform[lIndex].right.x,
								pInfo->m_astTransform[lIndex].right.y,
								pInfo->m_astTransform[lIndex].right.z,
								pInfo->m_astTransform[lIndex].up.x,
								pInfo->m_astTransform[lIndex].up.y,
								pInfo->m_astTransform[lIndex].up.z,
								pInfo->m_astTransform[lIndex].at.x,
								pInfo->m_astTransform[lIndex].at.y,
								pInfo->m_astTransform[lIndex].at.z,
								pInfo->m_astTransform[lIndex].pos.x,
								pInfo->m_astTransform[lIndex].pos.y,
								pInfo->m_astTransform[lIndex].pos.z,
								pInfo->m_alPartID[lIndex] );

			pcsStream->WriteValue(AGCMITEM_INI_NAME_TRANSFORMDATA, szBuffer);
		}
	}

	return  TRUE;
}