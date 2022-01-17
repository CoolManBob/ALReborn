// ApWorldSector.cpp: implementation of the ApWorldSector class.
//
//////////////////////////////////////////////////////////////////////

#include "ApModule.h"
#include "ApmMap.h"
#include "ApWorldSector.h"
#include "MagDebug.h"
//#include "rtintsec.h"
#include <stdio.h>
#include "ApMemoryTracker.h"
#include <algorithm>

using namespace std;

bool operator==( ApWorldSector::Dimension &lParam , INT32 nCreatedIndex )
{
	if( lParam.nIndex == nCreatedIndex ) return true;
	else return false;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ApmMap * ApWorldSector::m_pModuleMap = NULL;

ApWorldSector::ApWorldSector()
{
	// 생성자는 명목상 존재. 전혀 호출될수 없다.
}

ApWorldSector::~ApWorldSector()
{
	// 이놈도 마찬가지.
}

ApWorldSector *	ApWorldSector::GetNearSector	( int direction )
{
	ASSERT( m_bInitialized									);
	ASSERT( NULL != ApWorldSector::m_pModuleMap				);
	ASSERT( 0 <= direction && direction < TD_DIRECTIONCOUNT	);

	switch( direction )
	{
	case	TD_NORTH	:	return ApWorldSector::m_pModuleMap->GetSector( m_nIndexX		, 0 , m_nIndexZ - 1	);
	case	TD_EAST		:	return ApWorldSector::m_pModuleMap->GetSector( m_nIndexX + 1	, 0 , m_nIndexZ		);
	case	TD_SOUTH	:	return ApWorldSector::m_pModuleMap->GetSector( m_nIndexX		, 0 , m_nIndexZ + 1	);
	case	TD_WEST		:	return ApWorldSector::m_pModuleMap->GetSector( m_nIndexX - 1	, 0 , m_nIndexZ		);
	default:
		ASSERT( !"존재하지 않는 Direction Flag!" );
		return NULL;
	}
}

int	ApWorldSector::GetArrayIndexDWORD		() const
{
	return	( SectorIndexToArrayIndexX( GetIndexX() ) << 16	)	|
			( SectorIndexToArrayIndexZ( GetIndexZ()	)		)	;
}

INT32	ApWorldSector::GetArrayIndexX() const { return SectorIndexToArrayIndexX( GetIndexX() ); }
INT32	ApWorldSector::GetArrayIndexZ() const { return SectorIndexToArrayIndexZ( GetIndexZ() ); }

FLOAT	ApWorldSector::GetStepSizeX()
{
	ASSERT( IsLoadedDetailData()			);
	ASSERT( GetDetailSectorInfo()->m_nDepth[ GetCurrentDetail() ] != 0 );

	return ( m_fxEnd - m_fxStart ) / ( FLOAT ) GetDetailSectorInfo()->m_nDepth[ GetCurrentDetail() ] ;
}

FLOAT	ApWorldSector::GetStepSizeZ()
{
	ASSERT( IsLoadedDetailData() );
	ASSERT( GetDetailSectorInfo()->m_nDepth[ GetCurrentDetail() ] != 0 );

	return ( m_fzEnd - m_fzStart ) / ( FLOAT ) GetDetailSectorInfo()->m_nDepth[ GetCurrentDetail() ] ;
}

FLOAT	ApWorldSector::GetStepSizeX( int nDetail )
{
	ASSERT( IsLoadedDetailData()							);
	ASSERT( 0 <= nDetail && SECTOR_DETAILDEPTH > nDetail	);
	ASSERT( GetDetailSectorInfo()->m_nDepth[ nDetail ] != 0 );

	return ( m_fxEnd - m_fxStart ) / ( FLOAT ) GetDetailSectorInfo()->m_nDepth[ nDetail ] ;
}

FLOAT	ApWorldSector::GetStepSizeZ( int nDetail )
{
	ASSERT( IsLoadedDetailData()							);
	ASSERT( 0 <= nDetail && SECTOR_DETAILDEPTH > nDetail	);
	ASSERT( GetDetailSectorInfo()->m_nDepth[ nDetail ] != 0 );

	return ( m_fzEnd - m_fzStart ) / ( FLOAT ) GetDetailSectorInfo()->m_nDepth[ nDetail ] ;
}


BOOL	ApWorldSector::SetupSector		( INT32 indexX , INT32 indexY , INT32 indexZ			)
{
	ASSERT( FALSE == m_bInitialized );
	if( m_bInitialized ) return FALSE;
	// 섹터의 정보를 초기화 한다.

	// 데이타 세팅 
	m_nIndexX				= indexX						;
	m_nIndexZ				= indexZ						;

	m_fxStart				= GetSectorStartX	( indexX )	;
	m_fzStart				= GetSectorStartZ	( indexZ )	;
	m_fxEnd					= GetSectorEndX		( indexX )	;
	m_fzEnd					= GetSectorEndZ		( indexZ )	;

	// 마고자 (2005-03-15 오후 3:25:09) : 
	// 지오메트리 옵션 추가..
	m_uFlag					= ApWorldSector::OP_NONE		;
	m_uLoadingFlag			= ApWorldSector::LF_NONE		;

	// 데이타 초기화..
	m_pCompactSectorInfo	= NULL							;
	m_pDetailSectorInfo		= NULL							;
	m_pHeightPool			= NULL							;

	m_bInitialized			= TRUE							;
	m_uQueueOffset			= -1							;

	// 락 초기화
	m_RWLock.Initialize();

	// 셋업은 섹터의 사용 준비만 해두는 것이다.
	// 일단 이것만 해두어도 , 오브젝트,케릭터등 리스트 관리는 가능하다.

	VERIFY( CreateDimension( 0 ) );

	return TRUE;
}

//BOOL	ApWorldSector::_LoadSectorOld	( INT32 indexX , INT32 indexY , INT32 indexZ			)
//{
//	ASSERT( FALSE == m_bInitialized );
//	if( m_bInitialized ) return FALSE;
//	// 섹터의 정보를 초기화 한다.
//
//
//	// 데이타 세팅 
//	m_nIndexX				= indexX										;
//	m_nIndexZ				= indexZ										;
//
//	m_fxStart				= ( float ) indexX * MAP_SECTOR_WIDTH			;
//	m_fzStart				= ( float ) indexZ * MAP_SECTOR_HEIGHT			;
//	m_fxEnd					= ( float ) ( indexX + 1 ) * MAP_SECTOR_WIDTH	;
//	m_fzEnd					= ( float ) ( indexZ + 1 ) * MAP_SECTOR_HEIGHT	;
//
//	// 데이타 초기화..
//	m_pCompactSectorInfo	= NULL											;
//	m_pDetailSectorInfo		= NULL											;
//	DeleteAllIndexArray();
//	m_pHeightPool			= NULL											;
//
//	m_bInitialized			= TRUE											;
//
//	// 셋업은 섹터의 사용 준비만 해두는 것이다.
//	// 일단 이것만 해두어도 , 오브젝트,케릭터등 리스트 관리는 가능하다.
//
//	// 이제 디테일 데이타 로딩..
//	// 디테일 섹터 정보를 저장할 메모리 할당함..
//
//	LoadCompactData	();
//
//	// 이전 데이타 로드
//	LoadDetailData	();
//
//	return TRUE;
//}

BOOL ApWorldSector::LoadSector( BOOL bLoadCompact , BOOL bLoadDetail )
{
	ASSERT( TRUE == m_bInitialized		);
	ASSERT( bLoadCompact || bLoadDetail );

	if( bLoadCompact )
		return LoadCompactData	();

	// 새 데이타 로드..
	if( bLoadDetail && LoadDetailDataGeometry() )
		return LoadDetailDataTile();

	return FALSE;
}

BOOL	ApWorldSector::LoadSectorServer()
{
	INT32 nDivision = GetDivisionIndex( ( int ) GetArrayIndexX() , ( int ) GetArrayIndexZ() );

	char	strFilename[ 1024 ];
	char	strFullPath[ 1024 ];

	wsprintf( strFilename , COMPACT_SERVER_FORMAT , nDivision );
	wsprintf( strFullPath , "%s\\server\\%s" ,  m_pModuleMap->GetWorldDirectory() , strFilename );

	FILE * pFile = fopen( strFullPath , "rb" );
	if( pFile )
	{
		// 버젼 확인
		UINT32	uVersion;
		fread( ( void * ) &uVersion , sizeof( UINT32 ) , 1 , pFile );
		if( uVersion == SERVER_DATA_VERSION )
		{
			// 새 데이타..
			INT32 nDivisionOffset = GetDivisionOffset();
			ASSERT( nDivisionOffset < 256 );

			UINT32	auOffset[ 256 ];
			fread( ( void * ) auOffset , sizeof( UINT32 ) , 256 , pFile );
			UINT32 uSize;

			if( nDivisionOffset == 255 )
			{
				// 마지막 꺼면.. 끝까지 읽는다.
				fseek( pFile , 0 , SEEK_END );
				uSize = ftell( pFile ) - auOffset[ nDivisionOffset ];
			}
			else
			{
				uSize = auOffset[ nDivisionOffset + 1 ] - auOffset[ nDivisionOffset ];
			}

			fseek( pFile , auOffset[ nDivisionOffset ] , SEEK_SET );

			BYTE pBuffer[ 65535 ];
			fread( ( void * ) pBuffer , sizeof( char ) , uSize , pFile );
			fclose( pFile );

			return _LoadCompactData_FromBuffer( pBuffer , uSize );
		}
		else
		{
			// 옛날데이타..
			// 처음으로 옮겨놓고..
			fseek( pFile , 0 , SEEK_SET );

			UINT32 uSize = 0;
			uSize += sizeof( UINT32 ); // version
			uSize += sizeof( UINT32 ); // flag;
			uSize += sizeof( int ); // Depth ( 16 고정 );
			uSize += ( sizeof ApCompactSegment ) * 16 * 16;

			fseek( pFile , uSize *  GetDivisionOffset() , SEEK_SET );

			BYTE pBuffer[ 65535 ];
			fread( ( void * ) pBuffer , sizeof( char ) , uSize , pFile );
			fclose( pFile );

			return _LoadCompactData_FromBuffer( pBuffer , uSize );
		}
	}

	return TRUE;
}

// Parn 작업, OTree 생성하기 위해서 Init만들다.
BOOL			ApWorldSector::Init				( float width , float unitsize , BOOL bBlock )
{
	// OTree가 Init되어 쓰이고 있었으면, DeleteLeaf하고, NOT_READY 상태로 만든다.
	if( m_csOTree.IsReady())
	{
		m_csOTree.Destroy();
	}

	// Blocking 정보를 만들기 위해서 OTree를 Init한다.
	if( !m_csOTree.Init( m_fxStart, m_fyStart, m_fzStart, width, unitsize, bBlock ) )
		return FALSE;

	return TRUE;
}

// AddObject로 들어간 Blocking 정보를 가지고 OTree를 Optimize한다.
void			ApWorldSector::OptimizeBlocking()
{
	m_csOTree.Optimize();
}

BOOL	ApWorldSector::AddChar			( INT32	nDimensionIndex , AuPOS pos , INT_PTR cid , INT_PTR cid2 , INT32 range	)
{
	ASSERT( m_bInitialized							);

	IdPos *	pUser;
	if (m_pModuleMap->m_csMemoryPool.GetTypeIndex() != INVALID_INDEX)
	{
		pUser = (IdPos*)m_pModuleMap->m_csMemoryPool.Alloc();
	}
	else
	{
		pUser	= new IdPos;
	}

	if (!pUser)
		return FALSE;

	pUser->id		= cid;
	pUser->id2		= cid2;
	pUser->pos		= pos;
	pUser->range	= range;

	return AddChar( nDimensionIndex , pUser );
}

BOOL ApWorldSector::AddChar				( INT32	nDimensionIndex , IdPos *pUser	)
{
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;
	ApAutoWriterLock	csLock( pDimension->lockUsers );

	pUser->pNext	= pDimension->pUsers;
	pUser->pPrev	= NULL;

	if (pDimension->pUsers)
		pDimension->pUsers->pPrev = pUser;

	pDimension->pUsers		= pUser;

	return TRUE;
}

ApWorldSector::IdPos *	ApWorldSector::GetChar			( INT32	nDimensionIndex , INT_PTR cid , BOOL bIsNeedLock	)
{
	ASSERT( m_bInitialized							);

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return NULL;

	if (bIsNeedLock)
		pDimension->lockUsers.LockReader();

	IdPos *	pUser = pDimension->pUsers;

	while (pUser)
	{
		if (pUser->id == cid )
		{
			if (bIsNeedLock)
				pDimension->lockUsers.UnlockReader();

			return pUser;
		}

		pUser = pUser->pNext;
	}

	if (bIsNeedLock)
		pDimension->lockUsers.UnlockReader();

	return NULL;
}

BOOL	ApWorldSector::UpdateChar		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid	)
{
	ASSERT( m_bInitialized							);

	// 포지션 변경은 Read Lock 만 설정.
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;
	ApAutoReaderLock	csLock( pDimension->lockUsers );

	IdPos *	pUser = GetChar( nDimensionIndex , cid, FALSE );

	if (!pUser)
		return FALSE;

	pUser->pos	= pos;

	return TRUE;
}
BOOL	ApWorldSector::DeleteChar		( INT32	nDimensionIndex , INT_PTR cid						)
{
	ASSERT( m_bInitialized							);
	// ID 삭제.

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;
	ApAutoWriterLock	csLock( pDimension->lockUsers );

	IdPos *	pUser = GetChar( nDimensionIndex , cid, FALSE );

	if (!pUser)
		return FALSE;

	RemoveChar( nDimensionIndex , pUser, FALSE );

	if (m_pModuleMap->m_csMemoryPool.GetTypeIndex() != INVALID_INDEX)
	{
		m_pModuleMap->m_csMemoryPool.Free(pUser);
	}
	else
	{
		delete pUser;
	}

	return TRUE;
}

VOID	ApWorldSector::RemoveChar		( INT32	nDimensionIndex , IdPos *pUser ,	BOOL bIsNeedLock		)
{
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return;

	if (bIsNeedLock)
		pDimension->lockUsers.LockWriter();

	if (pUser->pNext)
		pUser->pNext->pPrev = pUser->pPrev;

	if (pUser->pPrev)
		pUser->pPrev->pNext = pUser->pNext;
	else
		pDimension->pUsers = pUser->pNext;

	if (bIsNeedLock)
		pDimension->lockUsers.UnlockWriter();
}

BOOL	ApWorldSector::AddNPC			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2 ,	INT32 range	)
{
	ASSERT( m_bInitialized							);

	IdPos *	pNPC;
	if (m_pModuleMap->m_csMemoryPool.GetTypeIndex() != INVALID_INDEX)
	{
		pNPC = (IdPos*)m_pModuleMap->m_csMemoryPool.Alloc();
	}
	else
	{
		pNPC	= new IdPos;
	}

	if (!pNPC)
		return FALSE;

	pNPC->id		= cid	;
	pNPC->id2		= cid2	;
	pNPC->pos		= pos	;
	pNPC->range		= range	;

	return AddNPC( nDimensionIndex , pNPC );
}

BOOL ApWorldSector::AddNPC				( INT32	nDimensionIndex , IdPos *pNPC	)
{
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;
	ApAutoWriterLock	csLock( pDimension->lockNPCs );

	pNPC->pNext	= pDimension->pNPCs;
	pNPC->pPrev	= NULL;

	if (pDimension->pNPCs)
		pDimension->pNPCs->pPrev = pNPC;

	pDimension->pNPCs	= pNPC;

	return TRUE;
}

ApWorldSector::IdPos *	ApWorldSector::GetNPC			( INT32	nDimensionIndex , INT_PTR cid , BOOL bIsNeedLock	)
{
	ASSERT( m_bInitialized							);

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return NULL;

	if (bIsNeedLock)
		pDimension->lockNPCs.LockReader();

	IdPos *	pNPC = pDimension->pNPCs;

	while (pNPC)
	{
		if (pNPC->id == cid )
		{
			if (bIsNeedLock)
				pDimension->lockNPCs.UnlockReader();

			return pNPC;
		}

		pNPC = pNPC->pNext;
	}

	if (bIsNeedLock)
		pDimension->lockNPCs.UnlockReader();

	return NULL;
}

BOOL	ApWorldSector::UpdateNPC		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid	)
{
	ASSERT( m_bInitialized							);

	// 포지션 변경은 Read Lock 만 설정.
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;
	ApAutoReaderLock	csLock( pDimension->lockNPCs );

	IdPos *	pNPC = GetNPC( nDimensionIndex , cid, FALSE );

	if (!pNPC)
		return FALSE;

	pNPC->pos	= pos;

	return TRUE;
}
BOOL	ApWorldSector::DeleteNPC		( INT32	nDimensionIndex , INT_PTR cid						)
{
	ASSERT( m_bInitialized							);
	// ID 삭제.

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;
	ApAutoWriterLock	csLock( pDimension->lockNPCs );

	IdPos *	pNPC = GetNPC( nDimensionIndex , cid, FALSE );

	if (!pNPC)
		return FALSE;

	RemoveNPC( nDimensionIndex , pNPC , FALSE );

	if (m_pModuleMap->m_csMemoryPool.GetTypeIndex() != INVALID_INDEX)
	{
		m_pModuleMap->m_csMemoryPool.Free(pNPC);
	}
	else
	{
		delete pNPC;
	}

	return TRUE;
}

VOID	ApWorldSector::RemoveNPC		( INT32	nDimensionIndex , IdPos *pNPC ,		BOOL bIsNeedLock	)
{
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return;

	if (bIsNeedLock)
		pDimension->lockNPCs.LockWriter();

	if (pNPC->pNext)
		pNPC->pNext->pPrev = pNPC->pPrev;

	if (pNPC->pPrev)
		pNPC->pPrev->pNext = pNPC->pNext;
	else
		pDimension->pNPCs = pNPC->pNext;

	if (bIsNeedLock)
		pDimension->lockNPCs.UnlockWriter();
}

