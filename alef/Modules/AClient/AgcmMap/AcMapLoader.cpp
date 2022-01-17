// AcMapLoader.cpp: implementation of the AcMapLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "ApBase.h"

#include <process.h>

#include "AcMapLoader.h"
#include "AgcmMap.h"

#include "ApMemoryTracker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcMapLoader::AcMapLoader()
{
	m_bEnd		= FALSE		;

	//m_csMutex.Init();

	m_nUnpackCount	= 0		;
}

AcMapLoader::~AcMapLoader()
{
	FlushUnpackManager();
}

/*
BOOL	AcMapLoader::StartThread()
{
	m_hThread = _beginthread(Process, 0, this);
	if (m_hThread == -1)
		return FALSE;

	return TRUE;
}

BOOL	AcMapLoader::StopThread()
{
	m_bEnd	= TRUE	;

//	WaitForSingleObject(m_hThread, 2000);

	return TRUE;
}

VOID	AcMapLoader::Process(PVOID pvArgs)
{
	AcMapLoader *	pThis = (AcMapLoader *) pvArgs;

	while (!pThis->m_bEnd)
	{
		pThis->m_csMutex.WLock();

		if (!pThis->IsEmpty())
		{
			pThis->m_csMutex.Release();

			pThis->OnIdleLoad();
		}
		else
		{
			pThis->m_csMutex.Release();

			Sleep(500);
		}
	}

	_endthread();
}
*/

BOOL	AcMapLoader::IsEmpty()
{
	if ( m_listSectorQueue.IsEmpty() )
		return TRUE;

	return FALSE;
}

BOOL	AcMapLoader::OnIdleLoad()
{
	PROFILE("AcMapLoader::OnIdleLoad");

	if( m_listSectorQueue.IsEmpty() ) return TRUE; // Queue Empty;
	
	LoadBox *		pLoadBox	= &m_listSectorQueue.GetHead()	;

	ASSERT( NULL != AGCMMAP_THIS );
	if( NULL == pLoadBox->pSector )
	{
		// TRACE( "AcMapLoader::OnIdleLoad 섹터 포인터가 널이예요\n" );
		m_listSectorQueue.RemoveHead();

		// Reculsive Call
		return OnIdleLoad();
	}
	else
	{
		TRACE("Get Load Queue ( Waiting Queue %d ) : %x\n", m_listSectorQueue.GetCount() , pLoadBox->pSector->GetArrayIndexDWORD() );

		switch( pLoadBox->nDetail )
		{
		case SECTOR_LOWDETAIL	:	AGCMMAP_THIS->OnLoadRough	( pLoadBox->pSector );	break;
		case SECTOR_HIGHDETAIL	:	AGCMMAP_THIS->OnLoadDetail	( pLoadBox->pSector );	break;
		default:
			// 여기 오면 안돼는데..;;;
			break;
		}
				
		// 이전에 설정된 디테일로 돌려놓음..
		// AGCMMAP_THIS->SetCurrentDetail( pLoadBox->pSector , pLoadBox->pSector->GetCurrentDetail() , TRUE );
	}

	m_listSectorQueue.RemoveHead();

	return TRUE;
}

BOOL	AcMapLoader::LoadDWSector( ApWorldSector * pvSector , INT32 nDetail )
{
	if( ! AGCMMAP_THIS->m_pcsApmMap->IsAutoLoadData() )
	{
		AGCMMAP_THIS->SetCurrentDetail( pvSector , nDetail );
		return TRUE;
	}
	
	//m_csMutex.WLock();

	AgcmMap *	pcsAgcmMap = AGCMMAP_THIS;

	if( AGCMMAP_THIS->IsLoadedDetail	( pvSector , nDetail )	||
		DoExistSectorInfo			( pvSector , nDetail )	)	return TRUE;
		// 이미 로딩되어있거나 , 로딩중이다.

	// 케릭터의 위치를 얻어냄..
	ApWorldSector *	pSector = AGCMMAP_THIS->m_pcsApmMap->GetSector(AGCMMAP_THIS->m_posCurrentAvatarPosition);
	
	if (pSector == pvSector)
	{
		// 케릭터가 있어야하는 곳은 큐에 넣지 않고 바로 로딩함..
		switch( nDetail )
		{
		case SECTOR_LOWDETAIL	:	AGCMMAP_THIS->OnLoadRough	( pvSector );	break;
		case SECTOR_HIGHDETAIL	:	AGCMMAP_THIS->OnLoadDetail	( pvSector );	break;
		default:
			// 여기 오면 안돼는데..;;;
			ASSERT( !"Unreachable code" );
			break;
		}
		
		return TRUE;
	}
	
	if( nDetail < 0 )
	{
		// 전부다 로딩..
		if( FALSE == AddLoadSectorInfo( pvSector , SECTOR_LOWDETAIL		) )	return FALSE;
		if( FALSE == AddLoadSectorInfo( pvSector , SECTOR_HIGHDETAIL	) )	return FALSE;
	}
	else if( nDetail < SECTOR_DETAILDEPTH && nDetail >= nDetail)
	{
		if( FALSE == AddLoadSectorInfo( pvSector , nDetail				) )	return FALSE;
	}
	else
	{
		ASSERT( !"디테일 이상!" );
	}

	//m_csMutex.Release();

	return TRUE;
}

