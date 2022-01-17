#include "AgcmUIAuction.h"



AgcmUICategoryInfo::AgcmUICategoryInfo( void )
{
	Clear();
}

void AgcmUICategoryInfo::Clear( void )
{
	for( int nCategoryCount = 0 ; nCategoryCount < AGCM_UI_AUCTION_MAX_CATEGORY_COUNT ; ++nCategoryCount )
	{
		m_csCategory[ nCategoryCount ].m_lCategoryID = 0;
		m_csCategory[ nCategoryCount ].m_pstrCategoryName = NULL;
	}
}

INT32 AgcmUICategoryInfo::SetData( char *pstrName, INT32 lCategory )
{
	INT32 lIndex = -1;

	int nCategoryCount = 0;
	for( nCategoryCount = 0 ; nCategoryCount < AGCM_UI_AUCTION_MAX_CATEGORY_COUNT ; ++nCategoryCount )
	{
		if( m_csCategory[ nCategoryCount ].m_lCategoryID == 0 )	lIndex = nCategoryCount;
	}

	if( lIndex != -1 )
	{
		m_csCategory[ nCategoryCount ].m_pstrCategoryName = pstrName;
		m_csCategory[ nCategoryCount ].m_lCategoryID = lCategory;
	}

	return lIndex;
}

AgcdUICategoryInfoUnit* AgcmUICategoryInfo::GetData( INT32 lIndex )
{
	AgcdUICategoryInfoUnit *pstrData = NULL;
	if( 0 <= lIndex && lIndex < AGCM_UI_AUCTION_MAX_CATEGORY_COUNT )
	{
		pstrData = &m_csCategory[ lIndex ];
	}

	return pstrData;
}