BOOL	ApWorldSector::AddMonster			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2 ,		INT32 range	)
{
	ASSERT( m_bInitialized							);

	IdPos *	pMonster;
	if (m_pModuleMap->m_csMemoryPool.GetTypeIndex() != INVALID_INDEX)
	{
		pMonster = (IdPos*)m_pModuleMap->m_csMemoryPool.Alloc();
	}
	else
	{
		pMonster	= new IdPos;
	}

	if (!pMonster)
		return FALSE;

	pMonster->id		= cid;
	pMonster->id2		= cid2;
	pMonster->pos		= pos;
	pMonster->range	= range;

	return AddMonster( nDimensionIndex , pMonster );
}

BOOL ApWorldSector::AddMonster				( INT32	nDimensionIndex , IdPos *pMonster	)
{
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;

	ApAutoWriterLock	csLock( pDimension->lockMonsters );

	pMonster->pNext	= pDimension->pMonsters;
	pMonster->pPrev	= NULL;

	if (pDimension->pMonsters)
		pDimension->pMonsters->pPrev = pMonster;

	pDimension->pMonsters		= pMonster;

	return TRUE;
}

ApWorldSector::IdPos *	ApWorldSector::GetMonster			( INT32	nDimensionIndex , INT_PTR cid , BOOL bIsNeedLock	)
{
	ASSERT( m_bInitialized	);

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return NULL;

	if (bIsNeedLock)
		pDimension->lockMonsters.LockReader();

	IdPos *	pMonster = pDimension->pMonsters;

	while (pMonster)
	{
		if (pMonster->id == cid )
		{
			if (bIsNeedLock)
				pDimension->lockMonsters.UnlockReader();

			return pMonster;
		}

		pMonster = pMonster->pNext;
	}

//	ASSERT( !"해당 케릭터 ID 가 섹터에 존재하지 않습니다."	);

	if (bIsNeedLock)
		pDimension->lockMonsters.UnlockReader();

	return NULL;
}

BOOL	ApWorldSector::UpdateMonster		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid	)
{
	ASSERT( m_bInitialized							);

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;

	ApAutoReaderLock	csLock( pDimension->lockMonsters );

	IdPos *	pMonster = GetMonster( nDimensionIndex , cid, FALSE );

	if (!pMonster)
		return FALSE;

	pMonster->pos	= pos;

	return TRUE;
}
BOOL	ApWorldSector::DeleteMonster		( INT32	nDimensionIndex , INT_PTR cid						)
{
	ASSERT( m_bInitialized							);
	// ID 삭제.

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return NULL;

	ApAutoWriterLock csLock(pDimension->lockMonsters);

	IdPos *	pMonster = GetMonster( nDimensionIndex , cid, FALSE );

	if (!pMonster)
		return FALSE;

	RemoveMonster( nDimensionIndex , pMonster , FALSE );

	if (m_pModuleMap->m_csMemoryPool.GetTypeIndex() != INVALID_INDEX)
	{
		m_pModuleMap->m_csMemoryPool.Free(pMonster);
	}
	else
	{
		delete pMonster;
	}

	return TRUE;
}

VOID	ApWorldSector::RemoveMonster		( INT32	nDimensionIndex , IdPos *pMonster ,		BOOL bIsNeedLock	)
{
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return;

	if (bIsNeedLock)
		pDimension->lockMonsters.LockWriter();

	if (pMonster->pNext)
		pMonster->pNext->pPrev = pMonster->pPrev;

	if (pMonster->pPrev)
		pMonster->pPrev->pNext = pMonster->pNext;
	else
		pDimension->pMonsters = pMonster->pNext;

	if (bIsNeedLock)
		pDimension->lockMonsters.UnlockWriter();
}

BOOL	ApWorldSector::AddItem			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid	)
{
	ASSERT( m_bInitialized							);

	IdPos *	pItem;
	if (m_pModuleMap->m_csMemoryPool.GetTypeIndex() != INVALID_INDEX)
	{
		pItem = (IdPos*)m_pModuleMap->m_csMemoryPool.Alloc();
	}
	else
	{
		pItem	= new IdPos;
	}

	if (!pItem)
		return FALSE;

	pItem->id		= iid;
	pItem->pos		= pos;

	return AddItem( nDimensionIndex , pItem );
}

BOOL ApWorldSector::AddItem				( INT32	nDimensionIndex , IdPos *pItem					)
{
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;

	ApAutoWriterLock	csLock( pDimension->lockItems );

	pItem->pNext	= pDimension->pItems;
	pItem->pPrev	= NULL;

	if (pDimension->pItems)
		pDimension->pItems->pPrev = pItem;

	pDimension->pItems		= pItem;

	return TRUE;
}

ApWorldSector::IdPos *	ApWorldSector::GetItem			( INT32	nDimensionIndex , INT_PTR iid , BOOL bIsNeedLock		)
{
	ASSERT( m_bInitialized							);

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return NULL;

	if (bIsNeedLock)
		pDimension->lockItems.LockReader();

	IdPos *	pItem = pDimension->pItems;

	while (pItem)
	{
		if (pItem->id == iid )
		{
			if (bIsNeedLock)
				pDimension->lockItems.UnlockReader();

			return pItem;
		}

		pItem = pItem->pNext;
	}

//	ASSERT( !"해당 아이템 ID 가 섹터에 존재하지 않습니다."	);

	if (bIsNeedLock)
		pDimension->lockItems.UnlockReader();

	return NULL;
}

BOOL	ApWorldSector::UpdateItem		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid	)
{
	ASSERT( m_bInitialized							);
	// 포지션 업데이트.

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;

	ApAutoReaderLock	csLock( pDimension->lockItems );

	IdPos *	pItem = GetItem( nDimensionIndex , iid, FALSE );

	if( !pItem )
		return FALSE;

	pItem->pos	= pos;

	return TRUE;
}

BOOL	ApWorldSector::DeleteItem		( INT32	nDimensionIndex , INT_PTR iid						)
{
	ASSERT( m_bInitialized							);
	// ID 삭제.
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;

	ApAutoWriterLock	csLock( pDimension->lockItems );

	IdPos *	pItem = GetItem( nDimensionIndex , iid, FALSE );

	if( !pItem )
		return FALSE;

	RemoveItem( nDimensionIndex , pItem , FALSE );

	if (m_pModuleMap->m_csMemoryPool.GetTypeIndex() != INVALID_INDEX)
	{
		m_pModuleMap->m_csMemoryPool.Free(pItem);
	}
	else
	{
		delete pItem;
	}

	return TRUE;
}

VOID	ApWorldSector::RemoveItem		( INT32	nDimensionIndex , IdPos *pItem ,	BOOL bIsNeedLock	)
{
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return;

	if (bIsNeedLock)
		pDimension->lockItems.LockWriter();

	if (pItem->pNext)
		pItem->pNext->pPrev = pItem->pPrev;

	if (pItem->pPrev)
		pItem->pPrev->pNext = pItem->pNext;
	else
		pDimension->pItems = pItem->pNext;

	if (bIsNeedLock)
		pDimension->lockItems.UnlockWriter();
}

BOOL	ApWorldSector::AddObject		( INT32	nDimensionIndex , AuPOS pos , INT_PTR oid , AuBLOCKING *pstBlocking	)
{
	ASSERT( m_bInitialized								);

	IdPos *	pObject	= new IdPos;
	if (!pObject)
		return FALSE;

	pObject->id		= oid;
	pObject->pos	= pos;

	return AddObject( nDimensionIndex , pObject, pstBlocking );
}

BOOL	ApWorldSector::AddObject		( INT32	nDimensionIndex , IdPos *pObject , AuBLOCKING *pstBlocking )
{
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;

	ApAutoWriterLock	csLock( pDimension->lockObjects );

	pObject->pNext	= pDimension->pObjects;
	pObject->pPrev	= NULL;

	if (pDimension->pObjects)
		pDimension->pObjects->pPrev = pObject;

	pDimension->pObjects		= pObject;

	// 이런작업은 0번 계에서만 처리..
	if( pstBlocking && nDimensionIndex == 0 )
		m_csOTree.AddBlocking( pstBlocking );

	return TRUE;
}

ApWorldSector::IdPos *	ApWorldSector::GetObject			( INT32	nDimensionIndex , INT_PTR oid	, BOOL bIsNeedLock	)
{
	ASSERT( m_bInitialized							);

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return NULL;

	if (bIsNeedLock)
		pDimension->lockObjects.LockReader();

	IdPos *	pObject = pDimension->pObjects;

	while (pObject)
	{
		if (pObject->id == oid )
		{
			if (bIsNeedLock)
				pDimension->lockObjects.UnlockReader();

			return pObject;
		}

		pObject = pObject->pNext;
	}

	// 마고자 (2004-04-28 오후 12:43:34) : 툴에서 죽어서 삭제함.
	//TRACE( "해당 오브젝트 ID 가 섹터에 존재하지 않습니다."	);
	// ASSERT( !"해당 오브젝트 ID 가 섹터에 존재하지 않습니다."	);

	if (bIsNeedLock)
		pDimension->lockObjects.UnlockReader();

	return NULL;
}

BOOL	ApWorldSector::UpdateObject		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid	)
{
	ASSERT( m_bInitialized							);
	// 포지션 업데이트.

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;

	ApAutoReaderLock	csLock( pDimension->lockObjects );

	IdPos *	pObject = GetObject( nDimensionIndex , oid, FALSE );

	if( !pObject )
		return FALSE;

	pObject->pos	= pos;

	return TRUE;
}

BOOL	ApWorldSector::DeleteObject		( INT32	nDimensionIndex , INT_PTR oid						)
{
	ASSERT( m_bInitialized							);
	// ID 삭제.

	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return FALSE;

	ApAutoWriterLock	csLock( pDimension->lockObjects );

	IdPos *	pObject = GetObject( nDimensionIndex , oid, FALSE );

	if( !pObject )
		return FALSE;

	RemoveObject( nDimensionIndex , pObject , FALSE );

	delete pObject;

	return TRUE;
}

VOID	ApWorldSector::RemoveObject	( INT32	nDimensionIndex , IdPos *pObject ,	BOOL bIsNeedLock		)
{
	Dimension * pDimension = GetDimension( nDimensionIndex );
	if( NULL == pDimension ) return;

	if (bIsNeedLock)
		pDimension->lockObjects.LockWriter();

	if (pObject->pNext)
		pObject->pNext->pPrev = pObject->pPrev;

	if (pObject->pPrev)
		pObject->pPrev->pNext = pObject->pNext;
	else
		pDimension->pObjects = pObject->pNext;

	if (bIsNeedLock)
		pDimension->lockObjects.UnlockWriter();
}

BOOL			ApWorldSector::IsPassThis		( AuPOS pos1 , AuPOS pos2 , AuPOS *pCollisionPoint )
{
	ASSERT( m_bInitialized	);
	// --;
	return FALSE;
}

FLOAT	ApWorldSector::GetHeight		( FLOAT x , FLOAT y , FLOAT z	)	// 해당 좌표의 이론적 높이를 계산해낸다.
{
	ASSERT( m_bInitialized	);

	if( IsLoadedDetailData() )
	{
		return D_GetHeight( x , z );
	}
	else if ( IsLoadedCompactData() )
	{
		return C_GetHeight( x , y , z );
	}
	// ASSERT!

	// ASSERT( !"데이타가 로딩돼지 않았습니다." );
	return 0.0f;
}

UINT8	ApWorldSector::GetType			( FLOAT x , FLOAT y , FLOAT z	)	// 바닥의 타입을 얻어낸다.
{
	ASSERT( m_bInitialized	);

	if( IsLoadedDetailData() )
	{
		ASSERT( !"아직 구현돼지 않았음!" );
	}
	else if ( IsLoadedCompactData() )
	{
		return C_GetType( x , y , z );
	}
	// ASSERT!

	// ASSERT( !"데이타가 로딩돼지 않았습니다." );
	return 0;
}

UINT8	ApWorldSector::GetBlocking		( FLOAT x , FLOAT y , FLOAT z , INT32 eType )
{
	ASSERT( m_bInitialized	);

// 마고자 (2003-03-06 오후 5:21:59) : 블러킹 잠시 처리하지 않음.. 
	if( IsLoadedDetailData() )
	{
		ApDetailSegment *	pSegment = D_GetSegment( x , z );
		if( NULL == pSegment ) return 0;

		int	xBlock , zBlock;
		xBlock = ( int ) ( ( x - GetXStart() ) / ( MAP_STEPSIZE / 2.0f ) );
		zBlock = ( int ) ( ( z - GetZStart() ) / ( MAP_STEPSIZE / 2.0f ) );

		return pSegment->stTileInfo.GetBlocking( xBlock % 2 + ( zBlock % 2 ) * 2 );
	}
	else if ( IsLoadedCompactData() )
	{
		return C_GetBlocking( x , y , z , eType );
	}
	else
	{
		// 이쪽으로 들어오는건 있을수 없는 일이다..
		// 어설트를 때리던가 할까나..
		// ASSERT( !"ApWorldSector::GetBlocking 발생하면 안돼는 코드예요~" );

		// 2004/05/04 6시 30분이라고함.
		// 마고자
		// 릴레이써버에서 맵데이타를 로딩하지 않기 때문에..
		// 기냥 처리 안해버림..
		// 그냥 클라이언트나 써버에선 별 문제없을거로 샤료됨.

		return ApTileInfo::BLOCKNONE;
	}
}

// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...
// Compact ApDetailSegment Funcitons...

BOOL	ApWorldSector::AllocCompactSectorInfo	()
{
	if( m_pCompactSectorInfo )
		return TRUE;

	m_pCompactSectorInfo = new ApCompactSectorInfo;
	ASSERT( NULL != m_pCompactSectorInfo );

	// 데이타 초기화..

	m_pCompactSectorInfo->m_pCompactSegmentArray	= NULL	;
	m_pCompactSectorInfo->m_nCompactSegmentDepth	= 0		;

	return TRUE;
}

BOOL	ApWorldSector::FreeCompactSectorInfo	()
{
	if( m_pCompactSectorInfo )
	{
		m_pCompactSectorInfo->Free();
		delete m_pCompactSectorInfo;
		m_pCompactSectorInfo = NULL;
	}

	return TRUE;
}

BOOL	ApWorldSector::LoadServerData()
{
	// 서버데이타 로딩 구현부
	INT32 nDivision = GetDivisionIndex( ( int ) GetArrayIndexX() , ( int ) GetArrayIndexZ() );

	static FILE *	psFile				= NULL	;
	static INT32	nsPrevLoadDivision	= 0		;

	this->m_ServerSectorInfo.FreeVector();
	
	if( nDivision == nsPrevLoadDivision && psFile )
	{
		// 처음으로 밀고..
		fseek( psFile , 0 , SEEK_SET );
	}
	else
	{
		if( psFile )
		{
			// 열려있던건 닫아버리고..
			fclose( psFile );
			psFile				= NULL	;
			nsPrevLoadDivision	= 0		;
		}

		// 파일열고
		char strFilename[ 256 ];
		wsprintf( strFilename , SECTOR_SERVERDATA_FILE , nDivision );
		char strPath[ 1024 ];
		wsprintf( strPath , "%s\\server\\%s" , m_pModuleMap->GetWorldDirectory() , strFilename );

		psFile = fopen( strPath , "rb" );
		if( NULL == psFile ) return FALSE;
		
		nsPrevLoadDivision = nDivision;
	}

	{
		int nVersion;
		fread( ( void * ) &nVersion , sizeof( int ) , 1 , psFile );

		switch( nVersion )
		{
		case ALEF_SERVER_DATA_VERSION:
			{
				// 제대로임..
			}
			break;
		default:
			{
				// 에러..
				fclose( psFile );
				psFile				= NULL	;
				nsPrevLoadDivision	= 0		;
				return FALSE;
			}
		}
	}

	// 옵셋만큼 건너뛴다.
	int nTotalCount = ( int ) GetDivisionOffset();

	// 임시 변수들..
	int nCount					;
	int nVectorBlocking			;
	int nVectorBlockingRidable	;
	int nVectorRidable			;

	for( int i = 0 ; i < nTotalCount ; i ++ )
	{
		fread( ( void * ) &nCount					, sizeof( int ), 1 , psFile );
		fread( ( void * ) &nVectorBlocking			, sizeof( int ), 1 , psFile );
		fread( ( void * ) &nVectorBlockingRidable	, sizeof( int ), 1 , psFile );
		fread( ( void * ) &nVectorRidable			, sizeof( int ), 1 , psFile );

		fseek( psFile , nCount * sizeof( AuPOS ) , SEEK_CUR );
	}

	// 제대로 읽어들인다.
	fread( ( void * ) &nCount					, sizeof( int ), 1 , psFile );
	fread( ( void * ) &nVectorBlocking			, sizeof( int ), 1 , psFile );
	fread( ( void * ) &nVectorBlockingRidable	, sizeof( int ), 1 , psFile );
	fread( ( void * ) &nVectorRidable			, sizeof( int ), 1 , psFile );

	// 버택스 버퍼 할당.
	this->m_ServerSectorInfo.AllocVector( nCount );
	 
	fread( ( void * ) this->m_ServerSectorInfo.GetVector() , sizeof( AuPOS ), nCount , psFile );

	this->m_ServerSectorInfo.SetBlockingVertexCount	( nVectorBlocking	+ nVectorBlockingRidable	);
	this->m_ServerSectorInfo.SetRidableVertexCount	( nVectorRidable	+ nVectorBlockingRidable	);

	// 원래 섹터 부분을 찾는다.  -> 디비젼 내에서의 옵셋으로 계산함.

	// 버택스와 각 변수를 읽어 들인다.

	return TRUE;
}