BOOL	AcMapLoader::AddLoadSectorInfo( ApWorldSector * pSector, INT32 nDetail )
{
	if ( m_listSectorQueue.GetCount() >= ACMAPLOADER_MAX_QUEUE )
	{
		//m_csMutex.Release();

		TRACE("Load Queue Full!!!\n");
		return FALSE;
	}
	
	// 중복 돼었는지 점검..
	if( DoExistSectorInfo( pSector , nDetail ) ) return FALSE;

	// 중복돼지않으면 로딩큐에 추가..

	LoadBox lb;
	lb.pSector	= pSector;
	lb.nDetail	= nDetail;

	TRACE("Insert Load Queue ( Waiting Queue %d ) : %d\n"	, 
		m_listSectorQueue.GetCount()						,
		pSector->GetArrayIndexDWORD()						);

	m_listSectorQueue.AddTail( lb );

	return TRUE;
}

BOOL	AcMapLoader::DoExistSectorInfo( ApWorldSector * pSector, INT32 nDetail )
{
	if ( m_listSectorQueue.GetCount() == 0 )
	{
		return FALSE;
	}
	
	// 중복 돼었는지 점검..
	LoadBox *		pLoadBox;

	AuNode< LoadBox >	*pNode			= m_listSectorQueue.GetHeadNode();

	while( pNode )
	{
		pLoadBox	= & pNode->GetData();

		if( pLoadBox->pSector == pSector	&&
			pLoadBox->nDetail == nDetail	)
		{
			return TRUE;
		}

		m_listSectorQueue.GetNext( pNode );
	}

	return FALSE;
}

CMagUnpackManager	*	AcMapLoader::GetUnpackManager( INT32 nDetail , UINT nBlockIndex , char * pFilename )
{
	INT32				i;
	CMagUnpackManager * pUnpackManager = NULL;

	for( i = 0 ; i < m_nUnpackCount ; ++i  )
	{
		if( m_aUnpackBlock[ i ].pUnpackManager				&&
			nDetail		== m_aUnpackBlock[ i ].nDetail		&&
			nBlockIndex	== m_aUnpackBlock[ i ].uBlockIndex	)
		{
			// 로딩이 된거면...
			pUnpackManager	= m_aUnpackBlock[ i ].pUnpackManager;

			// 억세스타임 업데이트..
			m_aUnpackBlock[ i ].uLastAccessTime	= GetTickCount();

			return pUnpackManager;
		}
	}

	// 요까지 왔으면.. 로딩됀게 없다는 소린데..
	// 1,빈데다가 넣는데
	// 2,가장 억세스 안한 녀석을 날리고 넣는다.

	// 빈데가 있나..
	if( m_nUnpackCount < ACMAPLOADER_PACK_QUEUE )
	{
		ASSERT( NULL == m_aUnpackBlock[ m_nUnpackCount ].pUnpackManager );//이거 걸리면 메모리 침범.

		// 빈데다!..
		pUnpackManager = new CMagUnpackManager;
		ASSERT( NULL != pUnpackManager );
		if( NULL == pUnpackManager ) return NULL;

		// 파일이 로딩가능한지 확인..
		if( !pUnpackManager->SetFile( pFilename ) )
		{
			// 파일이 없나보다.
			delete pUnpackManager;
			return NULL;
		}

		// 자 삽입한다.
		m_aUnpackBlock[ m_nUnpackCount ].pUnpackManager	= pUnpackManager;
		m_aUnpackBlock[ m_nUnpackCount ].nDetail		= nDetail		;
		m_aUnpackBlock[ m_nUnpackCount ].uBlockIndex	= nBlockIndex	;
		m_aUnpackBlock[ m_nUnpackCount ].uLastAccessTime= GetTickCount();
		m_nUnpackCount++;

		return pUnpackManager;
	}

	ASSERT( m_nUnpackCount == ACMAPLOADER_PACK_QUEUE );

	// 음 가장 오래된걸 고른다.
	int		nOld		= 0;
	UINT	uOldTime	= m_aUnpackBlock[ 0 ].uLastAccessTime;

	for( i = 1 ; i < m_nUnpackCount ; ++i )
	{
		if( m_aUnpackBlock[ i ].pUnpackManager				&&
			uOldTime > m_aUnpackBlock[ i ].uLastAccessTime	)
		{
			nOld		= i;
			uOldTime	= m_aUnpackBlock[ i ].uLastAccessTime;
		}
	}

	// 자 이제 추가..
	pUnpackManager = new CMagUnpackManager;
	ASSERT( NULL != pUnpackManager );
	if( NULL == pUnpackManager ) return NULL;

	// 파일이 로딩가능한지 확인..
	if( !pUnpackManager->SetFile( pFilename ) )
	{
		// 파일이 없나보다.
		delete pUnpackManager;
		return NULL;
	}

	TRACE( "\"%s\"Pack파일 제거 , \"%s\"파일 로딩.\n"				, 
		m_aUnpackBlock[ nOld ].pUnpackManager->GetPackFileName()	,
		pFilename													);

	ASSERT( NULL != m_aUnpackBlock[ nOld ].pUnpackManager );//이거 걸리면 메모리 침범.
	if( m_aUnpackBlock[ nOld ].pUnpackManager ) delete m_aUnpackBlock[ nOld ].pUnpackManager;

	// 자 삽입한다.
	m_aUnpackBlock[ nOld ].pUnpackManager	= pUnpackManager;
	m_aUnpackBlock[ nOld ].nDetail			= nDetail		;
	m_aUnpackBlock[ nOld ].uBlockIndex		= nBlockIndex	;
	m_aUnpackBlock[ nOld ].uLastAccessTime	= GetTickCount();

	return pUnpackManager;
}

