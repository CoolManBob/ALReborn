#include "AgcmUIAuction.h"
#include "ApMemoryTracker.h"


void AgcdUIAuctionBoardUnit::Reset( void )
{
	m_llDocID = 0;
	m_lIndex = 0;
	m_lPrice = 0;
	m_lCount = 0;

	if( m_pstrItemName != NULL )
	{
		delete[] m_pstrItemName;
		m_pstrItemName = NULL;
	}

	if( m_pstrSellerName != NULL )
	{
		delete[] m_pstrSellerName;
		m_pstrSellerName = NULL;
	}
}

AgcdUIAuctionBoardInfo::AgcdUIAuctionBoardInfo( void )
{
}

void AgcdUIAuctionBoardInfo::clear( void )
{
	for( int nBoardCount = 0 ; nBoardCount < AGCM_UI_AUCTION_MAX_BOARD_COUNT ; ++nBoardCount )
	{
		m_csBoard[ nBoardCount ].Reset();
	}
}

INT32 AgcdUIAuctionBoardInfo::SetData( INT64 llDocID, INT32 lIndex, char *pstrItemName, INT32 lPrice, INT32 lCount, char *pstrSellerName )
{
	INT32 lCounter = -1;

	int nBoardCount = 0;
	for( nBoardCount = 0 ; nBoardCount < AGCM_UI_AUCTION_MAX_BOARD_COUNT ; ++nBoardCount )
	{
		if( m_csBoard[ nBoardCount ].m_llDocID == 0 )
		{
			lCounter = nBoardCount;
		}
	}

	if( lCounter != -1 )
	{
		m_csBoard[ nBoardCount ].m_llDocID = llDocID;
		m_csBoard[ nBoardCount ].m_lIndex = lIndex;
		m_csBoard[ nBoardCount ].m_lPrice = lPrice;
		m_csBoard[ nBoardCount ].m_lCount = lCount;

		//혹시 모르니 Null이 아니면 delete한다.
		if( m_csBoard[ nBoardCount ].m_pstrItemName )
		{
			delete[] m_csBoard[ nBoardCount ].m_pstrItemName;
			m_csBoard[ nBoardCount ].m_pstrItemName = NULL;
		}

		m_csBoard[ nBoardCount ].m_pstrItemName = new char[ 255 ];
		memset( m_csBoard[ nBoardCount ].m_pstrItemName, 0, 255 );
		strcat( m_csBoard[ nBoardCount ].m_pstrItemName, pstrItemName );

		//혹시 모르니 Null이 아니면 delete한다.
		if( m_csBoard[ nBoardCount ].m_pstrSellerName )
		{
			delete[] m_csBoard[ nBoardCount ].m_pstrSellerName;
			m_csBoard[ nBoardCount ].m_pstrSellerName = NULL;
		}

		m_csBoard[ nBoardCount ].m_pstrSellerName = new char[ 255 ];
		memset( m_csBoard[ nBoardCount ].m_pstrSellerName, 0, 255 );
		strcat( m_csBoard[ nBoardCount ].m_pstrSellerName, pstrSellerName );
	}

	return lCounter;
}

AgcdUIAuctionBoardUnit* AgcdUIAuctionBoardInfo::GetData( INT32 lIndex )
{
	AgcdUIAuctionBoardUnit* pstrData = NULL;
	if( 0 <= lIndex && lIndex < AGCM_UI_AUCTION_MAX_BOARD_COUNT )
	{
		pstrData = &m_csBoard[ lIndex ];
	}

	return pstrData;
}