// 로딩 관련..
BOOL ApWorldSector::LoadCompactData()	//	Compact 데이타를 읽어들임.
{
	ASSERT( NULL != m_pModuleMap );

//#ifdef _DEBUG
//	char strCurDir[ 1024 ];
//	GetCurrentDirectory( 1024 , strCurDir );
//	TRACE( "현재 디렉토리 %s\n" , strCurDir );
//#endif
		
	CMagUnpackManager* pUnPackManager = 
		m_pModuleMap->GetUnpackManagerDivisionCompact( GetDivisionIndex( GetArrayIndexX() , GetArrayIndexZ() ) );

	if( pUnPackManager && pUnPackManager->GetFileCount() )
	{
		UINT nTotalCount = GetDivisionOffset();
		BYTE* pBuffer;

		UINT uPackedSize;
		if( uPackedSize = pUnPackManager->GetFileBinary( nTotalCount , pBuffer ) )
			return _LoadCompactData_FromBuffer( pBuffer , uPackedSize );
	}

	return FALSE;
}

//BOOL	ApWorldSector::_LoadCompactData_Old					()	//	Compact 데이타를 읽어들임.
//{
//	ASSERT( m_bInitialized	);
//	// 파일이름 기준으로하여 출력함.
//	if( IsLoadedCompactData() )
//	{
//		// TRACE( "이미로딩 돼어 있는 클라이언트 데이타 로딩 요구\n" );
//		return FALSE;
//	}
//
//	char	filename[ 256 ];
//	sprintf( filename , "%s\\c%d,%d.amf"					,
//		ApWorldSector::m_pModuleMap->GetMapCompactDirectory()	,
//		GetArrayIndexX()										,
//		GetArrayIndexZ()										);
//
//	FILE	*pFile = fopen( filename , "rb" );
//	if( !pFile )
//	{
//		TRACE( "LoadCompactData (%d,%d) 지역 데이타가 없습니다.!\n" , m_nIndexX , m_nIndexZ );
////		TRACEFILE(ALEF_ERROR_FILENAME, "LoadCompactData (%d,%d) 지역 데이타가 없습니다.!\n" , m_nIndexX , m_nIndexZ );
//		return FALSE;
//	}
//
//	VERIFY( AllocCompactSectorInfo() );
//	
//	INT32	nDepth;
//	// 데이타 저장.
//	fread( ( void * ) & nDepth	, sizeof ( int ) , 1	, pFile );
//
//	VERIFY( m_pCompactSectorInfo->Alloc( nDepth ) );
//
//	VERIFY(	fread( ( void * ) m_pCompactSectorInfo->m_pCompactSegmentArray , sizeof ApCompactSegment	,
//			m_pCompactSectorInfo->m_nCompactSegmentDepth * m_pCompactSectorInfo->m_nCompactSegmentDepth	,
//			pFile																						) );
//
//	VERIFY( 0 == fclose( pFile ) );
//
//	return FALSE;
//}

BOOL	ApWorldSector::_LoadCompactData_FromBuffer		( BYTE * pBuffer , UINT uSize )
{
	ASSERT( m_bInitialized	);
	// 파일이름 기준으로하여 출력함.
	if( IsLoadedCompactData() )
	{
		// TRACE( "이미로딩 돼어 있는 클라이언트 데이타 로딩 요구\n" );
		return FALSE;
	}

	VERIFY( AllocCompactSectorInfo() );

	// 버젼정보 읽기..
	UINT32	uVersion;
	INT32	nDepth;

	BYTE	*pBufferStart = pBuffer;

	memcpy( ( void * ) & uVersion , ( void * ) pBuffer , sizeof ( UINT32 ) );
	pBuffer += 4;

	// 버젼별 처리..
	switch( uVersion )
	{
	case ALEF_COMPACT_FILE_VERSION4:
		{
			DWORD	uFlag;
			memcpy( ( void * ) &uFlag , ( void * ) ( pBuffer ), sizeof ( int ) );
			pBuffer += 4;

			this->m_uFlag = uFlag;

			if( ApWorldSector::OP_DONOTLOADSECTOR & uFlag )
			{
				// 읽어들일 필요가 없으므로..
				// 스키입~
				
				return FALSE;
			}

			memcpy( ( void * ) &nDepth , ( void * ) ( pBuffer ), sizeof ( int ) );
			pBuffer += 4;

			VERIFY( m_pCompactSectorInfo->Alloc( nDepth ) );

			memcpy( ( void * ) m_pCompactSectorInfo->m_pCompactSegmentArray	,
					( void * ) ( pBuffer )								,
					( sizeof ApCompactSegment ) * nDepth * nDepth			);

			// 라인블러킹 정보 저장..
			pBuffer += ( sizeof ApCompactSegment ) * nDepth * nDepth;

			if( uSize > ( UINT ) ( pBuffer - pBufferStart ) )
			{
				INT32	nCount;
				memcpy( ( void * ) &nCount , ( void * ) ( pBuffer ), sizeof ( int ) );
				pBuffer += 4;

				ClearLineBlock();

				for( int i = 0 ; i < nCount ; i ++ )
				{
					AuLineBlock	*pLine = ( AuLineBlock * ) pBuffer;
					pBuffer += sizeof( AuLineBlock );

					AddLineBlock( * pLine );
				}
			}
		}
		break;

	case ALEF_COMPACT_FILE_VERSION3:
		{
			DWORD	uFlag;
			memcpy( ( void * ) &uFlag , ( void * ) ( pBuffer ), sizeof ( int ) );
			pBuffer += 4;

			this->m_uFlag = uFlag;

			if( ApWorldSector::OP_DONOTLOADSECTOR & uFlag )
			{
				// 읽어들일 필요가 없으므로..
				// 스키입~
				
				return FALSE;
			}

			memcpy( ( void * ) &nDepth , ( void * ) ( pBuffer ), sizeof ( int ) );
			pBuffer += 4;

			VERIFY( m_pCompactSectorInfo->Alloc( nDepth ) );

			memcpy( ( void * ) m_pCompactSectorInfo->m_pCompactSegmentArray	,
					( void * ) ( pBuffer )								,
					( sizeof ApCompactSegment ) * nDepth * nDepth			);

			// 라인블러킹 정보 저장..
			pBuffer += ( sizeof ApCompactSegment ) * nDepth * nDepth;

			// 호환성 코드.
			{
				int nTotal = nDepth * nDepth;
				for( int nIndex = 0 ; nIndex < nTotal ; nIndex++ )
				{
					m_pCompactSectorInfo->m_pCompactSegmentArray[ nIndex ].stTileInfo.bNoLayer = FALSE	;
					m_pCompactSectorInfo->m_pCompactSegmentArray[ nIndex ].stTileInfo.reserved = 0		;
				}
			}

			if( uSize > ( UINT ) ( pBuffer - pBufferStart ) )
			{
				INT32	nCount;
				memcpy( ( void * ) &nCount , ( void * ) ( pBuffer ), sizeof ( int ) );
				pBuffer += 4;

				ClearLineBlock();

				for( int i = 0 ; i < nCount ; i ++ )
				{
					AuLineBlock	*pLine = ( AuLineBlock * ) pBuffer;
					pBuffer += sizeof( AuLineBlock );

					AddLineBlock( * pLine );
				}
			}
		}
		break;
	case ALEF_COMPACT_FILE_VERSION2:
		{
			DWORD	uFlag;
			memcpy( ( void * ) &uFlag , ( void * ) ( pBuffer ), sizeof ( int ) );
			pBuffer += 4;

			this->m_uFlag = uFlag;

			if( ApWorldSector::OP_DONOTLOADSECTOR & uFlag )
			{
				// 읽어들일 필요가 없으므로..
				// 스키입~
				
				return FALSE;
			}

			memcpy( ( void * ) &nDepth , ( void * ) ( pBuffer ), sizeof ( int ) );
			pBuffer += 4;

			VERIFY( m_pCompactSectorInfo->Alloc( nDepth ) );

			//memcpy( ( void * ) m_pCompactSectorInfo->m_pCompactSegmentArray	,
			//		( void * ) ( pBuffer )								,
			//		( sizeof ApCompactSegment_20040917 ) * nDepth * nDepth			);

			ApCompactSegment_20040917 *pCompactArray = ( ApCompactSegment_20040917 * )pBuffer;

			// 마고자 (2005-12-13 오후 4:56:04) : 
			// 수동으로 복사한다.

			int x , z;
			for( z = 0 ; z < nDepth ; z++ )
			{
				for( x = 0 ; x < nDepth ; x++ )
				{
					ApCompactSegment *pCompactNew = m_pCompactSectorInfo->GetSegment( x , z );
					pCompactNew->stTileInfo		= pCompactArray[ x + z * nDepth ].stTileInfo;
					pCompactNew->uRegioninfo	= ( UINT16 ) pCompactArray[ x + z * nDepth ].uRegioninfo;
				}
			}
		}
		break;

	case ALEF_COMPACT_FILE_VERSION:
		// 최신버젼..
		{
			memcpy( ( void * ) &nDepth , ( void * ) ( pBuffer ), sizeof ( int ) );
			pBuffer += 4;

			VERIFY( m_pCompactSectorInfo->Alloc( nDepth ) );

			ApCompactSegment_20040917 *pCompactArray = ( ApCompactSegment_20040917 * )pBuffer;

			// 마고자 (2005-12-13 오후 4:56:04) : 
			// 수동으로 복사한다.

			int x , z;
			for( z = 0 ; z < nDepth ; z++ )
			{
				for( x = 0 ; x < nDepth ; x++ )
				{
					ApCompactSegment *pCompactNew = m_pCompactSectorInfo->GetSegment( x , z );
					pCompactNew->stTileInfo		= pCompactArray[ x + z * nDepth ].stTileInfo;
					pCompactNew->uRegioninfo	= ( UINT16 ) pCompactArray[ x + z * nDepth ].uRegioninfo;
				}
			}
		}
		break;

	default:
		{
			// 마고자 (2005-12-13 오후 4:48:55) : 
			ASSERT( !"컴펙트 데이타 버젼이 다름. 다시 익스포트하세요" );
			return FALSE;
			/*
			// 옜날 버전...
			// 버젼 정보가 뎁스임..
			nDepth = ( INT32 ) uVersion;

			VERIFY( m_pCompactSectorInfo->Alloc( nDepth ) );

			for( int i = 0 ; i < nDepth * nDepth ; ++ i )
			{
				memcpy( ( void * ) m_pCompactSectorInfo->m_pCompactSegmentArray	,
						( void * ) ( pBuffer + i * APCOMPACTSEGMENTSIZE )	,
						APCOMPACTSEGMENTSIZE			);
			}

			// 리전과 리저브 값은 디폴트로 채워진다.
			*/
		}
		break;
	}

	return TRUE;
}

BOOL	ApWorldSector::SaveCompactData					(  BOOL bServer )	//	현재 로드됀 컴펙트 데이타를 저장함.
{
	ASSERT( m_bInitialized	);
	// 파일이름 기준으로하여 출력함.
	if( !IsLoadedCompactData() )
	{
		return FALSE;
	}

	char	filename[ 256 ];
	

	CreateDirectory( ApWorldSector::m_pModuleMap->GetMapCompactDirectory() , NULL );

	if( bServer )
	{
		sprintf( filename , "%s\\s%d,%d.amf"					,
			ApWorldSector::m_pModuleMap->GetMapCompactDirectory()	,
			GetArrayIndexX()										,
			GetArrayIndexZ()										);
	}
	else
	{
		sprintf( filename , "%s\\c%d,%d.amf"					,
			ApWorldSector::m_pModuleMap->GetMapCompactDirectory()	,
			GetArrayIndexX()										,
			GetArrayIndexZ()										);
	}

	FILE	*pFile = fopen( filename , "wb" );
	if( !pFile )
	{
		TRACE( "SaveCompactData (%d,%d) 지역 데이타가 없습니다.!\n" , m_nIndexX , m_nIndexZ );
//		TRACEFILE(ALEF_ERROR_FILENAME, "SaveCompactData (%d,%d) 지역 데이타가 없습니다.!\n" , m_nIndexX , m_nIndexZ );
		return FALSE;
	}

	// 버젼정보 저장..
	UINT32	uVersion = ALEF_COMPACT_FILE_VERSION4;
	fwrite( ( void * ) & uVersion	, sizeof ( UINT32 ) , 1	, pFile );

	// 마고자 (2005-07-19 오전 11:34:05) : 
	// 플래그 정보 기록..
	fwrite( ( void * ) & m_uFlag	, sizeof ( UINT32 ) , 1	, pFile );

	/*
	if( m_uFlag & ApWorldSector::OP_DONOTLOADSECTOR )
	{
		// 아무것도 하지 않는다.
	}
	else
	{
	*/

	// 좋든 싫든 라이팅.
		// 데이타 저장.
		fwrite( ( void * ) & m_pCompactSectorInfo->m_nCompactSegmentDepth	, sizeof ( int ) , 1	, pFile );
		fwrite( ( void * ) m_pCompactSectorInfo->m_pCompactSegmentArray, sizeof ApCompactSegment	,
			m_pCompactSectorInfo->m_nCompactSegmentDepth * m_pCompactSectorInfo->m_nCompactSegmentDepth	,
			pFile																						);

		if( !bServer )
		{
			// 라인블러킹 정보 저장..
			INT32	nCount = (INT32) m_vecBlockLine.size();
			fwrite( ( void * ) &nCount , sizeof INT32 , 1 , pFile );

			vector< AuLineBlock >::iterator iter;
			for( iter = m_vecBlockLine.begin() ;
				iter != m_vecBlockLine.end() ;
				iter ++ )
			{
				AuLineBlock	line = *iter;

				// AuLine 싸이즈로 읽는건 버근데 -_-;; 걍 이쪽에 맞추겠음.. 익스포트를 다시할 수 없으니 ;
				fwrite( ( void * ) &line , sizeof AuLineBlock , 1 , pFile );
			}
		}
	// }

	fclose( pFile );

	return TRUE;
}

BOOL	ApWorldSector::CreateCompactDataFromDetailInfo	( int nTargetDetail , int nDepth )	//	디테일 데이타에서 컴펙트 데이타를 생성함.
{
	// 마고자 (2004-08-24 오후 4:25:48) : 
	ASSERT( !"이거 사용안하고 , AgcmMap 에 있는거 쓰셔야함....이라고 해봐야 나밖에 안쓰나 -_-;;" );
	return FALSE;
	
	ASSERT( m_bInitialized						);
	ASSERT( IsLoadedDetailData( nTargetDetail )	);
	ASSERT( nDepth > 0							);
	ASSERT( nDepth == ALEF_COMPACT_DATA_DEFAULT_DEPTH );

	// 메모리 준비..
	VERIFY( FreeCompactSectorInfo	() );
	VERIFY( AllocCompactSectorInfo	() );
	ASSERT( NULL != m_pCompactSectorInfo	);
	ASSERT( NULL == m_pCompactSectorInfo->m_pCompactSegmentArray	);

	// 메모리 할당..

	m_pCompactSectorInfo->Alloc( nDepth );
	ASSERT( NULL != m_pCompactSectorInfo->m_pCompactSegmentArray	);
	ASSERT( nDepth == m_pCompactSectorInfo->m_nCompactSegmentDepth 	);

	// 데이타 모으기
	int		x , z	;

	ApDetailSegment	*	pSegment		;
	ApCompactSegment *	pCompactSegment	;

	for( z = 0 ; z < nDepth ; ++z )
	{
		for( x = 0 ; x < nDepth ; ++x )
		{
			// 세그먼트 정보 얻음..
			pSegment		= D_GetSegment( nTargetDetail , x , z );
			pCompactSegment	= m_pCompactSectorInfo->GetSegment( x , z );

			ASSERT( NULL != pSegment			);
			ASSERT( NULL != pCompactSegment		);

			// pCompactSegment->SetHeight( pSegment->height );

			pCompactSegment->stTileInfo.tiletype	= GetTileCompactType( pSegment->pIndex[ TD_FIRST ] );

			// 블러킹은 임시..
			pCompactSegment->stTileInfo.geometryblock	= pSegment->stTileInfo.geometryblock;
			pCompactSegment->stTileInfo.bNoLayer		= pSegment->stTileInfo.bNoLayer		;				
			pCompactSegment->stTileInfo.reserved		= pSegment->stTileInfo.reserved		;				
		}
	}

	return TRUE;
}

BOOL	ApWorldSector::RemoveCompactData				()	//	컴펙트 데이타 메모리에서 제거..
{
	ASSERT( m_bInitialized	);

	VERIFY( FreeCompactSectorInfo() );

	return TRUE;
}
BOOL	ApWorldSector::RemoveCompactDataFile			()	//	컴펙트 데이타 파일 제거.
{
	ASSERT( m_bInitialized	);

	ASSERT( !"아직 구현돼지 않았습니다." );

	return FALSE;
}

FLOAT	ApWorldSector::C_GetHeight						( FLOAT x , FLOAT y , FLOAT z )	// 해당 좌표의 이론적 높이를 계산해낸다.
{
	return INVALID_HEIGHT;
	/*
	ASSERT( m_bInitialized	);

	ApCompactSegment *	pSegment = C_GetSegment( x , y , z );
	ASSERT( NULL != pSegment );

	//return GET_REAL_HEIGHT_FROM_COMPACT_HEIGHT( pSegment->uiheight );
	return pSegment->GetHeight();
	*/
}

FLOAT	ApWorldSector::C_GetHeight						( INT32 nX , INT32 nZ			)
{
	return INVALID_HEIGHT;
	/*
	ApCompactSegment *	pSegment = m_pCompactSectorInfo->GetSegment( nX , nZ );
	if( pSegment )
	{
		//return GET_REAL_HEIGHT_FROM_COMPACT_HEIGHT( pSegment->uiheight );
		return pSegment->GetHeight();
	}
	else
	{
 		return INVALID_HEIGHT;
	}
	*/
}