void		AcMapLoader::EmptyQueue		()	// 큐에 있는 로딩 내용을 삭제한다..
{
	m_listSectorQueue.RemoveAll();
}

BOOL		AcMapLoader::RemoveQueue		( ApWorldSector * pvSector , INT32 nDetail )	// 해당 섹터의 로딩 큐를 모두 제거한다.
{
	PROFILE("AcMapLoader::RemoveQueue");

	// 이미 로딩이 되어 있는 녀석이면 바로바로 척살시킴..
	if( ! AGCMMAP_THIS->m_pcsApmMap->IsAutoLoadData() )
	{
		if( nDetail > 0 )
			AGCMMAP_THIS->SetCurrentDetail( pvSector , nDetail - 1 );

		return TRUE;
	}

	if( nDetail != -1 )
	{
		if( AGCMMAP_THIS->IsLoadedDetail	( pvSector , nDetail )	)
		{
			// 로딩이 끝난 녀석만 Destroy 콜벡을 호출해줌..
			switch( nDetail )
			{
			case SECTOR_HIGHDETAIL	:	AGCMMAP_THIS->OnDestroyDetail( pvSector );	break;
			case SECTOR_LOWDETAIL	:	AGCMMAP_THIS->OnDestroyRough	( pvSector );	break;
			case SECTOR_EMPTY		:												break;	// do nothing..
			}

			return TRUE;
		}
	}
	else
	{
		// 모두 없에기다.
		// 로딩된놈은 기냥 죽이고... 따로 리스트에서도 제거함..
		if( AGCMMAP_THIS->IsLoadedDetail	( pvSector , SECTOR_HIGHDETAIL	) ) AGCMMAP_THIS->OnDestroyDetail( pvSector );
		if( AGCMMAP_THIS->IsLoadedDetail	( pvSector , SECTOR_LOWDETAIL	) ) AGCMMAP_THIS->OnDestroyRough	( pvSector );
	}

	// 로딩이 되진 않았는데 , 혹시 로딩큐에 들어있지는 않는지 점검.
	LoadBox				*pLoadBox		;

	AuNode< LoadBox >	*pNode			= m_listSectorQueue.GetHeadNode();
	AuNode< LoadBox >	*pNodeDelete	;
	//BOOL				bFound			= FALSE;

	while( pNode )
	{
		pLoadBox	= & pNode->GetData();

		pNodeDelete	= pNode;

		m_listSectorQueue.GetNext( pNode );

		if( pLoadBox->pSector == pvSector )
		{
			if( nDetail == -1 )
			{
				// 해당 녀석 모두 제거..
				m_listSectorQueue.RemoveNode( pNodeDelete );
			}
			else
			// 해당 디테일만 삭제.. 인경우
			if( pLoadBox->nDetail	== nDetail	)
			{
				// 하나 제거하면 리턴..
				m_listSectorQueue.RemoveNode( pNodeDelete );
				return TRUE;
			}
		}
	}

	// 충돌하는게 없음..
	return FALSE;
}

void	AcMapLoader::FlushUnpackManager()
{
	for( int i = 0 ; i < ACMAPLOADER_PACK_QUEUE ; ++ i)
	{
		if( m_aUnpackBlock[ i ].pUnpackManager ) 
			delete m_aUnpackBlock[ i ].pUnpackManager;
		m_aUnpackBlock[ i ].pUnpackManager		= NULL;
		m_aUnpackBlock[ i ].nDetail				= 0;
		m_aUnpackBlock[ i ].uBlockIndex			= 0;
		m_aUnpackBlock[ i ].uLastAccessTime		= 0;
	}

	m_nUnpackCount = 0;
}