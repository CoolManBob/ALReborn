#include "ApBase.h"
#include <skeleton.h>
#include "AgcmMap.h"
#include "MagDebug.h"
#include <memory.h>
#include "dwsstream.h"
#include "aplib.h"

RwStream *
RpDWSectorStreamWrite(RpDWSector *pDWSector, RwStream *pStream)
{
	ASSERT( NULL != pDWSector	);
	ASSERT( NULL != pStream		);

	if( NULL == pDWSector	||
		NULL == pStream		)
		return NULL;
	
	//RwInt32		nSize			;
	RwInt32		nSizeTotal = 0	;
	/*
	{
		RwInt32			nSrcLength		;
		RwInt32			nPackLength = 0	;
		RwInt32			nWorkLength		;

		RwChar		*	pSrcBuffer		;
		RwChar		*	pPackBuffer		;
		RwChar		*	pWorkBuffer		;
		RwStream	*	pMemStream		;
		RwMemory		memory			;

		VERIFY( pMemStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMWRITE, &memory) );
		if (!pMemStream)
		{
			return NULL;
		}

		// 널이면 안됀다.
		ASSERT( NULL != pDWSector->atomic );
		if ( pDWSector->atomic )
		{
			if ( !RpAtomicStreamWrite( pDWSector->atomic, pMemStream ) )
			{
				ASSERT( !"RpDWSectorStreamWrite , RpAtomicStreamWrite" );
				return NULL;
			}
		}

		VERIFY( RwStreamClose(pMemStream, &memory) );

		nSrcLength	= memory.length	;
		nWorkLength	= 10000000		;

		ASSERT( nSrcLength );
		if( nSrcLength )
		{
			pSrcBuffer	= ( char * ) memory.start					;
			pPackBuffer	= ( char * ) RwMalloc( ( ( nSrcLength * 9 ) / 8 ) + 16 );
			pWorkBuffer	= ( char * ) RwMalloc( nWorkLength )		;

			nPackLength	= aP_pack(	( unsigned char * ) pSrcBuffer	, 
									( unsigned char * ) pPackBuffer	,
									nSrcLength						,
									( unsigned char * ) pWorkBuffer	,
									NULL							);
		}

		nSizeTotal = sizeof(nPackLength) + sizeof(nSrcLength) + nPackLength;

		VERIFY( RwStreamWriteChunkHeader(pStream, rwID_DWSECTOR_DATA, nSizeTotal) );

		nSize = sizeof(nPackLength);
		if (!RwStreamWrite(pStream, (void *) &nPackLength, nSize))
		{
			ASSERT( !"RwStreamWrite , nPackLength" );
			return NULL;
		}

		nSizeTotal -= nSize;

		nSize = sizeof(nSrcLength);
		if (!RwStreamWrite(pStream, (void *) &nSrcLength, nSize))
		{
			ASSERT( !"RwStreamWrite , nSrcLength" );
			return NULL;
		}

		nSizeTotal -= nSize;

		nSize = nPackLength;
		if (!RwStreamWrite(pStream, (void *) pPackBuffer, nSize))
		{
			ASSERT( !"RwStreamWrite , pPackBuffer" );
			return NULL;
		}
		nSizeTotal -= nSize;

		RwFree( pPackBuffer	);
		RwFree( pWorkBuffer	);
		RwFree( pSrcBuffer	);

	}
	*/
	RwBool	bExist = (pDWSector->atomic != NULL);

	RwStreamWrite(pStream, &bExist, sizeof(RwBool));

	if ( bExist )
	{
		if ( !RpAtomicStreamWrite( pDWSector->atomic, pStream ) )
		{
			ASSERT( !"RpDWSectorStreamWrite , RpAtomicStreamWrite" );
			return NULL;
		}
	}

	return pStream;
}


/*
 *****************************************************************************
 */
RpAtomic *
RpDWSectorStreamRead(RpDWSector *pDWSector, RwStream *pStream)
{
	if( 	NULL == pStream		)
		return NULL;

	// ASSERT(pDWSector->atomic == NULL);

	RwBool		bExist		;
	RpAtomic *	pstAtomic	= NULL;

	RwStreamRead(pStream, &bExist, sizeof(RwBool));

	if (bExist)
	{
		if (RwStreamFindChunk(pStream, rwID_ATOMIC, NULL, NULL))
		{
			AGCMMAP_THIS->LockFrame();

			pstAtomic = RpAtomicStreamRead(pStream);

			if (pstAtomic)
			{
				VERIFY( RpAtomicSetFrame(pstAtomic, RwFrameCreate()) );
				
				if( pDWSector )
				{
					pDWSector->geometry = RpAtomicGetGeometry(pstAtomic);
					pDWSector->atomic	= pstAtomic;
					ASSERT( NULL != pDWSector->geometry );
				}
			}

			AGCMMAP_THIS->UnlockFrame();
		}
	}

	return pstAtomic;
}


/*
 *****************************************************************************
 */
RwInt32
RpDWSectorStreamGetSize(RpDWSector *pDWSector)
{
	ASSERT( NULL != pDWSector	);

	if( NULL == pDWSector )
		return NULL;

	RwInt32 nSizeTotal = 0;

	/*
	{
		RwInt32	nSrcLength		;
		RwInt32	nPackLength = 0	;
		RwInt32	nWorkLength		;

		RwChar		*	pSrcBuffer	;
		RwChar		*	pPackBuffer	;
		RwChar		*	pWorkBuffer	;
		RwStream	*	pMemStream	;
		RwMemory		memory		;

		VERIFY( pMemStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMWRITE, &memory) );
		if (!pMemStream)
		{
			return -1;
		}

		ASSERT( NULL != pDWSector->atomic );
		if (pDWSector->atomic)
		{
			if (!RpAtomicStreamWrite(pDWSector->atomic, pMemStream))
			{
				ASSERT( !"RpDWSectorStreamGetSize . Atomic Stream Write" );
				return -1;
			}
		}
		else return -1;

		RwStreamClose(pMemStream, &memory);

		nSrcLength	= memory.length	;
		nWorkLength	= 10000000		;

		if (nSrcLength)
		{
			pSrcBuffer	= ( char * ) memory.start							;
			pPackBuffer	= ( char * ) RwMalloc(((nSrcLength * 9 ) / 8 ) + 16);
			pWorkBuffer	= ( char * ) RwMalloc(nWorkLength)					;

			nPackLength = aP_pack(	( unsigned char * ) pSrcBuffer	,
									( unsigned char * ) pPackBuffer	,
									nSrcLength						,
									( unsigned char * ) pWorkBuffer	,
									NULL							);

			RwFree	( pPackBuffer	);
			RwFree	( pWorkBuffer	);
			RwFree	( pSrcBuffer	);
		}

		return sizeof(nPackLength) + sizeof(nSrcLength) + nPackLength + rwCHUNKHEADERSIZE;
	}
	*/

	nSizeTotal = sizeof(RwBool);
	if (pDWSector->atomic)
		nSizeTotal += RpAtomicStreamGetSize(pDWSector->atomic);

	return nSizeTotal;
}