UINT8	ApWorldSector::C_GetType						( FLOAT x , FLOAT y , FLOAT z )	// 바닥의 타입을 얻어낸다.
{
	ASSERT( m_bInitialized	);
	ApCompactSegment *	pSegment = C_GetSegment( x , y , z );
	ASSERT( NULL != pSegment );
	return pSegment->stTileInfo.tiletype;
}

UINT8	ApWorldSector::C_GetBlocking						( FLOAT x , FLOAT y , FLOAT z , INT32 eType )	// 바닥의 타입을 얻어낸다.
{
	ASSERT( m_bInitialized	);
	ApCompactSegment *	pSegment = C_GetSegment( x , y , z );
	// ASSERT( NULL != pSegment );
	if( NULL == pSegment ) return 0;

	int	xBlock , zBlock;
	xBlock = ( int ) ( ( x - GetXStart() ) / ( MAP_STEPSIZE / 2.0f ) );
	zBlock = ( int ) ( ( z - GetZStart() ) / ( MAP_STEPSIZE / 2.0f ) );

	switch( eType )
	{
	default:
	case ApmMap::GROUND		:
		// 정상처리
		return pSegment->stTileInfo.GetBlocking( xBlock % 2 + ( zBlock % 2 ) * 2 );
	case ApmMap::SKY		:
		return pSegment->stTileInfo.GetSkyBlocking();
	case ApmMap::UNDERGROUND:
		return 1;
	case ApmMap::GHOST		:
		return 0;
	}
}

ApCompactSegment	* ApWorldSector::C_GetSegment		( FLOAT x , FLOAT y , FLOAT z )
{
	if( IsLoadedCompactData() )
	{
		INT32	indexX, indexZ	;
		FLOAT	fStepSize		;

		// 마고자 (2003-12-30 오후 12:33:23) : 임시로 수정..
	//	fStepSize	= ( m_fxEnd - m_fxStart ) / ( ( float ) m_pCompactSectorInfo->m_nCompactSegmentDepth )	;
		fStepSize	= MAP_STEPSIZE;

		indexX		= ( INT32 ) ( ( x - m_fxStart ) / fStepSize )		;
		indexZ		= ( INT32 ) ( ( z - m_fzStart ) / fStepSize )		;

		// 인덱스에 프로텍션을 검..
		if( indexX < 0 ) indexX = 0;
		if( indexZ >= m_pCompactSectorInfo->m_nCompactSegmentDepth ) indexZ = m_pCompactSectorInfo->m_nCompactSegmentDepth - 1 ;

		if( indexX <	0												||										
			indexZ <	0												||
			indexX >=	m_pCompactSectorInfo->m_nCompactSegmentDepth	||
			indexZ >=	m_pCompactSectorInfo->m_nCompactSegmentDepth	)
		{
			return NULL;
		}
		else
			return m_pCompactSectorInfo->GetSegment( indexX , indexZ );
	}
	else
	{
		return NULL;
	}
}

ApCompactSegment	* ApWorldSector::C_GetSegment		( INT32 x , INT32 z )
{
//	ASSERT( 0 <= x && x < MAP_DEFAULT_DEPTH );
//	ASSERT( 0 <= z && z < MAP_DEFAULT_DEPTH );

	if( m_pCompactSectorInfo )	return m_pCompactSectorInfo->GetSegment( x , z );
	else						return NULL										;
}

// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....
// Detail ApDetailSegment Functions....

BOOL	ApWorldSector::IsLoadedDetailData		( INT32 nTargetDetail )
{
	ASSERT( m_bInitialized														);
	ASSERT( nTargetDetail >= SECTOR_EMPTY && nTargetDetail < SECTOR_DETAILDEPTH	);

	if( FALSE == IsLoadedDetailData() ) return FALSE;
	else
	{
		if( m_pDetailSectorInfo->m_pSegment[ nTargetDetail ] ) return TRUE;
		else return FALSE;
	}
}

BOOL	ApWorldSector::LoadDetailData			()	// 디테일 데이타 로딩.
{
	ASSERT( !"이거 쓰면 안돼는데.." );
	
//	ASSERT( m_bInitialized						);
//	ASSERT( NULL != ApWorldSector::m_pModuleMap	);
//
//	// 로드 돼어 있는지 검사..
//	if( IsLoadedDetailData() )
//	{
//		// 모든 데이타 초기화..
//		RemoveAllDetailData();
//	}
//
//	char	filename[ 256 ];
//	sprintf( filename , "%s\\%02d,%02d.amf" ,
//		ApWorldSector::m_pModuleMap->GetMapDirectory()		,
//		m_nIndexX											,
//		m_nIndexZ											);
//
//	FILE	*pFile = fopen( filename , "rb" );
//	if( !pFile )
//	{
//		//TRACE( "%s 파일을 읽을수 없습니다" , filename );
//		return FALSE;
//	}
//
//	// 데이타를 메모리에.. 저장.
//	int	version = MAPSTRUCTURE_FILE_HEADER;
//	int	x , y;
//
//	fread( ( void * ) & version			, sizeof version		, 1 , pFile );
//
//	// 파일 타입 체크.
//
//	if( ALEF_GetFileType( version ) != ALEF_MAPFILE )
//	{
//		ASSERT( !"맵버젼 오류!파일이 아닙니다!" );
//		return FALSE;
//	}
//
//	if( ALEF_GetFileVersion( version ) != MAPSTRUCTURE_FILE_VERSION )
//	{
//		TRACE( "맵파일 버젼이 다릅니다. 현재버젼이 %x 이나 파일에는 %x 입니다.!" , MAPSTRUCTURE_FILE_VERSION , ALEF_GetFileVersion( version ) );
//		switch( ALEF_GetFileVersion( version )  )
//		{
//		case 0x0001:
//		default:
//			return FALSE;
//		}
//	}
//
//	// 계속진행.
//	AllocDetailSectorInfo();
//
//	// 커맨트 카피..
//	fread( ( void * ) m_strComment , sizeof ( char	 )		,256, pFile );
//
//	int							j						;
//	int							subdivisioncount		;
//	ApSubDivisionSegmentInfo *	pSubDivisionSegmentInfo	;
//
//	for( int i = 0 ; i < SECTOR_DETAILDEPTH ; i++ )
//	{
//		fread( ( void * ) & m_pDetailSectorInfo->m_nDepth[ i ]		, sizeof ( int ) , 1	, pFile );
//
//		if( m_pDetailSectorInfo->m_nDepth[ i ] > 0 )
//		{
//			m_pDetailSectorInfo->m_pSegment[ i ] = 
//				new ApDetailSegment[ m_pDetailSectorInfo->m_nDepth[ i ] * m_pDetailSectorInfo->m_nDepth[ i ] ];
//
//			fread( ( void * ) m_pDetailSectorInfo->m_pSegment[ i ]	, sizeof ApDetailSegment	,
//					m_pDetailSectorInfo->m_nDepth[ i ] * m_pDetailSectorInfo->m_nDepth[ i ] , pFile );
//
//			// Blocking 요소 추가. 로딩은 같이 된다.
//		}
//		// Reserved Field
//		fread( ( void * ) & m_pDetailSectorInfo->m_nAlignment		, sizeof ( int ) , 1	, pFile );
//		fread( ( void * ) & m_pDetailSectorInfo->m_nReserved1		, sizeof ( int ) , 1	, pFile );
//		fread( ( void * ) & m_pDetailSectorInfo->m_nReserved2		, sizeof ( int ) , 1	, pFile );
//		fread( ( void * ) & m_pDetailSectorInfo->m_nReserved3		, sizeof ( int ) , 1	, pFile );
//		fread( ( void * ) & m_pDetailSectorInfo->m_nReserved4		, sizeof ( int ) , 1	, pFile );
//
//		// Read Sub Division...
//		fread( ( void * ) &subdivisioncount	, sizeof ( int )		, 1 , pFile );
//
//
//		for( j = 0 ; j < subdivisioncount ; j ++ )
//		{
//			pSubDivisionSegmentInfo = ( ApSubDivisionSegmentInfo * ) new ApSubDivisionSegmentInfo;
//
//			pSubDivisionSegmentInfo->m_nDetail	= i;
//
//			fread( ( void * ) &pSubDivisionSegmentInfo->pAlignment[ TD_FIRST	]	,	sizeof ( int )	, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->pAlignment[ TD_SECOND	]	,	sizeof ( int )	, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->pAlignment[ TD_THIRD	]	,	sizeof ( int )	, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->pAlignment[ TD_FOURTH	]	,	sizeof ( int )	, 1 , pFile );
//			
//			fread( ( void * ) &pSubDivisionSegmentInfo->x1				,	sizeof ( int )		, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->z1				,	sizeof ( int )		, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->x2				,	sizeof ( int )		, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->z2				,	sizeof ( int )		, 1 , pFile );
//
//			pSubDivisionSegmentInfo->xStart	= m_fxStart + ( ( m_fxEnd - m_fxStart ) / ( ( float ) m_pDetailSectorInfo->m_nDepth[ i ] ) ) * pSubDivisionSegmentInfo->x1			;
//			pSubDivisionSegmentInfo->zStart	= m_fzStart + ( ( m_fxEnd - m_fxStart ) / ( ( float ) m_pDetailSectorInfo->m_nDepth[ i ] ) ) * pSubDivisionSegmentInfo->z1			;
//			pSubDivisionSegmentInfo->xEnd	= m_fxStart + ( ( m_fxEnd - m_fxStart ) / ( ( float ) m_pDetailSectorInfo->m_nDepth[ i ] ) ) * ( pSubDivisionSegmentInfo->x2 + 1 )	;
//			pSubDivisionSegmentInfo->zEnd	= m_fzStart + ( ( m_fxEnd - m_fxStart ) / ( ( float ) m_pDetailSectorInfo->m_nDepth[ i ] ) ) * ( pSubDivisionSegmentInfo->z2 + 1 )	;
//
//			fread( ( void * ) &pSubDivisionSegmentInfo->nHorizontalDepth		,	sizeof ( int )		, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->nVerticalDepth			,	sizeof ( int )		, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->pIndex[ TD_FIRST	]	,	sizeof ( int )		, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->pIndex[ TD_SECOND	]	,	sizeof ( int )		, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->pIndex[ TD_THIRD	]	,	sizeof ( int )		, 1 , pFile );
//			fread( ( void * ) &pSubDivisionSegmentInfo->pIndex[ TD_FOURTH	]	,	sizeof ( int )		, 1 , pFile );
//
//			// Reserved Field
//			fread( ( void * ) &pSubDivisionSegmentInfo->Reserved				,	sizeof ( int )		, 1	, pFile );
//
//			pSubDivisionSegmentInfo->pSubdivisionSegment = new ApSubDivisionSegmentInfo::SubdivisionSegment[ pSubDivisionSegmentInfo->nHorizontalDepth * pSubDivisionSegmentInfo->nVerticalDepth ];
//			ASSERT( NULL!= pSubDivisionSegmentInfo->pSubdivisionSegment );
//
//			fread( ( void * ) pSubDivisionSegmentInfo->pSubdivisionSegment	,	sizeof ApSubDivisionSegmentInfo::SubdivisionSegment ,
//				pSubDivisionSegmentInfo->nHorizontalDepth * pSubDivisionSegmentInfo->nVerticalDepth , pFile );
//
//			pSubDivisionSegmentInfo->m_pSector	= this;
//
//			for( y = pSubDivisionSegmentInfo->z1 ; y <= pSubDivisionSegmentInfo->z2 ; y ++ )
//			{
//				for( x = pSubDivisionSegmentInfo->x1 ; x <= pSubDivisionSegmentInfo->x2 ; x ++ )
//				{
//					D_GetSegment( i , x , y )->ds.pDetailSegment	= pSubDivisionSegmentInfo;
//				}
//			}
//
//			m_pDetailSectorInfo->m_listSubSegment[ i ].AddTail( pSubDivisionSegmentInfo );
//		}		
//	}
//
//	fclose( pFile );

	return TRUE;

}

BOOL	ApWorldSector::SaveDetailData			()	// 디테일 데이타 저장.
{
	SaveDetailDataGeometry	();
	SaveDetailDataTile		();

	return TRUE;
}

BOOL	ApWorldSector::CreateDetailData			( INT32 nTargetDetail , INT32 nDepth , FLOAT fDefaultHeight )	// 해당 디테일 정보 생성..
{
	ASSERT( SECTOR_EMPTY <= nTargetDetail && nTargetDetail < SECTOR_DETAILDEPTH );
	ASSERT( nDepth >= 0 );

	// 메모리 얼록과정.
	RemoveDetailData( nTargetDetail );

	AllocDetailSectorInfo();

	m_pDetailSectorInfo->m_pSegment	[ nTargetDetail ] = new ApDetailSegment[ nDepth * nDepth ];

	if( m_pDetailSectorInfo->m_pSegment	[ nTargetDetail ] == NULL )
	{
		TRACE( "메모리부족" );
//		TRACEFILE(ALEF_ERROR_FILENAME, "메모리부족" );
		return FALSE;
	}

	m_pDetailSectorInfo->m_nDepth		[ nTargetDetail ] = nDepth;


	int tileindex;
	// 디폴트 타일 텍스쳐 설정..
	switch( nTargetDetail )
	{
	case SECTOR_EMPTY		:	tileindex = ALEF_SECTOR_DEFAULT_TILE_INDEX		;	break;
	case SECTOR_LOWDETAIL	:	tileindex = ALEF_SECTOR_DEFAULT_TILE_INDEX		;	break;
	case SECTOR_HIGHDETAIL	:	tileindex = ALEF_SECTOR_DEFAULT_TILE_INDEX		;	break;
	default					:	tileindex = ALEF_SECTOR_DEFAULT_TILE_INDEX		;	break;
	}
	
	// 할당한 세그먼트 초기화.
	ApDetailSegment	* pSegment;

	for( int x = 0 ; x < nDepth ; ++x )
	{
		for( int y = 0 ; y < nDepth ; ++y )
		{
			pSegment = D_GetSegment( nTargetDetail , x , y );

			ASSERT( NULL != pSegment );

			pSegment->SetDefault( fDefaultHeight );
		}
	}
	
	return TRUE;
}

BOOL	ApWorldSector::GenerateRoughMap			( INT32 nTargetDetail , INT32 nSourceDetail , FLOAT offset ,
			 									BOOL bHeight , BOOL bTile , BOOL bVertexColor )
{
	return TRUE;
	/*
	ASSERT( 0 <= nSourceDetail && nSourceDetail < SECTOR_DETAILDEPTH 	);
	ASSERT( 0 <= nTargetDetail && nTargetDetail < SECTOR_DETAILDEPTH 	);
	ASSERT( IsLoadedDetailData( nSourceDetail )							);
	ASSERT( IsLoadedDetailData( nTargetDetail )							);

	// 마고자 (2005-04-21 오전 10:54:38) : 
	// 사용하지 않는다.

	return TRUE;
	*/
}

BOOL	ApWorldSector::RemoveAllDetailData			()	// 디테일 데이타 메모리에서 제거.
{
	// 모든 디테일 삭제함.
	for( int i = 0 ; i < SECTOR_DETAILDEPTH ; ++i )
	{
		RemoveDetailData( i );
	}
	return TRUE;
}

BOOL	ApWorldSector::RemoveDetailData			( INT32 nTargetDetail )	// 디테일 데이타 메모리에서 제거. 해당 디테일만.
{
	// 구현해야함..
	ASSERT( nTargetDetail >= SECTOR_EMPTY && nTargetDetail < SECTOR_DETAILDEPTH	);

	// 해당 디테일 없앰..

	if( NULL == GetDetailSectorInfo() ) return FALSE;

	ApDetailSectorInfo * pInfo = GetDetailSectorInfo();

	delete [] pInfo->m_pSegment[ nTargetDetail ];
	pInfo->m_pSegment	[ nTargetDetail ]	= NULL	;
	pInfo->m_nDepth		[ nTargetDetail ]	= 0		;

	return TRUE;
}
BOOL	ApWorldSector::RemoveDetailDataFile		()	// 데이타 파일까지 제거함..
{
	// 역시..

	return FALSE;
}

INT32	ApWorldSector::D_GetDepth				( INT32 nTargetDetail )
{
	//ASSERT( NULL != m_pDetailSectorInfo );
	if( m_pDetailSectorInfo ) return m_pDetailSectorInfo->m_nDepth[ nTargetDetail ];
	else return 0;
}

// 섹터 편지 펑션들..
ApDetailSegment * ApWorldSector::D_GetSegment	( INT32 nTargetDetail , INT32 x , INT32 z ,	FLOAT *pfPosX	, FLOAT *pfPosZ	)
{
	// 마고자 (2005-04-21 오전 11:01:34) : 디테일만 사용
	nTargetDetail = SECTOR_HIGHDETAIL;

	if( pfPosX ) *pfPosX = m_fxStart + D_GetStepSize( nTargetDetail ) * x ;
	if( pfPosZ ) *pfPosZ = m_fzStart + D_GetStepSize( nTargetDetail ) * z ;

	// 범위체크..
	if( x < 0 || z < 0 || IsLoadedDetailData( nTargetDetail ) == FALSE )
	{
		// 이런경우는 거의 없으니까 큰 문제는 없다..
		return NULL;
	}

	if( x >= D_GetDepth( nTargetDetail ) && z >= D_GetDepth( nTargetDetail ) )
	{
		// ;;;
		if( GetNearSector( TD_SOUTH ) && GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) )
		{
			// 남동쪽 섹터가 있으면..
			// 남동쪽 왼쪽 위 섹터를 리턴한다.
			return GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST )->D_GetSegment( nTargetDetail , 0 , 0 );
		}
		return NULL;
	}
	else if( z >= D_GetDepth( nTargetDetail ) )
	{
		if( GetNearSector( TD_SOUTH ) )
			return GetNearSector( TD_SOUTH )->D_GetSegment( nTargetDetail , x , 0 );
		else
			return NULL;
	}
	else if( x >= D_GetDepth( nTargetDetail ) )
	{
		if( GetNearSector( TD_EAST ) )
			return GetNearSector( TD_EAST )->D_GetSegment( nTargetDetail , 0 , z );
		else
			return NULL;
	}

	return &m_pDetailSectorInfo->m_pSegment[ nTargetDetail ][ ( x ) + D_GetDepth( nTargetDetail ) * ( z ) ];
}

ApDetailSegment	* ApWorldSector::D_GetSegment	( INT32 nTargetDetail , FLOAT x , FLOAT z ,	INT32 *pPosX	, INT32 *pPosZ	)
{
	// 마고자 (2005-04-21 오전 11:01:34) : 디테일만 사용
	nTargetDetail = SECTOR_HIGHDETAIL;

	INT32	nX = ( INT32 ) x;
	INT32	nZ = ( INT32 ) z;
	INT32	nXStart = ( INT32 ) m_fxStart;
	INT32	nZStart = ( INT32 ) m_fzStart;
	INT32	nXEnd = ( INT32 ) m_fxEnd;
	INT32	nZEnd = ( INT32 ) m_fzEnd;
	INT32	nStepSize = ( INT32 ) D_GetStepSize( nTargetDetail );

	INT32	indexX, indexZ;

	/*
	indexX = ( INT32 ) ( ( x - m_fxStart ) / D_GetStepSize( nTargetDetail ) );
	indexZ = ( INT32 ) ( ( z - m_fzStart ) / D_GetStepSize( nTargetDetail ) );
	*/
	indexX = ( ( nX - nXStart ) / nStepSize );
	indexZ = ( ( nZ - nZStart ) / nStepSize );

	if( pPosX ) *pPosX = indexX;
	if( pPosZ ) *pPosZ = indexZ;
	
	if( nTargetDetail == SECTOR_EMPTY || NULL == m_pDetailSectorInfo ) return NULL;

	// 범위 체크..
	if( nX < nXStart || nX >= nXEnd ||
		nZ < m_fzStart || nZ >= nZEnd )
	{
		return NULL ;
	}

	if( m_pDetailSectorInfo->m_nDepth[ nTargetDetail ] <= indexX || m_pDetailSectorInfo->m_nDepth[ nTargetDetail ] <= indexZ ||
		indexX < 0 || indexZ < 0 ) return NULL;

	return &m_pDetailSectorInfo->m_pSegment[ nTargetDetail ][ ( indexX ) + m_pDetailSectorInfo->m_nDepth[ nTargetDetail ] * ( indexZ ) ];	
}

FLOAT	ApWorldSector::D_GetHeight				( INT32 nTargetDetail , FLOAT x , FLOAT z )	// 해당 좌표의 이론적 높이를 계산해낸다.
{
	// 마고자 (2005-04-21 오전 11:01:34) : 디테일만 사용
	nTargetDetail = SECTOR_HIGHDETAIL;

	if( nTargetDetail == SECTOR_EMPTY ) return 0.0f;

	int segx, segz;
	ApDetailSegment * pSegment = D_GetSegment( nTargetDetail , x , z , & segx , & segz );

	if( pSegment )
	{
		BOOL	bEdgeTurn	= FALSE;
		float	fsegstartx , fsegstartz;
		float	fstepsize;
		float	height;

		bEdgeTurn = pSegment->stTileInfo.GetEdgeTurn();

		D_GetSegment( nTargetDetail , segx , segz , & fsegstartx , &fsegstartz );
		fstepsize = D_GetStepSize( nTargetDetail );

		if( x == fsegstartx && z == fsegstartz )
		{
			return pSegment->height;
		}
		else if( x == fsegstartx )
		{
			// y 축만고려
			float startheight	= pSegment->height;
			float endheight		= D_GetHeight( nTargetDetail , fsegstartx + fstepsize * ( 0 )	, fsegstartz + fstepsize * ( 1 ) );

			height = startheight + ( z - fsegstartz ) / fstepsize * ( endheight - startheight );
			return height;
		}
		else if ( z == fsegstartz )
		{
			// x 축만 고려
			float startheight	= pSegment->height;
			float endheight		= D_GetHeight( nTargetDetail ,  fsegstartx + fstepsize * ( 1 )	, fsegstartz + fstepsize * ( 0 ) );

			height = startheight + ( x - fsegstartx ) / fstepsize * ( endheight - startheight );
			return height;
		}
		else
		{
			// -_-;
			float h11 , h21 , h12 , h22;
			h11 = pSegment->height;
			h21 = D_GetHeight( nTargetDetail ,  fsegstartx + fstepsize * ( 1 )	, fsegstartz + fstepsize * ( 0 ) );
			h12 = D_GetHeight( nTargetDetail ,  fsegstartx + fstepsize * ( 0 )	, fsegstartz + fstepsize * ( 1 ) );
			h22 = D_GetHeight( nTargetDetail ,  fsegstartx + fstepsize * ( 1 )	, fsegstartz + fstepsize * ( 1 ) );

			// 기울기..
			x -= fsegstartx;
			z -= fsegstartz;

			float kiulki = z / x;

			if( bEdgeTurn )
			{
				float kiulki = z / x;

				if( kiulki < 1.0f )
				{
					// 오른쪽 위것.

					float daechoongy = kiulki * fstepsize;
					float daechoongheight;

					daechoongheight = h21 + ( daechoongy ) * ( h22 - h21 ) / fstepsize;

					height = h11 + ( x ) * ( daechoongheight - h11 ) / fstepsize;

					return height;
				}
				else
				{
					// 왼쪽 아래 삼각형.
					float daechoongx = fstepsize / kiulki;
					float daechoongheight;

					daechoongheight = h12 + ( daechoongx ) * ( h22 - h12 ) / fstepsize;

					height = h11 + ( z ) * ( daechoongheight - h11 ) / fstepsize;

					return height;
				}
			}
			else
			{
				//TRACEFILE( "GetHeight.txt" , "x=%f , z=%f ,%f,%f,%f,%f" , x , z , h11 , h12 , h21 , h22 );

				float kiulki = - z / ( fstepsize - x );

				if( kiulki > -1.0f )
				{
					// 왼쪽 아래것..

					float fZp = - kiulki * fstepsize;
					float fHy ;

					fHy = h11 + ( fZp ) * ( h12 - h11 ) / fstepsize;

					height = fHy + x * ( h21 - fHy  ) / fstepsize;

					return height;
				}
				else
				{
					float fXp = ( kiulki + 1.0f ) / kiulki * fstepsize;
					float fHx;

					fHx = h12 + ( fXp ) * ( h22 - h12 ) / fstepsize;

					height = h21 + z * ( fHx - h21 ) / fstepsize;

					return height;
				}

			}
			////
		}
	}

	if( x >= m_fxEnd && z >= m_fzEnd )
	{
		ApWorldSector * pNearSector = NULL ;
		if( GetNearSector( TD_SOUTH ) && GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) )
			pNearSector = GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST );
		else if ( GetNearSector( TD_EAST ) && GetNearSector( TD_EAST )->GetNearSector( TD_SOUTH ) )
			pNearSector = GetNearSector( TD_EAST )->GetNearSector( TD_SOUTH );

		if( pNearSector )
		{
			return pNearSector->D_GetHeight( nTargetDetail , x , z );
		}
	}
	else if ( x >= m_fxEnd )
	{
		if( GetNearSector( TD_EAST ) )
		{
			return GetNearSector( TD_EAST )->D_GetHeight( nTargetDetail , x , z );
		}
	}
	else if ( z >= m_fzEnd )
	{
		if( GetNearSector( TD_SOUTH ) )
		{
			return GetNearSector( TD_SOUTH )->D_GetHeight( nTargetDetail , x , z );
		}
	}

	if( x < m_fxStart && z < m_fzStart )
	{
		ApWorldSector * pNearSector = NULL ;
		if( GetNearSector( TD_NORTH ) && GetNearSector( TD_NORTH )->GetNearSector( TD_WEST ) )
			pNearSector = GetNearSector( TD_NORTH )->GetNearSector( TD_WEST );
		else if ( GetNearSector( TD_WEST ) && GetNearSector( TD_WEST )->GetNearSector( TD_NORTH ) )
			pNearSector = GetNearSector( TD_WEST )->GetNearSector( TD_NORTH );

		if( pNearSector )
		{
			return pNearSector->D_GetHeight( nTargetDetail , x , z );
		}
	}
	else if ( x < m_fxStart )
	{
		if( GetNearSector( TD_WEST ) )
		{
			return GetNearSector( TD_WEST )->D_GetHeight( nTargetDetail , x , z );
		}
	}
	else if ( z < m_fzStart )
	{
		if( GetNearSector( TD_NORTH ) )
		{
			return GetNearSector( TD_NORTH )->D_GetHeight( nTargetDetail , x , z );
		}
	}

	return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
}

ApRGBA	ApWorldSector::D_GetValue				( INT32 nTargetDetail , FLOAT x , FLOAT z )
{
	// 일단 테스트코드
	ApRGBA rgb;
	rgb.alpha	= 255;
	rgb.red		= 255;
	rgb.green	= 255;
	rgb.blue	= 255;

	int segx, segz;
	ApDetailSegment * pSegment = D_GetSegment( nTargetDetail , x , z , & segx , & segz );

	if( pSegment )
	{
		return pSegment->vertexcolor;
	}

	if( x >= m_fxEnd && z >= m_fzEnd )
	{
		ApWorldSector * pNearSector = NULL ;
		if( GetNearSector( TD_SOUTH ) && GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) )
			pNearSector = GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST );
		else if ( GetNearSector( TD_EAST ) && GetNearSector( TD_EAST )->GetNearSector( TD_SOUTH ) )
			pNearSector = GetNearSector( TD_EAST )->GetNearSector( TD_SOUTH );

		if( pNearSector )
		{
			return pNearSector->D_GetValue( nTargetDetail , x , z );
		}
	}
	else if ( x >= m_fxEnd )
	{
		if( GetNearSector( TD_EAST ) )
		{
			return GetNearSector( TD_EAST )->D_GetValue( nTargetDetail , x , z );
		}
	}
	else if ( z >= m_fzEnd )
	{
		if( GetNearSector( TD_SOUTH ) )
		{
			return GetNearSector( TD_SOUTH )->D_GetValue( nTargetDetail , x , z );
		}
	}

	// 결국 못찾으면..
	// 최대 밝기로..
	return rgb;
}
FLOAT	ApWorldSector::D_GetRoughHeight			( INT32 nTargetDetail , INT32 x , INT32 z )	// 해당 좌표의 이론적 높이를 계산해낸다.
{
	float fsegx, fsegz;
	ApDetailSegment * pSegment = D_GetSegment( nTargetDetail , x , z , & fsegx , & fsegz );

	if( pSegment )
	{
		return pSegment->height;
	}

	return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;	
}

FLOAT	ApWorldSector::D_SetHeight				( INT32 nTargetDetail , INT32 x , INT32 z , FLOAT height	)
{
	// 그냥 변경을 하기도 하고..
	// 경계면의 경우는.. 해당 부분에 대한 처리를 해준다.
	// Left , Top 은.. 섹터 경계에 부분을 기준으로 직선을 이루도록한다.
	// right bottom 은 아래쪽, 오른쪽 섹터의 퍼스트 라인을 기준으로 한다.

	float	fx , fz;
	ApDetailSegment * pSegment = D_GetSegment( nTargetDetail , x , z , &fx , &fz );
	
	if( pSegment )
	{
		pSegment->height = height;

		return height;
	}
	else return 1.0f;
}

ApRGBA	ApWorldSector::D_SetValue				( INT32 nTargetDetail , INT32 x , INT32 z , ApRGBA value	)
{
	float	fx , fz;
	ApDetailSegment * pSegment = D_GetSegment( nTargetDetail , x , z , &fx , &fz );
	
	if( pSegment )
	{
		pSegment->vertexcolor	= value;
		return value;
	}

	return value;
}

INT32	ApWorldSector::D_SetAllTile				( INT32 nTargetDetail , INT32 tileindex					)
{
	if( !IsLoadedDetailData( nTargetDetail ) ) return ALEF_TEXTURE_NO_TEXTURE;

	int			i , j				;
	ApDetailSegment *	pSegment	;
	float		fSegPosX , fSegPosZ	;
	UINT		currenttick	= GetTickCount();

	for( j = 0 ; j < m_pDetailSectorInfo->m_nDepth[ nTargetDetail ] ; ++j )
	{
		for( i = 0 ; i < m_pDetailSectorInfo->m_nDepth[ nTargetDetail ] ; ++i )
		{
			pSegment = D_GetSegment( nTargetDetail , i , j , &fSegPosX , &fSegPosZ );

			if( pSegment )
			{
				pSegment->pIndex[ TD_FIRST	]	= tileindex					;
				pSegment->pIndex[ TD_SECOND	]	= ALEF_TEXTURE_NO_TEXTURE	;
				pSegment->pIndex[ TD_THIRD	]	= ALEF_TEXTURE_NO_TEXTURE	;
				pSegment->pIndex[ TD_FOURTH	]	= ALEF_TEXTURE_NO_TEXTURE	;

			}
		}
	}
	return 0;
}

BOOL	ApWorldSector::RemoveAllData	()
{
	m_pModuleMap->DeleteChars	( 0 , this );
	m_pModuleMap->DeleteItems	( 0 , this );
	m_pModuleMap->DeleteObjects	( 0 , this );

	RemoveCompactData		();
	RemoveAllDetailData		();

	FreeCompactSectorInfo	();
	FreeDetailSectorInfo	();

	FreeHeightPool			();
	DeleteAllIndexArray		();
	return TRUE;
}


BOOL	ApWorldSector::AllocDetailSectorInfo	()
{
	if( m_pDetailSectorInfo )
		return FALSE;

	m_pDetailSectorInfo = new ApDetailSectorInfo;
	ASSERT( NULL != m_pDetailSectorInfo );

	// 데이타 초기하ㅗ,.

	for( int i = 0 ; i < SECTOR_DETAILDEPTH ; ++i )
	{
		m_pDetailSectorInfo->m_pSegment		[ i ] = NULL	;
		m_pDetailSectorInfo->m_nDepth		[ i ] = 0		;
	}
	
	m_pDetailSectorInfo->m_nAlignment = 0;	// 텍스쳐 사용 타입.
	m_pDetailSectorInfo->m_nReserved1 = 0;
	m_pDetailSectorInfo->m_nReserved2 = 0;
	m_pDetailSectorInfo->m_nReserved3 = 0;
	m_pDetailSectorInfo->m_nReserved4 = 0;

	return TRUE;
}

BOOL	ApWorldSector::FreeDetailSectorInfo		()
{
	if( m_pDetailSectorInfo )
	{
		delete m_pDetailSectorInfo	;
		m_pDetailSectorInfo = NULL;
	}

	return TRUE;
}

BOOL	ApWorldSector::IsInSectorRadius		( float x , float z , float radius )
{
	FLOAT	aBoxSector	[ 4 ];	// 0 X1 , 1 X2 , 2 Z1 , 3 Z2;
	FLOAT	aBoxRange	[ 4 ];	// 0 X1 , 1 X2 , 2 Z1 , 3 Z2;

	// 마고자 (2004-04-09 오후 2:46:30) : 
	//충돌 사각형 검출
	//사각형 두개가 있을 때, 
	//
	// 1번째 사각형: AX1, AX2, AY1, AY2
	// 2번째 사각형: BX1, BX2, BY1, BY2
	//
	// 이런 경우에 서로 겹치는지 체크한다 
	//
	// if (AX2 > BX1 && BX2 > AX1)
	//  if (AY2 > BY1 && BY2 > AY1)
	//   return true;
	// else return false;

	aBoxRange[ 0 ]	= x - radius;//AX1
	aBoxRange[ 1 ]	= x + radius;//AX2
	aBoxRange[ 2 ]	= z - radius;//AZ1
	aBoxRange[ 3 ]	= z + radius;//AZ2

	aBoxSector[ 0 ]	= GetXStart	()	; // BX1
	aBoxSector[ 1 ]	= GetXEnd	()	; // BX2
	aBoxSector[ 2 ]	= GetZStart	()	; // BZ1
	aBoxSector[ 3 ]	= GetZEnd	()	; // BZ2

	if( aBoxSector[ 0 ] < aBoxRange[ 0 ]  &&
		aBoxSector[ 1 ] > aBoxRange[ 1 ]  &&
		aBoxSector[ 2 ] < aBoxRange[ 2 ]  &&
		aBoxSector[ 3 ] > aBoxRange[ 3 ]  )
	{
		return 2 ; // 완전 내부.
	}


	// 충돌검사.
	if (aBoxSector[ 1 ] > aBoxRange[ 0 ] && aBoxRange[ 1 ] > aBoxSector[ 0 ])
	{
		if (aBoxSector[ 3 ] > aBoxRange[ 2 ] && aBoxRange[ 3 ] > aBoxSector[ 2 ])
		{
			// 충돌!
			return TRUE;
		}
	}

	return FALSE;
}

BOOL	ApWorldSector::IsInSectorRadiusBBox( float x1 , float z1 , float x2 , float z2 )
{
	FLOAT	aBoxSector	[ 4 ];	// 0 X1 , 1 X2 , 2 Z1 , 3 Z2;
	FLOAT	aBoxRange	[ 4 ];	// 0 X1 , 1 X2 , 2 Z1 , 3 Z2;

	// 마고자 (2004-04-09 오후 2:46:30) : 
	//충돌 사각형 검출
	//사각형 두개가 있을 때, 
	//
	// 1번째 사각형: AX1, AX2, AY1, AY2
	// 2번째 사각형: BX1, BX2, BY1, BY2
	//
	// 이런 경우에 서로 겹치는지 체크한다 
	//
	// if (AX2 > BX1 && BX2 > AX1)
	//  if (AY2 > BY1 && BY2 > AY1)
	//   return true;
	// else return false;

	aBoxRange[ 0 ]	= x1;//AX1
	aBoxRange[ 1 ]	= x2;//AX2
	aBoxRange[ 2 ]	= z1;//AZ1
	aBoxRange[ 3 ]	= z2;//AZ2

	aBoxSector[ 0 ]	= GetXStart	()	; // BX1
	aBoxSector[ 1 ]	= GetXEnd	()	; // BX2
	aBoxSector[ 2 ]	= GetZStart	()	; // BZ1
	aBoxSector[ 3 ]	= GetZEnd	()	; // BZ2

	if( aBoxSector[ 0 ] < aBoxRange[ 0 ]  &&
		aBoxSector[ 1 ] > aBoxRange[ 1 ]  &&
		aBoxSector[ 2 ] < aBoxRange[ 2 ]  &&
		aBoxSector[ 3 ] > aBoxRange[ 3 ]  )
	{
		return 2 ; // 완전 내부.
	}


	// 충돌검사.
	if (aBoxSector[ 1 ] > aBoxRange[ 0 ] && aBoxRange[ 1 ] > aBoxSector[ 0 ])
	{
		if (aBoxSector[ 3 ] > aBoxRange[ 2 ] && aBoxRange[ 3 ] > aBoxSector[ 2 ])
		{
			// 충돌!
			return TRUE;
		}
	}

	return FALSE;
}


UINT32 *ApWorldSector::D_SetTile( INT32 nTargetDetail , INT32 x , INT32 z , UINT32 firsttexture ,
								UINT32 secondtexture		,
								UINT32 thirdtexture			,
								UINT32 fourthtexture		,
								UINT32 fifthtexture			,
								UINT32 sixthtexture			)
{
	if( !IsLoadedDetailData( nTargetDetail ) ) return NULL;

	ApDetailSegment	* pDetailSegment = D_GetSegment( nTargetDetail , x , z );

	if( NULL == pDetailSegment ) return NULL;
	else
	{
		// 노멀..
		pDetailSegment->pIndex[ TD_FIRST	] = firsttexture	;
		pDetailSegment->pIndex[ TD_SECOND	] = secondtexture	;
		pDetailSegment->pIndex[ TD_THIRD	] = thirdtexture	;
		pDetailSegment->pIndex[ TD_FOURTH	] = fourthtexture	;
		pDetailSegment->pIndex[ TD_FIFTH	] = fifthtexture	;
		pDetailSegment->pIndex[ TD_SIXTH	] = sixthtexture	;
		pDetailSegment->pIndex[ TD_SEVENTH	] = ALEF_TEXTURE_NO_TEXTURE	;
		pDetailSegment->pIndex[ TD_EIGHTTH	] = ALEF_TEXTURE_NO_TEXTURE	;

		return pDetailSegment->pIndex;
	}
}

UINT32 *ApWorldSector::D_GetTile( INT32 nTargetDetail , FLOAT x , FLOAT z )
{
	if( !IsLoadedDetailData( nTargetDetail ) ) return NULL;

	ApDetailSegment	* pDetailSegment = D_GetSegment( nTargetDetail , x , z );

	if( NULL == pDetailSegment ) return NULL;
	else
	{
		return pDetailSegment->pIndex;
	}
}

BOOL ApWorldSector::LoadDetailDataGeometry	()	// 높이정보
{
	CMagUnpackManager * pUnPackManager = 
		m_pModuleMap->GetUnpackManagerDivisionGeometry( GetDivisionIndex( GetArrayIndexX() , GetArrayIndexZ() ) );

	if( pUnPackManager && pUnPackManager->GetFileCount() )
	{
		int nTotalCount = (GetArrayIndexX() % 16) + (GetArrayIndexZ() % 16 ) * 16;

		char compactDir[ _MAX_PATH ];
		wsprintf( compactDir , "%s" , ApWorldSector::m_pModuleMap->GetMapGeometryDirectory() );
		VERIFY( pUnPackManager->GetFile( nTotalCount , compactDir ) );

		BOOL bRet = _LoadDetailDataGeometry( pUnPackManager->GetFileName( nTotalCount ) );

		char filename[ _MAX_PATH ];

		sprintf( filename, "%s\\%s",
			ApWorldSector::m_pModuleMap->GetMapGeometryDirectory(),
			pUnPackManager->GetFileName( nTotalCount ) );

		DeleteFile( filename );

		return bRet;
	}
	else
	{
		return _LoadDetailDataGeometry();
	}
}

BOOL	ApWorldSector::LoadDetailDataTile		()	// 타일+버텍스 칼라
{
	CMagUnpackManager * pUnPackManager = NULL;
	pUnPackManager = m_pModuleMap->GetUnpackManagerDivisionMoonee( GetDivisionIndex( GetArrayIndexX() , GetArrayIndexZ() ) );

	if( pUnPackManager && pUnPackManager->GetFileCount() )
	{
		int nTotalCount =
					GetArrayIndexX() % 16 +
				(	GetArrayIndexZ() % 16 ) * 16;

		char	compactDir[ _MAX_PATH ];
		wsprintf( compactDir , "%s" , ApWorldSector::m_pModuleMap->GetMapMooneeDirectory() );
		if( !pUnPackManager->GetFile( nTotalCount , compactDir ) )
		{
			// 파일없음..
			return _LoadDetailDataTile();
		}

		BOOL bRet = _LoadDetailDataTile( pUnPackManager->GetFileName( nTotalCount ) );

		char	filename[ _MAX_PATH ];

		sprintf( filename , "%s\\%s"					,
			ApWorldSector::m_pModuleMap->GetMapMooneeDirectory()	,
			pUnPackManager->GetFileName( nTotalCount )				);

		DeleteFile( filename );

		return bRet;
	}
	else
	{
		return _LoadDetailDataTile();
	}
}

BOOL	ApWorldSector::_LoadDetailDataGeometry	( char * pfilename )	// 높이정보
{
	ASSERT( m_bInitialized						);
	ASSERT( NULL != ApWorldSector::m_pModuleMap	);

	// 로드 돼어 있는지 검사..
	if( IsLoadedDetailData() )
	{
		// 모든 데이타 초기화..
		RemoveAllDetailData();
	}

	char	filename[ _MAX_PATH ];

	if( pfilename )
	{
		sprintf( filename , "%s\\%s"					,
			ApWorldSector::m_pModuleMap->GetMapGeometryDirectory()	,
			pfilename												);
	}
	else
	{
		sprintf( filename , "%s\\g%d,%d.amf"					,
			ApWorldSector::m_pModuleMap->GetMapGeometryDirectory()	,
			GetArrayIndexX()										,
			GetArrayIndexZ()										);
	}

	FILE	*pFile = fopen( filename , "rb" );
	if( !pFile )
	{
		//TRACE( "%s 파일을 읽을수 없습니다" , filename );
		return FALSE;
	}

	// 데이타를 메모리에.. 저장.
	int	version = ALEF_GEOMETRYFILE_HEADER;

	BOOL	bReadFlag	= TRUE;

	fread( ( void * ) & version			, sizeof version		, 1 , pFile );

	// 파일 타입 체크.

	if( ALEF_GetFileType( version ) != ALEF_GEOMETRYFILE )
	{
		fclose( pFile );
		ASSERT( !"맵버젼 오류!파일이 아닙니다!" );
		return FALSE;
	}

	BOOL bDisableLoadCallback = FALSE;

	if( ALEF_GetFileVersion( version ) != GEOMETRY_FILE_VERSION )
	{
		TRACE( "맵파일 버젼이 다릅니다. 현재버젼이 %x 이나 파일에는 %x 입니다.!" , MAPSTRUCTURE_FILE_VERSION , ALEF_GetFileVersion( version ) );
//		TRACEFILE(ALEF_ERROR_FILENAME, "맵파일 버젼이 다릅니다. 현재버젼이 %x 이나 파일에는 %x 입니다.!" , MAPSTRUCTURE_FILE_VERSION , ALEF_GetFileVersion( version ) );
		switch( ALEF_GetFileVersion( version )  )
		{
		case 0x0001:
		default:
			fclose( pFile );
			return FALSE;

		case 0x0002:	// Normal 값 추가 수정..
			// 플래그만 읽지 않게 한다..
			bReadFlag	= FALSE;
			break;

		case 0x0003:
			// 마고자 (2005-03-23 오후 3:43:24) : 
			// 콜백을 무시하게 한다.
			bDisableLoadCallback = TRUE;
			break;
		}
	}

	// 계속진행.
	AllocDetailSectorInfo();

	// 커맨트 카피..
	char	strComment[ 256 ];
	fread( ( void * ) strComment , sizeof ( char	 )		,256, pFile );

	// 마고자 (2005-03-10 오전 10:21:42) : 쓰지 않으므로 삭제.
	// 이전 포멧과 호완 코드.

	int							j						;
	int							dummy					;

	for( int i = 0 ; i < SECTOR_DETAILDEPTH ; ++i )
	{
		fread( ( void * ) & m_pDetailSectorInfo->m_nDepth[ i ]		, sizeof ( int ) , 1	, pFile );

		if( m_pDetailSectorInfo->m_nDepth[ i ] > 0 )
		{
			// Memory Alloc...
			m_pDetailSectorInfo->m_pSegment[ i ] = 
				new ApDetailSegment[ m_pDetailSectorInfo->m_nDepth[ i ] * m_pDetailSectorInfo->m_nDepth[ i ] ];

			int		limit1 = m_pDetailSectorInfo->m_nDepth[ i ] * m_pDetailSectorInfo->m_nDepth[ i ];

			for( j = 0 ; j <  limit1 ; ++j )
			{
				// Segment 초기화!!!!!!!!!!!!!!!!!!!
				m_pDetailSectorInfo->m_pSegment[ i ][ j ].SetDefault();

				// 높이 정보만 카피함..
				fread( ( void * ) & m_pDetailSectorInfo->m_pSegment[ i ][ j ].height	, sizeof FLOAT , 1 , pFile );
				if( bReadFlag )
					fread( ( void * ) & m_pDetailSectorInfo->m_pSegment[ i ][ j ].nFlags	, sizeof FLOAT , 1 , pFile );
				else
					m_pDetailSectorInfo->m_pSegment[ i ][ j ].nFlags	= 0;
			}
			// Blocking 요소 추가. 로딩은 같이 된다.
		}

		// 이전 버젼 호완용..
		fread( ( void * ) &dummy	, sizeof ( int )		, 1 , pFile );
	}

	// 콜벡 호출..
	if( bDisableLoadCallback )
		m_pModuleMap->EnumCallback( APMMAP_CB_ID_LOADGEOMETRY , ( void * ) NULL , ( void * ) this );
	else
		m_pModuleMap->EnumCallback( APMMAP_CB_ID_LOADGEOMETRY , ( void * ) pFile , ( void * ) this );

	fclose( pFile );

	return TRUE;
}

BOOL	ApWorldSector::_LoadDetailDataTile		( char * pfilename )	// 타일+버텍스 칼라
{
	ASSERT( m_bInitialized						);
	ASSERT( NULL != ApWorldSector::m_pModuleMap	);

	// 로드 돼어 있는지 검사..
	if( !IsLoadedDetailData() )
	{
		// 로딩이 돼어있찌 않으면 타일은 읽히지 않는다.
		ASSERT( !"데이타 로딩이 돼지 않았습니다" );
		return FALSE;
	}

	char	filename[ 256 ];

	if( pfilename )
	{
		sprintf( filename , "%s\\%s"					,
			ApWorldSector::m_pModuleMap->GetMapMooneeDirectory()	,
			pfilename												);
	}
	else
	{
		sprintf( filename , "%s\\t%d,%d.amf" ,
			ApWorldSector::m_pModuleMap->GetMapMooneeDirectory(),
			GetArrayIndexX()										,
			GetArrayIndexZ()										);
	}

	FILE	*pFile = fopen( filename , "rb" );
	if( !pFile )
	{
		//TRACE( "%s 파일을 읽을수 없습니다" , filename );
		return FALSE;
	}

	// 데이타를 메모리에.. 저장.
	int	version = ALEF_MOONIEFILE_HEADER;
	int	x , z;

	fread( ( void * ) & version			, sizeof version		, 1 , pFile );

	// 파일 타입 체크.

	if( ALEF_GetFileType( version ) != ALEF_MOONIEFILE )
	{
		fclose(pFile);
		ASSERT( !"맵버젼 오류!파일이 아닙니다!" );
		return FALSE;
	}

	int	nReadTD_DEPTH	= TD_DEPTH;
	char	str[ 256 ];

	switch( ALEF_GetFileVersion( version )  )
	{
	case 0x0001:
		{
			// do nothing
			TRACE( "너무옜날 맵파일이라 사용 불가능..\n" );
		}
		return FALSE;
	case 0x0002:
		{
			TRACE( "이번 무늬 맵파일입니다. 컨버팅 들어갑니다.\n" );
			// 텍스쳐 갯수만 바뀐것이므로..
			// 큰 문제없이 숫자 변경으로 해결 된다.
			nReadTD_DEPTH = 4;

			// 텍스트 읽음
			fread( ( void * ) str		, sizeof ( char	 )		,256, pFile );
		}
		break;
	case 0x0003:
		{
			// 텍스트 읽음
			fread( ( void * ) str		, sizeof ( char	 )		,256, pFile );

			// 마고자 (2005-03-15 오후 3:38:06) : 지형 효과 펙터 초기화함..
			this->m_uFlag	= OP_NONE;

			// 기타 지형효과 데이타 초기화는 여기서..
		}
		break;

	case 0x0004:
		{
			// 마고자 (2005-04-14 오후 7:49:39) : 
			// Flag 정보 읽음
			fread( ( void * ) & m_uFlag		, sizeof m_uFlag		, 1 , pFile );
		}
		break;
	default:
		fclose(pFile);
		return FALSE;
	}

	int							j						;
	int							subdivisioncount		;
	int							nDepth					;
	ApDetailSegment *			pSegment				= NULL;

	UINT32						pIndex[ TD_DEPTH ]		;
	ApRGBA						sRGB					;

	ApTileInfo					nTileInfo				;
	UINT16						nPadding				;


	for( int i = 0 ; i < SECTOR_DETAILDEPTH ; ++i )
	{
		fread( ( void * ) & nDepth	, sizeof ( int ) , 1	, pFile );

		// 디테일이 같아야한다..
		//ASSERTONCE( m_pDetailSectorInfo->m_nDepth[ i ] == nDepth );

		// 안같으면 뭐.. 어쩔수 없이 -_- 

		if( nDepth > 0 )
		{
			for( z = 0 ; z < nDepth ; ++z )
			{
				for( x = 0 ; x < nDepth ; ++x )
				{
					
					if( m_pDetailSectorInfo->m_nDepth[ i ] == nDepth )
						pSegment = D_GetSegment( i ,  x , z )	;
					else
						pSegment = NULL							;

					// 초기화..
					memset( ( void * ) pIndex , 0 , ( sizeof UINT32 ) * TD_DEPTH );

					fread( ( void * ) pIndex		, sizeof UINT32		, nReadTD_DEPTH	, pFile );
					fread( ( void * ) & sRGB		, sizeof ApRGBA		, 1			, pFile );
					fread( ( void * ) & nTileInfo	, sizeof ApTileInfo	, 1			, pFile );
					fread( ( void * ) & nPadding	, sizeof UINT16		, 1			, pFile );

					if( pSegment )
					{
						// 존재하며 , 나뉘어지지 않은 것이라면..
						for( j = 0 ; j < TD_DEPTH ; ++j )
							pSegment->pIndex[ j ]	= pIndex[ j ]	;
						pSegment->vertexcolor		= sRGB			;
						pSegment->stTileInfo		= nTileInfo		;
						pSegment->nPadding			= nPadding		;
					}
				}
			}

			// Blocking 요소 추가. 로딩은 같이 된다.
		}

		// Reserved Field
		fread( ( void * ) & m_pDetailSectorInfo->m_nAlignment		, sizeof ( int ) , 1	, pFile );
		fread( ( void * ) & m_pDetailSectorInfo->m_nReserved1		, sizeof ( int ) , 1	, pFile );
		fread( ( void * ) & m_pDetailSectorInfo->m_nReserved2		, sizeof ( int ) , 1	, pFile );
		fread( ( void * ) & m_pDetailSectorInfo->m_nReserved3		, sizeof ( int ) , 1	, pFile );
		fread( ( void * ) & m_pDetailSectorInfo->m_nReserved4		, sizeof ( int ) , 1	, pFile );

		// Read Sub Division...
		fread( ( void * ) &subdivisioncount	, sizeof ( int )		, 1 , pFile );
	}

	// 콜벡 호출..
	m_pModuleMap->EnumCallback( APMMAP_CB_ID_LOADMOONEE , ( void * ) pFile , ( void * ) this );

	fclose( pFile );

	return TRUE;
}

BOOL	ApWorldSector::SaveDetailDataGeometry	()
{
	// 파일이름 기준으로하여 출력함.
	if( !IsLoadedDetailData() )
	{
		TRACE( "(%d,%d) 로드돼지 않았음!\n" , m_nIndexX , m_nIndexZ );
		return FALSE;
	}

	char	filename[ 256 ];
	sprintf( filename , "%s\\g%d,%d.amf" ,
		ApWorldSector::m_pModuleMap->GetMapGeometryDirectory(),
		GetArrayIndexX()										,
		GetArrayIndexZ()										);

	FILE	*pFile = fopen( filename , "wb" );
	if( !pFile )
	{
		TRACE( "SaveDetailDataGeometry (%d,%d) 지역 데이타가 없습니다.!\n" , m_nIndexX , m_nIndexZ );
		return FALSE;
	}

	// 데이타 저장.
	int	version = ALEF_GEOMETRYFILE_HEADER;

	fwrite( ( void * ) & version		, sizeof version		, 1 , pFile );

	// 마고자 (2005-03-10 오전 10:22:32) : 쓰지 않으므로 삭제..
	// 이전 데이타와 호완.
	char	strComment[ 256 ] = "Sector Data";
	fwrite( ( void * ) strComment		, sizeof ( char	 )		,256, pFile );

	int										j						;
	int										dummy					= 0;

	// 마고자 (2005-04-21 오전 10:58:44) : 
	// 디테일만 저장하게 변경함..

	// SECTOR_EMPTY
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	// SECTOR_LOWDETAIL
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );

	//for( int i = 0 ; i < SECTOR_DETAILDEPTH ; ++i )
	int i = SECTOR_HIGHDETAIL;
	{
		fwrite( ( void * ) & m_pDetailSectorInfo->m_nDepth[ i ]		, sizeof ( int ) , 1	, pFile );

		// 높이정보 저장..
		int		limit1 = m_pDetailSectorInfo->m_nDepth[ i ] * m_pDetailSectorInfo->m_nDepth[ i ];
		for( j = 0 ; j < limit1 ; ++j )
		{
			fwrite( ( void * ) & m_pDetailSectorInfo->m_pSegment[ i ][ j ].height	, sizeof FLOAT	, 1 , pFile );
						// 플래그 저장..
			fwrite( ( void * ) &m_pDetailSectorInfo->m_pSegment[ i ][ j ].nFlags	, sizeof INT32	, 1 , pFile	);
		}

		// 이전버젼 호완용.
		fwrite( ( void * ) &dummy		, sizeof ( int )	, 1 , pFile );
	}

	// 콜벡 호출..
	m_pModuleMap->EnumCallback( APMMAP_CB_ID_SAVEGEOMETRY , ( void * ) pFile , ( void * ) this );

	fclose( pFile );

	return TRUE;
}

BOOL	ApWorldSector::SaveDetailDataTile		()
{
	// 파일이름 기준으로하여 출력함.
	if( !IsLoadedDetailData() )
	{
		TRACE( "(%d,%d) 로드돼지 않았음!\n" , m_nIndexX , m_nIndexZ );
		return FALSE;
	}

	char	filename[ 256 ];
	sprintf( filename , "%s\\t%d,%d.amf" ,
		ApWorldSector::m_pModuleMap->GetMapMooneeDirectory(),
		GetArrayIndexX()										,
		GetArrayIndexZ()										);

	FILE	*pFile = fopen( filename , "wb" );
	if( !pFile )
	{
		TRACE( "SaveDetailDataTile (%d,%d) 지역 데이타가 없습니다.!\n" , m_nIndexX , m_nIndexZ );
		return FALSE;
	}

	// 데이타 저장.

	// 마고자 (2005-03-15 오후 3:32:17) : 
	// 지오메트리 이펙트 정보 저장..
	if( this->m_uFlag ) // 무언가 옵션이 있으면..
	{
		// 새버젼으로 기록..
		int	version = ALEF_MOONIEFILE_HEADER;
		fwrite( ( void * ) & version		, sizeof version		, 1 , pFile );

		// 마고자 (2005-04-14 오후 7:48:49) : 
		// Flag 정보 저장..
		fwrite( ( void * ) & m_uFlag		, sizeof m_uFlag		, 1 , pFile );
	}
	else
	{
		int	version = ALEF_MOONIEFILE_HEADER_COMPATIBLE;
		// 구버젼으로 기록..
		fwrite( ( void * ) & version		, sizeof version		, 1 , pFile );

		// 마고자 (2005-03-10 오전 10:22:32) : 쓰지 않으므로 삭제..
		// 이전 데이타와 호완.
		char	strComment[ 256 ] = "Sector Data";
		fwrite( ( void * ) strComment		, sizeof ( char	 )		,256, pFile );
	}

	int										dummy					= 0;
	int										j						;

	// 마고자 (2005-04-21 오전 11:00:37) : 디테일 데이타만 저장함..
	// SECTOR_EMPTY
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );

	// SECTOR_LOWDETAIL
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );
	fwrite( ( void * ) & dummy		, sizeof ( int ) , 1	, pFile );

	// for( int i = 0 ; i < SECTOR_DETAILDEPTH ; ++i )
	int i = SECTOR_HIGHDETAIL;
	{
		fwrite( ( void * ) & m_pDetailSectorInfo->m_nDepth[ i ]		, sizeof ( int ) , 1	, pFile );

		int		limit1 = m_pDetailSectorInfo->m_nDepth[ i ] * m_pDetailSectorInfo->m_nDepth[ i ];

		for( j = 0 ; j < limit1 ; ++j )
		{
			// 타일정보..
			fwrite( ( void * ) m_pDetailSectorInfo->m_pSegment[ i ][ j ].pIndex			, sizeof UINT32	, TD_DEPTH , pFile	);

			// 버텍스칼라..
			fwrite( ( void * ) &m_pDetailSectorInfo->m_pSegment[ i ][ j ].vertexcolor	, sizeof ApRGBA, 1 , pFile	);

			// Reserved
			fwrite( ( void * ) &m_pDetailSectorInfo->m_pSegment[ i ][ j ].stTileInfo		, sizeof ApTileInfo	, 1 , pFile	);
			// Reserved
			fwrite( ( void * ) &m_pDetailSectorInfo->m_pSegment[ i ][ j ].nPadding		, sizeof UINT16	, 1 , pFile	);
		}
	
		// Reserved Field
		fwrite( ( void * ) & m_pDetailSectorInfo->m_nAlignment		, sizeof ( int ) , 1	, pFile );
		fwrite( ( void * ) & m_pDetailSectorInfo->m_nReserved1		, sizeof ( int ) , 1	, pFile );
		fwrite( ( void * ) & m_pDetailSectorInfo->m_nReserved2		, sizeof ( int ) , 1	, pFile );
		fwrite( ( void * ) & m_pDetailSectorInfo->m_nReserved3		, sizeof ( int ) , 1	, pFile );
		fwrite( ( void * ) & m_pDetailSectorInfo->m_nReserved4		, sizeof ( int ) , 1	, pFile );

		fwrite( ( void * ) &dummy		, sizeof ( int )	, 1 , pFile );
	}

	// 콜벡 호출..
	m_pModuleMap->EnumCallback( APMMAP_CB_ID_SAVEMOONEE , ( void * ) pFile , ( void * ) this );

	fclose( pFile );
	return FALSE;
}

UINT8	ApWorldSector::GetTileCompactType				( INT32 tileindex	)
{
	// 타일에 타입을 알아낸다..

	// 지금은 카테고리를 리턴함. 
	return ( UINT8 )GET_TEXTURE_TYPE( tileindex ) ;
}

/*
// 이건 50x50 좌표..
UINT8	ApWorldSector::SetBlocking		( INT32 x , INT32 z , INT32 nBlocking	)
{
	ASSERT( x >= 0 && x < 32				);
	ASSERT( z >= 0 && z < 32				);
	ASSERT( NULL != GetDetailSectorInfo()	);// 디테일 데이타가 존재해야한다.
	ASSERT( IsLoadedDetailData( SECTOR_HIGHDETAIL ) );

	int sector_index_x = x / 2 ;
	int sector_index_z = z / 2 ;

	ApDetailSegment * pSegment;
	pSegment = D_GetSegment( SECTOR_HIGHDETAIL , sector_index_x , sector_index_z );

	if( pSegment )
	{
		// 찾았을때만..
		pSegment->stTileInfo.SetObjectBlocking( 
			( x - ( sector_index_x << 1 ) ) +
			( z - ( sector_index_z << 1 ) ) * 2	,
			nBlocking
		);

		return 1;
	}
	else
		return 0;
}
*/

UINT8	ApWorldSector::GetBlocking		( INT32 x , INT32 z	, INT32 eType			)
{
	ASSERT( x >= 0 && x < 32				);
	ASSERT( z >= 0 && z < 32				);
	ASSERT( NULL != GetDetailSectorInfo()	);// 디테일 데이타가 존재해야한다.
	ASSERT( IsLoadedDetailData( SECTOR_HIGHDETAIL ) );

	// 컴펙트 가 있으면 컴팩트의 데이타를 리턴해야한다.

	int sector_index_x = x / 2 ;
	int sector_index_z = z / 2 ;

	ApDetailSegment * pSegment;
	pSegment = D_GetSegment( SECTOR_HIGHDETAIL , sector_index_x , sector_index_z );

	if( pSegment )
	{
		// 찾았을때만.
		switch( eType )
		{
		default:
		case ApmMap::GROUND		:
			// 정상처리
			return pSegment->stTileInfo.GetBlocking( 
				( x - ( sector_index_x << 1 ) ) +
				( z - ( sector_index_z << 1 ) ) * 2
			);
		case ApmMap::SKY		:
			return pSegment->stTileInfo.GetSkyBlocking();
		case ApmMap::UNDERGROUND:
			return 1;
		case ApmMap::GHOST		:
			return 0;
		}
	}
	else
		return 0;
}


/*
UINT8	ApWorldSector::AddBlocking		( AuBLOCKING * pBlocking  , INT32 count	, INT32 nBlockType)
{
//	ASSERT( NULL != pBlocking );
//	ASSERT( NULL != GetDetailSectorInfo()	);// 디테일 데이타가 존재해야한다.
//	ASSERT( IsLoadedDetailData( SECTOR_HIGHDETAIL ) );

	if(	NULL == pBlocking				||
		NULL == GetDetailSectorInfo()	||
		!IsLoadedDetailData( SECTOR_HIGHDETAIL ) ) return 0;

	int nStartX , nStartZ	;
	int nEndX , nEndZ		;

	FLOAT	fStepSize = ( GetXEnd() - GetXStart() ) / ( MAP_STEPSIZE / 2.0f ) ;
	// 블러킹 요소 별로.. 체크해야함.
	for( int i = 0 ; i < count ; ++i )
	{
		switch( pBlocking[ i ].type )
		{
		case AUBLOCKING_TYPE_BOX		:
			{
				// 박스형..
				// pBlocking[ i ].data.box

				// 인덱스 구하고..
				nStartX	= ( int ) ( ( pBlocking[ i ].data.box.inf.x - GetXStart() ) / fStepSize );
				nStartZ	= ( int ) ( ( pBlocking[ i ].data.box.inf.z - GetZStart() ) / fStepSize );
				
				nEndX	= ( int ) ( ( pBlocking[ i ].data.box.sup.x - GetXStart() ) / fStepSize );
				nEndZ	= ( int ) ( ( pBlocking[ i ].data.box.sup.z - GetZStart() ) / fStepSize );

				// 프로텍션 걸고..
				if( nStartX < 0		)	nStartX	= 0	;
				if( nStartZ < 0		)	nStartZ	= 0	;
				if( nEndX >= 50		)	nEndX	= 49;
				if( nEndZ >= 50		)	nEndZ	= 49;

				if( nStartX >= 50 || nStartZ >= 50 || nEndX < 0 || nEndZ < 0 )
				{
					// 범위 초과..
					break;
				}

				for( int x = nStartX ; x <= nEndX ; ++x )
				{
					for( int z = nStartZ ; z <= nEndZ ; ++z )
					{
						if( 
							// 높이검사..
							// 좌표검사..
							1
							)
						{
							AddBlocking( x , z , nBlockType );
						}
					}
				}
			}
			break;
		case AUBLOCKING_TYPE_SPHERE		:
			{
				// 구형..
				// pBlocking[ i ].data.sphere

				// 인덱스 구하고..
				nStartX	= ( int ) ( (	pBlocking[ i ].data.sphere.center.x	-
										pBlocking[ i ].data.sphere.radius	-
										GetXStart()							) / fStepSize );
				nStartZ	= ( int ) ( (	pBlocking[ i ].data.sphere.center.z	-
										pBlocking[ i ].data.sphere.radius	-
										GetZStart()							) / fStepSize );
				
				nEndX	= ( int ) ( (	pBlocking[ i ].data.sphere.center.x	+
										pBlocking[ i ].data.sphere.radius	-
										GetXStart()							) / fStepSize );
				nEndZ	= ( int ) ( (	pBlocking[ i ].data.sphere.center.z	-
										pBlocking[ i ].data.sphere.radius	+
										GetZStart()							) / fStepSize );

				// 프로텍션 걸고..
				if( nStartX < 0		)	nStartX	= 0	;
				if( nStartZ < 0		)	nStartZ	= 0	;
				if( nEndX >= 50		)	nEndX	= 49;
				if( nEndZ >= 50		)	nEndZ	= 49;

				if( nStartX >= 50 || nStartZ >= 50 || nEndX < 0 || nEndZ < 0 )
				{
					// 범위 초과..
					break;
				}

				for( int x = nStartX ; x <= nEndX ; ++x )
				{
					for( int z = nStartZ ; z <= nEndZ ; ++z )
					{
						// 범위체크
						if( 
							sqrt(
								( x * fStepSize + GetXStart() - pBlocking[ i ].data.sphere.center.x )	*
								( x * fStepSize + GetXStart() - pBlocking[ i ].data.sphere.center.x )	+
								( z * fStepSize + GetZStart() - pBlocking[ i ].data.sphere.center.z )	*
								( z * fStepSize + GetZStart() - pBlocking[ i ].data.sphere.center.z )	)
							> pBlocking[ i ].data.sphere.radius ) continue;
						AddBlocking( x , z , nBlockType );
					}
				}

			}
			break;
		case AUBLOCKING_TYPE_CYLINDER	:
			{
				// 원주형..
				// pBlocking[ i ].data.cylinder
				nStartX	= ( int ) ( (	pBlocking[ i ].data.cylinder.center.x	-
										pBlocking[ i ].data.cylinder.radius	-
										GetXStart()							) / fStepSize );
				nStartZ	= ( int ) ( (	pBlocking[ i ].data.cylinder.center.z	-
										pBlocking[ i ].data.cylinder.radius	-
										GetZStart()							) / fStepSize );
				
				nEndX	= ( int ) ( (	pBlocking[ i ].data.cylinder.center.x	+
										pBlocking[ i ].data.cylinder.radius	-
										GetXStart()							) / fStepSize );
				nEndZ	= ( int ) ( (	pBlocking[ i ].data.cylinder.center.z	-
										pBlocking[ i ].data.cylinder.radius	+
										GetZStart()							) / fStepSize );

				// 프로텍션 걸고..
				if( nStartX < 0		)	nStartX	= 0	;
				if( nStartZ < 0		)	nStartZ	= 0	;
				if( nEndX >= 50		)	nEndX	= 49;
				if( nEndZ >= 50		)	nEndZ	= 49;

				if( nStartX >= 50 || nStartZ >= 50 || nEndX < 0 || nEndZ < 0 )
				{
					// 범위 초과..
					break;
				}

				for( int x = nStartX ; x <= nEndX ; ++x )
				{
					for( int z = nStartZ ; z <= nEndZ ; ++z )
					{
						// 범위체크
						if( 
							sqrt(
								( x * fStepSize + GetXStart() - pBlocking[ i ].data.cylinder.center.x )	*
								( x * fStepSize + GetXStart() - pBlocking[ i ].data.cylinder.center.x )	+
								( z * fStepSize + GetZStart() - pBlocking[ i ].data.cylinder.center.z )	*
								( z * fStepSize + GetZStart() - pBlocking[ i ].data.cylinder.center.z )	)
							> pBlocking[ i ].data.cylinder.radius ) continue;
						AddBlocking( x , z , nBlockType );
					}
				}
			}
			break;
		case AUBLOCKING_TYPE_NONE		:
		default							:
			// do no op..
			break;
		}
	}

	return 1;
}
*/

/*
void	ApWorldSector::ClearObjectBlocking()
{
	if( NULL == GetDetailSectorInfo() || !IsLoadedDetailData( SECTOR_HIGHDETAIL ) )
		return;

	INT32				uBlocking	= FALSE;

	ApDetailSegment	*	pSegment	;
	int					nDepth		= D_GetDepth( SECTOR_HIGHDETAIL );
	int					j , i;

	for( j = 0 ; j < nDepth ; ++j )
	{
		for( i = 0 ; i < nDepth ; ++i )
		{
			pSegment = D_GetSegment( SECTOR_HIGHDETAIL , i , j );

			//pSegment->stTileInfo.geometryblock	= 0;
			pSegment->stTileInfo.objectblock		= 0;
		}
	}
}
*/

BOOL	ApWorldSector::AllocHeightPool			()
{
	if( m_pHeightPool )
	{
		// do nothing...
	}
	else
	{
		// SetHeight하는 부분에서 x,z가 0~16이 들어온다. 0~15가 들어와야 정상인데, 더 큰 값이 들어와서 메모리 침범하면서 죽고 있었다.
		// 일단 안죽게 하기 위해서 nDepth를 하나씩 키운다.
		int	nDepth = MAP_DEFAULT_DEPTH + 1;
		// ASSERT( nDepth > 0 );
		if( nDepth <= 0 )
		{
			return FALSE;
		}

		m_pHeightPool	= new	HeightPool[ nDepth * nDepth ];
		ASSERT( NULL != m_pHeightPool );
	}

	return TRUE;
}

BOOL	ApWorldSector::FreeHeightPool			()
{
	if( m_pHeightPool )
	{
		//@{ 2006/11/16 burumal
		// 임시방편입니다
		if ( IsBadReadPtr(m_pHeightPool, sizeof(HeightPool*)) != TRUE )
		//@}
			delete [] m_pHeightPool;

		m_pHeightPool	= NULL;
	}
	else
	{
		// do nothing
	}

	return TRUE;
}

BOOL	ApWorldSector::HP_SetHeight			( int x , int z , FLOAT fHeight )
{
	if( NULL == m_pHeightPool )
	{
		if( AllocHeightPool() )
		{
			// do nothing..
		}
		else
		{
			// -_-;;
			return FALSE;
		}
	}

	// 마고자 (2004-08-17 오후 7:11:27) : 
	// 이 어설트 뜨면 저 호출 부탁.

	ASSERT( x >= 0 && x < MAP_DEFAULT_DEPTH );
	ASSERT( z >= 0 && z < MAP_DEFAULT_DEPTH );

	if( x < 0 || x >= MAP_DEFAULT_DEPTH )
	{
		// 이런경우가 발생하면 안된다..
		return FALSE;
	}
	if( z < 0 || z >= MAP_DEFAULT_DEPTH )
	{
		// 이런경우가 발생하면 안된다..
		return FALSE;
	}

	// 메모리는 16*16만 할당해놓고, 더 큰 값으로 Access하면 어짜자는 것인가... -_-;
	// 그동안 안죽은게 신기할 따름.

	HeightPool	* pHeightPool	=  &m_pHeightPool[ ( x ) + MAP_DEFAULT_DEPTH * ( z ) ];

	pHeightPool->uDataAvailable	|= HEIGHTWITHRIDABLEOBJECT;
	pHeightPool->fHeight		= fHeight	;

	return TRUE;
}

FLOAT *	ApWorldSector::HP_GetHeight			( int x , int z )
{
	if( NULL == m_pHeightPool ) return NULL;

	// 마고자 (2004-08-17 오후 7:11:27) : 
	// 이 어설트 뜨면 저 호출 부탁.
	ASSERT( x >= 0 && x < MAP_DEFAULT_DEPTH );
	ASSERT( z >= 0 && z < MAP_DEFAULT_DEPTH );

	if( x < 0 || x >= MAP_DEFAULT_DEPTH )
	{
		// 이런경우가 발생하면 안된다..
		return NULL;
	}
	if( z < 0 || z >= MAP_DEFAULT_DEPTH )
	{
		// 이런경우가 발생하면 안된다..
		return NULL;
	}
	
	HeightPool	* pHeightPool	=  &m_pHeightPool[ ( x ) + MAP_DEFAULT_DEPTH * ( z ) ];

	if( pHeightPool->uDataAvailable & HEIGHTWITHRIDABLEOBJECT )
		return &pHeightPool->fHeight;
	else
		return NULL;
}

BOOL	ApWorldSector::HP_SetHeightGeometryOnly( int x , int z , FLOAT fHeight )
{
	if( NULL == m_pHeightPool )
	{
		if( AllocHeightPool() )
		{
			// do nothing..
		}
		else
		{
			// -_-;;
			return FALSE;
		}
	}

	// 마고자 (2004-08-17 오후 7:11:27) : 
	// 이 어설트 뜨면 저 호출 부탁.

	ASSERT( x >= 0 && x < MAP_DEFAULT_DEPTH );
	ASSERT( z >= 0 && z < MAP_DEFAULT_DEPTH );

	if( x < 0 || x >= MAP_DEFAULT_DEPTH )
	{
		// 이런경우가 발생하면 안된다..
		return FALSE;
	}
	if( z < 0 || z >= MAP_DEFAULT_DEPTH )
	{
		// 이런경우가 발생하면 안된다..
		return FALSE;
	}

	// 메모리는 16*16만 할당해놓고, 더 큰 값으로 Access하면 어짜자는 것인가... -_-;
	// 그동안 안죽은게 신기할 따름.

	HeightPool	* pHeightPool	=  &m_pHeightPool[ ( x ) + MAP_DEFAULT_DEPTH * ( z ) ];

	pHeightPool->uDataAvailable		|= HEIGHTWITHGEOMETRYONLY	;
	pHeightPool->fGeometryHeight	= fHeight					;

	return TRUE;
}

FLOAT *	ApWorldSector::HP_GetHeightGeometryOnly( int x , int z )
{
	if( NULL == m_pHeightPool ) return NULL;

	// 마고자 (2004-08-17 오후 7:11:27) : 
	// 이 어설트 뜨면 저 호출 부탁.
	ASSERT( x >= 0 && x < MAP_DEFAULT_DEPTH );
	ASSERT( z >= 0 && z < MAP_DEFAULT_DEPTH );

	if( x < 0 || x >= MAP_DEFAULT_DEPTH )
	{
		// 이런경우가 발생하면 안된다..
		return NULL;
	}
	if( z < 0 || z >= MAP_DEFAULT_DEPTH )
	{
		// 이런경우가 발생하면 안된다..
		return NULL;
	}
	
	HeightPool	* pHeightPool	=  &m_pHeightPool[ ( x ) + MAP_DEFAULT_DEPTH * ( z ) ];

	if( pHeightPool->uDataAvailable & HEIGHTWITHGEOMETRYONLY )
		return &pHeightPool->fGeometryHeight;
	else
		return NULL;
}


void	ApWorldSector::CreateIndexArray( INT32 nIndexType )
{
	ASSERT( 0 <= nIndexType && nIndexType < AWS_COUNT );
	ASSERT( NULL == m_aIndexArray[ nIndexType ].pIndexStruct );

	INT32	nDepth = MAP_DEFAULT_DEPTH; // D_GetDepth( SECTOR_HIGHDETAIL );
	ASSERT( nDepth > 0 );
	if( nDepth <= 0 )
	{
		// 에에잉??..;

		return;
	}
	else
	{
		m_aIndexArray[ nIndexType ].nDepth	= nDepth;
		VERIFY( m_aIndexArray[ nIndexType ].pIndexStruct = new IndexStruct[ nDepth * nDepth ] );
	}
}

void	ApWorldSector::DeleteIndexArray( INT32 nIndexType )
{
	ASSERT( 0 <= nIndexType && nIndexType < AWS_COUNT );
	if( m_aIndexArray[ nIndexType ].pIndexStruct )
	{
		delete [] m_aIndexArray[ nIndexType ].pIndexStruct	;
		m_aIndexArray[ nIndexType ].pIndexStruct	= NULL	;
		m_aIndexArray[ nIndexType ].nDepth			= 0		;
	}
}

BOOL	ApWorldSector::AddIndex( INT32	nIndexType , int x , int z , INT32 oid , BOOL bOverlappingTest )
{
	ASSERT( 0 <= nIndexType && nIndexType < AWS_COUNT );
	if( NULL == m_aIndexArray[ nIndexType ].pIndexStruct )
	{
		CreateIndexArray( nIndexType );
	}

	ASSERT( 0 <= x && x < m_aIndexArray[ nIndexType ].nDepth );
	ASSERT( 0 <= z && z < m_aIndexArray[ nIndexType ].nDepth );

	IndexStruct	*	pCollisionSegment;
	pCollisionSegment	=	&m_aIndexArray[ nIndexType ].pIndexStruct[ x + z * m_aIndexArray[ nIndexType ].nDepth ];

	if( bOverlappingTest )
	{
		for( int i = 0 ; i < pCollisionSegment->nCount ; ++ i )
		{
			if( oid == pCollisionSegment->pArray[ i ] )
			{
				// 중복~!
				return TRUE;// 따로 에러 리턴 안해요~..
			}
		}
	}

	if( pCollisionSegment->nCount >= SECTOR_MAX_COLLISION_OBJECT_COUNT )
		return FALSE;

	pCollisionSegment->pArray[ pCollisionSegment->nCount++ ] = oid;
	
	switch( nIndexType )
	{
	case AWS_RIDABLEOBJECT:
		// 마고자 (2004-04-29 오후 4:57:36) : 
		// 라이더블 오브젝트가 추가되면 캐시정보를 없에버린다.
		FreeHeightPool();
		break;
	default:
		break;
	}
	return TRUE;
}

INT32	ApWorldSector::GetIndex( INT32	nIndexType , int x , int z , INT32 * pArrayID , INT32 nMax )	// Return is its count.
{
	ASSERT( 0 <= nIndexType && nIndexType < AWS_COUNT );
	if( NULL == m_aIndexArray[ nIndexType ].pIndexStruct ) return 0;

	ASSERT( 0 <= x && x < m_aIndexArray[ nIndexType ].nDepth );
	ASSERT( 0 <= z && z < m_aIndexArray[ nIndexType ].nDepth );
	ASSERT( NULL != pArrayID						);

	if( 0 > x || x >= m_aIndexArray[ nIndexType ].nDepth )
		return 0;

	if( 0 > z || z >= m_aIndexArray[ nIndexType ].nDepth )
		return 0;

	if( !pArrayID )
		return 0;

	IndexStruct	*	pCollisionSegment;
	pCollisionSegment	=	&m_aIndexArray[ nIndexType ].pIndexStruct[ x + z * m_aIndexArray[ nIndexType ].nDepth ];

	for( int i = 0 ; i < pCollisionSegment->nCount && i < nMax ; ++ i )
	{
		pArrayID[ i ]	= pCollisionSegment->pArray[ i ];
	}

	return pCollisionSegment->nCount;
}

BOOL	ApWorldSector::DeleteIndex		( INT32	nIndexType , int x , int z )
{
	ASSERT( 0 <= nIndexType && nIndexType < AWS_COUNT );
	if( NULL == m_aIndexArray[ nIndexType ].pIndexStruct ) return FALSE;

	ASSERT( 0 <= x && x < m_aIndexArray[ nIndexType ].nDepth );
	ASSERT( 0 <= z && z < m_aIndexArray[ nIndexType ].nDepth );

	IndexStruct	*	pCollisionSegment;
	pCollisionSegment	=	&m_aIndexArray[ nIndexType ].pIndexStruct[ x + z * m_aIndexArray[ nIndexType ].nDepth ];

	// 해당 들어있는 익덱스의 카운트만 0으로 밖꾸어 놓는다..
	pCollisionSegment->nCount = 0;

	return TRUE;
}

BOOL	ApWorldSector::DeleteIndex		( INT32	nIndexType , int x , int z , INT32 nIndex	)
{
	ASSERT( 0 <= nIndexType && nIndexType < AWS_COUNT );
	if( NULL == m_aIndexArray[ nIndexType ].pIndexStruct ) return FALSE;

	ASSERT( 0 <= x && x < m_aIndexArray[ nIndexType ].nDepth );
	ASSERT( 0 <= z && z < m_aIndexArray[ nIndexType ].nDepth );

	IndexStruct	*	pCollisionSegment;
	pCollisionSegment	=	&m_aIndexArray[ nIndexType ].pIndexStruct[ x + z * m_aIndexArray[ nIndexType ].nDepth ];

	for( int i = 0 ; i < pCollisionSegment->nCount ; ++ i )
	{
		if( pCollisionSegment->pArray[ i ] == nIndex )
		{
			// 인덱스 삭제과정..
			for( int j = i ; j < pCollisionSegment->nCount - 1 ; ++j )
			{
				pCollisionSegment->pArray[ j ] = pCollisionSegment->pArray[ j + 1 ];
			}

			// 갯수감소..
			--pCollisionSegment->nCount;
			--i;
		}
	}

	return TRUE;
}

BOOL	ApWorldSector::AddSkyObjectTemplateID	( int x , int z , INT32 nTemplate , FLOAT fDistance )
{
	ASSERT( !"이것은 사용하지 않는다" );
	
//	INT32	nDistance = ( INT32 ) fDistance;
//	INT32	nCount , nArray[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
//
//	nCount = GetIndex( AWS_SKYOBJECT , x , z , nArray );
//	if( nCount )
//	{
//		// 0 번째는 스카이 테플릿 번호 들어감..
//		// 1 번째는 거리가 INT로 들어감..
//
//		if( nArray[ 0 ] == nTemplate )
//		{
//			// 이미 들어있을때 처리
//			// 같은거면 거리만 검사해서 , 가까운걸로 세팅해둠..
//			DeleteIndex( AWS_SKYOBJECT , x , z );
//			AddSkyObjectTemplateID( x , z , nTemplate , fDistance );
//		}
//		else
//		if( nArray[ 1 ] > nDistance )
//		{
//			// 새 녀석으로 교체..
//			DeleteIndex( AWS_SKYOBJECT , x , z );
//			AddSkyObjectTemplateID( x , z , nTemplate , fDistance );
//		}
//		else
//		{
//			// 무시..
//			// 거리가 더 멀다.
//			return TRUE;
//		}
//	}
//	else
//	{
//		// 그냥 삽입..
//		AddIndex( AWS_SKYOBJECT , x , z , nTemplate );
//		AddIndex( AWS_SKYOBJECT , x , z , nDistance );
//	}

	return TRUE;
}

INT32	ApWorldSector::GetSkyObjectTemplateID	( int x , int z , INT32 * pArrayID , INT32 nMax )	// Return is its count.
{
	return GetIndex( AWS_SKYOBJECT , x , z , pArrayID , nMax );
}

FLOAT	ApWorldSector::D_GetHeight2	( INT32 x , INT32 z					)
{
	ApDetailSegment * pSegment;

	if( x >= MAP_DEFAULT_DEPTH && z >= MAP_DEFAULT_DEPTH )
	{
		ApWorldSector * pNearSector = NULL ;
		if( GetNearSector( TD_SOUTH ) && GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST ) )
			pNearSector = GetNearSector( TD_SOUTH )->GetNearSector( TD_EAST );
		else if ( GetNearSector( TD_EAST ) && GetNearSector( TD_EAST )->GetNearSector( TD_SOUTH ) )
			pNearSector = GetNearSector( TD_EAST )->GetNearSector( TD_SOUTH );

		return pNearSector ? pNearSector->D_GetHeight2( 0 , 0 ) : 1.0f;
	}
	else if ( x >= MAP_DEFAULT_DEPTH )
	{
		return GetNearSector( TD_EAST ) ? GetNearSector( TD_EAST )->D_GetHeight2( 0 , z ) : 1.0f;
	}
	else if ( z >= MAP_DEFAULT_DEPTH )
	{
		return GetNearSector( TD_SOUTH ) ? GetNearSector( TD_SOUTH )->D_GetHeight2( x , 0 ) : 1.0f;
	}	

	pSegment = D_GetSegment( x , z );
	return pSegment ? pSegment->height : 1.0f;	
}

ApWorldSector::Dimension *	ApWorldSector::GetDimension( INT32 nCreatedIndex )
{
	Dimension * pFound;
	INT32		nCount = GetDimensionCount(); 

	if( nCount != 0 )
	{
		Dimension * pArray = GetDimensionArray();

		pFound = find( pArray , pArray + nCount , nCreatedIndex );

		if( pArray + nCount == pFound )
			return NULL		;
		else
			return pFound	;
	}
	else
	{
		return NULL;
	}
}

BOOL	ApWorldSector::CreateDimension( INT32 nCreatedIndex )
{
	// 일단 중복 검사..
	Dimension * pFound = GetDimension( nCreatedIndex );
	if( NULL == pFound )
	{
		// 음따... 추가함..
		Dimension stNewDivision;
		stNewDivision.nIndex	= nCreatedIndex;

		if( 0 != nCreatedIndex )
		{
			// 0 이 아닌것은 복사된
			// 차원이므로.. 여기서 0 차원을 복사하는 과정이 필요하다..

		}

		m_arrayDimension.push_back( stNewDivision );
		return TRUE;
	}
	else
	{
		// 겹친다..
		TRACE( "디멘젼 인덱스 중복!\n" );
		return FALSE;
	}
}

BOOL	ApWorldSector::DeleteDimension( INT32 nCreatedIndex )
{
	std::vector<ApWorldSector::Dimension>::iterator itr;
	for(itr=m_arrayDimension.begin(); itr!=m_arrayDimension.end(); ++itr)
	{
		if((*itr).nIndex == nCreatedIndex)
		{
			m_arrayDimension.erase(itr);
			return TRUE;
		}
	}

	// 그런거 없는데?..
	TRACE( "디멘젼 인덱스 중복!\n" );
	return FALSE;
}

FLOAT	ApWorldSector::D_GetMinHeight			()
{
	if( !IsLoadedDetailData() ) return SECTOR_MIN_HEIGHT;

	int	nDepth = D_GetDepth( SECTOR_HIGHDETAIL );

	// 데이타 모으기
	int		x , z	;

	ApDetailSegment	*	pSegment		;
//	ApCompactSegment *	pCompactSegment	;

	FLOAT	fMinHeight = SECTOR_MAX_HEIGHT;

	for( z = 0 ; z < nDepth ; ++z )
	{
		for( x = 0 ; x < nDepth ; ++x )
		{
			// 세그먼트 정보 얻음..
			pSegment		= D_GetSegment( SECTOR_HIGHDETAIL , x , z );
			if( pSegment )
			{
				if( pSegment->height < fMinHeight )
				{
					fMinHeight = pSegment->height;
				}
			}
		}
	}

	return fMinHeight;
}

BOOL	ApWorldSector::AddLineBlock( AuLineBlock & stParam )
{
	m_vecBlockLine.push_back( stParam );
	return TRUE;
}

BOOL	ApWorldSector::ClearLineBlock()
{
	m_vecBlockLine.clear();
	return TRUE;
}

INT32	ApWorldSector::GetLineBlockCount()
{
	return ( INT32 ) m_vecBlockLine.size();
}

bool	ApWorldSector::LockSector		()
{
	for( INT32 i = 0 ; i < GetDimensionCount() ; i ++ )
	{
		Dimension	* pDimension = GetDimension( i );
		if( pDimension )
		{
			// 전부다 락함.
			pDimension->lockUsers	.LockWriter();
			pDimension->lockNPCs	.LockWriter();
			pDimension->lockMonsters.LockWriter();
			pDimension->lockItems	.LockWriter();
			pDimension->lockObjects	.LockWriter();
		}
	}

	return true;
}
void	ApWorldSector::ReleaseSector	()
{
	for( INT32 i = 0 ; i < GetDimensionCount() ; i ++ )
	{
		Dimension	* pDimension = GetDimension( i );
		if( pDimension )
		{
			// 전부다 락함.
			pDimension->lockUsers	.UnlockWriter();
			pDimension->lockNPCs	.UnlockWriter();
			pDimension->lockMonsters.UnlockWriter();
			pDimension->lockItems	.UnlockWriter();
			pDimension->lockObjects	.UnlockWriter();
		}
	}
}
